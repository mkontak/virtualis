// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlConnectionStrings.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once



namespace sql
{

#define ConnectionStringCollection std::map<std::string,std::string>


/**
 * Reads the connectionstrings.config file
 */
class VX_BASE_DLL DatabaseConnectionStrings
{

public:

  DatabaseConnectionStrings();


  /// Get the database collection strings for the application specified
  static ConnectionStringCollection & GetConnectionStrings(const std::string & sApplication = "default");

  /// Gets the specific database connections string 
  static std::string GetConnectionString(const std::string & sDatabase, const std::string & sApplication = "default");

protected:

  static void BuildDatabaseConnectionStrings();
 
#pragma region fields

private:

  static std::map<std::string, ConnectionStringCollection > m_databaseConnectionStrings;

#pragma endregion

}; // class SqlConnectionStrings


}; // namespace sql