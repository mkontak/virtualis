using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Licensing
{
  /// <summary>
  /// Exception to denote the license file is malformed.
  /// </summary>
  public class LicenseFileMalformedException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public LicenseFileMalformedException() : base()
    {
    } // LicenseFileMalformedException()


    /// <summary>
    /// Constructor.
    /// </summary>
    public LicenseFileMalformedException(string message) : base(message)
    {
    } // LicenseFileMalformedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    public LicenseFileMalformedException(string message, Exception inner) : base(message, inner)
    {
    } // LicenseFileMalformedException()

    #endregion

  } // class LicenseFileMalformedException
} // namespace Viatronix.Licensing
