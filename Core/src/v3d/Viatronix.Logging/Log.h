//

#pragma once

#include "LoggersSection.h"
#include "LogEnums.h"

using namespace System;

OPEN_LOGGING_NS



/**
 * Log object allow creating and logging to specified logger 
 */
public ref class Log
{
public:

  /// Initializes using the loggers section
  static void Initialize()
  { Initialize("loggers"); }

  /// INitiazlie using a custom name
  static void Initialize(String ^ msSectionName);

  /// Creates a logger from the xml node
  static void Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the loggers in the list
  static void Create(LoggersSection ^ mpSection);

  /// Creates a logger using the parameters
  static inline void Create(System::String ^ msName, System::String ^ msFilename, LogLevel eLogLevel,  MessageFormat eFormat,  bool bDefault)
  { Create(msName, msFilename, eLogLevel, eFormat, bDefault, String::Empty, 0); }

  /// Creates a logger using the parameters
  static void Create(System::String ^ msName, System::String ^ msFilename, LogLevel eLogLevel,  MessageFormat eFormat, bool bDefault, String ^ msServer, unsigned int iPort);
  
  /// Gets the default log level
  static LogLevel GetLogLevel();

  /// Gets the default log file name
  static String ^ GetDefaultLoggerFilename();

  /// Gets the default log level
  static LogLevel GetDefaultLoggerLogLevel();


#pragma region logging methods

public:

  /// Logs a warning message to the specified logger
  static void Warning(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs a critical message to the specified logger
  static void Critical(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs an error  message to the specified logger
  static void Error(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs a debug message to the specified logger
  static void Debug(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);
 
  /// Logs an information message to the specified logger
  static void Information(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

 /// Logs a debug message to the specified logger
  static void Timing(System::String ^ msSource, System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);
 
   /// Logs a warning message to the specified logger
  static void Warning(System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs a critial message to the specified logger
  static void Critical( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs an error  message to the specified logger
  static void Error( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs a debug message to the specfied logger
  static void Debug( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);
 
  /// Logs an information message to the specified logger
  static void Information( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

  /// Logs an timing message as debug to the specified logger
  static void Timing( System::String ^ msMessage, System::String ^ msClass, System::String ^ msMethod);

#pragma endregion

};  // class Log


CLOSE_LOGGING_NS
