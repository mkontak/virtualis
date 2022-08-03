// $Id: SystemSetting.cpp,v 1.15.2.3 2011/05/18 17:54:55 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Mark Kontak (mailto:mkontak@viatronix.com)


// includes
#include "stdafx.h"
#include "SystemSettings.h"


using namespace System;
using namespace Viatronix::Logging;

// namespaces

USING_REGISTRY_NS



SystemSettings::SystemSettings() :
m_sCacheBasePath("D:\\Studies"),
m_iDiskWarningThreshold(75),
m_iDiskErrorThreshold(95),
m_bSplitByAcquisitionNumber(false),
m_sInstallDirectory(""),
m_sConfigDirectory(""), 
m_sViewersDirectory(""),
m_sUsersDirectory(""),
m_sTempDirectory(""),
m_sUtilitiesDirectory(""),
m_sCacheServer(""),
m_sLogDirectory(""),
m_sServicesDirectory(""),
m_sPreferencesDirectory(""),
m_sProcessorDirectory(""),
m_sLicenseServer(""),
m_sComDirectory(""),
m_bViewer(false),
m_bProcessor(false),
m_iSystemId(0),
m_eLogLevel(Viatronix::Logging::LogLevel::Exception)
{
}



/**
 * Gets the node value
 *
 * @param pNode       Xml node
 * @param sNoneName   Node name
 * @param sDefault    Deafult value
 */
String ^ SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodeName, String ^ sDefault )
{
  String ^ sReturnValue(sDefault);

  XmlNode ^pSettingsNode = pNode->SelectSingleNode(sNodeName);

  if ( pSettingsNode != __nullptr )
  {
    sReturnValue = pSettingsNode->ChildNodes[0]->Value->ToString();
  }

  return sReturnValue;

} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )


/**
 * Gets the node value
 *
 * @param pNode       Xml node
 * @param sNoneName   Node name
 * @param iDefault    Deafult value
 */
int SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodeName, int iDefault )
{
  int iReturnValue(iDefault);

  XmlNode ^pSettingsNode = pNode->SelectSingleNode(sNodeName);

  if ( pSettingsNode != __nullptr )
  {
    iReturnValue = Int32::Parse(pSettingsNode->ChildNodes[0]->Value->ToString());
  }

  return iReturnValue;

} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )


/**
 * Gets the node value
 *
 * @param pNode       Xml node
 * @param sNoneName   Node name
 * @param bDefault    Deafult value
 */
bool SystemSettings::GetNodeValue(XmlNode ^pNode, String ^ sNodeName, bool bDefault )
{
  bool bReturnValue(bDefault);

  XmlNode ^pSettingsNode = pNode->SelectSingleNode(sNodeName);

  if ( pSettingsNode != __nullptr )
  {
    bReturnValue = Boolean::Parse(pSettingsNode->ChildNodes[0]->Value->ToString());
  }

  return bReturnValue;

} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )
/**
 * Gets the attribute value
 *
 * @param pNode       Xml node
 * @param sAttribute  Attrbute name
 */
String ^ SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, String ^ sDefault )
{
  return ( pNode->Attributes[sAttribute] == __nullptr ? sDefault : pNode->Attributes[sAttribute] ->Value->ToString() );
} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )

/**
 * Gets the attribute value
 *
 * @param pNode       Xml node
 * @param sAttribute  Attrbute name
 */
int SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, int iDefault )
{
  return ( pNode->Attributes[sAttribute] == __nullptr ? iDefault : Int32::Parse(pNode->Attributes[sAttribute] ->Value) );
} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )

/**
 * Gets the attribute value
 *
 * @param pNode       Xml node
 * @param sAttribute  Attrbute name
 */
bool SystemSettings::GetAttributeValue(XmlNode ^pNode, String ^ sAttribute, bool bDefault )
{
  return ( pNode->Attributes[sAttribute] == __nullptr ? bDefault : Boolean::Parse(pNode->Attributes[sAttribute] ->Value) );
} // GetAttributeValue(XmlNode ^pNode, String ^ sAttribute )

/**
 * Creates the SystemSettings object using the XML supplied
 *
 * @param sConfigFile     Config file
 *
 * @return Return the newly created SystemSettings object
 */
