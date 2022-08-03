using System;
using System.Runtime.InteropServices;

using Viatronix.Imapi2.Interop;

namespace Viatronix.Imapi
{
  /// <summary>
  /// Represents a recorder.
  /// </summary>
  /// <remarks>Wrapper class for IMAPI IDiscRecorder2.</remarks>
  public class DiscRecorder : IDisposable
  {

    #region fields

    /// <summary>
    /// The disc recorder.
    /// </summary>
    private MsftDiscRecorder2 m_recorder = null;

    /// <summary>
    /// Has this instance been disposed.
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DiscRecorder()
    {
      m_recorder = new MsftDiscRecorder2();
    } // DiscRecorder()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="recorder">The disc recorder.</param>
    internal DiscRecorder(MsftDiscRecorder2 recorder)
    {
      m_recorder = recorder;
    } // DiscRecorder( recorder )


    /// <summary>
    /// Destructor.
    /// </summary>
    ~DiscRecorder()
    {
      Dispose(true);
    } // ~DiscRecorder()

    #endregion

    #region properties

    /// <summary>
    /// Gets the active disc recorder.
    /// </summary>
    public string ActiveDiscRecorder
    {
      get { return m_recorder.ActiveDiscRecorder; }
    } // ActiveDiscRecorder


    /// <summary>
    /// Gets the product id.
    /// </summary>
    public string ProductId
    {
      get { return m_recorder.ProductId; }
    } // ProductId


    /// <summary>
    /// Gets the product revision.
    /// </summary>
    public string ProductRevision
    {
      get { return m_recorder.ProductRevision; }
    } // ProductRevision


    /// <summary>
    /// Gets the recorder.
    /// </summary>
    internal MsftDiscRecorder2 ComRecorder
    {
      get { return m_recorder; }
    } // ComRecorder
    
    #endregion

    #region methods

    /// <summary>
    /// Initializes the recorder.
    /// </summary>
    /// <param name="deviceId">The device id.</param>
    public void InitializeDiscRecorder(string deviceId)
    {
      m_recorder.InitializeDiscRecorder(deviceId);
    } // InitializeDiscRecorder( deviceId )


    /// <summary>
    /// Acquires exclusive access to the device.
    /// </summary>
    /// <param name="force">Force exclusive access?</param>
    /// <param name="clientName">The client name.</param>
    public void AcquireExclusiveAccess(bool force, string clientName)
    {
      m_recorder.AcquireExclusiveAccess(force, clientName);
    } // AcquireExclusiveAccess( force, clientName )

    
    /// <summary>
    /// Closes the tray.
    /// </summary>
    public void CloseTray()
    {
      m_recorder.CloseTray();
    } // CloseTray()


    /// <summary>
    /// Ejects the media.
    /// </summary>
    public void EjectMedia()
    {
      m_recorder.EjectMedia();
    } // EjectMedia()


    /// <summary>
    /// Releases exclusive access to the device.
    /// </summary>
    public void ReleaseExclusiveAccess()
    {
      m_recorder.ReleaseExclusiveAccess();
    } // ReleaseExclusiveAccess()

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
        Marshal.ReleaseComObject(m_recorder);
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class DiscRecorder
} // namespace Viatronix.Imapi
