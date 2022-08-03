// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ProcessingMessage.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



#pragma once

#include "IMessage.h"
#include "scoped_ptr.h"

using namespace System;

OPEN_MESSAGE_NS

/**
 * Wrape the natiuve ProcessingMessage from vxMessaging.dll
 */
 public ref class ProcessingMessage : Viatronix::Messaging::IMessage
{
protected:

  /// Default constructor
  ProcessingMessage();

  // Constructor
  ProcessingMessage(String ^ msJob, String ^ msName, Single fProgress,  String ^ msStage,  String ^ msMessage );

 

public:

  /// Creates the message from the xml
  static ProcessingMessage ^ Create(String ^ sXml);

  // Creates the message
  static ProcessingMessage ^ Create(String ^ msJob, String ^ msName, Single fProgress, String ^ msStage , String ^ msMessage );
 
  // Creates the message
  static ProcessingMessage ^ Create(String ^ msJob, String ^ msName, Single fProgress, String ^ msStage )
  { return Create(msJob, msName, fProgress, msStage, String::Empty); }


  // Creates the message
  static ProcessingMessage ^ Create(String ^ msJob, String ^ msName, Single fProgress)
  { return Create(msJob, msName, fProgress, String::Empty, String::Empty); }

  // Creates the message
  static ProcessingMessage ^ Create(String ^ msJob, String ^ msName)
  { return Create(msJob, msName, 0.0F, String::Empty, String::Empty); }


  /// Generate the XML
  virtual String ^ ToXml()
  { return gcnew String((*m_pMessage)->ToXml().c_str()); }

#pragma region properties

public:

  // Gets/Sets the Stage
  property String ^ Stage
  { 
    String ^ get() { return gcnew String((*m_pMessage)->GetStage().c_str()); }
    void set(String ^ msStage) { (*m_pMessage)->SetStage(ss(msStage)); }
  }

  /// Gets/Sets the Message
  property String ^ Message
  {
    String ^ get() { return gcnew String( (*m_pMessage)->GetStatusMessage().c_str() ); }
    void set(String ^ msMessage) { (*m_pMessage)->SetStatusMessage( ss(msMessage) ); }
  }

    /// Gets the job id
  property String ^ JobId
  { String ^ get() { return gcnew String((*m_pMessage)->GetJobId().c_str()); } }

  /// Gets the module name (task name)
  property String ^ ModuleName
  { String ^ get() { return gcnew String((*m_pMessage)->GetModuleName().c_str()); } }

  /// Gets the overall progress
  property Single Progress
  { 
    Single get() { return (*m_pMessage)->GetProgress(); }
    void set(Single fProgress) { (*m_pMessage)->SetProgress(fProgress); }
  }




#pragma endregion


#pragma region fields

private:

  /// Natve StatusMessage
  scoped_ptr<std::shared_ptr<messaging::ProcessingMessage>> m_pMessage;

#pragma endregion

};  // class ProcessingMessage


CLOSE_MESSAGE_NS