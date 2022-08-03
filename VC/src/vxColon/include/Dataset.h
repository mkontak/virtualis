// $Id: Dataset.h,v 1.19.2.15 2010/11/15 20:37:39 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.net)


/**
 * This class encapsulates the data required for a dataset.
 */


// pragmas
#pragma once


// includes
#include <map>
#include <string>
#include "StudyLoad.h"
#include "VolumeRender.h"
#include "TransferFunctionVC.h"
#include "Submarine.h"
#include "VolumeRender.h"
#include "ColonSession.h"
#include "TrackBall.h"
#include "SegmentList.h"
#include "Patch.h"
#include "Typedef.h"
#include "SkeletonVCExt.h"
#include "vxCadFindings.h"
#include "vxCadFinding.h"
#include "vxComponentArray.h"
#include "SeriesVersion.h"
#include "ObservableObject.h"

class ReaderLib::Dataset;

namespace vxCAD
{

  struct vxCadFindingsArgs
  {
    enum vxCadFindingsAction
    {
      CAD_ACTION_UPDATED,
      CAD_ACTION_SELECTED,
      CAD_ACTION_VISIBILITY,
      CAD_ACTION_REFRESH,
    };

    vxCadFindingsArgs( vxCadFindingsAction eAction, ReaderLib::Dataset * pDataset ) : m_eAction( eAction ), m_pDataset( pDataset ) {}

    vxCadFindingsAction m_eAction;

    ReaderLib::Dataset * m_pDataset;

  };
}


// namespaces
namespace ReaderLib
{

  // user-created findings
  class SuspiciousRegions : public vxCAD::vxCadFindings, public ObservableObject< vxCAD::vxCadFindingsArgs >
  {
    public:

      /// default constructor
      SuspiciousRegions() { }

      /// copy constructor
      SuspiciousRegions(vxCAD::vxCadFindings& rList) : vxCAD::vxCadFindings(rList) { }

      /// removes a finding
      void RemoveFinding( Dataset & rDataset, const int4 iIndex );

      /// clear all findings
      void ClearFindings( Dataset & rDataset )
      {
        for (int4 i(GetCount()-1); i >= 0; i--)
          RemoveFinding(rDataset, i);
      } // ClearFindings

    private:
      /// restrict use of this version because the dataset is needed in order to clear finding from label/paint volume
      void RemoveFinding( const int4 iIndex ) { }

      /// clear all findings
      void ClearFindings() { }
  }; // SuspiciousRegions

  // StudyInfo
  class StudyInfo
  {
    public:
      /// returns a line-feed delimited display string containing all info
      const std::string Format() const
      {
        return
          m_sPatientName + "\n" +
          m_sPhysicianName + "\n" +
          m_sExamSite + "\n" +
          m_sPatientID + ", " + m_sPatientData + ", " + m_sExamTime;
      } // Format()

    public:
      /// patient name
      std::string m_sPatientName;
      /// referring physician
      std::string m_sPhysicianName;
      /// age/gender
      std::string m_sPatientData;
      /// patient ID
      std::string m_sPatientID;
      /// exam date string
      std::string m_sExamTime;
      /// exam date
      DATE m_dateExam;
      /// exam location
      std::string m_sExamSite;
  }; // StudyInfo

  // Dataset
  class Dataset
  {
    // enums
    public:
      enum StudyLoadStage
      {
        eLoadHeader = 0,
        ePreAllocLinearVols = 1,
        eLoadOverview = 2,
        eLoadVolumeFields = 3,
        eLoadThresholds = 4,
        eLoadNavigationFields = 5,
        eLoadRenderFields = 6,
        ePostLoadInit = 7,
        eLoadStageCount = 8,
      };

      /// Data modality
      enum DataModality { CT, MR, PET, US };

    // Member Functions
    public:

      /// Constructor.
      Dataset();

      /// Destructor.
      virtual ~Dataset();

      /// Is this the same dataset (based on internal m_uUniqueID field)
      inline bool IsEqualID(const Dataset & rRight) const { return this->m_uUniqueID == rRight.m_uUniqueID; }
      /// Returns the unique internal ID
      inline uint4 GetUniqueID() const { return m_uUniqueID; }

      /// gets the volume prefix
      const std::string & GetVolumePrefix() const { return m_sVolumePrefix; }

      /// sets the volume prefix
      void SetVolumePrefix( const std::string & sVolumePrefix ){ m_sVolumePrefix = sVolumePrefix; }

