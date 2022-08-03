// $Id: vxSystemSettings.h,v 1.11 2008/04/24 21:48:29 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Mark Kontak (mailto:mkontak@viatronix.com)


// pragmas
#pragma once


// includes
#include "File.h"


/*
 *  Implements the System Setting registry subkey access interface
 */
class VX_BASE_DLL vxSystemSettings
{
// functions
private:

  /// Constructor
  vxSystemSettings() { }

public:

  /// Creates the key
  static void Create() { Create(false); }

  /// Determines the existence of this subkey
  static bool Exists();

  /// Returns the registry value for tghe entry supplied
  static std::string GetValue(const std::string & sEntry, const std::string & sDefaultValue);

  /// Returns the registry value for tghe entry supplied
  inline static int GetValue(const std::string & sEntry, int iDefaultValue)
  { return atoi(GetValue(sEntry, ToAscii(iDefaultValue)).c_str()); }

  /// Returns the registry value for tghe entry supplied
  inline static bool GetValue(const std::string & sEntry, bool bDefaultValue)
  { return (  (GetValue(sEntry, std::string(bDefaultValue ? "true" : "false")) == std::string("true") ) ? true : false ); } 

  /// Sets a registry value foor the entry supplied
  static void SetValue(const std::string & sEntry, const std::string & sValue);

  /// Sets a registry value foor the entry supplied
  inline static void SetValue(const std::string & sEntry, int iValue)
  { SetValue( sEntry, ToAscii(iValue) ); }

  /// Sets a registry value foor the entry supplied
  inline static void SetValue(const std::string & sEntry, bool bValue)
  { SetValue( sEntry, std::string(bValue ? "true" : "false")); }

  /// Parses expression replacing values with config values
  static std::string ParseExpression(const std::string & sExpression);

  /// Gets the viewer flag
  inline static bool IsViewer() 
  { return ( GetValue(std::string("SystemType"),std::string("")).find_first_of('V', 0) > 0 ? true : false ); }

    /// Gets the viewer flag
  inline static bool IsProcessor() 
  { return ( GetValue(std::string("SystemType"),std::string("")).find_first_of('P', 0) > 0 ? true : false ); }

private:

  /// Creates this subkey if it does not exist and returns the key (You must close it)
  static HKEY Create(bool bReturnKey);

  /// Trim character from th end
  static std::string TrimEnd(const std::string & sValue, const std::string & sTrimChars );

  /// Trim charaters from front and end of string
  static std::string Trim(const std::string & sValue, const std::string & sTrimChars );

  /// Replaces the search string in the value with the replace string
  static std::string Replace(const std::string & sValue, const std::string & sSearch, const std::string & sReplace);

  /// Returns the registry key for this sub key as Read-Only
  static HKEY Open();

  /// Returns the registry key for this sub key as Read-Only
  static void Close(HKEY hkey);

  /// Sets the directory value
  static void SetDirectoryValue(const std::string & sEntry, const std::string & sValue)
  { SetValue(sEntry, TrimEnd(sValue, m_sTrimChars)); }

  /// Gets the directory value
  static std::string GetParsedDirectoryValue(const std::string & sEntry, const std::string & sDefault)
  { return ParseDirectory(GetDirectoryValue(sEntry, sDefault)); }

    /// Gets the directory value
  static std::string GetDirectoryValue(const std::string & sEntry, const std::string & sDefault)
  { return TrimEnd(GetValue(sEntry, sDefault), m_sTrimChars) + "\\"; }

  /// Parses directory replacing the InstallDirectory with the actual directory
  static std::string ParseDirectory(const std::string & sDirectory);

  /// Combines the paths 
  static std::string PathCombine(const std::string & sPath1, const std::string & sPath2);

public:

  /// Gets the split dicom by acquisition number
  inline static bool GetDicomSplitByAcquisitionNumber() 
  { return GetValue(std::string("DicomSplitByAquisitionNumber"), false); }

  /// Sets the split dicom by acquisition number
  inline static void SetDicomSplitByAcquisitionNumber(bool bValue) 
  { return SetValue(std::string("DicomSplitByAquisitionNumber"), bValue); }

