// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmLog.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


namespace dcm
{

// =========================================================================================
//  Implements the logging through the Enterprise system so that Event tracing can be used.
// =========================================================================================
class DcmLog
{

#pragma region conctructors/destructors

  /// Constructor
  DcmLog() {}

  /// Destructor
  virtual ~DcmLog() {}

#pragma endregion

#pragma region Logging

public:

  /// Logs a Timimng message
  static void Timing( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a debug message
  static void Timing( const std::string & sMessage, const std::string sClass, const std::string & sMethod );


  /// Logs a debug message
  static void Debug( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a debug message
  static void Debug( const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a warning message
  static void Warning( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a warning message
  static void Warning( const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a error message
  static void Error( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a error message
  static void Error( const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a error message
  static void Error( const std::string & sSource,  const vxLogRecord & record );

  /// Logs a error message
  static void Error( const vxLogRecord & record );

  /// Logs a information message
  static void Information( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod );

  /// Logs a information message
  static void Information( const std::string & sMessage, const std::string sClass, const std::string & sMethod );

#pragma endregion


private:

  /// Gets the default source
  const std::string & GetDefaultSource();

private:

  /// Default source
  static std::string m_sDefaultSource;

};  // class DcmLog

};  // namespace dcm