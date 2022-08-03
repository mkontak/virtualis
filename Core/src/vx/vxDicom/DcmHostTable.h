// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmHostTable.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "DcmEnums.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


// namespace
namespace dcm
{

// forward declarations
class DCM_DLL DcmHost;


/**
 * Implements a host table
 */
class DCM_DLL DcmHostTable
{
private:

  /// constructor
  DcmHostTable() { }

public:

  /// Creates the table using the file specified
  static std::shared_ptr<DcmHostTable> Create(const std::string & sXml);

  /// Creates the host table using the xml node
  static std::shared_ptr<DcmHostTable> Create(const xml::XmlNode & node);

  /// Gets the host by the name specified
  std::shared_ptr<DcmHost> GetHostByName(const std::string & sName);

  /// Gets the host by the name specified
  std::shared_ptr<DcmHost> GetHostByAETitle(const std::string & sAETItle);

  /// Tries to get the host and returns true or false
  bool TryGetHostByName(const std::string & sName,  std::shared_ptr<DcmHost>  & pHost );

  /// Tries to get the host and returns true or false
  bool TryGetHostByAETitle(const std::string & sName,  std::shared_ptr<DcmHost> & pHost );

  /// Returns a list of hosts that support a particular service
  std::vector<std::shared_ptr<DcmHost>> GetHosts(DcmServiceTypes eSupports = ST_ANY);

  /// Adds host to the lists
  std::shared_ptr<DcmHost> & AddHost(std::shared_ptr<DcmHost> & pHost);


  /// Xml representation of the host table
  std::string ToXml();

  

  /// Gets the current host table
  static std::shared_ptr<DcmHostTable> & GetCurrentHostTable();


private:

  /// Loads the file path
  void Load(const std::string & sXml);

 /// Loads the file path
  void Load(const xml::XmlNode & node);

private:

  /// Hosts by Name
  std::map<std::string, std::shared_ptr<DcmHost>> m_hostsByName;

  /// Hosts by AETItle
  std::map<std::string, std::shared_ptr<DcmHost>> m_hostsByAETitle;
 
};  // class DcmHostTable

};  // namespace dcm

#pragma warning(pop)