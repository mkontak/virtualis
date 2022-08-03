// $Id: v3D_Enums.h,v 1.12 2007/10/24 20:08:55 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "InteractiveSegmentation.h"
#include "vxUtils.h"
#include "vxViewerType.h"
#include "vxCine.h"
#include "vxIRenderingContext.h"


// namespaces
OPEN_VISUALIZATION_NS


/**
 * Orthogonal plane orientations.
 */
public enum class Orientations
{
  Axial    = vxUtils::AXIAL,
  Coronal  = vxUtils::CORONAL,
  Sagittal = vxUtils::SAGITTAL,
}; // enum Orientations


/**
 * Viewer control viewer types.
 */
[ System::Flags ]
public enum class ViewerTypes
{
  Sub  = 0x02,
  Main = 0x04,
}; // enum ViewerTypes


/**
 * Volume types.
 */
public enum class VolumeType
{
  Intensity  = 0,
  Label = 1,
}; // enum VolumeDataTypes
    

/**
 * Volume header variables.
 */
public enum class HeaderVariables
{
  Current                   = vxVolumeHeader::Current,
  Voltage                   = vxVolumeHeader::Voltage,
  SliceThickness            = vxVolumeHeader::SliceThickness,
  Pitch                     = vxVolumeHeader::Pitch,
  Tilt                      = vxVolumeHeader::Tilt,
  FieldOfView               = vxVolumeHeader::FieldOfView,
  HeaderVersion             = vxVolumeHeader::HeaderVersion,
  SiteID                    = vxVolumeHeader::SiteID,
  StudyDate                 = vxVolumeHeader::StudyDate,
  StudyTime                 = vxVolumeHeader::StudyTime,
  SeriesUID                 = vxVolumeHeader::SeriesUID,
  StudyUID                  = vxVolumeHeader::StudyUID,
  PhysicianName             = vxVolumeHeader::PhysicianName,
  PatientID                 = vxVolumeHeader::PatientID,
  InstitutionName           = vxVolumeHeader::InstitutionName,
  OperatorName              = vxVolumeHeader::OperatorName,
  PatientName               = vxVolumeHeader::PatientName,
  PatientBirthdate          = vxVolumeHeader::PatientBirthdate,
  PatientAge                = vxVolumeHeader::PatientAge,
  PatientAgeUnits           = vxVolumeHeader::PatientAgeUnits,
  PatientWeight             = vxVolumeHeader::PatientWeight,
  PatientSex                = vxVolumeHeader::PatientSex,
  PatientHistory            = vxVolumeHeader::PatientHistory,
  PatientPosition           = vxVolumeHeader::PatientPosition,
  StudyType                 = vxVolumeHeader::StudyType,
  SliceOrder                = vxVolumeHeader::SliceOrder,
  Manufacturer              = vxVolumeHeader::Manufacturer,
  DicomHeader               = vxVolumeHeader::DicomHeader,
  DataModality              = vxVolumeHeader::DataModality,
	ReconstructionInterval    = vxVolumeHeader::ReconstructionInterval,
	Contrast                  = vxVolumeHeader::Contrast,
	BodyPart                  = vxVolumeHeader::BodyPart,
	ExposureTime              = vxVolumeHeader::ExposureTime,
	TransducerFrequency       = vxVolumeHeader::TransducerFrequency,
	ScanningSequence          = vxVolumeHeader::ScanningSequence,
	SequenceVariant           = vxVolumeHeader::SequenceVariant,
	AcquisitionType           = vxVolumeHeader::AcquisitionType,
	EchoTime                  = vxVolumeHeader::EchoTime,
	RepetitionTime            = vxVolumeHeader::RepetitionTime,
  ModificationString        = vxVolumeHeader::ModificationString,
  StudyDescription          = vxVolumeHeader::StudyDescription,
}; // enum HeaderVariables


/**
 * Segmentation Modes.
 */
