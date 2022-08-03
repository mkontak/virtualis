// $Id: CommandLineInfoEx.C,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
//
// Copyright © 2005-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net

// includes
#include "StdAfx.h"
#include "CommandLineInfoEx.h"

/**
 * Constructor
 */
CommandLineInfoEx::CommandLineInfoEx() : 
  m_bLaunchedFromConsole( false ),
  m_bReportView( false ),
  m_bRegServer( false ),
  m_bUnregServer( false ),
  m_bLicenseApplication( false ),
  m_bCreateNewSession( false ),
  m_iSendToPacs( 0 )
{
} // CommandLineInfoEx()


/**
 * Destructor
 */
CommandLineInfoEx::~CommandLineInfoEx()
{  
} // ~CommandLineInfoEx()


/**
 * Framework calls this function to parse/interpret individual parameters from the command line
 *
 * @param   sParam    parameter or flag
 * @param   bFlag     indicates whether pszParam is a parameter or a flag
 * @param   bLast     indicates if this is the last parameter or flag on the command line
 */
void CommandLineInfoEx::ParseParam( const char* sParam, BOOL bFlag, BOOL bLast )
{
  CCommandLineInfo::ParseParam( sParam, bFlag, bLast );

  if( bFlag )
  {
    if( !lstrcmpi( "console", sParam ) )
      m_bLaunchedFromConsole = true;

    else if( !lstrcmpi( "report", sParam ) )
      m_bReportView = true;

    else if( !lstrcmpi( "regserver", sParam ) )
      m_bRegServer = true;
    
    else if( !lstrcmpi( "unregserver", sParam ) )
      m_bUnregServer = true;

    else if( !lstrcmpi( "license", sParam ) )
      m_bLicenseApplication = true;

    else if( !lstrcmpi( "uid", sParam ) )
      m_eLastFlag = CommandLineInfoEx::UniqueID;

    else if( !lstrcmpi( "vrx", sParam ) )
      m_eLastFlag = CommandLineInfoEx::ReportVrxFilename;

    else if( !lstrcmpi( "pdf", sParam ) )
      m_eLastFlag = CommandLineInfoEx::ReportPdfFilename;

    else if( !lstrcmpi( "out", sParam ) )
      m_eLastFlag = CommandLineInfoEx::OutputDirectory;

    else if( !lstrcmpi( "user", sParam ) )
      m_eLastFlag = CommandLineInfoEx::User;

    else if( !lstrcmpi( "pacs", sParam ))
      m_eLastFlag = CommandLineInfoEx::Pacs;

    else if( !lstrcmpi( "newsession", sParam ) )
      m_bCreateNewSession = true;

  }
  else
  {
    if( m_eLastFlag == CommandLineInfoEx::ReportVrxFilename )
      m_sReportVrxFilename = sParam;

    else if( m_eLastFlag == CommandLineInfoEx::ReportPdfFilename )
      m_sReportPdfFilename = sParam;

    else if( m_eLastFlag == CommandLineInfoEx::UniqueID )
      m_sUID = sParam;

    else if( m_eLastFlag == CommandLineInfoEx::OutputDirectory )
      m_sOutputDirectory = sParam;

    else if( m_eLastFlag == CommandLineInfoEx::User )
      m_sLogin = sParam;

    else if( m_eLastFlag == CommandLineInfoEx::Pacs )
      m_iSendToPacs = std::stoi( sParam );
  }
} // ParseParam( const char* sParam, BOOL bFlag, BOOL bLast )


// $Log: CommandLineInfoEx.C,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/CommandLineInfoEx.C,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
// $Id: CommandLineInfoEx.C,v 1.5.2.1 2009/04/03 14:21:57 mkontak Exp $
