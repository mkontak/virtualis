// $Id: ValueRange.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef ValueRange_h
#define ValueRange_h

/*
 * Use this class to get the lower and upper bound of a sequence of numbers.
 * It takes care of the startup and comparison logic for you.
 */
template <class T>
class ValueRange
{

// Member Functions
public:

  /// Constructor
  inline ValueRange<T>();

  /// Convenience constructor
  inline ValueRange<T>( const T & initialValue );

  /// Add a new value
  inline void AddValue( const T & value );

  /// Compute where this value lies as a percentile of the range
  inline float4 GetPercentile( const T & inValue ) const;

  /// Get the minimum value
  const T & GetMinimum() const { return m_minValue; }

  /// Get the maximum value
  const T & GetMaximum() const { return m_maxValue; }
  
  /// Equality operator
  inline bool operator ==( const ValueRange & other ) const;


// Data Members
private:

  /// Whether or not this object has been initialized with any values
  bool m_bInitialized;

  /// The minimum value
  T m_minValue;

  /// The maximum value
  T m_maxValue;

}; // class ValueRange


#include "ValueRange.inl"


#endif // ValueRange_h


// $Log: ValueRange.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.5  2003/05/15 15:02:12  frank
// code review
//
// Revision 1.4  2003/05/13 21:09:19  dongqing
// code review
//
// Revision 1.3  2002/11/13 01:08:31  frank
// Added a convenience function to initialize the value to some constant.
//
// Revision 1.2  2002/08/16 21:08:17  ana
// Equals operator implemented.
//
// Revision 1.1  2002/06/26 16:54:10  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ValueRange.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ValueRange.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
