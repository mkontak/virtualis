// $Id: SegmentationParameters.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates the Segmentation Parameters. It provides set/get
 * for access to all parameters.
 *
 */

#ifndef SegParams_h
#define SegParams_h


// class definition
class VX_VIEWER_DLL SegParams
{
public:
  
  /// default constructor
  SegParams();

  /// destructor
  virtual ~SegParams() {};

// Enumerations
public:

  enum PropIDsEnum          { REGION_GROW_METHOD, REGION_THRESHOLD_SEGMENT_METHOD, REGION_ADD_OPT, REGION_SAMPLE_RADIUS, REGION_THRESHOLD_LOW, REGION_THRESHOLD_HIGH, REGION_CLASS_LOW, REGION_CLASS_HIGH, MORPH_OP_SEL, MORPH_OP_ITER };
  enum RegionGrowMethodEnum { DEFAULT_REGION_GROW  = 0, INTENSITY_THRESHOLD, VOXEL_CLASS_RANGE, SAMPLE_INTENSITY_RANGE, SAMPLE_CLASS_RANGE };
  enum RegionThresholdSegmentMethodEnum { DEFAULT_REGION_THRESHOLD_SEGMENT = 0, INTENSITY_THRESHOLD_SEGMENT, VOXEL_CLASS_THRESHOLD_SEGMENT };
  enum RegionAddOptionEnum  { DEFAULT_REGION_ADDS  = 0, NEW_REGIONS, APPEND_REGIONS };
  enum ClassWindowSelEnum   { DEFAULT_CLASS_WINDOW = 0, CLASS_2DWINDOW, CLASS_3DWINDOW };
  enum MorphOperSelEnum     { DEFAULT_MORPH_OP     = 0, EROSION, DILATION, OPEN, CLOSE, FILL };
  enum ConnectivitySelEnum  { DEFAULT_CONNECTIVITY = 0, n4CONNECTED, n6CONNECTED, n8CONNECTED, n18CONNECTED, n26CONNECTED };

// Member Functions
public:

  /// get parameters
  int4 Get( const PropIDsEnum ePropID, ... );

  /// set parameters
  int4 Set( const PropIDsEnum ePropID, ... );

// Member Variables
private:

  RegionGrowMethodEnum m_eRegionGrowMethodSel;     // region grow method selection
  RegionAddOptionEnum  m_eRegionGrowAddSel;        // region grow add selection
  uint4                m_uRegionGrowIterations;    // region grow iterations
  RegionThresholdSegmentMethodEnum
                       m_eRegionThresSegMethodSel; // region threshold segmentation method selection
  float4               m_fRegionSampleRadius;      // region sample radius
  int4                 m_iRegionIntensityLow;      // region grow intensity range, low
  int4                 m_iRegionIntensityHigh;     // region grow intensity range, high
  uint4                m_uRegionClassHigh;         // region grow class range, high
  uint4                m_uRegionClassLow;          // region grow class range, low
  MorphOperSelEnum     m_eMorphOpSelection;        // morphological operation selection
  uint4                m_uMorphOpIterations;       // morphological operation iterations
  ConnectivitySelEnum  m_eConnectivitySel;         // connectivity selection
  uint4                m_uNumberOfClasses;         // number of classes
  uint4                m_uClassVoxelType;          // classification voxel type
  uint4                m_uClassIntensityRangeHigh; // class intensity range, high
  uint4                m_uClassIntensityRangeLow;  // class intensity range, low
  ClassWindowSelEnum   m_eClassWinSel;             // classification window selection
  uint4                m_uContourAdjTension;       // contour adjustment tension
};


#endif // SegParams_h


// Revision History:
// $Log: SegmentationParameters.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/12/22 16:20:58  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.16  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.14  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.13  2003/04/04 12:58:23  frank
// Exported the class to the DLL interface
//
// Revision 1.12  2002/06/13 22:19:47  jaddonisio
// i. New RegionThresholdSegment function.
// ii. Renamed Get/Set function for threshold range.
//
// Revision 1.11  2002/04/12 15:52:44  jaddonisio
// Comment.
//
// Revision 1.10  2002/04/04 16:19:03  geconomos
// Changed al 16-bit integers to 32-bit integers.
//
// Revision 1.9  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.8  2001/12/17 16:36:43  jaddonisio
// Changed thresholds to int2 instead of uint2.
//
// Revision 1.7  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.6  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.5  2001/11/30 23:45:52  jaddonisio
// Changed 'REGION_GROW_ADDS' to REGION_ADD_OPTIONS'
//
// Revision 1.4  2001/11/10 03:59:11  jaddonisio
// Modified enums.
//
// Revision 1.3  2001/11/09 21:53:59  jaddonisio
// New property: MorphOpIter
//
// Revision 1.2  2001/11/06 21:38:13  jaddonisio
// New region grow method selection: SampleIntensityRange
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/SegmentationParameters.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: SegmentationParameters.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
