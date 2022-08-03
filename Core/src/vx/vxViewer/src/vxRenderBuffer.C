// $Id: vxRenderBuffer.C,v 1.13.2.1 2010/06/01 19:33:11 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


#include "stdafx.h"
#include "vxRenderBuffer.h"
#include "vxVisualizationPreferences.h"
#include "SystemInfo.h"


/**
 * Default constructor
 */
vxRenderBuffer::vxRenderBuffer() : 
 m_colorImage( PixelRGBA< uint1 >( 0, 0, 0, 255 ) ),
 m_depthImageWC( 0.0F ),
 m_depthImageVC( 0.0F ),
 m_fRenderTime( (std::numeric_limits<float4>::max)() ),
 m_eState( vxRenderBuffer::EMPTY ),
 m_iRenderThreadCount( 0 ),
 m_bIsModified( false ),
 m_uDepthBufferFlags( 0 )
{
  //m_threadStatistics.resize( min( vxVisualizationPreferences::GetMaxRenderThreads(), SystemInfo::GetCpuInfo().GetTotalThreads() ) );
  m_threadStatistics.resize(vxVisualizationPreferences::GetMaxRenderThreads());
  SetBufferSize( Triple< uint4>( 2, 2, 0 ) );
} // vxRenderBuffer()


/*
 * Constructor. Sets the initial buffer size.
 *
 * @param   size      initial size
 */
 vxRenderBuffer::vxRenderBuffer( const Triple< uint4 > & size ) : 
 m_colorImage( PixelRGBA< uint1 >( 0, 0, 0, 255 ) ),
 m_depthImageWC( 0.0F ),
 m_depthImageVC( 0.0F ),
 m_fRenderTime( (std::numeric_limits<float4>::max)() ),
 m_eState( vxRenderBuffer::EMPTY ),
 m_iRenderThreadCount( 0 ),
 m_bIsModified( false ),
 m_uDepthBufferFlags( 0 )
{
  //m_threadStatistics.resize( min( vxVisualizationPreferences::GetMaxRenderThreads(), SystemInfo::GetCpuInfo().GetTotalThreads() ) );
  m_threadStatistics.resize( vxVisualizationPreferences::GetMaxRenderThreads() );
  SetBufferSize( size );
} // vxRenderBuffer( const Triple< uint4 > & size )


/**
 * Copy constructor
 *
 * @param   other   other vxRenderBuffer
 */
vxRenderBuffer::vxRenderBuffer( const vxRenderBuffer & other ) : 
  m_colorImage( other.m_colorImage ),
  m_depthImageWC( other.m_depthImageWC ),
  m_depthImageVC( other.m_depthImageVC ),
  m_renderSize( other.m_renderSize ),
  m_fRenderTime( other.m_fRenderTime ),
  m_iRenderThreadCount( other.m_iRenderThreadCount ),
  m_eState( other.m_eState ),
  m_bIsModified( other.m_bIsModified ),
  m_threadStatistics( other.m_threadStatistics ),
  m_uDepthBufferFlags( other.m_uDepthBufferFlags )
{
} // vxRenderBuffer()


/**
 * Assignment operator
 *
 * @param   rhs   right hand side of operator
 * @return  reference to self
 */
vxRenderBuffer & vxRenderBuffer::operator=( const vxRenderBuffer & rhs )
{
  if( &rhs != this )
  {
    m_colorImage  = rhs.m_colorImage;
    m_depthImageWC  = rhs.m_depthImageWC;
    m_depthImageVC  = rhs.m_depthImageVC;
    m_renderSize  = rhs.m_renderSize; 
    m_fRenderTime = rhs.m_fRenderTime;
    m_eState      = rhs.m_eState;
    m_iRenderThreadCount = rhs.m_iRenderThreadCount;
    m_bIsModified = rhs.m_bIsModified;
    m_threadStatistics =  rhs.m_threadStatistics;
    m_uDepthBufferFlags = rhs.m_uDepthBufferFlags;
  }
  return *this;
} // operator=( const vxRenderBufdfer & rhs )


/**
 * Destructor
 */
vxRenderBuffer::~vxRenderBuffer()
{
} // ~vxRenderBuffer()


 /**
  *  Clears out the render buffer with the specified color
  *
  * @param   color     celar color
  */
void vxRenderBuffer::Clear( const PixelRGBA< uint1 > & color )
{
  m_colorImage.SetClearColor( color );
  m_colorImage.Clear();
} // Clear( const PixelRGBA< uint1 > & color )


/**
 * Sets the buffer size
 */
void vxRenderBuffer::SetBufferSize( const Triple< uint4 > & bufferSize )
{
  m_colorImage.Resize( bufferSize );
  
  if( m_uDepthBufferFlags & vxRenderBuffer::DEPTH_IMAGE_VC )
  {
    m_depthImageVC.Resize( bufferSize );
  }
  else
  {
    const Triplei empty( 1, 1, 0 );
    m_depthImageVC.Resize( empty );
  }

  if( m_uDepthBufferFlags & vxRenderBuffer::DEPTH_IMAGE_WC )
  {
    m_depthImageWC.Resize( bufferSize );
  }
  else
  {
    const Triplei empty( 1, 1, 0 );
    m_depthImageWC.Resize( empty );
  }
} // SetBufferSize( const Triple< int4 > & bufferSize )

// $Log: vxRenderBuffer.C,v $
// Revision 1.13.2.1  2010/06/01 19:33:11  dongqing
// remove the maximum number of thread control capped by number of cores
//
// Revision 1.13  2007/09/17 18:48:57  geconomos
// added flag to indicate which depth buffers to use
//
// Revision 1.12  2007/04/12 14:11:47  geconomos
// added threading statistics
//
// Revision 1.11  2007/04/02 21:17:47  gdavidson
// Added IsModified
//
// Revision 1.10  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.9  2006/12/06 15:28:01  geconomos
// added missing initialization of m_iRenderThreadCount and added to copy and assigment operator
//
// Revision 1.8  2006/10/23 01:38:08  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.7  2006/10/04 14:35:55  geconomos
// Issue # 5033: Default the "last render time" to max_float to ensure a low quality image is generated when when the viewer type changes for given viewer
//
// Revision 1.6  2006/09/28 18:29:50  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.5  2006/09/20 14:27:28  geconomos
// defaulted buffer size to 2x2 on initialization
//
// Revision 1.4  2006/08/03 14:11:30  geconomos
// removed the Last from GetLastRenderSize/SetLastRenderSize and GetLastRenderTime/SetLastRenderTime
//
// Revision 1.3  2006/07/07 13:37:16  geconomos
// + added constructor from size
// + renamed property "RenderSize" -> "LastRenderSize"
//
// Revision 1.2  2006/06/27 17:23:58  geconomos
// + converted render and buffer size to Triple<uint4>
// + added "last frame time"
//
// Revision 1.1  2006/06/20 20:06:28  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderBuffer.C,v 1.13.2.1 2010/06/01 19:33:11 dongqing Exp $
// $Id: vxRenderBuffer.C,v 1.13.2.1 2010/06/01 19:33:11 dongqing Exp $
