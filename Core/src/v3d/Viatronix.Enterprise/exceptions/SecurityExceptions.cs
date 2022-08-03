using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region LoginFailedException class

  /// <summary>
  /// The login has failed.
  /// </summary>
  public class LoginFailedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LoginFailedException(string message) : base(message, ErrorCodes.LoginFailed)
    {
    } // LoginFailedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception</param>
    public LoginFailedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.LoginFailed)
    {
    } // LoginFailedException( message, innerException )

    #endregion

  } // class LoginFailedException

  #endregion

  #region NotAuthorizedException class

  /// <summary>
  /// The user doesn't have authorization.
  /// </summary>
  public class NotAuthorizedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NotAuthorizedException(string message) : base(message, ErrorCodes.NotAuthorized)
    {
    } // NotAuthorizedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NotAuthorizedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NotAuthorized)
    {
    } // NotAuthorizedException( message, innerException )

    #endregion

  } // class NotAuthorizedException

  #endregion

  #region UserAlreadyHasContextException class

  /// <summary>
  /// The user already has a context!
  /// </summary>
  public class UserAlreadyHasContextException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public UserAlreadyHasContextException(string message) : base(message, ErrorCodes.UserAlreadyHasAContext)
    {
    } // UserAlreadyHasContextException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public UserAlreadyHasContextException(string message, Exception innerException) : base(message, innerException, ErrorCodes.UserAlreadyHasAContext)
    {
    } // UserAlreadyHasContextException( message, innerException )

    #endregion

  } // class UserAlreadyHasContextException

  #endregion

  #region PasswordInvalidException class

  /// <summary>
  /// The password is incorrect.
  /// </summary>
  public class PasswordInvalidException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public PasswordInvalidException(string message) : base(message, ErrorCodes.PasswordInvalid)
    {
    } // PasswordInvalidException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public PasswordInvalidException(string message, Exception innerException) : base(message, innerException, ErrorCodes.PasswordInvalid)
    {
    } // PasswordInvalidException( message, innerException )


    #endregion

  } // class PasswordInvalidException

  #endregion

  #region CannotUpdateSystemAccountException class

  /// <summary>
  /// The user attempted to modify a system user account.
  /// </summary>
  public class CannotUpdateSystemAccountException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public CannotUpdateSystemAccountException(string message) : base(message, ErrorCodes.CannotUpdateSystemAccount)
    {
    } // CannotUpdateSystemAccountException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotUpdateSystemAccountException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotUpdateSystemAccount)
    {
    } // CannotUpdateSystemAccountException( message, innerException )

    #endregion

  } // class CannotUpdateSystemAccountException

  #endregion

  #region CannotDeleteUserLoggedInException class

  /// <summary>
  /// The logged in user cannot be deleted.
  /// </summary>
  public class CannotDeleteUserLoggedInException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public CannotDeleteUserLoggedInException(string message) : base(message, ErrorCodes.CannotDeleteUserLoggedIn)
    {
    } // CannotDeleteUserLoggedInException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotDeleteUserLoggedInException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotDeleteUserLoggedIn)
    {
    } // CannotDeleteUserLoggedInException( message, innerException )

    #endregion

  } // class CannotDeleteUserLoggedInException

  #endregion

  #region CannotDeleteSystemAccountException class

  /// <summary>
  /// Cannot delete a system account.
  /// </summary>
  public class CannotDeleteSystemAccountException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public CannotDeleteSystemAccountException(string message) : base(message, ErrorCodes.CannotDeleteSystemAccount)
    {
    } // CannotDeleteSystemAccountException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotDeleteSystemAccountException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotDeleteSystemAccount)
    {
    } // CannotDeleteSystemAccountException( message, innerException )

    #endregion

  } // class CannotDeleteSystemAccountException

  #endregion

  #region AccountRestrictedException class

  /// <summary>
  /// The account has been disabled.
  /// </summary>
  public class AccountRestrictedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public AccountRestrictedException(string message) : base(message, ErrorCodes.AccountRestricted)
    {
    } // AccountRestrictedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public AccountRestrictedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.AccountRestricted)
    {
    } // AccountRestrictedException( message, innerException )

    #endregion

  } // class AccountRestrictedException

  #endregion
  
  #region InvalidLoginHoursException class

  /// <summary>
  /// The user can only login during normal buisness hours.
  /// </summary>
  public class InvalidLoginHoursException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public InvalidLoginHoursException(string message) : base(message, ErrorCodes.InvalidLoginHours)
    {
    } // InvalidLoginHoursException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidLoginHoursException(string message, Exception innerException) : base(message, innerException, ErrorCodes.InvalidLoginHours)
    {
    } // InvalidLoginHoursException( message, innerException )

    #endregion

  } // class InvalidLoginHoursException

  #endregion

  #region CannotLoginToWorkstationException class

  /// <summary>
  /// Cannot login to specified workstation.
  /// </summary>
  public class CannotLoginToWorkstationException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public CannotLoginToWorkstationException(string message) : base(message, ErrorCodes.CannotLoginToWorkstation)
    {
    } // CannotLoginToWorkstationException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLoginToWorkstationException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLoginToWorkstation)
    {
    } // CannotLoginToWorkstationException( message, innerException )

    #endregion

  } // class CannotLoginToWorkstationException

  #endregion

  #region LoginTypeNotGrantedException class

  /// <summary>
  /// Login type was not granted.
  /// </summary>
  public class LoginTypeNotGrantedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LoginTypeNotGrantedException(string message) : base(message, ErrorCodes.LoginTypeNotGranted)
    {
    } // LoginTypeNotGrantedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public LoginTypeNotGrantedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.LoginTypeNotGranted)
    {
    } // LoginTypeNotGrantedException( message, innerException )

    #endregion

  } // class LoginTypeNotGrantedException

  #endregion

  #region InvalidIdentityException class

  /// <summary>
  /// Identity is invalid.
  /// </summary>
  public class InvalidIdentityException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public InvalidIdentityException(string message) : base(message, ErrorCodes.InvalidIdentity)
    {
    } // InvalidIdentityException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidIdentityException(string message, Exception innerException) : base(message, innerException, ErrorCodes.InvalidIdentity)
    {
    } // InvalidIdentityException( message, innerException )

    #endregion

  } // class InvalidIdentityException

  #endregion

  #region PasswordMustDifferException class

  /// <summary>
  /// Password must differ.
  /// </summary>
  public class PasswordMustDifferException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public PasswordMustDifferException(string message) : base(message, ErrorCodes.PasswordMustDiffer)
    {
    } // PasswordMustDifferException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public PasswordMustDifferException(string message, Exception innerException) : base(message, innerException, ErrorCodes.PasswordMustDiffer)
    {
    } // PasswordMustDifferException( message, innerException )

    #endregion

  } // class PasswordMustDifferException

  #endregion

  #region LoginCannotBeEmbeddedInPasswordException class

  /// <summary>
  /// The password cannot contain the username.
  /// </summary>
  public class LoginCannotBeEmbeddedInPasswordException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LoginCannotBeEmbeddedInPasswordException(string message) : base(message, ErrorCodes.LoginCannotBeEmbeddedInPassword)
    {
    } // LoginCannotBeEmbeddedInPasswordException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public LoginCannotBeEmbeddedInPasswordException(string message, Exception innerException) : base(message, innerException, ErrorCodes.LoginCannotBeEmbeddedInPassword)
    {
    } // LoginCannotBeEmbeddedInPasswordException( message, innerException )

    #endregion

  } // class LoginCannotBeEmbeddedInPasswordException

  #endregion

  #region SpacesNotAllowedInPasswordException class

  /// <summary>
  /// A password cannot contain spaces.
  /// </summary>
  public class SpacesNotAllowedInPasswordException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SpacesNotAllowedInPasswordException(string message) : base(message, ErrorCodes.SpacesNotAllowedInPassword)
    {
    } // SpacesNotAllowedInPasswordException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SpacesNotAllowedInPasswordException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SpacesNotAllowedInPassword)
    {
    } // SpacesNotAllowedInPasswordException( message, innerException )

    #endregion

  } // class SpacesNotAllowedInPasswordException

  #endregion

  #region CannotReUsePasswordException class

  /// <summary>
  /// Cannot reuse a previously used password.
  /// </summary>
  public class CannotReUsePasswordException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotReUsePasswordException(string message) : base(message, ErrorCodes.CannotReUsePassword)
    {
    } // CannotReUsePasswordException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotReUsePasswordException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotReUsePassword)
    {
    } // CannotReUsePasswordException( message, innerException )

    #endregion

  } // class CannotReUsePasswordException

  #endregion

  #region CannotChangePasswordAgainException class

  /// <summary>
  /// Cannot change the password again.
  /// </summary>
  public class CannotChangePasswordAgainException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotChangePasswordAgainException(string message) : base(message, ErrorCodes.CannotChangePasswordAgain)
    {
    } // CannotChangePasswordAgainException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotChangePasswordAgainException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotChangePasswordAgain)
    {
    } // CannotChangePasswordAgainException( message, innerException )

    #endregion

  } // class CannotChangePasswordAgainException

  #endregion

  #region PasswordChangeRequiredException class

  /// <summary>
  /// @TODO Fill in
  /// </summary>
  public class PasswordChangeRequiredException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public PasswordChangeRequiredException(string message) : base(message, ErrorCodes.PasswordChangeRequired)
    {
    } // PasswordChangeRequiredException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public PasswordChangeRequiredException(string message, Exception innerException) : base(message, innerException, ErrorCodes.PasswordChangeRequired)
    {
    } // PasswordChangeRequiredException( message, innerException )

    #endregion

  } // class PasswordChangeRequiredException

  #endregion

  #region PasswordMinimumXCharsNotMetException class

  /// <summary>
  /// The minimum number of whatever characters were not met.
  /// </summary>
  public class PasswordMinimumXCharsNotMetException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public PasswordMinimumXCharsNotMetException(string message) : base(message, ErrorCodes.PasswordMinimumXCharsNotMet)
    {
    } // PasswordMinimumXCharsNotMetException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public PasswordMinimumXCharsNotMetException(string message, Exception innerException) : base(message, innerException, ErrorCodes.PasswordMinimumXCharsNotMet)
    {
    } // PasswordMinimumXCharsNotMetException( message, innerException )

    #endregion

  } // class PasswordMinimumXCharsNotMetException

  #endregion

  #region PasswordMaximumXCharsExceededException class

  /// <summary>
  /// The maximum number of whatever character was not met.
  /// </summary>
  public class PasswordMaximumXCharsExceededException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public PasswordMaximumXCharsExceededException(string message) : base(message, ErrorCodes.PasswordMaximumXCharsExceeded)
    {
    } // PasswordMaximumXCharsExceededException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public PasswordMaximumXCharsExceededException(string message, Exception innerException) : base(message, innerException, ErrorCodes.PasswordMaximumXCharsExceeded)
    {
    } // PasswordMaximumXCharsExceededException( message, innerException )

    #endregion

  } // class PasswordMaximumXCharsExceededException

  #endregion

} // namespace Viatronix.Enterprise
