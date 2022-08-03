// $Id: vxRendererUtils.inl,v 1.2 2006/09/20 20:34:43 frank Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Draw a point
 * @param left-hand value
 */
inline void vxRendererUtils::GlVertex(const Point<float4> & vPoint)
{
  glVertex3fv(&(vPoint.m_x));
} // GlVertex


/**
 * Invert color
 * @param color
 * @return inverted color
 */
inline PixelRGBA<float4> vxRendererUtils::InvertColor(const PixelRGBA<float4> & color)
{
  return PixelRGBA<float4>(1.0, 1.0F-0.5*color.G(), 1.0F-0.5*color.B(), color.A());
} // InvertColor


/**
 * Efficiently interpolate two uint1 values using fixed point math (Jim Blinn, 3 wrong make a right)
 * @param left-hand value
 * @param right-hand value
 * @param weight of the right-hand value (0 corresponds to all left, 255 corresponds to all right)
 */
inline uint2 vxRendererUtils::InterpolateFixedPoint(const uint1 & uLeft, 
                                                    const uint1 & uRight, 
                                                    const uint1 & uWeightRight)
{
  const uint2 uWeightLeft(255 - uWeightRight);
  const uint2 uTmp1(128 + uWeightLeft * uLeft);
  const uint2 uTmp2(128 + uWeightRight * uRight);
  const uint2 uRes1((uTmp1 + (uTmp1 >> 8)) >> 8);
  const uint2 uRes2((uTmp2 + (uTmp2 >> 8)) >> 8);

  return uRes1 + uRes2;
} // InterpolateFixedPoint()


// undefines
#undef FILE_REVISION


// $Log: vxRendererUtils.inl,v $
// Revision 1.2  2006/09/20 20:34:43  frank
// Issue #4963: Added a utility function to display a small orange box on the view to indicate that the image is not fully rendered
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.4  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.3  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.2  2003/05/14 17:28:25  michael
// code review
//
// Revision 1.1  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.17  2002/11/15 18:10:15  frank
// Removed unused parameter (iOpacity)
//
// Revision 1.16  2002/08/22 13:50:38  michael
// fixed cpy/past comment in doxygen text
//
// Revision 1.15  2002/08/21 18:30:40  michael
// extended utility method for drawing a rectangle
//
// Revision 1.14  2002/08/07 14:37:07  michael
// Added utility to draw a rectangle in OpenGL
//
// Revision 1.13  2002/08/02 14:51:37  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.12  2002/07/30 17:17:11  michael
// Removed redundant methods
//
// Revision 1.11  2002/07/29 22:38:25  michael
// Fixed 32 bit encoding and evaluation whether a block is visible. There were
// several thigns wrong. only 31 bits were used and the comparison of the
// logical AND was compared with >0 butneeds to be !=0
//
// Revision 1.10  2002/07/28 14:43:04  michael
// fixed bug: Ingmar moved from 16 to 32 bins but forgot to the NUMBER_OF_BINS to 32, too.
//
// Revision 1.9  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.8  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.7  2002/07/18 15:22:49  ingmar
// changed bins from 16 to 32
//
// Revision 1.6  2002/06/27 19:55:39  soeren
// fixed GenerateBitCodeFor12BitTransferFunction
//
// Revision 1.5.2.8  2002/07/18 14:57:13  jaddonisio
// Typecast initialization of const iBinSize to int4. This avoids a serious conversion problem on my machine.
//
// Revision 1.5.2.7  2002/07/17 20:49:09  frank
// Made block visibility more convenient to use.
//
// Revision 1.5.2.6  2002/07/17 17:06:02  frank
// Added utility class to manage block visibility complexity.
//
// Revision 1.5.2.5  2002/07/09 20:08:37  michael
// cosmetics
//
// Revision 1.5.2.4  2002/07/08 15:22:53  frank
// Added label visible utility function.
//
// Revision 1.5.2.3  2002/07/02 22:24:20  michael
// fixed bug in bit encoding for intervals
//
// Revision 1.5.2.2  2002/07/02 13:47:50  michael
// added 32 bit encoding method for interval and fixed 12 bit encoding code
//
// Revision 1.5.2.1  2002/07/01 21:23:15  frank
// Added utility function to traverse histograms.
//
// Revision 1.5  2002/06/27 16:02:21  soeren
// made parameter const
//
// Revision 1.4  2002/06/27 15:42:54  soeren
// added GenerateBitCodeFor12BitTransferFunction
//
// Revision 1.3  2002/06/27 14:07:32  michael
// using 16 bins for transfer function and 31 bins for labels and moved helper
// methods to vxRendererUtils
//
// Revision 1.2  2002/03/20 19:46:02  michael
// moved OpenGl back-projection stuff to 74_vxUtils
//
// Revision 1.1  2002/03/20 19:19:16  michael
// extended vxRendererUtils to perform the OpenGL backprojection
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererUtils.inl,v 1.2 2006/09/20 20:34:43 frank Exp $
// $Id: vxRendererUtils.inl,v 1.2 2006/09/20 20:34:43 frank Exp $
