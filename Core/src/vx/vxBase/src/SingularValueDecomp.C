/// $Id: SingularValueDecomp.C,v 1.1 2006/10/11 20:36:39 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/vxBase/src/SingularValueDecomp.C,v 1.1 2006/10/11 20:36:39 dongqing Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: J. Addonisio
//
// Complete history at bottom of file.


/** 
 *  Description: This defines the Singular Value Decomposition(SVD) class.
 *               SVD is defined as
 *                      ( A ) = ( U ) ( W ) ( V' )
 *               where
 *                      A - An M x N matrix
 *                      U - M x N column-orthogonal matrix
 *                      W - N x N diagonal matrix with singular values (decending order)
 *                      V - N x N orthogonal matrix(V' is the transpose of V)
 *
 *               This class provides the following interfaces:
 *
 *                 1) Solve - Takes an M x N matrix A and computes the SVD.
 *                 2) GetOrthoMatrixU - Returns the value of the matrix U in the SVD
 *                 2) GetOrthoMatrixV - Returns the value of the matrix V in the SVD
 *                 2) GetEigenVectorW - Returns the diagonal values of the matrix W in the SVD
 *
 */

#include "stdafx.h"
#include "SingularValueDecomp.h"

#define FILE_REVISION "$Revision: 1.1 $"


/** 
 *  Constructor
 *
 *  @param uMaxDimM  the maximum number of rows for matrix A.
 *  @param uMaxDimN  the maximum number of colmuns for matrix A.
 */
SingularValueDecomp::SingularValueDecomp( uint4 uMaxDimM, uint4 uMaxDimN )
: m_uMaxDimM(uMaxDimM), m_uMaxDimN(uMaxDimN) 
{

  // Allocate matrices(add 1 to index using 1-base).
  m_hfU = new float4*[uMaxDimM+1];
  if( m_hfU == NULL ) 
  {
	throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","SingularValueDecomp" , "no memory space" ));
  }

  m_hfV = new float4*[uMaxDimN+1];
  if( m_hfV == NULL ) 
  {
	throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","SingularValueDecomp", "no memory space" ));
  }
  
  m_pfW = new float4[uMaxDimN+1];
  if( m_pfW == NULL ) 
  {
	throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","SingularValueDecomp", "no memory space" ));
  }

  int i, j;
  for( i = 0 ; i < uMaxDimM+1 ; i++ )
  {
    m_hfU[i] = new float4[uMaxDimN+1];
    if( m_hfU[i] == NULL ) 
	{
	  throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","SingularValueDecomp", "no memory space"  ));
	}
  }
  for( i = 0 ; i < uMaxDimN+1 ; i++ )
  {
    m_hfV[i] = new float4[uMaxDimN+1];
    if( m_hfV[i] == NULL ) 
	{
	  throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","SingularValueDecomp", "no memory space"  ));
	}
  }

  // Initialization: All matrices and vectors to zero.  
  m_uDimM = 0;
  m_uDimN = 0;

  for( i = 0 ; i < uMaxDimM+1 ; i++ )
  {
    for( j = 0 ; j < uMaxDimN+1 ; j++ )
    {
      m_hfU[i][j] = 0.0;
    }
  }

  for( i = 0 ; i < uMaxDimN+1 ; i++ )
  {
    for( j = 0 ; j < uMaxDimN+1 ; j++ )
    {
      m_hfV[i][j] = 0.0;
    }
  }

  for( j = 0 ; j < uMaxDimN+1 ; j++ )
  {
    m_pfW[j] = 0.0;
  }
}


/** 
 *  Destructor
 */
SingularValueDecomp::~SingularValueDecomp()
{
  for( int i = 0 ; i < m_uMaxDimM+1 ; i++ )
  {
    delete [] m_hfU[i];
  }
  for( int j = 0 ; j < m_uMaxDimN+1 ; j++ )
  {
    delete [] m_hfV[j];
  }
  delete [] m_hfU;
  delete [] m_hfV;
  delete [] m_pfW;
}


