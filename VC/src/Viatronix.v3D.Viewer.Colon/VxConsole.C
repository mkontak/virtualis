    // $Id: VxConsole.C,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: VxConsole.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "VxConsole.h"
#include "VxVCReport.h"
#include "BackgroundProcess.h"
#include "FunctionProgress.h"
#include "File.h"
#pragma warning(disable:4005)
#include "vxVolumeHeader.h"
#include "Crypt.h"
#include "SystemInfo.h"
#include "VCResource.h"

#include <lm.h>

#define FILE_REVISION "$Revision: 1.6.2.1 $"

//using namespace ConsoleLib;

//void StripLeadTrailSpaces(std::string& sStringIn);
//inline bool IsComplete(const DatasetStateEnum eState);


//////////////////////////////////////////////////////////////////////
// VxConsole namespace

///**
// * Initalizes the database source
// */
//void VxConsole::InitializeDatabaseSourceParams()
//{
//  // Initialize the local studies sharename paths and directories
//  std::string sLocalStudiesShare;
//  std::string sLocalStudyDir;
//
//  // Colon
//  V3DSystemSettings::m_colonStudiesShareAndPath.first.Get(sLocalStudiesShare);
//  V3DSystemSettings::m_colonStudiesShareAndPath.second.Get(sLocalStudyDir);
//  if (!CreateLocalShare(sLocalStudiesShare, sLocalStudyDir))
//  {
//    LogErr("Could not create share: " + sLocalStudiesShare, "VxConsole", "InitializeDatabaseSourceParams");
//  }
//
//  // Calcium
//  V3DSystemSettings::m_calciumStudiesShareAndPath.first.Get(sLocalStudiesShare);
//  V3DSystemSettings::m_calciumStudiesShareAndPath.second.Get(sLocalStudyDir);
//  if (!CreateLocalShare(sLocalStudiesShare, sLocalStudyDir))
//  {
//    LogErr("Could not create share: " + sLocalStudiesShare, "VxConsole", "InitializeDatabaseSourceParams");
//  }
//
//  // Explorer
//  V3DSystemSettings::m_explorerStudiesShareAndPath.first.Get(sLocalStudiesShare);
//  V3DSystemSettings::m_explorerStudiesShareAndPath.second.Get(sLocalStudyDir);
//  if (!CreateLocalShare(sLocalStudiesShare, sLocalStudyDir))
//  {
//    LogErr("Could not create share: " + sLocalStudiesShare, "VxConsole", "InitializeDatabaseSourceParams");
//  }
//}


/**
 * Returns the full pathname of the current application instance
 *
 * @return   Full pathname of the current application
 */
std::string VxConsole::GetAppPathname()
{
  char szThisExe[_MAX_PATH+1];
  GetModuleFileName(NULL, szThisExe, _MAX_PATH);
  return std::string(szThisExe);
} // GetAppPathname()


/**
 * Gets an entry from the volume header
 *
 * @param sHeaderVar   Header entry to retrieve
 * @return             Value of header entry
 */
