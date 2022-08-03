// $Id: Vector4d.inl,v 1.2 2004/04/06 12:09:15 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com
//
// Complete history at bottom of file.


/**
 * Default constructor.
 * @param & x
 * @param & y
 * @param & z
 * @param & t
 */
template < class Type > 
Vector4d<Type>::Vector4d(const Type & x, const Type & y, const Type & z, const Type & t)
{
  X() = x;
  Y() = y;
  Z() = z;
  T() = t;
} // Vector4d(const Type & x, const Type & y, const Type & z, const Type & t)


/**
 * Copy constructor.
 * @param & src
 */
template < class Type >
Vector4d<Type>::Vector4d(const Vector4d & src)
{
  X() = src.X();
  Y() = src.Y();
  Z() = src.Z();
  T() = src.T();
} // Vector4d(const Vector4d & src)

/**
 * Constructor for a Triple.
 * @param & src
 * @param t
 */
template < class Type >
Vector4d<Type>::Vector4d(const Triple<Type> & src, Type t)
{
  X() = src.X();
  Y() = src.Y();
  Z() = src.Z();
  T() = t;
} // Vector4d(const Triple<Type> & src, Type t)


/**
 * Constructor for a Triple.
 * @param & src
 * @param t
 */
template < class Type >
Vector4d<Type>::Vector4d(const Point3D<Type> & src, Type t)
{
  X() = src.X();
  Y() = src.Y();
  Z() = src.Z();
  T() = t;
} // Vector4d(const Point3D<Type> & src, Type t)


/**
 * Constructor for a Vector3D.
 * @param & src
 * @param t
 */
template < class Type >
Vector4d<Type>::Vector4d(const Vector3D<Type> & src, Type t)
{
  X() = src.X();
  Y() = src.Y();
  Z() = src.Z();
  T() = t;
} // Vector4d(const Vector3D<Type> & src, Type t)


/**
 * assignment operator.
 * @param & otherVector
 */
template < class Type >
Vector4d<Type> & Vector4d<Type>::operator= (const Vector4d & otherVector)
{
  X() = otherVector.X();
  Y() = otherVector.Y();
  Z() = otherVector.Z();
  T() = otherVector.T();
  return *this;
} // assignment operator


/**
 * Read access for any component.    
 * 
 * @param iIndex Index of the wanted component.
 * @return const reference to indexed component.
 */
template < class Type >
const Type & Vector4d<Type>::operator[](int4 iIndex) const
{
  return(m_xyzt[iIndex]);
} // [] operator


/**
 * Read access for x component.    
 * 
 * @return const reference to x component.
 */
template < class Type >
const Type & Vector4d<Type>::X() const
{
  return(m_xyzt[0]);
} // X()


/**
 * Read access for y component.    
 * 
 * @return const reference to y component.
 */
template < class Type >
const Type & Vector4d<Type>::Y() const
{
  return(m_xyzt[1]);
} // Y()


/**
 * Read access for z component.    
 * 
 * @return const reference to z component.
 */
template < class Type >
const Type & Vector4d<Type>::Z() const
{
  return(m_xyzt[2]);
} // Z()


/**
 * Read access for z component.    
 * 
 * @return const reference to z component.
 */
template < class Type >
const Type & Vector4d<Type>::T() const
{
  return(m_xyzt[3]);
} // T()


/**
 * Write access for any component.
 * 
 * @param iIndex Index of the wanted component.
 * @return reference to indexed component.
 */
template < class Type >
Type & Vector4d<Type>::operator[](int4 iIndex)
{
  return(m_xyzt[iIndex]);
} // [] operator


/**
 * Write access for x component.    
 * 
 * @return reference to x component.
 */
template < class Type >
Type & Vector4d<Type>::X()
{
  return(m_xyzt[0]);
} // X()


/**
 * Write access for y component.    
 * 
 * @return reference to y component.
 */
template < class Type >
Type & Vector4d<Type>::Y()
{
  return(m_xyzt[1]);
} // Y()


/**
 * Write access for z component.    
 * 
 * @return reference to z component.
 */
template < class Type >
Type & Vector4d<Type>::Z()
{
  return(m_xyzt[2]);
} // Z()


/**
 * Write access for z component.    
 * 
 * @return reference to z component.
 */
template < class Type >
Type & Vector4d<Type>::T()
{
  return(m_xyzt[3]);
} // T()


/**
 * Read access to maximum value component.
 * @return
 */
template < class Type >
Type Vector4d<Type>::GetMaxComponentXYZ() const
{
  if ( X() > Y() )
  {
    if ( X() > Z() )
    {
      return(X());
    }
    else
    {
      return(Z());
    }
  }
  else
  {
    if ( Y() > Z() )
    {
      return(Y());
    }
    else
    {
      return(Z());
    }
  }
} // GetMaxComponentXYZ()


