#include "stdafx.h"
#include "TcpFilePtr.h"
#include "vxFileServer.h"
#include "TcpClient.h"
#include "FileServerHeader.h"
#include "vxTask.h"

using namespace io;

/**
 * Constructor 
 *
 * @param sFilePath     File to be opened
 * @param uCacheSize    Cache size for this object
 */
TcpFilePtr::TcpFilePtr(const std::string & sFilePath, const uint4 uCacheSize) :
FilePtr(sFilePath, uCacheSize)
{
 

}


/**
 * Destructor
 */
TcpFilePtr::~TcpFilePtr()
{

  Close();

}

/**
 * Creates the TcpFilePtr object
 *
 * @param sFilePath     File path to open
 */
std::shared_ptr<TcpFilePtr> TcpFilePtr::Create(const std::string & sFilePath, const  uint4 uCacheSize)
{
  std::shared_ptr<TcpFilePtr> pFilePtr = std::shared_ptr<TcpFilePtr>(new TcpFilePtr(sFilePath, uCacheSize));

  if ( pFilePtr == __nullptr ) 
    throw ex::OutOfMemoryException(LogRec("Failed to allocate TcpFilePtr","TcpFilePtr","Create"));

  // Create the TCP client using the current transport layer
  pFilePtr->m_pTcpClient = TcpClient::Create();

  return pFilePtr;
} // Create()


/**
 * Returns true if the file is open/connected
 */
inline const bool TcpFilePtr::IsOpen() 
{
  return m_pTcpClient->IsConnected();
} // IsOPen()

/**
 * Opens a file using fopen
 *
 * @param sMode     the open mode
 */
