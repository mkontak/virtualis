// $Id: vxSystemSettings.C,v 1.16 2008/04/24 21:48:29 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxSystemSettings.h"
#include "SystemInfo.h"

// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.16 $"

// static initialization
const std::string vxSystemSettings::m_sSubKey  = "SOFTWARE\\Viatronix\\SystemSettings";
const std::string vxSystemSettings::m_sTrimChars = "\\/";
const char vxSystemSettings::m_ProcessorNameDelimiterChars [] = { '|', ';', ',', ':' };
const std::string vxSystemSettings::m_sDefaultCacheBasePath = "\\\\127.0.0.1\\Studies";
const std::string vxSystemSettings::m_sLicencesFilename = "viatronix.lic";
std::map<std::string, std::string> vxSystemSettings::m_cache;


/** 
 * Returns the registry key for this sub key
 *
 * @return Returns the registry key for this sub key
 */
HKEY vxSystemSettings::Open()
{
  HKEY hkey;

  long rc(ERROR_SUCCESS);

  if ( (rc = RegOpenKey(HKEY_LOCAL_MACHINE, (LPCSTR)m_sSubKey.c_str(), & hkey )) != ERROR_SUCCESS )
  {
    throw ex::VException(LogRec("Could not open registry key for SystemSettings [rc=" + ToAscii(rc) + "]", "vxSystemSettings", "OpenKey"));
  }

  return hkey;

} // Open()

/** 
 * Closes the supplied key
 *
 * @param hkey  Registry key to close
 */
void vxSystemSettings::Close(HKEY hkey)
{
  RegCloseKey(hkey);
} // Close(HKEY hkey)


/** 
 * Determmines if the subkey exists
 *
 * @return Returns true if the subkey exists false otherwsie
 */
bool vxSystemSettings::Exists()
{

  bool bReturn(false);

  HKEY hkey;
  long rc(ERROR_SUCCESS);

  if ( (rc = RegOpenKey(HKEY_LOCAL_MACHINE, (LPCSTR)m_sSubKey.c_str(), & hkey )) == ERROR_SUCCESS )
  {
    bReturn = true;
    RegCloseKey(hkey);
  }

  return bReturn;

} // Exist()
  
/** 
 * Creates the subkey
 *
 * @param bReturnKey    Flag to indicate tha the created key is to be returned
 *
 * @return Newly created key or NULL
 */
HKEY vxSystemSettings::Create(bool bReturnKey)
{

  HKEY hkey;
  long rc(ERROR_SUCCESS);

  if ( (rc = RegOpenKey(HKEY_LOCAL_MACHINE, (LPCSTR)m_sSubKey.c_str(), & hkey )) != ERROR_SUCCESS )
  {
    LogDbg("Creating SystemSettings registry key", "vxSystemSettings", "Create");

    if ( (rc = RegCreateKey(HKEY_LOCAL_MACHINE, (LPCSTR)m_sSubKey.c_str(), & hkey )) != ERROR_SUCCESS )
    {  
      throw ex::VException(LogRec("Could not create registry key for SystemSettings [rc=" + ToAscii(rc) + "]", "vxSystemSettings", "OpenKey"));
    }
   
  }

  if ( ! bReturnKey )
  {
    RegCloseKey(hkey);
    hkey = NULL;
  }

  return hkey;

} // Create()


