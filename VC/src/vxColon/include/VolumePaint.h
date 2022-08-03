// $Id: VolumePaint.h,v 1.6.2.1 2007/06/15 20:04:24 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


// pragmas
#pragma once


// includes
#include "ReaderLib.h" // defines Sec_ReaderLib
#include "Global.h"
#include <string>
#include "Volume.h"
#include "SegmentList.h"
#include "Patch.h"
#include "Date.h"
#include "vxBlockVolume.h"


// namespaces
namespace ReaderLib
{


// forward declarations
class Dataset;


// This should really be a member of the VolumePaint class.
static enum PaintMarkValsEnum { COUNTMASK = 0x20, NOTCOUNTMASK = 0x1f, SUSPICIOUSROI = 0x80, CADROI = 0x40 }; //, POLYPMARKBITS = 0x1f << 8 };

/**
 * Class to implement painting, marking and counting of visualized surfaces
 */  
class VolumePaint : public LinVolUC, Sec_ReaderLib
{
public:

  /// Constructor: 
  VolumePaint();

  /// Constructor: 
  VolumePaint::VolumePaint(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel,
                           const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0),
                           hdr::UnitTypeEnum unitType = hdr::MILLIMETER,
                           const uint1 & background = 0, hdr::ExtensionEnum ext=hdr::VOL,
                           const std::string & sFileName = "Blank.vol",
                           const std::string & sTitle    = "Blank",
                           const std::string & sAuthor   = "Blank",
                           const vx::Date & date = vx::Date());

  /// Destructor: Releases all data structures
  virtual ~VolumePaint();

  /// Resets all variables that change whenever a new dataset is loaded
  void Reset();

  /// Reads the volume specified into memory.
  void ReadData(const std::string& sFileName);

  /// Writes the volume to the specified file using the new V3D File format.
  void WriteData(const std::string& sFileName = "NULL", bool bBuildIcon = true);

  void InitializeMarkedSurfaces(SegmentList & segmentList, vxBlockVolume<uint1> & insVol);

  void MakeViewingGeometry(Dataset & dataset, Patch & thisPatch, Segment & segment, Vector<float4> vAveNormal);

  bool MakePatch(Dataset & dataset, Patch & thisPatch, Point<uint2> seedPoint, uint1* pPaintVol, BlockVolB  &maskVol);

  /// Stores a list of missed patches for, and in, the given dataset
  void CreateMissedPatchList(Dataset & dataset);

  /// I don't know, this is someone else's uncommented function.
  void Transfer3DMarking();

  /// Calculate the percent of the colon which has been examined.
  float4 GetPercentExamined() const;

private:

  /// Reads the volume from the specified file, made private for specific serialization (use ReadData)
  inline void Read (std::string sFileName) {  }

  /// Writes the volume to the specified file, made private for specific serialization (use WriteData)
  inline void Write(std::string sFileName = "NULL", bool bBuildIcon = true) { }

  /// Writes the volume to the specified file, made private for specific serialization (use WriteData)
  inline void WriteNew(std::string sFileName = "NULL", bool bBuildIcon = true) { }

public:

  /// Accumulator value for painting display, all voxels above this value displayed as "seen"
  uint4 m_uCurrentSeenVal;

  /// Total number of surface voxels.
  uint4 m_uTotalSurfaceCount;

  /// Current number of seen surface voxels.
  uint4 m_uCurrentSurfaceCount;

  /// true immediately after the paint volume have been initialized, set false when surface marking changed by flight.
  bool m_bSurfacesNeedReinit;

private:

  /// Header variable name entry for current surface count value
  static const std::string sHdrVarName_CurrentSurfaceCount;

  /// Header variable description for current surface count value
  static const std::string sHdrVarDesc_CurrentSurfaceCount;

  /// Header variable name entry for current surface count value
  static const std::string sHdrVarName_TotalSurfaceCount;

  /// Header variable description for current surface count value
  static const std::string sHdrVarDesc_TotalSurfaceCount;

}; // class VolumePaint


} // namespace ReaderLib


// $Log: VolumePaint.h,v $
// Revision 1.6.2.1  2007/06/15 20:04:24  jmeade
// comments.
//
// Revision 1.6  2007/01/26 22:33:05  dongqing
// add CAD ROI display
//
// Revision 1.5  2006/10/02 21:01:47  dongqing
// make the PaintMarkValsEnum static
//
// Revision 1.4  2006/09/20 18:51:59  dongqing
// 1st code adjustment for the Suspicious region tool
//
// Revision 1.3  2006/01/31 14:18:22  frank
// code review
//
// Revision 1.2  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/09/01 19:28:19  geconomos
// Issue# 4392: Paint volume was not saving properly. Error turned out to be in the linear volume class.
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.10.2.1  2004/10/18 02:25:07  jmeade
// Different marking color for polyp segmentation.
//
// Revision 3.10  2002/11/26 00:39:46  kevin
// Changed surface initialization logic
//
// Revision 3.9  2002/11/24 17:46:21  kevin
// Cleaned up and made sub-functions out of mising patch creation to handle large paatches now that the 90% limit is no longer utilized
//
// Revision 3.8  2002/10/29 19:37:29  kevin
// Read and Writing should not be inline functions
//
// Revision 3.7  2002/09/13 17:33:29  jmeade
// Paint volume handles all volume and value (e.g. current/total seen voxel values) serialization.
//
// Revision 3.6  2002/09/13 00:54:41  jmeade
// Move paint volume characteristics to the VolumePaint class.
//
// Revision 3.5  2002/09/12 18:24:14  jmeade
// Setup for saving session data.
//
// Revision 3.4  2002/04/12 14:51:52  jmeade
// Code standard.
//
// Revision 3.3  2002/04/12 02:20:41  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2001/12/28 19:51:54  ingmar
// moved Sec_ReaderLib to ReaderLib.h
//
// Revision 3.1  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:22   ingmar
// Initial revision.
// Revision 1.1  2001/05/07 01:40:08  kevin
// Added VolumePaint back in, somehow it got lost in the CVS debacle that occurred today
//

//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/VolumePaint.h,v 1.6.2.1 2007/06/15 20:04:24 jmeade Exp $
// $Id: VolumePaint.h,v 1.6.2.1 2007/06/15 20:04:24 jmeade Exp $
