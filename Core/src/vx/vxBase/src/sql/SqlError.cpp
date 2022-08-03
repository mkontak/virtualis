// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlError.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

// includes
#include "stdafx.h"
#include "sql\SqlError.h"

// using
using namespace sql;

/**
 * Constructor
 *
 *
 * @param handleType      Handle type
 * @param handle          Handle
 */

SqlError::SqlError(SQLSMALLINT handleType, SQLHANDLE handle) :
m_iCode(0),
m_sState(""),
m_sSqlServerInformation(""),
m_sMessage("")
{

  m_iCode = 0;

  SQLCHAR state[10];
  SQLCHAR errorMessage[512];
  SQLSMALLINT overrun(0);

  RtlZeroMemory(state,10);
  RtlZeroMemory(errorMessage,512);

  // Get diagnostic record
  SQLGetDiagRec(handleType, handle, 1, state, &m_iCode, errorMessage, 512, &overrun);

  // Set the state
  m_sState = std::string((LPSTR)state);

  // Set the message
  std::string sMessage = std::string((LPSTR)errorMessage);

  size_t pos;
  if ( (pos = sMessage.find("VX")) != std::string::npos )
  {
    m_sSqlServerInformation = sMessage.substr(0,pos);
    m_sMessage = sMessage.substr(pos);
  }
  else
    m_sMessage = sMessage;


  
  
  
} // SqlError(SQLSMALLINT handleType, SQLHANDLE handle)


/**
 * Copy constructor
 */
SqlError::SqlError(const SqlError & error) :
m_iCode(error.GetCode()),
m_sState(error.GetState()),
m_sMessage(error.GetMessage())
{
} // SqlError(const SqlError & error) 


/**
 * Copy constructor
 */
SqlError::SqlError() :
m_iCode(0),
m_sState(""),
m_sMessage("")
{
} // SqlError() 


/**
 * Gets the Log Message
 */
std::string SqlError::GetLogMessage()
{
  return util::Strings::Format("%s (%s/%d)",m_sMessage.c_str(), m_sState.c_str(), m_iCode);
}   // GetLogMessage()