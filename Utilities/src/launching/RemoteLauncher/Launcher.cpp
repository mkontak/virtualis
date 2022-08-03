// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Launcher.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "Launcher.h"
#include "LaunchRequest.h"
#include "http\HttpClient.h"
#include "http\\HttpConnection.h"
#include "http\HttpEnums.h"
#include "http\HttpRequest.h"
#include "http\HttpResponse.h"
#include "Exceptions.h"




/**
 * Launches the request specified
 *
 * @param launchRequest     Launch Request
 * @param session         out   Session UID if launch was sucessful
 */
void Launcher::Launch(LaunchRequest launchRequest, CString & session)
{


  try
  {

    // Create the HTTP request
    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateGet((LPCSTR)launchRequest.GetResource(), "get"));

    // Add th URL attributes
    request->AddAttribute("app", (LPCSTR)launchRequest.GetApplication());
    request->AddAttribute("uids", (LPCSTR)launchRequest.GetUids());
    request->AddAttribute("user", (LPCSTR)launchRequest.GetUserName());
    request->AddAttribute("pwd", (LPCSTR)launchRequest.GetPassword());
    request->AddAttribute("cid", (LPCSTR)launchRequest.GetContext());
    request->AddAttribute("desc", (LPCSTR)launchRequest.GetDescription());
    request->AddAttribute("new", (launchRequest.GetNewSession() ? "1" : "0"));


    // Create the HTTP client
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url((LPCSTR)launchRequest.GetUrlBase())));

    // Perform GET request
    std::shared_ptr<http::HttpResponse> response(client->Send(request));


    // Interpret response
    xml::XmlDocument doc;
    doc.LoadXml(response->GetResponse());

    // If the request failed then raise error
    if (response->IsFailed())
      throw LaunchException((doc.GetDocumentElement().HasChildren() ? doc.GetDocumentElement().GetFirstChild().GetContent().c_str() : "Failed to launch viewer"));
    
    // If success then extract the session UID
    xml::XmlNode node = doc.GetDocumentElement().SelectSingleNode("session");
    session = node.GetFirstChild().GetContent().c_str();
    
  }
  catch (LaunchException)
  {
    throw;
  }
  catch (ex::VException & ex)
  {
    throw LaunchException( ex.GetLogRecord().GetTextMessage().c_str());
  }
  catch (...)
  {
    throw LaunchException("unspecified error");
  }

} // Launch(LaunchRequest launchRequest, CString & session)