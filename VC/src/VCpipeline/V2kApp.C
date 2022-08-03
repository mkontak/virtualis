// $Id: V2kApp.C,v 1.16 2006/08/31 20:57:57 dongqing Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Soeren Grimm soeren@viatronix.net
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "V2kApp.h"
#include "vxPipeline.h"
#include "File.h"
#include "v2kmonitorclient.h"
#include "SystemInfo.h"
#ifdef _WIN32
#include <Winbase.h>
#include <Windows.h>
#endif

#include <string>
#include <atlbase.h>
#include <atlcomcli.h>  // for License codes

using namespace std;

#define FILE_REVISION "$Revision: 1.16 $"

/**
 * Constructor. Creates a v2k application object.
 */
V2kApp::V2kApp(int iArgc, char * psArgv[]) :
/*****************************************************************************************************************/
m_cmdLine("-stage" , "all","all allnoec",
          "-icon"  , "0",  "compute volume preview icons",
					"-dsroot", "", "Old format source file path",
          "-sourceFilePath", "", "Source file path",
          "-destFilePath", "",  "Destination file path",
          "-eTime", "0.0","elapsed time for Pipeline (due to Dicom stage I/O)",
          "-logFilePath", "", "Log file path",
          "-logLevel", "2" ,"sets the loglevel",
          "-messagePort", "8021", "message port",
          "-messageHost", "localhost", "message host",
          "-patientFirstName", "unknown", "patient first name (string)",
          "-patientLastName", "unknown", "patient first name (string)",
          "-datasetType", "0", "Dataset type (it is an enum)",
          "-studyType",   "0", "Study Type",
          "-procId", "1",  "process number: main-0 pipeline A-1 pipeline B-2",
          "-eTime", "0.0","elapsed time for Pipeline (due to Dicom stage I/O)",
          "-progName", "vcpipeline.exe", "used intern",
          "-isOnCustomerSite","0", "true-false",
          "-pf", "c:/V-System/vxProcessor/VC/vcpipeline.pf","additional parameter file",
          "$"), m_iArgc(iArgc),m_psArgv(psArgv)
{ 

  m_cmdLine.ReadCommandLine(iArgc,psArgv);
  m_cmdLine.Hide("-eTime");  
  m_cmdLine.Hide("-logLevel");
  m_cmdLine.Hide("-logFilePath");
  m_cmdLine.Hide("-messagePort");
  m_cmdLine.Hide("-messageHost");

  std::string sLogFilePath(m_cmdLine.ToString("-logFilePath"));

  if( sLogFilePath.size() == 0 )
  {
		std::string s( m_cmdLine.ToString("-destFilePath") );
		if( s.size() > 0 )
      sLogFilePath = s + "_VCPipeline.log";
		else
		{
			s = m_cmdLine.ToString("-dsroot");
			if( s.size() > 0 )
        sLogFilePath = s + "_VCPipeline.log";
		}		
  } // if
  
  vxLogFactory::CreateLogger("vc processor", sLogFilePath, (vxLogRecord::SeverityEnum) m_cmdLine.ToInt("-logLevel"), true);
  //vxLogger::SetDefaultHandlers(sTmp,(LogRecord::SeverityEnum) m_cmdLine.ToInt("-logLevel"),m_cmdLine.ToBool("-isOnCustomerSite"));  
  //vxLogger::CreateConsoleHandler();

  stringstream tt;
  tt << "Starting Pipeline: [";

  for ( int i(0); i < iArgc; tt << " " << psArgv[i++] );

  tt << " ]";

  LogFyi(tt.str(), "V2kApp","constructor");

} // Constructor


/** 
 * Run the v2k application.
 */
int4 V2kApp::Run()
{
  bool bIsUpdate(true);
  
  try 
  {        
    V2kMonitorClient::ConnectToHost(m_cmdLine.ToInt("-messagePort"), m_cmdLine.ToString("-messageHost")); 
  }
  catch(ex::VException &)
  {
    LogWrn("Could not establish connection to port 8021 -> can not update progress monitor.", "V2kApp", "Run");
    bIsUpdate = false;
  }
  if(bIsUpdate)
  {
    LogFyi("Could establish connection to port 8021 -> Can update progress monitor.", "V2kApp", "Run");
  }
  
  vxPipeline pipeline(m_cmdLine); // Create with pointer to success - set in pipeline
  pipeline.Start();  
  pipeline.Wait(); // Wait for pipeline thread. 

	int4 i = pipeline.GetErrorCode();
  return (i);
} // Run

#undef FILE_REVISION

