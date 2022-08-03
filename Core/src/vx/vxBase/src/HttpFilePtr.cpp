#include "stdafx.h"
#include "HttpFilePtr.h"
#include "vxFileServer.h"
#include "vxTask.h"
#include "http\HttpRequest.h"
#include "http\HttpRequest.h"
#include "vxBuffer.h"
#include "vxTransportLayer.h"
#include "http\HttpClient.h"
#include "http\HttpResponse.h"
#include "Base64.h"

using namespace io;
using namespace http;


// static
std::string HttpFilePtr::m_sResource = "fileserver";


/**
 * Constructor 
 *
 * @param sFilePath     File to be opened
 * @param uCacheSize    Cache size for this object
 */
HttpFilePtr::HttpFilePtr(const std::string & sFilePath, const uint4 uCacheSize) :
FilePtr(sFilePath, uCacheSize),
m_bIsOpen(false)
{
 

}


/**
 * Destructor
 */
HttpFilePtr::~HttpFilePtr()
{

  Close();

}

/**
 * Creates the HttpFilePtr object
 *
 * @param sFilePath     File path to open
 */
std::shared_ptr<HttpFilePtr> HttpFilePtr::Create(const std::string & sFilePath, const  uint4 uCacheSize)
{
  std::shared_ptr<HttpFilePtr> pFilePtr = std::shared_ptr<HttpFilePtr>(new HttpFilePtr(sFilePath, uCacheSize));

  if ( pFilePtr == __nullptr ) 
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpFilePtr","HttpFilePtr","Create"));

  // Create the Http client using the current transport layer
  pFilePtr->m_pHttpClient = http::HttpClient::Create();

  return pFilePtr;
} // Create()


/**
 * Returns true if the file is open/connected
 */
inline const bool HttpFilePtr::IsOpen() 
{
 
  return m_bIsOpen;
  //std::shared_ptr<http::HttpRequestMessage>  message = http::HttpRequestMessage::Create(http::HTTP_GET, "fileserver", "open");


  //return m_pHttpClient->IsConnected();
} // IsOPen()

/**
 * Opens a file using fopen
 *
 * @param sMode     the open mode
 */
void HttpFilePtr::Open( const int4 iModeFlags, const int4 iCreateFlags )
{


  LogFyi(util::Strings::Format("Opening file %s on %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "Open");

  // ==========================================
  // Only open if we are not already open
  // ==========================================
  if ( ! IsOpen() )
  {

    try
    {

  
      std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE + m_sFilePath.size()));
                                                    
       
      FILESERVERHEADER * pRequestHeader = (FILESERVERHEADER *)request->GetData();
      RtlZeroMemory(pRequestHeader, FILESERVERHEADER_SIZE );

      pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
      pRequestHeader->Command      = FileServerCommand::CMD_OPEN;
      pRequestHeader->Size         = m_sFilePath.size(); 
      pRequestHeader->ModeFlags    = iModeFlags;
      pRequestHeader->CreateFlags  = iCreateFlags;

      memcpy( request->GetData(FILESERVERHEADER_SIZE),  m_sFilePath.c_str(), m_sFilePath.size() );

      LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");
 
      std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver"));

      ValidateResponse(request, response, m_sFilePath );

      FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

      // Copy the handle
      m_sHandle.assign(pResponseHeader->Handle);


      

      m_bIsOpen = true;
 
    }
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to open file %s : %s", m_sFilePath.c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "HttpFilePtr", "Open" );
      throw;
    }
    catch ( ... )
    {
      throw ex::IOException(LogRec(util::Strings::Format("Failed to open file %s : unspecified error", m_sFilePath.c_str()),"HttpFilePtr","Open"));

    }
  }
  else
    LogWrn(util::Strings::Format("File %s is already open", m_sFilePath.c_str()), "HttpFilePtr", "Open");


} // Open()


/**
 * Closes the file
 *
 * @return success
 */
