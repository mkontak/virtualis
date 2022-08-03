// $Id: Fields.h,v 1.12.2.3 2009/01/15 04:14:09 jmeade Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "global.h"
#include "StudyLoad.h"
#include "Volume.h"
#include "VolumeRender.h"
#include "Exception.h"
#include "VolumePaint.h"
#include "vxSliceRendererData.h"
#include "vxBlockVolume.h"


// namespaces
namespace ReaderLib
{


// forward declarations
class Dataset;



///
/// Class containing fields necessary for navigation and rendering.
///
class Fields : Sec_ReaderLib
{

// member functions
public:

  /// Default Constructor
	Fields();

  /// Copy constructor
  Fields (const Fields & other);
  
  /// Assignment operator
  Fields & operator = (const Fields & fields);

  // Access functions
  // ----------------

  /// Retrieve the SLD scale factor
  inline const float4 & GetSldScale() const { return m_fSldScale; }
  
  /// Return whether or not the opacity volume is created
  inline const bool & IsOpacityVolumeCreated() const { return m_bOpacityVolCreated; }

  /// Sets whether or not the opacity volume is created
  inline void SetOpacityVolumeCreated( const bool bOpacityVolumeCreated ) { m_bOpacityVolCreated = bOpacityVolumeCreated; }

  /// Gets whether the opacity volume can be created
  inline const bool & GetOpacityVolumePossible() const { return m_bOpacityVolumePossible; }

  /// Sets whether the opacity volume can be created
  inline void SetOpacityVolumePossible( const bool bOpacityVolumePossible ) { m_bOpacityVolumePossible = bOpacityVolumePossible; }

  /// gets the label volume (create it if necessary)
  vxBlockVolume<uint2> & GetLabelVolume();

  
  /// return: whether the .ecv is loaded
  inline const bool IsEcvVolLoaded() const { return m_bEcvVolLoaded; }

  /// return: whether the .dif is loaded
  inline const bool IsDiffVolLoaded() const { return m_bDiffVolumeLoaded; }

  /// return: whether the .ecv is loaded
  inline const bool DoesDiffVolExist() const { return m_bDiffVolumeExists; }

  /// return: whether the HQ .dif is loaded
  inline const bool IsHQDiffVolLoaded() const { return m_bHQDiffVolumeLoaded; }

  inline const bool IsElecCleansed() const { return m_bIsElecCleansed; }

  // Primary functions
  // -----------------

  /// Reset all data members
  void Reset();

  /// Load space leap (both external and internal if they exis into one linear volume
  void LoadSpaceLeapFields(const std::string & sFilePrefix);

  /// Load space leap data
  void LoadSpaceLeapFields2( vxBlockVolume<uint1> * pSld );

  /// Load space leap data
  void LoadSpaceLeapFields3( const std::string & sPreprocessedPrefix );

  /// Load render fields from disk into memory except for the diff volume (which will be lazy loaded)
  //void LoadVolumeFields(const std::string & sFilePrefix);

  /// Load render fields from disk into memory except for the diff volume (which will be lazy loaded)
  void LoadVolumeFields2(const std::string & sVolumePrefix, const std::string & sPreprocessedPrefix, const bool & bRotateProneToSupine );

  void LoadVolumeFields3(const std::string & sVolumePrefix, const std::string & sPreprocessedPrefix);

	/// load volume fields from builts of 64bit preprocessing
  void LoadVolumeFields64b(const std::string & sVolumePrefix, const std::string & sPreprocessedPrefix);

	/// load in the intensity block volume only
  void CreateVolumeOnly( vxBlockVolume<uint2> * pv, const std::string & sImageOrientation );

	/// create CDV, 8bits intensity, and INS volumes
  void CreateCDV( vxBlockVolume<uint2> * pv, uint2 * plv, uint1 * pIns, 
	                const vxVolumeHeader & vHeader, const std::string & sImageOrientation );

  /// set the scale factor for SLD
  void SetScaleFacter4SLD( const vxVolumeHeader & vHeader );

	/// Load the Diff Volume only when it is needed
  void LazyLoadDiffVolume();
  
  /// Create the research/trial volume ONLY if a single dataset
  bool LazyCreateTestVolume();

