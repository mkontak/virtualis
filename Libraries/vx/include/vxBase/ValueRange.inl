// $Id: ValueRange.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * Constructor
 */
template <class T>
ValueRange<T>::ValueRange()
: m_bInitialized(false),
m_minValue(0),
m_maxValue(0)
{
} // ValueRange()


/**
 * Convenience constructor
 *
 * @param initialValue  the initial value
 */
template <class T>
ValueRange<T>::ValueRange( const T & initialValue )
: m_bInitialized(true),
m_minValue( initialValue ),
m_maxValue( initialValue )
{
} // ValueRange()


/**
 * Add a new value
 *
 * @param value  the new value to add
 */
template <class T>
void ValueRange<T>::AddValue( const T & value )
{
  if ( m_bInitialized )
  {
    if ( value < m_minValue ) m_minValue = value;
    if ( value > m_maxValue ) m_maxValue = value;
  }
  else
  {
    m_bInitialized = true;
    m_minValue = value;
    m_maxValue = value;
  }
} // AddValue()


/**
  * Compute where this value lies as a percentile of the range.
  *
  * @param inValue
  * @return 0.0F when value is at the lower end of the range.
  *         1.0F when value is at the upper end of the range.
  *         return value is clamped to range (0,1).
  */
template <class T>
float4 ValueRange<T>::GetPercentile( const T & inValue ) const
{
  if ( m_bInitialized && m_minValue < m_maxValue )
  {
    const float4 fPercent( ( float4(inValue) - float4(m_minValue) ) / ( float4(m_maxValue) - float4(m_minValue) ) );
    return Bound( 0.0F, fPercent, 1.0F );
  }
  else
  {
    return 0.0F;
  }
} // GetPercentile()


/**
  * Equality operator
  *
  * @param other  the ValueRange we are to compare
  * @return true if they are equal, false otherwise
  */
template <class T>
inline bool ValueRange<T>::operator ==( const ValueRange & other ) const
{

  return m_minValue == other.m_minValue && m_maxValue == other.m_maxValue;

} // operator ==


// $Log: ValueRange.inl,v $
// Revision 1.2  2006/01/18 20:35:35  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.1  2003/05/15 15:03:04  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ValueRange.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
// $Id: ValueRange.inl,v 1.2 2006/01/18 20:35:35 geconomos Exp $
