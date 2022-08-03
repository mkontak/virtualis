// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: AESCrypt.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"
#include "crypto\AESCrypt.h"
#include "crypto\CryptKey.h"
#include "crypto\CryptData.h"
#include "crypto\CryptHash.h"
#include "Exception.h"
#include "Base64.h"
#include "BitConverter.h"

using namespace crypto;

std::string AESCrypt::m_sKey("");
const char AESCrypt::m_password[]  = { 0x65, 0x70, 0x6C, 0x75, 0x72, 0x69, 0x62, 0x75, 0x73, 0x55, 0x6E, 0x75, 0x6D, 0x39, 0x39, 0x21, 0x00 };  // epluribusUnum99!
const char AESCrypt::m_salt[] = { 0x6F, 0x36, 0x38, 0x30, 0x36, 0x36, 0x34, 0x32, 0x6B, 0x62, 0x4D, 0x37, 0x63, 0x35 , 0x00 };                  // o6806642kbM7c5


/**
 * Encrypts the specified text
 *
 * @param sPlainText    Plain text
 * @param sKey          Key to use for encrypting
 * @param bHexEncode    Encode string as base64
 *
 * @return Encrypted text
 */
std::string AESCrypt::Encrypt(const std::string & sPlainText, const std::string & sKey, bool bHexEncode )
{

  if ( sPlainText.empty() )
    return sPlainText;

  LogErr(util::Strings::Format("Encrypting text (key supplied=%s)", (sKey.empty(), "false", "true")), "AESCrypt","Encrypt");

  // Create the key
  std::shared_ptr<CryptKey> pKey = ( !sKey.empty() ) ? CryptKey::Create( sKey ) : CryptKey::Create( );

   
  // Create the data blob
  crypto::CryptData data(sPlainText);

  
  // Encrypt
  if ( ! CryptEncrypt( pKey->GetHandle(), NULL, true, CRYPT_OAEP , data.m_blob.pbData, &data.m_blob.cbData, data.m_iAllocatedSize) )
    throw ex::CryptException( LogRec( "Failed to encrypt text", "AESCrypt", "Encrypt" ));


  // Encode to a bit stream
  if( bHexEncode )
    return text::BitConverter::ToString( data.ToString() );
  else
    return Base64::Encode(data.ToString());
} // Encrypt


/**
 * Decrypts the specified text
 *
 * @param sCipherText   Encrypted text
 * @param sKey          Key to be used for decrypting
 * @param bHexEncode    Encode as base64
 *
 * @return Decrypted text
 */
std::string AESCrypt::Decrypt(const std::string & sCipherText, const std::string & sKey, bool bHexEncode )
{
    LogErr(util::Strings::Format("Decrypting text (key supplied=%s)", (sKey.empty(), "false", "true")), "AESCrypt","Decrypt");

  // Create the key
  std::shared_ptr<CryptKey> pKey = ( !sKey.empty() ) ? CryptKey::Create( sKey ) : CryptKey::Create();

  // Create the data blob
  std::string decoded;
  if( bHexEncode )
  {
    uint4 size = sCipherText.length() / 2;
    std::shared_ptr< char > buffer( new char[ size ], []( const char * p ) { delete[] p; });
    text::BitConverter::ToBuffer( sCipherText.c_str(), sCipherText.length(), buffer.get(), size );
    decoded.assign( buffer.get() );
  }
  else
    decoded = Base64::Decode( sCipherText );

  crypto::CryptData data( decoded );

  
  // Decrypt
  if ( ! CryptDecrypt( pKey->GetHandle(), NULL, true, CRYPT_OAEP , data.m_blob.pbData, &data.m_blob.cbData ) )
    throw ex::CryptException( LogRec( "Failed to decrypt text", "AESCrypt", "Decrypt" ));


  // Returns the string
  return data.ToString();

} // Decrypt(const std::string & sCipherText)


