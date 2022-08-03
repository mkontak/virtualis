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



// include declarations
#include "stdafx.h"
#include "CubicSplineInterpolator.h"



// namespace declarations
using namespace std;



/** Constructor
 *
 *  @return N/A
 */
CubicSplineInterpolator::CubicSplineInterpolator()
{
  m_bSplineComputed = false;
  m_yBndryCond1 = (float4) 1e30;
  m_yBndryCondN = (float4) 1e30;
  m_vX  = NULL;
  m_vY  = NULL;
  m_vY2 = NULL;
} // CubicSplineInterpolator


/** Destructor
 *
 *  @return N/A
 */
CubicSplineInterpolator::~CubicSplineInterpolator()
{
  // Clean-up:
  delete[] m_vX;
  delete[] m_vY;
  delete[] m_vY2;
} // ~CubicSplineInterpolator


/** Pt - Returns the interpolated point for a given input.
 *
 *  @return float4
 *  @param x  input x.
 */
float4 CubicSplineInterpolator::Pt( const float4 x )
{
  float4 fIntrPt = 0.0;

  if ( m_bSplineComputed )
  {
    Splint( m_vX, m_vY, m_vY2, m_uNumPts, x, &fIntrPt );
  }

  return( fIntrPt );

} // Pt


/** Compute - Computes the cubic spline.
 *
 *  @return void
 *  @param vPts vector of (x,y)input vector of x's.
 */
void CubicSplineInterpolator::Compute( const vector< Point2D<float4> > & vPts )
{
  int4  i = 0;

  // Initialize paramters:

  m_bSplineComputed = false;

  m_uNumPts = static_cast< uint4 >( vPts.size() );

  if ( m_vX  != NULL ) delete [] m_vX;
  if ( m_vY  != NULL ) delete [] m_vY;
  if ( m_vY2 != NULL ) delete [] m_vY2;
  m_vX  = new float4[m_uNumPts+1];
  m_vY  = new float4[m_uNumPts+1];
  m_vY2 = new float4[m_uNumPts+1];

  // Set-up vectors of tabulated points(required for numerical recipy routines).

  vector< Point2D<float4> >::const_iterator vecFloat4Pt2D_iter;

  for ( i = 1, vecFloat4Pt2D_iter = vPts.begin() ; vecFloat4Pt2D_iter != vPts.end() ; vecFloat4Pt2D_iter++  )
  {
    m_vX[i] = vPts[i-1].m_x;
    m_vY[i] = vPts[i-1].m_y;
    ++i;
  }

  // Call Spline routine - Numerical Recipy:
  Spline( m_vX, m_vY, m_uNumPts, m_yBndryCond1, m_yBndryCondN, m_vY2 );

  m_bSplineComputed = true;

} // Compute


/** Spline - Numerical Recipy
 *  Given arrays x[1..n] and y[1..n] containing a tabulated function, i.e., yi = f(xi), with
 *  x1<x2< :::<xN , and given values yp1 and ypn for the first derivative of the interpolating
 *  function at points 1 and n, respectively, this routine returns an array y2[1..n] that contains
 *  the second derivatives of the interpolating function at the tabulated points xi. If yp1 and/or
 *  ypn are equal to 1 . 10 30 or larger, the routine is signaled to set the corresponding boundary
 *  condition for a natural spline, with zero second derivative on that boundary.
 *
 *  @return void
 *  @param x[]  input vector of x's.
 *  @param y[]  input vector of y's associated with the x's.
 *  @param n    number of (x,y) data points.
 *  @param yp1  2nd derivative specifier.
 *  @param ypn  2nd derivative specifier.
 *  @param y2[] 2nd derivatives of the interpolating function at the tabulated x's.
 */
