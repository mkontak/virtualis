// $Id: vxRendererUtils.h,v 1.4.8.1 2009/03/23 15:43:15 gdavidson Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * Define vxRender utilities 
 */


// pragmas
#pragma once


// includes
#include "Image.h"
#include "Point.h"
#include "Normal.h"
#include "Vector.h"
#include "Triple.h"
#include "PixelRGBA.h"


// forward declarations
template <class T> class Image;
template <class T> class Box;
class BaseHistogram;
class vxWinRect;


// class definition
namespace vxRendererUtils
{

  // projects a object space point into screen space (using current OpenGL state)
  VX_VIEWER_DLL void Project( float4 fObjX, float4 fObjY, float4 fObjZ,
                              float4 & fWinX, float4 & fWinY, float4 & fWinZ );

  // projects a object space point into screen space (using specified matrices)
  VX_VIEWER_DLL void Project( float4 fObjX, float4 fObjY, float4 fObjZ, 
                              GLint * pViewPort, GLdouble * pModelmatrix, GLdouble * pProjmatrix,
                              float4 & fWinX, float4 & fWinY, float4 & fWinZ );
  
  // draw rectangle 2f
  VX_VIEWER_DLL void DrawRectangle2i( GLenum mode,
                                      int4 iRed, int4 iGreen, int4 iBlue,
                                      int4 iX, int4 iY, int4 iSizeX, int4 iSizeY );

  /// invert a color
  VX_VIEWER_DLL PixelRGBA<float4> InvertColor( const PixelRGBA<float4> & color );

  /// efficiently interpolate two uint1 values using fixed point math
  VX_VIEWER_DLL uint2 InterpolateFixedPoint( const uint1 & uLeft, const uint1 & uRight, const uint1 & uWeightRight );

  /// draw a floating point point
  VX_VIEWER_DLL inline void GlVertex( const Point<float4> & vPoint );

  /// draw box
  VX_VIEWER_DLL void GLDrawBox( Box<float4> & box, const PixelRGBA<float4> & color );

  /// draw a 2D point
  VX_VIEWER_DLL void GLDrawPoint2f( const Point2D<float4> & point2D, const PixelRGBA<float4> & color, const float4 & fSize );

  /// draw a 2D point with border
  VX_VIEWER_DLL void GLDrawPointWithBorder2f( const Point2D<float4> & point2D, 
                                              const PixelRGBA<float4> & color, const PixelRGBA<float4> & colorBorder, 
                                              const float4 & fSize, const float4 & fBorderSize );

  /// helper function to render a floating point Point
  VX_VIEWER_DLL inline void GlVertex2f( const Point2D<float4> & vPoint ) { glVertex2fv( & ( vPoint.m_x ) ); }

  /// helper function to render a floating point Point
  VX_VIEWER_DLL inline void GlVertex( const Vector<float4> & vVector ) { glVertex3fv( & ( vVector.m_x ) ); }

  /// helper function to set the color from a floating point Triple
  VX_VIEWER_DLL inline void GlColor3f( const Triple<float4> & colorTriple ) { glColor3fv( & ( colorTriple.m_x ) ); }

  /// render a 3D arrow at some position, direction
  VX_VIEWER_DLL void RenderArrow( const Point<float4> & vPosition, const Normal<float4> & vDirection,
                                  const float4 & fLength, const bool & bCentered, 
                                  const PixelRGBA<float4> & color = PixelRGBA<float4>( 0.3F, 0.0F, 0.5F, 1.0F ) );

  /// render a 3D arrow at some position, direction by enabling or disabling depth buffer
  VX_VIEWER_DLL void RenderArrow( const Point<float4> & vPosition, const Normal<float4> & vDirection,
                                  const float4 & fLength, const bool & bCentered, const bool & bUseDepthBuffer,
                                  const PixelRGBA<float4> & color = PixelRGBA<float4>( 0.3F, 0.0F, 0.5F, 1.0F ) );


