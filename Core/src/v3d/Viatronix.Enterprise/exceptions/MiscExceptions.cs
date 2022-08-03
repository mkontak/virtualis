using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region InvalidConfigurationException class

  /// <summary>
  /// An error has been found in application config file.
  /// </summary>
  public class InvalidConfigurationException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public InvalidConfigurationException(string message) : base(message, ErrorCodes.None)
    {
    } // InvalidConfigurationException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidConfigurationException(string message, Exception innerException) : base(message, innerException, ErrorCodes.None)
    {
    } // InvalidConfigurationException( message, innerException )

    #endregion

  } // class InvalidConfigurationException

  #endregion

  #region InvalidDateException class

  /// <summary>
  /// Invalid date has been supplied.
  /// </summary>
  public class InvalidDateException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public InvalidDateException(string message) : base(message, ErrorCodes.None)
    {
    } // InvalidDateException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public InvalidDateException(string message, Exception innerException) : base(message, innerException, ErrorCodes.None)
    {
    } // InvalidDateException( message, innerException )

    #endregion

  } // class InvalidDateException

  #endregion

  #region LaunchException class

  /// <summary>
  /// An error has occurred in launch.
  /// </summary>
  public class LaunchException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public LaunchException(string message) : base(message, ErrorCodes.None)
    {
    } // LaunchException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public LaunchException(string message, Exception innerException) : base(message, innerException, ErrorCodes.None)
    {
    } // LaunchException( message, innerException )

    #endregion

  } // class LaunchException

  #endregion

  #region MissingAttributeException class

  /// <summary>
  /// An attribute was missing.
  /// </summary>
  public class MissingAttributeException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public MissingAttributeException(string message) : base(message, ErrorCodes.None)
    {
    } // MissingAttributeException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public MissingAttributeException(string message, Exception innerException) : base(message, innerException, ErrorCodes.None)
    {
    } // MissingAttributeException( message, innerException )

    #endregion

  } // class MissingAttributeException

  #endregion

  #region ServerAlreadyStartedException class

  /// <summary>
  /// The server has already started.
  /// </summary>
  public class ServerAlreadyStartedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public ServerAlreadyStartedException(string message) : base(message, ErrorCodes.None)
    {
    } // ServerAlreadyStartedException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public ServerAlreadyStartedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.None)
    {
    } // ServerAlreadyStartedException( message, innerException )

    #endregion

  } // class ServerAlreadyStartedException

  #endregion

} // namespace Viatronix.Enterprise
