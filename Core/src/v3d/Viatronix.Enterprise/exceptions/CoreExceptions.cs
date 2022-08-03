using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region SystemDisabledException class

  /// <summary>
  /// System is disabled exception.
  /// </summary>
  public class SystemDisabledException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public SystemDisabledException(string message) : base(message, ErrorCodes.SystemDisabled)
    {
    } // SystemDisabledException( message )


    public SystemDisabledException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SystemDisabled)
    {
    } // SystemDisabledException( message, innerException )

    #endregion

  } // class SystemDisabledException

  #endregion

  #region NotInstalledException class

  /// <summary>
  /// Not installed exception.
  /// </summary>
  public class NotInstalledException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NotInstalledException(string message) : base(message, ErrorCodes.NotInstalled)
    {
    } // NotInstalledException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NotInstalledException(string message, Exception innerException) : base(message, ErrorCodes.NotInstalled)
    {
    } // NotInstalledException( message, innerException )

    #endregion

  } // class NotInstalledException

  #endregion

  #region NotRegisteredException class

  /// <summary>
  /// Not registered exception.
  /// </summary>
  public class NotRegisteredException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NotRegisteredException(string message) : base(message, ErrorCodes.NotRegistered)
    {
    } // NotRegisteredException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NotRegisteredException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NotRegistered)
    {
    } // NotRegisteredException( message, innerException )

    #endregion

  } // class NotRegisteredException

  #endregion

  #region ProcessingException class

  /// <summary>
  /// Series is processing exception.
  /// </summary>
  public class ProcessingException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public ProcessingException(string message) : base(message, ErrorCodes.Processing)
    {
    } // ProcessingException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public ProcessingException(string message, Exception innerException) : base(message, innerException, ErrorCodes.Processing)
    {
    } // ProcessingException( message, innerException )

    #endregion

  } // class ProcessingException

  #endregion

  #region ApplicationLockException class

  /// <summary>
  /// Application lock failed exception.
  /// </summary>
  public class ApplicationLockException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public ApplicationLockException(string message) : base(message, ErrorCodes.ApplicationLock)
    {
    } // ApplicationLockException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public ApplicationLockException(string message, Exception innerException) : base(message, innerException, ErrorCodes.ApplicationLock)
    {
    } // ApplicationLockException( message, innerException )

    #endregion

  } // class ApplicationLockException

  #endregion

  #region MissingOrInvalidException class

  /// <summary>
  /// Entity is missing or invalid.
  /// </summary>
  public class MissingOrInvalidException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public MissingOrInvalidException(string message) : base(message, ErrorCodes.MissingOrInvalid)
    {
    } // MissingOrInvalidException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public MissingOrInvalidException(string message, Exception innerException) : base(message, innerException, ErrorCodes.MissingOrInvalid)
    {
    } // MissingOrInvalidException( message, innerException )

    #endregion

  } // class MissingOrInvalidException

  #endregion

  #region NotFoundException class

  /// <summary>
  /// The entity was not found.
  /// </summary>
  public class NotFoundException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NotFoundException(string message) : base(message, ErrorCodes.NotFound)
    {
    } // NotFoundException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NotFoundException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NotFound)
    {
    } // NotFoundException( message, innerException )

    #endregion

  } // class NotFoundException

  #endregion

  #region AlreadyExistsException class

  /// <summary>
  /// The entity already exists.
  /// </summary>
  public class AlreadyExistsException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public AlreadyExistsException(string message) : base(message, ErrorCodes.AlreadyExists)
    {
    } // AlreadyExistsException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public AlreadyExistsException(string message, Exception innerException) : base(message, innerException, ErrorCodes.AlreadyExists)
    {
    } // AlreadyExistsException( message, innerException )

    #endregion

  } // class AlreadyExistsException

  #endregion

  #region ExpiredException class

  /// <summary>
  /// The entity has expired.
  /// </summary>
  public class ExpiredException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public ExpiredException(string message) : base(message, ErrorCodes.Expired)
    {
    } // ExpiredException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public ExpiredException(string message, Exception innerException) : base(message, innerException, ErrorCodes.Expired)
    {
    } // ExpiredException( message, innerException )

    #endregion

  } // class ExpiredException

  #endregion

  #region LockedException class

  /// <summary>
  /// An action cannot be performed because the entity is locked.
  /// </summary>
  public class LockedException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public LockedException(string message) : base(message, ErrorCodes.Locked)
    {
    } // LockedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public LockedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.Locked)
    {
    } // LockedException( message, innerException )

    #endregion

  } // class LockedException

  #endregion

  #region UnsupportedException class

  /// <summary>
  /// The entity/operation/... is unsupporteed.
  /// </summary>
  public class UnsupportedException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public UnsupportedException(string message) : base(message, ErrorCodes.Unsupported)
    {
    } // UnsupportedException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerExcption">The inner exception.</param>
    public UnsupportedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.Unsupported)
    {
    } // UnsupportedException( message, innerException )

    #endregion

  } // class UnsupportedException

  #endregion

  #region NoDefaultException class

  /// <summary>
  /// No default was set.
  /// </summary>
  public class NoDefaultException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public NoDefaultException(string message) : base(message, ErrorCodes.NoDefault)
    {
    } // NoDefaultException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">the inner exception.</param>
    public NoDefaultException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NoDefault)
    {
    } // NoDefaultException( message, innerException )

    #endregion

  } // class NoDefaultException

  #endregion

  #region DisabledException class

  /// <summary>
  /// The entity is disabled.
  /// </summary>
  public class DisabledException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public DisabledException(string message) : base(message, ErrorCodes.Disabled)
    {
    } // DisabledException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public DisabledException(string message, Exception innerException) : base(message, innerException, ErrorCodes.Disabled)
    {
    } // DisabledException( message, innerException )

    #endregion

  } // class DisabledException

  #endregion

  #region SystemErrorException class

  /// <summary>
  /// System error exception.
  /// </summary>
  public class SystemErrorException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public SystemErrorException(string message) : base(message, ErrorCodes.SystemError)
    {
    } // SystemErrorException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SystemErrorException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SystemError)
    {
    } // SystemErrorException( message, innerException )

    #endregion

  } // class SystemErrorException

  #endregion

} // namespace Viatronix.Enterprise