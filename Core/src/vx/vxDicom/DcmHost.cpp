// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmHost.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// includes
#include "Stdafx.h"
#include "DcmHost.h"
#include "DcmUtility.h"

// using
using namespace dcm;


/**
 * Constructor
 */
DcmHost::DcmHost() :
m_sName(""),
m_sServer(""),
m_sAETitle(""),
m_iPort(0),
m_servicesMask(ST_ALL)
{
} // DcmHost()


/**
* Creates the Host object 
*/
std::shared_ptr<DcmHost> DcmHost::Create()
{

  std::shared_ptr<DcmHost> pDcmHost(new DcmHost());


  // Make sure the allocation was successful
  if (pDcmHost == __nullptr)
    throw DcmOutOfMemoryException(LogRec("Failed to allocate memory for DcmHost", "DcmHost", "Create"));

  return pDcmHost;

} //Create(const std::string & sName)

/**
 * Creates the DcmHost from the parameters specified
 *
 * @param name      Host name
 * @param aetitle   AE Title of host
 * @param server    server name/ip
 * @param port      Port
 *
 * @return DcmHost
 */
std::shared_ptr<DcmHost> DcmHost::Create(const std::string & sName, const std::string & sAETitle, const std::string & sServer, unsigned int iPort)
{
  // Create the Host
  std::shared_ptr<DcmHost> pDcmHost(Create());

  pDcmHost->m_sName = sName;
  pDcmHost->m_sAETitle = sAETitle;
  pDcmHost->m_sServer = sServer;
  pDcmHost->m_iPort = iPort;

  return pDcmHost;

}

/**
 * Creates the Host object from the XML
 *
 * @param pNode     XML node to parse
 */
std::shared_ptr<DcmHost> DcmHost::Create(const xml::XmlNode & node)
{

  // Make sure the XML node is valid
  if ( ! node.IsValid() )
    throw DcmInvalidDataException(LogRec("Invalid host node received, cannot create DcmHost","DcmHost","Create"));

  // Create the Host
  std::shared_ptr<DcmHost> pDcmHost(Create());


  /// Host Name
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "name", pDcmHost->m_sName) )
    throw DcmNotFoundException(LogRec("No name attribute specified", "DcmHost", "Create"));

  /// Server Name/Ip
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "server", pDcmHost->m_sServer) )
    throw DcmNotFoundException(LogRec("No server attribute specified", "DcmHost", "Create"));
 
  /// Server AETitle
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "aetitle", pDcmHost->m_sAETitle) )
    throw DcmNotFoundException(LogRec("No aeTitle attribute for the service node", "DcmHost", "Create"));

  /// Description
  const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("desc", pDcmHost->m_sDescription);

  /// Server Port
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<unsigned int>( "port", pDcmHost->m_iPort) )
    throw DcmNotFoundException(LogRec("No port attribute for the service node", "DcmHost", "Create"));

  /// Service Mask
  if (!const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>("services", pDcmHost->m_servicesMask))
    throw DcmNotFoundException(LogRec("No services attribute specified", "DcmHost", "Create"));


  // Get the attributes node
  xml::XmlNode attributesNode( const_cast<xml::XmlNode &>(node).SelectSingleNode("attributes") );

  // =================================
  // Make sure that attributes exists
  // =================================
  if ( attributesNode.IsValid() && attributesNode.HasChildren() )
  {

    /// Get the list of child node values
    for (xml::XmlNode child = attributesNode.GetFirstChild(); child.IsValid(); child = attributesNode.GetNextChild())
    {
      // ===========================================================
      // Check for children and if the node is an element node
      // ==========================================================
      if ( child.GetType() == xml::XML_ELEMENT_NODE && child.HasChildren() )
        pDcmHost->m_attributes.insert(std::pair<std::string, std::string>(child.GetName(), child.GetFirstChild().GetContent() ));
        

    } // END ... For each child

  } // END ... If there are attributes
  
  // Return the Host
  return pDcmHost;

} // Create(xmlNodePtr pNode)


/**
 * Creates the Host object from the XML
 *
 * @param sXml      XML string
 */
