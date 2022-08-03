// $Id: Quadrilateral.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachillle (frank@viatronix.com)

/**
 * define quadrilateral
 */

#ifndef Quadrilateral_h
#define Quadrilateral_h


// Includes
#include "Point.h"
#include "Normal.h"
#include "Vector.h"
#include "Matrix.h"
#include "Point2D.h"


/**
 * A quadrilateral object useful for exactly specifying an oblique view
 */
class VX_BASE_DLL Quadrilateral
{
// Enumerations
public:

  // point locations enumeration so that you can specify the top of quadrilateral on the screen
  enum PointLocationEnum
  {
    BOTTOM_LEFT  = 0,
    BOTTOM_RIGHT = 1,
    TOP_LEFT     = 2,
    TOP_RIGHT    = 3,
    MIDDLE       = 4
  };

// Member Functions
public:

  /// Defaul constructor
  Quadrilateral();

  /// Copy constructor
  Quadrilateral( const Quadrilateral & other );

  /// Convenience constructor from a center, size, and orientation
  Quadrilateral( const Point3Df & centerPoint, const Normal3Df & upNormal, const Normal3Df & rightNormal, const float4 fRadius );

  /// Convenience constructor from four points
  Quadrilateral( const Point3Df & topLeftPoint, const Point3Df & topRightPoint, const Point3Df & bottomLeftPoint, const Point3Df & bottomRightPoint );

  /// Assignment operator
  Quadrilateral & operator =( const Quadrilateral & other );

  /// Transform the points by a float matrix
  void Transform( const Matrix<float4> & transformationMatrix );

  /// Compute and return the area of the shape
  float4 GetArea() const;

  /// Compute the projected position in normalized coordinates { (0-1), (0-1) }
  Point2D<float4> GetProjectedPosition( const Point3Df & worldPos );

  /// Compute the projected position in world coordinates given normalized screen coordinates { (0-1), (0-1) }
  Point3Df GetBackProjectedPosition( const Point2D<float4> & normalizedPos );

  /// Retrieve the point data
  Point3Df GetPoint( const enum PointLocationEnum eLocation ) const;

  /// Set the point data
  void SetPoint( const enum PointLocationEnum eLocation, const Point3Df & point );

  /// Retrieve the plane normal
  Normal3Df GetNormal() const;

  /// Retrieve all the points at once
  const Point3Df * GetPoints() { return m_vertices; }

  /// Set all the points at once
  void SetPoints( const Point3Df vertices[4] ) { for (int4 i(0); i<4; ++i) m_vertices[i] = vertices[i]; }


// Data Members
private:

  /// The four corner points
  Point3Df m_vertices[4];

}; // class Quadrilateral

#endif

// $Log: Quadrilateral.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/05/23 13:01:56  frank
// Additional constructors
//
// Revision 3.13  2003/05/20 18:25:11  dongqing
// code review
//
// Revision 3.12  2003/05/20 17:51:10  frank
// code review
//
// Revision 3.11  2003/05/16 11:44:21  michael
// code review
//
// Revision 3.10  2003/03/04 17:58:53  frank
// Additional functionality
//
// Revision 3.9  2001/01/02 19:38:45  ingmar
// better documentation structure
//
// Revision 3.8  2002/12/16 23:46:32  frank
// Changed to be a vector instead of four individual points.
//
// Revision 3.7  2002/11/15 18:27:39  frank
// Added back projection function
//
// Revision 3.6  2002/11/12 16:12:40  frank
// Added projection function.
//
// Revision 3.5  2002/10/17 16:14:49  frank
// Removed Quadrilateral template parameter.
//
// Revision 3.4  2002/10/17 15:20:53  frank
// Moved code to inline.
//
// Revision 3.3  2002/09/25 20:43:42  ingmar
// replaced pow(x,2)  with x*x
//
// Revision 3.2  2002/01/02 21:20:09  ingmar
// moved to Sec_12_mathGFX
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:18   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:14   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.1  2001/08/27 12:32:04  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quadrilateral.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Quadrilateral.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
