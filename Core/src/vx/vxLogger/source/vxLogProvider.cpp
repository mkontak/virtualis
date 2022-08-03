// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogProvider.cpp
//
// Description: This is the provider needed for using the ETW logging within the 
//              Windows OS. It is only supported in Windows 7 and up. You will
//              need to have performance logging security for the user to use
//              this type of logging. Removing WINXP in the preprocessor 
//              list will enabled ETW logging, however you will still be able to
//              specify the classic style.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogProvider.h"
#include "vxLogUtility.h"
#include "vxLogRecord.h"
#include "vxLogExceptions.h"

// ==============================================================
// If WINXP is not defined then we use ETW logging that is only
// supported on WINDOWS 7.
// =============================================================
#ifndef WINXP


/**
 * Constructor
 *
 * @param sName   Name of the provider
 */
vxLogProvider::vxLogProvider(const std::string & sName) :
m_sHandle(0),
m_sName(sName),
m_eSeveritylevel(vxLogRecord::SL_VERBOSE),
m_flags(0),
m_bTracingOn(FALSE)
{

  CoCreateGuid(&m_guid); 

  m_sGuid = util::Strings::ToString(m_guid);
 

} // vxLogProvider()


/**
 * Destructor
 *
 * Unregisters the provider from the system
 */
vxLogProvider::~vxLogProvider()
{
  // Unregisters 
  Unregister();

} // ~vxLogProvider()


/**
 * Registers the provider
 */
void vxLogProvider::Register()
{

  ULONG status(ERROR_SUCCESS);

  // ====================================================
  // Make sure the provider is not already registered
  // ====================================================
  if ( m_sHandle == 0 )
  {
 
    LogDbg(util::Strings::Format(std::string(""),"Registering provider %s", m_sGuid.c_str()), "vxLogProvider", "Register");

    status = EventRegister(  &m_guid,                                               // GUID that identifies the provider
                             (PENABLECALLBACK)&vxLogProvider::EnableCallback,       // Callback 
                             (LPVOID)this,                                          // Context not used
                             &m_sHandle );                                           // Used when calling EventWrite and EventUnregister
               
    // =============================================
    // Check if the registration was successful
    // ============================================
    if (status != ERROR_SUCCESS )
    {
      throw vxLogRegistrationException(LogRec(util::Strings::Format(std::string(""),"Registering of %s failed [%ld]",m_sGuid.c_str(), status), "vxLogProvider", "Register"));
      Unregister();
    }  // END .. If the registration failed


    LogDbg(util::Strings::Format(std::string(""),"Provider %s assigned sHandle %ld", m_sGuid.c_str(), (ULONG)m_sHandle), "vxLogProvider", "Register");
    

  } // END ... If the provider has not already been registered

} // Register()

 /**
  * Unregisters the provider
  */
 void vxLogProvider::Unregister()
 {
  if ( m_sHandle != 0 )
  {
    LogDbg(util::Strings::Format(std::string(""),"Unregistering provider %s", m_sGuid.c_str()),"vxLogProvider", "Unregister");

    ULONG status = EventUnregister(m_sHandle);

    if ( status != ERROR_SUCCESS )
      LogDbg(util::Strings::Format(std::string(""),"Unregistering of provider %s failed [%ld]", m_sGuid.c_str(), status),"vxLogProvider", "Unregister");
  }

  m_sHandle = 0;

} // Unregister()

/**
 * Enable callback
 *
 */
void vxLogProvider::EnableCallback(LPCGUID SourceId, BOOL IsEnabled, UCHAR Level, ULONGLONG MatchAnyKeyword, ULONGLONG MatchAllKeyword, PEVENT_FILTER_DESCRIPTOR FilterData, PVOID CallbackContext )
{
  vxLogProvider * pProvider = reinterpret_cast<vxLogProvider *>(CallbackContext);

  pProvider->m_bTracingOn = IsEnabled;

} // EnableCallback(LPCGUID SourceId, BOOL IsEnabled, UCHAR Level, ULONGLONG MatchAnyKeyword, ULONGLONG MatchAllKeyword, PEVENT_FILTER_DESCRIPTOR FilterData, PVOID CallbackContext )

/**
 * Writes the event to the provider
 *
 * @param record      Log record to write
 */
void vxLogProvider::WriteEvent(const vxLogRecord & record)
{
  
  ULONG iCount(const_cast<vxLogRecord &>(record).GetEventDataCount());

  PEVENT_DATA_DESCRIPTOR data(const_cast<vxLogRecord &>(record).GetEventData());

  ULONG status = EventWrite(m_sHandle, &const_cast<vxLogRecord &>(record).GetEventDescriptor(), iCount, data);
 
  // Changed to the EvenWrite to utilize the descriptor 
  //ULONG status = EventWriteString(m_sHandle, (UCHAR)(const_cast<vxLogRecord &>(record).GetSeverityLevel()), 0, wxml.c_str());

  // ================================================
  // Only using DEBUG do I dump the log messages
  // ================================================
#ifdef DEBUG
  vxLogUtility::LogMessage(const_cast<vxLogRecord &>(record).ToXml());
#endif

  if (status != ERROR_SUCCESS) 
    vxLogUtility::LogMessage(util::Strings::Format(std::string(""),"EventWrite for provider %s failed [%ld]", m_sGuid.c_str(), status));

} // WriteEvent(const vxLogRecord & record)


#endif
