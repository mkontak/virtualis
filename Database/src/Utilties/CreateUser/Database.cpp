// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Database.C
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "Database.h"
#include "Exceptions.h"
#include "sql\SqlCommand.h"
#include "sql\SqlError.h"
#include "sql\SqlParameter.h"
#include "sql\SqlHandle.h"
#include "sql\DatabaseConnectionStrings.h"
#include "crypto\AESCrypt.h"


#define XML_SIZE 32768
#define DATABASE_SECURITY "security"
#define DATABASE_STORAGE "storage"
#define DATABASE_SYSTEM "system"


/**
* Acquires a context for the specified parameters. The newly acquired context will be returned in the sContextId parameters.
*
* @paarm sLogin              In      Users login
* @param sPassword           IN      Users password
*/
CString Database::AcquireContext(CString & sLogin, CString & sPassword)
{
  std::shared_ptr<sql::SqlConnection > connection(CreateConnection(DATABASE_SECURITY));

  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_acquireContext]", sql::SqlCommand::CT_StoredProcedure, connection);

  std::string sContextId;

  command->AddParameter(sql::SqlInParameter::Create<std::string>("LOGIN", (LPCTSTR)sLogin, sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create<std::string>("PASSWORD",  crypto::AESCrypt::Encrypt((LPCTSTR)sPassword), sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create<std::string>("NEW_PASSWORD", "", sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInOutParameter::Create("CONTEXT_ID", sContextId, 256, sql::ST_VARCHAR));

  command->Execute();

  if (sContextId.empty())
    sContextId.assign((char *)command->GetParameter("CONTEXT_ID")->GetValue());

  return CString(sContextId.c_str());

} // Login(CString & sLogin, CString & sPassword)


/**
* Releases the Context specified
*
* @param sCOnnectionString   In      Connection String
* @param sContextId          In      Context to be released
*/
void Database::ReleaseContext(CString & sContextId)
{

  std::shared_ptr<sql::SqlConnection > connection(CreateConnection(DATABASE_SECURITY));

  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_releaseContext]", sql::SqlCommand::CT_StoredProcedure, connection);

  command->AddParameter(sql::SqlInParameter::Create("ID", (LPCTSTR)sContextId, sql::ST_VARCHAR));

  command->Execute();

} // Logout(CString& sContextId)


/**
 * Returns the global context
 */
CString Database::GetContext()
{
  static CString sContext(crypto::AESCrypt::Decrypt("cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ").c_str());

  return sContext;
}

/**
* Get roles from security database
*  @return All roles
*/
std::vector<std::string> Database::GetRoles()
{
  std::shared_ptr<sql::SqlConnection > connection(CreateConnection(DATABASE_SECURITY));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_getRoleNative]", sql::SqlCommand::CT_StoredProcedure, connection);

  std::string sQuery = "<role />";

  command->AddParameter(sql::SqlInParameter::Create< std::string >("ID", (LPCTSTR)GetContext(), sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create("QUERY", sQuery.c_str(), sql::ST_VARCHAR));
  command->AddParameter(sql::SqlOutParameter::Create("RESULTS", XML_SIZE, sql::ST_VARCHAR));

  command->Execute();

  std::string results((char *)command->GetParameter("RESULTS")->GetValue());

  std::vector<std::string> roles;

  xml::XmlDocument doc;
  doc.LoadXml(results);

  xml::XmlNodeList list = doc.GetDocumentElement().SelectNodes("role");

  std::for_each(list.begin(), list.end(), [&roles](xml::XmlNode node)
  {
    std::string name;
    node.TryGetAttributeValue<std::string>("name", name);

    roles.push_back(name);
  });

  return roles;

} // GetRoles()


/**
*  set profile
*
*  @param sLogin
*  @param sProfile
*/
void Database::AddUser(CString & sLogin, CString & sPassword, CString & sName, CString & sRole, bool bPasswordChange)
{
  std::shared_ptr<sql::SqlConnection > connection(CreateConnection(DATABASE_SECURITY));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_insertUser]", sql::SqlCommand::CT_StoredProcedure, connection);

  CString sRecord;

  if ( bPasswordChange )
    sRecord.Format("<user login=\"%s\" name=\"%s\" pwd=\"%s\" role=\"%s\" state=\"256\" />", (LPCTSTR)sLogin, (LPCTSTR)sName,  crypto::AESCrypt::Encrypt((LPCTSTR)sPassword).c_str(), (LPCTSTR)sRole);
  else
    sRecord.Format("<user login=\"%s\" name=\"%s\" pwd=\"%s\" role=\"%s\"  />", (LPCTSTR)sLogin, (LPCTSTR)sName,  crypto::AESCrypt::Encrypt((LPCTSTR)sPassword).c_str(), (LPCTSTR)sRole);

  command->AddParameter(sql::SqlInParameter::Create< std::string >("ID", (LPCTSTR)GetContext(), sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create("RECORD", (LPCTSTR)sRecord, sql::ST_VARCHAR));

  command->Execute();
} // SetProfile( sLogin, sProfile )



/**
* Creates the database connection
*
* @param       database            security or storage
* @return      connection handle
*/
std::shared_ptr< sql::SqlConnection > Database::CreateConnection(const std::string & sDatabaseName)
{
  return sql::SqlConnection::Create(sql::DatabaseConnectionStrings::GetConnectionString(sDatabaseName));
} // CreateConnection( database )

