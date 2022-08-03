// $Id: InteractiveSegmentation.inl,v 1.3 2006/02/28 15:24:42 frank Exp $
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


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Public method to set a pointer to a scan volume
 * @param volume reference to Volume
 */
inline void InteractiveSegmentation::SetScanVolume(vxBlockVolume<uint2> & volume)
//*******************************************************************
{
  m_pIntensityVol = &volume;
  m_volDim        = Triple<int4>(volume.GetHeader().GetVolDim());
} // SetScanVolume()


/**
 * Public method to set a pointer to a segmentation volume
 * @param pVolume pointer to a label volume set
 */
inline void InteractiveSegmentation::SetSegmentVolume(vxShareableObject< vxBlockVolume<uint2> > * pVolume)
//*******************************************************************
{
  m_pLabelVolume = pVolume;
  m_segVols.SetSegLabelVol(pVolume);
} // SetSegmentVolume()


/**
 * Public method to set region grow method
 * @param eVal enumeration of region grow method selection
 */
inline void InteractiveSegmentation::SetRegionGrowMethod(SegParams::RegionGrowMethodEnum eValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::REGION_GROW_METHOD, eValue);
} // SetRegionGrowMethod()


/**
 * Public method to set option for region grow adds
 * @param eVal enumeration of region grow adds options
 */
inline void InteractiveSegmentation::SetRegionGrowAddOpt(SegParams::RegionAddOptionEnum eValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::REGION_ADD_OPT, eValue);
} // SetRegionGrowAddOpt()


/**
 * Public method to set radius of sample area
 * @param uValue enumeration of region sample radius
 */
inline void InteractiveSegmentation::SetRegionSampleRadius(float4 fValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::REGION_SAMPLE_RADIUS, fValue);
} // SetRegionSampleRadius()


/**
 * Public method to set low intensity for region threshold
 * @param iVal intensity value
 */
inline void InteractiveSegmentation::SetRegionIntensityLow(int4 iValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::REGION_THRESHOLD_LOW, iValue);
} // SetRegionIntensityLow()


/**
 * Public method to set high intensity for region grow threshold
 * @param iVal intensity value
 */
inline void InteractiveSegmentation::SetRegionIntensityHigh(int4 iValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::REGION_THRESHOLD_HIGH, iValue);
} // SetRegionIntensityHigh()


/**
 * Public method to set number of iterations for morphological operations
 * @param uValue number of iterations
 */
inline void InteractiveSegmentation::SetMorphOpIterations(uint4 uValue)
//*******************************************************************
{
  m_segParams.Set(SegParams::MORPH_OP_ITER, uValue);
} // SetMorphOpIterations()


/**
 * Public method to get ptr. to delta segmentation volume
 * @return Pointer to delta segmentation tag volume
 */
vxBlockVolume<bool> * InteractiveSegmentation::GetDeltaSegTagVol() const
//*******************************************************************
{
  return m_segVols.GetDeltaSegTagVol();
} // GetDeltaSegTagVol()


/**
 * Public method to get region grow method
 * @return Region grow method
 */
inline uint4 InteractiveSegmentation::GetRegionGrowMethod()
//*******************************************************************
{
  uint4 uValue(0);
  m_segParams.Get(SegParams::REGION_GROW_METHOD, &uValue);

  return uValue;
} // GetRegionGrowMethod()


/**
 * Public method to get region grow add option
 * @return Region grow add option
 */
inline uint4 InteractiveSegmentation::GetRegionGrowAddOpt()
//*******************************************************************
{
  uint4 uValue(0);
  m_segParams.Get(SegParams::REGION_ADD_OPT, &uValue);

  return uValue;
} // GetRegionGrowAddOpt()


/**
 * Public method to get radius of sample area
 * @return Region sample radius
 */
inline float4 InteractiveSegmentation::GetRegionSampleRadius()
//*******************************************************************
{
  float4 fValue(0.0F);
  m_segParams.Get(SegParams::REGION_SAMPLE_RADIUS, &fValue);

  return fValue;
} // GetRegionSampleRadius()


/**
 * Public method to get low intensity for region threshold
 * @return Region grow intensity low threshold
 */
inline int4 InteractiveSegmentation::GetRegionIntensityLow()
//*******************************************************************
{
  int4 iValue(0);
  m_segParams.Get(SegParams::REGION_THRESHOLD_LOW, &iValue);

  return iValue;
} // GetRegionIntensityLow()


/**
 * Public method to get high intensity for region threshold
 * @return Region grow intensity high threshold
 */
inline int4 InteractiveSegmentation::GetRegionIntensityHigh()
//*******************************************************************
{
  int4 iValue(0);
  m_segParams.Get(SegParams::REGION_THRESHOLD_HIGH, &iValue);

  return iValue;
} // GetRegionIntensityHigh()


/**
 * Public method to get number of iterations for morphological operations
 * @return Number of iterations for morphological operations
 */