void CubicSplineInterpolator::Spline( float4 x[], float4 y[], int4 n, float4 yp1, float4 ypn, float4 y2[] )
{
  int4   i,k;
  float4 p,qn,sig,un,*u;
  
  u = AllocVector(1,n-1);

  if ( yp1 > 0.99e30 )  // The lower boundary condition is set either to be "natural"
    y2[1] = u[1] = 0.0;
  else {                // or else to have a specified first derivative.
    y2[1] = -0.5;
    u[1]  = ((float4)3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
  }

  for ( i = 2; i <= n-1; i++ ) { // This is the decomposition loop of the tridiagonal algorithm.
                                 // y2 and u are used for temporary storage of the decomposed factors.
    sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p   = sig*y2[i-1] + (float4)2.0;
    y2[i] = (sig - (float4)1.0)/p;
    u[i]  = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i]  = ((float4)6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }

  if ( ypn > 0.99e30 ) // The upper boundary condition is set either to be "natural"
    qn = un = 0.0;
  else { // or else to have a specified first derivative.
    qn = 0.5;
    un = ((float4)3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
  }
  y2[n] = (un - qn*u[n-1])/(qn*y2[n-1] + (float4)1.0);

  for ( k = n-1; k >= 1; k-- ) // This is the back substitution loop of the tridiagonal algorithm.
    y2[k] = y2[k]*y2[k+1]+u[k];
  
  FreeVector(u,1,n-1);

} // Spline( float4 x[], float4 y[], int4 n, float4 yp1, float4 ypn, float4 y2[] )


/** Splint - Numerical Recipy
 *  Given the arrays xa[1..n] and ya[1..n], which tabulate a function (with the xai's in order),
 *  and given the array y2a[1..n], which is the output from spline above, and given a value of
 *  x, this routine returns a cubic-spline interpolated value y.
 *
 *  @return void
 *  @param   xa[]  input vector of x's.
 *  @param   ya[]  input vector of y's associated with the x's.
 *  @param   y2a[] input vector of 2nd derivatives from Spline routine.
 *  @param   n     number of (x,y) data points.
 *  @param   x     given value of x.
 *  @param * y     interpolated point at x.
 */

void CubicSplineInterpolator::Splint( float4 xa[], float4 ya[], float4 y2a[], int4 n, float4 x, float4 *y )
{
  //void  nrerror( char error_text[] );
#define nrerror(x) printf( "\n%s", x )
  //throw ex::InvalidDataException(vxLogRecord(vxLogRecord::SL_EXCEPTION, Date::GetCurrentDate(), /
  // __FILE__, "$Revision: 1.2 $", __LINE__, &typeid(SVD), "CubicSplineInterpolator",UNDEFINED,"invalid data"))
 
  int   klo,khi,k;
  float h,b,a;
  klo = 1;// We will find the right place in the table by means of
          // bisection. This is optimal if sequential calls to this
          // routine are at random values of x. If sequential calls
          // are in order, and closely spaced, one would do better
          // to store previous values of klo and khi and test if
          // they remain appropriate on the next call.
  khi = n;
  
  while ( khi-klo > 1 )
  {
    k = (khi+klo) >> 1;
    if ( xa[k] > x ) khi = k;
    else klo = k;
  } // xa[klo] and xa[khi] now bracket the input value of x.
  
  h = xa[khi]-xa[klo];
  if ( h == 0.0 ) nrerror("Bad xa input to routine splint"); // The xa's must be distinct.
  
  a = (xa[khi]-x)/h;
  b = (x-xa[klo])/h; // Cubic spline polynomial is now evaluated.
  
  //   2*-1        -1*1          0             0      0
  *y = a*ya[klo] + b*ya[khi] + ((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/(float4)6.0;

  //*y = xa[khi];
} // Splint( float4 xa[], float4 ya[], float4 y2a[], int4 n, float4 x, float4 *y )


/** AllocVector - allocates a float vector.
 *
 *  @return void
 *  @param iS1  subscript #1.
 *  @param iS2  subscript #2.
 */

float4 * CubicSplineInterpolator::AllocVector( int8 iS1, int8 iS2 )
{
  float4 * pVector = NULL;

  pVector = (float4 *) new float4[ (int4)(iS2 - iS1 + 2) ];

  if ( pVector == NULL )
  {
    //throw ex::OutOfMemoryException("SVD: no memory space");
  }

  return( pVector - iS1 + 1 );

} // AllocVector


/** FreeVector - free a float vector allocated with AllocVector()
 *
 *  @return void
 *  @param *pV
 *  @param iS1  subscript #1.
 *  @param iS2  subscript #2.
 */
void CubicSplineInterpolator::FreeVector( float4 *pVector, int8 iS1, int8 iS2 )
{
  delete [] ( pVector - iS1 + 1 );
} // FreeVector



// $Log: CubicSplineInterpolator.C,v $
// Revision 1.2  2005/07/01 12:30:07  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:43:45  michael
// code review
//
// Revision 1.3  2003/05/14 14:06:34  dongqing
// code review
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/CubicSplineInterpolator.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
// $Id: CubicSplineInterpolator.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
