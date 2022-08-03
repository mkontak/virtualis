// $Id: Vector3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


// include declarations
#include "stdafx.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"


// define declarations
#define FILE_REVISION "$Revision: 1.4 $"


// namespaces
using namespace std;


// explicit instantiations of all used types
template class VX_BASE_DLL Vector3D<uint1>;
template class VX_BASE_DLL Vector3D<uint2>;
template class VX_BASE_DLL Vector3D<uint4>;
template class VX_BASE_DLL Vector3D< int1>;
template class VX_BASE_DLL Vector3D< int2>;
template class VX_BASE_DLL Vector3D< int4>;
template class VX_BASE_DLL Vector3D<float4>;
template class VX_BASE_DLL Vector3D<float8>;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor & Assignment

/**
 * Copy Constructor from Triple<Type>.
 * @param triple
 */
template <class Type> 
Vector3D<Type>::Vector3D(const Triple<Type> & triple)
: m_x(triple.X()), m_y(triple.Y()), m_z(triple.Z())
{
} // Copy Constructor from Triple


/**
 * Copy Constructor from Point3D<Type>.
 * @param point3D
 */
template <class Type> 
Vector3D<Type>::Vector3D(const Point3D<Type> & point3D)
: m_x(point3D.X()), m_y(point3D.Y()), m_z(point3D.Z())
{
} // Copy Constructor from Point3D


/**
 * Copy Constructor from Normal3D<Type>.
 * @param normal3D
 */
template <class Type> 
Vector3D<Type>::Vector3D(const Normal3D<Type> & normal3D)
: m_x(normal3D.X()), m_y(normal3D.Y()), m_z(normal3D.Z())
{
} // Copy Constructor from Normal3D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const info

/**
 * @return the component value of the component with the maximum signed value
 */
template <class Type>
const Type & Vector3D<Type>::GetMaxComponent() const
{
  if (X() > Y())
    if (X() > Z())
      return X();
    else
      return Z();
  else
    if (Y() > Z())
    return Y();
  else
    return Z();
} // GetMaxComponent()


/**
 * @return the component value of the component with the maximum absolute value
 */
template<>
const double & Vector3D<double>::GetMaxAbsComponent() const
{
  if (fabs(X()) > fabs(Y()))
    if (fabs(X()) > fabs(Z()))
      return X();
    else
      return Z();
  else
    if (fabs(Y()) > fabs(Z()))
    return Y();
  else
    return Z();
} // GetAbsMaxComponent()


/**
 * @return the component value of the component with the maximum absolute value
 */
template<>
const long double & Vector3D<long double>::GetMaxAbsComponent() const
{
  if (fabs(X()) > fabs(Y()))
    if (fabs(X()) > fabs(Z()))
      return X();
    else
      return Z();
  else
    if (fabs(Y()) > fabs(Z()))
    return Y();
  else
    return Z();
} // GetAbsMaxComponent()

/**
 * @return the component value of the component with the maximum absolute value
 */
template <class Type>
const Type & Vector3D<Type>::GetMaxAbsComponent() const
{
  if (fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Y())))
    if (fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Z())))
      return X();
    else
      return Z();
  else
    if (fabs(static_cast<float4>(Y())) > fabs(static_cast<float4>(Z())))
    return Y();
  else
    return Z();
} // GetAbsMaxComponent()


/**
 * @return the coordinate axis index of the component with the maximum signed value
 */
template <class Type>
const int4 Vector3D<Type>::GetMaxComponentCoord() const
{
  if (X() > Y())
    if (X() > Z())
      return 0;
    else
      return 2;
  else
    if (Y() > Z())
    return 1;
  else
    return 2;
} // GetMaxComponentCoord()


/**
 * @return the coordinate axis index of the component with the maximum absolute value
 */
template<> 
const int4 Vector3D<double>::GetMaxAbsComponentCoord() const
{
  if (fabs(X()) > fabs(Y()))
    if (fabs(X()) > fabs(Z()))
      return 0;
    else
      return 2;
  else
    if (fabs(Y()) > fabs(Z()))
    return 1;
  else
    return 2;
} // GetAbsMaxComponentCoord()


/**
 * @return the coordinate axis index of the component with the maximum absolute value
 */
template <> 
const int4 Vector3D<long double>::GetMaxAbsComponentCoord() const
{
  if (fabs(X()) > fabs(Y()))
    if (fabs(X()) > fabs(Z()))
      return 0;
    else
      return 2;
  else
    if (fabs(Y()) > fabs(Z()))
    return 1;
  else
    return 2;
} // GetAbsMaxComponentCoord()


