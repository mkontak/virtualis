// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfExceptions.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "Logger.h"

namespace pdf
{


/** 
 * Implements a exception thats the base exception all others are built from
 */
class PdfException 
{

public:

  /// constructor for log record
  PdfException(const vxLogRecord & record );

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

}; // PdfException



/** 
 * Implements a exception that identifies an association rejection condition
 */
class PdfInvalidKeyException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfInvalidKeyException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfInvalidKeyException



/** 
 * Implements a exception that identifies an unsupported race condition
 */
class PdfUnsupportedException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfUnsupportedException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfUnsupportedException



/** 
 * Implements a exception that identifies an abort has occurred
 */
class PdfAbortException : public PdfException
{

public:

  /// constructor for log record
  PdfAbortException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfAbortException



/** 
 * Implements a exception that identifies an configuration exception has occurred
 */
class PdfConfigurationException : public PdfException
{

public:

  /// constructor for log record
  PdfConfigurationException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfConfigurationException



/** 
 * Implements a exception that identifies that there was some IO error
 */
class PdfIOException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfIOException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();


}; // PdfIOException



/** 
 * Implements a exception that identifies that there was invalid data
 */
class PdfInvalidDataException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfInvalidDataException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfInvalidDataException



/** 
 * Implements a exception that identifies that there was invalid data
 */
class PdfNotImplemented : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfNotImplemented(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfNotImplemented


/** 
 * Implements a exception that identifies that we are out of memory
 */
class PdfOutOfMemoryException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfOutOfMemoryException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfOutOfMemoryException


/** 
 * Implements a exception that identifies that an index was out of bounds
 */
class PdfOutOfBoundsException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfOutOfBoundsException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfOutOfBoundsException


/** 
 * Implements a exception that identifies an item was not found
 */
class PdfNotFoundException : public PdfException
{
// functions
public:

  /// constructor for log record
  PdfNotFoundException(const vxLogRecord & record  ) : PdfException(record) {}
  
  /// test
  static void Test();

}; // PdfNotFoundException


};  // namespace Pdf