/** 
* Gets the registry value specified in the entry
* 
* @param sEntry             Registry entry 
* @param sDefaultValue      Registry default value
*
* @return Returns the registry value if it exists or the default value.
*/
std::string vxSystemSettings::GetValue(const std::string & sEntry, const std::string & sDefaultValue)
{ 
  std::map <std::string, std::string> :: const_iterator iter;


  std::string sValue(sDefaultValue);

  iter = m_cache.find(sEntry);
  
  if ( iter == m_cache.end() )
  {
    LogTmp("Retreiving value for [ENTRY=" + sEntry + "]", "vxSystemSettings", "GetValue");

    HKEY hkey(NULL);

    try
    {
      hkey = Open();
    }
    catch ( ex::VException & )
    {
      LogErr( "Invalid installation missing SystemSettings, using defaults", "vxSystemSettings", "GetValue" );
      return sValue;
      //throw ex::V3DRegistryNotFoundException( LogRec( "Invlaid installation missing SystemSettings", "vxSystemSettings", "GetValue" ) );
    }

    try
    {
      
      DWORD size(1024);
      BYTE buffer[1024];
      DWORD type(REG_SZ);
      long rc(ERROR_SUCCESS);

      if ( (rc = RegQueryValueEx(hkey, sEntry.c_str(), NULL, &type, buffer, &size)) != ERROR_SUCCESS )
      {
        LogTmp("Could not retrieve value from registry [ENTRY=" + sEntry + "] [ERROR_CODE=" + ToAscii(rc) + "], using the default [VALUE=" + sValue + "]", "vxSystemSettings", "GetValue");
      }
      else
      {
        sValue = std::string(reinterpret_cast<char *>(buffer));
        m_cache.insert(std::pair<std::string,std::string>(sEntry, sValue));
      }

      LogTmp("Value for [ENTRY=" + sEntry + "] is [VALUE=" + sValue + "]", "vxSystemSettings", "GetValue");

      Close(hkey);
    }
    catch ( ... )
    {
      Close(hkey);
    }
  }
  else
  {
    sValue = iter->second;
  }

  return sValue;

} // GetValue(const std::string & sEntry, const std::string & sDefaultValue)

/** 
 * Sets the registry value specified in the entry
 * 
 * @param sEntry      Registry entry 
 * @param sValue      Registry value
 */
void vxSystemSettings::SetValue(const std::string & sEntry, const std::string & sValue)
{ 

  HKEY hkey = Create(true); 

  try
  {      
    if ( RegSetValue(hkey, sEntry.c_str(), REG_SZ, sValue.c_str(),sValue.size()) != ERROR_SUCCESS )
    {
      throw ex::VException(LogRec("Failed to set value for entry [" + sEntry + "]", "vxSystemSettings", "SetValue"));
    }

    m_cache.insert(std::pair<std::string,std::string>(sEntry, sValue));

    Close(hkey);

  }
  catch ( ... )
  {
   Close(hkey);

  }

} // SetValue(const std::string & sEntry, const std::string & sValue))

/** 
 * Gets the ADO connection string
 */
std::string vxSystemSettings::GetADOConnectionString()
{
  std::string sADOConnectionString(tolower(GetConnectionString()));

  if ( sADOConnectionString.size() > 0 )
  {
    sADOConnectionString = Replace(sADOConnectionString, std::string("data source"),std::string("server"));
    sADOConnectionString = Replace(sADOConnectionString, std::string("(local)"), SystemInfo::GetNetBiosComputerName() );
    sADOConnectionString = Replace(sADOConnectionString, std::string("($ComputerName)"), SystemInfo::GetNetBiosComputerName() );
    sADOConnectionString = Replace(sADOConnectionString, std::string("initial catalog"),std::string("database"));
    sADOConnectionString = Replace(sADOConnectionString, std::string("provider=sqloledb"),std::string(""));
    sADOConnectionString = Trim(sADOConnectionString, std::string(" "));
    sADOConnectionString = Trim(sADOConnectionString, std::string(";"));

  }

  return sADOConnectionString;
} // GetADOConnectionString()


/**
 * Parses the expression replacing occurences to global expressions
 *
 * @param sExpression   expression to be parse
 *
 * @return Parse expression with all variables expanded
 */
