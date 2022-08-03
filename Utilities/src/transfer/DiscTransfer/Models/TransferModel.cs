using System;
using System.IO;
using System.Threading;
using System.Collections.Generic;

using Viatronix.Imapi;

namespace Viatronix.Utilities.Transfer.DiscTransfer.Models
{
  /// <summary>
  /// Performs disc transfer using IMAPI version 2.
  /// </summary>
  public class TransferModel
  {

    #region events

    /// <summary>
    /// Notifies that the transfer has started.
    /// </summary>
    public event EventHandler TransferStarted;

    /// <summary>
    /// Notifies that the transfer has completed.
    /// </summary>
    public event EventHandler TransferCompleted;

    /// <summary>
    /// Notifies that the transfer has failed.
    /// </summary>
    public event ExceptionEventHandler TransferFailed;

    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    public event ProgressEventHandler Progress;

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
    public event LoadEventHandler LoadMedia;

    #endregion

    #region fields

    /// <summary>
    /// The disc format data to write files to the media.
    /// </summary>
    private DiscFormatData m_discFormatData = null;

    /// <summary>
    /// Was the operation cancelled.
    /// </summary>
    private bool m_cancelled = false;
    
    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TransferModel()
    {
    } // TransferModel()

    #endregion

    #region methods

    /// <summary>
    /// Performs the transfer.
    /// </summary>
    /// <param name="deviceId">The device id.</param>
    /// <param name="performVerification">Perform verification.</param>
    /// <param name="directory">The directory to transfer.</param>
    public void Transfer(string deviceId, bool performVerification, string directory)
    {
      Thread thread = new Thread(() => StartTransfer(deviceId, performVerification, directory));
      thread.Start();
    } // Transfer( deviceId, directory )


    /// <summary>
    /// Cancels the current operation.
    /// </summary>
    public void Cancel()
    {
      m_cancelled = true;
      if(m_discFormatData != null)
      {
        m_discFormatData.CancelWrite();
      }
    } // Cancel()


    /// <summary>
    /// Returns the list of devices.
    /// </summary>
    /// <returns>Queries the list of devices.</returns>
    public DeviceItem[] QueryDevices()
    {
      List<DeviceItem> devices = new List<DeviceItem>();
      using (DiscMaster discMaster = new DiscMaster())
      {
        for (int i = 0; i < discMaster.Count; ++i)
        {
          devices.Add(new DeviceItem(discMaster[i]));
        }

        return devices.ToArray();
      }
    } // QueryDevices()


    /// <summary>
    /// Starts the transfer, note that this occurs in a seperate thread.
    /// </summary>
    /// <param name="deviceId">The device id to perform the transfer.</param>
    /// <param name="isVerificationEnabled">Is verification enabled?</param>
    /// <param name="directory">The directory to transfer.</param>
    private void StartTransfer(string deviceId, bool isVerificationEnabled, string directory)
    {
      m_cancelled = false;

      DiscRecorder recorder = null;
      FileSystemImage.FileSystemImageResult fsImageResult = null;

      try
      {
        OnTransferStarted(EventArgs.Empty);

        recorder = new DiscRecorder();
        recorder.InitializeDiscRecorder(deviceId);

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

            LoadEventArgs args = new LoadEventArgs("Device is empty or could not load media.", true);

            OnLoadMedia(args);
            if (args.Abort)
            {
              throw new OperationCanceledException("User chose to abort transfer.");
            }
          }
          else
          {
            OnProgress(new ProgressEventArgs("Media Successfully Loaded", 2, 100));

            if (!IsMediaBlank(recorder))
            {
              OnProgress(new ProgressEventArgs("Media Not Blank", 3, 100));

              if (IsMediaErasable(recorder))
              {
                EraseEventArgs args = new EraseEventArgs(false) { Abort = false };
                OnErasing(args);

                if (!args.Erase)
                {
                  recorder.EjectMedia();
                  continue;
                }
                else if (args.Abort)
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
        fsImageResult = CreateImage(recorder, directory);
        
        // Burn the disc.
        CheckForCancel();
        OnProgress(new ProgressEventArgs("Burning Media...", 20, 100));
        BurnMedia(recorder, isVerificationEnabled, fsImageResult);
        
        // Notify that transfer was completed.
        CheckForCancel();
        OnProgress(new ProgressEventArgs("Successfully Transferred To Media!", 20, 100));
        OnTransferCompleted(EventArgs.Empty);
      }
      catch (Exception e)
      {
        OnTransferFailed(e);
      }
      finally
      {
        if (recorder != null)
        {
          recorder.EjectMedia();
          recorder.Dispose();
        }
      }
    } // StartTransfer( deviceId, isVerificationEnabled, directory )


    /// <summary>
    /// Detects if media is in the device.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    /// <returns></returns>
    private bool IsDeviceReady(DiscRecorder recorder)
    {
      using(DiscFormatData discData = new DiscFormatData())
      {
        if (discData.IsCurrentMediaSupported(recorder))
        {
          return true;
        }

        return false;
      }
    } // IsDeviceReady( recorder )


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
    /// Creates the disc image to prepare for burning.
    /// </summary>
    /// <param name="recorder">The disc recorder.</param>
    /// <param name="directory">The directory to transfer.</param>
    /// <returns>The fs image representation of directory.</returns>
    private FileSystemImage.FileSystemImageResult CreateImage(DiscRecorder recorder, string directory)
    {
      using(FileSystemImage fileSystemImage = new FileSystemImage())
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


    /// <summary>
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
    /// Handles the transfer being started.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnTransferStarted(EventArgs args)
    {
      if (TransferStarted != null)
      {
        TransferStarted(this, args);
      }
    } // OnTransferStarted( args )


    /// <summary>
    /// Handles the transfer completed.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnTransferCompleted(EventArgs args)
    {
      if (TransferCompleted != null)
      {
        TransferCompleted(this, args);
      }
    } // OnTransferCompleted( args )


    /// <summary>
    /// Handles the transfer failed.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnTransferFailed(Exception args)
    {
      if (TransferFailed != null)
      {
        TransferFailed(this, args);
      }
    } // OnTransferFailed( args )


    /// <summary>
    /// Handles an update in progress.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnProgress(ProgressEventArgs args)
    {
      if (Progress != null)
      {
        Progress(this, args);
      }
    } // OnProgress( args )


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
    protected virtual void OnLoadMedia(LoadEventArgs args)
    {
      if (LoadMedia != null)
      {
        LoadMedia(this, args);
      }
    } // OnLoadMedia( args )

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

  } // class TransferModel
} // namespace Viatronix.Utilities.Transfer.DiscTransfer.Models
