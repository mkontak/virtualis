// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlExceptions.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogRecord.h"
#include "vxXmlExceptions.h"
#include "vxUtilStrings.h"


// using
using namespace xml;



/**
 * Constructor
 *
 * @param  Log record
 *
 */
XmlException::XmlException(const vxLogRecord & record) : 
m_record(record)
{
  LogErr( record.GetTextMessage(), record.GetClassName(), record.GetMethod() );
} //   XmlException(const vxLogRecord & record) 


/** 
 * Test method
 */
void XmlException::Test()
{
  try
  {
    throw XmlException(vxLogRecord("Error message", "XmlException", "Test"));
  }
  catch(XmlException & ex)
  {
    throw XmlException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "XmlException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void XmlNodeNotFoundException::Test()
{
  try
  {
    throw XmlNodeNotFoundException(vxLogRecord("Error message", "XmlNodeNotFoundException", "Test"));
  }
  catch(XmlNodeNotFoundException & ex)
  {
    throw XmlNodeNotFoundException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "XmlNodeNotFoundException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void XmlAttributeNotFoundException::Test()
{
  try
  {
    throw XmlAttributeNotFoundException(vxLogRecord("Error message", "XmlAttributeNotFoundException", "Test"));
  }
  catch(XmlAttributeNotFoundException & ex)
  {
    throw XmlAttributeNotFoundException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "XmlAttributeNotFoundException", "Test"));
  }
} // Test()


