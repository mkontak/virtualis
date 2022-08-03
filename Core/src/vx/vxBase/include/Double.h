// $Id: Double.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Ingmar (ingmar@viatronix.com)


#ifndef Double_h
#define Double_h

     
/**
 * Template class to store two values x,y (dimension, units)  <BR>
 * Double<type> for any type                                  <BR>
 * Doublei for default integer values (int4)                  <BR>
 * Doublef for default floating point values (float4)         <BR>
 */
template <class Type> 
class Double
{
// Member Functions
public:
  ///////////////////////////////////////////////////////////////////////
  // constructor & assignment
  
  /// two scalar constructor
  inline Double(const Type & x=0, const Type & y=0);

  /// copy constructor
  template <class OtherType>
  inline explicit Double(const Double<OtherType> & rhs) { X()=rhs.X(); Y()=rhs.Y(); }

  /// Assignment Operator
  inline Double<Type> & operator = (const Double<Type> & rhs);


  ///////////////////////////////////////////////////////////////////////
  // Comparators

  /// Equality operator.
  inline bool operator ==(const Double<Type> & rhs) const;

  /// Inequality operator.
  inline bool operator !=(const Double<Type> & rhs) const;


  ///////////////////////////////////////////////////////////////////////
  // Access Functions

  /// Set the x and y components
  inline void SetXY(const Type & x, const Type & y);

  
  /// read access any component
  inline const Type & operator[](int4 iIndex) const { return m_xy[iIndex]; }
  /// write access any component
  inline       Type & operator[](int4 iIndex)       { return m_xy[iIndex]; }


  /// read access x component
  inline const Type & X() const { return m_xy[0]; }
  /// write access x component
  inline       Type & X()       { return m_xy[0]; }

  
  /// read access y component
  inline const Type & Y() const { return m_xy[1]; }
  /// write access y component
  inline       Type & Y()       { return m_xy[1]; }


  ///////////////////////////////////////////////////////////////////////
  // Math
    
  /// returns the maximum value of all absolut component values
  const Type & GetMaxAbsComponent() const;

  /// returns the maximum value of all signed component values
  const Type & GetMaxComponent() const;

  /// returns the coordinate axis index of the maximum value of all absolut component values
  int4 GetMaxAbsComponentCoord() const;

  /// returns the coordinate axis index of the maximum value of all signed component values
  int4 GetMaxComponentCoord() const;

  ///////////////////////////////////////////////////////////////////////
  // status reports

  /// conversion of pixel data into string (now Double can be used in an ostream)
  operator std::string() const { return ToString(); }

  /// conversion of data members into a string
  std::string ToString() const;

// Data Members
private:

  /// the two data components
  Type m_xy[2];

}; // class Double


// explicit type names for most frequently used types
typedef Double< int4 > Doublei;
typedef Double<float4> Doublef;


#include "Double.inl"

#endif // Double_h

// $Log: Double.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/01/02 19:37:31  ingmar
// better documentation structure
//
// Revision 1.5  2001/01/02 18:51:33  ingmar
// better documentation structure
//
// Revision 1.4  2001/01/02 16:20:13  ingmar
// added GetMaxComponent() and GetMaxComponentCoord()
//
// Revision 1.3  2002/12/23 22:51:00  ingmar
// code beautify
//
// Revision 1.2  2002/12/23 16:52:47  ingmar
// added GetMaxAbsComponent() and GetMaxAbsComponentCoord()
//
// Revision 1.1  2002/12/21 00:37:05  ingmar
// initial version
//
// $Id: Double.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Double.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $

