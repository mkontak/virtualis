// $Id: DNSRegistry.h,v 1.4 2005/06/27 17:49:29 mkontak Exp $
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

/**
 * Implements a DSN registry.
 *
 * Allows access to the DNS settings in the regiistry for extraction of DATABASE settings. This
 * class is a READ-ONLY and does not allow the changing of any of these values. You must use the 
 * ODBC console to make any changes.
 */
public ref class DSNRegistry 
{

public:
  
  /// Constructor  
  DSNRegistry(String ^  sDSNName);

  /// Checks if the DSN nbame exists
  bool DSNExists(System::String ^ sDSNName);


public:       // Public Properties

  /// Returns a SQL connection string only
  property String ^ ConnectionString { String ^ get(); }

  /// returns the DSN driver
  property String ^ Driver { String ^ get() { return(m_sDriver); } }

  /// returns the DSN server
  property String ^ Server { String ^ get() { return(m_sServer); } }

  /// returns the DSN user
  property String ^ User { String ^ get() { return(m_sUser); } }

  /// returns the DSN password
  property String ^ Password { String ^ get() { return(m_sPassword); } }

  ///  returns the DSN description
  property String ^ Description { String ^ get() { return(m_sDescription); } }

  /// returns the DSN last user
  property String ^ LastUser { String ^ get() { return(m_sLastUser); } }

  /// returns the DSN system database (JET Only)
  property String ^ SystemDatabase { String ^ get() { return(m_sSystemDatabase); } }

  /// returns the DSN option
  property String ^ Option { String ^ get() { return(m_sOption); } }

  /// returns the DSN statement
  property String ^ Statement { String ^ get() { return(m_sStatement); } }

  /// returns the DSN Database
  property String ^ Database { String ^ get() { return(m_sDatabase); } }
  
  /// returns the DSN port to connect to
  property int Port { int get() { return(m_iPort); } }

  /// returns true if SQL server
  property bool IsSQLServer { bool get() { return(m_bSQLServer); } }

  /// returns the DSN name
  property String ^ DSNName { String ^ get() { return(m_sDSNName); } }
            
private:

  /// Subkey
  static String ^ m_sSubKey = "SOFTWARE\\ODBC\\ODBC.INI\\";

  /// Registry Key
  Microsoft::Win32::RegistryKey ^ m_pRegistryKey;

  /// SQL Server Driver Name (Partial Name) 
  static const String ^ m_sSQLServer = L"SQLSRV";

  /// DSN name
  String ^ m_sDSNName;

  /// DSN Database name
  String ^ m_sDatabase;

  /// DSN driver
  String ^ m_sDriver;

  /// DSN server name
  String ^ m_sServer;

  /// DSN user name
  String ^ m_sUser;

  /// DSN description
  String ^ m_sDescription;

  /// DSN last user
  String ^ m_sLastUser;

  /// DSN System database (JET Only)
  String ^ m_sSystemDatabase;

  /// DSN password
  String ^ m_sPassword;

  /// DSN options
  String ^ m_sOption;

  /// DSN Statement
  String ^ m_sStatement;

  /// DSN server port
  int m_iPort;

  /// DSN SQL Server flag
  bool m_bSQLServer;
  

};  // class DSNRegsitry


CLOSE_REGISTRY_NS



// $Log: DNSRegistry.h,v $
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
// Revision 1.4  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.3  2003/07/10 17:56:03  mkontak
// Coding standards, additions to DcmServerService registry
//
// Revision 1.2  2003/06/20 18:33:08  mkontak
// Coding standards
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/DNSRegistry.h,v 1.4 2005/06/27 17:49:29 mkontak Exp $
// $Id: DNSRegistry.h,v 1.4 2005/06/27 17:49:29 mkontak Exp $
