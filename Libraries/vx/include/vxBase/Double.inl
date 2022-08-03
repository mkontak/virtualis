// $Id: Double.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright© 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Ingmar (ingmar@viatronix.com)


/**
 * Constructor from two scalars.
 */
template <class T> 
Double<T>::Double(const T & x, const T & y) 
{
  m_xy[0] = x;
  m_xy[1] = y;
} // Double(x,y)

/**
 * Assignment Operator.
 */
template <class T> 
Double<T> & Double<T>::operator = (const Double<T> & rhs)
{
  X() = rhs.X();
  Y() = rhs.Y();
  return *this;
} // operator =


///////////////////////////////////////////////////////////////////////
// comparators


/**
 * Equality operator.
 */
template <class T>
inline bool Double<T>::operator ==(const Double<T> & rhs) const
{
  return((m_xy[0] == rhs.m_xy[0]) && 
         (m_xy[1] == rhs.m_xy[1])    );
} // operator ==()


/**
 * Inequality operator.
 */
template <class T>
inline bool Double<T>::operator !=(const Double<T> & rhs) const
{
  return !(*this == rhs); 
} // operator !=()


/**
 * Set the red, green, blue and alpha components
 */
template <class T> 
inline void Double<T>::SetXY(const T & x, const T & y)
{
  m_xy[0] = x;
  m_xy[1] = y;
} // SetXY()


// $Log: Double.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2002/12/23 16:52:47  ingmar
// added GetMaxAbsComponent() and GetMaxAbsComponentCoord()
//
// Revision 1.1  2002/12/21 00:37:05  ingmar
// initial version
//
// $Id: Double.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Double.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $

