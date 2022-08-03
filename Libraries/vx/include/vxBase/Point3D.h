// $Id: Point3D.h,v 1.3 2005/11/18 21:12:52 frank Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Point3D_h
#define Point3D_h

/**
 * Template class to store a geometric 3D Point/Location (x,y,z) <BR>
 * Point3D<Type> for any type                                    <BR>
 * Point3Di for default integer values (int4)                    <BR>
 * Point3Df for default floating point values (float4)           <P>
 * $Id: Point3D.h,v 1.3 2005/11/18 21:12:52 frank Exp $
 */
template <class Type> 
class Point3D 
{
// Public Member Functions.
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Constructor & Assignment

  /// Default Constructor.
  inline Point3D<Type>() : m_x(static_cast<Type>(0)), m_y(static_cast<Type>(0)), m_z(static_cast<Type>(0)) {}

  /// Constructor from three scalars.
  inline Point3D<Type>(const Type & x, const Type & y, const Type & z) : m_x(x), m_y(y), m_z(z) {}

  /// Copy Constructor from Triple<Type>.
  explicit Point3D<Type>(const Triple  <Type> & triple  );

  /// Copy Constructor from Point3D<Type>.
  inline   Point3D<Type>(const Point3D <Type> & point3D ) : m_x(point3D.X()), m_y(point3D.Y()), m_z(point3D.Z()) {}

  /// Copy Constructor from Vector3D<Type>.
  explicit Point3D<Type>(const Vector3D<Type> & vector3D);

  /// Copy Constructor from Vector3D<Type>.
  explicit Point3D<Type>(const Vector4d<Type> & vector4D);

  /// Assignment from Point3D<Type>.
  inline Point3D<Type> & operator = (const Point3D<Type > & point3D) { m_x=point3D.X(); m_y=point3D.Y(); m_z=point3D.Z(); return *this;} 

  /// Casting Operator (same class, other template parameter)
  template <class OtherType>
  inline operator Point3D<OtherType>() const { return Point3D<OtherType>(static_cast<OtherType>(m_x), static_cast<OtherType>(m_y), static_cast<OtherType>(m_z));}

  /// Casting Operator (same class, uint2 template parameter)
  inline operator Point3D<uint2 >() const { return Point3D<uint2 >(static_cast<uint2 >(m_x), static_cast<uint2 >(m_y), static_cast<uint2 >(m_z));}

  /// Casting Operator (same class, uint4 template parameter)
  inline operator Point3D<uint4 >() const { return Point3D<uint4 >(static_cast<uint4 >(m_x), static_cast<uint4 >(m_y), static_cast<uint4 >(m_z));}

  /// Casting Operator (same class, int4 template parameter)
  inline operator Point3D< int4 >() const { return Point3D< int4 >(static_cast< int4 >(m_x), static_cast< int4 >(m_y), static_cast< int4 >(m_z));}

  /// Casting Operator (same class, float4 template parameter)
  inline operator Point3D<float4>() const { return Point3D<float4>(static_cast<float4>(m_x), static_cast<float4>(m_y), static_cast<float4>(m_z));}


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

  /// == Operator:  if (Point3D == Point3D) 
  inline bool operator == (const Point3D<Type> & point3D) const { return( X() == point3D.X() && Y() == point3D.Y() && Z() == point3D.Z() );}

  /// != Operator:  if (Point3D != Point3D) 
  inline bool operator != (const Point3D<Type> & point3D) const { return( X() != point3D.X() || Y() != point3D.Y() || Z() != point3D.Z() );}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const info

  /// returns the component value of the component with the maximum signed value
  const Type & GetMaxComponent() const;

  /// returns the component value of the component with the maximum absolute value
  const Type & GetMaxAbsComponent() const;

  /// returns the coordinate axis index of the component with the maximum signed value
  const int4 GetMaxComponentCoord() const;

  /// returns the coordinate axis index of the component with the maximum absolute value
  const int4 GetMaxAbsComponentCoord() const;


  /// The Point3D scaled anisotropically 
  const Point3D<Type> GetScaledAnIso(const Triple<float4> & scale3D) const;

  /// The Point3D divided anisotropically 
  const Point3D<Type> GetDividedAnIso(const Triple<float4> & divide3D) const;


  /// The Euclidean Distance to another Point3D as a float8
  inline const float8 GetEuclideanDistance(const Point3D<Type> & point3D) const;


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const operators  

