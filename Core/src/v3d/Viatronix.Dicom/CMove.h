// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CMove.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)




#pragma once

// includes
#include "scoped_ptr.h"
#include "Client.h"
#include "Enums.h"


OPEN_DICOM_NS


// forward declarations
ref class CMoveParameters;
ref class Host;

/**
 * Wraps the C-MOVE native object for the pushin of datasets
 */
public ref class CMove : public Client
{

#pragma region constructors/destructors

private:

  /// Default constructor
  CMove();

public:


  /// Start process
  virtual void Start(String ^ sCallbackData) override;

  /// Abort process
  virtual void Abort() override;

  /// Wait for process to complete
  virtual void Wait() override;

#pragma endregion

#pragma region methods

protected:

  /// Creates a empty CMove
  static CMove ^ Create();

public:

  /// Creates the CMove object
  static CMove ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the CMove object
  static CMove ^ Create(String ^ msAETitle, String ^ mxHostXml );

  /// Create the CMove object
  static CMove ^ Create(String ^ msAETitle, Host ^ mpServer);

  /// Adds the move parameters
  CMoveParameters ^ AddParameters( CMoveParameters ^ mpParameters );

   /// Clear the parameter list
  void ClearParameters();

public:

   /// Gets/Sets the log source
  virtual property String ^ LogSource
  {
    String ^ get() override            { return gcnew String((*m_pDcmMove)->GetLogSource().c_str()); }
    void set(String ^ sValue) override { (*m_pDcmMove)->SetLogSource(ss(sValue)); } 
  }

    /// Get the port number
  virtual property Host ^ Server
  {  Host ^ get() override; } 


  /// Gets the network timeout
  virtual property int NetworkTimeout
  {
    int get()  override                    { return (*m_pDcmMove)->GetNetworkTimeout(); } 
    void set(int iNetworkTimeout) override { (*m_pDcmMove)->SetNetworkTimeout(iNetworkTimeout); } 
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  virtual property int MaxPDU
  {
    int get() override               { return (*m_pDcmMove)->GetMaxPDU(); } 
    void set(int iMaxPDU) override   { (*m_pDcmMove)->SetMaxPDU(iMaxPDU); } 
  } // MaxPDU

  /// Gets/Sets the AE TItle of the server
  virtual property System::String ^ AETitle
  {
    System::String ^ get() override                { return gcnew String((*m_pDcmMove)->GetAETitle().c_str()); }
    void set(System::String ^ msAETitle) override  { (*m_pDcmMove)->SetAETitle(ss(msAETitle)); } 
  } // AETitle

 /// Gets/Sets the blocking mode
  virtual property BlockingModes BlockingMode
  {
    BlockingModes get() override                { return (BlockingModes)((*m_pDcmMove)->GetBlockingMode()); }
    void set(BlockingModes eMode) override  { (*m_pDcmMove)->SetBlockingMode((dcm::DcmBlockingMode)eMode); } 
  } // BlockingMode


protected:

  /// Callback 
  virtual void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) override;

#pragma endregion


private:

    /// Store object
  scoped_ptr<std::shared_ptr<dcm::DcmMove>> m_pDcmMove;



};  // class CMove

CLOSE_DICOM_NS
