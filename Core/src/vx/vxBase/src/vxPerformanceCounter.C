// $Id: vxPerformanceCounter.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailtot:george@viatronix.com)

#include "StdAfx.h"
#include "vxPerformanceCounter.h"

#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor from category and counter name
 *
 * @param   sCategoryName   category name
 * @param   sCounterName    counter name
 */
vxPerformanceCounter::vxPerformanceCounter( const std::string & sCategoryName, const std::string & sCounterName ) : 
 m_hPerformanceQuery( NULL ),
 m_hPerformanceCounter( NULL ),
 m_sCategoryName( sCategoryName ),
 m_sCounterName( sCounterName )
{
  OpenCounter();
} // vxPerformanceCounter( sCategoryName, sCounterName ) : 


/**
 * Constructor from category, counter and instance name
 *
 * @param   sCategoryName   category name
 * @param   sCounterName    counter name
 * @param   sInsatnceName    instance name
 */
vxPerformanceCounter::vxPerformanceCounter( const std::string & sCategoryName, const std::string & sCounterName,  
                                        const std::string & sInstanceName ) : 
 m_hPerformanceQuery( NULL ),
 m_hPerformanceCounter( NULL ),
 m_sCategoryName( sCategoryName ),
 m_sCounterName( sCounterName ),
 m_sInstanceName( sInstanceName )
{
  OpenCounter();
} // vxPerformanceCounter( sCategoryName, sCounterName, sInstanceName ) : 


/**
 * Destructor
 */
vxPerformanceCounter::~vxPerformanceCounter()
{
  if( m_hPerformanceCounter != NULL )
    PdhRemoveCounter( m_hPerformanceCounter );

  if( m_hPerformanceQuery != NULL )
    PdhCloseQuery( m_hPerformanceQuery ); 

} // ~vxPerformanceCounter()


/**
 * Creates a query object and adds the requested counter
 */
void vxPerformanceCounter::OpenCounter()
{
  // create a query structure to maintain the collection of performance data
  PDH_STATUS status = PdhOpenQuery( NULL, 0, &m_hPerformanceQuery );
  if( status != ERROR_SUCCESS )
    throw ex::VException( LogRec( "Unable to create peformance query object. error code: " + ToAscii( status ), "vxPerformanceCounter", "Create" ) );

  // format the counter path( \category_name(instance_name)\counter_name )
  std::stringstream ss;
  if( m_sInstanceName.empty() )
    ss << "\\" << m_sCategoryName << "\\" << m_sCounterName;
  else
    ss << "\\" << m_sCategoryName << "(" << m_sInstanceName << ")\\" << m_sCounterName;
  
  // add the specified performance counter
  if( ( status = PdhAddCounter( m_hPerformanceQuery, ss.str().c_str(), 0, &m_hPerformanceCounter ) ) != ERROR_SUCCESS )
  {
    // format and throw exception
    std::strstream sMsg;
    sMsg << "Unable to create peformance counter ( " << ss.str() << " ) Error Code: " << status;
    throw ex::VException( LogRec( sMsg.str(), "vxPerformanceCounter", "Create" ) );
  }
} // OpenCounter()


/**
 * Returns the current value of the counter in the specified format 
 *
 * @param     dwFormat    value format
 * @return    value
 */
PDH_FMT_COUNTERVALUE vxPerformanceCounter::GetCurrentValue( DWORD dwFormat )
{
  // collect the current raw data value for thel counter
  PDH_STATUS status = PdhCollectQueryData( m_hPerformanceQuery );
  if( status != ERROR_SUCCESS )
    throw ex::VException( LogRec( "Error collecting peformance data. error code: " + ToAscii( status ), "vxPerformanceCounter", "GetCurrentValue" ) );

  // get the current value of the counter in the specified format 
  PDH_FMT_COUNTERVALUE value;
  if( ( status = PdhGetFormattedCounterValue( m_hPerformanceCounter, dwFormat, 0, &value ) ) != ERROR_SUCCESS )
    throw ex::VException( LogRec( "Error formatting peformance data. error code: " + ToAscii( status ), "vxPerformanceCounter", "GetCurrentValue" ) );
  
  return value;
} // GetCurrentValue( DWORD dwFormat )


/**
 * Gets the current value as 32-bit integer
 *
 * @return current value
 */
int4 vxPerformanceCounter::GetCurrentValueAsInt32()
{
  return GetCurrentValue( PDH_FMT_LONG ).longValue;
} // GetCurrentValueAsInt32()


/**
 * Gets the current value as 64-bit integer
 *
 * @return current value
 */
int8 vxPerformanceCounter::GetCurrentValueAsInt64()
{
  return GetCurrentValue( PDH_FMT_LARGE ).largeValue;
} // GetCurrentValueAsInt64()


/**
 * Gets the current value as double
 *
 * @return current value
 */
float8 vxPerformanceCounter::GetCurrentValueAsDouble()
{
  return GetCurrentValue( PDH_FMT_DOUBLE ).doubleValue;
} // GetCurrentValueAsDouble()


// $Log: vxPerformanceCounter.C,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/06/01 13:45:02  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxPerformanceCounter.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: vxPerformanceCounter.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
