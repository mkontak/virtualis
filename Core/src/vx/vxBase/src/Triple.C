// $Id: Triple.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $
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


// define declarations
#define FILE_REVISION "$Revision: 1.3 $"


// namespaces
using namespace std;


// explicit instantiations of all used types
template class VX_BASE_DLL Triple<uint1 >;
template class VX_BASE_DLL Triple<uint2 >;
template class VX_BASE_DLL Triple<uint4 >;
template class VX_BASE_DLL Triple< int1 >;
template class VX_BASE_DLL Triple< int2 >;
template class VX_BASE_DLL Triple< int4 >;
template class VX_BASE_DLL Triple<float4>;
template class VX_BASE_DLL Triple<float8>;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor & Assignment

/**
 * Copy Constructor from Point3D<Type>.
 */
template < class Type >
Triple<Type>::Triple(const Point3D<Type> & point3D)
: m_x(point3D.X()), m_y(point3D.Y()), m_z(point3D.Z())
{
} // Copy Constructor from Point3D


/**
 * Copy Constructor from Vector3D<Type>.
 */
template <class Type> 
Triple<Type>::Triple(const Vector3D<Type> & vector3D)
: m_x(vector3D.X()), m_y(vector3D.Y()), m_z(vector3D.Z())
{
} // Copy Constructor from Vector3D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const info


/**
 * returns the component value of the component with the minimum signed value
 *
 * @return the minimum component
 */
template <class Type>
const Type & Triple<Type>::GetMinComponent() const
{

  if ( m_x < m_y )
    return ( m_x < m_z ) ? m_x : m_z;
  else
    return ( m_y < m_z ) ? m_y : m_z;

} // GetMinComponent()


/**
 * returns the component value of the component with the maximum signed value
 *
 * @return the maximum component
 */
template <class Type>
const Type & Triple<Type>::GetMaxComponent() const
{

  if ( m_x > m_y )
    return ( m_x > m_z ) ? m_x : m_z;
  else
    return ( m_y > m_z ) ? m_y : m_z;

} // GetMaxComponent()


/**
 * returns the component value of the component with the maximum absolute value
 */
template <>
const double &  Triple<double>::GetMaxAbsComponent() const
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
 * returns the component value of the component with the maximum absolute value
 */
template <>
const long double &  Triple<long double>::GetMaxAbsComponent() const
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
 * returns the component value of the component with the maximum absolute value
 */
template <class Type>
const Type &  Triple<Type>::GetMaxAbsComponent() const
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
 * returns the coordinate axis index of the component with the maximum signed value
 */
template <class Type>
const int4 Triple<Type>::GetMaxComponentCoord() const
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
 * returns the coordinate axis index of the component with the maximum absolute value
 */
template <> 
const int4 Triple<double>::GetMaxAbsComponentCoord() const
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
 * returns the coordinate axis index of the component with the maximum absolute value
 */
template <> 
const int4 Triple<long double>::GetMaxAbsComponentCoord() const
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
 * returns the coordinate axis index of the component with the maximum absolute value
 */
template <class Type> 
const int4 Triple<Type>::GetMaxAbsComponentCoord() const
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * Clamp the values to a range.
 * @param minimum           the lower bound.
 * @param maximum           the upper bound.
 */
template <class Type>
void Triple<Type>::Clamp(const Type & minimum, const Type & maximum)
{
  m_x = Bound(minimum, m_x, maximum);
  m_y = Bound(minimum, m_y, maximum);
  m_z = Bound(minimum, m_z, maximum);
} // Clamp()


/**
 * Rotate the three data members.
 * to should be within -2 to 2.
 * @return void 
 */
template <class T> 
void Triple<T>::Permute(const int4 iTo)
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Status reports

/**
 * convert class state to string
 */
template <class Type> 
std::string Triple<Type>::ToString() const
{
  std::strstream ss; 
  ss << "Triple("<<X()<<", "<<Y()<<", "<<Z()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


/**
 * Parsing and setting of data members from an istream. <BR>
 * Assumes that the data is formatted either as         <BR>
 * number whitespace number whitespace number           <BR>
 * or as                                                <BR>
 * "Triple(" number "," number "," number ")"           <BR>
 * 
 * @param is     the input stream with the triple info.
 * 
 * @return the input stream without the Triple info.
 */
template <class Type> 
std::istream & Triple<Type>::SetFromIStream(std::istream & is)
{ 
  // first skip leading whitespace
  char nextCharacter = is.peek();
  while (nextCharacter == ' ' || nextCharacter == '\t')
  {
    nextCharacter = is.get();
    nextCharacter = is.peek();
  }

  // now determine which format we are reading
  if (nextCharacter == 'T')
  {
    //// We are reading "Triple(x,y,z)"

    // skipp "Triple("
    char iOneChar='x';
    for ( ; iOneChar!= '('; is >> iOneChar )
      if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Triple","istream","operator>>(Triple<T>)"));

    is >> X() >> iOneChar; //","
    is >> Y() >> iOneChar; //","
    is >> Z() >> iOneChar; //")"
    if (!is.good())   throw ex::EOFException(LogRec("End of file reached while looking for Triple","istream","operator>>(Triple<T>)"));
  } // end "Triple(x,y,z)" 
  else
  {
    //// We are reading "x y z"
    is >> X() >> Y() >> Z();
  } // end "x y z"

  return is;
} // SetFromIStream


#undef FILE_REVISION

// $Log: Triple.C,v $
// Revision 1.3  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2004/03/04 19:05:33  geconomos
// Fixed up code to resolve ambiguity errors with math functions caught with the 7.1 complier
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/16 11:48:47  michael
// code review
//
// Revision 3.7  2003/05/15 15:54:09  frank
// spelling
//
// Revision 3.6  2003/05/15 13:39:46  frank
// Added GetMinComponent()
//
// Revision 3.5  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 3.4  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 3.3  2003/01/22 20:55:24  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Triple.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $
// $Id: Triple.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $