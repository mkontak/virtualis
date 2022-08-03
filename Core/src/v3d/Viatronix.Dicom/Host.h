// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Host.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


/// Pragmes
#pragma once

#include "Enums.h"
#include "scoped_ptr.h"

/// namespaces
using namespace System;
using namespace System::Xml;
using namespace System::Collections;
using namespace System::Collections::Generic;


OPEN_DICOM_NS

// forward declarations
ref class HostAttribute;

/**
 * class implements a host entry for the Dicom HOSTS table
 */
public ref class Host
{

public:

  /// Constructor
  Host();

   /// Creates the managed host from the unmanaged
  static Host ^ Create(std::shared_ptr<dcm::DcmHost> & dcmHost);


    /// Creates the managed host from the unmanaged
  static Host ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Returns true if the server type is supported
  bool Supports(ServiceTypes eServiceType);

   /// Xml
  String ^ ToXml();

  /// Gets the attribute using the name
  bool TryGetAttributeValue(String ^ sName, String ^ sValue);

  
  /// Gets a list of attributes
  Dictionary<String ^, String ^> ^ GetAttributes();

  /// String representation
  virtual String ^ ToString() override
  { return this->Name; }

  /// Get the internal host
  std::shared_ptr<dcm::DcmHost> GetInternalHost()
  { return (*m_host); }

  /// Validates the entries
  void Validate();

public:

  /// Host name
  property String ^ Name
  { String ^ get(); void set(String ^ msName); }

  /// Host server name / ip
  property String ^ Server
  { String ^ get(); void set(String ^ msServer); }

  /// AE Title
  property String ^ AETitle
  { String ^ get(); void set(String ^ msAETItle); }

  property ServiceTypes ServicesMask
  {
    ServiceTypes get();
    void set(ServiceTypes services);
  }

  /// Port
  property unsigned int Port
  { unsigned int get(); void set(unsigned int iPort); } 

  // flag to indicate if the host supports the encapsulated pdf
  property bool SupportsEncapsulatedPdf
  { bool get(); }

  // flag to indicates if the host supports the get command
  property bool SupportsGetCommand
  { bool get(); }

  /// flag supports the series query level 
  property bool SupportsSeriesLevelQuery
  { bool get(); }

 
private:

    scoped_ptr<std::shared_ptr<dcm::DcmHost>> m_host;
      
}; // class Host



CLOSE_DICOM_NS

