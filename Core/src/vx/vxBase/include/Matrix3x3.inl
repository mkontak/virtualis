// $Id: Matrix3x3.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


/**
 * Default Constructor, creates an identity matrix.
 */
template <class T> 
Matrix3x3<T>::Matrix3x3<T>()
//*************************************************************
{
  m_mat[0][0] = 1;  m_mat[1][0] = 0;  m_mat[2][0] = 0;  
  m_mat[0][1] = 0;  m_mat[1][1] = 1;  m_mat[2][1] = 0;  
  m_mat[0][2] = 0;  m_mat[1][2] = 0;  m_mat[2][2] = 1;  
} // default constructor



/**
 * Constructor takes 3 vectors
 * @param vector1
 * @param vector2
 * @param vector3
 */
template <class T> 
Matrix3x3<T>::Matrix3x3<T>(Vector<T> vector1, Vector<T> vector2, Vector<T> vector3)
//*************************************************************
{
  m_mat[0][0] = vector1.m_x; m_mat[1][0] = vector2.m_x; m_mat[2][0] = vector3.m_x;
  m_mat[0][1] = vector1.m_y; m_mat[1][1] = vector2.m_y; m_mat[2][1] = vector3.m_y;
  m_mat[0][2] = vector1.m_z; m_mat[1][2] = vector2.m_z; m_mat[2][2] = vector3.m_z;
}

/**
 * Constructor takes the 9 values in x,y order (Euclidean Co-ords)
 * @param m_mat[0][0] = ax.
 * @param m_mat[1][0] = bx.
 * @param m_mat[2][0] = cx.
 * @param m_mat[0][1] = ay.
 * @param m_mat[1][1] = by.
 * @param m_mat[2][1] = cy.
 * @param m_mat[0][2] = az.
 * @param m_mat[1][2] = bz.
 * @param m_mat[2][2] = cz.
 * @param @param ax const T &
 * @param @param bx const T &
 * @param @param cx const T &
 * @param @param ay const T &
 * @param @param by const T &
 * @param @param cy const T &
 * @param @param az const T &
 * @param @param bz const T &
 * @param @param cz const T &
 */
template <class T> 
Matrix3x3<T>::Matrix3x3<T>(const T & ax, const T & bx, const T & cx, 
                           const T & ay, const T & by, const T & cy, 
                           const T & az, const T & bz, const T & cz) 
//*************************************************************
{
    m_mat[0][0] = ax;  m_mat[1][0] = bx;  m_mat[2][0] = cx; 
    m_mat[0][1] = ay;  m_mat[1][1] = by;  m_mat[2][1] = cy;  
    m_mat[0][2] = az;  m_mat[1][2] = bz;  m_mat[2][2] = cz; 
} // constructor


/**
 * Copy constructor
 * @param other const Matrix3x3<T> &
 */
template <class T> 
Matrix3x3<T>::Matrix3x3<T>(const Matrix3x3<T> & other)
//*************************************************************
{
  *this = other
} // copy constructor


/**
 * Assignment Operator overloaded for template class Matrix3x3<T>.
 * @param other const Matrix3x3<T> &
 */
template <class T> 
inline Matrix3x3<T> & Matrix3x3<T>::operator=(const Matrix3x3<T> & other)
//*************************************************************
{
  if (this != &other)
  {
    m_mat[0][0] = other.m_mat[0][0]; m_mat[1][0] = other.m_mat[1][0]; 
    m_mat[2][0] = other.m_mat[2][0]; 
    m_mat[0][1] = other.m_mat[0][1]; m_mat[1][1] = other.m_mat[1][1]; 
    m_mat[2][1] = other.m_mat[2][1]; 
    m_mat[0][2] = other.m_mat[0][2]; m_mat[1][2] = other.m_mat[1][2]; 
    m_mat[2][2] = other.m_mat[2][2]; 
  }

  return *this;
} // operator=()


