// $Id: CommandLineInfoEx.h,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
//
// Copyright © 2005-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net

#pragma once

class CommandLineInfoEx : public CCommandLineInfo
{

// member functions
public:
  
  /// constructor
  CommandLineInfoEx();
  
  /// destructor
  virtual ~CommandLineInfoEx();

  /// framework calls this function to parse/interpret individual parameters from the command line
  virtual void ParseParam( const char* pszParam, BOOL bFlag, BOOL bLast );

  /// gets the start mode flag
  const bool & GetLaunchedFromConsole() const { return m_bLaunchedFromConsole; }

  /// gets the start mode flag
  const bool & GetReportView() const { return m_bReportView; }

  /// gets the vrx filename
  const std::string & GetVrxFilename() const { return m_sReportVrxFilename; }

  /// gets the pdf filename
  const std::string & GetPdfFilename() const { return m_sReportPdfFilename; }

  /// gets the output filename
  const std::string & GetOutputDirectory() const { return m_sOutputDirectory; }

  /// gets the uids
  const std::string & GetUIDs() const { return m_sUID; }

  /// gets the login
  const std::string & GetLogin() const { return m_sLogin; }

  /// gets if the regserver flag has been specified
  const bool & GetRegServer() const { return m_bRegServer; }

  /// gets if the unregserver flag has been specified
  const bool & GetUnregServer() const { return m_bUnregServer; }

  /// gets if the license flag has been specified
  const bool & GetLicenseApplication() const { return m_bLicenseApplication; }

  /// gets the send to pacs options
  const uint4 GetSendToPacs() const { return m_iSendToPacs; }

  /// Gets the create new session flag
  const bool CreateNewSession() const { return m_bCreateNewSession; }

private:

  // specifies the available command line flags
  enum CommandLineFlags
  {
    ReportVrxFilename,
    ReportPdfFilename,
    OutputDirectory,
    UniqueID,
    User,
    Pacs,
  };

  /// last command line flag processed
  CommandLineFlags m_eLastFlag;

  /// indicates if the viewer was launched from the console
  bool m_bLaunchedFromConsole;

  /// indicates if the viewer was launched from the console
  bool m_bReportView;

  /// name of vrx file
  std::string m_sReportVrxFilename;

  /// name of pdf file
  std::string m_sReportPdfFilename;

  /// name of output directory
  std::string m_sOutputDirectory;

  /// uids
  std::string m_sUID;

  /// login/password
  std::string m_sLogin;

  /// indicates that the com server should be registered
  bool m_bRegServer;

  /// indicates that the com server should be unregistered
  bool m_bUnregServer;

  /// indicates that the license should be generated
  bool m_bLicenseApplication;

  /// send to pacs settings
  uint4 m_iSendToPacs;

  /// ================================================================================
  /// Flag used to indicate that a new session should be created instead of using the 
  /// method of finding the last session created for the series listed in the UID.
  /// ================================================================================
  bool m_bCreateNewSession;

}; // class CommandLineInfoEx 


// $Log: CommandLineInfoEx.h,v $
// Revision 1.5.2.1  2009/04/03 14:21:57  mkontak
// Fix so that the Console tab is not enabled when launched stand-alone or from a
// launcher
//
// Revision 1.5  2006/11/11 01:32:18  gdavidson
// Added command line arguments that launch the viewer from the enterprise client
//
// Revision 1.4  2006/09/29 21:02:08  frank
// added a thin-client mode
//
// Revision 1.3  2005/12/20 20:33:29  geconomos
// added report related arguments
//
// Revision 1.2  2005/11/18 00:05:59  geconomos
// added flags for regserver and unregserver
//
// Revision 1.1  2005/11/07 20:36:40  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/CommandLineInfoEx.h,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
// $Id: CommandLineInfoEx.h,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
