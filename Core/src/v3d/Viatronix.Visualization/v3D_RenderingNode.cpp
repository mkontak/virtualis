// $Id: v3D_RenderingNode.cpp,v 1.4 2007/06/28 19:34:14 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "v3D_RenderingNode.h"
#include "v3D_Viewer.h"
#include "vxModifiedQueue.h"
#include "vxControllerGlobalDatasets.h"
#include "vxControllerGlobalActiveEnvironment.h"
#include "vxControllerGlobalSliceShadows.h"
#include "v3D_Triplet.h"
#include "vxRenderingNodeThreadManager.h"


USING_VISUALIZATION_NS


/**
 * constructor
 */
RenderingNode::RenderingNode( System::IntPtr renderingNode ) :
  m_pRenderingNode( reinterpret_cast< vxRenderingNode * >( renderingNode.ToPointer() ))
{
} // RenderingNode( renderingNode )


/**
 * Aborts rendering and waits
 */
void RenderingNode::AbortRenderingAndWait()
{
  m_pRenderingNode->Abort();
} // AbortRenderingAndWait()


/**
 *  Gets the is rendering flag
 *
 * @return true if rendering
 */
bool RenderingNode::IsRendering::get()
{
  return m_pRenderingNode->GetThreadManager()->IsRendering();
} // get_IsRendering()


/**
 *  Sets the high quality thread enabled flag
 *
 * @param   bEnabled    true to enable
 */
void RenderingNode::EnableHighQualityThread::set( bool bEnabled )
{
  m_pRenderingNode->SetHighQualityThreadEnabled( bEnabled );
} // set_EnableHighQualityThread( bool bEnabled )


/**
 *  Gets the high quality thread enabled flag
 *
 * @return true if enable
 */
bool RenderingNode::EnableHighQualityThread::get()
{
  return m_pRenderingNode->GetHighQualityThreadEnabled();
} // get_EnableHighQualityThread()


/**
 *  Sets the fixed sized rendering enabled flag
 *
 * @param   bEnabled    true to enable
 */
void RenderingNode::EnableFixedSizeRendering::set( bool bEnabled )
{
  m_pRenderingNode->SetFixedSizeRenderingEnabled( bEnabled );
} // set_EnableFixedSizeRendering( bool bEnabled )


/**
 *  Gets the fixed sized rendering enabled flag
 *
 * @return true if enable
 */
bool RenderingNode::EnableFixedSizeRendering::get()
{
  return m_pRenderingNode->GetFixedSizeRenderingEnabled();
} // get_EnableFixedSizeRendering()


/**
 *  Sets the fixed rendering size
 *
 * @param   mpSize    rendering size
 */
void RenderingNode::FixedRenderingSize::set( Triplet ^ mpSize )
{
  m_pRenderingNode->SetFixedRenderingSize( Triple< uint4 >( mpSize->x, mpSize->y, 0.0F ) );
} // set_EnableFixedSizeRendering( Triplet ^ mpSize )


/**
 *  Gets the high quality thread enabled flag
 *
 * @return rendering size
 */
Triplet ^ RenderingNode::FixedRenderingSize::get()
{
  const Triple< uint4 > & size = m_pRenderingNode->GetFixedRenderingSize();
  return gcnew Triplet( size.m_x, size.m_y, 0 );
} // get_EnableFixedSizeRendering()


/**
 * Gets the number of available threads for rendering
 *
 * @return          number of threads available for rendering
 */
int4 RenderingNode::NumberOfRenderThreads::get()
{
  return m_pRenderingNode->GetThreadManager()->GetNumberOfRenderThreads();
} // get_NumberOfRenderThreads


/**
 * Gets the number of available threads for rendering
 *
 * @param       iThreadCount    number of threads available for rendering
 */
void RenderingNode::NumberOfRenderThreads::set( int4 iThreadCount )
{
  m_pRenderingNode->GetThreadManager()->SetNumberOfRenderThreads( iThreadCount );
} // set_NumberOfRenderThreads


// $Log: v3D_RenderingNode.cpp,v $
// Revision 1.4  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.3  2007/03/22 18:24:01  gdavidson
// Added property for NumberOfRenderThreads
//
// Revision 1.2.2.1  2007/04/05 17:54:53  geconomos
// Issue# 5560: crash while switching "detail view" viewer type
//
// Revision 1.2  2007/03/06 19:36:47  geconomos
// code review
//
// Revision 1.1  2007/02/05 19:28:00  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.13  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.12  2006/12/11 19:14:09  geconomos
// issue #5152 - crashing when application is minimized or maximized
//
// Revision 1.11  2006/11/09 15:02:53  gdavidson
// Commented out GlobalControllers from the Initialize method
//
// Revision 1.10  2006/10/24 12:36:16  geconomos
// added slcie shadows global controller
//
// Revision 1.9  2006/10/20 20:53:55  geconomos
// initialization of gloabl controllers
//
// Revision 1.8  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.7  2006/09/27 16:08:14  geconomos
// added support for fixed size rendering
//
// Revision 1.6  2006/09/14 20:25:17  geconomos
// controller initialization (a mess for now)
//
// Revision 1.5  2006/08/31 14:14:21  geconomos
// changes to rendering manager
//
// Revision 1.4  2006/08/28 21:07:55  geconomos
// added ability to disable the high quality image thread
//
// Revision 1.3  2006/08/28 14:04:07  geconomos
// added Reset() method
//
// Revision 1.2  2006/08/23 18:02:38  geconomos
// updated for change to vxRenderingNode public interface
//
// Revision 1.1  2006/08/03 14:19:50  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_RenderingNode.cpp,v 1.4 2007/06/28 19:34:14 jmeade Exp $
// $Id: v3D_RenderingNode.cpp,v 1.4 2007/06/28 19:34:14 jmeade Exp $