      /// gets the preprocessed prefix
      const std::string & GetPreprocessedPrefix() const { return m_sPreprocessedPrefix; }

      /// sets the preprocessed prefix
      void SetPreprocessedPrefix( const std::string & sPreprocessedPrefix ){ m_sPreprocessedPrefix = sPreprocessedPrefix; }
      
      /// gets the cad prefix
      const std::string & GetCadPrefix() const { return m_sCadPrefix; }

      /// sets the cad prefix
      void SetCadPrefix( const std::string & sCadPrefix ){ m_sCadPrefix = sCadPrefix; }

      /// gets the session prefix
      const std::string & GetSessionPrefix() const { return m_sSessionPrefix; }

      /// sets the session prefix
      void SetSessionPrefix( const std::string & sSessionPrefix ){ m_sSessionPrefix = sSessionPrefix; }

      /// gets the uid
      inline const std::string & GetUID() const { return m_sUID; }

      /// sets the uid
      inline void SetUID( const std::string & sUID ){ m_sUID = sUID; }

      /// gets the session uid
      inline const std::string & GetSessionUID() const { return m_sSessionUID; }

      /// sets the session uid
      inline void SetSessionUID( const std::string & sSessionUID ){ m_sSessionUID = sSessionUID; }

      /// Returns the number of steps for a dataset load
      static inline uint4 GetLoadStepsCount() { return(uint4(eLoadStageCount)); }

      /// Load Stages one at a time (this allows the dovetailing of loading the two studies)
      void LoadStep2(const uint4 uStage, StudyLoad::LoadCallback & LoadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetupZoomFactor);

      /// Load Stages one at a time with vxBlockVolumeFileReader (this allows the dovetailing of loading the two studies)
      void LoadStep3(const uint4 uStage, StudyLoad::LoadCallback & LoadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetupZoomFactor);

      /// Load Stages one at a time with 64 bit pipeline output
      void LoadStep64b(const uint4 uStage, StudyLoad::LoadCallback & LoadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetupZoomFactor);

			/// The following code block is for PNVC loading.
			/// execute the entire vc pipeline to build the 3D model
      //void LoadVolumeOnly( vxBlockVolume<uint2> * pv, vxViewerPipeline & pipeline, StudyLoad::LoadCallback & LoadCallback, bool & bSetupZoomFactor );

			/// execute the entire vc pipeline to build the 3D model
      //void Preprocessing ( vxBlockVolume<uint2> * pv, vxViewerPipeline & pipeline, StudyLoad::LoadCallback & LoadCallback, std::string & series );

      /// Reset all data members.
      void Reset();
      
      /// FreeAllLargeVolumesButPaint.
      void FreeAllLargeVolumesButPaint();
      
      /// Perform initialization: after (1) volumes are loaded and (2) verified, this
      /// means that all the data is guaranteed to be here and the user has chosen the
      /// segments and order for the fly-through.
      bool InitializeVisualization();

      /// Based on user segment selections, updates segment length and performs some registration initialization
      void UpdateSegmentOrder();
        
      /// Jumps the view position to the start or end of the current segment centerline.
      bool MoveViewpointToSegmentEnd(const bool bStartOrEnd);
      
      /// Jumps the viewpoint to the first segment.
      void GoToFirstSegment();
      
      /// Jumps the viewpoint to the last segment.
      void GoToLastSegment();
      
      /// Jumps the viewpoint one segment toward the cecum.
      void GoToNextSegmentToCecum();
      
      /// Jumps the viewpoint one segment toward the rectum.
      void GoToNextSegmentToRectum();

      /// Gets the sorted segment index segment corresponding to a particular point in space.
      int4 GetSortedSegmentIndex(const Point<float4> & vPositionWorld);
      int4 GetSortedSegmentIndex(const Point<uint2> & positionVol);

      /// Select the segment corresponding to a particular point in space.
      void SelectSegment(const Point<float4> & vPositionWorld);

      /// Retrieves the colon segment enumeration from the given offset.
      ColonSegmentEnum GetColonSegmentFromOffset(const float4 fOffsetMM) const;
      
      /// Retrieves the current colon segment enumeration.
      ColonSegmentEnum GetCurrentColonSegment();
      
      /// Smooth a skeleton but constrain it to stay within the segment.
      void ConstrainedSkeletonSmooth(Skeleton & skeleton, const float4 fRadius);
      
      /// Nibble off the ends of the centerline that are in a narrow opening.
      void NibbleCenterlineEnds(Skeleton & skeleton);
      
