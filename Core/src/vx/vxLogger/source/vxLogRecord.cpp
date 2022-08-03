// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogRecord.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogUtility.h"
#include "vxLogRecord.h"
#include "vxUtilStrings.h"
#include "vxUtilIO.h"
#include "vxXmlNode.h"
#include "vxXmlExceptions.h"

/// Record delimiter
const char vxLogRecord::m_delimiter = 0x1F;


#pragma region constructors

/**
 * Default Constructor
 */
vxLogRecord::vxLogRecord() : 
m_sFileName(""),
m_iLine(0),
m_sClassName(""),
m_sMethod(""),
m_sMessage(""),
m_eSeverity(vxLogRecord::SL_ERROR),
m_iProcessId(vxLogFactory::GetProcessId()),
m_iThreadId(GetCurrentThreadId())
{
  GetSystemTime(&m_time);
}//vxLogRecord()


/**
 * Constructor
 *
 * @params  sXml      Xml representation of a log record
 */
vxLogRecord::vxLogRecord(const std::string & sXml) : 
m_sFileName(""),
m_iLine(0),
m_sClassName(""),
m_sMethod(""),
m_sMessage(""),
m_eSeverity(vxLogRecord::SL_ERROR),
m_iProcessId(0),
m_iThreadId(0)
{

  Initialize(sXml);
  
  GetSystemTime(&m_time);

 }//vxLogRecord()


/**
 * Constructor
 *
 * @params  node      XmlNode contains the log record
 */
vxLogRecord::vxLogRecord(const xml::XmlNode & node) : 
m_sFileName(""),
m_iLine(0),
m_sClassName(""),
m_sMethod(""),
m_sMessage(""),
m_eSeverity(vxLogRecord::SL_ERROR),
m_iProcessId(0),
m_iThreadId(0)
{


  Initialize(const_cast<xml::XmlNode &>(node));
  
  GetSystemTime(&m_time);

 }//vxLogRecord()


/**
 * Constructor
 *
 * @param sMessage         filename of the code that is logging a message.
 * @param sClassName      version of the file logging a message.
 * @param sMethod         function name in which log message occurs.
 * @param sMessage        Detailed description of event triggering log message.
 * @param eSeverity       Severity Level
 */
vxLogRecord::vxLogRecord( const std::string & sMessage, const std::string & sClassName, 
                          const std::string & sMethod,  const SeverityEnum eSeverity):
m_sFileName(""),
m_iLine(0),
m_sClassName(sClassName),
m_sMethod(sMethod),
m_sMessage(sMessage),
m_eSeverity(eSeverity),
m_iProcessId(GetCurrentProcessId()),
m_iThreadId(GetCurrentThreadId())
{

  
  /// Get the current time for the event
  GetSystemTime(&m_time);

} // vxLogRecord()

/**
 * Constructor
 *
 * @param sFileName       Filename of the code that is logging a message. 
 * @param iLine           Line number of the log entry call within file.
 * @param sClassInfo      Class name in which log message occurs.
 * @param sFunction       Function name in which log message occurs.
 * @param sMessage        Detailed description of event triggering log message.
 * @param eSeverity       Severity Level
 * @param eFormat         Message Format type

 */
vxLogRecord::vxLogRecord(const std::string & sFileName,   const int iLine,
                         const std::string & sClassInfo,  const std::string & sFunction, 
                         const std::string & sMessage,    const SeverityEnum eSeverity ) :
m_sFileName(util::IO::GetFilename(sFileName)),
m_iLine(iLine),
m_sClassName(sClassInfo),
m_sMethod(sFunction),
m_sMessage(sMessage),
m_eSeverity(eSeverity),
m_iProcessId(GetCurrentProcessId()),
m_iThreadId(GetCurrentThreadId())
{

  /// Get the current time for the event
  GetSystemTime(&m_time);

} // vxLogRecord()




/**
 * Copy Constructor
 *
 * @param record      Log record to copy from
 */
vxLogRecord::vxLogRecord( const vxLogRecord & record ):
m_sFileName( record.m_sFileName ),
m_iLine( record.m_iLine ),
m_sClassName( record.m_sClassName ),
m_sMethod( record.m_sMethod ),
m_sMessage( record.m_sMessage ),
m_eSeverity( record.m_eSeverity ),
m_iProcessId( record.m_iProcessId),
m_iThreadId( record.m_iThreadId),
m_time(record.m_time)
{


} // vxLogRecord( const vxLogRecord & record )

#pragma endregion 

/** 
 * Returns time stamp
 *
 * @return time stamp as a string
 */
const std::string vxLogRecord::GetTimeStamp() const
{

  char date[64];
  memset(date,0,64);

  sprintf_s(date, 63, "%04u-%02u-%02uT%02u:%02u:%02u.%03u", 
            m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond, m_time.wMilliseconds);

  return std::string(date);

} // GetTimeStamp()


/**
 * Gets the severity level as a string
 */
const std::string vxLogRecord::GetLevelAsString() const
{
  switch(m_eSeverity)
  {
  case SL_NONE:         return "None";
  case SL_WARNING:      return "Warning";
  case SL_INFORMATION:  return "Information";
  case SL_CRITICAL:     return "Critical";
  //case SL_FATAL:      return "Fatal";       ( Same as critical )
  case SL_ERROR:        return "Error";
  case SL_VERBOSE:      return "Verbose";
  case SL_TIMING:       return "Timing";
  //case SL_DEBUG:        return "Debug";
  //case SL_TEMPORARY:    return "Temporary";
  //case SL_HIGHEST:      return "Highest";
  default:              return "Undefined";
  }
} // GetLevelAsString()

/**
 * Gets the severity level as a string
 */
const std::string vxLogRecord::GetLevelAsSpacedString() const
{
  switch(m_eSeverity)
  {
  case SL_NONE:         return "None       ";
  case SL_WARNING:      return "Warning    ";
  case SL_INFORMATION:  return "Information";
  case SL_CRITICAL:     return "Critical   ";
  //case SL_FATAL:      return "Fatal";       ( Same as critical )
  case SL_ERROR:        return "Error      ";
  case SL_VERBOSE:      return "Verbose    ";
  case SL_TIMING:       return "Timing     ";
  //case SL_DEBUG:        return "Debug";
  //case SL_TEMPORARY:    return "Temporary";
  //case SL_HIGHEST:      return "Highest";
  default:              return "Unddefined ";
  }
} // GetLevelAsString()


std::string vxLogRecord::FormatMessage(vxLogMessageFormats eFormat)
{
  return ( eFormat == FORMAT_XML ?  ToXml() :  ToString() ) + "\n";
 
} // FormatMessage()


/**
 * Generates the XML for the log record
 *
 * @param bAddNewLine   Flag to indicate that a newline is to added at the end of the xml r
 *
 * @return Xml representation if the record
 */
std::string vxLogRecord::ToXml()
{
  std::ostringstream os;

  std::string sTag(GetLevelAsString());

  os << "<" << sTag;

  os << " date=\""    << this->GetTimeStamp() << "\"";
  os << " class=\""   << m_sClassName << "\"";
  os << " method=\""  << m_sMethod    << "\"";
  os << " pid=\""     << m_iProcessId << "\"";
  os << " tid=\""     << m_iThreadId  << "\"";
  os << " file=\""    << m_sFileName  << "\"";
  os << " line=\""    << m_iLine      << "\">";
  os << "<![CDATA["   << m_sMessage   << "]]>";
  os << "</" << sTag << ">";
  

  /// Return the xml
  return os.str();

} // ToXml()

/**
 * Generates the XML for the log record
 *
 * @param bAddNewLine   Flag to indicate that a newline is to added at the end of the xml r
 *
 * @return Xml representation if the record
 */
std::string vxLogRecord::ToString()
{
  std::ostringstream os;

  os <<  GetLevelAsSpacedString() << "  " << this->GetTimeStamp() << "  " << m_sMessage;


  /// Return the xml
  return os.str();

} // ToXml()



/**
 * Initializes the object using the xml string
 */
void vxLogRecord::Initialize(const std::string & sXml)
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
   
    throw vxLogException(LogRec(util::Strings::Format("Failed to parse log record : %s",sXml.c_str()),"vxLogRecord","vxLogRecord"));
  }

} // Initialize(const std::strong & sXml)

/**
 * Initializes the object using the xml node
 *
 * @param pNode     Xml node
 */
void vxLogRecord::Initialize(xml::XmlNode & node)
{
  

  if ( ! node.IsValid() )
    throw vxLogException(LogRec("Xml node is NULL","vxLogRecord","Initialize"));
 
   m_eSeverity = ToSeverityEnum(node.GetName());

  if ( ! node.TryGetAttributeValue<std::string>( "class", m_sClassName) )
    throw vxLogException(LogRec("Invalid xml missing handle attribute", "vxLogRecord","vxLogRecord"));

  if ( ! node.TryGetAttributeValue<std::string>( "method", m_sMethod) )
    throw vxLogException(LogRec("Invalid xml missing method attribute", "vxLogRecord","vxLogRecord"));

  if ( ! node.TryGetAttributeValue<int>( "pid", m_iProcessId) )
    throw vxLogException(LogRec("Invalid xml missing pid attribute", "vxLogRecord","vxLogRecord"));

  if ( ! node.TryGetAttributeValue<int>("tid", m_iThreadId) )
    throw vxLogException(LogRec("Invalid xml missing tid attribute", "vxLogRecord","vxLogRecord"));

  if ( ! node.TryGetAttributeValue<int>( "line", m_iLine) )
    throw vxLogException(LogRec("Invalid xml missing line attribute", "vxLogRecord","vxLogRecord"));

  if ( ! node.TryGetAttributeValue<std::string>( "file", m_sFileName) )
    throw vxLogException(LogRec("Invalid xml missing tid attribute", "vxLogRecord","vxLogRecord"));

 
  if ( node.HasChildren() )
    m_sMessage = node.GetFirstChild().GetContent();


} // Initialize(xmlNodePtr pNode)




vxLogRecord::SeverityEnum vxLogRecord::ToSeverityEnum(const std::string & sSeverity)
{


  vxLogRecord::SeverityEnum severity(vxLogRecord::SL_ERROR);


  if ( strncmp(sSeverity.c_str(), "Error", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_ERROR;
  else if ( strncmp(sSeverity.c_str(), "Verbose", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_VERBOSE;
    else if ( strncmp(sSeverity.c_str(), "Debug", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_DEBUG;
  else if ( strncmp(sSeverity.c_str(), "Information", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_INFORMATION;
  else if ( strncmp(sSeverity.c_str(), "Critical", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_CRITICAL;
    else if ( strncmp(sSeverity.c_str(), "Warning", sSeverity.size() ) == 0 )
    severity = vxLogRecord::SL_WARNING;



  return severity;
}