std::string vxSystemSettings::ParseExpression(const std::string & sExpression)
{

  std::string sParse(sExpression);

  sParse = Replace(sParse, std::string("$(LogDirectory)"), vxSystemSettings::GetLogDirectory());  
  sParse = Replace(sParse, std::string("$(ViewersDirectory)"), vxSystemSettings::GetViewersDirectory());
  sParse = Replace(sParse, std::string("$(UtilitiesDirectory)"), vxSystemSettings::GetUtilitiesDirectory());
  sParse = Replace(sParse, std::string("$(TempDirectory)"), vxSystemSettings::GetTempDirectory());
  sParse = Replace(sParse, std::string("$(UsersDirectory)"), vxSystemSettings::GetUsersDirectory());
  sParse = Replace(sParse, std::string("$(ServicesDirectory)"), vxSystemSettings::GetServicesDirectory());
  sParse = Replace(sParse, std::string("$(ProcessorDirectory)"), vxSystemSettings::GetProcessorDirectory());
  sParse = Replace(sParse, std::string("$(PreferencesDirectory)"), vxSystemSettings::GetPreferencesDirectory());
  sParse = Replace(sParse, std::string("$(ConfigDirectory)"), vxSystemSettings::GetConfigDirectory());
  sParse = Replace(sParse, std::string("$(InstallDirectory)"), vxSystemSettings::GetInstallDirectory());
  sParse = Replace(sParse, std::string("$(CacheBasePath)"), vxSystemSettings::GetCacheBasePath());
  sParse = Replace(sParse, std::string("$(LicenseServer)"), vxSystemSettings::GetLicenseServer());
  sParse = Replace(sParse, std::string("$(StoreAETitle)"), vxSystemSettings::GetStoreAETitle());
  sParse = Replace(sParse, std::string("$(RetrievalAETitle)"), vxSystemSettings::GetRetrievalAETitle());
  sParse = Replace(sParse, std::string("$(RetrievalPort)"), ToAscii(vxSystemSettings::GetRetrievalPort()) );
  sParse = Replace(sParse, std::string("$(SmtpServer)"), vxSystemSettings::GetSmtpServer());
  sParse = Replace(sParse, std::string("$(SupportEmailAddress)"), vxSystemSettings::GetSupportEmailAddress());
  sParse = Replace(sParse, std::string("$(AdministratorEmailAddress)"), vxSystemSettings::GetAdministratorEmailAddress());
  sParse = Replace(sParse, std::string("$(ComputerName)"), SystemInfo::GetNetBiosComputerName());


  return sParse;
} // ParseExpression(std::string & sExpression)

  /// Gets the processor name

/**
 * Returns the liost of processors
 *
 * @return list of processors
 */
std::vector<std::string> vxSystemSettings::GetProcessors()
{
  std::vector<std::string> vValues;

  std::string sValue = GetValue(std::string("Processors"), std::string("")); 

  int start(0);
  int end(0);

  while ( (end = sValue.find(',',start)) > 0 )
  {
    std::string sProcessor = sValue.substr(start, end - start);
    start = end + 1;
    vValues.push_back(sProcessor);
  }

  return vValues;
} // GetProcessors()


/**
 * Sets the processor name
 *
 * @param vValues       vector of processors
 */
void vxSystemSettings::SetProcessors(std::vector<std::string> vValues) 
{
  std::string sValue;
  int last(vValues.size() - 1);
  for ( int i(0); i < vValues.size(); ++i )
  {
    sValue += vValues[i] + ( i < last ? "," : "" );
  }


  SetValue("Processors", sValue); 
} // SetProcessors(std::vector<std::string> vValues) 

/**
 * Gets the license server
 *
 * @return license server
 */
std::string vxSystemSettings::GetLicenseServer()
{ 
  return GetValue(std::string("LicenseServer"), SystemInfo::GetNetBiosComputerName() ); 
} // GetLicenseServer()


/**
 * Parses the directory by replacing the $(InstallDirectory) macro with the actual directory.
 * This is used in the Get...() methods (properties).
 *
 * @param sDirectory    Directory to be parsed
 *
 * @return Returns the newly parsed directory
 */
std::string vxSystemSettings::ParseDirectory(const std::string & sDirectory)
{
  return Replace(sDirectory, std::string("$(InstallDirectory)"), vxSystemSettings::GetInstallDirectory());  
} // ParseDirectory(const std::string & sDirectory)


/**
 * Gets the connection string
 *
 * @return connection string
 */
std::string vxSystemSettings::GetConnectionString()
{ 
  std::string sConnectionString(GetValue(std::string("ConnectionString"), std::string("") ));

  return Replace(Replace(sConnectionString, std::string("(local)"), SystemInfo::GetNetBiosComputerName() ), std::string("($ComputerName)"), SystemInfo::GetNetBiosComputerName()); 
} // GetConnectionString()


/**
 * Combines tow paths
 *
 * @param sPath1    First path
 * @paran sPath2    Second path
 *
 * @return Returns the combined paths with the appropirte delimiter in the middle
 */
