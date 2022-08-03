// $Id: Triple.h,v 1.2 2004/03/04 19:02:47 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Triple_h
#define Triple_h


/**
 * Template class to store three values x,y,z (dimension, units) <BR>
 * Triple<Type> for any type                                     <BR>
 * Triplei for default integer values (int4)                     <BR>
 * Triplef for default floating point values (float4)            <P>
 * $Id: Triple.h,v 1.2 2004/03/04 19:02:47 geconomos Exp $
 */
template <class Type>
class Triple 
{
// Public Member Functions.
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Constructor & Assignment

  /// Default Constructor.
  inline Triple<Type>() : m_x(static_cast<Type>(0)), m_y(static_cast<Type>(0)), m_z(static_cast<Type>(0)) { }
  

  /// Constructor from three scalars.
  inline Triple<Type>(const Type & x, const Type & y, const Type & z) : m_x(x), m_y(y), m_z(z) { }

  /// Copy Constructor from Triple<Type>.
  inline   Triple<Type>(const Triple  <Type> & triple  ) : m_x(triple.X()), m_y(triple.Y()), m_z(triple.Z()) { }

  /// Copy Constructor from Point3D<Type>.
  explicit Triple<Type>(const Point3D <Type> & point3D );

  /// Copy Constructor from Vector3D<Type>.
  explicit Triple<Type>(const Vector3D<Type> & vector3D);


  /// Assignment from Triple<Type>.
  inline Triple<Type> & operator = (const Triple<Type> & triple) { m_x=triple.X(); m_y=triple.Y(); m_z=triple.Z(); return *this;} 

  /// Casting Operator (same class, other template parameter)
  template <class OtherType> inline operator Triple<OtherType>() const { return Triple<OtherType>(static_cast<OtherType>(m_x), static_cast<OtherType>(m_y), static_cast<OtherType>(m_z));}
  inline operator Triple<uint2 >() const { return Triple<uint2 >(static_cast<uint2 >(m_x), static_cast<uint2 >(m_y), static_cast<uint2 >(m_z));}
  inline operator Triple<uint4 >() const { return Triple<uint4 >(static_cast<uint4 >(m_x), static_cast<uint4 >(m_y), static_cast<uint4 >(m_z));}
  inline operator Triple< int4 >() const { return Triple< int4 >(static_cast< int4 >(m_x), static_cast< int4 >(m_y), static_cast< int4 >(m_z));}
  inline operator Triple<float4>() const { return Triple<float4>(static_cast<float4>(m_x), static_cast<float4>(m_y), static_cast<float4>(m_z));}

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Access Functions

  /// Sets 'this' to the three args passed.
  inline void SetXYZ(const Type & x, const Type & y, const Type & z) { m_x=x; m_y=y; m_z=z;}

  /// Read access any component
  inline const Type & operator[](int4 iIndex) const { return((Type*)(&m_x))[iIndex];}

  /// Write access any component
  inline       Type & operator[](int4 iIndex)       { return((Type*)(&m_x))[iIndex];}

  /// Read access x component
  inline const Type & X() const { return m_x;}

  /// Write access x component
  inline       Type & X()       { return m_x;}

  /// Read access y component
  inline const Type & Y() const { return m_y;}

  /// Write access y component
  inline       Type & Y()       { return m_y;}

  /// Read access y component
  inline const Type & Z() const { return m_z;}

  /// Write access y component
  inline       Type & Z()       { return m_z;}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Comparators

  /// == Operator:  if (Triple == Triple) 
  inline bool operator == (const Triple<Type> & triple) const { return( X() == triple.X() && Y() == triple.Y() && Z() == triple.Z() ); }

  /// != Operator:  if (Triple != Triple) 
  inline bool operator != (const Triple<Type> & triple) const { return( X() != triple.X() || Y() != triple.Y() || Z() != triple.Z() ); } 


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const info

  /// returns the component value of the component with the minimum signed value
  const Type & GetMinComponent() const;

  /// returns the component value of the component with the maximum signed value
  const Type & GetMaxComponent() const;

  /// returns the component value of the component with the maximum absolute value
  const Type & GetMaxAbsComponent() const;

  /// returns the coordinate axis index of the component with the maximum signed value
  const int4 GetMaxComponentCoord() const;

  /// returns the coordinate axis index of the component with the maximum absolute value
  const int4 GetMaxAbsComponentCoord() const;


  /// The Triple scaled anisotropically 
  inline const Triple<Type> GetScaledAnIso(const Triple<float4> & scale3D) const { return Triple<Type>(static_cast<Type>( X()*scale3D.X() ), static_cast<Type>( Y()*scale3D.Y() ), static_cast<Type>( Z()*scale3D.Z() ));}

