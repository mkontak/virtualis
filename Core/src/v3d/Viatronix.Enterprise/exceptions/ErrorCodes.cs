// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ErrorCodes.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;

namespace Viatronix
{
  /// <summary>
  /// The list of database error codes
  /// </summary>
  public enum ErrorCodes : int
  {

    /// <summary>
    /// The default value.
    /// </summary>
    None = -1,

    #region core errors

    /// <summary>
    /// System Disabled = 900980
    /// </summary>
    SystemDisabled = 900980,

    /// <summary>
    /// Not Installed = 900981
    /// </summary>
    NotInstalled = 900981, 

    /// <summary>
    /// Not Registered = 900982
    /// </summary>
    NotRegistered = 900982,

    /// <summary>
    /// Processing = 900983
    /// </summary>
    Processing = 900983,

    /// <summary>
    /// Application Lock Failed = 900984
    /// </summary>
    ApplicationLock = 900984,

    /// <summary>
    /// [Object/Entity] is missing or invalid = 900990
    /// </summary>
    MissingOrInvalid = 900990,

    /// <summary>
    /// [Object/Entity] not found = 900991
    /// </summary>
    NotFound = 900991,

    /// <summary>
    /// [Object/Entity] already exists = 900992
    /// </summary>
    AlreadyExists = 900992,

    /// <summary>
    /// [Object/Entity] has expired = 900993
    /// </summary>
    Expired = 900993,

    /// <summary>
    /// [Object/Entity] is locked = 900994
    /// </summary>
    Locked = 900994, 
    
    /// <summary>
    /// [Object/Entity] is unsupported = 900995
    /// </summary>
    Unsupported = 900995,
 
    /// <summary>
    /// [Object/Entity] no default value found = 900996 
    /// </summary>
    NoDefault = 900996,

    /// <summary>
    /// [Object/Entity] is disabled = 900997
    /// </summary>
    Disabled = 900997,

    /// <summary>
    /// A system error has occurred = 900998
    /// </summary>
    SystemError = 900998,
    
    #endregion

    #region licensing errors

    /// <summary>
    /// Cannot register the server = 901000
    /// </summary>
    CannotRegister = 901000,

    /// <summary>
    /// All licenses are in use = 901001
    /// </summary>
    NoAvailableLicenses = 901001,

    /// <summary>
    /// Specified License is in use = 901002
    /// </summary>
    LicenseInUse = 901002,

    /// <summary>
    /// Server has already been registered = 901003
    /// </summary>
    ServerAlreadyRegistered = 901003,

    /// <summary>
    /// Licenses have already been added = 901004
    /// </summary>
    LicenseAlreadyAdded = 901004,

    #endregion

    #region security errors

    /// <summary>
    /// An error occurred in performing the login = 901200
    /// </summary>
    LoginFailed = 901200,

    /// <summary>
    /// The user does not have the authorization to perform the desired action = 901201
    /// </summary>
    NotAuthorized = 901201,

    /// <summary>
    /// User laready is logged in = 901202
    /// </summary>
    UserAlreadyHasAContext = 901202,

    /// <summary>
    /// The password entered is invalid = 901203
    /// </summary>
    PasswordInvalid = 901203,

    /// <summary>
    /// Cannot update/change/modify a system account = 901204
    /// </summary>
    CannotUpdateSystemAccount = 901204,

    /// <summary>
    /// Cannot delete the user who is logged in = 901205
    /// </summary>
    CannotDeleteUserLoggedIn = 901205,

    /// <summary>
    /// Cannot delete a system account = 901206
    /// </summary>
    CannotDeleteSystemAccount = 901206,

    /// <summary>
    /// Account is restricted = 901207
    /// </summary>
    AccountRestricted = 901207,

    /// <summary>
    /// User is attempting to login during restricted hours = 901208
    /// </summary>
    InvalidLoginHours = 901208,

    /// <summary>
    /// Cannot login to specified workstation = 901209
    /// </summary>
    CannotLoginToWorkstation = 901209,

    /// <summary>
    /// Login type was not granted = 901211
    /// </summary>
    LoginTypeNotGranted = 901211,

    /// <summary>
    /// Invalid login identity = 901212
    /// </summary>
    InvalidIdentity = 901212,

    /// <summary>
    /// Passwords must differ by x number of characters = 901213
    /// </summary>
    PasswordMustDiffer = 901213,

