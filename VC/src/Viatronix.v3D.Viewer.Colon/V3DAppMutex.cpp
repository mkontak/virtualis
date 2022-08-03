// $Id: V3DAppMutex.cpp,v 1.2 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "V3DAppMutex.h"

using namespace VxConsole;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VxConsole namespace Globals

const int1 * VxMutex::sV3D_VC_CONSOLE_MUTEX_NAME = "Viatronix.v3D.AppMutex.Name.Console";
const int1 * VxMutex::sV3D_VC_VIEWER_MUTEX_NAME = "Viatronix.v3D.AppMutex.Name.ColonViewer";
const int1 * VxMutex::sV3D_CS_VIEWER_MUTEX_NAME = "Viatronix.v3D.AppMutex.Name.CalciumScoringViewer";

const uint4 VxMutex::m_uRWM_ShowViewerMessageID(RegisterWindowMessage("Viatronix.v3D.WindowsMessage.Show.v3DViewer"));
const uint4 VxMutex::m_uRWM_CloseViewerMessageID(RegisterWindowMessage("Viatronix.v3D.WindowsMessage.Close.v3DViewer"));

const uint4 VxMutex::m_uRWM_ShowConsole(RegisterWindowMessage("Viatronix.v3D.WindowsMessage.Show.v3DConsole"));

const uint4 VxMutex::m_uRWM_v3DViewerLaunched(RegisterWindowMessage("Viatronix.v3D.WindowsMessage.ApplicationInit"));

const uint4 VxMutex::m_uRWM_v3DAppClosing(RegisterWindowMessage("Viatronix.v3D.WindowsMessage.ApplicationClosing"));


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VxConsole::SingleMutex class

/**
 * Constructor; Constructs the mutex and the lock.
 *
 * @param bInitiallyOwn   True to attempt to gain ownership of the mutex.
 * @param lpszName        Mutex name.
 * @param lpsaAttribute   Security attributes (NULL for default attributes).
 */
SingleMutex::SingleMutex(BOOL bInitiallyOwn, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpsaAttribute)
  : m_mutex(bInitiallyOwn, lpszName, lpsaAttribute), m_lock(&m_mutex, FALSE)
{
} // constructor


/**
 * Checks the lock state of the CSingleLock instance
 *
 * @return   True if mutex is locked, false otherwise.
 */
BOOL SingleMutex::IsLocked()
{
  return m_lock.IsLocked();
} // IsLocked()


/**
 * Locks the CSingleLock instance.
 *
 * @param dwTimeOut   Amount of time, in milliseconds, to wait to gain the lock.
 * @return            True if the lock was gained, false otherwise.
 */
BOOL SingleMutex::Lock(DWORD dwTimeOut)
{
  return m_lock.Lock(dwTimeOut);
} // Lock()


/**
 * Unlocks the CSingleLock instance.
 *
 * @return   True if the unlocking was successful.
 */
BOOL SingleMutex::Unlock()
{
  return m_lock.Unlock();
} // Unlock()


/**
 * Returns a handle to the internal mutex
 *
 * @return   Handle to the internal mutex.
 */
SingleMutex::operator HANDLE() const
{
  return HANDLE(m_mutex);
} // HANDLE()

// $Log: V3DAppMutex.cpp,v $
// Revision 1.2  2007/03/08 18:29:00  jmeade
// code standards.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.4.2.2  2003/07/07 20:31:25  jmeade
// Code standards, comments.
//
// Revision 1.4.2.1  2003/06/09 17:32:26  jmeade
// code review prep.
//
// Revision 1.4  2002/11/27 17:38:30  jmeade
// Changed to more succint message name.
//
// Revision 1.3  2002/11/21 18:22:33  jmeade
// Mutex and registered windows message naming.
//
// Revision 1.2  2002/11/21 01:24:36  jmeade
// No code in header, comments.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/V3DAppMutex.cpp,v 1.2 2007/03/08 18:29:00 jmeade Exp $
// $Id: V3DAppMutex.cpp,v 1.2 2007/03/08 18:29:00 jmeade Exp $
