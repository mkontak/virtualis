/// $Id: MergeClasses.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MergeClasses.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete history at bottom of file.


/**  post-processing for the segmentation mask. 
 *   The pure air region will be eroded 2 layers.
 *   The eroded pure air region will be merged into the air partial volume layer.
 *   The tagged region with big horizontal plane will be merged with near by classes
 */

#ifndef MergeClasses_h
#define MergeClasses_h



class MergeClasses
{
  // member functions
public :
  /// constructor
  MergeClasses( LinVolS & volume, LinVolUC & segMask, std::vector<float4> & averageIntensity);

  /// destructor
  ~MergeClasses();

  /// applying class merge procedure
  void Run( int4 airLabel );

private :

  /// Erode air region while preserving the air-fluid boundary.
  void ConstrainedErosion( uint1 labelROI );

  /// merge classes in tagged area
  bool MergingMainParts();

  // member functions
private :
  /// volume
  LinVolS & m_Volume;

  /// segmentation mask
  LinVolUC & m_SegMask;

  /// average intensities
  std::vector<float4> m_vAverageIntensity;
};

#endif


// complete revision history:
// $Log: MergeClasses.h,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:45  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:38:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:24:22   ingmar
// Initial revision.
// Revision 1.4  2001/09/02 01:16:40  soeren
// moved exceptions from 03_utils to 01_logger
// and removed std lib warnings
//
// Revision 1.3  2001/06/25 22:21:20  uday
// coding conventions for the segmentation module
//
// Revision 1.2  2001/05/23 21:37:36  dongqing
// rewrite Seg module
//
// Revision 1.1.2.4  2001/05/10 21:40:59  dongqing
// re-write seg module
//
// Revision 1.1.2.3  2001/05/09 22:07:01  dongqing
// new
//
// Revision 1.1.2.2  2001/05/08 21:38:39  dongqing
// new
//
// Revision 1.1.2.1  2001/05/03 15:15:05  dongqing
// new
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MergeClasses.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: MergeClasses.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $