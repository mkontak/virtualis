// $Id: HessianMatrix.h,v 1.1 2006/08/30 21:16:09 frank Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owners: Wenli Cai  wenli@viatronix.com


/**
 * The class HessianMatrix tracks a centerline via Hessian Filter
 */


// pragmas
#pragma once


// includes
#include <vector>
#include "Matrix.h"


class VX_BLOCK_VOLUME_DLL HessianMatrix 
{
public:
 
  /// constructor 
  HessianMatrix( Matrix<float4> & matrixA );

  /// deconstructor 
  virtual ~HessianMatrix();
 
  /// Run to find a centerline from one seed point
  bool Solve();

  void GetEigenValues( float4 & fLm1, float4 & fLm2, float4 & fLm3 );
  void GetEigenValues( float8 & fLm1, float8 & fLm2, float8 & fLm3 );

  void GetEigenVectors( Normal3Df & v1, Normal3Df & v2, Normal3Df & v3 );

  void GetEigenValueAndVector( int4 iOption, float4 & fLm1, Normal3Df & v1 );

public:


private:  // Utility functions

  /// calculate the hessian matrix by Jacobi method
  void CalEigenByJacobi( float8 a[4][4], int n, float8 d[4], float8 v[4][4], int* nrot );

  /// sort the three eigenvalues  
  void SortEigen( float8 d[4], float8 v[4][4], int n );


private:

  // some variables  
  Matrix<float4> m_Matrix;

  /// EigenValues
  float8 m_Lm1, m_Lm2, m_Lm3;

  /// EigenVectors
  Normal3Df m_V1, m_V2, m_V3;

  /// parameters
  float4 m_fSigma;

  /// Jacobi matrix 
  int4 m_iJacobiMaxRot;

}; // class HessianMatrix


// $Log: HessianMatrix.h,v $
// Revision 1.1  2006/08/30 21:16:09  frank
// moved here from vascular
//
// Revision 1.2  2005/06/08 13:16:22  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/05/11 15:34:07  wenli
// Get double type eigen values
//
// Revision 1.1  2004/04/07 15:56:38  wenli
// Calculate the EigenValue and EigneVectors
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/HessianMatrix.h,v 1.1 2006/08/30 21:16:09 frank Exp $
// $Id: HessianMatrix.h,v 1.1 2006/08/30 21:16:09 frank Exp $
