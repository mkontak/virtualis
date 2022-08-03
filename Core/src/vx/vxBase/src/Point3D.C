// $Id: Point3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
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
#include "Vector4d.h"


// define declarations
#define FILE_REVISION "$Revision: 1.4 $"


// namespaces
using namespace std;


// explicit instantiations of all used types
template class VX_BASE_DLL Point3D<uint1 >;
template class VX_BASE_DLL Point3D<uint2 >;
template class VX_BASE_DLL Point3D<uint4 >;
template class VX_BASE_DLL Point3D< int1 >;
template class VX_BASE_DLL Point3D< int2 >;
template class VX_BASE_DLL Point3D< int4 >;
template class VX_BASE_DLL Point3D<float4>;
template class VX_BASE_DLL Point3D<float8>;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor & Assignment

/**
 * Copy Constructor from Triple<Type>.
 * @param & triple
 */
template <class Type> 
Point3D<Type>::Point3D(const Triple<Type> & triple)
: m_x(triple.X()), m_y(triple.Y()), m_z(triple.Z())
{
} // Copy Constructor from Triple


/**
 * Copy Constructor from Vector3D<Type>.
 * @param & vector3D
 */
template <class Type> 
Point3D<Type>::Point3D(const Vector3D<Type> & vector3D)
: m_x(vector3D.X()), m_y(vector3D.Y()), m_z(vector3D.Z())
{
} // Copy Constructor from Vector3D


/**
 * Copy Constructor from Vector4D<Type>.
 * @param & vector4D
 */
template <class Type> 
Point3D<Type>::Point3D(const Vector4d<Type> & vector4D)
: m_x(vector4D.X()), m_y(vector4D.Y()), m_z(vector4D.Z())
{
} // Copy Constructor from Vector4D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const info


/**
 * @return the component value of the component with the maximum signed value
 */
template <class Type>
const Type & Point3D<Type>::GetMaxComponent() const
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
template <>
const double &  Point3D<double>::GetMaxAbsComponent() const
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
template <>
const long double &  Point3D<long double>::GetMaxAbsComponent() const
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
const Type &  Point3D<Type>::GetMaxAbsComponent() const
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
const int4 Point3D<Type>::GetMaxComponentCoord() const
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
template <> 
int4 const Point3D<double>::GetMaxAbsComponentCoord() const
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
int4 const Point3D<long double>::GetMaxAbsComponentCoord() const
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
int4 const Point3D<Type>::GetMaxAbsComponentCoord() const
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
 * The Point3D scaled anisotropically
 * @param & scale3D
 * @return
 */
template <class Type> 
const Point3D<Type> Point3D<Type>::GetScaledAnIso(const Triplef & scale3D) const
{ 
  return Point3D<Type>(static_cast<Type>( X()*scale3D.X() ), 
                       static_cast<Type>( Y()*scale3D.Y() ), 
                       static_cast<Type>( Z()*scale3D.Z() ));
} // GetScaledAnIso()


/**
 * The Point3D scaled anisotropically
 * @param & divide3D
 * @return
 */
template <class Type> 
const Point3D<Type> Point3D<Type>::GetDividedAnIso(const Triplef & divide3D) const
{ 
  return Point3D<Type>(static_cast<Type>( X()/divide3D.X() ), 
                       static_cast<Type>( Y()/divide3D.Y() ), 
                       static_cast<Type>( Z()/divide3D.Z() ));
} // GetDividedAnIso()


/**
 * @return the length as a float8 based on Eucledian distance between this point and the parameter Point3D
 */
template <class Type> 
const float8 Point3D<Type>::GetEuclideanDistance(const Point3D<Type> & point3D) const 
{
  return Vector3D<float8>(X()-point3D.X(), Y()-point3D.Y(), Z()-point3D.Z()).GetLength();
} // GetEuclideanDistance() const


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * Clamp the values to a range.
 * @param minimum           the lower bound.
 * @param maximum           the upper bound.
 */
template <class Type>
void Point3D<Type>::Clamp(const Type & minimum, const Type & maximum)
{
  m_x = Bound(minimum, m_x, maximum);
  m_y = Bound(minimum, m_y, maximum);
  m_z = Bound(minimum, m_z, maximum);
} // Clamp()


/**
 * Rotate the three data members.
 * to should be within -2 to 2.
 * @param iTo
 * @return the three data members
 */
template <class T> 
void Point3D<T>::Permute(const int4 iTo)
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
 * @return *this
 */
template <class Type> 
Point3D<Type> & Point3D<Type>::ScaleAnIso(const Triplef & scale3D) 
{ 
  X()*=scale3D.X(); 
  Y()*=scale3D.Y(); 
  Z()*=scale3D.Z(); 
  return *this;
} // ScaledAnIso()


/**
 * Divide this Point3D anisotropically
 * @param & divide3D
 * @return *this
 */
template <class Type> 
Point3D<Type> & Point3D<Type>::DivideAnIso(const Triplef & divide3D) 
{ 
  X()/=divide3D.X(); 
  Y()/=divide3D.Y(); 
  Z()/=divide3D.Z(); 
  return *this;
} // DivideAnIso()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Status reports

/**
 * convert class state to string
 * @return
 */
template <class Type> 
std::string Point3D<Type>::ToString() const
{
  std::strstream ss; 
  ss << "Point3D("<<X()<<", "<<Y()<<", "<<Z()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


/**
 * Parsing and setting of data members from an istream. <BR>
 * Assumes that the data is formatted either as         <BR>
 * number whitespace number whitespace number           <BR>
 * or as                                                <BR>
 * "Point3D(" number "," number "," number ")"          <BR>
 * 
 * @param is     the input stream with the Point3D info.
 * 
 * @return the input stream without the Point3D info.
 */
template <class Type> 
std::istream & Point3D<Type>::SetFromIStream(std::istream & is)
{ 
  // first skip leading whitespace
  int1 nextCharacter=is.peek();;
  while (nextCharacter == ' ' || nextCharacter == '\t')
  {
    nextCharacter = is.get();
    nextCharacter = is.peek();
  }

  // now determine which format we are reading
  if (nextCharacter == 'P')
  {
    //// We are reading "Point3D(x,y,z)"

    // skipp "Point3D("
    char iOneChar='x';
    for ( ; iOneChar!= '('; is >> iOneChar )
    {
      if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Point3D","istream","operator>>(Point3D<T>)"));
    }

    is >> X() >> iOneChar; //","
    is >> Y() >> iOneChar; //","
    is >> Z() >> iOneChar; //")"
    if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Point3D","istream","operator>>(Point3D<T>)"));
  } // end "Point3D(x,y,z)" 
  else
  {
    //// We are reading "x y z"
    is >> X() >> Y() >> Z();
  } // end "x y z"

  return is;
} // SetFromIStream


#undef FILE_REVISION

// $Log: Point3D.C,v $
// Revision 1.4  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:12:52  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/08/01 19:11:13  frank
// changed function name according to coding standards
//
// Revision 1.2  2004/03/04 19:05:33  geconomos
// Fixed up code to resolve ambiguity errors with math functions caught with the 7.1 complier
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:43:47  michael
// code review
//
// Revision 1.6  2003/05/15 15:54:39  frank
// spelling
//
// Revision 1.5  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 1.4  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.3  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 1.2  2003/01/22 21:01:45  ingmar
// initial version completed
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Point3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
// $Id: Point3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
