// $Id: Normal3D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Normal3D_h
#define Normal3D_h

/**
 * Template class to store a geometric 3D Normal (x,y,z)         <BR>
 * Normal3D<Type> for any type                                   <BR>
 * Normal3Df for default floating point values (float4)          <P>
 * $Id: Normal3D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
 */
template <class Type> 
class Normal3D 
{
// Public Member Functions.
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Constructor & Assignment

  /// Default Constructor.
  inline Normal3D<Type>() : m_x(static_cast<Type>(0)),   m_y(static_cast<Type>(0)),   m_z(static_cast<Type>(1)) { }

  /// Constructor from three scalars.
  inline Normal3D<Type>(const Type & x, const Type & y, const Type & z) : m_x(x), m_y(y), m_z(z) { Normalize(); }

  /// Copy Constructor from Triple<Type>.
  explicit Normal3D<Type>(const Triple  <Type> & triple  );

  /// Copy Constructor from Point3D<Type>.
  explicit Normal3D<Type>(const Point3D <Type> & point3D );

  /// Copy Constructor from Vector3D<Type>.
  explicit Normal3D<Type>(const Vector3D<Type> & vector3D);

  /// Copy Constructor from Normal3D<Type>.
  inline   Normal3D<Type>(const Normal3D<Type> & normal3D) : m_x(normal3D.X()), m_y(normal3D.Y()), m_z(normal3D.Z()) { }


  /// Assignment from Normal3D<Type>.
  inline Normal3D<Type> & operator = (const Normal3D<Type> & normal3D) { m_x=normal3D.X(); m_y=normal3D.Y(); m_z=normal3D.Z(); return *this;} 


  /// Casting Operator (same class, other template parameter)
  template <class OtherType>
  inline operator Normal3D<OtherType>() { return Normal3D<OtherType>(static_cast<OtherType>(m_x), static_cast<OtherType>(m_y), static_cast<OtherType>(m_z));}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Access Functions

  /// Sets 'this' to the three args passed.
  inline void SetXYZ(const Type & x, const Type & y, const Type & z) { m_x=x; m_y=y; m_z=z; Normalize(); }

  /// read access any component
  inline const Type & operator[](int4 iIndex) const { return ((Type*)(&m_x))[iIndex];}

  /// Read access x component
  inline const Type & X() const { return m_x;}

  /// Read access y component
  inline const Type & Y() const { return m_y;}

  /// Read access y component
  inline const Type & Z() const { return m_z;}



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Comparators

  /// == Operator:  if (Normal3D == Normal3D) 
  inline bool operator == (const Normal3D<Type> & normal3D) const { return( X() == normal3D.X() && Y() == normal3D.Y() && Z() == normal3D.Z() );}

  /// != Operator:  if (Normal3D != Normal3D) 
  inline bool operator != (const Normal3D<Type> & normal3D) const { return( X() != normal3D.X() || Y() != normal3D.Y() || Z() != normal3D.Z() );}


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


  /// Returns a new Vector3D with each component scaled by the corresponding component of scale3D.
  inline const Vector3D<Type> GetScaledAnIso(const Triple<Type> & scale3D) const;

  /// Returns a new Vector3D with each component divided by the corresponding component of divide3D.
  const Vector3D<Type> GetDividedAnIso(const Triple<Type> & divide3D) const;


  /// The dot product of this Normal3D<Type> with a Vector3D<Type>
  inline const Type Dot(const Vector3D<Type> & vector3D) const;

  /// The dot product of this Normal3D<Type> with a Normal3D<Type>
  inline const Type Dot(const Normal3D<Type> & normal3D) const { return(X() * normal3D.X() + Y() * normal3D.Y() + Z() * normal3D.Z()); }


  /// The cross product of this Normal3D<Type> with a Vector3D<Type>
  const Vector3D<Type> Cross(const Vector3D<Type> & vector3D) const;

  /// The cross product of this Normal3D<Type> with a Normal3D<Type>
  inline const Vector3D<Type> Cross(const Normal3D<Type> & n) const { return Vector3D<Type>(Y()*n.Z() - Z()*n.Y(), Z()*n.X() - X()*n.Z(), X()*n.Y() - Y()*n.X());}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math const operators  

  /// Unary + Operator: +Normal3D
  inline const Normal3D<Type>   operator + () const { return Normal3D<Type>(+X(), +Y(), +Z());}

  /// Unary - Operator: -Normal3D
  inline const Normal3D<Type>   operator - () const { return Normal3D<Type>(-X(), -Y(), -Z());}


  /// * Operator: Vector3D<T> = scalar<T> * Normal3D<T>
  inline friend const Vector3D<Type> operator * (const Type & scalar, const Normal3D<Type> & normal3D) { return normal3D * scalar; }
  
  /// * Operator: Vector3D<T> = Normal3D<T> * scalar<T>
  inline const Vector3D<Type>   operator * (const Type & scalar) const { return Vector3D<Type>(X()*scalar, Y()*scalar, Z()*scalar);}

  /// / Operator: Vector3D<T> = Normal3D<T> / scalar<T>
  inline const Vector3D<Type>   operator / (const Type & scalar) const { return Vector3D<Type>(X()/scalar, Y()/scalar, Z()/scalar);}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Math non-const

  /// Permute the three values, so that points and vectors can be rotated in 3D space
  /// @param "to" is the position that the first element mapping to. 
  /// "to" should be within [-2,2]
  void Permute(const int4 to);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // status reports

  /// automatic conversion of member data into a string
  operator std::string() const { return ToString();}

  /// conversion of data members into a string
  std::string ToString() const;

  /// parsing of data members from an istream
  std::istream &  SetFromIStream(std::istream & is);

  /// << Operator
  inline friend std::ostream & operator << (std::ostream & os, const Normal3D<Type> & normal3D) { os << normal3D.ToString(); return os;}  

  /// >> Operator
  inline friend std::istream & operator >>(std::istream & is, Normal3D<Type> & normal3D) { normal3D.SetFromIStream(is); return is; }

private:

  /// Normalizes the Vector ( X()*X() + Y()*Y() + Z()*Z() == 1).
  /// As a Normal3D is normalized after each public member function complestes, 
  /// Normalize does not have to be public (calling it from client code would be redudnant)
  void Normalize();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Members.
private:

  /// the x component
  Type m_x;

  /// the y component
  Type m_y;

  /// the z component
  Type m_z;

}; // class Normal3D


#endif // Normal3D_h


// $Log: Normal3D.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 11:43:46  michael
// code review
//
// Revision 1.4  2003/01/24 23:31:23  ingmar
// code walkthrough changes
//
// Revision 1.3  2003/01/24 21:46:16  ingmar
// code walkthrough changes
//
// Revision 1.2  2003/01/24 18:17:41  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.1  2003/01/22 20:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Normal3D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Normal3D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
