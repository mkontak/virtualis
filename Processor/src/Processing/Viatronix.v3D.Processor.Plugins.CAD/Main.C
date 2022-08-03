// $Id: Main.C,v 1.27.2.3 2009/12/18 20:55:53 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// includes
#include "stdafx.h"
#include "ProcessorFactory.h"
#include "ProcessorEnvironment.h"
#include "FindingCollection.h"
#include "ConsoleBuffer.h"
//#include "LicenseValidator.h"
#include "ColonEnvironmentArgs.h"
#include "Conversion.h"
#include "CommandlineDlg.h"



// import the cad tlb
#import "vxComCad.tlb" 
using namespace vxComCAD;

#define FILE_REVISION "$Revision: 1.27.2.3 $"

// atl module
class MyAtlModule : public CAtlModuleT< MyAtlModule > {};

/// Messaging manager
std::shared_ptr<messaging::MessagingManager> g_pManager;

/// Message to be sent
std::shared_ptr<messaging::ProcessingMessage> g_pMessage;


/**
 * Initializes the messaging used to communicte state back to the WORK QUEUE
 *
 * @param  cmdLine      Command line arguements
 *
 */
void InializeMessaging(CmdLineSwitches & cmdline)
{
  // =======================================
  // Attempt to set up the messaging manager
  // =======================================
  try
  {

    g_pManager = messaging::MessagingManager::Create( cmdline.ToString("-messageHost"), cmdline.ToInt("-messagePort") );

    LogFyi(util::Strings::Format("Messaging manager created, messages will be sent to (%s,%d)", g_pManager->GetServer().c_str(), g_pManager->GetPort() ), "CadServer", "InializeMessaging" );

    // =============================================
    // Create the message to be sent to work queue
    // =============================================
    try
    {
      g_pMessage = messaging::ProcessingMessage::Create(cmdline.ToString("-job"), "cad" );
    }
    catch ( ex::VException )
    {
      g_pMessage = __nullptr;
    }


  }
  catch ( ex::VException )
  {
    LogFyi("Messaging manager could not be created, no host and/or port specified", "CadServer", "InializeMessaging" );
    g_pManager = __nullptr;
  }


} // InitailizeMessaging()

/**
 * Sends State Information to message handler
 *
 * @param sStage        Stage of the process
 * @param iProgress     Progress (Overall)
 */
 void SendState( const int4 iProgress , const std::string & sStage )
 {

   if ( g_pManager != __nullptr && g_pMessage != __nullptr )
   {
      LogDbg(util::Strings::Format("Sending state to message handler %s-%d",sStage.c_str(), iProgress),"CadServer", "SendState");
      g_pMessage->SetStage(sStage);
      g_pMessage->SetProgress((float)iProgress);

      g_pManager->Send(g_pMessage);
   }

 }  // SendState()

/**
 * main procedure 
 *
 * @param   argc    number of command line arguments
 * @param   argv    arguments
 * @return  0 for success
 */
