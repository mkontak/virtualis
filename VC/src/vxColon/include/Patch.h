// $Id: Patch.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Patch.h
// Description: A class to store automatically detected areas (patches) which
//              have not been seen yet
// Created: October 28, 2000
// Author: Kevin Kreeger (kevin@viatronix.net)
//
// Complete History at bottom of file.


#if !defined(Patch_h)
#define Patch_h

// include declarations
#include "Point.h"
#include "Vector.h"



// class definition
class Patch
{
// Member functions
public:
  /// Constructor.
  Patch();

  /// Destructor.
  ~Patch();

  /// Copy Constructor.
  Patch(const Patch & other);

  /// Assignment operator.
  Patch & operator =(const Patch & other);

  /// Comparison function, for Descending sorted list of patches
  static int SurfaceCountGreaterCompare(const void * pVoidPatch1, const void * pVoidPatch2);

// Data members
public:
  
  /// Point along skel to view this patch from
  int4 m_iSegment;
  
  /// 
  uint2 m_uCSI;
  
  /// Distance along centerline
  float4 m_fDistanceFromRectumMM;

  /// World view point
  Point<float4> m_viewPointWorld;

  /// Best direction to view this patch
  Vector<float4> m_vViewDirWorld;

  /// Size of this patch
  uint4 m_uSurfaceCount;

  /// Center of Mass of this patch
  Point<float4> m_centerWorld;
};

#endif // !defined(Patch_h)


// $Log: Patch.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.4.24.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.4  2002/04/12 14:48:42  jmeade
// Changed sort function to sort in Descending order.
//
// Revision 3.3  2002/04/12 02:13:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 3.1  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.9  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.8  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.7  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.6  2001/01/11 23:46:13  kevin
// Added navigation based missed patch viewing
//
// Revision 1.5  2001/01/08 02:42:50  kevin
// Cleaned up missed patch viewing by multiple methods
//
// Revision 1.4  2000/12/28 19:49:35  kevin
// Added new viewppoint and viewdir to see whcihc is better
//
// Revision 1.3  2000/11/25 03:04:43  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.2  2000/11/16 17:38:58  kevin
// Added distance from Rectum to Patch structure
//
// Revision 1.1  2000/10/29 17:37:03  kevin
// Added Patch calss which holds a detected missed region of
// colon surface
//
