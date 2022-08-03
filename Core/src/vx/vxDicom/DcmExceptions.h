// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmExceptions.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "Logger.h"

namespace dcm
{


/** 
 * Implements a exception thats the base exception all others are built from
 */
class DCM_DLL DcmException : std::exception
{

public:

  /// constructor for log record
  DcmException(const vxLogRecord & record, const std::string & sLogSource  = "default" );

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

}; // DcmException


/** 
 * Implements a exception that identifies an unsupported race condition
 */
class DCM_DLL DcmUnsupportedException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmUnsupportedException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}

  
  /// test
  static void Test();

}; // DcmUnsupportedException



/** 
 * Implements a exception that identifies an Connection race condition
 */
class DCM_DLL DcmConnectionException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmConnectionException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmUnsupportedException


/** 
 * Implements a exception that identifies an Association race condition
 */
class DCM_DLL DcmAssociationException : public DcmException
{

public:

  /// constructor for log record
  DcmAssociationException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmAssociationException


/** 
 * Implements a exception that identifies an association rejection condition
 */
class DCM_DLL DcmRejectionException : public DcmAssociationException
{
// functions
public:

  /// constructor for log record
  DcmRejectionException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmAssociationException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmRejectionException



/** 
 * Implements a exception that identifies an abort has occurred
 */
class DCM_DLL DcmAbortException : public DcmException
{

public:

  /// constructor for log record
  DcmAbortException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmAbortException


/** 
 * Implements a exception that identifies an configuration exception has occurred
 */
class DCM_DLL DcmConfigurationException : public DcmException
{

public:

  /// constructor for log record
  DcmConfigurationException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmConfigurationException


/** 
 * Implements a exception that identifies that a process is already running has occurred
 */
class DCM_DLL DcmAlreadyRunningException : public DcmException
{

public:

  /// constructor for log record
  DcmAlreadyRunningException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmAlreadyRunningException


/** 
 * Implements a exception that identifies that there was some IO error
 */
class DCM_DLL DcmIOException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmIOException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmIOException


/** 
 * Implements a exception that identifies that there was some IO error
 */
class DCM_DLL DcmInvalidDicomFile : public DcmIOException
{
// functions
public:

  /// constructor for log record
  DcmInvalidDicomFile(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmIOException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmInvalidDicomFile


/** 
 * Implements a exception that identifies that there was invalid data
 */
class DCM_DLL DcmInvalidDataException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmInvalidDataException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmInvalidDataException

/** 
 * Implements a exception that identifies that there was no pixel data
 */
class DCM_DLL DcmNoPixelDataException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmNoPixelDataException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmNoPixelDataException


/** 
 * Implements a exception that identifies that there was invalid data
 */
class DCM_DLL DcmMissingDataException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmMissingDataException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmMissingDataException

/** 
 * Implements a exception that identifies that there was a duplicate image
 */
class DCM_DLL DcmDuplicateImageException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmDuplicateImageException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmDuplicateImageException


/** 
 * Implements a exception that identifies that there was invalid data
 */
class DCM_DLL DcmNotImplemented : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmNotImplemented(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmNotImplemented


/** 
 * Implements a exception that identifies that we are out of memory
 */
class DCM_DLL DcmOutOfMemoryException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmOutOfMemoryException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmOutOfMemoryException


/** 
 * Implements a exception that identifies that we failed during a C-FIND operation
 */
class DCM_DLL DcmFindException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmFindException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmFindException


/** 
 * Implements a exception that identifies that we failed during a C-MOVE operation
 */
class DCM_DLL DcmMoveException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmMoveException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmMoveException


/** 
 * Implements a exception that identifies that an index was out of bounds
 */
class DCM_DLL DcmOutOfBoundsException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmOutOfBoundsException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmOutOfBoundsException


/** 
 * Implements a exception that identifies an item already exists
 */
class DCM_DLL DcmAlreadyExistsException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmAlreadyExistsException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();

}; // DcmAlreadyExistsException


/** 
 * Implements a exception that identifies an item was not found
 */
class DCM_DLL DcmNotFoundException : public DcmException
{
// functions
public:

  /// constructor for log record
  DcmNotFoundException(const vxLogRecord & record, const std::string & sLogSource  = "" ) : DcmException(record, sLogSource) {}
  
  /// test
  static void Test();


}; // DcmNotFoundException


};  // namespace dcm

