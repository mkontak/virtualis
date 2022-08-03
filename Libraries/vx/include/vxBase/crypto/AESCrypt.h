// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: AESCrypt.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


namespace crypto
{

/**
 * Implements the AES 256bit encryption
 */
class VX_BASE_DLL AESCrypt
{
public:

  /// Returns the key
  static const std::string & GetKey() { return m_sKey; }

  /// Sets the Key to use with the encryption
  static void SetKey(const std::string & sKey) { m_sKey = sKey; }

  /// Encrypt plain text
  static std::string Encrypt(const std::string & sPlainText, bool bHexEncode ) { return Encrypt( sPlainText, GetKey(), bHexEncode ); }

  /// Encrypt plain text
  static std::string Encrypt(const std::string & sPlainText, const std::string & sKey = GetKey(), bool bHexEncode = false );

  /// Decrypt plaint text
  static std::string Decrypt(const std::string & sCipherText, bool bHexEncode ) { return Decrypt( sCipherText, GetKey(), bHexEncode ); }

  /// Decrypt plaint text
  static std::string Decrypt(const std::string & sCipherText, const std::string & sKey = GetKey(), bool bHexEncode = false );


  /// Decrypt 3.5 version
  static std::string Decrypt35(const std::string & sCipherText, const std::string & sKey = GetKey(), bool bHexEncode = false);

private:

  /// Key
  static std::string m_sKey;


  /// Standard password
  static const char m_password[];

  /// Standard salt
  static const char m_salt[];

};  // class AESCrypt


}  // namespase crypto