using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Viatronix.Imapi;
using Viatronix.Logging;
using Viatronix.Utilities.IO;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// A base disc transfer model.
  /// </summary>
  public class DiscTransferModel : TransferModel
  {

    #region events

    /// <summary>
    /// Notifies the media beginning to be erased.
    /// </summary>
    public event EraseEventHandler Erasing;

    /// <summary>
    /// Notifies the media having been erased.
    /// </summary>
    public event EventHandler Erased;

    /// <summary>
    /// Notifies media having been loaded into the device.
    /// </summary>
    public event DeviceEmptyEventHandler DeviceEmpty;

    /// <summary>
    /// Notifies the media cannot hold the contents of selected transfer contents.
    /// </summary>
    public event InsufficientMediaCapacityEventHandler InsufficientMediaCapacity;

    #endregion

    #region fields

    /// <summary>
    /// Was the transfer cancelled.
    /// </summary>
    private bool m_cancelled = false;

    /// <summary>
    /// The disc format data to write files to the media.
    /// </summary>
    private DiscFormatData m_discFormatData = null;

    #endregion

    #region methods

    /// <summary>
    /// Performs the transfer.
    /// </summary>
    /// <param name="studies">The list of studies to transfer.</param>
    /// <param name="args">Arguments to take into account.</param>
    /// <param name="stagingArea">The staging area directory.</param>
    public override void Transfer(StudyViewModel[] studies, object args, string stagingArea)
    {
      DeviceViewModel device = (DeviceViewModel)args;

      m_cancelled = false;

      DiscRecorder recorder = null;
      FileSystemImage.FileSystemImageResult fsImageResult = null;

      try
      {
        recorder = new DiscRecorder();
        recorder.InitializeDiscRecorder(device.DeviceId);

        do
        {
          OnProgress(new ProgressEventArgs("Checking Device Status", 0, 100));

          // Media has been inserted into the device, verify it is blank.
          if (m_cancelled)
          {
            throw new LoadAbortException("User cancelled transfer");
          }

          if (!IsDeviceReady(recorder))
          {
            OnProgress(new ProgressEventArgs("Media Not Present or Unreadable", 0, 100));

            recorder.EjectMedia();

            DeviceEmptyEventArgs deviceEmptyArgs = new DeviceEmptyEventArgs();

            OnDeviceEmpty(deviceEmptyArgs);
            if (deviceEmptyArgs.Abort)
            {
              throw new LoadAbortException("User cancelled transfer");
            }
          }
          else
          {
            OnProgress(new ProgressEventArgs("Media Successfully Loaded", 2, 100));

            if(!IsMediaCapacitySuitable(recorder, stagingArea))
            {
              recorder.EjectMedia();

              Int64 minimumCapacity = PathUtilities.GetSize(stagingArea, true);
              InsufficientMediaCapacityEventArgs insufficientCapacityArgs = new InsufficientMediaCapacityEventArgs(minimumCapacity);

              OnMediaCapacityInssuffient(insufficientCapacityArgs);
              if (insufficientCapacityArgs.Abort)
              {
                throw new LoadAbortException("User cancelled transfer");
              }
            }

            if (!IsMediaBlank(recorder))
            {
              OnProgress(new ProgressEventArgs("Media Not Blank", 3, 100));

              if (IsMediaErasable(recorder))
              {
                EraseEventArgs eraseArgs = new EraseEventArgs(false) { Abort = false };
                OnErasing(eraseArgs);

                if (!eraseArgs.Erase)
                {
                  recorder.EjectMedia();
                  continue;
                }
                else if (eraseArgs.Abort)
                {
                  throw new OperationCanceledException("User chose to abort transfer.");
                }

                OnProgress(new ProgressEventArgs("Erasing Media...", 5, 100));

                if (IsDeviceReady(recorder))
                {
                  EraseMedia(recorder);
                }
                else
                {
                  continue;
                }

                OnProgress(new ProgressEventArgs("Media Successfully Erased", 20, 100));

                OnErased(EventArgs.Empty);
              }
              else
              {
                recorder.EjectMedia();
                continue;
              }
            }
          }

          Thread.Sleep(200);
        } while (!IsDeviceReady(recorder) || !IsMediaBlank(recorder));

        // Create the file system.
        CheckForCancel();
        fsImageResult = CreateImage(recorder, stagingArea);
        
        // Burn the disc.
        CheckForCancel();
        OnProgress(new ProgressEventArgs("Burning Media...", 20, 100));
        BurnMedia(recorder, true, fsImageResult);
        
        // Notify that transfer was completed.
        CheckForCancel();
        OnProgress(new ProgressEventArgs("Successfully Transferred To Media!", 20, 100));
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Disc Transfer Failed.  [ERROR=\"{0}\"]", e.Message), "DiscTransferModel", "Transfer");
        throw;
      }
      finally
      {
        if (recorder != null)
        {
          recorder.EjectMedia();
          recorder.Dispose();
        }
      }
    } // Transfer( studies, args, stagingArea )


    /// <summary>
    /// Detects if media is in the device.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    /// <returns></returns>
    private bool IsDeviceReady(DiscRecorder recorder)
    {
      using (DiscFormatData discData = new DiscFormatData())
      {
        if (discData.IsCurrentMediaSupported(recorder))
        {
          return true;
        }

        return false;
      }
    } // IsDeviceReady( recorder )


    /// <summary>
    /// Determines if the current media in the device can hold the staging area contents.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    /// <param name="stagingArea">The directory for transfer.</param>
    /// <returns>True if media can contain staging, false otherwise.</returns>
    private bool IsMediaCapacitySuitable(DiscRecorder recorder, string stagingArea)
    {
      using(DiscFormatData discData = new DiscFormatData())
      {
        if (discData.IsCurrentMediaSupported(recorder))
        {
          discData.Recorder = recorder;

          using(FileSystemImage fsImage = new FileSystemImage())
          {
            // ------------------------------------------------------------------------------------
            // Get the image defaults for the inserted media type, it may not have been erased yet.
            // ====================================================================================

            fsImage.ChooseImageDefaultsForMediaType(discData.CurrentPhysicalMediaType);

            Int64 deviceCapacity = fsImage.FreeMediaBlocks * 2048;
            if(deviceCapacity > PathUtilities.GetSize(stagingArea, true))
            {
              return true;
            }
          }
        }
      }

      return false;
    } // IsMediaCapacitySuitable( recorder, stagingArea )


    /// <summary>
    /// Determines if the media needs to be erased or not.
    /// </summary>
    /// <param name="recorder"></param>
    /// <returns></returns>
    private bool IsMediaBlank(DiscRecorder recorder)
    {
      using (DiscFormatData formatErase = new DiscFormatData())
      {
        if (formatErase.IsCurrentMediaSupported(recorder))
        {
          formatErase.Recorder = recorder;
          return formatErase.MediaHeuristicallyBlank || (!formatErase.MediaHeuristicallyBlank && formatErase.MediaPhysicallyBlank);
        }
      }

      return false;
    } // IsMediaBlank( recorder )


    /// <summary>
    /// Detects if the media can be erased.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    private bool IsMediaErasable(DiscRecorder recorder)
    {
      using (DiscFormatErase mediaErase = new DiscFormatErase())
      {
        if (mediaErase.IsRecorderSupported(recorder) && mediaErase.IsCurrentMediaSupported(recorder))
        {
          mediaErase.Recorder = recorder;
          return mediaErase.CurrentPhysicalMediaType == MediaTypes.DVD_PLUS_RW_DL ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.CDRW ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.DVD_DASH_RW ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.DVD_PLUS_RW ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.DVD_RAM ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.HD_DVD_RAM ||
            mediaErase.CurrentPhysicalMediaType == MediaTypes.BDRE;
        }
      }

      return false;
    } // IsMediaErasable( recorder )


    /// <summary>
    /// Erases the media in the device.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    private void EraseMedia(DiscRecorder recorder)
    {
      try
      {
        recorder.AcquireExclusiveAccess(true, "DiscTransfer");
        using (DiscFormatErase eraseDisc = new DiscFormatErase())
        {
          eraseDisc.Progress += new Imapi.ProgressEventHandler(OnMediaEraseProgressHandler);
          eraseDisc.Recorder = recorder;
          eraseDisc.ClientName = "DiscTransfer";

          eraseDisc.EraseMedia();

          eraseDisc.Progress -= new Imapi.ProgressEventHandler(OnMediaEraseProgressHandler);
        }
      }
      finally
      {
        recorder.ReleaseExclusiveAccess();
      }
    } // EraseMedia( recorder )


    /// <summary>
    /// Checks for cancel.
    /// </summary>
    private void CheckForCancel()
    {
      if (m_cancelled)
      {
        throw new OperationCanceledException("User chose to cancel transfer.");
      }
    } // CheckForCancel()


    /// <summary>
    /// Creates the disc image to prepare for burning.
    /// </summary>
    /// <param name="recorder">The disc recorder.</param>
    /// <param name="directory">The directory to transfer.</param>
    /// <returns>The fs image representation of directory.</returns>
    private FileSystemImage.FileSystemImageResult CreateImage(DiscRecorder recorder, string directory)
    {
      using (FileSystemImage fileSystemImage = new FileSystemImage())
      {
        fileSystemImage.ChooseImageDefaults(recorder);
        fileSystemImage.FileSystemsToCreate = FileSystemTypes.ISO9660 | FileSystemTypes.Joliet;

        //
        // Set the disc volume name.
        //

        //fsImage.VolumeName = "aaaaaa";

        BuildFileSystem(fileSystemImage, fileSystemImage.Root, directory);

        return fileSystemImage.CreateResultImage();
      }
    } // CreateImage( recorder, directory )


    /// <summary>
    /// Builds the file system recursively.
    /// </summary>
    /// <param name="fsImage"></param>
    /// <param name="directory"></param>
    private void BuildFileSystem(FileSystemImage fsImage, FileSystemImage.DirectoryItem directoryItem, string directory)
    {
      foreach (string file in Directory.GetFiles(directory))
      {
        directoryItem.AddFile(Path.GetFileName(file), file);
      }

      foreach (string dir in Directory.GetDirectories(directory))
      {
        FileSystemImage.DirectoryItem subDirectoryItem = fsImage.CreateDirectory(Path.GetFileName(dir));
        directoryItem.AddDirectory(subDirectoryItem);

        BuildFileSystem(fsImage, subDirectoryItem, dir);
      }
    } // BuildFileSystem( fsImage, directory )


    /// Burns the image to the device.
    /// </summary>
    /// <param name="recorder">The recorder to perform the write.</param>
    /// <param name="isVerificationEnabled">Should perform verification or not?</param>
    /// <param name="fsImageResult">The file system image to write.</param>
    private void BurnMedia(DiscRecorder recorder, bool isVerificationEnabled, FileSystemImage.FileSystemImageResult fsImageResult)
    {
      try
      {
        m_discFormatData = new DiscFormatData();
        m_discFormatData.Progress += new Imapi.ProgressEventHandler(OnBurnMediaProgressHandler);
        recorder.AcquireExclusiveAccess(true, "DiscTransfer");

        //discData.Update += new DDiscFormat2DataEvents_UpdateEventHandler(OnMediaWriteUpdateHandler);
        if (m_discFormatData.IsRecorderSupported(recorder) && m_discFormatData.IsCurrentMediaSupported(recorder))
        {
          m_discFormatData.Recorder = recorder;
          m_discFormatData.ClientName = "DiscTransfer";
          m_discFormatData.VerificationLevel = isVerificationEnabled ? VerificationLevel.Full : VerificationLevel.None;

          m_discFormatData.Write(fsImageResult);
        }

        m_discFormatData.Progress += new Imapi.ProgressEventHandler(OnBurnMediaProgressHandler);
      }
      finally
      {
        m_discFormatData.Dispose();
        m_discFormatData = null;

        recorder.ReleaseExclusiveAccess();
      }
    } // BurnMedia( recorder, isVerificationEnabled, fsImageResult )


    /// <summary>
    /// Handles an impending erase operation.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnErasing(EraseEventArgs args)
    {
      if (Erasing != null)
      {
        Erasing(this, args);
      }
    } // OnErasing( args )


    /// <summary>
    /// Handles a completed Erase operation.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnErased(EventArgs args)
    {
      if (Erased != null)
      {
        Erased(this, args);
      }
    } // OnErased( args )


    /// <summary>
    /// Handles media not in the device.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnDeviceEmpty(DeviceEmptyEventArgs args)
    {
      if (DeviceEmpty != null)
      {
        DeviceEmpty(this, args);
      }
    } // OnLoad( args )


    /// <summary>
    /// Handles media not being able to contain the selected transfer items.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnMediaCapacityInssuffient(InsufficientMediaCapacityEventArgs args)
    {
      if(InsufficientMediaCapacity != null)
      {
        InsufficientMediaCapacity(this, args);
      }
    } // OnMediaCapacityInssuffient( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles media erase progress update event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="progress">The current progress.</param>
    /// <param name="progressTotal">The progress total.</param>
    private void OnMediaEraseProgressHandler(object sender, Imapi.ProgressEventArgs args)
    {
      OnProgress(new ProgressEventArgs(args.Message, args.Progress, args.Total));
    } // OnMediaEraseProgressHandler( sender, args )


    /// <summary>
    /// Handles burn media progress update event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="progress">The current progress.</param>
    /// <param name="progressTotal">The progress total.</param>
    private void OnBurnMediaProgressHandler(object sender, Imapi.ProgressEventArgs args)
    {
      OnProgress(new ProgressEventArgs(args.Message, args.Progress, args.Total));
    } // OnBurnMediaProgressHandler( sender, args )

    #endregion

  } // class DiscTransferModel
} // namespace Viatronix.Utilities.Transfer.Models
