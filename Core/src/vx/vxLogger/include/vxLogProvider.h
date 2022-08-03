// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogProvider.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

//pragma
#pragma warning(push)
#pragma warning(disable:4251)


// =================================================================
// Only expose if we are building fro Windows 7 and greater only.
// =================================================================
#ifndef WINXP


// includes
#include "evntprov.h"
#include "vxLogRecord.h"


/**
 * Implements ETW provider allowing for logging to sessions. 
 */
class VX_LOGGER_DLL vxLogProvider
{

public:

  /// Constructor
  vxLogProvider(const std::string & sName = "");

  /// Destructor
  virtual ~vxLogProvider();

  /// Unregister the provider  
  void Unregister();

  /// Registers the provider
  void Register();

   /// Write the event
  void WriteEvent(const vxLogRecord & record);


private:

  /// Enable callback
  static VOID NTAPI EnableCallback(LPCGUID SourceId, BOOL IsEnabled, UCHAR Level, ULONGLONG MatchAnyKeyword, ULONGLONG MatchAllKeyword, PEVENT_FILTER_DESCRIPTOR FilterData, PVOID CallbackContext );


public:

  /// Gets the provider name
  const std::string & GetName()
  { return ( m_sName.empty() ? m_sGuid : m_sName ); }

  /// Gets the security level for logging
  const vxLogRecord::SeverityEnum GetSecurityLevel() 
  { return m_eSeveritylevel; }

  /// Gets the GUID for this provider
  const GUID & GetGuid() 
  { return m_guid; }

  /// Gets the tracing on flag
  BOOL IsTracingOn() 
  { return m_bTracingOn; }

  /// Gets the GUID as a string
  const std::string & GetGuidAsString()
  { return m_sGuid; }


  /// Registered flag
  bool IsRegistered()
  { return ( m_sHandle != 0 ); }


private:

  /// Providr GUID
  GUID m_guid;

  /// Guid as a string
  std::string m_sGuid;

  // Determines which class of events to log.
  ULONG m_flags; 

  // Determines the severity of events to log.
  vxLogRecord::SeverityEnum m_eSeveritylevel; 

  // Used by the provider to determine whether it should log events.
  BOOL m_bTracingOn;  

  /// Provider tracing sHandle
  REGHANDLE m_sHandle;

  /// Provider name
  std::string m_sName;


};  // class vxLogProvider

#endif

// pragma
#pragma warning(pop)