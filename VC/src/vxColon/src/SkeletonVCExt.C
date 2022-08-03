// $Id: SkeletonVCExt.C,v 1.3.2.1 2009/01/10 07:24:55 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Kevin Kreeger (kevin@viatronix.net)


// includes
#include "stdafx.h"
#include "SkeletonVCExt.h"

using namespace ReaderLib;

// const member initialization
const SkeletonNodeVCExt SkeletonVCExt::m_backgroundVCExt;


// defines
#define FILE_REVISION "$Revision: 1.3.2.1 $"


/*
 * Default constructor.
 */
SkeletonVCExt::SkeletonVCExt()
{
} // Skeleton()


/**
 * Destructor.
 */
SkeletonVCExt::~SkeletonVCExt()
{
} // ~Skeleton()


/**
 * Resize the skeleton node array.
 *
 * @param uNumElements            New size.
 */
void SkeletonVCExt::Resize(const uint2 uNumElements, const uint2 uNumSegments)
{
  m_vSkeVCExt.resize(uNumElements);
  m_vSegmentFirstNodeInJoined.resize(uNumSegments);
  Skeleton::Resize(uNumElements);
} // Resize()


/**
 * Erase a particular element.
 *
 * @param uIndex                 which element to remove.
 */
void SkeletonVCExt::Erase(const uint2 uIndex)
{
  if (uIndex < m_vSke.size())
  {
    std::vector<SkeletonNodeVCExt>::iterator it(m_vSkeVCExt.begin() + uIndex);
    m_vSkeVCExt.erase(it);
  } 
  else 
  {
    // throw ???
  }
  Skeleton::Erase(uIndex);
} // Erase()


/**
 * get complete Skeleton Node: we need this version in some cases. 
 *
 * @param uIndex                 Which element to return.
 * @param bNotOutOfBound         Whether to clamp to end of skeleton.
 * @return                       Pointer to return class.
 */
const SkeletonNodeVCExt & SkeletonVCExt::GetExtSkelNode(uint2 uIndex, bool bClampToEndOfSkeleton) const
{
  if (uIndex >= GetNumOfElements()) 
  {
    if (bClampToEndOfSkeleton)
    {
      uIndex = GetNumOfElements() - 1;
    }
    else
    {
      std::stringstream tt;
      tt << "tried to Get VCExt Skeleton Node out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
      LogWrn(tt.str(),"SkeletonVCExt","GetExtSkelNode");
      return m_backgroundVCExt;
    }
  }
  return m_vSkeVCExt[uIndex];
} // GetExtSkelNode()


/**
 * get Acceleration along the skeleton in World coordinates.
 *
 * @param uIndex                 Which element to return.
 * @return                       Acceleration Value.
 */
float4 SkeletonVCExt::GetAcceleration(const uint2 uIndex) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to read Skeleton Node Acceleration out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","GetAcceleration");
    return m_backgroundVCExt.m_fAcceleration;
  }
  return m_vSkeVCExt[uIndex].m_fAcceleration;
} // GetAcceleration()


/**
 * get Registration Confidence along the skeleton.
 *
 * @param uIndex                 Which element to return.
 * @return                       Confidence Value.
 */
float4 SkeletonVCExt::GetRegistrationConfidence(const uint2 uIndex) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to read Skeleton Node Confidence out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","GetRegistrationConfidence");
    return m_backgroundVCExt.m_fRegistrationConfidence;
  }
  return m_vSkeVCExt[uIndex].m_fRegistrationConfidence;
} // GetRegistrationConfidence()


/**
 * get registered index along the skeleton.
 *
 * @param uIndex                 Which element to return.
 * @return                       Index Value.
 */
int4 SkeletonVCExt::GetRegistrationOtherIndex(const uint2 uIndex) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to read Skeleton Node Registration Other Index out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","GetRegistrationOtherIndex");
    return m_backgroundVCExt.m_iRegistrationOtherIndex;
  }
  return m_vSkeVCExt[uIndex].m_iRegistrationOtherIndex;
} // GetRegistrationOtherIndex()