/** 
 *  SVD API, - GetOrthoMatrixU
 *
 *  @return void
 *  @param ** hfU pointer to store the matrix U of the SVD, ( A ) = ( U ) ( W ) ( V' ).
 */
void SingularValueDecomp::GetOrthoMatrixU( float4 ** hfU )
{
  if( hfU != NULL )
  {
    for( int i = 0 ; i < m_uDimM ; i++ )
	{
      for( int j = 0 ; j < m_uDimN ; j++ ) 
	  {
		hfU[i][j] = m_hfU[i+1][j+1];
	  }
	}
  }
}


/** 
 *  SVD API, - GetOrthoMatrixV
 *
 *  @return void
 *  @param ** hfV pointer to store the matrix V of the SVD, ( A ) = ( U ) ( W ) ( V' ).
 */
void SingularValueDecomp::GetOrthoMatrixV( float4 ** hfV )
{
  if( hfV != NULL )
  {
    for( int i = 0 ; i < m_uDimN ; i++ )
	{
      for( int j = 0 ; j < m_uDimN ; j++ ) 
	  {
		hfV[i][j] = m_hfV[i+1][j+1];
	  }
	}
  }
}


/** 
 *  SVD API, - GetDiagonalVectorW
 *
 *  @return void
 *  @param * pfW pointer to store the diagonal values from matrix W of the SVD, ( A ) = ( U ) ( W ) ( V' ).
 */
void SingularValueDecomp::GetDiagonalVectorW( float4 * pfW )
{
  if( pfW != NULL )
  {
    for( int j = 0 ; j < m_uDimN ; j++ ) 
	{
	  pfW[j] = m_pfW[j+1];
	}
  }
}


/** 
 *  SVD API, - Solve
 *
 *  @return void
 *  @param ** hfA the input matrix, ( A ) ( x ) = ( b ), representing the Linear Algebraic Equation.
 *  @param uM Number of rows for ** hfA.
 *  @param uN Number of colmuns for ** hfA.
 */
void SingularValueDecomp::Solve( float4 ** hfA, const uint4 uDimM, const uint4 uDimN ) 
{
  // Verify input matrix, A.
  if( hfA == NULL ) 
  {
	throw ex::OutOfMemoryException(LogRec( "SingularValueDecomp","Solve", "no memory space" ));
  }

  // Set matrix dimensions:  
  if( uDimM > m_uMaxDimM )
  {
	throw ex::AbortDataProcessingException(LogRec("SingularValueDecomp","Solve","dimension overflow" ));
  }
  else
  {
    m_uDimM = uDimM;
  }
  if ( uDimN > m_uMaxDimN ) 
  {
	throw ex::AbortDataProcessingException(LogRec("SingularValueDecomp","Solve", "dimension overflow" ));
  }
  else
  {
    m_uDimN = uDimN;
  }
  
  // Copy input matrix A[0..M-1][0..N-1] to member matrix U[1..M][1..N]
  // (necessary to utilize standard code for SVD whose indexing is 1-based).

  for( int i = 0 ; i < m_uDimM ; i++ )
  {
    for( int j = 0 ; j < m_uDimN ; j++ ) 
	{
	  m_hfU[i+1][j+1] = hfA[i][j];
	}
  }

  // Singular Value Decomposition, Numerical Recipy.
  Svdcmp( m_hfU, m_uDimM, m_uDimN, m_pfW, m_hfV );

  // Sort SVD solution so that (W) has descending non-zero members.
  SortSvd( m_hfU, m_pfW, m_hfV, m_uDimM+1, m_uDimN+1 );
}//Solve


/////////////////////////////////////////////////////////////////////////////////////
// The following are standard codes for Singular Value Decomposition.
// These routine(s) are based on the routine(s) from the book "Numerical Recipes in C"
// (Cambridge University Press, 2nd edition), Copyright (C) 1987-1992 by Numerical
// Recipes Software.  Used by permission.
// Use of this routine(s) other than as an integral part of Singular Value Decomposition
// requires an additional license from Numerical Recipes Software.
// Further distribution in any form is prohibited.
//////////////////////////////////////////////////////////////////////////////////////


