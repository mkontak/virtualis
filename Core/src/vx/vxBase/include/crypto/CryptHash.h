// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptHash.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include <wincrypt.h>

namespace crypto
{

class VX_BASE_DLL CryptProvider;


/**
 * Wraps the hash algorithim
 */
class VX_BASE_DLL CryptHash
{
protected:

  /// CryptHash based on algorithm
  CryptHash(DWORD dwAlgorithType);

  /// CryptHase based on provider and algorithm
  CryptHash(std::shared_ptr<CryptProvider> pProvider, DWORD dwAlgorithType);

public:

  /// Destructor
  ~CryptHash();

  //// Clones the hash  including state
  std::shared_ptr<CryptHash> Clone();

  /// Creates the  Hash based on the provider and algorithm
  static std::shared_ptr<CryptHash> Create(std::shared_ptr<CryptProvider> pProvider, DWORD dwAlgorithType);

  /// Creates the hash based on the algorithm (uses internal provider)
  static std::shared_ptr<CryptHash> Create( DWORD dwAlgorithType );

  /// Hashs in the supplied text
  void Hash(const std::string & sText);

  /// Gets the hash value
  std::string GetValue();

  /// Gets the Handle
  HCRYPTHASH GetHandle() 
  { return m_hHash; }

private:

  /// Destroys the hash
  void DestroyHash();

  /// Creates the hash
  void CreateHash();

private:

  /// Algosith type
  DWORD m_dwAlgorithmType;

  /// Hash provider
  std::shared_ptr<CryptProvider> m_pProvider;

  /// Hash Handle
  HCRYPTHASH  m_hHash;

  /// Not used
  HCRYPTKEY   m_hKey;

};  // class CryptHash

};  // namespace crypto