/**
 * get registered index along the skeleton.
 *
 * @param uIndex                 Which element to return.
 * @return                       Index Value.
 */
const Vector<float4> & SkeletonVCExt::GetCenterOfGravity() const
{
  return m_vCenterOfGravity;
} // GetCenterOfGravity()


/**
 * set complete Skeleton Node.
 *
 * @param uIndex                 Which element to update.
 * @param skeVCExtNode           The VC extension portion of the node.
 * @param skeNode                The common portion of the node.
 * @return                       Success or Failure.
 */
bool SkeletonVCExt::SetSkelNode(const uint2 uIndex, const SkeletonNodeVCExt & skeVCExtNode, const SkeletonNode & skeNode)
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to set VCExt Skeleton Node out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","SetSkelNode");
    return false;
  }
  m_vSkeVCExt[uIndex] = skeVCExtNode;
  return Skeleton::SetSkelNode(uIndex,skeNode);
} // SetSkelNode()


/**
 * Set confidence of registration for the specified skel node.
 *
 * @param uIndex                 Which element to update.
 * @param fConfidence            The confidence value.
 * @return                       Success or Failure.
 */
bool SkeletonVCExt::SetRegistrationConfidence(const uint2 uIndex, const float4 fConfidence)
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to Set Registration Confidence out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","SetRegistrationConfidence");
    return false;
  }
  m_vSkeVCExt[uIndex].m_fRegistrationConfidence = fConfidence;
  return true;
} // SetRegistrationConfidence()


/**
 * Set index of registration for the specified skel node.
 *
 * @param uIndex                  Which element to update.
 * @param iRegistrationOtherIndex The registered index.
 * @return                        Success or Failure.
 */
bool SkeletonVCExt::SetRegistrationOtherIndex(const uint2 uIndex, const int4 iRegistrationOtherIndex)
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to SetRegistrationOtherIndex out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","SetRegistrationOtherIndex");
    return false;
  }
  m_vSkeVCExt[uIndex].m_iRegistrationOtherIndex = iRegistrationOtherIndex;
  return true;
} // SetRegistrationOtherIndex()


/**
 * Assign the pointer for each segment into the total skeleton for quick conversion segment-Index intio joined-index later.
 *
 * @param uSegmentId              Which segmnetID to update.
 * @param uFirstNodeInSegment     the index pointer into the joined list.
 * @return                        Success or Failure.
 */
bool SkeletonVCExt::SetFirstNodeInSegment(const uint4 uSegmentId, const uint4 uFirstNodeInSegment)
{
  m_vSegmentFirstNodeInJoined[uSegmentId] = uFirstNodeInSegment;
  return true;
} // SetFirstNodeInSegment()


/**
 * Calculate Suzi's acceleration for the entire skeleton.
 *
 * @param uStart                 Index to start on.
 * @param uStop                  Index to end on.
 */
void SkeletonVCExt::CalculateAcceleration(const uint4 uStart, const uint4 uStop)
{
  const int4 iNormalResolution = 50;
  const int4 iMinResolution = 20;
  
  register int4 i;
  // If this is a short skeleton set all to zero
  if(int4(uStop-uStart) < 2*iMinResolution)
  {
    for ( i=uStart; i<uStop; i++)
    {
      m_vSkeVCExt[i].m_fAcceleration = 0.0F;
    } // loop over all skeleton nodes
    return;
  } // endif very small skeleton
  
  // Set startup to zero
  for (i=uStart; i<uStart+iMinResolution; i++)
  {
    m_vSkeVCExt[i].m_fAcceleration = 0.0F;
  } // loop over startup skeleton nodes
  
	int4 iResolution;
	const int4 isize = m_vSke.size()-1;
  // Loop through middle of the skeleton
  for (i=uStart+iMinResolution; i<uStop-iMinResolution; i++)
  {
    // what should the resolution be? (smaller at the ends)
    iResolution = iNormalResolution;
    if(i-uStart < iNormalResolution) iResolution = i-uStart;
    if((uStop-1-i) < iNormalResolution) iResolution = uStop-1-i;
    
    // Calculate the acceleration
    Vector<float4> vIn  = m_vSke[i].m_vWorldPos - m_vSke[(i-iResolution > 0) ? (i-iResolution) : 0].m_vWorldPos;
    Vector<float4> vOut = m_vSke[(i+iResolution < isize) ? i+iResolution : isize].m_vWorldPos - m_vSke[i].m_vWorldPos;
    Vector<float4> vAccel = vOut - vIn;
    m_vSkeVCExt[i].m_fAcceleration = vAccel.GetLength();
  } // loop over middle skeleton nodes
  
  // set stopdown to zero
  for (i=uStop-iMinResolution; i<uStop; i++)
  {
    m_vSkeVCExt[i].m_fAcceleration = 0.0F;
  } // loop over stopdown skeleton nodes

} // CalculateAcceleration()