  /// Gets the install directory
  inline static std::string GetInstallDirectory() 
  { 
		 try
		 {
			 return GetDirectoryValue(std::string("InstallDirectory"), std::string("C:\\V-System\\")); 
		 }
		 catch ( ex::V3DRegistryNotFoundException e )
		 {
			 // return the working directory. This will help viewers run without the registry 
		 	 return vx::File::GetWorkingDir().GetName();
		 }
	} // GetInstallDirectory() 

  /// Sets the install directory
  inline static void SetInstallDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("InstallDirectory"), sValue); }

  /// Gets the config directory
  inline static std::string GetConfigDirectory() 
  { return GetParsedDirectoryValue(std::string("ConfigDirectory"), PathCombine(GetInstallDirectory(),std::string("vxConfig")) ); }

  /// Sets the config directory
  inline static void SetConfigDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("ConfigDirectory"), sValue); }

  /// Gets the services directory
  inline static std::string GetServicesDirectory() 
  { return GetParsedDirectoryValue(std::string("ServicesDirectory"), PathCombine(GetInstallDirectory(),std::string("vxServices")) ); }

  /// Sets the services directory
  inline static void SetServicesDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("ServicesDirectory"), sValue); }

  /// Gets the viewers directory
  inline static std::string GetViewersDirectory() 
  { return GetParsedDirectoryValue(std::string("ViewersDirectory"),PathCombine(GetInstallDirectory(),std::string("vxViewers")) ); }

  /// Sets the viewers directory
  inline static void SetViewersDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("ViewersDirectory"), sValue); }

  /// Gets the processor directory
  inline static std::string GetProcessorDirectory() 
  { return GetParsedDirectoryValue(std::string("ProcessorDirectory"), PathCombine(GetInstallDirectory(),std::string("vxProcessor")) ); }

  /// Sets the processor directory
  inline static void SetProcessorDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("ProcessorDirectory"), sValue); }

  /// Gets the preferences directory
  inline static std::string GetPreferencesDirectory() 
  { return GetParsedDirectoryValue(std::string("PreferencesDirectory"), std::string("C:\\Documents and Settings\\All Users\\Application Data\\Viatronix") ); }

  /// Sets the preferences directory
  inline static void SetPreferencesDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("PreferencesDirectory"), sValue); }

  /// Gets the Temp directory
	inline static std::string GetTempDirectory() 
	{
		try
		{
			return GetParsedDirectoryValue(std::string("TempDirectory"),PathCombine(GetInstallDirectory(),std::string("Temp")) ); 
		}
		catch ( ex::V3DRegistryNotFoundException e  )
		{
			// return the working directory. This will help viewers run without the registry
			return PathCombine(GetInstallDirectory(),std::string("Temp"));
		}
	} // GetTempDirectory() 

  /// Sets the Temp directory
  inline static void SetTempDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("TempDirectory"), sValue); }

  /// Gets the users directory
  inline static std::string GetUsersDirectory() 
  { return GetParsedDirectoryValue(std::string("UsersDirectory"), PathCombine(GetInstallDirectory(),std::string("vxUsers")) ); }

  /// Sets the users directory
  inline static void SetUsersDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("UsersDirectory"), sValue); }

  /// Gets the com directory
  inline static std::string GetComDirectory() 
  { return GetParsedDirectoryValue(std::string("ComDirectory"), PathCombine(GetInstallDirectory(),std::string("com")) ); }

  /// Sets the com directory
  inline static void SetComDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("ComDirectory"), sValue); }

  /// Gets the utilities directory
  inline static std::string GetUtilitiesDirectory() 
  { return GetParsedDirectoryValue(std::string("UtilitiesDirectory"), PathCombine(GetInstallDirectory(),std::string("vxUtilities")) ); }

  /// Sets the utilities directory
  inline static void SetUtilitiesDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("UtilitiesDirectory"), sValue); }

  /// Gets the log directory
  inline static std::string GetLogDirectory() 
  { return GetParsedDirectoryValue(std::string("LogDirectory"), PathCombine(GetInstallDirectory(),std::string("Log")) ); }

  /// Sets the log directory
  inline static void SetLogDirectory(const std::string & sValue) 
  { SetDirectoryValue(std::string("LogDirectory"), sValue); }

  /// Gets the processor name
  static std::vector<std::string> GetProcessors(); 

  /// Sets the proicessor name
  static void SetProcessors(std::vector<std::string>);

  /// Gets the log level
  inline static int4 GetLogLevel() 
  { return GetValue(std::string("LogLevel"), 5 ); }

  /// Sets the log level
  inline static void SetLogLevel(int4 iValue) 
  { SetValue(std::string("LogLevel"), iValue); }


  /// Gets the connection string
  static std::string GetConnectionString();

  /// Sets the connection string
  inline static void SetConnectionString(const std::string & sValue) 
  { SetValue(std::string("ConnectionString"), sValue); }

  /// Gets the ADO connection string
  static std::string GetADOConnectionString();


  /// Sets the viewer flag
  inline static void SetViewer(bool bValue) 
  { SetValue(std::string("SystemType"), std::string( bValue ? ( IsProcessor() ? "VP" : "V" ) : ( IsProcessor() ? "P" : "" ) ) ); }

  /// Sets the processor flag
  inline static void SetProcessor(bool bValue) 
  { SetValue( std::string("SystemType"), std::string( bValue ? ( IsViewer() ? "VP" : "P" ) : ( IsViewer() ? "V" : "" ) ) ); }


  /// Gets the license server
  static std::string GetLicenseServer(); 

  /// Sets the license server
  inline static void SetLicenseServer(const std::string & sValue) 
  {  SetValue(std::string("LicenseServer"), sValue ); }

  /// Gets the Store AE Title
  inline static std::string GetStoreAETitle() 
  { return GetValue(std::string("StoreAETitle"), std::string("V3DSTORE")); }

  /// Sets the Store AE Title
  inline static void SetStoreAETitle(const std::string & sValue) 
  { SetValue(std::string("StoreAETitle"), sValue); }

  /// Gets the Retrieval AE Title
  inline static std::string GetRetrievalAETitle() 
  { return GetValue(std::string("RetrievalAETitle"), std::string("V3DQR")); }

  /// Sets the Retrieval AE Title
  inline static void SetRetrievalAETitle(const std::string & sValue) 
  { SetValue("RetrievalAETitle", sValue); }

  /// Gets the Retrieval port
  inline static int GetRetrievalPort() 
  { return GetValue(std::string("RetrievalPort"), 20); }

  /// Sets the Retrieval port
  inline static void SetRetrievalPort(int iValue) 
  { SetValue(std::string("RetrievalPort"), iValue); }

  /// Sets the SMTP server value
  inline static void SetSmtpServer(const std::string & sValue) 
  { SetValue(std::string("SmtpServer"), sValue); }

  /// Gets the SMTP server value
  inline static std::string GetSmtpServer() 
  { return GetValue(std::string("SmtpServer"), std::string("")); }

  /// Sets the Support Email Addresss
  inline static void SetSupportEmailAddress(const std::string & sValue) 
  { SetValue(std::string("SupportEmailAddress"), sValue); }

  /// Gets the Support Email Addresss 
  inline static std::string GetSupportEmailAddress() 
  { return GetValue(std::string("SupportEmailAddress"), std::string("")); }

  /// Sets the Support Email Addresss
  inline static void SetSupportEmailLevel(int4 iValue) 
  { SetValue(std::string("SupportEmailLevel"), iValue); }

  /// Gets the Support Email Addresss 
  inline static int4 GetSupportEmailLevel() 
  { return GetValue(std::string("SupportEmailLevel"), 5); }

    /// Sets the Support Email Addresss
  inline static void SetAdministratorEmailAddress(const std::string & sValue) 
  { SetValue(std::string("AdministratorEmailAddress"), sValue); }

  /// Gets the Support Email Addresss 
  inline static std::string GetAdministratorEmailAddress() 
  { return GetValue(std::string("AdministratorEmailAddress"), std::string("")); }

  /// Sets the Support Email Addresss
  inline static void SetAdministratorEmailLevel(int4 iValue) 
  { SetValue(std::string("AdministratorEmailLevel"), iValue); }

  /// Gets the Support Email Addresss 
  inline static int4 GetAdministratorEmailLevel() 
  { return GetValue(std::string("AdministratorEmailLevel"),  4); }

    /// Sets the Caches Base Path
  inline static void SetCacheBasePath(const std::string & sValue) 
  { SetDirectoryValue(std::string("CacheBasePath"), sValue); }

  /// Gets the Caches Base Path
  inline static std::string  GetCacheBasePath() 
  { return GetParsedDirectoryValue(std::string("CacheBasePath"), m_sDefaultCacheBasePath); }

  /// Sets the Cache Server
  inline static void SetCacheServer(const std::string & sValue) 
  { SetValue(std::string("CacheServer"), sValue); }

  /// Gets the Cache Server
  inline static std::string  GetCacheServer() 
  { return GetValue(std::string("CacheServer"), std::string("")); }

    /// Sets the Cache Type
  inline static void SetCacheType(const std::string & sValue) 
  { SetValue(std::string("CacheType"), sValue); }

  /// Gets the Cache Type
  inline static std::string  GetCacheType() 
  { return GetValue(std::string("CacheType"), std::string("Local")); }


    /// Sets the disk warning threshold
  inline static void SetDiskWarningThreshold(int iValue) 
  { SetValue(std::string("DiskWarningThreshold"), iValue); }

  /// Gets the disk warning threshol
  inline static int  GetDiskWarningThreshold() 
  { return GetValue(std::string("DiskWarningThreshold"), m_iDefaultDiskWarningThreshold); }

      /// Sets the disk error threshold
  static void SetDiskErrorThreshold(int iValue) { SetValue("DiskErrorThreshold", iValue); }

  /// Gets the disk error threshol
  static int  GetDiskErrorThreshold() { return GetValue("DiskErrorThreshold", m_iDefaultDiskErrorThreshold); }
  
  /// Sets the system id
  static void SetSystemId(int iValue) { SetValue("SystemId", iValue); }

  /// Gets the system id
	static int  GetSystemId() 
	{ 
		try
		{
			return GetValue("SystemId", (int)0); 
		}
		catch ( ex::V3DRegistryNotFoundException e  )
		{
			// return the working directory. This will help viewers run without the registry
			return 0;
		}
	}// GetSystemId()

