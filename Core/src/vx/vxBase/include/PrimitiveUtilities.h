// $Id: PrimitiveUtilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * A set of utilities for mathematical primitives
 */

#ifndef PrimitiveUtilities_h
#define PrimitiveUtilities_h


// Includes
#include "Vector3D.h"
#include "Normal3D.h"


// Forwards
class Plane;
class Line;
class RaySegment;


namespace PrimitiveUtilities
{

  /// Generate a point on a plane.  It could be any point, not necessarily random.
  VX_BASE_DLL Point3D<float> GetPointOnPlane( const Plane & plane );

  /// Generate three points on a plane.  They could be any three points, not necessarily random.
  VX_BASE_DLL std::vector< Point3D<float4> > GetThreePointsOnPlane( const Plane & plane );

  /// Compute the point on the line nearest to the given point
  VX_BASE_DLL Point3D<float4> ProjectPointOntoLine( const Line & line, const Point3D<float4> & point );

  /// Get the percent distance along a ray segment to which a given point projects.
  VX_BASE_DLL float4 GetPercentAlongSegment( const RaySegment & segment, const Point3D<float4> & position );

  /// Projects a vector so that it becomes parallel to the plane.
  VX_BASE_DLL Vector3Df GetProjectionParallelToPlane( const Vector3Df & vector3D, const Normal3Df & normalToPlane );

  /// Returns a new plane parallel to the original one, but offset by a distance
  VX_BASE_DLL Plane GetOffsetPlane( const Plane & originalPlane, const float4 & fOffset );

}; // namespace PrimitiveUtilities


#endif // PrimitiveUtilities_h


// $Log: PrimitiveUtilities.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/23 12:35:32  frank
// Moved utility functions to PrimitiveUtilities
//
// Revision 1.5  2003/05/20 17:12:30  dongqing
// code review
//
// Revision 1.4  2003/05/20 16:25:18  frank
// cleaned up log
//
// Revision 1.3  2003/02/27 22:32:42  frank
// Added point-line projection and percent distance routines
//
// Revision 1.2  2003/01/24 18:20:30  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.1  2003/01/24 15:18:35  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PrimitiveUtilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: PrimitiveUtilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