    /// <summary>
    /// Login cannot be embedded in password = 901214
    /// </summary>
    LoginCannotBeEmbeddedInPassword = 901214,

    /// <summary>
    /// Spaces are not allowed in a password = 901217
    /// </summary>
    SpacesNotAllowedInPassword = 901217,

    /// <summary>
    /// Cannot re-use a previous password = 901221
    /// </summary>
    CannotReUsePassword = 901221,

    /// <summary>
    /// Cannot change a password again = 901222
    /// </summary>
    CannotChangePasswordAgain = 901222,

    /// <summary>
    /// Password change is required = 901223
    /// </summary>
    PasswordChangeRequired = 901223,

    /// <summary>
    /// Password minimum of charaters wasn't met = 901225
    /// </summary>
    PasswordMinimumXCharsNotMet = 901225,

    /// <summary>
    /// Password maximum of characters was met = 901226
    /// </summary>
    PasswordMaximumXCharsExceeded = 901226,

    #endregion

    #region storage errors

    /// <summary>
    /// Missing or invalid dicom field = 901400
    /// </summary>
    MissingInvalidDicomField = 901400,

    /// <summary>
    /// Failed to serialize the entity = 901402
    /// </summary>
    FailedSerialize = 901402,

    /// <summary>
    /// Failed to delete the provided directory = 901403
    /// </summary>
    DeleteDirectoryFailed = 901403,

    /// <summary>
    /// Failed to delete the provided file = 901404
    /// </summary>
    DeleteFileFailed = 901404,

    /// <summary>
    /// Series contains at lease one reference (cannot delete, etc.) = 901405
    /// </summary>
    SeriesContainsReference = 901405,

    /// <summary>
    /// Series contains at least one dependent = 901406
    /// </summary>
    SeriesContainsDependents = 901406,

    /// <summary>
    /// Series is processing = 901407
    /// </summary>
    SeriesProcessing = 901407,

    /// <summary>
    /// Series contains no dicom images = 901408
    /// </summary>
    SeriesContainsNoImages = 901408,

    /// <summary>
    /// No launchable data was found = 901430
    /// </summary>
    NoLaunchableDataFound = 901430,

    /// <summary>
    /// Cannot launch while data is processing = 901431
    /// </summary>
    CannotLaunchWhileProcessing = 901431,

    /// <summary>
    /// Cannot launch the specified type = 901432
    /// </summary>
    CannotLaunchSpecifiedType = 901432,

    /// <summary>
    /// Cannot launch the while series is processing: 901433
    /// </summary>
    CannotLaunchWhileProcessingWEntity = 901433,

    /// <summary>
    /// Cannot launch a failed series = 901434
    /// </summary>
    CannotLaunchFailedSeries = 901434,

    /// <summary>
    /// Cannot launch multiple series linked to the same dicom.
    /// </summary>
    CannotLaunchMultiSeriesLinkedToDicom = 901435,

    /// <summary>
    /// Series doesn't match the application = 901436
    /// </summary>
    SessionDoesNotMatchApplication = 901436,

    /// <summary>
    /// The series has not been processed for an application = 901437
    /// </summary>
    SeriesHasNotBeenProcessedForApp = 901437,

    /// <summary>
    /// Cannot specify x and y together = 901438
    /// </summary>
    CannotSpecifyXAndYTogether = 901438,

    /// <summary>
    /// Cannot specify more than x = 901439
    /// </summary>
    CannotSpecifyMoreThanX = 901439,

    #endregion

    #region system errors

    /// <summary>
    /// Missing an average dataset setting = 901800
    /// </summary>
    MissingAverageDatasetSetting = 901800,

    /// <summary>
    /// Invalid average dataset size = 901801
    /// </summary>
    InvalidAverageDatasetSize = 901801,

    /// <summary>
    /// No storage devices were specified = 901802
    /// </summary>
    NoStorageDevicesDefined = 901802,

    /// <summary>
    /// Storage device is missing the local setting = 901803
    /// </summary>
    MissingLocalSettingForStorageDevice = 901803,

    /// <summary>
    /// The device's storage space is full = 901804
    /// </summary>
    OutOfSpace = 901804,

    /// <summary>
    /// Failed to get the device's disk space = 901805
    /// </summary>
    FailedToGetDiskSpace = 901805,

    /// <summary>
    /// Invalid storage device = 903017
    /// </summary>
    InvalidStorageDevice = 903017,

    #endregion

  };  // enum ErrorCodes
} // namespace Viatronix
