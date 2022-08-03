// $Id: SkeletonRender.h,v 1.2 2006/01/31 14:05:19 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)


// pragmas
#pragma once


// includes
#include "Skeleton.h"


// namespaces
namespace ReaderLib
{

  /**
   * Description: A class to support OpenGL rendering of the centerline skelton.
   */
  class SkeletonRender : public Skeleton
  {

  // Member Functions
  public:

    /// Render the skeleton, hiding it behind the rendered endoluminal depth image.
    void DrawWithHiddenLines(const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize,
                            const Point<float4> viewpoint, const Point<float4> obliquePoint, const bool bShowObliquePoint,
                            const Triple<uint1> & colorTriple);

    /// Render a single branch of the skeleton
    void DrawBranch(const Vector<float4> & vColorVector, const float4 fLineWidth);

    /// Render a portion of a single branch of the skeleton
    void DrawBranch(const Vector<float4> & colorVector, const float4 fLineWidth, uint4 start, uint4 stop);

    /// Render a single branch of the skeleton (in Verify-Picking mode)
    void DrawBranch(const Vector<float4> & vStartColor, const Vector<float4> & vEndColor, const float4 fLineWidth);
    
  }; // class SkeletonRender

} // namespace ReaderLib


// $Log: SkeletonRender.h,v $
// Revision 1.2  2006/01/31 14:05:19  frank
// code review
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.2  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.1  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:14   ingmar
// Initial revision.
// Revision 1.16  2001/06/14 19:50:11  binli
// show each segment(skeleton) with gradient (color: red->green)
//
// Revision 1.15  2001/06/12 12:57:55  frank
// Updates after code review.
//
// Revision 1.14  2001/06/11 13:40:04  frank
// Removed unused skeleton rendering parameter.
//
// Revision 1.13  2001/06/04 18:27:06  frank
// Removed dead code, beautified.
//
// Revision 1.12  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.11  2001/03/19 11:59:14  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.10  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.9  2001/01/16 19:07:38  binli
// the skeleton is rendered in arrows in verification
//
// Revision 1.8  2001/01/16 15:50:15  binli
// get a node in the skeleton (for Verification)
//
// Revision 1.7  2001/01/12 23:00:27  binli
// added a ball to represents the middle of skeleton (used in vertification).
//
// Revision 1.6  2001/01/11 21:16:34  frank
// Added color gradient rendering.
//
// Revision 1.5  2001/01/11 15:30:00  frank
// Added function to load checker texture into OpenGL.
//
// Revision 1.4  2001/01/09 18:23:28  frank
// Animated centerline.
//
// Revision 1.3  2001/01/09 16:50:49  frank
// Animated texture.
//
// Revision 1.2  2000/11/17 21:27:13  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 11    9/05/00 1:34a Ingmar
// now works with new naming
// 
// 10    8/28/00 12:37p Antonio
// updated header and footer
//
//*****************  Version 9  *****************
//User: Kevin           Date:  8/20/00  Time:  1:48p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed centerline coloring
//
//*****************  Version 8  *****************
//User: Kevin           Date:  8/07/00  Time:  6:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed CSI problems with new segment data structures
//
//*****************  Version 7  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/04/00  Time: 11:02a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Made the directional indicator be centerline color
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/02/00  Time: 11:47a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colors
//
//*****************  Version 4  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 3  *****************
//User: Frank           Date:  7/26/00  Time:  4:21p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MPR slicing along the centerline
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 1  *****************
//User: Frank           Date:  7/24/00  Time:  2:30p
//Created SkeletonRender.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SkeletonRender.h,v 1.2 2006/01/31 14:05:19 frank Exp $
// $Id: SkeletonRender.h,v 1.2 2006/01/31 14:05:19 frank Exp $