/**
 * Locates the center of gravity
 */
void SkeletonVCExt::LocateCenterOfGravity()
{
  Vector<float4> vSum(0.0F,0.0F,0.0F);
  
  for(uint4 i=0; i< m_vSkeVCExt.size(); i++)
  {
    vSum += Vector<float4>(m_vSke[i].m_vWorldPos);
  }
  
  m_vCenterOfGravity = vSum / m_vSkeVCExt.size();
} // LocateCentersOfGravity()


/**
 * set complete Skeleton Node flipped prone to supine.
 *
 * @param uIndex               Which element to update.
 * @param skeVCExtNode         The VC extension portion of the node.
 * @param skeNode              The common portion of the node.
 * @param vUnits               Volume Header Parameters.
 * @param vDim                 Volume Header Parameters.
 * @return                     Pass or Fail.
 */
bool SkeletonVCExt::FlipSkelNode(const uint2 uIndex, const SkeletonNodeVCExt & skeVCExtNodeIn, const SkeletonNode & skeNodeIn,
                                 const Triple<float4> vUnits, const Triple<uint4> vDim)
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to FlipSkelNode out of bounds index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"SkeletonVCExt","FlipSkelNode");
    return false;
  }
  Triple<float4> vWorldDim(vUnits.m_x*vDim.m_x,vUnits.m_y*vDim.m_y,vUnits.m_z*vDim.m_z);
  SkeletonNode skeNode;
  skeNode.m_branchIdx  = skeNodeIn.m_branchIdx;
  skeNode.m_worldDfe   = skeNodeIn.m_worldDfe;
  skeNode.m_worldDfs   = skeNodeIn.m_worldDfs;
  // need to take (dim-1 - X&Y coordinate) to rotate from prone to supine
  skeNode.m_vWorldPos  = Point<float4> (skeNodeIn.m_vWorldPos.X() ,skeNodeIn.m_vWorldPos.Y() ,skeNodeIn.m_vWorldPos.Z() );
  skeNode.m_vWorldTan  = Normal<float4>(skeNodeIn.m_vWorldTan.X() ,skeNodeIn.m_vWorldTan.Y() ,skeNodeIn.m_vWorldTan.Z() );
  skeNode.m_vWorldUp   = Normal<float4>(skeNodeIn.m_vWorldUp .X() ,skeNodeIn.m_vWorldUp .Y() ,skeNodeIn.m_vWorldUp .Z() );
  skeNode.m_vVolumePos = Point<float4> (skeNodeIn.m_vVolumePos.X(),skeNodeIn.m_vVolumePos.Y(),skeNodeIn.m_vVolumePos.Z());
  SkeletonNodeVCExt skeVCExtNode;
  skeVCExtNode.m_fAcceleration = skeVCExtNodeIn.m_fAcceleration;
  skeVCExtNode.m_iSegmentIndex = skeVCExtNodeIn.m_iSegmentIndex;
  skeVCExtNode.m_uSkelIndexWithinSegment = skeVCExtNodeIn.m_uSkelIndexWithinSegment;
  skeVCExtNode.m_iRegistrationOtherIndex = skeVCExtNodeIn.m_iRegistrationOtherIndex;
  skeVCExtNode.m_fRegistrationConfidence = skeVCExtNodeIn.m_fRegistrationConfidence;
  
  m_vSkeVCExt[uIndex] = skeVCExtNode;
  return Skeleton::SetSkelNode(uIndex,skeNode);
} // FlipSkelNode()


