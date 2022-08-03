// $Id: InteractiveSegmentation.h,v 1.8 2006/03/29 20:03:19 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (mailto:jaddonisio@viatronix.com)

/**
 * Main class that encapsulates interactive segmentation. This class provides
 * methods to get/set segmentation parameters and running various segmentation
 * operations.
 */


// pragmas
#pragma once


// includes
#include "SegmentationParameters.h"
#include "SegmentationVolumes.h"
#include "ComponentHandler.h"
#include "RegionGrow.h"
#include "RegionThresholdSegment.h"
#include "RegionSampler.h"
#include "MorphOp.h"
#include "Box.h"
#include "PixelRGBA.h"


// forward declarations
class ComponentHandler;


// class definition
class VX_VIEWER_DLL InteractiveSegmentation
{
public:

  /// default constructor
  InteractiveSegmentation();

  /// destructor
  virtual ~InteractiveSegmentation() {};

// enums
public:

  enum ModeIDsEnum { 
    MODE_NOT_IMPL = 0, 
    REGION_GROW_MODE, 
    REGION_THRESHOLD_SEGMENT_MODE, 
    REGION_MERGE_MODE, 
    REGION_SPLIT_MODE,
    REGION_REMOVE_MODE, 
    CLASSIFICATION_MODE, 
    MORPH_OP_MODE, 
    REGION_SAMPLE_2D, 
    REGION_SAMPLE_3D, 
    AUTO_CENTERLINE_MODE,
    AUTO_SKELETON_MODE, 
    MANUAL_SKELETON_MODE, 
    MANUAL_SKELETON_BRANCH_MODE, 
    COMPHDLR_UPDATE_MODE }; // ModeIDsEnum

  enum EventsEnum
  {
    EVENT_SEGPARAMS_UPDATE  = 0x00000001,
    EVENT_SEGVOL_UPDATE     = 0x00000002,
    EVENT_INTENSITY_RANGE   = 0x00000004,
  }; // EventsEnum

  enum StatusEnum
  {
    SEGMENTATION_OK,
    FAILED_TO_SEGMENT,
    NO_SELECTED_REGION,
  }; // StatusEnum

// functions
public:

  /// get region grow method selection.
  inline uint4 GetRegionGrowMethod();

  /// set region grow method selection
  inline void SetRegionGrowMethod(SegParams::RegionGrowMethodEnum eVal);

  /// get region grow add option
  inline uint4 GetRegionGrowAddOpt();

  /// set region grow add option
  inline void SetRegionGrowAddOpt(SegParams::RegionAddOptionEnum  eVal);

  /// get region grow iterations
  inline uint4 GetRegionGrowIterations();

  /// set region grow iterations
  inline void SetRegionGrowIterations(uint4 uValue);

  /// get region sample radius
  inline float4 GetRegionSampleRadius();

  /// set region sample radius
  inline void SetRegionSampleRadius(float4 fValue);

  /// get region intensity range, high
  inline int4 GetRegionIntensityHigh();

  /// set region intensity range, high
  inline void SetRegionIntensityHigh(int4 iVal);

  /// get region intensity range, low
  inline int4 GetRegionIntensityLow();

  /// set region intensity range, low
  inline void SetRegionIntensityLow(int4 iVal);

  /// get region class range, high.
  inline uint4 GetRegionClassHigh();

  /// set region class range, high
  inline void SetRegionClassHigh(uint4 uValue);

  /// get region class range, low
  inline uint4 GetRegionClassLow();

  /// get morphological operation iterations.
  inline uint4 GetMorphOpIterations();

  /// set morphological operation iterations
  inline void SetMorphOpIterations(uint4 uValue);

  /// get number of classes.
  inline uint4 GetNumberOfClasses();

  /// set number of classes
  inline void SetNumberOfClasses(uint4 uValue);

  /// get classification voxel type
  inline uint4 GetClassVoxelType();
  
  /// set classification voxel type
  inline void SetClassVoxelType(uint4 uValue);

  /// get class intensity range, high
  inline uint4 GetClassIntensityRangeHigh();

  /// set class intensity range, high
  inline void SetClassIntensityRangeHigh(uint4 uValue);

