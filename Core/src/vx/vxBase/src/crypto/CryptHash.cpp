// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptHash.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "crypto\CryptHash.h"
#include "crypto\CryptProvider.h"
#include "Exception.h"


using namespace crypto;

/**
 *  constructor
 *
 *  @param pProvider
 *  @param dwAlgorithType
 */
CryptHash::CryptHash(std::shared_ptr<CryptProvider> pProvider, DWORD dwAlgorithType) :
m_pProvider(pProvider),
m_hHash(NULL),
m_dwAlgorithmType(dwAlgorithType)
{
} // CryptHash


/**
 *  constructor
 *
 *  @param dwAlgorithmType
 */
CryptHash::CryptHash(DWORD dwAlgorithmType) :
m_pProvider(CryptProvider::Create()),
m_hHash(NULL),
m_dwAlgorithmType(dwAlgorithmType)
{
} //CryptHash


/**
 *  destructor
 */
CryptHash::~CryptHash()
{
  DestroyHash();
} // ~CryptHash()


/**
 * clone
 *
 *  @return cloned one
 */
std::shared_ptr<CryptHash> CryptHash::Clone()
{
  std::shared_ptr<CryptHash> pHash = std::shared_ptr<CryptHash>(new CryptHash(m_pProvider, m_dwAlgorithmType));  

  if ( ! CryptDuplicateHash(m_hHash, NULL, 0, &pHash->m_hHash ) )
    throw ex::CryptException( LogRec( "Failed to duplicate hash", "CryptHash", "Clone" ));
 
  pHash->CreateHash();

  return pHash;
} // Clone


/**
 *  Create
 *
 *  @param pProvider
 *  @param dwAlgorithmType
 *  @return cryphash pointer
 */
std::shared_ptr<CryptHash> CryptHash::Create(std::shared_ptr<CryptProvider> pProvider, DWORD dwAlgorithmType)
{

  std::shared_ptr<CryptHash> pHash = std::shared_ptr<CryptHash>(new CryptHash(pProvider, dwAlgorithmType));

  if ( pHash == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate CryptHash", "CryptHash", "Create" ));

  pHash->CreateHash();

  return pHash;

} // Create


/**
 *  Create
 * 
 *  @param dwAlgorithmType
 *  @return crypthash pointer
 */
std::shared_ptr<CryptHash> CryptHash::Create(DWORD dwAlgorithmType)
{

  std::shared_ptr<CryptHash> pHash = std::shared_ptr<CryptHash>(new CryptHash(dwAlgorithmType));

  if ( pHash == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate CryptHash", "CryptHash", "Create" )); 

  pHash->CreateHash();

  return pHash;

} // Create


/**
 *  create new one
 */
void CryptHash::CreateHash()
{

  if ( m_hHash == NULL )
  {
    m_pProvider->Acquire();

    // =============
    // Create Hash
    // ============
    if ( ! CryptCreateHash( m_pProvider->GetHandle(), m_dwAlgorithmType , NULL, 0, &m_hHash) )
      throw ex::CryptException( LogRec( "Failed to create hash", "CryptHash", "CreateHash" )); 

  }

} // CreateHash


/**
 *  Destroy 
 */
void CryptHash::DestroyHash()
{

  if ( m_hHash != NULL )
  {
    if ( ! CryptDestroyHash(m_hHash) )
      printf("Failed to destroy hash handle\n");

    m_hHash = NULL;
  }

} // DestroyHash()


/**
 *  hash
 *
 *  @param sText
 */
void CryptHash::Hash(const std::string & sText)
{

  if ( m_hHash == NULL )
    throw ex::UnspecifiedCryptException( LogRec( "Hash has not been created", "CryptHash", "Hash" )); 

  // ==================
  // Hash the password
  // ===================
  if ( ! CryptHashData( m_hHash, (BYTE*)sText.c_str(), sText.size(), 0) )
    throw ex::CryptException( LogRec( "Failed to hash text", "CryptHash", "Hash" ));

} // Hash


/**
 *  get value
 *
 *  @param the string value
 */
std::string CryptHash::GetValue()
{

  std::string sReturn;

  DWORD dwValueLength(0);
  DWORD dwSize = sizeof(DWORD);

  if( ! CryptGetHashParam(m_hHash, HP_HASHSIZE,(BYTE *)&dwValueLength, &dwSize,0))
  {
    throw ex::CryptException( LogRec( "Failed to extract hash value length", "CryptHash", "Hash" ));
  } // if


  BYTE * hashValue = new BYTE[dwValueLength];

  RtlZeroMemory((char *)hashValue, dwValueLength);


  if ( ! CryptGetHashParam(m_hHash,HP_HASHVAL,hashValue,&dwValueLength,0) )
  {
    throw ex::CryptException( LogRec( "Failed to extract hash value", "CryptHash", "Hash" ));
  } // if


  for ( register int i(0); i < dwValueLength; i++ )
    sReturn.push_back(hashValue[i]);

  return sReturn;

} // GetValue