  /// creates an image of a given histogram and range
  VX_VIEWER_DLL void CreateHistogramImage( BaseHistogram * pHistogram, Image<PixelRGBA<uint1> > & image,
                                           const uint2 & uLowerBin, const uint2 & uUpperBin, PixelRGBA<uint1> & color );

  /// draws a small rectangle on the screen to indicate non-optimal image quality
  VX_VIEWER_DLL void DrawImageQualityWarningRectangle( const vxWinRect & viewerRect, bool bLowQuality );

  /// draws viatronix logo
  VX_VIEWER_DLL void DrawLogo( const vxWinRect & window, bool bTranslucent = false );

} // namespace vxRenderUtils


// inlines
#include "vxRendererUtils.inl"


// $Log: vxRendererUtils.h,v $
// Revision 1.4.8.1  2009/03/23 15:43:15  gdavidson
// Added code to renderer a viatronix logo
//
// Revision 1.4  2007/01/25 22:41:39  gdavidson
// Added a flag that allows rendering for two warning levels
//
// Revision 1.3  2006/12/12 16:50:23  romy
// Disables depth buffer during the arrow rendering to bring it to the front of Slice shadows.
//
// Revision 1.2  2006/09/20 20:34:43  frank
// Issue #4963: Added a utility function to display a small orange box on the view to indicate that the image is not fully rendered
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2003/07/30 18:43:41  michael
// added utility to create a histogram image
//
// Revision 1.21  2003/07/29 11:05:09  michael
// replaced redundant code by one centralized piece that draws the histogram.
// However, it is still actively drawn but should use a texture that is only updated
// if the BaseHistogram has changed ...
//
// Revision 1.20  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.19  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.18  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.17  2003/05/14 17:28:25  michael
// code review
//
// Revision 1.16  2003/05/08 21:06:56  geconomos
// Fixed Michael's stuff.
//
// Revision 1.15  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.14  2002/11/15 18:10:15  frank
// Removed unused parameter (iOpacity)
//
// Revision 1.13  2002/08/21 18:30:40  michael
// extended utility method for drawing a rectangle
//
// Revision 1.12  2002/08/07 14:37:07  michael
// Added utility to draw a rectangle in OpenGL
//
// Revision 1.11  2002/08/02 14:51:37  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.10  2002/07/30 17:17:11  michael
// Removed redundant methods
//
// Revision 1.9  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.8  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.7.2.6  2002/07/17 20:49:09  frank
// Made block visibility more convenient to use.
//
// Revision 1.7.2.5  2002/07/17 17:06:02  frank
// Added utility class to manage block visibility complexity.
//
// Revision 1.7.2.4  2002/07/08 15:22:53  frank
// Added label visible utility function.
//
// Revision 1.7.2.3  2002/07/02 22:24:20  michael
// fixed bug in bit encoding for intervals
//
// Revision 1.7.2.2  2002/07/02 13:47:50  michael
// added 32 bit encoding method for interval and fixed 12 bit encoding code
//
// Revision 1.7.2.1  2002/07/01 21:23:16  frank
// Added utility function to traverse histograms.
//
// Revision 1.7  2002/06/27 16:02:21  soeren
// made parameter const
//
// Revision 1.6  2002/06/27 15:42:54  soeren
// added GenerateBitCodeFor12BitTransferFunction
//
// Revision 1.5  2002/06/27 14:07:32  michael
// using 16 bins for transfer function and 31 bins for labels and moved helper
// methods to vxRendererUtils
//
// Revision 1.4  2002/03/20 19:46:02  michael
// moved OpenGl back-projection stuff to 74_vxUtils
//
// Revision 1.3  2002/03/20 19:19:16  michael
// extended vxRendererUtils to perform the OpenGL backprojection
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:26:20  michael
// added inital version containing enums and others needed for rendering ...
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererUtils.h,v 1.4.8.1 2009/03/23 15:43:15 gdavidson Exp $
// $Id: vxRendererUtils.h,v 1.4.8.1 2009/03/23 15:43:15 gdavidson Exp $
