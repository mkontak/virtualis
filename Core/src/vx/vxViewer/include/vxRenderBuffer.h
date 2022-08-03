// $Id: vxRenderBuffer.h,v 1.14 2007/09/17 18:48:57 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "Image.h"
#include "PixelRgba.h"
#include "CriticalSection.h"


/**
 * Contains the color image, depth buffer and rendering statistics used by all of
 * the cpu-based renderers
 *
 * TODO: Disable depth buffer usuage when not needed
 */
class VX_VIEWER_DLL vxRenderBuffer
{
// enums
public:

  /// buffer state enumeration
  enum BufferStatEnum { EMPTY, LOW, INTERMEDIATE, HIGH };


  /// buffer flags
  enum DepthBufferFlagsEnum
  {
    DEPTH_IMAGE_NONE = 0x00,
    DEPTH_IMAGE_VC = 0x01,
    DEPTH_IMAGE_WC = 0x02,
  };

// member functions
public:
  
  /// constructor
  vxRenderBuffer();

  /// constructor from buffer size
  vxRenderBuffer( const Triple< uint4 > & size );

  /// destructor
  virtual ~vxRenderBuffer();

  /// celars out the render buffer with the specified color
  void Clear( const PixelRGBA< uint1 > & color );

  /// gets the buffer size
  inline const Triple< uint4 > & GetBufferSize() const  { return m_colorImage.GetDim(); }

  /// sets the buffer size
  void SetBufferSize( const Triple< uint4 > & bufferSize );

  /// gets the color image
  inline Image< PixelRGBA< uint1 > > & GetColorImage() { return m_colorImage; }

  /// sets the color image
  inline void SetColorImage( const Image< PixelRGBA< uint1 > > & colorImage ) { m_colorImage = colorImage; }

  /// gets the depth image in world coordinates
  inline Image< float4 > & GetDepthImageWC() { return m_depthImageWC; }

  /// gets the depth image in world coordinates
  inline void SetDepthImageWC( const Image< float4 > & depthImageWC ) { m_depthImageWC = depthImageWC; }

  /// gets the depth image in volume coordinates
  inline Image< float4 > & GetDepthImageVC() { return m_depthImageVC; }

  /// gets the depth image in volume coordinates
  inline void SetDepthImageVC( const Image< float4 > & depthImageVC ) { m_depthImageVC = depthImageVC; }

  /// gets the last render size
  inline const Triple< uint4 > & GetRenderSize() const { return m_renderSize; }

  /// sets the render size used to generate the image
  inline void SetRenderSize( const Triple< uint4 > & lastRenderSize ){ m_renderSize = lastRenderSize; }
  
  /// gets the time in second taken to generate the image
  inline float4 GetRenderTime() const { return m_fRenderTime; }

  /// set the time in second taken to generate the image
  inline void SetRenderTime( const float4 & fLastRenderTime ) { m_fRenderTime = fLastRenderTime; }

	///Sets the Thread Count used by Rendering Engine
	inline void SetRenderThreadCount( const int4 count ) { m_iRenderThreadCount = count ; }

	///Gets the Thread Count used by Rendering Engine
	inline int4 GetRenderThreadCount() const { return m_iRenderThreadCount; }
	
  /// gets the buffer state
  inline const vxRenderBuffer::BufferStatEnum & GetState() const { return m_eState; }

  /// sets the buffer state
  inline void SetState( const vxRenderBuffer::BufferStatEnum & eState ) { m_eState = eState; }

  /// gets the depth buffer flags
  inline const uint4 GetDepthBufferFlags() const { return m_uDepthBufferFlags; }

  /// sets the depth buffer flags
  inline void SetDepthBufferFlags( const uint4 uDepthBufferFlags ) { m_uDepthBufferFlags = uDepthBufferFlags; }

  /// gets if the render buffer has been modified
  const bool & IsModified() const { return m_bIsModified; }

  /// sets if the render buffer has been modified
  void SetModified( const bool & bRenderBufferModified ) { m_bIsModified = bRenderBufferModified; }

  /// gets the thread statistics
  inline std::vector< float4 > & GetThreadingStatistics() { return m_threadStatistics; }

  /// returns a critical section that should be used when modifying the buffer
  inline CriticalSection &  GetSyncObject() { return m_syncobj; }

  /// copy constructor
  vxRenderBuffer( const vxRenderBuffer & other );
  
  /// assignment operator
  vxRenderBuffer & operator=( const vxRenderBuffer & rhs );

// member variables
private:

  // color image
  Image< PixelRGBA< uint1 > > m_colorImage;

  /// depth image in world coordinates
  Image< float4 > m_depthImageWC;

  /// depth image in volume coordinates
  Image< float4 > m_depthImageVC;

  /// last render size
  Triple< uint4 > m_renderSize;

  /// last render time (in seconds)
  float4 m_fRenderTime;

	/// number of threads used to render this buffer
	int4 m_iRenderThreadCount;

  /// number of times a given thread's renderwindow was called
  std::vector< float4 > m_threadStatistics;

  /// buffer state
  BufferStatEnum m_eState;

  /// used to lock buffer while updating environment and rendering
  CriticalSection m_syncobj;

  /// indicates if the render buffers has been modified
  bool m_bIsModified;

  /// depth buffer flags, indicates which depth buffers are used
  uint4 m_uDepthBufferFlags;

}; // class vxRenderBuffer


// $Log: vxRenderBuffer.h,v $
// Revision 1.14  2007/09/17 18:48:57  geconomos
// added flag to indicate which depth buffers to use
//
// Revision 1.13  2007/04/12 14:11:46  geconomos
// added threading statistics
//
// Revision 1.12  2007/04/02 21:17:47  gdavidson
// Added IsModified
//
// Revision 1.11  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.10  2007/01/25 22:43:58  gdavidson
// Added an intermediate render level
//
// Revision 1.9  2006/12/04 20:33:33  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.8  2006/10/23 01:38:08  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.7  2006/10/05 19:43:56  geconomos
// added critical section that can be used synchronize access to the buffer
//
// Revision 1.6  2006/09/28 18:29:50  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.5  2006/08/03 14:11:14  geconomos
// removed the Last from GetLastRenderSize/SetLastRenderSize and GetLastRenderTime/SetLastRenderTime
//
// Revision 1.4  2006/07/07 13:37:16  geconomos
// + added constructor from size
// + renamed property "RenderSize" -> "LastRenderSize"
//
// Revision 1.3  2006/06/27 17:24:23  geconomos
// + converted render and buffer size to Triple<uint4>
// + added "last frame time"
//
// Revision 1.2  2006/06/21 14:07:49  geconomos
// + code cleanup
// + exported classes
//
// Revision 1.1  2006/06/20 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderBuffer.h,v 1.14 2007/09/17 18:48:57 geconomos Exp $
// $Id: vxRenderBuffer.h,v 1.14 2007/09/17 18:48:57 geconomos Exp $