#define vtxMyESCAPE_CHARACTERS    "`~!@#$%^&*=_+[]\\{}|;:\'\"/<>?\0"
std::string VxConsole::GetVolumeHeaderVariable(const vxVolumeHeader& volHdr, const std::string& sHeaderVarIdentifer)
{
  std::string sEntry;
  bool bPatientName(false);

  if ( (sHeaderVarIdentifer.substr(0, strlen("patientName")) == "patientName"))
  {
    volHdr.GetHeaderEntry(vxVolumeHeader::PatientName, sEntry);
    bPatientName = true;
  }
  //else if ( (sHeaderVarIdentifer.substr(0, strlen("encrypted")) == "encrypted"))
  //{
  //  volHdr.GetHdrVar(sHeaderVarIdentifer, sEntry);
  //  Crypt crypt;
  //  crypt.DecryptReadable(sEntry);
  //}
  else if ( (sHeaderVarIdentifer.substr(0, strlen("patientAge")) == "patientAge"))
  {
    sEntry = ToAscii(int4(volHdr.GetPatientAge()));

    //vxVolumeHeader::VarVolHeaderEntryEnum::PatientAge, sEntry);
    //if (sEntry.empty() || (atoi(sEntry.c_str()) <= 0))
    //{
    //  std::string sBirthDate; volHdr.GetHdrVar("patientBirthDate", sBirthDate);
    //  if (!sBirthDate.empty())
    //  {
    //    vx::Date birthDate; birthDate.SetDate(sBirthDate);

    //    vx::Date scanDate;
    //    std::string sStudyDate; volHdr.GetHdrVar("studyDate", sStudyDate);
    //    if (!sStudyDate.empty())
    //    {  scanDate.SetDate(sStudyDate); }
    //    else
    //    {
    //      COleDateTime currTime = COleDateTime::GetCurrentTime();
    //      scanDate.SetYear(currTime.GetYear()); scanDate.SetMonth(currTime.GetMonth()); scanDate.SetDay(currTime.GetDay());
    //    }

    //    int4 iAge = abs(int4(scanDate.GetYear() - birthDate.GetYear()));

    //    if (COleDateTime(scanDate.GetYear(), scanDate.GetMonth(),  scanDate.GetDay(),  0,0,0)
    //      < COleDateTime(scanDate.GetYear(), birthDate.GetMonth(), birthDate.GetDay(), 0,0,0))
    //    {  iAge--;  }
    //    sEntry = ToAscii(iAge);
    //  }
    //}
    // just return at this point, nothing more to do

    return sEntry;
  }
  else
  {
    volHdr.GetVariableVolumeHeader().GetHdrVar(sHeaderVarIdentifer, sEntry);
  }

  CString sReturnEntry(sEntry.c_str());
  sReturnEntry.TrimLeft();
  sReturnEntry.TrimRight();

  // if it is a patient name, just return.
  if (bPatientName)
  {
    return LPCSTR(sReturnEntry);
  }

  for (char* sEscChars = vtxMyESCAPE_CHARACTERS; *sEscChars != char(NULL);
        sReturnEntry.Replace(*sEscChars, ' '), ++sEscChars);

  return LPCTSTR(sReturnEntry);
} // GetVolumeHeaderVariable()


/**
 * Creates a local share directory
 *
 * @param sShareName        Directory sharename
 * @param sShareDirectory   Local directory to be shared
 * @return                  true if share and path either exist already or was created here
 */
//bool VxConsole::CreateLocalShare(const std::string& sShareName, const std::string& sShareDirectory)
//{
//  CString sShare(vx::File(sShareName).GetLocalPath().c_str());
//  if (sShareName.empty() || sShareDirectory.empty() || sShare.IsEmpty())
//  {
//    std::strstream ssMsg;
//    ssMsg << "Empty share name (" << sShareName.c_str() << "), path (" << sShareDirectory.c_str()
//      << ") or local path (" << LPCTSTR(sShare) << ")." << std::ends;
//    LogWrn(ssMsg.str(), "VxConsole", "CreateLocalShare");
//    ssMsg.freeze(false);
//    return false;
//  }
//
//  // Delete the first separator, and replace any others with underscores
//  if (CString("\\/").Find(sShare[0]) >= 0) { sShare.Delete(0); }
//  sShare.Replace('/','_'); sShare.Replace('\\','_');
//
//  //const std::wstring wsShare(widestring(LPCTSTR(sShare)));
//  const std::string wsShare = LPCTSTR(sShare);
//
//  // Initialize the local studies sharename
//  LPSHARE_INFO_2 pShareInfo(NULL);
//  //const std::wstring wsServer(widestring(vx::File::GetComputerName()));
//  const std::string wsServer(vx::File::GetComputerName());
//  NET_API_STATUS result = NetShareGetInfo((LPWSTR)wsServer.c_str(), (LPWSTR)wsShare.c_str(), 2, (LPBYTE*) &pShareInfo);
//
//  if ( (result != ERROR_SUCCESS) || (pShareInfo == NULL) )
//  {
//    // Set up local studies path
//    vx::File localPath(sShareDirectory);
//    localPath.Mkdirs();
//    //const std::wstring wsLocalPath(widestring(localPath.ToString()));
//    const std::string wsLocalPath(localPath.ToString());
//
//    // TODO: Set up local studies share
//    SHARE_INFO_2 shareInfo;
//    memset(&shareInfo, 0, sizeof(shareInfo));
//    shareInfo.shi2_netname = (LPWSTR)wsShare.c_str();
//    shareInfo.shi2_type = STYPE_DISKTREE;
//    shareInfo.shi2_max_uses = -1;
//    shareInfo.shi2_path = (LPWSTR)wsLocalPath.c_str();
//
//    DWORD iError;
//    result = NetShareAdd( (LPWSTR)wsServer.c_str(), 2, LPBYTE(&shareInfo), &iError );
//    if (result != ERROR_SUCCESS)
//    {
//      std::strstream ssMsg; ssMsg << "NetShareAdd() function failed, NET_API error code = " << result << std::ends;
//      LogWrn(ssMsg.str(), "VxConsole", "CreateLocalShare");
//      ssMsg.freeze(false);
//    }
//  } // end if share not found
//  else // share found, ensure directories are present
//  {
//    vx::File localPath(sShareDirectory);
//    localPath.Mkdirs();
//  } // end if share found
//
//  if (pShareInfo != NULL)
//  {
//    NetApiBufferFree(pShareInfo);
//  }
//
//  return ((result == ERROR_SUCCESS) && vx::File(sShareDirectory).Exists());
//}
//