std::string vxSystemSettings::PathCombine(const std::string & sPath1, const std::string & sPath2)
{

  return TrimEnd(sPath1,m_sTrimChars) + "\\" + sPath2;
} // PathCombine(const std::string & sPath1, const std::string & sPath2)

/**
* Trims all characters specified from the end of the supplied sValue striong
*
* @param  sValue      Value to be trimmed
* @param  sTrimChars  Characters to be trimmed
*
* @return Returns the newly trimmed string
*/
std::string vxSystemSettings::TrimEnd(const std::string & sValue, const std::string & sTrimChars )
{
  int i(sValue.size() - 1);
  for ( ; i >= 0 && sTrimChars.find(sValue[i],0) != std::string::npos; --i );

  std::string sNew(sValue.substr(0,i + 1));

  return sNew;

} // TrimEnd(const std::string & sValue, const std::string & sTrimChars )

/**
 * Trims all the charactes specified from the beginning and end of the supplied string sValue
 *
 * @param   sValue      Value to be trimmed
 * @param   sTrimChars  Characters to be trimmed
 *
 * @return Returns a newly trimnmed string
 */
std::string vxSystemSettings::Trim( const std::string & sValue, const std::string & sTrimChars )
{

  std::string sNew("");

  int i(0);

  // 
  for ( i = 0; i < sValue.size() && sTrimChars.find(sValue[i],0) != std::string::npos; ++i  );

  sNew = sValue.substr(i, sValue.size() - i);

  return TrimEnd(sNew, sTrimChars);

} // Trim( const std::string & sValue, const std::string & sTrimChars )


/**
 * Search for the string specified in the sSearch parameter in the sValue string and replaces it
 * with the sReplace string.
 *
 * @param sValue    Value to be searched and replaced
 * @param sSearch   Search string
 * @param sReplace  Replace string
 *
 * @return Returns the newly generated string with the all string of sSearch replaced with sReplace
 */
std::string vxSystemSettings::Replace(const std::string & sValue, const std::string & sSearch, const std::string & sReplace)
{
  std::string sNew(sValue);

  int pos = sNew.find(sSearch,0);

  while ( pos != std::string::npos )
  {
    sNew.replace(pos, sSearch.size(), sReplace);

    pos = sNew.find(sSearch, pos);
  }

  return sNew;
} // Replace(std::string & sValue, const std::string & sSearch, const std::string & sReplace)

#undef FILE_REVISION

// $Log: vxSystemSettings.C,v $
// Revision 1.16  2008/04/24 21:48:29  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.15  2008/03/19 19:45:22  vxconfig
// fix cardiac viewing in the console
//
// Revision 1.12.2.3  2008/03/19 19:44:55  vxconfig
// fix cardiac viewing in the console
//
// Revision 1.12  2007/03/09 18:56:43  mkontak
// Coding standards
//
// Revision 1.11  2006/11/10 12:10:18  mkontak
// Fix problem with no registry
//
// Revision 1.10  2006/11/10 12:07:03  mkontak
// Fix problem with no registry
//
// Revision 1.9  2006/08/28 17:31:26  frank
// formatting
//
// Revision 1.8  2006/08/24 16:37:44  romy
// Exception Handles for V3D Registry not Fond exception.  This helps all viewers to run independent of Viatronix Registry.
//
// Revision 1.7  2006/05/31 16:01:00  geconomos
// porting to visual studio 2005
//
// Revision 1.6  2006/05/04 12:55:20  mkontak
// Changed warning message for missing registry elements to a temp message
// since we get these messages to much in the logs.
//
// Revision 1.5  2006/04/01 04:03:45  mkontak
// Change to fix the problem with the \ at the end of the path
//
// Revision 1.4  2005/12/29 13:28:23  mkontak
// Fix
//
// Revision 1.3  2005/12/27 19:58:03  mkontak
// Fix to registry Retrieval
//
// Revision 1.2  2005/12/09 14:24:27  mkontak
// Coding standards
//
// Revision 1.1  2005/12/09 14:00:43  mkontak
// Moved code ffrom managed to unmanaged so that the logger can access it.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxSystemSettings.C,v 1.16 2008/04/24 21:48:29 jmeade Exp $
// $Id: vxSystemSettings.C,v 1.16 2008/04/24 21:48:29 jmeade Exp $
