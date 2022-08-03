//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Joseph Addonisio, jaddonisio@viatronix.com
//
// Complete history at bottom of file.

/**
 *  This class implements the Cubic Spline Interpolation
 */


#ifndef CubicSplineInterpolator_h
#define CubicSplineInterpolator_h



// include declarations
#include "Point2D.h"
#include <vector>



// class definition
class CubicSplineInterpolator
{
public:

  /// Constructor
  CubicSplineInterpolator();

  /// Destructor
  virtual ~CubicSplineInterpolator();

public: // Member Functions...

  /// compute
  void Compute( const std::vector< Point2D<float4> > & vPts );

  ///
  float4 Pt( const float4 x );

private: // Member Functions...

  /// Spline routine - reference "Numerical Recipes in C":
  void Spline( float4 x[], float4 y[], int4 n, float4 yp1, float4 ypn, float4 y2[] );

  /// Splint routine - reference "Numerical Recipes in C":
  void Splint( float4 xa[], float4 ya[], float4 y2a[], int4 n, float4 x, float4 *y );

  /// Allocate a vector of floats:
  float4 * AllocVector( int8 iS1, int8 iS2 );

  /// Free a vector of floats, allocated by AllocVector:
  void FreeVector( float4 *pVector, int8 iS1, int8 iS2 );

private: // Member Variables...

  bool     m_bSplineComputed;
  uint4    m_uNumPts;
  float4 * m_vX;
  float4 * m_vY;
  float4 * m_vY2;
  float4   m_yBndryCond1;
  float4   m_yBndryCondN;
};



#endif // #ifndef CubicSplineInterpolator_h



// $Log: CubicSplineInterpolator.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:43:45  michael
// code review
//
// Revision 1.6  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 1.5  2003/01/02 19:37:31  ingmar
// better documentation structure
//
// Revision 1.4  2001/01/02 18:51:33  ingmar
// better documentation structure
//
// Revision 1.3  2002/03/11 20:01:14  michael
// compiles again
//
// Revision 1.2  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//