// $Id: Matrix3x3.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/*
  Template class Matrix3x3<T> is a 3x3 matrix.
*/

#ifndef Matrix3x3_h
#define Matrix3x3_h


// includes
#include "Vector.h"


// class definition
template <class T> 
class Matrix3x3 : Sec_math_3D
{
// functions
public:
  
  /// default constructor creating an identity matrix
  Matrix3x3();
  
  /// constructor taking the 3 vectors
  Matrix3x3(Vector<T> c1, Vector<T> c2, Vector<T> c3);
  
  /// constructor taking the 9 values in x,y order (Euclidean Co-ords).
  Matrix3x3(const T & m11, const T & m12, const T & m13,
            const T & m21, const T & m22, const T & m23,
            const T & m31, const T & m32, const T & m33);
  
  /// copy constructor
  Matrix3x3<T>(const Matrix3x3<T> & other);
  
  /// assignment operator
  inline Matrix3x3<T> & operator=(const Matrix3x3<T> & other);
  
  /// access function for the private data (matrix elements)
  inline T const &  operator()(const uint1 & x, const uint1 & y) const;
  
  /// equals binary operator
  inline bool operator ==(const Matrix3x3<T> & other) const;
  
  /// not equals binary operator
  inline bool operator !=(const Matrix3x3<T> & other) const;
  
  /// unary plus operator
  inline Matrix3x3<T> operator +() const;
  
  /// unary minus operator
  inline Matrix3x3<T> operator -() const;
  
  /// binary plus operator
  inline Matrix3x3<T> operator +(const Matrix3x3<T> & other) const;
  
  /// binary minus operator
  inline Matrix3x3<T> operator -(const Matrix3x3<T> & other) const;
  
  /// binary += operator
  inline Matrix3x3<T> & operator +=(const Matrix3x3<T> & other);
  
  /// binary -= operator
  inline Matrix3x3<T> & operator -=(const Matrix3x3<T> & other);
  
  /// binary * operator: Multiplication of a Matrix3x3<T> with a scalar(type T)
  inline Matrix3x3<T> operator *(const T & scalar) const ;
  
  /// binary * operator:Multiplication of a Matrix3x3<T> by a Point<T>
  inline Point<T> operator *(const Point<T> & point) const;
  
  /// binary * operator:Multiplication of a Matrix3x3<T> by a Vector<T>
  inline Vector<T> operator *(const Vector<T> & vector) const;
  
  /// binary * operator:Multiplication of a Matrix3x3<T> by a Normal<T>
  inline Normal<T> operator *(const Normal<T> & normal) const;
  
  /// binary * operator:Multiplication of a Matrix3x3<T> by a Matrix3x3<T>
  inline Matrix3x3<T> operator *(const Matrix3x3<T> & other) const;
  
  /// binary * operator:Multiplication of a Matrix3x3<T> by a Scalar of type T
  inline Matrix3x3<T> & operator *=(const T & scalar);
  
  /// binary *= operator:Multiplication of a Matrix3x3<T> by a Matrix3x3<T>
  inline Matrix3x3<T> & operator *=(const Matrix3x3<T> & other);
  
  /// binary / operator:Multiplication of a Matrix3x3<T> by a Scalar of type T
  inline Matrix3x3<T> operator /(const T & scalar) const;
  
  /// binary /= operator:Multiplication of a Matrix3x3<T> by a Scalar of type T
  inline Matrix3x3<T> & operator /=(const T & scalar);
  
  /// changes the Matrix3x3 elemnts to an identity matrix
  inline Matrix3x3<T> &	Identity();
  
  /// returns a matrix that is the transpose of this one
  inline Matrix3x3<T> GetTranspose() const;
  
  /// reassigns its elements to become its transpose
  inline Matrix3x3<T> & Transpose();
  
  /// returns a scalar value that is the 4x4 determinant of the matrix
  inline T GetDeterminant() const;
  
  /// returns the corresponding adjoint Matrix3x3<T>
  inline Matrix3x3<T> GetAdjoint() const;
  
  /// returns the Inverse of the Matrix3x3<T> as a Matrix3x3<T>
  inline Matrix3x3<T> GetInverse() const;
  
  /// rotates the Matrix3x3 by a Normal<T>, and an angle of type T
  inline void Rotate(const Normal<T> & normal, const T & angle);
  
  /// scaled and added a Matrix3x3
  Matrix3x3<T> plusScaled(Matrix3x3 A, float s);

  /// overloaded << operator
  friend std::ostream & operator <<(std::ostream & os, const Matrix3x3<T> & matrix);

// data
public:

   // Matrix3x3 elements: First index is column, 2nd index is row
   T m_mat[3][3]; 
}; // Matrix3x3


#include "Matrix3x3.inl"


#endif // Matrix3x3_h


// Revision History:
// $Log: Matrix3x3.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:43:46  michael
// code review
//
// Revision 1.6  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.5  2003/01/02 19:38:03  ingmar
// better documentation structure
//
// Revision 1.4  2001/01/02 18:51:48  ingmar
// better documentation structure
//
// Revision 1.3  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// Revision 1.2  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 1.1  2001/10/17 23:36:02  huamin
// Initial implementation for image warping.  Adapted from matrix class
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Matrix3x3.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Matrix3x3.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