      /// Returns the number of mm we are from the rectum.
      float4 GetCurrentDistanceFromRectumMM();
      
      /// Returns the number of mm this is from the rectum.
      float4 GetDistanceFromRectumMM( Segment & segment, Skeleton & skeleton, const Point3D<float4> & position );
      
      /// Write all dataset session data to disk.
      bool SaveSessionData(const bool bSavePaintVol = true);

      /// Prepare the percent visualized data structure.
      void InitializeMarkedSurfaces();

      /// Prepare CAD findings for visualization 
      void InitializeCadFindings();

      /// initialize SRT ROI 
      void InitializeSRTroi();

      /// Calculate the percent of the colon which has been examined.
      float4 GetPercentExamined() const;

      /// For TTP rendering
      void BuildOpacityVol();

      /// For TTP rendering
      void BuildHomogenityVolume();
     
      /// Orientation query method.
      DatasetOrientations::TypeEnum GetOrientation() const { return m_eOrientation; }

      /// Dicom patient orientation value
      SerializedDicom::PatientOrientation GetPatientOrientation() const;
      
      /// Sets the study orientation.
      void SetOrientation(const DatasetOrientations::TypeEnum eOrientation) { m_eOrientation = eOrientation; }

      /// Smooths out the centerlines by box filtering.
      void SmoothCenterlines();

      /// Data ready query methods.
      const bool IsLoadingComplete() const { return m_bVolumeLoaded; }

      /// Data ready query methods.
      const bool IsDistanceValid() const { return m_bDistanceValid; }

      /// Calculate Cross-Section Plane.
      void CrossSectionCalc();

      /// Set cross-sectional distance.
      void SetCrossSecDistance(const float4 fCrossSecDist) { m_fCrossSecDistance = fCrossSecDist; }

      float4 GetCrossSecDistance() const { return m_fCrossSecDistance; }

      /// match the study orientation to tracball orientation.
      inline TrackBall::StudyOrientationEnum MatchOrientation(const DatasetOrientations::TypeEnum eStudyOrientation) const;

      /// Extracts the registration input data.
      //void GoThroughSegments();

      /// The joined skeleton 
      void CreateJoinedSkeleton();

      /// The flipped skeleton for Supine/Prone Registration NOTE: THIS IS ONLY FILLED IN FOR THE PRONE STUDY
      void CreateFlippedSkeleton();

      /// Locates the features for registration, which are the exactly points where the colon bends.
      void LocateFeatures();

      /// If there are features too close to each other, we have to average them.
      void AverageFeatures();

      /// The list of straight segments along the colon
      void CreateSkeletonSegments();

      void PrintFeatures();

      /// updates the biopsy colormap in the vxSlcieRendererData class
      void UpdateBiopsyColormap();

      /// returns the dataset modality
      inline const Dataset::DataModality GetModality() const { return m_eDataModality; }

      /// is the joined skeleton computed?
      bool IsJoined() const;

      /// is the CAD findings file loaded
      const bool & IsCadFindingsLoaded() const { return m_bCadFindingsLoaded; }

      /// retrieve the world position of the nearest skeleton to the finding
      Point3D<float4> GetNearestSkeletonPosition( const vxCAD::vxCadFinding & finding );

      /// retrieve the DFS value of the nearest skeleton to the finding
      float4 GetNearestSkeletonDFS( const vxCAD::vxCadFinding & finding );

      /// gets the component array
      vxComponentArray & GetComponentArray();

      /// gets the version of the preprocessed series
      SeriesVersion GetSeriesVersion() { return m_seriesVersion; }

      /// sets the version of the preprocessed series
      void SetSeriesVersion( SeriesVersion seriesVersion ) { m_seriesVersion = seriesVersion; }

      /// determine if a point is within (or nearly within) the selected segments of the colon lumen
      bool IsPointNearSelectedLumen( const Point3Df & pt );

      /// returns true if the dataset is an old prone that needs to be rotated into the supine position
      bool IsDatasetRotatedProne() const { return m_bRotateProneToSupine; }

			/// compute volume for all segments
      void ComputeVolume4Segments();


  // static methods
      /// finding comparison function that sorts by visibility
      static bool CALLBACK SortByVisibility( vxCAD::vxCadFinding & x, vxCAD::vxCadFinding & y );

      /// finding comparison function that sorts by skeleton index
      static bool CALLBACK SortBySkeletonIndex( vxCAD::vxCadFinding & x, vxCAD::vxCadFinding & y );