/**
 * Binary * operator.
 * @param scalar const Type &
 * @return Vector4d<Type>
 */
template <class Type> 
inline Vector4d<Type> Vector4d<Type>::operator * (const Type & scalar) const 
{
  return(Vector4d<Type>(X() * scalar, 
                        Y() * scalar, 
                        Z() * scalar, 
                        T() * scalar));
} // operator * (const Type & scalar) const.


/**
 * Binary / operator.
 * @param scalar const Type &
 * @return Vector4d<Type>
 */
template <class Type> 
inline Vector4d<Type> Vector4d<Type>::operator / (const Type & scalar) const
{
  const float fReci = (scalar == static_cast<Type>(0)) ? 1 : 1/static_cast<float4>(scalar);
  return(Vector4d<Type>(static_cast<Type>(static_cast<float4>(X()) * fReci), 
                        static_cast<Type>(static_cast<float4>(Y()) * fReci), 
                        static_cast<Type>(static_cast<float4>(Z()) * fReci), 
                        static_cast<Type>(static_cast<float4>(T()) * fReci)));
} // operator / (const Type & scalar) const.


/**
 * Binary *= operator.
 * @param scalar const Type &
 * @return Vector4d<Type> &
 */
template <class Type> 
inline Vector4d<Type> & Vector4d<Type>::operator *= (const Type & scalar)
{
  X() *= scalar; 
  Y() *= scalar; 
  Z() *= scalar;
  T() *= scalar;
  return(* this);
} // operator *= (const Type & scalar).


/**
 * Binary /= operator.
 * @param scalar const Type &
 * @return Point<Type> &
 */
template <class Type> 
inline Vector4d<Type> & Vector4d<Type>::operator /= (const Type & scalar)
{
  const float fReci = (scalar == static_cast<Type>(0)) ? 1 : 1/static_cast<float4>(scalar);
  X() = static_cast<Type>(static_cast<float4>(X()) * fReci);
  Y() = static_cast<Type>(static_cast<float4>(Y()) * fReci);
  Z() = static_cast<Type>(static_cast<float4>(Z()) * fReci);
  T() = static_cast<Type>(static_cast<float4>(T()) * fReci);
  return(* this);
} // operator /= (const Type & scalar).

/**
 * Returns a Normalized Vector<Type>.
 * @param Vector<Type> &
 */
template <class Type> 
Vector4d<Type> Vector4d<Type>::GetNormalized() const
{
  const float4 fLengthInv(1/GetLength());
  return(Vector4d<Type>(X() * fLengthInv,
                        Y() * fLengthInv,
                        Z() * fLengthInv,
                        T() * fLengthInv )); 
} // GetNormalized()


/**
 * Returns a Normalized Vector<Type>.
 * @param Vector<Type> &
 */
template <class Type> 
Vector4d<Type> Vector4d<Type>::GetNormalizedXYZ() const
{
  const float4 fLengthInv(1/GetLengthXYZ());
  return(Vector4d<Type>(X() * fLengthInv,
                        Y() * fLengthInv,
                        Z() * fLengthInv,
                        T() * fLengthInv )); 
} // GetNormalizedXYZ()


/**
 * Returns a Normalized Vector<Type>.
 * @param Vector<Type> &
 */
template <class Type> 
inline void Vector4d<Type>::Normalize()
{
  const float4 fLengthInv(1/GetLength());
  X() *= fLengthInv; 
  Y() *= fLengthInv; 
  Z() *= fLengthInv;
  T() *= fLengthInv;
} // Normalize().


/**
 * Binary plus operator.
 * @param other const Vector<Type> &
 * @return Vector<Type>
 */
template <class Type>
inline Vector4d<Type> Vector4d<Type>::operator + (const Vector4d<Type> & other) const
{
  return(Vector4d<Type>(X() + other.X(), 
                        Y() + other.Y(), 
                        Z() + other.Z(), 
                        T() + other.T()));
} // operator + (const Vector<Type> & other) const.

/**
 * Binary plus operator.
 * @param other const Vector<Type> &
 * @return Vector<Type>
 */
template <class Type>
inline Vector4d<Type> Vector4d<Type>::operator - (const Vector4d<Type> & other) const
{
  return(Vector4d<Type>(X() - other.X(), 
                        Y() - other.Y(), 
                        Z() - other.Z(), 
                        T() - other.T()));
} // operator + (const Vector<Type> & other) const.

/**
 * @return the length of the Normal.
 * @param double
 */
template <class Type> 
inline float4 Vector4d<Type>::GetLength() const 
{
  return(sqrt((X() * X() + Y() * Y() + Z() * Z() + T() * T())));
} // GetLength() const


/**
 * @return the length of the XYZ components of the vector.
 * @param double
 */
