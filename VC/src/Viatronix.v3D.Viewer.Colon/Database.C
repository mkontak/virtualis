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

#include "Exception.h"
#include "sql\SqlCommand.h"
#include "sql\SqlError.h"
#include "sql\SqlParameter.h"
#include "sql\SqlHandle.h"
#include "sql\DatabaseConnectionStrings.h"
#include "crypto\AESCrypt.h"
#include "SystemInfo.h"

#include "VCSettings.h"
#include "User.h"

#define XML_SIZE 32768
#define DATABASE_SECURITY "security"
#define DATABASE_STORAGE "storage"
#define DATABASE_SYSTEM "system"


/**
 * Acquires a context for the specified parameters. The newly acquired context will be returned in the sContextId parameters.
 *
 * @param sContextId          In/Out  Context Id
 * @paarm user                In      User object
 * @param sPassword           IN      Users password
 */
void Database::AcquireContext( std::string & sContextId, User & user, const std::string & sPassword )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));

  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_acquireContext]", sql::SqlCommand::CT_StoredProcedure, connection);

  command->AddParameter(sql::SqlInParameter::Create<std::string>("LOGIN",user.GetLogin(),sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create<std::string>("PASSWORD",sPassword,sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create<std::string>("NEW_PASSWORD", "", sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInOutParameter::Create( "CONTEXT_ID", sContextId, 256, sql::ST_VARCHAR ));

  command->Execute();

  if ( sContextId.empty() )
    sContextId.assign((char *)command->GetParameter("CONTEXT_ID")->GetValue());

  /// Gets the user information
  GetUser(sContextId, user);


} // AcquireContext(const std::string & sConnectString, const std::string & sUser, const std::string & sPassword, std::string & sContextId)


/** 
 * Releases the Context specified
 *
 * @param sCOnnectionString   In      Connection String
 * @param sContextId          In      Context to be released
 */
void Database::ReleaseContext( const std::string & sContextId )
{
  
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));

  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_releaseContext]", sql::SqlCommand::CT_StoredProcedure, connection);

  command->AddParameter(sql::SqlInParameter::Create("ID",sContextId.c_str(),sql::ST_VARCHAR));

  command->Execute();

} // ReleaseContext(const std::string & sConnectString, const std::string & sContextId)


/** 
 * Renews the Context specified
 *
 * @param sCOnnectionString   In      Connection String
 * @param sContextId          In      Context to be released
 */
void Database::RenewContext( const std::string & sContextId )
{
  
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));

  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_renewContext]", sql::SqlCommand::CT_StoredProcedure, connection);

  command->AddParameter(sql::SqlInParameter::Create("ID",sContextId.c_str(),sql::ST_VARCHAR));

  command->Execute();

} // RenewContext(const std::string & sConnectString, const std::string & sContextId)


/**
 * get profile
 *
 *  @param sLogin
 *  @return the login string
 */
std::string Database::GetConfiguration( const std::string & sContextId, const std::string & sName )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SYSTEM ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getSystemConfigNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  std::string sQuery = "<systemconfig name=\"" + sName + "\" />";

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "QUERY", sQuery.c_str(), sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlOutParameter::Create( "RESULTS", XML_SIZE, sql::ST_VARCHAR ));

  command->Execute();

  return (char *) command->GetParameter( "RESULTS" )->GetValue();
} // GetProfile( sLogin )


/**
 *  set profile
 *
 *  @param sLogin
 *  @param sProfile
 */
void Database::SetConfiguration( const std::string & sContextId,  const std::string & sName, const std::string & sProfile )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SYSTEM ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_insertSystemConfig]", sql::SqlCommand::CT_StoredProcedure, connection );

 
  std::string sRecord = "<systemconfig  name=\"" + sName + "\" ><config>" + sProfile + "</config></systemconfig>";

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "RECORD", sRecord.c_str(), sql::ST_VARCHAR ));

  command->Execute();
} // SetProfile( sLogin, sProfile )


/**
 *  Deletes a configuration
 *
 *  @param sLogin
 */
