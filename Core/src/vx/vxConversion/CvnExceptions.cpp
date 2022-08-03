// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "CvnExceptions.h"
#include "CvnLog.h"

using namespace cvn;

/**
 * Constructor
 * @param log informtaion
 */
CvnException::CvnException(const vxLogRecord & record) : 
m_record(record)
{
  CvnLog::Error(record);
}


/** 
 * Test method
 */
void CvnException::Test()
{
  try
  {
    throw cvn::CvnException(vxLogRecord( "Error message", "CvnException", "Test"));
  }
  catch(cvn::CvnException & ex)
  {
    throw cvn::CvnException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void CvnUnsupportedException::Test()
{
  try
  {
    throw cvn::CvnUnsupportedException(vxLogRecord("Error message", "CvnCUnsupportedException", "Test"));
  }
  catch(cvn::CvnUnsupportedException & ex)
  {
    throw cvn::CvnUnsupportedException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnCUnsupportedException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void CvnOutOfMemoryException::Test()
{
  try
  {
    throw cvn::CvnOutOfMemoryException(vxLogRecord("Error message", "CvnCOutOfMemoryException", "Test"));
  }
  catch(cvn::CvnOutOfMemoryException & ex)
  {
    throw cvn::CvnOutOfMemoryException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnCOutOfMemoryException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void CvnInvalidDataException::Test()
{
  try
  {
    throw cvn::CvnInvalidDataException(vxLogRecord("Error message", "CvnCInvalidDataException", "Test"));
  }
  catch(cvn::CvnInvalidDataException & ex)
  {
    throw cvn::CvnInvalidDataException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnCInvalidDataException", "Test"));
  }
} // Test()


/** 
 * Test method
 */
void CvnAbortException::Test()
{
  try
  {
    throw cvn::CvnAbortException(vxLogRecord("Error message", "CvnCAbortException", "Test"));
  }
  catch(cvn::CvnAbortException & ex)
  {
    throw cvn::CvnAbortException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnCAbortException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void CvnNoPixelDataException::Test()
{
  try
  {
    throw cvn::CvnNoPixelDataException(vxLogRecord("Error message", "CvnCNoPixelDataException", "Test"));
  }
  catch(cvn::CvnNoPixelDataException & ex)
  {
    throw cvn::CvnNoPixelDataException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnCNoPixelDataException", "Test"));
  }
} // Test()

/** 
 * Test method
 */
void CvnIOException::Test()
{
  try
  {
    throw cvn::CvnIOException(vxLogRecord("Error message", "CvnIOException", "Test"));
  }
  catch(cvn::CvnIOException & ex)
  {
    throw cvn::CvnIOException( vxLogRecord(const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage() + " was caught", "CvnIOException", "Test"));
  }
} // Test()