  /// get class intensity range, low
  inline uint4 GetClassIntensityRangeLow();

  /// set class intensity range, low
  inline void SetClassIntensityRangeLow(uint4 uValue);

  /// get classification window selection
  inline SegParams::ClassWindowSelEnum GetClassWinSel();

  /// get contour adjustment tension
  inline uint4 GetContourAdjTension(uint2 uValue);
  
  /// get pointer to component handler object
  inline ComponentHandler * GetCompHdlrPtr() const { return m_pCompHdlr; }

  /// get threshold display flag
  inline uint4 GetThresholdDisplayFlag() const;

  /// set threshold display flag
  inline void SetThresholdDisplayFlag(const bool bDisplay);

  /// get seed point
  inline const Point<uint2> & GetSeedPoint() const { return m_seedPoint; }

  /// set a seed point
  inline void SetSeedPoint(const Point<uint2> & seedPoint) { m_seedPoint = seedPoint; }

  /// get pointer to delta segmentation tag volume
  inline vxBlockVolume<bool> * GetDeltaSegTagVol() const;

  /// get the segmentation volume object pointer
  inline SegmentationVolumes * GetSegVolsPtr() { return &m_segVols; }

  /// get the region sampler object
  RegionSampler & GetRegionSampler() { return m_regionSampler; }

  /// get status
  inline uint4 GetStatus() const { return m_iStatus; }

  /// tests if segmentation is successful
  inline bool IsSuccessful() const;

  /// set classification window selection.
  inline void SetClassWinSel(SegParams::ClassWindowSelEnum eVal);

  /// set contour adjustment tension.
  inline void SetContourAdjTension(uint4 uValue);

  /// set pointer to component handler object.
  inline void SetCompHdlrPtr(ComponentHandler * pCompHdlr) { m_pCompHdlr = pCompHdlr; }

  /// set a pointer to a scan volume.
  inline void SetScanVolume(vxBlockVolume<uint2> & rVol);

  /// set a pointer to a segmentation volume
  inline void SetSegmentVolume(vxShareableObject< vxBlockVolume<uint2> > * pVol);

  /// reset status
  inline void ResetStatus() { m_iStatus = SEGMENTATION_OK; }

  /// General run method:
  void Run(const ModeIDsEnum & eModeID, const int4 & iOption = 0);

  /// Notify the GUI that we have completed our task.
  virtual void Modified(uint4 uFlags) {};

  /// Unit test function(tests the class functionality).
  const bool UnitTest();

  /// Set the valid subset of the volume which is allowed to change
  void SetValidSubset( Box<int4> validSubset ) { m_validSubset = validSubset; }

  /// gets the next component color
  const PixelRGBA<uint1> & GetNextComponentColor() const { return m_nextComponentColor; }

  /// sets the next component color
  void SetNextComponentColor( const PixelRGBA<uint1> & color ) { m_nextComponentColor = color; }

  /// gets the next component name
  const std::string & GetNextComponentName() const { return m_nextComponentName; }

  /// sets the next component name
  void SetNextComponentName( const std::string & name ) { m_nextComponentName = name; }

  /// grow a component
  bool GrowComponent( const int4 iComponentIndex );

  /// shrink a component
  bool ShrinkComponent( const int4 iComponentIndex );

  /// threshold segment
  bool ThresholdSegment();

  /// get the region grow object
  RegionGrow & GetRegionGrow() { return m_regionGrow; }

private:

  /// run method, region grow
  void RunRegionGrow(const Point<uint2> & seedPoint);

  /// run method, region threshold segmentation
  void RunRegionThresholdSegment();

  /// run method, morphlogical operations
  void RunMorphOp(SegParams::MorphOperSelEnum eMorphOpSel);

  /// run method, region sample
  void RunRegionSample(const Point<uint2> & seedPoint, const uint4 & uAxisIndex);

  /// run method, component handler update
  void RunCompHdlrUpdate();

// data
private:

  /// mode of interactive segmentation
  ModeIDsEnum m_eMode;

  /// segmentation paramters
  SegParams m_segParams;

  /// component handler pointer
  ComponentHandler * m_pCompHdlr;

