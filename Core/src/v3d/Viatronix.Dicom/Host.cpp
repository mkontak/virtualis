// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Host.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



// Includes
#include "StdAfx.h"
#include "Enums.h"
#include "Host.h"
#include "scoped_ptr.h"


// Namespaces
using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Data;
using namespace System::Text;
using namespace Viatronix;
using namespace Viatronix::Logging;


USING_DICOM_NS


/**
 * Constructor
 */
Host::Host() 
{
  m_host.reset(new std::shared_ptr<dcm::DcmHost>(dcm::DcmHost::Create()));
} // Host()





/**
 * Returns true if the entry supports the specified type
 *
 * @param sType     Type to check support for
 *
 * @return true if the host supports the type specified
 */
bool Host::Supports(ServiceTypes eServiceType)
{

  bool bSupports(false);
 
  return (*m_host)->Supports(static_cast<dcm::DcmServiceTypes>(eServiceType));

} // Supports(String ^ msServiceType)




/**
 * Creates the managed host from the unmanaged
 */
Host ^ Host::Create(std::shared_ptr<dcm::DcmHost> & dcmHost)
{ 
  Host ^ mpHost(gcnew Host());

  if ( mpHost == nullptr )
    throw gcnew System::OutOfMemoryException("Failed to allocation Host object");

  mpHost->m_host.reset(new std::shared_ptr<dcm::DcmHost>(dcmHost)); 

  return mpHost;
} // Create(std::shared_ptr<dcm::DcmHost> & dcmHost)


/**
 * Creates the managed host from the unmanaged
 */
Host ^ Host::Create(System::Xml::XmlNode ^ mpNode)
{ 
  Host ^ mpHost(gcnew Host());

  if ( mpHost == nullptr )
    throw gcnew System::OutOfMemoryException("Failed to allocation Host object");

  mpHost->m_host.reset(new std::shared_ptr<dcm::DcmHost>(dcm::DcmHost::Create(ss(mpNode->OuterXml)))); 

  return mpHost;
} // Create(std::shared_ptr<dcm::DcmHost> & dcmHost)

/**
 * Gets the host name
 */
String ^ Host::Name::get()
{
  return gcnew String((*m_host)->GetName().c_str());
} // Name::get()

/**
 * Sets the host name
 */
void Host::Name::set(String ^ msName)
{
  (*m_host)->SetName(ss(msName));
} // Name::set()



/**
 * Gets the server name
 */
String ^ Host::Server::get()
{
  return gcnew String((*m_host)->GetServer().c_str());
} // Server::get()



/**
 * Gets the server name
 */
void Host::Server::set(String ^ msServer)
{
  (*m_host)->SetServer(ss(msServer));
} // Server::set()

/**
 * Gets the hosts AE Title
 */
String ^ Host::AETitle::get()
{
  return gcnew String((*m_host)->GetAETitle().c_str()); 
} // AETitle::get()

/**
 * Gets the hosts AE Title
 */
void Host::AETitle::set(String ^ msAETitle)
{
  (*m_host)->SetAETitle(ss(msAETitle)); 
} // AETitle::set()

/**
 * Gets the hosts port
 */
unsigned int Host::Port::get()
{
  return (*m_host)->GetPort();
} // Port::get()

/**
 * Gets the hosts port
 */
void Host::Port::set(unsigned int iPort)
{
  (*m_host)->SetPort(iPort);
} // Port::set()

/**
 * Creates the XML string for the service
 *
 * @return Returns the XML representation of the service
 */
String ^ Host::ToXml()
{
  return gcnew String((*m_host)->ToXml().c_str()); 
} // ToXml()

/**
 * Validates the data
 *
 */
void Host::Validate()
{
  (*m_host)->Validate(); 
} // Validate()

/**
 * Gets the list f attributes for the host
 *
 * @return Returns a list of host attributes
 */
Dictionary<String ^, String ^> ^ Host::GetAttributes()
{

  Dictionary<String ^, String ^> ^ list = gcnew Dictionary<String ^, String ^>();

  std::map<std::string, std::string> attributes = (*m_host)->GetAttributes();

  for ( std::map<std::string, std::string>::iterator it = attributes.begin(); it != attributes.end(); it++ )
  {
    list->Add(gcnew String(it->first.c_str()), gcnew String(it->second.c_str()));
  }

  return list;
} // GetAttributes()


/**
 * Returns true if the entry supports the specified type
 *
 * @return true if the host supports encapsulated pdf
 */
bool Host::SupportsEncapsulatedPdf::get()
{

  bool bSupports(false);
 
  return (*m_host)->IsEncapsulatedPdfSupported();

} // SupportsEncapsulatedPdf::get()

/**
 * Returns true if the entry supports the specified type
 *
 * @return true if the host supports get command
 */
bool Host::SupportsGetCommand::get()
{

  bool bSupports(false);
 
  return (*m_host)->IsGetCommandSupported();

} // SupportsGetCommand::get()

/**
 * Returns true if the entry supports the specified type
 *
 * @return true if the host supports series level query
 */
bool Host::SupportsSeriesLevelQuery::get()
{

  bool bSupports(false);
 
  return (*m_host)->IsSeriesLevelQuerySupported();

} // SupportsSeriesLevelQuery::get()


/**
 * Returns true if the entry supports the specified type
 *
 * @return Services map
 */
ServiceTypes Host::ServicesMask::get()
{
 
  return (ServiceTypes)(*m_host)->GetServicesMask();

} // Services::get()


/**
 * Returns true if the entry supports the specified type
 *
 * @return Services map
 */
void Host::ServicesMask::set(ServiceTypes eServices)
{
 
  return (*m_host)->SetServicesMask((dcm::DcmServiceTypes)eServices);

} // Services::get()

/**
 * Try and get the attribute value
 */
bool Host::TryGetAttributeValue(String ^ sKey, String ^ sValue)
{

  std::string ssValue;

  bool found = (*m_host)->TryGetAttributeValue(ss(sKey), ssValue);

  if (found)
    sValue = gcnew String(ssValue.c_str());

  return found;
}
