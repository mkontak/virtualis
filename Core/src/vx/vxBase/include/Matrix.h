// $Id: Matrix.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Name] [e-mail]
//
// Complete history at bottom of file.

#ifndef Matrix_h
#define Matrix_h



// include declarations 
#include "Point.h"
#include "Vector.h"
#include "Normal.h"

#ifdef __linux__
#include <typeinfo>
#endif



/**
 * Template class Matrix<T> is a 4x4 matrix.
 * First index is column, 2nd index is row, this makes it portable for OpenGL.
 * $Id: Matrix.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ <a href="../../VxBase/3_math/Spec/MathDesignSpecification.htm">Design Specification</a>
 */
template <class T> 
class Matrix
{
// member functions
public:
  
  /// Default Constructor, creates an identity matrix.
  Matrix<T>();
  
  /// Constructor: takes the 16 values in x,y order (Euclidean Co-ords).
  Matrix<T>(const T & ax, const T & bx, const T & cx, const T & dx,
    const T & ay, const T & by, const T & cy, const T & dy,
    const T & az, const T & bz, const T & cz, const T & dz,
    const T & aw, const T & bw, const T & cw, const T & dw);
  
  /// Copy Constructor.
  Matrix<T>(const Matrix<T> & other);
  
  /// Assignment Operator.
  inline Matrix<T> & operator=(const Matrix<T> & other);
  
  /// Access function for the private data (matrix elements)
  inline T const & 
    operator()(const uint1 & x, const uint1 & y) const;
  
  /// Equals binary operator.
  inline bool operator ==(const Matrix<T> & other) const;
  
  /// Not Equals binary operator.
  inline bool operator !=(const Matrix<T> & other) const;
  
  /// Unary plus operator.
  inline Matrix<T> operator +() const;
  
  /// Unary minus operator.
  inline Matrix<T> operator -() const;
  
  /// Binary plus operator.
  inline Matrix<T> operator +(const Matrix<T> & other) const;
  
  /// Binary minus operator.
  inline Matrix<T> operator -(const Matrix<T> & other) const;
  
  /// Binary += operator.
  inline Matrix<T> & operator +=(const Matrix<T> & other);
  
  /// Binary -= operator.
  inline Matrix<T> & operator -=(const Matrix<T> & other);
  
  /// Binary * operator: Multiplication of a Matrix<T> with a scalar(type T).
  inline Matrix<T> operator *(const T & scalar) const ;
  
  /// Binary * operator:Multiplication of a Matrix<T> by a Point<T>.
  inline Point<T> operator *(const Point<T> & point) const;
  
  /// Binary * operator:Multiplication of a Matrix<T> by a Vector<T>.
  inline Vector<T> operator *(const Vector<T> & vector) const;
    
  /// Binary * operator:Multiplication of a Matrix<T> by a Normal<T>.
  inline Normal<T> operator *(const Normal<T> & normal) const;
  
  /// Binary * operator:Multiplication of a Matrix<T> by a Matrix<T>.
  inline Matrix<T> operator *(const Matrix<T> & other) const;
  
  /// Binary * operator:Multiplication of a Matrix<T> by a Scalar of type T.
  inline Matrix<T> & operator *=(const T & scalar);
  
  /// Binary *= operator:Multiplication of a Matrix<T> by a Matrix<T>.
  inline Matrix<T> & operator *=(const Matrix<T> & other);
  
  /// Binary / operator:Multiplication of a Matrix<T> by a Scalar of type T.
  inline Matrix<T> operator /(const T & scalar) const;
  
  /// Binary /= operator:Multiplication of a Matrix<T> by a Scalar of type T.
  inline Matrix<T> & operator /=(const T & scalar);
  
  /// Changes the matrix elemnts to an identity matrix.
  inline Matrix<T> &	Identity();
  
  /// Returns a matrix that is the transpose of this one.
  inline Matrix<T> GetTranspose() const;
  
  /// Reassigns its elements to become its transpose.
  inline Matrix<T> & Transpose();
  
  /// Returns a scalar value that is the 4x4 determinant of the matrix.
  inline T GetDeterminant() const;
  
  /// Returns a scalar value of the 3x3 determinant passed as 9 scalars.
  inline T GetDeterminant3x3 (
    const T & a, const T & b, const T & c,
    const T & d, const T & e, const T & f,
    const T & g, const T & h, const T & i) const;
  
