// $Id: MatchingFeature.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright (c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Kevin Kreeger     Kevin@viatronix.com


#ifndef MatchingFeature_h
#define MatchingFeature_h


// includes
#include "ReaderLib.h"
#include "Typedef.h"

namespace ReaderLib
{

  /**
   * Structure for storing index into the joined skelton structure for features which match
   */
  class MatchingFeature : Sec_ReaderLib
  {
  // member functions
  public:

    /// default constructor
    MatchingFeature() : m_uSupineFeatureIndex(0), m_uProneFeatureIndex(0), m_fConfidence(0.0F), 
                        m_fQuality(0.0F), m_fWindowedQuality(0.0F) {}

    /// Constructor
    MatchingFeature(const uint4 uSupineFeatureIndex, const uint4 uProneFeatureIndex, const float4 fConfidence,
                    const float4 fQuality, const float4 fWindowedQuality = 0.0F) :
                    m_uSupineFeatureIndex(uSupineFeatureIndex), m_uProneFeatureIndex(uProneFeatureIndex), m_fConfidence(fConfidence),
                    m_fQuality(fQuality), m_fWindowedQuality(fWindowedQuality) {}

  // data members
  public:

    /// Index into supine joined skeleton
    uint4 m_uSupineFeatureIndex;

    /// Index into prone joined skeleton
    uint4 m_uProneFeatureIndex;  

    /// Confidence of this matching feature
    float4 m_fConfidence;

    /// Quality of this matching feature (from m_mMatchQuality matrix)
    float4 m_fQuality;

    /// Computed quality to display to the user
    float4 m_fWindowedQuality;

  }; // class MatchingFeature

} // namespace ReaderLib

#endif // MatchingFeature_h


// $Log: MatchingFeature.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3.2.2  2003/05/01 18:07:24  frank
// code review
//
// Revision 3.3.2.1  2003/05/01 16:56:06  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.3  2002/10/31 20:04:59  kevin
// Changed user feature points to make them more robust and sorted
// so that they cannot become invalid (e.g. crossing user features)
//
// Revision 3.2  2002/10/23 00:24:09  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 3.1  2002/10/21 18:51:52  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/MatchingFeature.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: MatchingFeature.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