// $Log: V2kApp.C,v $
// Revision 1.16  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.15  2006/08/29 18:38:29  gchin
// Changes from Dongqing for Dr. Cadi
//
// Revision 1.14  2006/06/22 20:43:08  dongqing
// decomment
//
// Revision 1.13  2006/06/15 13:33:21  dongqing
// temporily disable license check
//
// Revision 1.12  2006/06/09 17:27:29  dongqing
// comment
//
// Revision 1.11  2006/06/09 17:26:48  dongqing
// decomments
//
// Revision 1.10  2006/06/09 17:25:51  dongqing
// Old console has -sourceFilePath. Fix path check for old console.
//
// Revision 1.9  2006/06/09 13:14:47  dongqing
// make the processor compatible with old Console
//
// Revision 1.8  2006/04/21 20:56:24  dongqing
// comments
//
// Revision 1.7  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.6  2006/02/06 21:24:03  mkontak
// Changed the exit code to -999
//
// Revision 1.5  2006/02/06 20:55:39  mkontak
// Fixed exit code for failed license (-100 now)
//
// Revision 1.4  2005/11/15 13:12:19  mkontak
// Fix to work with the new WQ and Console 3.0
//
// Revision 1.3  2005/08/05 13:08:49  geconomos
// fixed linker error for debug builds
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.5.6.4  2005/02/15 17:41:01  jmeade
// merge from vc1.4 branch.
//
// Revision 3.5.6.3.2.2  2005/01/13 17:19:15  dongqing
// Release license client at once after get license
//
// Revision 3.5.6.3.2.1  2005/01/12 17:14:32  dongqing
// Using new licensing mechanism
//
// Revision 3.5.6.3  2004/09/10 15:38:20  romy
// App is now looking for ColonPipeline License
//
// Revision 3.5.6.2  2003/12/22 21:26:37  dongqing
// add license checking
//
// Revision 3.5.6.1  2003/04/25 18:59:31  mkontak
// New arguements
//
// Revision 3.5  2002/08/05 20:42:32  dongqing
// rename the Logger to vxLogger due to the re-name of Logger Class
//
// Revision 3.4  2002/08/01 21:27:57  dongqing
// Add commend line option for bypassing electronic cleansing
//
// Revision 3.3  2002/05/01 17:23:45  mkontak
// Added study type to broadcast message
//
// Revision 3.2  2002/02/28 18:12:49  geconomos
// Removal of QT.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:01:51  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:30   ingmar
// Initial revision.
// Revision 1.27  2001/09/13 17:42:47  soeren
// added quotes to function name in logger macro
//
// Revision 1.26  2001/09/12 21:31:43  soeren
// added more info messages
//
// Revision 1.25  2001/09/12 21:31:04  soeren
// added more info messages
//
// Revision 1.24  2001/09/12 21:14:32  soeren
// changed name of log file
//
// Revision 1.23  2001/09/12 20:27:57  soeren
// changed dbg message to inf message
//
// Revision 1.22  2001/09/10 22:31:27  soeren
// changed Logger::SetDefault parameter
//
// Revision 1.21  2001/09/10 09:05:01  ingmar
// fixed use of logger defines
//
// Revision 1.20  2001/09/10 07:54:03  ingmar
// logger console on by default
//
// Revision 1.19  2001/09/08 23:36:04  ingmar
// code beautify
//
// Revision 1.18  2001/08/31 18:54:24  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.17  2001/08/24 18:58:31  soeren
// adde new messages to WQHandler
//
// Revision 1.16  2001/08/13 20:27:41  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.15  2001/08/13 16:19:40  soeren
// beautified messages
//
// Revision 1.14  2001/08/10 19:59:07  soeren
// beautified messages in pipeline.c
//
// Revision 1.13  2001/08/10 15:22:50  soeren
// changed logger settings
//
// Revision 1.12  2001/08/09 21:59:29  soeren
// fixed information logger problem
//
// Revision 1.11  2001/08/09 20:56:03  lihong
// enabled console handler
//
// Revision 1.10  2001/08/09 13:29:35  soeren
// clean up logger and constructor
//
// Revision 1.9  2001/08/09 13:01:55  soeren
// Optimized  creating of log files in the pipeline
//
// Revision 1.8  2001/08/07 17:05:28  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.7  2001/08/01 23:02:54  soeren
// changed default of parameter machinename
//
// Revision 1.6  2001/08/01 19:48:38  soeren
// enabled ConsoaleFormatter again
//
// Revision 1.5  2001/07/31 19:20:45  soeren
// renamed parameter
//
// Revision 1.4  2001/07/31 19:32:28  soeren
// changed parameter -proc# to -procId
//
// Revision 1.3  2001/07/27 18:41:30  soeren
// removed unnecessary switches
//
// Revision 1.2  2001/07/26 17:11:03  soeren
// fixed compiler errors
//
// Revision 1.1  2001/07/26 16:29:59  soeren
// new main for vcpipeline
//
// Revision 1.59  2001/07/25 04:24:51  sachin
// All errors due to New Exception class fixed
//
// Revision 1.58  2001/06/07 19:13:40  soeren
// added better messages for the database error
//
// Revision 1.57  2001/06/07 19:12:40  soeren
// added better messages for the database error
//
// Revision 1.56  2001/06/07 18:46:30  soeren
// changed some messages
//
// Revision 1.55  2001/06/06 23:49:20  dave
// changed dicom temp path name in cmdLine
//
// Revision 1.54  2001/06/06 15:29:57  soeren
// added new switch -tmpDicomFilesLoc
//
// Revision 1.53  2001/06/06 15:16:27  soeren
// added new switch -tmpDicomFilesLoc
//
// Revision 1.52  2001/06/05 22:50:11  soeren
// added support for 127.0.0.1 and localhost
//
// Revision 1.51  2001/06/05 22:48:18  soeren
// added support for 127.0.0.1 and localhost
//
// Revision 1.50  2001/06/05 22:44:43  soeren
// added support for 127.0.0.1 and localhost
//
// Revision 1.49  2001/06/05 17:12:57  soeren
// changed database behavior
//
// Revision 1.48  2001/06/05 16:26:51  dmitry
// Updated database location.
//
// Revision 1.47  2001/05/29 04:10:01  dave
// added better message for the share error
//
// Revision 1.46  2001/05/28 22:33:43  soeren
// changed localhost to 127.0.0.1
//
// Revision 1.45  2001/05/28 21:55:38  soeren
// changed localhost to 127.0.0.1
//
// Revision 1.44  2001/05/28 21:50:25  soeren
// changed localhost to 127.0.0.1
//
// Revision 1.43  2001/05/28 20:18:18  dave
// moving to UNC paths, new database
//
// Revision 1.42  2001/05/28 07:12:53  soeren
// changed again
//
// Revision 1.41  2001/05/28 07:10:57  soeren
// changed some defaults
//
// Revision 1.40  2001/05/23 23:37:14  soeren
// changed exception message
//
// Revision 1.39  2001/05/23 22:58:58  soeren
// changed exception message
//
// Revision 1.38  2001/05/23 18:02:45  soeren
// Implementaion of database
//
// Revision 1.37  2001/05/23 15:12:30  dave
// dicom
//
// Revision 1.36  2001/05/23 00:57:06  dave
// moving to new database classes
//
// Revision 1.35  2001/05/20 20:44:57  soeren
// More Coding standard complient
//
// Revision 1.34  2001/05/20 20:41:27  soeren
// More Coding standard complient
//
// Revision 1.33  2001/05/20 20:26:30  soeren
// More Coding standard complient
//
// Revision 1.32  2001/05/20 20:23:47  soeren
// More Coding standard complient
//
// Revision 1.31  2001/05/09 19:07:16  dave
// changed dicom init
//
// Revision 1.30  2001/05/09 19:02:11  soeren
// change init function of v2kFrontend to ConnectToHost
//
// Revision 1.29  2001/05/09 17:02:32  soeren
// change portnumber offset to real portnumber
//
// Revision 1.28  2001/05/07 22:34:05  soeren
// rewrote the whole SystemInfo class
//
// Revision 1.27  2001/05/07 18:03:04  soeren
// added system and memory informations
//
// Revision 1.26  2001/05/07 18:02:43  soeren
// added system and memory informations
//
// Revision 1.25  2001/05/03 20:50:30  dave
// updated to use vxBase dicom
//
// Revision 1.24  2001/05/02 19:15:46  dave
// converted to new stream classes
//
// Revision 1.23  2001/04/29 21:52:58  soeren
// uups was necessary ;-)
//
// Revision 1.21  2001/04/29 21:36:10  soeren
// add guiOutput parameter
//
// Revision 1.20  2001/04/29 20:18:07  soeren
// added feeGui
//
// Revision 1.19  2001/04/26 02:05:45  soeren
// changed parameter of tab widget
//
// Revision 1.18  2001/04/25 21:29:54  soeren
// activated hide functionality of CmdLine class
//
// Revision 1.17  2001/04/25 14:30:52  soeren
// fixed path bug
//
// Revision 1.16  2001/04/24 19:34:13  soeren
// added restart functionality to gui
//
// Revision 1.15  2001/04/23 21:24:29  soeren
// added some informations
//
// Revision 1.14  2001/04/23 21:20:25  soeren
// added some informations
//
// Revision 1.13  2001/04/19 01:39:14  soeren
// pipeline now a thread !!!
//
// Revision 1.12  2001/04/19 01:32:10  soeren
// beautified
//
// Revision 1.11  2001/04/19 01:30:40  soeren
// beautified
//
// Revision 1.10  2001/04/19 01:06:54  soeren
// changed Dicom constructor
//
// Revision 1.9  2001/04/18 21:22:28  soeren
// removed switch
//
// Revision 1.8  2001/04/18 19:51:25  soeren
// new V2kFrontEnd (alpha Version)
//
// Revision 1.7  2001/04/18 14:23:23  soeren
// fixed readcdmline bug
//
// Revision 1.6  2001/04/17 22:53:18  soeren
// removed messages
//
// Revision 1.5  2001/04/17 22:47:27  soeren
// changed messages
//
// Revision 1.4  2001/04/17 20:23:45  soeren
// no message
//
// Revision 1.3  2001/04/17 19:54:39  soeren
// new main
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/V2kApp.C,v 1.16 2006/08/31 20:57:57 dongqing Exp $
// $Id: V2kApp.C,v 1.16 2006/08/31 20:57:57 dongqing Exp $
