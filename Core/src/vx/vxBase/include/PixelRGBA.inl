// $Id: PixelRGBA.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Ingmar (ingmar@viatronix.com)


/**
 * Default Constructor. All color component set to zero
 */
template <class T> 
PixelRGBA<T>::PixelRGBA() 
{
  m_rgba[0] = T();
  m_rgba[1] = T();
  m_rgba[2] = T();
  m_rgba[3] = T();
} // Default constructor()


/**
 * Constructor from four scalars.
 *
 * @param red   the red component
 * @param green the green component
 * @param blue  the blue component
 * @param alpha the alpha component
 */
template <class T> 
PixelRGBA<T>::PixelRGBA( const T & red, const T & green, const T & blue, const T & alpha )
{
  m_rgba[0] = red;
  m_rgba[1] = green;
  m_rgba[2] = blue;
  m_rgba[3] = alpha;
} // PixelRGBA(red,green,blue,alpha)

/**
 * Assignment Operator from PixelRGBA
 *
 * @param other the other pixel
 */
template <class T> 
PixelRGBA<T> & PixelRGBA<T>::operator =( const PixelRGBA<T> & other )
{
  R() = other.R();
  G() = other.G();
  B() = other.B();
  A() = other.A();
  return * this;
} // operator =


///////////////////////////////////////////////////////////////////////
// comparators


/**
 * Equality operator
 *
 * @param other the other pixel
 */
template <class T>
inline bool PixelRGBA<T>::operator ==( const PixelRGBA<T> & other ) const
{
  return( ( m_rgba[0] == other.m_rgba[0] ) && 
          ( m_rgba[1] == other.m_rgba[1] ) &&
          ( m_rgba[2] == other.m_rgba[2] ) && 
          ( m_rgba[3] == other.m_rgba[3] ) );
} // operator ==()


/**
 * Inequality operator
 *
 * @param other the other pixel
 */
template <class T>
inline bool PixelRGBA<T>::operator !=( const PixelRGBA<T> & other ) const
{
  return ! ( * this == other );
} // operator !=()


/**
 * Set the red, green, blue and alpha components
 *
 * @param red   the red component
 * @param green the green component
 * @param blue  the blue component
 * @param alpha the alpha component
 */
template <class T> 
inline void PixelRGBA<T>::SetRGBA( const T & red, const T & green, const T & blue, const T & alpha )
{
  m_rgba[0] = red;
  m_rgba[1] = green;
  m_rgba[2] = blue;
  m_rgba[3] = alpha;
} // SetRGBA()


///////////////////////////////////////////////////////////////////////
// pixel math

/**
 * add a scalar to each of RGB, but not A
 *
 * @param additionalGray the amount you want added to each RGB component
 */
template <class T> 
inline void PixelRGBA<T>::AddRGB( const T & additionalGray )
{
  R() += additionalGray;
  G() += additionalGray;
  B() += additionalGray;
} // AddRGB()


/**
 * multiply each of RGB, but not A, with the scaleFactor
 *
 * @param scaleFactor the scale factor by which to mulitply each RGB component
 */
template <class T> 
inline void PixelRGBA<T>::ScaleRGB( const float4 & scaleFactor )
{
  R() *= scaleFactor;
  G() *= scaleFactor;
  B() *= scaleFactor;
} // ScaleRGB()


/**
 * blend the specified fraction of color onto RGB, but not A
 * multiplies blend color with this->A()
 * 
 * @param color  alpha pre-multiplied blending color
 * @param fBlendFraction
 *               range [0,1], proportion of second color
 */
template <class T> 
inline void PixelRGBA<T>::BlendRGB(const PixelRGBA<T> & blendColor, const float4 & fBlendFraction)
{
  R() += ( ( blendColor.R()*A() - R() ) * fBlendFraction );
  G() += ( ( blendColor.G()*A() - G() ) * fBlendFraction );
  B() += ( ( blendColor.B()*A() - B() ) * fBlendFraction );
} // BlendRGB()


/**
 * Make a compositing between two pixels. 
 * This pixel is in front.
 * Assumes alpha pre-multiplied colors.
 * This probably only works for T == float
 *
 * @param backPixel the pixel in back of this one
 */
template <class T> 
inline void PixelRGBA<T>::Composite( const PixelRGBA<T> & backPixel )
{
  float4 fTacc( 1 - A() );
  R() += fTacc * backPixel.R();
  G() += fTacc * backPixel.G();
  B() += fTacc * backPixel.B();
  A() += fTacc * backPixel.A();
} // Composite()


// $Log: PixelRGBA.inl,v $
// Revision 1.2  2006/01/18 20:35:35  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 3.7  2003/05/16 13:14:52  frank
// formatting
//
// Revision 3.6  2003/05/15 19:05:51  dongqing
// code review
//
// Revision 3.5  2002/12/20 19:34:54  ingmar
// fixed blending
//
// Revision 3.4  2002/12/20 19:23:36  ingmar
// fixed blending
//
// Revision 3.3  2002/12/20 19:00:27  ingmar
// finished code review
//
// Revision 3.2  2002/12/20 17:04:50  ingmar
// towards coding standards compliance
//
// $Id: PixelRGBA.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PixelRGBA.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
