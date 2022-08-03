// $Id: ReaderGlobal.h,v 1.12.2.1 2010/12/20 19:06:05 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)
//
// Complete History at bottom of file.


#if !defined(ReaderGlobal_h)
#define ReaderGlobal_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ReaderLib.h" // defines Sec_ReaderLib

#include "Typedef.h"
#include "TrackBall.h"
#include "Dataset.h"
#include "StudyLoad.h"
#include "Exception.h"
#include "Hazards.h"
#include "MatchingFeature.h"
#include "MatrixNxM.h"
#include "VCSettings.h"
#include "StudyLoadParams.h"
#include <memory>

namespace ReaderLib
{

/**
 *
 * Contains all the information related to a particular patient.  These
 * variables are available application-wide.  
 */
class ReaderGlobal : Sec_ReaderLib
{
// Member Functions
public:

  /// Constructor.
  ReaderGlobal();

  /// Destructor.
  virtual ~ReaderGlobal();

  /// Reset to default all the members.
  void Reset();
  
  /// FreeAllLargeVolumesButPaint.
  void FreeAllLargeVolumesButPaint();
  
  bool LoadStudy2( StudyLoadParams & params, StudyLoad::LoadCallback loadCallback );

  /// Save any modified data to disk.
  bool SaveData(bool bSavePaintVolumeData = true);

  /// Triggers abort-study-load process
  void AbortLoad() { m_bAbortLoad = true; }

  /// Returns true if one or more of supine & prone were detected and are finished loading.
  inline bool IsLoadingComplete() const;

  /// Returns which datasets are loading while LoadStudy() fn is active, eDEFAULT otherwise
  inline DatasetOrientations::TypeEnum IsStudyLoading() const;

  /// sets the datset loading type
  inline void SetStudyLoading( DatasetOrientations::TypeEnum eOrientation );

  /// Returns true if the dataset is loading based on orientation
  inline bool IsDatasetLoading( DatasetOrientations::TypeEnum eOrientation ) const;

  /// Swap supine and prone.
  void SetDatasetOrientation(const DatasetOrientations::TypeEnum eDesiredOrientation);
    
  /// Have both supine and prone datasets been detected.
  inline bool IsDualDataDetected() const;

  /// Are both supine and prone datasets loaded.
  inline bool IsDualDataLoaded() const;

  /// Retrieve any pre-processing hazards.
  std::list<Hazards::HazardEnum> GetHazards(const std::string & sFilePrefix) const;

  /// Gets a GUID for the specified Codec
  static bool GetCodecClsid(const WCHAR* sFormat, CLSID& codecCLSID);
  /// Writes a JPEG image to the specified file
  static void WriteJPEGImage(uint2 uWidth, uint2 uHeight, uint1* puPixels, BSTR sFile);

  /// Updates the world location of the destination dataset given that of the (registered) source dataset
  void UpdateWorldPosition(const Dataset& sourceDataset, Dataset& destDataset) const;

  /// Called to setup default zoom factor for overview rendering.
  void SetupZoomFactorInOverview(const Triple<uint4> & vDim, const Triple<float4> & vUnits);

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void BuildMatchQuality();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeEuclideanDistanceMatrix();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeOutVectorMatrix();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeInVectorMatrix();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeInCrossOutMatrix();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeHorizConfidence();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeVertConfidence();

  /// ReaderGlobalRegistration: Build the Seed Matrices
  void ComputeConfidence();

  /// ReaderGlobalRegistration: The entry point into Supine/Prone Registration
  void Register();

  /// ReaderGlobalRegistration: Computes the centerline confidence accordingly with the seeds confidence
  void ComputeCenterlineConfidence();

  /// ReaderGlobalRegistration: Computes the indexes into the other centerline for the current registration
  void ComputeRegistrationTranslations();

  /// ReaderGlobalRegistration: Maps the registration quality to a weight.
  float4 ColorFunction(float4 fCorrelation);
  
  /// ReaderGlobalRegistration: This is the recursive registration function to register 2 sub-sections of the colon
  std::vector<MatchingFeature> FindSeeds(const MatchingFeature startFeature, const MatchingFeature stopFeature,
                                         const int4 iStartRow, const int4 iStopRow, const int4 iStartCol, const int4 iStopCol,
                                         const uint4 uLevel);

  /// ReaderGlobalRegistration: Given 2 endpoints compute the stretch of the colon at some potential match point between them
  float4 Stretch(const MatchingFeature startFeature, const MatchingFeature stopFeature,const MatchingFeature potentialMatchFeature);

  /// ReaderGlobalRegistration: Given 2 featyures compute deicde if a feature is not too close
  bool FeaturesNotTooClose(const MatchingFeature feature1, const MatchingFeature feature2);
  
  /// Are the two datasets registered?
  bool IsRegistered();
    
protected:
private:

// Data Members
public:
  template <class T> class SetResetVar
  {
  public:
    SetResetVar(T& var, const T& onval = T(1), const T& offval = T(0))
      : m_var(var), m_offval(offval) { m_var = onval; }
    ~SetResetVar() { m_var = m_offval; }
  private:
    T& m_var;
    const T m_offval;
  };

  /// The currently viewing dataset.
  Dataset * m_pCurrDataset;

  /// The "other" dataset.
  Dataset * m_pSecondaryDataset;

  /// The scan taken with the patient on his back
  Dataset m_supine;

  /// The scan taken with the patient on his belly
  Dataset m_prone;

  /// A blank dataset for freeing up stuff
  static Dataset m_nullDataset;

  /// Supine/Prone lock status.
  bool m_bSPLocked;

  /// The offset between supine and prone when slices are linked
  Triple<int4> m_vProneOffset;

  /// Read only indicator
  bool m_bReadOnly;

  /// Flag to abort study loading.
  bool m_bAbortLoad;

  /// Flag to indicate the study loading was actually aborted.
  bool m_bLoadAborted;

  /// The full path of the volume study dataset.
  std::string m_sVolumeTitlePath;

  /// Both supine and prone datasets have been detected.
  bool m_bDualDataDetected;
  
  /// Both supine and prone segments are available for verification.
  bool m_bDualSegmentsLoaded;
  
  /// Both supine and prone data are completed loaded
  bool m_bDualDataLoaded;
  
  ///////////////////////////////////////////////////////////////
  // Flags to control the rendering of different features      //
  ///////////////////////////////////////////////////////////////

  /// Toggle biopsy mode.
  bool m_bBiopsyMode;
  
  /// Toggle space leaping.
  bool m_bSpaceLeaping;
  
  /// Render the Painted information
  bool m_bRaycastPaintVol;
  
  /// Toggle the gray Oblique slice shadow in the navigation view.
  bool m_bObliqueShadow;
  
  /// Toggle the oblique 2D view to be perpinducular to view or centerline
  bool m_bAlignCrossSectional;
  
  /// Toggle the colored orthogonal shadows in the navigation view.
  bool m_bOrthogonalShadow;
  
  /// Toggle the colored axial shadows in the navigation view.
  bool m_bAxialShadow;
  
  /// Toggle the colored sagittal shadows in the navigation view.
  bool m_bSagittalShadow;
  
  /// Toggle the colored coronal shadows in the navigation view.
  bool m_bCoronalShadow;

  /// Toggle the colored shadows which represent the 3DSegment half-plane in the navigation view.
  bool m_bSegmentHalfPlaneShadow;

  /// Toggle the grid shaodow
  bool m_bGridShadow;

  /// Whether report has been created.
  bool m_bReportCreated;

  /// Whether to display the 4-voxels in a row info (From Dr. Brewington).
  bool m_bHomogenityCreated;
  bool m_bShowHomogenity;

	/// whether to use the ULD volume for improving the 3D endo fly performance
	bool m_bUseULDvolume;

	/// whether to enable dual display tabs
	bool m_bEnableDualDisplayTabs;

  /// Normal or Colorblind rendering colors
  int4 m_iColorScheme;

  /// The Seed matrices.
  MatrixNxM m_mEuclideanDistances;
  
  /// The Seed matrices.
  MatrixNxM m_mInVectors;
  
  /// The Seed matrices.
  MatrixNxM m_mOutVectors;
  
  /// The Seed matrices.
  MatrixNxM m_mCrossProducts;
  
  /// The Seed matrices.
  MatrixNxM m_mMatchQuality;
  
  /// The Seed matrices.
  MatrixNxM m_mHorizConfidence;
  
  /// The Seed matrices.
  MatrixNxM m_mVertConfidence;
  
  /// The Seed matrices.
  MatrixNxM m_mConfidence;

  /// The Output Matching Features (although the translations in SkeletonNodeVCExt are actually used to jump back and forth).
  std::vector<MatchingFeature> m_vRegMatchingFeatures;
  
  /// Mean of all values in the m_mMatchQuality Matrix.
  float4 m_fRegistrationMatchMean;
  
  /// StdDev of all values in the m_mMatchQuality Matrix.
  float4 m_fRegistrationMatchStdDev;

  /// Registration centerline confidence display parameter.
  float4 m_fRegistrationQualityCenterlineColorBlendTransitionPoint;

  /// Registration centerline confidence display parameter.
  float4 m_fRegistrationQualityCenterlineColorBlendStrength;

  /// flag to indicate the bookmarks are showing all the registration features.
  bool m_bAllFeatures;

  /// flag to indicate the bookmarks are showing the matched features.
  bool m_bMatchedFeatures;

  /// whether or not to display any cad findings
  bool m_bDisplayCadFindings;

  /// whether to display cad arrows in 2D/3D regardless of display-cad-findings flag
  bool m_bAlwaysShowCadArrows;

  /// whether to display extra-colonic cad findings
  bool m_bShowExternalCadFindings;

  /// whether or not to display any SRT ROIs
  bool m_bDisplaySRTroi;
	bool m_bDisplaySRTroi2D;

  /// Gets the directory path of the executable
  inline std::string GetModulePath();

  /// get the settings - a singleton
  inline const VCSettings * GetSettings();

	/// flag for the toolbar position for wide screen
	bool m_bToolbarOnBottomForWideScreen;

	/// flag for the display ruler in all 2D views
	bool m_bDisplay2DRuler;

	/// flag for the synchronized 3D endo-fly for both datasets
	bool m_bDual3DEndoFly;

	/// flag for the snapshot size standardization. If it is true, all snapshots will resize to 512x512.
	bool m_bNormalizeSnapshotDim;

private:

  /// Set to which datasets are loading while LoadStudy() fn is active, eDEFAULT otherwise
  DatasetOrientations::TypeEnum m_eIsLoading;

}; // class ReaderGlobal

//////////////////////////////////////////////////////////////////////
// Defining and exporting the one and only Global object
#ifdef READER_GLOBAL_OBJECT_DEFINED_LOCAL
ReaderGlobal rdrGlobal;
#else
extern ReaderGlobal rdrGlobal;
#endif

#include "ReaderGlobal.inl"

} // namespace ReaderLib

#endif // !defined(ReaderGlobal_h)

