// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CFind.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// include
#include "scoped_ptr.h"
#include "Client.h"



OPEN_DICOM_NS


// Forward declarations
ref class Image;
ref class CFindParameters;
ref class Host;

/**
 * Wraps the C-FIND operation from the vxDicom library
 */
public ref class CFind : public Client
{

#pragma region constructor/destructor

private:

    /// Default constructor
  CFind();

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
  static CFind ^ Create();

public:

  /// Creates the CFind object
  static CFind ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the CFind object
  static CFind ^ Create(String ^ msAETitle, String ^ msHostXml );

  /// Creates the CFind object
  static CFind ^ Create(String ^ msAETitle, Host ^ mpServer);


  /// Adds a parameters object to the vector.
  CFindParameters ^ AddParameters(CFindParameters ^ mpParameters);

  /// Clear the parameter list
  void ClearParameters();
 
public:

   /// Gets/Sets the log source
  virtual property String ^ LogSource
  {
    String ^ get() override            { return gcnew String((*m_pDcmFind)->GetLogSource().c_str()); }
    void set(String ^ sValue) override { (*m_pDcmFind)->SetLogSource(ss(sValue)); } 
  }

    /// Get the port number
  virtual property Host ^ Server
  { Host ^ get() override; } 


  /// Gets the network timeout
  virtual property int NetworkTimeout
  {
    int get() override                     { return (*m_pDcmFind)->GetNetworkTimeout(); } 
    void set(int iNetworkTimeout) override { (*m_pDcmFind)->SetNetworkTimeout(iNetworkTimeout); } 
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  virtual property int MaxPDU
  {
    int get() override               { return (*m_pDcmFind)->GetMaxPDU(); } 
    void set(int iMaxPDU) override   { (*m_pDcmFind)->SetMaxPDU(iMaxPDU); } 
  } // MaxPDU

  /// Gets/Sets the AE TItle of the server
  virtual property System::String ^ AETitle
  {
    System::String ^ get() override                { return gcnew String((*m_pDcmFind)->GetAETitle().c_str()); }
    void set(System::String ^ msAETitle) override  { (*m_pDcmFind)->SetAETitle(ss(msAETitle)); } 
  } // AETitle

   /// Gets/Sets the blocking mode
  virtual property BlockingModes BlockingMode
  {
    BlockingModes get() override                { return (BlockingModes)((*m_pDcmFind)->GetBlockingMode()); }
    void set(BlockingModes eMode) override  { (*m_pDcmFind)->SetBlockingMode((dcm::DcmBlockingMode)eMode); } 
  } // BlockingMode


protected:

  /// Callback
  virtual void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) override;


#pragma endregion

#pragma region fields

private:

  /// Elements
  System::Collections::Generic::List<Image ^> ^ m_mpData;

    /// Store object
  scoped_ptr<std::shared_ptr<dcm::DcmFind>> m_pDcmFind;

#pragma endregion





};  // class CFind

CLOSE_DICOM_NS
