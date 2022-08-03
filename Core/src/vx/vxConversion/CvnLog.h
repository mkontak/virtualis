// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


namespace cvn
{



// =========================================================================================
//  Implements the logging through the Enterprise system so that Event tracing can be used.
// =========================================================================================
class CvnLog
{

#pragma region conctructors/destructors

  /// onstructor
  CvnLog() {}

  /// Destructor
  virtual ~CvnLog() {}

#pragma endregion

#pragma region Logging

public:
  
  // Log Debug Message
  static void Debug( const std::string & sMessage, const std::string sClass = "", const std::string & sMethod = "" );
    
  // Log Timing Message
  static void Timing( const std::string & sMessage, const std::string sClass = "", const std::string & sMethod = "" );

  // Log Warning Message
  static void Warning( const std::string & sMessage, const std::string sClass = "", const std::string & sMethod = "" );

  // Log Error Message
  static void Error( const std::string & sMessage, const std::string sClass = "", const std::string & sMethod = "" );

 
  // Log Error Message
  static void Error( const vxLogRecord & record );


  // Log Information Message
  static void Information( const std::string & sMessage, const std::string sClass = "", const std::string & sMethod = "" );


#pragma endregion

public:

    
  // Gets the log source
  static const std::string & GetLogSource()
  { return m_sLogSource; }
    
  // Sets the log source
  static void SetLogSource(const std::string & sLogSource) 
  { m_sLogSource = sLogSource; }

private:

  /// Log Source
  static std::string m_sLogSource;

};  // class Log

};  // namespace cvn