// $Id: Exceptions.cs,v 1.2 2007/03/06 19:13:58 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Runtime;
using System.Runtime.Remoting;
using System.Runtime.Serialization;

namespace Viatronix.Security

{
  #region SecurityException

  /// <summary>
  /// Implements a base Cach Exception to be returned bay the UMAS whenever an exception is detected. All
  /// other UMAS should use this as it's base.
  /// </summary>
  [Serializable]
  public class SecurityException : ApplicationException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public SecurityException() : base() { }

    /// <summary>
    /// Constructor with a messag only
    /// </summary>
    /// <param name="message">Error message</param>
    public SecurityException(string message) : base(message) { }

    /// <summary>
    /// Constructor with a message and an inner exception
    /// </summary>
    /// <param name="message">Error message</param>
    /// <param name="ex">Inner exception</param>
    public SecurityException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes the cache exception
    /// </summary>
    /// <param name="info">Serialization information object</param>
    /// <param name="context">Streaming context</param>
    public SecurityException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion  // constructors


  } // SecurityException

  #endregion

  #region NoAuthenticationModuleFound

  /// <summary>
  /// Implements a no authentication module found exception
  /// </summary>
  [Serializable]
  public class NoAuthenticationModulesFound : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public NoAuthenticationModulesFound() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NoAuthenticationModulesFound(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NoAuthenticationModulesFound(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NoAuthenticationModulesFound(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // NoAuthenticationModulesFound

  #endregion

  #region InvalidAuthenticationType

  /// <summary>
  /// Implements a invalid autentication type exception
  /// </summary>
  [Serializable]
  public class InvalidAuthenticationType : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public InvalidAuthenticationType() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public InvalidAuthenticationType(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public InvalidAuthenticationType(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public InvalidAuthenticationType(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // class InvalidAuthenticationType

  #endregion

  #region InvalidUserException

  /// <summary>
  /// Implements an invalid user exception
  /// </summary>
  [Serializable]
  public class InvalidUserException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public InvalidUserException() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public InvalidUserException(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public InvalidUserException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public InvalidUserException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // class InvalidUserException

  #endregion

  #region UserNotFoundException

  /// <summary>
  /// Implements a user not found exception
  /// </summary>
  [Serializable]
  public class UserNotFoundException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public UserNotFoundException() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public UserNotFoundException(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public UserNotFoundException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public UserNotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // class UserNotFoundException

  #endregion

  #region InvalidRoleException

  /// <summary>
  /// Implements an invalid role exception
  /// </summary>
  [Serializable]
  public class InvalidRoleException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public InvalidRoleException() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public InvalidRoleException(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public InvalidRoleException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public InvalidRoleException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // class InvalidRoleException

  #endregion

  #region RoleNotFoundException

  /// <summary>
  /// Implements role not found exception
  /// </summary>
  [Serializable]
  public class RoleNotFoundException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public RoleNotFoundException() : base() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public RoleNotFoundException(string message) : base(message) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public RoleNotFoundException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public RoleNotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // RoleNotFoundException

  #endregion

  #region InvalidPasswordException

  /// <summary>
  /// Implements an invalid passwrd exception
  /// </summary>
  [Serializable]
  public class InvalidPasswordException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Deserializes the cache locked exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public InvalidPasswordException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    /// <summary>
    /// Base Constructor
    /// </summary>
    public InvalidPasswordException() : base() { }

    /// <summary>
    /// Constructor with message
    /// </summary>
    /// <param name="message">Error message</param>
    public InvalidPasswordException(string message) : base(message) { }

    /// <summary>
    /// Constructor with message and inner exception
    /// </summary>
    /// <param name="message">Error message</param>
    /// <param name="ex">Inner exception</param>
    public InvalidPasswordException(string message, Exception ex) : base(message, ex) { }

    #endregion

  } // InvalidPasswordException

  #endregion

  #region DuplicateSessionException
  /// <summary>
  /// Implements a duplicate session exception .
  /// </summary>
  [Serializable]
  public class DuplicateLoginSessionException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public DuplicateLoginSessionException() : base() { }

    /// <summary>
    /// Duplicate login session exception with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public DuplicateLoginSessionException(string message) : base(message) { }

    /// <summary>
    /// Duplicate login session exception with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public DuplicateLoginSessionException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes the cache locked exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public DuplicateLoginSessionException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // DuplicateLoginSessionException

  #endregion DuplicateLoginSessionException

  #region AccountLockedException

  /// <summary>
  /// Implements a lock exception.
  /// </summary>
  [Serializable]

  public class AccountLockedException : SecurityException
  {

    #region constructors


    /// <summary>
    /// Constructs a Cache Locked Exception
    /// </summary>
    public AccountLockedException() : base() { }

    /// <summary>
    ///  Deserializes the account locked exception
    /// </summary>
    /// <param name="message">Error message</param>
    public AccountLockedException(string message) : base(message) { }

    /// <summary>
    ///  Deserializes the account locked exception
    /// </summary>
    /// <param name="message">Error message</param>
    /// <param name="ex">Inner exception</param>
    public AccountLockedException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes the cache locked exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public AccountLockedException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // AccountLockedException

  #endregion

  #region SystemDisabledException

  /// <summary>
  /// Implements a system disabled exception.
  /// </summary>
  public class SystemDisabledException : SecurityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public SystemDisabledException()
      : base()
    {
    } // SystemDisabledException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to indicate the issue.</param>
    public SystemDisabledException(string message)
      : base(message)
    {
    } // SystemDisabledException( message )

    #endregion

  } // class SystemDisabledException

  #endregion

  #region NoLicenseException
  /// <summary>
  /// Implements a duplicate session exception .
  /// </summary>
  [Serializable]
  public class NoLicenseException : SecurityException
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public NoLicenseException() : base() { }

    /// <summary>
    /// Duplicate login session exception with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NoLicenseException(string message) : base(message) { }

    /// <summary>
    /// Duplicate login session exception with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NoLicenseException(string message, Exception ex) : base(message, ex) { }

    /// <summary>
    /// Deserializes the cache locked exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NoLicenseException(SerializationInfo info, StreamingContext context) : base(info, context) { }

    #endregion

  } // NoLicenseException

  #endregion NoLicenseException

} // namespace Viatronix.v3D.Management

#region revision history

// $Log: Exceptions.cs,v $
// Revision 1.2  2007/03/06 19:13:58  mkontak
// Coding standards
//
// Revision 1.1  2006/12/05 21:01:49  romy
// Console Dlls Consolidation
//
// Revision 1.6  2006/02/17 18:26:17  romy
// exception for account lock added
//
// Revision 1.5  2005/09/22 15:36:32  romy
// added DuplicateLoginSession exception.
//
// Revision 1.4  2005/05/27 12:51:26  mkontak
// no message
//
// Revision 1.3  2005/05/26 20:05:14  mkontak
// UMAS changes
//
// Revision 1.2  2005/05/26 18:01:37  mkontak
// Changes for UMAS
//
// Revision 1.1  2005/05/25 13:12:07  mkontak
// Changes to UMAS
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/UMAS/Management/Exceptions.cs,v 1.2 2007/03/06 19:13:58 mkontak Exp $
// $Id: Exceptions.cs,v 1.2 2007/03/06 19:13:58 mkontak Exp $

#endregion

