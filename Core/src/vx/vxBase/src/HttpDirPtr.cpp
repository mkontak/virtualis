#include "stdafx.h"
#include "HttpDirPtr.h"
#include "vxFileServer.h"
#include "http\HttpClient.h"
#include "http\HttpRequest.h"
#include "http\HttpResponse.h"
#include "FileServerHeader.h"
#include "vxBuffer.h"
#include "Base64.h"
#include "HttpFilePtr.h"


using namespace io;

/// static initialization
std::string HttpDirPtr::m_sResource = "fileserver";

/**
 * Constructor 
 *
 * @param sFilePath     File to be opened
 */
HttpDirPtr::HttpDirPtr(const std::string & sPath) :
DirPtr(sPath)
{
 

}


/**
 * Destructor
 */
HttpDirPtr::~HttpDirPtr()
{

}

/**
 * Creates the HttpDirPtr object
 *
 * @param sFilePath     File path to open
 */
std::shared_ptr<HttpDirPtr> HttpDirPtr::Create(const std::string & sPath)
{
  std::shared_ptr<HttpDirPtr> dirPtr = std::shared_ptr<HttpDirPtr>(new HttpDirPtr(sPath));

  if ( dirPtr == __nullptr ) 
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpDirPtr","HttpDirPtr","Create"));


  return dirPtr;
} // Create()




/**
 * Gets the list of files from the path
 *
 * @param sPattern      Pattern to match
 *
 * @return list of files
 */
std::vector<std::string> HttpDirPtr::GetFiles( const std::string & sPattern)
{
  std::vector<std::string> files;


  if ( ! m_sPath.empty() )
  {

    std::string message          = m_sPath + "|" + sPattern;

    // Cretae a local client
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create());

    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE + message.size()));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_GETFILES;
    pRequestHeader->Size         = message.size(); 

    memcpy(request->GetData() + FILESERVERHEADER_SIZE, message.c_str(), message.size());


 
    std::shared_ptr<Buffer> response(client->Send(request, "POST", "fileserver" ));
 

    HttpFilePtr::ValidateResponse(request, response, m_sPath);

    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

    std::string sMessage( (char *)response->GetData(FILESERVERHEADER_SIZE),  response->GetLength() - FILESERVERHEADER_SIZE);

    // ==================================================
    // Only perform split if there is files in the list
    // ==================================================
    if ( pRequestHeader->Size > 0 )
      files = util::Strings::Split(sMessage, '|');
  }


  /// If the status is OK the file exists
  return files;
  

} // IsExists()



/**
 * Gets the list of sub-dir's from the path
 *
 * @param sPattern      Pattern to match
 *
 * @return list of sub-directories
 */
std::vector<std::string> HttpDirPtr::GetDirs( const std::string & sPattern)
{

  std::vector<std::string> dirs;


  if ( ! m_sPath.empty() )
  {

    std::string message          = m_sPath + "|" + sPattern;

    // Cretae a local client
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create());

    std::shared_ptr<Buffer> buffer(Buffer::Create(FILESERVERHEADER_SIZE + message.size()));
    buffer->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)buffer->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_GETDIRS;
    pRequestHeader->Size         = message.size(); 

    memcpy(buffer->GetData() + FILESERVERHEADER_SIZE, message.c_str(), message.size());


    std::shared_ptr<http::HttpRequest> request( http::HttpRequest::CreatePost(m_sResource,"process",NOCONTEXT, buffer->ToBase64()) );
    std::shared_ptr<http::HttpResponse> response(client->Send(request));
 

    std::string sResponse(Base64::Decode(response->GetResponse()));

     HttpFilePtr::ValidateResponse(request, response, m_sPath);

    // ==================================================
    // Only perform split if there is files in the list
    // ==================================================
    if ( pRequestHeader->Size > 0 )
      dirs = util::Strings::Split(Base64::Decode(response->GetMessage()), '|');
  }


  /// If the status is OK the file exists
  return dirs;

} // IsExists()




/**
 * Determines if the file exists 
 */
const bool HttpDirPtr::Exists( )
{

  bool bExists(false);

  if ( ! m_sPath.empty() )
  {

    // Cretae a local client
    std::shared_ptr<http::HttpClient> client(http::HttpClient::Create());

    std::shared_ptr<Buffer> buffer(Buffer::Create(FILESERVERHEADER_SIZE + m_sPath.size()));
    buffer->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)buffer->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_EXISTS;
    pRequestHeader->Size         = m_sPath.size(); 

    memcpy(buffer->GetData() + FILESERVERHEADER_SIZE, m_sPath.c_str(), m_sPath.size());


 
    std::shared_ptr<http::HttpRequest> request( http::HttpRequest::CreatePost(m_sResource,"process",NOCONTEXT, buffer->ToBase64() ) );
    std::shared_ptr<http::HttpResponse> response(client->Send(request));
 

    std::string sResponse(Base64::Decode(response->GetResponse()));

     HttpFilePtr::ValidateResponse(request, response, m_sPath);

    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetResponse().c_str());

    /// If the status is OK the file exists
    bExists = ( pResponseHeader->StatusCode == STATUS_OK );
  }


  return bExists;
} // IsExists()