public enum class SegmentationModes
{
  Null                    = InteractiveSegmentation::MODE_NOT_IMPL,
  RegionGrow              = InteractiveSegmentation::REGION_GROW_MODE,
  RegionThresholdSegment  = InteractiveSegmentation::REGION_THRESHOLD_SEGMENT_MODE,
  RegionMerge             = InteractiveSegmentation::REGION_MERGE_MODE,
  RegionSplit             = InteractiveSegmentation::REGION_SPLIT_MODE,
  RegionRemove            = InteractiveSegmentation::REGION_REMOVE_MODE,
  Classification          = InteractiveSegmentation::CLASSIFICATION_MODE,
  Morphological           = InteractiveSegmentation::MORPH_OP_MODE,
  RegionSample2D          = InteractiveSegmentation::REGION_SAMPLE_2D,
  RegionSample3D          = InteractiveSegmentation::REGION_SAMPLE_3D,
  AutoCenterline          = InteractiveSegmentation::AUTO_CENTERLINE_MODE,
  AutoSkeleton            = InteractiveSegmentation::AUTO_SKELETON_MODE,
  ManualSkeleton          = InteractiveSegmentation::MANUAL_SKELETON_MODE,
  ManualSkeletonBranch    = InteractiveSegmentation::MANUAL_SKELETON_BRANCH_MODE,
  ComponentHandlerUpdate  = InteractiveSegmentation::COMPHDLR_UPDATE_MODE,
}; // enum SegmentationModes


/**
 * Region grow add options.
 */
public enum class RegionGrowAddOptions
{
  Default = SegParams::DEFAULT_REGION_ADDS,
  New     = SegParams::NEW_REGIONS,
  Append  = SegParams::APPEND_REGIONS,
}; // enum RegionGrowAddOptions


/**
 * Region grow methods.
 */
public enum class RegionGrowMethods 
{ 
  Default               = SegParams::DEFAULT_REGION_GROW, 
  IntensityThreshold    = SegParams::INTENSITY_THRESHOLD, 
  VoxelClassRange       = SegParams::VOXEL_CLASS_RANGE, 
  SampleIntensityRange  = SegParams::SAMPLE_INTENSITY_RANGE, 
  SampleClassRange      = SegParams::SAMPLE_CLASS_RANGE 
}; // enum RegionGrowMethods 


/**
 * Cine modes.
 */
public enum class CineModes
{
  Idle           = vxCine::IDLE,
  Swing          = vxCine::SWING,
  LoopForward    = vxCine::LOOP_FORWARD,
  LoopBackward   = vxCine::LOOP_BACKWARD
}; // enum CineModes


/**
 * Standard camera view positions.
 */
public enum class CameraViews
{
  Anterior  = vxUtils::ANTERIOR,
  Posterior = vxUtils::POSTERIOR,
  Left      = vxUtils::LEFT,
  Right     = vxUtils::RIGHT,
  Superior  = vxUtils::SUPERIOR,
  Inferior  = vxUtils::INFERIOR,
  Axial     = Inferior,
  Coronal   = Anterior,
  Sagittal  = Left,
}; // enum CameraViews


/**
 * Viewer drop types.
 */ 
public enum class DropType
{
  Dataset  = vxViewerType::DATASET,
  Camera   = vxViewerType::CAMERA,
  Colormap = vxViewerType::COLORMAP,
  Preset   = vxViewerType::PRESET,
}; // enum DropType

[ System::Flags ]
public enum class RenderingOptions
{
  ShowPatientInfo         = vxRenderingMode::SHOW_PATIENT_INFORMATION,
  ShowImageInfo           = vxRenderingMode::SHOW_IMAGE_INFORMATION,
  ShowCropBox             = vxRenderingMode::SHOW_CROP_BOX,
  ShowCenterlinePath      = vxRenderingMode::SHOW_VESSEL_CENTERLINE,
  ShowComponentsInMPR     = vxRenderingMode::SHOW_COMPONENT_IN_MPR,
  ShowColorInMPR          = vxRenderingMode::SHOW_COLORMAP_IN_MPR,
  ShowCursor3DInMPR       = vxRenderingMode::SHOW_CURSOR_3D_IN_MPR,
  ShowIntensityRange      = vxRenderingMode::SHOW_INTENSITY_RANGE,
  ShowComponentsOnTop     = vxRenderingMode::SHOW_LABEL_ON_TOP,
  ShowBoxOfInterest       = vxRenderingMode::SHOW_BOX_OF_INTEREST,
  AutoFlyMode             = vxRenderingMode::AUTO_FLY_MODE,
  ShowJittering           = vxRenderingMode::SHOW_JITTERING,
  ShowPreIntegration      = vxRenderingMode::SHOW_PRE_INTEGRATION,
  ShowShading             = vxRenderingMode::SHOW_SHADING,
  ShowShadows             = vxRenderingMode::SHOW_SHADOWS,
  ShowEveryPixel          = vxRenderingMode::SHOW_EVERY_PIXEL,
  ShowCutsShaded          = vxRenderingMode::SHOW_CUTS_SHADED,
  ShowCutsGrayscale       = vxRenderingMode::SHOW_CUTS_GRAYSCALE,
  ShowAnnotations         = vxRenderingMode::SHOW_ANNOTATIONS,
 }; // enum RenderingOptions
 
 