/** 
 *  Singular Value Decomposition - Numerical Recipy
 *
 *  @return void
 *  @param ** hfA  the matrix A on input and the matrix U on output of ( A ) = ( U ) ( W ) ( V' ).
 *  @param iDimM  the dimension of matrix A.
 *  @param iDimN  the dimension of matrix A.
 *  @param *  pfW  the diagonal matrix W.
 *  @param ** hfV  the output matrix V(not the transpose Vt) of ( A ) = ( U ) ( W ) ( V' ).
 */
void SingularValueDecomp::Svdcmp( float4 ** hfA, int4 iDimM, int4 iDimN, float4 * pfW, float4 ** hfV )
{
  int4      iFlag;    // Flag - Test for splitting.
  int4      i;        // Index variable.
  int4      iIter;    // Iteration.
  int4      iNM1;     // "N minus 1" index.
  int4      j;        // Index variable.
  int4      jj;       // Index variable.
  int4      k;        // Index variable.
  int4      l;        // Index variable.

  float4    fAnorm;
  float4    fScale;   // Scale factor.
  float4    c,f,g,h,s;// Algebraic variables.
  float4    x,y,z;    // Algebraic variables.
  float4 *  pfRv1;     // Pointer to vector rv1.

  
  // Allocate rv1 vector:
  pfRv1 = AllocVector( 1, iDimN );

  // Initialization for reduction to bidiagonal form:
  g = fScale = fAnorm = 0.0;
  
  // Reduction to bidiagonal form:
  for( i = 1 ; i <= iDimN ; i++ )
  {
    l = i+1;
    pfRv1[i] = fScale * g;
    g = s = fScale = 0.0;
    if( i <= iDimM )
    {
      for( k = i; k <= iDimM; k++ ) 
	  {
		fScale += static_cast<float4> (fabs(hfA[k][i]));
	  }
      if ( fScale )
      {
        for ( k = i; k <= iDimM; k++ )
        {
          hfA[k][i] /= fScale;
          s += hfA[k][i]*hfA[k][i];
        }
        f = hfA[i][i];
        g = -SIGN( sqrt(s),f );
        h = f * g - s;
        hfA[i][i] = f - g;
        for( j = l;j <= iDimN; j++ )
        {
          for( s = 0.0, k = i; k <= iDimM; k++ ) 
		  {
			s += hfA[k][i]*hfA[k][j];
		  }
          f = s/h;
          for( k = i; k <= iDimM; k++ ) 
		  {
			hfA[k][j] += f*hfA[k][i];
		  }
        }
        for( k = i; k <= iDimM; k++ ) 
		{
	      hfA[k][i] *= fScale;
		}
      }
    }
    pfW[i] = fScale * g;
    g = s = fScale = 0.0;
    if( i <= iDimM && i != iDimN )
    {
      for( k = l; k <= iDimN; k++ ) 
	  {
		fScale += static_cast<float4> (fabs(hfA[i][k]));
	  }
      if( fScale )
      {
        for( k = l ; k <= iDimN; k++ )
        {
          hfA[i][k] /= fScale;
          s += hfA[i][k]*hfA[i][k];
        }
        f = hfA[i][l];
        g = -SIGN( sqrt(s), f );
        h = f * g - s;
        hfA[i][l] = f - g;
        for( k = l; k <= iDimN; k++ ) 
		{
	      pfRv1[k] = hfA[i][k]/h;
		}
        for( j = l; j <= iDimM; j++ )
        {
          for( s = 0.0, k = l; k <= iDimN; k++ ) 
		  {
			s += hfA[j][k]*hfA[i][k];
		  }
          for( k = l; k <= iDimN; k++ ) 
		  {
			hfA[j][k] += s*pfRv1[k];
		  }
        }
        for( k = l; k <= iDimN; k++ ) 
		{
		  hfA[i][k] *= fScale;
		}
      }
    }
    fAnorm = max( fAnorm, (fabs(pfW[i])+fabs(pfRv1[i])) );
  }

  // Accumulation of right-hand transformations:
  for( i = iDimN; i >= 1; i-- )
  {
    if( i < iDimN )
    {
      if( g )
      {
        for( j = l; j <= iDimN; j++ )
		{
          hfV[j][i] = (hfA[i][j]/hfA[i][l])/g;
		}
        for( j = l; j <= iDimN; j++ )
        {
          for( s = 0.0, k=l; k <= iDimN; k++ ) 
		  {
			s += hfA[i][k]*hfV[k][j];
		  }
          for( k = l; k <= iDimN; k++ ) 
		  {
			hfV[k][j] += s*hfV[k][i];
		  }
        }
      }
      for( j = l; j <= iDimN; j++ ) 
	  {
		hfV[i][j] = hfV[j][i] = 0.0;
	  }
    }
    hfV[i][i] = 1.0;
    g = pfRv1[i];
    l = i;
  }

  // Accumulation of left-hand transformations:
  for( i = min( iDimM, iDimN ); i >= 1; i-- )
  {
    l = i+1;
    g = pfW[i];
    for( j = l; j <= iDimN; j++ ) 
	{
	  hfA[i][j] = 0.0;
	}
    if( g )
    {
      g = 1.0/g;
      for( j = l; j <= iDimN; j++ )
      {
        for( s = 0.0, k = l; k <= iDimM; k++ ) 
		{
		  s += hfA[k][i]*hfA[k][j];
		}
        f = (s/hfA[i][i])*g;
        for( k = i; k <= iDimM; k++ ) 
		{
		  hfA[k][j] += f*hfA[k][i];
		}
      }
      for( j = i; j <= iDimM; j++ ) 
	  {
		hfA[j][i] *= g;
	  }
    }
    else
    {
      for( j = i; j <= iDimM; j++ ) 
	  {
		hfA[j][i] = 0.0;
	  }
    }
    ++hfA[i][i];
  }

  // Diagonalization of the bidiagonal form: Loop over singular values, 
  //   and over allowed iterations.
  for( k = iDimN; k >= 1; k-- )
  {
    for( iIter = 1; iIter <= 30; iIter++ )
    {
      iFlag = 1;
      for( l = k; l >= 1; l-- )
      {
        iNM1 = l - 1;
        if( (float4) (fabs(pfRv1[l])+fAnorm) == fAnorm )
        {
          iFlag = 0;
          break;
        }
        if( (float4) (fabs(pfW[iNM1])+fAnorm) == fAnorm ) break;
      }
      if ( iFlag )
      {
        c = 0.0;
        s = 1.0;
        for( i = l; i <= k; i++ )
        {
          f = s*pfRv1[i];
          pfRv1[i] = c*pfRv1[i];
          if( (float4) (fabs(f) + fAnorm) == fAnorm ) 
		  {
			break;
		  }
          g = pfW[i];
          h = Pythag( f, g );
          pfW[i] = h;
          h = 1.0/h;
          c = g*h;
          s = -f*h;
          for( j = 1; j <= iDimM; j++ )
          {
            y = hfA[j][iNM1];
            z = hfA[j][i];
            hfA[j][iNM1] = y*c + z*s;
            hfA[j][i]    = z*c - y*s;
          }
        }
      }
      z = pfW[k];
      if( l == k )
      {
        if( z < 0.0 )
        {
          pfW[k] = -z;
          for( j = 1; j <= iDimN; j++ ) 
		  {
			hfV[j][k] = -hfV[j][k];
		  }
        }
        break;
      }
      if( iIter == 30 )
      {
        throw ex::AbortDataProcessingException(LogRec("No convergence in 30 iterations", "SingularValueDecomp", "Svdcmp" ));
      }
      x = pfW[l];
      iNM1 = k - 1;
      y = pfW[iNM1];
      g = pfRv1[iNM1];
      h = pfRv1[k];
      f = ((y-z)*(y+z) + (g-h)*(g+h))/(2.0*h*y);
      g = Pythag( f, 1.0 );
      f = ((x-z)*(x+z) + h*((y/(f+SIGN(g,f))) - h))/x;
      c = s = 1.0;
      for( j = l; j <= iNM1; j++ )
      {
        i = j + 1;
        g = pfRv1[i];
        y = pfW[i];
        h = s*g;
        g = c*g;
        z = Pythag( f, h );
        pfRv1[j] = z;
        c = f/z;
        s = h/z;
        f = x*c + g*s;
        g = g*c - x*s;
        h = y*s;
        y *= c;
        for( jj = 1; jj <= iDimN; jj++ )
        {
          x = hfV[jj][j];
          z = hfV[jj][i];
          hfV[jj][j] = x*c + z*s;
          hfV[jj][i] = z*c - x*s;
        }
        z = Pythag( f, h );
        pfW[j] = z;
        if ( z )
        {
          z = 1.0/z;
          c = f*z;
          s = h*z;
        }
        f = c*g + s*y;
        x = c*y - s*g;
        for( jj = 1; jj <= iDimM; jj++ )
        {
          y = hfA[jj][j];
          z = hfA[jj][i];
          hfA[jj][j] = y*c + z*s;
          hfA[jj][i] = z*c - y*s;
        }
      }
      pfRv1[l] = 0.0;
      pfRv1[k] = f;
      pfW[k]   = x;
    }
  }

  // Deallocate rv1 vector:
  FreeVector( pfRv1, 1, iDimN );
}