  /// segmentation volume class
  SegmentationVolumes m_segVols;

  /// pointer to a scan volume(e.g., intensity volume)
  vxBlockVolume<uint2> * m_pIntensityVol;

  /// pointer to a segmentation volume(e.g., label volume)
  vxShareableObject< vxBlockVolume<uint2> > * m_pLabelVolume;

  /// Volume dimensions
  Triple<int4> m_volDim;

  /// Region grow object
  RegionGrow m_regionGrow;

  /// Region threshold segmentation object
  RegionThresholdSegment m_regionThresSeg;

  /// Region sampler object
  RegionSampler m_regionSampler;

  /// Morphological operator object
  MorphOp m_morphOp;

  /// (x,y,z) of a seed point
  Point<uint2> m_seedPoint;

  /// Default intensity volume
  static vxBlockVolume<uint2> m_defaultIntensityVol;

  /// Default segmentation volume
  static vxShareableObject< vxBlockVolume<uint2> > m_defaultLabelVolume;

  /// Default component handler object
  ComponentHandler m_defaultCompHdlr;

  /// Status of segmentation
  int4 m_iStatus;

  /// The subset of the volume that is allowed to change
  Box<int4> m_validSubset;

  /// the next component color
  PixelRGBA<uint1> m_nextComponentColor;

  /// the next component name
  std::string m_nextComponentName;

}; // class InteractiveSegmentation


#include "InteractiveSegmentation.inl"


