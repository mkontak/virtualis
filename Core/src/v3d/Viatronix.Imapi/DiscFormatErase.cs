using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

using Viatronix.Imapi.Interop;
using Viatronix.Imapi2.Interop;

namespace Viatronix.Imapi
{
  /// <summary>
  /// Represents a class that can erase a disc.
  /// </summary>
  /// <remarks>Wrapper class for IDiscFormat2Erase.</remarks>
  public class DiscFormatErase : IDisposable
  {

    #region events

    /// <summary>
    /// Notifies that progress has occurred during the erase.
    /// </summary>
    public event ProgressEventHandler Progress;

    #endregion

    #region fields

    /// <summary>
    /// The format erase.
    /// </summary>
    private MsftDiscFormat2Erase m_discFormatErase = null;

    /// <summary>
    /// Has this instance been disposed?
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DiscFormatErase()
    {
      m_discFormatErase = new MsftDiscFormat2Erase();
    } // DiscFormatErase()


    /// <summary>
    /// Destructor.
    /// </summary>
    ~DiscFormatErase()
    {
      Dispose(true);
    } // ~DiscFormatErase()

    #endregion

    #region properties 

    /// <summary>
    /// Gets or sets the client name.
    /// </summary>
    public string ClientName
    {
      get { return m_discFormatErase.ClientName; }
      set { m_discFormatErase.ClientName = value; }
    } // ClientName


    /// <summary>
    /// Gets or sets the physical media type.
    /// </summary>
    public MediaTypes CurrentPhysicalMediaType
    {
      get { return (MediaTypes)m_discFormatErase.CurrentPhysicalMediaType; }
    } // CurrentPhysicalMediaType


    /// <summary>
    /// Gets or sets the full erase.
    /// </summary>
    public bool FullErase
    {
      get { return m_discFormatErase.FullErase; }
      set { m_discFormatErase.FullErase = value; }
    } // FullErase


    /// <summary>
    /// Gets if the media is heuristically blank.
    /// </summary>
    public bool MediaHeuristicallyBlank
    {
      get { return m_discFormatErase.MediaHeuristicallyBlank; }
    } // MediaHeuristicallyBlank


    /// <summary>
    /// Gets if the media is heuristically blank.
    /// </summary>
    public bool MediaPhysicallyBlank
    {
      get { return m_discFormatErase.MediaPhysicallyBlank; }
    } // MediaHeuristicallyBlank


    /// <summary>
    /// Gets or sets the disc recorder.
    /// </summary>
    public DiscRecorder Recorder
    {
      get { return new DiscRecorder(m_discFormatErase.Recorder); }
      set { m_discFormatErase.Recorder = value.ComRecorder; }
    } // Recorder


    /// <summary>
    /// Gets the list of supported media types.
    /// </summary>
    public MediaTypes[] SupportedMediaTypes
    {
      get
      {
        List<MediaTypes> mediaTypes = new List<MediaTypes>();
        foreach (IMAPI_MEDIA_PHYSICAL_TYPE mediaType in m_discFormatErase.SupportedMediaTypes)
        {
          mediaTypes.Add((MediaTypes)mediaType);
        }

        return mediaTypes.ToArray();
      }
    } // SupportedMediaTypes

    #endregion

    #region methods

    /// <summary>
    /// Erases the media.
    /// </summary>
    public void EraseMedia()
    {
      ((DiscFormat2Erase_Event)m_discFormatErase).Update += new DiscFormat2Erase_EventHandler(OnEraseProgressHandler);

      m_discFormatErase.EraseMedia();

      ((DiscFormat2Erase_Event)m_discFormatErase).Update -= new DiscFormat2Erase_EventHandler(OnEraseProgressHandler);
    } // EraseMedia()


    /// <summary>
    /// Determines if the current media is supported.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    /// <returns>True if the media can be loaded, false otherwise.</returns>
    public bool IsCurrentMediaSupported(DiscRecorder recorder)
    {
      return m_discFormatErase.IsCurrentMediaSupported(recorder.ComRecorder);
    } // IsCurrentMediaSupported( recorder )


    /// <summary>
    /// Determines if the recorder is supported.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    /// <returns>True if the recorder is supported, false otheriwse.</returns>
    public bool IsRecorderSupported(DiscRecorder recorder)
    {
      return m_discFormatErase.IsRecorderSupported(recorder.ComRecorder);
    } // IsRecorderSupported( recorder )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles progress on the erase.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="progress">The progress.</param>
    /// <param name="total">The total.</param>
    private void OnEraseProgressHandler(object sender, int progress, int total)
    {
      if (Progress != null)
      {
        Progress(sender, new ProgressEventArgs("Erasing Media...", progress, total));
      }
    } // OnEraseProgressHandler( sender, progress, total )

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
        ((DiscFormat2Erase_Event)m_discFormatErase).Update -= new DiscFormat2Erase_EventHandler(OnEraseProgressHandler);
        Marshal.ReleaseComObject(m_discFormatErase);
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class DiscFormatErase
} // namespace Viatronix.Imapi
