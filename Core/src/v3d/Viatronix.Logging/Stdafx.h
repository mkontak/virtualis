// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once


#pragma warning(disable:4251)
#pragma warning(disable:4482)


#include <stdio.h>
#include <xstring>
#include <string>
#include <cstring>



#include "Logger.h"


#define USING_LOGGING_NS  using namespace Viatronix::Logging;
#define OPEN_LOGGING_NS    namespace Viatronix { namespace Logging {
#define CLOSE_LOGGING_NS   } } 



/// Callback used for the dicom services
public delegate void LoggingCallbackDelegate(PLOG_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData);



/*
 * Converts a mananged System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string represenetatioon of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ss()


#pragma region Exceptions


/**
 * Impelements an Abort Exception
 */
public ref class AbortException : public System::Exception 
{
public:
  /// Constructor
  AbortException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  AbortException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Destructor
  virtual ~AbortException() { this->!AbortException(); }

  /// Finalizer
  !AbortException() { }

};  // class AbortException

/**
 * Implements a base Dicom exception
 ^ All other exceptions should inheret from this.
 */
public ref class LogException : public System::Exception
{
public:
  /// Constructor
  LogException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  LogException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Constructor
  LogException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : System::Exception(pInfo, context)  {  }

  /// Destructor
  virtual ~LogException() { this->!LogException(); }

  /// Finalizer
  !LogException() {}

};  // class LogException



#pragma endregion