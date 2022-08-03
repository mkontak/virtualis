// $Id: ColonSession.h,v 1.13.2.3 2010/01/25 22:24:32 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include <map>
#include "BookmarkList.h"
#include "SegmentList.h"
#include "Camera.h"
#include "vxCadFinding.h"
#include "MeasureLineList.h"


// forward declarations
namespace ReaderLib
{
  class Dataset;
}


class ColonSession  
{
// inner classes
public:

  class FindingInfo
  {
  // enumerations
  public:

    /// whether a CAD finding has been accepted, denied, or undecided between the two
    enum DecisionEnum { eDecisionUndecided, eDecisionAccepted, eDecisionDenied };

  // member functions
  public:

    /// constructor
    FindingInfo();

    /// copy constructor
    FindingInfo( const FindingInfo & other );

    /// assignment operator
    FindingInfo & operator =( const FindingInfo & other );

    /// get the decision on this finding
    const DecisionEnum & GetDecision() const { return m_eDecision; }

    /// set the decision on this finding
    void SetDecision( const DecisionEnum  & eDecision ) { m_eDecision = eDecision; }

    /// get the location along centerline
    const int4 GetSkeletonIndex() const { return m_iSkeletonIndex; }

    /// set the location along centerline
    void SetSkeletonIndex( const int4 iSkeletonIndex ) { m_iSkeletonIndex = iSkeletonIndex; }

    /// get if the finding is within the lumen
    const bool & GetInLumen() const { return m_bInLumen; }

    /// set if the finding is within the lumen
    void SetInLumen( const bool & bInLumen ) { m_bInLumen = bInLumen; }

    /// The Skeleton index value is confuse since it sometimes represents Joined Skeleton, sometimes
    /// represent the current segment. We had better separate them for clarification. That is why we need the following
    /// Get the location along the joint skeleton centerline
    const int4 GetJoinedSkeletonIndex() const { return m_iJoinedSkeletonIndex; }

    /// set the location along the joint skeleton centerline
    void SetJoinedSkeletonIndex( const int4 iJoinedSkeletonIndex ) { m_iJoinedSkeletonIndex = iJoinedSkeletonIndex; }


  // data members
  private:

    /// the user's decision on whether to show or hide this finding
    DecisionEnum m_eDecision;

    /// the nearest skeleton index
    int4 m_iSkeletonIndex;

    /// if the finding is 'within' the lumen
    bool m_bInLumen;

    /// the nearest joined skeleton index
    int4 m_iJoinedSkeletonIndex;

  }; // class FindingInfo

// member functions
public:

  /// constructor
  ColonSession();

  /// reset function
  void Reset();

  /// input from file
  bool Read(const std::string& sFileTitle, ReaderLib::Dataset& inDataset);

  /// output to file
  bool Write(const std::string& sFileTitle, const ReaderLib::Dataset& outDataset) const;

  /// determine if a CAD finding is within (or nearly within) the colon lumen (TODO - move to a utilities class)
  static bool IsFindingNearLumen( const vxCAD::vxCadFinding & finding, ReaderLib::Dataset & dataset );

  /// returns version as a numerical value
  inline float4 GetVersion() const { return m_sVersion.empty() ? 1.0F : atof(m_sVersion.c_str()); }

  /// rotates the prone study into the supine position, for pre-3.0 studies
  bool RotateProneToSupine( const Triple<float4> & worldDim );

private:

  /// input from xml file
  bool ReadXML(const std::string& sFileTitle, ReaderLib::Dataset& inDataset);

  /// output to xml file
  bool WriteXML(const std::string& sFileTitle, const ReaderLib::Dataset& outDataset) const;

// data members
public:

  /// list of bookmarks
  ReaderLib::BookmarkList m_bookmarkList;

  /// 2D Annotations: For each Bookmark: m_name is annotation text, m_position is location,
  //   m_orientation.m_x is slice orientation enum, m_bVisible indicates whether to show text

  /// List of 2D-annotations
  ReaderLib::BookmarkList m_2DAnnotationList;

  /// user-defined registration points
  std::vector<uint4> m_vUserFeatures;

  /// the list of segments
  SegmentList::SegmentOrderVector m_vSegmentOrder;

  /// for the display of measurement lines in this view
  MeasureLineList m_measureLineList;

  /// current world camera coordinates
  Camera m_worldCamera;

  /// additional information for each CAD finding
  typedef std::map< std::string, FindingInfo > FindingInfoMap;
  FindingInfoMap m_mapCadFindingInfo;

	/// status flag
	bool m_bIsReadOnly;

private:

  /// version information
  std::string m_sVersion;

}; // class ColonSession


// $Log: ColonSession.h,v $
// Revision 1.13.2.3  2010/01/25 22:24:32  dongqing
// add JoinedSkeletonIndex for correctly display cad mark in UDS view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.2  2008/11/12 00:09:31  jmeade
// Issue 6128: update old prone session data for new volume orientation.
//
// Revision 1.13.2.1  2007/06/05 23:36:16  jmeade
// don't output session file or marking volume for studies older than session version 3.0.
//
// Revision 1.13  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.12  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.11  2007/01/11 02:36:41  jmeade
// static method on helper method.
//
// Revision 1.10  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.9  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.8  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.7.2.6  2006/03/29 14:17:16  frank
// name change for finding info property
//
// Revision 1.7.2.5  2006/03/28 20:49:03  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.7.2.4  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.7.2.3  2006/03/22 20:52:26  frank
// moved the finding visibility and other information into a class
//
// Revision 1.7.2.2  2006/01/07 04:33:23  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.7.2.1  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.7  2005/10/06 15:59:41  jmeade
// output cad session data.
//
// Revision 1.6  2005/10/04 18:04:29  jmeade
// display cad findings study-wide global variable.
//
// Revision 1.5  2005/09/27 21:41:47  jmeade
// show/hide all findings; hide on 1st open.
//
// Revision 1.4  2005/09/27 01:07:58  jmeade
// enums for cad display constants.
//
// Revision 1.3  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.2  2005/09/21 17:16:55  jmeade
// read, display cad findings list info.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.4.2.1  2003/03/07 01:31:24  jmeade
// Comments, removed empty destructor.
//
// Revision 1.4  2003/01/27 21:43:13  jmeade
// XML Session data file.
//
// Revision 1.3  2002/10/31 20:05:18  kevin
// Changed user feature points to make them more robust and sorted
// so that they cannot become invalid (e.g. crossing user features)
//
// Revision 1.2  2002/09/27 19:06:14  jmeade
// Save world location in session; File header and version.
//
// Revision 1.1  2002/09/13 17:30:36  jmeade
// ColonSession class for appropriate data.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/ColonSession.h,v 1.13.2.3 2010/01/25 22:24:32 dongqing Exp $
// $Id: ColonSession.h,v 1.13.2.3 2010/01/25 22:24:32 dongqing Exp $
