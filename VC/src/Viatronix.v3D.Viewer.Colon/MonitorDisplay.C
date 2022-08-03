// $Id: MonitorDisplay.C,v 1.1.2.12 2010/04/15 12:26:10 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MonitorDisplay.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "MonitorDisplay.h"
#include "Viatron.h"
#include "ApplicationSettings.h"

using namespace WinUtils;

std::map<std::string, std::pair< HDC, MONITORINFOEX > > MonitorDisplay::m_mapMonitorInfo;
std::pair< bool, int4 > MonitorDisplay::s_currentMonitor( std::make_pair( false, -1 ));


/**
 * move the application to the specified monitor
 *
 * @param iMonitor   identifies monitor for application
 */
void MonitorDisplay::SetCurrentMonitor(int4 iMonitor)
{
  m_mapMonitorInfo.clear();
  EnumDisplayMonitors(NULL, NULL, MyMonitorEnumProc, LPARAM(NULL));

  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_LOCAL ));
  if( iMonitor < 0 )
    state.TryGet<int4>( APP_SETTING_VIEWER_MONITOR, iMonitor );

  DISPLAY_DEVICE dispdev; dispdev.cb = sizeof(dispdev);
  if ((iMonitor >= 0) && (EnumDisplayDevices(NULL, iMonitor, &dispdev, 0) == TRUE))
  {
    MapMonitorInfo::iterator itr = m_mapMonitorInfo.find(dispdev.DeviceName);
    if (itr != m_mapMonitorInfo.end())
    {
      AfxGetMainWnd()->ShowWindow(SW_SHOWNORMAL);
      AfxGetMainWnd()->ShowWindow(SW_HIDE);

      state.SetInt( APP_SETTING_VIEWER_MONITOR, iMonitor );
      s_currentMonitor.second = iMonitor;

      AfxGetMainWnd()->SetWindowPos(NULL, itr->second.second.rcMonitor.left,itr->second.second.rcMonitor.top,
        itr->second.second.rcMonitor.right-itr->second.second.rcMonitor.left,itr->second.second.rcMonitor.bottom-itr->second.second.rcMonitor.top,
        SWP_NOZORDER);

      AfxGetMainWnd()->ShowWindow(SW_SHOWMAXIMIZED);
    }
  }
  else
  {
    for ( MapMonitorInfo::const_iterator it = m_mapMonitorInfo.begin(); it != m_mapMonitorInfo.end(); it++ )
    {
      if ( (m_mapMonitorInfo.size()==1) || (AfxMessageBox((std::string("Use this monitor?  ") + it->second.second.szDevice).c_str(), MB_YESNO) == IDYES) )
      {
        AfxGetMainWnd()->ShowWindow(SW_SHOWNORMAL);

        AfxGetMainWnd()->SetWindowPos(NULL, it->second.second.rcMonitor.left,it->second.second.rcMonitor.top,
          it->second.second.rcMonitor.right-it->second.second.rcMonitor.left,it->second.second.rcMonitor.bottom-it->second.second.rcMonitor.top,
          SWP_NOZORDER);

        AfxGetMainWnd()->ShowWindow(SW_SHOWMAXIMIZED);

        break;
      }
    }
  }

} // SetCurrentMonitor( iMonitor )


/**
 * retrieves the current saved setting for the application monitor
 *
 * @return    the current setting for monitor display of the application
 */
int4 MonitorDisplay::GetCurrentMonitorSetting()
{
  if( !s_currentMonitor.first )
  {
    s_currentMonitor.first = true;
    ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_LOCAL ));
    s_currentMonitor.second = state.Get<int4>( APP_SETTING_VIEWER_MONITOR );
  }

  return s_currentMonitor.second;
} // GetCurrentMonitorSetting()


/**
 * get information on a single display monitor
 * @param hMon     handle to monitor
 * @param hDCMon   handle to device context
 * @param pRCMon   monitor info
 * @param dwData   unused
 * @return         whether info request succeeded
 **/
BOOL CALLBACK MonitorDisplay::MyMonitorEnumProc(HMONITOR hMon, HDC hDCMon, LPRECT pRCMon, LPARAM dwData)
{
  MONITORINFOEX moninfo; moninfo.cbSize = sizeof(moninfo);
  GetMonitorInfo(hMon, &moninfo);
  m_mapMonitorInfo[moninfo.szDevice] = std::make_pair(hDCMon, moninfo);

  return TRUE;
} // MyMonitorEnumProc( hMon, hDCMon, pRCMon, dwData )


/**
 * gets a list of all the computer's attached displays
 * @return a list of all the computer's attached displays
 **/
