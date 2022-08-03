// $Id: HessianMatrix.C,v 1.1 2006/08/30 21:16:09 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owners: Wenli Cai  wenli@viatronix.com


// includes
#include "StdAfx.h"
#include "HessianMatrix.h"

// namespaces
using namespace std;

// Defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * constructor
 */
HessianMatrix::HessianMatrix( Matrix<float4> & matrixA )
:m_Matrix( matrixA )
{
  m_fSigma = 1.0F;
  m_iJacobiMaxRot = 50;
} // HessianMatrix()

/**
*	Deconstructor
*/
HessianMatrix::~HessianMatrix()
{
}

/**
 * Run to solve the matrix to calculate the eigen values and eigen vectos
 */
bool HessianMatrix::Solve()
{
  float8 HessMat[4][4], eigenVect[4][4], eigenVal[4];
  int4 iJrot;

  //Hessian Filter
  HessMat[1][1] = m_Matrix(0,0);
  HessMat[2][2] = m_Matrix(1,1);
  HessMat[3][3] = m_Matrix(2,2);

  HessMat[1][2] = m_Matrix(0,1);
  HessMat[2][1] = m_Matrix(1,0);
  HessMat[2][3] = m_Matrix(1,2);
  HessMat[3][2] = m_Matrix(2,1);  
  HessMat[1][3] = m_Matrix(0,2);
  HessMat[3][1] = m_Matrix(2,0);

  CalEigenByJacobi( HessMat, 3, eigenVal, eigenVect, &iJrot );

  // no eignevalue -- illness matrix
  if ( iJrot > m_iJacobiMaxRot )
    return false;

  m_Lm1 = eigenVal[1];
  m_Lm2 = eigenVal[2];
  m_Lm3 = eigenVal[3]; 

  m_V1 = Normal<float4>( eigenVect[1][1], eigenVect[2][1], eigenVect[3][1] );
  m_V2 = Normal<float4>( eigenVect[1][2], eigenVect[2][2], eigenVect[3][2] );
  m_V3 = Normal<float4>( eigenVect[1][3], eigenVect[2][3], eigenVect[3][3] );

  return true;

}

void HessianMatrix::GetEigenValues( float4 & fLm1, float4 & fLm2, float4 & fLm3 )
{
  fLm1 = m_Lm1;
  fLm2 = m_Lm2;
  fLm3 = m_Lm3;
}

void HessianMatrix::GetEigenValues( float8 & fLm1, float8 & fLm2, float8 & fLm3 )
{
  fLm1 = m_Lm1;
  fLm2 = m_Lm2;
  fLm3 = m_Lm3;
}

void HessianMatrix::GetEigenVectors( Normal3Df & v1, Normal3Df & v2, Normal3Df & v3 )
{
  v1 = m_V1;
  v2 = m_V2;
  v3 = m_V3;
}

/**
 * return the nTh eigen value and eigen vector
 */
void HessianMatrix::GetEigenValueAndVector( int4 iNth, float4 & fEigenValue, Normal3Df & vEigenVector )
{
  switch( iNth ) {
  case 1:
    fEigenValue = m_Lm1;
    vEigenVector = m_V1;
  	break;
  case 2:
    fEigenValue = m_Lm2;
    vEigenVector = m_V2;
    break;
  case 3:
    fEigenValue = m_Lm3;
    vEigenVector = m_V3;
    break;
  default:
     break;
  }
}


/// All the Hessian matrix functions

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);

/**
 *	Calculate the eigen values and eigen vectors using Jacobi method
 *
 *  @param a     the input matrix
 *  @param n     the dimension
 *  @param d     ???
 *  @param v     ???
 *  @param pNrot ???
 */
