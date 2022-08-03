// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptKey.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once



namespace crypto
{

class VX_BASE_DLL CryptProvider;

/**
 * Wraps a Key for the encryption
 */
class VX_BASE_DLL CryptKey
{
protected:

  // Default constructor
  CryptKey(DWORD dwProviderType = PROV_RSA_AES);

  

public:

  /// Creates the Key using the password and salt
  static std::shared_ptr<CryptKey> Create(int iKeySize = 256, DWORD dwProviderId = PROV_RSA_AES);

  /// Creates the Key using the Hash supplied
  static std::shared_ptr<CryptKey> Create(const std::string & sKeyHash, int iKeySize = 256, DWORD dwProviderId = PROV_RSA_AES);


  /// Destructor
  ~CryptKey();

  /// Save the key to the specified file
  void SaveKey(const std::string & sFilePath);

  static std::string ReadKey(const std::string & sFilePath);

  /// Gets the Key Hash
  const std::string & GetKeyHash() const
  { return m_sKeyHash; }

private:

  /// Release the resources
  void ReleaseResources();

 public:

  /// Gets the key handle
  const HCRYPTKEY & GetHandle() const
  { return m_hKey; }


private:


  /// Provider used to generate keys hash
  std::shared_ptr<CryptProvider> m_pProvider;

  /// Keys handle
  HCRYPTKEY   m_hKey;

  /// Key Hash
  std::string m_sKeyHash;

  /// Key size
  int m_iKeySize;

};  // class DerivedKey

};  // namespace crypto