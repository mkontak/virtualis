// This is the main DLL file.

#include "stdafx.h"

#include "DcmImage.h"
#include "DcmExceptions.h"
#include "DcmCallback.h"

#include "Server.h"
#include "Enums.h"

#pragma region usings

using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;


USING_DICOM_NS
 

#pragma endregion


/** 
 * Default constructor
 */
Server::Server() :
m_msStoragePath(String::Empty)
{
   m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &Server::Callback);

} // Server()






/** 
 * Creates a dicom server from the xml node
 * 
 * @param mpNode        Xml node
 *
 * @return  Server object
 */
Server ^ Server::Create(XmlNode ^ mpNode)
{

  Server ^ mpServer = gcnew Server();

  if ( mpServer == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocated Server object");


  /// Creates the unmanaged dicom server
  mpServer->m_pDcmServer.reset(new std::shared_ptr<dcm::DcmServer>(dcm::DcmServer::Create(ss(mpNode->OuterXml)))); 


  try
  {
    
    /// Storage path is required
    mpServer->m_msStoragePath =  ( mpNode->Attributes["storagePath"] != nullptr ? mpNode->Attributes["storagePath"]->Value : String::Empty );

    ///// If the storage path is empty then we have invalid data
    //if ( String::IsNullOrEmpty(mpServer->m_msStoragePath) )
    //  throw gcnew InvalidDataException("Storage path missing or invalid");

    ///// If the storage path does not exists throw and exception
    //if ( ! System::IO::Directory::Exists(mpServer->m_msStoragePath) )
    //  throw gcnew System::IO::DirectoryNotFoundException("Storage path does not exist");

  }
  catch (Exception ^ )
  {
    // rethrow the exception
    throw;
  }


  return mpServer;

} // FromXml(XmlNode ^ mpNode)

/**
 * Sets the transfer syntaxes for the server
 *
 * @param transferSyntax    Defines a group of UIDS to use for the syntax
 */
void Server::SetTransferSyntax(E_TransferSyntax transferSyntax)
{
  (*m_pDcmServer)->SetTransferSyntax(transferSyntax);

} // SetTransferSyntaxes(E_TransferSyntax transferSyntax)



/** 
 * Starts the servers thread
 */
void Server::Start()
{

  
  try
  {
    //// Allocate an new handle for the callback delegate
    m_gcHandle = GCHandle::Alloc(m_mpCallbackDelegate);

    //// Marshall the function pointer for unmanaged usage
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

    //// Cast the int pointer the unmanaged callback function pointer for usage in the call
    dcm::DCM_CALLBACK_FUNCTION callback = static_cast<dcm::DCM_CALLBACK_FUNCTION>(ip.ToPointer());


    (*m_pDcmServer)->Start(__nullptr, callback);

  }
  catch ( ... )
  {
    throw gcnew DicomException("Failed to start the dicom server");
  }

  //// Free the global handle
  //gh.Free();

}

/**
 * Stops the Dicom Server thread.
 *
 * @return void.
 */
void Server::Stop()
{

  if ( m_gcHandle.IsAllocated )
    m_gcHandle.Free();

  try
  {
    (*m_pDcmServer)->Abort();
  }
  catch ( dcm::DcmException & ex )
  {
    Viatronix::Logging::Log::Warning(this->AETitle, "Abort failed : " + gcnew String(ex.GetTextMessage().c_str()), "Server", "Stop");
  }

 
} // Stop()

/**
 * Waits for the server
 *
 * @return void.
 */
void Server::Wait()
{

  try
  {
    (*m_pDcmServer)->Wait(INFINITE);
  }
  catch ( dcm::DcmException & ex )
  {
    Viatronix::Logging::Log::Warning(this->AETitle, "Wait failed : " + gcnew String(ex.GetTextMessage().c_str()), "Server", "Wait");
  }

 
} // Stop()


/**
 * Handles the callback from the server
 */
void Server::Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData)
{

  try
  {
    // Create the args
    ServerEventArgs ^ pArgs( ServerEventArgs::Create(IntPtr(pParams), IntPtr(lpCallbackData)));
 
    switch ( pParams->CommandType )
    {
    case dcm::DcmCommands::C_NONE:
      {
        if (  pParams->EventType == dcm::DcmEvents::E_STARTED )
          OnStarted(pArgs);
        else if (  pParams->EventType == dcm::DcmEvents::E_FAILED )
          OnFailed(pArgs);
        else if (  pParams->EventType == dcm::DcmEvents::E_COMPLETED )
          OnCompleted(pArgs);

        break;
      }
    case dcm::DcmCommands::C_ECHO:             // Echo command
      {
        break;
      }
    case dcm::DcmCommands::C_STORE:            // Store Command
      {

        switch ( pParams->EventType )
        {
        case dcm::DcmEvents::E_PROGRESS:
          {

            vxLogTimer timer;

            dcm::DcmImage * pDcmImage(pParams->Command.Store.Image);

  
            // ====================================================================================================================
            // If the storage path is set we assume that we are acting a just a receiver so the data is saved. The syetem validates
            // the dicom so that we have the minimum fields such as Study Uid, Series UID and Instance UId. Also the path for the 
            // image is set ( <STORAGE_PATH>\MID_STUDY-UID\SERIES_UID\dicom\INSTANCE_UID.dcm )
            // ===================================================================================================================
            if ( !String::IsNullOrEmpty(m_msStoragePath) )
            {

              std::string sPatientName;
              std::string sMedicalId;
              std::string sStudyUid;
              std::string sSeriesUid;
              std::string sInstanceUid;

              pDcmImage->TryGetValue<std::string>(DCM_PatientName, sPatientName); 
              pDcmImage->TryGetValue<std::string>(DCM_PatientID, sMedicalId);

              // We require a study uid
              if ( ! pDcmImage->TryGetValue<std::string>(DCM_StudyInstanceUID, sStudyUid) ) 
                throw gcnew InvalidDataException("No study uid found in dicom header");
  
              // We rquire a series uid
              if ( ! pDcmImage->TryGetValue<std::string>(DCM_SeriesInstanceUID, sSeriesUid) ) 
                throw gcnew InvalidDataException("No series uid found in dicom header");

              // We require an instace uid
              if ( ! pDcmImage->TryGetValue<std::string>(DCM_SOPInstanceUID, sInstanceUid) ) 
                throw gcnew InvalidDataException("No instance uid found in dicom header");

              /// Convert from std::string to Sting ^
              String ^ msMedicalId(gcnew String(sMedicalId.c_str()));
              String ^ msPatientName(gcnew String(sPatientName.c_str()));
              String ^ msStudyUid (gcnew String(sStudyUid.c_str()));
              String ^ msSeriesUid(gcnew String(sSeriesUid.c_str()));
              String ^ msInstanceUid(gcnew String(sInstanceUid.c_str()));


              // Study directory name
              String ^ msStudyDirectoryName = ( msMedicalId->Length > 0 ? 
                                            String::Format("{0}_{1}", msMedicalId->Replace(" ","_"), msStudyUid->Replace(".","_")) :
                                            msStudyUid->Replace(".","_"));


              // current date and time
              DateTime now = System::DateTime::Now;

              // series directory name
              String ^ msSeriesDirectoryName =  msSeriesUid->Replace(".","_");

              // Creates the series path
              String ^ msSeriesDirectory(Path::Combine(m_msStoragePath, msStudyDirectoryName , msSeriesDirectoryName));
              String ^ msDicomPath(Path::Combine(msSeriesDirectory, "dicom"));

              // ===============================================
              // If the directory does not exist then create it
              // ===============================================
              if ( ! Directory::Exists(msDicomPath) )
                Directory::CreateDirectory(msDicomPath);
        
  
              pArgs->FilePath = Path::Combine(msDicomPath, msInstanceUid->Replace(".","_") + ".dcm" );

        
            } // END ... If the storage path was set

            // On Image Received
            OnImageReceived(pArgs);

            // ========================================================================================
            // If the FILEPATH was set and does not already exists then save the dicom image received
            // ========================================================================================
            if ( ! String::IsNullOrEmpty(pArgs->FilePath) && ! File::Exists(pArgs->FilePath ) )
             pDcmImage->Save(ss(pArgs->FilePath));
            

            // Fire a file saved event which allows for the saving of the thumbnail or post process
            OnFileSaved(pArgs);

            timer.Trace("Server", "Callback", "Receiving image");
          } 

          break;

        case dcm::DcmEvents::E_STARTED:
          OnAssociationStarted(pArgs);
          break;

        case dcm::DcmEvents::E_FAILED:
          OnAssociationFailed(pArgs);
          break;

        case dcm::DcmEvents::E_COMPLETED:
          OnAssociationCompleted(pArgs);
          break;

        case dcm::DcmEvents::E_ABORTED:
          OnStopped(pArgs);
          break;

        }; // END ... Switch on the event type
      

        break;
      }
    case dcm::DcmCommands::C_MOVE:             // Move Command
     {

        if ( pParams->EventType == dcm::DcmEvents::E_PROGRESS )
        {


          /// Fire the move received event
          OnMoveReceived(pArgs);

          if ( pArgs->Results->Length > 0 )
          {

            int iSize(pArgs->Results->Length + 1);
            pParams->Command.Move.Directories = new char [iSize];
            RtlZeroMemory(pParams->Command.Move.Directories, iSize);

            for ( int i(0); i < pArgs->Results->Length; i++ )
            {
              pParams->Command.Move.Directories[i] = (char)pArgs->Results[i];
            }
          }
        }

      }
      break;
    case dcm::DcmCommands::C_GET:              // Get COmmand
      {
        break;
      }
    case dcm::DcmCommands::C_FIND:             // Find/Query command
      {

        if ( pParams->EventType == dcm::DcmEvents::E_PROGRESS )
        {


          /// Fire the query received event
          OnQueryReceived(pArgs);

          if ( pArgs->Results->Length > 0 )
          {

            int iSize(pArgs->Results->Length + 1);
            pParams->Command.Find.Results = new char [iSize];
            RtlZeroMemory(pParams->Command.Find.Results, iSize);

            for ( int i(0); i < pArgs->Results->Length; i++ )
            {
              pParams->Command.Find.Results[i] = (char)pArgs->Results[i];
            }
          }
        }

      }
      break;

    };  // END ... swicth command

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(gcnew String((*m_pDcmServer)->GetLogSource().c_str()), "Exception detected during the handling of the callback : " + ex->Message, "Server","Callback");
    
    throw dcm::DcmException(LogRec(ss(ex->Message),"Server","Callback"));
  }

} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);


#pragma region properties

/**
 * Gets the port for this server
 */
String ^ Server::Id::get()
{
  return gcnew String((*m_pDcmServer)->GetThreadId().c_str());
} // Id::get()

/**
 * Gets the port for this server
 */
unsigned int Server::Port::get()
{
  return (*m_pDcmServer)->GetPort();
} // Port::get()


/**
 * Gets the AETitle for this server
 */
String ^ Server::AETitle::get()
{
  return gcnew String((*m_pDcmServer)->GetAETitle().c_str());
} // AETitle::get();

/**
 * Gets the network timeout
 */
unsigned int Server::NetworkTimeout::get()
{
  return (*m_pDcmServer)->GetNetworkTimeout();
} // NetworkTimeout::get()



/**
 * Gets the MAX PDU
 */
int Server::MaxPDU::get()
{
  return (*m_pDcmServer)->GetMaxPDU();
} // MaxPDU::get()


#pragma endregion
 