void HttpFilePtr::Close()
{

  // =====================================
  // Can only close if the file is open
  // =====================================
  if ( IsOpen() )
  {

    LogFyi(util::Strings::Format("Closing file %s on %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "Close");

    // Wait for any outstanding tasks
    m_task->Wait();

    // Close the base which clears the cache
    FilePtr::Close();



    try
    {

      std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
      request->Fill(0);

      // Setup the request header
      FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

      pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
      pRequestHeader->Command      = FileServerCommand::CMD_CLOSE;
      pRequestHeader->Size         = 0;  
      strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());

      LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

      std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver"));

      m_bIsOpen = false;

    } 
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to close the file %s on %s : %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "HttpFilePtr","Close");

    }
    catch ( ... )
    {
      LogErr(util::Strings::Format("Failed to close the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), errno), "HttpFilePtr","Close");


    }

  }


} // Close()


/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 HttpFilePtr::Read( void * pBuffer, const  uint4 uBytesToRead )
{

  /// Read from file buffer
  uint4 uBytesRead(0);

  // ======================
  // Only read if open
  // ======================
  if ( IsOpen() )
  {
    
    //LogDbg(util::Strings::Format("Reading %d byte(s) from file %s on %s", uBytesToRead, m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "Read");
 
    if ( IsCachingEnabled() )
    {

      /// Wait for any read ahead's
      m_task->Wait();

      // First read from the cache
      uBytesRead = ReadCache(pBuffer, uBytesToRead);

#ifdef _DEBUG
      // Uncomment this statement to gain additional debug information
      LogDbg(util::Strings::Format("%s: %d byte(s) read from buffer",m_sFilename.c_str(), uBytesRead), "HttpFilePtr","Read");
#endif

      // Calculate the remaining byte I need
      uint4 uRemaining( uBytesToRead - uBytesRead );
 
      // ======================================================
      // If there are remaining bytes then read from the file
      // ======================================================
      if ( uRemaining > 0 )
      {

#ifdef _DEBUG
        // Uncomment this statement to gain additional debug information
        LogDbg(util::Strings::Format("%s: Attempting read of %d byte(s) from %s",m_sFilename.c_str(), uRemaining, m_pHttpClient->GetServer().c_str()), "HttpFilePtr","Read");
#endif

        uBytesRead += ReadFile(((char *)pBuffer + uBytesRead), uRemaining );

#ifdef _DEBUG
        // Uncomment this statement to gain additional debug information
        LogDbg(util::Strings::Format("%s: %d bytes read",m_sFilename.c_str(), uBytesRead), "HttpFilePtr","Read");
#endif
        // Re-fill the buffer
        ReadAhead();
  
      } // END ... If there are remaining bytes to read
     

    }
    else
    {

      
      uBytesRead = ReadFile(pBuffer, uBytesToRead);

#ifdef _DEBUG
      // Uncomment this statement to gain additional debug information
      LogDbg(util::Strings::Format("%s: %d bytes read",m_sFilename.c_str(), uBytesRead), "HttpFilePtr","Read");
#endif

    }
  }
  else
    LogWrn(util::Strings::Format("Cannot read from %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","Read")

  return uBytesRead;


} // Read()

/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 HttpFilePtr::ReadFile( void * pBuffer, const  uint4 uBytesToRead )
{

  uint4 uBytesRead(0);


  try
  {
 
    //LogDbg(util::Strings::Format("Reading %d bytes(s) from %s on %s",uBytesToRead, m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()),"HttpFilePtr","ReadFile");
    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_READ;
    strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
    pRequestHeader->Size         = uBytesToRead;  
 
    LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");


    std::shared_ptr<Buffer> response(Buffer::Create(FILESERVERHEADER_SIZE + uBytesToRead));
    m_pHttpClient->Send( request, "POST", "fileserver", response );
 
    ValidateResponse(request, response, m_sFilePath);

    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());


    /// Set the EOF
    m_bEof = pResponseHeader->IsEof;

    if ( pResponseHeader->Size > 0 )
    {

  

      uBytesRead = pResponseHeader->Size;

      memcpy(pBuffer, (char *)response->GetData(FILESERVERHEADER_SIZE) , uBytesRead);
    
      // ===================
      // Only issue warning
      // ===================
      if ( uBytesRead != uBytesToRead )
         LogWrn( "Expected to read " + ToAscii( uBytesToRead ) + " but only read " + ToAscii( uBytesRead ), "HttpFilePtr", "ReadFile" );

    }
    
     
    return uBytesRead;
 
  } 
  catch ( ex::VException & ex )
  {
    LogErr(util::Strings::Format("Failed to read the file %s on %s : %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "HttpFilePtr","Close");

    throw;
  }
  catch ( ... )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to read the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), errno), "HttpFilePtr","Close"));
  }

} // Read()



