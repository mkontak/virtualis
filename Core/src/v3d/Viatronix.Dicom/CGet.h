// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "scoped_ptr.h"
#include "Client.h"




OPEN_DICOM_NS


/**
 * Wraps the C-MOVE native object for the pushin of datasets
 */
public ref class CGet : public Client
{

#pragma region constructors/destructors

private:

  /// Default constructor
  CGet();

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

  /// Creates the find object
  static CGet ^ Create();

public:

  /// Creates the Get object
  static CGet ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the Get object
  static CGet ^ Create(String ^ msAETitle, String ^ msHostXml );


  /// Creates the Get object
  static CGet ^ Create(String ^ msAETitle, Host ^ mpServer);

  /// Add parameters for move (UID Retrieval)
  void Add(String ^ msDestinationAETitle, int iDestinationPort, String ^ sStudyUid, String ^ msSeriesUid, String ^ msInstanceUid);
 
  /// Add parameters for move (Patient)
  void Add(String ^ msDestinationAETitle, int iDestinationPort, String ^ sPatientId, String ^ msPatientName);

public:

   /// Gets/Sets the log source
  virtual property String ^ LogSource
  {
    String ^ get() override            { return gcnew String((*m_pDcmGet)->GetLogSource().c_str()); }
    void set(String ^ sValue) override { (*m_pDcmGet)->SetLogSource(ss(sValue)); } 
  }

    /// Get the port number
  virtual property Host ^ Server
  {  Host ^ get() override; } 


  /// Gets the network timeout
  virtual property int NetworkTimeout
  {
    int get() override                     { return (*m_pDcmGet)->GetNetworkTimeout(); } 
    void set(int iNetworkTimeout) override { (*m_pDcmGet)->SetNetworkTimeout(iNetworkTimeout); } 
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  virtual property int MaxPDU
  {
    int get() override               { return (*m_pDcmGet)->GetMaxPDU(); } 
    void set(int iMaxPDU) override   { (*m_pDcmGet)->SetMaxPDU(iMaxPDU); } 
  } // MaxPDU

  /// Gets/Sets the AE TItle of the server
  virtual property System::String ^ AETitle
  {
    System::String ^ get() override                { return gcnew String((*m_pDcmGet)->GetAETitle().c_str()); }
    void set(System::String ^ msAETitle) override  { (*m_pDcmGet)->SetAETitle(ss(msAETitle)); } 
  } // AETitle

    /// Gets/Sets the blocking mode
  virtual property BlockingModes BlockingMode
  {
    BlockingModes get() override                { return (BlockingModes)((*m_pDcmGet)->GetBlockingMode()); }
    void set(BlockingModes eMode) override  { (*m_pDcmGet)->SetBlockingMode((dcm::DcmBlockingMode)eMode); } 
  } // BlockingMode

protected:

  /// Callback 
  virtual void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) override;

#pragma endregion

private:

  /// Store object
  scoped_ptr<std::shared_ptr<dcm::DcmGet>> m_pDcmGet;


};  // class CGet

CLOSE_DICOM_NS
