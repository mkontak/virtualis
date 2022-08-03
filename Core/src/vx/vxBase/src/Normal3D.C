// $Id: Normal3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
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
template class VX_BASE_DLL Normal3D<float4>;
template class VX_BASE_DLL Normal3D<float8>;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor & Assignment

/**
 * Copy Constructor from Triple<Type>.
 *
 * @param triple
 */
template <class Type> 
Normal3D<Type>::Normal3D(const Triple<Type> & triple)
: m_x(triple.X()), m_y(triple.Y()), m_z(triple.Z())
{
  Normalize();
} // Copy Constructor from Triple


/**
 * Copy Constructor from Point3D<Type>.
 *
 * @param point3D
 */
template <class Type> 
Normal3D<Type>::Normal3D(const Point3D<Type> & point3D)
: m_x(point3D.X()), m_y(point3D.Y()), m_z(point3D.Z())
{
  Normalize();
} // Copy Constructor from Point3D


/**
 * Copy Constructor from Vector3D<Type>.
 *
 * @param vector3D
 */
template <class Type> 
Normal3D<Type>::Normal3D(const Vector3D<Type> & vector3D)
: m_x(vector3D.X()), m_y(vector3D.Y()), m_z(vector3D.Z())
{
  Normalize();
} // Copy Constructor from Vector3D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const info

/**
 * @return the component value of the component with the maximum signed value
 */
template <class Type>
const Type & Normal3D<Type>::GetMaxComponent() const
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
template <class Type>
const Type &  Normal3D<Type>::GetMaxAbsComponent() const
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
} // GetMaxAbsComponent()


/**
 * @return the coordinate axis index of the component with the maximum signed value
 */
template <class Type>
const int4 Normal3D<Type>::GetMaxComponentCoord() const
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
template <class Type> 
const int4 Normal3D<Type>::GetMaxAbsComponentCoord() const
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
} // GetMaxAbsComponentCoord()


/**
 * @param scale3D
 * @returns a new Vector3D with each component scaled by the corresponding component of scale3D.
 */
template <class Type> 
const Vector3D<Type> Normal3D<Type>::GetScaledAnIso(const Triple<Type> & scale3D) const 
{
  return(Vector3D<Type>(X() * scale3D.X(), 
                        Y() * scale3D.Y(), 
                        Z() * scale3D.Z()) );
} // GetScaleUniso()


/**
 * @param divide3D
 * @return a new Vector3D with each component divided by the corresponding component of divide3D.
 */
template <class Type> 
const Vector3D<Type> Normal3D<Type>::GetDividedAnIso(const Triple<Type> & divide3D) const 
{
  return(Vector3D<Type>(X() * divide3D.X(), 
                        Y() * divide3D.Y(), 
                        Z() * divide3D.Z()) );
} // GetDividedAnIso()


/**
 * The dot product of this Normal3D<Type> with a Vector3D<Type>
 *
 * @param vector3D
 * @return dot product
 */
template <class Type> 
const Type Normal3D<Type>::Dot(const Vector3D<Type> & vector3D) const
{
  return(X()*vector3D.X() + 
         Y()*vector3D.Y() + 
         Z()*vector3D.Z()); 
} // Dot product with Vector3D


/**
 * The cross product of this Normal3D<Type> with a Vector3D<Type>
 *
 * @param vector3D
 * @return cross product
 */
template <class Type> 
const Vector3D<Type> Normal3D<Type>::Cross(const Vector3D<Type> & vector3D) const
{
  return Vector3D<Type>(Y()*vector3D.Z() - Z()*vector3D.Y(), 
                        Z()*vector3D.X() - X()*vector3D.Z(), 
                        X()*vector3D.Y() - Y()*vector3D.X());
} // Cross product with Vector3D


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * Rotate the three data members.
 * to should be within -2 to 2.
 *
 * @param iTo
 */
template <class T> 
void Normal3D<T>::Permute(const int4 iTo)
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
 * Normalizes the Vector (X()*X() + Y()*Y() + Z()*Z() == 1).
 */
template <class Type> 
void Normal3D<Type>::Normalize()
{
  float8 fLengthSquare = X()*X() + Y()*Y() + Z()*Z();
  if (fLengthSquare > 0)
  {
    float8 fReci = 1.0/sqrt(fLengthSquare);
    m_x *= fReci;
    m_y *= fReci;
    m_z *= fReci;
  }
  else
  {
    //LogWrn("tried to normalize Normal3D(0,0,0)", "Normal3D","Normalize");
    // throw ex::IllegalArgumentException(LogRec("tried to normalize Normal3D(0,0,0)", "Normal3D","Normalize"));
  }
} // Normalize().


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// status reports

/**
 * convert class state to string
 *
 * @return string
 */
template <class Type> 
std::string Normal3D<Type>::ToString() const
{
  std::strstream ss; 
  ss << "Normal3D("<<X()<<", "<<Y()<<", "<<Z()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


/**
 * Parsing and setting of data members from an istream. <BR>
 * Assumes that the data is formatted either as         <BR>
 * number whitespace number whitespace number           <BR>
 * or as                                                <BR>
 * "Normal3D(" number "," number "," number ")"          <BR>
 * 
 * @param is     the input stream with the Normal3D info.
 * 
 * @return the input stream without the Normal3D info.
 */
template <class Type> 
std::istream & Normal3D<Type>::SetFromIStream(std::istream & is)
{ 
  // first skip leading whitespace
  int1 nextCharacter=is.peek();
  while (nextCharacter == ' ' || nextCharacter == '\t')
  {
    nextCharacter = is.get();
    nextCharacter = is.peek();
  }

  // now determine which format we are reading
  if (nextCharacter == 'N')
  {
    //// We are reading "Normal3D(x,y,z)"

    // skipp "Normal3D("
    char iOneChar='x';
    for ( ; iOneChar!= '('; is >> iOneChar )
    {
      if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Normal3D","istream","operator>>(Normal3D<T>)"));
    }

    is >> m_x >> iOneChar; //","
    is >> m_y >> iOneChar; //","
    is >> m_z >> iOneChar; //")"
    if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for Normal3D","istream","operator>>(Normal3D<T>)"));
  } // end "Normal3D(x,y,z)" 
  else
  {
    //// We are reading "x y z"
    is >> m_x >> m_y >> m_z;
  } // end "x y z"

  Normalize();
  return is;
} // SetFromIStream



#undef FILE_REVISION


// $Log: Normal3D.C,v $
// Revision 1.4  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:12:52  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.1.1.1.16.1  2005/09/21 20:04:32  geconomos
// No longer throwing an exception when trying to normalize when all components are zero. (logging an error instead)
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/08/28 16:52:37  frank
// formatting
//
// Revision 1.9  2003/05/16 11:43:46  michael
// code review
//
// Revision 1.8  2003/05/15 15:54:22  frank
// spelling
//
// Revision 1.7  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 1.6  2003/02/11 00:59:13  jmeade
// Normalize() exception message if COLON and FINAL_RELEASE defined, exception if COLON not defined.
//
// Revision 1.5  2003/02/05 20:18:46  kevin
// Fix for Ingmar change from Normal to Normal3D> This should make Colon go back to the way it was before.
//
// Revision 1.4  2003/02/05 20:03:34  kevin
// Fix for Ingmar change from Normal to Normal3D> This should make Colon go back to the way it was before.
//
// Revision 1.3  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.2  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 1.1  2003/01/22 20:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Normal3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
// $Id: Normal3D.C,v 1.4 2006/01/18 20:36:48 geconomos Exp $
