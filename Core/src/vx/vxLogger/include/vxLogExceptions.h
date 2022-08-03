// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogExceptions.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


#pragma region Logger Exceptions

/** 
 * Implements a exception thats the base exception all others are built from
 */
class VX_LOGGER_DLL vxLogException : std::exception
{

public:

  /// constructor for log record
  vxLogException(const vxLogRecord & record, bool bLogMessage = false );

  /// Gets the log record
  virtual const vxLogRecord & GetLogRecord() const
  { return m_record; }
  
  /// Gets the message for the exception
  virtual const std::string & GetTextMessage() 
  { return m_record.GetTextMessage(); }

  /// Test
  static void Test();


private:

  /// log record
  vxLogRecord m_record;

}; // vxLogException


/** 
 * Implements a exception that identifies that there was some IO error
 */
class VX_LOGGER_DLL vxLogIOException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogIOException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogIOException

/** 
 * Implements a exception that identifies that there was some IO error
 */
class VX_LOGGER_DLL vxLogRegistrationException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogRegistrationException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogRegistrationException

/** 
 * Implements a exception that identifies that there was invalid data
 */
class VX_LOGGER_DLL vxLogInvalidDataException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogInvalidDataException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogInvalidDataException




/** 
 * Implements a exception that identifies that we are out of memory
 */
class VX_LOGGER_DLL vxLogOutOfMemoryException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogOutOfMemoryException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogOutOfMemoryException


/** 
 * Implements a exception that identifies an item already exists
 */
class VX_LOGGER_DLL vxLogAlreadyExistsException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogAlreadyExistsException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogAlreadyExistsException

/** 
 * Implements a exception that identifies an item already exists
 */
class VX_LOGGER_DLL vxLogAbortException : public vxLogException
{
// functions
public:

  /// constructor for log record
  vxLogAbortException(const vxLogRecord & record, bool logMessage = true ) : vxLogException(record, logMessage) {}
  
  /// test
  static void Test();


}; // vxLogAlreadyExistsException

#pragma endregion



#pragma warning(pop)