private:

  /// Hashtable Cache
  //static System::Collections::Hashtable * m_pCache = new System::Collections::Hashtable();
  static std::map<std::string, std::string> m_cache;

  /// Sub Key
  static const std::string m_sSubKey;

  /// License file name
  static const std::string  m_sLicencesFilename;

  /// Trim characters
  static const std::string m_sTrimChars;

  /// Process Name Delimiters
  static const char m_ProcessorNameDelimiterChars [];

  /// Caches Base Paths deafult
  static const std::string m_sDefaultCacheBasePath;

  /// Default disk warning threshold
  static const int m_iDefaultDiskWarningThreshold = 75;

  /// Default disk error threshold
  static const int m_iDefaultDiskErrorThreshold = 90;

};  // class V3DSystemSettings


// $Log: vxSystemSettings.h,v $
// Revision 1.11  2008/04/24 21:48:29  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.10  2008/03/19 19:45:16  vxconfig
// fix cardiac viewing in the console
//
// Revision 1.7.2.3  2008/03/19 19:45:03  vxconfig
// fix cardiac viewing in the console
//
// Revision 1.7  2007/03/09 18:56:44  mkontak
// Coding standards
//
// Revision 1.6  2006/09/08 18:49:54  mkontak
// Added CacheType to system registry entries
//
// Revision 1.5  2006/08/28 17:31:26  frank
// formatting
//
// Revision 1.4  2006/08/24 16:36:54  romy
// Exception Handles for V3D Registry not Fond exception.  This helps all viewers to run independent of Viatronix Registry.
//
// Revision 1.3  2006/04/01 04:03:57  mkontak
// Change to fix the problem with the \ at the end of the path
//
// Revision 1.2  2005/12/09 14:23:25  mkontak
// Coding standards
//
// Revision 1.1  2005/12/09 14:00:33  mkontak
// Moved code ffrom managed to unmanaged so that the logger can access it.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxSystemSettings.h,v 1.11 2008/04/24 21:48:29 jmeade Exp $
// $Id: vxSystemSettings.h,v 1.11 2008/04/24 21:48:29 jmeade Exp $
