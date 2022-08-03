// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ImpersonateUser.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

#pragma once


namespace sys
{

	class VX_BASE_DLL ImpersonateUser
	{

	public:

		/// Constructor
		ImpersonateUser();


		/// Destructor
		~ImpersonateUser();

		/// Logs on
		void Logon(const std::string & sUserName, const std::string & sPassword, const std::string & sDomain );

		/// Logs off
		void Logoff();

		// Gets the Error Code
		const DWORD GetErrorCode() const
		{ return m_dwErrorCode; }

		///Returns true if you are logged on
		const bool IsLoggedOn() const
		{ return m_bLoggedOn; }

	private:
		/// logon flag
		bool m_bLoggedOn;

		/// handle
		HANDLE m_hUserToken;

		/// error code
		DWORD m_dwErrorCode;

	};  // class ImpersonateUser

} // namespace sys

