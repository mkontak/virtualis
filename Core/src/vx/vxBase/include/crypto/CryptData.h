// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptData.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once



namespace crypto
{

class VX_BASE_DLL DerivedKey;
class VX_BASE_DLL AESCrypt;

/**
 * Wraps the data to be encryted 
 */
class VX_BASE_DLL CryptData
{

public:

  /// Default constructor
  CryptData();

  /// Constructs the CryptData from the text string
  CryptData(const std::string & sText);

  /// Creates a CryptData of a specified size
  CryptData(int iSize);

  /// Cleans up memory
  ~CryptData();

  /// Returns the string 
  std::string ToString();

public:

  /// Gets the data size
  const DWORD GetSize() const
  { return m_blob.cbData; }

  /// Gets the BYTE data
  const BYTE * GetData() const
  { return m_blob.pbData; }

private:

  /// Allocated size
  int m_iAllocatedSize;

  /// Crypt BLOB
  CRYPT_DATA_BLOB m_blob;

  /// Friend classes
  friend class VX_BASE_DLL DerivedKey;
  friend class VX_BASE_DLL AESCrypt;

};  // class CryptData


}  // namespace crypto