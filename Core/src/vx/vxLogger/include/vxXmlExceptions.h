// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlExceptions.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



#pragma once

// pragma 
#pragma warning(push)
#pragma warning(disable:4251)


// forward declarations
class VX_LOGGER_DLL vxLogRecord;


// namespace
namespace xml
{
 
  
/** 
 * Implements a exception that's the base exception all xml exceptions
 */
class VX_LOGGER_DLL XmlException
{

public:

  /// constructor for log record
  XmlException(const vxLogRecord & record );
  
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

}; // XmlException

/** 
 * Implements a exception that identifies that a node was not found
 */
class VX_LOGGER_DLL XmlNodeNotFoundException : public XmlException
{
// functions
public:

  /// constructor for log record
  XmlNodeNotFoundException(const vxLogRecord & record) : XmlException(record) {}
  
  /// test
  static void Test();


}; // XmlNodeNotFoundException

/** 
 * Implements a exception that identifies that a attribute was not found
 */
class VX_LOGGER_DLL XmlAttributeNotFoundException : public XmlException
{
// functions
public:

  /// constructor for log record
  XmlAttributeNotFoundException(const vxLogRecord & record) : XmlException(record) {}
  
  /// test
  static void Test();


}; // XmlAttributeNotFoundException



} // namespace util

#pragma warning(pop)