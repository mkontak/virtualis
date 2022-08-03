// $Id: ComUtilities.h,v 1.1 2005/11/15 19:17:32 geconomos Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#pragma once

inline HRESULT WINAPI _This( void* pv, REFIID iid, void** ppvObject, DWORD_PTR ) 
{
	ATLASSERT(iid == IID_NULL);
	*ppvObject = pv;
	return S_OK;
}

#define COM_INTERFACE_ENTRY_THIS() COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)

template<class T>
inline T* GetCoClass(IUnknown* punk)
{
	T* p = NULL;
	
	punk->QueryInterface(IID_NULL, reinterpret_cast<void**>(&p));
	
	ATLASSERT(p);
	
	return p;
}
