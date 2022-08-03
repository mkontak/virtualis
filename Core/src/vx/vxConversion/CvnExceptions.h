// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "Logger.h"

namespace cvn
{


/** 
 * Implements a exception thats the base exception all others are built from
 */
class CVN_DLL CvnException 
{

public:

  /// constructor for log record
  CvnException(const vxLogRecord & record );

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

}; // CvnException

/** 
 * Implements a exception that identifies an association rejection condition
 */
class CVN_DLL CvnNoImagesFoundException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnNoImagesFoundException(const vxLogRecord & record) : CvnException(record) {}
  
  /// test
  static void Test();


}; // CvnNoImagesFoundException

/** 
 * Implements a exception that identifies an unsupported race condition
 */
class CVN_DLL CvnUnsupportedException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnUnsupportedException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnUnsupportedException

/** 
 * Implements a exception that identifies an unsupported race condition
 */
class CVN_DLL CvnInvalidDataException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnInvalidDataException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnInvalidDataException

/** 
 * Implements a exception that identifies an unsupported race condition
 */
class CVN_DLL CvnOutOfMemoryException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnOutOfMemoryException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnOutOfMemoryException


/** 
 * Implements a exception that identifies the process is being aborted
 */
class CVN_DLL CvnAbortException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnAbortException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnAbortException


/** 
 * Implements a exception that identifies there is no pixel data
 */
class CVN_DLL CvnNoPixelDataException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnNoPixelDataException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnNoPixelDataException

/** 
 * Implements a exception that identifies there was an IO exception
 */
class CVN_DLL CvnIOException : public CvnException
{
// functions
public:

  /// constructor for log record
  CvnIOException(const vxLogRecord & record) : CvnException(record) {}

  
  /// test
  static void Test();


}; // CvnIOException

};  // namespace cvn

