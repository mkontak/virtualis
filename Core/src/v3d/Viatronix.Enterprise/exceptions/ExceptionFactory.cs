using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Enterprise
{
  /// <summary>
  /// Creates exceptions from an exception's message.
  /// </summary>
  public static class ExceptionFactory
  {

    #region methods

    /// <summary>
    /// Creates an exception of the specified type with the provided message.
    /// </summary>
    /// <param name="e">The provided exception.</param>
    /// <returns>Type specific exception with the provided message.</returns>
    public static Exception Create(Exception e)
    {
      return Create(e.Message);
    } // Create( e )
     

    /// <summary>
    /// Creates an exception of the specified type with the provided message.
    /// </summary>
    /// <param name="e">The provided exception.</param>
    /// <returns>Type specific exception with the provided message.</returns>
    public static Exception Create(string message)
    {
      // extract the message after the code:
      string msgtext = message.Substring(7).Trim();

      int errorCode;
      if (Int32.TryParse(message.Substring(0, 6), out errorCode))
      {
        return Create(msgtext, errorCode);
      } // END ... If the code can be converted to an int
      else
      {
        return new ApplicationException(message);
      }
    } // Create( e )


    /// <summary>
    /// Creates an exception of the specified type with the provided message.
    /// </summary>
    /// <param name="errorCode">Error Code</param>
    /// <returns>Type specific exception with the provided message.</returns>
    public static Exception Create(string message, int errorCode)
    {
      switch ((ErrorCodes)errorCode)
      {
        case ErrorCodes.AccountRestricted:
          return new AccountRestrictedException(message);
        case ErrorCodes.AlreadyExists:
          return new AlreadyExistsException(message);
        case ErrorCodes.ApplicationLock:
          return new ApplicationLockException(message);
        case ErrorCodes.CannotChangePasswordAgain:
          return new CannotChangePasswordAgainException(message);
        case ErrorCodes.CannotDeleteSystemAccount:
          return new CannotDeleteSystemAccountException(message);
        case ErrorCodes.CannotDeleteUserLoggedIn:
          return new CannotDeleteUserLoggedInException(message);
        case ErrorCodes.CannotLaunchFailedSeries:
          return new CannotLaunchFailedSeriesException(message);
        case ErrorCodes.CannotLaunchMultiSeriesLinkedToDicom:
          return new CannotLaunchMultiSeriesLinkedToDicomException(message);
        case ErrorCodes.CannotLaunchSpecifiedType:
          return new CannotLaunchSpecifiedTypeException(message);
        case ErrorCodes.CannotLaunchWhileProcessing:
          return new CannotLaunchWhileProcessingException(message);
        case ErrorCodes.CannotLaunchWhileProcessingWEntity:
          return new CannotLaunchWhileProcessingWEntityException(message);
        case ErrorCodes.CannotLoginToWorkstation:
          return new CannotLoginToWorkstationException(message);
        case ErrorCodes.CannotRegister:
          return new CannotRegisterException(message);
        case ErrorCodes.CannotReUsePassword:
          return new CannotReUsePasswordException(message);
        case ErrorCodes.CannotSpecifyMoreThanX:
          return new CannotSpecifyMoreThanXException(message);
        case ErrorCodes.CannotSpecifyXAndYTogether:
          return new CannotSpecifyXAndYTogetherException(message);
        case ErrorCodes.CannotUpdateSystemAccount:
          return new CannotUpdateSystemAccountException(message);
        case ErrorCodes.DeleteDirectoryFailed:
          return new DeleteDirectoryFailedException(message);
        case ErrorCodes.DeleteFileFailed:
          return new DeleteFileFailedException(message);
        case ErrorCodes.Disabled:
          return new DisabledException(message);
        case ErrorCodes.Expired:
          return new ExpiredException(message);
        case ErrorCodes.FailedSerialize:
          return new FailedSerializeException(message);
        case ErrorCodes.FailedToGetDiskSpace:
          return new FailedToGetDiskSpaceException(message);
        case ErrorCodes.InvalidAverageDatasetSize:
          return new InvalidAverageDatasetSizeException(message);
        case ErrorCodes.InvalidIdentity:
          return new InvalidIdentityException(message);
        case ErrorCodes.InvalidLoginHours:
          return new InvalidLoginHoursException(message);
        case ErrorCodes.InvalidStorageDevice:
          return new InvalidStorageDeviceException(message);
        case ErrorCodes.LicenseAlreadyAdded:
          return new LicensesAlreadyAddedException(message);
        case ErrorCodes.LicenseInUse:
          return new LicenseInUseException(message);
        case ErrorCodes.Locked:
          return new LockedException(message);
        case ErrorCodes.LoginCannotBeEmbeddedInPassword:
          return new LoginCannotBeEmbeddedInPasswordException(message);
        case ErrorCodes.LoginFailed:
          return new LoginFailedException(message);
        case ErrorCodes.LoginTypeNotGranted:
          return new LoginTypeNotGrantedException(message);
        case ErrorCodes.MissingAverageDatasetSetting:
          return new MissingAverageDatasetSettingException(message);
        case ErrorCodes.MissingInvalidDicomField:
          return new MissingInvalidDicomFieldException(message);
        case ErrorCodes.MissingLocalSettingForStorageDevice:
          return new MissingLocalSettingForStorageDeviceException(message);
        case ErrorCodes.MissingOrInvalid:
          return new MissingOrInvalidException(message);
        case ErrorCodes.NoAvailableLicenses:
          return new NoAvailableLicensesException(message);
        case ErrorCodes.NoDefault:
          return new NoDefaultException(message);
        case ErrorCodes.NoLaunchableDataFound:
          return new NoLaunchableDataFoundException(message);
        case ErrorCodes.NoStorageDevicesDefined:
          return new NoStorageDevicesDefinedException(message);
        case ErrorCodes.NotAuthorized:
          return new NotAuthorizedException(message);
        case ErrorCodes.NotFound:
          return new NotFoundException(message);
        case ErrorCodes.NotInstalled:
          return new NotInstalledException(message);
        case ErrorCodes.NotRegistered:
          return new NotRegisteredException(message);
        case ErrorCodes.OutOfSpace:
          return new OutOfSpaceException(message);
        case ErrorCodes.PasswordChangeRequired:
          return new PasswordChangeRequiredException(message);
        case ErrorCodes.PasswordInvalid:
          return new PasswordInvalidException(message);
        case ErrorCodes.PasswordMaximumXCharsExceeded:
          return new PasswordMaximumXCharsExceededException(message);
        case ErrorCodes.PasswordMinimumXCharsNotMet:
          return new PasswordMinimumXCharsNotMetException(message);
        case ErrorCodes.PasswordMustDiffer:
          return new PasswordMustDifferException(message);
        case ErrorCodes.Processing:
          return new ProcessingException(message);
        case ErrorCodes.SeriesContainsDependents:
          return new SeriesContainsDependentsException(message);
        case ErrorCodes.SeriesContainsNoImages:
          return new SeriesContainsNoImagesException(message);
        case ErrorCodes.SeriesContainsReference:
          return new SeriesContainsReferenceException(message);
        case ErrorCodes.SeriesHasNotBeenProcessedForApp:
          return new SeriesHasNotBeenProcessedForAppException(message);
        case ErrorCodes.SeriesProcessing:
          return new SeriesProcessingException(message);
        case ErrorCodes.ServerAlreadyRegistered:
          return new ServerAlreadyRegisteredException(message);
        case ErrorCodes.SessionDoesNotMatchApplication:
          return new SessionDoesNotMatchApplicationException(message);
        case ErrorCodes.SpacesNotAllowedInPassword:
          return new SpacesNotAllowedInPasswordException(message);
        case ErrorCodes.SystemDisabled:
          return new SystemDisabledException(message);
        case ErrorCodes.SystemError:
          return new SystemErrorException(message);
        case ErrorCodes.Unsupported:
          return new UnsupportedException(message);
        case ErrorCodes.UserAlreadyHasAContext:
          return new UserAlreadyHasContextException(message);
        default:
          return new ViatronixException(message, ErrorCodes.None);
      };
    } // Create( e )

    #endregion

  } // class ExceptionFactory
} // namespace Viatronix.Enterprise