public enum class RenderingAlgorithm
{
  Default  = vxRenderingMode::DEFAULT,
  MIP      = vxRenderingMode::MIP,
  XRay     = vxRenderingMode::XRAY,
  ThinSlab = vxRenderingMode::THIN_SLAB,
};

 public enum class SliceShadowMode
 {
  None                = vxRenderingMode::SHADOWS_NONE,
  Active              = vxRenderingMode::SHADOWS_ACTIVE,
  ActiveSliceImage    = vxRenderingMode::SHADOWS_ACTIVE_IMAGE,
  ActiveSliceClipped  = vxRenderingMode::SHADOWS_ACTIVE_CLIPPED,
  All                 = vxRenderingMode::SHADOWS_ALL,
 };


/// rendering context modes
public enum class RenderingContextMode
{
  Windowed      = vxIRenderingContext::WINDOWED,
  Shared        = vxIRenderingContext::SHARED,
  Dib           = vxIRenderingContext::DIB,
};

CLOSE_VISUALIZATION_NS


// $Log: v3D_Enums.h,v $
// Revision 1.12  2007/10/24 20:08:55  gdavidson
// Added RenderingContextMode
//
// Revision 1.11  2007/08/21 19:41:25  gdavidson
// Added ShowAnnotations enum
//
// Revision 1.10  2007/03/23 14:36:55  romy
// new DropType added
//
// Revision 1.9  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.8  2007/01/18 15:44:16  geconomos
// added new slice shadow mode
//
// Revision 1.7  2007/01/16 17:11:03  gdavidson
// Added vxCine
//
// Revision 1.6  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/10/03 20:14:16  geconomos
// removed slice shadow enum
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.2  2004/10/05 14:41:29  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.31  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.30  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.29  2003/10/15 01:26:06  geconomos
// Added RenderingModes flags enum.
//
// Revision 1.28  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.27  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.26  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.25  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.24  2003/05/06 18:51:47  frank
// Added Study Description header entry, but it still needs the actual DICOM lookup values from Dongqing.
//
// Revision 1.23  2003/05/06 15:12:18  geconomos
// Coding standards.
//
// Revision 1.22  2003/04/07 17:12:47  frank
// Added a modification string to the variable list.  This captures the sequence of modifications done to the volume since creation from original DICOM.
//
// Revision 1.21  2003/03/24 18:06:33  frank
// Issue #2953: Added referring physician to volume header display
//
// Revision 1.20  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.19  2003/03/17 19:48:01  geconomos
// Issue #2914. Implemented DropType property from vxViewer.
//
// Revision 1.18  2003/01/03 16:58:16  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.17  2002/11/15 21:46:26  geconomos
// Updated to use new ViewType enum value from vxUtils.
//
// Revision 1.16  2002/11/06 22:05:26  geconomos
// Added Flags attribute to ViewerTypes enum
//
// Revision 1.15  2002/11/05 20:51:03  geconomos
// Added enum for CameraViews.
//
// Revision 1.14  2002/11/04 14:30:35  geconomos
// new toolbar class.
//
// Revision 1.13  2002/10/14 20:06:39  geconomos
// Segmentation enums.
//
// Revision 1.12  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.11  2002/09/24 16:08:22  geconomos
// Exception handling and formatting.
//
// Revision 1.10  2002/09/18 18:02:56  geconomos
// Changed values of ViewerTypes.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Enums.h,v 1.12 2007/10/24 20:08:55 gdavidson Exp $
// $Id: v3D_Enums.h,v 1.12 2007/10/24 20:08:55 gdavidson Exp $
