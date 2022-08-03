// $Id: Matrix.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
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



// include declarations
#include "Vector.h"
#include "Vector3D.h"
#include "Normal.h"
#include <math.h>



/**
 * Default Constructor, creates an identity matrix.
 */
template <class T> 
Matrix<T>::Matrix()
{
    m_mat[0][0] = 1;  m_mat[1][0] = 0;  m_mat[2][0] = 0;  m_mat[3][0] = 0;
    m_mat[0][1] = 0;  m_mat[1][1] = 1;  m_mat[2][1] = 0;  m_mat[3][1] = 0;
    m_mat[0][2] = 0;  m_mat[1][2] = 0;  m_mat[2][2] = 1;  m_mat[3][2] = 0;
    m_mat[0][3] = 0;  m_mat[1][3] = 0;  m_mat[2][3] = 0;  m_mat[3][3] = 1;
} // end of Matrix<T>().


/**
 *Constructor takes the 16 values in x,y order (Euclidean Co-ords).
 *
 *m_mat[0][0] = ax.
 *m_mat[1][0] = bx.
 *m_mat[2][0] = cx.
 *m_mat[3][0] = dx.
 *m_mat[0][1] = ay.
 *m_mat[1][1] = by.
 *m_mat[2][1] = cy.
 *m_mat[3][1] = dy.
 *m_mat[0][2] = az.
 *m_mat[1][2] = bz.
 *m_mat[2][2] = cz.
 *m_mat[3][2] = dz.
 *m_mat[0][3] = aw.
 *m_mat[1][3] = bw.
 *m_mat[2][3] = cw.
 *m_mat[3][3] = dw.
 *@param ax const T &
 *@param bx const T &
 *@param cx const T &
 *@param dx const T &
 *@param ay const T &
 *@param by const T &
 *@param cy const T &
 *@param dy const T &
 *@param az const T &
 *@param bz const T &
 *@param cz const T &
 *@param dz const T &
 *@param aw const T &
 *@param bw const T &
 *@param cw const T &
 *@param dw const T &
 */
template <class T> 
Matrix<T>::Matrix
  (const T & ax, const T & bx, const T & cx, const T & dx,
   const T & ay, const T & by, const T & cy, const T & dy,
   const T & az, const T & bz, const T & cz, const T & dz,
   const T & aw, const T & bw, const T & cw, const T & dw) 
{
    m_mat[0][0] = ax;  m_mat[1][0] = bx;  m_mat[2][0] = cx;  m_mat[3][0] = dx;
    m_mat[0][1] = ay;  m_mat[1][1] = by;  m_mat[2][1] = cy;  m_mat[3][1] = dy;
    m_mat[0][2] = az;  m_mat[1][2] = bz;  m_mat[2][2] = cz;  m_mat[3][2] = dz;
    m_mat[0][3] = aw;  m_mat[1][3] = bw;  m_mat[2][3] = cw;  m_mat[3][3] = dw;
} // end of Matrix<T>( 16 scalars ).



/**
 * Copy Constructor.
 * @param other const Matrix<T> &
 */
template <class T> 
Matrix<T>::Matrix(const Matrix<T> & other)
{
    m_mat[0][0] = other.m_mat[0][0]; m_mat[1][0] = other.m_mat[1][0]; 
    m_mat[2][0] = other.m_mat[2][0]; m_mat[3][0] = other.m_mat[3][0]; 
    m_mat[0][1] = other.m_mat[0][1]; m_mat[1][1] = other.m_mat[1][1]; 
    m_mat[2][1] = other.m_mat[2][1]; m_mat[3][1] = other.m_mat[3][1]; 
    m_mat[0][2] = other.m_mat[0][2]; m_mat[1][2] = other.m_mat[1][2]; 
    m_mat[2][2] = other.m_mat[2][2]; m_mat[3][2] = other.m_mat[3][2]; 
    m_mat[0][3] = other.m_mat[0][3]; m_mat[1][3] = other.m_mat[1][3]; 
    m_mat[2][3] = other.m_mat[2][3]; m_mat[3][3] = other.m_mat[3][3];
} // end of Matrix<T>(const Matrix<T> & other).


/**
 * Assignment Operator overloaded for template class Matrix<T>.
 * @param other const Matrix<T> &
 */
template <class T> 
inline Matrix<T> & Matrix<T>::operator=(const Matrix<T> & other)
{
    m_mat[0][0] = other.m_mat[0][0]; m_mat[1][0] = other.m_mat[1][0]; 
    m_mat[2][0] = other.m_mat[2][0]; m_mat[3][0] = other.m_mat[3][0]; 
    m_mat[0][1] = other.m_mat[0][1]; m_mat[1][1] = other.m_mat[1][1]; 
    m_mat[2][1] = other.m_mat[2][1]; m_mat[3][1] = other.m_mat[3][1]; 
    m_mat[0][2] = other.m_mat[0][2]; m_mat[1][2] = other.m_mat[1][2]; 
    m_mat[2][2] = other.m_mat[2][2]; m_mat[3][2] = other.m_mat[3][2]; 
    m_mat[0][3] = other.m_mat[0][3]; m_mat[1][3] = other.m_mat[1][3]; 
    m_mat[2][3] = other.m_mat[2][3]; m_mat[3][3] = other.m_mat[3][3];
    return *this;
} // end of operator=(const Matrix<T> & other).


/**
 * () Operator: Reassigns the matrix elemnts to the 16 sclalars passed.
 * Takes 16 parameters of type T 
 * assigns them to the matrix elements (x,y) order
 * for more details see the copy constructor
 * @param x const T &
 * @param y const T &
 * @return T &
 */
template <class T> 
inline T const & Matrix<T>::operator ()
    (const unsigned char & x, const unsigned char & y) const
{
    return m_mat[x][y];
} // end of operator (). 


/**
 * Equals binary operator.
 * @param  other const Matrix<T>&
 * @return bool
 */
template <class T> 
inline bool Matrix<T>::operator == (const Matrix<T> & other) const
{	
    return 
      m_mat[0][0] == other.m_mat[0][0] && m_mat[1][0] == other.m_mat[1][0] && 
      m_mat[2][0] == other.m_mat[2][0] && m_mat[3][0] == other.m_mat[3][0] && 
      m_mat[0][1] == other.m_mat[0][1] && m_mat[1][1] == other.m_mat[1][1] && 
      m_mat[2][1] == other.m_mat[2][1] && m_mat[3][1] == other.m_mat[3][1] && 
      m_mat[0][2] == other.m_mat[0][2] && m_mat[1][2] == other.m_mat[1][2] && 
      m_mat[2][2] == other.m_mat[2][2] && m_mat[3][2] == other.m_mat[3][2] && 
      m_mat[0][3] == other.m_mat[0][3] && m_mat[1][3] == other.m_mat[1][3] && 
      m_mat[2][3] == other.m_mat[2][3] && m_mat[3][3] == other.m_mat[3][3] ;
} // end of operator == (const Matrix<T> & other) const.
 

/**
 * Not Equals binary operator.
 * @param  other const Matrix<T>&
 * @return bool
 */ 
template <class T> 
inline bool Matrix<T>::operator != (const Matrix<T> & other) const
{
  return 
      m_mat[0][0] != other.m_mat[0][0] || m_mat[1][0] != other.m_mat[1][0] || 
      m_mat[2][0] != other.m_mat[2][0] || m_mat[3][0] != other.m_mat[3][0] || 
      m_mat[0][1] != other.m_mat[0][1] || m_mat[1][1] != other.m_mat[1][1] || 
      m_mat[2][1] != other.m_mat[2][1] || m_mat[3][1] != other.m_mat[3][1] || 
      m_mat[0][2] != other.m_mat[0][2] || m_mat[1][2] != other.m_mat[1][2] || 
      m_mat[2][2] != other.m_mat[2][2] || m_mat[3][2] != other.m_mat[3][2] || 
      m_mat[0][3] != other.m_mat[0][3] || m_mat[1][3] != other.m_mat[1][3] || 
      m_mat[2][3] != other.m_mat[2][3] || m_mat[3][3] != other.m_mat[3][3] ;
} // end of operator != (const Matrix<T> & other) const.