/**
 * Returns the local share path for studies of the specified type
 *
 * @param eStudyType   Type of study
 * @return             Corresponding local share for study of the specified type
 */
//std::string VxConsole::GetLocalSharePath(const StudyTypeEnum eStudyType)
//{
//  std::string sLocalPath;
//  // TODO: Generalize using a map or similar structure, or simply add cases for other modalities
//  switch (eStudyType)
//  {
//  case StudyType_CalciumScoring:
//    V3DSystemSettings::m_calciumStudiesShareAndPath.first.Get(sLocalPath);
//    break;
//
//  case StudyType_Colonoscopy:
//    V3DSystemSettings::m_colonStudiesShareAndPath.first.Get(sLocalPath);
//    break;
//
//  case StudyType_Explorer:
//  default:
//    V3DSystemSettings::m_explorerStudiesShareAndPath.first.Get(sLocalPath);
//    break;
//  }
//
//  return vx::File(sLocalPath).ConvertUNCToIP();
//}
//
//
//

/**
 * Returns a vector of strings for the local disk drives (physical or mapped)
 *
 * @return   Drive string list
 */
//std::vector<std::string> VxConsole::GetDriveStrings()
//{
//  std::vector<std::string> vsDrives;
//
//  // Get the length of the array needed to store the drive strings
//  DWORD dwLen = GetLogicalDriveStrings(0, NULL);
//  if (dwLen <= 0)
//  {
//    return std::vector<std::string>();
//  }
//
//  // Get the drive strings
//  LPTSTR pDriveStrBuffer = new char[dwLen+1];
//  if (GetLogicalDriveStrings(dwLen, pDriveStrBuffer) <= 0)
//  {
//    delete []pDriveStrBuffer;
//    return std::vector<std::string>();
//  }
//
//  LPTSTR pDrive = NULL;
//
//  // Count the number of drive
//  int4 iDriveCt = 0;
//  for (pDrive = pDriveStrBuffer; (pDrive != NULL) && (*pDrive != 0); pDrive = strchr(pDrive, 0), ++pDrive, iDriveCt++)
//  {
//    ;
//  }
//
//  // Resize the vector accordingly
//  vsDrives.resize(iDriveCt);
//
//  // Store the strings
//  int4 iCurrDrive = 0;
//  for (pDrive = pDriveStrBuffer; (pDrive != NULL) && (*pDrive != 0); pDrive = strchr(pDrive, 0), ++pDrive, iCurrDrive++)
//  {
//    vsDrives[iCurrDrive] = pDrive;
//  }
//
//  delete []pDriveStrBuffer;
//
//  return vsDrives;
//}
//

/**
 * Typical handler for a drop-target (WM_DROPFILES message)
 *
 * @param volHdr         Volume header instance for reading.
 * @param filePathList   List of file paths.
 * @return               List of study item data instances pertaining to (study files in the) the list of files given.
 */
