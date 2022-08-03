// $Id: Quaternion.inl,v 1.2 2006/06/01 20:29:14 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quaternion.inl,v 1.2 2006/06/01 20:29:14 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Bin Li (binli@viatronix.com)



// includes
#include "Quaternion.h"

 
// define declarations
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor with 4 default arguments
 */
template <class T> 
Quaternion<T>::Quaternion() 
  :m_x(0), m_y(0), m_z(0), m_w(1) 
{
}


/**
 * Constructor from four scalars.
 *
 * @param x T.
 * @param y T.
 * @param z T.
 * @param w T.
 */
template <class T> 
Quaternion<T>::Quaternion(T x, T y, T z, T w) 
  : m_x(x), m_y(y), m_z(z), m_w(w)
{
}


/**
 * Constructor from four scalars.
 * @param v Vector<T> &.
 * @param w T (value goes to m_w).
 */
template <class T> 
Quaternion<T>::Quaternion(const Vector<T> & v, T w) 
  : m_x(v.m_x), m_y(v.m_y), m_z(v.m_z), m_w(w) 
{ 
}
  

/** 
 * Length the length of quaternion.
 */
template <class T> 
inline float4 Quaternion<T>::Length() 
{ 
  return sqrt(m_w*m_w + m_x*m_x + m_y*m_y + m_z*m_z); 
} // Length().


/**
 * Normalize normalizes the quaternion.
 * @return Quaternion<T>.
 */
template <class T> 
inline Quaternion<T> & Quaternion<T>::Normalize() 
{ 
  float4 fLength = sqrt(m_x*m_x + m_y*m_y + m_z*m_z + m_w*m_w);
  if(fLength != 0)
  {
    m_x /= fLength;
    m_y /= fLength;
    m_z /= fLength;
    m_w /= fLength;
  }
  return *this;
} // Normalize().


/** 
 * Multiply
 * @param q const Quaternion<T> &.
 * @return 
 */
template <class T> 
inline Quaternion<T> Quaternion<T>::multiply(const Quaternion<T> & q) const 
{
  return Quaternion(
    m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z,
    m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y,
    m_w*q.m_y - m_x*q.m_z + m_y*q.m_w + m_z*q.m_x,
    m_w*q.m_z + m_x*q.m_y - m_y*q.m_x + m_z*q.m_w
  );
} // multiply()


/**
 * Assignment Operator  from Quaternion.
 * @param  q  const Quaternion<T> &.
 * @return  Quaternion<T>.
 */
template <class T> 
inline Quaternion<T> & Quaternion<T>::operator = (const Quaternion<T> & q)
{
  m_x = T(q.m_x);
  m_y = T(q.m_y);
  m_z = T(q.m_z);
  m_w = T(q.m_w);

  return *this;
} // operator =


/**
 * Operator *
 *
 * @param  q reference to a const quaternion.
 * @return quaternion.
 */
template <class T> 
inline Quaternion<T> Quaternion<T>::operator * (const Quaternion<T> & q) const 
{ 
	return multiply(q);
} // operator *


/** 
 * Operator ()
 *
 * @param x T.
 * @param y T.
 * @param z T.
 * @param w T.
 */
template <class T> 
inline void Quaternion<T>::operator () (T x, T y, T z, T w) 
{ 
	m_x = x; 
	m_y = y; 
	m_z = z; 
	m_w = w; 
} // operator ()


/**
 * Operator *=
 * @param  q  const Quaternion<T> &.
 * @return Quaternion<T>.
 */
template <class T> 
inline Quaternion<T> & Quaternion<T>::operator *= (const Quaternion<T> & q) 
{
	T tw = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
	T tx = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
	T ty = m_w*q.m_y - m_x*q.m_z + m_y*q.m_w + m_z*q.m_x;
	T tz = m_w*q.m_z + m_x*q.m_y - m_y*q.m_x + m_z*q.m_w;

	m_x = tx; 
	m_y = ty; 
	m_z = tz;
	m_w = tw; 
	
	return *this;
} // operator *=


/**
 * operator []
 * @param index int (index to for one of 4 elements).
 * @return the value of the indexed element. 
 */
