// This is the main DLL file.

#include "stdafx.h"

#include "FileServerResourceProvider.h"
#include "MemoryStream.h"


USING_PROVIDER_NS


/**
 * Constructor
 */
FileServerResourceProvider::FileServerResourceProvider()
{
  m_fileServer.reset(new std::shared_ptr<io::FileServer>(io::FileServer::Create(0)));
} // FileServerResourceProvider()

/**
 * Handles the READ operation for the FileServer resource
 *
 * @param mpRequest     Request
 */
String ^ FileServerResourceProvider::ReadFileServer(RequestMessage ^ mpRequest)
{

  char * buffer = __nullptr;

  try
  {

    // Convert the payload back into Binary
    array<unsigned char> ^ mpBuffer = System::Convert::FromBase64String(mpRequest->Payload);

    std::shared_ptr<io::MemoryStream> stream(io::MemoryStream::Create(mpBuffer->Length));

    // Copy the managed into the unmanaged
    for ( register int i(0); i < mpBuffer->Length; stream->Push(mpBuffer[i++]) );

    (*m_fileServer)->RequestHandler(  stream );

    // Create unmanaged array
    buffer =  new  char[stream->GetOutputSize()];


    stream->CopyTo( buffer, stream->GetOutputSize() );

    
    mpBuffer = gcnew array<unsigned char>(stream->GetOutputSize());

    // Copy the managed into the unmanaged
    for ( register unsigned int i(0); i < stream->GetOutputSize(); mpBuffer[i] = buffer[i], i++ );


    return System::Convert::ToBase64String(mpBuffer);




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

/**
 * Handles the WRITE operation for the FileServer resource
 *
 * @param mpRequest     Request
 */
String ^ FileServerResourceProvider::WriteFileServer(RequestMessage ^ mpRequest)
{

  char * buffer = __nullptr;

  try
  {

    // Convert the payload back into Binary
    array<unsigned char> ^ mpBuffer = System::Convert::FromBase64String(mpRequest->Payload);

    std::shared_ptr<io::MemoryStream> stream(io::MemoryStream::Create(mpBuffer->Length));

    // Copy the managed into the stream
    for ( register int i(0); i < mpBuffer->Length; stream->Push(mpBuffer[i++]) );
    
    (*m_fileServer)->RequestHandler(  stream );

    // Create unmanaged array
    buffer = new  char[stream->GetOutputSize()];

    stream->CopyTo( buffer, stream->GetOutputSize() );

    mpBuffer = gcnew array<unsigned char>(stream->GetOutputSize());

    // Copy the managed into the unmanaged
    for ( register unsigned int i(0); i < stream->GetOutputSize(); mpBuffer[i] = buffer[i], i++ );


    return System::Convert::ToBase64String(mpBuffer);


  }
  catch ( ex::VException & ex )
  {

    LogErr(util::Strings::Format("File Server Write Failed : %s",ex.GetLogRecord().GetTextMessage().c_str()), "FileServerResourceProvider", "WriteFileServer");
    throw;
  }
  finally
  {
    if ( buffer != __nullptr )
      delete [] buffer;
  }

}

