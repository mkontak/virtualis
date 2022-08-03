#include "stdafx.h"
#include "vxTransportLayer.h"


using namespace io;


/**
 * Constructor
 *
 * Default transport layer is local
 */
TransportLayer::TransportLayer() :
m_uCacheSize(0)
{
}

/**
 * Sets the TransportLayer to a network based on the type
 *
 * @param sSettings     Settings as an XML string
 */
void TransportLayer::Initialize(const std::string & sXml)
{
  xml::XmlDocument doc;

  doc.LoadXml(sXml);


  unsigned int uPort(0);
  std::string sUrl("");
  std::string sAddress("");
  std::string sServer("");

  std::string sProtocol("local");           // Protocal ... default to local
  std::string sPort("0");                   // Default the port to 80 standard or http
  std::string sCacheSize("0");              // Cache size

  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("protocol", "value", sProtocol);
  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("url", "value", sUrl);
  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("address", "value", sAddress);
  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("server", "value", sServer);
  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("port", "value", sPort);
  doc.GetDocumentElement().TryGetNodeAttributeValue<std::string>("cachesize", "value", sCacheSize);

  // ================================
  // Convert the cache size to integer
  // ==============================
  try { m_uCacheSize = atoi(sCacheSize.c_str()); } catch ( ... )  { throw ex::InvalidDataException(LogRec("Invalid cache size specified", "TransportLayer","Initialize")); }

  if ( ! sUrl.empty() )
    SetUrl(Url(sUrl));
  else
  {
    sProtocol =  tolower(sProtocol);
     
    // ================================
    // Convert the port to integer
    // ==============================
    try { uPort = atoi(sPort.c_str()); } catch ( ... )  { throw ex::InvalidDataException(LogRec("Invalid port specified", "TransportLayer","Initialize")); }


    Protocols protocol(Protocols::P_LOCAL);

    if ( sProtocol == "tcp" )
      SetUrl(Url(Protocols::P_TCP, sServer, sAddress, uPort));
    else if ( sProtocol == "http" )
      SetUrl(Url(Protocols::P_HTTP, sServer, sAddress, uPort));
    else if ( sProtocol == "https" )
      SetUrl(Url(Protocols::P_HTTPS, sServer, sAddress, uPort));
    else if ( sProtocol == "ssl" )
      SetUrl(Url(Protocols::P_SSL, sServer, sAddress, uPort));
    else if ( sProtocol == "local" )
      SetUrl(Url(Protocols::P_LOCAL, sServer, sAddress, uPort));
    else
      throw ex::InvalidDataException(LogRec("Invalid transport protocol specified", "TransportLayer","Set"));

    LogFyi(util::Strings::Format("Setting transport layer to %s (Url: %s  Buffer: %d)", sProtocol, m_url.ToString().c_str(), m_uCacheSize),"TransportLayer","Initialize");

  }
 
} // Initialize(const std::string & sXml)




/**
 * Returns the current TransportLayer singleton
 */
TransportLayer & TransportLayer::GetCurrentTransportLayer()
{ 

  static TransportLayer transportLayer; 

  return transportLayer; 

} // GetCurrentTransportLayer()
