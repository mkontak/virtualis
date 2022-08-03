// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptStream.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "StdAfx.h"
#include "crypto\CryptStream.h"
#include "crypto\AESCrypt.h"

USING_CRYPTO_NS


/**
 *  encrypt
 *
 *  @param stream
 *  @param data
 *  @param key
 */
void CryptStream::Encrypt( std::ostream & stream, const std::string & data, const std::string & key )
{
  stream << AESCrypt::Encrypt( data, key );
} // Encrypt


/**
 *  Decrypt
 *
 *  @param stream
 *  @param key
 *  @return the decrypted string
 */
std::string CryptStream::Decrypt( std::istream & stream, const std::string & key )
{
  std::stringstream buffer;
  std::string temp;
  while( !stream.eof() )
  {
    stream >> temp;
    buffer << temp;
  }

  return AESCrypt::Decrypt( buffer.str(), key );
} // Decrypt


/**
 *  constructor
 */
CryptStream::CryptStream()
{
} // CryptStream()
