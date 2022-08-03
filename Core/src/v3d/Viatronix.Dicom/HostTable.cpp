// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HostTable.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// Includes
#include "StdAfx.h"
#include "Host.h"
#include "HostTable.h"



// Namespaces

using namespace System;
using namespace System::Xml;
using namespace System::Data;
using namespace Viatronix;
using namespace Viatronix::Logging;


USING_DICOM_NS




/**
 * Constructor
 */
HostTable::HostTable()
{
  
} // HostTable


/**
 * Gets the current host table
 */
HostTable ^ HostTable::CurrentHostTable::get()
{
 
  if ( m_mpCurrentHostTable == nullptr )
    m_mpCurrentHostTable = Create();

  return m_mpCurrentHostTable;
} // 


/**
 * Loads the host table. If it is loaded it will force a reload.
 */
HostTable ^ HostTable::Create()
{

  HostTable ^ mpTable = nullptr;

  if ( mpTable == nullptr )
  {
    mpTable = gcnew HostTable();

    if ( mpTable == nullptr )
      throw gcnew OutOfMemoryException("Failed to allocate HostTable");

  }

  try
  {
    mpTable->m_hostTable.reset(new std::shared_ptr<dcm::DcmHostTable>(dcm::DcmHostTable::GetCurrentHostTable()));
   
  }
  catch ( dcm::DcmNotFoundException & ex )
  {
    throw gcnew System::IO::FileNotFoundException(gcnew String(ex.GetTextMessage().c_str()));
  }
  catch ( ... )
  {
    throw gcnew System::IO::IOException("Failed to get host table");
  }
 
  return mpTable;

} // Create()

/**
 * gets all the host that support the service specified
 */
Generic::List<Host ^> ^ HostTable::GetHosts(ServiceTypes eServiceType)
{
 
  Generic::List<Host ^> ^ mpHosts(gcnew Generic::List<Host ^>());

  try
  {
    std::vector<std::shared_ptr<dcm::DcmHost>> hosts = (*m_hostTable)->GetHosts(static_cast<dcm::DcmServiceTypes>(eServiceType));

    for ( std::vector<std::shared_ptr<dcm::DcmHost>>::iterator it = hosts.begin(); it != hosts.end(); it++ )
    {
      mpHosts->Add(Host::Create((*it)));
    }
  }
  catch(dcm::DcmException & )
  {
    Viatronix::Logging::Log::Warning("Failed in retrieving the host table.  Please ensure that it exists.", "HostTable", "GetHosts");
  }

  return mpHosts;
} // GetHosts(String ^ msServiceType)
  
/**
 * Gets the host by the specified name
 * 
 * @param sName   Name
 *
 * @return Host
 */
Host ^ HostTable::GetHostByName(String ^ sName)
{
  return Host::Create((*m_hostTable)->GetHostByName(ss(sName)));
} // GetHostByName(String ^ sName)

/**
 * Gets the host by the specified aetitle
 * 
 * @param sAETitle   AE Title
 *
 * @return Host
 */
Host ^ HostTable::GetHostByAETitle(String ^ sAETitle)
{
 return Host::Create((*m_hostTable)->GetHostByAETitle(ss(sAETitle)));
} // GetHostByAETitle(String ^ sAETitle)