  /// Get the closest point on the Skeleton for the given position
  uint2 GetClosestSkeletonIndex( const Point<float4> & volumePosition ) const;

  /// Get the nearest CSI to the given point
  const bool FindClosestValidCSI(const Point<uint2> & startPositionPoint, const int2 iThisSize, const int2 iMax, Point<uint2> & foundPoint) const;

  /// Get the gradient direction of the appropriate potential field
  //Vector<float4> GetResampledPotentialGradient(const Point<float4> & positionVolumePoint, const bool bFromStart);
  
  /// Search either of the potential fields and return a gradient vector which points in the direction of smaller values
  Vector<float4> GetMinimumPotentialDirection(const Point<float4> & positionVolumePoint, const bool bFromStart);

private:

  /// not implemented - do not call!!!
  bool operator ==( const Fields & ) const { return false; }

  /// not implemented - do not call!!!
  bool operator !=( const Fields & ) const { return false; }

// data members
public:

  /// The cleansed volume dataset
  VolumeRender m_volumeRender;
  
  /// Tells whether or not each voxel has been viewed yet during this exam
  VolumePaint m_paintVol;

  /// The potential distance from the starting point
  vxBlockVolume<float4> m_dfs;

  /// The potential distance from the ending point
  vxBlockVolume<float4> m_dfe;

  /// The closest skeleton index field
  mutable vxBlockVolume<uint2> m_csi;

  /// The interior spaceleap distance used to accelerate volume rendering
  LinVolUC m_sldInterior;

  /// The space leap distance for all exterior voxels
  LinVolUC m_sldExterior;

  /// The pre-computed opacity volume
	LinVolUC m_opacityVol;

  /// The inside volume
  vxBlockVolume<uint1> m_ins;

  /// The ecv-vol difference volume
  vxBlockVolume<uint1> m_diffVolume;

  /// A usually unused volume for research/trial ideas. Only valid on m_bTestVolValid
  LinVolUC m_testVolumeUC;
  bool m_bTestVolValid;

  /// vxSliceRendererData
  vxSliceRendererData m_sliceRendererData;

  /// 2D slice volume dimensions
  Triple<int4> m_sliceDimensions;

  /// The units of the 2D volume
  Triple<float4> m_units2D;

  /// Default window (first)/level (second)
  std::pair<int4,int4> m_defaultWL;

  /// Window/level for different volume view option
  std::pair<int4,int4> m_secondaryWL;

  /// Shift offset for converting Viatronix world coordinate to the DICOM world coordinate
  Vector3D<float4> m_vfShiftOffsetForDICOMWorldCoordinate;

  /// patient orientation while imaging
  std::string m_sPatientOrientation;

private:

  /// A factor used to control the sld values
  float4 m_fSldScale;

  /// Whether or not the opacity volume is created
  bool m_bOpacityVolCreated;

  /// Whether or not the .ecv volume is loaded
  bool m_bEcvVolLoaded;

  /// Flag whether the Diff Vol Exists or not
  bool m_bDiffVolumeExists;

  /// Flag whether the Diff Vol is loaded yet
  bool m_bDiffVolumeLoaded;

  /// Flag whether the HQ Diff volume is loaded
  bool m_bHQDiffVolumeLoaded;

  /// DiffVolume File to Lazy Load
  std::string m_sDiffVolFilename;

  /// if we have the memory left to create the opacity volume
  bool m_bOpacityVolumePossible;

  /// Whether electronic cleansing was run on the dataset 
  bool m_bIsElecCleansed;

  /// the label volume (dialated form of the ins volume)
  vxBlockVolume<uint2> m_labelVolume;

}; // class Fields


// namespaces
} // namespace ReaderLib