      /// returns the dataset that an in-operation sort routine is processing
      inline static Dataset * GetCurrentSortDataset() { return m_pCurrentSortDataset; };

    private:

      /// Line Approximation routine
      void RecursiveDouglasPeucker(const uint4 uStartIndex, const uint4 uStopIndex);

      /// Copy constructor - not implemented
      Dataset( const Dataset & ) : m_uUniqueID(uGlobalUniqueID++) {}

      /// Assignment operator - not implemented
      Dataset & operator =( const Dataset & ) {}

      /// extract the associated skeleton indexes for a cad finding
      bool ExtractSkeletonIndexesForFinding( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint1> & iti, const float4 & fMaximumColonDiameter );


    // Data Members
    public:

      /// what am I
      static uint4 vLoadStage1GB[eLoadStageCount];

      /// what am I
      static uint4 vLoadStage2GB[eLoadStageCount];

      /// The value which separates the colon wall from the lumen.
      float4 m_fColonWallThreshold;

      /// The color and opacity of the rendered volume.
      TransferFunctionVC m_surfaceTransferFunction;

      /// The color and opacity of the rendered volume.
      TransferFunctionVC m_biopsyTransferFunction;

      /// The color and opacity of the rendered volume.
      TransferFunctionVC m_TTPTransferFunction;

      /// The volume sizes.
      Triple<float4> m_vUnits;

      /// The volume sizes.
      Triple<uint4> m_vDim;

      /// The user's point of view.
      Submarine m_submarine;

      /// Current colon dataset session data
      ColonSession m_session;

      /// The list of segments.
      SegmentList m_segmentList;

      /// The joined skeleton (for segmental view and supine/prone registration, someday we should use this for everything. At the moment there is too much dependance on "segmetns" when jumping around to change it))
      SkeletonVCExt m_joinedSkeleton;

      /// The flipped skeleton for Supine/Prone Registration NOTE: THIS IS ONLY FILLED IN FOR THE PRONE STUDY
      SkeletonVCExt m_joinedFlippedSkeleton;

      /// Index into joined skeleton of detected features (bends) along the centerline
      std::vector<int4> m_ivNominatedFeatures;  
      
      /// Index into joined skeleton of detected features (bends) along the centerline
      std::vector<int4> m_ivFeatures;

      // feature which the info is going to be displayed
      int4 m_iFeatureToDisplayInfo;
      
      /// The list of straight segments along the colon
      std::vector<uint2> m_straightSegmentNodes;

      /// Which Segment to Display in Segmental view
      uint4 m_currentSegment;

      /// Error threshold for creating straight segments
      float4 m_fSegmentErrorThreshold;

      /// The various fields necessary for navigation and rendering.
      Fields m_fields;

      /// The current orthogonal slice image location set by slice viewers.
      Point<float4> m_vOrthogonalPtWorld;
      Point<float4> m_vLastUpdatedOrthoPtWorld;

      /// Volume header information.
      vxVolumeHeader m_volumeHeader;

      /// List of surface areas missed (no viewed) during current examination.
      std::vector<Patch> m_vPatchList;

      /// Whether visualization needs to be reinitialized.
      bool m_bNeedVisualizationReinit;

      /// The distance from the eyepoint of the MPR slice.
      Point<float4>  m_vObliqueLocation;
      float4         m_vObliqueDistanceFromRectumMM;
      Normal<float4> m_vObliqueOrientation;
      Normal<float4> m_vObliqueUp;
      Point<float4>  m_vObliqueBottomLeft, m_vObliqueBottomRight, m_vObliqueTopLeft, m_vObliqueTopRight;
      uint4 m_uObliqueViewIdentifier;
      uint2 m_uCrossSecIndex;
      int4 m_iXsectionScrollBarLocationIndex;
      
      /// Local trackball that controls overview. Pointer to current trackball.
      TrackBall * m_pTrackball;

      /// trackball that controls verify.
      TrackBall m_trackballVerify;
      
      /// trackball that controls StandandView & S&PView.
      TrackBall m_trackballStandard; 

      /// set this variable to keep the current value of csi.
      /// (use this variable in case of un-continuous jump of skeleton node: see bug 567)
      uint2 m_uCsi;

      /// how much of this study has been loaded
      uint4 m_uLoadPercent;

      /// indicates of the user is interacting with the dataset
      bool m_bUserInteracting;

      /// CAD Findings
      Observable< vxCAD::vxCadFindings, vxCAD::vxCadFindingsArgs > m_cadFindings;

