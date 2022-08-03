
// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptExceptions.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

namespace crypto
{


class VX_BASE_DLL CryptException
{
public:

	/// constructor
  CryptException(const std::string & sMessage, DWORD dwErrorCode = 0) :
  m_sMessage(sMessage),
  m_dwErrorCode(dwErrorCode)
  {
  } // 


public:
	/// get message
  const std::string & GetMessage() const
  { return m_sMessage; }

	/// get error code
  const DWORD & GetErrorCode() const
  { return m_dwErrorCode; }

private:
	/// message string
  std::string m_sMessage;

	/// error code
  DWORD m_dwErrorCode;
};  // class CryptException


class VX_BASE_DLL CryptOutOfMemoryException : CryptException
{
public:

	/// constructor
  CryptOutOfMemoryException(const std::string & sMessage, DWORD dwErrorCode = 0) :
  CryptException(sMessage,dwErrorCode)
  {
  }

};  // class CryptOutOfMemoryException


};  // namespace crypto