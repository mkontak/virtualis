// $Id: Intersection.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Detect intersections between lines, planes, rays, etc.
 */

#ifndef Intersection_h
#define Intersection_h



// Forward declarations
class Line;
class Plane;


/**
 * Detect intersections between lines, planes, rays, etc.
 */
namespace Intersection
{

  /// Compute the intersection of a line with a plane.
  VX_BASE_DLL bool ComputeIntersection( const Line & line, const Plane & plane, Point3Df & intersection );

  /// Get the intersection of two planes
  VX_BASE_DLL bool ComputeIntersection( const Plane & plane1, const Plane & plane2, Line & line );

}; // namespace Intersection


#endif // Intersection_h


// $Log: Intersection.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/08/31 03:01:53  frank
// Added plane-plane intersection - testing to follow
//
// Revision 1.4  2003/05/20 17:12:29  dongqing
// code review
//
// Revision 1.3  2003/05/20 15:44:24  frank
// code review
//
// Revision 1.2  2003/01/24 18:19:20  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.1  2003/01/21 20:38:04  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Intersection.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Intersection.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