SystemSettings ^ SystemSettings::Create(String ^ sConfigFile)
{

  if ( ! System::IO::File::Exists(sConfigFile) )
    throw gcnew System::IO::FileNotFoundException("Config file does not exist");

  System::Xml::XmlDocument ^ pDoc(gcnew XmlDocument());
  pDoc->Load(sConfigFile);

  return Create(pDoc->DocumentElement);
} // Create(String ^ sConfigFile)

/**
 * Creates the SystemSettings object using the XML supplied
 *
 * @param pNode     XML node
 *
 * @return Return the newly created SystemSettings object
 */
SystemSettings ^ SystemSettings::Create(XmlNode ^pNode)
{
  if ( pNode == __nullptr )
    throw gcnew System::ArgumentException("Node is null");

  SystemSettings ^ pSettings(gcnew SystemSettings());


  pSettings->m_sCommonAppDataDirectory        = System::Environment::GetFolderPath(System::Environment::SpecialFolder::CommonApplicationData);
  pSettings->m_sComputerName                  = System::Windows::Forms::SystemInformation::ComputerName;
  pSettings->m_sCacheServer                   = GetNodeValue(pNode, "cacheServer",          pSettings->m_sCacheServer);
 
  pSettings->m_sCacheBasePath                 = GetNodeValue(pNode, "cacheBasePath",        pSettings->m_sCacheBasePath);
  pSettings->m_sInstallDirectory              = GetNodeValue(pNode, "installDirectory",     pSettings->m_sInstallDirectory);
  pSettings->m_sConfigDirectory               = GetNodeValue(pNode, "configDirectory",      pSettings->m_sConfigDirectory);
  pSettings->m_sViewersDirectory              = GetNodeValue(pNode, "viewersDirectory",     pSettings->m_sViewersDirectory);
  pSettings->m_sUsersDirectory                = GetNodeValue(pNode, "usersDirectory",       pSettings->m_sUsersDirectory);
  pSettings->m_sUtilitiesDirectory            = GetNodeValue(pNode, "utilitiesDirectory",   pSettings->m_sUtilitiesDirectory);
  pSettings->m_sTempDirectory                 = GetNodeValue(pNode, "tempDirectory",        System::IO::Path::GetTempPath() );
  pSettings->m_sLogDirectory                  = GetNodeValue(pNode, "logDirectory",         System::Environment::GetFolderPath(System::Environment::SpecialFolder::CommonApplicationData) );
  pSettings->m_sComDirectory                  = GetNodeValue(pNode, "comDirectory",         pSettings->m_sComDirectory);
  pSettings->m_sServicesDirectory             = GetNodeValue(pNode, "servicesDirectory",    pSettings->m_sUtilitiesDirectory);
  pSettings->m_sPreferencesDirectory          = GetNodeValue(pNode, "preferencesDirectory", pSettings->m_sUtilitiesDirectory);
  pSettings->m_sServicesDirectory             = GetNodeValue(pNode, "servicesDirectory",    pSettings->m_sUtilitiesDirectory);
  pSettings->m_sProcessorDirectory            = GetNodeValue(pNode, "processorDirectory",   pSettings->m_sUtilitiesDirectory);

  pSettings->m_sLicenseServer                 = GetNodeValue(pNode, "licenseServer",        pSettings->m_sLicenseServer);


  pSettings->m_eLogLevel                      = (Viatronix::Logging::LogLevel)Enum::Parse( Viatronix::Logging::LogLevel::typeid , GetNodeValue(pNode, "logLevel", "Exception" ));
 


  pSettings->m_iDiskErrorThreshold            = GetNodeValue(pNode, "diskErrorThreshold",   pSettings->m_iDiskErrorThreshold);
  pSettings->m_iDiskWarningThreshold          = GetNodeValue(pNode, "diskWarningThreshold", pSettings->m_iDiskWarningThreshold);
  pSettings->m_bSplitByAcquisitionNumber      = GetNodeValue(pNode, "splitByAcquisition",   pSettings->m_bSplitByAcquisitionNumber);

  pSettings->m_bViewer                        = GetNodeValue(pNode, "viewer",               pSettings->m_bViewer);
  pSettings->m_bProcessor                     = GetNodeValue(pNode, "processor",            pSettings->m_bProcessor);
 

  return pSettings;
  
} //Create(XmlNode ^pNode)

