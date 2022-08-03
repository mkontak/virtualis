// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ImpersonateUser.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#include "stdafx.h"
#include "ImpersonateUser.h"
#include "Exception.h"


using namespace sys;

#pragma warning(push, 3)
#pragma warning(disable : 4702)



/**
 * Constructor
 */
ImpersonateUser::ImpersonateUser() : 
m_bLoggedOn(false), 
m_hUserToken(NULL), 
m_dwErrorCode(0) 
{
} // ImpersonateUser()


/**
 * Destructor
 */
ImpersonateUser::~ImpersonateUser()
{

	RevertToSelf();

	if(m_hUserToken != __nullptr)
	{
		CloseHandle(m_hUserToken); // Do not throw an exception here (as we are in the destructor)
		m_hUserToken = __nullptr;
	}

	m_bLoggedOn = false;
} // ~ImpersonateUser()


#pragma warning(default : 4702)
#pragma warning(pop)


/**
 * Logon
 *
 *  @param sUserName
 *  @param sPassword
 *  @param sDomain
 */
void ImpersonateUser::Logon(const std::string & sUserName, const std::string & sPassword, const std::string & sDomain )
{

	if( m_bLoggedOn )
		Logoff();


  // =======================================
  // Make sure the user name was specified
  // =======================================
	if(sUserName.empty()) 
    throw ex::VException(LogRec("No user name specified","ImpersonateUser","Logon"));

  // =======================================
  // Make sure the user name was specified
  // =======================================
	if(sPassword.empty()) 
    throw ex::VException(LogRec("No password was specified","ImpersonateUser","Logon"));

  BOOL bLoggedOn(FALSE);

  // ======================================================================
  // If the DOMAIN is specified it is the system we are trying to access. 
  // ======================================================================
	if( ! sDomain.empty() ) 
		bLoggedOn = LogonUser(sUserName.c_str(), sDomain.c_str(), sPassword.c_str(), LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_WINNT50, &m_hUserToken);
	else
		bLoggedOn = LogonUser(sUserName.c_str(), sDomain.c_str(), sPassword.c_str(), LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &m_hUserToken);

	if( !bLoggedOn )
   throw ex::VException(LogRec(util::Strings::Format("Login failed [%ld]",GetLastError()),"ImpersonateUser","Logon"));


  // Now impersonate them
	if( !ImpersonateLoggedOnUser(m_hUserToken) )
   throw ex::VException(LogRec(util::Strings::Format("Impersonation failed [%ld]",GetLastError()),"ImpersonateUser","Logon"));

	m_bLoggedOn = true;

} // Logon()


/**
 * Log Off
 */
void ImpersonateUser::Logoff()
{

	if( ! m_bLoggedOn )
		return;

	RevertToSelf(); // Revert to our user

	if( m_hUserToken != __nullptr )
	{
		if( ! CloseHandle(m_hUserToken) )
			throw ex::VException(LogRec("CloseHandle Failed","ImpersonateUser","LogOff"));

		m_hUserToken = __nullptr;
	}

	m_bLoggedOn = false;
} // Logoff

