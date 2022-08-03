// $Id: Vector3D.h,v 1.2 2004/03/04 19:02:39 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Vector3D_h
#define Vector3D_h


/**
 * Template class to store a geometric 3D Vector (x,y,z)         <BR>
 * Vector3D<Type> for any type                                   <BR>
 * Vector3Di for default integer values (int4)                   <BR>
 * Vector3Df for default floating point values (float4)          <P>
 * $Id: Vector3D.h,v 1.2 2004/03/04 19:02:39 geconomos Exp $
 */
template <class Type>
class Vector3D
{
// Public Member Functions.
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Constructor & Assignment

  /// Default Constructor.
  inline Vector3D<Type>() : m_x(static_cast<Type>(0)), m_y(static_cast<Type>(0)), m_z(static_cast<Type>(0)) {}

  /// Constructor from three scalars.
  inline Vector3D(const Type & x, const Type & y, const Type & z) : m_x(x), m_y(y), m_z(z) {}


  /// Copy Constructor from Triple<Type>.
  explicit Vector3D<Type>(const Triple  <Type> & triple  );

  /// Copy Constructor from Point3D<Type>.
  explicit Vector3D<Type>(const Point3D <Type> & point3D );

  /// Copy Constructor from Vector3D<Type>.
  inline   Vector3D<Type>(const Vector3D<Type> & vector3D) : m_x(vector3D.X()), m_y(vector3D.Y()), m_z(vector3D.Z()) {}

  /// Copy Constructor from Normal3D<Type>.
  explicit Vector3D<Type>(const Normal3D<Type> & normal3D);


  /// Assignment from Vector3D<Type>.
  inline Vector3D<Type> & operator = (const Vector3D<Type> & vector3D) { m_x=vector3D.X(); m_y=vector3D.Y(); m_z=vector3D.Z(); return *this;} 


  /// Casting Operator (same class, other template parameter)
  template <class OtherType>
  inline operator Vector3D<OtherType>() const { return Vector3D<OtherType>(static_cast<OtherType>(m_x), static_cast<OtherType>(m_y), static_cast<OtherType>(m_z));}

  /// Casting Operator (same class, uint2 template parameter)
  inline operator Vector3D<uint2 >() const { return Vector3D<uint2 >(static_cast<uint2 >(m_x), static_cast<uint2 >(m_y), static_cast<uint2 >(m_z));}

  /// Casting Operator (same class, uint4 template parameter)
  inline operator Vector3D<uint4 >() const { return Vector3D<uint4 >(static_cast<uint4 >(m_x), static_cast<uint4 >(m_y), static_cast<uint4 >(m_z));}

  /// Casting Operator (same class, int4 template parameter)
  inline operator Vector3D< int4 >() const { return Vector3D< int4 >(static_cast< int4 >(m_x), static_cast< int4 >(m_y), static_cast< int4 >(m_z));}

  /// Casting Operator (same class, float4 template parameter)
  inline operator Vector3D<float4>() const { return Vector3D<float4>(static_cast<float4>(m_x), static_cast<float4>(m_y), static_cast<float4>(m_z));}

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

  /// == Operator:  if (Vector3D == Vector3D) 
  inline bool operator == (const Vector3D<Type> & vector3D) const { return( X() == vector3D.X() && Y() == vector3D.Y() && Z() == vector3D.Z() );}

  /// != Operator:  if (Vector3D != Vector3D) 
  inline bool operator != (const Vector3D<Type> & vector3D) const { return( X() != vector3D.X() || Y() != vector3D.Y() || Z() != vector3D.Z() );}


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


  /// The length as a double based on Eucledian distance.
  inline const float8 GetLength() const;

  /// The length^2 based on Eucledian distance.
  inline const Type GetLengthSquared() const { return((X() * X() + Y() * Y() + Z() * Z()));}

  /// The length based on "city block" distance.
  inline const Type GetLengthCityBlock() const { return( Abs(X()) + Abs(Y()) + Abs(Z()) );}

  /// The length based on "chess board" distance.
  inline const Type GetLengthChessBoard() const { return( Max( Abs(X()), Abs(Y()), Abs(Z()) ) );}

  /// returns new normalized Vector3D.
  inline const Vector3D<Type> GetNormalized() const;

  /// Returns a new Vector3D with each component scaled by the corresponding component of scale3D.
  const Vector3D<Type> GetScaledAnIso(const Triplef & scale3D) const;

  /// The Vector3D divided anisotropically 
  const Vector3D<Type> GetDividedAnIso(const Triplef & divide3D) const;


  /// The Dot Product of this Vector3D<Type> with a Vector3D<Type>.
  inline const Type Dot(const Vector3D<Type> & vector3D) const { return(X() * vector3D.X() + Y() * vector3D.Y() + Z() * vector3D.Z());}

  /// The dot product of this Vector3D<Type> with a Normal3D<Type>.
  inline const Type Dot(const Normal3D<Type> & normal3D) const;


  /// The Cross product of this Vector3D<Type> with a Vector3D<Type>.
  inline const Vector3D<Type> Cross(const Vector3D<Type> & v) const { return(Vector3D<Type>(Y()*v.Z() - Z()*v.Y(), Z()*v.X() - X()*v.Z(), X()*v.Y() - Y()*v.X()));}

