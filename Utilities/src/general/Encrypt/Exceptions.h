// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Exeptions.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


/**
* Implements a base launch exception
*/
class EncryptionException
{
public:

	EncryptionException(CString sMessage) : m_sMessage(sMessage) { }


	CString GetMessage() { return m_sMessage; }
private:

	CString m_sMessage;
};

/**
* Invalid data exception
*/
class InvalidDataException : public EncryptionException
{
public:

	InvalidDataException(CString sMessage) : EncryptionException(sMessage) { }

};


/**
* Usage exception
*/
class UsageException : public EncryptionException
{
public:

	UsageException(CString sMessage) : EncryptionException(sMessage) { }

};

/**
* InvalidPasswordException exception
*/
class InvalidPasswordException : public EncryptionException
{
public:

  InvalidPasswordException(CString sMessage) : EncryptionException(sMessage) { }

};