void TcpFilePtr::Open( const int4 iModeFlags, const int4 iCreateFlags )
{


  LogFyi(util::Strings::Format("Opening file %s on %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "Open");

  // ==========================================
  // Only open if we are not already open
  // ==========================================
  if ( ! IsOpen() )
  {

    try
    {

       // Connect to the server 
      m_pTcpClient->Connect();


      FILESERVERHEADER request;
      RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

      request.Type         = FileServerTranscationTypes::REQUEST;
      request.Command      = FileServerCommand::CMD_OPEN;
      request.Size         = m_sFilePath.size(); 
      request.ModeFlags    = iModeFlags;
      request.CreateFlags  = iCreateFlags;

 
      m_pTcpClient->Send(request, m_sFilePath.c_str(), m_sFilePath.size());

      FILESERVERHEADER response;

      /// Don't validate since we are expecting the handle back in the response.
      ReceiveResponse(request, response, false);

      // Copy the handle
      m_sHandle.assign(response.Handle);

      // We only need to read ahead if we are reading
      if ((iModeFlags & GENERIC_READ) == GENERIC_READ)
        ReadAhead();

 
    }
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to open file %s : %s", m_sFilePath.c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "TcpFilePtr", "Open" );
      throw;
    }
    catch ( ... )
    {
      throw ex::IOException(LogRec(util::Strings::Format("Failed to open file %s : unspecified error", m_sFilePath.c_str()),"TcpFilePtr","Open"));

    }
  }
  else
    LogWrn(util::Strings::Format("File %s is already open", m_sFilePath.c_str()), "TcpFilePtr", "Open");


} // Open()


/**
 * Closes the file
 *
 * @return success
 */
void TcpFilePtr::Close()
{

  // =====================================
  // Can only close if the file is open
  // =====================================
  if ( IsOpen() )
  {

    LogFyi(util::Strings::Format("Closing file %s on %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "Close");

    // Wait for any outstanding tasks
    m_task->Wait();

    // Close the base which clears the cache
    FilePtr::Close();


    // Disconnect at the end
    SCOPE_EXIT 
    {
      // Disconnects the 
      m_pTcpClient->Disconnect();

    };

    try
    {

      FILESERVERHEADER request;
      RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

      request.Type       = FileServerTranscationTypes::REQUEST;
      request.Command    = FileServerCommand::CMD_CLOSE;
      request.Size = 0;  
 
      strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
  
      m_pTcpClient->Send(request);
 


    } 
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to close the file %s on %s : %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "TcpFilePtr","Close");

    }
    catch ( ... )
    {
      LogErr(util::Strings::Format("Failed to close the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), errno), "TcpFilePtr","Close");


    }

  }


} // Close()


/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 TcpFilePtr::Read( void * pBuffer, const  uint4 uBytesToRead )
{

  /// Read from file buffer
  uint4 uBytesRead(0);

  // ======================
  // Only read if open
  // ======================
  if ( IsOpen() )
  {
    
    LogDbg(util::Strings::Format("Reading %d byte(s) from file %s on %s", uBytesToRead, m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "Read");
 
    if ( IsCachingEnabled() )
    {

      /// Wait for any read ahead's
      m_task->Wait();

      // First read from the cache
      uBytesRead = ReadCache(pBuffer, uBytesToRead);

#ifdef _DEBUG
      // Uncomment this statement to gain additional debug information
      LogDbg(util::Strings::Format("%s: %d byte(s) read from buffer",m_sFilename.c_str(), uBytesRead), "TcpFilePtr","Read");
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
        LogDbg(util::Strings::Format("%s: Attempting read of %d byte(s) from %s",m_sFilename.c_str(), uRemaining, m_pTcpClient->GetServer().c_str()), "TcpFilePtr","Read");
#endif

        uBytesRead += ReadFile(((char *)pBuffer + uBytesRead), uRemaining );

#ifdef _DEBUG
        // Uncomment this statement to gain additional debug information
        LogDbg(util::Strings::Format("%s: %d bytes read",m_sFilename.c_str(), uBytesRead), "TcpFilePtr","Read");
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
      LogDbg(util::Strings::Format("%s: %d bytes read",m_sFilename.c_str(), uBytesRead), "TcpFilePtr","Read");
#endif

    }
  }
  else
    LogWrn(util::Strings::Format("Cannot read from %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","Read")

  return uBytesRead;


} // Read()

/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 TcpFilePtr::ReadFile( void * pBuffer, const  uint4 uBytesToRead )
{

  uint4 uBytesRead(0);


  try
  {
 
    LogDbg(util::Strings::Format("Reading %d bytes(s) from %s on %s",uBytesToRead, m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()),"TcpFilePtr","ReadFile");


    FILESERVERHEADER request;
    RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

    request.Type           = FileServerTranscationTypes::REQUEST;
    request.Command        = FileServerCommand::CMD_READ;
    request.Size           = uBytesToRead;
    strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
 
    // Send read request
    m_pTcpClient->Send(request);
 

    FILESERVERHEADER response;

    // Receive the response
    ReceiveResponse(request, response); 

    /// Set the EOF
    m_bEof = response.IsEof;

    if ( response.Size > 0 )
    {

      //LogDbg(util::Strings::Format("Reading %d bytes(s) from file server",header.Size),"TcpFilePtr","ReadFile");
 
      uBytesRead = m_pTcpClient->Receive((char *)pBuffer, response.Size);
    
      // ===================
      // Only issue warning
      // ===================
      if ( uBytesRead != uBytesToRead )
         LogWrn( "Expected to read " + ToAscii( uBytesToRead ) + " but only read " + ToAscii( uBytesRead ), "TcpFilePtr", "ReadFile" );

    }
    
     
    return uBytesRead;
 
  } 
  catch ( ex::VException & ex )
  {
    LogErr(util::Strings::Format("Failed to read the file %s on %s : %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "TcpFilePtr","Close");

    throw;
  }
  catch ( ... )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to read the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), errno), "TcpFilePtr","Close"));
  }

} // Read()



/** 
 * Writes data to the file
 *
 * @param pBuffer the output buffer
 * @uBytesToWrite the size of the output buffer
 */
void TcpFilePtr::Write( const void * pBuffer,  const uint4 uBytesToWrite )
{
 

  if ( IsOpen() )
  {
    LogDbg(util::Strings::Format("Writing %d bytes to file %s on %s", uBytesToWrite, m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "Write");

    /// Wait for any read ahead's
    m_task->Wait();

    try
    {
      FILESERVERHEADER request;
      RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

      request.Type           = FileServerTranscationTypes::REQUEST;
      request.Command        = FileServerCommand::CMD_READ;
      strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
      request.Size = uBytesToWrite;  
 
      m_pTcpClient->Send(request, (const char *)pBuffer, uBytesToWrite);

      FILESERVERHEADER response;

      ReceiveResponse(request, response); 

    }
    catch ( ex::VException & ex )
    {
      LogErr(util::Strings::Format("Failed to write the file %s on %s : %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), ex.GetLogRecord().GetTextMessage().c_str()), "TcpFilePtr","Close");

      throw;
    }
    catch ( ... )
    {
      throw ex::IOException(LogRec(util::Strings::Format("Failed to write the file %s on %s : unspecified error (%d)", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str(), errno), "TcpFilePtr","Close"));
    }
  }
  else
    LogWrn(util::Strings::Format("Cannot write to %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","Write")

} // Write()


/**
 * Flush the file to disk
 */
void TcpFilePtr::Flush()
{

  if ( IsOpen() )
  {
    LogDbg(util::Strings::Format("Flushing file %s on %s", m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "Flush");

    /// Wait for any read ahead's
    m_task->Wait();


    FILESERVERHEADER request;
    RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

    request.Type       = FileServerTranscationTypes::REQUEST;
    request.Command   = FileServerCommand::CMD_FLUSH;
    strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
    request.Size = 0;  
 
    m_pTcpClient->Send(request);
 
    FILESERVERHEADER response;

    ReceiveResponse(request, response); 

 }
  else
    LogWrn(util::Strings::Format("Cannot flush %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","Write")


} // Flush()


/**
 * Seek the file pointer ahead from the current position
 *
 * @param uNumBytes the number of bytes to seek
 */
bool TcpFilePtr::SeekCurrent( const __int64  uNumBytes )
{

  // ==================================
  // Can only seek of the file is open
  // =================================
  if ( IsOpen() )
  {

    LogDbg(util::Strings::Format("Seek (%d bytes) in file %s on %s", uNumBytes, m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr", "SeekCurrent");

    /// Wait for any read ahead's
    m_task->Wait();


    if ( uNumBytes > m_uCacheBytesRemaining )
    {

      const uint4 seekBytes(uNumBytes - m_uCacheBytesRemaining);


      ClearCache();

      FILESERVERHEADER request;
      RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

      request.Type           = FileServerTranscationTypes::REQUEST;
      request.Command        = FileServerCommand::CMD_SEEK;
      request.Size           = uNumBytes;

      strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());

      m_pTcpClient->Send(request);
 

      FILESERVERHEADER response;

      ReceiveResponse(request, response); 

      m_bEof = response.IsEof;

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
    LogWrn(util::Strings::Format("Cannot perform seek in %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","SeekCurrent")

  return FilePtr::IsEof();

} // SeekCurrent()


/**
 * Returns true iff the read/write last attempted was outside the file. It does not necessarily return true
 * even if the file pointer currently points outside the file.
 *
 * @return true if end of file
 */
const bool TcpFilePtr::IsEof()
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


      LogDbg(util::Strings::Format("Checking for EOF in %s on server %s",m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr","IsEof");

      /// Wait for any read ahead's
      m_task->Wait();

      if ( ! m_bEof )
      {
        FILESERVERHEADER request;
        RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

        request.Type       = FileServerTranscationTypes::REQUEST;
        request.Command    = FileServerCommand::CMD_FEOF;
        strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
        request.Size = 0;  
 
        m_pTcpClient->Send(request);
 
        FILESERVERHEADER response;

        ReceiveResponse(request, response); 

        m_bEof = response.IsEof;
      }

    } // END ... If the object is not EOF

  }
  else
    LogWrn(util::Strings::Format("Cannot check EOF in %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","IsEof")

  return FilePtr::IsEof();
} // IsEof()


/**
 * Determines if the file pointer currently points outside the file
 *
 * @return true if the file pointer currently points outside the file
 */	
const bool TcpFilePtr::IsEofSmart()
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

      LogDbg(util::Strings::Format("Checking for Smart-EOF in %s on server %s",m_sFilePath.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr","IsEofSmart");

      /// Wait for any read ahead's
      m_task->Wait();

 
      if ( ! m_bEof )
      {
        FILESERVERHEADER request;
        RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

        request.Type       = FileServerTranscationTypes::REQUEST;
        request.Command   = FileServerCommand::CMD_SMART_EOF;
        strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
        request.Size = 0;  

        m_pTcpClient->Send(request);
 
        FILESERVERHEADER response;

        ReceiveResponse(request, response); 
  
        m_bEof = response.IsEof;
      } // END ... If the FILE is not EOF

    } // END ... If the object is not EOF

  }
  else
    LogWrn(util::Strings::Format("Cannot check eof in %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","IsEofSmart")

  return FilePtr::IsEof();
} // IsEofSmart()


/**
 * Gets the size of the file
 *
 * @return the size of the file
 */	
const uint4  TcpFilePtr::Size()
{

  uint4  uSize(0);

  // ============================================
  // Can only get the size of the file is open
  // ============================================
  if ( IsOpen() )
  {

    /// Wait for any read ahead's
    m_task->Wait();

    FILESERVERHEADER request;
    RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

    request.Type        = FileServerTranscationTypes::REQUEST;
    request.Command     = FileServerCommand::CMD_SIZE;
    strncpy(request.Handle, m_sHandle.c_str(), m_sHandle.size());
    request.Size = 0;  
 
    m_pTcpClient->Send(request);
 
    FILESERVERHEADER response;

    ReceiveResponse(request, response); 
  

    uSize = response.Size;
  }
  else
    LogWrn(util::Strings::Format("Cannot get file size for %s, file is not open",m_sFilePath.c_str()), "TcpFilePtr","IsEofSmart")


  return uSize;

} // Size()


/**
 * Receives the response from the request. Will interpret the status code and throw an exception
 * if necessary.
 *
 * @param request        Sender's header
 * @param response      Receiver's header
 * @param client        Client connection
 * @param validate      Validate the handle to make sure that the request and responses handle match if true, false handles
 *                      are ignored.
 * 
 */
void TcpFilePtr::ReceiveResponse(FILESERVERHEADER & request, FILESERVERHEADER & response, std::shared_ptr<TcpClient> & client, bool validate )
{


  std::string sCommand( ToString( request.Command  ));


  LogDbg(util::Strings::Format("Waiting for response for %s on %s", sCommand.c_str(), m_pTcpClient->GetServer().c_str()), "TcpFilePtr","ReceiveResponse");

  int rc = client->Receive(response);

  // Make sure that we are getting a RESPONSE
  if ( response.Type != io::RESPONSE )
    throw ex::IOException(LogRec("Unknown header type received, RESPONSE was expected", "TcpFilePtr","ReceiveResponse"));


  // =====================================================================================
  // If the validation flag is set then validate the file handle. THere are some 
  // operations that the handles will not match such as the OPEN.
  // =====================================================================================
  if ( validate && strncmp(request.Handle, response.Handle, FILE_HANDLE_SIZE) != 0 )
    throw ex::IOException(LogRec("Receieved status with a different file handle then expected", "TcpFilePtr","ReceiveResponse"));

  // =================================================================================
  // If the status code indicate failure then the message should be extra payload data 
  // after the response header.
  // =================================================================================
  if ( response.StatusCode == STATUS_FAIL )
  {

    /// Default Error message used in case one was not sent back
    std::string sMessage(util::Strings::Format("Attempting %s of %s failed : unspecified error", sCommand.c_str(), m_sFilePath.c_str()));

    // =====================================================================================
    // Check if the response size is greater then 0 then an error message was sent back
    // =====================================================================================
    if ( response.Size > 0 )
    {


      std::shared_ptr<char> messageBuffer( new char[response.Size],  [] (char * value) { delete [] value; } );

      if ( messageBuffer == __nullptr )
        throw ex::OutOfMemoryException(LogRec("Failed to allocate message buffer", "TcpFilePtr", "ReceiveResponse"));

      
      client->Receive(messageBuffer.get(), response.Size);

      sMessage = std::string(messageBuffer.get(), response.Size);

    } // END .. If there was additional data

    throw ex::IOException(LogRec(sMessage, "TcpFilePtr","ReceiveResponse"));

  } // END ... If the status indicates a FAILURE
  
 
} // ReceiveResponse(FILESERVERHEADER & request, FILESERVERHEADER & response, std::shared_ptr<TcpClient> & client, bool validate )

/**
 * Determines if the file exists 
 */
const bool TcpFilePtr::Exists(const std::string & sFilePath )
{

  bool bExists(false);

  if ( ! sFilePath.empty() )
  {

    // Cretae a local client
    std::shared_ptr<TcpClient> client = TcpClient::Create();

    // Connect to the server
    client->Connect();

    // Setup the request header
    FILESERVERHEADER request;
    RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

    request.Type         = FileServerTranscationTypes::REQUEST;
    request.Command      = FileServerCommand::CMD_EXISTS;
    request.Size         = sFilePath.size(); 
 
    client->Send(request, sFilePath.c_str(), sFilePath.size());    

    // Receiver
    FILESERVERHEADER recevier;

    // Get the response header which should indicate status
    client->Receive(recevier);

    // Disconnect
    client->Disconnect();

    /// If the status is OK the file exists
    bExists = ( recevier.StatusCode == STATUS_OK );
  }

  return bExists;

} // IsExists()





/**
 * Determines if the file exists 
 */
const bool TcpFilePtr::Delete( )
{

  bool bDeleted(false);

  if ( ! m_sFilePath.empty() )
  {

    // Cretae a local client
    std::shared_ptr<TcpClient> client = TcpClient::Create();

    // Connect to the server
    client->Connect();

    // Setup the request header
    FILESERVERHEADER request;
    RtlZeroMemory((char *)&request, FILESERVERHEADER_SIZE );

    request.Type         = FileServerTranscationTypes::REQUEST;
    request.Command      = FileServerCommand::CMD_DELETE;
    request.Size         = m_sFilePath.size(); 
 
    client->Send(request, m_sFilePath.c_str(), m_sFilePath.size());    

    // Receiver
    FILESERVERHEADER recevier;

    // Get the response header which should indicate status
    client->Receive(recevier);

    // Disconnect
    client->Disconnect();

    /// If the status is OK the file exists
    bDeleted = ( recevier.StatusCode == STATUS_OK );
  }

  return bDeleted;

} // IsExists()





