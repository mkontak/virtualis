// $Id: colonEnvironmentArgs.h,v 1.8 2006/12/18 21:32:31 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )


#pragma once

#include "ConsoleBuffer.h"
#include "conversion.h"

/*
 *Class mainitains the supported arguments for Colon Processor
 */
class ColonEnvironmentArgs
{
public:
  /// Constructor
  ColonEnvironmentArgs(void);

  ///Destructor
  ~ColonEnvironmentArgs(void);

  ///copy constructor
  ColonEnvironmentArgs( const ColonEnvironmentArgs  & args );

  ///assignement operator - needed here to calculate the output and cad file names
  const ColonEnvironmentArgs & operator= (const ColonEnvironmentArgs  & rhs );

  ///Constrcutor takes commandline args as parameter 
  ColonEnvironmentArgs ( CmdLineSwitches cmdline ):
    m_sPreProcessDirectory( Conversion::UNC2Local( cmdline.ToString( "-sourceFilePath" )  )),
    m_sDestFilePath( Conversion::UNC2Local(cmdline.ToString( "-destFilePath" ) )),
    m_sPluginAppId( cmdline.ToString( "-appid" )  ),
    m_sDicomDirectory( Conversion::UNC2Local( cmdline.ToString( "-dicomFilePath" ) ) ),
    m_sVolumeFilePath( Conversion::UNC2Local( cmdline.ToString( "-volumeFilePath" ) ) ),
    m_bDicomOnly( true ), //cmdline.ToBool( "-dicomonly" ) ),
		m_iLogLevel( cmdline.ToInt( "-logLevel" ) )
    {
      // Since the destintaion file path contains the root we only need to add the extension
      m_sLogFilename = ( m_sDestFilePath + ".cad.log" );

      // Since the destination file path conatins the root we only need to add the extension
      m_sOutputFilename = ( m_sDestFilePath + ".cad.xml" );
    }//ColonEnvironmentArgs ( CmdLineSwitches cmdline )

  
  ///Sets the arguments
  void SetArguments(  CmdLineSwitches cmdline );
  
  /// Gets or sets the PluginAppId
  inline const std::string & GetPluginAppId()  { return m_sPluginAppId; }
  inline void  SetPluginAppId( const std::string & appId )  { m_sPluginAppId = appId; }

  /// Gets or sets the DSRoot
  inline const std::string & GetPreProcessDirectory()  { return m_sPreProcessDirectory; }
  inline void  SetPreProcessDirectory( const std::string & dir )  { m_sPreProcessDirectory = dir ; }
     
  /// Gets the DicomDirectory
  inline const std::string & GetDicomDirectory()  { return m_sDicomDirectory; }
  inline void  SetDicomDirectory( const std::string & dir )  { m_sDicomDirectory = dir; }
  
  /// Gets or sets the LogFilename
  inline const std::string & GetLogFilename()  { return m_sLogFilename; }
  inline void  SetLogFilename( const std::string & logFileName )  { m_sLogFilename = logFileName; }
  
	/// gets or sets the log level
	inline const int4 & GetLogLevel()  { return m_iLogLevel; }
	inline void  SetLogLevel( const int4 & iLogLevel )  { m_iLogLevel = iLogLevel; }

  /// Gets or sets the OutputFilename
  inline const std::string & GetOutputFilename()  { return m_sOutputFilename; }
  inline void SetOutputFilename( const std::string & outPutFileName  )  { m_sOutputFilename = outPutFileName; }
        
  /// Gets or sets the VolumeFilePath
  inline const std::string & GetVolumeFilePath()  { return m_sVolumeFilePath; }
  inline void SetVolumeFilePath( const std::string & volFileName  )  { m_sVolumeFilePath = volFileName; }
  
  ///Gets or sets the Destination File path
  inline const std::string & GetDestinationFilePath()  { return m_sDestFilePath; }
  inline void SetDestinationFilePath(  const std::string & destFilePath )  { m_sDestFilePath = destFilePath; }


  ///Gets or sets whether it is DICOM only or not.
  inline const bool IsDicomOnly()  { return m_bDicomOnly; }
  inline void SetDicomOnly( const bool dicomOnly )  { m_bDicomOnly = dicomOnly; }

  ///Prints the argument list
  void PrintArguments();

  //Prints arguments to a file
  void PrintArgumentsToLogFile( );


private:

  ///Output file Name
  std::string m_sDestFilePath;

  ///Preprocess directory
  std::string m_sPreProcessDirectory;

  ///Plugin app id
  std::string m_sPluginAppId;

  ///Dicom Directory
  std::string m_sDicomDirectory; 

  ///Volume File Name
  std::string m_sVolumeFilePath;

  ///Log File Name
  std::string m_sLogFilename;

  ///Output File Name
  std::string m_sOutputFilename;

  ///Dicom Only Flag
  bool m_bDicomOnly;

	/// log lever: 0 = no log; 1 = exception; 2 = error; 3 = dbg; 4 = info
	int4 m_iLogLevel;  

};// class ColonEnvironmentArgs


// $Log: colonEnvironmentArgs.h,v $
// Revision 1.8  2006/12/18 21:32:31  romy
// extended for Commandline GUI
//
// Revision 1.7  2006/05/05 14:30:03  mkontak
// Fixes for starting of cad and removal of unused parameters
//
// Revision 1.6  2006/05/03 16:46:19  romy
// destFile Path added
//
// Revision 1.5  2006/05/03 16:42:07  romy
// destFile Path added
//
// Revision 1.4  2006/05/03 16:17:17  romy
// destFile Path added
//
// Revision 1.3  2006/05/03 14:58:03  romy
// added the output file name
//
// Revision 1.2  2006/02/09 21:20:01  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.1  2006/02/09 19:27:54  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/colonEnvironmentArgs.h,v 1.8 2006/12/18 21:32:31 romy Exp $
// $Id: colonEnvironmentArgs.h,v 1.8 2006/12/18 21:32:31 romy Exp $

