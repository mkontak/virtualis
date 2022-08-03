// $Id: Vector4d.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com

#ifndef Vector4d_h
#define Vector4d_h


// includes 
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"

                  
// forward declarations
template <class Type> class Vector4d;


// typedefs
typedef Vector4d<int4>   Vector4Di; 
typedef Vector4d<float4> Vector4Df; 
                  
                  
/**
 * Speed optimized concrete 3D vector of float4.
 * Access members through X() Y() and Z().
 */
template < class Type >
class Vector4d 
{
  // public member functions
public:

  /// Default Contructor.
  inline Vector4d(const Type & x=0, const Type & y=0, const Type & z=0, const Type & t=0);

  /// Copy Contructor.
  inline Vector4d(const Vector4d & src);
  
  /// Constructor that takes a Triple
  inline Vector4d(const Triple<Type> & src, Type t);

  /// Constructor that takes a Vector3D
  inline Vector4d(const Point3D<Type> & src, Type t);

  /// Constructor that takes a Vector3D
  inline Vector4d(const Vector3D<Type> & src, Type t);

  /// assignment operator.
  inline Vector4d<Type> & operator= (const Vector4d & otherVector);

  /// Read access for any component.
  inline const Type & operator[](int4 iIndex) const;

  /// Overloaded Vector<Type>*scalar Operator.
  inline Vector4d<Type> operator *(const Type & scalar) const;

  /// Overloaded Vector<Type>/scalar Operator.
  inline Vector4d<Type> operator /(const Type & scalar) const;

  /// Overloaded Vector<Type>*=scalar Operator.
  inline Vector4d<Type> & operator *=(const Type & scalar);

  /// Overloaded Vector<Type>/=scalar Operator.
  inline Vector4d<Type> & operator /=(const Type & scalar);

  /// Write access for any component.
//  inline Type & At(int4 iIndex);

  /// Read access for x component.
  inline const Type & X() const;

  /// Read access for y component.
  inline const Type & Y() const;

  /// Read access for z component.
  inline const Type & Z() const;

  /// Read access for z component.
  inline const Type & T() const;

  /// Write access for any component.
  inline Type & operator[](int4 iIndex);

  /// Write access for x component.
  inline Type & X();

  /// Write access for y component.
  inline Type & Y();

  /// Write access for z component.
  inline Type & Z();

  /// Write access for z component.
  inline Type & T();

  /// Read access to maximum value component.
  inline Type GetMaxComponentXYZ() const;

  /// returns new normalized Vector4d.
  inline Vector4d<Type> GetNormalized() const;

  /// returns new normalized Vector4d, but the normalization scale factor ignored the T component, however T will be normalized as well
  inline Vector4d<Type> GetNormalizedXYZ() const;

  /// Normalizes the Vector<T>.
  inline void Normalize();

  /// Overloaded Binary + Operator: Vector<T> = Vector<T> + Vector<T>.
  inline Vector4d<Type> operator +(const Vector4d<Type> & other) const;

  /// Overloaded Binary - Operator: Vector4d<T> = Vector4d<T> - Vector4d<T>.
  inline Vector4d<Type> operator -(const Vector4d<Type> & other) const;

  /// Returns the length of the Vector<T> as a double.
  inline float4 GetLength() const;

  /// Returns the length of the Vector<T> as a double.
  inline float4 GetLengthXYZ() const;

  inline Type GetSquaredLength() const;

  /// Returns the Dot Product with another Vector<T>.
  inline Type Dot(const Vector4d<Type> & other) const;

  /// Returns a new Vector4d with each component scaled by the corresponding component of vScale.
  inline const Vector4d<Type> GetScaledUnIso(const Vector4d<Type> & vScale) const;

  /// Returns a new Vector4d with each component divided by the corresponding component of vDivide.
  inline const Vector4d<Type> GetDividedUnIso(const Vector4d<Type> & vDivide) const;

  /// Overloaded Unary + Operator.
  inline Vector4d<Type> operator +() const;

  /// Overloaded Unary - Operator.
  inline Vector4d<Type> operator -() const;

  /// Overloaded Unary += Operator.
  inline Vector4d<Type> & operator +=(const Vector4d<Type> & other);

  /// Overloaded Unary -= Operator.
  inline Vector4d<Type> & operator -=(const Vector4d<Type> & other);

  /// Overloaded equal - Operator.
  inline bool operator == (const Vector4d<Type> & otherVector) const;

  /// Overloaded equal - Operator.
  inline bool operator != (const Vector4d<Type> & otherVector) const;

  /// automatic conversion of data members  into a string
  operator std::string() const { return ToString();}

  /// conversion of data members into a string
  std::string ToString() const;

  /// Overloaded << Operator.
  inline friend std::ostream & operator<<(std::ostream & os, const Vector4d<Type> & vector4d)
  {
	  os << vector4d.ToString();
	  return( os );
  } // operator<<
  

  /// Overloaded >> Operator.
  inline friend std::istream & operator>>(std::istream & is, Vector4d<Type> & vector4d)
  {
    is >> vector4d.X() >> vector4d.Y() >> vector4d.Z() >> vector4d.T();
    return(is);
  } // end of operator>>(istream &is, Vector4d<Type> & vector4d)
  

// private data members
public:

  /// the elements/components of the vector
  union
  {
    /// all four elements/components as array
    Type m_xyzt[4];
    struct 
    {
      /// the x component
      Type m_x;
      
      /// the y component
      Type m_y;
      
      /// the z component
      Type m_z;
      
      /// the t component
      Type m_t;
    };
  };

}; // class Vector4d


#include "Vector4d.inl"


#endif // Vector4d_h


// $Log: Vector4d.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.7  2003/01/22 21:07:57  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.6  2001/01/03 16:00:42  ingmar
// now with union of array and m_x/y/z/t
//
// Revision 1.5  2001/01/02 19:39:29  ingmar
// better documentation structure
//
// Revision 1.4  2001/01/02 18:53:06  ingmar
// better documentation structure
//
// Revision 1.3  2002/12/24 17:36:07  frank
// Cleaned up picking in 3D
//
// Revision 1.2  2002/11/26 16:04:43  ingmar
// added operator += and -=
//
// Revision 1.1  2002/11/21 22:24:12  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Vector4d.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Vector4d.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