/**
 * Operator(): Reassigns the matrix elemnts to the 9 sclalars passed.
 * Takes 9 parameters of type T 
 * assigns them to the matrix elements (x,y) order
 * for more details see the copy constructor
 * @param x const T &
 * @param y const T &
 * @return T &
 */
template <class T> 
inline T const & Matrix3x3<T>::operator()(const unsigned char & x, 
                                          const unsigned char & y) const
//*************************************************************
{
  return m_mat[x][y];
} // operator() 


/**
 * Equals binary operator.
 * @param  other const Matrix3x3<T>&
 * @return bool
 */
template <class T> 
inline bool Matrix3x3<T>::operator == (const Matrix3x3<T> & other) const
//*************************************************************
{	
  return m_mat[0][0] == other.m_mat[0][0] && m_mat[1][0] == other.m_mat[1][0] && 
         m_mat[2][0] == other.m_mat[2][0] && 
         m_mat[0][1] == other.m_mat[0][1] && m_mat[1][1] == other.m_mat[1][1] && 
         m_mat[2][1] == other.m_mat[2][1] && 
         m_mat[0][2] == other.m_mat[0][2] && m_mat[1][2] == other.m_mat[1][2] && 
         m_mat[2][2] == other.m_mat[2][2] && ;
} // operator==()
 

/**
 * Not Equals binary operator.
 * @param  other const Matrix3x3<T>&
 * @return bool
 */ 
template <class T> 
inline bool Matrix3x3<T>::operator != (const Matrix3x3<T> & other) const
//*************************************************************
{
  return m_mat[0][0] != other.m_mat[0][0] || m_mat[1][0] != other.m_mat[1][0] || 
         m_mat[2][0] != other.m_mat[2][0] ||
         m_mat[0][1] != other.m_mat[0][1] || m_mat[1][1] != other.m_mat[1][1] || 
         m_mat[2][1] != other.m_mat[2][1] || 
         m_mat[0][2] != other.m_mat[0][2] || m_mat[1][2] != other.m_mat[1][2] || 
         m_mat[2][2] != other.m_mat[2][2];
} // operator!=()


/**
 * Unary plus operator
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator +() const 
//*************************************************************
{ 
  return Matrix3x3<T>(*this);
} // operator+()


/**
 * Unary minus operator
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator -() const 
//*************************************************************
{
  return Matrix3x3<T>( -m_mat[0][0], -m_mat[1][0], -m_mat[2][0], 
                       -m_mat[0][1], -m_mat[1][1], -m_mat[2][1], 
                       -m_mat[0][2], -m_mat[1][2], -m_mat[2][2]);
} // operator-()


/**
 * Binary plus operator
 * @param other const Matrix3x3<T> &
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T> & other) const 
//*************************************************************
{
  return Matrix3x3<T> (m_mat[0][0] + other.m_mat[0][0], m_mat[1][0] + other.m_mat[1][0], 
                       m_mat[2][0] + other.m_mat[2][0],
                       m_mat[0][1] + other.m_mat[0][1], m_mat[1][1] + other.m_mat[1][1], 
                       m_mat[2][1] + other.m_mat[2][1], 
                       m_mat[0][2] + other.m_mat[0][2], m_mat[1][2] + other.m_mat[1][2], 
                       m_mat[2][2] + other.m_mat[2][2]);
} // operator+()


/**
 * Binary minus operator.
 * @param other const Matrix3x3<T> &
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T> & other) const
//*************************************************************
{
  return Matrix3x3<T> (m_mat[0][0] - other.m_mat[0][0], m_mat[1][0] - other.m_mat[1][0], 
                       m_mat[2][0] - other.m_mat[2][0], 
                       m_mat[0][1] - other.m_mat[0][1], m_mat[1][1] - other.m_mat[1][1], 
                       m_mat[2][1] - other.m_mat[2][1], 
                       m_mat[0][2] - other.m_mat[0][2], m_mat[1][2] - other.m_mat[1][2], 
                       m_mat[2][2] - other.m_mat[2][2]);
} // operator-()


/**
 * Binary += operator.
 * @param other Matrix3x3<T> &
 * @return Matrix3x3<T> &
 */
