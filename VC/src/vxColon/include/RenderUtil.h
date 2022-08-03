// $Id: RenderUtil.h,v 1.8.2.2 2010/02/01 13:40:09 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


/**
 * Utilities for rendering
 */


// pragmas
#pragma once


// includes
#include <vector>
#include <string>
#include "Point2D.h"
#include "Point.h"
#include "Rect.h"
#include "Vector.h"
#include "Vector4d.h"
#include "Matrix.h"
#include "TransferFunctionVC.h"
#include "GLFontGeorge.h"
#include "PixelRGBA.h"
#include "vxVolumeHeader.h"
#include "ColonSession.h"
#include "vxCadFinding.h"


// forward declarations
template <class VoxelType> class vxBlockVolume;
template <class VoxelType> class Box;


class RenderUtil
{
// Member Functions
public:

  /// Constructor.
	RenderUtil();

  /// Destructor.
	virtual ~RenderUtil();
  
  /// Prints a debug message then checks the OpenGL error state with verbose diagnostics.
  static void GLDebug(const std::string & sMessage);

  /// OpenGL helper function to render a floating point Point.
  static inline void GlVertex2f(const Point2D<float4> & vPoint) { glVertex2fv(&(vPoint.m_x)); }
  
  /// OpenGL helper function to render a floating point Point.
  static inline void GlVertex(const Point<float4> & vPoint) { glVertex3fv(&(vPoint.m_x)); }
  
  /// OpenGL helper function to render a floating point Point.
  static inline void GlVertex(const Vector<float4> & vVector) { glVertex3fv(&(vVector.m_x)); }
  
  /// OpenGL helper function to render a floating point Point.
  static inline void GlVertex(const Vector4Df & vVector) { glVertex3fv(&(vVector.m_x)); }
  
  /// OpenGL helper function to set the color from a floating point Triple.
  static inline void GlColor3f(const Triple<float4> & colorTriple) { glColor3fv(&(colorTriple.m_x)); }
  
  /// OpenGL helper function to set the color from an unsigned char Triple.
  static inline void GlColor3ub(const Triple<uint1> & colorTriple) { glColor3ubv(&(colorTriple.m_x)); }

  /// Render a 3D arrow at some position, direction.
  static void RenderArrow(const Point<float4> & vPosition, const Normal<float4> & vDirection, const float4 & fLength,
                          const bool & bCentered, const int4 iMark,
                          const PixelRGBA<float4> & color, const bool & bExtraLights );

  /// Draw the profile of a histogram using lines.
  static void RenderHistogram(const std::vector<uint4> & vHistogram, const Rect<uint4> & vDrawArea,
    const std::vector<float4> & vRGB256);

  /// Draw the profile of a histogram using lines.
  static void RenderHistogram(const std::vector<uint4> & vHistogram, const Rect<uint4> & vDrawArea, const float * pRGBA256);

  /// Draw a histogram as bars.
  static void RenderHistogramBars(std::vector<uint4> & vHistogram, const Rect<uint4> & vDrawArea,
                                  TransferFunctionVC & transferFunction, GLFontGeorge & font,
                                  const std::pair<uint4, uint4> & minMaxPairHU);
  
  /// Inverts color
  static PixelRGBA<float4> InvertColor(const PixelRGBA<float4> & color);

  /// Draw a circle.
  static void GLDrawCircle(const Point2D<float4> &center, const float4 &radius, const bool &bFill = false);
                                     
  /// Draw a point.
  static void GLDrawPoint(const Point<float4> & point, const PixelRGBA<float4> & color, const float4 & fSize);

  /// Draw a 2D point
  static void GLDrawPoint2f(const Point2D<float4> & point2D, const PixelRGBA<float4> & color, const float4 & fSize);

  /// Draw a 2D point with border
  static void GLDrawPointWithBorder2f(const Point2D<float4> & point2D, 
                                      const PixelRGBA<float4> & color, const PixelRGBA<float4> & colorBorder, 
                                      const float4 & fSize, const float4 & fBorderSize);

  /// Draw Axis.
  static void GLDrawAxis(const Point<uint2> & origin, const uint2 & uLineWidth, const uint2 & uLineLength);

  /// Draw Box.
  static void GLDrawBox( Box<float4> & box, const PixelRGBA<float4> & color );

  /// Draw a Rhombus.
  static void RenderUtil::GLDrawRhombus( const Point2D<float4> &center, const float4 &radius, const float4 & fLineWidth, const bool &bFill );

  /// Efficiently interpolate two uint1 values using fixed point math.
  static inline uint2 InterpolateFixedPoint( const uint1 & uLeft, const uint1 & uRight, const uint1 & uWeightRight );

  /// gets a matrix to convert from world to volume coordinates
  static Matrix<float4> GetWorldToVolumeMatrix( const vxVolumeHeader & header );