/**
 * @return the coordinate axis index of the component with the maximum absolute value
 */
template <class Type> 
const int4 Vector3D<Type>::GetMaxAbsComponentCoord() const
{
  if (fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Y())))
    if (fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Z())))
      return 0;
    else
      return 2;
  else
    if (fabs(static_cast<float4>(Y())) > fabs(static_cast<float4>(Z())))
    return 1;
  else
    return 2;
} // GetAbsMaxComponentCoord()


/**
 * @return the length as a float8 based on Euclidean distance.
 */
template <class Type> 
const float8 Vector3D<Type>::GetLength() const 
{
  return(sqrt(static_cast< double >( (X() * X() + Y() * Y() + Z() * Z()))));
} // GetLength() const


/**
 * @return a Normalized Vector3D<Type>.
 */
template <class Type> 
const Vector3D<Type> Vector3D<Type>::GetNormalized() const
{
  float8 fLengthSquare = X()*X() + Y()*Y() + Z()*Z();
  if (fLengthSquare > 0)
  {
    float8 fReci = 1.0/sqrt(fLengthSquare);
    return(Vector3D<Type>(X() * fReci,
                          Y() * fReci,
                          Z() * fReci )); 
  }
  else
  {
#ifdef COLON
  #ifdef FINAL_RELEASE
    LogWrn("tried to normalize Vector(0,0,0)", "Vector3D","GetNormalized");
  #else
    LogErr("tried to normalize Vector(0,0,0)", "Vector3D","GetNormalized");
  #endif
    return Vector3D<Type>(static_cast<Type>(0),static_cast<Type>(0),static_cast<Type>(1));
#else
    throw ex::IllegalArgumentException(LogRec("tried to normalize Vector(0,0,0)", "Vector3D","GetNormalized"));
#endif
  }
} // GetNormalized()


/**
 * The Vector3D scaled anisotropically
 * @param scale3D
 * @return
 */
template <class Type> 
const Vector3D<Type> Vector3D<Type>::GetScaledAnIso(const Triple<float4> & scale3D) const
{ 
  return Vector3D<Type>(static_cast<Type>( X()*scale3D.X() ), 
                        static_cast<Type>( Y()*scale3D.Y() ), 
                        static_cast<Type>( Z()*scale3D.Z() ));
} // GetScaledAnIso()


/**
 * The Vector3D divided anisotropically
 * @param & divide3D
 * @return
 */
template <class Type> 
const Vector3D<Type> Vector3D<Type>::GetDividedAnIso(const Triple<float4> & divide3D) const
{ 
  return Vector3D<Type>(static_cast<Type>( X()/divide3D.X() ), 
                        static_cast<Type>( Y()/divide3D.Y() ), 
                        static_cast<Type>( Z()/divide3D.Z() ));
} // GetDividedAnIso()


/**
 * The dot product of this Vector3D<Type> with a Normal3D<Type>.
 * @param & normal3D
 * @return
 */
template <class Type> 
inline const Type Vector3D<Type>::Dot(const Normal3D<Type> & normal3D) const 
{
  return(X() * normal3D.X() + 
         Y() * normal3D.Y() + 
         Z() * normal3D.Z()); 
} // Dot product with Normal3D


/**
 * The Cross product of this Vector3D<Type> with a Normal3D<Type>.
 * @param & normal3D
 * @return
 */
template <class Type> 
inline const Vector3D<Type> Vector3D<Type>::Cross(const Normal3D<Type> & normal3D) const 
{
  return(Vector3D<Type>(Y()*normal3D.Z() - Z()*normal3D.Y(), 
                        Z()*normal3D.X() - X()*normal3D.Z(), 
                        X()*normal3D.Y() - Y()*normal3D.X()));
} // Cross product with Normal3D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * Clamp the values to a range.
 * @param minimum           the lower bound.
 * @param maximum           the upper bound.
 */
template <class T>
void Vector3D<T>::Clamp(const T & minimum, const T & maximum)
{
  m_x = Bound(minimum, m_x, maximum);
  m_y = Bound(minimum, m_y, maximum);
  m_z = Bound(minimum, m_z, maximum);
} // Clamp()


/**
 * Rotate the three data members.
 * to should be within -2 to 2.
 * @param iTo
 */
template <class T> 
void Vector3D<T>::Permute(const int4 iTo)
{
  T tmp;
  switch ((3+iTo)%3)
  {
  case 0:
    return;
  case 1:
    tmp = m_y;
    m_y = m_x;
    m_x = m_z;
    m_z = tmp;
    break;
  case 2:
    tmp = m_z;
    m_z = m_x;
    m_x = m_y;
    m_y = tmp;
    break;
  default:
    break;
  }
} // Permute()


