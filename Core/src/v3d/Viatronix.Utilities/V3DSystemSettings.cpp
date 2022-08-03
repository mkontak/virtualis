// $Id: V3DSystemSettings.cpp,v 1.6.12.2 2008/04/19 01:16:17 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/V3DSystemSettings.cpp,v 1.6.12.2 2008/04/19 01:16:17 jmeade Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak mkontak@viatronix.net


// Includes
#include "Stdafx.h"
#include "SoftwareRegistry.h"
#include "V3DSystemSettings.h"
#include "DNSRegistry.h"
#include "vxSystemSettings.h"
#include <windows.h>



// Namespaces
using namespace System;
using namespace System::Windows::Forms;

USING_REGISTRY_NS


/**
 * Constructor
 */
V3DSystemSettings::V3DSystemSettings()
{

  m_pSoftwareRegistry = gcnew SoftwareRegistry("Viatronix", "Viatronix V3D System");
  if ( m_pSoftwareRegistry == __nullptr )
  {
    throw gcnew System::OutOfMemoryException(L"Could not allocate a software registry");
  }


  Init();
} // V3DSystemSettings()

/**
 * Constructor
 *
 * @param sComputer     COmnputer to connect to
 */
V3DSystemSettings::V3DSystemSettings(String ^ sComputer)
{


  m_pSoftwareRegistry = gcnew SoftwareRegistry(sComputer, "Viatronix", "Viatronix V3D System");
  if ( m_pSoftwareRegistry == __nullptr )
  {
    throw gcnew System::OutOfMemoryException(L"Could not allocate a software registry");
  }


  Init();
} // V3DSystemSettings()

/** 
 * Initialize
 */
void V3DSystemSettings::Init()
{

  m_pSharePathList = gcnew System::Collections::ArrayList();
  if ( m_pSharePathList == __nullptr )
  {
    throw gcnew OutOfMemoryException(L"Could not allcate share/path list");
  }

  m_pDatabaseUsers = gcnew System::Collections::ArrayList();
  if ( m_pDatabaseUsers == __nullptr )
  {
    throw gcnew OutOfMemoryException(L"Could not allcate database users list");
  }

  Read();

} // Init()


/**
 * Reads the registry settings
 */
void V3DSystemSettings::Read()
{

  m_sConnectionString = L"";
  m_sGlobalConnectionString = L"";

  m_pSoftwareRegistry->Clear();

  String ^ sShare = String::Concat(L"\\\\",System::Windows::Forms::SystemInformation::ComputerName);

  /////////////////
  // Colonoscopy
  /////////////////
  ShareAndPathSetting ^ Colon = gcnew ShareAndPathSetting();
  Colon->LocalPath = m_pSoftwareRegistry->GetValue(L"LocalColonStudiesDir", L"D:\\Viatronix\\VxColonStudies");
  Colon->Share = m_pSoftwareRegistry->GetValue(L"LocalColonStorePath", String::Concat(sShare, System::IO::Path::DirectorySeparatorChar.ToString(), L"VxColonStudies"));
  m_pSharePathList->Add(Colon);


  //////////
  // AAA
  //////////
  ShareAndPathSetting ^ AAA = gcnew ShareAndPathSetting();
  AAA->LocalPath = m_pSoftwareRegistry->GetValue(L"LocalAAAStudiesDIr", L"D:\\Viatronix\\VxAAAStudies");
  AAA->Share = m_pSoftwareRegistry->GetValue(L"LocalAAAStorePath", String::Concat(sShare, System::IO::Path::DirectorySeparatorChar.ToString(), L"VxAAAStudies"));
  m_pSharePathList->Add(AAA);


  ////////////////////
  // Calcium Scoring
  ////////////////////
  ShareAndPathSetting ^ Calcium = gcnew ShareAndPathSetting();
  Calcium->LocalPath = m_pSoftwareRegistry->GetValue(L"LocalCalciumStudiesDir", L"D:\\Viatronix\\VxCalciumStudies");
  Calcium->Share = m_pSoftwareRegistry->GetValue(L"LocalCalciumStorePath", String::Concat(sShare, System::IO::Path::DirectorySeparatorChar.ToString(), L"VxCalciumStudies"));
  m_pSharePathList->Add(Calcium);


  //////////////
  // Explorer
  //////////////
  ShareAndPathSetting ^ Explorer = gcnew ShareAndPathSetting();
  Explorer->LocalPath = m_pSoftwareRegistry->GetValue(L"LocalExplorerStudiesDir", L"D:\\\\Viatronix\\VxExplorerStudies");
  Explorer->Share = m_pSoftwareRegistry->GetValue(L"LocalExplorerStorePath", String::Concat(sShare, System::IO::Path::DirectorySeparatorChar.ToString(), L"VxExplorerStudies"));
  m_pSharePathList->Add(Explorer);


  // System Log Level
  m_iSystemLogLevel = m_pSoftwareRegistry->GetValue(L"SystemLogLevel", (int)2);


  /// Dicom messenger host name
  m_sDicomMessengerHostname = m_pSoftwareRegistry->GetValue(L"DicomMessengerHost", L"127.0.0.1");

  /// Dicom Messenger Port
  m_iDicomMessengerPort = m_pSoftwareRegistry->GetValue(L"DicomMessengerPort", (int)2002);

  // System Log Location
  m_sSystemLogLocation = m_pSoftwareRegistry->GetValue(L"SystemLogLocation", L"C:\\\\V-System\\Log\\V3DViewer");

  m_sDatabaseDSN = m_pSoftwareRegistry->GetValue(L"DatabaseDSN", L"", L"ViatronixV3D");
  DSNRegistry ^ pDSN = gcnew DSNRegistry(m_sDatabaseDSN);
  m_sConnectionString = pDSN->ConnectionString;

  m_sGlobalDatabaseDSN = m_pSoftwareRegistry->GetValue(L"GlobalDatabaseDSN", L"", L"ViatronixGlobal");
  pDSN = gcnew DSNRegistry(m_sDatabaseDSN);
  m_sGlobalConnectionString = pDSN->ConnectionString;

  ///////////////////////
  // Report Parameters
  //////////////////////

  // Bottom Margin 
  m_fBottomMargin = m_pSoftwareRegistry->GetValue(L"Report", L"BottomMargin", 1.0f);

  // Top Margin
  m_fTopMargin = m_pSoftwareRegistry->GetValue(L"Report", L"TopMargin", 1.0f);

  // Get the report numbering type
  m_eNumbering = (ReportNumberingType)m_pSoftwareRegistry->GetValue(L"Report", L"Numbering", (int)ReportNumberingType::NUMBERING_NONE);

  // Get the report header type
  m_eReportHeader = (ReportHeaderType)m_pSoftwareRegistry->GetValue(L"Report", L"HeaderSelection", (int)ReportHeaderType::HEADER_NONE);


  // Gets the header text
  m_sHeaderText = m_pSoftwareRegistry->GetValue(L"Report",L"HeaderText", L"");

  // Get the Header Bit Map File
  String ^ sViewerBitmap = String::Concat(gcnew String(vxSystemSettings::GetViewersDirectory().c_str()), L"VC\\config\\ReportHeader\\RptHdr.png");
  m_sHeaderBitmapFile = m_pSoftwareRegistry->GetValue(L"Report", L"HeaderBitmapFile", sViewerBitmap);

} // Read()


