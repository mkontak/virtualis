// $Id: vxPerformanceCounter.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailtot:george@viatronix.com)

// pragma
#pragma once
#pragma comment ( lib, "pdh.lib" )

// includes
#include <pdh.h> 

// class declaration
class vxPerformanceCounter
{
// member functions
public:

  /// constructor from category and counter name
  vxPerformanceCounter( const std::string & sCategoryName, const std::string & sCounterName );

  /// constructor from category, counter and instance name
  vxPerformanceCounter( const std::string & sCategoryName, 
                        const std::string & sCounterName, 
                        const std::string & sInstanceName );

  /// destructor
  virtual ~vxPerformanceCounter();

  /// returns the current value of the counter as 32-bit integer
  int4 GetCurrentValueAsInt32();

  /// returns the current value of the counter as 64-bit integer
  int8 GetCurrentValueAsInt64();

  /// returns the current value of the counter as double 
  float8 GetCurrentValueAsDouble();

private:

  /// private copy constructor
  vxPerformanceCounter( const vxPerformanceCounter & counter );

  /// private assignment operator
  vxPerformanceCounter & operator=( const vxPerformanceCounter & rhs );  

  /// creates a query object and adds the requested counter
  void OpenCounter();

  /// returns the current value of the counter in the specified format 
  PDH_FMT_COUNTERVALUE GetCurrentValue( DWORD dwFormat );


// member variables
private:

  /// category name
  std::string m_sCategoryName;

  /// counter name
  std::string m_sCounterName;

  PDH_FMT_COUNTERVALUE m_currentValue;
  /// instance name
  std::string m_sInstanceName;

  /// peformance query object handle
  HQUERY m_hPerformanceQuery;
  
  /// peformance counter object handle
  HCOUNTER m_hPerformanceCounter;

}; // class vxPerformanceCounter


// $Log: vxPerformanceCounter.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/06/01 13:45:02  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxPerformanceCounter.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: vxPerformanceCounter.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $

