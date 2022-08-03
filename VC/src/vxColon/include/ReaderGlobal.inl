// $Id: ReaderGlobal.inl,v 1.4 2006/12/05 20:13:45 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)


/**
 * Have both supine and prone datasets been detected.
 *
 * @return  true if both supine and prone files are available.
 */
bool ReaderGlobal::IsDualDataDetected() const
{
  return m_bDualDataDetected;
} // IsDualDataDetected()


/**
 * Check that both supine and prone have finished loading
 *
 * @return  true if both supine and prone have finished loading.
 */
bool ReaderGlobal::IsDualDataLoaded() const
{
  return IsDualDataDetected() && m_supine.IsLoadingComplete() && m_prone.IsLoadingComplete();
} // IsDualDataLoaded()


/**
 * Returns true if one or more of supine & prone were detected and are finished loading.
 *
 * @return  true if all loading that will be done has completed.
 */
bool ReaderGlobal::IsLoadingComplete() const
{
  if (m_bDualDataDetected)
  {
    return (m_supine.IsLoadingComplete() && m_prone.IsLoadingComplete());
  }
  else
  {
    return (m_supine.IsLoadingComplete() || m_prone.IsLoadingComplete());
  } // check both or either
} // IsLoadingComplete()


/**
 * Returns which datasets are loading while LoadStudy() fn is active, eDEFAULT otherwise
 *
 * @return value of m_eIsLoading
 */
DatasetOrientations::TypeEnum ReaderGlobal::IsStudyLoading() const
{
  return m_eIsLoading;
}


/**
 * sets the datset loading type
 *
 * @param       eOrientation        DatasetOrientations::TypeEnum
 */
void ReaderGlobal::SetStudyLoading( DatasetOrientations::TypeEnum eOrientation )
{
  m_eIsLoading = eOrientation;
} // SetStudyLoading( eOrientation )


/**
 * Returns true if the dataset is loading based on orientation
 *
 * @param       eOrientation        DatasetOrientations::TypeEnum
 * @return      true if the dataset is loading based on orientation; otherwise false
 */
bool ReaderGlobal::IsDatasetLoading( DatasetOrientations::TypeEnum eOrientation ) const
{
  return ( m_eIsLoading & eOrientation ) == eOrientation;
}


/**
 * get the settings - a singleton
 *
 * @return the singleton settings object
 */
const VCSettings * ReaderGlobal::GetSettings()
{
  // singleton pattern
  return &VCSettings::GetInstance();
}


/**
 * get the settings - a singleton
 *
 * @return module path
 */
std::string ReaderGlobal::GetModulePath()
{
  // get the path of the executable
  char sExePath[ _MAX_PATH ];
  GetModuleFileName( NULL, sExePath, _MAX_PATH );

  // get the drive letter and directory of executable
  char sDrive[_MAX_DRIVE];
  char sDirectory[_MAX_DIR ];
  _splitpath( sExePath, sDrive, sDirectory, NULL, NULL );

  return std::string(sDrive) + sDirectory;
}


// $Log: ReaderGlobal.inl,v $
// Revision 1.4  2006/12/05 20:13:45  jmeade
// Issue 5047: get module path function.
//
// Revision 1.3  2006/10/04 20:52:37  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.2  2005/11/22 16:38:40  jmeade
// movie creation reflecting new directory structure.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1.16.1  2005/04/13 18:23:37  jmeade
// Issue 4117: Identify which datasets are loading.
//
// Revision 3.1  2002/04/12 02:13:35  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:04   ingmar
// Initial revision.
// Revision 1.12  2001/06/27 18:22:58  jmeade
// bool variable set only when LoadPatient() fn is active, for querying with IsLoading() fn
//
// Revision 1.11  2001/05/25 20:28:58  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.10  2001/04/30 12:31:50  frank
// Reflected changes in Study class.
//
// Revision 1.9  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.8  2001/03/20 15:57:00  frank
// Updated to reflect new coding standards.
//
// Revision 1.7  2001/03/09 01:09:11  jmeade
// protect access to some more Study members
//
// Revision 1.6  2001/02/28 17:54:16  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.5  2001/01/26 02:46:47  jeff
// dual data detected/loaded, dual segments loaded query functions
//
// Revision 1.4  2001/01/26 01:57:33  jeff
// added IsLoadingComplete(), a more generic single-or-dual study query function
//
// Revision 1.3  2001/01/09 13:28:13  frank
// Removed unused Swap function.
//
// Revision 1.2  2001/01/08 16:41:51  frank
// Added function to test for secondary dataset.
//
// Revision 1.1  2000/11/15 14:03:36  frank
// Moved some functions to inline file
//
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/ReaderGlobal.inl,v 1.4 2006/12/05 20:13:45 jmeade Exp $
// $Id: ReaderGlobal.inl,v 1.4 2006/12/05 20:13:45 jmeade Exp $
