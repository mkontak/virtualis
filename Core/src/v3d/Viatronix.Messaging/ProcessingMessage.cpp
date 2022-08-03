// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ProcessingMessage.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// This is the main DLL file.

#include "stdafx.h"
#include "ProcessingMessage.h"

USING_MESSAGE_NS

/**
 * Constructor
 */
ProcessingMessage::ProcessingMessage()
{

  m_pMessage.reset(new std::shared_ptr<messaging::ProcessingMessage>(messaging::ProcessingMessage::Create("", "", 0.0F, "", ""))); 
} // ProcessingMessage()

/**
 * Constructor
 */
ProcessingMessage::ProcessingMessage(String ^ msJob, String ^ msName, Single fProgress, String ^ msStage,  String ^ msMessage)
{

  m_pMessage.reset(new std::shared_ptr<messaging::ProcessingMessage>(messaging::ProcessingMessage::Create(ss(msJob), ss(msName), fProgress, ss(msStage), ss(msMessage)))); 
} // ProcessingMessage(String ^ msJob, String ^ msName, String ^ msStage, Single fOverallProgress, Single fStageProgress, String ^ msMessage)


/**
 * Creates the processing message
 */
ProcessingMessage ^ ProcessingMessage::Create(String ^ msJob, String ^ msName, Single fProgress,  String ^ msStage,  String ^ msMessage)
{

  ProcessingMessage ^ mpMessage = gcnew ProcessingMessage(msJob, msName, fProgress, msStage, msMessage);

  if ( mpMessage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ProcessingMessage");


  return mpMessage;
} // Create(String ^ msJob, String ^ msName, Single fProgress,  String ^ msStage,  String ^ msMessage)

/**
 * Creates the processing message
 */
ProcessingMessage ^ ProcessingMessage::Create(String ^ msXml )
{

  ProcessingMessage ^ mpMessage = gcnew ProcessingMessage();

  if ( mpMessage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ProcessingMessage");


  mpMessage->m_pMessage.reset(new std::shared_ptr<messaging::ProcessingMessage>(messaging::ProcessingMessage::Create(ss(msXml)))); 

  return mpMessage;
} // Create(String ^ msJob, String ^ msName, String ^ msStage, Single fOverallProgress, Single fStageProgress, String ^ msMessage)