  /// * Operator: Point3D<T> = scalar<T> * Point3D<T>
  inline friend const Point3D<Type> operator * (const Type & scalar, const Point3D<Type> & point3D) { return point3D * scalar;}

  /// * Operator: Point3D<T> = Point3D<T> * scalar<T>
  inline const Point3D<Type>   operator * (const Type & scalar) const { return Point3D<Type>(m_x*scalar, m_y*scalar, m_z*scalar);}

  /// / Operator: Point3D<T> = Point3D<T> / scalar<T>
  inline const Point3D<Type>   operator / (const Type & scalar) const { return Point3D<Type>(m_x/scalar, m_y/scalar, m_z/scalar);}


  /// - Operator: Vector3D<T> = Point3D<T> - Point3D<T>
  inline const Vector3D<Type> operator - (const Point3D<Type> & point3D) const { return Vector3D<Type>(m_x-point3D.X(), m_y-point3D.Y(), m_z-point3D.Z());}

  /// + Operator: Point3D<T> = Point3D<T> + Vector3D<T>
  inline const Point3D<Type> operator + (const Vector3D<Type> & vector3D) const { return Point3D<Type>(m_x+vector3D.X(), m_y+vector3D.Y(), m_z+vector3D.Z());}

  /// - Operator: Point3D<T> = Point3D<T> - Vector3D<T>.
  inline const Point3D<Type> operator - (const Vector3D<Type> & vector3D) const { return Point3D<Type>(m_x-vector3D.X(), m_y-vector3D.Y(), m_z-vector3D.Z());}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const

  /// Clamp the values to a range.
  inline void Clamp(const Type & minimum, const Type & maximum);

  /// Permute the three values, so that points and vectors can be rotated in 3D space
  /// @param "to" is the position that the first element mapping to. 
  /// "to" should be within [-2,2]
  inline void Permute(const int4 to);

  /// Scale this Point3D anisotropically 
  Point3D<Type> & ScaleAnIso(const Triple<float4> & scale3D);

  /// Divide the Point3D anisotropically 
  Point3D<Type> & DivideAnIso(const Triple<float4> & divide3D);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const operators

  /// *= Operator: Point3D<T> *= scalar<T>
  inline Point3D<Type> & operator *= (const Type & scalar) { m_x*=scalar; m_y*=scalar; m_z*=scalar; return *this;}

  /// /= Operator: Point3D<T> /= scalar<T>
  inline Point3D<Type> & operator /= (const Type & scalar) { m_x/=scalar; m_y/=scalar; m_z/=scalar; return *this;}


  /// Binary += Operator: point3D<T> += vector3D<T>.
  inline Point3D<Type> & operator += (const Vector3D<Type> & vector3D) { X()+=vector3D.X(); Y()+=vector3D.Y(); Z()+=vector3D.Z(); return *this;}

  /// Binary -= Operator: point3D<T> -= vector3D<T>.
  inline Point3D<Type> & operator -= (const Vector3D<Type> & vector3D)  { X()-=vector3D.X(); Y()-=vector3D.Y(); Z()-=vector3D.Z(); return *this;}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // status reports

  /// automatic conversion of data members  into a string
  operator std::string() const { return ToString();}

  /// conversion of data members into a string
  std::string ToString() const;

  /// parsing of data members from an istream
  std::istream &  SetFromIStream(std::istream & is);

  /// << Operator
  inline friend std::ostream & operator << (std::ostream & os, const Point3D<Type> & point3D) { os << point3D.ToString(); return os;}

  /// >> Operator
  inline friend std::istream & operator >>(std::istream & is, Point3D<Type> & point3D) { point3D.SetFromIStream(is); return is;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Members.
public:

  /// the x component
  Type m_x;

  /// the y component
  Type m_y;

  /// the z component
  Type m_z;
}; // class Point3D.


#endif // Point3D_h


// $Log: Point3D.h,v $
// Revision 1.3  2005/11/18 21:12:52  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/08/01 19:11:13  frank
// changed function name according to coding standards
//
// Revision 1.2  2004/03/04 19:03:04  geconomos
// Removed specializations of casting operator.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/16 11:43:47  michael
// code review
//
// Revision 1.5  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.4  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 1.3  2003/01/24 18:18:16  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.2  2003/01/22 21:02:40  ingmar
// initial version completed
//
// Revision 1.1  2002/03/14 15:39:47  ingmar
// intial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Point3D.h,v 1.3 2005/11/18 21:12:52 frank Exp $
// $Id: Point3D.h,v 1.3 2005/11/18 21:12:52 frank Exp $
