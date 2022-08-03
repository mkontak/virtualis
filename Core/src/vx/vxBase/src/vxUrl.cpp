#include "stdafx.h"
#include "Exception.h"
#include "vxUrl.h"




/**
 * Default constructor
 */
Url::Url() :
m_protocol(Protocols::P_LOCAL),
m_sServer(""),
m_sPath(""),
m_uPort(0),
m_sUrl("local://")
{
}

/**
 * URL constructor using the full url string
 *    http://server:8000/Eneterprise
 */
Url:: Url(const std::string & sUrl) :
m_sUrl(sUrl),
m_sServer(""),
m_sPath(""),
m_uPort(0),
m_protocol(Protocols::P_LOCAL)
{

  if ( m_sUrl.empty() )
    throw ex::InvalidDataException(LogRec("Invalid url supplied","Url","Url"));


  std::string sProtocol("");
  std::string sPort("0");

  size_t start(0);
  size_t pos = m_sUrl.find(':',start);


  if ( pos != std::string::npos )
  {
    sProtocol = m_sUrl.substr(0, pos);
    start = pos + 1;
  }

  if ( m_sUrl.substr(start,2) != "//" )
    throw ex::InvalidDataException(LogRec("Invalid ursl supplied","Url","Url"));

  start += 2;


  pos = m_sUrl.find('/',start);

  if ( pos != std::string::npos )
  {
    size_t len = pos - start;
    m_sServer = m_sUrl.substr(start, len);
    start = pos + 1;
    pos = m_sServer.find(':',0);

    if ( pos != std::string::npos )
    {
      sPort = m_sServer.substr(pos + 1);
      m_sServer = m_sServer.substr(0,pos);
    }

    
  }

  m_sPath = m_sUrl.substr(start);
  
 
  // ================================
  // Convert the port to integer
  // ==============================
  try { m_uPort = atoi(sPort.c_str()); } catch ( ... )  { throw ex::InvalidDataException(LogRec("Invalid port specified", "Url","Url")); }


  if ( sProtocol == "tcp" )
  {
    m_protocol = Protocols::P_TCP;

    if ( m_sServer.empty() || m_uPort == 0 )
      throw ex::InvalidDataException(LogRec("No server or port specified for TCP transport layer","Url","Url"));


  }
  else if ( sProtocol == "http" )
  {
    if (  m_sServer.empty()  )
      throw ex::InvalidDataException(LogRec("No url specified for HTTP transport layer","Url","Url"));

    if ( m_sUrl.empty() )
      m_sUrl = util::Strings::Format("http://%s%s/%s", 
                                      m_sServer.c_str(), 
                                      ( m_uPort == 0 ? "" : util::Strings::Format(":%d",m_uPort) ),
                                      m_sPath );

    m_protocol = Protocols::P_HTTP;
  }
  else if ( sProtocol == "https" )
  {
    if ( m_sServer.empty()  )
      throw ex::InvalidDataException(LogRec("No url specified for HTTPS transport layer","TransportLayer","Set"));

    if ( m_sUrl.empty() )
      m_sUrl = util::Strings::Format("https://%s%s/%s", 
                                      m_sServer.c_str(), 
                                      ( m_uPort == 0 ? "" : util::Strings::Format(":%d",m_uPort) ),
                                      m_sPath );

    m_protocol = Protocols::P_HTTPS;
  }
  else if ( sProtocol == "local" )
  {
    m_sServer   = "";
    m_sPath  = "";
    m_uPort     = 0;
    m_protocol  = Protocols::P_LOCAL;
  }
  else
    throw ex::InvalidDataException(LogRec("Invalid transport protocol specified", "Url","Url"));

}


/**
 * Constructor using all the pieces of the URL
 *
 * @param protocol      Protocol
 * @param sServer       Server
 * @param sPath         Path 
 * @param uPort         Port
 */
Url::Url(const Protocols & protocol, const std::string & sServer, const std::string & sPath, const unsigned int uPort ) :
m_protocol(protocol),
m_sServer(sServer),
m_uPort(uPort),
m_sPath(sPath)
{

  if ( protocol == P_LOCAL )
  {
    m_sServer = "";
    m_uPort = 0;
    m_sPath = "";
  }
  
  if (  m_sServer.empty() )
    throw ex::InvalidDataException(LogRec("No server specified","Url","Url"));

  /// Build the URL
  m_sUrl = util::Strings::Format("http://%s%s/%s", m_sServer.c_str(), 
                                 ( m_uPort == 0 ? "" : util::Strings::Format(":%d",m_uPort), m_sPath ));


}


/**
 * Copy constructor
 *
 * @param  url      URL object to copy
 */
Url::Url(const Url & url) :
m_sServer(url.m_sServer),
m_sUrl(url.m_sUrl),
m_uPort(url.m_uPort),
m_protocol(url.m_protocol),
m_sPath(url.m_sPath)
{
}