// This is the main DLL file.

#include "stdafx.h"


#include "Log.h"
#include "LoggersSection.h"


using namespace System;
using namespace System::Xml;
USING_LOGGING_NS


/**
 * Gets the default log leve; (Backward compatibility)
 */
LogLevel Log::GetLogLevel()
{
  return (LogLevel)vxLoggerInfo::GetDefaultLogLevel();
} // GetLogLevel()

/**
 * Gets the default log leve; (Backward compatibility)
 */
String ^ Log::GetDefaultLoggerFilename()
{
  return gcnew String(vxLogFactory::GetDefaultLoggerFilename().c_str());
} // GetLogLevel()

/**
 * Gets the default log leve; (Backward compatibility)
 */
LogLevel Log::GetDefaultLoggerLogLevel()
{
  return (LogLevel)vxLogFactory::GetDefaultLoggerLogLevel();
} // GetLogLevel()


/**
 * Creates all loggers in the supplied Loggers Section
 *
 * @param mpSection     Loggers section
 */
void Log::Initialize(String ^ msSectionName)
{

  LoggersSection ^ mpSection = LoggersSection::GetSection(msSectionName);

  Create(mpSection);

} //  Create(LoggersSection ^ mpSection)

/**
 * Creates all loggers in the supplied Loggers Section
 *
 * @param mpSection     Loggers section
 */
void Log::Create(LoggersSection ^ mpSection)
{
  if ( mpSection != nullptr )
  {
    for each ( LoggerElement ^ mpElement in mpSection->Lists)
    {
      Create(mpElement->Name, mpElement->File, mpElement->Level, mpElement->Format, mpElement->Default, mpElement->Server, mpElement->Port );
    } // END ... for each logger element
  }

} //  Create(LoggersSection ^ mpSection)

/**
 * Creates a logger from the xml node
 *
 * @param mpNode      Xml node defining the logger
 *
 */
void Log::Create(XmlNode ^ mpNode)
{

  System::String ^ msName(mpNode->Attributes["name"]->Value);
  System::String ^ msFilename(mpNode->Attributes["file"]->Value);
  LogLevel logLevel((LogLevel)Enum::Parse(LogLevel::typeid, mpNode->Attributes["level"]->Value, true));
  bool bDefault(  mpNode->Attributes["default"]  != nullptr ? System::Convert::ToBoolean(mpNode->Attributes["default"]->Value) : false );
  System::String ^ msServer(  mpNode->Attributes["server"]  != nullptr ? mpNode->Attributes["server"]->Value : String::Empty );
  unsigned int iPort (  mpNode->Attributes["port"]  != nullptr ? System::Convert::ToUInt32(mpNode->Attributes["port"]->Value) : 0 );
  MessageFormat format( ( mpNode->Attributes["format"] != nullptr ? (MessageFormat)Enum::Parse(MessageFormat::typeid, mpNode->Attributes["format"]->Value, true) : MessageFormat::Default ) );

  Create(msName, msFilename, logLevel, format, bDefault);

} // Create(XmlNode ^ mpNode)

/**
 * Creates the logger
 *
 * @param msName      Name of logger
 * @param msFilename  File name to log to
 * @param eLogLevel   Logging level
 */
void Log::Create(String ^ msName, String ^ msFilename, LogLevel eLogLevel, MessageFormat eFormat, bool bDefault, String ^ msServer, unsigned int iPort)
{
  vxLogFactory::CreateLogger( ss(msName), ss(msFilename), static_cast<vxLogRecord::SeverityEnum>((int)eLogLevel), 
                              static_cast<vxLogMessageFormats>(eFormat), 
                              bDefault, ss(msServer), iPort );
} // Create(String ^ msName, String ^ msFilename, LogLevel eLogLevel)


#pragma region Log Methods With Source

/**
 * Logs and error message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Error(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogErrEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Error(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a warning  message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Warning(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogWrnEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Warning(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)


/**
 * Logs a critical  message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Critical(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogCrtEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Critical(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a debug  message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Debug(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogDbgEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Debug(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a information  message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Information(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogFyiEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Information(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a timing  message to the specified logger
 *
 * @param msSource        Log source/name
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Timing(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogTimeEx(ss(msSource),ss(msMessage), ss(msClass), ss(msMethod));
} // Debug(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

#pragma endregion


#pragma region Log Methods With-out Source
/**
 * Logs and error message to the specified logger
 *
  * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Error(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogErr(ss(msMessage), ss(msClass), ss(msMethod));
} // Error(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a warning  message to the specified logger
 *
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Warning(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogWrn(ss(msMessage), ss(msClass), ss(msMethod));
} // Warning(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)


/**
 * Logs a critical  message to the specified logger
 *
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Critical( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogCrt(ss(msMessage), ss(msClass), ss(msMethod));
} // Critical(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a debug  message to the specified logger
 *
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Debug(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogDbg(ss(msMessage), ss(msClass), ss(msMethod));
} // Debug(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

/**
 * Logs a information  message to the specified logger
 *
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Information(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogFyi(ss(msMessage), ss(msClass), ss(msMethod));
} // Information(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)


/**
 * Logs a information  timing message to the specified logger
 *
 * @param msMessage       Message
 * @param msClass         Class name
 * @param msMtheod        Method name
 */
void Log::Timing(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)
{
  LogTime(ss(msMessage), ss(msClass), ss(msMethod));
} // DebugTiming(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod)

#pragma endregion