      /// if the CAD findings file was loaded
      bool m_bCadFindingsLoaded;

      /// suspicious regions
      SuspiciousRegions m_suspiciousRegions;

      /// information on the study for this dataset
      StudyInfo m_studyinfo; // ---- IMPORTANT: For the time being, the members of this instance are set IN THE COLON VIEWER PROJECT, not here in ReaderLib.

    private:

      /// Marks the study as supine or prone.
      DatasetOrientations::TypeEnum m_eOrientation;

      /// Data modality
      Dataset::DataModality m_eDataModality;

      /// Whether or not the volume is loaded.
      bool m_bVolumeLoaded;

      /// Whether or not the centerline length is valid yet.
      bool m_bDistanceValid;

      /// cross-section distance.
      float4 m_fCrossSecDistance;
      Point<float> m_obliqueBottomLeft, m_obliqueBottomRight, m_obliqueTopLeft, m_obliqueTopRight;
      unsigned short m_crossSecIndex;
      Point<float> m_obliqueLocation;
      Normal<float> m_nObliqueOrientation;
      Normal<float> m_nObliqueUp;

      /// prefix used to form all volume related filenames
      std::string m_sVolumePrefix;

      /// prefix used to form all preprocessed related filenames
      std::string m_sPreprocessedPrefix;

      /// prefix used to form all cad related filenames
      std::string m_sCadPrefix;

      /// prefix used to form all session related filenames
      std::string m_sSessionPrefix;

      /// uid
      std::string m_sUID;

      /// uid
      std::string m_sSessionUID;

      /// the component array
      vxComponentArray m_componentArray;

      /// the version of the preprocessed series
      SeriesVersion m_seriesVersion;

      /// if we should rotate this prone dataset to supine
      bool m_bRotateProneToSupine;

      /// Unique ID
      const uint4 m_uUniqueID;

      /// current dataset for (global) sort routine callback
      static Dataset * m_pCurrentSortDataset;

      /// Unique ID
      static uint4 uGlobalUniqueID;

  }; // class Dataset


  /**
   * match the dataset orientation to trackball orientation.
   *
   * @param eStudyOrientation   dataset orientation to map
   * @return                    corresponding trackball orientation
   */
  inline TrackBall::StudyOrientationEnum Dataset::MatchOrientation(const DatasetOrientations::TypeEnum eStudyOrientation) const
  {
    switch (eStudyOrientation)
    {
    case DatasetOrientations::eDEFAULT: return TrackBall::DEFAULT;
    case DatasetOrientations::eSUPINE:  return TrackBall::SUPINE;
    case DatasetOrientations::ePRONE:   return TrackBall::PRONE;
    case DatasetOrientations::ePRIMARY: return TrackBall::PRIMARY;
    case DatasetOrientations::eSECONDARY: return TrackBall::SECONDARY;
    case DatasetOrientations::eSWAP:    return TrackBall::SWAP;
    default:  return TrackBall::NON_ROTATE;
    }
    return TrackBall::NON_ROTATE;
  } // MatchOrientation()

} // namespace ReaderLib