/**
 * Unary plus operator.
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator +() const 
{ 
  return Matrix<T>(*this);
} // end of operator +() const.


/**
 * Unary minus operator.
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator -() const 
{
  return Matrix<T>( -m_mat[0][0], -m_mat[1][0], -m_mat[2][0], -m_mat[3][0],
                    -m_mat[0][1], -m_mat[1][1], -m_mat[2][1], -m_mat[3][1],
                    -m_mat[0][2], -m_mat[1][2], -m_mat[2][2], -m_mat[3][2],
                    -m_mat[0][3], -m_mat[1][3], -m_mat[2][3], -m_mat[3][3]);
}  // end of operator -() const.


/**
 * Binary plus operator.
 * @param other const Matrix<T> &
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator +(const Matrix<T> & other) const 
{
  return
    Matrix<T>
    (m_mat[0][0] + other.m_mat[0][0], m_mat[1][0] + other.m_mat[1][0], 
     m_mat[2][0] + other.m_mat[2][0], m_mat[3][0] + other.m_mat[3][0], 
     m_mat[0][1] + other.m_mat[0][1], m_mat[1][1] + other.m_mat[1][1], 
     m_mat[2][1] + other.m_mat[2][1], m_mat[3][1] + other.m_mat[3][1], 
     m_mat[0][2] + other.m_mat[0][2], m_mat[1][2] + other.m_mat[1][2], 
     m_mat[2][2] + other.m_mat[2][2], m_mat[3][2] + other.m_mat[3][2], 
     m_mat[0][3] + other.m_mat[0][3], m_mat[1][3] + other.m_mat[1][3], 
     m_mat[2][3] + other.m_mat[2][3], m_mat[3][3] + other.m_mat[3][3]);
} // end of operator +(const Matrix<T> & other) const.


/**
 * Binary minus operator.
 * @param other const Matrix<T> &
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator -(const Matrix<T> & other) const
{
  return
    Matrix<T>
    (m_mat[0][0] - other.m_mat[0][0], m_mat[1][0] - other.m_mat[1][0], 
     m_mat[2][0] - other.m_mat[2][0], m_mat[3][0] - other.m_mat[3][0], 
     m_mat[0][1] - other.m_mat[0][1], m_mat[1][1] - other.m_mat[1][1], 
     m_mat[2][1] - other.m_mat[2][1], m_mat[3][1] - other.m_mat[3][1], 
     m_mat[0][2] - other.m_mat[0][2], m_mat[1][2] - other.m_mat[1][2], 
     m_mat[2][2] - other.m_mat[2][2], m_mat[3][2] - other.m_mat[3][2], 
     m_mat[0][3] - other.m_mat[0][3], m_mat[1][3] - other.m_mat[1][3], 
     m_mat[2][3] - other.m_mat[2][3], m_mat[3][3] - other.m_mat[3][3]);
} // end of operator -(const Matrix<T> & other) const.


/**
 * Binary += operator.
 * @param other Matrix<T> &
 * @return Matrix<T> &
 */
template <class T> 
inline Matrix<T> & Matrix<T>::operator += (const Matrix<T> & other)
{	
  m_mat[0][0] += other.m_mat[0][0]; m_mat[1][0] += other.m_mat[1][0]; 
  m_mat[2][0] += other.m_mat[2][0]; m_mat[3][0] += other.m_mat[3][0]; 
  m_mat[0][1] += other.m_mat[0][1]; m_mat[1][1] += other.m_mat[1][1]; 
  m_mat[2][1] += other.m_mat[2][1]; m_mat[3][1] += other.m_mat[3][1]; 
  m_mat[0][2] += other.m_mat[0][2]; m_mat[1][2] += other.m_mat[1][2]; 
  m_mat[2][2] += other.m_mat[2][2]; m_mat[3][2] += other.m_mat[3][2]; 
  m_mat[0][3] += other.m_mat[0][3]; m_mat[1][3] += other.m_mat[1][3]; 
  m_mat[2][3] += other.m_mat[2][3]; m_mat[3][3] += other.m_mat[3][3];
  return *this;
} // end of operator += (const Matrix<T> & other).


/**
 * Binary -= operator.
 * @param other Matrix<T> &
 * @return Matrix<T> &
 */
template <class T> 
inline Matrix<T> & Matrix<T>::operator -=(const Matrix<T> & other) 
{	
  m_mat[0][0] -= other.m_mat[0][0]; m_mat[1][0] -= other.m_mat[1][0]; 
  m_mat[2][0] -= other.m_mat[2][0]; m_mat[3][0] -= other.m_mat[3][0]; 
  m_mat[0][1] -= other.m_mat[0][1]; m_mat[1][1] -= other.m_mat[1][1]; 
  m_mat[2][1] -= other.m_mat[2][1]; m_mat[3][1] -= other.m_mat[3][1]; 
  m_mat[0][2] -= other.m_mat[0][2]; m_mat[1][2] -= other.m_mat[1][2]; 
  m_mat[2][2] -= other.m_mat[2][2]; m_mat[3][2] -= other.m_mat[3][2]; 
  m_mat[0][3] -= other.m_mat[0][3]; m_mat[1][3] -= other.m_mat[1][3]; 
  m_mat[2][3] -= other.m_mat[2][3]; m_mat[3][3] -= other.m_mat[3][3];
  return *this;
} // end of operator -=(const Matrix<T> & other).


/**
 * Binary * operator: Multiplication of a Matrix<T> with a scalar of type T.
 * Scalar multiplication of all the elements of the matrix by the same scalar
 * @param scalar const T &
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator *(const T & scalar) const
{
  return Matrix<T>
    (m_mat[0][0] * scalar, m_mat[1][0] * scalar, 
     m_mat[2][0] * scalar, m_mat[3][0] * scalar,
     m_mat[0][1] * scalar, m_mat[1][1] * scalar, 
     m_mat[2][1] * scalar, m_mat[3][1] * scalar,
     m_mat[0][2] * scalar, m_mat[1][2] * scalar, 
     m_mat[2][2] * scalar, m_mat[3][2] * scalar,
     m_mat[0][3] * scalar, m_mat[1][3] * scalar, 
     m_mat[2][3] * scalar, m_mat[3][3] * scalar);
} // end of operator *(const T & scalar) const.


/**
 * Binary * operator:Multiplication of a Matrix<T> by a Point<T> .
 * This returns a Point after multiplying the matrix with the point
 * @param point const Point<T> &
 * @return Point<T>
 */
template <class T>  
inline Point<T> Matrix<T>::operator *(const Point<T> & point) const 
{ 
  return Point<T>
    ((m_mat[0][0]*point.m_x + m_mat[1][0]*point.m_y + 
      m_mat[2][0]*point.m_z + m_mat[3][0]),
     (m_mat[0][1]*point.m_x + m_mat[1][1]*point.m_y + 
      m_mat[2][1]*point.m_z + m_mat[3][1]),
     (m_mat[0][2]*point.m_x + m_mat[1][2]*point.m_y + 
      m_mat[2][2]*point.m_z + m_mat[3][2]));
} // end of operator *(const Point<T> & point) const.


/**
 * Binary * operator:Multiplication of a Matrix<T> by a Vector<T>.
 * This returns a Vector after multiplying the matrix with the point
 * @param vec const Vector<T> &
 * @return Vector<T>
 */
template <class T>  
inline Vector<T> Matrix<T>::operator *(const Vector<T> & vec) const 
{ 
  return Vector<T>
    ((m_mat[0][0]*vec.m_x + m_mat[1][0]*vec.m_y + m_mat[2][0]*vec.m_z),
     (m_mat[0][1]*vec.m_x + m_mat[1][1]*vec.m_y + m_mat[2][1]*vec.m_z),
     (m_mat[0][2]*vec.m_x + m_mat[1][2]*vec.m_y + m_mat[2][2]*vec.m_z));
} // end of operator *(const Vector<T> & vec) const.



