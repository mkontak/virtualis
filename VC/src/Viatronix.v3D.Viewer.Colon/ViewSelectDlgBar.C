// $Id: ViewSelectDlgBar.C,v 1.10 2007/03/12 20:47:11 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViewSelectDlgBar.C  
// Description:  
// Created:  
// Author: Jeff Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "viatron.h"
#include "ViewSelectDlgBar.h"
#include "V1KMessages.h"
#include "WinUtils.h"
#include "Theme.h"
#include "IToolTipProvider.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Dialogs;
using namespace WinUtils;

#define     IDC_CHILD_PROGRESS      1000
#define     IDC_CHILD_MOUSE         1001
#define     IDC_CHILD_STATUS        1002


/**
 * Constructor 
 */
ViewSelectDlgBar::ViewSelectDlgBar() 
{
} // ViewSelectDlgBar()


/**
 * Destructor 
 */
ViewSelectDlgBar::~ViewSelectDlgBar()
{
} // ~ViewSelectDlgBar()


/**
 * message map
 */
BEGIN_MESSAGE_MAP( ViewSelectDlgBar, TabStrip )
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNeedTextHandler )
END_MESSAGE_MAP()


/**
 * WM_CREATE, called during the creation phase of the window.
 *
 * @param   lpCreateStruct   Info on the window being created
 * @return                   Return 0 to continue the creation of the CWnd object. If –1, the window will be destroyed.
 */
int4 ViewSelectDlgBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( TabStrip::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  EnableToolTips( TRUE );

  const uint4 iTabStringIds[NUM_VIEW_SCREENS]
    = { IDS_STUDY_BROWSER, IDS_VERIFICATION, IDS_NAVIGATION, IDS_REPORT, IDS_STUDY_COMPLETE };

  const int4 iNumItems(sizeof(iTabStringIds) / sizeof(iTabStringIds[0]));

  // add the tabs
  TabStrip::BeginInit();
  for( uint4 i( 0 ); i < iNumItems; ++i ) 
    TabStrip::AddItem( TabStripItem( static_cast< const char * >( LoadResourceString( iTabStringIds[ i ] ) ) ) );
  TabStrip::EndInit();
  
  //// create the progress bar and position
  m_progressbar.Create( WS_CHILD|PBS_SMOOTH, CRect( 0, 0, 1, 1 ), this, IDC_CHILD_PROGRESS );
  
  //// create the mouse help control
  m_mouseHelp.Create( NULL, "", WS_CHILD|WS_VISIBLE, CRect( 0, 0, 1, 1 ), this, IDC_CHILD_MOUSE );
  m_mouseHelp.ShowWindow(SW_SHOW);

  // create status indicator
  m_statusIndicator.Create( NULL, "", WS_CHILD|WS_VISIBLE, CRect( 0, 0, 1, 1 ), this, IDC_CHILD_STATUS ); 
  
  RepositionChildren();

  Colon::ApplicationController::Connect( this );

  return 0;
} // OnCreate() 



/**
 * Called when the window is being destroyed
 */
void ViewSelectDlgBar::OnDestroy()
{
  Colon::ApplicationController::Disconnect( this );
} // OnDestroy()


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void ViewSelectDlgBar::OnSize( UINT uType, int4 cx, int4 cy )
{
  TabStrip::OnSize( uType, cx, cy );
  RepositionChildren();

} // OnSize( UINT uType, int cx, int cy )


/**
 * Repositions the child controls ( lots of fudge factors make for great code !)
 */