/** 
 *  Sorts(by Straight Insertion) the solution of SVD, ( A ) = ( U ) ( W ) ( V' ),
 *  so that the Wj's are in decending order. 
 *
 *  Note: other sorting techniques are recommended for Dimensions N > 20.
 *
 *  @return void
 *  @param ** hfU  the matrix U of the SVD.
 *  @param *  pfW  the diagonal matrix W of the SVD.
 *  @param ** hfV  the output matrix V(not the transpose Vt) of the SVD.
 *  @param uDimM  dimension M of the M x N matrix U.
 *  @param uDimN  dimension N of the N x N matrix V and N-vector W.
 */
void SingularValueDecomp::SortSvd( float4 ** hfU, float4 * pfW, float4 ** hfV, uint4 uDimM, uint4 uDimN )
{
  int     i = 0, j = 0;
  float4  fTmp = 0.0;

  for( j = 2 ; j <= uDimN ; j++ )
  {
    fTmp = pfW[j];
    i = j-1;
    while( i > 0 && pfW[i] < fTmp )
    {
      pfW[i+1] = pfW[i];
      SwapCol( hfU, uDimM, i, i+1 );
      SwapCol( hfV, uDimN, i, i+1 ); // Since its transpose use SwapCol instead of SwapRow
      i--;
    }
    pfW[i+1] = fTmp;
  }
}


