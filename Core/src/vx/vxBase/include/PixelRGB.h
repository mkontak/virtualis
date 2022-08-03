// $Id: PixelRGB.h,v 1.2 2004/05/19 20:23:57 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Huamin Qu (huamin@viatronix.com)

#ifndef PixelRGB_h
#define PixelRGB_h


// includes
#include <string>


/**
 * Template class to store GB pixel values
 */
template <class T>
class PixelRGB 
{
// Member Functions
public:

  /// Default Constructor.
  PixelRGB<T>();

  /// Constructor from components
  PixelRGB( const T & red, const T & green, const T & blue );

  /// Assignment Operator from PixelRGB
  inline PixelRGB<T> & operator =( const PixelRGB<T> & other );
  
  /// Equality operator
  inline bool operator ==( const PixelRGB<T> & other ) const;
  
  /// Inequality operator
  inline bool operator !=( const PixelRGB<T> & other ) const;
  
  /// output to a string
  std::string ToString() const;

// Access Functions
public:

  /// Set the colors
  inline void SetColor( const T & red, const T & green, const T & blue );

  /// access red component
  inline       T & R()       { return m_red; }

  /// access red component
  inline const T & R() const { return m_red; }

  /// access green component
  inline       T & G()       { return m_green; }

  /// access green component
  inline const T & G() const { return m_green; }

  /// access blue component
  inline       T & B()       { return m_blue; }

  /// access blue component
  inline const T & B() const { return m_blue; }


// Data Members
public:
  
  /// The red component
  T m_red;

  /// The green component
  T m_green;

  /// The blue component
  T m_blue;

}; // Class PixelRGB


/// Overloaded >> Operator for Pixel<T>.
template <class T> 
inline std::istream & operator >>(std::istream & is, PixelRGB<T> & pixel);


/// Overloaded << Operator for Pixel<T>.
template <class T> 
inline std::ostream & operator <<(std::ostream & os, const PixelRGB<T> & pixel); 


#include "PixelRGB.inl"


#endif // PixelRGB_h


// $Log: PixelRGB.h,v $
// Revision 1.2  2004/05/19 20:23:57  frank
// added to string function
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 3.7  2003/05/16 11:44:21  michael
// code review
//
// Revision 3.6  2003/05/15 19:05:51  dongqing
// code review
//
// Revision 3.5  2003/01/17 19:53:09  frank
// footer
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PixelRGB.h,v 1.2 2004/05/19 20:23:57 frank Exp $
// $Id: PixelRGB.h,v 1.2 2004/05/19 20:23:57 frank Exp $
