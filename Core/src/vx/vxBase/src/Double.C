// $Id: Double.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


// include declarationss
#include "stdafx.h"
#include "Double.h"


// define declarations
#define FILE_REVISION "$Revision: 1.2 $"

// namespaces
using namespace std;

// explicit instantiations of all used types
template class Double<  int4>;    
template class Double<float4>;

///////////////////////////////////////////////////////////////////////
// math

/**
 * returns the maximum value of all absolut component values
 */
template <> 
const double & Double<double>::GetMaxAbsComponent() const
{
  if ( fabs(X()) > fabs(Y()) ) return X();
  else                         return Y();
} // GetMaxAbsComponent()

/**
 * returns the maximum value of all absolut component values
 */
template <> 
const long double & Double<long double>::GetMaxAbsComponent() const
{
  if ( fabs(X()) > fabs(Y()) ) return X();
  else                         return Y();
} // GetMaxAbsComponent()

/**
 * returns the maximum value of all absolut component values
 */
template <class Type> 
const Type & Double<Type>::GetMaxAbsComponent() const
{
  if ( fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Y())) ) return X();
  else  return Y();
} // GetMaxAbsComponent()

/**
 * returns the maximum value of all signed component values
 */
template <class Type> 
const Type & Double<Type>::GetMaxComponent() const
{
  if ( X() > Y() ) return X();
  else             return Y();
} // GetMaxComponent()


/**
 * returns the coordinate axis index of the maximum value of all absolut component values
 */
template <> 
int4 Double<double>::GetMaxAbsComponentCoord() const
{
  if ( fabs(X()) > fabs(Y()) ) return 0;
  else                         return 1;
} // GetMaxAbsComponentCoord()

/**
 * returns the coordinate axis index of the maximum value of all absolut component values
 */
template <> 
int4 Double<long double>::GetMaxAbsComponentCoord() const
{
  if ( fabs(X()) > fabs(Y()) ) return 0;
  else                         return 1;
} // GetMaxAbsComponentCoord()

/**
 * returns the coordinate axis index of the maximum value of all absolut component values
 */
template <class Type> 
int4 Double<Type>::GetMaxAbsComponentCoord() const
{
  if ( fabs(static_cast<float4>(X())) > fabs(static_cast<float4>(Y())) ) return 0;
  else  return 1;
} // GetMaxAbsComponentCoord()

/**
 * returns the coordinate axis index of the maximum value of all signed component values
 */
template <class Type> 
int4 Double<Type>::GetMaxComponentCoord() const
{
  if ( X() > Y() ) return 0;
  else             return 1;
} // GetMaxComponentCoord()


///////////////////////////////////////////////////////////////////////
// status reports

/**
 * convert class state to string
 */
template <class T> 
std::string Double<T>::ToString() const
{
  std::strstream ss; 
  ss << "Double("<<X()<<", "<<Y()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


#undef FILE_REVISION


// $Log: Double.C,v $
// Revision 1.2  2004/03/04 19:05:33  geconomos
// Fixed up code to resolve ambiguity errors with math functions caught with the 7.1 complier
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:43:45  michael
// code review
//
// Revision 1.3  2003/01/02 16:20:13  ingmar
// added GetMaxComponent() and GetMaxComponentCoord()
//
// Revision 1.2  2002/12/23 16:52:47  ingmar
// added GetMaxAbsComponent() and GetMaxAbsComponentCoord()
//
// Revision 1.1  2002/12/21 00:37:05  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Double.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $
// $Id: Double.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $