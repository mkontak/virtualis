
// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDevice.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm 
{


/**
 * Implements a base DICOM device used by an SCU and SCP
 */
class DCM_DLL DcmDevice
{

protected:

  /// constructor
  DcmDevice();

  /// constructor
  DcmDevice(const std::string & sAETitle, int iMaxPDU, int iNetworkTimeout);

  /// constructor
  DcmDevice(const std::string & sAETitle);

  /// destructor
  virtual ~DcmDevice();

private:

  /// Initialize the objects state
  void Initialize();

  /// Initializes the network
  virtual void InitializeNetwork();

public:

  
  /// Sets the transfer syntax to a preconfigured group
  virtual void SetTransferSyntax(E_TransferSyntax transferSyntax);

  /// Adds transfer syntax
  void AddTransferSyntax(const std::string & sTransferSyntax);


  /// Returns true if the specified syntax is supported
  bool SupportsTransferSyntax(const std::string & sTransferSyntax);


  /// Adds the abstract syntax
  void AddAbstractSyntax(const std::string & sAbtractSyntax);

  /// Returns true if the specified syntax is supported
  bool SupportsAbstractSyntax(const std::string & sAbtractSyntax);

public:

  /// Gets the AE Title of the device
  virtual const std::string & GetAETitle() 
  { return m_sAETitle; }

  /// Sets the AE Title of the device
  virtual void  SetAETitle(const std::string & sAETitle) 
  {  m_sAETitle = sAETitle; }

  /// Gets the log source for the dicom device
  virtual const std::string & GetLogSource() 
  { return m_sLogSource; }

  /// Sets the log source for the device
  virtual void SetLogSource(const std::string & sLogSource) 
  { m_sLogSource = sLogSource; }

   /// Gets the network timeout
  virtual const unsigned int GetNetworkTimeout() const
  { return m_iNetworkTimeout; }

  /// Sets the network timeout
  virtual void SetNetworkTimeout(unsigned int iValue) 
  { m_iNetworkTimeout = iValue; }

  /// Gets the max pdu
  virtual const unsigned int GetMaxPDU() const
  { return m_iMaxPDU; }

  /// Sets the max pdu
  virtual void SetMaxPDU(unsigned int iValue)
  { m_iMaxPDU = iValue; }
 
  /// Gets the list of transfer syntaxes
  const char ** GetTransferSyntaxes();

  /// Gets the list of abstract syntaxes
  const char ** GetAbstractSyntaxes();

  /// Gets the number of transfer syntaxes
  int GetTransferSyntaxCount() const
  { return (int)m_transferSyntaxes.size(); }

  /// Gets the number of abstract syntaxes
  int GetAbstractSyntaxCount() const
  { return (int)m_abstractSyntaxes.size(); }

protected:


  /// Logging source
  std::string m_sLogSource;

  /// AE Title of device
  std::string m_sAETitle;

  /// Network timeout
  unsigned int m_iNetworkTimeout;

  /// Max PDU size
  unsigned int m_iMaxPDU;

  /// Command Syntaxes
  STRINGLIST m_abstractSyntaxes; 

  /// Transfer Syntaxes
  STRINGLIST  m_transferSyntaxes;

  /// Array used for dicom library
  const char ** m_transferArray;

  /// Array used for dicom library
  const char ** m_abstractArray;

}; // class DcmDevice

};  // namespace dcm

#pragma warning(pop)