/**
 * Binary * operator:Multiplication of a Matrix<T> by a Normal<T>.
 * This returns a Normal<T> after multiplying the lefthand 3x3 sub-matrix
 * @param normal const Normal<T> &
 * @return Normal<T>
 */
template <class T>  
inline Normal<T> Matrix<T>::operator *(const Normal<T> & normal) const 
{ 
  return Normal<T>
    ( (m_mat[0][0]*normal.X() + m_mat[1][0]*normal.Y() + 
       m_mat[2][0]*normal.Z()) ,
      (m_mat[0][1]*normal.X() + m_mat[1][1]*normal.Y() + 
       m_mat[2][1]*normal.Z()) ,
      (m_mat[0][2]*normal.X() + m_mat[1][2]*normal.Y() + 
       m_mat[2][2]*normal.Z()) );
} // end of operator *(const Normal<T> & normal) const.


/**
 * Binary * operator:Multiplication of a Matrix<T> by a Matrix<T>.
 * This returns a 4x4 Matrix<T> 
 * @param other const Matrix<T> &
 * @return Matrix<T>
 */
template <class T>  
inline Matrix<T> Matrix<T>::operator *(const Matrix<T> & other) const
{ 
  return Matrix<T>
    ( m_mat[0][0]*other.m_mat[0][0] + m_mat[1][0]*other.m_mat[0][1] + 
      m_mat[2][0]*other.m_mat[0][2] + m_mat[3][0]*other.m_mat[0][3],
      m_mat[0][0]*other.m_mat[1][0] + m_mat[1][0]*other.m_mat[1][1] + 
      m_mat[2][0]*other.m_mat[1][2] + m_mat[3][0]*other.m_mat[1][3],
      m_mat[0][0]*other.m_mat[2][0] + m_mat[1][0]*other.m_mat[2][1] + 
      m_mat[2][0]*other.m_mat[2][2] + m_mat[3][0]*other.m_mat[2][3],
      m_mat[0][0]*other.m_mat[3][0] + m_mat[1][0]*other.m_mat[3][1] + 
      m_mat[2][0]*other.m_mat[3][2] + m_mat[3][0]*other.m_mat[3][3],
		
      m_mat[0][1]*other.m_mat[0][0] + m_mat[1][1]*other.m_mat[0][1] + 
      m_mat[2][1]*other.m_mat[0][2] + m_mat[3][1]*other.m_mat[0][3],
      m_mat[0][1]*other.m_mat[1][0] + m_mat[1][1]*other.m_mat[1][1] + 
      m_mat[2][1]*other.m_mat[1][2] + m_mat[3][1]*other.m_mat[1][3],
      m_mat[0][1]*other.m_mat[2][0] + m_mat[1][1]*other.m_mat[2][1] + 
      m_mat[2][1]*other.m_mat[2][2] + m_mat[3][1]*other.m_mat[2][3],
      m_mat[0][1]*other.m_mat[3][0] + m_mat[1][1]*other.m_mat[3][1] + 
      m_mat[2][1]*other.m_mat[3][2] + m_mat[3][1]*other.m_mat[3][3],

      m_mat[0][2]*other.m_mat[0][0] + m_mat[1][2]*other.m_mat[0][1] + 
      m_mat[2][2]*other.m_mat[0][2] + m_mat[3][2]*other.m_mat[0][3],
      m_mat[0][2]*other.m_mat[1][0] + m_mat[1][2]*other.m_mat[1][1] + 
      m_mat[2][2]*other.m_mat[1][2] + m_mat[3][2]*other.m_mat[1][3],
      m_mat[0][2]*other.m_mat[2][0] + m_mat[1][2]*other.m_mat[2][1] + 
      m_mat[2][2]*other.m_mat[2][2] + m_mat[3][2]*other.m_mat[2][3],
      m_mat[0][2]*other.m_mat[3][0] + m_mat[1][2]*other.m_mat[3][1] + 
      m_mat[2][2]*other.m_mat[3][2] + m_mat[3][2]*other.m_mat[3][3],

      m_mat[0][3]*other.m_mat[0][0] + m_mat[1][3]*other.m_mat[0][1] + 
      m_mat[2][3]*other.m_mat[0][2] + m_mat[3][3]*other.m_mat[0][3],
      m_mat[0][3]*other.m_mat[1][0] + m_mat[1][3]*other.m_mat[1][1] + 
      m_mat[2][3]*other.m_mat[1][2] + m_mat[3][3]*other.m_mat[1][3],
      m_mat[0][3]*other.m_mat[2][0] + m_mat[1][3]*other.m_mat[2][1] + 
      m_mat[2][3]*other.m_mat[2][2] + m_mat[3][3]*other.m_mat[2][3],
      m_mat[0][3]*other.m_mat[3][0] + m_mat[1][3]*other.m_mat[3][1] + 
      m_mat[2][3]*other.m_mat[3][2] + m_mat[3][3]*other.m_mat[3][3]);
} // end of operator *(const Matrix<T> & other) const.


/**
 * Binary * operator:Multiplication of a Matrix<T> by a Scalar of type T.
 * This returns a reference to the resultant 4x4 Matrix<T> 
 * @param scalar const T &
 * @return Matrix<T> &
 */
template <class T> 
inline Matrix<T> & Matrix<T>::operator *=(const T & scalar)
{ 
  m_mat[0][0] *= scalar; m_mat[1][0] *= scalar; 
  m_mat[2][0] *= scalar; m_mat[3][0] *= scalar;
  m_mat[0][1] *= scalar; m_mat[1][1] *= scalar; 
  m_mat[2][1] *= scalar; m_mat[3][1] *= scalar;
  m_mat[0][2] *= scalar; m_mat[1][2] *= scalar; 
  m_mat[2][2] *= scalar; m_mat[3][2] *= scalar;
  m_mat[0][3] *= scalar; m_mat[1][3] *= scalar; 
  m_mat[2][3] *= scalar; m_mat[3][3] *= scalar;
  return *this;
} // end of operator *=(const T & scalar).


/**
 * Binary *= operator:Multiplication of a Matrix<T> by a Matrix<T>.
 * This returns a refernce to the resultant 4x4 Matrix<T> 
 * @param other const Matrix<T> &
 * @return Matrix<T> &
 */
template <class T>  
inline Matrix<T> & Matrix<T>::operator *=(const Matrix<T> & other)
{ 
  T a,b,c,d;
  a = m_mat[0][0]; b = m_mat[1][0]; c = m_mat[2][0]; d = m_mat[3][0];
  m_mat[0][0] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] + d*other.m_mat[0][3];
  m_mat[1][0] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] + d*other.m_mat[1][3];
  m_mat[2][0] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] + d*other.m_mat[2][3];
  m_mat[3][0] = a*other.m_mat[3][0] + b*other.m_mat[3][1] + 
                c*other.m_mat[3][2] + d*other.m_mat[3][3];
  a = m_mat[0][1]; b = m_mat[1][1]; c = m_mat[2][1]; d = m_mat[3][1];
  m_mat[0][1] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] + d*other.m_mat[0][3];
  m_mat[1][1] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] + d*other.m_mat[1][3];
  m_mat[2][1] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] + d*other.m_mat[2][3];
  m_mat[3][1] = a*other.m_mat[3][0] + b*other.m_mat[3][1] + 
                c*other.m_mat[3][2] + d*other.m_mat[3][3];
  a = m_mat[0][2]; b = m_mat[1][2]; c = m_mat[2][2]; d = m_mat[3][2];
  m_mat[0][2] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] + d*other.m_mat[0][3];
  m_mat[1][2] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] + d*other.m_mat[1][3];
  m_mat[2][2] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] + d*other.m_mat[2][3];
  m_mat[3][2] = a*other.m_mat[3][0] + b*other.m_mat[3][1] + 
                c*other.m_mat[3][2] + d*other.m_mat[3][3];
  a = m_mat[0][3]; b = m_mat[1][3]; c = m_mat[2][3]; d = m_mat[3][3];
  m_mat[0][3] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] + d*other.m_mat[0][3];
  m_mat[1][3] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] + d*other.m_mat[1][3];
  m_mat[2][3] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] + d*other.m_mat[2][3];
  m_mat[3][3] = a*other.m_mat[3][0] + b*other.m_mat[3][1] + 
                c*other.m_mat[3][2] + d*other.m_mat[3][3];
  return *this;
} // end of operator *=(const Matrix<T> & other).


