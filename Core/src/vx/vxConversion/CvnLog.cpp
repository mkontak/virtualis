#include "Stdafx.h"
#include "CvnLog.h"


using namespace cvn;


// Static definitions
std::string CvnLog::m_sLogSource;




/**
* Logs a debug message
*/
void CvnLog::Debug( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  if ( m_sLogSource.empty() )
  {
    LogDbg(sMessage, sClass, sMethod );
  }
  else
  {
    LogDbgEx(m_sLogSource, sMessage, sClass, sMethod );
  }
} // Debug( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod);

  /**
* Logs a debug message
*/
void CvnLog::Timing( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  if ( m_sLogSource.empty() )
  {
    LogTime(sMessage, sClass, sMethod );
  }
  else
  {
    LogTimeEx(m_sLogSource, sMessage, sClass, sMethod );
  }

} // DebugTiming( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod);

/**
* Logs a warning message
*/
void CvnLog::Warning( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  if ( m_sLogSource.empty() )
  {
    LogWrn(sMessage, sClass, sMethod );
  }
  else
  {
    LogWrnEx(m_sLogSource, sMessage, sClass, sMethod );
  }
} // Warning( td::string & sMessage, std::string sClass, std::string & sMethod)

/**
* Logs a error message
*/
void CvnLog::Error( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  if ( m_sLogSource.empty() )
  {
    LogErr(sMessage, sClass, sMethod );
  }
  else
  {
    LogErrEx(m_sLogSource, sMessage, sClass, sMethod );
  }
} // Error( std::string & sMessage, std::string sClass, std::string & sMethod)

/**
* Logs a error message
*/
void CvnLog::Error( const vxLogRecord & record )
{

  if ( m_sLogSource.empty() )
  {
    vxLogRec(record);
  }
  else
  {
    vxLogRecEx(m_sLogSource, record );
  }
} // Error(  const LogRec & logRec )

/**
* Logs a information message
*/
void CvnLog::Information( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  if ( m_sLogSource.empty() )
  {
    LogFyi(sMessage, sClass, sMethod );
  }
  else
  {
    LogFyiEx(m_sLogSource, sMessage, sClass, sMethod );
  }
} // Information( std::string & sMessage, std::string sClass, std::string & sMethod)
