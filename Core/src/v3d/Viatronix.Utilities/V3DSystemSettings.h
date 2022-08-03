// $Id: V3DSystemSettings.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Mark Kontak  mkontak@viatronix.com


// Pragmas
#pragma once

using namespace System;

// Namespaces
OPEN_REGISTRY_NS

// Forward declarations
ref class SoftwareRegistry;

/**
 * V3D Configuration 
 */
public ref class V3DSystemSettings 
{
public:

  /// Constructor
  V3DSystemSettings();

  /// Constructor
  V3DSystemSettings(System::String ^ sComputer);

  /// Destructor
  virtual ~ V3DSystemSettings() { delete(m_pSoftwareRegistry); }

  /// Reads the registry settings
  void Read();

  /// Writes the registry settings
  void Write();

  /// Enumerators
  enum class ReportNumberingType { NUMBERING_NONE, NUMBERING_BOTTOM };
  enum class StudyType { VC, AAA, CS, XP }; 
  enum class ReportHeaderType { HEADER_NONE, HEADER_BITMAP, HEADER_USER };

  /**
   * Implements a Share/Path setting object
   */
  ref class ShareAndPathSetting 
  {
  public:
    ShareAndPathSetting() { }
    virtual ~ShareAndPathSetting() { };

    System::String ^ Share;
    System::String ^ LocalPath;
  };  // ShareAndPathSetting


  /// Returns the Share and Path settings for each study type
  ShareAndPathSetting ^ GetShareAndPath(StudyType eStudyType) 
  {
    int iIndex = static_cast<int>(eStudyType);
    return(dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[iIndex])); 
  }

  /// Sets the Share and Path settings for each study type
  void SetShareAndPath(StudyType eStudyType, System::String ^ sLocalPath, System::String ^ sShare ) 
  {
    int iIndex = static_cast<int>(eStudyType);
    ShareAndPathSetting ^ setting = dynamic_cast<ShareAndPathSetting ^>(m_pSharePathList[iIndex]);
    setting->Share = sShare;
    setting->LocalPath = sLocalPath;
  }

private:

  /// Initialize
  void Init();

public:

  /// Returns the connected computer
  property String ^ Computer 
  { 
    String ^ get() { return m_pSoftwareRegistry->Computer; }
  }

  /// Returns the top margin
  property float TopMargin 
  {
    float get() { return(m_fTopMargin); }
    void set(float fTopMargin) { m_fTopMargin = fTopMargin; }
  }

  /// Returns the bottom margin
  property float BottomMargin
  {
    float get() { return(m_fBottomMargin); }
    void set(float fBottomMargin) { m_fBottomMargin = fBottomMargin; }
  }

  /// Returns the numbering
  property ReportNumberingType Numbering 
  {
    ReportNumberingType get() { return(m_eNumbering); }
    void set(ReportNumberingType eNumbering) { m_eNumbering = eNumbering; }
  }


  /// Returns the report header
  property ReportHeaderType ReportHeader
  {
    ReportHeaderType get() { return(m_eReportHeader); }
    void set(ReportHeaderType eReportHeader) { m_eReportHeader = eReportHeader; }
  }

  /// Returns the connection string
  property System::String ^ ConnectionString
  {
    String ^ get() { return(m_sConnectionString); }
    void set(System::String ^ sConnectionString) { m_sConnectionString = sConnectionString; }
  }

  /// Returns the global connection string
  property System::String ^ GlobalConnectionString
  {
    String ^ get() { return(m_sGlobalConnectionString); }
    void set(System::String ^ sGlobalConnectionString) { m_sGlobalConnectionString = sGlobalConnectionString; }
  }


  /// Returns the DSN name
  property System::String ^ DatabaseDSN
  {
    String ^ get() { return(m_sDatabaseDSN); }
    void set(System::String ^ sDatabaseDSN) { m_sDatabaseDSN = sDatabaseDSN; }
  }

  /// Returns the DSN name
  property System::String ^ GlobalDatabaseDSN
  {
    String ^ get() { return(m_sGlobalDatabaseDSN); }
    void set(System::String ^ sGlobalDatabaseDSN) { m_sGlobalDatabaseDSN = sGlobalDatabaseDSN; }
  }


  /// Gets the system log level
  property int SystemLogLevel
  {
    int get() { return(m_iSystemLogLevel); }
    void set(int iSystemLogLevel) { m_iSystemLogLevel = iSystemLogLevel; }
  }

  /// Gets the system log location
  property System::String ^ SystemLogLocation
  {
    String ^ get() { return(m_sSystemLogLocation); }
    void set(System::String ^ sSystemLogLocation) { m_sSystemLogLocation = sSystemLogLocation; }
  }

  /// Returns the header text
  property System::String ^ HeaderText
  {
    String ^ get() { return(m_sHeaderText); }
    void set(System::String ^ sHeaderText) { m_sHeaderText = sHeaderText; }
  }

  /// Returns the header bitmap file
  property System::String ^ HeaderBitmapFile
  {
    String ^ get() { return(m_sHeaderBitmapFile); }
    void set(System::String ^ sHeaderBitmapFile) { m_sHeaderBitmapFile = sHeaderBitmapFile; }
  }


  /// Returns the dicom messenger host name/ip
  property System::String ^ DicomMessengerHostname
  {
    String ^ get() { return m_sDicomMessengerHostname; }
    void set(System::String ^ sHostname) { m_sDicomMessengerHostname = sHostname; }
  }


  /// Returns the dicom messenger port
  property int DicomMessengerPort
  {
    int get() { return m_iDicomMessengerPort; }
    void set(int iPort) { m_iDicomMessengerPort = iPort; }
  }

private:

  ///
  static const System::String ^ m_sDefaultPath = L"D:\\\\Viatronix";

  /// Share/Path list array
  System::Collections::ArrayList ^ m_pSharePathList;

  /// User List
  System::Collections::ArrayList ^ m_pDatabaseUsers;


  /// Bottom Margin
  float m_fBottomMargin;

  /// Top Margin
  float m_fTopMargin;

  /// System Log Level
  int m_iSystemLogLevel;

  /// System Log Location
  System::String ^ m_sSystemLogLocation;

  ///
  ReportNumberingType m_eNumbering;

  ///
  ReportHeaderType m_eReportHeader;

  /// Header text
  System::String ^ m_sHeaderText;

  /// Header Bitmap File
  System::String ^ m_sHeaderBitmapFile;

  /// Databases DSN
  System::String ^ m_sDatabaseDSN;

  /// Databases connection string
  System::String ^ m_sConnectionString;

  /// Global databases DSN
  System::String ^ m_sGlobalDatabaseDSN;

  /// Global databases connection string
  System::String ^ m_sGlobalConnectionString;

  ///
  System::String ^ m_sDicomMessengerHostname;

  ///
  int m_iDicomMessengerPort;

  ///
  Viatronix::Utilities::Registry::SoftwareRegistry ^ m_pSoftwareRegistry;

}; // class V3DSystemSettings



CLOSE_REGISTRY_NS


// Revision History:
// $Log: V3DSystemSettings.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.4  2003/08/05 14:09:46  mkontak
// Requery Interval
//
// Revision 1.3  2003/08/05 12:43:09  mkontak
// Added entries for GET AETItle and Port
//
// Revision 1.2  2003/06/18 20:02:21  mkontak
// int2 to uint2
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/V3DSystemSettings.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: V3DSystemSettings.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