// $Log: InteractiveSegmentation.h,v $
// Revision 1.8  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.7  2006/03/02 15:50:01  frank
// cleaned up grow and shrink operations
//
// Revision 1.6  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.5  2006/02/17 15:08:05  frank
// added functions to grow and shrink a component
//
// Revision 1.4  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.3  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.1.1.12.2  2005/07/11 17:43:42  frank
// cleaned up naming and color of components
//
// Revision 1.1.1.1.12.1  2005/07/05 17:52:16  frank
// spelling
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.63  2004/01/23 16:31:15  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.62  2003/12/22 16:20:57  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.61  2003/12/22 12:47:29  frank
// Added a valid subset to the threshold and region grow operations
//
// Revision 1.60  2003/06/23 19:29:27  dongqing
// change from int2 to uint2
//
// Revision 1.59  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.58  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.57  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.56  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.55  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.54  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.53  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.52  2003/03/18 21:21:12  geconomos
// Added new event EVENT_INTENSITY_RANGE.
//
// Revision 1.51  2003/01/22 21:51:36  ingmar
// 2D seedpoints are now set through Point2D (not the 3D Point)
//
// Revision 1.50  2001/01/09 16:58:17  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.49  2002/12/13 19:22:39  ingmar
// restricted "grow" operation (Dilate) to only include voxels in the active intensity range as long as it is beeing displayed
// also renamed Scan Vol and Seg Vol to IntensityVol and Label Vol
//
// Revision 1.48  2002/11/15 23:08:08  ingmar
// added binary label histogram
//
// Revision 1.47  2002/10/17 22:16:45  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.46  2002/10/04 12:52:20  geconomos
// Exported segmentation classes.
//
// Revision 1.45  2002/08/05 15:39:46  jaddonisio
// Removed path in forward include(not needed since vxUtils was moved to '70_'.
//
// Revision 1.44  2002/08/05 15:15:46  jaddonisio
// Removed path in forward include(not needed since vxUtils was moved to '70_'.
//
// Revision 1.43  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.42  2002/07/25 19:47:28  ingmar
// added relative path to include files that are further down in the dependency list (they really should not be referenced here)
//
// Revision 1.41  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.40  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.39.2.2  2002/07/11 14:57:22  jaddonisio
// Changed access functions for thresholds to take int4 instead of uint4.
//
// Revision 1.39.2.1  2002/07/02 22:24:51  michael
// added optimization using the binary histogram of the intensity volume for auto segmentation
//
// Revision 1.39  2002/06/21 18:16:02  jaddonisio
// Modified region sample to use different planes(XY,YZ,XZ).
//
// Revision 1.38  2002/06/13 22:19:00  jaddonisio
// i. New RegionThresholdSegment function.
// ii. Renamed Get/Set function for threshold range.
//
// Revision 1.37  2002/06/03 21:30:36  jaddonisio
// Coding standard changes.
//
// Revision 1.36  2002/05/15 21:10:22  jaddonisio
// i. New status member variable(along with access functions) for segmentation.
// ii. No longer calling Windows message box. Now setting status.
//
// Revision 1.35  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.34  2002/05/02 19:09:06  frank
// Added Gets for ComponentHandler and SegmentationVolumes.
//
// Revision 1.33  2002/05/01 19:50:04  jaddonisio
// More clean up(VolumeSet/AnyVolume no longer used).
//
// Revision 1.32  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.31  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.30  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.29  2002/04/18 16:19:23  jaddonisio
// InteractiveSegmentation class now contains a pointer to a Component Handler.
//
// Revision 1.28  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// Revision 1.27  2002/04/10 18:29:44  jaddonisio
// Changed name of delta seg tag vol.
//
// Revision 1.26  2002/04/06 00:05:54  jaddonisio
// Removed display option out of segmentation(its part of rendering mode).
//
// Revision 1.25  2002/04/04 16:19:03  geconomos
// Changed al 16-bit integers to 32-bit integers.
//
// Revision 1.24  2002/04/04 00:44:22  jaddonisio
// New delta segmentation tag volume.
//
// Revision 1.23  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.22  2002/03/07 22:31:07  dmitry
// Modified to fit new architecture.
//
// Revision 1.21  2001/12/20 22:17:40  jaddonisio
// New UnitTest() method.
//
// Revision 1.20  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.19  2001/12/05 21:24:43  jaddonisio
// Modifications for compliance with coding standard.
//
// Revision 1.18  2001/11/30 23:46:56  jaddonisio
// Changed 'REGION_GROW_ADDS' to REGION_ADD_OPTIONS'.
//
// Revision 1.17  2001/11/18 00:48:16  jaddonisio
// i. Checks for no region grow voxels.
// ii. For region removal: new method called 'RemoveLabelFromSegVolume' instead of 'UpdateSegVolume'.
//
// Revision 1.16  2001/11/16 19:58:41  jaddonisio
// New public method 'RunCompHdlr" to update Component Handler.
//
// Revision 1.15  2001/11/16 00:27:00  jaddonisio
// New RegionRemove.
//
// Revision 1.14  2001/11/14 16:40:55  dmitry
// Events added to segmentation.
//
// Revision 1.13  2001/11/14 16:00:21  jaddonisio
// i. New enum events.
// ii. New RunRegionRemove method.
//
// Revision 1.12  2001/11/13 21:39:11  jaddonisio
// New RunRegionSample and Set/Get seed point.
//
// Revision 1.11  2001/11/12 23:52:45  jaddonisio
// Set a default VolumeSet to avoid crash.
//
// Revision 1.10  2001/11/12 02:17:04  jaddonisio
// Changed event to EVENT_LABEL_VOLUME.
//
// Revision 1.9  2001/11/12 00:34:51  jaddonisio
// New bool for displaying thresholds.
//
// Revision 1.8  2001/11/11 19:08:14  geconomos
// Changed signature on SetSegmentVolume and added volume events.
//
// Revision 1.7  2001/11/11 05:29:39  jaddonisio
// MorphOp mods and new method to set seg. volume.
//
// Revision 1.6  2001/11/09 21:54:31  jaddonisio
// New MorphOP class.
//
// Revision 1.5  2001/11/07 23:22:05  jaddonisio
// Renamed 'SetRegionGrowAddOpts'
//
// Revision 1.4  2001/11/06 21:43:38  jaddonisio
// New SetRegionSampleRadius.
//
// Revision 1.3  2001/01/13 09:27:16  jaddonisio
// First integration of Interactive Segmentation into v3D Explorer.
//
// Revision 1.2  2001/11/01 04:50:07  frank
// Added GUI notification function.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/InteractiveSegmentation.h,v 1.8 2006/03/29 20:03:19 geconomos Exp $
// $Id: InteractiveSegmentation.h,v 1.8 2006/03/29 20:03:19 geconomos Exp $