/** 
 * Writes data to the file
 *
 * @param pBuffer the output buffer
 * @uBytesToWrite the size of the output buffer
 */
void HttpFilePtr::Write( const void * pBuffer,  const uint4 uBytesToWrite )
{
 

  if ( IsOpen() )
  {
    LogDbg(util::Strings::Format("Writing %d bytes to file %s on %s", uBytesToWrite, m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "Write");

    /// Wait for any read ahead's
    m_task->Wait();

    try
    {
      std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE + uBytesToWrite));
      request->Fill(0);

      // Setup the request header
      FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

      pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
      pRequestHeader->Command      = FileServerCommand::CMD_WRITE;
      strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
      pRequestHeader->Size         = uBytesToWrite;  

      memcpy(request->GetData(FILESERVERHEADER_SIZE), pBuffer, uBytesToWrite);
 
      LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

      std::shared_ptr<Buffer> response(m_pHttpClient->Send( request, "POST", "fileserver" ));
 

      ValidateResponse(request, response, m_sFilePath);


    }
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to write the file %s on %s : %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "HttpFilePtr","Close");

      throw;
    }
    catch ( ... )
    {
      throw ex::IOException(LogRec(util::Strings::Format("Failed to write the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str(), errno), "HttpFilePtr","Close"));
    }
  }
  else
    LogWrn(util::Strings::Format("Cannot write to %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","Write")

} // Write()


/**
 * Flush the file to disk
 */
void HttpFilePtr::Flush()
{

  if ( IsOpen() )
  {
    //LogDbg(util::Strings::Format("Flushing file %s on %s", m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "Flush");

    /// Wait for any read ahead's
    m_task->Wait();


    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_SEEK;
    strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
    pRequestHeader->Size = 0;  

    LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

    std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver" ));
 
    ValidateResponse(request, response, m_sFilePath);


 }
  else
    LogWrn(util::Strings::Format("Cannot flush %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","Write")


} // Flush()


/**
 * Seek the file pointer ahead from the current position
 *
 * @param uNumBytes the number of bytes to seek
 */
bool HttpFilePtr::SeekCurrent( const __int64  uNumBytes )
{

  // ==================================
  // Can only seek of the file is open
  // =================================
  if ( IsOpen() )
  {

    //LogDbg(util::Strings::Format("Seek (%d bytes) in file %s on %s", uNumBytes, m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr", "SeekCurrent");

    /// Wait for any read ahead's
    m_task->Wait();


    if ( uNumBytes > m_uCacheBytesRemaining )
    {

      const uint4 seekBytes(uNumBytes - m_uCacheBytesRemaining);


      ClearCache();

      
      std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
      request->Fill(0);

      // Setup the request header
      FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

      pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
      pRequestHeader->Command      = FileServerCommand::CMD_SEEK;
      strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
      pRequestHeader->Size         = uNumBytes;  

 
      LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

      std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver" ));
 

      ValidateResponse(request, response, m_sFilePath);

      FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

      m_bEof = pResponseHeader->IsEof;

    }
    else
    {
      m_uCacheBytesRead += uNumBytes;
      m_uCacheBytesRemaining -= uNumBytes;
    }


     // Read Ahead
     ReadAhead();
 
 
  }
  else
    LogWrn(util::Strings::Format("Cannot perform seek in %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","SeekCurrent")

  return FilePtr::IsEof();

} // SeekCurrent()


/**
 * Returns true iff the read/write last attempted was outside the file. It does not necessarily return true
 * even if the file pointer currently points outside the file.
 *
 * @return true if end of file
 */
const bool HttpFilePtr::IsEof()
{


  // ========================================
  // Can only check EOF if the file is open
  // =========================================
  if ( IsOpen() )
  {


    // ========================================================
    // If the object is not at the End of File then continue
    // ========================================================
    if ( ! FilePtr::IsEof() ) 
    {


      //LogDbg(util::Strings::Format("Checking for EOF in %s on server %s",m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr","IsEof");

      /// Wait for any read ahead's
      m_task->Wait();

      if ( ! m_bEof )
      {

        std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
        request->Fill(0);

        // Setup the request header
        FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

        pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
        pRequestHeader->Command      = FileServerCommand::CMD_FEOF;
        strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
        pRequestHeader->Size = 0;  

        LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");
 
        std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver" ));
 

        ValidateResponse(request, response, m_sFilePath);

        FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

        m_bEof = pResponseHeader->IsEof;
      }

    } // END ... If the object is not EOF

  }
  else
    LogWrn(util::Strings::Format("Cannot check EOF in %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","IsEof")

  return FilePtr::IsEof();
} // IsEof()


/**
 * Determines if the file pointer currently points outside the file
 *
 * @return true if the file pointer currently points outside the file
 */	
const bool HttpFilePtr::IsEofSmart()
{

  // ==========================================
  // Can only check EOF if the file is open
  // ==========================================
  if ( IsOpen()  )
  {

    // ============================================
    // If the obect is not EOF then check further
    // ===========================================
    if ( !  FilePtr::IsEof() )
    {

      //LogDbg(util::Strings::Format("Checking for Smart-EOF in %s on server %s",m_sFilePath.c_str(), m_pHttpClient->GetServer().c_str()), "HttpFilePtr","IsEofSmart");

      /// Wait for any read ahead's
      m_task->Wait();

 
      if ( ! m_bEof )
      {

        std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
        request->Fill(0);

        // Setup the request header
        FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

        pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
        pRequestHeader->Command      = FileServerCommand::CMD_SMART_EOF;
        strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
        pRequestHeader->Size = 0;  

        LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

        std::shared_ptr<Buffer> response(m_pHttpClient->Send(request, "POST", "fileserver" ));
 

        ValidateResponse(request, response, m_sFilePath);


        FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

        m_bEof = pResponseHeader->IsEof;
      } // END ... If the FILE is not EOF

    } // END ... If the object is not EOF

  }
  else
    LogWrn(util::Strings::Format("Cannot check eof in %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","IsEofSmart")

  return FilePtr::IsEof();
} // IsEofSmart()


/**
 * Gets the size of the file
 *
 * @return the size of the file
 */	
const uint4  HttpFilePtr::Size()
{

  uint4  uSize(0);

  // ============================================
  // Can only get the size of the file is open
  // ============================================
  if ( IsOpen() )
  {

    /// Wait for any read ahead's
    m_task->Wait();

    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_SIZE;
    strncpy(pRequestHeader->Handle, m_sHandle.c_str(), m_sHandle.size());
    pRequestHeader->Size = 0;  



    LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

    std::shared_ptr<Buffer> response(m_pHttpClient->Send( request, "POST", "fileserver" ));
 

    ValidateResponse(request, response, m_sFilePath);


    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());
  

    uSize = pResponseHeader->Size;
  }
  else
    LogWrn(util::Strings::Format("Cannot get file size for %s, file is not open",m_sFilePath.c_str()), "HttpFilePtr","IsEofSmart")


  return uSize;

} // Size()


/**
 * Receives the response from the request. Will interpret the status code and throw an exception
 * if necessary.
 *
 * @param request        Sender's header
 * @param response      Receiver's header
 * @param validate      Validate the handle to make sure that the request and responses handle match if true, false handles
 *                      are ignored.
 * 
 */
void HttpFilePtr::ValidateResponse(const std::shared_ptr<http::HttpRequest> & request, const std::shared_ptr<http::HttpResponse> & response, const std::string & sFilePath )
{

  FILESERVERHEADER * pResponseHeader( (FILESERVERHEADER *)response->GetResponse().c_str() );
  FILESERVERHEADER * pRequestHeader( (FILESERVERHEADER *)request->GetMessage()->GetData() );

  std::string sCommand( ToString( pRequestHeader->Command  ));


  // Make sure that we are getting a RESPONSE
  if ( pResponseHeader->Type != io::RESPONSE )
    throw ex::IOException(LogRec("Unknown header type received, RESPONSE was expected", "HttpFilePtr","ReceiveResponse"));


  // =====================================================================================
  // If the validation flag is set then validate the file handle. THere are some 
  // operations that the handles will not match such as the OPEN.
  // =====================================================================================
  if ( pRequestHeader->Command != CMD_OPEN && strncmp(pRequestHeader->Handle, pResponseHeader->Handle, FILE_HANDLE_SIZE) != 0 )
    throw ex::IOException(LogRec("Receieved status with a different file handle then expected", "HttpFilePtr","ReceiveResponse"));

  // =================================================================================
  // If the status code indicate failure then the message should be extra payload data 
  // after the response header.
  // =================================================================================
  if ( pResponseHeader->StatusCode == STATUS_FAIL )
  {

    /// Default Error message used in case one was not sent back
    std::string sMessage(util::Strings::Format("Attempting %s of %s failed : unspecified error", sCommand.c_str(), sFilePath.c_str()));

    // =====================================================================================
    // Check if the response size is greater then 0 then an error message was sent back
    // =====================================================================================
    if ( pResponseHeader->Size > 0 )
    {


      std::shared_ptr<char> messageBuffer( new char[pResponseHeader->Size],  [] (char * value) { delete [] value; } );

      if ( messageBuffer == __nullptr )
        throw ex::OutOfMemoryException(LogRec("Failed to allocate message buffer", "HttpFilePtr", "ReceiveResponse"));

      // Assume the format of the response is TEXT for a failed event 
      std::string message( response->GetMessage() );

      memcpy(messageBuffer.get(), message.c_str(), message.size() );

      sMessage = std::string(messageBuffer.get(), pResponseHeader->Size);

    } // END .. If there was additional data

    throw ex::IOException(LogRec(sMessage, "HttpFilePtr","ReceiveResponse"));

  } // END ... If the status indicates a FAILURE
  
 
} // ReceiveResponse(FILESERVERHEADER & request, FILESERVERHEADER & response, std::shared_ptr<TcpClient> & client, bool validate )

/**
 * Receives the response from the request. Will interpret the status code and throw an exception
 * if necessary.
 *
 * @param request        Sender's header
 * @param response      Receiver's header
 * @param validate      Validate the handle to make sure that the request and responses handle match if true, false handles
 *                      are ignored.
 * 
 */
void HttpFilePtr::ValidateResponse(const std::shared_ptr<Buffer> & request, const std::shared_ptr<Buffer> & response, const std::string & sFilePath )
{


  if ( ! response->IsEmpty() )
  {
    FILESERVERHEADER * pResponseHeader( (FILESERVERHEADER *)response->GetData() );
    FILESERVERHEADER * pRequestHeader( (FILESERVERHEADER *)request->GetData() );

    std::string sCommand( ToString( pRequestHeader->Command  ));

    LogDbg( util::Strings::Format("RSP:%d:%s:%s:%d:%d", (int)pResponseHeader->Type, sCommand.c_str(), pResponseHeader->Handle, pResponseHeader->Size, pResponseHeader->StatusCode ), "HttpFilePtr","ValidateResponse");


    // Make sure that we are getting a RESPONSE
    if ( pResponseHeader->Type != io::RESPONSE )
      throw ex::IOException(LogRec("Unknown header type received, RESPONSE was expected", "HttpFilePtr","ValidateResponse"));



    // =====================================================================================
    // If the validation flag is set then validate the file handle. THere are some 
    // operations that the handles will not match such as the OPEN.
    // =====================================================================================
    if ( pRequestHeader->Command != CMD_OPEN && strncmp(pRequestHeader->Handle, pResponseHeader->Handle, FILE_HANDLE_SIZE) != 0 )
      throw ex::IOException(LogRec("Receieved status with a different file handle then expected", "HttpFilePtr","ValidateResponse"));

    // =================================================================================
    // If the status code indicate failure then the message should be extra payload data 
    // after the response header.
    // =================================================================================
    if ( pResponseHeader->StatusCode == STATUS_FAIL )
    {

      /// Default Error message used in case one was not sent back
      std::string sMessage(util::Strings::Format("Attempting %s of %s failed : unspecified error", sCommand.c_str(), sFilePath.c_str()));

      // =====================================================================================
      // Check if the response size is greater then 0 then an error message was sent back
      // =====================================================================================
      if ( pResponseHeader->Size > 0 )
      {


        std::shared_ptr<char> messageBuffer( new char[pResponseHeader->Size],  [] (char * value) { delete [] value; } );

        if ( messageBuffer == __nullptr )
          throw ex::OutOfMemoryException(LogRec("Failed to allocate message buffer", "HttpFilePtr", "ValidateResponse"));

        // Assume the format of the response is TEXT for a failed event 
        sMessage = std::string( (char *)response->GetData(FILESERVERHEADER_SIZE), response->GetLength() - FILESERVERHEADER_SIZE );


      } // END .. If there was additional data

      throw ex::IOException(LogRec(sMessage, "HttpFilePtr","ReceiveResponse"));

    } // END ... If the status indicates a FAILURE
  }
  else
    throw ex::NullPointerException(LogRec("Response is null or missing","HttpFilePtr","ValidateResponse"));
 
} // ReceiveResponse(FILESERVERHEADER & request, FILESERVERHEADER & response, std::shared_ptr<TcpClient> & client, bool validate )



/**
 * Determines if the file exists 
 */
const bool HttpFilePtr::Exists(const std::string & sFilePath )
{

  bool bExists(false);

  if ( ! sFilePath.empty() )
  {

    // Cretae a local client
    std::shared_ptr<http::HttpClient> client(HttpClient::Create());

    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE + sFilePath.size()));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetData());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_EXISTS;
    pRequestHeader->Size         = sFilePath.size(); 

    memcpy(request->GetData(FILESERVERHEADER_SIZE), sFilePath.c_str(), sFilePath.size());


     LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");

    std::shared_ptr<Buffer> response(client->Send( request, "POST", "fileserver" ));
 

    ValidateResponse(request, response, sFilePath);


    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());
 

    /// If the status is OK the file exists
    bExists = ( pResponseHeader->StatusCode == STATUS_OK );
  }

  return bExists;

} // IsExists()





/**
 * Determines if the file exists 
 */
const bool HttpFilePtr::Delete( )
{

  bool bDeleted(false);

  if ( ! m_sFilePath.empty() )
  {

    // Cretae a local client
    std::shared_ptr<HttpClient> client(HttpClient::Create());

    std::shared_ptr<Buffer> request(Buffer::Create(FILESERVERHEADER_SIZE));
    request->Fill(0);

    // Setup the request header
    FILESERVERHEADER * pRequestHeader((FILESERVERHEADER *)request->GetBuffer().get());

    pRequestHeader->Type         = FileServerTranscationTypes::REQUEST;
    pRequestHeader->Command      = FileServerCommand::CMD_DELETE;
    pRequestHeader->Size         = m_sFilePath.size(); 
 

    LogDbg(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)pRequestHeader->Type, ToString(pRequestHeader->Command).c_str(), pRequestHeader->Handle, pRequestHeader->Size, pRequestHeader->CreateFlags, pRequestHeader->ModeFlags ), "FileServer","RequestHandler");
 
    std::shared_ptr<Buffer> response(client->Send( request, "POST", "fileserver" ));
 

    ValidateResponse(request, response, m_sFilePath);


    FILESERVERHEADER * pResponseHeader((FILESERVERHEADER *)response->GetData());

    /// If the status is OK the file exists
    bDeleted = ( pResponseHeader->StatusCode == STATUS_OK );
  }

  return bDeleted;

} // IsExists()