template <class T> 
inline Matrix3x3<T> & Matrix3x3<T>::operator += (const Matrix3x3<T> & other)
//*************************************************************
{	
  m_mat[0][0] += other.m_mat[0][0]; m_mat[1][0] += other.m_mat[1][0]; 
  m_mat[2][0] += other.m_mat[2][0];
  m_mat[0][1] += other.m_mat[0][1]; m_mat[1][1] += other.m_mat[1][1]; 
  m_mat[2][1] += other.m_mat[2][1]; 
  m_mat[0][2] += other.m_mat[0][2]; m_mat[1][2] += other.m_mat[1][2]; 
  m_mat[2][2] += other.m_mat[2][2];

  return *this;
} // operator+=()


/**
 * Binary -= operator.
 * @param other Matrix3x3<T> &
 * @return Matrix3x3<T> &
 */
template <class T> 
inline Matrix3x3<T> & Matrix3x3<T>::operator -=(const Matrix3x3<T> & other) 
//*************************************************************
{	
  m_mat[0][0] -= other.m_mat[0][0]; m_mat[1][0] -= other.m_mat[1][0]; 
  m_mat[2][0] -= other.m_mat[2][0]; 
  m_mat[0][1] -= other.m_mat[0][1]; m_mat[1][1] -= other.m_mat[1][1]; 
  m_mat[2][1] -= other.m_mat[2][1]; 
  m_mat[0][2] -= other.m_mat[0][2]; m_mat[1][2] -= other.m_mat[1][2]; 
  m_mat[2][2] -= other.m_mat[2][2];

  return *this;
} // operator-=()


/**
 * Binary * operator: Multiplication of a Matrix3x3<T> with a scalar of type T.
 * Scalar multiplication of all the elements of the matrix by the same scalar
 * @param scalar const T &
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator *(const T & scalar) const
//*************************************************************
{
  return Matrix3x3<T> (m_mat[0][0] * scalar, m_mat[1][0] * scalar, 
                       m_mat[2][0] * scalar,
                       m_mat[0][1] * scalar, m_mat[1][1] * scalar, 
                       m_mat[2][1] * scalar, 
                       m_mat[0][2] * scalar, m_mat[1][2] * scalar, 
                       m_mat[2][2] * scalar);
} // operator*()


/**
 * Binary * operator:Multiplication of a Matrix3x3<T> by a Point<T> .
 * This returns a Point after multiplying the matrix with the point
 * @param point const Point<T> &
 * @return Point<T>
 */
template <class T>  
inline Point<T> Matrix3x3<T>::operator *(const Point<T> & point) const 
//*************************************************************
{ 
  return Point<T> ((m_mat[0][0]*point.m_x + m_mat[1][0]*point.m_y + 
                    m_mat[2][0]*point.m_z ),
                   (m_mat[0][1]*point.m_x + m_mat[1][1]*point.m_y + 
                    m_mat[2][1]*point.m_z ),
                   (m_mat[0][2]*point.m_x + m_mat[1][2]*point.m_y + 
                    m_mat[2][2]*point.m_z ));
} // operator*()


/**
 * Binary * operator:Multiplication of a Matrix3x3<T> by a Vector<T>.
 * This returns a Vector after multiplying the matrix with the point
 * @param vec const Vector<T> &
 * @return Vector<T>
 */
