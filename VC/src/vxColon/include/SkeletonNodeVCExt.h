// $Id: SkeletonNodeVCExt.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright (c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Kevin Kreeger     Kevin@viatronix.com


#ifndef SkeletonNodeVCExt_h
#define SkeletonNodeVCExt_h

// includes
#include "ReaderLib.h" // defines Sec_ReaderLib
#include "Typedef.h"

namespace ReaderLib
{

  /**
   * Structure for storing the supplemental fields in the VC skeleton above and beyond the normal Skeleton in vxBase::50_datastruct
   */
  class SkeletonNodeVCExt : Sec_ReaderLib
  {
  // member functions
  public:

    /// default constructor
    SkeletonNodeVCExt() : m_fAcceleration(0.0F), m_iSegmentIndex(-1), m_uSkelIndexWithinSegment(0), 
                          m_iRegistrationOtherIndex(-1), m_fRegistrationConfidence(1.0F) {}

  // data members
  public:

    /// Suzi's Acceleration values used for registration
    float4 m_fAcceleration;

    /// Index of which segment this node came from  (-1 indicates gap)
    int4 m_iSegmentIndex; // the sorted segment index (starts at 0, Note: does NOT match ins file)

    /// Index of the skeleton index for the local (non-joined) skeleton within each segment
    uint4 m_uSkelIndexWithinSegment;

    /// Index pointer into other joined skeleton from registration (-1 indicates not filled in)
    int4 m_iRegistrationOtherIndex;

    /// Our confidence in the registration quality at this point
    float4 m_fRegistrationConfidence;

  }; // class SkeletonNodeVCExt

} // namespace ReaderLib

#endif // SkeletonNodeVCExt_h


// $Log: SkeletonNodeVCExt.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3.2.2  2003/05/01 18:15:37  frank
// code review in progress
//
// Revision 1.3.2.1  2003/05/01 16:56:27  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.3  2002/11/16 22:28:31  kevin
// Default skeleton quality to 1.0 (so single datasets are green, not brown)
//
// Revision 1.2  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 1.1  2002/09/30 19:11:19  kevin
// Re-Factor of SUpine/Prone registration ot place data structures
// in more meaningful places and remove so much copying of
// large data strucutres (there was a lot of creating local copies of
// refernces passed into toomany classes).
//
// Suzi and Ana's original registration is still presetn. In fact the registration
// still runs off their data structures. I jsut added new ones. From here on, I will
// have to break theres. So I wanted a version I could always go back
// to in case I broke anything.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SkeletonNodeVCExt.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: SkeletonNodeVCExt.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
