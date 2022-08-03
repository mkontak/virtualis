/// $Id: SingularValueDecomp.h,v 1.1 2006/10/11 20:36:17 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/vxBase/include/SingularValueDecomp.h,v 1.1 2006/10/11 20:36:17 dongqing Exp $
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
 * This routine, Singular Value Decomposition, is based on the routine, 
 * Singular Value Decomposition from the book Numerical Recipes in C
 * (Cambridge University Press), Copyright (C) 1987-1992 by Numerical Recipes Software.  
 * Used by permission.  Use of this routine other than as an integral part of Segmentation 
 * requires an additional license from Numerical Recipes Software.  
 * Further distribution in any form is prohibited.
 *
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


#ifndef SingularValueDecomp_h         // prevent multiple includes
#define SingularValueDecomp_h

class SingularValueDecomp
{
public: 

  /// Constructor
  SingularValueDecomp( uint4 uMaxDimM, uint4 uMaxDimN );
  
  /// Destructor
  virtual ~SingularValueDecomp();

public: // Functions...

  /// Computes the SVD, ( A ) = ( U ) ( W ) ( V' )
  void Solve( float4 ** hfA, const uint4 uM, const uint4 uN );

  /// Returns value of the matrix U of the SVD
  void GetOrthoMatrixU( float4 ** hfU );

  /// Returns value of the matrix V of the SVD
  void GetOrthoMatrixV( float4 ** hfV );

  /// Returns value along diagonal of matrix W of the SVD
  void GetDiagonalVectorW( float4 * pfW );

private: // Functions...

  /// Singular Value Decomposition - reference "Numerical Recipes in C"
  void Svdcmp( float4 ** hfa, int4 m, int4 n, float4 * pfw, float4 ** hfv );

  /// Sort SVD: Wj's in decending order
  void SortSvd( float4 ** hfu, float4 * pfw, float4 ** hfv, uint4 uDimM, uint4 uDimN );

  /// Swap Column of M x N Matrix: Col1 <-> Col2
  void SwapCol( float4 ** hfMatrix, uint4 uDimM, uint4 iCol1, uint4 iCol2 );

  /// Swap Row of M x N Matrix: Row1 <-> Row2
  void SwapRow( float4 ** hfMatrix, uint4 uDimN, uint4 iRow1, uint4 iRow2 );

  /// Pythagorean value(i.e., sqrt(a*a + b*b) )
  float4 Pythag( float4 fa, float4 fb );

  /// Allocates a float vector with subscript range v[s1..s2]
  float4 * AllocVector( int8 iS1, int8 iS2 );

  /// Free a float vector allocated with vector()
  void FreeVector( float4 * pfV, int8 iS1, int8 iS2 );
  
  /**  
   *    inline function to get a sign value based on another number
   *
   *    @para   a is the input number that its absolute value will be output
   *    @para   b is the number that its sign will be output
   *    @return float
   */
  inline float4 SIGN( float4 a,float4 b )
  {
    return( b > static_cast<float4> (0.0) ) ? fabs(a) : -fabs(a);
  }
  
  /** 
   *   inline function to get square of a float number
   *
   *   @param  a is the input float number
   *   @return float
   */
  inline float4 SQ( float4 a )
  {
    return( a*a );
  }

private: // Variables...

  // Max. dimension M, for allocation of pointers to M x N matrices
  const uint4 m_uMaxDimM; 

  // Max. dimension N, for allocation of pointers to M x N matrices
  const uint4 m_uMaxDimN; 

  // Dimension M, for M x N matrix A
  uint4  m_uDimM;  
  
  // Dimension N, for M x N matrix A
  uint4  m_uDimN;    

  // Column-Orthogonal matrix U, in SVD, A = U x W x V'
  float4 ** m_hfU;

  // Column-Orthogonal matrix V(not transpose), in SVD, A = U x W x V'
  float4 ** m_hfV;

  // Diagonal matrix, W in SVD, A = U x W x V'
  float4 * m_pfW;       

};


#endif  // #ifndef SVD_h


// complete revision history:
// $Log: SingularValueDecomp.h,v $
// Revision 1.1  2006/10/11 20:36:17  dongqing
// add to the vxBass
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
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
//    Rev 1.0   Sep 16 2001 17:24:30   ingmar
// Initial revision.
// Revision 1.5  2001/07/25 15:35:18  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.4  2001/06/25 22:21:20  uday
// coding conventions for the segmentation module
//
// Revision 1.3  2001/05/29 21:03:20  lihong
// added acknowledgement from numerical recipes in c
//
// Revision 1.2  2001/05/23 21:36:45  dongqing
// rewrite Seg module
//
// Revision 1.1.2.3  2001/05/09 22:07:03  dongqing
// new
//
// Revision 1.1.2.2  2001/05/03 21:45:49  dongqing
// new
//
// Revision 1.1.2.1  2001/05/03 15:10:39  addonisio
// new
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/SingularValueDecomp.h,v 1.1 2006/10/11 20:36:17 dongqing Exp $
// $Id: SingularValueDecomp.h,v 1.1 2006/10/11 20:36:17 dongqing Exp $