template <class T>  
inline Vector<T> Matrix3x3<T>::operator *(const Vector<T> & vec) const 
//*************************************************************
{
  return Vector<T> ((m_mat[0][0]*vec.m_x + m_mat[1][0]*vec.m_y + 
                     m_mat[2][0]*vec.m_z),
                     (m_mat[0][1]*vec.m_x + m_mat[1][1]*vec.m_y + 
                      m_mat[2][1]*vec.m_z),
                     (m_mat[0][2]*vec.m_x + m_mat[1][2]*vec.m_y + 
                      m_mat[2][2]*vec.m_z));
} // operator*()


/**
 * Binary * operator:Multiplication of a Matrix3x3<T> by a Normal<T>.
 * This returns a Normal<T> after multiplying the lefthand 3x3 sub-matrix
 * @param normal const Normal<T> &
 * @return Normal<T>
 */
template <class T>  
inline Normal<T> Matrix3x3<T>::operator *(const Normal<T> & normal) const 
//*************************************************************
{
  return Normal<T> ( (m_mat[0][0]*normal.X() + m_mat[1][0]*normal.Y() + 
                      m_mat[2][0]*normal.Z()) ,
                      (m_mat[0][1]*normal.X() + m_mat[1][1]*normal.Y() + 
                       m_mat[2][1]*normal.Z()) ,
                      (m_mat[0][2]*normal.X() + m_mat[1][2]*normal.Y() + 
                       m_mat[2][2]*normal.Z()) );
} // operator*()


/**
 * Binary * operator:Multiplication of a Matrix3x3<T> by a Matrix3x3<T>.
 * This returns a 3x3 Matrix3x3<T> 
 * @param other const Matrix3x3<T> &
 * @return Matrix3x3<T>
 */
template <class T>  
inline Matrix3x3<T> Matrix3x3<T>::operator *(const Matrix3x3<T> & other) const
//*************************************************************
{
  return Matrix3x3<T> ( m_mat[0][0]*other.m_mat[0][0] + m_mat[1][0]*other.m_mat[0][1] + 
                        m_mat[2][0]*other.m_mat[0][2] ,
                         m_mat[0][0]*other.m_mat[1][0] + m_mat[1][0]*other.m_mat[1][1] + 
                         m_mat[2][0]*other.m_mat[1][2] ,
                         m_mat[0][0]*other.m_mat[2][0] + m_mat[1][0]*other.m_mat[2][1] + 
                         m_mat[2][0]*other.m_mat[2][2] ,
		
                         m_mat[0][1]*other.m_mat[0][0] + m_mat[1][1]*other.m_mat[0][1] + 
                         m_mat[2][1]*other.m_mat[0][2] ,
                         m_mat[0][1]*other.m_mat[1][0] + m_mat[1][1]*other.m_mat[1][1] + 
                         m_mat[2][1]*other.m_mat[1][2] ,
                         m_mat[0][1]*other.m_mat[2][0] + m_mat[1][1]*other.m_mat[2][1] + 
                         m_mat[2][1]*other.m_mat[2][2] ,

                         m_mat[0][2]*other.m_mat[0][0] + m_mat[1][2]*other.m_mat[0][1] + 
                         m_mat[2][2]*other.m_mat[0][2] ,
                         m_mat[0][2]*other.m_mat[1][0] + m_mat[1][2]*other.m_mat[1][1] + 
                         m_mat[2][2]*other.m_mat[1][2] ,
                         m_mat[0][2]*other.m_mat[2][0] + m_mat[1][2]*other.m_mat[2][1] + 
                         m_mat[2][2]*other.m_mat[2][2] );
} // operator*()


/**
 * Binary * operator:Multiplication of a Matrix3x3<T> by a Scalar of type T.
 * This returns a reference to the resultant 3x3 Matrix3x3<T> 
 * @param scalar const T &
 * @return Matrix3x3<T> &
 */
