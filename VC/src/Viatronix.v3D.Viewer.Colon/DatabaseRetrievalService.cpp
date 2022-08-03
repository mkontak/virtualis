// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseRetrievalService.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "DatabaseRetrievalService.h"
#include "Database.h"
#include "User.h"


/**
 * destructor
 */
DatabaseRetrievalService::~DatabaseRetrievalService()
{
} // ~DatabaseRetrievalService()


/**
  * gets the launch list
  *
  * @param     sContextId                Context Id
  * @param     sUid                      dataset's UID
  * @param     sApp                      application id
  * @param     bNewSession               true to create new session series; otherwise use existing
  * @return    launch xml
  */
std::string DatabaseRetrievalService::GetLaunchList( const std::string & sContextId, const std::string & sUid, const std::string & sApp, bool bNewSession )
{
  return Database::GetLaunchList( sContextId, sUid, sApp, User::GetCurrentUser().GetLogin(), bNewSession );
} // GetLaunchList( sUid, sApp, bNewSession )


/**
  * gets the dicom header
  *
  * @param     sContextId                Context Id
  * @param     sUid                      dataset's UID
  * @return    dicom header xml
  */
std::string DatabaseRetrievalService::GetDicomHeader( const std::string & sContextId, const std::string & sUid )
{
  return Database::GetDicomHeader( sContextId, sUid );
} // GetDicomHeader( sUID )

/**
 * Completes the session
 *
 * @param sContextId      Context id
 * @param sUid            Session Uid
 */
void DatabaseRetrievalService::CompleteSession(const std::string & sContextId, const std::string & sUid)
{
  Database::CompleteSession(sContextId, sUid);
} // CompleteSession()

/**
* Close the session
*
* @param sContextId      Context id
* @param sUid            Session Uid
*/
void DatabaseRetrievalService::CloseSession(const std::string & sContextId, const std::string & sUid)
{
  Database::CloseSession(sContextId, sUid);
} // CloseSession()

/**
 * Reopens the session
 *
 * @param sContextId      Context id
 * @param sUid            Session Uid
 */
void DatabaseRetrievalService::ReopenSession(const std::string & sContextId, const std::string & sUid)
{
  Database::ReopenSession(sContextId, sUid);
}   // ReopenSession()

