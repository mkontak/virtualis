// $Id: MonitorDisplay.h,v 1.1.2.8 2010/03/24 07:21:32 dongqing Exp $
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
#include <map>


/// Win utilities
namespace WinUtils
{

  /**
   * Class for handling multi-monitor display setups
   */
  class MonitorDisplay
  {
  public:
    /// whether or not widescreen display will be accounted for
    enum WindowLayoutEnum { WL_AUTODETECT, WL_STANDARD, WL_WIDESCREEN, WL_PORTRAIT, WL_MAX, };

  // construction
  public:
    /// Constructor
    MonitorDisplay() { }

    /// Destructor
    ~MonitorDisplay() { }

  // member functions
  public:

    /// set the appropriate monitor
    static void SetCurrentMonitor(int4 iMonitor = 0);
    /// get the current monitor setting
    static int4 GetCurrentMonitorSetting();

    /// get info on all attached monitor displays
    static std::map<int4,DEVMODE> EnumAttachedDisplays();

    /// whether the current monitor is widescreen
    static bool IsWideScreen();

    /// whether to detect for resolution or force a particular layout
    static WindowLayoutEnum GetWindowLayoutDetect();

    /// whether the current resolution is portrait orientation
    static bool IsPortrait();

    /// returns the display rectangle for the monitor on which the application is currently being displayed
    static const CRect GetCurrentDisplayRect();

    /// returns the display name
    static std::string GetLayoutDisplayName( WindowLayoutEnum eLayout );

  private:

    /// monitor enumeration callback
    static BOOL CALLBACK MyMonitorEnumProc(HMONITOR hMon, HDC hDCMon, LPRECT pRCMon, LPARAM dwData);

  // member variables
  private:

    /// information on local displays
    typedef std::map<std::string, std::pair< HDC, MONITORINFOEX > > MapMonitorInfo;
    static MapMonitorInfo m_mapMonitorInfo;

    /// current monitor index
    static std::pair< bool, int4 > s_currentMonitor;

  }; // MonitorDisplay

} // namespace WinUtils


// $Log: MonitorDisplay.h,v $
// Revision 1.1.2.8  2010/03/24 07:21:32  dongqing
// use selected monitor for calculations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.7  2010/02/04 21:20:28  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.6  2009/12/09 19:37:45  dongqing
// is portrait orientation check
//
// Revision 1.1.2.5  2009/11/24 21:53:19  dongqing
// user can choose whether to auto-detect widescreen
//
// Revision 1.1.2.4  2009/11/16 21:56:39  dongqing
// IsWideScreen method on current display.
//
// Revision 1.1.2.3  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.2  2009/01/21 04:22:02  jmeade
// set saved/current monitor
//
// Revision 1.1.2.1  2008/04/18 22:16:38  jmeade
// multi-monitor support.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MonitorDisplay.h,v 1.1.2.8 2010/03/24 07:21:32 dongqing Exp $
// $Id: MonitorDisplay.h,v 1.1.2.8 2010/03/24 07:21:32 dongqing Exp $