template <class T> 
inline Matrix3x3<T> & Matrix3x3<T>::operator *=(const T & scalar)
//*************************************************************
{
  m_mat[0][0] *= scalar; m_mat[1][0] *= scalar; 
  m_mat[2][0] *= scalar; 
  m_mat[0][1] *= scalar; m_mat[1][1] *= scalar; 
  m_mat[2][1] *= scalar;
  m_mat[0][2] *= scalar; m_mat[1][2] *= scalar; 
  m_mat[2][2] *= scalar;

  return *this;
} // operator*=()


/**
 * Binary *= operator:Multiplication of a Matrix3x3<T> by a Matrix3x3<T>.
 * This returns a refernce to the resultant 3x3 Matrix3x3<T> 
 * @param other const Matrix3x3<T> &
 * @return Matrix3x3<T> &
 */
template <class T>  
inline Matrix3x3<T> & Matrix3x3<T>::operator *=(const Matrix3x3<T> & other)
//*************************************************************
{ 
  T a,b,c,d;

  a = m_mat[0][0]; 
  b = m_mat[1][0]; 
  c = m_mat[2][0]; 
  m_mat[0][0] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] ;
  m_mat[1][0] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] ;
  m_mat[2][0] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] ;

  a = m_mat[0][1]; 
  b = m_mat[1][1]; 
  c = m_mat[2][1]; 
  m_mat[0][1] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] ;
  m_mat[1][1] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] ;
  m_mat[2][1] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] ;

  a = m_mat[0][2]; 
  b = m_mat[1][2]; 
  c = m_mat[2][2]; 
  m_mat[0][2] = a*other.m_mat[0][0] + b*other.m_mat[0][1] + 
                c*other.m_mat[0][2] ;
  m_mat[1][2] = a*other.m_mat[1][0] + b*other.m_mat[1][1] + 
                c*other.m_mat[1][2] ;
  m_mat[2][2] = a*other.m_mat[2][0] + b*other.m_mat[2][1] + 
                c*other.m_mat[2][2] ;
  return *this;
} // operator*=()


/**
 * Binary / operator:Multiplication of a Matrix3x3<T> by a Scalar of type T.
 * This returns a 3x3 Matrix3x3<T> 
 * @param scalar const T &
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::operator /(const T & scalar) const
//*************************************************************
{
  if (scalar == 0)
  {
    return *this;
  }
  const float4 fReci(1/scalar);

  return Matrix3x3<T> (static_cast<T>(static_cast<float4>(m_mat[0][0]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[1][0]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[2][0]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[0][1]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[1][1]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[2][1]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[0][2]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[1][2]) * fReci), 
                       static_cast<T>(static_cast<float4>(m_mat[2][2]) * fReci));
} // operator/()


/**
 * Binary /= operator:Multiplication of a Matrix3x3<T> by a Scalar of type T.
 * This returns a reference to the resultant 3x3 Matrix3x3<T> 
 * @param scalar const T &
 * @return Matrix3x3<T> &
 */
template <class T>  
inline Matrix3x3<T> & Matrix3x3<T>::operator /=(const T & scalar) 
//*************************************************************
{
  if (scalar==0)
	{
		return *this;
	}

  float4 fReci(1/scalar);
  m_mat[0][0] = static_cast<T>(static_cast<float4>(m_mat[0][0]) * fReci);
  m_mat[1][0] = static_cast<T>(static_cast<float4>(m_mat[1][0]) * fReci); 
  m_mat[2][0] = static_cast<T>(static_cast<float4>(m_mat[2][0]) * fReci); 
  m_mat[0][1] = static_cast<T>(static_cast<float4>(m_mat[0][1]) * fReci); 
  m_mat[1][1] = static_cast<T>(static_cast<float4>(m_mat[1][1]) * fReci); 
  m_mat[2][1] = static_cast<T>(static_cast<float4>(m_mat[2][1]) * fReci); 
  m_mat[0][2] = static_cast<T>(static_cast<float4>(m_mat[0][2]) * fReci); 
  m_mat[1][2] = static_cast<T>(static_cast<float4>(m_mat[1][2]) * fReci); 
  m_mat[2][2] = static_cast<T>(static_cast<float4>(m_mat[2][2]) * fReci); 
  return *this;
} // operator/=()


