// $Id: Plane.h,v 1.2 2005/08/05 13:03:50 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef Plane_h
#define Plane_h


// Includes
#include "Triple.h"
#include "Point.h"
#include "Vector.h"
#include "Normal3D.h"


/*
 * Contains a mathematical description of a plane using the equation Ax + By + Cz + D = 0.
 */
class Plane
{
// Member Functions
public:

  /// Default constructor
  inline Plane();

  /// Constructor from a point and a normal
  inline Plane( const Point3Df & pointInPlane, const Normal3Df & normalToPlane );

  /// Constructor from three points
  inline Plane( const Point3Df & point1, const Point3Df & point2, const Point3Df & point3 );

  /// Constructor from four scalars
  inline Plane( const float4 & fCoefficientA, const float4 & fCoefficientB,
                const float4 & fCoefficientC, const float4 & fCoefficientD );
  
  /// Copy constructor from another plane
  inline Plane( const Plane & other );

  /// Assignment Operator from another plane
  inline Plane & operator=( const Plane & other );

  /// Returns whether or not the point is on the positive side of the plane
  inline float4 GetDistance( const Point3Df & point ) const;
  
  /// Returns whether or not the point is on the positive side of the plane
  inline bool IsOnPositiveSide( const Point3Df & point ) const;

  /// Projects a vector so that it becomes parallel to the plane
  inline Vector3Df GetProjectionParallelToPlane( const Vector3Df & inVector ) const;

  /// Projects a point so that it lies within the plane
  inline Point3Df GetProjectionOntoPlane( const Point3Df & inPoint ) const;

  /// Retrieve the plane normal
  inline Normal3Df GetNormal() const { return Normal3Df( m_fA, m_fB, m_fC ); }

  /// Returns the first coefficient of the plane
  inline float4 GetA() const;

  /// Returns the second coefficient of the plane
  inline float4 GetB() const;

  /// Returns the third coefficient of the plane
  inline float4 GetC() const;

  /// Returns the fourth coefficient of the plane
  inline float4 GetD() const;

  /// Returns a point on the plane
  inline Point3Df GetPointOnPlane() const;

  /// Overloaded << operator
  friend std::ostream & operator<<( std::ostream & os, const Plane & plane );

  /// Overloaded >> operator
  friend std::istream & operator>>( std::istream & is, Plane & plane );

private:

  /// Normalize the coefficients
  void NormalizeCoefficients();

// Data Members
private:

  /// The plane coefficient A.
  float4 m_fA;

  /// The plane coefficient B.
  float4 m_fB;

  /// The plane coefficient C.
  float4 m_fC;

  /// The plane coefficient D.
  float4 m_fD;

}; // class Plane

/// Projects a vector so that it becomes parallel to the plane.
inline Vector3Df GetProjectionParallelToPlane( const Vector3Df & vector3D, const Normal3Df & normalToPlane );


#include "Plane.inl"


#endif // Plane_h


// $Log: Plane.h,v $
// Revision 1.2  2005/08/05 13:03:50  geconomos
// added GetProjectionParallelToPlane()
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/08/31 03:53:22  frank
// Added constructor from three points
//
// Revision 3.13  2003/05/23 12:35:31  frank
// Moved utility functions to PrimitiveUtilities
//
// Revision 3.12  2003/05/14 19:33:58  frank
// code review
//
// Revision 3.11  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 3.10  2003/01/24 18:19:01  frank
// Removed Plane template parameter
//
// Revision 3.9  2003/01/22 20:56:56  ingmar
// code beautify and removal of unused code
//
// Revision 3.8  2001/01/02 19:38:33  ingmar
// better documentation structure
//
// Revision 3.7  2001/01/02 18:52:08  ingmar
// better documentation structure
//
// Revision 3.6  2001/01/02 18:00:11  ingmar
// added independent function GetProjectionParallelToPlane()
//
// Revision 3.5  2001/01/02 16:16:14  ingmar
// added independent function GetProjectionParallelToPlane()
//
// Revision 3.4  2002/11/21 19:26:32  frank
// added point projection
//
// Revision 3.3  2002/06/21 16:18:17  frank
// Normalized the coefficients.
//
// Revision 3.2.2.1  2002/05/28 20:25:04  frank
// Normalized the coefficients.
//
// Revision 3.2  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:14   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:10   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.6  2001/08/20 14:39:48  frank
// Added vector projection onto the plane.
//
// Revision 1.5  2001/05/31 21:32:36  ana
// changes regarding code review
//
// Revision 1.4  2001/05/30 20:29:43  ana
// changed the members access function
//
// Revision 1.3  2001/05/29 18:38:53  ana
// added head comments
//
// Revision 1.2  2001/05/25 20:52:34  ana
// Plane.C added and TestByTyple method created
//
// Revision 1.1  2001/04/30 14:38:17  frank
// New plane class.
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Plane.h,v 1.2 2005/08/05 13:03:50 geconomos Exp $
// $Id: Plane.h,v 1.2 2005/08/05 13:03:50 geconomos Exp $
