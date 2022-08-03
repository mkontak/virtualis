// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LaunchRequest.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include <afxwin.h>


/**
 * Implements a launch request containing all the required information to perform a remote launch of a viewer/application
 */
class LaunchRequest 
{

public:

  // constructor
  LaunchRequest();

public:

  /// Gets the resource (HTTP - resource)
  CString GetResource() { return m_sResource; }

  /// Gets the application code
  CString GetApplication() { return m_sApplication;  }

  /// Sets the application code
  void SetApplication(CString sApplication)  { m_sApplication = sApplication; }

  /// Gets the user name
  CString GetUserName() { return m_sUserName; }

  /// Sets the user name
  void SetUserName(CString sUserName) {  m_sUserName = sUserName; }

  /// Gets the server name
  CString GetServer() { return m_sServer; }

  /// Sets the server name
  void SetServer(CString sServer) { m_sServer = sServer; }

  /// Gets the users password
  CString GetPassword() { return m_sPassword; }

  /// Sets the users password
  void SetPassword(CString sPassword) { m_sPassword = sPassword; }

  /// Gets the port the service is listening on
  unsigned int GetPort() { return m_iPort; }

  /// Sets the port the service is listening on
  void SetPort(unsigned int iPort) { m_iPort = iPort;  }

  /// Gets the Uids to be launched
  CString GetUids() { return m_sUids; }

  /// Sets the UID's to be launched
  void SetUids(CString  sUids) { m_sUids = sUids; }

  /// Get the context
  CString GetContext() { return m_sContext; }

  /// Sets the context
  void SetContext(CString sContext) { m_sContext = sContext; }

  /// Gets the session description
  CString GetDescription() { return m_sDescription; }

  /// Sets the session description
  void SetDescription(CString sDescription) { m_sDescription = sDescription; }

  /// Gets the New Session Flag ( 1 or 0 )
  bool GetNewSession() { return ( m_sNewSession == "1" ? true : false ); }

  /// Sets the New session flag
  void SetNewSession(bool bNewSesion) { m_sNewSession = (bNewSesion  ? "1" : ""); }

  /// Gets the URL for launching
  CString GetUrl();
  void SetUrl(CString sUrl);


  /// Gets the Base Url
  CString GetUrlBase();

  /// Returns true if the request is empty
  bool IsEmpty();

private:

  /// Returns true if there are parameters
  bool ContainsParameters();

private:

 
  /// Resource
  CString m_sResource;

  /// Port number
  unsigned int m_iPort;

  /// Application code
  CString m_sApplication;

  /// Session description
  CString m_sDescription;

  /// Server
  CString m_sServer;

  /// UID's
  CString m_sUids;

  /// User name
  CString m_sUserName;

  /// Users password
  CString m_sPassword;

  /// Context Id
  CString m_sContext;

  /// New Session
  CString m_sNewSession;


};  // class LaunchRequest