// $Log: ReaderGlobal.h,v $
// Revision 1.12.2.1  2010/12/20 19:06:05  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12  2006/12/21 16:22:22  dongqing
// Add the switch for display SRT ROI on/off
//
// Revision 1.11  2006/12/05 20:13:45  jmeade
// Issue 5047: get module path function.
//
// Revision 1.10  2006/10/04 20:52:37  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.9  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.8  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.7  2005/11/22 16:38:40  jmeade
// movie creation reflecting new directory structure.
//
// Revision 1.6  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5.2.1  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.5  2005/10/13 18:11:31  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.4  2005/10/12 22:38:37  jmeade
// option to always show cad findings arrows in 3D
//
// Revision 1.3  2005/10/04 17:33:11  jmeade
// display cad findings study-wide global variable.
//
// Revision 1.2  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
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
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.23.2.2.4.3.2.1  2005/07/13 17:27:53  geconomos
// Added method to indicate if the two datsets have been registered
//
// Revision 3.23.2.2.4.3  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.23.2.2.4.2.2.1  2005/05/25 15:50:21  geconomos
// initial backup
//
// Revision 3.23.2.2.4.2  2005/04/13 18:23:37  jmeade
// Issue 4117: Identify which datasets are loading.
//
// Revision 3.23.2.2.4.1  2005/03/23 22:16:37  frank
// Issue #3983: Revamped the slice linking for all ortho directions
//
// Revision 3.23.2.2  2003/05/01 16:58:26  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.23.2.1  2003/03/26 23:18:53  jmeade
// Issue 2849:  Combined duplicate register-update methods, added check to ensure jump within destination skeleton.
//
// Revision 3.23  2003/02/05 18:32:50  kevin
// Make Registration Debug Global
//
// Revision 3.22  2002/11/24 17:50:34  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.21  2002/11/14 19:34:37  kevin
// Don't set detectecd features too close to user features
//
// Revision 3.20  2002/11/11 14:32:07  kevin
// First attempt at making the thermometer match the centerline color
//
// Revision 3.19  2002/10/31 20:03:18  kevin
// 1) Free up memroy on close to allow paint vol to be written to linear-vol file
// 2) Add load step so post-load init is always at the end even for both memory configurations
//
// Revision 3.18  2002/10/23 00:24:09  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 3.17  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.16  2002/09/29 21:44:15  kevin
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
// Revision 3.15  2002/09/11 19:16:34  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.14  2002/08/28 19:23:37  kevin
// Lots of updates to segmental viewing
//
// Revision 3.13  2002/07/18 19:56:29  jmeade
// Coding standards, clarified a variable name/meaning.
//
// Revision 3.12  2002/07/15 15:49:51  jmeade
// Coding standards; variables localized to where appropriate.
//
// Revision 3.11  2002/06/06 19:13:09  suzi
// Major refactoring job.
//
// Revision 3.10  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.9  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.8  2002/04/12 02:13:35  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.7  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.6.2.6  2002/05/31 03:04:13  jmeade
// Issue 2331:  Expose (declare as static) functions for snapshot.
//
// Revision 3.6.2.5  2002/04/02 16:59:36  jmeade
// Issue 2026: Allow open of single dataset from Study Info.
//
// Revision 3.6.2.4  2002/02/27 18:59:01  kevin
// Added user message Box for large studies (Note: had to pass
// mesage back to main thread to get the box displayed from
// the worker load thread)
//
// Revision 3.6.2.3  2002/02/27 15:29:45  binli
// issue 1699: refixed (refer Study.C Revision 3.13.2.4)
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
// Revision 3.6.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.6  2001/12/29 00:13:22  jmeade
// Merge from 1.1 branch.
//
// Revision 3.5  2001/12/28 19:51:54  ingmar
// moved Sec_ReaderLib to ReaderLib.h
//
// Revision 3.4  2001/12/12 20:03:09  jmeade
// Made SetResetVar helper class accessible to others.
//
// Revision 3.3  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.2  2001/11/05 17:52:26  ana
// Registration view added.
//
// Revision 3.1  2001/10/29 14:13:42  kevin
// Added Jpeg Picture saving and tidied up avi/mpeg generation
//
// Revision 3.0.2.1  2001/11/14 02:02:27  jmeade
// Comments.
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 03 2001 20:06:22   jmeade
// User load abort should throw AbortLoadException, not AbortOperationEx; spelling errors in exception comment
// 
//    Rev 2.2   Oct 03 2001 11:12:22   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar : added a flag variable.
// 
//    Rev 2.1   Oct 01 2001 16:10:10   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:40:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:04   ingmar
// Initial revision.
// Revision 1.115  2001/08/31 18:57:23  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.114  2001/08/24 20:03:20  frank
// Synchronized exceptions.
//
// Revision 1.113  2001/06/27 18:22:58  jmeade
// bool variable set only when LoadPatient() fn is active, for querying with IsLoading() fn
//
// Revision 1.112  2001/06/06 20:16:15  jmeade
// LoadPatient() throws exceptions (file not found so far)
//
// Revision 1.111  2001/05/25 20:28:58  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.108  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.107  2001/05/09 18:34:47  liwei
// variable ReaderGlobal::m_bInteractingOverview deleted
// Unnecessary raycasting in translucent mode avoided.
//
// Revision 1.106  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.105  2001/04/30 02:20:02  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.104  2001/04/18 15:47:59  frank
// Output stream class has buggy destructor...removed console window.
//
// Revision 1.103  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.102  2001/04/11 12:09:10  frank
// Worked on implementing the new output streams.
//
// Revision 1.101  2001/04/07 04:45:30  jmeade
// Bug: Interface reporting error on user abort.  Fix:  Better throw and catch
// of abort exception in ReaderGlobal, Study, Fields
//
// Revision 1.100  2001/03/26 16:09:09  frank
// Fixed typo.
//
// Revision 1.99  2001/03/26 16:06:48  frank
// Added OpenGL helper shortcut functions and console output.
//
// Revision 1.98  2001/03/20 18:41:12  frank
// Added function to eliminate redundant code.
//
// Revision 1.97  2001/03/20 15:57:00  frank
// Updated to reflect new coding standards.
//
// Revision 1.96  2001/03/19 11:59:43  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.95  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.94  2001/02/28 17:54:16  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.93  2001/02/28 16:39:07  geconomos
// Added read only flag
//
// Revision 1.92  2001/02/27 17:59:17  jmeade
// Missed regions list (m_patchList) a member of Study class
//
// Revision 1.91  2001/02/09 12:39:27  frank
// Globalized ROI measurement list.
//
// Revision 1.90  2001/02/06 21:43:55  jeff
// clarified segment jump operations
//
// Revision 1.89  2001/02/05 19:30:06  frank
// Moved hazard strings into string table.
//
// Revision 1.88  2001/02/05 13:30:28  frank
// Made a single global null study.
//
// Revision 1.87  2001/02/01 16:34:16  geconomos
// Added accecors to get current segment from an offset
//
// Revision 1.86  2001/02/01 12:54:43  frank
// Went with standard template library strings.
//
// Revision 1.85  2001/01/31 22:18:49  geconomos
// Added GetCurrentColonSegment() and ColonSegment enum
//
// Revision 1.84  2001/01/31 19:59:44  frank
// Displays pre-processing warnings during study load.
//
// Revision 1.83  2001/01/31 18:15:32  jeff
// Removed MFC dependencies in missed patch list calculation
//
// Revision 1.82  2001/01/29 02:39:06  frank
// Added histogram rendering.
//
// Revision 1.81  2001/01/26 19:59:00  binli
// defect 000188: default Overview position: added bool variable m_bResetTrackball
//
// Revision 1.80  2001/01/26 02:46:47  jeff
// dual data detected/loaded, dual segments loaded query functions
//
// Revision 1.79  2001/01/26 01:57:23  jeff
// added IsLoadingComplete(), a more generic single-or-dual study query function
//
// Revision 1.78  2001/01/25 15:32:01  frank
// Added DualSegmentAvailable flag.
//
// Revision 1.77  2001/01/25 00:04:39  jeff
// m_bDualDataAvailable ==> m_bDualDataDetected;
// hey Frank, quit moving m_bDualDataDetected = true, I need it set ASAP! :)
//
// Revision 1.76  2001/01/24 15:55:19  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.75  2001/01/24 01:00:18  jeff
// for safekeeping, m_bDualDataAvailable made private, with a query function
//
// Revision 1.74  2001/01/23 19:47:13  frank
// Exposed information about the availability of both supine and prone datasets.
//
// Revision 1.73  2001/01/23 19:29:14  binli
// improved the visibility of arrow.
//
// Revision 1.72  2001/01/23 17:44:13  jeff
// enums should be capitalized; SetStudyOrientation() - more generalized than SwapStudy()
//
// Revision 1.71  2001/01/20 17:13:36  kevin
// Made the OnBothVOlumeLoadedAndDoneVerification
// be it's own thread so that the user can interact with
// the system and status is printed.
//
// Revision 1.70  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.69  2001/01/18 17:45:42  binli
// removed 'trackball' defined in the class
//
// Revision 1.68  2001/01/12 21:48:15  frank
// Made parameter non-const.
//
// Revision 1.67  2001/01/12 19:53:16  frank
// Moved code from NavigationView
//
// Revision 1.66  2001/01/11 15:29:33  frank
// Moved static member elsewhere.
//
// Revision 1.65  2001/01/11 13:08:45  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.64  2001/01/09 18:23:28  frank
// Animated centerline.
//
// Revision 1.63  2001/01/08 16:41:50  frank
// Added function to test for secondary dataset.
//
// Revision 1.62  2001/01/05 20:13:49  frank
// Added Supine/Prone swap function.
//
// Revision 1.61  2001/01/05 19:27:39  frank
// Moved marking functions into Study class.
//
// Revision 1.60  2000/12/27 17:50:23  kevin
// CHanges to remove Mprviewer
//
// Revision 1.59  2000/12/19 02:30:34  kevin
// Added Grid Shadow
//
// Revision 1.58  2000/12/18 23:38:40  kevin
// Removed 2D unwrap-painting and clenaed up it's memory and flags.
// (NOTE; This seemed to be causing a weird memory crash where
// there was memory, but the call to new was creating an out-of-mem
// exception and there was plenty of memory left. Since this code never
// caused problems before, I am concerned that this is really some
// other problem rearing it's ugly head)
//
// Revision 1.57  2000/11/22 12:37:05  frank
// Added study load aborting
//
// Revision 1.56  2000/11/21 15:39:05  frank
// Fixed overview synchronization problem
//
// Revision 1.55  2000/11/20 19:05:19  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.54  2000/11/20 14:35:54  frank
// Added function to quickly select among different segments
//
// Revision 1.53  2000/11/20 03:36:19  kevin
// Switched to "paint volume" schema.
//
// Revision 1.52  2000/11/19 23:47:05  kevin
// Changed RGBvol to PaintVol to reflect new meaning
//
// Revision 1.51  2000/11/17 19:13:21  frank
// Improved arrow rendering
//
// Revision 1.50  2000/11/17 14:32:18  frank
// Fixed arrow direction
//
// Revision 1.49  2000/11/16 19:48:01  jeff
// SystemInfo class now stores number of processors
//
// Revision 1.48  2000/11/16 18:49:48  frank
// Added RenderArrow function
//
// Revision 1.47  2000/11/16 15:25:22  jeff
// LoadCallback class to keep track of study load state information
// Number of processors data member in ReaderGlobal, used to determine
//  whether to use threads for rendering
//
// Revision 1.46  2000/11/15 14:03:36  frank
// Moved some functions to inline file
//
// Revision 1.45  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.44  2000/11/14 18:35:37  binli
// added * assignment function
//
// Revision 1.43  2000/11/14 04:38:21  kevin
// Changed marking and painting to utilize bits within the ins vol
//
// Revision 1.42  2000/11/13 20:01:08  frank
// Barlett filtered smoothing algorithm
//
// Revision 1.41  2000/11/13 19:15:34  binli
// 1. moved some (Supine/Prone related) stuffs from 'ReaderGlobal' to 'Study'.
// 2. adjusted lock/unlock controls in VerifyView.
//
// Revision 1.40  2000/11/13 12:12:17  frank
// Added skeleton smoothing function
//
// Revision 1.39  2000/11/10 22:35:52  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.38  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.37  2000/11/09 17:41:59  frank
// Added reading for the secondary study
//
// Revision 1.36  2000/11/09 14:52:08  frank
// Renamed functions
//
// Revision 1.35  2000/11/09 02:55:03  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.34  2000/11/08 21:15:32  jeff
// Put volume header info into reader global (so as to be accessible elsewhere)
//
// Revision 1.33  2000/11/08 20:05:58  frank
// Added m_secondary to complement primary m_study
//
// Revision 1.32  2000/11/08 19:45:50  frank
// Added m_supine and m_prone to Study and resolved upon Load
//
// Revision 1.31  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.30  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.29  2000/11/08 03:06:07  frank
// Marked study and application global members
//
// Revision 1.28  2000/11/07 15:52:55  frank
// Added empty Study data member
//
// Revision 1.27  2000/11/04 00:52:19  jeff
// Added MultiMediaReport, ReportEntry
//
// Revision 1.26  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
// Revision 1.25  2000/11/03 19:57:03  liwei
// Using hardware blending for 2D marking
//
// Revision 1.24  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.23  2000/10/31 23:51:14  jeff
// Reset() methods, encapsulating all the initializing done in constructor,
// now also called prior to load
//
// Revision 1.22  2000/10/31 14:37:40  frank
// Merged in new segment structure
//
// Revision 1.21  2000/10/29 19:42:04  kevin
// Added X-Sec alignment functionality
//
// Revision 1.20  2000/10/29 17:42:47  kevin
// FIxed up visible surface determination with bug fixes
// and additoin of missed region detection
//
// Revision 1.19  2000/10/27 01:13:49  kevin
// Lots of changes to Marking (and cleaned up volume measurement a lttile bit)
//
// Revision 1.18  2000/10/26 20:46:38  jeff
// Added class for storing 2D and 3D endoscopic measurements
//
// Revision 1.17  2000/10/25 17:41:18  jeff
// Save(), GetCurrentDistanceFromRectum() functions
//
// Revision 1.16.2.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.16  2000/10/20 17:14:20  kevin
// Added debug for load timing (and changed the organization
// of some of the different projection methods)
//
// Revision 1.15  2000/10/12 21:05:04  kevin
// Added Axial, Sagittal and Coronal Flags. Also added direcitonal
// flag to semgnet jumping
//
// Revision 1.14  2000/10/12 02:46:31  kevin
// Added start at Cecum end (as weel as start at Rectum end)
//
// Revision 1.13  2000/10/11 15:01:20  kevin
// Changes for rdrGloabl current seg/skel pointers and removed
// StudyLoad
//
// Revision 1.12  2000/10/05 00:01:27  jeff
// Added an info read step (in Load() fn), for volume header read
//
// Revision 1.11  2000/10/04 14:27:04  kevin
// Added  rdrGlobal units and dim for fast
// reading for study load speed
//
// Revision 1.10  2000/10/03 00:55:49  kevin
// Updated Angle and Volume Measurement
//
// Revision 1.9  2000/10/02 23:02:56  jeff
// Moved bookmark list to ReaderGlobal (instance)
// Added a Reset() (re-initialize) method
//
// Revision 1.8  2000/10/02 21:56:59  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.7  2000/09/30 20:35:23  jeff
// Notifications to main application during study load process
//
// Revision 1.6  2000/09/26 02:58:13  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.5  2000/09/21 19:44:43  kevin
// Changes to unwrapped display
//
// Revision 1.4  2000/09/19 16:37:38  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
//
// Revision 1.3  2000/09/18 21:42:49  binli
// Changed the source files from 'OverviewTrackball.C, .h' to 'TrackBall.C, .h'
// The revision histories are reserved.
//
// Revision 1.2  2000/09/18 15:04:54  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 53    9/09/00 1:55p Kevin
// Added angular measurement and hooks for volume measurement
// 
// 52    9/07/00 4:02p Frank
// Added new colon unwrapping features:
// -green composited overlay
// -continuous examination confidence (compared to binary)
// -percent examined
// 
// 51    9/06/00 3:10p Frank
// Added data structure to store examined 2D unwrapped region
// 
// 50    9/05/00 1:34a Ingmar
// now works with new naming
// 
// 49    9/01/00 4:43p Kevin
// Updates for Colon Unwrapping
// 
// 48    9/01/00 10:37a Frank
// Initial work on unwrapping the colon wall
// 
// 47    8/31/00 4:19p Frank
// Added spherical projection visualization mode
// 
// 46    8/28/00 12:32p Antonio
// updated header and footer
//
//*****************  Version 45  *****************
//User: Jeff            Date:  8/22/00  Time: 10:32a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  The m_flying flag is kept in Submarine member (m_submarine)
//
//*****************  Version 44  *****************
//User: Kevin           Date:  8/21/00  Time:  5:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added scaleing (window/level-like functionality) of the translucent
//transfer function
//
//*****************  Version 43  *****************
//User: Kevin           Date:  8/21/00  Time:  3:41p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added segment length checking
//
//*****************  Version 42  *****************
//User: Binli           Date:  8/18/00  Time:  3:31p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 41  *****************
//User: Frank           Date:  8/18/00  Time: 11:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colored orthogonal slice viewing in navigation view
//
//*****************  Version 40  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved tri strips to the segmnet data structure so that the correlate
//with the skeleton indexes for picking
//
//*****************  Version 39  *****************
//User: Frank           Date:  8/17/00  Time: 11:54a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added navigation test mode
//
//*****************  Version 38  *****************
//User: Frank           Date:  8/17/00  Time: 10:44a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added orthogonal slice shadows
//Fixed cross-section slice shadow location
//
//*****************  Version 37  *****************
//User: Jeff            Date:  8/15/00  Time:  7:25p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved OverviewTrackBall (class and instance) to ReaderGlobal so that
//overview position/orientation can persist throughout application
//
//*****************  Version 36  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added RGB marked volume writing for Revli viewing
//
//*****************  Version 35  *****************
//User: Kevin           Date:  8/13/00  Time:  4:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Visible Rendering Marking
//
//*****************  Version 34  *****************
//User: Frank           Date:  8/11/00  Time:  4:26p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a plane projection function
//
//*****************  Version 33  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 32  *****************
//User: Kevin           Date:  8/09/00  Time:  4:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added update of preipheral views only when not flying live
//
//*****************  Version 31  *****************
//User: Kevin           Date:  8/08/00  Time:  9:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed RearView rendering hooks
//
//*****************  Version 30  *****************
//User: Frank           Date:  8/08/00  Time:  2:27p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Reflected name change
//
//*****************  Version 29  *****************
//User: Frank           Date:  8/08/00  Time:  2:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Broke Segment class out into it's own class
//
//*****************  Version 28  *****************
//User: Frank           Date:  8/08/00  Time:  1:28p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MprViewer
//
//*****************  Version 27  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 26  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 25  *****************
//User: Kevin           Date:  8/04/00  Time: 11:02a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Made the directional indicator be centerline color
//
//*****************  Version 24  *****************
//User: Kevin           Date:  8/02/00  Time: 10:52a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colors
//
//*****************  Version 23  *****************
//User: Kevin           Date:  8/01/00  Time: 10:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added num segments to inf file
//
//*****************  Version 22  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 21  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 20  *****************
//User: Kevin           Date:  7/31/00  Time:  1:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed axial images to be dfb for debug
//
//*****************  Version 19  *****************
//User: Jeff            Date:  7/28/00  Time:  5:53p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added functions for serializing bookmark list and for adding
//bookmarks thru GUI
//
//*****************  Version 18  *****************
//User: Binli           Date:  7/28/00  Time:  1:59p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 17  *****************
//User: Binli           Date:  7/28/00  Time:  1:05p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 16  *****************
//User: Jeff            Date:  7/27/00  Time:  6:12p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Nightly checkin (added a class variable to note the path of the
//current opened volume)
//
//*****************  Version 15  *****************
//User: Frank           Date:  7/26/00  Time:  4:21p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MPR slicing along the centerline
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/26/00  Time:  8:18a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added necessary #include
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 12  *****************
//User: Kevin           Date:  7/24/00  Time:  8:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added translucent rendering as a "magic window" with context in
//surface rendering
//
//*****************  Version 11  *****************
//User: Kevin           Date:  7/20/00  Time: 10:17p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Moving Lights and ifxed some distance measuring stuff
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/20/00  Time: 12:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added utility function
//
//*****************  Version 9  *****************
//User: Kevin           Date:  7/19/00  Time:  3:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added global Field-Of-View for rear and forward view
//
//*****************  Version 8  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 7  *****************
//User: Jeff            Date:  7/18/00  Time:  1:35p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 6  *****************
//User: Jeff            Date:  7/18/00  Time:  1:02p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/18/00  Time:  1:00p
//Branched at version 5
//Comment:
//  
//
//*****************  Version 4  *****************
//User: Kevin           Date:  7/14/00  Time:  2:20p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Changed RGBA of floats to RGB of char and moved
//so it doesn't always grab large memory untill needed
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/13/00  Time: 12:57p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Reversed Tfunc Initialization and VolumeRender Initialization since
//the later uses it to create RGBA volume
//
//*****************  Version 2  *****************
//User: Kevin           Date:  7/13/00  Time:  9:37a
//Checked in $/NewViatron1000/ReaderLib
//Comment:
//  new boolean toggle fields for rendering debug
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Global.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/ReaderGlobal.h,v 1.12.2.1 2010/12/20 19:06:05 dongqing Exp $
// $Id: ReaderGlobal.h,v 1.12.2.1 2010/12/20 19:06:05 dongqing Exp $
