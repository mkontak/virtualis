// $Id: vxFunctionProfiler.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "Timer.h"


/**
  * Class to aid in the profiling of algorithms.
  * Automatically times the execution of the code in the enclosing scope of
  * any object of this class and logs with proper indentation.
  */
class VX_BASE_DLL vxFunctionProfiler
{
// inner classes
public:

  struct FunctionSummary
  {
  public:
  
    /// function name
    std::string m_sFunctionName;
  
    /// number of invocations
    uint4 m_uInvocations;
    
    /// total elapsed time inclusive
    float4 m_fTotalTime;
    
  }; // struct FunctionSummary

// member functions
public:

  /// constructor
  vxFunctionProfiler( const std::string & sFunctionName );

  /// destructor
  ~vxFunctionProfiler();

// data members    
private:

  /// current level of indentation
  static uint4 m_uIndentationLevel;
  
  /// a full copy of the log
  static std::string m_sFullLog;
  
  /// timer to time gap between calls at same level
  static Timer m_gapTimer;
  
  /// the indentation level at the last invocation
  static int4 m_uLastIndentationLevel;

  /// current function name
  std::string m_sFunctionName;
  
  /// timer for current function
  Timer m_timer;
  
  /// table of repeated function calls for by-function summary
  static std::map< std::string, struct FunctionSummary > m_functionSummaryMap;
  
}; // class vxFunctionProfiler


// $Log: vxFunctionProfiler.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/08/30 21:15:00  frank
// moved here from vascular
//
// Revision 1.2  2005/06/08 14:58:37  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1  2004/05/26 12:42:36  frank
// renamed the function profiler
//
// Revision 1.2  2004/04/29 17:57:47  frank
// added the function summary
//
// Revision 1.1  2004/04/26 15:52:53  frank
// moved function profiler into its own file
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxFunctionProfiler.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: vxFunctionProfiler.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
