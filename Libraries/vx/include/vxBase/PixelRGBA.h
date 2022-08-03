// $Id: PixelRGBA.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Ingmar (ingmar@viatronix.com)

#ifndef PixelRGBA_h
#define PixelRGBA_h



/**
 * Template class to store RGBA pixel values
 * PixelRGBA<type> for any type
 * PixelRGBAu for default integer pixels (uint1)
 * PixelRGBAf for default floating point pixels (float4)
 */
template <class T> 
class VX_BASE_DLL PixelRGBA 
{
// Member Functions
public:
  ///////////////////////////////////////////////////////////////////////
  // constructor & assignment
  
  /// Default Constructor.
  inline PixelRGBA<T>();

  /// four scalar constructor
  inline PixelRGBA( const T & red, const T & green, const T & blue, const T & alpha );

  /// copy constructor
  template <class C> inline explicit PixelRGBA( const PixelRGBA<C> & other )
  {
    R() = T( other.R() );
    G() = T( other.G() );
    B() = T( other.B() );
    A() = T( other.A() );
  } // copy constructor

  /// Assignment Operator  from Pixel. 
  inline PixelRGBA<T> & operator = (const PixelRGBA<T> & other);


  ///////////////////////////////////////////////////////////////////////
  // comparators

  /// Equality operator.
  inline bool operator ==(const PixelRGBA<T> & other) const;

  /// Inequality operator.
  inline bool operator !=(const PixelRGBA<T> & other) const;


  ///////////////////////////////////////////////////////////////////////
  // Access Functions

  /// Set the red, green, blue and alpha components
  inline void SetRGBA( const T & red, const T & green, const T & blue, const T & alpha );

  
  /// access any component
  inline       T & operator[]( int4 iIndex )       { return m_rgba[iIndex]; }

  /// access any component
  inline const T & operator[]( int4 iIndex ) const { return m_rgba[iIndex]; }


  /// access red component
  inline       T & R()       { return m_rgba[0]; }

  /// access red component
  inline const T & R() const { return m_rgba[0]; }

  
  /// access green component
  inline       T & G()       { return m_rgba[1]; }

  /// access green component
  inline const T & G() const { return m_rgba[1]; }


  /// access blue component
  inline       T & B()       { return m_rgba[2]; }

  /// access blue component
  inline const T & B() const { return m_rgba[2]; }


  /// access alpha component
  inline       T & A()       { return m_rgba[3]; }

  /// access alpha component
  inline const T & A() const { return m_rgba[3]; }


  /// Get a pixel color from a color palette that has easy to differentiate colors as neighbors
  static PixelRGBA<T> GetColorPalettePixel( uint1 iIndex );


  ///////////////////////////////////////////////////////////////////////
  // pixel math

  /// add a scalar to each of RGB, but not A
  inline void AddRGB( const T & additionalGray );
  
  /// multiply each of RGB, but not A, with the scaleFactor
  inline void ScaleRGB( const float4 & scaleFactor );

  /// blend the specified fraction of color onto RGB, but not A
  inline void BlendRGB( const PixelRGBA<T> & color, const float4 & fBlendFraction );

  /// Composite this pixel with another pixel. This pixel is in front
  inline void Composite( const PixelRGBA<T> & backPixel );
    
  ///////////////////////////////////////////////////////////////////////
  // status reports

  /// conversion of pixel data into string (now PixelRGBA can be used in an ostream)
  operator std::string() const { return ToString(); }

  /// conversion of pixel data into string
  std::string ToString() const;

// Data Members
private:

  /// the four color components
  T m_rgba[4];

}; // class PixelRGBA


// explicit type names for most frequently used types
typedef PixelRGBA< uint1> PixelRGBAu;
typedef PixelRGBA<float4> PixelRGBAf;


#include "PixelRGBA.inl"


#endif // PixelRGBA_h


// $Log: PixelRGBA.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.12  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 3.11  2003/05/16 13:14:51  frank
// formatting
//
// Revision 3.10  2003/01/02 19:38:33  ingmar
// better documentation structure
//
// Revision 3.9  2002/12/21 00:36:29  ingmar
// improved comment formatting
//
// Revision 3.8  2002/12/20 19:00:27  ingmar
// finished code review
//
// Revision 3.7  2002/12/20 17:04:50  ingmar
// towards coding standards compliance
//
// $Id: PixelRGBA.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PixelRGBA.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
