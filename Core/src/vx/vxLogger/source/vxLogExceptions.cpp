// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogExceptions.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// includes
#include "Stdafx.h"
#include "vxLogExceptions.h"
#include "vxLogUtility.h"

/**
 * Constructor
 *
 * @param log informtaion
 */
vxLogException::vxLogException(const vxLogRecord & record, bool logMessage ) : 
m_record(record),
std::exception(record.GetTextMessage().c_str())
{

  
  if ( logMessage )
    LogErr( record.GetTextMessage(), record.GetClassName(), record.GetMethod() );
} //   vxLogException(const vxLogRecord & record) 


/** 
 * Test method
 */
void vxLogException::Test()
{
  try
  {
    throw vxLogException( vxLogRecord( "Error message", "vxLogException", "Test") );
  }
  catch(vxLogException & ex)
  {
    throw vxLogException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "vxLogException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void vxLogIOException::Test()
{
  try
  {
    throw vxLogIOException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogIOException & ex)
  {
    throw vxLogIOException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage()), "vxLogIOException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void vxLogRegistrationException::Test()
{
  try
  {
    throw vxLogRegistrationException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogRegistrationException & ex)
  {
    throw vxLogRegistrationException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "vxLogRegistrationException", "Test"));
  }
} // Test()
/** 
 * Test method
 */
void vxLogInvalidDataException::Test()
{
  try
  {
    throw vxLogInvalidDataException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogInvalidDataException & ex)
  {
    throw vxLogInvalidDataException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "vxLogInvalidDataException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void vxLogOutOfMemoryException::Test()
{
  try
  {
    throw vxLogOutOfMemoryException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogOutOfMemoryException & ex)
  {
    throw vxLogOutOfMemoryException( LogRec(util::Strings::Format(std::string(""), "%s was caught", ex.GetTextMessage().c_str()), "vxLogOutOfMemoryException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void vxLogAlreadyExistsException::Test()
{
  try
  {
    throw vxLogAlreadyExistsException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogAlreadyExistsException & ex)
  {
    throw vxLogAlreadyExistsException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "vxLogAlreadyExistsException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void vxLogAbortException::Test()
{
  try
  {
    throw vxLogAbortException(LogRec( "Error message", "vxLogException", "Test"));
  }
  catch(vxLogAbortException & ex)
  {
    throw vxLogAbortException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "vxLogAbortException", "Test"));
  }
} // Test()