//VxDBItemDataList VxConsole::OnDropFiles(HeaderVol& volHdr, const std::list<std::string>& filePathList)
//{
//  VxDBItemDataList resultItemList;
//
//  for (std::list<std::string>::const_iterator itFile = filePathList.begin(); itFile != filePathList.end(); itFile++)
//  {
//    std::string sIthDroppedFilePath = *itFile;
//    VxDBItemDataList studyItemList = VxConsole::OnDropFile(volHdr, sIthDroppedFilePath);
//    if (!studyItemList.empty())
//    {
//      resultItemList.push_back(studyItemList.front());
//    }
//  }
//
//  return resultItemList;
//}
//

/**
 * Typical handler for an entry in a drop-target (WM_DROPFILES message)
 *
 * @param volHdr              Volume header instance for reading.
 * @param sFilePath           File path.
 * @return VxDBItemDataList   List of item data instances pertaining to (datasets in the) the list of files given.
 */
//VxDBItemDataList VxConsole::OnDropFile(HeaderVol& volHdr, const std::string& sIthDroppedFilePath)
//{
//  std::string sVolumeFile;
//
//  // Add to local studies list if it is an ecv file
//  if ( (sIthDroppedFilePath == vx::ElecCleansedVolExt) || (sIthDroppedFilePath == vx::VolumeExt) )
//  {
//    sVolumeFile = sIthDroppedFilePath;
//  }
//  else if (vx::File(sIthDroppedFilePath).IsDirectory()) // Add the study if an ecv file located in the directory
//  {
//    vx::File sDir(sIthDroppedFilePath);
//    const std::vector<vx::File> ecvFileList(sDir.ListFiles(SuffixFilter(vx::ElecCleansedVolExt)));
//    const std::vector<vx::File> volFileList(sDir.ListFiles(SuffixFilter(vx::VolumeExt)));
//
//    if (!ecvFileList.empty())
//    {
//      sVolumeFile = ecvFileList[0].ToString();
//    }
//    else if (!volFileList.empty())
//    {
//      sVolumeFile = volFileList[0].ToString();
//    }
//  }
//  else if (sIthDroppedFilePath == vx::ReportExt) // Open the report if the file has .rpt extension
//  {
//    VxVCReport::OpenReportPreview(volHdr, sIthDroppedFilePath);
//    return VxDBItemDataList();
//  }
//
//  if (sVolumeFile.empty() || !vx::File(sVolumeFile).Exists())
//  {
//    return VxDBItemDataList();
//  }
//
//  VxDBItemDataList studyItemList = VxConsole::ReadVolumeHeader(sVolumeFile);
//  if (!studyItemList.empty())
//  {
//    studyItemList.front().m_sFilePath = vx::File(sVolumeFile).GetParent() + vx::File::GetPathSeparator();
//  }
//
//  return studyItemList;
//}
//
//
//

/**
 * Returns a current-region-specific string representation of the given date.
 *
 * @param sysDate   Date source.
 * @return          String representation of sysDate.
 */
//std::string FormatDateString(const SYSTEMTIME& sysDate)
//{
//  const COleDateTime dt(sysDate);
//  return (dt.GetStatus()!=COleDateTime::invalid) ?
//    LPCTSTR(dt.Format(VAR_DATEVALUEONLY, LANG_USER_DEFAULT)) : std::string();
//}
//

/**
 * Removes leading and trailing spaces from string
 *
 * @param sStringIn   Input/output string
 */
//void StripLeadTrailSpaces(std::string& sStringIn)
//{
//  for ( int4 j=sStringIn.length()-1; j>0 && (sStringIn[j] == ' '); --j )
//  {
//  }
//  for ( int4 k=0; k<=sStringIn.length()-1 && (sStringIn[k] == ' '); ++k )
//  {
//  }
//
//  sStringIn = sStringIn.substr(k, j-k+1);
//} // StripLeadTrailSpaces
//

#undef FILE_REVISION

