// $Id: ColonEnvironmentArgs.C,v 1.1.2.2 2010/01/12 16:31:13 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File: Co
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

#include "StdAfx.h"
#include "colonenvironmentargs.h"

#define FILE_REVISION "$Revision: 1.1.2.2 $"

/*
 * Constructor
 */
ColonEnvironmentArgs::ColonEnvironmentArgs(void)
{
}//ColonEnvironmentArgs(void)


/*
 * Destructor
 */
ColonEnvironmentArgs::~ColonEnvironmentArgs(void)
{
}//~ColonEnvironmentArgs(void)


/*
 * Copy Constructor
 */
ColonEnvironmentArgs::ColonEnvironmentArgs( const ColonEnvironmentArgs & other  ):
  m_sPreProcessDirectory( other.m_sPreProcessDirectory ),
  m_sDestFilePath( other.m_sDestFilePath ),
  m_sPluginAppId( other.m_sPluginAppId ),
  m_sDicomDirectory( other.m_sDicomDirectory ),
  m_sVolumeFilePath( other.m_sVolumeFilePath ),
  m_bDicomOnly( other.m_bDicomOnly ),
  m_sLogFilename( other.m_sLogFilename ),
  m_sOutputFilename ( other.m_sOutputFilename ),
	m_iLogLevel( other.m_iLogLevel )
{
  // If they are missing
    if ( m_sLogFilename == "" )
      m_sLogFilename = ( m_sDestFilePath + ".cad.log" );
    
    if ( m_sOutputFilename == "" )
      m_sOutputFilename = ( m_sDestFilePath + ".cad.xml" );

}//ColonEnvironmentArgs( const ColonEnvironmentArgs & other  )


/*
* Asignement operator
*/
const ColonEnvironmentArgs & ColonEnvironmentArgs::operator= (const ColonEnvironmentArgs  & rhs )
{
  if ( this != &rhs )
  {
    m_sPreProcessDirectory = rhs.m_sPreProcessDirectory ;
    m_sDestFilePath = rhs.m_sDestFilePath ;
    m_sPluginAppId = rhs.m_sPluginAppId ;
    m_sDicomDirectory = rhs.m_sDicomDirectory ;
    m_sVolumeFilePath = rhs.m_sVolumeFilePath;
    m_bDicomOnly = rhs.m_bDicomOnly ;

    m_sLogFilename = rhs.m_sLogFilename ;
    m_sOutputFilename = rhs.m_sOutputFilename ;

		m_iLogLevel = rhs.m_iLogLevel;

    // If they are missing
    if ( m_sLogFilename == "" )
      m_sLogFilename = ( m_sDestFilePath + ".cad.log" );
    
    if ( m_sOutputFilename == "" )
      m_sOutputFilename = ( m_sDestFilePath + ".cad.xml" );
  }
  return * this;
}//operator= (const ColonEnvironmentArgs  & rhs )


/*
 * SetArguments(  CmdLineSwitches cmdline )
 */
void ColonEnvironmentArgs::SetArguments(  CmdLineSwitches cmdline )
{
  m_sPluginAppId  =  cmdline.ToString( "-appid" )  ;
  m_sPreProcessDirectory =  Conversion::UNC2Local( cmdline.ToString( "-sourceFilePath" )  );
  m_sDicomDirectory = Conversion::UNC2Local( cmdline.ToString( "-dicomFilePath" ) ) ;
  m_sVolumeFilePath = Conversion::UNC2Local(cmdline.ToString( "-volumeFilePath" ));
  m_sDestFilePath =  Conversion::UNC2Local(cmdline.ToString( "-destFilePath" ) );

  m_bDicomOnly = true; //cmdline.ToBool( "-dicomonly" );

	m_iLogLevel = cmdline.ToInt( "-logLevel" );

  // Since the destintaion file path contains the root we only need to add the extension
  m_sLogFilename = ( m_sDestFilePath + ".cad.log" );
  // Since the destination file path conatins the root we only need to add the extension
  m_sOutputFilename = ( m_sDestFilePath + ".cad.xml" );
}//SetArguments(  CmdLineSwitches cmdline )


/*
 * Prints the arguments list
 */
void ColonEnvironmentArgs::PrintArguments()
{
  ConsoleBuffer::WriteLine ( "Running with the following arguments:" );
  ConsoleBuffer::Newline();
  ConsoleBuffer::WriteArgument( "  sourceFilePath:  ", m_sPreProcessDirectory.c_str() );
  ConsoleBuffer::WriteArgument( "  appid:   ", m_sPluginAppId.c_str() );
  ConsoleBuffer::WriteArgument( "  dcmdir:  ", m_sDicomDirectory.c_str() );
  ConsoleBuffer::WriteArgument( "  dcmonly: ", m_bDicomOnly? "true"  : "false" );
  ConsoleBuffer::WriteArgument( "  logfile: ", m_sLogFilename.c_str() );
  ConsoleBuffer::WriteArgument( "  outfile: ", m_sOutputFilename.c_str() );
  ConsoleBuffer::WriteArgument( "  volumePath: ", m_sVolumeFilePath.c_str() );
  ConsoleBuffer::WriteArgument( "  destPath: ", m_sDestFilePath.c_str() );
	ConsoleBuffer::WriteArgument( "  logLevel: ", ToAscii( m_iLogLevel ).c_str() );
  ConsoleBuffer::Newline( 2 );
}//OutputArgs()


/*
 * Prints the arguments list to the log file
 */
void ColonEnvironmentArgs::PrintArgumentsToLogFile()
{
  LogErr( "Running with the following arguments:" , "", "");
  LogErr(  m_sPreProcessDirectory.c_str() , "", "" );
  LogErr(  m_sPluginAppId.c_str() , "", "");
  LogErr(  m_sDicomDirectory.c_str() , "", "");
  LogErr(  m_bDicomOnly? "true"  : "false", "", "" );
  LogErr(  m_sLogFilename.c_str() , "", "");
  LogErr(  m_sOutputFilename.c_str() , "", "");
  LogErr(  m_sVolumeFilePath.c_str() , "", "");
  LogErr(  m_sDestFilePath.c_str(), "", "");
	LogErr(  ToAscii( m_iLogLevel ).c_str(), "", "");
}//OutputArgs()



// $Log: ColonEnvironmentArgs.C,v $
// Revision 1.1.2.2  2010/01/12 16:31:13  dongqing
// comments out the temporary codes for volume file path
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.1  2009/12/18 20:56:33  dongqing
// Add temporary code to allow extracting volume path from sourceFilePath
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1  2007/03/01 20:40:11  geconomos
// changed extension from .cpp to .C
//
// Revision 1.4  2006/12/18 21:32:31  romy
// extended for Commandline GUI
//
// Revision 1.3  2006/05/03 16:04:56  romy
// destFile Path added
//
// Revision 1.2  2006/02/09 21:20:16  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.1  2006/02/09 19:27:54  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ColonEnvironmentArgs.C,v 1.1.2.2 2010/01/12 16:31:13 dongqing Exp $
// $Id: ColonEnvironmentArgs.C,v 1.1.2.2 2010/01/12 16:31:13 dongqing Exp $