void Database::DeleteConfiguration( const std::string & sContextId, const std::string & sName )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SYSTEM ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_deleteSystemConfig]", sql::SqlCommand::CT_StoredProcedure, connection );

 
  std::string sRecord = "<systemconfig name=\"" + sName + "\" />";
  
  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "RECORD", sRecord.c_str(), sql::ST_VARCHAR ));

  command->Execute();
} // DeleteProfile( sLogin )



/**
 *  get launch list
 *
 *  @param sUID
 *  @param sApp
 *  @param sLogin
 *  @param bNewSession
 *  @return string of the list
 */
std::string Database::GetLaunchList( const std::string & sContextId, const std::string & sUID, const std::string & sApp, const std::string & sLogin, bool bNewSession )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_STORAGE ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getLaunchListNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  std::string computerName = SystemInfo::GetNetBiosComputerName();

  std::string query = util::Strings::Format("<launchlist uids=\"%s\" login=\"%s\" app=\"%s\" new=\"%d\" sys=\"%s\" desc=\"\" />", sUID.c_str(), sLogin.c_str(), sApp.c_str(), (bNewSession ? 1 : 0), computerName.c_str());


  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter(sql::SqlInParameter::Create< std::string >("QUERY", query, sql::ST_VARCHAR));
  command->AddParameter( sql::SqlOutParameter::Create( "RESULTS", XML_SIZE, sql::ST_VARCHAR ));
  
  command->Execute();

  return (char *) command->GetParameter( "RESULTS" )->GetValue();
} // GetLaunchList( sUID, sApp )


/**
 * Retrieves the DICOM header from the database for the UID specified.
 *
 * @param sUID      UID to get the dicom for. This does not have to be a dicom uid.
 *
 * @return  Dicom header
 */
std::string Database::GetDicomHeader( const std::string & sContextId, const std::string & sUID )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_STORAGE ));
  connection->Open();

  std::string query = util::Strings::Format("<dicomheader uid=\"%s\" />", sUID.c_str());

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getDicomHeaderNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create< std::string >( "QUERY", query, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlOutParameter::Create( "RESULTS", XML_SIZE, sql::ST_VARCHAR ));
  
  command->Execute();

  return (char *) command->GetParameter( "RESULTS" )->GetValue();

} // GetLaunchList( sUID, sApp )GetDicomHeader( const std::string & sUID )


/**
 * Creates the database connection
 *
 * @param       database            security or storage
 * @return      connection handle
 */
std::shared_ptr< sql::SqlConnection > Database::CreateConnection( const std::string & sDatabaseName )
{
   return sql::SqlConnection::Create( sql::DatabaseConnectionStrings::GetConnectionString( sDatabaseName ));
} // CreateConnection( database )


/**
 * Gets the user object
 *
 * @param user    User object to fill in
 *
 */