void ViewSelectDlgBar::RepositionChildren()
{
  // get client area
  CRect rc; GetClientRect( &rc );

  // reposition progress bar
  const int iProgressbarWidth( 130 );
  const int iProgressbarHeight( 25 );
  int x = rc.Width() - iProgressbarWidth - 10; // 10 pixels for padding on right of progress bar
  int y = ( ( rc.Height() - iProgressbarHeight ) / 2 ) - 2;
  m_progressbar.MoveWindow( x, y, iProgressbarWidth, iProgressbarHeight );

  // reposition mouse control 
  const int4 iDesiredWidth = 450;
  const int4 iMinXPos = 530;
  x = max( iMinXPos, rc.Width() - Theme::BkgndCornerBottomRight->GetWidth() - iDesiredWidth );
  y = 0;
  int4 iWidth = rc.Width() -  x - Theme::BkgndCornerBottomRight->GetWidth();

  // reposition mouse help
  m_mouseHelp.MoveWindow( x, y, iWidth, rc.Height() );

  // reposition status indicator
  const int4 iIndicatorWidth = 150;
  x -= iIndicatorWidth;
  m_statusIndicator.MoveWindow( x, y, iIndicatorWidth, rc.Height() );
} // RepositionChildren()


/**
 * Shows/hides a window
 * 
 * @param HWND hwnd      Handle to child window.
 * @param LPARAM lParam  Non-zero to show, hide otherwise.
 * @return               To continue enumeration, return TRUE; to stop enumeration, return FALSE. 
 */
BOOL CALLBACK ViewSelectDlgBar_ShowWindowProc(HWND hwnd, LPARAM lParam)
{
  ShowWindow(hwnd, (lParam!=0) ? SW_SHOW : SW_HIDE);
  return TRUE;
} // ViewSelectDlgBar_ShowWindowProc(HWND hwnd, LPARAM lParam)


/**
 * Shows/hides all children
 *
 * @param const bool bShow   Whether to show or hide
 */
void ViewSelectDlgBar::ShowAllChildren(const bool bShow)
{
  EnumChildWindows( *this, ViewSelectDlgBar_ShowWindowProc, LPARAM(bShow==true) );
} // ShowAllChildren( const bool bShow )


/**
 * Called before an item is changed
 *
 * @param   iPrevious   index of previous item
 * @param   iNew        index of new item
 */
void ViewSelectDlgBar::OnItemChanging( int4 iPrevious, int4 iNew ) 
{
} // OnItemChanging( int4 iPrevious, int4 iNew ) 


/**
 * Called after an item has changed
 *
 * @param   iPrevious   index of previous item
 * @param   iNew        index of new item
 */
void ViewSelectDlgBar::OnItemChanged( int4 iPrevious, int4 iNew ) 
{
  if( !AfxGetMainWnd()->SendMessage(WM_APP_VIEW_SELECTION_CHANGING, iNew, 0) )
  {
    TabStrip::SetSelectedIndex( iPrevious );
  }
  else
  {
    AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, iNew, 0);
  }
} // OnItemChanged( int4 iPrevious, int4 iNew ) 


/**
 * Gets the current selection of the tab control.
 *
 * @return   Current tab control selection.
 */
int4 ViewSelectDlgBar::GetCurrentViewSelection()
{
  if( !IsWindow( GetSafeHwnd() ) )
    return -1;
  return TabStrip::GetSelectedIndex();
} // GetCurrentViewSelection()


/**
 * Sets the current tab control selection.
 *
 * @param iView   The new tab control selection.
 */
void ViewSelectDlgBar::SetCurrentViewSelection( int4 iView )
{
  if( !IsWindow( GetSafeHwnd() ) )
    return;
  TabStrip::SetSelectedIndex( iView );
} // SetCurrentViewSelection( int4 iView )


/**
 * Enables or disables the specified tab item.
 *
 * @param iItem     The index of the item.
 * @param bEnable   True to enable, false to disable.
 * @return          Indicates success/failure.
 */
bool ViewSelectDlgBar::EnableItem(const int4 iItem, const bool bEnable)
{
  TabStrip::EnableItem( iItem, bEnable );
  return true;
} // EnableItem()


/**
 * Enables/disables tab items.
 *
 * @param enableTabBitSet   Set of bits indicating enable/disable state of the item.
 * @return                  Indicates success/failure.
 */
bool ViewSelectDlgBar::EnableItems( const EnableViewsBitSet & enableTabBitSet )
{
  TabStrip::BeginInit();
  for( uint4 i( 0 ); i < TabStrip::GetItemCount(); ++i )
    TabStrip::EnableItem( i, enableTabBitSet[ i ] );
  TabStrip::EndInit();
  return true;
} // EnableItems( const EnableViewsBitSet & enableTabBitSet )