std::shared_ptr<DcmHost> DcmHost::Create(const std::string & sXml)
{

  std::shared_ptr<DcmHost> pDcmHost(__nullptr);

  xml::XmlDocument doc;
  doc.LoadXml(sXml);
    
  pDcmHost = Create(doc.GetDocumentElement());
  

  // Make sure the allocation was successful
  if ( pDcmHost == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate memory for DcmHost","DcmHost","Create"));

  return pDcmHost;

} //Create(const std::string & sName)


/**
 * Generates the xml representation of this host
 *
 * @return xml std::string
 */
std::string DcmHost::ToXml()
{
 
  std::ostringstream os;

  // Open Host
  os <<  "<host name=\"" << m_sName << "\" desc=\"" << m_sDescription << "\" aetitle=\"" << m_sAETitle << "\" server=\"" << m_sServer << "\" port=\"" << m_iPort << "\" services=\"" << m_servicesMask + "\" ";

  if ( m_attributes.size() > 0 )
  {
    // Close Host
    os << " >";

    // Open the Attributes
    os << "<attributes>";

    // Add attributes
    std::for_each ( m_attributes.begin(), m_attributes.end(), [ &os ] ( std::pair<std::string,std::string> attribute )
    {
      os << "<" << attribute.first << ">" << attribute.second << "</" << attribute.first << ">";
    });

    // Close attributes and host
    os << "</attributes></host>";

  } // END ... If there at
  else
  {
    // Close Host
    os << " />";
  }

  return os.str();

} // ToXml()



/**
 * Sets the Called AE tITle
 *
 * @param sAETitle      AE TItle of the server
 */
void DcmHost::SetAETitle(const std::string  & sAETitle)
{
  /// Make sure the AE Title was specified
  if ( sAETitle.empty() )
    throw DcmInvalidDataException(LogRec("Server's AE Title must be specified", "DcmHost", "SetAETitle"));

  if ( ! DcmUtility::IsValidXmlString(sAETitle) )
      throw DcmInvalidDataException(LogRec("Server's AE Title cannot contain a '<' or '>' character", "DcmHost", "SetAETitle"));

  m_sAETitle = sAETitle;

} // SetAETitle(const std::string  & sAETitle)


/**
 * Sets the servers name
 *
 * @param sServer     Server network name or ip
 */
void DcmHost::SetServer(const std::string  & sServer)
{
  /// Make sure the server name is not blank
  if ( sServer.empty() )
    throw DcmInvalidDataException(LogRec("Servers name/ip must be specified", "DcmHost", "SetServer"));

  if ( ! DcmUtility::IsValidXmlString(sServer) )
      throw DcmInvalidDataException(LogRec("Server's name cannot contain a '<' or '>' character", "DcmHost", "SetServer"));

  m_sServer = sServer;

} // SetServer(const std::string  & sServer)

/**
* Sets the servers description
*
* @param sDescription     description
*/
void DcmHost::SetDescription(const std::string  & sDescription)
{

  if (!DcmUtility::IsValidXmlString(sDescription))
    throw DcmInvalidDataException(LogRec("Server's description cannot contain a '<' or '>' character", "DcmHost", "SetDescription"));

  m_sDescription = sDescription;

} // SetDescription(const std::string  & sDescription)


/**
 * Sets the servers name
 *
 * @param iPort     Server's listening port
 */
void DcmHost::SetPort(unsigned int iPort)
{
  /// Make sure the servers port is not 0
  if ( iPort == 0 )
    throw DcmInvalidDataException(LogRec("Server's port must be specified", "DcmClientParameters", "SetPort"));

  m_iPort = iPort;

} // SetPort(unsigned int iPort))



/**
 * Validates the servers connection information
 */
void DcmHost::Validate(const dcm::DcmServiceTypes & servicesSupported)
{
  /// Make sure the servers port is not 0
  if ( m_iPort == 0 )
    throw DcmInvalidDataException(LogRec("Server's port must be specified", "DcmHost", "Validate"));

  /// Make sure the AE Title was specified
  if ( m_sAETitle.empty() )
    throw DcmInvalidDataException(LogRec("Server's AE Title must be specified", "DcmHost", "Validate"));

  /// Make sure the server name was specified
  if ( m_sServer.empty() )
    throw DcmInvalidDataException(LogRec("Servers name/ip must be specified", "DcmHost", "Validate"));

  // ====================================================
  // Make sure the host supports the services specified
  // =====================================================
  if ( ! Supports(servicesSupported) )
    throw DcmUnsupportedException(LogRec(util::Strings::Format("Host does not support (%04x)",(short)servicesSupported), "DcmHost", "Validate"));

} // Validate()

/**
 * Tries and gets the specified attribute key
 *
 * @param   sKey      Key to lookup
 * @param   sValue    Value to return
 *
 * @return True of found, false if not
 */
bool DcmHost::TryGetAttributeValue(const std::string & sKey, std::string & sValue)
{
  bool found = false;

  std::map<std::string, std::string>::const_iterator iter = m_attributes.find(sKey);

  if (iter != m_attributes.end())
  {
    sValue.assign(iter->second);
    found = true;
  }

  return found;
} //

/**
 * Determines if the attrbute is the value specified
 *
 * @param sKey      Key to find
 * @param sValue    Value to compare to
 *
 * @return Returns true if a match, false if not
 */
bool DcmHost::IsAttribute(const std::string & sKey, const std::string & sValue)
{
  bool isAttribute(false);
  std::string sAttributeValue;

  if (TryGetAttributeValue(sKey, sAttributeValue))
  {
    isAttribute = (sValue == sAttributeValue);
  }

  return isAttribute;
}