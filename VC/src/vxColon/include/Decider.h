// $Id: Decider.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Decider.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net
//
// Complete History at bottom of file.


#ifndef Decider_h
#define Decider_h

// includes
#include "Neighbor6Iterator.h"
#include "Neighbor18Iterator.h"
#include "Neighbor26Iterator.h"


/// Define several useful Decider classes
template <class Iter, class Vox > class DeciderThreshLess 
{
  Vox m_threshold;
public:
  DeciderThreshLess(const Vox & threshold = 1):m_threshold(threshold) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() < m_threshold); }
}; // <

template <class Iter, class Vox > class DeciderThreshLessEqual  {
  Vox m_threshold;
public:
  DeciderThreshLessEqual(const Vox & threshold = 1):m_threshold(threshold) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() <= m_threshold); }
}; // <=

template <class Iter, class Vox > class DeciderThreshGreater  {
  Vox m_threshold;
public:
  DeciderThreshGreater(const Vox & threshold = 1):m_threshold(threshold) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() > m_threshold); }
}; // >

template <class Iter, class Vox > class DeciderThreshGreaterEqual  {
  Vox m_threshold;
public:
  DeciderThreshGreaterEqual(const Vox & threshold = 1):m_threshold(threshold) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() >= m_threshold); }
}; // >=

template <class Iter, class Vox > class DeciderEqual  {
  Vox m_value;
public:
  DeciderEqual(const Vox & value = 1):m_value(value) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() == m_value); }
}; // ==

template <class Iter, class Vox > class DeciderRange  {
  Vox m_value_low;
  Vox m_value_high;
public:
  DeciderRange(const Vox & value_low, const Vox & value_high):m_value_low(value_low), m_value_high(value_high) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return(dataIter.GetVoxelRaw() >= m_value_low && dataIter.GetVoxelRaw() <= m_value_high); }
}; // a <= x <= b


template <class Iter, class Vox, class InRegionDecider > class DeciderOutsideSurface6  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderOutsideSurface6(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderOutsideSurface6


template <class Iter,  class Vox, class InRegionDecider > class DeciderOutsideSurface18  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderOutsideSurface18(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderOutsideSurface18


template <class Iter,  class Vox, class InRegionDecider > class DeciderOutsideSurface26  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderOutsideSurface26(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderOutsideSurface26


template <class Iter, class Vox, class InRegionDecider > class DeciderInsideSurface6  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderInsideSurface6(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderInsideSurface6


template <class Iter, class Vox, class InRegionDecider > class DeciderInsideSurface18  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderInsideSurface18(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderInsideSurface18


template <class Iter, class Vox, class InRegionDecider > class DeciderInsideSurface26  {
  InRegionDecider m_inRegionDecider;
public:
  DeciderInsideSurface26(const InRegionDecider & inRegionDecider)
    :m_inRegionDecider(inRegionDecider) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // DeciderInsideSurface26


template <class Iter, class Vox > class DeciderSphere  {
  uint2 m_uRadiusSquare;
  Vector<uint2> m_startPos;
public:
  DeciderSphere(const Vector<uint2> & startPos, const uint2 & radius = 1):m_startPos(startPos), m_uRadiusSquare(radius*radius) {}
  inline bool IsPartOfRegion(const Iter & dataIter) const { return((m_startPos-Vector<uint2>(dataIter.GetPos().m_x,dataIter.GetPos().m_y,dataIter.GetPos().m_z)).GetSquaredLength() < m_uRadiusSquare); }
}; // DeciderSphere



#include "Decider.inl"

#endif // Decider_h

// $Log: Decider.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.3  2001/12/24 18:13:00  ingmar
// moved Sec_Decider from 03_utils/Documentation.h to 31_volIters/Decider.h
//
// Revision 3.2  2001/11/06 21:54:13  jaddonisio
// New decider: decider within a sphere.
//
// Revision 3.1  2001/10/24 21:39:08  jaddonisio
// New decider for range.
//
// Revision 3.0  2001/10/14 23:02:22  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:28   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.20  2001/05/17 00:22:00  ingmar
// DeciderHullOutside now has its own files
//
// Revision 1.19  2001/05/15 13:44:37  ingmar
// DeciderHullInside now has its own file
//
// Revision 1.18  2001/05/13 19:52:48  ingmar
// added DeciderHullOutside
//
// Revision 1.17  2001/05/08 22:44:42  ingmar
// new hull decider
//
// Revision 1.16  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.15  2001/04/06 22:58:00  ingmar
// added documentation inheritance
//
// Revision 1.14  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.13  2000/11/21 19:38:56  sarang
// gcc_bug_fix : header file was not included!!
//
// Revision 1.12  2000/11/21 03:27:13  ingmar
// implemented and tested Decider Outside/Inside Surface 18
//
// Revision 1.11  2000/11/20 17:32:48  ingmar
// implemented and tested DeciderInsideSurface 6 and 26
//
// Revision 1.10  2000/11/20 01:38:05  ingmar
// implemented DeciderOutsideSurface26
//
// Revision 1.9  2000/11/20 00:53:50  ingmar
// added Test function
//
// Revision 1.8  2000/11/19 01:34:52  ingmar
// made DeciderOutsideSurface6  use the new neighbor iterator
//
// Revision 1.7  2000/11/19 00:43:08  ingmar
// IsInsideRegion is now const
//
// Revision 1.6  2000/11/01 20:35:59  ingmar
// added inside surface crawlers
//
// Revision 1.5  2000/11/01 17:18:04  ingmar
// added surface crawl decider for 26 connected voxels
//
// Revision 1.4  2000/10/31 22:33:56  ingmar
// renamed DeciderDilate to DeciderOutsideSurface
//
// Revision 1.3  2000/10/31 16:58:28  ingmar
// fixed syntax error that sneaked in before
//
// Revision 1.2  2000/10/30 18:55:04  ingmar
// added dilate region iterator
//
// Revision 1.1  2000/10/30 18:08:02  ingmar
// added 26 connected region grow iterator to volume module
//
//
// $Id: Decider.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