  /// is the finding visible
  static bool IsFindingVisible( const vxCAD::vxCadFinding & finding, const ColonSession::FindingInfoMap & cadFindInfo,
                                const int4 iCadSliderThreshold, const bool bShowExternal);

  /// gets the decision info for the finding
  static ColonSession::FindingInfo::DecisionEnum GetFindingDecision( const vxCAD::vxCadFinding & finding,
                                                                     const ColonSession::FindingInfoMap & cadFindInfo );

// Data Members
public:

  /// Segment colors. -- Nope, sorry Kev.  These pasty-bright colors only used in SliceViewerDBG :-) jrm
  static Triple<uint1> m_vTranslucentSliceViewColors[16];

}; // class RenderUtil


#include "RenderUtil.inl"


// $Log: RenderUtil.h,v $
// Revision 1.8.2.2  2010/02/01 13:40:09  dongqing
// add the GLDrawRhombus
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.8.2.1  2009/01/15 04:13:03  jmeade
// const modifier on parameter variable
//
// Revision 1.8  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.7  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.4  2006/03/28 20:22:14  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.4.2.3  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.4.2.2  2006/03/22 20:53:44  frank
// moved the finding visibility and other information into a class
//
// Revision 1.4.2.1  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.4  2005/09/29 18:30:34  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.3  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.2  2005/09/23 03:19:39  vxconfig
// working on display of cad findings
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.25.2.1  2003/04/22 18:18:52  jmeade
// Issue 3123: RenderHistogram override with (safer) std::vector<> for colormap data.
//
// Revision 3.25  2003/01/22 21:52:58  ingmar
// added GlVertex(const Vector4Df & vVector)
//
// Revision 3.24  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 3.23  2002/11/20 22:29:09  frank
// Added fixed-point interpolation
//
// Revision 3.22  2002/09/26 01:17:08  michael
// Added code to "invert" a color which sticks out if drawn on top of something with the original color
//
// Revision 3.21  2002/09/25 20:37:24  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 3.20  2002/09/23 14:58:11  michael
// cosmetic
//
// Revision 3.19  2002/09/19 14:25:25  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 3.18  2002/09/09 21:31:13  frank
// Cleaning out dead code.
//
// Revision 3.17  2002/07/31 18:33:27  ingmar
// added missing path to vxTransferFunction
//
// Revision 3.16  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 3.15  2002/06/04 18:37:11  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.14  2002/05/07 18:33:38  frank
// Removed unneccessary includes.
//
// Revision 3.13  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.12  2002/04/23 15:27:25  jenny
// switched to vxBlockVolume<int2>
//
// Revision 3.11  2002/04/23 15:02:39  jenny
// switched to vxBlockVolume<int2>
//
// Revision 3.10  2002/04/12 02:13:44  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.9  2002/04/08 13:55:30  manju
// Using TransferFunction instead of TransferFunctionVC in RenderHistogram.
// The function has been modified accordingly.
// Added a new method to draw profile for linear annotation and moved
// SetAgatston method here.
//
// Revision 3.8  2002/03/14 22:54:01  jaddonisio
// New functions:
//  i. GLDrawPoint
//  ii. GLDrawBox
// and placed DrawPolygon back in vxRenderTexMap
// because it had multitexture specific things.
//
// Revision 3.7  2002/03/13 20:03:28  jaddonisio
// New drawing utilities.
//
// Revision 3.6  2002/03/11 20:06:44  michael
// formatting
//
// Revision 3.5  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.4  2002/02/12 19:39:09  frank
// Added min/max histogram limits.
//
// Revision 3.3  2002/02/06 00:48:07  frank
// Improved the histogram drawing.
//
// Revision 3.2.2.3  2002/05/29 18:20:27  jmeade
// Enable alpha blending of RenderArrow color (though fn doesn't yet support this).
//
// Revision 3.2.2.2  2002/03/07 19:53:55  binli
// moved the global GL render function GLDrawCircle() from 10-math::Point2D to RenderUtil
//
// Revision 3.2.2.1  2002/02/12 23:20:18  frank
// Added a new histogram drawing function.
//
// Revision 3.2  2001/12/18 17:19:00  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.1  2001/11/13 22:15:58  manju
// Added color to RenderArrow.
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:22   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:50:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:18  ingmar
// new directory structure
//
// Revision 1.6  2001/08/31 18:53:42  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.5  2001/07/30 16:28:38  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.4  2001/06/27 18:51:18  geconomos
// no message
//
// Revision 1.3  2001/06/11 14:42:12  frank
// Added some more OpenGl helper functions.
//
// Revision 1.2  2001/05/29 17:27:00  jmeade
// slice viewer dbg colors
//
// Revision 1.1  2001/05/15 17:50:53  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/RenderUtil.h,v 1.8.2.2 2010/02/01 13:40:09 dongqing Exp $
// $Id: RenderUtil.h,v 1.8.2.2 2010/02/01 13:40:09 dongqing Exp $
