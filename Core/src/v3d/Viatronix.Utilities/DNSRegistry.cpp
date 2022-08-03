// $Id: DNSRegistry.cpp,v 1.4 2005/06/27 17:49:29 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/DNSRegistry.cpp,v 1.4 2005/06/27 17:49:29 mkontak Exp $
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
#include "DNSRegistry.h"
#include <windows.h>



// Namespaces
using namespace Microsoft;
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;


USING_REGISTRY_NS


/**
 * Constructor
 *
 * @param sDSNName      DSN name to lookup
 */
DSNRegistry::DSNRegistry(String ^ sDSNName) :
/**^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
m_sDSNName(L""),
m_sDriver(L""),
m_sServer(L""),
m_sUser(L""),
m_sDescription(L""),
m_sLastUser(L""),
m_sSystemDatabase(L""),
m_sPassword(L""),
m_sOption(L""),
m_sStatement(L""),
m_iPort(-1),
m_pRegistryKey(__nullptr),
m_bSQLServer(false)
{
  //////////////////////////////////////////
  // Make sure the DSN name was specified
  /////////////////////////////////////////
  if ( String::Compare(sDSNName, L"") == 0 )
  {
    throw gcnew System::Exception(L"No DSN name specified");
  } // END ... If the DSN name is not specified

  m_sDSNName = sDSNName;

  Win32::RegistryKey ^ pKey = Win32::Registry::LocalMachine->OpenSubKey(String::Concat(m_sSubKey,  m_sDSNName));

  if ( pKey != __nullptr )
  {

    //////////////////////////////////////////////
    // Extract the properties from the registry
    //////////////////////////////////////////////
    m_sServer = pKey->GetValue(L"Server", String::Empty)->ToString();
    m_sUser = pKey->GetValue(L"User", String::Empty)->ToString();
    m_sDescription = pKey->GetValue(L"Description", String::Empty)->ToString();
    m_sLastUser = pKey->GetValue(L"LastUser", String::Empty)->ToString();
    m_sSystemDatabase = pKey->GetValue(L"SystemDatabase", String::Empty)->ToString();
    m_sPassword = pKey->GetValue(L"Password", String::Empty)->ToString();
    m_sStatement = pKey->GetValue(L"Stmt", String::Empty)->ToString();
    m_sDriver = pKey->GetValue(L"Driver", String::Empty)->ToString();
    m_sOption = pKey->GetValue(L"Option", String::Empty)->ToString();
    m_sDatabase = pKey->GetValue(L"Database", String::Empty)->ToString();

    m_iPort = Convert::ToInt32(pKey->GetValue("Port", gcnew String("-1")));


    //////////////////////////////////////////////////
    // Determine if the entry is a SQL server entry
    ///////////////////////////////////////////////////
    if ( m_sDriver->ToUpper()->LastIndexOf(const_cast<String ^>(m_sSQLServer)) >= 0 )
    {
      m_bSQLServer = true;
    } // END ... If the SQLSRV text is in the driver text

  }
} // DSNRegistry()


/** 
 * Returns true if the DSN exists
 *
 * @param sDSNName      DSN Name to check
 *
 * @return Returns true if the DSN name exists, false otherwise
 */
bool DSNRegistry::DSNExists(String ^ sDSNName)
{
  Win32::RegistryKey ^ pKey = Win32::Registry::LocalMachine->OpenSubKey(String::Concat(m_sSubKey,sDSNName));

  try
  {
    return ( pKey != __nullptr ); 
  }
  __finally
  {
    pKey->Close();
  }
} // Exists()



/**
 * Returns a connection string.
 *
 * @return Returns the connection string using the DNS database settings
 */
String ^ DSNRegistry::ConnectionString::get()
{

  String ^ sConnectionString = L"";

  if ( m_bSQLServer )
  {
    array<String ^> ^ m_sList = gcnew array<String ^>(5);

    m_sList[0] = L"Provider=sqloledb;";
    m_sList[1] = L"Data Source=";
    m_sList[2] = m_sServer;
    m_sList[3] = L";Initial Catalog=";
    m_sList[4] = m_sDatabase;
 
    sConnectionString = String::Concat(m_sList);
  }

  return(sConnectionString);

} // get_ConnectionString()



// $Log: DNSRegistry.cpp,v $
// Revision 1.4  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.3  2004/11/16 15:01:34  mkontak
// Fix
//
// Revision 1.2  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/10/30 15:37:04  mkontak
// Fix for DNSExists
//
// Revision 1.3  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.2  2003/07/10 17:56:03  mkontak
// Coding standards, additions to DcmServerService registry
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/DNSRegistry.cpp,v 1.4 2005/06/27 17:49:29 mkontak Exp $
// $Id: DNSRegistry.cpp,v 1.4 2005/06/27 17:49:29 mkontak Exp $