template <class Type> 
inline float4 Vector4d<Type>::GetLengthXYZ() const 
{
  return static_cast< float4>(sqrt((X() * X() + Y() * Y() + Z() * Z())));
} // GetLengthXYZ() const


/**
 * @return the squared length of the Vector.
 * @param double
 */
template <class Type> 
inline Type Vector4d<Type>::GetSquaredLength() const 
{
  return((X() * X() + Y() * Y() + Z() * Z() + T() * T()));
} // GetSquaredLength() const


/**
 * Returns the dot product of the Vector<Type> with another Vector<Type>.
 * @param other const Vector<Type> &
 * @return T
 */
template <class Type> 
inline Type Vector4d<Type>::Dot(const Vector4d<Type> & other) const 
{
  return(X() * other.X() + 
         Y() * other.Y() + 
         Z() * other.Z() + 
         T() * other.T()); 
} // Dot(const Vector<Type> & other) const.

/**
 * @eturn a new Vector4d with each component scaled by the corresponding component of vScale.
 * The return value is a Vector<Type> (same type)
 */
template <class Type> 
const Vector4d<Type> Vector4d<Type>::GetScaledUnIso(const Vector4d<Type> & vScale) const 
{
  return(Vector4d<Type>(
                       X() * vScale.X(), 
                       Y() * vScale.Y(), 
                       Z() * vScale.Z(), 
                       T() * vScale.T()) );
} // GetScaleUniso()


/**
 * @param vDivide
 * @return a new Vector4d with each component divided by the corresponding component of vDivide.
 * The return value is a Vector<Type> (same type)
 */
template <class Type> 
const Vector4d<Type> Vector4d<Type>::GetDividedUnIso(const Vector4d<Type> & vDivide) const 
{
  return(Vector4d<Type>(
                       X() / vDivide.X(), 
                       Y() / vDivide.Y(), 
                       Z() / vDivide.Z(), 
                       T() / vDivide.T()) );
} // GetDivideUniso()


/**
 * Unary plus operator.
 * @return Vector<Type>
 */
template <class Type> 
inline Vector4d<Type> Vector4d<Type>::operator + () const 
{
  return(* this);
} // operator + () const.


/**
 * Unary minus operator.
 * @return Vector<Type>
 */
template <class Type> 
inline Vector4d<Type> Vector4d<Type>::operator - () const 
{
  return(Vector4d<Type>(-X(), -Y(), -Z(), -T()));
} // operator - () const.


/**
 * Overloaded Unary += Operator.
 */
template <class Type> 
inline Vector4d<Type> &  Vector4d<Type>::operator += (const Vector4d<Type> & otherVector) 
{
  X() += otherVector.X();
  Y() += otherVector.Y();
  Z() += otherVector.Z();
  T() += otherVector.T();
  return *this;
} // += operator

/**
 * Overloaded Unary -= Operator.
 */
template <class Type> 
inline Vector4d<Type> &  Vector4d<Type>::operator -= (const Vector4d<Type> & otherVector) 
{
  X() -= otherVector.X();
  Y() -= otherVector.Y();
  Z() -= otherVector.Z();
  T() -= otherVector.T();
  return *this;
} // -= operator



/**
 * Equal operator.
 * @param & otherVector
 * @return Vector<Type>
 */
template <class Type> 
inline bool Vector4d<Type>::operator == (const Vector4d<Type> & otherVector) const 
{
  return(X() == otherVector.X() && Y() == otherVector.Y() && Z() == otherVector.Z() && T() == otherVector.T());
} // Equal operator

/**
 * UnEqual operator.
 * @param & otherVector
 * @return Vector<Type>
 */
template <class Type> 
inline bool Vector4d<Type>::operator != (const Vector4d<Type> & otherVector) const 
{
  return(X() != otherVector.X() || Y() != otherVector.Y() || Z() != otherVector.Z() || T() != otherVector.T());
} // Equal operator


/**
 * @return
 */
template <class Type>
std::string Vector4d<Type>::ToString() const
{
  std::strstream ss;
  ss << "Vector4d("<<X()<<", "<<Y()<<", "<<Z()<<", "<<T()<<")"<<'\0' ;
  return( std::string(ss.str()) );
} // operator<<


// $Log: Vector4d.inl,v $
// Revision 1.2  2004/04/06 12:09:15  frank
// removed unused includes
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.4  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 1.3  2003/01/22 21:07:57  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.2  2002/11/26 16:04:44  ingmar
// added operator += and -=
//
// Revision 1.1  2002/11/21 22:24:12  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Vector4d.inl,v 1.2 2004/04/06 12:09:15 frank Exp $
// $Id: Vector4d.inl,v 1.2 2004/04/06 12:09:15 frank Exp $