  /// The Triple divided anisotropically 
  inline const Triple<Type> GetDividedAnIso(const Triple<float4> & divide3D) const { return Triple<Type>(static_cast<Type>( X()/divide3D.X() ), static_cast<Type>( Y()/divide3D.Y() ), static_cast<Type>( Z()/divide3D.Z() ));}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const operators  

  /// Unary + Operator: +Point3D
  inline const Triple<Type>   operator + () const { return Triple<Type>(+X(), +Y(), +Z());}

  /// Unary - Operator: -Point3D
  inline const Triple<Type>   operator - () const { return Triple<Type>(-X(), -Y(), -Z());}


  /// * Operator: Point3D<T> = scalar<T> * Point3D<T>
  inline friend const Triple<Type> operator * (const Type & scalar, const Triple<Type> & triple) { return triple * scalar;}

  /// * Operator: Point3D<T> = Point3D<T> * scalar<T>
  inline const Triple<Type>   operator * (const Type & scalar) const { return Triple<Type>(m_x*scalar, m_y*scalar, m_z*scalar);}

  /// / Operator: Point3D<T> = Point3D<T> / scalar<T>
  inline const Triple<Type>   operator / (const Type & scalar) const { return Triple<Type>(m_x/scalar, m_y/scalar, m_z/scalar);}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const

  /// Clamp the values to a range.
  inline void Clamp(const Type & minimum, const Type & maximum);

  /// Permute the three values, so that points and vectors can be rotated in 3D space
  /// @param "to" is the position that the first element mapping to. 
  /// "to" should be within [-2,2]
  inline void Permute(const int4 to);

  /// Scale this Triple anisotropically 
  inline Triple<Type> & ScaleAnIso(const Triple<float4> & scale3D) { X()*=scale3D.X(); Y()*=scale3D.Y(); Z()*=scale3D.Z(); return *this;}

  /// Divide the Triple anisotropically 
  inline Triple<Type> & DivideAnIso(const Triple<float4> & divide3D) { X()/=divide3D.X(); Y()/=divide3D.Y(); Z()/=divide3D.Z(); return *this;}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const operators

  /// *= Operator: Point3D<T> *= scalar<T>
  inline Triple<Type> & operator *= (const Type & scalar) { m_x*=scalar; m_y*=scalar; m_z*=scalar; return *this;}

  /// /= Operator: Point3D<T> /= scalar<T>
  inline Triple<Type> & operator /= (const Type & scalar) { m_x/=scalar; m_y/=scalar; m_z/=scalar; return *this;}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // status reports

  /// automatic conversion of data members  into a string
  operator std::string() const { return ToString();}

  /// conversion of data members into a string
  std::string ToString() const;

  /// parsing of data members from an istream
  std::istream &  SetFromIStream(std::istream & is);

  /// << Operator
  friend std::ostream & operator << (std::ostream & os, const Triple<Type> & triple) { os << triple.ToString(); return os;}

