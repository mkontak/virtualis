// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseLoginService.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "Exception.h"
#include "DatabaseLoginService.h"
#include "Database.h"
#include "User.h"


/**
 * destructor
 */
DatabaseLoginService::~DatabaseLoginService()
{
} // ~DatabaseLoginService()


/**
 * Acquires a context for the specified parameters. The newly acquired context will be returned in the sContextId parameters.
 *
 * @param sUser               In      User name
 * @param sConnectId          In      Context Id
 * @param sPassword           IN      Users password
 * @return                    context id
 */
std::string DatabaseLoginService::Login( const std::string & sUser, const std::string & sContextId, const std::string & sPassword )
{
  std::string id( sContextId );
	std::string sUserFullName("");

  User::GetCurrentUser().SetLogin(sUser);
  Database::AcquireContext( id, User::GetCurrentUser(), sPassword );


  return id;
} // Login( sUser, sContextId, sPassword )


/** 
 * Releases the Context specified
 */
void DatabaseLoginService::Logout( const std::string & sContextId )
{
  if( !sContextId.empty() )
    Database::ReleaseContext( sContextId );
} // Logout()


/**
  * Renew context
  */
void DatabaseLoginService::Renew( const std::string & sContextId )
{
  if( !sContextId.empty() )
    Database::RenewContext( sContextId );
} // Renew()


/**
  * Renew context
  */
void DatabaseLoginService::Authenticate(  const std::string & sContextId, const std::string & sUser, const std::string & sPassword  )
{
   Database::Authenticate( sContextId, sUser, sPassword );
} // Renew()