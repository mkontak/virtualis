// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfExceptions.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "PdfExceptions.h"



using namespace pdf;

/**
 * Constructor
 * @param log informtaion
 */
PdfException::PdfException(const vxLogRecord & record) : 
m_record(record)
{
  LogErr(record.GetTextMessage(), record.GetClassName(), record.GetMethod() );
} //   PdfException(const vxLogRecord & record) 


/** 
 * Test method
 */
void PdfException::Test()
{
  try
  {
    throw PdfException(vxLogRecord( "Error message", "PdfException", "Test"));
  }
  catch(PdfException & ex)
  {
    throw PdfException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfInvalidKeyException::Test()
{
  try
  {
    throw PdfInvalidKeyException(vxLogRecord("Error message", "PdfInvalidKeyException", "Test"));
  }
  catch(PdfException & ex)
  {
    throw PdfInvalidKeyException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfInvalidKeyException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfUnsupportedException::Test()
{
  try
  {
    throw PdfUnsupportedException(vxLogRecord("Error message", "PdfUnsupportedException", "Test"));
  }
  catch(PdfUnsupportedException & ex)
  {
    throw PdfUnsupportedException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfUnsupportedException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfConfigurationException::Test()
{
  try
  {
    throw PdfConfigurationException(vxLogRecord("Error message", "PdfConfigurationException", "Test"));
  }
  catch(PdfConfigurationException & ex)
  {
    throw PdfConfigurationException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "PdfConfigurationException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfIOException::Test()
{
  try
  {
    throw PdfIOException(vxLogRecord( "Error message", "PdfIOException", "Test"));
  }
  catch(PdfIOException & ex)
  {
    throw PdfIOException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfIOException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfInvalidDataException::Test()
{
  try
  {
    throw PdfInvalidDataException(vxLogRecord( "Error message", "PdfInvalidDataException", "Test"));
  }
  catch(PdfInvalidDataException & ex)
  {
    throw PdfInvalidDataException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfInvalidDataException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfNotImplemented::Test()
{
  try
  {
    throw PdfNotImplemented(vxLogRecord( "Error message", "PdfNotImplemented", "Test"));
  }
  catch(PdfNotImplemented & ex)
  {
    throw PdfNotImplemented( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfNotImplemented", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfOutOfMemoryException::Test()
{
  try
  {
    throw PdfOutOfMemoryException(vxLogRecord( "Error message", "PdfOutOfMemoryException", "Test"));
  }
  catch(PdfOutOfMemoryException & ex)
  {
    throw PdfOutOfMemoryException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfOutOfMemoryException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void PdfNotFoundException::Test()
{
  try
  {
    throw PdfNotFoundException(vxLogRecord( "Error message", "PdfNotFoundException", "Test"));
  }
  catch(PdfNotFoundException & ex)
  {
    throw PdfNotFoundException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "PdfNotFoundException", "Test"));
  }
} // Test()

