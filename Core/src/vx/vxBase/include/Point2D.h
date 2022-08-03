// $Id: Point2D.h,v 1.4 2006/10/04 01:15:01 jmeade Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Point2D_h
#define Point2D_h

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000


// Includes

#include "Point.h"



/**
 * Template class Point2D<T>
 * Point2D is a 2 dimension point, it just has x and y
 * coordinates.
 * $Id: Point2D.h,v 1.4 2006/10/04 01:15:01 jmeade Exp $ 
 */
template <class Type>
class Point2D 
{
// member functions
public:

  /// Default Constructor
  Point2D<Type>();

  /// Constructor from two coordinates: x and y
  Point2D<Type>(const Type & x, const Type & y);

  /// Copy constructor
  template<class T2>
  Point2D<Type>(const Point2D<T2> & other)
  {
    m_x = static_cast<Type>(other.m_x);
    m_y = static_cast<Type>(other.m_y); 
  }

  /// Constructor from Point2D<Type>
  Point2D<Type>(const Point2D<Type> & other);

  /// Assignment operator from x,y coordinates
  inline Point2D<Type> & Set(const Type & x, const Type & y);

  /// Assignment operator from Point2D<Type>
  inline const Point2D<Type> & operator=(const Point2D<Type> & other);

  ///////////////////////////////////////////////////////////////////////
  // Comparators

  /// binary operator Equals.
  inline const bool operator == (const Point2D<Type> & other) const;

  /// binary operator Not Equals.
  inline const bool operator != (const Point2D<Type> & other) const;


  ///////////////////////////////////////////////////////////////////////
  // Access Functions

  /// read access any component
  inline const Type & operator[](int4 iIndex) const { return((Type*)(&m_x))[iIndex];}

  /// write access any component
  inline       Type & operator[](int4 iIndex)       { return((Type*)(&m_x))[iIndex];}

  /// read access x component
  inline const Type & X() const { return m_x;}

  /// write access x component
  inline       Type & X()       { return m_x;}

  /// read access y component
  inline const Type & Y() const { return m_y;}

  /// write access y component
  inline       Type & Y()       { return m_y;}

  /// conversion of data members into a string
  std::string ToString() const;


  ///////////////////////////////////////////////////////////////////////
  // Math

  /// Unary + operator
  inline Point2D<Type> operator+() const;

  /// Unary - operator
  inline Point2D<Type> operator-() const;

  /// binary + operator: takes a Point2D<Type>
  inline Point2D<Type> operator+(const Point2D<Type> & other) const;

  /// binary - operator: takes a Point2D<Type>
  inline Point2D<Type> operator-(const Point2D<Type> & other) const;

  /// binary * operator: takes a Point2D<Type>
  inline Point2D<Type> operator*(const Point2D<Type> & other) const;

  /// binary / operator: takes a Point2D<Type>
  inline Point2D<Type> operator/(const Point2D<Type> & other) const;

  /// binary += operator: takes a Point2D<Type>
  inline Point2D<Type> & operator+=(const Point2D<Type> & other);

  /// binary -= operator: takes a Point2D<Type>
  inline Point2D<Type> & operator-=(const Point2D<Type> & other);

  /// binary *= operator: takes a Point2D<Type>
  inline Point2D<Type> & operator*=(const Point2D<Type> & other);

  /// binary /= operator: takes a Point2D<Type>
  inline Point2D<Type> & operator/=(const Point2D<Type> & other);

  /// binary + operator: takes a scalar value
  inline Point2D<Type> operator+(const Type & scalar) const;

  /// binary - operator: takes a scalar value
  inline Point2D<Type> operator-(const Type & scalar) const;

  /// binary * operator: takes a scalar value
  inline Point2D<Type> operator*(const Type & scalar) const;

  /// binary / operator: takes a scalar value
  inline Point2D<Type> operator/(const Type & scalar) const;

  /// binary += operator: takes a scalar value
  inline Point2D<Type> & operator+=(const Type & scalar);

  /// binary -= operator: takes a scalar value
  inline Point2D<Type> & operator-=(const Type & scalar);

  /// binary *= operator: takes a scalar value
  inline Point2D<Type> & operator*=(const Type & scalar);

  /// binary /= operator: takes a scalar value
  inline Point2D<Type> & operator/=(const Type & scalar);

  /// Test function to verify and demonstrate the class functionality.
  static bool Test(std::ostream & os=std::cout);

  /// Test function to verify just one particular template instance 
  static bool TestByType(std::ostream & os=std::cout);

  /// << Operator.
  friend std::ostream & operator<<(std::ostream & os, const Point2D<Type> & point)
  { os << "Point2D:(" ; os << point.m_x << ","; os << point.m_y << ")";   return os; }

  /// >> Operator.
  friend std::istream & operator>>(std::istream & is, Point2D<Type> & point)
  {
    char iOneChar='x';
    for ( ; iOneChar!= '('; is >> iOneChar );
    char oneCharacter;
    std::strstream ss;
    do 
    {
      is >> oneCharacter;
      if (isalnum(oneCharacter))
        ss << oneCharacter;
      else
        ss << ' ';
    } while(oneCharacter != ')');
    ss >> point.m_x;
    ss >> point.m_y;
    return is;
  }

public:
  /// the x component
  Type m_x;

  /// the y component
  Type m_y;
}; // end of Point2D class


#include "Point2D.inl"


#endif


// $Log: Point2D.h,v $
// Revision 1.4  2006/10/04 01:15:01  jmeade
// fixing point2D read for integers choking on comma character (due to internationalized numbers).
//
// Revision 1.3  2005/08/05 13:02:33  geconomos
// moved implementation of operator<< and operator>> to .h
//
// Revision 1.2  2004/03/04 19:02:47  geconomos
// Removed specializations of casting operator.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 3.13  2003/05/16 11:43:47  michael
// code review
//
// Revision 3.12  2003/03/04 17:59:42  frank
// Added ToString()
//
// Revision 3.11  2003/01/13 19:29:14  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 3.7  2003/01/09 00:11:57  ingmar
// No longer derived from Point3D
//
// Revision 3.6  2001/01/02 18:44:53  ingmar
// added missing include
//
// Revision 3.5  2002/10/14 15:34:29  michael
// made assignment operator to take const obejct reference and return const obj ref
//
// Revision 3.4  2002/09/25 20:27:46  ingmar
// replaced pow(x,2)  with x*x
//
// Revision 3.3  2002/03/11 16:46:21  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.2  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.1.2.1  2002/03/07 19:53:12  binli
// moved out the global GL render function GLDrawCircle()
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
//    Rev 2.1   Sep 19 2001 14:33:16   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:14   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.7  2001/08/23 17:50:11  sarang
// linux/gcc fix.
//
// Revision 1.6  2001/07/20 13:26:09  geconomos
// white space(sorry)
//
// Revision 1.5  2001/07/11 22:26:39  liwei
// Added copy constructor with an object of different template.
//
// Revision 1.4  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.3  2001/05/30 19:31:56  ana
// beautify code
//
// Revision 1.2  2001/05/21 21:42:19  ana
// test function added
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Point2D.h,v 1.4 2006/10/04 01:15:01 jmeade Exp $
// $Id: Point2D.h,v 1.4 2006/10/04 01:15:01 jmeade Exp $

