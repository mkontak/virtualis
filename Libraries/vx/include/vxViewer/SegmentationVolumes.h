// $Id: SegmentationVolumes.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)


/**
 * Class definiion for SegmentationVolumes. This class encapsulates the
 * updates and modifications, associated with segmentation, on a (label) volume.
 */


#ifndef SegmentationVolumes_h
#define SegmentationVolumes_h


// includes
#include "vxBlockVolumeIterator.h"


// class definition
class VX_VIEWER_DLL SegmentationVolumes
{
// functions
public:

  /// default constructor
  SegmentationVolumes();

  /// destructor
  virtual ~SegmentationVolumes() {}

  /// get segmentation label volume pointer
  vxBlockVolume<uint2> * GetSegLabelVol() const { return m_pSegLabelVol; }

  /// set segmentation label volume
  void SetSegLabelVol(vxBlockVolume<uint2> * pLabelVolume);

  /// pointer to delta segmentation tag volume
  vxBlockVolume<bool> * GetDeltaSegTagVol() const { return & m_delSegTagVol; }

  /// get dimension of volumes
  Triple<uint2> GetDim() { return m_volDim; }

  /// set a delta tag volume for segmentation
  void InitDeltaSegTagVolume(); 

  /// updates to segmentation label volume
  void UpdateSegVolume(vxBlockVolume<bool> * pTagVol, const uint2 & uLabelIndex);

  /// remove label from segmentation volume
  void RemoveLabelFromSegVolume(const uint2 & uLabelIndex);

  /// remove label from segmentation volume
  void RemoveLabelFromSegVolume(vxBlockVolume<bool> & rRegTagVol);
  
  /// remove all labels from segmentation volume
  void RemoveAllLabelsFromSegVolume();

  /// condense labels in segmentation volume
  void CondenseLabelsInSegVolume();

// data
private:
  
  /// dimension of volume for segmentation
  Triple<int4> m_volDim;        

  /// tag volume to track effected voxels of each segmentation operation
  static vxBlockVolume<bool> m_delSegTagVol;

  /// pointer to segmentation label volume
  vxBlockVolume<uint2> * m_pSegLabelVol;

  /// default segmentation label volume
  static vxBlockVolume<uint2> m_defaultSegLblVol;

}; // SegmentationVolumes


#endif // SegmentationVolumes_h


// Revision History:
// $Log: SegmentationVolumes.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2003/07/09 13:25:51  michael
// moved 1-liners to header file and brought code back in that was once commented
// out due to a bug in the vxBlockVolume homogeneous flag.
//
// Revision 1.21  2003/07/02 22:01:21  michael
// took out destructor from source file, put single lined code in header file, and
// added comment for outcommented code that needs to be revisited later on.
//
// Revision 1.20  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.19  2003/06/07 15:36:38  michael
// reformatting
//
// Revision 1.18  2003/04/24 13:34:44  frank
// Code review
//
// Revision 1.17  2003/04/16 12:49:09  frank
// Code review
//
// Revision 1.16  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.15  2003/04/09 15:07:00  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.14  2003/04/04 12:58:23  frank
// Exported the class to the DLL interface
//
// Revision 1.13  2002/10/17 22:16:45  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.12  2002/10/04 12:52:20  geconomos
// Exported segmentation classes.
//
// Revision 1.11  2002/09/11 15:46:20  jaddonisio
// New 'CondenseLabelsInSegVol' function.
//
// Revision 1.10  2002/09/05 22:44:02  jaddonisio
// New vxComponent(don't shift labels).
//
// Revision 1.9  2002/07/02 23:55:03  soeren
// i. Made MaskVolume & deltaVolume compact.
// ii. Created 'RemoveAllLabelsFromSegVol'
//
// Revision 1.8  2002/06/03 21:19:29  jaddonisio
// Coding standard changes.
//
// Revision 1.7  2002/05/06 15:56:39  jaddonisio
// More clean-up.
//
// Revision 1.6  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.5  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.4  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.3  2002/04/18 16:17:45  jaddonisio
// Made delta tag volume static.
//
// Revision 1.2  2002/04/16 19:04:09  michael
// added copyright date (year)
//
// Revision 1.1  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/SegmentationVolumes.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: SegmentationVolumes.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