/**
 * Binary / operator:Multiplication of a Matrix<T> by a Scalar of type T.
 * This returns a 4x4 Matrix<T> 
 * @param scalar const T &
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::operator /(const T & scalar) const
{
  if (scalar == 0)
  {
    return *this;
  }
  const float4 fReci = 1/scalar;
  return Matrix<T>
    (static_cast<T>(static_cast<float4>(m_mat[0][0]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[1][0]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[2][0]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[3][0]) * fReci),
     static_cast<T>(static_cast<float4>(m_mat[0][1]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[1][1]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[2][1]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[3][1]) * fReci),
     static_cast<T>(static_cast<float4>(m_mat[0][2]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[1][2]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[2][2]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[3][2]) * fReci),
     static_cast<T>(static_cast<float4>(m_mat[0][3]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[1][3]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[2][3]) * fReci), 
     static_cast<T>(static_cast<float4>(m_mat[3][3]) * fReci));
} // end of operator /(const T & scalar) const.


/**
 *Binary /= operator:Multiplication of a Matrix<T> by a Scalar of type T.
 *This returns a reference to the resultant 4x4 Matrix<T> 
 *@param scalar const T &
 *@return Matrix<T> &
 */
template <class T>  
inline Matrix<T> & Matrix<T>::operator /=(const T & scalar) 
{
  if (scalar==0)
	{
		return *this;
	} // endif (scalar==0)
  float4 fReci = 1/scalar;
  m_mat[0][0] = static_cast<T>(static_cast<float4>(m_mat[0][0]) * fReci);
  m_mat[1][0] = static_cast<T>(static_cast<float4>(m_mat[1][0]) * fReci); 
  m_mat[2][0] = static_cast<T>(static_cast<float4>(m_mat[2][0]) * fReci); 
  m_mat[3][0] = static_cast<T>(static_cast<float4>(m_mat[3][0]) * fReci);
  m_mat[0][1] = static_cast<T>(static_cast<float4>(m_mat[0][1]) * fReci); 
  m_mat[1][1] = static_cast<T>(static_cast<float4>(m_mat[1][1]) * fReci); 
  m_mat[2][1] = static_cast<T>(static_cast<float4>(m_mat[2][1]) * fReci); 
  m_mat[3][1] = static_cast<T>(static_cast<float4>(m_mat[3][1]) * fReci);
  m_mat[0][2] = static_cast<T>(static_cast<float4>(m_mat[0][2]) * fReci); 
  m_mat[1][2] = static_cast<T>(static_cast<float4>(m_mat[1][2]) * fReci); 
  m_mat[2][2] = static_cast<T>(static_cast<float4>(m_mat[2][2]) * fReci); 
  m_mat[3][2] = static_cast<T>(static_cast<float4>(m_mat[3][2]) * fReci);
  m_mat[0][3] = static_cast<T>(static_cast<float4>(m_mat[0][3]) * fReci); 
  m_mat[1][3] = static_cast<T>(static_cast<float4>(m_mat[1][3]) * fReci); 
  m_mat[2][3] = static_cast<T>(static_cast<float4>(m_mat[2][3]) * fReci); 
  m_mat[3][3] = static_cast<T>(static_cast<float4>(m_mat[3][3]) * fReci);
  return *this;
} // end of operator /=(const T & scalar).


/**
 * Changes the matrix to an identity matrix, returns a reference to that.
 * @return Matrix<T> &
 */
template <class T> 
inline Matrix<T> & Matrix<T>::Identity() 
{
    m_mat[0][0] = 1;  m_mat[1][0] = 0;  m_mat[2][0] = 0;  m_mat[3][0] = 0;
    m_mat[0][1] = 0;  m_mat[1][1] = 1;  m_mat[2][1] = 0;  m_mat[3][1] = 0;
    m_mat[0][2] = 0;  m_mat[1][2] = 0;  m_mat[2][2] = 1;  m_mat[3][2] = 0;
    m_mat[0][3] = 0;  m_mat[1][3] = 0;  m_mat[2][3] = 0;  m_mat[3][3] = 1;
    return *this;
} // end of Identity().


/**
 * Returns a matrix that is the transpose of this one.
 * @return Matrix<T>
 */
template <class T>  
inline Matrix<T> Matrix<T>::GetTranspose() const 
{ 
  return Matrix<T>
    (m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[0][3],
     m_mat[1][0], m_mat[1][1], m_mat[1][2], m_mat[1][3],
     m_mat[2][0], m_mat[2][1], m_mat[2][2], m_mat[2][3],
     m_mat[3][0], m_mat[3][1], m_mat[3][2], m_mat[3][3]);
} // end of GetTranspose() const .


/**
 * Reassigns its elements to become its transpose, returns a ref to itself.
 * @return Matrix<T> &
 */
template <class T>  
inline Matrix<T> & Matrix<T>::Transpose()
{ 
  T temp;
  temp=m_mat[0][1]; m_mat[0][1]=m_mat[1][0]; m_mat[1][0]=temp;
  temp=m_mat[0][2]; m_mat[0][2]=m_mat[2][0]; m_mat[2][0]=temp;
  temp=m_mat[0][3]; m_mat[0][3]=m_mat[3][0]; m_mat[3][0]=temp;

  temp=m_mat[1][2]; m_mat[1][2]=m_mat[2][1]; m_mat[2][1]=temp;
  temp=m_mat[1][3]; m_mat[1][3]=m_mat[3][1]; m_mat[3][1]=temp;

  temp=m_mat[2][3]; m_mat[2][3]=m_mat[3][2]; m_mat[3][2]=temp;
  return *this;
} // end of Transpose().


/**
 * Returns a scalar value of type T that is the 4x4 determinant of the matrix.
 * @return T
 */
template <class T>  
inline T Matrix<T>::GetDeterminant() const
{
  Matrix<T> adj = GetAdjoint();
  return
    + m_mat[0][0] * adj.m_mat[0][0] +
    + m_mat[1][0] * adj.m_mat[1][0] +
    + m_mat[2][0] * adj.m_mat[2][0] +
    + m_mat[3][0] * adj.m_mat[3][0];
} // end of GetDeterminant().


/**
 * Returns a scalar value of type T that is the value of a 3x3 determinant.
 * The values of the elements of the determinant are passed in x,y order
 * @param a T &
 * @param b T &
 * @param c T &
 * @param d T &
 * @param e T &
 * @param f T &
 * @param g T &
 * @param h T &
 * @param i T &
 * @return T
 */
template <class T>  
inline T Matrix<T>::GetDeterminant3x3(
        const T & a, const T & b, const T & c,
        const T & d, const T & e, const T & f,
        const T & g, const T & h, const T & i) const 
{
    return a*(e*i-f*h) - b*(d*i-f*g) + c*(d*h-e*g);
} // end of GetDeterminant3x3().


/**
 * Returns a scalar value of type T that is the value of a 2x2 determinant.
 * The values of the elements of the determinant are passed in x,y order
 * @param a T &
 * @param b T &
 * @param c T &
 * @param d T &
 * @return T
 */
template <class T> 
inline T Matrix<T>::GetDeterminant2x2(
  const T & a, const T & b, const T & c, const T & d)const
{
  return a*d - b*c;
} // end of GetDeterminant2x2().


