// $Id: SkeletonVCExt.h,v 1.1.12.1 2009/01/10 07:24:55 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Kevin Kreeger (kevin@viatronix.net)

#ifndef SkeletonVCExt_h
#define SkeletonVCExt_h


// include declarations
#include "ReaderLib.h" // defines Sec_ReaderLib
#include "Typedef.h"
#include "SkeletonNodeVCExt.h"
#include "Skeleton.h"
#include "Segment.h"

namespace ReaderLib
{

  /**
   * A class to encapsulate the skeleton information along with the V.C. specific additional fields.
   * Note that at the moment this class is only used for the "Joined Skeleton" after segmentation
   * verification and not for the stored segment skeletons on disk..
   */
  class SkeletonVCExt : public Skeleton
  {
  // Member Functions
  public:

    /// Default Constructor.
    SkeletonVCExt();

    /// Destructor
    virtual ~SkeletonVCExt();

    /// Change the number of elements in the skeleton.
    void Resize(const uint2 uNumElements, const uint2 uNumSegments);

    /// Remove an element.
    void Erase(const uint2 uIndex);
    
    /// Get Suzi's acceleration value.
    float4 GetAcceleration(const uint2 uIndex) const;
    
    /// Get how confident this specified node
    float4 GetRegistrationConfidence(const uint2 uIndex) const;
    
    /// Get a pointer into the other skeleton
    int4 GetRegistrationOtherIndex(const uint2 uIndex) const;
    
    /// get complete Skeleton Node: not out of range.
    const SkeletonNodeVCExt & GetExtSkelNode(uint2 uIndex, bool bClampToEndOfSkeleton = false) const; 

    /// Get the Point at the center of the colon voxels
    const Vector<float4> & GetCenterOfGravity() const;

    /// Set complete Skeleton Node.
    bool SetSkelNode(const uint2 uIndex, const SkeletonNodeVCExt & skeVCExtNode, const SkeletonNode & skeNode);
    
    /// Set confidence eof registration for the specified skel node.
    bool SetRegistrationConfidence(const uint2 uIndex, const float4 fConfidence);
    
    /// Set confidence eof registration for the specified skel node.
    bool SetRegistrationOtherIndex(const uint2 uIndex, const int4 iRegistrationOtherIndex);

    /// Assign the pointer for each segment into the total skeleton for quick conversion segment-Index into joined-index later
    bool SetFirstNodeInSegment(const uint4 uSegmentId, const uint4 uFirstNodeInSegment);
    
    /// Find the average world pos.
    void LocateCenterOfGravity();

    /// Calculate Suzi's acceleration for a section of the skeleton
    void CalculateAcceleration(const uint4 uStart, const uint4 uStop);

    /// Set complete Skeleton Node flipped Prone to Supine
    bool FlipSkelNode(const uint2 uIndex, const SkeletonNodeVCExt & skeVCExtNodeIn, const SkeletonNode & skeNodeIn,
                      const Triple<float4> vUnits, const Triple<uint4> vDim);
    
    /// Outputs joined data to an file
    void PrintJoinedData(const std::string& sMessage) const;

    /// Converts a given local centerline index to an index in the joined dataset.
    uint4 ConvertToJoinedIndex(const Segment & segment, const uint4 uLocalCenterlineIndex) const;

  // Data Members
  private:  

    /// The center of gravity of this skeleton
    Vector<float4> m_vCenterOfGravity;

    /// Pointers into "joined skeleton" for the segments
    std::vector<uint4> m_vSegmentFirstNodeInJoined;
    
    /// The list of skeleton nodes
    std::vector<SkeletonNodeVCExt> m_vSkeVCExt;

    /// A default node to return in case of error
    static const SkeletonNodeVCExt m_backgroundVCExt;

  }; // class SkeletonVCExt

} // namespace ReaderLib

#endif // SkeletonVCExt_h


// $Log: SkeletonVCExt.h,v $
// Revision 1.1.12.1  2009/01/10 07:24:55  jmeade
// comments
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3.2.5  2003/05/27 12:23:43  frank
// code review
//
// Revision 1.3.2.4  2003/05/23 19:34:11  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.3.2.3  2003/05/01 18:37:25  frank
// code review
//
// Revision 1.3.2.2  2003/05/01 16:56:27  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.3.2.1  2003/03/26 23:17:35  jmeade
// Issue 2849:  Appropriate const's and comments.
//
// Revision 1.3  2002/10/31 20:00:37  kevin
// Moved "convert segment-localIndex to JoinedIndex"conversion to here
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SkeletonVCExt.h,v 1.1.12.1 2009/01/10 07:24:55 jmeade Exp $
// $Id: SkeletonVCExt.h,v 1.1.12.1 2009/01/10 07:24:55 jmeade Exp $
