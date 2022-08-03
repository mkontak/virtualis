// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RestConfigurationService.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#include "StdAfx.h"
#include "RestConfigurationService.h"
#include "User.h"
#include "VCSettings.h"

#include "vxUrl.h"
#include "vxBuffer.h"
#include "http\HttpClient.h"
#include "http\HttpRequest.h"
#include "http\HttpResponse.h"

/**
 * destructor
 */
RestConfigurationService::~RestConfigurationService()
{
} // ~RestConfigurationService()


/** 
  * get Profile
  *
  * @param     sProfile            Profile name 
  * @return    xml
  */
std::string RestConfigurationService::GetConfiguration( const std::string & sContextId, const std::string & sConfigName )
{

  try
  {

    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateGet( "systemconfig", "get", sContextId));

    request->AddAttribute("name",sConfigName);

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("GetConfiguration failed : " + response->GetError(), "RestConfigurationService", "GetConfiguration"));

    return response->GetResponse();

   
  }
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "GetConfiguration");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("GetConfiguration failed : unspecified error", "RestConfigurationService", "GetConfiguration"));
  }

} // GetProfile( sConfigName )


/**
  * Sets the Profile
  *
  * @param       sXMl                    xml
  */
void RestConfigurationService::SetConfiguration( const std::string & sContextId, const std::string & sXml )
{
  try
  {
    // extract settings from Profile file
    vxDOMDocument doc( vxDOMDocument::FromXml( sXml ));
    std::string sConfigName( doc.GetDocumentElement().GetNodeName() );

    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut("systemconfig", "insert", sContextId));

    request->AddAttribute("name",sConfigName);
    request->AddAttribute("config",sXml);

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("SetConfiguration failed : " + response->GetError(), "RestConfigurationService", "SetConfiguration"));

   
  }  
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "SetConfiguration");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("SetConfiguration failed : unspecified error", "RestConfigurationService", "SetConfiguration"));
  }
} // SetProfile( sXml )


/**
  * Deletes the Profile
  *
  * @param     sConfigName               Profile name
  */
void RestConfigurationService::DeleteConfiguration( const std::string & sContextId, const std::string & sConfigName )
{
  try
  {

    // Deletion of the translucency is not supported
    if ( sConfigName == CONFIGURATION_TRANSLUCENCY )
      return;

    std::shared_ptr<http::HttpRequest>  request( http::HttpRequest::CreateDelete("systemconfig", "delete", sContextId) );

    request->AddAttribute("name",sConfigName);
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("DeleteConfiguration failed : " + response->GetError(), "RestConfigurationService", "DeleteConfiguration"));

   
  }  
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "DeleteConfiguration");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("DeleteConfiguration failed : unspecified error", "RestConfigurationService", "DeleteConfiguration"));
  }
} // DeleteProfile( sConfigName )



/** 
  * get Profile
  *
  * @param     sProfile            Profile name 
  * @return    xml
  */
std::string RestConfigurationService::GetUserProfile( const std::string & sContextId, const std::string & sConfigName )
{
  try
  {
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateGet("userprofile", "get", sContextId));
    request->AddAttribute("name",sConfigName);
    request->AddAttribute("id",User::GetCurrentUser().GetLogin());

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("GetUserProfile failed : " + response->GetError(), "RestConfigurationService", "GetUserProfile"));

    return response->GetResponse();
   
  }  
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "GetUserProfile");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("GetUserProfile failed : unspecified error", "RestConfigurationService", "GetUserProfile"));
  }


} // GetProfile( sConfigName )


/**
  * Sets the Profile
  *
  * @param       sXMl                    xml
  */
void RestConfigurationService::SetUserProfile( const std::string & sContextId, const std::string & sXml )
{
  try
  {
    // extract settings from Profile file
    vxDOMDocument doc( vxDOMDocument::FromXml( sXml ));
    std::string sConfigName( doc.GetDocumentElement().GetNodeName() );

    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut( "userprofile", "add", sContextId));

    request->AddAttribute("name",sConfigName);
    request->AddAttribute("login", User::GetCurrentUser().GetLogin());

     request->AddAttribute("cfg",sXml);

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("SetUserProfile failed : " + response->GetError(), "RestConfigurationService", "SetUserProfile"));

   
  }  
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "SetUserProfile");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("SetUserProfile failed : unspecified error", "RestConfigurationService", "SetUserProfile") );
  }
} // SetProfile( sXml )


/**
  * Deletes the Profile
  *
  * @param     sConfigName               Profile name
  */
void RestConfigurationService::DeleteUserProfile( const std::string & sContextId, const std::string & sConfigName )
{
  try
  {

    // Deletion of the translucency is not supported
    if ( sConfigName == CONFIGURATION_TRANSLUCENCY )
      return;

    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateDelete( "userprofile", "delete", sContextId));

    request->AddAttribute("name",sConfigName);
    request->AddAttribute("login", User::GetCurrentUser().GetLogin());
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("DeleteUserProfile failed : " + response->GetError(), "RestConfigurationService", "DeleteUserProfile"));

  }  
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestConfigurationService", "DeleteUserProfile");
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec("DeleteUserProfile failed : unspecified error", "RestConfigurationService", "DeleteUserProfile"));
  }
} // DeleteProfile( sConfigName )