int _tmain( int argc, _TCHAR* argv[] )
{
  bool bSuccess( true );
  try
  {
    // atl requires a module to be define to work
    MyAtlModule _AtlModule;

    // are we registering or unregistering the server?
    if( argc == 2 )
    {
      if( !lstrcmpi( argv[ 1 ], "-regserver" ) || !lstrcmpi( argv[ 1 ], "/regserver" ))
      {
        if( SUCCEEDED( _AtlModule.RegisterServer( TRUE ) ) )
          std::cout << "server registered successfully!" << std::endl;
        }

      else if( !lstrcmpi( argv[ 1 ], "-unregserver" ) || !lstrcmpi( argv[ 1 ], "/unregserver" ))
      {
        if( SUCCEEDED( _AtlModule.UnregisterServer( TRUE ) ) )
          std::cout << "server unregistered successfully!" << std::endl;
      }

      // we are done we can exit
      return 0;
    } // if( argc == 2 ) registering server
    

    // start a timer
    Timer timer;

    // display banner
    ConsoleBuffer::WriteLine( "--------------------------------------" );
    ConsoleBuffer::WriteLine( "Viatronix CAD Processor (c) 2005-2013 " );
    ConsoleBuffer::WriteLine( "--------------------------------------" );
    ConsoleBuffer::Newline();

    // parse the command line arguments
    CmdLineSwitches cmdline
    (
      "-sourceFilePath",    "c:\\",       "root filename of dataset",
      "-appid",             "unknown",    "patient first name (string)",
      "-dicomonly",         "0",          "used dicom only", 
      "-messagePort",       "8021",       "message port",
      "-messageHost",       "localhost",  "message host",
      "-dicomFilePath",     "c:\\",       "Dicom File Path",
      "-destFilePath",      "c:\\",       "Dest Path",
      "-volumeFilePath",    "c:\\",       "Volume File Path",
			"-logLevel",          "4",          "Log level",
      "-job",               "",           "Job Id",
      "$"
    );

    ColonEnvironmentArgs colonArgs;
    
    if ( argc == 1 )
    {
      CommandlineDlg dlg;
      if ( dlg.DoModal() ==  IDOK )
      {
        colonArgs = dlg.GetColonEventArgs();
        //dlg.DestroyWindow();
      }
      else
        return 0;
    }
    else
    {
      cmdline.ReadCommandLine( argc, argv );
      colonArgs.SetArguments( cmdline );
    }

    /// Initializes the messaging from the command lin
    InializeMessaging(cmdline);

    colonArgs.PrintArguments();
   
    // initalize the base xml library
    vxDOMDocument::Initialize();
      
    // initialize the logger
		vxLogFactory::CreateLogger( "v3D CAD", colonArgs.GetLogFilename(), vxLogRecord::SeverityEnum::SL_ERROR, vxLogMessageFormats::FORMAT_DEFAULT, true );

    // initialize the com runtime
    CoInitialize( NULL );
    
    //error code tracking
    HRESULT hr( S_OK );

    /**
     * verify the license
     */
    //LogFyi( "Verifying CAD processor license ...", "", "Main" );
    //ConsoleBuffer::WriteStepBegin( "Verifying processor license... " );
    //LicenseValidator licenseValidator;
    //if( !licenseValidator.Request( LicenseRequestInfo( colonArgs.GetPluginAppId().c_str(), "-1", "na" ) ) )
    //  throw ex::VException( LogRec( "License request failed. " + licenseValidator.GetLastErrorMessage(), "", "Main" ) );
    //ConsoleBuffer::WriteStepEnd(  true );

    /**
     * create an instance of the cad processor
     */
    IProcessorPtr  processor;
    if( FAILED( hr = ProcessorFactory::Create( reinterpret_cast< void ** >( &processor ) ) ) )
      throw _com_error( hr );

		/**
     * initialize the processor's environment
     */
    SendState(0, "Initializing");
    LogFyi( "Initializing processor ...", "", "Main" );
    ConsoleBuffer::WriteStepBegin( "Initializing processor ... " );
		LogFyi( "Source file path  =  " + colonArgs.GetPreProcessDirectory(), "Main.C", "main");
		LogFyi( "Dicom file path   =  " + colonArgs.GetDicomDirectory(), "Main.C", "main");
		LogFyi( "Volume file path  =  " + colonArgs.GetVolumeFilePath(), "Main.C", "main");
		LogFyi( "CAD file path     =  " + colonArgs.GetDestinationFilePath(), "Main.C", "main");
		LogFyi( "Log level         =  " + ToAscii( colonArgs.GetLogLevel() ), "Main.C", "main");
    GetCoClassPtr< ProcessorEnvironment >( processor->Environment )->Initialize( colonArgs.GetPreProcessDirectory(), colonArgs.GetDicomDirectory(), colonArgs.GetVolumeFilePath(), colonArgs.GetDestinationFilePath(), colonArgs.IsDicomOnly() );
    ConsoleBuffer::WriteStepEnd(  true );


    SendState(10, "Initializing");


    /**
     * create an instance of the plugin ( out of process )
     */
    SendState(10, "Loading Plugin");
    LogFyi( "Creating Plugin ... [ID=" + colonArgs.GetPluginAppId() + "]", "", "Main" );
    ConsoleBuffer::WriteStepBegin( "Creating plugin ... " );
 
    IProcessorPluginPtr plugin;
    if( FAILED( hr = plugin.CreateInstance( colonArgs.GetPluginAppId().c_str(), NULL, CLSCTX_LOCAL_SERVER) ) )
      throw _com_error( hr );
  
    ConsoleBuffer::WriteStepEnd( true );
    SendState(25, "Loading Plugin");

    /**
     * run the plugin
     */
    SendState(25, "Running Plugin");
    LogFyi( "Runnning Plugin ...", "", "Main" );
    ConsoleBuffer::WriteStepBegin( "Running plugin ... " );
    if( FAILED( hr = plugin->Run( processor ) ) )
		{
      LogErr( "Runnning Plugin FAIL!    <<<  ", "", "Main" );
			throw _com_error( hr );
		}
    ConsoleBuffer::WriteStepEnd( true );
    SendState(75, "Running Plugin");
  
    /**
     * save the findings
     */
    SendState(75, "Saving Findings");
    LogFyi( "Saving Findings ...", "", "Main" );
    ConsoleBuffer::WriteStepBegin( "Saving Findings ... " );
    GetCoClassPtr< FindingCollection >( processor->Findings )->ToFile( _bstr_t( colonArgs.GetOutputFilename().c_str() ) );
    ConsoleBuffer::WriteStepEnd( true );
    ConsoleBuffer::Newline();
    SendState(100, "Saving Findings");
    /**
     * processing complete
     */
    std::string sElapsedTime( timer.AsString2().c_str() );
    LogFyi( "Processing took " + sElapsedTime, "", "Main" );
    ConsoleBuffer::WriteLine(  "Processing complete without errors" );
    ConsoleBuffer::Newline();
    ConsoleBuffer::WriteArgument( "  time:      ", sElapsedTime.c_str() );
    ConsoleBuffer::WriteArgument( "  findings:  ", ToAscii( processor->Findings->Count ).c_str() );
    ConsoleBuffer::Newline( 2 );
  } // try
  catch( _com_error & e )
  {
    LogErr( "COM exception : " + std::string(e.ErrorMessage()), "", "Main" );

    ConsoleBuffer::WriteStepEnd( false );
    ConsoleBuffer::Newline( 2 );
    ConsoleBuffer::WriteComException( e );
    ConsoleBuffer::Newline();
    bSuccess = false;
  } // catch( _com_error & e )
  catch( ex::VException & e )
  {
    LogErr( "Viatronix exception : " + e.GetLogRecord().GetTextMessage(), "", "Main" );

    ConsoleBuffer::WriteStepEnd( false );
    ConsoleBuffer::Newline( 2 );
    ConsoleBuffer::WriteViatronixException( e );
    ConsoleBuffer::Newline();
    bSuccess = false;
  } // catch( ex::VException & e )
  catch( ... )
  {
    LogErr( "Unhandled exception", "", "Main" );

    ConsoleBuffer::WriteStepEnd( false );
    ConsoleBuffer::Newline( 2 );
    ConsoleBuffer::WriteUnhandledException();
    ConsoleBuffer::Newline();
    bSuccess = false;
  } // catch( ... )

  // uninitialize the com runtime
  CoUninitialize();

  // free xml resources
  vxDOMDocument::Terminate();
  
  return bSuccess? 0 : 1; // 
} // _tmain( int argc, _TCHAR* argv[] )


