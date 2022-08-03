// $Id: Line2D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)
// Created: Oct 16, 2001

// Complete history at bottom of file.

#ifndef Line_h
#define Line_h



// include declarations 
#include "Point.h"
#include "utilities.h"



/**
 * utility functions for floating point operations
 * @param fValue
 * @param fLimit1
 * @param fLimit2
 * @return 
 */
inline const bool IsBetween(const float4 fValue, const float4 fLimit1, const float4 fLimit2)
{
  return (fValue > fLimit1 - EPSILON && fValue < fLimit2 + EPSILON) || (fValue < fLimit1 + EPSILON && fValue > fLimit2 - EPSILON);
} // IsBetween

/**
 * @param f1
 * @param f2
 * @return 
 */
inline const bool IsEqual(const float4 f1, const float4 f2)
{
  return (fabs(f1 - f2) < EPSILON);
} // IsEqual

/**
 * @param p1
 * @param p2
 * @return 
 */
inline const bool IsSamePoint(const Point<float4>* p1, const Point<float4>* p2)
{
  if(IsEqual(p1->m_x, p2->m_x) && IsEqual(p1->m_y, p2->m_y))
    return true;
  else
    return false;
} // IsSamePoint

/**
 * @param f1
 * @param f2
 * @return
 */
inline const bool IsGreaterThan(const float4 f1, const float4 f2)
{
  return ((f1 - EPSILON) > f2);
} // IsGreaterThan

/**
 * @param f1
 * @param f2
 * @return
 */
inline const bool IsSmallerThan(const float4 f1, const float4 f2)
{
  return ((f1 + EPSILON) < f2);
} // IsSmallerThan



/// T should be a class that has members named m_x and m_y
template <class T>
class Line2D : Sec_math_2D
{
// member functions
public:
	/// Constructor
	inline Line2D(const T* p1, const T* p2)
    : m_pV1(p1), m_pV2(p2)
  {
    ComputeABC();
  } 

  /// Copy constructor
  inline Line2D(const float4 A, const float4 B, const float4 C)
    : m_A(A), m_B(B), m_C(C), m_pV1(NULL), m_pV2(NULL)
  {
  }

  /// Access the vertices;
  inline const T* Vertex(const uint4 i) const;
  inline const float4 GetA() const {return m_A;}
  inline const float4 GetB() const {return m_B;}
  inline const float4 GetC() const {return m_C;}

protected:
  /// Compute A, B, C from the two points
  inline void ComputeABC(){
    m_A = m_pV1->m_y - m_pV2->m_y;
    m_B = m_pV2->m_x - m_pV1->m_x;
    m_C = m_pV2->m_y * m_pV1->m_x - m_pV2->m_x * m_pV1->m_y;

  }
/*
  inline const bool InOpenRange(const float4 fValue, const float4 fLimit1, const float4 fLimit2){
    return (fValue > fLimit1 && fValue < fLimit2) || (fValue < fLimit1 && fValue > fLimit2);
  }

  inline const bool InClosedRange(const float4 fValue, const float4 fLimit1, const float4 fLimit2){
    return (fValue >= fLimit1 && fValue =< fLimit2) || (fValue =< fLimit1 && fValue >= fLimit2);
  }
*/

// data members
protected:
  /// vertices
	const T * m_pV1, * m_pV2;
private:
  
  float4 m_k;
  float4 m_b;

  float m_A, m_B, m_C;
};

/**
 */
template<class T>
class Edge2D : public Line2D<T>
{
public:
 	inline Edge2D(const T* p1, const T* p2) : Line2D<T>(p1, p2) {} 

  const float4 SquareLength() const {
    return (m_pV1->m_x - m_pV2->m_x) * (m_pV1->m_x - m_pV2->m_x) + (m_pV1->m_y - m_pV2->m_y) * (m_pV1->m_y - m_pV2->m_y);
  }

  /// Whether the whole edge is on the right of point, that is, all the points on the edge have greater x values
  const bool IsRightOf(const T* point) const {
    return IsGreaterThan(Vertex(1)->m_x, point->m_x) && IsGreaterThan(Vertex(2)->m_x, point->m_x);
  }

  /// Whether the whole edge is on the left of point, that is, all the points on the edge have smaller x values
  const bool IsLeftOf(const T* point) const {
    return IsSmallerThan(Vertex(1)->m_x, point->m_x) && IsSmallerThan(Vertex(2)->m_x, point->m_x);
  }

  /// Whether the whole edge is on the right of point, that is, all the points on the edge have greater x values
  const bool IsHigherThan(const T* point) const {
    return IsGreaterThan(Vertex(1)->m_y, point->m_y) && IsGreaterThan(Vertex(2)->m_y, point->m_y);
  }

  /// Whether the whole edge is on the left of point, that is, all the points on the edge have smaller x values
  const bool IsLowerThan(const T* point) const {
    return IsSmallerThan(Vertex(1)->m_y, point->m_y) && IsSmallerThan(Vertex(2)->m_y, point->m_y);
  }

};



#include "Line2D.inl"



#endif



// $Log: Line2D.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:43:45  michael
// code review
//
// Revision 3.8  2003/05/15 15:55:10  frank
// spelling
//
// Revision 3.7  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 3.6  2003/01/02 19:37:43  ingmar
// better documentation structure
//
// Revision 3.5  2001/01/02 18:51:33  ingmar
// better documentation structure
//
// Revision 3.4  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.3  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.2  2001/10/23 21:45:22  liwei
// Code cleaned.
//
// Revision 3.1  2001/10/16 21:48:22  liwei
// Added classes Line2D, Edge2D and Intersector.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Line2D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Line2D.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