void Database::GetUser( const std::string & sContextId, User & user )
{


  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
  connection->Open();

  
  
  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getUserNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  std::string query = util::Strings::Format("<user login=\"%s\" />", user.GetLogin().c_str());

  command->AddParameter(sql::SqlInParameter::Create<std::string>("ID", sContextId, sql::ST_VARCHAR));
  command->AddParameter( sql::SqlInParameter::Create<std::string>( "QUERY", query, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlOutParameter::Create( "RESULTS", XML_SIZE, sql::ST_VARCHAR ));
  
  command->Execute();

  try
  {
    xml::XmlDocument document;
    document.LoadXml((char *) command->GetParameter( "RESULTS" )->GetValue() );
    xml::XmlNode node(document.GetDocumentElement());

    std::vector<xml::XmlNode> users = node.SelectNodes("user" );

    if ( users.size() > 0 )
    {
      std::string sValue("");
      users[0].TryGetAttributeValue<std::string>("name", sValue);
      user.SetFullName(sValue);


      int perms(0);
      users[0].TryGetAttributeValue<int>("perm", perms);
      user.SetRightsMask(perms);

      
    } // END ... If there are users
  } // try
  catch ( ... )
  {
    LogErr("Failed to get user information", "Database", "GetUser");
  }

  std::shared_ptr<sql::SqlCommand> rightsCommand = sql::SqlCommand::Create( "[v3d].[sp_getRightNative]", sql::SqlCommand::CT_StoredProcedure, connection );
  
  rightsCommand->AddParameter( sql::SqlInParameter::Create( "ID", sContextId, sql::ST_VARCHAR ));
  rightsCommand->AddParameter(sql::SqlInParameter::Create("QUERY", "<right />", sql::ST_VARCHAR));
  rightsCommand->AddParameter(sql::SqlOutParameter::Create("RESULTS", XML_SIZE, sql::ST_VARCHAR));

  rightsCommand->Execute();

  try
  {
    xml::XmlDocument document;
    document.LoadXml((char *) rightsCommand->GetParameter( "RESULTS" )->GetValue() );
    xml::XmlNode node(document.GetDocumentElement());

    std::vector<xml::XmlNode> rights = node.SelectNodes("right" );

    std::string sValue("");

    std::for_each(rights.begin(), rights.end(), [ &user ] ( xml::XmlNode node ) 
    {

      std::string sName("");
      int4 iMask(0);
      node.TryGetAttributeValue<std::string>("name",sName);
      node.TryGetAttributeValue<int4>("mask",iMask);
      user.AddRight(tolower(sName), iMask);
     
    } );




  } // try
  catch ( ... )
  {
    LogErr("Failed to get user information", "Database", "GetUser");
  }


} // GetUser( User & user)


/**
 * Gets the user rights
 *
 * @param user    User object to fill in
 *
 */
void Database::GetRights( const std::string & sContextId, User & user )
{


  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getRightsNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  command->AddParameter(sql::SqlInParameter::Create("ID", sContextId, sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create("QUERY", "<right />", sql::ST_VARCHAR));
  command->AddParameter(sql::SqlOutParameter::Create("RESULTS", XML_SIZE, sql::ST_VARCHAR));

  command->Execute();

  try
  {
    xml::XmlDocument document;
    document.LoadXml((char *) command->GetParameter( "RESULTS" )->GetValue() );
    xml::XmlNode node(document.GetDocumentElement());

    std::vector<xml::XmlNode> rights = node.SelectNodes("right" );

    std::string sValue("");

    std::for_each(rights.begin(), rights.end(), [ &user ] ( xml::XmlNode node ) 
    {

      std::string sName("");
      int4 iMask(0);
      node.TryGetAttributeValue<std::string>("name",sName);
      node.TryGetAttributeValue<int4>("mask",iMask);
      user.AddRight(tolower(sName), iMask);
     
    } );




  } // try
  catch ( ... )
  {
    LogErr("Failed to get user information", "Database", "GetUser");
  }


} // GetUser( User & user)




/**
 * Completed the session.
 *
 * @param sSessionUid      Session uid to be completed.
 *
 */
void Database::CompleteSession( const std::string & sContextId, const std::string & sSessionUid )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_STORAGE ));
  connection->Open();

  std::string sRecord = "<session uid=\"" + sSessionUid + "\" state=\"Completed\" />";


  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_updateSession]", sql::SqlCommand::CT_StoredProcedure, connection );

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create< std::string >( "RECORD", sRecord, sql::ST_VARCHAR ));
 
  command->Execute();
} // CompleteSession( const std::string & sSessionUid )


/**
* Completed the session.
*
* @param sSessionUid      Session uid to be completed.
*
*/
void Database::CloseSession(const std::string & sContextId, const std::string & sSessionUid)
{
  std::shared_ptr<sql::SqlConnection > connection(CreateConnection(DATABASE_STORAGE));
  connection->Open();


  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_closeSession]", sql::SqlCommand::CT_StoredProcedure, connection);

  command->AddParameter(sql::SqlInParameter::Create< std::string >("ID", sContextId, sql::ST_VARCHAR));
  command->AddParameter(sql::SqlInParameter::Create< std::string >("UID", sSessionUid, sql::ST_VARCHAR));



  command->Execute();
} // CompleteSession( const std::string & sSessionUid )


