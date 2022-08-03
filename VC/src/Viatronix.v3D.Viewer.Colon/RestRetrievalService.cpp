// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RestRetreivalSystem.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "StdAfx.h"
#include "RestRetrievalService.h"
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
RestRetrievalService::~RestRetrievalService()
{
} // ~RestRetrievalService()


/**
  * gets the launch list
  *
  * @param     sContextId                Context Id
  * @param     sUid                      dataset's UID
  * @param     sApp                      application id
  * @param     bNewSession               true to create new session series; otherwise use existing
  * @return    launch xml
  */
std::string RestRetrievalService::GetLaunchList( const std::string & sContextId, const std::string & sUid, const std::string & sApp, bool bNewSession )
{

  LogDbg(util::Strings::Format("Getting launch list (%s) (%s)", sUid.c_str(), sApp.c_str()), "RestRetrievalService", "GetLaunchList");

  try
  {
    // Get the computer name
    char computerName[512];
    DWORD dwSize(512);
    RtlZeroMemory(computerName,512);
    ::GetComputerName(computerName, &dwSize);

    std::shared_ptr<http::HttpRequest>  request( http::HttpRequest::CreateGet( "launchlist", "get", sContextId) );
    request->AddAttribute("uids",sUid);
    request->AddAttribute("app",sApp);
    request->AddAttribute("sys",computerName);
    request->AddAttribute("new",( bNewSession ? "true" : "false" ));

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("GetLaunchList failed : " + response->GetError(), "RestRetrievalService", "GetLaunchList"));


    std::string sResponse(response->GetResponse());

    xml::XmlDocument doc;

    doc.LoadXml(sResponse);

    xml::XmlNode node = doc.GetDocumentElement().SelectSingleNode("session");

    LogDbg(node.ToXml(),"RestRetrievalService", "GetLaunchList");

    return node.ToXml();
  }
  catch ( ex::VException & ex )
  {
    LogErr( ex.GetLogRecord().GetTextMessage(), "RestRetrievalService", "GetLaunchList");
    throw;
  }
  catch ( ... )
  {
    LogErr("GetLaunchList failed : unspecified error", "RestRetrievalService", "GetLaunchList");
    throw ex::VException(LogRec("GetLaunchList failed : unspecified error", "RestRetrievalService", "GetLaunchList"));

  }

 
} // GetLaunchList( sUID, sApp, bNewSession )


/**
  * gets the dicom header
  *
  * @param      sContextId              Context id
  * @param     sUid                     dataset's UID
  * @return    dicom header xml
  */
std::string RestRetrievalService::GetDicomHeader( const std::string & sContextId, const std::string & sUid )
{

  LogDbg(util::Strings::Format("Getting dicom header (%s)", sUid.c_str()), "RestRetrievalService", "GetDicomHeader");

  try
  {
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateGet( "dicomheader", "get", sContextId));
    request->AddAttribute("uid",sUid);
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("GetDicomHeader failed : " +response->GetError(), "RestRetrievalService", "GetDicomHeader"));

    return response->GetResponse();
  }
  catch ( ex::VException & ex )
  {

    LogErr( ex.GetLogRecord().GetTextMessage(), "RestRetrievalService", "GetDicomHeader");
    throw;
  }
  catch ( ... )
  {
    LogErr("GetDicomHeader failed : unspecified error", "RestRetrievalService", "GetDicomHeader");
    throw ex::VException(LogRec("GetDicomHeader failed : unspecified error", "RestRetrievalService", "GetLaunchList"));

  }

} // GetDicomHeader( sUID )


/**
 * Completes the session
 *
 * @param sContextId      Context id
 * @param sUid            Session Uid
 */
void RestRetrievalService::CompleteSession(const std::string & sContextId, const std::string & sUid)
{
  LogFyi(util::Strings::Format("Completing session (%s)", sUid.c_str()), "RestRetrievalService", "CompleteSession");

  try
  {
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut( "session", "complete", sContextId));
    request->AddAttribute("uid",sUid);
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("CompleteSession failed : " + response->GetError(), "RestRetrievalService", "CompleteSession"));

  }
  catch ( ex::VException & ex )
  {

    LogErr( ex.GetLogRecord().GetTextMessage(), "RestRetrievalService", "CompleteSession");
    throw;
  }
  catch ( ... )
  {
    LogErr("CompleteSession failed : unspecified error", "RestRetrievalService", "CompleteSession");
    throw ex::VException(LogRec("CompleteSession failed : unspecified error", "RestRetrievalService", "CompleteSession"));

  }
  

} // CompleteSession()

/**
 * Completes the session
 *
 * @param sContextId      Context id
 * @param sUid            Session Uid
 */
void RestRetrievalService::CloseSession(const std::string & sContextId, const std::string & sUid)
{
  LogFyi(util::Strings::Format("Closing session (%s)", sUid.c_str()), "RestRetrievalService", "CompleteSession");

  try
  {
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut( "session", "close", sContextId));
    request->AddAttribute("uid",sUid);
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("CloseSession failed : " + response->GetError(), "RestRetrievalService", "CloseSession"));

  }
  catch ( ex::VException & ex )
  {

    LogErr( ex.GetLogRecord().GetTextMessage(), "RestRetrievalService", "CloseSession");
    throw;
  }
  catch ( ... )
  {
    LogErr("CloseSession failed : unspecified error", "RestRetrievalService", "CloseSession");
    throw ex::VException(LogRec("CloseSession failed : unspecified error", "RestRetrievalService", "CloseSession"));

  }

} // CompleteSession()



/**
 * Reopens the session
 *
 * @param sContextId      Context id
 * @param sUid            Session Uid
 */
void RestRetrievalService::ReopenSession(const std::string & sContextId, const std::string & sUid)
{
  LogDbg(util::Strings::Format("Reopening session (%s)", sUid.c_str()), "RestRetrievalService", "ReopenSession");

  try
  {
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut("session", "update", sContextId));
    request->AddAttribute("uid",sUid);
    request->AddAttribute("state", "");
 
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("ReopenSession failed : " + response->GetError(), "RestRetrievalService", "ReopenSession"));
  }
  catch ( ex::VException & ex )
  {

    LogErr( ex.GetLogRecord().GetTextMessage(), "RestRetrievalService", "ReopenSession");
    throw;
  }
  catch ( ... )
  {
    LogErr("ReopenSession failed : unspecified error", "RestRetrievalService", "ReopenSession");
    throw ex::VException(LogRec("ReopenSession failed : unspecified error", "RestRetrievalService", "ReopenSession"));

  }
 

}   // ReopenSession()

