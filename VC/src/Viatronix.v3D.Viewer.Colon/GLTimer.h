// $Id: GLTimer.h,v 1.3.12.1 2007/11/08 16:51:55 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade (jmeade@viatronix.com)


// pragmas
#pragma once


// includes
#include "GLFont.h"
#include "typedef.h"
#include "GLChildView.h"
//#include "Rect.h"
//#include "WmTimer.h"
//#include "V1KMessages.h"
//#include "GLUpdateBuffer.h"
//#include <math.h>

namespace WinUtils
{

  /**
  * Win timer for GLChildViews
  */
  class GLTimer : public WmTimer
  {
  // methods
  public:

    /// Constructor
    inline GLTimer() : WmTimer() {}

    /// Set a timer
    inline void SetTimer(const ColonViews::ChildViews::GLChildView& childView, const uint4 uElapse)
    {
      WmTimer::SetTimer(childView.GetParent(), uElapse);
      childView.GetParent()->PostMessage( WM_APP_CHILDTIMER_STATUSCHANGE, uint4(*this), childView.GetControlID() );
    } // SetTimer()

    /// Reset timer
    inline void ResetTimer(const ColonViews::ChildViews::GLChildView& childView, const uint4 uElapse)
    {
      WmTimer::ResetTimer(childView.GetParent(), uElapse);
      childView.GetParent()->PostMessage( WM_APP_CHILDTIMER_STATUSCHANGE, uint4(*this), childView.GetControlID());
    } // ResetTimer()

    /// Unset a timer
    inline void KillTimer()
    {
      WmTimer::KillTimer();
      ::PostMessage(GetTimerWnd(), WM_APP_CHILDTIMER_STATUSCHANGE, uint4(*this), 0);
    } // KillTimer()

  private:

    /// Overridden to disable use; use GLChildView-parameter equivalent
    inline void SetTimer(CWnd* pWnd, const uint4 uElapse);

    /// Overridden to disable use; use GLChildView-parameter equivalent
    inline void SetTimer(HWND hWnd, const uint4 uElapse);

    /// Overridden to disable use; use GLChildView-parameter equivalent
    inline void ResetTimer(CWnd* pWnd, const uint4 uElapse);

    /// Overridden to disable use; use GLChildView-parameter equivalent
    inline void ResetTimer(HWND hWnd, const uint4 uElapse);

  }; // class GLTimer

} // namespace WinUtils

// $Log: GLTimer.h,v $
// Revision 1.3.12.1  2007/11/08 16:51:55  jmeade
// comments.
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.1.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 1.1.2.1  2005/04/13 19:33:43  frank
// split out GL Timer class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLTimer.h,v 1.3.12.1 2007/11/08 16:51:55 jmeade Exp $
// $Id: GLTimer.h,v 1.3.12.1 2007/11/08 16:51:55 jmeade Exp $
