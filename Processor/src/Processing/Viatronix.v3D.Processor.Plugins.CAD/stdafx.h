// $Id: stdafx.h,v 1.7.2.1 2008/01/18 17:15:02 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )
//

#pragma once


#define _WIN32_WINNT  0x0501

#define _CRT_SECURE_NO_DEPRECATE
#define _SECURE_SCL 0



#include <iostream>
#include <tchar.h>

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS

#include <atlbase.h>
#include <atlcom.h>

#include <lm.h>

#include "Logger.h"
#include "vxBlockVolumeLibrary.h"
#include "Triple.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include  "vxMessagingLibrary.h"
#include "vxDOmDocument.h"
#include "CmdLineSwitches.h"
#include "Timer.h"

using namespace ATL;

inline HRESULT WINAPI _This(void* pv, REFIID iid, void** ppvObject, DWORD_PTR) 
{
	ATLASSERT(iid == IID_NULL);
	*ppvObject = pv;
	return S_OK;
}

#define COM_INTERFACE_ENTRY_THIS() COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)

template<class T>
inline T* GetCoClassPtr(IUnknown* punk)
{
	T* p = NULL;
	punk->QueryInterface(IID_NULL, reinterpret_cast<void**>(&p));
	ATLASSERT(p);
	return p;
}

// $Log: stdafx.h,v $
// Revision 1.7.2.1  2008/01/18 17:15:02  geconomos
// disabled iterator debugging
//
// Revision 1.7  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.2.1  2005/12/19 15:50:53  geconomos
// added header
//
// Revision 1.5  2005/09/23 12:10:11  geconomos
// added more includes
//
// Revision 1.4  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.3  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:39:01  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/stdafx.h,v 1.7.2.1 2008/01/18 17:15:02 geconomos Exp $
// $Id: stdafx.h,v 1.7.2.1 2008/01/18 17:15:02 geconomos Exp $