// $Log: VxConsole.C,v $
// Revision 1.6.2.1  2010/01/07 18:46:16  dongqing
// const declaration
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.5  2006/06/27 17:32:28  jmeade
// Read patient position from volume header.
//
// Revision 1.4  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.3  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.2  2005/08/05 15:17:32  geconomos
// removed netshare code
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.17.2.19.4.2  2005/05/03 00:28:47  jmeade
// Issue 3980: handle either date format.
//
// Revision 1.17.2.19.4.1  2005/03/03 16:09:40  vxconfig
// Changed LPSTR to LPWSTR
//
// Revision 1.17.2.19  2004/08/11 19:52:47  mkontak
// Fix for DOB format error
//
// Revision 1.17.2.18  2004/05/06 19:13:05  mkontak
// Changed logs
//
// Revision 1.17.2.17  2004/05/06 18:30:14  mkontak
// Fixed issue with older anonymized CD's
//
// Revision 1.17.2.16  2004/04/14 21:08:05  romy
// Changed the DSN string to ADO.NET string. Reading the server name from the registry.
//
// Revision 1.17.2.15  2004/04/12 14:28:52  romy
// added the new BRL import statement
//
// Revision 1.17.2.14  2004/03/27 00:47:07  jmeade
// Correct for study titles that include '_' marks apart from that separating the mm/orientation indicator (last 2 letters e.g. xx_x_1P.ecv)
//
// Revision 1.17.2.13  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.17.2.12  2004/02/05 20:52:11  mkontak
// Fixes to delete of old directory
//
// Revision 1.17.2.11  2004/02/03 19:01:37  mkontak
// Missing #undef FILE_REVISION
//
// Revision 1.17.2.10  2004/02/03 18:55:16  mkontak
// Remove debug message boxes
//
// Revision 1.17.2.9  2004/02/02 20:32:44  mkontak
// Issue 3624
//
// Revision 1.17.2.8  2004/01/20 13:47:10  mkontak
// Fix issue with transfer to local when there is a duplicate
//
// Revision 1.17.2.7  2004/01/19 21:15:32  mkontak
// Fix messages
//
// Revision 1.17.2.6  2003/11/18 20:55:52  mkontak
// Fix dataset typing
//
// Revision 1.17.2.5  2003/11/18 16:25:54  mkontak
// Issue 2746
//
// Revision 1.17.2.4  2003/10/22 14:02:08  mkontak
// Issue 3513
//
// Revision 1.17.2.3  2003/07/07 20:31:02  jmeade
// Code standards.
//
// Revision 1.17.2.2  2003/06/09 17:32:26  jmeade
// code review prep.
//
// Revision 1.17.2.1  2003/06/06 20:55:38  jmeade
// code review prep.
//
// Revision 1.17  2002/12/24 03:49:52  jmeade
// Report errors if unable to create local studies shares/directories.
//
// Revision 1.16  2002/11/07 21:48:44  jmeade
// Method to OnDrop... on a single drop-files entry.
//
// Revision 1.15  2002/10/14 19:00:00  jmeade
// Use proper FileExtension instance instead of hard-coded extension.
//
// Revision 1.14  2002/10/08 15:05:18  mkontak
// Fixed problem with retrospective detection in the console
//
// Revision 1.13  2002/10/07 23:05:42  jmeade
// Replaced MFC scan date with Win32 SYSTEMTIME; FormatDateString for SYSTEMTIME.
//
// Revision 1.12  2002/09/30 23:09:50  jmeade
// Moved messages with others in VxMutex.
//
// Revision 1.11  2002/09/30 20:37:19  jmeade
// Interapp messages to ConsoleLib.
//
// Revision 1.10  2002/09/27 19:27:45  jmeade
// Cleaned up comments.
//
// Revision 1.9  2002/09/25 20:01:11  jmeade
// Moved database search function to ConsoleLib.
//
// Revision 1.8  2002/09/25 19:33:53  jmeade
// StoreStudyData changed to reflect method that gets study path and title.
//
// Revision 1.7  2002/09/20 16:47:06  mkontak
// Use Viatronix.v3D.Serialize.dll instead of the vxSerialize.Dll
//
// Revision 1.6  2002/09/18 23:12:05  jmeade
// Replaced code in XP study serialize to store current user ID.
//
// Revision 1.5  2002/09/17 19:54:03  jmeade
// Removed commented code.
//
// Revision 1.4  2002/09/17 15:12:05  jmeade
// Moving transfer and background process functionality to ConsoleLib.
//
// Revision 1.3  2002/07/18 21:47:45  jmeade
// Moved drop files-type functionality.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxConsole.C,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
// $Id: VxConsole.C,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
