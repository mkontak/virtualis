// $Id: vxMathUtils.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMathUtils.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/*
  This class should rather be methods within the 10_math objects 
  but for now, I put this here as helpe functions. It should be 
  moved later, once we start to rework 10_math ...
*/

#ifndef vxMathUtils_h
#define vxMathUtils_h


// includes
#include "Point.h"


// forward declarations
template <class T> class Box;
template <class T> class vxPlaneOblique;
class vxCamera;


// class definition
class VX_VIEWER_DLL vxMathUtils
{
// functions
public:

  /// default constructor
  vxMathUtils() {};

  /// destructor
  ~vxMathUtils() {};

  /// computes the minimum distance for the given plane so that it is still within the box
  static float4 GetMinDistance(const Box<float4> & box, const vxPlaneOblique<float4> & plane);

  /// computes the index of the minimum distance for the given plane so that it is still within the box
  static uint2 GetMinDistanceCornerIndex(const Box<float4> & box, const vxPlaneOblique<float4> & plane);

  /// computes the minimum distance for the given plane so that it is still within the box
  static float4 GetMaxDistance(const Box<float4> & box, const vxPlaneOblique<float4> & plane);

  /// computes the index of the maximum distance for the given plane so that it is still within the box
  static uint2 GetMaxDistanceCornerIndex(const Box<float4> & box, const vxPlaneOblique<float4> & plane);

}; // vxMathUtils


#include "vxMathUtils.inl"


#endif // vxMathUtils_h


// Revision History:
// $Log: vxMathUtils.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/09/23 14:44:53  michael
// removed utility functions to computer Euler Angles for Shimadzu. Is now part
// of the V3D Angio plugin code.
//
// Revision 1.3  2003/08/29 16:41:44  michael
// adjusted code to reflect discussion with Takeshi Ozaki (Shimadzu)
//
// Revision 1.2  2003/08/07 14:57:44  michael
// added general Euler angle function
//
// Revision 1.1  2003/08/07 14:22:35  michael
// moved vxMathUtils from 70_vxUtils to 12_mathGFX were it belongs to
//
// Revision 1.9  2003/05/16 14:56:27  michael
// code review
//
// Revision 1.8  2003/05/16 13:58:18  michael
// code review
//
// Revision 1.7  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.6  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.5  2003/01/22 21:45:30  ingmar
// added missing includes
//
// Revision 1.4  2002/11/21 22:52:19  michael
// added corner helper methods
//
// Revision 1.3  2002/11/20 21:14:35  michael
// added a few more helper methods
//
// Revision 1.2  2002/11/20 18:18:27  michael
// added compilable version(s)
//
// Revision 1.1  2002/11/20 14:32:24  michael
// added inital version of math utilities to find intersections of objects, etc.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMathUtils.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxMathUtils.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