// $Log: Main.C,v $
// Revision 1.27.2.3  2009/12/18 20:55:53  dongqing
// add log info output for file paths
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.27.2.2  2009/10/13 16:43:26  dongqing
// add log info for file paths
//
// Revision 1.27.2.1  2009/07/16 19:05:34  dongqing
// add cad destination path into the processorEnvironment
//
// Revision 1.27  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.26  2007/01/16 20:00:56  dongqing
// turn licence back on
//
// Revision 1.25  2006/12/18 21:55:34  romy
// added Commandline GUI
//
// Revision 1.24  2006/09/19 20:34:38  dongqing
// disable the license
//
// Revision 1.23  2006/08/16 20:08:09  geconomos
// updated banner with new copyright
//
// Revision 1.22  2006/06/26 21:11:58  dongqing
// change the input volume voxel type from SHORT to UINT2
//
// Revision 1.21  2006/05/05 14:30:03  mkontak
// Fixes for starting of cad and removal of unused parameters
//
// Revision 1.20  2006/05/03 15:56:02  romy
// destFile Path added
//
// Revision 1.19  2006/04/27 19:24:40  mkontak
// Fix the messaging used to inform the WQ of the progress
//
// Revision 1.18  2006/04/25 13:11:14  mkontak
// Added ERROR logging to log file
//
// Revision 1.17  2006/04/18 19:49:07  geconomos
// reenabled license check
//
// Revision 1.16  2006/02/09 21:19:46  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.15  2006/02/09 19:29:57  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.14  2006/01/31 19:13:58  romy
// added DICOM FIle Path commandline arg
//
// Revision 1.13  2006/01/31 15:50:22  geconomos
// added -dicomFilePath support
//
// Revision 1.12  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.11  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.10  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.9  2005/11/08 14:42:17  geconomos
// corrected error related to "-dicomonly" flag
//
// Revision 1.8.2.7  2006/01/16 15:43:38  geconomos
// added licensing per application id
//
// Revision 1.8.2.6  2005/12/20 13:50:51  geconomos
// Added detailed logging for each stage as well as time taken to complete
//
// Revision 1.8.2.5  2005/12/19 21:40:37  dongqing
// add vendor specific parameter in the output
//
// Revision 1.8.2.4  2005/12/19 15:51:41  geconomos
// added a method to convert a unc share to a local path (assuming that the computer share name is that of the localhost)
//
// Revision 1.8.2.3  2005/11/18 16:23:53  geconomos
// Issue# 4551: Not reporting progress to console
//
// Revision 1.8.2.2  2005/11/17 13:27:55  geconomos
// Issue# 5431 - Not returning errors back properly to the work queue handler
//
// Revision 1.8.2.1  2005/11/10 15:48:06  geconomos
// issue# 4531: fixed return code when plugin returns an error
//
// Revision 1.8  2005/10/21 13:42:31  geconomos
// added ability to register and unregister the server from the command line
//
// Revision 1.7  2005/10/03 18:28:08  geconomos
// returning 0 for success from main; otherwise failure
//
// Revision 1.6  2005/09/30 19:54:18  geconomos
// added -dicomonly command line argument
//
// Revision 1.5  2005/09/29 13:20:44  geconomos
// updated dmcdir to be based off dsroot
//
// Revision 1.4  2005/09/23 14:33:40  geconomos
// improved error handing
//
// Revision 1.3  2005/09/23 12:09:42  geconomos
// clean up
//
// Revision 1.2  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:42:09  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Main.C,v 1.27.2.3 2009/12/18 20:55:53 dongqing Exp $
// $Id: Main.C,v 1.27.2.3 2009/12/18 20:55:53 dongqing Exp $
