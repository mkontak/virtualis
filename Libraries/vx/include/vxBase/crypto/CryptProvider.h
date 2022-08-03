// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptProvider.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once



namespace crypto
{

/**
 * Wraps the hash providers
 */
class VX_BASE_DLL CryptProvider
{
protected:

 /// Constructor
 CryptProvider(DWORD dwProviderType = PROV_RSA_AES);

public:

 
  /// Destructor
  ~CryptProvider();

  /// Creates the provider using the id specified (Default is PROV_RSA_AES)
  static std::shared_ptr<CryptProvider> Create(DWORD dwProviderType = PROV_RSA_AES);

  /// Acquire the provider
  void Acquire();

  /// Releases the provider
  void Release();

 public:

  /// Gets the handle
  const HCRYPTPROV & GetHandle() const
  { return m_hProvider; }


private:

  /// Provider type
  DWORD m_dwType;

  /// Provider handle
  HCRYPTPROV  m_hProvider;


};  // class CryptProvider

};  // namsespace crypto