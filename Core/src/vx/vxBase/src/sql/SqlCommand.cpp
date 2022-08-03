// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlCommand.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

// includes
#include "stdafx.h"
#include "sql\SqlConnection.h"
#include "sql\SqlCommand.h"
#include "sql\SqlError.h"
#include "sql\SqlParameter.h"
#include "sql\SqlHandle.h"
#include "Exception.h"

// using
using namespace sql;

/**
 * Sql Command constructor
 *
 * @param sCommandText      Command text
 * @param type              Command type
 * @param connection        SqlConnection object
 */
SqlCommand::SqlCommand(const std::string & sCommandText, CommandTypes type, std::shared_ptr<SqlConnection> connection) :
m_sCommandText(sCommandText),
m_commandType(type),
m_handle(SqlHandle::HT_Statement),
m_connection(connection)
{

} // SqlCommand(const std::string & sCommandText, CommandTypes type, std::shared_ptr<SqlConnection> connection)


/**
 * Destructor
 */
SqlCommand::~SqlCommand()
{
  m_handle.Free();
} // ~SqlCommand()


/**
 * Creates the SqlCommand
 *
 * @param connection      Sql Connection
 *
 * @return std::shared_ptr<SqlCommand>
 */
std::shared_ptr<SqlCommand> SqlCommand::Create(const std::string & sCommandText, CommandTypes type, std::shared_ptr<SqlConnection> connection)
{
  std::shared_ptr<SqlCommand> pSqlCommand = std::shared_ptr<SqlCommand>(new SqlCommand(sCommandText, type, connection));

  if ( pSqlCommand == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate SqlCommand","SqlCommand","Create"));

  return pSqlCommand;

} // Create(const std::string & sCommandText,std::shared_ptr<SqlConnection> connection)


/**
 * Adds parameter to list
 */
void SqlCommand::AddParameter(std::shared_ptr<SqlParameter> & parameter)
{
  m_parameters.push_back(parameter);
  parameter->m_iNumber = m_parameters.size();
}


/**
 * Gets the SqlParameter object associated with the name
 * 
 * @param sName       Name of parameter
 * @return sql parameter pointer
 */
std::shared_ptr<SqlParameter> SqlCommand::GetParameter(const std::string & sName)
{

  for ( std::vector<std::shared_ptr<SqlParameter>>::iterator it = m_parameters.begin(); it != m_parameters.end(); it++ )
  {
    if ( (*it)->GetName() == sName )
      return (*it);
  }

  return std::shared_ptr<SqlParameter>(__nullptr);
  //throw SqlNotFoundException(vsLogUtility::Format("Parameter %s was not found". sName.c_str()));
} // GetParameter


/**
 * Executes the command
 */
void SqlCommand::Execute()
{
  vxLogTimer timer;

  if ( ! m_connection->IsOpen() )
    throw ex::SqlException(LogRec("Connection is not open","SqlCommand","Execute"));

  m_handle.Allocate(m_connection->m_connection);
   
 
  switch ( m_commandType )
  {
  case CT_StoredProcedure:
    ExecuteStoredProcedure();
    break;
  case CT_Text:
    ExecuteText();
    break;
  case CT_TableDirect:
    ExecuteTableDirect();
    break;
  };

  timer.Trace(util::Strings::Format("Executing %s ", m_sCommandText.c_str()));

} // Execute()


/**
 * Executes the stored procedure
 */
void SqlCommand::ExecuteStoredProcedure()
{

  SQLSMALLINT pcount(0);

  SQLRETURN retcode;


  std::ostringstream os;

  os << " {call " << m_sCommandText;

  if ( m_parameters.size() > 0 )
  {

    os << "(";

    int last((int)(m_parameters.size() - 1));
    for ( int i(0); i < m_parameters.size(); i++ )
    {
      if ( i == last )
        os << "?";
      else 
        os << "?,";
    } // for

    os << ")";

  } // if
  
  os << "} ";


  std::string sPrepared(os.str().c_str());


  retcode = SQLPrepare(m_handle.GetHandle(), (SQLCHAR *)os.str().c_str(), SQL_NTS);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
  {
    SqlError error(SQL_HANDLE_STMT,m_handle.GetHandle());
    m_handle.Free();
    throw ex::SqlException(LogRec(util::Strings::Format("Failed preparing command : %s",error.GetLogMessage().c_str()),"SqlCOmmand","ExecuteStoreProcedure"));
  } // END ... If the prepare failed

  SQLNumParams(m_handle.GetHandle(), &pcount );

  // Make sure we have the right amount of parameters
  if ( pcount > m_parameters.size() )
    throw ex::SqlException(LogRec("Invalid parameter count","SqlCommand","ExecuteStoredProcedure") );

  // =======================================
  // Bind all parameters to the statement
  // ========================================
  for ( std::vector<std::shared_ptr<SqlParameter>>::iterator it = m_parameters.begin(); it != m_parameters.end(); it++ )
  {
    std::shared_ptr<SqlParameter> pParameter(*it);


    SQLUSMALLINT iNumber(pParameter->GetNumber());
    SQLPOINTER pValue(pParameter->GetValue());
    SQLINTEGER iSize(pParameter->GetSize());
    SQLLEN * pLength(pParameter->GetLengthPtr());

    retcode = SQLBindParameter( m_handle.GetHandle(),
                                iNumber,
                                (SQLSMALLINT)pParameter->GetDirection(), 
                                (SQLSMALLINT)pParameter->GetCType(), 
                                (SQLSMALLINT)pParameter->GetSqlType(),
                                0, 
                                0, 
                                pValue,
                                iSize, 
                                pLength);
 
  } // END ... For each parameter


  retcode = SQLExecute(m_handle.GetHandle());

  // =======================
  // Check the return code
  // =======================
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
  {
    SqlError error(m_handle.GetError());
    m_handle.Free();


    LogErr(util::Strings::Format("Execution of store procedure %s failed - %s",m_sCommandText.c_str(), error.GetLogMessage().c_str()), "SqlCommand", "ExecuteStoredProcedure");

    throw ex::SqlException(LogRec(error.GetMessage(), "SqlCommand", "ExecuteStoredProcedure"));
  } // END ... If the execute failed
    

} // ExecuteStoredProcedure()

