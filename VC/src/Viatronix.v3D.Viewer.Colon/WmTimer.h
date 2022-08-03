// $Id: WmTimer.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include <afxwin.h>     // MFC core and standard components
#include "typedef.h"

/// Win utilities
namespace WinUtils
{

  /// Class for setting/resetting/killing timers
  class WmTimer
  {
  public:
    /// Constructor
    inline WmTimer() : m_uTimerId(0), m_hWnd(NULL) { }

    /// Destructor, kills timer
    inline ~WmTimer() { KillTimer(); }

    /// Type-cast operator
    inline operator uint4() { return m_uTimerId; }

    /// Sets the timer only if one doesn't exists, in milliseconds
    inline void SetTimer(HWND hWnd, const uint4 uElapse)
    {
      if (!IsSet())
      {
        m_uTimerId = ::SetTimer(m_hWnd = hWnd, GetNewTimerId(), uElapse, NULL);
      }
    } // SetTimer()

    /// Resets the timer if one already exists, in milliseconds
    inline void ResetTimer(HWND hWnd, const uint4 uElapse)
    {
      if (IsSet())
      {
        m_uTimerId = ::SetTimer(m_hWnd = hWnd, m_uTimerId, uElapse, NULL);
      }
      else
      {
        SetTimer(hWnd, uElapse);
      }
    } // ResetTimer()

    /// Sets the timer only if one doesn't exists, in milliseconds
    inline void SetTimer(CWnd* pWnd, const uint4 uElapse) { SetTimer(pWnd->GetSafeHwnd(), uElapse); }

    /// Resets the timer if one already exists, in milliseconds
    inline void ResetTimer(CWnd* pWnd, const uint4 uElapse) { ResetTimer(pWnd->GetSafeHwnd(), uElapse); }
    
    /// Kills timer if it exists
    inline void KillTimer() { if (IsSet()) { ::KillTimer(m_hWnd, m_uTimerId); } m_uTimerId = 0; m_hWnd = NULL; }

    /// Returns true if timer is set, false otherwise
    inline bool IsSet() { return (m_uTimerId > 0); }

    /// Return the HWND that will receive the timer events
    inline const HWND GetTimerWnd() const { return m_hWnd; }

  private:
    /// Gets a new (assumed-to-be-unique) ID for a timer
    static inline uint4 GetNewTimerId() { return ((m_uGlobalNextTimerId++) % UINT_MAX); }

  private:
    /// Timer event id (returned from ::SetTimer())
    uint4 m_uTimerId;
    
    /// Handle to window owner for the timer (valid only when a timer is set)
    HWND m_hWnd;

    /// Global, unique id for a newly installed timer
    static uint4 m_uGlobalNextTimerId;
  }; // WmTimer

} // namespace WinUtils


// $Log: WmTimer.h,v $
// Revision 1.4  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.3  2006/01/31 14:28:56  frank
// code review
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1.10.1  2003/07/24 18:30:34  jmeade
// Child view timers.
//
// Revision 3.1  2002/03/11 20:29:13  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.0.4.1  2002/02/06 23:14:15  jmeade
// Is timer set query function.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:40:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:52   ingmar
// Initial revision.
// Revision 1.4  2001/08/23 21:46:14  jmeade
// Better comments
//
// Revision 1.3  2001/08/01 20:38:40  jmeade
// Use a static var for new timer IDs
//
// Revision 1.2  2001/06/20 19:53:20  jmeade
// no message
//
// Revision 1.1  2001/06/08 00:31:36  jmeade
// class for getting/setting/killing timers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WmTimer.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
// $Id: WmTimer.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
