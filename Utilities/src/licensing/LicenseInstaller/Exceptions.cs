using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Licensing.LicenseInstaller
{

  #region SystemIdMismatchException class

  /// <summary>
  /// Exception to indicate that the system id in the license file doesn't match that of this system.
  /// </summary>
  public class SystemIdMismatchException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public SystemIdMismatchException() : base()
    {
    } // SystemIdMismatchException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to include.</param>
    public SystemIdMismatchException(string message) : base(message)
    {
    } // SystemIdMismatchException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to include.</param>
    /// <param name="innerException">The inner exception.</param>
    public SystemIdMismatchException(string message, Exception innerException) : base(message, innerException)
    {
    } // SystemIdMismatchException()

    #endregion

  } // class SystemIdMismatchException

  #endregion

  #region InstallationAlreadyPerformedException class

  public class InstallationAlreadyPerformedException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public InstallationAlreadyPerformedException() : base()
    {
    } // InstallationAlreadyPerformedException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to include.</param>
    public InstallationAlreadyPerformedException(string message) : base(message)
    {
    } // InstallationAlreadyPerformedException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to include.</param>
    /// <param name="innerException">The inner exception.</param>
    public InstallationAlreadyPerformedException(string message, Exception innerException) : base(message, innerException)
    {
    } // InstallationAlreadyPerformedException()

    #endregion

  } // class InstallationAlreadyPerformedException

  #endregion

} // namespace Viatronix.Utilities.Licensing.LicenseInstaller
