// $Id: vxFixedPoint.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com

#ifndef vxFixedPoint_h
#define vxFixedPoint_h


// include declarations


class vxFixedPoint
{
  // public member functions
public:
  /// default constructor
  vxFixedPoint()                                            : m_int(0)                                   { }

  /// constructor
  vxFixedPoint(const int4 iFixedPointFormat, int4 iDummy)   : m_int(iFixedPointFormat)                   { }

  /// constructor
  vxFixedPoint(const int4 iNum)                             : m_int(iNum << 16)                          { }

  /// constructor
  vxFixedPoint(const float4 fNum)                           : m_int(static_cast<int4>(fNum * (1 << 16))) { }

  /// constructor
  vxFixedPoint(const float8 fNum)                           : m_int(static_cast<int4>(fNum * (1 << 16))) { }

  /// copy constructor
  vxFixedPoint(const vxFixedPoint & other)                  : m_int(other.m_int)                         { }

  /// destructor
  ~vxFixedPoint()                                                                                        { }

  inline vxFixedPoint & operator =  (const int4   iNum)                        {        m_int = iNum<<16;                            return *this; }
  inline vxFixedPoint & operator =  (const float4 fNum)                        {        m_int = static_cast<int4>(fNum * (1 << 16)); return *this; }
  inline vxFixedPoint & operator =  (const float8 fNum)                        {        m_int = static_cast<int4>(fNum * (1 << 16)); return *this; }
                                                                               
  inline vxFixedPoint   operator +  (const vxFixedPoint & rhs)           const { return vxFixedPoint(m_int + rhs.m_int, 0); }
  inline vxFixedPoint   operator -  (const vxFixedPoint & rhs)           const { return vxFixedPoint(m_int - rhs.m_int, 0); }
  inline vxFixedPoint   operator *  (const vxFixedPoint & rhs)           const { return vxFixedPoint((m_int>>8) * (rhs.m_int>>8), 0); }
                                                                         
  inline vxFixedPoint & operator += (const vxFixedPoint & rhs)                 {        m_int += rhs.m_int;                          return *this; }
  inline vxFixedPoint & operator -= (const vxFixedPoint & rhs)                 {        m_int -= rhs.m_int;                          return *this; }
  inline vxFixedPoint & operator *= (const vxFixedPoint & rhs)                 {        m_int = m_int>>8; m_int *= rhs.m_int>>8;     return *this; }
                                                                             
  inline vxFixedPoint   operator +  (const int4 rhs)                     const { return vxFixedPoint(m_int + vxFixedPoint(rhs).m_int, 0);          }
  inline vxFixedPoint   operator -  (const int4 rhs)                     const { return vxFixedPoint(m_int - vxFixedPoint(rhs).m_int, 0);          }
  inline vxFixedPoint   operator *  (const int4 rhs)                     const { return vxFixedPoint(m_int * rhs, 0);                              }
                                                                               
  inline vxFixedPoint & operator += (const int4 rhs)                           {        m_int += vxFixedPoint(rhs).m_int;            return *this; }
  inline vxFixedPoint & operator -= (const int4 rhs)                           {        m_int -= vxFixedPoint(rhs).m_int;            return *this; }
  inline vxFixedPoint & operator *= (const int4 rhs)                           {        m_int *= rhs;                                return *this; }
  
  friend vxFixedPoint   operator +  (const int4 lhs, const vxFixedPoint & rhs) { return (vxFixedPoint(lhs) + rhs);                                 }
  friend vxFixedPoint   operator -  (const int4 lhs, const vxFixedPoint & rhs) { return (vxFixedPoint(lhs) - rhs);                                 }
  friend vxFixedPoint   operator *  (const int4 lhs, const vxFixedPoint & rhs) { return vxFixedPoint(rhs.m_int * lhs, 0);                          }

  inline vxFixedPoint   operator +  (const float4 rhs)                     const { return vxFixedPoint(m_int + vxFixedPoint(rhs).m_int, 0);          }
  inline vxFixedPoint   operator -  (const float4 rhs)                     const { return vxFixedPoint(m_int - vxFixedPoint(rhs).m_int, 0);          }
  inline vxFixedPoint   operator *  (const float4 rhs)                     const { return vxFixedPoint(m_int * rhs, 0);                              }
                                                                               
  inline vxFixedPoint & operator += (const float4 rhs)                           {        m_int += vxFixedPoint(rhs).m_int;            return *this; }
  inline vxFixedPoint & operator -= (const float4 rhs)                           {        m_int -= vxFixedPoint(rhs).m_int;            return *this; }
  inline vxFixedPoint & operator *= (const float4 rhs)                           {        m_int *= rhs;                                return *this; }
  
  friend vxFixedPoint   operator +  (const float4 lhs, const vxFixedPoint & rhs) { return (vxFixedPoint(lhs) + rhs);                                 }
  friend vxFixedPoint   operator -  (const float4 lhs, const vxFixedPoint & rhs) { return (vxFixedPoint(lhs) - rhs);                                 }
  friend vxFixedPoint   operator *  (const float4 lhs, const vxFixedPoint & rhs) { return vxFixedPoint(rhs.m_int * lhs, 0);                          }

  inline int4 GetFractionToMinusInfinityAsInt4()   
  { 
    return m_int & 0x0000FFFF;
  }

  inline int4 GetTruncatedToMinusInfinity() 
  { 
    int4 b = m_int & 0xFFFF0000; 
    return b >> 16;
  }

  inline float4 AsFloat() const { const float4 fOne(1<<16);  const float4 fOneReciprocal(1.0f/fOne); return m_int * fOneReciprocal; }
private:

  /// the 32bit integer representing the 16.16 fixed point number
  int4 m_int;
}; // class vxFixedPoint


#endif // vxFixedPoint_h


// $Log: vxFixedPoint.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.1  2002/10/01 14:40:18  ingmar
// initial version
//
// $Id: vxFixedPoint.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
