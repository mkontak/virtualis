// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HostTable.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



// Pragmas
#pragma once

#include "Enums.h"
#include "scoped_ptr.h"

using namespace System;
using namespace System::Collections;




// Namespaces
OPEN_DICOM_NS

// Forward Declarations
ref class Host;




/**
  * Implements the Dicom Host Table (Collection of hosts entries)
  */
public ref class HostTable
{
private:

  /// Constructor
  HostTable();

public:

 
  /// Creates the table using the defaults
  static HostTable ^ Create();

  /// Returns the list of host that support the service listed
  Generic::List<Host ^> ^ GetHosts(ServiceTypes  eServiceType);
  Generic::List<Host ^> ^ GetHosts()
  { return GetHosts(ServiceTypes::ANY); }
 
  /// Gets the host by the specified name 
  Host ^ GetHostByName(String ^ sName);

  /// Gets the host by the specified ae title
  Host ^ GetHostByAETitle(String ^ sAETitle);



  /// Current host Table
  static property HostTable ^ CurrentHostTable
  { HostTable ^ get(); };


private:

  scoped_ptr<std::shared_ptr<dcm::DcmHostTable>> m_hostTable;

  /// Host table
  static HostTable ^ m_mpCurrentHostTable;


};  // class HostTable

CLOSE_DICOM_NS