  /// The Cross Product of this Vector3D<Type> with a Normal3D<Type>.
  inline const Vector3D<Type> Cross(const Normal3D<Type> & normal3D) const;


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const operators  

  /// Unary + Operator:  +Vector3D
  inline const Vector3D<Type> operator + () const { return Vector3D<Type>(+X(), +Y(), +Z());}

  /// Unary - Operator:  -Vector3D
  #pragma warning( disable : 4146 )
  inline const Vector3D<Type> operator - () const { return Vector3D<Type>(-X(), -Y(), -Z());}
  #pragma warning( default : 4146 )


  /// * Operator: Vector3D<T> = scalar<T> * Vector3D<T>
  inline friend const Vector3D<Type> operator * (const Type & scalar, const Vector3D<Type> & vector3D) { return vector3D * scalar;}

  /// * Operator: Vector3D<T> = Vector3D<T> * scalar<T>
  inline const Vector3D<Type>   operator * (const Type & scalar) const { return Vector3D<Type>(X()*scalar, Y()*scalar, Z()*scalar);}

  /// / Operator: Vector3D<T> = Vector3D<T> / scalar<T>
  inline const Vector3D<Type>   operator / (const Type & scalar) const { return Vector3D<Type>(X()/scalar, Y()/scalar, Z()/scalar);}


  /// + Operator: Vector3D<T> = Vector3D<T> + Vector3D<T>
  inline const Vector3D<Type> operator + (const Vector3D<Type> & v) const { return Vector3D<Type>(X()+v.X(), Y()+v.Y(), Z()+v.Z());}

  /// - Operator: Vector3D<T> = Vector3D<T> - Vector3D<T>.
  inline const Vector3D<Type> operator - (const Vector3D<Type> & v) const { return Vector3D<Type>(X()-v.X(), Y()-v.Y(), Z()-v.Z());}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const

  /// Clamp the values to a range.
  inline void Clamp(const Type & minimum, const Type & maximum);

  /// Permute the three values, so that points and vectors can be rotated in 3D space
  /// @param "to" is the position that the first element mapping to. 
  /// "to" should be within [-2,2]
  inline void Permute(const int4 to);

  /// Normalize this.
  inline void Normalize();

  /// Scales the Vector3D with a scalar,
  //inline Vector3D<Type> & Scale(const Type & scalar) { m_x*=scalar; m_y*=scalar; m_z*=scalar; return *this;}

  /// Scale this Vector3D anisotropically 
  Vector3D<Type> & ScaleAnIso(const Triplef & triple);

  /// The Point3D divided anisotropically 
  Vector3D<Type> & DivideAnIso(const Triplef & divide3D);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const operators

  /// *= Operator: Vector3D<T> *= scalar<T>
  inline Vector3D<Type> & operator *= (const Type & scalar) { m_x*=scalar; m_y*=scalar; m_z*=scalar; return *this;}

  /// /= Operator: Vector3D<T> /= scalar<T>
  inline Vector3D<Type> & operator /= (const Type & scalar) { m_x/=scalar; m_y/=scalar; m_z/=scalar; return *this;}


  /// += Operator: Vector3D<T> += Vector3D<T>.
  inline Vector3D<Type> & operator += (const Vector3D<Type> & vector3D) { m_x+=vector3D.X(); m_y+=vector3D.Y(); m_z+=vector3D.Z(); return *this;}

  /// -= Operator: Vector3D<T> -= Vector3D<T>.
  inline Vector3D<Type> & operator -= (const Vector3D<Type> & vector3D) { m_x-=vector3D.X(); m_y-=vector3D.Y(); m_z-=vector3D.Z(); return *this;}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // status reports

  /// automatic conversion of data members  into a string
  operator std::string() const { return ToString();}

  /// conversion of data members into a string
  std::string ToString() const;

  /// parsing of data members from an istream
  std::istream &  SetFromIStream(std::istream & is);

  /// << Operator
  friend std::ostream & operator << (std::ostream & os, const Vector3D<Type> & vector3D) { os << vector3D.ToString(); return os;}

  /// >> Operator
  friend std::istream & operator >>(std::istream & is, Vector3D<Type> & vector3D) { vector3D.SetFromIStream(is); return is;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Members.
public:

  /// the x component
  Type m_x;

  /// the y component
  Type m_y;

  /// the z component
  Type m_z;

}; // class Vector3D


#endif // Vector3D_h


// $Log: Vector3D.h,v $
// Revision 1.2  2004/03/04 19:02:39  geconomos
// Removed specializations of casting operator.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.24  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.23  2003/01/24 21:46:17  ingmar
// code walkthrough changes
//
// Revision 1.22  2003/01/24 18:18:27  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.21  2003/01/22 21:06:58  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes,
// there is now only one vector class (this calss Vector3D) that has the best of both prior existing classes Vector and Vector3D
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Vector3D.h,v 1.2 2004/03/04 19:02:39 geconomos Exp $
// $Id: Vector3D.h,v 1.2 2004/03/04 19:02:39 geconomos Exp $