/**
 * Converts a given local centerline index to an index in the joined dataset.
 *
 * @param segment                 Segment for the centerline index.
 * @param uLocalCenterlineIndex   The local index.
 * @return                        Corresponding index in the joined skeleton.
 */
uint4 SkeletonVCExt::ConvertToJoinedIndex(const Segment & segment, const uint4 uLocalCenterlineIndex) const
{
  uint4 uJoinedCenterlineIndex = m_vSegmentFirstNodeInJoined[segment.GetIdentifier()];
  uJoinedCenterlineIndex += (segment.IsRegularDirection()) ? uLocalCenterlineIndex : -1*uLocalCenterlineIndex;
  return uJoinedCenterlineIndex;
} // ConvertToJoinedIndex()


/**
 * Print joined data.
 *
 * @param sMessage   Message to be output at start of file
 */
void SkeletonVCExt::PrintJoinedData(const std::string& sMessage) const
{
  // note: This stream should really be created outside and passed in as an argument.
  //       Formatting could be done here, though.
  std::ofstream outStream("C:/V-System/Log/JoinedData-SKE.txt", std::ios::app); outStream.precision(2); outStream.width(10); outStream.setf(std::ios::right | std::ios::showpoint | std::ios::fixed );
  
  outStream << sMessage << std::endl;

  outStream << "joinIndex dfs      x       y       z      accel seg index regOther conf" << std::endl;
  for (uint4 i=0; i<m_vSkeVCExt.size(); i++)
  {
    outStream.width(4); outStream << i << " ";
    outStream.width(8); outStream << m_vSke[i].m_worldDfs << " (";
    outStream.width(7); outStream << m_vSke[i].m_vWorldPos.m_x <<" ";
    outStream.width(7); outStream << m_vSke[i].m_vWorldPos.m_y << " ";
    outStream.width(7); outStream << m_vSke[i].m_vWorldPos.m_z << ") ";
    outStream.width(7); outStream << m_vSkeVCExt[i].m_fAcceleration << " ";
    outStream.width(2); outStream << m_vSkeVCExt[i].m_iSegmentIndex << " ";
    outStream.width(4); outStream << m_vSkeVCExt[i].m_uSkelIndexWithinSegment << " ";
    outStream.width(4); outStream << m_vSkeVCExt[i].m_iRegistrationOtherIndex << " ";
    outStream.width(6); outStream << m_vSkeVCExt[i].m_fRegistrationConfidence << std::endl;
  }
  
} // PrintJoinedData()

#undef FILE_REVISION 

// $Log: SkeletonVCExt.C,v $
// Revision 1.3.2.1  2009/01/10 07:24:55  jmeade
// comments
//
// Revision 1.3  2006/09/13 15:27:40  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.2  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.4.2.5  2003/05/27 12:23:43  frank
// code review
//
// Revision 1.4.2.4  2003/05/23 19:34:11  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.4.2.3  2003/05/01 18:37:25  frank
// code review
//
// Revision 1.4.2.2  2003/05/01 16:56:27  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.4.2.1  2003/03/26 23:17:35  jmeade
// Issue 2849:  Appropriate const's and comments.
//
// Revision 1.4  2003/01/22 22:27:42  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SkeletonVCExt.C,v 1.3.2.1 2009/01/10 07:24:55 jmeade Exp $
// $Id: SkeletonVCExt.C,v 1.3.2.1 2009/01/10 07:24:55 jmeade Exp $
