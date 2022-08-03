// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Database.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

/// includes
#include <memory>
#include "sql\SqlConnection.h"

/// Forward declarations
class User;

/**
* Implements the database procedures to be performed
*/
class Database
{
public:

  /// Acquires a context
  static void AddUser(CString & sLogin, CString & sPassword, CString & sName, CString & sRole, bool bPasswordChange);

  /// Log into the system using the specified username and password
  static CString AcquireContext(CString & sLogin, CString & sPassword);

  /// Logout context
  static void ReleaseContext(CString & sContext);

  /// Get Roles
  static std::vector<std::string> GetRoles();

protected:

  /// Global context
  static CString GetContext();


private:
  /// create connection
  static std::shared_ptr< sql::SqlConnection > CreateConnection(const std::string & sDatabaseName);

};  // class Database

