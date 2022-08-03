
// $Id: SystemSettings.h,v 1.16.2.2 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


// Pragmas
#pragma once

#include "SystemSettingsSectionHandler.h"

// Namespaces
using namespace Microsoft;
using namespace System;
using namespace System::Xml;


//USING_LOGGER_NS

OPEN_REGISTRY_NS


/***
 * Implements a managed wrapper class for the vxSystemSettings class in  vxBase. 
 */
public ref  class SystemSettings 
{

private:

  /// Constructor
  SystemSettings();

public:

  /// Parses expression replacing values with config values
  String ^ ParseExpression(String ^ sExpression);


private:
   

  /// Creates the system settings from the xml node
  static SystemSettings ^ Create(String ^ sConfigFile);

  /// Creates the system settings from the xml node
  static SystemSettings ^ Create(XmlNode ^pNode);

  /// Gets the attribute value
  static String ^ SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, String ^ sDefault );

  /// Gets the attribute value
  static int SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, int iDefault );

  /// Gets the attribute value
  static bool  SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, bool bDefault );

  /// Gets the XML node value as a STRING
  static String ^ SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodename, String ^ sDefault );

  /// Gets the XML node value as an INT
  static int SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodename, int iDefault );

  /// Gets the XML node value as a BOOLEAN
  static bool SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodename, bool bDefault );

  /// Parses the install directory from all other directories
  inline String ^ ParseDirectory(String ^ sDirectory)
  { return sDirectory->Replace("$(InstallDirectory)",m_sInstallDirectory)->Replace("$(APPDATA)", m_sCommonAppDataDirectory); }

public:

  /// Gets the global 
  property static SystemSettings ^ CurrentSystemSettings
  {
    SystemSettings ^ get();
  }

    /// Gets the computer name
  property String ^ ComputerName
  {
    String ^ get() { return m_sComputerName; }
    void set(String ^ sValue) { m_sComputerName = sValue; }
  }

 

  /// Gets the split dicom by acquisition number
  property bool SplitByAcquisitionNumber
  {
    bool get() { return m_bSplitByAcquisitionNumber; }
    void set(bool bValue) { m_bSplitByAcquisitionNumber = bValue; }
  }

  /// Gets the install directory
  property String ^ InstallDirectory
  {
    String ^ get() { return m_sInstallDirectory; }
    void set(String ^ sValue) { m_sInstallDirectory = sValue; }
  }

  /// Gets the config directory
  property String ^ ConfigDirectory
  {
    String ^ get() { return ParseDirectory(m_sConfigDirectory); }
    void set(String ^ sValue) { m_sConfigDirectory = sValue; }
  }

  /// Gets the services directory
  property String ^ ServicesDirectory
  {
    String ^ get() { return ParseDirectory(m_sServicesDirectory); }
    void set(String ^ sValue) { m_sServicesDirectory = sValue; }
  }

  /// Gets the viewers directory
  property  String ^ ViewersDirectory
  {
    String ^ get() { return ParseDirectory(m_sViewersDirectory); }
    void set(String ^ sValue) { m_sViewersDirectory = sValue; }
  }
  
  /// Gets the processor directory
  property String ^ ProcessorDirectory
  {
    String ^ get() { return ParseDirectory(m_sProcessorDirectory); }
    void set(String ^ sValue) { m_sProcessorDirectory = sValue; }
  }

  /// Gets the preferences directory
  property String ^ PreferencesDirectory
  {
    String ^ get() { return ParseDirectory(m_sPreferencesDirectory); }
    void set(String ^ sValue) { m_sPreferencesDirectory = sValue; }
  }

  /// Gets the Temp directory
  property String ^ TempDirectory
  {
    String ^ get() { return ParseDirectory(m_sTempDirectory); }
    void set(String ^ sValue) { m_sTempDirectory = sValue; }
  }

  /// Gets the users directory
  property String ^ UsersDirectory
  {
    String ^ get() { return ParseDirectory(m_sUsersDirectory); }
    void set(String ^ sValue) { m_sUsersDirectory = sValue; }
  }

  /// Gets the com directory
  property  String ^ ComDirectory
  {
    String ^ get() { return ParseDirectory(m_sComDirectory); }
    void set(String ^ sValue) { m_sComDirectory = sValue; }
  }

  /// Gets the utilities directory
  property String ^ UtilitiesDirectory
  {
    String ^ get() { return ParseDirectory(m_sUtilitiesDirectory); }
    void set(String ^ sValue) { m_sUtilitiesDirectory = sValue; }
  }

  /// Gets the log directory
  property String ^ LogDirectory
  {
    String ^ get() { return ParseDirectory(m_sLogDirectory); }
    void set(String ^ sValue)  { m_sLogDirectory = sValue; }
  }


  /// Gets the log level
  property Viatronix::Logging::LogLevel LogLevel
  {
    Viatronix::Logging::LogLevel get() { return m_eLogLevel; }
    void set(Viatronix::Logging::LogLevel eValue) { m_eLogLevel = eValue; }
  }


  /// Gets the viewre flag
  property bool Viewer
  {
    bool get()  { return m_bViewer; }
    void set(bool bValue) { m_bViewer = bValue; }
  }

  /// Gets the processor flag
  property  bool Processor
  {
    bool get() { return m_bProcessor; }
    void set(bool bValue) { m_bProcessor = bValue; }
  }

  /// Gets the license server
  property String ^ LicenseServer
  {
    String ^ get() { return m_sLicenseServer; }
    void set(String ^ sValue) { m_sLicenseServer = sValue; }
  }

  
  /// Gets the Caches Base Path
  property String ^  CacheBasePath
  {
    String ^ get() { return m_sCacheBasePath; }
    void set(String ^ sValue) { m_sCacheBasePath = sValue; }
  }

  /// Gets the Caches Base Path
  property String ^  CacheServer
  {
    String ^ get() { return m_sCacheServer; }
    void set(String ^ sValue) { m_sCacheServer = sValue; }
  }


  /// Gets the disk warning threshol
  property int  DiskWarningThreshold
  { 
    int get() { return m_iDiskWarningThreshold; }
    void set ( int iValue) { m_iDiskWarningThreshold = iValue; }
  }

      /// Sets the disk error threshold
  property int DiskErrorThreshold
  {
      void set(int iValue) { m_iDiskErrorThreshold = iValue; }
      int  get() { return m_iDiskErrorThreshold; }
  }
  
  /// Sets the system id
  property int SystemId
  {
    void set(int iValue) { m_iSystemId = iValue; }
    int get() { return m_iSystemId; }
  }

 
private:

  /// Internal static
  static SystemSettings ^ m_mpSystemSettings = __nullptr;

  /// System id
  int m_iSystemId;

  ///Computer Name
  String ^ m_sComputerName;

  /// Caches Base Paths deafult
  String ^ m_sCacheBasePath;

  /// Default disk warning threshold
  int m_iDiskWarningThreshold;

  /// Default disk error threshold
  int m_iDiskErrorThreshold;

  /// Split by acquisition
  bool m_bSplitByAcquisitionNumber;

 
  /// Install directory
  String ^ m_sInstallDirectory;

  /// Config directory
  String ^ m_sConfigDirectory; 

  /// Viewer directory
  String ^ m_sViewersDirectory;

  /// Cache server
  String ^ m_sCacheServer;

  /// Utilities Directory
  String ^ m_sUtilitiesDirectory;

  /// Users directory
  String ^ m_sUsersDirectory;

  /// Temp directory
  String ^ m_sTempDirectory;

  /// Log driectory
  String ^ m_sLogDirectory;

  /// COM directory
  String ^ m_sComDirectory;

  /// Log level
  Viatronix::Logging::LogLevel  m_eLogLevel;

  /// Services directory
  String ^ m_sServicesDirectory;

  /// Prefreneces directory
  String ^ m_sPreferencesDirectory;

  /// Processor directory
  String ^ m_sProcessorDirectory;

  /// Licesnes server
  String ^ m_sLicenseServer;

  /// virewer flag
  bool m_bViewer;

  /// processor flag
  bool m_bProcessor;

 
  /// Common application data directory
  String ^ m_sCommonAppDataDirectory;

};  // class SystemSettings()



CLOSE_REGISTRY_NS


// $Log: SystemSettings.h,v $
// Revision 1.16.2.2  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.1  2008/03/18 17:35:41  vxconfig
// Fix for caridac report viewing in the details pane
//
// Revision 1.16  2006/09/08 18:50:21  mkontak
// Added CacheType to system registry entries
//
// Revision 1.15  2005/12/09 14:25:42  mkontak
// Moved main code to vxBase and this class now wrap the vxBase vxSystemSettings class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SystemSettings.h,v 1.16.2.2 2011/05/18 01:56:58 mkontak Exp $
// $Id: SystemSettings.h,v 1.16.2.2 2011/05/18 01:56:58 mkontak Exp $
