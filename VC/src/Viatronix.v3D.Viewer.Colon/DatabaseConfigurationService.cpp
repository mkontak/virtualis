// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseConfigurationService.C
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "DatabaseConfigurationService.h"
#include "Database.h"
#include "User.h"


/**
 * destructor
 */
DatabaseConfigurationService::~DatabaseConfigurationService()
{
} // ~DatabaseConfigurationService()


/** 
  * get Profile
  *
  * @param     sProfile            Profile name 
  * @return    xml
  */
std::string DatabaseConfigurationService::GetConfiguration( const std::string & sContextId, const std::string & sConfigName )
{
  if ( sConfigName == CONFIGURATION_TRANSLUCENCY || sConfigName == CONFIGURATION_USER )
   return Database::GetUserProfile( sContextId,  User::GetCurrentUser().GetLogin(), sConfigName );
  else    
   return Database::GetConfiguration( sContextId,  sConfigName );
} // GetProfile( sConfigName )


/**
  * Sets the Profile
  *
  * @param       sXMl                    xml
  */
void DatabaseConfigurationService::SetConfiguration( const std::string & sContextId, const std::string & sXml )
{
  try
  {
    // extract settings from Profile file
    vxDOMDocument doc( vxDOMDocument::FromXml( sXml ));
    std::string sConfigName( doc.GetDocumentElement().GetNodeName() );


    if ( sConfigName == CONFIGURATION_TRANSLUCENCY || sConfigName == CONFIGURATION_USER )
      Database::SetUserProfile( sContextId,  User::GetCurrentUser().GetLogin(), sConfigName, sXml );
    else    
      Database::SetConfiguration( sContextId,  sConfigName,  sXml );
  }
  catch( ex::VException & e )
  {
    LogErr( e.GetLogRecord().GetTextMessage(), "DatabaseConfigurationService", "SetConfiguration" );
  }

} // SetProfile( sXml )


/**
  * Deletes the Profile
  *
  * @param     sConfigName               Profile name
  */
void DatabaseConfigurationService::DeleteConfiguration( const std::string & sContextId, const std::string & sConfigName )
{
  try
  {
    if( sConfigName == CONFIGURATION_USER  )
      Database::DeleteUserProfile( sContextId, User::GetCurrentUser().GetLogin(), sConfigName );
    else if  ( sConfigName != CONFIGURATION_TRANSLUCENCY ) 
      Database::DeleteConfiguration( sContextId, sConfigName );
  }
  catch( ex::VException & e )
  {
    LogErr( e.GetLogRecord().GetTextMessage(), "DatabaseConfigurationService", "DeleteConfiguration" );
  }

} // DeleteProfile( sConfigName )