template <class T> 
inline T Quaternion<T>::operator [] (int index) 
{ 
  switch (index) 
	{
	case 0: 
		return m_x; 
	case 1: 
		return m_y; 
	case 2:	
		return m_z;
  default:
		return m_w;
  } 
} // operator [].


/**
 * operator []
 * @param index index to for one of 4 elements.
 * @return the value of the indexed element. 
 */
template <class T> 
inline const T & Quaternion<T>::operator [] (int index) const
{ 
	switch (index) 
	{
	case 0: 
		return m_x; 
	case 1: 
		return m_y; 
	case 2:	
		return m_z;
	default:
		return m_w;
 	}
} // operator [].


/**
 * Get an array from quaternion elements.
 * @param arr T * (pointer to a array).
 */
template <class T> 
inline void Quaternion<T>::GetArrayFromQuaternion(T * pvArray) const
{
  pvArray[0] = m_x;
  pvArray[1] = m_y;
  pvArray[2] = m_z;;
} // GetArrayFromQuaternion().


/**
 * Get a vec from quaternion elements.
 * @param vec Vector<T> &.
 */
template <class T> 
inline void Quaternion<T>::GetVectorFromQuaternion(Vector<T> & vec) const
{
  vec.m_x = m_x;
  vec.m_y = m_y;
  vec.m_z = m_z;
} // GetVectorFromQuaternion().


/**
 * Get a vec from quaternion elements.
 * @return a Vector<T>.
 */
template <class T> 
inline Vector<T> Quaternion<T>::GetVectorFromQuaternion() const
{
  return Vector<T>(m_x, m_y, m_z);
} // GetVectorFromQuaternion().


/**
 *  SetW - set up the value of member m_w.
 *  @param value T (goes to m_w).
 */ 
template <class T> 
inline void Quaternion<T>::SetW(T value)
{
	m_w = value;
} // SetW(),


/**
 *  SetConjugate conjugate set of quaternion
 */
template <class T> 
inline void Quaternion<T>::SetConjugate() 
{ 
  m_x = -m_x; 
  m_y = -m_y; 
  m_z = -m_z; 
} // SetConjugate().


/**
 * AsConjugate sets quaternion as conjugate.
 * @return Quaternion<T>.
 */
template <class T> 
inline Quaternion<T> Quaternion<T>::AsConjugate() const 
{
  return Quaternion(m_w,-m_x,-m_y,-m_z); 
} // AsConjugate().


/** 
 * SetRotationAboutAxis sets the rotation about the axis
 * @param vec  Vector<T>
 * @param phi T
 */
template <class T> 
inline void Quaternion<T>::SetRotationAboutAxis(Vector<T>vec, T phi)
{
  T temp = static_cast<T>(sin(static_cast<float4>(phi)/2.0));
  vec.Normalize();
  m_x = temp * vec.m_x;
  m_y = temp * vec.m_y;
  m_z = temp * vec.m_z;
  m_w = static_cast<float4>(cos(phi/2.0));
} // SetRotationAboutAxis()


/**
 * accumulate quats
 * @param lastQuat  reference to a quat.
 */
template <class T>
inline void Quaternion<T>::AddQuats(Quaternion & lastQuat)
{
  const int iRENORMCOUNT	= 97;
  static	int iCount = 0;
  Vector<T> vector1, vector2, vector3;
  Vector<T> vector4, vector5;

  vector1 = lastQuat.GetVectorFromQuaternion();
  vector4 = lastQuat.GetVectorFromQuaternion();
  vector1 = vector1 * m_w;

  vector2 = GetVectorFromQuaternion();
  vector5 = GetVectorFromQuaternion();
  vector2 = vector2 * lastQuat[3];

  vector3.m_x = vector5.m_y * vector4.m_z - vector5.m_z * vector4.m_y;
  vector3.m_y = vector5.m_z * vector4.m_x - vector5.m_x * vector4.m_z;
  vector3.m_z = vector5.m_x * vector4.m_y - vector5.m_y * vector4.m_x;
 
  m_x = vector1.m_x + vector2.m_x + vector3.m_x;
  m_y = vector1.m_y + vector2.m_y + vector3.m_y;
  m_z = vector1.m_z + vector2.m_z + vector3.m_z;
  m_w = lastQuat[3]*m_w - (vector4.m_x * vector5.m_x + 
                           vector4.m_y * vector5.m_y + 
                           vector4.m_z * vector5.m_z);

  if (++iCount > iRENORMCOUNT) 
  {
    iCount = 0;
    Normalize();
  } // end   if (++iCount > iRENORMCOUNT) 
} // AddQuats().


