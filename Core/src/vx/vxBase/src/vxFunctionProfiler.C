// $Id: vxFunctionProfiler.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxFunctionProfiler.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


// static member declarations
uint4 vxFunctionProfiler::m_uIndentationLevel( 0 );
std::string vxFunctionProfiler::m_sFullLog( "" );
Timer vxFunctionProfiler::m_gapTimer;
int4 vxFunctionProfiler::m_uLastIndentationLevel( -1 );
std::map< std::string, struct vxFunctionProfiler::FunctionSummary > vxFunctionProfiler::m_functionSummaryMap;


/**
 * constructor
 *
 * @param sFunctionName the function name
 */
vxFunctionProfiler::vxFunctionProfiler( const std::string & sFunctionName )
: m_sFunctionName( sFunctionName )
{

  if ( m_uIndentationLevel == 0 ) m_sFullLog = "";

  std::string sIndent( "" );
  for ( int i=0; i<m_uIndentationLevel; ++i ) sIndent += "  ";

  if ( m_uLastIndentationLevel >= 0 && m_gapTimer.ElapsedTime() > 1.0F )
  {
    std::string sMessage( sIndent + "  " + ToAscii( m_gapTimer.ElapsedTime() ) + "sec in the gap between messages" );
    LogFyi( sMessage, "vxFunctionProfiler", "vxFunctionProfiler" );
    m_sFullLog += sMessage + "\n";
    m_gapTimer.Reset();
  }
  
  std::string sMessage( sIndent + "Entering \"" + m_sFunctionName + "\"" );
  LogFyi( sMessage, "vxFunctionProfiler", "vxFunctionProfiler" );
  m_sFullLog += sMessage + "\n";

  ++m_uIndentationLevel;

} // vxFunctionProfiler()


/**
 * comparison operator for function summary structure
 *
 * @param a first struct
 * @param b second struct
 *
 * @return a < b ?
 */
bool operator <( const struct vxFunctionProfiler::FunctionSummary & a, const struct vxFunctionProfiler::FunctionSummary & b )
{
  return a.m_fTotalTime > b.m_fTotalTime;
} // operator < ()


/**
 * destructor
 */
vxFunctionProfiler::~vxFunctionProfiler()
{

  m_timer.Stop();

  --m_uIndentationLevel;

  std::string sIndent( "" );
  for ( int i=0; i<m_uIndentationLevel; ++i ) sIndent += "  ";

  std::string sMessage( sIndent + ToAscii( m_timer.ElapsedTime() ) + "sec  Leaving \"" + m_sFunctionName + "\"" );
  LogFyi( sMessage, "vxFunctionProfiler", "~vxFunctionProfiler" );
  m_sFullLog += sMessage + "\n";
  
  m_uLastIndentationLevel = m_uIndentationLevel;
  m_gapTimer.Reset();
  
  std::map< std::string, struct FunctionSummary >::iterator iter( m_functionSummaryMap.find( m_sFunctionName ) );
  if ( iter == m_functionSummaryMap.end() )
  {
    struct FunctionSummary summary;
    summary.m_sFunctionName = m_sFunctionName;
    summary.m_uInvocations = 1;
    summary.m_fTotalTime = m_timer.ElapsedTime();
    m_functionSummaryMap[ m_sFunctionName ] = summary;
  }
  else
  {
    m_functionSummaryMap[ m_sFunctionName ].m_uInvocations ++;
    m_functionSummaryMap[ m_sFunctionName ].m_fTotalTime += m_timer.ElapsedTime();
  }

  if ( m_uIndentationLevel == 0 )
  {
    std::string sSummary( "" );
    std::vector< struct FunctionSummary > sorted;
    std::for_each(m_functionSummaryMap.begin(), m_functionSummaryMap.end(), [&sorted] (std::pair<std::string, struct FunctionSummary> item) { sorted.push_back(item.second); } );

    for ( int i=0; i<sorted.size(); ++i )
      sSummary += ToAscii( sorted[i].m_uInvocations ) + "\t" + ToAscii( sorted[i].m_fTotalTime ) + "\t" + sorted[i].m_sFunctionName + "\n";
    LogFyi( "Full profiling log to follow:\n\n" + m_sFullLog + "\n\n", "vxFunctionProfiler", "~vxFunctionProfiler" );
    LogFyi( "Summary of function calls:\n\n" + sSummary + "\n\n", "vxFunctionProfiler", "~vxFunctionProfiler" );
  }

} // ~vxFunctionProfiler()


// $Log: vxFunctionProfiler.C,v $
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
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxFunctionProfiler.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: vxFunctionProfiler.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
