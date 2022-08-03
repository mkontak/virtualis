// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmExceptions.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmUtility.h"


using namespace dcm;

/**
 * Constructor
 *
 * @param record
 * @param sLogSource
 */
DcmException::DcmException(const vxLogRecord & record, const std::string & sLogSource) : 
m_record(record)
{
  DcmLog::Error(sLogSource, record);
} //   DcmException(const vxLogRecord & record, const std::string & sLogSource) 


/** 
 * Test method
 */
void DcmException::Test()
{
  try
  {
    throw dcm::DcmException(vxLogRecord( "Error message", "DcmException", "Test"));
  }
  catch(dcm::DcmException & ex)
  {
    throw dcm::DcmException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void DcmMissingDataException::Test()
{
  try
  {
    throw dcm::DcmMissingDataException(vxLogRecord("Error message", "DcmMissingDataException", "Test"));
  }
  catch(dcm::DcmException & ex)
  {
    throw dcm::DcmMissingDataException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmMissingDataException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmNoPixelDataException::Test()
{
  try
  {
    throw dcm::DcmNoPixelDataException(vxLogRecord("Error message", "DcmNoPixelDataException", "Test"));
  }
  catch(dcm::DcmException & ex)
  {
    throw dcm::DcmNoPixelDataException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmNoPixelDataException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmConnectionException::Test()
{
  try
  {
    throw dcm::DcmConnectionException(vxLogRecord("Error message", "DcmConnectionException", "Test"));
  }
  catch(dcm::DcmException & ex)
  {
    throw dcm::DcmConnectionException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmConnectionException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmRejectionException::Test()
{
  try
  {
    throw dcm::DcmRejectionException(vxLogRecord("Error message", "DcmRejectionException", "Test"));
  }
  catch(dcm::DcmRejectionException & ex)
  {
    throw dcm::DcmRejectionException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "DcmRejectionException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmUnsupportedException::Test()
{
  try
  {
    throw dcm::DcmUnsupportedException(vxLogRecord("Error message", "DcmUnsupportedException", "Test"));
  }
  catch(dcm::DcmUnsupportedException & ex)
  {
    throw dcm::DcmUnsupportedException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmUnsupportedException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmAssociationException::Test()
{
  try
  {
    throw dcm::DcmAssociationException(vxLogRecord( "Error message", "DcmAssociationException", "Test"));
  }
  catch(dcm::DcmAssociationException & ex)
  {
    throw dcm::DcmAssociationException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmAssociationException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmAlreadyRunningException::Test()
{
  try
  {
    throw dcm::DcmAlreadyRunningException(vxLogRecord("Error message", "DcmAlreadyRunningException", "Test"));
  }
  catch(dcm::DcmAssociationException & ex)
  {
    throw dcm::DcmAlreadyRunningException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "DcmAlreadyRunningException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmConfigurationException::Test()
{
  try
  {
    throw dcm::DcmConfigurationException(vxLogRecord("Error message", "DcmConfigurationException", "Test"));
  }
  catch(dcm::DcmConfigurationException & ex)
  {
    throw dcm::DcmConfigurationException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "DcmConfigurationException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void DcmIOException::Test()
{
  try
  {
    throw dcm::DcmIOException(vxLogRecord( "Error message", "DcmIOException", "Test"));
  }
  catch(dcm::DcmIOException & ex)
  {
    throw dcm::DcmIOException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmIOException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmInvalidDataException::Test()
{
  try
  {
    throw dcm::DcmInvalidDataException(vxLogRecord( "Error message", "DcmInvalidDataException", "Test"));
  }
  catch(dcm::DcmInvalidDataException & ex)
  {
    throw dcm::DcmInvalidDataException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmInvalidDataException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmNotImplemented::Test()
{
  try
  {
    throw dcm::DcmNotImplemented(vxLogRecord( "Error message", "DcmNotImplemented", "Test"));
  }
  catch(dcm::DcmNotImplemented & ex)
  {
    throw dcm::DcmNotImplemented( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmNotImplemented", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmOutOfMemoryException::Test()
{
  try
  {
    throw dcm::DcmOutOfMemoryException(vxLogRecord( "Error message", "DcmOutOfMemoryException", "Test"));
  }
  catch(dcm::DcmOutOfMemoryException & ex)
  {
    throw dcm::DcmOutOfMemoryException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmOutOfMemoryException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmFindException::Test()
{
  try
  {
    throw dcm::DcmFindException(vxLogRecord( "Error message", "DcmFindException", "Test"));
  }
  catch(dcm::DcmMoveException & ex)
  {
    throw dcm::DcmFindException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmFindException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmMoveException::Test()
{
  try
  {
    throw dcm::DcmMoveException(vxLogRecord( "Error message", "DcmMoveException", "Test"));
  }
  catch(dcm::DcmMoveException & ex)
  {
    throw dcm::DcmMoveException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmMoveException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmOutOfBoundsException::Test()
{
  try
  {
    throw dcm::DcmOutOfBoundsException(vxLogRecord( "Error message", "DcmOutOfBoundsException", "Test"));
  }
  catch(dcm::DcmOutOfBoundsException & ex)
  {
    throw dcm::DcmOutOfBoundsException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmOutOfBoundsException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmAlreadyExistsException::Test()
{
  try
  {
    throw dcm::DcmAlreadyExistsException(vxLogRecord( "Error message", "DcmAlreadyExistsException", "Test"));
  }
  catch(dcm::DcmAlreadyExistsException & ex)
  {
    throw dcm::DcmAlreadyExistsException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmAlreadyExistsException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void DcmNotFoundException::Test()
{
  try
  {
    throw dcm::DcmNotFoundException(vxLogRecord( "Error message", "DcmNotFoundException", "Test"));
  }
  catch(dcm::DcmNotFoundException & ex)
  {
    throw dcm::DcmNotFoundException( LogRec(util::Strings::Format( "%s was caught", ex.GetTextMessage().c_str()), "DcmNotFoundException", "Test"));
  }
} // Test()