/** 
 *  Swaps one column for another in Matrix.
 *
 *  @return void
 *  @param hMatrix  pointer to M x N matrix.
 *  @param uDimM    dimension M of the M x N matrix.
 *  @param iCol1    column #1 to swap with column #2.
 *  @param iCol2    column #2 to swap with column #1.
 */
void SingularValueDecomp::SwapCol( float4 ** hMatrix, uint4 uDimM, uint4 iCol1, uint4 iCol2 )
{
  float4 fTmp = 0.0;

  for( int i = 0 ; i < uDimM ; i++ )
  {
    fTmp = hMatrix[i][iCol2];
    hMatrix[i][iCol2] = hMatrix[i][iCol1];
    hMatrix[i][iCol1] = fTmp;
  }
}


/** 
 *  Swaps one row for another in Matrix.
 *
 *  @return void
 *  @param hMatrix  pointer to M x N matrix.
 *  @param uDimN    dimension N of the M x N matrix.
 *  @param iRow1    column #1 to swap with row #2.
 *  @param iRow2    column #2 to swap with row #1.
 */
void SingularValueDecomp::SwapRow( float4 ** hMatrix, uint4 uDimN, uint4 iRow1, uint4 iRow2 )
{
  float4 fTmp = 0.0;

  for( int j = 0 ; j < uDimN ; j++ )
  {
    fTmp = hMatrix[iRow2][j];
    hMatrix[iRow2][j] = hMatrix[iRow1][j];
    hMatrix[iRow1][j] = fTmp;
  }
}



