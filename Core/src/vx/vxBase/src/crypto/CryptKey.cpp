// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptKey.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"
#include "crypto\CryptKey.h"
#include "Exception.h"
#include "crypto\CryptData.h"
#include "crypto\CryptProvider.h"
#include "crypto\CryptHash.h"



using namespace crypto;

/**
 * Constructor
 *
 * @param dwProviderId    Provider id to use
 *
 */
CryptKey::CryptKey(DWORD dwProviderId) :
m_pProvider(CryptProvider::Create(dwProviderId)),
m_hKey(NULL),
m_sKeyHash(""),
m_iKeySize(256)
{
} // CryptKey(


/**
 * Destructor
 */
CryptKey::~CryptKey()
{

  ReleaseResources();

} // ~CryptKey()


/**
 * Release the key's resources
 */
void CryptKey::ReleaseResources()
{
  if ( m_hKey != NULL )
  {
    if ( ! CryptDestroyKey(m_hKey) )
      printf("Failed to destroy key handle\n");

    m_hKey = NULL;
  }

  m_pProvider->Release();

} // ReleaseResources()




/**
 * Creates the key using the password and salt
 *
 * @param dwProviderId  Provider Id
 *
 * @return shared ptr of CryptKey
 */
std::shared_ptr<CryptKey> CryptKey::Create(int iKeySize,  DWORD dwProviderId ) 
{


  // ==============================
  // Set up for 256 but can be less
  // ==============================
  const BYTE keyhash[] = { 0x17, 0xee, 0x25, 0xb8, 0x7b, 0x24, 0x7e, 0x4b, 0x64, 0x1a, 0xd5, 0x94, 0x61, 0xd6, 0x57, 0x42, 0x25, 0xb5, 0x1e,
                           0x28, 0x95, 0x5f, 0x38, 0x9e, 0xd1, 0x74, 0x8b, 0x7d, 0x2b, 0x3f, 0x91, 0xdf, 0x29, 0x18, 0x64, 0xf2, 0x80, 0xfd, 
                           0x4b, 0xdd, 0xe0, 0x1d, 0xcb, 0xe1, 0x91, 0x9c, 0x5e, 0x4e, 0x00 };

  std::string sKeyHash((char *)keyhash);

  LogErr( util::Strings::Format("The key hash is (%d)", sKeyHash.size()), "CryptKey", "Create" );

  return Create(  sKeyHash, iKeySize, dwProviderId );

} // Create(const std::string & sPassword, const std::string & sSalt, DWORD dwProviderId ) 



/**
 * Creates the key using the password and salt
 *
 * @param sKey     Key hash
 * @param iKeySize
 *
 * @return shared ptr of CryptKey
 */
std::shared_ptr<CryptKey> CryptKey::Create(const std::string & sKeyHash, int iKeySize, DWORD dwProviderId ) 
{

  // Allocate object
  std::shared_ptr<CryptKey> pCryptKey(std::shared_ptr<CryptKey>(new CryptKey(dwProviderId)));

  // Make sure the objet was allocate
  if ( pCryptKey == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate CryptKey", "CryptKey", "Create" ));

  try
  {


    const int keysize(iKeySize/8);
    const int ivsize(128/8);

    // ================================================================
    // Make sure the key hash is big enough to extract the information
    // =================================================================
    if ( sKeyHash.size() < (keysize + ivsize) )
      throw ex::UnspecifiedCryptException( LogRec( util::Strings::Format("The key hash is too small (%d < %d)", sKeyHash.size(), (keysize + ivsize) ), "CryptKey", "Create" ));


    // Acquire Provider
    pCryptKey->m_pProvider->Acquire();

    const unsigned char * pKeyHash((unsigned char *)sKeyHash.c_str());
 
    // Key Blob structure
    struct {
      BLOBHEADER header;
      DWORD cbKeySize;
      BYTE bKeyData[256/8];
    } keyBlob;


    // Initialize the KeyBlob
    keyBlob.header.bType      = PLAINTEXTKEYBLOB;
    keyBlob.header.bVersion   = CUR_BLOB_VERSION;
    keyBlob.header.reserved   = 0;
    keyBlob.header.aiKeyAlg   = CALG_AES_256;
    keyBlob.cbKeySize         = keysize;
    RtlZeroMemory(keyBlob.bKeyData,keysize);
    memcpy(keyBlob.bKeyData,pKeyHash, keysize);


    const int iBlobSize(sizeof(BLOBHEADER) + sizeof(DWORD) + keysize);
  
    // =================
    // Import the Key
    // ================
    if ( ! CryptImportKey(pCryptKey->m_pProvider->GetHandle(), (BYTE *)&keyBlob, iBlobSize, NULL, CRYPT_OAEP , &pCryptKey->m_hKey) )
    {
      pCryptKey->ReleaseResources();
      throw ex::CryptException( LogRec( "Failed to import key", "CryptKey", "Create" ));
    } // if

    pKeyHash += keysize;

    // Assume the Block size is 128 bits max
    if ( ! CryptSetKeyParam(pCryptKey->m_hKey, KP_IV, pKeyHash, 0) )
    {
      pCryptKey->ReleaseResources();
      throw ex::CryptException( LogRec( "Failed to set the IV for the key", "CryptKey", "Create" ));
    } // if

    pCryptKey->m_sKeyHash = sKeyHash;

    return pCryptKey;

  }
  catch( ex::CryptException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::UnspecifiedCryptException( LogRec( "Failed to create the key, unspecified error", "CryptKey", "Create" ));
  }
             
} // Create(const std::string & KeyHash, int iKeySize, DWORD dwProviderId ) 





/**
 * Saves the Key Hash to a binary file
 *
 * @param sKeyFile      File to save the key to
 */
void CryptKey::SaveKey(const std::string & sFilePath)
{
  
  // Open file
  int fh = _open(sFilePath.c_str(),_O_BINARY | _O_WRONLY | _O_CREAT, _S_IWRITE );

  if ( fh != -1 )
  {
    // Wirte the key hash
    _write(fh, (void *)m_sKeyHash.c_str(), m_sKeyHash.size());

    // Close the file
    _close(fh);
  }
  else
    throw ex::UnspecifiedCryptException( LogRec( "Failed to open file " + sFilePath, "CryptKey", "SaveKey" ));

} // SaveKey(const std::string & sFilePath)


/**
 * Reads the key from the file
 *
 * @param sKeyFile      File to save the key to
 */
std::string CryptKey::ReadKey(const std::string & sFilePath)
{

  // Open file
  int fh = _open(sFilePath.c_str(),_O_BINARY | _O_RDONLY | _O_CREAT, _S_IREAD );


  char key[1024];
  RtlZeroMemory(key,1024);

  if ( fh != -1 )
  {
    // read the key hash
    int bytesRead = _read(fh, (void *)key, 1024);

    // Close the file
    _close(fh);
  }

  return std::string(key);

} // ReadKey(const std::string & sFilePath)
