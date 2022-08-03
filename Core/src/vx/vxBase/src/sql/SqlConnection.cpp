// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlConnection.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

// includes
#include "stdafx.h"
#include "sql\SqlConnection.h"
#include "sql\SqlError.h"
#include "Exception.h"

// using
using namespace sql;

/**
 * Constructor 
 *
 * Reformats the connection string to an ODBC 
 *
 * @param sConnectionString     Connection string.
 */
SqlConnection::SqlConnection(const std::string & sConnectionString) :
m_sConnectionString(sConnectionString),
m_environment(SqlHandle::HT_Environment),
m_connection(SqlHandle::HT_Connection)
{
  //============================================================================================================
  // Connection Strings
  //
  // ODBC: "Data Source=myServer\myInstance;Initial Catalog=myDataBase;User Id=myUsername;Password=myPassword;"
  //
  // OURS: "user id=myUser;password=myPassword;server=mySERVER\myINSTANCE;database=myDatabase"
  //
  //==============================================================================================================

  size_t pos = m_sConnectionString.find("user id");
  if ( pos != std::string::npos )
      m_sConnectionString.replace(pos, 7, "Uid", 0, 3);

  pos = m_sConnectionString.find("password");
  if ( pos != std::string::npos )
      m_sConnectionString.replace(pos, 8, "Pwd", 0, 3);

  pos = m_sConnectionString.find("server");
  if ( pos != std::string::npos )
      m_sConnectionString.replace(pos, 6, "Server");

  pos = m_sConnectionString.find("database");
  if ( pos != std::string::npos )
      m_sConnectionString.replace(pos, 8, "Database");

  pos = m_sConnectionString.find("(local)");
  if ( pos != std::string::npos )
  {
    char hostName[256];
    DWORD size = 256;
    GetComputerName(hostName, &size);
    m_sConnectionString.replace(pos, 7, hostName, 0, size);
  }

  /// Add driver
  //m_sConnectionString.insert(0,"Driver={SQL Server Native Client 11.0};");

} // SqlConnection(const std::string & sConnectionString)


/**
 * Destructor performs a cleanup of resources
 */
SqlConnection::~SqlConnection()
{
  Close();
} // ~SqlConnection()


/**
 * Creates the SqlConnection shared object
 *
 * @param sConnectionString     Connection string
 * @return sql connection pointer
 */
std::shared_ptr<SqlConnection> SqlConnection::Create(const std::string & sConnectionString)
{

  // Create object
  std::shared_ptr<SqlConnection> pConnection(std::make_shared<SqlConnection>(sConnectionString));

  // Make sure it is not NULL
  if ( pConnection == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate SqlConnection","SqlConnection","Create"));

  // Return the created object as a shared pointer
  return pConnection;

} // Create(const std::string & sConnectionString)


/**
 * Closes the connection
 */
void SqlConnection::Close()
{

  if ( IsOpen() )
  {
    SQLRETURN retcode = SQLDisconnect(m_connection.GetHandle());

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
    {
      SqlError error(m_connection.GetError());
      LogWrn(util::Strings::Format("Problem disconnecting from database : %s", error.GetLogMessage().c_str()), "SqlConnection", "Close");
    }
  }


  // Free the connection handle
  m_connection.Free();

  // Free the environment handle
  m_environment.Free();
 
} // Close()


/**
 * Opens the Sql connection
 */
void SqlConnection::Open()
{

  // Allocates a new environment handle
  m_environment.Allocate();

  // Set  ODBC 3.0
  SQLSetEnvAttr(m_environment.GetHandle(), SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

  // Allocate a new connection handle
  m_connection.Allocate(m_environment);

  // Set the login timeout so we don't wait so long
  SQLSetConnectAttr(m_connection.GetHandle(), SQL_LOGIN_TIMEOUT, (void *)5, 0);


  SQLSMALLINT overrun;
  SQLCHAR ocs[1024];
     
  // Connect
  SQLRETURN retcode = SQLDriverConnect(m_connection.GetHandle(), NULL, (SQLCHAR *)m_sConnectionString.c_str(), SQL_NTS, ocs, 1024, &overrun, SQL_DRIVER_NOPROMPT);

  // ==================================
  // Check the connections status
  // =================================
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
  {
    SqlError error(m_connection.GetError());
    Close();
    throw ex::SqlException(LogRec(util::Strings::Format("Failed to open connection : %s", error.GetLogMessage().c_str()), "SqlConnection", "Open"));
  }

} // Open()


/**
 * Indicates if the connection is open
 */
const bool SqlConnection::IsOpen() const
{ 
  return ( m_connection.IsAllocated() ); 
} // IsOpen()

