// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlCommand.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once

// includes
#include "SqlHandle.h"


namespace sql
{

// Forward declarations
class VX_BASE_DLL SqlConnection;
class VX_BASE_DLL SqlParameter;


/**
 * Emulates the SqlCommand operation like in .NET. Allows the adding of the parameters and the
 * execution of stored procedures.
 */
class VX_BASE_DLL SqlCommand
{

public:

  /// Command Type
  typedef enum CommandTypes
  {
    CT_StoredProcedure,
    CT_Text,
    CT_TableDirect
  } CommandTypes;

private:

  /// Constructor ( Private so that only shared_ptr are created )
  SqlCommand(const std::string & sCommandText, CommandTypes type, std::shared_ptr<SqlConnection> connection);

public:

  /// Destructor
  ~SqlCommand();

  /// Creates a SqlCommand object
  static std::shared_ptr<SqlCommand> Create(const std::string & sCommandText, CommandTypes type, std::shared_ptr<SqlConnection> connection);

  /// Executes the command
  void Execute();

  /// Add parameter
  void AddParameter(std::shared_ptr<SqlParameter> & parameter);

  /// Gets the parameter by name
  std::shared_ptr<SqlParameter> GetParameter(const std::string & sName);

private:

  /// Executes the stored procedure
  void ExecuteStoredProcedure();

  /// Not Implemented
  void ExecuteText() {}

  /// Not implemented
  void ExecuteTableDirect() {}


public:

  /// Gets the command text
  const std::string & GetCommandText() const
  { return m_sCommandText; }


  /// Gets the command type
  const CommandTypes GetCommandType() const
  { return m_commandType; }

  /// Sets the command type
  void SetCommandType(const CommandTypes commandType)
  { m_commandType = commandType; }

  /// Gets the connections
  std::shared_ptr<SqlConnection> GetConnection() 
  { return m_connection; }

private:

  /// Command Type
  CommandTypes m_commandType;

  /// Command text
  std::string m_sCommandText;

  /// Connection object
  std::shared_ptr<SqlConnection> m_connection;

  /// Statement handle
  SqlHandle m_handle;

  /// Parameter list
  std::vector<std::shared_ptr<SqlParameter>> m_parameters;

};  // class SqlCommand

}; //namespace sql