// $Id: V3DAppMutex.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: V3DAppMutex.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_V3DAPPMUTEX_H__BACDFD73_75F6_49D0_8F53_AAEACFB3F8B9__INCLUDED_)
#define AFX_V3DAPPMUTEX_H__BACDFD73_75F6_49D0_8F53_AAEACFB3F8B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include <afxmt.h>

/**
 * Helpers for ensuring only one copy of an application will exist at a time.
 */

namespace VxConsole
{
  /**
   * Application mutex class
   */
  class SingleMutex
  {
  public:
    /// Constructs the mutex and the lock
    SingleMutex(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
    /// Checks the lock state of the CSingleLock instance
    BOOL IsLocked();
    /// Locks the CSingleLock instance
    BOOL Lock(DWORD dwTimeOut = INFINITE);
    /// Unlocks the CSingleLock instance
    BOOL Unlock();
    /// Returns a handle to the internal mutex
    operator HANDLE() const;
  private:
    /// NOTE:  As the constructor (above) for the CSingleLock needs a valid CMutex to be properly constructed,
    ///         be aware that the CMutex member MUST BE DECLARED BEFORE the CSingleLock member in this class (below).
    /// The mutex instance
    CMutex m_mutex;
    /// The single lock instance
    CSingleLock m_lock;
  };

  
  /**
   * Wrapper class, just used to enclose/expose mutex names
   */
  class VxMutex : public SingleMutex
  {
  public:
    /// Constant name for v3D console mutex instance
    static const int1 * sV3D_VC_CONSOLE_MUTEX_NAME;
    /// Constant name for v3D viewer colon mutex instance
    static const int1 * sV3D_VC_VIEWER_MUTEX_NAME;
    /// Constant name for v3D viewer calcium scoring mutex instance
    static const int1 * sV3D_CS_VIEWER_MUTEX_NAME;

    // Registered/inter-application messages

    /// Message to show v3D Viewer
    static const uint4 m_uRWM_ShowViewerMessageID;
    /// Message to close v3D Viewer
    static const uint4 m_uRWM_CloseViewerMessageID;

    /// Message to bring the console (window) to front of window stack
    static const uint4 m_uRWM_ShowConsole;

    /// Message informing a launching app (e.g. console) that viewer has launched and loaded the intended study
    static const uint4 m_uRWM_v3DViewerLaunched;
    /// Message informing console that a v3D application is exiting
    static const uint4 m_uRWM_v3DAppClosing;
  };
}; // namespace VxConsole


#endif // !defined(AFX_V3DAPPMUTEX_H__BACDFD73_75F6_49D0_8F53_AAEACFB3F8B9__INCLUDED_)

// $Log: V3DAppMutex.h,v $
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3.2.2  2003/07/07 20:31:25  jmeade
// Code standards, comments.
//
// Revision 1.3.2.1  2003/06/09 17:32:26  jmeade
// code review prep.
//
// Revision 1.3  2002/11/21 18:22:33  jmeade
// Mutex and registered windows message naming.
//
// Revision 1.2  2002/11/21 01:24:36  jmeade
// No code in header, comments.
//
// Revision 1.1  2002/11/21 01:06:32  jmeade
// Moved app mutex code to separate file (for non-MFC compilation purposes).
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/V3DAppMutex.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
// $Id: V3DAppMutex.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
