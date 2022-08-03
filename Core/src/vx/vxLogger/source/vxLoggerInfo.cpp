// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLoggerInfo.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogEnums.h"
#include "vxLoggerInfo.h"
#include "vxUtilIO.h"
#include "vxXmlNode.h"
#include "vxXmlExceptions.h"


// statics
std::string vxLoggerInfo::m_sDefaultFilename("");
vxLogRecord::SeverityEnum vxLoggerInfo::m_eDefaultLogLevel(vxLogRecord::SL_EXCEPTION);



#pragma region constructors

/**
 * Constructor
 *
 * @param   sHandle       Logger name
 * @param   sFilename     Loggers file name
 * @param   eLogLevel     Loggers level 
 * @param   eType         Type of logger
 * @param   sServer       Server name
 * @param   iPort         Servers port
 */
vxLoggerInfo::vxLoggerInfo( const std::string & sHandle, 
                            const std::string & sFilename, 
                            const vxLogRecord::SeverityEnum eLogLevel,
                            const vxLogMessageFormats eFormat,
                            const std::string & sServer,
                            const unsigned int iPort ) :
m_eLogLevel(eLogLevel),
m_sHandle( ( sHandle.empty() ? vxLogUtility::GenerateGuid() : sHandle ) ),
m_sSystem( vxLogUtility::GetHostname() ),
m_sServer(sServer),
m_eFormat(eFormat),
m_iPort(iPort),
m_sExtension(".log")
{

  SetFilename(sFilename);
} // vxLoggerInfo()

/**
 * Constructor
 *
 * @param sXml      Xml representation of the loggers attributes
 */
vxLoggerInfo::vxLoggerInfo(const std::string & sXml) :
m_eLogLevel(m_eDefaultLogLevel),
m_eFormat(vxLogMessageFormats::FORMAT_DEFAULT),
m_sHandle( vxLogUtility::GenerateGuid()),
m_sSystem(vxLogUtility::GetHostname()),
m_sFilename(""),
m_sServer(""),
m_sExtension(".log"),
m_iPort(0)
{
  Initialize(sXml);
} // vxLoggerInfo(const std::string & sXml) 


/**
 * Constructor
 *
 * @param node      Xml representation of the loggers attributes
 */
vxLoggerInfo::vxLoggerInfo(const xml::XmlNode & node) :
m_eLogLevel(m_eDefaultLogLevel),
m_eFormat(vxLogMessageFormats::FORMAT_DEFAULT),
m_sHandle( vxLogUtility::GenerateGuid()),
m_sSystem(vxLogUtility::GetHostname()),
m_sFilename(""),
m_sServer(""),
m_sExtension(".log"),
m_iPort(0)
{
  Initialize(const_cast<xml::XmlNode &>(node));
} // vxLoggerInfo(const xml::XmlNode & node)


/*
 * Constructor
 *
 * @param loggerInfo    Logger information
 */
vxLoggerInfo::vxLoggerInfo(const vxLoggerInfo & loggerInfo) :
m_eLogLevel(loggerInfo.m_eLogLevel),
m_eFormat(loggerInfo.m_eFormat),
m_sHandle(loggerInfo.m_sHandle),
m_sSystem(loggerInfo.m_sSystem),
m_sServer(loggerInfo.m_sServer),
m_iPort(loggerInfo.m_iPort),
m_sExtension(".log"),
m_sFilename(loggerInfo.m_sFilename)
{

  
} // vxLoggerInfo(const vxLoggerInfo & loggerInfo) 

#pragma endregion


/**
 * Initializes logger info from the xml string
 *
 * @param sXml      Xml string
 */
void vxLoggerInfo::Initialize(const std::string & sXml)
{

  
  try
  {
    xml::XmlDocument document;

    document.LoadXml(sXml);
 
    Initialize(document.GetDocumentElement());

 
  }
  catch ( vxLogException &  )
  {
 
    throw;
  }
  catch ( ... )
  {
 
    throw vxLogException(LogRec(util::Strings::Format("Failed to parse logger info : %s",sXml.c_str()), "vxLoggerInfo", "Initialize"));
  }

} // Initialize(xmlNodePtr pNode)

/**
 * Initializes logger info from the xml string
 *
 * @param pNoide      Xml node
 */
void vxLoggerInfo::Initialize(xml::XmlNode & node)
{

  if ( ! node.IsValid() )
    throw vxLogException(LogRec("Xml node is NULL","vxLoggerInfo","Initialize"));

  // Get the handle which is required
  if ( ! node.TryGetAttributeValue<std::string>("handle", m_sHandle) )
    LogWrn("Logger xml missing handle attribute", "vxLoggerInfo","Initialize");

  // Get the system if present
  node.TryGetAttributeValue<std::string>("sys", m_sSystem);


  // =============================================================================================================
  // Gets the file name and then sets it (The system and type need to be extracted first since the filename is 
  // dependent on them
  // ============================================================================================================
  std::string sFilename("");
  node.TryGetAttributeValue<std::string>("file", sFilename);
  SetFilename(sFilename);
  
 
  int iValue(0);

  /// Logging level
  if ( node.TryGetAttributeValue<int>("lvl", iValue) )
    m_eLogLevel = static_cast<vxLogRecord::SeverityEnum >(iValue);


  /// Message format
  if ( node.TryGetAttributeValue<int>("fmt", iValue) )
    m_eFormat = static_cast<vxLogMessageFormats>(iValue);

  node.TryGetAttributeValue<unsigned int>("port", m_iPort);
  node.TryGetAttributeValue<std::string>("svr", m_sServer);


} // Initialize(xmlNodePtr pNode)


/**
 * Sets the file name for the logger. Allows for the changing of the log name during logging operations.
 *
 * @param sFilename     New log file name
 */
void vxLoggerInfo::SetFilename(const std::string & sFilename)
{
  m_sFilename = sFilename;

  // ============================================
  // If no file name use the handle as the name
  // ============================================
  if ( m_sFilename.empty()  )
  {

    if ( vxLogUtility::GetHostname() == m_sSystem ) 
      m_sFilename = util::IO::GetDefaultLogDirectory() + m_sHandle + m_sExtension;
    else
      m_sFilename = util::IO::GetDefaultLogDirectory() + m_sSystem + "\\" + m_sHandle + m_sExtension;

  }

  // ================================================================================================
  // If filename is set an there are no directory separators assume just a name with no path
  //=================================================================================================
  else if ( m_sFilename.find("\\") ==  std::string::npos &&  m_sFilename.find("/") ==  std::string::npos )
  {

    // If there is an extension then leave it otherwise add the standard extension for this logger
    std::string sExt("");
    if ( m_sFilename.find(".") == std::string::npos) 
      sExt = m_sExtension;

    if ( vxLogUtility::GetHostname() == m_sSystem ) 
      m_sFilename = util::IO::GetDefaultLogDirectory() + m_sFilename + sExt;
    else
      m_sFilename = util::IO::GetDefaultLogDirectory() + m_sSystem + "\\" + m_sFilename + sExt;

  }

  // =======================
  // Else assume file-path
  // =======================
  else
  {

    size_t pos(m_sFilename.size() - 4);

    std::string ext(( m_sFilename[pos] != '.' ? "" : m_sFilename.substr(pos,4) ));

  
    if ( ext.empty() )
      m_sFilename.append(m_sExtension);
    else if ( ext != m_sExtension )
      m_sFilename.replace( pos, 4, m_sExtension );
  

    
  }

  util::IO::CreateDirectory(m_sFilename);

} // SetFilename(const std::string & sFilename)


/**
 * Returns true if the logger is server based
 */
const bool vxLoggerInfo::IsServerBased() const
{
  return ( (m_sServer != "")  && (m_iPort != 0) );
}