void HessianMatrix::CalEigenByJacobi( float8 a[4][4], int n, float8 d[4], float8 v[4][4], int * pNrot )
{
  int j, iq, ip, i;
  float8 tresh, theta, tau, t, sm, s, h, g, c, b[4], z[4];


  // Initialize the identity matrix.
  for( ip = 1; ip <= n; ip++ )
  {
    for( iq = 1; iq <= n; iq++ )
    {
      v[ip][iq] = 0.;
    }

    v[ip][ip] = 1.;
  }

  // Initialize b and d to the diagonal of a.
  for( ip = 1; ip <= n; ip++ )
  {
    b[ip] = d[ip] = a[ip][ip];

    // This vector will accumulate terms of the form ta(pq) 
    // as in equation (11.1.14).
    z[ip] = 0.;
  }

  *pNrot = 0;

  for( i = 1; i <= m_iJacobiMaxRot; i++ )
  {
    sm = 0.;

    // Sum off-diagonal elements.
    for( ip = 1; ip <= n-1; ip++ )
    {
      for( iq = ip + 1; iq <= n; iq++ )
      {
        sm += fabs( a[ip][iq] );
      }
    }

    // The normal return, which relies on quadratic convergence
    // to machine underflow.
    if( sm == 0. )
    {
      //free_dvector( z, 1, n );
      //free_dvector( b, 1, n );

      // Sort by eigenvalue.
      SortEigen( d, v, n );

      return;
    }

    if( i < 4 )
    {
      // On the first three sweeps.
      tresh = .2 * sm/(n*n);
    }
    else // Thereafter.
    {
      tresh = 0.;
    }

    for( ip = 1; ip <= n-1; ip++ )
    {
      for( iq = ip + 1; iq <= n; iq++ )
      {
        g = 100. * fabs( a[ip][iq] );

        // After four sweeps, skip the rotation if the 
        // off-diagonal element is small.
        if( i > 4 && 
          (float8) ( fabs( d[ip] ) + g ) == (float8) fabs(d[ip]) &&
          (float8) ( fabs( d[iq] ) + g ) == (float8) fabs(d[iq]) )
        {
          a[ip][iq] = 0.;
        }
        else
        {   
          if( fabs( a[ip][iq] ) > tresh )
          {
            h = d[iq] - d[ip];

            if( (float8) (fabs(h) + g) == (float8) fabs(h) )
            {
              // t = 1/(2theta).
              t = (a[ip][iq])/h;
            }
            else
            {
              // Equation (11.1.10).
              theta = .5*h/(a[ip][iq]);
              t = 1./(fabs(theta) + sqrt(1.+theta*theta) );

              if( theta < 0. )
              {
                t = -t;
              }
            }

            c = 1./sqrt(1+t*t);
            s = t * c;
            tau = s/(1.+c);
            h = t * a[ip][iq];
            z[ip] -= h;
            z[iq] += h;
            d[ip] -= h;
            d[iq] += h;
            a[ip][iq] = 0.;

            for( j = 1; j <= ip-1; j++ )
            {
              // Case of rotations 1 <= j < p.
              ROTATE(a,j,ip,j,iq)
            }

            for( j = ip+1; j <= iq-1; j++ )
            {
              // Case of rotations p < j < q.
              ROTATE(a,ip,j,j,iq)
            }

            for( j = iq+1; j <= n; j++ )
            {
              // Case of rotations q < j <= n.
              ROTATE(a,ip,j,iq,j)
            }

            for( j = 1; j <= n; j++ )
            {
              ROTATE( v, j, ip, j, iq )
            }

            ++(*pNrot);
          }
        }
      }
    }

    for( ip = 1; ip <= n; ip++ )
    {
      b[ip] += z[ip];

      // Update d with the sum of ta(pq) and
      // reinitialize z
      d[ip] = b[ip];
      z[ip] = 0.;
    }
  }

} // CalEigenByJacobi()


/*
*  SortEigen : To sort eigenvalues and eigenvectors in descending order of eigenvalue.
*
*  DESCRIPTION: Given the eigenvalues d[1..n] and eigenvectors
*  v[1..n][1..n] as output from 'jacobi' or 'tqli', this
*  routine sorts the absolute eigenvalues into ascending order, 
*  and rearranges the columns of v correspondingly.  The method
*  is straight insertion.
* 
*  @param d array of eigenvalues
*  @param v array of eigenvectors
*  @param n number of eigenvalues
*/
void HessianMatrix::SortEigen( float8 d[4], float8 v[4][4], int n )
{
  int i, j, k;
  float8 p;

  for( i = 1; i < n; i++ )
  {
    k=i;

    p = fabs(d[i]);

    for( j = i + 1; j <= n; j++ )
    {
      if( fabs(d[j]) < p )
      {
        k = j;

        p = fabs(d[j]);
      }
    }

    if( k != i )
    {
      p = d[k];
      d[k] = d[i];
      d[i] = p;

      for( j = 1; j <= n; j++ )
      {
        p = v[j][i];
        v[j][i] = v[j][k];
        v[j][k] = p;
      }
    }
  }
} // SortEigen()


// undefines
#undef FILE_REVISION


// $Log: HessianMatrix.C,v $
// Revision 1.1  2006/08/30 21:16:09  frank
// moved here from vascular
//
// Revision 1.2  2005/06/08 14:52:39  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/05/11 15:34:22  wenli
// Get double type eigen values
//
// Revision 1.1  2004/04/07 15:56:38  wenli
// Calculate the EigenValue and EigneVectors
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/HessianMatrix.C,v 1.1 2006/08/30 21:16:09 frank Exp $
// $Id: HessianMatrix.C,v 1.1 2006/08/30 21:16:09 frank Exp $
