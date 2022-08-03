// This is the main DLL file.

#include "stdafx.h"

#include "FileServerHeader.h"
#include "FileServerProvider.h"
#include "MemoryStream.h"
#include "vxBuffer.h"


USING_PROVIDER_NS
using namespace  System::IO;

/**
 * Constructor
 */
FileServerProvider::FileServerProvider()
{
 m_fileServer.reset(new std::shared_ptr<io::FileServer>(io::FileServer::Create(0)));
} // FileServerProvider()


/**
 * Handles the READ operation for the FileServer resource
 *
 * @param mpRequest     Request
 */
array<unsigned char> ^ FileServerProvider::ProcessRequest(array<System::Byte> ^ mpRequest)
{

     
  vxLogTimer timer;

  char * buffer = __nullptr;

  array<unsigned char> ^ mpResponse(__nullptr);

  try
  {

    buffer = new char [mpRequest->Length];

    register int i(0);

    for ( i =  0 ; i < mpRequest->Length; buffer[i] = mpRequest[i], i++ );

    std::shared_ptr<io::MemoryStream> stream(io::MemoryStream::Create((unsigned char *)buffer, mpRequest->Length));

    delete [] buffer;


    (*m_fileServer)->RequestHandler(  stream );
    

    // Create unmanaged array
    buffer =  new char[stream->GetOutputSize()];


    /// Copies the stream to the unmanaged buffer
    stream->CopyTo( buffer, stream->GetOutputSize() );

    /// Create a new managed buffer
    mpResponse = gcnew array<unsigned char>((int)stream->GetOutputSize());

    // Copy the umanaged buffer to the managed buffer
    for ( i = 0; i < stream->GetOutputSize(); mpResponse[i] = buffer[i], i++ );

    timer.Trace("Process request");

    return mpResponse;

  }
  catch ( ex::VException & ex )
  {
    LogErr(util::Strings::Format("File Server Read Failed : %s",ex.GetLogRecord().GetTextMessage().c_str()), "FileServerResourceProvider", "ReadFileServer");
    throw;
  }
  finally
  {
    if ( buffer != __nullptr )
      delete [] buffer;
  }



}

