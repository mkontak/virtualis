using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region MissingAverageDatasetSettingException class

  /// <summary>
  /// Missing the average dataset setting.
  /// </summary>
  public class MissingAverageDatasetSettingException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public MissingAverageDatasetSettingException(string message) : base(message, ErrorCodes.MissingAverageDatasetSetting)
    {
    } // MissingAverageDatasetSettingException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public MissingAverageDatasetSettingException(string message, Exception innerException) : base(message, innerException, ErrorCodes.MissingAverageDatasetSetting)
    {
    } // MissingAverageDatasetSettingException( message, innerException )

    #endregion

  } // class MissingAverageDatasetSettingException

  #endregion

  #region InvalidAverageDatasetSizeException class

  /// <summary>
  /// Invalid average dataset size.
  /// </summary>
  public class InvalidAverageDatasetSizeException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public InvalidAverageDatasetSizeException(string message) : base(message, ErrorCodes.InvalidAverageDatasetSize)
    {
    } // InvalidAverageDatasetSizeException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidAverageDatasetSizeException(string message, Exception innerException) : base(message, innerException, ErrorCodes.InvalidAverageDatasetSize)
    {
    } // InvalidAverageDatasetSizeException( message, innerException )

    #endregion

  } // class InvalidAverageDatasetSizeException

  #endregion

  #region NoStorageDevicesDefinedException class

  /// <summary>
  /// @TODO Fill in
  /// </summary>
  public class NoStorageDevicesDefinedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public NoStorageDevicesDefinedException(string message) : base(message, ErrorCodes.NoStorageDevicesDefined)
    {
    } // NoStorageDevicesDefinedException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NoStorageDevicesDefinedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NoStorageDevicesDefined)
    {
    } // NoStorageDevicesDefinedException( message, innerException )

    #endregion

  } // class ...

  #endregion

  #region MissingLocalSettingForStorageDeviceException class

  /// <summary>
  /// Missing the local setting for storage device.
  /// </summary>
  public class MissingLocalSettingForStorageDeviceException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public MissingLocalSettingForStorageDeviceException(string message) : base(message, ErrorCodes.MissingLocalSettingForStorageDevice)
    {
    } // MissingLocalSettingForStorageDeviceException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public MissingLocalSettingForStorageDeviceException(string message, Exception innerException) : base(message, innerException, ErrorCodes.MissingLocalSettingForStorageDevice)
    {
    } // MissingLocalSettingForStorageDeviceException( message, innerException )

    #endregion

  } // class MissingLocalSettingForStorageDeviceException

  #endregion

  #region OutOfSpaceException class

  /// <summary>
  /// The storage device is out of space.
  /// </summary>
  public class OutOfSpaceException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public OutOfSpaceException(string message) : base(message, ErrorCodes.OutOfSpace)
    {
    } // OutOfSpaceException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public OutOfSpaceException(string message, Exception innerException) : base(message, innerException, ErrorCodes.OutOfSpace)
    {
    } // OutOfSpaceException( message, innerException )

    #endregion

  } // class OutOfSpaceException

  #endregion

  #region FailedToGetDiskSpaceException class

  /// <summary>
  /// @TODO Fill in
  /// </summary>
  public class FailedToGetDiskSpaceException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public FailedToGetDiskSpaceException(string message) : base(message, ErrorCodes.FailedToGetDiskSpace)
    {
    } // FailedToGetDiskSpaceException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public FailedToGetDiskSpaceException(string message, Exception innerException) : base(message, innerException, ErrorCodes.FailedToGetDiskSpace)
    {
    } // FailedToGetDiskSpaceException( message, innerException )

    #endregion

  } // class FailedToGetDiskSpaceException

  #endregion

  #region InvalidStorageDeviceException class

  /// <summary>
  /// @TODO Fill in
  /// </summary>
  public class InvalidStorageDeviceException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public InvalidStorageDeviceException(string message) : base(message, ErrorCodes.InvalidStorageDevice)
    {
    } // InvalidStorageDeviceException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidStorageDeviceException(string message, Exception innerException) : base(message, innerException, ErrorCodes.InvalidStorageDevice)
    {
    } // InvalidStorageDeviceException( message, innerException )

    #endregion

  } // class InvalidStorageDeviceException

  #endregion
  
} // namespace Viatronix.Enterprise