  /// >> Operator
  friend std::istream & operator >>(std::istream & is, Triple<Type> & triple) { triple.SetFromIStream(is); return is; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Members.
public:

  /// the x component
  Type m_x;

  /// the y component
  Type m_y;

  /// the z component
  Type m_z;

}; // class Triple


#endif // Triple_h


// Revision History:
// $Log: Triple.h,v $
// Revision 1.2  2004/03/04 19:02:47  geconomos
// Removed specializations of casting operator.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.21  2003/12/22 14:14:28  michael
// need to work on this for the 7.1 compiler
//
// Revision 3.20  2003/05/16 11:48:47  michael
// code review
//
// Revision 3.19  2003/05/15 13:39:46  frank
// Added GetMinComponent()
//
// Revision 3.18  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 3.17  2003/01/22 21:04:34  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.16  2001/01/02 19:39:16  ingmar
// better documentation structure
//
// Revision 3.15  2001/01/02 18:52:29  ingmar
// better documentation structure
//
// Revision 3.14  2001/01/02 16:21:41  ingmar
// removed unnecessarry include
//
// Revision 3.13  2002/12/23 23:10:00  ingmar
// AsString() -> ToString()
//
// Revision 3.12  2002/12/23 23:05:05  ingmar
// VC++ 6.0 only likes one copy contructor per src type
//
// Revision 3.11  2002/12/23 22:52:14  ingmar
// towarsds coding standards compliance
//
// Revision 3.10  2002/12/23 13:21:47  frank
// Added a const to Triple
//
// Revision 3.9  2002/04/23 15:04:10  jenny
// created assignment operator and constructor to accept Vector3D.
//
// Revision 3.8  2002/03/11 20:01:14  michael
// compiles again
//
// Revision 3.7  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.6  2002/03/07 22:31:09  dmitry
// Modified to fit new architecture.
//
// Revision 3.5  2002/03/04 20:34:43  michael
// formatting
//
// Revision 3.4  2002/02/22 17:01:54  michael
// Get...MaxComponentCoord() returns now uint1 (index) and not the template type
//
// Revision 3.3  2002/02/15 14:47:32  jaddonisio
// Moved the [] operator to the Triple class.
//
// Revision 3.2  2002/02/11 18:35:57  michael
// added helper functions to the Triple which are used in the texture map based renderers
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:20   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:47:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:20   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.16  2001/07/11 22:26:39  liwei
// Added copy constructor with an object of different template.
//
// Revision 1.15  2001/05/31 21:32:13  ana
// changes regarding code review
//
// Revision 1.14  2001/05/08 18:21:47  ana
// Modifications after code review
//
// Revision 1.13  2001/05/01 23:04:00  ana
// code standards
//
// Revision 1.12  2001/04/30 13:59:32  ana
// changes to fit in code standards
//
// Revision 1.11  2001/04/17 23:34:16  ana
// added heading comments
//
// Revision 1.10  2001/04/16 13:51:50  ana
// added TestByType method
//
// Revision 1.9  2001/04/12 14:37:28  dmitry
// Sec_3_math base class added
//
// Revision 1.8  2001/04/09 18:03:19  dmitry
// Removed Revision History from Top of File
//
// Revision 1.7  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.6  2001/03/07 16:02:21  liwei
// Added some comments
//
// Revision 1.5  2001/01/24 20:16:49  frank
// Renamed permutation function.
//
// Revision 1.4  2001/01/24 20:09:16  liwei
// Forgot to check in these files...
// Implemented Rotate function.
//
// Revision 1.3  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/21 19:53:57  frank
// Added new Clamp function
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 18    8/17/00 1:28p Antonio
// modified header
// 
// *****************  Version 17  *****************
// User: Antonio      Date: 8/10/00    Time: 4:28p
// Updated in $/vx/src/Math
// Modified header to newer version.
// 
// *****************  Version 16  *****************
// User: Antonio      Date: 8/02/00    Time: 9:36p
// Updated in $/vx/src/Math
// added complete history to end of file
// 
// *****************  Version 15  *****************
// User: Antonio         Date:  8/01/00  Time:  4:33p
// Checked in $/vx/src/Math
// Comment:
//   testing history -A
// 
// *****************  Version 14  *****************
// User: Antonio         Date:  8/01/00  Time:  4:22p
// Checked in $/vx/src/Math
// Comment:
//   test 2
// 
// *****************  Version 13  *****************
// User: Antonio         Date:  8/01/00  Time:  4:21p
// Checked in $/vx/src/Math
// Comment:
//   testing history -R
// 
// *****************  Version 12  *****************
// User: Antonio         Date:  8/01/00  Time: 12:34p
// Checked in $/vx/src/Math
// Comment:
//   added copyright
// 
// *****************  Version 11  *****************
// User: Antonio         Date:  7/31/00  Time: 11:16a
// Checked in $/vx/src/Math
// Comment:
//   ChAnges made for testing these utilites
// 
// *****************  Version 10  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point, Normal, Vector
// Since no virtual functions are desired, the same fn. appears in all despite
// inheritance
// 
// *****************  Version 9  *****************
// User: Jay             Date:  7/27/00  Time: 10:58a
// Checked in $/vx/src/Math
// Comment:
//   Indented the Cast function
// 
// *****************  Version 8  *****************
// User: Ingmar          Date:  7/25/00  Time:  4:32p
// Checked in $/v2k/src/Math
// Comment:
//   added Cast function
// 
// *****************  Version 7  *****************
// User: Ingmar          Date:  7/25/00  Time: 10:48a
// Checked in $/vx/src/Math
// Comment:
//   removed virtual
// 
// *****************  Version 6  *****************
// User: Antonio         Date:  7/24/00  Time: 11:52a
// Checked in $/vx/src/Math
// Comment:
//   added history
// 
// *****************  Version 5  *****************
// User: Jay             Date:  7/18/00  Time:  4:56p
// Checked in $/vx/src/Math
// 
// *****************  Version 4  *****************
// User: Frank           Date:  7/18/00  Time:  3:55p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Triple.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Triple.h,v 1.2 2004/03/04 19:02:47 geconomos Exp $
// $Id: Triple.h,v 1.2 2004/03/04 19:02:47 geconomos Exp $
