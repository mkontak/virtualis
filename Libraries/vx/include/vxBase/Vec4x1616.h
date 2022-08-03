// $Id: Vec4x1616.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com

#ifndef Vec4x1616_h
#define Vec4x1616_h


// includes 
#include "10_math.h"
#include "Global.h"
#include "Typedef.h"
#ifdef __ICL
  #include <dvec.h>
  #define ALIGN16  __declspec(align(16))
#else
  #define ALIGN16
#endif
//#include <sse2mmx.h>



class ALIGN16 Vec4x1616 : Sec_math_4D
{
  // public member functions
public:
  /// default constructor
  Vec4x1616()                                            : m_iVec(0,0,0,0)                           { }

  /// constructor
  Vec4x1616(const Is32vec4 i1616Vec4, int4 iDummy)       : m_iVec(i1616Vec4)                         { }

  /// constructor
  Vec4x1616(const __m128 i1616Vec4, int4 iDummy)        : m_iVec(*((Is32vec4*)(&i1616Vec4)))        { }

  /// constructor
  Vec4x1616(int4 i0, int4 i1, int4 i2, int4 i3)          : m_iVec(i0<<16, i1<<16, i2<<16, i3<<16 )   { }

  /// constructor
  Vec4x1616(float4 f0, float4 f1, float4 f2, float4 f3)  : m_iVec(static_cast<int4>(f0 * (1 << 16)), 
                                                                  static_cast<int4>(f1 * (1 << 16)), 
                                                                  static_cast<int4>(f2 * (1 << 16)), 
                                                                  static_cast<int4>(f3 * (1 << 16))) { }

  /// constructor
  Vec4x1616(float8 f0, float8 f1, float8 f2, float8 f3)  : m_iVec(static_cast<int4>(f0 * (1 << 16)), 
                                                                  static_cast<int4>(f1 * (1 << 16)), 
                                                                  static_cast<int4>(f2 * (1 << 16)), 
                                                                  static_cast<int4>(f3 * (1 << 16))) { }
  
  /// copy constructor
  Vec4x1616(const Vec4x1616 & other)                     : m_iVec(other.m_iVec)                      { }

  /// destructor
  ~Vec4x1616()                                                                                       { }

  inline Vec4x1616   operator +  (const Vec4x1616 & rhs)           const { return Vec4x1616(m_iVec + rhs.m_iVec, 0); }
  inline Vec4x1616   operator -  (const Vec4x1616 & rhs)           const { return Vec4x1616(m_iVec - rhs.m_iVec, 0); }
  inline Vec4x1616   operator *  (const Vec4x1616 & rhs)           const 
  {   
    Is32vec4 iVecA = _mm_srai_epi32(    m_iVec,8);
    Is32vec4 iVecB = _mm_srai_epi32(rhs.m_iVec,8);

    __m128 *a = &m_iVec;
    __m128 *b = &rhs.m_iVec;

    __m128 c = _mm_mul_ps(*a, *b);
    //__m128i result = _mm_mul_epu32(iVecA, iVecB);
    


    return Vec4x1616(c, 0); 
  }
                                                                         
  inline Vec4x1616 & operator += (const Vec4x1616 & rhs)                 {        m_iVec += rhs.m_iVec;                          return *this; }
  inline Vec4x1616 & operator -= (const Vec4x1616 & rhs)                 {        m_iVec -= rhs.m_iVec;                          return *this; }
  inline Vec4x1616 & operator *= (const Vec4x1616 & rhs)                 
  {
    _mm_srli_si128(m_iVec, 1);
    
    Is32vec4 iVecB( rhs.m_iVec );
    _mm_srli_si128(iVecB, 1);

    _mm_mul_epu32(m_iVec, iVecB);
    
    return *this; 
  }
                                                                                                                                                            

  inline Vec4x1616 GetFractionToMinusInfinityAsInt4()   
  { 
    const ALIGN16 Is32vec4 iVecLowBits(0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF);
    return Vec4x1616(m_iVec & iVecLowBits, 0);
  }

  inline Vec4x1616 GetTruncatedToMinusInfinity() 
  { 
    const ALIGN16 Is32vec4 iVecHighBits(0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000);
    return Vec4x1616(m_iVec & iVecHighBits, 0);
  }

  inline float4 AsFloat(const int4 iIndex) const { const float4 fOne(1<<16);  const float4 fOneReciprocal(1.0f/fOne); return m_iVec[iIndex] * fOneReciprocal; }

  /// convert to string.
  std::string ToString() const
  {
    std::strstream ss;
    ss << "Vec4x1616("<<AsFloat(3)<<", "<<AsFloat(2)<<", "<<AsFloat(1)<<", "<<AsFloat(0)<<")"<<'\0' ;
    return( std::string(ss.str()) );
  }

  /// Overloaded << Operator.
  inline friend std::ostream & operator<<(std::ostream & os, const Vec4x1616 & vec)
  {
	os << vec.ToString();
	return( os );
  } // operator<<

private:

  /// the four 32bit integer representing the four 16.16 fixed point numbers
  Is32vec4 m_iVec;
}; // class Vec4x1616


#endif // Vec4x1616_h


// $Log: Vec4x1616.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:48:47  michael
// code review
//
// Revision 1.3  2003/01/02 19:39:16  ingmar
// better documentation structure
//
// Revision 1.2  2001/01/02 18:52:50  ingmar
// better documentation structure
//
// Revision 1.1  2002/10/01 14:42:19  ingmar
// initial version
//
// $Id: Vec4x1616.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