inline uint4 InteractiveSegmentation::GetMorphOpIterations()
//*******************************************************************
{
  uint4 uValue(0);
  m_segParams.Get(SegParams::MORPH_OP_ITER, &uValue);

  return uValue;
} // GetMorphOpIterations()


/**
 * Public method to test if segmentation is successful
 * @return Success test
 */
inline bool InteractiveSegmentation::IsSuccessful() const
//*******************************************************************
{
  return(!(GetStatus() & 0xF0000000));
} // IsSuccessful()


#undef FILE_REVISION


// Revision History:
// $Log: InteractiveSegmentation.inl,v $
// Revision 1.3  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.2  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.43  2003/12/22 16:20:57  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.42  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.41  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.40  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.39  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.38  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.37  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.36  2003/01/22 21:50:22  ingmar
// dimension and units are now Triples
//
// Revision 1.35  2002/12/13 19:22:40  ingmar
// restricted "grow" operation (Dilate) to only include voxels in the active intensity range as long as it is beeing displayed
// also renamed Scan Vol and Seg Vol to IntensityVol and Label Vol
//
// Revision 1.34  2002/11/15 23:08:08  ingmar
// added binary label histogram
//
// Revision 1.33  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.32.2.2  2002/07/11 14:57:22  jaddonisio
// Changed access functions for thresholds to take int4 instead of uint4.
//
// Revision 1.32.2.1  2002/07/02 22:24:51  michael
// added optimization using the binary histogram of the intensity volume for auto segmentation
//
// Revision 1.32  2002/06/13 22:19:17  jaddonisio
// i. New RegionThresholdSegment function.
// ii. Renamed Get/Set function for threshold range.
//
// Revision 1.31  2002/06/03 21:30:29  jaddonisio
// Coding standard changes.
//
// Revision 1.30  2002/05/15 21:10:22  jaddonisio
// i. New status member variable(along with access functions) for segmentation.
// ii. No longer calling Windows message box. Now setting status.
//
// Revision 1.29  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.28  2002/05/01 23:01:49  michael
// cosmetics
//
// Revision 1.27  2002/05/01 22:10:54  michael
// Replaced DensityVolume by IntensityVolume
//
// Revision 1.26  2002/05/01 19:50:03  jaddonisio
// More clean up(VolumeSet/AnyVolume no longer used).
//
// Revision 1.25  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.24  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.23  2002/04/18 16:19:23  jaddonisio
// InteractiveSegmentation class now contains a pointer to a Component Handler.
//
// Revision 1.22  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// Revision 1.21  2002/04/10 18:29:44  jaddonisio
// Changed name of delta seg tag vol.
//
// Revision 1.20  2002/04/06 00:05:54  jaddonisio
// Removed display option out of segmentation(its part of rendering mode).
//
// Revision 1.19  2002/04/04 16:19:03  geconomos
// Changed al 16-bit integers to 32-bit integers.
//
// Revision 1.18  2002/04/04 00:44:32  jaddonisio
// New delta segmentation tag volume.
//
// Revision 1.17  2002/03/07 22:31:07  dmitry
// Modified to fit new architecture.
//
// Revision 1.16  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.15  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.14  2001/12/05 21:24:43  jaddonisio
// Modifications for compliance with coding standard.
//
// Revision 1.13  2001/11/30 23:46:56  jaddonisio
// Changed 'REGION_GROW_ADDS' to REGION_ADD_OPTIONS'.
//
// Revision 1.12  2001/11/15 01:17:35  jaddonisio
// New events to display thresholds and display seg. params.
//
// Revision 1.11  2001/11/13 21:39:11  jaddonisio
// New RunRegionSample and Set/Get seed point.
//
// Revision 1.10  2001/11/12 23:52:45  jaddonisio
// Set a default VolumeSet to avoid crash.
//
// Revision 1.9  2001/11/12 02:03:11  jaddonisio
// added events to update view with changed range thresholds
//
// Revision 1.8  2001/11/12 00:33:24  jaddonisio
// Fire event when displaying and changing threshold.
//
// Revision 1.7  2001/11/11 21:18:51  jaddonisio
// FireOnChanged event for morphing.
//
// Revision 1.6  2001/11/11 19:08:14  geconomos
// Changed signature on SetSegmentVolume and added volume events.
//
// Revision 1.5  2001/11/09 21:54:31  jaddonisio
// New MorphOP class.
//
// Revision 1.4  2001/11/07 23:22:23  jaddonisio
// Corrected comments.
//
// Revision 1.3  2001/11/06 21:43:20  jaddonisio
// New SetRegionSampleRadius.
//
// Revision 1.2  2001/11/02 16:10:29  jaddonisio
// Added implementation of 'get's' for region grow threshold.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/InteractiveSegmentation.inl,v 1.3 2006/02/28 15:24:42 frank Exp $
// $Id: InteractiveSegmentation.inl,v 1.3 2006/02/28 15:24:42 frank Exp $