/**
 * Sets the position of the progress control (0-100)
 *
 * @param iProgress   Percentage of progress (0-100)
 */
void ViewSelectDlgBar::SetProgress( const int4 iProgress )
{
 if ( (iProgress <= 0) && (m_progressbar.IsWindowVisible()) )
  {
    m_progressbar.ShowWindow( SW_HIDE );
  }
  else if ( (iProgress > 0) && (!m_progressbar.IsWindowVisible()) )
  {
    m_progressbar.ShowWindow( SW_SHOW );
  }  
  m_progressbar.SetPos( iProgress );
} // SetProgress( const int4 iProgress )


/**
 * Strings to set in the button use key.
 *
 * @param sLeft    String for left mouse button.
 * @param sMid     String for middle mouse button.
 * @param sRight   String for right mouse button.
 * @param bCtrl    Whether CTRL key is enabled.
 * @param bShift   Whether SHIFT key is enabled.
 */
void ViewSelectDlgBar::SetButtonFunctionText(const std::string & sLeft, const std::string & sMid,
                                             const std::string & sRight, const bool bCtrl, const bool bShift)
{
  if( !IsWindow( m_hWnd ) )
    return;

  m_mouseHelp.BeginInit();
  m_mouseHelp.SetLeftButtonText( sLeft );
  m_mouseHelp.SetMiddleButtonText( sMid );
  m_mouseHelp.SetRightButtonText( sRight );
  m_mouseHelp.SetControlKeyEnabled( bCtrl );
  m_mouseHelp.SetShiftKeyEnabled( bShift );
  m_mouseHelp.EndInit();
  m_mouseHelp.Invalidate();
} // SetButtonFunctionText(...)


/**
 * Sets text for the tool tip balloon for a control
 * 
 * @param UINT uID            ID for the child control (not to be used, use ID in pTTTStruct)
 * @param NMHDR *pTTTStruct   Tool tip event notify struct
 * @param LRESULT *pResult    Success/failure
 * @return BOOL               True tells framework to set tooltip
 */
BOOL ViewSelectDlgBar::OnToolTipNeedTextHandler( UINT uID, NMHDR *pTTTStruct, LRESULT *pResult )
{
  TOOLTIPTEXT *pTTT = reinterpret_cast< TOOLTIPTEXT * >( pTTTStruct );
  UINT uIDFromCtrl = pTTTStruct->idFrom;
  if( pTTT->uFlags & TTF_IDISHWND )
  {
    // pTTTStruct->idFrom is actually the HWND of the tool, not it's ID
    uIDFromCtrl = ::GetDlgCtrlID( (HWND) uIDFromCtrl );
    if( uIDFromCtrl == 0 )
    {
      return FALSE;
    }

    static char sStr[32];
    sStr[0] = '\0';

    IToolTipProvider * pToolTipProvider = dynamic_cast< IToolTipProvider * >( GetDlgItem( uIDFromCtrl ));
    if( pToolTipProvider )
    {
      std::string sToolTip = pToolTipProvider->GetToolTip();
      if( sToolTip.empty() )
        return FALSE;

      sprintf( sStr, sToolTip.c_str() );
      
      pTTT->lpszText = sStr;
      pTTT->hinst = NULL;
      return TRUE;
    }
  }

  return FALSE;
} // OnToolTipNeedText()


/**
 * Called when the registered observable object is changed 
 *
 * @param       data          observable data
 */
void ViewSelectDlgBar::OnNotified( const Colon::ReportArgs & args )
{
  if( args.IsCompleted() )
    EnableItem( NUM_VIEW_SCREENS - 1, false );
} // OnNotified( data )


/**
 * set link state indicator display
 *
 * @param bShowLinkState
 */
void ViewSelectDlgBar::SetLinkStateDisplay( const bool & bShowLinkState )
{
	m_statusIndicator.SetLinkIconVisibility( bShowLinkState );
} // SetLinkStateDisplay()

