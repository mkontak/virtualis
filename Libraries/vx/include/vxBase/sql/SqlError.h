// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlError.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once


namespace sql
{

/**
 * Implements a SQL Error object containing diagnostic information about an error
 */
class SqlError
{
public:

  /// Constructor
  SqlError(SQLSMALLINT handleType, SQLHANDLE handle);

  /// Copy constructor
  SqlError(const SqlError & error);

  /// Default constructor
  SqlError();

	/// constructor
  SqlError(const std::string & sMessage, const std::string & sState = "", SQLINTEGER iCode = 0 ) :
  m_iCode(iCode),
  m_sState(sState),
  m_sMessage(sMessage)
  {
  }


public:

  /// Gets the Error Code
  const SQLINTEGER GetCode() const 
  { return m_iCode; }

  /// Gets the Error State
  const std::string & GetState() const
  { return m_sState; }

  /// Gets the Error Message
  const std::string & GetMessage() const
  { return m_sMessage; }
  
  const std::string & GetSqlServerInformation() const
  { return m_sSqlServerInformation; }

  /// Gets the Log Message
  std::string GetLogMessage();

  

private:

  /// Error code
  SQLINTEGER m_iCode;

  /// State(Code)
  std::string m_sState;

  /// Error Message
  std::string m_sMessage;

  /// Database info
  std::string m_sSqlServerInformation;

}; // class SqlError

}; // namespace sql