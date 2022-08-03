// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CGet.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "CGet.h"
#include "Host.h"
#include <vector>


// usings
using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;
USING_DICOM_NS
 
/**
 * Constructor
 */
CGet::CGet() :
Client(),
m_pDcmGet(scoped_ptr<std::shared_ptr<dcm::DcmGet>>(__nullptr))
{

  m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &CGet::Callback);

} // Get()


/** 
 * Starts the store
 */
void CGet::Start(String ^ sCallbackData)
{

 

  // ==============================================
  // Make sure the thread has not already started
  // =============================================
  if ( ! (*m_pDcmGet)->IsRunning() )
  {
    // Allocate an new handle for the callback delegate
    GCHandle gh = GCHandle::Alloc(m_mpCallbackDelegate);

    // Marshall the function pointer for unmanaged usage
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

    // Cast the int pointer the unmanaged callback function pointer for usage in the call
    dcm::DCM_CALLBACK_FUNCTION callback = static_cast<dcm::DCM_CALLBACK_FUNCTION>(ip.ToPointer());

    if ( ! String::IsNullOrEmpty(sCallbackData) )
    {
      m_callbackData = new char [sCallbackData->Length + 1];

      int i(0);
      for each ( char ch in sCallbackData )
      {
        m_callbackData[i++] = ch;
      }
      m_callbackData[i] = '\0';


    }

    (*m_pDcmGet)->Start(m_callbackData, callback);

    // Free the global handle
    gh.Free();

    ///Sets the event waqit handle to the thread handle
    m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr((*m_pDcmGet)->GetHandle()), false);

  } // END ... If the store hs not already started

} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void CGet::Abort()
{

  try
  {
    (*m_pDcmGet)->Abort();
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Stop()

/**
 * Waits for the thread
 *
 * @return void.
 */
void CGet::Wait()
{

  try
  {
    (*m_pDcmGet)->Wait(INFINITE);
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Wait()




/**
 * Creates a store object
 */
CGet ^ CGet::Create()
{
  CGet ^ mpGet(gcnew CGet());

  if ( mpGet == nullptr )
    throw gcnew System::OutOfMemoryException("Failed to allocate CGet object");

  return mpGet;
} // Create()

/**
 * Creates a Get object
 *
 * @param mpNode      Xml node
 */
CGet ^ CGet::Create(XmlNode ^ mpNode)
{

  CGet ^ pGet(gcnew CGet());

  try
  {
    pGet->m_pDcmGet.reset( new std::shared_ptr<dcm::DcmGet>( dcm::DcmGet::Create(ss(mpNode->OuterXml))));
  }
  catch ( dcm::DcmConfigurationException & ex )
  {
    throw gcnew ConfigurationException( gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return pGet;

} // Create(XmlNode ^ mpNode)

/**
 * Creates a Get object
 *
 * @param msAETitle       AE Title of this client
 * @param msHostXml      Host XML
 */
CGet ^ CGet::Create(String ^ msAETitle, String ^ msHostXml )
{

  CGet ^ pGet(gcnew CGet());

  try
  {
    pGet->m_pDcmGet.reset(new std::shared_ptr<dcm::DcmGet>(dcm::DcmGet::Create(ss(msAETitle), dcm::DcmHost::Create(ss(msHostXml)))));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return pGet;

} // Create(String ^ msAETitle, String ^ msServersAEtitle, String ^ msServerName, int iPort )

/**
 * Creates a Get object
 *
 * @param msAETitle       AE Title of this client
 * @param mpServer        Server
 */
CGet ^ CGet::Create(String ^ msAETitle, Host ^ mpServer)
{

  CGet ^ pGet(gcnew CGet());

  try
  {
    pGet->m_pDcmGet.reset(new std::shared_ptr<dcm::DcmGet>(dcm::DcmGet::Create(ss(msAETitle), mpServer->GetInternalHost())));
  }
  catch (dcm::DcmException & ex)
  {
    throw gcnew DicomException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return pGet;

} // Create(String ^ msAETitle,  Host ^ mpServer)



/**
 * Adds the move parameters to the list
 * 
 * @param msDestinationAETItle      AE Title of the destination dicom server
 * @param msDestinationPort         Port the destination server is to listen on
 * @param msStudyUid                Study Uid
 * @param msSeriesUid               Series Uid
 * @param msInstanceUid             Instance Uid
 */
void CGet::Add(String ^ msDestinationAETitle, int iDestinationPort, String ^ msStudyUid, String ^ msSeriesUid, String ^ msInstanceUid)
{


  try
  {
    (*m_pDcmGet)->AddParameters(std::make_shared<dcm::DcmGetParameters>(ss(msDestinationAETitle), iDestinationPort, ss(msStudyUid), ss(msSeriesUid), ss(msInstanceUid)));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }
} // Add(String ^ msDestination, int iDestinationPort, String ^ msStudyUid, String ^ msSeriesUid, String ^ msInstanceUid)


/**
 * Adds the move parameters to the list
 * 
 * @param msDestinationAETItle      AE Title of the destination dicom server
 * @param msDestinationPort         Port the destination server is to listen on
 */
void CGet::Add(String ^ msDestinationAETitle, int iDestinationPort, String ^ msPatientId, String ^ msPatientName)
{
  try
  {
    (*m_pDcmGet)->AddParameters(std::make_shared<dcm::DcmGetParameters>(ss(msDestinationAETitle), iDestinationPort, ss(msPatientId), ss(msPatientName)));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }
} // Add(String ^ msDestination, String ^ msPatientId, String ^ msPatientName)




/**
 * Handles the callback from the server
 */
void CGet::Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {
    
   
    /// Create the args
    ClientEventArgs ^ pArgs( ClientEventArgs::Create(IntPtr(pCallbackParameters), IntPtr(lpCallbackData) ));
 
    if ( pArgs->EventType != Events::Started || ( pArgs->EventType == Events::Started && pArgs->CommandType == Commands::Get ) )
      Client::FireEvent(pCallbackParameters, pArgs);

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->LogSource, "Exception detected during the handling of the callback : " + ex->Message, "CGet","Calback");
    throw;
  }


} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

/**
 * Get the server for the client
 *
 * @return server Host object
 */
Host ^ CGet::Server::get()
{ 
  return Host::Create((*m_pDcmGet)->GetServer()); 
} // Server::get()
 
