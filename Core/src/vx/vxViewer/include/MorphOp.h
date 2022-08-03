// $Id: MorphOp.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates Morphological Operation image processing.
 */

#ifndef MorphOp_h
#define MorphOp_h


// includes
#include "vxBlockVolume.h"


// forward declarations
class SegmentationVolumes;
template <class T> class vxBlockVolumeIteratorNeighbors;


// class definition
class MorphOp
{
public:
  
  /// default constructor
  MorphOp();

  /// destructor
  virtual ~MorphOp() {};

// enums
public:

  enum PropIDsEnum { 
    MORPH_OP_TYPE, 
    NUMBER_OF_ITERATIONS
  }; // PropIDsEnum

  enum MorphOpEnum { 
    ERODE_OP = 1, 
    DILATE_OP, 
    CLOSE_OP, 
    OPEN_OP, 
    DILATE_WITHIN_RANGE_OP,
    ERODE_WITHIN_RANGE_OP 
  }; // MorphOpEnum

// functions
public:

  /// set the segmentation volume class
  void SetSegVol(SegmentationVolumes * pSegVols) { m_pSegVols = pSegVols; }

  /// run method
  void Run(const std::vector<vxBlockVolume<bool> *> & vpRegTagVol, 
           vxBlockVolume<bool> * pMaskTagVol, 
           uint2 uCompLabel, 
           uint2 uOpType, 
           vxBlockVolume<uint2> * pIntensityVol, 
           uint4 uIterN,
           uint2 uThresholdLow=0, uint2 uThresholdHigh=4095);

private:

  /// erosion method
  void Erosion(uint2 uThresholdLow, uint2 uThresholdHigh);

  /// dilation method
  void Dilation(uint2 uThresholdLow, uint2 uThresholdHigh);

  /// update segmentation volume
  void UpdateSegVolume(vxBlockVolume<bool> * pTagVol, uint2 uCompLabel);

  /// apply morphing to tag volume
  void ApplyMorphToTag(vxBlockVolume<bool> * pTagVol, bool bTag);

  /// block boundary traversal
  typedef void (MorphOp::*NtraversalFunctionPtrType)(vxBlockVolumeIterator<bool> & iterRegTag, 
                                                     vxBlockVolumeIteratorNeighbors<uint2> & neighIter, 
                                                     vxBlockVolumeIterator<bool> & iterMorphTag);
  
  /// bounding box traversal
  void MorphOp::BlockFaceTraversal(vxBlockVolumeIterator<bool> & iterRegTag, 
                                   vxBlockVolumeIteratorNeighbors<uint2> & neighIter, 
                                   vxBlockVolumeIterator<bool> & iterProcTagVol, 
                                   NtraversalFunctionPtrType pNtraversal);

  /// neighborhood traversal (for erosion)
  void ExamErosionCandidate(vxBlockVolumeIterator<bool> & iterRegTag, 
                            vxBlockVolumeIteratorNeighbors<uint2> & neighIter, 
                            vxBlockVolumeIterator<bool> & iterProcTagVol);

  /// neighborhood traversal (for dilation)
  void ExamDilationCandidate(vxBlockVolumeIterator<bool> & iterRegTag, 
                             vxBlockVolumeIteratorNeighbors<uint2> & neighIter, 
                             vxBlockVolumeIterator<bool> & iterProcTagVol);

// data
private:

  const std::vector<vxBlockVolume<bool> *> * m_pvpRegTagVols; // Pointer to vector of region tag volume pointers.
  SegmentationVolumes *  m_pSegVols;    // Pointer to the segmentation volumes class.
  vxBlockVolume<uint2> * m_pIntensityVol;
  vxBlockVolume<uint2> * m_pSegLblVol;  // Pointer to segmentation label volume(block volume of uint2).
  vxBlockVolume<bool> *  m_pMaskTagVol; // Pointer to mask tag volume(used to tag voxels that are locked).
  vxBlockVolume<bool>    m_morphTagVol; // Morphing tag volume(tags voxels affected by morphing).
  uint2                  m_uCompLabel;  // Component label.
  uint4                  m_uThrPhi;     // Phi. thresold.
  uint4                  m_uThrChi;     // Chi. thresold.

  uint4 m_uNumberOfVoxels;

}; // MorphOp


#endif // MorphOp_h


// Revision History:
// $Log: MorphOp.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/07/08 14:41:41  michael
// renamed functions to be descriptive
//
// Revision 1.24  2003/07/03 17:49:26  michael
// added functionality to grow and shrink within range
//
// Revision 1.23  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.22  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.21  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.20  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.19  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.18  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.17  2002/12/13 19:21:36  ingmar
// restricted "grow" operation (Dilate) to only include voxels in the active intensity range as long as it is beeing displayed
//
// Revision 1.16  2002/07/25 20:36:59  jaddonisio
// Skip traversal inside homogeneous blocks during morph operations.
//
// Revision 1.15  2002/06/03 21:30:18  jaddonisio
// Coding standard changes.
//
// Revision 1.14  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.13  2002/05/02 22:02:05  jaddonisio
// Commented out old block volume and associated functions(will cleanup soon).
//
// Revision 1.12  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.11  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// Revision 1.10  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.9  2001/12/17 16:38:55  jaddonisio
// Changed Seta and Get to pass iPropID by value(to fix problem when value is an enum).
//
// Revision 1.8  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.7  2001/12/06 19:58:59  jaddonisio
// Mods to comply with coding standard
//
// Revision 1.6  2001/11/21 20:05:52  jaddonisio
// New morphology based on vector or region tag volumes(removed seed point methods).
//
// Revision 1.5  2001/11/11 21:18:06  jaddonisio
// include global.h to remove warnings from std.
//
// Revision 1.4  2001/11/11 05:28:22  jaddonisio
// Implemented dilation.
//
// Revision 1.3  2001/11/11 02:23:58  jaddonisio
// i. new MorphTagVol, volume
// ii. replace start point with vector of seed points.
//
// Revision 1.2  2001/11/10 03:58:15  jaddonisio
// Modified enums. Also, disable 'Erosion' temporarily(until I can get it to work).
//
// Revision 1.1  2001/11/09 21:55:03  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/MorphOp.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: MorphOp.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