  /// Returns a scalar value of the 2x2 determinant passed as 4 scalars.
  inline T 
    GetDeterminant2x2(const T & a, const T & b, const T & c, const T & d) const;
  
  /// Returns the corresponding adjoint Matrix<T>.
  inline Matrix<T> GetAdjoint() const;
  
  /// Returns the Inverse of the Matrix<T> as a Matrix<T>.
  inline Matrix<T> GetInverse() const;
  
  /// Rotates the Matrix by a Normal<T>, and an angle of type T.  The angle period is [0, 2Pi]
  inline void Rotate(const Normal<T> & normal, const T & angle);
  
  /// Translates the Matrix<T> by a Vector<T>.
  inline Matrix<T> & Translate(const Vector<T> & vector);
  
  /// Scales the Marix<T> by the Scaling Transorm Matrix:Vector<T>(sx,sy,sz).
  inline Matrix<T> & Scale(const Vector<T> & vector);
  
  /// Reassigns the matrix elements from an OpenGL Matrix.
  inline void	SetFromOpenGLMatrix(const T * pOpenGLMatrix);
  
  /// Sets an OpenGL Matrix from the matrix elemnts.
  inline void	SetOpenGLMatrix(T * pOpenGLMatrix) const;
  
  /// Test function for the template class Matrix<T>.
  static bool Test (std::ostream & os=std::cout);
  
  /// Test function to verify just one particular template instance 
  static bool TestByType(std::ostream & os=std::cout);

  /// Overloaded << Operator.
  friend std::ostream & operator <<(std::ostream & os, const Matrix<T> & matrix);

private:
   // Matrix elements: First index is column, 2nd index is row.
   T m_mat[4][4]; 
};// class Matrix



#include "Matrix.inl"



#endif // Matrix_h



// $Log: Matrix.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:43:45  michael
// code review
//
// Revision 3.8  2003/01/22 20:39:31  ingmar
// removed duplicate operator * (Vector3D) , as Vector and Vector3D are now the same
//
// Revision 3.7  2001/01/02 19:38:03  ingmar
// better documentation structure
//
// Revision 3.6  2001/01/02 18:51:48  ingmar
// better documentation structure
//
// Revision 3.5  2002/07/05 21:43:07  ingmar
// added rotation for Vector3D
//
// Revision 3.4  2002/07/05 21:10:04  ingmar
// added comment to explain the input range expected for the rotation angle
//
// Revision 3.3  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.2  2002/01/29 23:06:47  michael
// indenting
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:10   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:06   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.14  2001/08/23 17:48:32  sarang
// linux/gcc fix.
//
// Revision 1.13  2001/05/31 21:32:48  ana
// changes regarding code review
//
// Revision 1.12  2001/05/08 18:21:02  ana
// Modifications after code review
//
// Revision 1.11  2001/05/01 23:03:22  ana
// code standards
//
// Revision 1.10  2001/04/30 13:59:12  ana
// changes to fit in code standards
//
// Revision 1.9  2001/04/17 23:34:31  ana
// added heading comments
//
// Revision 1.8  2001/04/17 22:28:18  ana
// added TestByType function
//
// Revision 1.7  2001/04/12 14:37:29  dmitry
// Sec_3_math base class added
//
// Revision 1.6  2001/04/09 18:03:20  dmitry
// Removed Revision History from Top of File
//
// Revision 1.5  2001/03/28 19:01:01  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.4  2001/03/23 20:08:52  dmitry
// Updated datatypes as declared in Typedef.h
//
// Revision 1.3  2000/12/28 20:14:43  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.2  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 8     8/17/00 1:00p Antonio
// modified header and footer.
// 
// *****************  Version 7  *****************
// User: Antonio      Date: 8/10/00    Time: 4:21p
// Updated in $/vx/src/Math
// modified header to newer version
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/08/00    Time: 3:00p
// Updated in $/vx/src/Math
// renamed GetIntoOpenGLMatirx->SetFromOpenGLMatrix,
//                SetFromOpenGLMatrix->SetOpenGLMatrix.
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/02/00    Time: 9:17p
// Updated in $/vx/src/Math
// dded complete history to end of file
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/01/00  Time:  3:02p
// Checked in $/vx/src/Math
// 
// *****************  Version 3  *****************
// User: Frank           Date:  7/18/00  Time:  3:55p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 2  *****************
// User: Jay             Date:  7/18/00  Time: 11:07a
// Checked in $/vx/src/Math
// Comment:
//   Made dll linking work for the static functions
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Matrix.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Matrix.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Matrix.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
