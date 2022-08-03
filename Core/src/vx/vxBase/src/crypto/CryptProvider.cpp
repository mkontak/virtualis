// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptProvider.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"
#include "crypto\CryptProvider.h"
#include "Exception.h"


using namespace crypto;


/**
 *  default constructor
 *
 *  @param dwProviderType
 */
CryptProvider::CryptProvider(DWORD dwProviderType) :
m_hProvider(NULL),
m_dwType(dwProviderType)
{
} // CyrptProvider


/**
 *  default destructor
 */
CryptProvider::~CryptProvider()
{
  Release();
} // ~CryptProvider()


/**
 * create
 *
 *  @param dwProviderType
 *  @return the pointer to the provider
 */
std::shared_ptr<CryptProvider> CryptProvider::Create(DWORD dwProviderType)
{

  std::shared_ptr<CryptProvider> pProvider = std::shared_ptr<CryptProvider>(new CryptProvider(dwProviderType));

  if ( pProvider == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate CryptProvider", "CryptProvider", "Create" ));

  return pProvider;

} // Create


/**
 *  acquire
 */
void CryptProvider::Acquire()
{

  if ( m_hProvider == NULL )
  {

    switch ( m_dwType )
    {
    case PROV_RSA_AES:
      {
        // ===================================================
        // Acquire the provider context for encryption/hashing 
        // ===================================================
        if ( ! CryptAcquireContext( &m_hProvider, __nullptr, MS_ENH_RSA_AES_PROV, m_dwType, CRYPT_VERIFYCONTEXT  ))
          throw ex::CryptException( LogRec( "Failed to create provider context", "CryptProvider", "Acquire" ));
      }
      break;

    default:
      {
        // ===================================================
        // Acquire the provider context for encryption/hashing
        // ===================================================
        if ( ! CryptAcquireContext( &m_hProvider, __nullptr, MS_ENHANCED_PROV, m_dwType, CRYPT_VERIFYCONTEXT  ))
          throw ex::CryptException( LogRec( "Failed to create provider context", "CryptProvider", "Acquire" ));
      }
      break;

    } // switch

  } // if
} // Acquire()


/**
 * release
 */
void CryptProvider::Release()
{
  // ====================================================
  // Only need to release if the provider was allocated
  // ====================================================
  if ( m_hProvider != NULL )
  {

    // =======================
    // Release the provider
    // ======================
    if ( ! CryptReleaseContext(m_hProvider,0) )
      LogWrn("Failed to release context", "CryptProvider", "Release");

    m_hProvider = NULL;

  } // END ... If the provider was allocated

} // Release