// $Log: Dataset.h,v $
// Revision 1.19.2.15  2010/11/15 20:37:39  dongqing
// add codes for measuring air lumen volume of selected segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.14  2010/04/05 20:11:23  dongqing
// Fix the bug that the Display Extracolonic CAD Finding toggle shown
// incorrect association between finding and selected colon segments.
// Note, the CAD Pane should always display the total number from the CAD
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.13  2010/03/05 08:59:58  dongqing
// separate studyinfo field for exam date (date structure)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.12  2010/02/01 19:23:42  dongqing
// study info into the dataset class
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.11  2010/01/25 22:24:33  dongqing
// add JoinedSkeletonIndex for correctly display cad mark in UDS view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.10  2009/11/16 23:09:09  dongqing
// Issue Cad09-02: giving overview cad marks colours to indicate visibility setting.
//
// Revision 1.19.2.9  2009/01/23 19:21:53  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.19.2.8  2009/01/21 06:02:48  jmeade
// comment
//
// Revision 1.19.2.7  2009/01/15 04:15:11  jmeade
// GetSortedSegmentIndex convenience method
//
// Revision 1.19.2.6  2008/11/17 17:06:42  jmeade
// give version info to each report entry;
// flip camera coordinates for report entries of old prone datasets.
//
// Revision 1.19.2.5  2008/10/22 02:19:01  jmeade
// improve threshold check for 3D movement to trigger synchronization to 2D slice positions.
//
// Revision 1.19.2.4  2008/10/11 00:15:17  jmeade
// remove unneeded member variable.
//
// Revision 1.19.2.3  2008/10/06 19:57:44  dongqing
// add bool flag to indicate if user interacts with 2D x-sectional or 3D endo view
// This will tell the SliceViewer selecting proper oblique 2D view renderer setting.
//
// Revision 1.19.2.2  2007/09/20 23:08:48  jmeade
// spelling in comment.
//
// Revision 1.19.2.1  2007/03/22 20:41:52  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.19  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.18  2007/03/05 23:18:06  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.17  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.16  2007/01/23 20:18:31  dongqing
// add functionto handle the SRT session save and load
//
// Revision 1.15  2007/01/09 19:15:10  jmeade
// method to determine if a point in/near lumen.
//
// Revision 1.14  2006/11/10 00:19:22  jmeade
// Issue 5097: Ensure cad list sorting will use intended dataset.
//
// Revision 1.13  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.12  2006/09/14 17:45:49  dongqing
// add m_suspiciousRegions
//
// Revision 1.11  2006/09/13 15:15:48  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.10  2006/08/25 21:18:59  jmeade
// Issue 4946: serialized dicom patient orientation info.
//
// Revision 1.9  2006/06/27 17:11:14  jmeade
// Read patient position from volume header.
//
// Revision 1.8  2006/06/26 13:03:08  frank
// creates a component array that matches the selected segments
//
// Revision 1.7  2006/06/22 14:01:24  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.6  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.5  2006/02/07 21:41:02  dongqing
// code walk through
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.2.2.4  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.2.2.3.2.1  2007/02/01 20:03:50  jmeade
// Issue 4933: use correct dataset for finding sort.
//
// Revision 1.2.2.3  2006/03/29 20:52:23  frank
// read the segment number from the cad findings file
//
// Revision 1.2.2.2  2006/03/22 20:53:10  frank
// moved cad finding sorting functions out of cad finding
//
// Revision 1.2.2.1  2006/01/10 16:46:18  frank
// Fixed display of CAD vendor image when only 1 of 2 cad files exists
//
// Revision 1.2  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.1  2005/09/28 20:24:11  vxconfig
// renamed study files to dataset
//
// Revision 1.4  2005/09/26 14:11:21  vxconfig
// added function to determine visibility of a cad finding
//
// Revision 1.3  2005/09/23 12:02:28  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.2  2005/09/22 16:59:43  jmeade
// store cad finding locations along centerline.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.27.2.4.2.5.2.4  2005/07/13 17:22:23  geconomos
// Added a method to indicate if the joined skeleton has been computed
//
// Revision 3.27.2.4.2.5.2.3  2005/07/08 18:54:44  jmeade
// privatize dataset modality variable.
//
// Revision 3.27.2.4.2.5.2.2  2005/06/30 14:53:09  geconomos
// Issue #4229: Changed slice number on oblique views to show cm from anal verge rather than 0->100 or skeleton node indices
//
// Revision 3.27.2.4.2.5.2.1  2005/06/27 17:36:38  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.27.2.4.2.5  2005/06/14 19:13:26  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.27.2.4.2.4  2005/03/24 16:23:00  frank
// removed Set Volume Loaded function - it should not be publically exposed
//
// Revision 3.27.2.4.2.3  2001/01/02 11:17:23  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 3.27.2.4.2.2  2001/01/02 09:24:31  jmeade
// Create separate stage for loading volume fields (w/ 2d slice loading).
//
// Revision 3.27.2.4.2.1  2005/03/03 19:22:39  jmeade
// Issue 4003: Removed unnecessary current patch placeholder.
//
// Revision 3.27.2.3.2.2  2005/02/04 23:57:09  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.27.2.3.2.1  2004/11/16 22:36:07  jmeade
// Identify dataset modality.
//
// Revision 3.27.2.3  2004/05/19 13:07:51  geconomos
// Added a methods to update the colormap in vxSliceRendererData from the biopsy transfer function.
//
// Revision 3.27.2.2  2003/02/27 02:20:44  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.27.2.1  2003/02/12 23:22:47  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.28  2003/02/10 18:49:46  kevin
// Registration Debug Info
//
// Revision 3.27  2002/11/24 17:46:38  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.26  2002/11/21 23:55:04  jmeade
// Code standard on m_trackball* variables.
//
// Revision 3.25  2002/10/31 19:59:26  kevin
// 1) Free up memroy on close to allow paint vol to be written to linear-vol file
// 2) Add load step so post-load init is always at the end even for both memory configurations
//
// Revision 3.24  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.23  2002/09/29 21:44:15  kevin
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
// Revision 3.22  2002/09/27 19:04:51  jmeade
// Save/set saved current world location; Function returning static member data should be static; Removed unused function/variable.
//
// Revision 3.21  2002/09/13 17:22:45  jmeade
// Function name change; Paint volume handles all volume and value serialization.
//
// Revision 3.20  2002/09/13 00:51:15  jmeade
// ColonSession class for appropriate data; Save to disk/reset paint volume data as appropriate.
//
// Revision 3.19  2002/09/11 19:18:33  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.18  2002/09/02 20:38:14  kevin
// Render a specific sized segment and mark which part the user should focus on
//
// Revision 3.17  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.16  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.15  2002/07/18 19:56:49  jmeade
// Segment list identifier should be with SegmentList class.
//
// Revision 3.14  2002/07/11 23:25:52  jmeade
// Implementing user-defined registration point specification.
//
// Revision 3.13  2002/07/03 03:23:41  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.12  2002/06/26 17:49:55  jmeade
// Moving more hard-coded strings to the string table; Code standards.
//
// Revision 3.11  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.10  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.9  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.8  2002/03/13 21:56:40  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7  2002/02/05 22:13:36  binli
// Proper setting of default zoom factor in trackball according to the volume size.
//
// Revision 3.6.2.6  2002/07/01 19:21:38  jmeade
// const specifier for query function.
//
// Revision 3.6.2.5  2002/06/06 01:36:36  kevin
// Added check for 2GB of memeory. If this is present we can load the linear volumes
// first since they will nto swap out. This helps with Address space fragmentation
// since the large temporary "uncompres" buffers are allocated earlier. This is
// required now that we use a graphics card which takes 128MB of address space
// for it's on baord texture memory that is mapped into the processes address sapce.
//
// Revision 3.6.2.4  2002/05/23 16:00:56  jmeade
// Issue 2270:  List of 2D annotations.
//
// Revision 3.6.2.3  2002/02/27 15:28:41  binli
// issue 1699: refixed (since the change in Revision 3.6.2.2)
//
// Revision 3.6.2.2  2002/02/25 16:04:32  kevin
// Dramatically changed the patient loading mechanism. This is required to fix
// memory erros both in physicial memory and in adress space fragmentation.
// Moved the control of the loading order to Study class. Fields now only
// handles mutiple volumes in one load. ReaderGlobal now only calls Study
// in a loop which allows Supine and Prone loads to be interleaved (the
// part in Readglobal was simplified alot and the logic placed in study so that
// all the logic is in one place in case we want to play with the order of
// loading in the future)
//
// Also added pre-allocation of all the linear volumes before address is fragmented.
//
// Also added timing info into the LogFyi log file so that we can track the time it
// takes to laod even in the field.
//
// Revision 3.6.2.1  2002/02/11 15:14:37  binli
// issue 1699 in branch: setting default zoom factor setting in trackball according to volume size
//
// Revision 3.6  2002/01/03 02:25:33  kevin
// Removed ses, set,sec and added build opacity volume
//
// Revision 3.5  2001/12/29 00:13:22  jmeade
// Merge from 1.1 branch.
//
// Revision 3.4  2001/11/08 20:43:11  binli
// 2D flight
//
// Revision 3.3  2001/11/06 15:25:23  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.2  2001/11/05 18:48:34  ana
// GoThroughSegments moved to Study.
//
// Revision 3.1  2001/11/05 17:52:18  ana
// Registration view added.
//
// Revision 3.0.2.1  2001/11/15 19:51:47  jmeade
// InitializeVisualization() returns false when total segment length less than 100 cm.
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:18   ingmar
// Initial revision.
// Revision 1.58  2001/08/21 20:23:47  binli
// bug 612: fixed
//
// Revision 1.57  2001/08/15 15:39:14  jmeade
// Bug 652:  Clear list of missed patches when initializing study visualization
//
// Revision 1.56  2001/08/09 22:27:36  binli
// bug 000567: make sure no out of bound
//
// Revision 1.55  2001/07/30 16:30:36  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.54  2001/06/21 21:15:19  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.53  2001/05/24 20:40:40  binli
// reflected movement of trackball class from readerlib to vxBase/9_render
//
// Revision 1.52  2001/05/19 00:41:05  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.51  2001/05/17 20:42:20  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.50  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.49  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.48  2001/04/30 12:30:30  frank
// Added exception handling.
//
// Revision 1.46  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.45  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.44  2001/03/19 15:28:26  frank
// Continued to update to reflect new coding standards.
//
// Revision 1.43  2001/03/19 11:58:51  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.42  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.41  2001/03/09 18:16:15  jmeade
// IsDistanceValid() access function
//
// Revision 1.40  2001/03/09 01:07:08  jmeade
// protect access to some more Study members
//
// Revision 1.39  2001/03/01 01:58:28  jmeade
// m_bNeedVisualizationReinit member variable; set true in volume load (LoadStep...),
// set false in MarkSurfaces()
//
// Revision 1.38  2001/02/27 23:43:16  jmeade
// hid m_eOrientation for safekeeping (more hiding to come)
//
// Revision 1.37  2001/02/27 17:59:17  jmeade
// Missed regions list (m_patchList) a member of Study class
//
// Revision 1.36  2001/02/13 12:57:13  frank
// Added a function to sample the original volume data instead of the e-cleansed data.
//
// Revision 1.35  2001/02/06 14:13:48  frank
// Revamped study load progress display.
//
// Revision 1.34  2001/01/29 22:15:50  liwei
// Attempted to move the opacity volume to study or fields, not finished yet
//
// Revision 1.33  2001/01/29 18:16:46  frank
// Removed error suppression.
//
// Revision 1.32  2001/01/25 17:28:57  frank
// Removed old functions.
//
// Revision 1.31  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.30  2001/01/24 16:41:41  frank
// Upgraded study load functions for supine/prone.
//
// Revision 1.29  2001/01/24 15:55:20  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.28  2001/01/11 13:08:45  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.27  2001/01/09 15:20:48  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.26  2001/01/08 18:42:59  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.25  2001/01/05 19:27:39  frank
// Moved marking functions into Study class.
//
// Revision 1.24  2001/01/05 14:59:10  frank
// Coding guidelines.
//
// Revision 1.23  2001/01/03 16:30:48  binli
// removed rotate marix (replaced it with 3_math::matrix)
//
// Revision 1.22  2000/12/27 20:22:08  binli
// zooming (6):make sure the arrow is visible  when double click in Overview.
//
// Revision 1.21  2000/12/12 18:01:14  frank
// Removed spurious warning upon loading secondary volume header
//
// Revision 1.20  2000/12/09 15:39:18  frank
// Removed spurious warnings
//
// Revision 1.19  2000/11/22 16:55:50  frank
// Updated progress meter with all data fields
//
// Revision 1.18  2000/11/21 19:57:09  frank
// Cleaned up code
//
// Revision 1.17  2000/11/20 19:05:04  kevin
// Fixed the lnegth display
//
// Revision 1.16  2000/11/17 18:27:30  frank
// Name change
//
// Revision 1.15  2000/11/16 22:41:33  binli
// Simply registation of Supine  and Prone.
//
// Revision 1.14  2000/11/16 15:22:02  jeff
// LoadCallback class to keep track of study load state information
//
// Revision 1.13  2000/11/15 21:51:56  liwei
// The errors in scaling vli volume corrected
// Axial slices loaded earlier so that vli volume can be created earlier
//
// Revision 1.12  2000/11/15 19:13:45  liwei
// Add features to read transfer funciton from file
//
// Revision 1.11  2000/11/14 23:53:04  liwei
// Add support for VolumePro
//
// Revision 1.10  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.9  2000/11/14 18:31:27  binli
// added * assignment function
//
// Revision 1.8  2000/11/14 16:03:18  binli
// made copy constructor and assignment functions private.
//
// Revision 1.7  2000/11/13 19:15:12  binli
// 1. moved some (Supine/Prone related) stuffs from 'ReaderGlobal' to 'Study'.
// 2. adjusted lock/unlock controls in VerifyView.
//
// Revision 1.6  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.5  2000/11/09 17:41:59  frank
// Added reading for the secondary study
//
// Revision 1.4  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.3  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.2  2000/11/07 15:52:07  frank
// Added copy constructor and assignment operator
//
// Revision 1.1  2000/11/07 14:44:17  frank
// Added Study class
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Dataset.h,v 1.19.2.15 2010/11/15 20:37:39 dongqing Exp $
// $Id: Dataset.h,v 1.19.2.15 2010/11/15 20:37:39 dongqing Exp $