/** 
 *  Computes the Pythagorean, Sqrt( a*a + b*b ), without destructive underflow or overflow.
 *
 *  @return float
 *  @param fA the variable a.
 *  @param fB the variable b.
 */
float4 SingularValueDecomp::Pythag( float4 fA, float4 fB )
{
  float4 fAbsA, fAbsB;  // Absolute values of A & B.
  float4 fPythag;       // Pythagorean.

  fAbsA = fabs(fA);
  fAbsB = fabs(fB);

  if( fAbsA > fAbsB )
  {
    fPythag = (fAbsA*sqrt(1.0 + SQ(fAbsB/fAbsA)));
  }
  else
  {
    fPythag = (fAbsB == 0.0 ? 0.0 : fAbsB*sqrt(1.0 + SQ(fAbsA/fAbsB)));
  }

  return( fPythag );
}


/** 
 *  allocate a float vector with subscript range v[s1..s2]
 *
 *  @return float4 *
 *  @param iS1
 *  @param iS2
 */
float4 * SingularValueDecomp::AllocVector( int8 iS1, int8 iS2 )
{
  float4 * pVector;

  pVector = (float4 *) new float4[ (iS2 - iS1 + 2) ];

  if( !pVector )
  {
    throw ex::OutOfMemoryException(LogRec("SingularValueDecomp","AllocVector", "no memory space" ));
  }

  return( pVector - iS1 + 1 );
}


/** 
 *  free a float vector allocated with AllocVector()
 *
 *  @return void
 *  @param *pV
 *  @param iS1
 *  @param iS2
 */
void SingularValueDecomp::FreeVector( float4 * pVector, int8 iS1, int8 iS2 )
{
  delete [] ( pVector + iS1 - 1 );
}


// complete revision history:
// $Log: SingularValueDecomp.C,v $
// Revision 1.1  2006/10/11 20:36:39  dongqing
// add to the vxBass
//
// Revision 1.3  2005/11/15 13:12:19  mkontak
// Fix to work with the new WQ and Console 3.0
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.1  2001/10/26 02:19:15  tushar
// removed errors due to changes made to logger class
//
// Revision 3.0  2001/10/14 23:01:46  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:38:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:24:28   ingmar
// Initial revision.
// Revision 1.10  2001/08/13 20:43:16  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.9  2001/07/25 15:35:18  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.8  2001/07/24 23:07:48  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.7  2001/07/23 18:25:11  lihong
// Defect ID: 599
// replaced previous exceptions with detailed LogRecord
//
// Revision 1.6  2001/07/23 16:53:59  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.1 $"
//
// Revision 1.5  2001/07/20 18:40:11  lihong
// Defect ID: 599
// chagned OutofMemoryException() to AbortDataProcessingException()
// for no convergence in SVD
//
// Revision 1.4  2001/07/02 21:36:49  uday
// coding conventions for variables
//
// Revision 1.3  2001/06/25 22:21:20  uday
// coding conventions for the segmentation module
//
// Revision 1.2  2001/05/23 21:36:52  dongqing
// rewrite Seg module
//
// Revision 1.1.2.3  2001/05/09 22:07:03  dongqing
// new
//
// Revision 1.1.2.2  2001/05/03 21:45:54  dongqing
// new
//
// Revision 1.1.2.1  2001/05/03 15:08:33  jadd
// new
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/SingularValueDecomp.C,v 1.1 2006/10/11 20:36:39 dongqing Exp $
// $Id: SingularValueDecomp.C,v 1.1 2006/10/11 20:36:39 dongqing Exp $