// $Id: vxTetrahedron.C,v 1.1 2004/04/06 18:18:16 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxTetrahedron.h"
#include "Typedef.h"
#include "Matrix.h"



// Defines
#define FILE_REVISION "$Revision: 1.5"


/**
 * Constructor
 *
 * @param v1 vertex 1
 * @param v2 vertex 2
 * @param v3 vertex 3
 * @param v4 vertex 4
 */
vxTetrahedron::vxTetrahedron( const Point<float4> & v1, const Point<float4> & v2, const Point<float4> & v3, const Point<float4> & v4 )
{
  m_vertices[0] = v1;
  m_vertices[1] = v2;
  m_vertices[2] = v3;
  m_vertices[3] = v4;
} // vxTetrahedron()


/**
 * Compute the volume of the tetrahedron using a complicated formula
 *
 * @return the volume
 */
float4 vxTetrahedron::GetVolumeMethodOne()
{
  // The volume can be computed for a general tetrahedron using the formula found
  // at http://mathworld.wolfram.com/Tetrahedron.html.
  //
  //          | 0   1     1     1     1   |
  // 288V^2 = | 1   0   d12^2 d13^2 d14^2 |
  //          | 1 d21^2   0   d23^2 d24^2 |
  //          | 1 d31^2 d32^2   0   d34^2 |
  //          | 1 d41^2 d42^2 d43^2   0   |
  //
  // to simplify,
  //
  // D12 = d12^2 = d21^2
  // D13 = d13^2 = d31^2
  // D14 = d14^2 = d41^2
  // D23 = d23^2 = d32^2
  // D24 = d24^2 = d42^2
  // D34 = d34^2 = d43^2

  const float4 fD12( ( m_vertices[0] - m_vertices[1] ).GetLengthSquared() );
  const float4 fD13( ( m_vertices[0] - m_vertices[2] ).GetLengthSquared() );
  const float4 fD14( ( m_vertices[0] - m_vertices[3] ).GetLengthSquared() );
  const float4 fD23( ( m_vertices[1] - m_vertices[2] ).GetLengthSquared() );
  const float4 fD24( ( m_vertices[1] - m_vertices[3] ).GetLengthSquared() );
  const float4 fD34( ( m_vertices[2] - m_vertices[3] ).GetLengthSquared() );

  // 
  // so we get,
  //
  // 288V^2 = | 0  1   1   1   1  |
  //          | 1  0  D12 D13 D14 |
  //          | 1 D12  0  D23 D24 |
  //          | 1 D13 D23  0  D34 |
  //          | 1 D14 D24 D34  0  |
  //
  
  Matrix<float8> mat1( 1, fD12, fD13, fD14,
                       1, 0   , fD23, fD24,
                       1, fD23, 0   , fD34,
                       1, fD24, fD34, 0    );

  Matrix<float8> mat2( 1, 0   , fD13, fD14,
                       1, fD12, fD23, fD24,
                       1, fD13, 0   , fD34,
                       1, fD14, fD34, 0    );

  Matrix<float8> mat3( 1, 0   , fD12, fD14,
                       1, fD12, 0   , fD24,
                       1, fD13, fD23, fD34,
                       1, fD14, fD24, 0    );

  Matrix<float8> mat4( 1, 0   , fD12, fD13,
                       1, fD12, 0   , fD23,
                       1, fD13, fD23, 0   ,
                       1, fD14, fD24, fD34 );

  const float8 fRightSideSum( 0.0F - mat1.GetDeterminant() + mat2.GetDeterminant() - mat3.GetDeterminant() + mat4.GetDeterminant() );

  return sqrt( abs( fRightSideSum ) / 288.0F );

} // GetVolumeMethodOne()


/**
 * Compute the volume of the tetrahedron using a simple formula
 *
 * @return the volume
 */
float4 vxTetrahedron::GetVolumeMethodTwo()
{
  // The volume can be computed for a general tetrahedron using the formula found
  // at http://mathworld.wolfram.com/Tetrahedron.html.
  //
  // V = | a dot ( b cross c ) | / 3!
  //
  // where
  //
  // a, b, and c are three edge vectors from any vertex
  //

  const Vector3Df a( m_vertices[1] - m_vertices[0] );
  const Vector3Df b( m_vertices[2] - m_vertices[0] );
  const Vector3Df c( m_vertices[3] - m_vertices[0] );

  return fabs( a.Dot( b.Cross( c ) ) ) / 6;

} // GetVolumeMethodTwo()


/**
 * Compute the volume of the tetrahedron.
 *
 * @return the volume
 */
float4 vxTetrahedron::GetVolume()
{
  return GetVolumeMethodTwo();
} // GetVolume()


#undef FILE_REVISION


// $Log: vxTetrahedron.C,v $
// Revision 1.1  2004/04/06 18:18:16  frank
// moved tetrahedron into vxBase
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/21 20:07:49  dongqing
// code review
//
// Revision 1.5  2003/05/21 14:20:44  frank
// code review
//
// Revision 1.4  2003/02/26 21:39:01  frank
// Simplified the Volume calculation.
// Improved the unit test.
//
// Revision 1.3  2003/02/20 21:28:41  frank
// Replaced volume calculation with proper formula.
//
// Revision 1.2  2003/01/22 21:14:24  ingmar
// added missing include
//
// Revision 1.1  2002/11/20 17:21:36  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTetrahedron.C,v 1.1 2004/04/06 18:18:16 frank Exp $
// $Id: vxTetrahedron.C,v 1.1 2004/04/06 18:18:16 frank Exp $