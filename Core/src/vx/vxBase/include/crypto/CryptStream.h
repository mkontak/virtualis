// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptStream.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include <iostream>
#include <string>

namespace crypto
{

/**
 * Implements a stream for encrypting and decrypting
 */
class VX_BASE_DLL CryptStream
{
public:
	/// encrypt
  static void Encrypt( std::ostream & stream, const std::string & data, const std::string & key = "" );

	/// decrypt
  static std::string Decrypt( std::istream & stream, const std::string & key = "" );

private:
	/// get crypt stream
  CryptStream();
};  // class CryptStream

} // namespace crypto