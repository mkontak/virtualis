// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
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
  static void AcquireContext( std::string & sContextId, User & user, const std::string & sPassword = "");

  /// Release the context
  static void ReleaseContext( const std::string & sContextId );

  /// Renew context
  static void RenewContext( const std::string & sContextId );


  /// Gets the user profile
  static std::string GetConfiguration( const std::string & sContextId,  const std::string & sName );

 
  /// set profile
  static void SetConfiguration( const std::string & sContextId,  const std::string & sName, const std::string & sConfigXml );

  /// delete profile
  static void DeleteConfiguration( const std::string & sContextId,  const std::string & sName );

   /// Gets the user profile
  static std::string GetUserProfile( const std::string & sContextId, const std::string & sLogin, const std::string & sName );

 
  /// set profile
  static void SetUserProfile( const std::string & sContextId, const std::string & sLogin, const std::string & sName, const std::string & sProfile );

  /// delete profile
  static void DeleteUserProfile( const std::string & sContextId, const std::string & sLogin, const std::string & sName );


  /// gets the launch list
  static std::string GetLaunchList( const std::string & sContextId, const std::string & sUID, const std::string & sApp, const std::string & sLogin, bool bNewSession );

  /// gets the dicom header
  static std::string GetDicomHeader( const std::string & sContextId, const std::string & sUID );

  /// Completed the session
  static void CompleteSession( const std::string & sContextId, const std::string & sSessionUid );

  /// Closes the session
  static void CloseSession(const std::string & sContextId, const std::string & sSessionUid);

  /// Reopen the session
  static void ReopenSession( const std::string & sContextId, const std::string & sSessionUid );

  /// Re-Authenticate
  static void Authenticate( const std::string & sContextId, const std::string & sLogin, const std::string & sPasswword );


private:

    /// Gets the user information
  static void GetUser( const std::string & sContextId,User & user );

  /// Gets the user rights
  static void GetRights( const std::string & sContextId,User & user );

private:
	/// create connection
  static std::shared_ptr< sql::SqlConnection > CreateConnection( const std::string & sDatabaseName );

};  // class Database

