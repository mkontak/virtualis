// $Id: Exception.h,v 1.5 2006/08/28 17:31:26 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff Meade (mailto:jmeade@viatronix.com)


// pragmas
#pragma once


namespace ex
{
/**
 * Exceptions are the preferred method for error handling in all applications.  
 * Although exceptions are the preferred mechanism for error handling they should not be used for normal
 * control flow, such as detecting EOF.
 * Exceptions should be caught by reference and not by value i.e. (catch VException& e).
 * Empty catch() statements must be commented, as to why no action is needed to correct the situation.
 * All exceptions thrown must be declared in the function declaration, see the example use in Exception.C.
 * No new exceptions should be added to this file without reaching the consensus of several developers
 * that the current set is lacking in some way.
 * The current set of exceptions should be extended within specific libraries where necessary.  For example,
 * a developer working sockets may add exceptions such as ConnectException and BindException.
 * Finally, note the spelling of the word exception, it can be difficult to type correctly.  Consult this 
 * class for a reference.
 *
 * Added note:  Noooo, the word "exception" isn't hard to spell; no harder than anonymous :-)
 */

#include "Logger.h"


// class definition
class VX_BASE_DLL VException : std::exception
{
// functions
public:

  /// constructor for log record
  VException(const vxLogRecord & logRec);

  /// returns the log record object.
  const vxLogRecord & GetLogRecord() const { return m_logRecord; }
  
  /// test
  static void Test();

// data
protected:

  /// log record
  vxLogRecord m_logRecord;

}; // VException


// forward declarations
class RuntimeException;              // Runtime base class exception
class NullPointerException;          // Null pointer detected, following this would cause bad things
class IllegalArgumentException;      // Illegal argument passed.
class OutOfMemoryException;          // Out of memory
class IndexOutOfBoundsException;     // Array out of bounds
class InvalidCastException;          // Invalid cast
/// Basic IO related Exceptions (also derived from RuntimeException)
class IOException;                   // Generic IO error
class FileNotFoundException;         // Signals that an attempt to open a file has failed. 
class EOFException;                  // Signals that an end of file or end of stream has been reached unexpectedly during input.
class DataCorruptionException;       // Data is corrupt, internal consistency check has failed.
class InvalidDataException;          // Data is not valid
class UnsupportedDataTypeException;  // Data type is not supported
/// Program Execution Exceptions
class AbortOperationException;       // Base class for any operations that may be aborted while running
class AbortLoadException;            // Signals than an ongoing load was aborted
class AbortDataProcessingException;  // Abort during data processing
class ResourceImageException;        // Unable to load image from resource
class UnsupportedOperationException; // The operation is unsupported
class V3DRegistryNotFoundException;  // The registry is not available
class LoadFailedException;           // Failute occurred during load operation
/// additional base exceptions
class ErrorCodeException;            // base class for exceptions containing error codes
class HResultException;
/// Crypt Exceptions
class CryptoException;
class CryptOutOfMemoryException;
class WmiException;                  //
class SqlException;                  // Sql Exception
class SqlNotFoundException;          // Sql Not FOund Exception
class LicenseException;
class LicenseExpiredException;
class SecurityException;
class UserExpiredException;
class AuthenticationException;
class AccountException;
class AccountExpiredException;
class AccountDisabledException;
class AccountUnknownException;
class PasswordException;
class PasswordChangeException;
class SystemException;
class StreamClosedException;
class HttpException;
class HttpInvalidDataException;
class DemoException;
class DemoInvalidDataException;
class DemoInvalidException;
class DemoExpiredException;
class DemoActivationKeyExpiredException;



/// Network exceptions
class NetworkException;         
class ConnectionResetException;     // WSAECONNRESET
class NotInitializedException;      // WSANOTINITIALISED
class NetworkFailedException;       // WSAENETDOWN
class BufferException;              // WSAEFAULT
class NotConnectedException;        // ENOTCONN
class BlockingWasCanceledException; // WSAEINTR
class InProgressException;          // WSAEINPROGRESS:
class KeepAliveException;           // WSAENETRESET:
class NotSocketException;           // WSAENOTSOCK:
class UnsupportedOptionException;   // WSAEOPNOTSUPP:
class SocketShutdownException;      // WSAESHUTDOWN:
class SocketMarkedAsNonBlocking;    // WSAEWOULDBLOCK:
class MessageTooLargeException;     // WSAEMSGSIZE:
class SocketNotBoundException;      // WSAEINVAL:
class ConnectionAbortedException;   // WSAECONNABORTED:
class ConnectionTimedoutException;  // WSAETIMEDOUT:

// class definition
class RuntimeException : public VException
{
// functions
public:

  /// constructor
  RuntimeException(const vxLogRecord & logRec) : VException(logRec) {}
};


// class definition
class NullPointerException : public RuntimeException 
{
// functions
public:

  /// constructor
  NullPointerException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class IllegalArgumentException : public RuntimeException 
{
// functions
public:

  /// constructor
  IllegalArgumentException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class OutOfMemoryException : public RuntimeException, public std::bad_alloc
{
// functions
public:

  /// constructor
  OutOfMemoryException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class IndexOutOfBoundsException : public RuntimeException
{
// functions
public:

  /// constructor
  IndexOutOfBoundsException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class InvalidCastException : public RuntimeException
{
// functions
public:

  /// constructor
  InvalidCastException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class IOException : public RuntimeException
{
// functions
public:

  /// constructor
  IOException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class FileNotFoundException : public IOException 
{
// functions
public:

  /// constructor
  FileNotFoundException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class V3DRegistryNotFoundException : public IOException 
{
// functions
public:

  /// constructor
  V3DRegistryNotFoundException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class LoadFailedException : public RuntimeException
{
// functions
public:

  /// constructor
  LoadFailedException(const vxLogRecord & logRec) : RuntimeException(logRec) { }
};


// class definition
class EOFException : public IOException 
{
// functions
public:

  /// constructor
  EOFException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class DataCorruptionException : public IOException 
{
// functions
public:

  /// constructor
  DataCorruptionException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class InvalidDataException : public IOException 
{
// functions
public:

  /// constructor
  InvalidDataException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class UnsupportedDataTypeException : public IOException 
{
// functions
public:

  /// constructor
  UnsupportedDataTypeException(const vxLogRecord & logRec) : IOException(logRec) { }
};


// class definition
class AbortOperationException : public VException
{
// functions
public:

  /// constructor
  AbortOperationException(const vxLogRecord & logRec) : VException(logRec) { }
};


// class definition
class AbortLoadException : public AbortOperationException
{
// functions
public:

  /// constructor
  AbortLoadException(const vxLogRecord & logRec) : AbortOperationException(logRec) { }
};


// class definition
class AbortDataProcessingException : public AbortOperationException
{
// functions
public:

  /// constructor
  AbortDataProcessingException(const vxLogRecord & logRec) : AbortOperationException(logRec) { }
};


// class definition
class UnsupportedOperationException : public VException
{
// functions
public:

  /// constructor
  UnsupportedOperationException(const vxLogRecord & logRec) : VException(logRec) { }
};


// class definition
class ResourceImageException : public VException
{
// functions
public:

  /// constructor
  ResourceImageException(const vxLogRecord & logRec) : VException(logRec) { }
};

// class definition
class XmlException : public VException
{
// functions
public:

  /// constructor
  XmlException(const vxLogRecord & logRec) : VException(logRec) { }
};


// class definition
class ErrorCodeException : public VException
{
// functions
public:

  /// constructor
  ErrorCodeException(const vxLogRecord & logRec) : VException(logRec), m_uErrorCode( GetLastError() ) { }

  const DWORD GetErrorCode() const { return m_uErrorCode; }

protected:

  ErrorCodeException(const vxLogRecord & logRec, const DWORD uErrorCode ) : VException(logRec), m_uErrorCode( uErrorCode ) { }

private:

  DWORD m_uErrorCode;

};


// class definition
class HResultException : public VException
{
// functions
public:

  /// constructor
  HResultException(const vxLogRecord & logRec, const HRESULT hr ) : VException(logRec), m_hr( hr ) { }

  const HRESULT GetHResult() const { return m_hr; }

private:

  DWORD m_hr;

};


// class definition
class CryptException : public ErrorCodeException
{
// functions
public:

  /// constructor
  CryptException(const vxLogRecord & logRec) : ErrorCodeException(logRec) { }

protected:

  /// constructor
  CryptException(const vxLogRecord & logRec, const DWORD uErrorCode ) : ErrorCodeException(logRec, uErrorCode) { }
};


// class definition
class UnspecifiedCryptException : public CryptException
{
// functions
public:

  /// constructor
  UnspecifiedCryptException(const vxLogRecord & logRec) : CryptException( logRec, 0 ) { }
};


// class definition
class CryptOutOfMemoryException : public OutOfMemoryException
{
// functions
public:

  /// constructor
  CryptOutOfMemoryException(const vxLogRecord & logRec) : OutOfMemoryException(logRec) { }
};


// class definition
class WmiException : public VException
{
// functions
public:

  /// constructor
  WmiException(const vxLogRecord & logRec) : VException(logRec) {}
};


/**
 * Base SQL exception
 */
class SqlException : public VException
{
public:

  
  /// constructor
  SqlException(const vxLogRecord & logRec) : VException(logRec) {}

};  // class SqlException

/**
 * Not Found Exception
 */
class SqlNotFoundException : public SqlException
{
public:

 /// constructor
  SqlNotFoundException(const vxLogRecord & logRec) : SqlException(logRec) {}

};  // class SqlNotFoundException

/**
 * Security Exception
 */
class SecurityException : public VException
{
public:

 /// constructor
  SecurityException(const vxLogRecord & logRec) : VException(logRec) {}

};  // class SecurityException


/**
 * License Exception
 */
class LicenseException : public SecurityException
{
public:

 /// constructor
  LicenseException(const vxLogRecord & logRec) : SecurityException(logRec) {}

};  // class LicenseException


/**
 * Expired Exception
 */
class LicenseExpiredException : public LicenseException
{
public:

 /// constructor
  LicenseExpiredException(const vxLogRecord & logRec) : LicenseException(logRec) {}

};  // class LicenseExpiredException



/**
 * Login Exception
 */
class AuthenticationException : public SecurityException
{
public:

 /// constructor
  AuthenticationException(const vxLogRecord & logRec) : SecurityException(logRec) {}

};  // class AuthenticationException

/**
 * Account Exception
 */
class AccountException : public SecurityException
{
public:

 /// constructor
  AccountException(const vxLogRecord & logRec) : SecurityException(logRec) {}

};  // class AccountException

/**
 * Account Exception
 */
class AccountExpiredException : public AccountException
{
public:

 /// constructor
  AccountExpiredException(const vxLogRecord & logRec) : AccountException(logRec) {}

};  // class AccountExpiredException

/**
 * Account Unknown Exception
 */
class AccountUnknownException : public AccountException
{
public:

 /// constructor
  AccountUnknownException(const vxLogRecord & logRec) : AccountException(logRec) {}

};  // class AccountUnknownException


/**
 * Account Disabled Exception
 */
class AccountDisabledException : public AccountException
{
public:

 /// constructor
  AccountDisabledException(const vxLogRecord & logRec) : AccountException(logRec) {}

};  // class AccountDisabledException


/**
 * Password Exception
 */
class PasswordException : public SecurityException
{
public:

 /// constructor
  PasswordException(const vxLogRecord & logRec) : SecurityException(logRec) {}

};  // class PasswordException


/**
 * Passowrd change Exception
 */
class PasswordChangeException : public PasswordException
{
public:

 /// constructor
  PasswordChangeException(const vxLogRecord & logRec) : PasswordException(logRec) {}

};  // class PasswordChangeException

/**
 * System Exception
 */
class SystemException : public VException
{
public:

 /// constructor
  SystemException(const vxLogRecord & logRec) : VException(logRec) {}

};  // class SystemException


/**
 * Network exception
 */
class NetworkException : public VException
{
public:

 /// constructor
  NetworkException(const vxLogRecord & logRec) : VException(logRec) {}

};  // class NetworkException

/**
 * Connection reset by peer
 */
class ConnectionResetException : public NetworkException
{
public:

 /// constructor
  ConnectionResetException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class ConnectionResetException

/**
 * WSA not initialized
 */
class NotInitializedException : public NetworkException
{
public:

 /// constructor
  NotInitializedException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class NotInitializedException

/**
 * Network failed
 */
class NetworkFailedException : public NetworkException
{
public:

 /// constructor
  NetworkFailedException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class NetworkFailedException

/**
 * Not connected
 */
class NotConnectedException : public NetworkException
{
public:

 /// constructor
  NotConnectedException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class NotConnectedException
      
/**
 * Blocking cancelled
 */
class BlockingCanceledException : public NetworkException
{
public:

 /// constructor
  BlockingCanceledException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class BlockingCanceledException

/**
 * Invalid buffer
 */
class BufferException : public NetworkException
{
public:

 /// constructor
  BufferException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class BufferException

/**
 * Operaion is in progress
 */
class InProgressException : public NetworkException
{
public:

 /// constructor
  InProgressException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class InProgressException

/**
 * Keep alvie or time to live error
 */
class KeepAliveException : public NetworkException
{
public:

 /// constructor
  KeepAliveException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class KeepAliveException

/**
 * Not a socket
 */
class NotSocketException : public NetworkException
{
public:

 /// constructor
  NotSocketException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class NotSocketException

/**
 * Unsupported option
 */
class UnsupportedOptionException : public NetworkException
{
public:

 /// constructor
  UnsupportedOptionException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class UnsupportedOptionException

/**
 * Socket shutdown
 */
class SocketShutdownException : public NetworkException
{
public:

 /// constructor
  SocketShutdownException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class SocketShutdownException

/**
 * Socket marked as non-blocking and the receiver ask to block
 */
class SocketMarkedAsNonBlocking : public NetworkException
{
public:

 /// constructor
  SocketMarkedAsNonBlocking(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class SocketMarkedAsNonBlocking



/**
 * Message to large for buffer
 */
class MessageTooLargeException : public NetworkException
{
public:

 /// constructor
  MessageTooLargeException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class MessageTooLargeException


/**
 * Socket not bound
 */
class SocketNotBoundException : public NetworkException
{
public:

 /// constructor
  SocketNotBoundException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class SocketNotBoundException


/**
 * Connection aborted
 */
class ConnectionAbortedException : public NetworkException
{
public:

 /// constructor
  ConnectionAbortedException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class SocketNotBoundException



/**
 * Connection timed out
 */
class ConnectionTimeOutException : public NetworkException
{
public:

 /// constructor
  ConnectionTimeOutException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class ConnectionTimeOutException



/**
 * Connection closed
 */
class ConnectionClosedException : public NetworkException
{
public:

 /// constructor
  ConnectionClosedException(const vxLogRecord & logRec) : NetworkException(logRec) {}

};  // class ConnectionClosedException


/**
 * stream closed
 */
class StreamException : public vxLogIOException
{
public:

 /// constructor
  StreamException(const vxLogRecord & logRec) : vxLogIOException(logRec) {}

};  // class StreamClosedException

/**
 * stream closed
 */
class StreamClosedException : public StreamException
{
public:

 /// constructor
  StreamClosedException(const vxLogRecord & logRec) : StreamException(logRec) {}

};  // class StreamClosedException

/**
 * stream cannot write
 */
class StreamCannotWriteException : public StreamException
{
public:

 /// constructor
  StreamCannotWriteException(const vxLogRecord & logRec) : StreamException(logRec) {}

};  // class StreamCannotWriteException

/**
 * stream cannot read
 */
class StreamCannotReadException : public StreamException
{
public:

 /// constructor
  StreamCannotReadException(const vxLogRecord & logRec) : StreamException(logRec) {}

};  // class StreamCannotReadException


/**
 * Base HTTP Exception
 */
class  HttpException  : public IOException
{

public:

  /// constructor for log record
  HttpException(const vxLogRecord & record) : IOException(record) {}

}; // class HttpException


/**
 * Http invalid data exception
 */
class HttpInvalidDataException : public HttpException
{

public:

  /// constructor for log record
  HttpInvalidDataException(const vxLogRecord & record ) : HttpException(record)  {}

}; // class HttpInvalidDataException


#pragma region Demo Exceptions

/**
* Demo Exception
*/
class DemoException : public VException
{
public:

  /// constructor
  DemoException(const vxLogRecord & logRec) : VException(logRec) {}

};  // class LicenseExpiredException


/**
* Demo Exception
*/
class DemoRegistryException : public DemoException
{
public:

  /// constructor
  DemoRegistryException(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoRegistryException

/**
* Expired Exception
*/
class DemoExpiredException : public DemoException
{
public:

  /// constructor
  DemoExpiredException(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoExpiredException

/**
* Expired Exception
*/
class DemoActivationKeyExpiredException : public DemoException
{
public:

  /// constructor
  DemoActivationKeyExpiredException(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoActivationKeyExpiredException


/**
* Demo was already installed
*/
class DemoAlreadyInstalled : public DemoException
{
public:

  /// constructor
  DemoAlreadyInstalled(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoAlreadyInstalled


/**
* Demo was already installed
*/
class DemoInvalidException : public DemoException
{
public:

  /// constructor
  DemoInvalidException(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoInvalidException


/**
* Demo had invalid data
*/
class DemoInvalidDataException : public DemoException
{
public:

  /// constructor
  DemoInvalidDataException(const vxLogRecord & logRec) : DemoException(logRec) {}

};  // class DemoInvalidDataException

#pragma endregion


} // namespace ex


// $Log: Exception.h,v $
// Revision 1.5  2006/08/28 17:31:26  frank
// formatting
//
// Revision 1.4  2006/08/24 16:18:24  romy
// added  V3D registry not found exception
//
// Revision 1.3  2005/07/01 12:29:26  vxconfig
// Added new Logger.
//
// Revision 1.2  2005/04/12 15:20:18  geconomos
// added XmlException class
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/06/04 20:10:56  jmeade
// code review.
//
// Revision 3.4  2003/05/12 16:31:07  michael
// code review
//
// Revision 3.3  2002/07/02 20:44:12  geconomos
// Exporting classes .
//
// Revision 3.2  2002/06/05 21:26:52  geconomos
// Added InvalidCastException.
//
// Revision 3.1  2002/03/25 14:05:09  geconomos
// Added exception for resource image errors.
//
// Revision 3.0  2001/10/14 23:02:13  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:12   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:45:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:48   ingmar
// Initial revision.
// Revision 1.3  2001/09/03 01:39:52  ingmar
// code beautify
//
// Revision 1.2  2001/09/02 23:40:43  ingmar
// added 01_logger.h
//
// Revision 1.1  2001/09/02 01:15:18  soeren
// moved exceptions from 03_utils to 01_logger
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.18  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.17  2001/08/12 18:54:28  soeren
// cosmetics - Ingmar
//
// Revision 1.16  2001/07/24 20:22:23  dmitry
// Forced constructor with vxLogRecord. vxLogRecord handling added to base class.
//
// Revision 1.15  2001/07/20 17:50:38  lihong
// Defect ID: 599
// added UnsupportedDataTypeException()
//
// Revision 1.14  2001/07/19 22:04:38  lihong
// defect ID: 599
// Added InvalidDataException() and AbortDataProcessingException()
//
// Revision 1.13  2001/07/16 23:46:02  jmeade
// vxLogRecord implementation
//
// Revision 1.12  2001/07/12 21:13:15  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.11  2001/05/22 23:52:17  dave
// Initial version of ExceptionSetup class
//
// Revision 1.10  2001/05/19 00:37:35  jmeade
// Abort exceptions, const& to const for parameters with storage <= int4
//
// Revision 1.9  2001/05/09 20:09:30  soeren
// fixed some bugs
//
// Revision 1.8  2001/04/25 19:27:41  dave
// OutOfMem inherits from std::bad_alloc
//
// Revision 1.7  2001/04/18 14:36:48  dave
// changed ArrayIndex to simply Index
//
// Revision 1.6  2001/04/12 18:50:49  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.5  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.4  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.3  2001/04/10 21:31:50  dave
// updated some comments
//
// Revision 1.2  2001/04/09 22:23:32  dave
// added RuntimeException
//
// Revision 1.1  2001/04/08 21:40:18  dave
// added stream related files
//
// $Id: Exception.h,v 1.5 2006/08/28 17:31:26 frank Exp $
