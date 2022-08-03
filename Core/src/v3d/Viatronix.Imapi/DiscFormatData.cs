using System;
using System.Runtime.InteropServices;

using Viatronix.Imapi.Interop;
using Viatronix.Imapi2.Interop;

namespace Viatronix.Imapi
{
  /// <summary>
  /// Performs the write to the media.
  /// </summary>
  /// <remarks>This is a wrapper class for IMAPI's IDiscFormat2Data.</remarks>
  public class DiscFormatData : IDisposable
  {

    #region events

    /// <summary>
    /// Notifies that progress has occurred during the write.
    /// </summary>
    public event ProgressEventHandler Progress;

    #endregion

    #region fields

    /// <summary>
    /// Object to perform the operation.
    /// </summary>
    private MsftDiscFormat2Data m_discFormatData = null;

    /// <summary>
    /// Currently disposing?
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DiscFormatData()
    {
      m_discFormatData = new MsftDiscFormat2Data();
    } // DiscFormatData()


    /// <summary>
    /// Destructor.
    /// </summary>
    ~DiscFormatData()
    {
      Dispose(true);
    } // ~DiscFormatData()

    #endregion

    #region properties

    /// <summary>
    /// Gets if the media is heuristically blank.
    /// </summary>
    public bool MediaHeuristicallyBlank
    {
      get { return m_discFormatData.MediaHeuristicallyBlank; }
    } // MediaHeuristicallyBlank


    /// <summary>
    /// Gets if the media is physically blank.
    /// </summary>
    public bool MediaPhysicallyBlank
    {
      get { return m_discFormatData.MediaPhysicallyBlank; }
    } // MediaPhysicallyBlank


    /// <summary>
    /// Gets the current media type inserted in the device.
    /// </summary>
    public MediaTypes CurrentPhysicalMediaType
    {
      get { return (MediaTypes)m_discFormatData.CurrentPhysicalMediaType; }
    } // CurrentPhysicalMediaType


    /// <summary>
    /// Gets or sets the recorder.
    /// </summary>
    public DiscRecorder Recorder
    {
      get { return new DiscRecorder(m_discFormatData.Recorder); }
      set { m_discFormatData.Recorder = value.ComRecorder; }
    } // Recorder


    /// <summary>
    /// Gets or sets whether to force the media to be closed.
    /// </summary>
    public bool ForceMediaToBeClosed
    {
      get { return m_discFormatData.ForceMediaToBeClosed; }
      set { m_discFormatData.ForceMediaToBeClosed = value; }
    } // ForceMediaToBeClosed


    /// <summary>
    /// Gets or sets the client name.
    /// </summary>
    public string ClientName
    {
      get { return m_discFormatData.ClientName; }
      set { m_discFormatData.ClientName = value; }
    } // ClientName


    /// <summary>
    /// Gets or sets the verification level.
    /// </summary>
    public VerificationLevel VerificationLevel
    {
      get { return (VerificationLevel)((IBurnVerification)m_discFormatData).BurnVerificationLevel; }
      set { ((IBurnVerification)m_discFormatData).BurnVerificationLevel = (IMAPI_BURN_VERIFICATION_LEVEL)value; }
    } // VerificationLevel

    #endregion

    #region methods

    /// <summary>
    /// Cancels the write operation.
    /// </summary>
    public void CancelWrite()
    {
      m_discFormatData.CancelWrite();
    } // CancelWrite()
    

    /// <summary>
    /// Determines if the inserted media is supported.
    /// </summary>
    /// <returns>True if the media can be loaded, false otherwise.</returns>
    public bool IsCurrentMediaSupported(DiscRecorder recorder)
    {
      return m_discFormatData.IsCurrentMediaSupported(recorder.ComRecorder);
    } // IsCurrentMediaSupported( recorder )


    /// <summary>
    /// Is the 
    /// </summary>
    /// <param name="recorder"></param>
    /// <returns></returns>
    public bool IsRecorderSupported(DiscRecorder recorder)
    {
      return m_discFormatData.IsRecorderSupported(recorder.ComRecorder);
    } // IsCurrentRecorderSupported( recorder )


    /// <summary>
    /// Writes the file system image result to the device.
    /// </summary>
    /// <param name="fileImageResult">The file image result.</param>
    public void Write(FileSystemImage.FileSystemImageResult fileImageResult)
    {
      ((DiscFormat2Data_Event)m_discFormatData).Update += new DiscFormat2Data_EventHandler(OnDiscDataUpdateHandler);

      try
      {
        m_discFormatData.Write((MsftStreamZero)fileImageResult.Stream);
      }
      catch (COMException e)
      {
        switch ((uint)e.HResult)
        {
          case 0xC0AA0002:
            throw new OperationCanceledException("Operation was cancelled.");
          default:
            throw;
        }
      }

      ((DiscFormat2Data_Event)m_discFormatData).Update -= new DiscFormat2Data_EventHandler(OnDiscDataUpdateHandler);
    } // Write( fileImageResult )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the disc data's Update event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="progress">Arguments to take into account.</param>
    private void OnDiscDataUpdateHandler(object sender, object progress)
    {
      if (Progress != null)
      {
        IDiscFormat2DataEventArgs args = (IDiscFormat2DataEventArgs)progress;
        switch(args.CurrentAction)
        {
          case IMAPI_FORMAT2_DATA_WRITE_ACTION.IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA:
            Progress(sender, new ProgressEventArgs("Burning Media...", args.ElapsedTime, args.TotalTime));
            break;
          case IMAPI_FORMAT2_DATA_WRITE_ACTION.IMAPI_FORMAT2_DATA_WRITE_ACTION_VERIFYING:
            Progress(sender, new ProgressEventArgs("Verifiying Media...", args.ElapsedTime, args.TotalTime));
            break;
        }
      }
    } // OnDiscDataUpdateHandler( sender, progress )

    #endregion

    #region IDisposable implementation

    /// <summary>
    /// Performs dispose.
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    } // Dispose()


    /// <summary>
    /// Disposes of this object.
    /// </summary>
    /// <param name="disposing">Disposing?</param>
    protected virtual void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {
          // Dispose managed resources.
        }

        // Dispose unmanaged resources.
        Marshal.ReleaseComObject(m_discFormatData);
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class DiscFormatData
} // namespace Viatronix.Imapi
