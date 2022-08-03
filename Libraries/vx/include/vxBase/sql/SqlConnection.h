// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlConnection.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once

// includes
#include "SqlHandle.h"

namespace sql
{

  
/**
 * Emulates the SqlConnection object from .Net. Handles the opening and closing of the database connections
 */
class VX_BASE_DLL SqlConnection
{

public:

  /// Constructor
  SqlConnection(const std::string & sConnectionString);


  /// Destructor
  ~SqlConnection();

  /// Creates the connection
  static std::shared_ptr<SqlConnection> Create(const std::string & sConnectionString);

  /// Opens the database connection
  void Open();

  /// Closes the database connection
  void Close();


public:

  /// True is open, false if not
  const bool IsOpen() const;
  
private:

  /// Connection string
  std::string m_sConnectionString;
 
  /// Environment handle
  SqlHandle m_environment;

  /// DB Handle
  SqlHandle  m_connection;

private:

  ///Connected flag
  bool m_bConnected;

  ///
  friend class SqlCommand;
  
}; // class SqlConnection

}; // namespace sql