/**
 * build a matrix from quat.
 * @param m[16] 1D OpenGL matrix.
 */
template <class T>
inline void Quaternion<T>::BuildOpenGLMatrixFromQuat(float4 vMatrix[16]) const 
{
  vMatrix[0] = static_cast<float4>(1.0 -  2.0*(m_y*m_y + m_z*m_z)); 
  vMatrix[4] = static_cast<float4>(2.0 * (m_x*m_y - m_w*m_z)); 
  vMatrix[8] = static_cast<float4>(2.0 * (m_x*m_z + m_w*m_y)); 
  vMatrix[12]= 0.0;
  
  vMatrix[1] = static_cast<float4>(2.0 * (m_x*m_y + m_w*m_z)); 
  vMatrix[5] = static_cast<float4>(1.0 - 2.0 * (m_x*m_x + m_z*m_z)); 
  vMatrix[9] = static_cast<float4>(2.0 * (m_y*m_z - m_w*m_x)); 
  vMatrix[13]= 0.0;

  vMatrix[2] = static_cast<float4>(2.0 * (m_x*m_z - m_w*m_y)); 
  vMatrix[6] = static_cast<float4>(2.0 * (m_y*m_z + m_w*m_x)); 
  vMatrix[10]= static_cast<float4>(1.0 - 2.0 * (m_x*m_x + m_y*m_y)); 
  vMatrix[14]= 0.0;
  
  vMatrix[3] = 0.0;              
  vMatrix[7] = 0.0;
  vMatrix[11]= 0.0;
  vMatrix[15]= 1.0; 
} // BuildOpenGLMatrixFromQuat().


/**
 * build a matrix from quat.
 * @param m[][4] 2D matrix.
 */
template <class T>
inline void Quaternion<T>::BuildMatrixFromQuat(float4 mMat[][4])
{
  mMat[0][0] = static_cast<float4>(1.0 - (2.0*(m_y*m_y + m_z*m_z)));
  mMat[0][1] = static_cast<float4>(2.0 * (m_x*m_y - m_z*m_w));
  mMat[0][2] = static_cast<float4>(2.0 * (m_z*m_x + m_y*m_w));
  mMat[0][3] = 0.0;

  mMat[1][0] = static_cast<float4>(2.0 * (m_x*m_y + m_z*m_w));
  mMat[1][1] = static_cast<float4>(1.0 - 2.0 * (m_z*m_z + m_x*m_x));
  mMat[1][2] = static_cast<float4>(2.0 * (m_y*m_z - m_x*m_w));
  mMat[1][3] = 0.0;

  mMat[2][0] = static_cast<float4>(2.0 * (m_z*m_x - m_y*m_w));
  mMat[2][1] = static_cast<float4>(2.0 * (m_y*m_z + m_x*m_w));
  mMat[2][2] = static_cast<float4>(1.0 - 2.0 * (m_y*m_y + m_x*m_x));
  mMat[2][3] = 0.0;

  mMat[3][0] = 0.0;
  mMat[3][1] = 0.0;
  mMat[3][2] = 0.0;
  mMat[3][3] = 1.0;
} // BuildMatrixFromQuat().


/**
 * build a matrix from quat.
 *
 * @param matrix refernce to a Matrix<float>.
 */
template <class T>
inline void Quaternion<T>::BuildMatrixFromQuat(Matrix<float4> & matrix)
{
	// build a matrix from quatanion.
	Matrix<float4> matrixFromQuat(
      static_cast<float4>(1.0 - (2.0*(m_y*m_y + m_z*m_z))),
      static_cast<float4>(2.0 * (m_x*m_y + m_z*m_w)),
      static_cast<float4>(2.0 * (m_z*m_x - m_y*m_w)),
      0.0,	

      static_cast<float4>(2.0 * (m_x*m_y - m_z*m_w)),
      static_cast<float4>(1.0 - 2.0 * (m_z*m_z + m_x*m_x)),			
      static_cast<float4>(2.0 * (m_y*m_z + m_x*m_w)),	
      0.0,	

      static_cast<float4>(2.0 * (m_z*m_x + m_y*m_w)),
      static_cast<float4>(2.0 * (m_y*m_z - m_x*m_w)),			
      static_cast<float4>(1.0 - 2.0 * (m_y*m_y + m_x*m_x)),			
      0.0,

      0.0,
      0.0,
      0.0,
      1.0
    );
  matrix = matrixFromQuat;
} // BuildMatrixFromQuat().