// $Log: Fields.h,v $
// Revision 1.12.2.3  2009/01/15 04:14:09  jmeade
// const modifier on method
//
// Revision 1.12.2.2  2008/02/28 19:19:09  dongqing
// the DICOM RCS convertion using Image Orientation Patient Cosine
// only now. The HFS stuff is no longer used for that purpose.
//
// Revision 1.12.2.1  2008/01/02 22:09:39  dongqing
// convert world coordinate display from Viatronix to Standardized DICOM
//
// Revision 1.12  2006/09/13 15:19:01  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.11  2006/06/26 12:58:47  frank
// creates a label volume out of the dilated ins volume
//
// Revision 1.10  2006/06/22 15:20:24  dongqing
// Get rid of reading SLV file
//
// Revision 1.9  2006/06/22 14:01:24  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.8  2006/06/01 20:25:16  frank
// updated to visual studio 2005
//
// Revision 1.7  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.6  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5.2.1  2005/11/09 01:37:13  jmeade
// Issue 4042: Check cdv file for flag indicating if study processed with EC.
//
// Revision 1.5  2005/09/29 15:01:35  vxconfig
// switched dfs and dfe volumes to new vx block volume
//
// Revision 1.4  2005/09/29 14:11:46  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.3  2005/09/29 13:33:13  vxconfig
// made the difference volume a new vx block volume
//
// Revision 1.2  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.10.2.3.2.3  2005/04/22 16:48:16  frank
// cleaned up field class interface
//
// Revision 3.10.2.3.2.2  2005/04/08 22:37:58  jmeade
// Issue 4042: Store whether hq diff volume loads.
//
// Revision 3.10.2.3.2.1  2005/04/01 17:33:33  frank
// Issue #4094: Overview will now try to create the transparent volume, if that fails, it will simply use triangles from that point forward.
//
// Revision 3.10.2.3  2005/02/15 18:04:25  jmeade
// merge from vc1.4 branch.
//
// Revision 3.10.2.2.2.3  2005/02/04 23:52:49  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.10.2.2.2.2  2004/11/17 21:55:47  jmeade
// Separate W/L for pre and post-contrast studies.
//
// Revision 3.10.2.2.2.1  2004/11/16 16:30:20  jmeade
// _16bit ==> _hq; Default window/level read from hq volume header (if there).
//
// Revision 3.10.2.2  2004/04/23 15:33:14  geconomos
// Initial addition of merging in newer 12-bit slice renderers.
//
// Revision 3.10.2.1  2003/03/26 23:08:37  jmeade
// Issue 2849:  Removed unnecessary non-const pointer parameter.
//
// Revision 3.10  2002/11/24 17:51:56  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.9  2002/04/12 02:12:21  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.8  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7.2.1  2002/02/25 16:04:32  kevin
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
// Revision 3.7  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.6  2002/01/03 02:29:08  kevin
// Removed smallINS volue and clean up
// Also fixed TTP rendering noew that set, ses and sec are gone
//
// Revision 3.5  2001/12/28 19:51:54  ingmar
// moved Sec_ReaderLib to ReaderLib.h
//
// Revision 3.4  2001/11/06 15:26:32  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.3  2001/11/02 16:13:50  binli
// 2D render from block vol (.diff)
//
// Revision 3.2  2001/11/01 19:52:59  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.1  2001/11/01 17:10:38  binli
// 2D render from one vol. (.ecv]
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 10 2001 11:55:32   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.1   Oct 03 2001 20:06:22   jmeade
// User load abort should throw AbortLoadException, not AbortOperationEx; spelling errors in exception comment
// 
//    Rev 2.0   Sep 16 2001 23:40:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:00   ingmar
// Initial revision.
// Revision 1.36  2001/08/24 20:03:20  frank
// Synchronized exceptions.
//
// Revision 1.35  2001/06/26 16:21:38  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.34  2001/05/31 21:56:48  binli
// bug 000484 (step1: distance): use current study, not rdrGlobal.m_pStudy.
//
// Revision 1.33  2001/05/19 00:41:04  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.32  2001/05/07 00:51:26  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.31  2001/05/02 20:12:10  ingmar
// BookmarkList.C
//
// Revision 1.30  2001/04/30 11:37:54  frank
// Added exception handling.
//
// Revision 1.29.2.1  2001/05/01 23:41:24  jmeade
// m_volume changed to m_volumeRender (variable names for instances of
// user-defined classes should explicitly denote class) (April 26 - jmeade)
//
// Revision 1.29  2001/04/18 19:11:37  liwei
// m_pOpacityVol changed to m_opacityVol
//
// Revision 1.28  2001/04/13 13:01:39  frank
// Moved slice loading to Fields.
//
// Revision 1.27  2001/04/13 11:53:39  frank
// Protected some members of Fields.
//
// Revision 1.26  2001/04/11 18:17:09  frank
// Renamed a csi search function.
//
// Revision 1.25  2001/04/11 17:09:51  frank
// Cleaned up pdr searching/gradient functions.
//
// Revision 1.24  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.23  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.22  2001/04/10 17:51:42  frank
// Code walkthrough changes.
//
// Revision 1.21  2001/03/19 15:29:55  frank
// Cleaned up header.
//
// Revision 1.20  2001/03/19 12:00:22  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.19  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.18  2001/01/29 22:15:50  liwei
// Attempted to move the opacity volume to study or fields, not finished yet
//
// Revision 1.17  2001/01/27 02:02:01  kevin
// 1) Fixed 90% missed Patch error
// 2) Moved GetPotenetil to fields from Submarine
// as part of this fix.
//
// Revision 1.16  2001/01/24 19:18:19  liwei
// Software volume rendering for overview
//
// Revision 1.15  2001/01/11 13:08:44  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.14  2000/11/25 03:04:57  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.13  2000/11/20 19:04:44  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.12  2000/11/17 18:25:52  frank
// Added Reset functions
//
// Revision 1.11  2000/11/16 15:26:28  jeff
// LoadCallback class to keep track of study load state information
//
// Revision 1.10  2000/11/02 17:35:47  kevin
// Changed to "Search"CSI with a sprial iterator
//
// Revision 1.9  2000/10/29 17:41:54  kevin
// Added GetClosestSkeletonINdex to a given point
// instead of the current viewpoint
//
// Revision 1.8  2000/10/27 00:33:29  kevin
// Changed ins to linear so it reads faster
//
// Revision 1.7  2000/10/11 15:40:36  kevin
// Added Notify for all Fields
//
// Revision 1.6  2000/09/28 20:46:10  frank
// Replaced DFB with SLD
//
// Revision 1.5  2000/09/21 19:43:54  kevin
// BLock .vs. Hash
//
// Revision 1.4  2000/09/12 20:42:45  kevin
// Switched back to Block volumes (including .ins)
//
// Revision 1.3  2000/09/12 00:02:26  kevin
// CHanged back to Hash due to "grey screen" bug
// even though block uses significantly less memory
//
// Revision 1.2  2000/09/11 18:06:55  frank
// Changed from Hash volumes to Block volumes
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 21    9/05/00 1:34a Ingmar
// now works with new naming
// 
// 20    8/28/00 12:17p Antonio
// updated header and footer
//
//*****************  Version 19  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 18  *****************
//User: Kevin           Date:  8/07/00  Time:  6:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed CSI problems with new segment data structures
//
//*****************  Version 17  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 16  *****************
//User: Kevin           Date:  8/04/00  Time:  4:43p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added inside file
//
//*****************  Version 15  *****************
//User: Kevin           Date:  8/02/00  Time:  1:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Aligned centerline with polygons and cleaned up overview a little
//
//*****************  Version 14  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 13  *****************
//User: Kevin           Date:  8/01/00  Time: 10:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Code Cleanup after changeing to new file format
//
//*****************  Version 12  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 11  *****************
//User: Kevin           Date:  7/26/00  Time:  8:51p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added the use of .sld (space lead distance" file. But it did not
//work, so commented it out (left it in though until v2000 gets it computed
//correctly)
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/26/00  Time:  4:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added DFS and CSI files and support
//
//*****************  Version 9  *****************
//User: Frank           Date:  7/26/00  Time: 10:15a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Cleaned out extra functions
//
//*****************  Version 8  *****************
//User: Frank           Date:  7/26/00  Time:  8:17a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Cleaned out old functions and updated header
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/25/00  Time: 10:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added validity testing for position
//
//*****************  Version 6  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 5  *****************
//User: Frank           Date:  7/20/00  Time:  1:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added PDS field using new Volume class
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/17/00  Time:  4:19a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Improved the performance of ray casting by 16%
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Fields.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Fields.h,v 1.12.2.3 2009/01/15 04:14:09 jmeade Exp $
// $Id: Fields.h,v 1.12.2.3 2009/01/15 04:14:09 jmeade Exp $
