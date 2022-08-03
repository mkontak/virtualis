// $Id: vxTetrahedron.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


#ifndef vxTetrahedron_h
#define vxTetrahedron_h


// Includes
#include "Typedef.h"
#include "Point.h"


/**
 * A generalized tetrahedron.
 * It is primarily used to compute volumes of general polyhedra.
 */
class VX_VIEWER_DLL vxTetrahedron
{
// Member Functions
public:

  /// Constructor
  vxTetrahedron( const Point<float4> & v1, const Point<float4> & v2, const Point<float4> & v3, const Point<float4> & v4 );

  /// Compute the volume of the tetrahedron
  float4 GetVolume();

private:

  /// Compute the volume of the tetrahedron using a complicated formula
  float4 GetVolumeMethodOne();

  /// Compute the volume of the tetrahedron using a simple formula
  float4 GetVolumeMethodTwo();

// Data Members
private:

  /// The four corner vertices
  Point<float4> m_vertices[4];

}; // class vxTetrahedron

#endif // vxTetrahedron_h

// $Log: vxTetrahedron.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/21 14:20:44  frank
// code review
//
// Revision 1.5  2003/02/26 21:39:01  frank
// Simplified the Volume calculation.
// Improved the unit test.
//
// Revision 1.4  2003/01/02 19:39:45  ingmar
// better documentation structure
//
// Revision 1.3  2002/11/21 19:29:13  frank
// Derived from documentation class
//
// Revision 1.2  2002/11/20 17:21:36  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTetrahedron.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxTetrahedron.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $