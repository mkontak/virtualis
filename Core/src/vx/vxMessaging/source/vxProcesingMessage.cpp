// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxProcesingMessage.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



#include "Stdafx.h"
#include "vxProcessingMessage.h"



using namespace messaging;


/**
 * Constructor
 *
 * @param sJob              Job id
 * @param sName             Module name
 * @param fProgress         Progress
 * @param sStage            Stage name
 * @param sMessage          Message
 *
 */
ProcessingMessage::ProcessingMessage(const std::string & sJob, const std::string & sName,  float fProgress, const std::string & sStage, const std::string & sMessage ) :
m_sJob(sJob),
m_sModuleName(sName),
m_sStage(sStage),
m_fProgress(fProgress),
m_sMessage(sMessage)
{
}

/**
 * Constructor
 */
ProcessingMessage::ProcessingMessage() :
m_sJob(""),
m_sModuleName(""),
m_sStage(""),
m_fProgress(0.0F),
m_sMessage("")
{

}


/**
 * Creates the ProcessingMessage object
 *
 * @param sJob              Job id
 * @param sName             Module name
 * @param sStage            Stage name
 * @param fOverallProgress  Overall progress of process
 * @param fStageProgress    Stage progress
 * @param sMessage          Message
 *
 * @return shared pointer to the processing message
 */
std::shared_ptr<ProcessingMessage> ProcessingMessage::Create(const std::string & sJob, const std::string & sName, float fProgress, const std::string & sStage, const std::string & sMessage ) 
{

  std::shared_ptr<ProcessingMessage> pMessage = std::shared_ptr<ProcessingMessage>(new ProcessingMessage(sJob, sName, fProgress, sStage, sMessage));

  if ( pMessage == __nullptr )
    throw std::exception("Failed to allocate ProcessingMessage, out of memory");

  return pMessage;
} // Create(const std::string & sJob, const std::string & sName, float fProgress, const std::string & sStage, const std::string & sMessage ) 


/**
 * Creates the processing message from the xml string
 *
 * @param sXml      Xml string
 *
 * @return shared pointer to the processing message
 */
std::shared_ptr<ProcessingMessage> ProcessingMessage::Create(const std::string & sXml)
{

  std::shared_ptr<ProcessingMessage> pMessage = std::shared_ptr<ProcessingMessage>(new ProcessingMessage());

  if ( pMessage == __nullptr )
    throw std::exception("Failed to allocate ProcessingMessage, out of memory");

  if ( ! sXml.empty() )
  {
    xml::XmlDocument doc;

    doc.LoadXml(sXml);

    doc.GetDocumentElement().TryGetAttributeValue<std::string>("name", pMessage->m_sModuleName );
    doc.GetDocumentElement().TryGetAttributeValue<std::string>("job", pMessage->m_sJob );
    doc.GetDocumentElement().TryGetAttributeValue<std::string>("msg", pMessage->m_sMessage );
    doc.GetDocumentElement().TryGetAttributeValue<std::string>("stage", pMessage->m_sStage );
    doc.GetDocumentElement().TryGetAttributeValue<float>("pct", pMessage->m_fProgress );
  }
  return pMessage;

} // Create(const std::string & sXml)

/**
 * Generates the xml representation of the ProcessingMessage object
 */
std::string ProcessingMessage::ToXml()
{
  std::ostringstream os;

  os << "<msg job=\"" << m_sJob << "\" name=\"" << m_sModuleName << "\" stage=\"" << m_sStage << "\" pct=\"" << m_fProgress  << "\" msg=\"" << m_sMessage << "\" />";

  return os.str();
}

