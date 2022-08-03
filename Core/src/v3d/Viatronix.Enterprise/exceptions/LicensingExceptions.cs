using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region CannotRegisterLicensingException class

  /// <summary>
  /// Cannot register exception.
  /// </summary>
  public class CannotRegisterException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public CannotRegisterException(string message) : base(message, ErrorCodes.CannotRegister)
    {
    } // CannotRegisterException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotRegisterException(string message, Exception innerException) : base(message, ErrorCodes.CannotRegister)
    {
    } // CannotRegisterException( message, innerException )

    #endregion

  } // CannotRegisterException

  #endregion

  #region NoAvailableLicensesException class

  /// <summary>
  /// No available licenses exception.
  /// </summary>
  public class NoAvailableLicensesException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NoAvailableLicensesException(string message) : base(message, ErrorCodes.NoAvailableLicenses)
    {
    } // NoAvailableLicensesException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NoAvailableLicensesException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NoAvailableLicenses)
    {
    } // NoAvailableLicensesException( message, innerException )

    #endregion

  } // class NoAvailableLicensesException

  #endregion

  #region LicenseInUseException class

  /// <summary>
  /// The license is in use.
  /// </summary>
  public class LicenseInUseException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LicenseInUseException(string message) : base(message, ErrorCodes.LicenseInUse)
    {
    } // LicenseInUseException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LicenseInUseException(string message, Exception innerException) : base(message, innerException, ErrorCodes.LicenseInUse)
    {
    } // LicenseInUseException( message, innerException )

    #endregion

  } // class LicenseInUseException

  #endregion

  #region ServerAlreadyRegisteredException class

  /// <summary>
  /// Exception imparting that the server has already been registered.
  /// </summary>
  public class ServerAlreadyRegisteredException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public ServerAlreadyRegisteredException(string message) : base(message, ErrorCodes.ServerAlreadyRegistered)
    {
    } // ServerAlreadyRegisteredException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public ServerAlreadyRegisteredException(string message, Exception innerException) : base(message, innerException, ErrorCodes.ServerAlreadyRegistered)
    {
    } // ServerAlreadyRegisteredException( message )


    #endregion

  } // class ServerAlreadyRegisteredException

  #endregion

  #region LicensesAlreadyAddedException class

  /// <summary>
  /// Licenses were added.
  /// </summary>
  public class LicensesAlreadyAddedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LicensesAlreadyAddedException(string message) : base(message, ErrorCodes.LicenseAlreadyAdded)
    {
    } // LicensesAlreadyAddedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public LicensesAlreadyAddedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.LicenseAlreadyAdded)
    {
    } // LicensesAlreadyAddedException( message, innerException )

    #endregion

  } // class LicensesAlreadyAddedException

  #endregion

} // namespace Viatronix.Enterprise