/**
 * Scale this Point3D anisotropically
 * @param & scale3D
 * @return
 */
template <class Type> 
Vector3D<Type> & Vector3D<Type>::ScaleAnIso(const Triple<float4> & scale3D) 
{ 
  X()*=scale3D.X(); 
  Y()*=scale3D.Y(); 
  Z()*=scale3D.Z(); 
  return *this;
} // ScaledAnIso()


/**
 * Divide this Point3D anisotropically
 * @param divide3D
 * @return
 */
template <class Type> 
Vector3D<Type> & Vector3D<Type>::DivideAnIso(const Triple<float4> & divide3D) 
{ 
  X()/=divide3D.X(); 
  Y()/=divide3D.Y(); 
  Z()/=divide3D.Z(); 
  return *this;
} // DivideAnIso()


/**
 * Normalizes the Vector (X()*X() + Y()*Y() + Z()*Z() == 1).
 */
template <class Type> 
void Vector3D<Type>::Normalize()
{
  float8 fLengthSquare = X()*X() + Y()*Y() + Z()*Z();
  if (fLengthSquare > 0)
  {
    float8 fReci = 1.0/sqrt(fLengthSquare);
    X() *= fReci;
    Y() *= fReci;
    Z() *= fReci;
  }
  else
  {
    LogErr("tried to normalize Vector3D(0,0,0)", "Vector3D","Normalize");
    // throw ex::IllegalArgumentException(LogRec("tried to normalize Vector3D(0,0,0)", "Vector3D","Normalize"));
  }
} // Normalize().


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// status reports

/**
 * convert class state to string
 * @return
 */
template <class Type> 
std::string Vector3D<Type>::ToString() const
{
  std::strstream ss; 
  ss << "Vector3D("<<X()<<", "<<Y()<<", "<<Z()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


/**
 * Parsing and setting of data members from an istream. <BR>
 * Assumes that the data is formatted either as         <BR>
 * number whitespace number whitespace number           <BR>
 * or as                                                <BR>
 * "Vector3D(" number "," number "," number ")"          <BR>
 * 
 * @param is     the input stream with the Vector3D info.
 * 
 * @return the input stream without the Vector3D info.
 */
template <class Type> 
std::istream & Vector3D<Type>::SetFromIStream(std::istream & is)
{ 
  // first skip leading whitespace
  int1 nextCharacter=is.peek();;
  while (nextCharacter == ' ' || nextCharacter == '\t')
  {
    nextCharacter = is.get();
    nextCharacter = is.peek();
  }

  // now determine which format we are reading
  if (nextCharacter == 'V')
  {
    //// We are reading "Vector3D(x,y,z)"

    // skipp "Vector3D("
    char iOneChar='x';
    for ( ; iOneChar!= '('; is >> iOneChar )
    {
      if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Vector3D","istream","operator>>(Vector3D<T>)"));
    }

    is >> X() >> iOneChar; //","
    is >> Y() >> iOneChar; //","
    is >> Z() >> iOneChar; //")"
    if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Vector3D","istream","operator>>(Vector3D<T>)"));
  } // end "Vector3D(x,y,z)" 
  else
  {
    //// We are reading "x y z"
    is >> X() >> Y() >> Z();
  } // end "x y z"

  return is;
} // SetFromIStream


#undef FILE_REVISION


// $Log: Vector3D.C,v $
// Revision 1.4  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.2.16.1  2005/09/21 20:04:32  geconomos
// No longer throwing an exception when trying to normalize when all components are zero. (logging an error instead)
//
// Revision 1.2  2004/03/04 19:05:33  geconomos
// Fixed up code to resolve ambiguity errors with math functions caught with the 7.1 complier
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.11  2003/05/15 15:54:50  frank
// spelling
//
// Revision 1.10  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 1.9  2003/02/11 00:59:13  jmeade
// Normalize() exception message if COLON and FINAL_RELEASE defined, exception if COLON not defined.
//
// Revision 1.8  2003/02/05 20:18:46  kevin
// Fix for Ingmar change from Normal to Normal3D> This should make Colon go back to the way it was before.
//
// Revision 1.7  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.6  2003/01/24 21:46:17  ingmar
// code walkthrough changes
//
// Revision 1.5  2003/01/22 23:40:07  ingmar
// added Vector3D<uint4> to allow VC to compile (link)
//
// Revision 1.4  2003/01/22 21:10:50  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Vector3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
// $Id: Vector3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $