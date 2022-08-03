// $Id: StatusMessage.h,v 1.1 2006/12/05 19:16:17 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once

using namespace System;

// Namespaces
OPEN_PIPELINE_NS 

/// Message Types
public enum class MessageTypes { Debug, Information, Error, Warning };

/**
 * Implements status message
 */
public ref class StatusMessage
{
public:

  /// Constructor
  StatusMessage();

  /// Xml representation of this status message
 virtual System::String ^ ToXml();

  /// Returns a status message from the xml node
  static StatusMessage ^ FromXml(System::Xml::XmlNode ^ pNode);

public:

    /// Sets the message
  property MessageTypes MessageType
  {
      void set(MessageTypes eType) { m_eMessageType = eType; }
      MessageTypes get() { return m_eMessageType; }
  }


  /// Sets the message
  property String ^ Message
  {
    void set(System::String ^ sMessage) { m_sMessage = sMessage; }
    String ^ get() { return m_sMessage; }
  }

  /// Sets the module type
  property String ^ ModuleType
  {
    void set(System::String ^ sModuleType) { m_sModuleType = sModuleType; }
    String ^ get() { return m_sModuleType; }
  }

  /// Sets the module state
  property String ^ Stage
  {
    void set(System::String ^ sStage) { m_sStage = sStage; }
    String ^ get() { return m_sStage; }
  }

  /// Sets the state progress
  property int StageProgress
  {
    void set(int iStageProgress) { m_iStageProgress = iStageProgress; }
    int get() { return m_iStageProgress; }
  }

  /// Sets the overall progress
  property int OverallProgress
  {
    void set(int iOverallProgress) { m_iOverallProgress = iOverallProgress; }
    int get() { return m_iOverallProgress; }
  }

private:

  /// Message Type
  MessageTypes m_eMessageType;

  /// Information/Error  Message 
  System::String ^ m_sMessage;

  /// Module type
  System::String ^ m_sModuleType;

  /// State progress 
  int m_iStageProgress;

  /// Overall progress
  int m_iOverallProgress;

  /// Stage
  System::String ^ m_sStage;

}; // class StatusMessage


CLOSE_PIPELINE_NS

// $Log: StatusMessage.h,v $
// Revision 1.1  2006/12/05 19:16:17  gdavidson
// Consolidated Viatronix.Utilities.Pipelining
//
// Revision 1.3  2006/04/11 19:41:47  mkontak
// Added message type to allow for message sending of different types and a
// default of DEBUG allowing the message to be logged to the WQ logger
//
// Revision 1.2  2005/06/08 13:39:04  mkontak
// Change message communication with executables
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/09/18 01:58:04  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/StatusMessage.h,v 1.1 2006/12/05 19:16:17 gdavidson Exp $
// $Id: StatusMessage.h,v 1.1 2006/12/05 19:16:17 gdavidson Exp $
