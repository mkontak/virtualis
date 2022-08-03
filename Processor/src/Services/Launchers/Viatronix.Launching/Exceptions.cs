// Copyright© 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  Exceptions.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace Viatronix.Launching
{



  /// <summary>
  /// Implements a base exception.
  /// </summary>
  public class LaunchException : Exception
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public LaunchException(int errorCode = -1) { m_errorCode = errorCode; }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public LaunchException(string message, int errorCode = -1) : base(message) { m_errorCode = errorCode; }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public LaunchException(string message, Exception e, int errorCode = -1) : base(message, e) { m_errorCode = errorCode; }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public LaunchException(SerializationInfo info, StreamingContext context) : base(info, context) { m_errorCode = info.GetInt32("errorCode"); }


    /// <summary>
    /// Gets the object data for serialization
    /// </summary>
    /// <param name="s">Serializatio informatiom</param>
    /// <param name="c">Streaming context</param>
    public override void GetObjectData(SerializationInfo s, StreamingContext c)
    {
      base.GetObjectData(s, c);

      s.AddValue("errorCode", m_errorCode);

    } // GetObjectData()

    #region properties

    /// <summary>
    /// Gets the ErrorCode
    /// </summary>
    public int ErrorCode
    {
      get { return m_errorCode; }
    }

    #endregion

    #region field

    /// <summary>
    /// Error code (-1 is default);
    /// </summary>
    private int m_errorCode = -1;

    #endregion

  }; // class LaunchException


  /// <summary>
  /// Implements a Not Found exception.
  /// </summary>
  public class NotFoundException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public NotFoundException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NotFoundException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NotFoundException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NotFoundException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class NotFoundException



  /// <summary>
  /// Implements a AlreadyRunningException.
  /// </summary>
  public class AlreadyRunningException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public AlreadyRunningException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public AlreadyRunningException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public AlreadyRunningException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public AlreadyRunningException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class AlreadyRunningException

  /// <summary>
  /// Implements a InvalidDataException.
  /// </summary>
  public class InvalidDataException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public InvalidDataException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public InvalidDataException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public InvalidDataException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public InvalidDataException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class InvalidDataException


  /// <summary>
  /// Implements a NotInstallException.
  /// </summary>
  public class NotInstallException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public NotInstallException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NotInstallException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NotInstallException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NotInstallException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class NotInstallException

  /// <summary>
  /// Implements a NotLoggedInException.
  /// </summary>
  public class NotLoggedInException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public NotLoggedInException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NotLoggedInException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NotLoggedInException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NotLoggedInException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class NotLoggedInException


  /// <summary>
  /// Implements a NoActiveSessionsException.
  /// </summary>
  public class NoActiveSessionsException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public NoActiveSessionsException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public NoActiveSessionsException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public NoActiveSessionsException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public NoActiveSessionsException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class NoActiveSessionsException

  /// <summary>
  /// Implements a UnsupportedException.
  /// </summary>
  public class UnsupportedException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public UnsupportedException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public UnsupportedException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public UnsupportedException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public UnsupportedException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class UnsupportedException


  /// <summary>
  /// Implements a Not Found exception.
  /// </summary>
  public class SecurityException : LaunchException
  {

    /// <summary>
    /// Base constructor
    /// </summary>
    public SecurityException() { }

    /// <summary>
    /// Constructor with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public SecurityException(string message, int errorCode = -1) : base(message, errorCode) { }

    /// <summary>
    /// Constructor with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public SecurityException(string message, Exception e, int errorCode = -1) : base(message, e) { }

    /// <summary>
    /// De-serializes a cache not found exception
    /// </summary>
    /// <param name="info">Serialization information</param>
    /// <param name="context">Streaming context</param>
    public SecurityException(SerializationInfo info, StreamingContext context) : base(info, context) { }

  }; // class SecurityException



}