/**
 * Changes the matrix to an identity matrix, returns a reference to that.
 * @return Matrix3x3<T> &
 */
template <class T> 
inline Matrix3x3<T> & Matrix3x3<T>::Identity() 
//*************************************************************
{
  m_mat[0][0] = 1;  m_mat[1][0] = 0;  m_mat[2][0] = 0;  
  m_mat[0][1] = 0;  m_mat[1][1] = 1;  m_mat[2][1] = 0; 
  m_mat[0][2] = 0;  m_mat[1][2] = 0;  m_mat[2][2] = 1; 

  return *this;
} // Identity()


/**
 * Returns a matrix that is the transpose of this one.
 * @return Matrix3x3<T>
 */
template <class T>  
inline Matrix3x3<T> Matrix3x3<T>::GetTranspose() const 
//*************************************************************
{ 
  return Matrix3x3<T> (m_mat[0][0], m_mat[0][1], m_mat[0][2],
                       m_mat[1][0], m_mat[1][1], m_mat[1][2],
                       m_mat[2][0], m_mat[2][1], m_mat[2][2]);
} // GetTranspose()


/**
 * Reassigns its elements to become its transpose, returns a ref to itself.
 * @return Matrix3x3<T> &
 */
template <class T>  
inline Matrix3x3<T> & Matrix3x3<T>::Transpose()
//*************************************************************
{ 
  T temp;

  temp = m_mat[0][1]; m_mat[0][1] = m_mat[1][0]; m_mat[1][0] = temp;
  temp = m_mat[0][2]; m_mat[0][2] = m_mat[2][0]; m_mat[2][0] = temp;
  temp = m_mat[1][2]; m_mat[1][2] = m_mat[2][1]; m_mat[2][1] = temp;

  return *this;
} // end of Transpose().


/**
 * Returns a scalar value of type T that is the  determinant of the matrix.
 * @return T
 */
template <class T>  
inline T Matrix3x3<T>::GetDeterminant() const
//*************************************************************
{
  return (m_mat[0][0]*(m_mat[1][1]*m_mat[2][2] - m_mat[2][1]*m_mat[1][2]) +
	        m_mat[1][0]*(m_mat[2][1]*m_mat[0][2] - m_mat[0][1]*m_mat[2][2]) +
  	      m_mat[2][0]*(m_mat[0][1]*m_mat[1][2] - m_mat[1][1]*m_mat[0][2]));
} // GetDeterminant()


/**
 * Returns the corresponding adjoint Matrix3x3<T>.
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::GetAdjoint() const
//*************************************************************
{
  return *(new Matrix3x3<T>(m_mat[1][1]*m_mat[2][2] - m_mat[2][1]*m_mat[1][2],
                            m_mat[2][0]*m_mat[1][2] - m_mat[1][0]*m_mat[2][2],
		                        m_mat[1][0]*m_mat[2][1] - m_mat[2][0]*m_mat[1][1],
                        		m_mat[2][1]*m_mat[0][2] - m_mat[0][1]*m_mat[2][2],
		                        m_mat[0][0]*m_mat[2][2] - m_mat[2][0]*m_mat[0][2],
		                        m_mat[2][0]*m_mat[0][1] - m_mat[0][0]*m_mat[2][1],
		                        m_mat[0][1]*m_mat[1][2] - m_mat[1][1]*m_mat[0][2],
		                        m_mat[1][0]*m_mat[0][2] - m_mat[0][0]*m_mat[1][2],
		                        m_mat[0][0]*m_mat[1][1] - m_mat[1][0]*m_mat[0][1]));
} // GetAdjoint()


/**
 * Returns the GetInverse of the Matrix3x3<T> as a Matrix3x3<T>.
 * @return Matrix3x3<T>
 */
