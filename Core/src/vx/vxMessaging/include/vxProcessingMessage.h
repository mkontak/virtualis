// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxProcessingMessage.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#include "vxIMessage.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace messaging
{

/**
 * Implements processing status message
 */
class MSG_DLL ProcessingMessage : public IMessage
{

#pragma region constructors/destructors

protected:

  // Constructors - Default 
  ProcessingMessage() ;

  // Constructor
  ProcessingMessage(const std::string & sJob, const std::string & sName, float fProgress,  const std::string & sStage = "", const std::string & sMessage = "" ); 

public:

  
  /// Destructor
  ~ProcessingMessage() { }

#pragma endregion


#pragma region methods

public:

  /// Creates the status message
  static std::shared_ptr<ProcessingMessage> Create(const std::string & sJob, const std::string & sName, float fProgress = 0.0F, const std::string & sStage = "", const std::string & sMessage = "");

  /// Creates the status message from the xml
  static std::shared_ptr<ProcessingMessage> Create(const std::string & sXml);

  /// Generates the xml representation of the status message
  virtual std::string ToXml();

#pragma endregion


#pragma region properties

public:


  /// Gets the module name
  const std::string & GetModuleName() const
  { return m_sModuleName; }

  /// Gets the job id
  const std::string & GetJobId() const
  { return m_sJob; }

  /// Gets the message
  const std::string & GetStatusMessage() const
  { return m_sMessage; }

  /// Sets the message
  void SetStatusMessage(const std::string & sMessage)
  { m_sMessage = sMessage; }

  /// Gets the overall progress
  const float GetProgress() const
  { return m_fProgress; }

  /// Sets the overall progress
  void SetProgress(float fProgress)
  { m_fProgress = fProgress; }


  /// Gets the stage name
  const std::string & GetStage() const
  { return m_sStage; }

  /// Sets the stage name
  void SetStage(const std::string & sStage) 
  {m_sStage = sStage; }


#pragma endregion


#pragma region fields

private:

  /// Module name
  std::string m_sModuleName;

  /// Job Id
  std::string m_sJob;

  /// Message
  std::string m_sMessage;

  /// Stage
  std::string m_sStage;

  /// Overall progress
  float m_fProgress;



#pragma endregion

};  // class ProcessingMessage

}  // namespace messaging

#pragma warning(pop)