/**
 * Gets the CurrentSystems settings object which is an internal static
 *
 * @return Current system settings
 */
SystemSettings ^ SystemSettings::CurrentSystemSettings::get()
{
	if ( m_mpSystemSettings == __nullptr )
	{

    
    XmlNode ^ pSystemSettings(__nullptr);
      
    try
    {
      pSystemSettings = dynamic_cast<XmlNode ^>(Configuration::ConfigurationManager::GetSection("systemSettings"));
    }
    catch ( Exception ^ )
    {
      // Ignore Assume NULL
    }

    if ( pSystemSettings != __nullptr )
    {
	    m_mpSystemSettings = Create(pSystemSettings);
    }
    else
    {

      // =================================================================================================================
      // Program data is C:\ProgramData if we are running on windows XP or the CommonApplicationData if we are on Vista 
      // or above.
      // =================================================================================================================
      String ^ sProgramData = Environment::GetFolderPath( Environment::SpecialFolder::CommonApplicationData );
      
      String ^ sConfigFile = System::IO::Path::Combine( sProgramData, "Viatronix\\Config\\SystemSettings.config" );
      
      m_mpSystemSettings = Create(sConfigFile);
    }
	}
	return m_mpSystemSettings;
} // get_CurrentSystemSettings()



/**
 * Parses the expression replacing occurences to global expressions
 */
String ^ SystemSettings::ParseExpression(String ^ sExpression)
{
  String ^ sParse(sExpression);

  // The order is important.  If you need to modify this method ensure that all macros that refer to other macros appear first in the list.  Otherwise control strings may not be replaced.
  sParse = sParse->Replace("$(LogDirectory)", m_sLogDirectory);  
  sParse = sParse->Replace("$(ViewersDirectory)", m_sViewersDirectory);
  sParse = sParse->Replace("$(UtilitiesDirectory)", m_sUtilitiesDirectory);
  sParse = sParse->Replace("$(TempDirectory)", m_sTempDirectory);
  sParse = sParse->Replace("$(UsersDirectory)", m_sUsersDirectory);
  sParse = sParse->Replace("$(ServicesDirectory)", m_sServicesDirectory);
  sParse = sParse->Replace("$(ProcessorDirectory)", m_sProcessorDirectory);
  sParse = sParse->Replace("$(PreferencesDirectory)", m_sPreferencesDirectory);
  sParse = sParse->Replace("$(ConfigDirectory)", m_sConfigDirectory);
  sParse = sParse->Replace("$(InstallDirectory)", m_sInstallDirectory);
  sParse = sParse->Replace("$(CacheBasePath)", m_sCacheBasePath);
  sParse = sParse->Replace("$(LicenseServer)", m_sLicenseServer);
  sParse = sParse->Replace("$(ComputerName)", m_sComputerName);
  sParse = sParse->Replace("$(APPDATA)", m_sCommonAppDataDirectory);


  return sParse;
} // ParseExpression(String ^ sExpression)





// $Log: SystemSetting.cpp,v $
// Revision 1.15.2.3  2011/05/18 17:54:55  kchalupa
// CONS-1640
// Order of ParseExpression is important.  If a macro references another one the first one in the chain must be replaced first.  The referenced macro later.
//
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.15.2.2  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.15.2.1  2008/03/18 17:35:41  vxconfig
// Fix for caridac report viewing in the details pane
//
// Revision 1.15  2006/11/10 12:10:30  mkontak
// Comments
//
// Revision 1.14  2006/09/08 18:50:21  mkontak
// Added CacheType to system registry entries
//
// Revision 1.13  2006/08/28 15:24:41  frank
// formatting
//
// Revision 1.12  2005/12/09 14:25:42  mkontak
// Moved main code to vxBase and this class now wrap the vxBase vxSystemSettings class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SystemSetting.cpp,v 1.15.2.3 2011/05/18 17:54:55 kchalupa Exp $
// $Id: SystemSetting.cpp,v 1.15.2.3 2011/05/18 17:54:55 kchalupa Exp $