/**
 * Overloaded << Operator.
 * Directs the two data members to the ostream, reference of which is passed
 * @param os ostream &
 * @param quat Quaternion<T> &
 * @return ostream &
 */  
template <class T> 
inline std::ostream & operator<<(std::ostream &os, const Quaternion<T> & quat)
{
  os << "Quaternion(" ;
  os << quat [0] << "," ;
  os << quat [1] << "," ;

  os << quat [2] << "," ;
  os << quat [3];
  os << ")"; 
  return os;
} // end of operator<<(ostream &os, const Quaternion<T> & quat)


/**
 * Test function for template class Quaternion<T>.
 * @param T type of the Quaternion
 * @param os output stream
 * @return bool bSuccess
 */
template <class T>
bool Quaternion<T>::TestByType(std::ostream & os) 
{
  static const int1 * const sBoolStr[] = {"    Oh No ! :-( ","   :-) "};
  bool bSuccess(true);
  bool bOk(true);
  float4 fFloatCompare(0.001f);
  T x(5.125), y(8.5), z(15.75), w(3.125);
  T phi(80.125);

  os << endl << "-Testing Quaternion class for type: " << typeid(T).name();
  os << endl <<"size of Quaternion<T>: " ;
  os << sizeof(Quaternion<T>) << " Bytes" << endl;

  os << "x=" << x << " y=" << y << " z=" << z << " w= " << w;
  os << " phi= " << phi << endl;


  // tests default constructor
  os << "Quaternion<T> quat1= ";
  Quaternion<T> quat1;
  os << quat1 << endl;
  bOk = ( quat1.m_x == 0 &&
          quat1.m_y == 0 &&
          quat1.m_z == 0 &&
          quat1.m_w == 1 );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  
  // tests constructor from four scalars
  os << "Quaternion<T> quat2(x,y,z,w)= ";
  Quaternion<T> quat2(x,y,z,w);
  os << quat2 << endl;
  bOk = ( quat2.m_x == x &&
          quat2.m_y == y &&
          quat2.m_z == z &&
          quat2.m_w == w );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests constructor from one vector and one scalar
  os << "Quaternion<T> quat3(vector1,w)= ";
  Vector<T> vector1(x,y,z);
  Quaternion<T> quat3(vector1,w);
  os << quat3 << endl;
  bOk = ( quat3.m_x == x &&
          quat3.m_y == y &&
          quat3.m_z == z &&
          quat3.m_w == w );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;
    

  // tests the length
  os << "quat3.Length()= " ;
  float4 fLength = quat3.Length();

  bOk = (fLength == (sqrt(quat3.m_x * quat3.m_x +
                          quat3.m_y * quat3.m_y +
                          quat3.m_z * quat3.m_z +
                          quat3.m_w * quat3.m_w)) );
  os << fLength << endl;
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests Normalize
  os << "quat3.Normalize()= ";
  quat1 = quat3;
  os << quat3.Normalize() << endl;  
  bOk = ( ( quat3.m_x - (quat1.m_x / fLength) ) < fFloatCompare  &&
          ( quat3.m_y - (quat1.m_y / fLength) ) < fFloatCompare  &&
          ( quat3.m_z - (quat1.m_z / fLength) ) < fFloatCompare  &&
          ( quat3.m_w - (quat1.m_w / fLength) ) < fFloatCompare );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;  


  // tests multiply operator
  os << "quat1.multiply(quat4)= ";
  Quaternion<T> quat4(x,y,z,w);
  quat3 = quat1.multiply(quat4);
  os << quat3 << endl;
  bOk = ( quat3.m_x == (T)( quat1.m_w*quat4.m_w - quat1.m_x*quat4.m_x - 
	                          quat1.m_y*quat4.m_y - quat1.m_z*quat4.m_z ) &&
          quat3.m_y == (T)( quat1.m_w*quat4.m_x + quat1.m_x*quat4.m_w + 
		                        quat1.m_y*quat4.m_z - quat1.m_z*quat4.m_y ) &&
		      quat3.m_z == (T)( quat1.m_w*quat4.m_y - quat1.m_x*quat4.m_z + 
		                        quat1.m_y*quat4.m_w + quat1.m_z*quat4.m_x ) &&
          quat3.m_w == (T)( quat1.m_w*quat4.m_z + quat1.m_x*quat4.m_y - 
		                        quat1.m_y*quat4.m_x + quat1.m_z*quat4.m_w ) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;  


  // tests () operator
  os << "quat1 (x,y,z,w)= ";
  quat1 (x,y,z,w);
  os << quat1 << endl;
  bOk = (quat1.m_x == x &&
         quat1.m_y == y &&
         quat1.m_z == z &&
         quat1.m_w == w );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;  

  // tests = operator
  os << "quat3=quat4: quat3= ";
  quat3 = quat4;
  os << quat3 << endl;
  bOk = (quat3.m_x == quat4.m_x &&
	       quat3.m_y == quat4.m_y &&
		     quat3.m_z == quat4.m_z &&
		     quat3.m_w == quat4.m_w );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  
  // tests * operator
  os << "quat1=quat3*quat4: quat1= ";
  quat1 = quat3*quat4;
  os << quat1 << endl;
  bOk = ( quat1.m_x == (T)(quat3.m_w*quat4.m_w - quat3.m_x*quat4.m_x - 
	                         quat3.m_y*quat4.m_y - quat3.m_z*quat4.m_z ) &&
		      quat1.m_y == (T)(quat3.m_w*quat4.m_x + quat3.m_x*quat4.m_w + 
		                       quat3.m_y*quat4.m_z - quat3.m_z*quat4.m_y ) &&
		      quat1.m_z == (T)(quat3.m_w*quat4.m_y - quat3.m_x*quat4.m_z + 
		                       quat3.m_y*quat4.m_w + quat3.m_z*quat4.m_x ) &&
		      quat1.m_w == (T)(quat3.m_w*quat4.m_z + quat3.m_x*quat4.m_y - 
		                       quat3.m_y*quat4.m_x + quat3.m_z*quat4.m_w ));
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests *= operator
  os << "quat4*=quat2: quat4= ";
  quat3 = quat4;
  quat4 *= quat2;
  os << quat4 << endl;
  bOk = ( quat4.m_w == (T)( quat3.m_w*quat2.m_w - quat3.m_x*quat2.m_x - 
	                          quat3.m_y*quat2.m_y - quat3.m_z*quat2.m_z ) &&
		      quat4.m_x == (T)( quat3.m_w*quat2.m_x + quat3.m_x*quat2.m_w + 
		                        quat3.m_y*quat2.m_z - quat3.m_z*quat2.m_y ) &&
	    	  quat4.m_y == (T)( quat3.m_w*quat2.m_y - quat3.m_x*quat2.m_z + 
		                        quat3.m_y*quat2.m_w + quat3.m_z*quat2.m_x ) &&
		      quat4.m_z == (T)( quat3.m_w*quat2.m_z + quat3.m_x*quat2.m_y - 
		                        quat3.m_y*quat2.m_x + quat3.m_z*quat2.m_w ) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests [] operator
  os << "quat4 [2]= ";
  T result = quat4 [2];
  os << result << endl;
  bOk = ( result == quat4.m_z );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetVectorFromQuaternion
  os << "quat4.GetVectorFromQuaternion(vector2): vector2= ";
  Vector<T> vector2;
  quat4.GetVectorFromQuaternion(vector2);
  os << vector2 << endl;
  bOk = ( quat4.m_x == vector2.m_x &&
	        quat4.m_y == vector2.m_y &&
		      quat4.m_z == vector2.m_z );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetArrayFromQuaternion
  os << "quat4.GetArrayFromQuaternion(array): array= ";
  T *p = new T[3];
  quat4.GetArrayFromQuaternion(p);
  os << *p << "," << *(p+1) << "," << *(p+2) << endl;
  bOk = (quat4.m_x == *p &&
	       quat4.m_y == *(p+1) &&
		     quat4.m_z == *(p+2) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetVectorFromQuaternion
  os << "quat3.GetVectorFromQuaternion(): vector= ";
  vector1 = quat3.GetVectorFromQuaternion();
  os << vector1 << endl;
  bOk = ( quat3.m_x == vector1.m_x &&
	        quat3.m_y == vector1.m_y &&
		      quat3.m_z == vector1.m_z );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests SetW
  os << "quat4.SetW(w): quat4= ";
  quat4.SetW(w);
  os << quat4 << endl;
  bOk = ( quat4.m_w == w ); 
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests AsConjugate
  // this test fails for unsigned char, which explains the "if" 
  if ( (typeid(T).name() != typeid(unsigned char).name())) 
	{
    os << "quat1.AsConjugate()= ";
    quat3 = quat1.AsConjugate();
    os << quat3 << endl;
    bOk = ( (quat3.m_x == quat1.m_w) &&
	          (quat3.m_y == -1*quat1.m_x) &&
		        (quat3.m_z == -1*quat1.m_y) &&
            (quat3.m_w == -1*quat1.m_z) );
    os << sBoolStr[bOk];
    bSuccess = bSuccess && bOk;
  }


  // tests SetRotationAboutAxis
  os << "quat3.SetRotationAboutAxis(vector1,phi)= ";
  quat2 = quat3;
  quat3.SetRotationAboutAxis(vector1,phi);
  os << quat3 << endl;
  T temp = static_cast<T>(sin(static_cast<float4>(phi)/2.0));
  vector1.Normalize();
  bOk = ( ( quat3.m_x - (temp * vector1.m_x) ) < fFloatCompare ) &&
        ( ( quat3.m_y - (temp * vector1.m_y) ) < fFloatCompare ) &&
        ( ( quat3.m_z - (temp * vector1.m_z) ) < fFloatCompare ) &&
        ( ( quat3.m_w - (T)(cos(phi/2.0)) ) < fFloatCompare );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  os << endl;

  if (bSuccess)
	{
    os << "Test of Quaternion class is a success!" << endl;
  }
	else 
	{
    os << "Test of Quaternion class is NOT a success" << endl;
  } //endif (bSuccess)
  
  return bSuccess;

} // end of TestByType


#undef FILE_REVISION


// $Log: Quaternion.inl,v $
// Revision 1.2  2006/06/01 20:29:14  frank
// updated for visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 3.4  2003/05/16 11:48:47  michael
// code review
//
// Revision 3.3  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 3.2  2002/03/11 17:24:25  ingmar
// removed exceptions to get more exec speed and compile speed
//
// Revision 3.1  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
//    Rev 2.1   Sep 19 2001 14:33:18   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:16   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.13  2001/08/23 14:33:21  sarang
// linux/gcc fix.
//
// Revision 1.12  2001/06/02 00:03:58  liwei
// vector renamed to vec to resolve name confilicts of STL vector AGAIN!
// Please don't use "vector" as a variable name.
// It is a STL class !
//
// Revision 1.11  2001/06/01 19:47:00  ana
// variable changes
//
// Revision 1.10  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.9  2001/05/29 18:39:20  ana
// took care of the warnings
//
// Revision 1.8  2001/05/25 20:54:10  ana
// finished tests in TestByType method
//
// Revision 1.7  2001/05/24 21:27:44  ana
// new tests added to TestByType
//
// Revision 1.6  2001/05/23 21:34:55  ana
// created TestByType function
//
// Revision 1.5  2001/05/21 16:06:31  liwei
// vector renamed to vec to resolve the name conflict with STL vector
//
// Revision 1.4  2001/04/05 16:27:31  binli
// enhanced
//
// Revision 1.3  2001/03/30 21:21:27  binli
// changed to template Quaternion
//
// Revision 1.2  2001/03/30 14:33:20  binli
// comments.
//
// Revision 1.1  2001/03/28 18:02:38  binli
// adjusted and added functions in Quaternion class.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quaternion.inl,v 1.2 2006/06/01 20:29:14 frank Exp $
// $Id: Quaternion.inl,v 1.2 2006/06/01 20:29:14 frank Exp $
