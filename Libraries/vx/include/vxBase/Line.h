// $Id: Line.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * define line
 */

#ifndef Line_h
#define Line_h


// Includes
#include "Point3D.h"
#include "Normal3D.h"


// Forward declarations
class Plane;


/**
 * A line class
 */
class VX_BASE_DLL Line 
{
// Member Functions
public:

  /// Constructor taking a point and a direction
  Line( const Point3Df & point, const Normal3Df & direction );

  /// Constructor taking two points
  Line( const Point3Df & pointOne, const Point3Df & pointTwo );

  /// Compute the point of nearest approach with another (potentially skew) line.
  Point3Df GetIntersection( const Line & other );

  /// Return a point on the line.
  const Point3Df & GetPoint() const { return m_point; }

  /// Return the directions of the line; could be either direction.
  const Normal3Df & GetDirection() const { return m_direction; }

  /// Compute the distance of a point from this line.
  float4 ComputeDistance( const Point3Df & point3D ) const;

// Data Members
private:

  /// One (of infinetely many) points on the line
  Point3Df m_point;

  /// The direction of the line
  Normal3Df m_direction;

}; // class Line

#endif // Line_h

// $Log: Line.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/20 16:13:51  dongqing
// code review
//
// Revision 1.5  2003/05/14 20:40:38  frank
// code review
//
// Revision 1.4  2003/01/24 18:20:45  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.3  2003/01/21 22:09:48  frank
// Added another constructor and a compute distance function.
//
// Revision 1.2  2001/01/02 19:37:43  ingmar
// better documentation structure
//
// Revision 1.1  2002/11/21 19:26:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Line.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Line.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
