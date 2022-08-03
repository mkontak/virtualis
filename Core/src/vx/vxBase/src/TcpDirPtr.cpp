#include "stdafx.h"
#include "TcpDirPtr.h"
#include "vxFileServer.h"
#include "TcpClient.h"
#include "FileServerHeader.h"

using namespace io;

/**
 * Constructor 
 *
 * @param sFilePath     File to be opened
 */
TcpDirPtr::TcpDirPtr(const std::string & sPath) :
DirPtr(sPath)
{
 

}


/**
 * Destructor
 */
TcpDirPtr::~TcpDirPtr()
{

}

/**
 * Creates the TcpDirPtr object
 *
 * @param sFilePath     File path to open
 */
std::shared_ptr<TcpDirPtr> TcpDirPtr::Create(const std::string & sPath)
{
  std::shared_ptr<TcpDirPtr> dirPtr = std::shared_ptr<TcpDirPtr>(new TcpDirPtr(sPath));

  if ( dirPtr == __nullptr ) 
    throw ex::OutOfMemoryException(LogRec("Failed to allocate TcpDirPtr","TcpDirPtr","Create"));


  return dirPtr;
} // Create()




/**
 * Receives the response from the request. Will interpret the status code and throw an exception
 * if necessary.
 *
 * @param header        Response header (Contains the request information)
 * 
 */
void TcpDirPtr::ReceiveResponse(FILESERVERHEADER & request, FILESERVERHEADER & response, std::shared_ptr<TcpClient> & client)
{
  std::string sCommand(ToString(request.Command ));


  LogDbg(util::Strings::Format("Waiting for response for %s", sCommand.c_str()), "TcpFilePtr","ReceiveResponse");

  client->Receive(response);

  // ================================================================
  // If the status code indicate failure then the message should be 
  // extra payload data after the response header.
  // =================================================================
  if ( response.StatusCode == STATUS_FAIL )
  {

    /// Error message
    std::string sMessage(util::Strings::Format("Attempting %s of %s failed : unspecified error", sCommand.c_str(), m_sPath.c_str()));

    if ( response.Size > 0 )
    {
      std::shared_ptr<char> buffer( new char[response.Size],  [] (char * value) { delete [] value; } );

      client->Receive(buffer.get(), response.Size);

      sMessage = std::string(buffer.get(), response.Size);
    }

    throw ex::IOException(LogRec(sMessage, "TcpDirPtr","ReceiveResponse"));
  }

 
}



/**
 * Gets the list of files from the path
 *
 * @param sPattern      Pattern to match
 *
 * @return list of files
 */
std::vector<std::string> TcpDirPtr::GetFiles( const std::string & sPattern)
{

  std::shared_ptr<TcpClient> client = TcpClient::Create();
  
  client->Connect();

  FILESERVERHEADER sender;
  RtlZeroMemory((char *)&sender, FILESERVERHEADER_SIZE );

  sender.Type         = FileServerTranscationTypes::REQUEST;
  sender.Command      = FileServerCommand::CMD_GETFILES;

  std::string message = m_sPath + "|" + sPattern;

  sender.Size = message.size();
 
  client->Send(sender, message.c_str(), message.size(), true);  
 

  FILESERVERHEADER receiver;
  ReceiveResponse(sender, receiver, client);

  std::vector<std::string> files;
 
  if ( receiver.Size > 0 )
  {

      //LogDbg(util::Strings::Format("Reading %d bytes(s) from file server",header.Size),"TcpFilePtr","ReadFile");
 
      uint4 uBytesRead = receiver.Size;
      const uint4 uSize(receiver.Size + 1);

      std::shared_ptr<char> buffer =  std::shared_ptr<char>(new char [uSize],  [] ( char * value ) { delete [] value; } );      
      RtlZeroMemory(buffer.get(), uSize);


      client->Receive((char *)buffer.get(),  receiver.Size);
    
      // ===================
      // Only issue warning
      // ===================
      if ( uBytesRead != receiver.Size )
      {
        LogWrn( "Expected to read " + ToAscii( receiver.Size ) + " but only read " + ToAscii( uBytesRead ), "TcpDirPtr", "GetFiles" );
        //throw ex::IOException( LogRec( std::string( "Read failed " ) + std::string( strerror( errno ) ), "TcpFilePtr", "ReadFile" ) );
      }

      files = util::Strings::Split(std::string(buffer.get()), '|');
  }

  client->Disconnect();

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
std::vector<std::string> TcpDirPtr::GetDirs( const std::string & sPattern)
{

  std::shared_ptr<TcpClient> client = TcpClient::Create();
  
  client->Connect();

  FILESERVERHEADER sender;
  RtlZeroMemory((char *)&sender, FILESERVERHEADER_SIZE );

  sender.Type         = FileServerTranscationTypes::REQUEST;
  sender.Command      = FileServerCommand::CMD_GETDIRS;

 
  std::string message = m_sPath + "|" + sPattern;

  sender.Size = message.size();

  client->Send(sender, message.c_str(), message.size(), true);     
 
  FILESERVERHEADER receiver;

  ReceiveResponse(sender, receiver, client);

  std::vector<std::string> dirs;
 
  if ( receiver.Size > 0 )
  {

      //LogDbg(util::Strings::Format("Reading %d bytes(s) from file server",header.Size),"TcpFilePtr","ReadFile");
 
      uint4 uBytesRead = receiver.Size;
      const uint4 uSize(receiver.Size + 1);

      std::shared_ptr<char> buffer =  std::shared_ptr<char>(new char [uSize],  [] ( char * value ) { delete [] value; } );      
      RtlZeroMemory(buffer.get(), uSize);


      client->Receive((char *)buffer.get(),  receiver.Size);
    
      // ===================
      // Only issue warning
      // ===================
      if ( uBytesRead != receiver.Size )
      {
        LogWrn( "Expected to read " + ToAscii( receiver.Size ) + " but only read " + ToAscii( uBytesRead ), "TcpDirPtr", "GetDirs" );
        //throw ex::IOException( LogRec( std::string( "Read failed " ) + std::string( strerror( errno ) ), "TcpFilePtr", "ReadFile" ) );
      }

      dirs = util::Strings::Split(std::string(buffer.get()), '|');
  }

  client->Disconnect();

  /// If the status is OK the file exists
  return dirs;

} // IsExists()




/**
 * Determines if the file exists 
 */
const bool TcpDirPtr::Exists( )
{

  bool bExists(false);

  if ( ! m_sPath.empty() )
  {
    
    std::shared_ptr<TcpClient> client = TcpClient::Create();
  
    FILESERVERHEADER sender;
    RtlZeroMemory((char *)&sender, FILESERVERHEADER_SIZE );

    sender.Type         = FileServerTranscationTypes::REQUEST;
    sender.Command      = FileServerCommand::CMD_EXISTS;
    sender.Size         = m_sPath.size(); 
 
    client->Send(sender);             // Header
    client->Send(m_sPath.c_str(), m_sPath.size());      // File Path
 
    FILESERVERHEADER receiver;
    RtlZeroMemory((char *)&receiver, FILESERVERHEADER_SIZE );

    ReceiveResponse(sender, receiver, client);

    /// If the status is OK the file exists
    bExists = ( receiver.StatusCode == STATUS_OK );
  }

  return bExists;
} // IsExists()


