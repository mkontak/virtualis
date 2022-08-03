// $Id: PixelRGB.inl,v 1.3 2006/01/18 20:35:35 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PixelRGB.inl,v 1.3 2006/01/18 20:35:35 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * Constructor with default arguments
 */
template <class T> 
PixelRGB<T>::PixelRGB()
: m_red( T() ),
m_green( T() ),
m_blue( T() )
{
} // PixelRGB()


/**
 * Constructor from 3 scalars
 *
 * @param red   the red component
 * @param green the green component
 * @param blue  the blue component
*/
template <class T> 
PixelRGB<T>::PixelRGB( const T & red, const T & green, const T & blue )
: m_red( red ),
m_green( green ),
m_blue( blue )
{
} // PixelRGB(red,green,blue,alpha)


/**
 * Assignment Operator from PixelRGB
 *
 * @param other the other pixel
 * @return itself
 */
template <class T> 
inline PixelRGB<T> & PixelRGB<T>::operator =(const PixelRGB<T> & other)
{
  m_red   = other.m_red;
  m_green = other.m_green;
  m_blue  = other.m_blue;

  return * this;
} // operator =()


/**
 * Set the red, green, blue and alpha components.
 *
 * @param red   the red component
 * @param green the green component
 * @param blue  the blue component
 * @param alpha the alpha component
 */
template <class T> 
inline void PixelRGB<T>::SetColor( const T & red, const T & green, const T & blue )
{
  m_red   = red;
  m_green = green;
  m_blue  = blue;
} // SetColor()


/**
 * Equality operator
 *
 * @param other the other pixel
 */
template <class T>
inline bool PixelRGB<T>::operator ==(const PixelRGB<T> & other) const
{
  return 
    ( m_red   == other.m_red   ) &&
    ( m_green == other.m_green ) &&
    ( m_blue  == other.m_blue  );
} // operator ==()


/**
 * Inequality operator
 *
 * @param other the other pixel
 */
template <class T>
inline bool PixelRGB<T>::operator !=( const PixelRGB<T> & other ) const
{
  return ! ( * this == other );
} // operator !=()


/**
 * output to a string
 *
 * @return data as string
 */
template <class T>
inline std::string PixelRGB<T>::ToString() const
{
  return "(" + ToAscii( m_red ) + "," + ToAscii( m_green ) + "," + ToAscii( m_blue ) + ")";
} // ToString()


/**
 * Overloaded >> operator
 * Accepts from istream 3 values for the Pixel<T> object
 *
 * @param is    input stream
 * @param pixel output pixel
 * @return      the input stream
 */ 
template <class T> 
inline std::istream & operator>>(std::istream & is, PixelRGB<T> & pixel)
{
  is >> pixel.m_red >> pixel.m_green >> pixel.m_blue;
  return is;
} // operator >>( istream &is, Pixel<T> & pixel )


/**
 * Overloaded << operator
 * Directs the three data members to the ostream, reference of which is passed
 *
 * @param os    the output stream
 * @param pixel the input pixel
 * @return      the output stream
 */  
template <class T> 
inline std::ostream & operator <<( std::ostream & os, const PixelRGB<T> & pixel )
{
  os << "Pixel(" ;
  os << pixel.m_red << "," ;
  os << pixel.m_green << "," ;
  os << pixel.m_blue << "," ;
  os << ")"; 
  return os;
} // end of operator<<( ostream &os, const PixelRGB<T> & pixel )


// $Log: PixelRGB.inl,v $
// Revision 1.3  2006/01/18 20:35:35  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2004/05/19 20:23:57  frank
// added to string function
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 3.3  2003/05/16 13:14:51  frank
// formatting
//
// Revision 3.2  2003/05/16 11:44:21  michael
// code review
//
// Revision 3.1  2003/05/15 19:05:51  dongqing
// code review
//
//
// $Id: PixelRGB.inl,v 1.3 2006/01/18 20:35:35 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PixelRGB.inl,v 1.3 2006/01/18 20:35:35 geconomos Exp $
