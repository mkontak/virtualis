
#include "StdAfx.h"
#include "Exception.h"
#include "RestLoginService.h"
#include "User.h"
#include "VCSettings.h"
#include "crypto\AESCrypt.h"

#include "vxUrl.h"
#include "vxBuffer.h"
#include "http\HttpClient.h"
#include "http\HttpRequest.h"
#include "http\HttpResponse.h"



/**
 * destructor
 */
RestLoginService::~RestLoginService()
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
std::string RestLoginService::Login( const std::string & sUser, const std::string & sContextId, const std::string & sPassword )
{
 
  try
  {
    LogDbg(util::Strings::Format("Attempting the login (%s) (%s)", sUser.c_str(), sContextId.c_str()), "RestLoginService", "Login");

    std::shared_ptr<http::HttpRequest>  request = http::HttpRequest::CreatePost( "context", "acquire");
    request->AddAttribute("login", sUser);
    request->AddAttribute("pwd", crypto::AESCrypt::Encrypt(sPassword));
    request->AddAttribute("id", sContextId);
 

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("Login failed : " + response->GetError(), "RestLoginService", "Login"));

    std::string context;

    response->TryGetAttribute("id",context);

    return sContextId;
  }
  catch ( ex::VException & ex )
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "RestLoginService", "Login");
    throw;
  }
  catch ( ... )
  {
    LogErr("Login failed : unspecified error", "RestLoginService", "Login");
    throw ex::VException(LogRec("Login failed : unspecified error", "RestLoginService", "Login"));
  }

} // Login( sUser, sContextId, sPassword )


/** 
 * Releases the Context specified
 */
void RestLoginService::Logout( const std::string & sContextId )
{

  try
  {
    // ===============================================
    // Only logout if the context id is not empty
    // ==============================================
    if( !sContextId.empty() )
    {

      LogDbg(util::Strings::Format("Attempting to release context %s", sContextId.c_str()), "RestLoginService", "Logout");

      std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreateDelete("context", "delete", sContextId));
      request->AddAttribute("id", sContextId);

      std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice" ))));    
      std::shared_ptr<http::HttpResponse> response(client->Send(request));

      if ( response->IsFailed() )
        throw ex::HttpException(LogRec("Logout failed : " + response->GetError(), "RestLoginService", "Logout"));

    }
  }
  catch ( ex::VException & ex )
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "RestLogoutService", "Logout");
    throw;
  }
  catch ( ... )
  {
    LogErr("Logout failed : unspecified error", "RestLoginService", "Logout");
    throw ex::VException(LogRec("Logout failed : unspecified error", "RestLoginService", "Logout"));
  }

   
} // Logout()


/**
  * Renew context
  */
void RestLoginService::Renew( const std::string & sContextId )
{
  try
  {
    // ========================================
    // Only renew if the context is not empty
    // =======================================
    if( !sContextId.empty() )
    {
      LogDbg(util::Strings::Format("Attempting to renew context %s", sContextId.c_str()), "RestLoginService", "Logout");

      std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePut("context", "renew", sContextId));
      request->AddAttribute("id",sContextId);
 
      std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice"))));   
      std::shared_ptr<http::HttpResponse> response(client->Send(request));

      if ( response->IsFailed() )
        throw ex::HttpException(LogRec("Renew failed : " + response->GetError(), "RestLoginService", "Renew"));

 
    }

  }
  catch ( ex::VException & ex )
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "RestLogoutService", "Renew");
    throw;
  }
    catch ( ... )
  {
    LogErr("Renew failed : unspecified error", "RestLoginService", "Renew");
    throw ex::VException(LogRec("Renew failed : unspecified error", "RestLoginService", "Renew"));
  }

} // Renew( const std::string & sContextId )



/**
 * Acquires a context for the specified parameters. The newly acquired context will be returned in the sContextId parameters.
 *
 * @param sConnectId          In      Context Id
 * @param sUser               In      User name
 * @param sPassword           IN      Users password
 * @return                    context id
 */
void RestLoginService::Authenticate(  const std::string & sContextId, const std::string & sUser, const std::string & sPassword )
{
 
  try
  {
    LogDbg(util::Strings::Format("Attempting the login (%s) (%s)", sUser.c_str(), sContextId.c_str()), "RestLoginService", "Authenticate");

    std::shared_ptr<http::HttpRequest>  request(http::HttpRequest::CreatePost("context", "authenticate"));
    request->AddAttribute("login", sUser);
    request->AddAttribute("pwd", crypto::AESCrypt::Encrypt(sPassword));
    request->AddAttribute("id", sContextId);
 

    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create(Url(VCSettings::GetInstance().GetValue( "webservice" ))));    
    std::shared_ptr<http::HttpResponse> response(client->Send(request));

    if ( response->IsFailed() )
      throw ex::HttpException(LogRec("Authentication failed : " + response->GetError(), "RestLoginService", "Authenticate"));

  }
  catch ( ex::VException & ex )
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "RestLoginService", "Authenticate");
    throw;
  }
  catch ( ... )
  {
    LogErr("Authentication failed : unspecified error", "RestLoginService", "Authenticate");
    throw ex::VException(LogRec("Authentication failed : unspecified error", "RestLoginService", "Authenticate"));
  }

} // Login( sUser, sContextId, sPassword )
