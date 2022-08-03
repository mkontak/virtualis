// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLoggerClassic.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// includes
#include "stdafx.h"
#include "vxLogEnums.h"
#include "vxLoggerClassic.h"
#include "vxLogRecord.h"
#include "vxLogExceptions.h"
#include "vxLogMutex.h"


/**
 * Copy Constructor
 *
 * @param loggerInfo        Logger information
 */
vxLoggerClassic::vxLoggerClassic ( const vxLoggerInfo & loggerInfo ) :
vxLogger(loggerInfo)
{
 
}//vxLoggerClassic ( const vxLogger & logger )


/**
 * Destructor
 */
vxLoggerClassic::~vxLoggerClassic()
{
    Release();
}//~vxLoggerClassic()



/**
 * Creates the logger
 *
 * @param loggerInfo        Logger information
 */
std::shared_ptr<vxLogger> vxLoggerClassic::Create(const vxLoggerInfo & loggerInfo)
{
  
  std::shared_ptr<vxLogger> pLogger(new vxLoggerClassic(loggerInfo));

  if ( pLogger == __nullptr ) 
    throw vxLogOutOfMemoryException(LogRec("Failed to allocation vxLogger", "vxLogger", "Create"));

 
  pLogger->CreateMutex();

  return pLogger;

} // Create()



/**
 * Writes the message out to the log file
 *
 * @param record   vxLogRecord object which contains all the log data.
 *
 */
void vxLoggerClassic::Trace( const vxLogRecord & record )
{
 

  if ( IsLoggable ( record.GetSeverityLevel() ) ) //Check the Log Level
  {


    try
    {
      
      

      m_mutex->Wait();


      FILE * fp(__nullptr);
      
      errno_t error = fopen_s( &fp, m_sFilename.c_str(), "a+" );
 
      if ( fp  != __nullptr )
      {

        std::string sMessage(const_cast<vxLogRecord &>(record).FormatMessage(m_eFormat));

        // Append the message to the log File.
        fwrite( sMessage.c_str(), sizeof( char ), sMessage.size(), fp );
  
        //Flush the stream.
        if ( EOF ==  fflush( fp ) )
            vxLogUtility::LogMessage( "fflush returns EOF, data might be lost due to a write failure");
 
        fclose( fp );
  
      }
      else
        vxLogUtility::LogMessage(util::Strings::Format("Could not open  %s  for editing [%d] : %s", m_sFilename.c_str(), error, record.GetTextMessage().c_str()));

      m_mutex->Release();

    }
    catch( ...  )
    {
      m_mutex->Release();
 
      vxLogUtility::LogMessage("Some exception in Logger Trace");
    }

  } // END ... If the message is loggable

} // Trace( const vxLogRecord & record ) 


