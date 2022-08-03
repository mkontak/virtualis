 // Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmHost.h
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


#define AttributeCollection std::map<std::string, std::string>

// forward declarations
class DCM_DLL DcmHostAttribute;



/**
  * Implements a host entry from the table
  */
class DCM_DLL DcmHost
{
protected:

  // constructor
  DcmHost();


public:

  /// Creates the 
  static std::shared_ptr<DcmHost> Create();

  /// Creates the DcmHost from the parameter list
  static std::shared_ptr<DcmHost> Create(const std::string & sName,  const std::string & sAETitle, const std::string & sServer, unsigned int iPort);


  /// Creates the DcmHost from the XML node
  static std::shared_ptr<DcmHost> Create(const xml::XmlNode & node);

  /// Creates the DcmHost from the XML string
  static std::shared_ptr<DcmHost> Create(const std::string & sXml);

  /// Creates the XML from the object
  std::string ToXml();
 
  /// Returns true if the host supports the types specified
  inline bool Supports(const DcmServiceTypes & eType)
  { return (  ( m_servicesMask & (char)eType ) == eType ); }
 

  /// Validates the host ( will validate against a specified or mask of services )
  void Validate(const dcm::DcmServiceTypes & servicesSupported = ST_ALL);

  /// Try and get attribute value
  bool TryGetAttributeValue(const std::string & sKey, std::string & sValue);

  inline void AddAttribute(const std::string & sKey, const std::string & sValue)
  {
    m_attributes.insert(std::pair<std::string, std::string>(sKey, sValue)); 
  }


private:


  /// Determines if the attribute of type flag (true/false)
  bool IsAttribute(const std::string & sName, const std::string & sValue);


#pragma region properties

public:

  /// Gets the Name
  const std::string & GetName() const
  { return m_sName; }

  /// Sets the name
  void SetName(const std::string & sName)
  { m_sName = sName; }

  /// Gets the description
  const std::string & GetDescription()
  { return m_sDescription; }

  /// Sets the description
  void SetDescription(const std::string & sDescription);


  /// Gets the server name or ip
  const std::string & GetServer() 
  { return m_sServer; }

  /// Sets the server
  virtual void SetServer(const std::string & sServer);

  /// Gets the port
  const unsigned int GetPort() const
  { return m_iPort; }

  /// Sets the port
  void SetPort(unsigned int iPort);

  /// Gets the AE Title
  const std::string & GetAETitle() const
  { return m_sAETitle; }

  /// Sets the AE Title
  void SetAETitle(const std::string & sAETitle);

  /// Gets the service mask
  unsigned int GetServicesMask() const
  { return m_servicesMask; }

  /// Sets the service mask
  void SetServicesMask(const char & servicesMask)
  { m_servicesMask = servicesMask; }


  /// Gets all the attributes 
  AttributeCollection & GetAttributes()
  { return m_attributes; }


  /// true if the host supports encapsulated pdf
  inline const bool IsEncapsulatedPdfSupported()
  { return IsAttribute(DCM_SupportsEncapsulatedPdf,"true"); }

  /// true if the host supports series level queries
  inline const bool IsSeriesLevelQuerySupported()
  { return IsAttribute(DCM_SupportsSeriesLevelQuery,"true"); }

  /// true if the host supports get command
  inline const bool IsGetCommandSupported()
  { return IsAttribute(DCM_SupportsGetCommand,"true"); }


#pragma endregion


private:

  /// Hosts internal name
  std::string m_sName;

  /// Server Network Name or IP
  std::string m_sServer;

  /// Description
  std::string m_sDescription;

  /// Services
  int m_servicesMask;

  /// AE Title
  std::string m_sAETitle;

  /// Port
  unsigned int m_iPort;

  /// Attributes
  AttributeCollection m_attributes;


}; // class DcmHost

}; // namespace dcm

#pragma warning(pop)