/**
 * Returns the corresponding adjoint Matrix<T>.
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::GetAdjoint() const
{
  // rename for easier reference
  const T a=m_mat[0][0], b=m_mat[1][0], c=m_mat[2][0], d=m_mat[3][0];
  const T e=m_mat[0][1], f=m_mat[1][1], g=m_mat[2][1], h=m_mat[3][1];
  const T i=m_mat[0][2], j=m_mat[1][2], k=m_mat[2][2], l=m_mat[3][2];
  const T m=m_mat[0][3], n=m_mat[1][3], o=m_mat[2][3], p=m_mat[3][3];
  return Matrix<T> (+GetDeterminant3x3(f,g,h,j,k,l,n,o,p),
                    -GetDeterminant3x3(e,g,h,i,k,l,m,o,p),
                    +GetDeterminant3x3(e,f,h,i,j,l,m,n,p),
                    -GetDeterminant3x3(e,f,g,i,j,k,m,n,o),
                    -GetDeterminant3x3(b,c,d,j,k,l,n,o,p),
                    +GetDeterminant3x3(a,c,d,i,k,l,m,o,p),
                    -GetDeterminant3x3(a,b,d,i,j,l,m,n,p),
                    +GetDeterminant3x3(a,b,c,i,j,k,m,n,o),
                    +GetDeterminant3x3(b,c,d,f,g,h,n,o,p),
                    -GetDeterminant3x3(a,c,d,e,g,h,m,o,p),
                    +GetDeterminant3x3(a,b,d,e,f,h,m,n,p),
                    -GetDeterminant3x3(a,b,c,e,f,g,m,n,o),
                    -GetDeterminant3x3(b,c,d,f,g,h,j,k,l),
                    +GetDeterminant3x3(a,c,d,e,g,h,i,k,l),
                    -GetDeterminant3x3(a,b,d,e,f,h,i,j,l),
                    +GetDeterminant3x3(a,b,c,e,f,g,i,j,k));
} // end of GetAdjoint() const


/**
 * Returns the GetInverse of the Matrix<T> as a Matrix<T>.
 * @return Matrix<T>
 */
template <class T> 
inline Matrix<T> Matrix<T>::GetInverse() const 
{
  return GetAdjoint().GetTranspose() / GetDeterminant();
} // end of 


/**
 * Rotates the Matrix by a Normal<T>, and an angle of type T. The angle period is [0, 2Pi]
 * @param normal const Normal<T> &
 * @param angle const T &
 * @param const T
 */
template <class T> 
inline void Matrix<T>::Rotate(const Normal<T> & normal, const T & angle)
{
  Matrix<T> xform; 

  T c = cos(static_cast<float8>(angle));
  T s = sin(static_cast<float8>(angle));
  T x = normal.X();
  T y = normal.Y();
  T z = normal.Z();
  T x2 = x*x;
  T y2 = y*y;
  T z2 = z*z;

  xform.m_mat[0][0] = x2 + c*(1-x2);
  xform.m_mat[0][1] = x*y*(1-c) - z*s;
  xform.m_mat[0][2] = z*x*(1-c) + y*s;
  xform.m_mat[0][3] = 0.0;
  
  xform.m_mat[1][0] = x*y*(1-c) + z*s;
  xform.m_mat[1][1] = y2 + c*(1-y2);
  xform.m_mat[1][2] = y*z*(1-c) - x*s;
  xform.m_mat[1][3] = 0.0;
  
  xform.m_mat[2][0] = z*x*(1-c) - y*s;
  xform.m_mat[2][1] = y*z*(1-c) + x*s;
  xform.m_mat[2][2] = z2 + c*(1-z2);
  xform.m_mat[2][3] = 0.0;
  
  xform.m_mat[3][0] = 0.0;
  xform.m_mat[3][1] = 0.0;
  xform.m_mat[3][2] = 0.0;
  xform.m_mat[3][3] = 1.0;
  
  *this = xform * (*this);
} // end of Rotate(const Normal<T> & normal, const T & angle).


/**
 * Translates the Matrix<T> by a Vector<T>.
 * @param vec const Vector<T> &
 * @return Matrix<T> &
 */
template <class T>  
inline Matrix<T> & Matrix<T>::Translate(const Vector<T> & vec) 
{
  Matrix<T> xform;
  xform.m_mat[3][0] = vec.m_x; 
  xform.m_mat[3][1] = vec.m_y; 
  xform.m_mat[3][2] = vec.m_z;
  *this = (xform * (*this));
  return *this;
} // end of Translate(const Vector<T> & vec).


/**
 *Reassigns the matrix elements from an OpenGL Matrix.
 *OpenGL Matrix: an array with 16 elements.
 *@param oglArray const T *
 *The array must be previously allocated
 */
template <class T>
inline void	Matrix<T>::SetFromOpenGLMatrix(const T * pOpenGLMatrix)
{
    // The array pOpenGLMatrix must be previously allocated
    m_mat[0][0] = pOpenGLMatrix[0];  m_mat[1][0] = pOpenGLMatrix[4]; 
    m_mat[2][0] = pOpenGLMatrix[8];  m_mat[3][0] = pOpenGLMatrix[12];
    m_mat[0][1] = pOpenGLMatrix[1];  m_mat[1][1] = pOpenGLMatrix[5]; 
    m_mat[2][1] = pOpenGLMatrix[9];  m_mat[3][1] = pOpenGLMatrix[13];
    m_mat[0][2] = pOpenGLMatrix[2];  m_mat[1][2] = pOpenGLMatrix[6]; 
    m_mat[2][2] = pOpenGLMatrix[10]; m_mat[3][2] = pOpenGLMatrix[14];
    m_mat[0][3] = pOpenGLMatrix[3];  m_mat[1][3] = pOpenGLMatrix[7]; 
    m_mat[2][3] = pOpenGLMatrix[11]; m_mat[3][3] = pOpenGLMatrix[15];
} // end of GetIntoOglMatrix(const T * pOpenGLMatrix).


/**
 *Sets an OpenGL Matrix from the matrix elemnts.
 *OpenGL Matrix: an array with 16 elements.
 *@param oglArray const T *
 *The array must be previously allocated
 */
template <class T>
inline void	Matrix<T>::SetOpenGLMatrix(T * pOpenGLMatrix) const
{
    // The array pOpenGLMatrix must be previously allocated
    pOpenGLMatrix[0]  = m_mat[0][0]; pOpenGLMatrix[4]  = m_mat[1][0]; 
    pOpenGLMatrix[8]  = m_mat[2][0]; pOpenGLMatrix[12] = m_mat[3][0];
    pOpenGLMatrix[1]  = m_mat[0][1]; pOpenGLMatrix[5]  = m_mat[1][1]; 
    pOpenGLMatrix[9]  = m_mat[2][1]; pOpenGLMatrix[13] = m_mat[3][1];
    pOpenGLMatrix[2]  = m_mat[0][2]; pOpenGLMatrix[6]  = m_mat[1][2]; 
    pOpenGLMatrix[10] = m_mat[2][2]; pOpenGLMatrix[14] = m_mat[3][2];
    pOpenGLMatrix[3]  = m_mat[0][3]; pOpenGLMatrix[7]  = m_mat[1][3]; 
    pOpenGLMatrix[11] = m_mat[2][3]; pOpenGLMatrix[15] = m_mat[3][3];
} // end of SetFromOGLMatrix(const T * pOpenGLMatrix) const.


/**
 *Scales the Marix<T> by the Scaling Transform Matrix. 
 *sx, sy, sz of which is specified by the Vector<T>
 *@param vec const Vector<T> &
 *@return Matrix<T> &
 */

template <class T>  
inline Matrix<T> & Matrix<T>::Scale(const Vector<T> & vec)
{  
  Matrix<T> xform;
  xform.m_mat[0][0] = vec.m_x;
  xform.m_mat[1][1] = vec.m_y;
  xform.m_mat[2][2] = vec.m_z;
  *this = (xform * (*this));
  return *this;
} // end of Scale(const Vector<T> & vec).



/**
 * << Operator.
 * Directs the three data members to the ostream, reference of which is passed
 * AsOstream member function is called 
 * @param os ostream &
 * @param matrix Matrix<T> &
 * @return ostream &
 */  