std::map<int4,DEVMODE> MonitorDisplay::EnumAttachedDisplays()
{
  std::map<int4,DEVMODE> lDisplays;

  m_mapMonitorInfo.clear();
  EnumDisplayMonitors(NULL, NULL, MyMonitorEnumProc, LPARAM(NULL));

  //{{ Init monitor selections
  DISPLAY_DEVICE dispdev; dispdev.cb = sizeof(dispdev);
  for (int4 iDev(0); EnumDisplayDevices(NULL, iDev, &dispdev, 0) != FALSE; iDev++)
  {
    if (dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
    {
      DEVMODE devmode; devmode.dmSize = sizeof(devmode);
      EnumDisplaySettings(dispdev.DeviceName, ENUM_CURRENT_SETTINGS, &devmode);
      strcpy((char *)devmode.dmFormName, dispdev.DeviceName);

      lDisplays[iDev] = devmode;
    }
  }

  return lDisplays;

} // EnumAttachedDisplays()


/**
 * whether the current monitor is widescreen
 * @return whether the current monitor is widescreen
 */
bool MonitorDisplay::IsWideScreen()
{
  CRect rtMon(GetCurrentDisplayRect());
  const int4 cx(rtMon.Width());
  const int4 cy(rtMon.Height());

  const bool bIsWS( cy != 0 ? (float4(cx)/float4(cy) > 1.37 ) : false );

  const MonitorDisplay::WindowLayoutEnum eDetectWS(MonitorDisplay::GetWindowLayoutDetect());
  return ( eDetectWS == MonitorDisplay::WL_AUTODETECT ? bIsWS : eDetectWS == MonitorDisplay::WL_WIDESCREEN );

} // IsWideScreen()


/**
 * whether to detect for widescreen resolution
 * @return whether to detect for widescreen resolution
 */
MonitorDisplay::WindowLayoutEnum MonitorDisplay::GetWindowLayoutDetect()
{
  MonitorDisplay::WindowLayoutEnum layout( WL_AUTODETECT );
  ApplicationSettings::UserSettings().TryGet< MonitorDisplay::WindowLayoutEnum >( APP_SETTING_DETECT_WIDESCREEN, layout );
  return layout;
} // GetWidescreenDetect()


/**
 * whether the current monitor is widescreen
 * @return whether the current monitor is widescreen
 */
bool MonitorDisplay::IsPortrait()
{
  // display resolution
  CRect rtMon(GetCurrentDisplayRect());
  const bool bIsPort(rtMon.Width() < rtMon.Height());

  const MonitorDisplay::WindowLayoutEnum eDetectWS(MonitorDisplay::GetWindowLayoutDetect());
  return ( eDetectWS == MonitorDisplay::WL_AUTODETECT ? bIsPort : eDetectWS == MonitorDisplay::WL_PORTRAIT);

} // IsPortrait()


/**
 * returns the display rectangle for the monitor on which the application is currently being displayed
 * @return display rectangle
 */
const CRect MonitorDisplay::GetCurrentDisplayRect()
{
  int4 iMon = GetCurrentMonitorSetting();
  DISPLAY_DEVICE dispdev; dispdev.cb = sizeof(dispdev);
  if ((iMon >= 0) && (EnumDisplayDevices(NULL, iMon, &dispdev, 0) == TRUE))
  {
    MapMonitorInfo::const_iterator itr = m_mapMonitorInfo.find(dispdev.DeviceName);
    if (itr != m_mapMonitorInfo.end())
    {
      return CRect(itr->second.second.rcMonitor);
    }
  }

  return CRect(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
} // GetCurrentDisplayRect()


/**
 * Converts the enum to string.
 *
 * @param     eLayout         layout enumeration
 * @return    string name for the enumeration
 */
std::string MonitorDisplay::GetLayoutDisplayName( WindowLayoutEnum eLayout )
{
  switch( eLayout )
  {
  case WL_AUTODETECT:
    return "Auto Detect";
  case WL_STANDARD:
    return "Standard (4:3)";
  case WL_WIDESCREEN:
    return "Widescreen (16:9)";
  case WL_PORTRAIT:
    return "Portrait";
  default:
    throw ex::IndexOutOfBoundsException( LogRec( "Invalid layout", "MonitorDisplay", "GetLayoutDisplayName" ));
  }
} // GetLayoutDisplayName( eLayout )


// Revision History:
// $Log: MonitorDisplay.C,v $
// Revision 1.1.2.12  2010/04/15 12:26:10  dongqing
// portrait detection to be sensitive of user's layout detection option selection
//
// Revision 1.1.2.11  2010/03/30 18:12:35  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.9  2010/03/24 07:57:54  dongqing
// set the intended monitor registry setting prior to moving window, so that sizing handlers can detect the proper display
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.8  2010/03/24 07:21:31  dongqing
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
// Revision 1.1.2.4  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.3  2009/02/03 22:23:54  jmeade
// Issue 6218.
//
// Revision 1.1.2.2  2009/01/21 04:22:02  jmeade
// set saved/current monitor
//
// Revision 1.1.2.1  2008/04/18 22:16:38  jmeade
// multi-monitor support.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MonitorDisplay.C,v 1.1.2.12 2010/04/15 12:26:10 dongqing Exp $
// $Id: MonitorDisplay.C,v 1.1.2.12 2010/04/15 12:26:10 dongqing Exp $