/**
 * Writes out the registry entries
 */
void V3DSystemSettings::Write()
{
  /////////////////
  // Colonoscopy
  /////////////////
  ShareAndPathSetting ^ Path = dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[(int)StudyType::VC]);
  m_pSoftwareRegistry->SetValue(L"LocalColonStudiesDir",Path->LocalPath);
  m_pSoftwareRegistry->SetValue(L"LocalColonStorePath", Path->Share);

  //////////
  // AAA
  //////////
  Path = dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[(int)StudyType::AAA]);
  m_pSoftwareRegistry->SetValue(L"LocalAAAStudiesDir",Path->LocalPath);
  m_pSoftwareRegistry->SetValue(L"LocalAAAStorePath", Path->Share);


  ////////////////////
  // Calcium Scoring
  ////////////////////
  Path = dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[(int)StudyType::CS]);
  m_pSoftwareRegistry->SetValue(L"LocalCalciumStudiesDir",Path->LocalPath);
  m_pSoftwareRegistry->SetValue(L"LocalCalciumStorePath", Path->Share);


  //////////////
  // Explorer
  //////////////
  Path = dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[(int)StudyType::XP]);
  m_pSoftwareRegistry->SetValue(L"LocalExplorerStudiesDir",Path->LocalPath);
  m_pSoftwareRegistry->SetValue(L"LocalExplorerStorePath", Path->Share);


  // System Log Level
  m_pSoftwareRegistry->SetValue(L"SystemLogLevel", m_iSystemLogLevel);

  // System Log Location
  m_pSoftwareRegistry->SetValue(L"SystemLogLocation", m_sSystemLogLocation);


  // Database DSN
  m_pSoftwareRegistry->SetValue(L"DatabaseDSN", L"", m_sDatabaseDSN);

  //
  m_pSoftwareRegistry->SetValue(L"DicomMessengerPort", m_iDicomMessengerPort);

  ///
  m_pSoftwareRegistry->SetValue(L"DicomMessengerHost", m_sDicomMessengerHostname);

  //
  m_pSoftwareRegistry->SetValue(L"GlobalDatabaseDSN", L"", m_sGlobalDatabaseDSN);

  ///////////////////////
  // Report Parameters
  //////////////////////

  // write the reports Bottom Margin setting
  m_pSoftwareRegistry->SetValue(L"Report", L"BottomMargin", m_fBottomMargin);

  // write the reports Top Margin setting
  m_pSoftwareRegistry->SetValue(L"Report", L"TopMargin", m_fTopMargin);

  // write the report numbering type
  m_pSoftwareRegistry->SetValue(L"Report", L"Numbering", (int)m_eNumbering);

  // writes the report header type
  m_pSoftwareRegistry->SetValue(L"Report", L"HeaderSelection", (int)m_eReportHeader);

  // writes the header text
  m_pSoftwareRegistry->SetValue(L"Report",L"HeaderText", m_sHeaderText);

  // writes the Header Bit Map File
  m_pSoftwareRegistry->SetValue(L"Report", L"HeaderBitmapFile", m_sHeaderBitmapFile);


} // Write()



// $Log: V3DSystemSettings.cpp,v $
// Revision 1.6.12.2  2008/04/19 01:16:17  jmeade
// text
//
// Revision 1.6.12.1  2008/04/16 23:58:51  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.6  2005/08/29 12:38:12  mkontak
// Added CheckDiskThreadholds()
//
// Revision 1.5  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.4  2005/05/03 16:25:43  mkontak
// no message
//
// Revision 1.3  2005/04/29 13:46:12  mkontak
// VRX report creation fixes
//
// Revision 1.1.1.1.2.1  2005/04/29 13:43:23  mkontak
// no message
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
// Revision 1.1.1.1.2.1  2005/04/29 13:43:23  mkontak
// no message
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.2  2003/08/05 14:09:46  mkontak
// Requery Interval
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/V3DSystemSettings.cpp,v 1.6.12.2 2008/04/19 01:16:17 jmeade Exp $
// $Id: V3DSystemSettings.cpp,v 1.6.12.2 2008/04/19 01:16:17 jmeade Exp $
