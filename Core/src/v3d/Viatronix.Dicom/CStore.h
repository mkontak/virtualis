// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CStore.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "scoped_ptr.h"
#include "Client.h"

OPEN_DICOM_NS

// forward declarations
ref class Host;

/**
 * Store client object used to perform C-STORE operations to a DICOM SCP device
 */
public ref class CStore : public Client
{


#pragma region constructor/destructor

private:

  /// Default constructor
  CStore();

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
  static CStore ^ Create();

public:

  /// Creates the CStore from the server parameters
  static CStore ^ Create(String ^ msAETitle, String ^ msServerAEtitle, String ^ msServer, uint iPort);


  /// Creates the CStore object
  static CStore ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the CStore object
  static CStore ^ Create(String ^ msAETitle, String ^ mxHostXml );

  /// Create the CStore object
  static CStore ^ Create(String ^ msAETitle, Host ^ mpServer);

  /// Adds file to the list
  void AddFile(String ^ msFile);

  /// Adds directory to the list
  void AddDirectory(String ^ msDirectory, String ^ msPattern, bool recursive);

  /// Adds an Image.
  void AddImage(Image ^ mpImage);

public:

   /// Gets/Sets the log source
  virtual property String ^ LogSource
  {
    String ^ get() override            { return gcnew String((*m_pDcmStore)->GetLogSource().c_str()); }
    void set(String ^ sValue) override { (*m_pDcmStore)->SetLogSource(ss(sValue)); } 
  }

    /// Get the port number
  virtual property Host ^ Server
  {
    Host ^ get() override;
   } // Server


  /// Gets the network timeout
  virtual property int NetworkTimeout
  {
    int get() override                     { return (*m_pDcmStore)->GetNetworkTimeout(); } 
    void set(int iNetworkTimeout) override { (*m_pDcmStore)->SetNetworkTimeout(iNetworkTimeout); } 
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  virtual property int MaxPDU
  {
    int get() override               { return (*m_pDcmStore)->GetMaxPDU(); } 
    void set(int iMaxPDU) override   { (*m_pDcmStore)->SetMaxPDU(iMaxPDU); } 
  } // MaxPDU

  /// Gets/Sets the AE TItle of the server
  virtual property System::String ^ AETitle
  {
    System::String ^ get() override                { return gcnew String((*m_pDcmStore)->GetAETitle().c_str()); }
    void set(System::String ^ msAETitle) override  { (*m_pDcmStore)->SetAETitle(ss(msAETitle)); } 
  } // AETitle

  /// Gets/Sets the blocking mode
  virtual property BlockingModes BlockingMode
  {
    BlockingModes get() override                { return (BlockingModes)((*m_pDcmStore)->GetBlockingMode()); }
    void set(BlockingModes eMode) override  { (*m_pDcmStore)->SetBlockingMode((dcm::DcmBlockingMode)eMode); } 
  } // BlockingMode

protected:

  /// Callback
  virtual void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) override;

private:

  /// Store object
  scoped_ptr<std::shared_ptr<dcm::DcmStore>> m_pDcmStore;

};  // class CStore

CLOSE_DICOM_NS
