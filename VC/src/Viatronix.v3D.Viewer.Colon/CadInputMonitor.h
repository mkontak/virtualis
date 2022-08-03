// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CadInputMonitor.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

namespace ColonViews { namespace Layouts {
  class ColonViatronixWnd;
}};


class CadInputMonitor
{
public:

  /// constructor
  CadInputMonitor( ColonViews::Layouts::ColonViatronixWnd * pLayout, bool bEnabled = true );

  /// destructor
  ~CadInputMonitor();

  /// activates the monitor
  void On() { m_bActive = true; }

  /// deactivates the monitor
  void Off() { m_bActive = false; }

  /// monitors the keyboard input
  bool Monitor( UINT uChar, UINT uRepCnt, UINT uFlags );

private:

  /// is enabled
  bool m_bEnabled;

  /// is active
  bool m_bActive;

  /// current layout
  ColonViews::Layouts::ColonViatronixWnd * m_pLayout;

};

