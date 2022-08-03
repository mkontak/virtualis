using System;
using System.Collections;
using System.Runtime.InteropServices;

using Viatronix.Imapi2.Interop;

namespace Viatronix.Imapi
{
  /// <summary>
  /// Returns a list of recorders.
  /// </summary>
  /// <remarks>Wrapper class for IMAPI2 IDiscMaster2.</remarks>
  public class DiscMaster : IDisposable
  {

    #region fields

    /// <summary>
    /// The disc master.
    /// </summary>
    private MsftDiscMaster2 m_discMaster = null;

    /// <summary>
    /// Has this object been disposed?
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DiscMaster()
    {
      m_discMaster = new MsftDiscMaster2();
    } // DiscMaster()


    /// <summary>
    /// Destructor.
    /// </summary>
    ~DiscMaster()
    {
      Dispose(true);
    } // ~DiscMaster()

    #endregion

    #region properties

    /// <summary>
    /// Gets the list of devices attached to the system.
    /// </summary>
    public int Count
    {
      get { return m_discMaster.Count; }
    } // Count


    /// <summary>
    /// Gets the deviceId at the specified index.
    /// </summary>
    /// <param name="index">The index.</param>
    /// <returns>The device id at the specified index.</returns>
    public string this[int index]
    {
      get { return m_discMaster[index]; }
    } // this[ index ]

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
        Marshal.ReleaseComObject(m_discMaster);
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class DiscMaster
} // namespace Viatronix.Imapi