template <class T> 
inline std::ostream & operator <<(std::ostream & os, const Matrix<T> & matrix)
{ 
  int4 iW=4; 
	os.precision(2);
  
  os<<"{(";
  os.width(iW); os<<matrix(0,0)<<","; 
  os.width(iW); os<<matrix(1,0)<<","; 
  os.width(iW); os<<matrix(2,0)<<","; 
  os.width(iW); os<<matrix(3,0)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,1)<<","; 
  os.width(iW); os<<matrix(1,1)<<","; 
  os.width(iW); os<<matrix(2,1)<<","; 
  os.width(iW); os<<matrix(3,1)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,2)<<","; 
  os.width(iW); os<<matrix(1,2)<<","; 
  os.width(iW); os<<matrix(2,2)<<","; 
  os.width(iW); os<<matrix(3,2)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,3)<<","; 
  os.width(iW); os<<matrix(1,3)<<","; 
  os.width(iW); os<<matrix(2,3)<<","; 
  os.width(iW); os<<matrix(3,3)<<")}";
 
  return os;
} // end of operator <<(ostream & os, const Matrix<T> & matrix).



/**
 * Test function for template class Matrix<T>.
 *
 * @param T type of the Matrix
 * @param os output stream
 * @return bool bSuccess
 */
template <class T>
bool Matrix<T>::TestByType(std::ostream & os) 
{
  static const int1 * const sBoolStr[] = {"    Oh No ! :-( ","   :-) "};
  bool bSuccess(true);
  bool bOk(true);

  Matrix<T> matrixA, matrixB(1,0,0,0, 0,2,0,0, 0,0,3,0, 0,0,0,1),matrixC;
  T scalarD = 2.5; 
  int4 iScalarI = 2;
  Vector<T> vec(2.5,2.5,2.5);
  T epsilon(T(0.001));

  os << endl <<"-Testing Matrix class for type: " << typeid(T).name();  
  os << endl <<"Matrix(4x4)";
  os << endl <<"size : " << sizeof(Matrix<T>) << " Bytes";
  os << endl <<"public member functions:";
  os << endl <<"Matrix<T> matrixA; = " << matrixA;
  os << endl <<"Matrix<T> matrixB; = "<< matrixB;

  // tests == operator
  os << endl <<"  (matrixA==matrixB): " << (matrixA==matrixB) <<"  (matrixA!=matrixB): " << (matrixA!=matrixB);
  bOk = (matrixA(0,0)==1 && matrixA(1,0)==0 && matrixA(2,0)==0 && matrixA(3,0)==0 && 
         matrixA(0,1)==0 && matrixA(1,1)==1 && matrixA(2,1)==0 && matrixA(3,1)==0 &&
         matrixA(0,2)==0 && matrixA(1,2)==0 && matrixA(2,2)==1 && matrixA(3,2)==0 &&
         matrixA(0,3)==0 && matrixA(1,3)==0 && matrixA(2,3)==0 && matrixA(3,3)==1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // tests = operator
  os <<"  matrixA=matrixB: " << (matrixA=matrixB);
  bOk = (matrixA(0,0)==1 && matrixA(1,0)==0 && matrixA(2,0)==0 && matrixA(3,0)==0 && 
         matrixA(0,1)==0 && matrixA(1,1)==2 && matrixA(2,1)==0 && matrixA(3,1)==0 &&
         matrixA(0,2)==0 && matrixA(1,2)==0 && matrixA(2,2)==3 && matrixA(3,2)==0 &&
         matrixA(0,3)==0 && matrixA(1,3)==0 && matrixA(2,3)==0 && matrixA(3,3)==1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // tests unary + operator
  os <<"  +matrixA= " << (+matrixA) ; 
  bOk = (matrixA(0,0)==1 && matrixA(1,0)==0 && matrixA(2,0)==0 && matrixA(3,0)==0 && 
         matrixA(0,1)==0 && matrixA(1,1)==2 && matrixA(2,1)==0 && matrixA(3,1)==0 &&
         matrixA(0,2)==0 && matrixA(1,2)==0 && matrixA(2,2)==3 && matrixA(3,2)==0 &&
         matrixA(0,3)==0 && matrixA(1,3)==0 && matrixA(2,3)==0 && matrixA(3,3)==1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // tests unary - operator
  os <<"  -matrixA= " << (matrixC=(-matrixA)) ; 
  bOk = (matrixC(0,0)==-1 && matrixC(1,0)==0 && matrixC(2,0)==0 && matrixC(3,0)==0 && 
         matrixC(0,1)==0 && matrixC(1,1)==-2 && matrixC(2,1)==0 && matrixC(3,1)==0 &&
         matrixC(0,2)==0 && matrixC(1,2)==0 && matrixC(2,2)==-3 && matrixC(3,2)==0 &&
         matrixC(0,3)==0 && matrixC(1,3)==0 && matrixC(2,3)==0 && matrixC(3,3)==-1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // tests += opermatrixAtor
  os <<"  matrixA+=matrixB: " << (matrixA+=matrixB);
  bOk = (matrixA(0,0)==2 && matrixA(1,0)==0 && matrixA(2,0)==0 && matrixA(3,0)==0 && 
         matrixA(0,1)==0 && matrixA(1,1)==4 && matrixA(2,1)==0 && matrixA(3,1)==0 &&
         matrixA(0,2)==0 && matrixA(1,2)==0 && matrixA(2,2)==6 && matrixA(3,2)==0 &&
         matrixA(0,3)==0 && matrixA(1,3)==0 && matrixA(2,3)==0 && matrixA(3,3)==2 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // tests -= operator
  os <<"  matrixA-=matrixB: " << (matrixA-=matrixB);
  bOk = (matrixA(0,0)==1 && matrixA(1,0)==0 && matrixA(2,0)==0 && matrixA(3,0)==0 && 
         matrixA(0,1)==0 && matrixA(1,1)==2 && matrixA(2,1)==0 && matrixA(3,1)==0 &&
         matrixA(0,2)==0 && matrixA(1,2)==0 && matrixA(2,2)==3 && matrixA(3,2)==0 &&
         matrixA(0,3)==0 && matrixA(1,3)==0 && matrixA(2,3)==0 && matrixA(3,3)==1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];
  // os <<"  T scalar(2.5)= " << iScalarI <<"  int i(2);";

  // tests copy constructor
  Matrix<T> matrixM(matrixA);
  os << "  matrixM(matrixA)= " << matrixM; 
  bOk = (matrixM(0,0)==1 && matrixM(1,0)==0 && matrixM(2,0)==0 && matrixM(3,0)==0 && 
         matrixM(0,1)==0 && matrixM(1,1)==2 && matrixM(2,1)==0 && matrixM(3,1)==0 &&
         matrixM(0,2)==0 && matrixM(1,2)==0 && matrixM(2,2)==3 && matrixM(3,2)==0 &&
         matrixM(0,3)==0 && matrixM(1,3)==0 && matrixM(2,3)==0 && matrixM(3,3)==1 );
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk] ;

  // test destructor
  Matrix<T> *matrixM2 = new Matrix<T>(matrixA);
  os << "  matrixM2: " << matrixM2 ;
  delete matrixM2;
  os << " deleted matrixM2.";
  bOk = 1;
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*scalar(doumatrixBle) operator
  os <<"  matrixM*scalarD= " << (matrixC=matrixM*scalarD);
  bOk = (matrixC(0,0)==1*scalarD && matrixC(1,0)==0 && matrixC(2,0)==0 && matrixC(3,0)==0 && 
         matrixC(0,1)==0 && matrixC(1,1)==2*scalarD && matrixC(2,1)==0 && matrixC(3,1)==0 &&
         matrixC(0,2)==0 && matrixC(1,2)==0 && matrixC(2,2)==3*scalarD && matrixC(3,2)==0 &&
         matrixC(0,3)==0 && matrixC(1,3)==0 && matrixC(2,3)==0 && matrixC(3,3)==1*scalarD );
  os << endl << sBoolStr[bOk];
  
  // tests matrix*scalar(int) operator
  os <<"  matrixM*iScalarI= " << (matrixC=matrixM*iScalarI);
  bOk = (matrixC(0,0)==1*iScalarI && matrixC(1,0)==0 && matrixC(2,0)==0 && matrixC(3,0)==0 && 
         matrixC(0,1)==0 && matrixC(1,1)==2*iScalarI && matrixC(2,1)==0 && matrixC(3,1)==0 &&
         matrixC(0,2)==0 && matrixC(1,2)==0 && matrixC(2,2)==3*iScalarI && matrixC(3,2)==0 &&
         matrixC(0,3)==0 && matrixC(1,3)==0 && matrixC(2,3)==0 && matrixC(3,3)==1*iScalarI );
  bSuccess = bSuccess && (bOk);
  os << endl  << sBoolStr[bOk];

  // tests *= operator
  os <<"  matrixM*=iScalarI= "<< (matrixM*=iScalarI);
  bOk = (matrixM(0,0)==1*iScalarI && matrixM(1,0)==0 && matrixM(2,0)==0 && matrixM(3,0)==0 && 
         matrixM(0,1)==0 && matrixM(1,1)==2*iScalarI && matrixM(2,1)==0 && matrixM(3,1)==0 &&
         matrixM(0,2)==0 && matrixM(1,2)==0 && matrixM(2,2)==3*iScalarI && matrixM(3,2)==0 &&
         matrixM(0,3)==0 && matrixM(1,3)==0 && matrixM(2,3)==0 && matrixM(3,3)==1*iScalarI );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests / operator
  Matrix<T> tmp4(matrixM/scalarD);
  os <<"  matrixM/scalarD= " << (matrixM/scalarD);
  bOk = 1;
  for (int4 s=0; s<4; s++)
	{
    for (int4 t=0; t<4; t++)
		{
      bOk = bOk && (tmp4(s,t)-matrixM(s,t)/scalarD)<epsilon;
    }
  }
  bSuccess = bSuccess && (bOk);
  os << endl  << sBoolStr[bOk];

  // tests /= operator
  os <<"  matrixM/=iScalarI= "<< (matrixM/=iScalarI);
  bOk = (matrixM(0,0)==1 && matrixM(1,0)==0 && matrixM(2,0)==0 && matrixM(3,0)==0 && 
         matrixM(0,1)==0 && matrixM(1,1)==2 && matrixM(2,1)==0 && matrixM(3,1)==0 &&
         matrixM(0,2)==0 && matrixM(1,2)==0 && matrixM(2,2)==3 && matrixM(3,2)==0 &&
         matrixM(0,3)==0 && matrixM(1,3)==0 && matrixM(2,3)==0 && matrixM(3,3)==1 );
  bSuccess = bSuccess && (bOk);
  os << endl  << sBoolStr[bOk];

  // tests matrix*vec 
  os <<"  matrixM*vec= " << (matrixM*vec);
  bOk = (matrixM*vec == Vector<T> (2.5,5,7.5));
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*point 
  matrixC = matrixM;
  Point<T> p(1,2,3);
  os <<"  matrixM*p= " << (matrixM*p);
  bOk = (matrixM*p == Point<T> (1,4,9));
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*normal
  matrixM = matrixC;
  Normal<T> n(1,2,3), ntmp;
  os <<"  matrixM*n= " << (ntmp=matrixM*n);
  bOk = (ntmp.X()-.1<epsilon,ntmp.Y()-.41<epsilon,ntmp.Z()-.91<epsilon);
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*=scalarD
  matrixM = matrixC;
  os <<"  matrixM*=scalarD: " << (matrixM*=scalarD);
  bOk = (matrixM(0,0)==2.5,matrixM(0,0)==0,matrixM(0,0)==0,matrixM(0,0)==0,
         matrixM(0,0)==0,matrixM(0,0)==5,matrixM(0,0)==0,matrixM(0,0)==0,
         matrixM(0,0)==0,matrixM(0,0)==0,matrixM(0,0)==7.5,matrixM(0,0)==0,
         matrixM(0,0)==0,matrixM(0,0)==0,matrixM(0,0)==0,matrixM(0,0)==2.5);
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests Translate 
  matrixM = matrixC;
  Vector<T> vector3(3,4,5);
  os <<"  matrixM.Translate(vector3)= " ;
  Matrix<T> tmp3(matrixM.Translate(vector3));
  os << tmp3 ;
  T pre = matrixM(0,1)*vector3.m_x + matrixM(0,2)*vector3.m_y + matrixM(0,2)*vector3.m_z ;
  T val1 = pre + matrixM(3,0);
  T val2 = pre + matrixM(3,1);
  T val3 = pre + matrixM(3,2);
  bOk = (((tmp3(3,0)-val1)<epsilon) && 
         ((tmp3(3,1)-val2)<epsilon) && 
         ((tmp3(3,2)-val3)<epsilon));
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*matrix 
  os <<"  matrixM*matrixB(matrix)= " << (matrixM*matrixB);
  Matrix<T> tmp1(matrixM*matrixB);
  bOk =((tmp1(0,0)-1)<epsilon &&  tmp1(0,1)==0 && 
         tmp1(0,2)==0         &&  tmp1(0,3)==0 &&
         tmp1(1,0)==0         && (tmp1(1,1)-4)<epsilon && 
         tmp1(1,2)==0         &&  tmp1(1,3)==0 &&
         tmp1(2,0)==0         &&  tmp1(2,1)==0 && 
        (tmp1(2,2)-9)<epsilon &&  tmp1(2,3)==0 &&
        (tmp1(3,0)-3)<epsilon && (tmp1(3,1)-4)<epsilon && 
        (tmp1(3,2)-5)<epsilon && (tmp1(3,3)-1)<epsilon);
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix*=matrix 
  os <<"  matrixM*=matrixM: " << (matrixM*=matrixM);
  bOk = (matrixM(0,0)==1 && matrixM(1,0)==0 && matrixM(2,0)==0 && matrixM(3,0)==6  && 
         matrixM(0,1)==0 && matrixM(1,1)==4 && matrixM(2,1)==0 && matrixM(3,1)==12 &&
         matrixM(0,2)==0 && matrixM(1,2)==0 && matrixM(2,2)==9 && matrixM(3,2)==20 &&
         matrixM(0,3)==0 && matrixM(1,3)==0 && matrixM(2,3)==0 && matrixM(3,3)==1 );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.GetTranspose()
  os <<"  matrixM.GetTranspose()= ";
  os << (matrixC=matrixM.GetTranspose());
  bOk = (matrixC(0,0)==1 && matrixC(1,0)==0  && matrixC(2,0)==0  && matrixC(3,0)==0 &&
         matrixC(0,1)==0 && matrixC(1,1)==4  && matrixC(2,1)==0  && matrixC(3,1)==0 &&
         matrixC(0,2)==0 && matrixC(1,2)==0  && matrixC(2,2)==9  && matrixC(3,2)==0 &&
         matrixC(0,3)==6 && matrixC(1,3)==12 && matrixC(2,3)==20 && matrixC(3,3)==1 );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.GetDeterminant()
  os <<"  matrixM.GetDeterminant()= ";
  bOk = (matrixM.GetDeterminant() == 36);
  os << matrixM.GetDeterminant();
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.GetInverse()
  os <<"  matrixM.GetInverse()= ";
  os << (matrixC=matrixM.GetInverse());
  bOk = ((matrixC(0,0)-1)<epsilon      && matrixC(0,1)==0 && 
          matrixC(0,2)==0              && matrixC(0,3)==0 && 
          matrixC(1,0)==0              && (matrixC(1,1)-0.2500)<epsilon && 
          matrixC(1,2)==0              && matrixC(1,3)==0 && 
          matrixC(2,0)==0              && matrixC(2,1)==0 && 
         (matrixC(2,2)-0.1111)<epsilon && matrixC(2,3)==0 && 
         (matrixC(3,0)-6)<epsilon      && (matrixC(3,1)-3)<epsilon && 
         (matrixC(3,2)-2.2222)<epsilon && (matrixC(3,3)-1)<epsilon );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.Scale(Vector<T>)
  Vector<T> u(-1,0,2);
  matrixM.Scale(Vector<T>(.5,.5,.5));
  os << "  u= " << u << "  matrixM*u= "<<(matrixM*u);
  bOk = ((matrixM*u) == Vector<T>(-0.5,0,9));
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];
  
  // test matrix.Transpose()
  matrixM = matrixM * 2;
  os << "  matrixM.Transpose()= ";
  matrixC = matrixM;
  os << (matrixM.Transpose());
  bOk = (matrixC(0,0)==1 && matrixC(1,0)==0  && matrixC(2,0)==0  && matrixC(3,0)==6  &&
         matrixC(0,1)==0 && matrixC(1,1)==4  && matrixC(2,1)==0  && matrixC(3,1)==12 &&
         matrixC(0,2)==0 && matrixC(1,2)==0  && matrixC(2,2)==9  && matrixC(3,2)==20 &&
         matrixC(0,3)==0 && matrixC(1,3)==0  && matrixC(2,3)==0  && matrixC(3,3)==2 );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // test matrix.Identity()
  os << "  matrixM.Identity()= ";
  os << (matrixM.Identity());
  bOk = (matrixM(0,0)==1 && matrixM(1,0)==0  && matrixM(2,0)==0  && matrixM(3,0)==0 &&
         matrixM(0,1)==0 && matrixM(1,1)==1  && matrixM(2,1)==0  && matrixM(3,1)==0 &&
         matrixM(0,2)==0 && matrixM(1,2)==0  && matrixM(2,2)==1  && matrixM(3,2)==0 &&
         matrixM(0,3)==0 && matrixM(1,3)==0  && matrixM(2,3)==0  && matrixM(3,3)==1 );
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.GetDeterminant3x3()
  os <<"  matrixM.GetDeterminant3x3(1,9,0, 2,7,1, 9,4,8)= ";
  bOk = (matrixM.GetDeterminant3x3(1,9,0, 2,7,1, 9,4,8) == -11);
  os << matrixM.GetDeterminant3x3(1,9,0, 2,7,1, 9,4,8);
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // tests matrix.GetDeterminant2x2()
  os <<"  matrixM.GetDeterminant2x2(7,2, 9,3)= ";
  bOk = (matrixM.GetDeterminant2x2(7,2, 9,3) == 3);
  os << matrixM.GetDeterminant2x2(7,2, 9,3);
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // test matrix.GetAdjoint
  matrixM = Matrix<T> (1,0,2,3, 2,3,4,6, 4,5,2,1, 3,2,1,1);
  os << "  matrixM.GetAdjoint()= " << (matrixC=matrixM.GetAdjoint());
  bOk = (matrixC == Matrix<T> (-3,14,-39, 20, 
                                3,-7, 18,-13,
                                3, 0,-24, 15,
                              -12, 0, 33,-18));
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // test matrix.Rotate(Normal<T>, <T>angle)
  Normal<T> n2(1,2,3);
  matrixC = matrixM; //saves values of m for later use
  os << "  matrixM.Rotate(n2,3.14)= ";
  matrixM.Rotate(n2, M_PI);
  os << matrixM;
  bOk = (matrixM(0,0)-1.43 < epsilon, matrixM(1,0)-3.0 < epsilon,
         matrixM(2,0)-.29  < epsilon, matrixM(3,0)+.429< epsilon, 
         matrixM(0,1)-2.86 < epsilon, matrixM(1,1)-3.0 < epsilon, 
         matrixM(2,1)-.571 < epsilon, matrixM(3,1)+.86 < epsilon, 
         matrixM(0,2)-3.3  < epsilon, matrixM(1,2)-4.0 < epsilon, 
         matrixM(2,2)-4.9  < epsilon, matrixM(3,2)-6.72< epsilon, 
         matrixM(0,3)-3.0  < epsilon, matrixM(1,3)-2.0 < epsilon, 
         matrixM(2,3)-1.0  < epsilon, matrixM(3,3)-1.0 < epsilon);
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];
  
  // test matrix.SetFromOpenGLMatrix
  T oglMatrix [16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  matrixM = matrixC; //resets value of matrixM
  matrixM.SetFromOpenGLMatrix(oglMatrix);
  os << "  matrixM.SetFromOpenGLMatrix(oglMatrix)= " 
     << matrixM;
  bOk = (matrixM == Matrix<T> (1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16));
  bSuccess = bSuccess && bOk;
  os << endl << sBoolStr[bOk];

  // test matrix.SetOpenGLMatrix
  T oglMatrix2 [16] ={0};
  matrixM.SetOpenGLMatrix(oglMatrix2);
  os << "  matrixM.SetOpenGLMatrix(oglMatrix2): " 
     << " openGlMatrix:{ ";
  bOk = 1;
  for (int4 index=0; index<16; index++)
	{
    bOk = bOk && oglMatrix2[index]==(index+1);
    os << oglMatrix2[index] << " ";
  }// endfor (int index=0; index<16; index++)
  os << "}";
  bOk = (matrixM == Matrix<T> (1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16));
  bSuccess = bSuccess && (bOk);
  os << endl << sBoolStr[bOk];

  // test size of Matrix<T>
  os << "  (4*4) * sizeof(T): " << sizeof(T);
  os << " == size of Matrix<T>: " << sizeof(Matrix<T>) << endl;
  bOk = ( ((4*4) * sizeof(T)) == sizeof(Matrix<T>) );
  bSuccess = bSuccess && (bOk);
  os << sBoolStr[bOk];

  os << endl;

  // end of tests, return whether bSuccessful or not.
  if (bSuccess) 
	{
    os << "Test of Matrix class is a success!" << endl;
  } 
	else 
	{
    os << "Test of Matrix class is NOT a success" << endl;
  } //endif (bSuccess)

  return bSuccess;
}



// $Log: Matrix.inl,v $
// Revision 1.2  2006/01/18 20:35:35  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/08/20 15:48:17  michael
// using M_PI instead of typed in values for it
//
// Revision 3.7  2003/05/16 11:43:46  michael
// code review
//
// Revision 3.6  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 3.5  2003/01/22 23:07:06  ingmar
// added missing includes
//
// Revision 3.4  2003/01/22 20:39:31  ingmar
// removed duplicate operator * (Vector3D) , as Vector and Vector3D are now the same
//
// Revision 3.3  2002/07/05 21:43:07  ingmar
// added rotation for Vector3D
//
// Revision 3.2  2002/07/05 21:10:04  ingmar
// added comment to explain the input range expected for the rotation angle
//
// Revision 3.1  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
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
// Revision 1.10  2001/06/28 20:18:05  liwei
// Fixed the name coflict with std::vector
//
// Revision 1.9  2001/05/31 21:32:48  ana
// changes regarding code review
//
// Revision 1.8  2001/05/08 18:21:07  ana
// Modifications after code review
//
// Revision 1.7  2001/05/01 23:03:28  ana
// code standards
//
// Revision 1.6  2001/04/30 13:59:12  ana
// changes to fit in code standards
//
// Revision 1.5  2001/04/24 23:46:53  ana
// modifications to fit in code standards
//
// Revision 1.4  2001/04/17 22:28:18  ana
// added TestByType function
//
// Revision 1.3  2001/04/09 18:03:20  dmitry
// Removed Revision History from Top of File
//
// Revision 1.2  2001/03/23 20:08:52  dmitry
// Updated datatypes as declared in Typedef.h
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 10    8/29/00 3:03p Frank
// Re-fixed namespace collision with std::vec
// 
// 9     8/17/00 1:02p Antonio
// modified header
// 
// *****************  Version 8  *****************
// User: Antonio      Date: 8/10/00    Time: 4:21p
// Updated in $/vx/src/Math
// modified header to newer version.
// 
// *****************  Version 7  *****************
// User: Antonio      Date: 8/08/00    Time: 3:01p
// Updated in $/vx/src/Math
// renamed GetIntoOpenGLMatirx->SetFromOpenGLMatrix,
//                SetFromOpenGLMatrix->SetOpenGLMatrix.
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/07/00    Time: 10:51p
// Updated in $/vx/src/Math
// removed some dead code
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/02/00    Time: 9:18p
// Updated in $/vx/src/Math
// added complete history to end of file
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/01/00  Time:  3:03p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 3  *****************
// User: Frank           Date:  7/25/00  Time:  3:04p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Fixed namespace collision with std::vec
// 
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:55p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Removed "using namespace *" from header files to prevent ammBiguities
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Matrix.inl
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Matrix.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
// $Id: Matrix.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