/**
 * Reopens the session.
 *
 * @param sSessionUid      Session uid to be reopened
 *
 */
void Database::ReopenSession( const std::string & sContextId, const std::string & sSessionUid )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_STORAGE ));
  connection->Open();

  std::string sRecord = "<session uid=\"" + sSessionUid + "\" state=\"\" />";


  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_updateSession]", sql::SqlCommand::CT_StoredProcedure, connection );

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create< std::string >( "RECORD", sRecord, sql::ST_VARCHAR ));
  
  command->Execute();
} // ReopenSession( const std::string & sSessionUid )

/**
 * get profile
 *
 *  @param sLogin
 *  @return the login string
 */
std::string Database::GetUserProfile( const std::string & sContextId, const std::string & sLogin, const std::string & sName )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_getUserProfileNative]", sql::SqlCommand::CT_StoredProcedure, connection );

  std::string sQuery = "<userprofile login=\"" + sLogin + "\" name=\"" + sName + "\" />";

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "QUERY", sQuery, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlOutParameter::Create( "RESULTS", XML_SIZE, sql::ST_VARCHAR ));

  command->Execute();

  return (char *) command->GetParameter( "RESULTS" )->GetValue();
} // GetProfile( sLogin )


/**
 *  set profile
 *
 *  @param sLogin
 *  @param sProfile
 */
void Database::SetUserProfile( const std::string & sContextId,  const std::string & sLogin, const std::string & sName, const std::string & sProfile )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_insertUserProfile]", sql::SqlCommand::CT_StoredProcedure, connection );

 
  std::string sRecord = "<userprofile  login=\"" + sLogin + "\" name=\"" + sName + "\" ><profile>" + sProfile + "</profile></userprofile>";

  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "RECORD", sRecord, sql::ST_VARCHAR ));

  command->Execute();
} // SetProfile( sLogin, sProfile )


/**
 *  Deletes a configuration
 *
 *  @param sLogin
 */
void Database::DeleteUserProfile( const std::string & sContextId, const std::string & sLogin, const std::string & sName )
{
  std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
  connection->Open();

  std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_deleteUserProfile]", sql::SqlCommand::CT_StoredProcedure, connection );

 
  std::string sRecord = "<userprofile login=\"" + sLogin + "\" name=\"" + sName + "\" />";
  
  command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sContextId, sql::ST_VARCHAR ));
  command->AddParameter( sql::SqlInParameter::Create( "RECORD", sRecord, sql::ST_VARCHAR ));

  command->Execute();
} // DeleteProfile( sLogin )


/**
 * Authorizes the user
 *
 * @param sId       Context
 * @param sLogin    Login
 * @param sPassword Password
 */
void Database::Authenticate( const std::string & sId, const std::string & sLogin, const std::string & sPasswword )
{

  try
  {
    std::shared_ptr<sql::SqlConnection > connection( CreateConnection( DATABASE_SECURITY ));
    connection->Open();

    std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create( "[v3d].[sp_authenticate]", sql::SqlCommand::CT_StoredProcedure, connection );

    command->AddParameter( sql::SqlInParameter::Create< std::string >( "ID", sId, sql::ST_VARCHAR ));
    command->AddParameter( sql::SqlInParameter::Create< std::string >( "LOGIN", sLogin, sql::ST_VARCHAR ));
    command->AddParameter( sql::SqlInParameter::Create< std::string >( "PASSWORD", sPasswword, sql::ST_VARCHAR ));


 
    command->Execute();
  }
  catch ( ex::SqlException & ex )
  {

    LogErr("Failed to authorize user " + sLogin + " : " + ex.GetLogRecord().GetTextMessage(), "Database","Authenticate");


    throw;

  }

} // Authorize( const std::string & sId, const std::string & sLogin, const std::string & sPasswword )