template <class T> 
inline Matrix3x3<T> Matrix3x3<T>::GetInverse() const 
//*************************************************************
{
  float m11(m_mat[1][1]*m_mat[2][2] - m_mat[2][1]*m_mat[1][2]);
  float m12(m_mat[2][0]*m_mat[1][2] - m_mat[1][0]*m_mat[2][2]);
  float m13(m_mat[1][0]*m_mat[2][1] - m_mat[2][0]*m_mat[1][1]);
  float d(m_mat[0][0]*m11 + m_mat[0][1]*m12 + m_mat[0][2]*m13);

  if (d == 0)
  {
    return *(new Matrix3x3<T>());
  }
  else 
  {
    d = 1/d;
	  return *(new Matrix3x3<T>(d*m11, d*m12, d*m13,
	                            d*(m_mat[2][1]*m_mat[0][2] - m_mat[0][1]*m_mat[2][2]),
                          	  d*(m_mat[0][0]*m_mat[2][2] - m_mat[2][0]*m_mat[0][2]),
	                            d*(m_mat[2][0]*m_mat[0][1] - m_mat[0][0]*m_mat[2][1]),
	                            d*(m_mat[0][1]*m_mat[1][2] - m_mat[1][1]*m_mat[0][2]),
	                            d*(m_mat[1][0]*m_mat[0][2] - m_mat[0][0]*m_mat[1][2]),
	                            d*(m_mat[0][0]*m_mat[1][1] - m_mat[1][0]*m_mat[0][1])));
  }
} // GetInverse()


/**
 * Returns matrix that is this one plus another one scaled
 * @param matrix
 * @param scale factor
 * @return plusScaled matrix
 */
template <class T> 
Matrix3x3<T> Matrix3x3<T>::plusScaled(Matrix3x3<T> A, float s)
//*************************************************************
{
  return *(new Matrix3x3(m_mat[0][0]+s*A.m_mat[0][0], m_mat[1][0]+s*A.m_mat[1][0], m_mat[2][0]+s*A.m_mat[2][0],
	                       m_mat[0][1]+s*A.m_mat[0][1], m_mat[1][1]+s*A.m_mat[1][1], m_mat[2][1]+s*A.m_mat[2][1],
	                       m_mat[0][2]+s*A.m_mat[0][2], m_mat[1][2]+s*A.m_mat[1][2], m_mat[2][2]+s*A.m_mat[2][2]));
} // plusScaled(()


/**
 * << Operator.
 * Directs the three data members to the ostream, reference of which is passed
 * AsOstream member function is called 
 * @param os ostream &
 * @param matrix Matrix3x3<T> &
 * @return ostream &
 */  
template <class T> 
inline std::ostream & operator<<(std::ostream & os, const Matrix3x3<T> & matrix)
//*************************************************************
{ 
  int4 iW(3); 
	os.precision(2);
  
  os<<"{(";
  os.width(iW); os<<matrix(0,0)<<","; 
  os.width(iW); os<<matrix(1,0)<<","; 
  os.width(iW); os<<matrix(2,0)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,1)<<","; 
  os.width(iW); os<<matrix(1,1)<<","; 
  os.width(iW); os<<matrix(2,1)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,2)<<","; 
  os.width(iW); os<<matrix(1,2)<<","; 
  os.width(iW); os<<matrix(2,2)<<"), ";
  os<<"("; 
  os.width(iW); os<<matrix(0,3)<<","; 
  os.width(iW); os<<matrix(1,3)<<","; 
  os.width(iW); os<<matrix(2,3)<<")}";
 
  return os;
} // operator<<()


// Revision History:
// $Log: Matrix3x3.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 11:43:46  michael
// code review
//
// Revision 1.4  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.3  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 1.2  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// Revision 1.1  2001/10/17 23:35:48  huamin
// Initial implementation for image warping.  Adapted from matrix class
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Matrix3x3.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Matrix3x3.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
