// $Id: PreferencesDlg.C,v 1.19.2.9 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PreferencesDlg.C
// Author: Jeffrey Meade  

/// Includes
#include "stdafx.h"
#include "Viatron.h"
#include "WinUtils.h"
#include "File.h"
#include "V3DSystemSettings.h"
#include "MainAppFrame.h"
#include "user.h"
#include "MonitorDisplay.h"
#include "PreferencesDlg.h"
#include "ApplicationSettings.h"
#include "utilities.h"
#include <regex>
#include "AppSettings.h"
#include "Theme.h"
#include "SliceChildView.h"
#include "vcUtils.h"



/// namespaces
using namespace WinUtils;
using namespace ColonViews::Dialogs;
using namespace ColonReport;
using namespace ReaderLib;


#define FILE_REVISION "$Revision: 1.19.2.9 $"

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::PreferencesDlgPage property page


/**
 * Constructor 
 *
 * @param uIDD   Resource Id for the property page
 */
PreferencesDlg::PreferencesDlgPage::PreferencesDlgPage(UINT uIDD) : CPropertyPage(uIDD)
{
  //{{AFX_DATA_INIT(PreferencesDlgPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
} /// PreferencesDlg::PreferencesDlgPage::PreferencesDlgPage(UINT uIDD)


/**
 * Destructor 
 */
PreferencesDlg::PreferencesDlgPage::~PreferencesDlgPage()
{

} // ~PreferencesDlgPage()


/**
*   Reset the item value to default after user hiting the Reset button
*/
void PreferencesDlg::GeneralPage::OnResetPage()
{ 
  m_comboLogLevel.SetCurSel( 2 );
  m_comboMonitor.SetCurSel( 0 );

} // OnResetpage


/**
 * Window message handler
 *
 * @param uMessage   Message
 * @param wParam     Message-dependent parameter
 * @param lParam     Message-dependent parameter
 * @return           Message-dependent.
 */
LRESULT PreferencesDlg::PreferencesDlgPage::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{

  //////////////////////////////
  // Check the message received
  //////////////////////////////
  switch (uMessage)
  {
    case WM_SIZE:
      OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
      return TRUE;

    case WM_CTLCOLORBTN:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_BTN));
    case WM_CTLCOLORDLG:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_DLG));
    case WM_CTLCOLOREDIT:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_EDIT));
    case WM_CTLCOLORLISTBOX:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_LISTBOX));
    case WM_CTLCOLORMSGBOX:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_MSGBOX));
    case WM_CTLCOLORSCROLLBAR:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_SCROLLBAR));
    case WM_CTLCOLORSTATIC:
      return LRESULT(OnCtlColor(CDC::FromHandle(HDC(wParam)), CWnd::FromHandle(HWND(lParam)), CTLCOLOR_STATIC));

    default:
      break;
  }

  // Call the base WindProc() handler
  return CPropertyPage::WindowProc(uMessage, wParam, lParam);

} // WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)

BEGIN_MESSAGE_MAP(PreferencesDlg::PreferencesDlgPage, CPropertyPage)
  //{{AFX_MSG_MAP(PreferencesDlgPage)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * WM_SIZE handler
 *
 * @param uType   Type of resize event
 * @param iCX     New width
 * @param iCY     New height
 */
void PreferencesDlg::PreferencesDlgPage::OnSize(UINT uType, int iCX, int iCY) 
{
  CPropertyPage::OnSize(uType, iCX, iCY);

  m_bkgdBrush.Create( this, IDB_BLUEMETAL );
} // OnSize(UINT uType, int iCX, int iCY) 


/**
 * WM_CTLCOLOR handler
 *
 * @param pDC         Device context for control being colored
 * @param pWnd        Pointer to control
 * @param uCtlColor   Type of control
 * @return            Brush to color control
 */
HBRUSH PreferencesDlg::PreferencesDlgPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, uCtlColor);

  // TODO: Return a different brush if the default is not desired
  if (uCtlColor != CTLCOLOR_EDIT)
  {
    hbr = HBRUSH(m_bkgdBrush);
    pDC->SetBkMode(TRANSPARENT);
  }

  if (pWnd->GetStyle() & WS_CHILD)
  {
    CRect ctlRect; pWnd->GetWindowRect(ctlRect);  ScreenToClient(ctlRect);
    switch (uCtlColor)
    {
      case CTLCOLOR_DLG:
      {
        pDC->SetBrushOrg(0, 0);
        break;
      }
      case CTLCOLOR_LISTBOX:
      {
        pDC->SetBrushOrg(-(ctlRect.left+2)-3, -(ctlRect.top+2)-3);
        break;
      }
      case CTLCOLOR_STATIC:
      {
        pDC->SetBrushOrg(-ctlRect.left, -ctlRect.top);
        break;
      }
      default:
      {
        break;
      }
    }
  }

  return hbr;
} // OnCtlColor()

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::NetworkPage property page

//IMPLEMENT_DYNCREATE(NetworkPage, CPropertyPage)


/**
 * Constructor 
 */
PreferencesDlg::NetworkPage::NetworkPage() : PreferencesDlgPage(PreferencesDlg::NetworkPage::IDD)
{
  //{{AFX_DATA_INIT(NetworkPage)
  //}}AFX_DATA_INIT
} // NetworkPage() 


/**
 * Destructor 
 */
PreferencesDlg::NetworkPage::~NetworkPage()
{
} // ~NetworkPage()


/**
 * Called by the framework to exchange and validate dialog data.
 *
 * @param pDX   Pointer to CDataExchange object
 */
void PreferencesDlg::NetworkPage::DoDataExchange(CDataExchange* pDX)
{
  PreferencesDlgPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(NetworkPage)
  //}}AFX_DATA_MAP
} // DoDataExchange(CDataExchange* pDX)


BEGIN_MESSAGE_MAP(PreferencesDlg::NetworkPage, PreferencesDlgPage)
  //{{AFX_MSG_MAP(NetworkPage)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::NetworkPage message handlers


/**
 * Handles dialog init
 *
 * @return   FALSE if focus set to a control, TRUE otherwise
 */
BOOL PreferencesDlg::NetworkPage::OnInitDialog()
{
  PreferencesDlgPage::OnInitDialog();
  
  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog()
  

/**
 * Called to apply changes.
 *
 * @return   TRUE
 */
BOOL PreferencesDlg::NetworkPage::OnApply() 
{
  // TODO: Add your specialized code here and/or call the base class
  
//  return PreferencesDlgPage::OnApply();
  SaveValues();
  return TRUE;
} // OnApply() 


/**
 * IDCANCEL handler
 */
void PreferencesDlg::NetworkPage::OnCancel() 
{
  // TODO: Add your specialized code here and/or call the base class
  
//  PreferencesDlgPage::OnCancel();
} // OnCancel() 


/**
 * IDOK handler
 */
void PreferencesDlg::NetworkPage::OnOK() 
{
  SaveValues();
//  PreferencesDlgPage::OnOK();
} // OnOK()


/**
 * Saves changes.
 */
void PreferencesDlg::NetworkPage::SaveValues() 
{
  UpdateData(TRUE);

//  PreferencesDlgPage::OnOK();
} // SaveValues()

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::GeneralPage property page

//IMPLEMENT_DYNCREATE(GeneralPage, PreferencesDlgPage)


/**
 * Constructor 
 */
PreferencesDlg::GeneralPage::GeneralPage() : PreferencesDlgPage(GeneralPage::IDD), m_iInitialDSNSelection(-1)
{
  //{{AFX_DATA_INIT(GeneralPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
} // GeneralPage()


/**
 * Destructor 
 */
PreferencesDlg::GeneralPage::~GeneralPage()
{
} // ~GeneralPage()


/**
 * Called by the framework to exchange and validate dialog data.
 *
 * @param pDX   Pointer to CDataExchange object
 */
void PreferencesDlg::GeneralPage::DoDataExchange(CDataExchange* pDX)
{
  PreferencesDlgPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(GeneralPage)
  DDX_Control(pDX, IDC_GENERALPAGE_LOGLEVELCOMBO, m_comboLogLevel);
  DDX_Control(pDX, IDC_GENERALPAGE_MONITORCOMBO, m_comboMonitor);
  DDX_Control(pDX, IDC_GENERALPAGE_CADDEFAULT_SLIDER, m_cadSlider);
  DDX_Control(pDX, IDC_GENERALPAGE_CADDEFAULT_DISP, m_display);
  DDX_Control(pDX, IDC_GENERALPAGE_CADDEFAULT_CHECK, m_btnCadDefEnabled);
  DDX_Control(pDX, IDC_GENERALPAGE_DETECTWIDESCREEN, m_comboWidescreen);
  //}}AFX_DATA_MAP
} // DoDataExchange(CDataExchange* pDX)


/**
 * message map
 **/
BEGIN_MESSAGE_MAP(PreferencesDlg::GeneralPage, PreferencesDlgPage)
  //{{AFX_MSG_MAP(GeneralPage)
  ON_WM_HSCROLL()
  ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDC_GENERALPAGE_CADDEFAULT_CHECK, PreferencesDlg::GeneralPage::OnCadDefaultCheckClicked)
  ON_BN_CLICKED(IDC_GENERALPAGE_CHANGEMONITOR, PreferencesDlg::GeneralPage::OnChangeMonitorClicked)
  ON_CBN_SELCHANGE(IDC_GENERALPAGE_DETECTWIDESCREEN, PreferencesDlg::GeneralPage::OnSelChangeDetectWidescreen)

  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// NOTE: Item data in m_dsnCombo signifies whether or not the connection has been tested
enum DSNConnTestResultEnum { eDSNUntested, eDSNTestFailed, eDSNTestSuceeded };


/**
 * Handles dialog init
 *
 * @return    FALSE if focus set to a control, TRUE otherwise
 */
BOOL PreferencesDlg::GeneralPage::OnInitDialog() 
{
  PreferencesDlgPage::OnInitDialog();

  // Init log level combo
  const User::UserTypeEnum eCurrUserType( User::GetCurrentUser().GetType()  );
  if(( eCurrUserType == User::UserType_Administrator) || (eCurrUserType == User::UserType_Service))
  {
    ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_LOCAL ));

    CString sStr;
    const vxLogRecord::SeverityEnum eCurrLogLevel( state.Get< vxLogRecord::SeverityEnum >( APP_SETTING_LOG_LEVEL ) );
    for (int4 iSev(0); iSev <= vxLogRecord::SL_HIGHEST; iSev++)
    {
      sStr.Format("%d", iSev);
      int4 iNewStr(-1);
      m_comboLogLevel.SetItemData(iNewStr = m_comboLogLevel.AddString(sStr), DWORD(iSev));
      if (vxLogRecord::SeverityEnum(iSev) == eCurrLogLevel)
      {
        m_comboLogLevel.SetCurSel(iNewStr);
      }
    }
  }

  m_comboLogLevel.EnableWindow(( eCurrUserType == User::UserType_Administrator) || (eCurrUserType == User::UserType_Service));

  //{{ Init monitor selections
  std::map<int4,DEVMODE> listDisplays = MonitorDisplay::EnumAttachedDisplays();
  for (std::map<int4,DEVMODE>::const_iterator it = listDisplays.begin(); it != listDisplays.end(); it++)
  {
    CString sName; sName.Format("%s (%d x %d @ %d-bpp)",
      it->second.dmFormName, it->second.dmPelsWidth, it->second.dmPelsHeight, it->second.dmBitsPerPel);

    int4 iEntry(m_comboMonitor.AddString(sName));
    m_comboMonitor.SetItemData(iEntry, it->first);
  }
  for ( int4 iItem(0), iCurrMonSetting(MonitorDisplay::GetCurrentMonitorSetting()); iItem < m_comboMonitor.GetCount(); iItem++ )
  {
    if ( m_comboMonitor.GetItemData(iItem) == iCurrMonSetting )
    {
      m_comboMonitor.SetCurSel(iItem);
      break;
    }
  }
  m_comboMonitor.EnableWindow(listDisplays.size() > 1);
  //}} Init monitor selections

  //{{ Detect monitor selection
  m_comboWidescreen.SetCurSel( ApplicationSettings::UserSettings().Get<int4>( APP_SETTING_DETECT_WIDESCREEN ));
  //}} Detect monitor selection

  // cad slider
  m_cadSlider.SetRange(1, 100);

  ApplicationSettings::ApplicationState state( ApplicationSettings::CreateDefaults( CONFIGURATION_USER ));
  int4 iValue( rdrGlobal.GetSettings()->GetIntValue( VCSettings::ViewerXML(), VCSettings::DefaultCadSliderValue() ));
  state.TryGet<int4>( "UserCadDefault", iValue );
  m_cadSlider.SetPos( iValue );

  bool bCheckValue( false );
  state.TryGet<bool>( "UserCadDefaultEnabled", bCheckValue );
  m_btnCadDefEnabled.SetCheck( bCheckValue );

  m_cadSlider.EnableWindow( bCheckValue ? TRUE : FALSE);

  return TRUE;
} // OnInitDialog()


/**
 * When checkbox for cad default value is clicked
 */
void PreferencesDlg::GeneralPage::OnCadDefaultCheckClicked()
{
  m_cadSlider.EnableWindow(m_btnCadDefEnabled.GetCheck() != 0 ? TRUE : FALSE);
} // OnCadDefaultCheckClicked()


/**
 * When monitor selection changes
 */
void PreferencesDlg::GeneralPage::OnChangeMonitorClicked()
{
  const int4 iCurrMonitor(m_comboMonitor.GetItemData(m_comboMonitor.GetCurSel()));

  ApplicationSettings::UserSettings().SetInt( APP_SETTING_VIEWER_MONITOR, iCurrMonitor );

  MainAppFrame::GetMainFrame()->GetCurrentLayout().ShowWindow(SW_HIDE);

  MonitorDisplay::SetCurrentMonitor(iCurrMonitor);
  AfxGetMainWnd()->ShowWindow(SW_HIDE);
  MainAppFrame::GetMainFrame()->ReloadCurrentLayout();
  AfxGetMainWnd()->ShowWindow(SW_SHOWMAXIMIZED);

  MainAppFrame::GetMainFrame()->GetCurrentLayout().ShowWindow(SW_RESTORE);
} // OnChangeMonitorClicked()


/**
 * When widescreen detection changes
 */
void PreferencesDlg::GeneralPage::OnSelChangeDetectWidescreen()
{
  ApplicationSettings::UserSettings().SetInt( APP_SETTING_DETECT_WIDESCREEN, m_comboWidescreen.GetCurSel() );

  // force the framework to reload the layout
  MainAppFrame::GetMainFrame()->ReloadCurrentLayout();

} // OnSelChangeDetectWidescreen()


/**
 * Called to apply changes.
 *
 * @return   TRUE
 */
BOOL PreferencesDlg::GeneralPage::OnApply()
{
  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_LOCAL ));

  // Effect change in log level
  const int4 iSelectedLogLevel(m_comboLogLevel.GetItemData(m_comboLogLevel.GetCurSel()));
  state.SetInt( APP_SETTING_LOG_LEVEL, iSelectedLogLevel );
  vxLogFactory::CreateLogger( "vxcolonviewer", state.Get( APP_SETTING_LOG_PATH ),
                              state.Get< vxLogRecord::SeverityEnum >( APP_SETTING_LOG_LEVEL ),
                              state.Get< vxLogMessageFormats >(APP_SETTING_LOG_MESSAGE_FORMAT), true );

  // Effect change in viewer monitor
  const int4 iMonitor(m_comboMonitor.GetItemData(m_comboMonitor.GetCurSel()));
  state.SetInt( APP_SETTING_VIEWER_MONITOR, iMonitor );
	/// save the change to the local app.config file
	state.Save();

  // set cad user default value
  ApplicationSettings::UserSettings().SetInt( "UserCadDefault", m_cadSlider.GetPos() );
  ApplicationSettings::UserSettings().SetBool( "UserCadDefaultEnabled", m_btnCadDefEnabled.GetCheck() );

  return TRUE;
} // OnApply()


/**
 * IDOK handler
 */
void PreferencesDlg::GeneralPage::OnOK()
{
  OnApply();
} // OnOK()


/**
* Event handler for scrolling, handles IDC_CDR_
* 
* @param uSBCode      Scroll event code
* @param uPos         Current slider position, validity depends on nSBCode
* @param pScrollBar   Pointer to control receiving the event
*/
void PreferencesDlg::GeneralPage::OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar)
{
  const int4 iCtrlID(pScrollBar->GetDlgCtrlID());

  if (iCtrlID != IDC_GENERALPAGE_CADDEFAULT_SLIDER)
  {
    CDialog::OnVScroll(uSBCode, uPos, pScrollBar);
    LogDbg("invalid scrollbar control ID argument", "ControlsDialog", "OnVScroll");
    return;
  }

  const CSliderCtrl * pSlider = (CSliderCtrl *) pScrollBar;

  float4 fPercent(0);
  switch (uSBCode)
  {
    // Scroll codes defined in <commctrl.h>
  case TB_THUMBTRACK:     // while user is moving slider

  case TB_ENDTRACK:       // called at the end of all movements
    // NOTE: Same code to get the value for all scroll events
    fPercent = float4( pSlider->GetPos() ) / float4( pSlider->GetRangeMax() - pSlider->GetRangeMin() );
    break;

  default: // don't handle any other notification codes
    break;
  }

  // fPercent is the value of the slider between 0.0 and 1.0
  // convert it to a valid slider value
  int4 iValue = Bound( 0, static_cast< int4 >( 100 * fPercent ), 100 );

  CString str; str.Format("%d", iValue);
  m_display.SetWindowText(str);

  CPoint pt;  CRect slRect, rect;

  GetCursorPos(&pt);              ScreenToClient(&pt);
  pSlider->GetWindowRect(slRect);  ScreenToClient(slRect);

  static const int4 iBorderSz(2);
  m_display.GetClientRect(rect);
  rect.OffsetRect(pt.x, slRect.top - rect.Height() - iBorderSz);
  rect.InflateRect(iBorderSz,iBorderSz);  // apparently, we need to give it room for the border?

  m_display.MoveWindow(rect);
  m_display.BringWindowToTop();
  m_display.UpdateWindow();
  m_display.ShowWindow(SW_SHOW);

  PreferencesDlgPage::OnHScroll(uSBCode, uPos, pScrollBar);
} // OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar)


/**
* WM_MOUSEMOVE handler
*
* @param uFlags   Current key flags
* @param point    Current point of mouse move
*/
void PreferencesDlg::GeneralPage::OnMouseMove(UINT uFlags, CPoint point)
{
  
  m_display.ShowWindow(SW_HIDE);
  PreferencesDlgPage::OnMouseMove(uFlags, point);

} // OnMouseMove(UINT uFlags, CPoint point)


/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::GeneralPage message handlers








/**
 * Constructor 
 */
PreferencesDlg::DefaultsPage::DefaultsPage() : PreferencesDlgPage( DefaultsPage::IDD ),
  m_bPreferencesChanged( false )
{
  //{{AFX_DATA_INIT(DefaultsPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
} // DefaultsPage()


/**
 * Destructor 
 */
PreferencesDlg::DefaultsPage::~DefaultsPage()
{
} // ~DefaultsPage()


/**
 * Called by the framework to exchange and validate dialog data.
 *
 * @param pDX   Pointer to CDataExchange object
 */
void PreferencesDlg::DefaultsPage::DoDataExchange( CDataExchange* pDX )
{
  PreferencesDlgPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(DefaultsPage)
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_LAYOUT, m_layoutComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_FLIGHTSPEED, m_flightSpeedSlider );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_FLIGHTSPEED_LABEL, m_flightSpeedLabel );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_MOUSE_COORDS, m_mouseCoordComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_QUICK_ANNOTATION, m_annotationEdit );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_SCROLLBAR_ORIENTATION, m_scrollbarComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_TOOLBAR_POSITION, m_toolbarPositionComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_DISPLAY_EC_CAD, m_ecFindingsComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_DISPLAY_CLEANSED, m_cleansedComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_2D_MEASUREMENT_PROFILE, m_measurementComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_COLOR_SCHEME, m_colorSchemeComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_PSUEDO_COLOR, m_pseudoColorComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_VOLUME_MEASURE, m_volumeMeasureComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_LINK_DATASETS, m_linkDatasetsComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_DISPLAY_CENTERLINE, m_centerlineComboBox );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_LAYOUT_APPLY_NOW, m_applyButton );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_FOV, m_fovSlider );
  DDX_Control( pDX, IDC_PREFS_DEFAULTS_FOV_DISPLAY, m_fovLabel );
  //}}AFX_DATA_MAP
} // DoDataExchange( CDataExchange* pDX )


/**
 * message map
 **/
BEGIN_MESSAGE_MAP( PreferencesDlg::DefaultsPage, PreferencesDlgPage )
  //{{AFX_MSG_MAP(DefaultsPage)
  ON_WM_HSCROLL()
  ON_BN_CLICKED( IDC_PREFS_DEFAULTS_LAYOUT_APPLY_NOW,           PreferencesDlg::DefaultsPage::OnApplyClickedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_LAYOUT,                 PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_FLIGHTSPEED,            PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_MOUSE_COORDS,           PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_QUICK_ANNOTATION,       PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_SCROLLBAR_ORIENTATION,  PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_TOOLBAR_POSITION,       PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_DISPLAY_EC_CAD,         PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_DISPLAY_CLEANSED,       PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_2D_MEASUREMENT_PROFILE, PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_COLOR_SCHEME,           PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_PSUEDO_COLOR,           PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_VOLUME_MEASURE,         PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_LINK_DATASETS,          PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_DISPLAY_CENTERLINE,     PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  ON_CBN_SELCHANGE ( IDC_PREFS_DEFAULTS_FOV,                    PreferencesDlg::DefaultsPage::OnSelectionChangedHandler )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Handles dialog init
 *

 * @return    FALSE if focus set to a control, TRUE otherwise
 */
BOOL PreferencesDlg::DefaultsPage::OnInitDialog() 
{
  PreferencesDlgPage::OnInitDialog();

  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_USER ));
  
  SetupComboPreference( m_layoutComboBox,         IDC_PREFS_DEFAULTS_LAYOUT_LABEL,                  IDS_PREFS_DEFAULTS_LAYOUT,                  IDS_PREFS_DEFAULTS_LAYOUT_OPTIONS,                state.Get<int4>( APP_SETTING_DETECT_WIDESCREEN ));
  SetupComboPreference( m_mouseCoordComboBox,     IDC_PREFS_DEFAULTS_MOUSE_COORDS_LABEL,            IDS_PREFS_DEFAULTS_MOUSE_COORDS,            IDS_PREFS_DEFAULTS_MOUSE_COORDS_OPTIONS,          state.Get< int4 >( APP_SETTING_DISPLAY_COORD ));
  SetupComboPreference( m_scrollbarComboBox,      IDC_PREFS_DEFAULTS_SCROLLBAR_ORIENTATION_LABEL,   IDS_PREFS_DEFAULTS_SCROLLBAR_ORIENTATION,   IDS_PREFS_DEFAULTS_SCROLLBAR_ORIENTATION_OPTIONS, state.Get< int4 >( APP_SETTING_SCROLLBAR_LOCATION ));
  SetupComboPreference( m_toolbarPositionComboBox,IDC_PREFS_DEFAULTS_TOOLBAR_POSITION_LABEL,        IDS_PREFS_DEFAULTS_TOOLBAR_POSITION,        IDS_PREFS_DEFAULTS_TOOLBAR_POSITION_OPTIONS,      !state.Get< bool >( APP_SETTING_TOOLBAR_POSITION ));
	SetupComboPreference( m_ecFindingsComboBox,     IDC_PREFS_DEFAULTS_DISPLAY_EC_CAD_LABEL,          IDS_PREFS_DEFAULTS_DISPLAY_EC_CAD,          IDS_PREFS_DEFAULTS_YES_NO_OPTIONS,                state.Get< bool >( APP_SETTING_SHOW_EXTRA_COLONIC_FINDINGS ));
  SetupComboPreference( m_cleansedComboBox,       IDC_PREFS_DEFAULTS_DISPLAY_CLEANSED_LABEL,        IDS_PREFS_DEFAULTS_DISPLAY_CLEANSED,        IDS_PREFS_DEFAULTS_DISPLAY_CLEANSED_OPTIONS,      !state.Get< bool >( APP_SETTING_SHOW_UNCLEANSED ));
  SetupComboPreference( m_measurementComboBox,    IDC_PREFS_DEFAULTS_2D_MEASUREMENT_PROFILE_LABEL,  IDS_PREFS_DEFAULTS_2D_MEASUREMENT_PROFILE,  IDS_PREFS_DEFAULTS_ON_OFF_OPTIONS,                state.Get< bool >( APP_SETTING_SHOW_2D_MEASUREMENT_PROFILE ));
  SetupComboPreference( m_colorSchemeComboBox,    IDC_PREFS_DEFAULTS_COLOR_SCHEME_LABEL,            IDS_PREFS_DEFAULTS_COLOR_SCHEME,            IDS_PREFS_DEFAULTS_COLOR_SCHEME_OPTIONS,          state.Get< int4 >( APP_SETTING_COLOR_SCHEME ));
  SetupComboPreference( m_pseudoColorComboBox,    IDC_PREFS_DEFAULTS_PSUEDO_COLOR_LABEL,            IDS_PREFS_DEFAULTS_PSUEDO_COLOR,            IDS_PREFS_DEFAULTS_ON_OFF_OPTIONS,                state.Get< SliceChildView::DbgShowEnum >( APP_SETTING_PSUEDO_COLOR ) == SliceChildView::SHOW_BIOPSY );
  SetupComboPreference( m_volumeMeasureComboBox,  IDC_PREFS_DEFAULTS_VOLUME_MEASURE_LABEL,          IDS_PREFS_DEFAULTS_VOLUME_MEASURE,          IDS_PREFS_DEFAULTS_VOLUME_MEASURE_OPTIONS,        state.Get< bool >( APP_SETTING_VOLUME_MEASURE_AIR_ONLY ));
  SetupComboPreference( m_linkDatasetsComboBox,   IDC_PREFS_DEFAULTS_LINK_DATASETS_LABEL,           IDS_PREFS_DEFAULTS_LINK_DATASETS,           IDS_PREFS_DEFAULTS_YES_NO_OPTIONS,                state.Get< bool >( APP_SETTING_LINK_DATASETS ));
  SetupComboPreference( m_centerlineComboBox,     IDC_PREFS_DEFAULTS_DISPLAY_CENTERLINE_LABEL,      IDS_PREFS_DEFAULTS_DISPLAY_CENTERLINE,      IDS_PREFS_DEFAULTS_ON_OFF_OPTIONS,                state.Get< bool >( APP_SETTING_SHOW_CENTERLINE ));

  // initialize flight speed slider
  SetStaticText( IDC_PREFS_DEFAULTS_FLIGHT_SPEED_LABEL, LoadResourceString( IDS_PREFS_DEFAULTS_FLIGHT_SPEED ));
  m_flightSpeedSlider.SetRange( 0, 200 );
  const int4 iFlightSpeed( state.Get< int4 >( APP_SETTING_FLIGHT_SPEED ));
  m_flightSpeedSlider.SetPos( iFlightSpeed );
  m_flightSpeedLabel.SetWindowText( ToAscii( iFlightSpeed ).c_str() );

  // initialize fov slider
  SetStaticText( IDC_PREFS_DEFAULTS_FOV_LABEL, LoadResourceString( IDS_PREFS_DEFAULTS_FOV ));
  m_fovSlider.SetRange( 90, 145 );
  const int4 iFOV( state.Get< int4 >( APP_SETTING_FOV ));
  m_fovSlider.SetPos( iFOV );
  m_fovLabel.SetWindowText( ToAscii( iFOV ).c_str() );

  // intialize quick drop annotation
  SetStaticText( IDC_PREFS_DEFAULTS_QUICK_ANNOTATION_LABEL, LoadResourceString( IDS_PREFS_DEFAULTS_QUICK_ANNOTATION ));
  m_annotationEdit.SetWindowText( state.Get( APP_SETTING_QUICK_ANNOTATION_NAME ).c_str() );
  m_annotationEdit.SetLimitText( 32 );

  SetStaticText( IDC_PREFS_DEFAULTS_GENERAL_LABEL, LoadResourceString( IDS_PREFS_DEFAULTS_GENERAL_LABEL ));
  SetStaticText( IDC_PREFS_DEFAULTS_DISPLAY_LABEL, LoadResourceString( IDS_PREFS_DEFAULTS_DISPLAY_LABEL ));

  m_applyButton.SetWindowText( LoadResourceString( IDS_PREFS_DEFAULTS_APPLY_NOW ));

  SetStaticText( IDC_PREFS_DEFAULTS_WARNING, LoadResourceString( IDS_PREFS_DEFAULTS_WARNING ));

  return TRUE;
} // OnInitDialog()


/**
 * Handler for the reset click handler
 */
void PreferencesDlg::DefaultsPage::OnResetPage()
{
    ApplicationSettings::ApplicationState state( ApplicationSettings::CreateDefaults( CONFIGURATION_USER ));
  
    m_layoutComboBox.SetCurSel( state.Get<int4>( APP_SETTING_DETECT_WIDESCREEN ));                                   
    m_mouseCoordComboBox.SetCurSel( state.Get< int4 >( APP_SETTING_DISPLAY_COORD ));                                 
    m_scrollbarComboBox.SetCurSel( state.Get< int4 >( APP_SETTING_SCROLLBAR_LOCATION ));  
		m_toolbarPositionComboBox.SetCurSel( state.Get< bool >( APP_SETTING_TOOLBAR_POSITION )); 
    m_ecFindingsComboBox.SetCurSel( !state.Get< bool >( APP_SETTING_SHOW_EXTRA_COLONIC_FINDINGS ));                  
    m_cleansedComboBox.SetCurSel( state.Get< bool >( APP_SETTING_SHOW_UNCLEANSED ));                                 
    m_measurementComboBox.SetCurSel( !state.Get< bool >( APP_SETTING_SHOW_2D_MEASUREMENT_PROFILE ));                 
    m_colorSchemeComboBox.SetCurSel( state.Get< int4 >( APP_SETTING_COLOR_SCHEME ));                                 
    
    int4 iPsuedo = state.Get< SliceChildView::DbgShowEnum >( APP_SETTING_PSUEDO_COLOR ) == SliceChildView::SHOW_BIOPSY ? 0 : 1;
    m_pseudoColorComboBox.SetCurSel( iPsuedo );                                
    m_volumeMeasureComboBox.SetCurSel( !state.Get< bool >( APP_SETTING_VOLUME_MEASURE_AIR_ONLY ));                   
    m_linkDatasetsComboBox.SetCurSel( !state.Get< bool >( APP_SETTING_LINK_DATASETS ));                              
    m_centerlineComboBox.SetCurSel( !state.Get< bool >( APP_SETTING_SHOW_CENTERLINE ));                              

    const int4 iFlightSpeed( state.Get< int4 >( APP_SETTING_FLIGHT_SPEED ));
    m_flightSpeedSlider.SetPos( iFlightSpeed );
    m_flightSpeedLabel.SetWindowText( ToAscii( iFlightSpeed ).c_str() );

    const int4 iFOV( state.Get< int4 >( APP_SETTING_FOV ));
    m_fovSlider.SetPos( iFOV );
    m_fovLabel.SetWindowText( ToAscii( iFOV ).c_str() );

    m_annotationEdit.SetWindowText( state.Get( APP_SETTING_QUICK_ANNOTATION_NAME ).c_str() );

		m_bPreferencesChanged = true;

		/// for Local page
		//m_comboLogLevel.SetCurSel( 2 );

		/// for Site page
} // OnResetClickedHandler()


/**
 * Handler for the apply click handler
 */
void PreferencesDlg::DefaultsPage::OnApplyClickedHandler()
{
  ApplicationSettings::UserSettings().SetInt( APP_SETTING_DETECT_WIDESCREEN, m_layoutComboBox.GetCurSel() );
  MainAppFrame::GetMainFrame()->ReloadCurrentLayout();
} // OnApplyClickedHandler()


/**
 * Handler for the selection changed
 */
void PreferencesDlg::DefaultsPage::OnSelectionChangedHandler()
{
  m_bPreferencesChanged = true;
  SetModified();
} // OnApplyClickedHandler()


/**
 * Called to apply changes.
 *
 * @return   TRUE
 */
BOOL PreferencesDlg::DefaultsPage::OnApply()
{
  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_USER ));

  CString text;
  m_annotationEdit.GetWindowText( text );
  std::string annotation = std::string( CT2CA( text ));
                              
  if( m_bPreferencesChanged )
  {  
    state.SetInt( APP_SETTING_DETECT_WIDESCREEN,            m_layoutComboBox.GetCurSel() );
    state.SetInt( APP_SETTING_DISPLAY_COORD,                m_mouseCoordComboBox.GetCurSel() );
    state.SetInt( APP_SETTING_SCROLLBAR_LOCATION,           m_scrollbarComboBox.GetCurSel() );
    state.SetBool( APP_SETTING_TOOLBAR_POSITION,            static_cast< bool >(  m_toolbarPositionComboBox.GetCurSel() ) );   
    state.SetBool( APP_SETTING_SHOW_EXTRA_COLONIC_FINDINGS, !static_cast< bool >( m_ecFindingsComboBox.GetCurSel() ));
    state.SetBool( APP_SETTING_SHOW_UNCLEANSED,             static_cast< bool >( m_cleansedComboBox.GetCurSel() ));
    state.SetBool( APP_SETTING_SHOW_2D_MEASUREMENT_PROFILE, !static_cast< bool >( m_measurementComboBox.GetCurSel() ));
    state.SetInt( APP_SETTING_COLOR_SCHEME,                 m_colorSchemeComboBox.GetCurSel() );

    int4 iPsuedo = m_pseudoColorComboBox.GetCurSel() == 0 ? SliceChildView::SHOW_BIOPSY : SliceChildView::SHOW_NONE;
    state.SetInt( APP_SETTING_PSUEDO_COLOR,                 iPsuedo );
    state.SetBool( APP_SETTING_VOLUME_MEASURE_AIR_ONLY,     !static_cast< bool >( m_volumeMeasureComboBox.GetCurSel() ));
    state.SetBool( APP_SETTING_LINK_DATASETS,               !static_cast< bool >( m_linkDatasetsComboBox.GetCurSel() ));
    state.SetBool( APP_SETTING_SHOW_CENTERLINE,             !static_cast< bool >( m_centerlineComboBox.GetCurSel() ));
    state.SetInt( APP_SETTING_FLIGHT_SPEED,                 m_flightSpeedSlider.GetPos() );
    state.SetInt( APP_SETTING_FOV,                          m_fovSlider.GetPos() );
    state.Set( APP_SETTING_QUICK_ANNOTATION_NAME,           annotation );

    state.Save();

    SetModified( FALSE );
  }
  return TRUE;
} // OnApply()


/**
 * IDOK handler
 */
void PreferencesDlg::DefaultsPage::OnOK()
{
  OnApply();
} // OnOK()


/**
 * IDCANCEL handler
 */
void PreferencesDlg::DefaultsPage::OnCancel()
{
  PreferencesDlgPage::OnCancel();
} // OnOK()


/**
* Event handler for scrolling, handles IDC_CDR_
* 
* @param uSBCode      Scroll event code
* @param uPos         Current slider position, validity depends on nSBCode
* @param pScrollBar   Pointer to control receiving the event
*/
void PreferencesDlg::DefaultsPage::OnHScroll( UINT uSBCode, UINT uPos, CScrollBar* pScrollBar )
{
  const int4 iCtrlID(pScrollBar->GetDlgCtrlID());

  if( iCtrlID == IDC_PREFS_DEFAULTS_FLIGHTSPEED )
    m_flightSpeedLabel.SetWindowText( ToAscii( m_flightSpeedSlider.GetPos() ).c_str() );

  else if( iCtrlID == IDC_PREFS_DEFAULTS_FOV )
    m_fovLabel.SetWindowText( ToAscii( m_fovSlider.GetPos() ).c_str() );

  PreferencesDlgPage::OnHScroll( uSBCode, uPos, pScrollBar );
} // OnHScroll( uSBCode, uPos, pScrollBar )


/**
 * Initializes the combo box and associated label
 *
 * @param         combobox            CComboBox
 * @param         iLabelControl       label id
 * @param         iLabelText          string table id for label text
 * @param         iOptions            string table id for item text
 * @param         bSetting            true or false
 */
void PreferencesDlg::DefaultsPage::SetupComboPreference( CComboBox & comboBox, UINT iLabelControl, UINT iLabelText, UINT iOptions, bool bSetting )
{
  SetupComboPreference( comboBox, iLabelControl, iLabelText, iOptions, static_cast< int4 >(( bSetting ) ? 0 : 1 ));
} // SetupComboPreference( comboBox, iLabelControl, iLabelText, iOptions, bSetting )


/**
 * Initializes the combo box and associated label
 *
 * @param         combobox            CComboBox
 * @param         iLabelControl       label id
 * @param         iLabelText          string table id for label text
 * @param         iOptions            string table id for item text
 * @param         iSetting            selected index
 */
void PreferencesDlg::DefaultsPage::SetupComboPreference( CComboBox & comboBox, UINT iLabelControl, UINT iLabelText, UINT iOptions, int4 iSetting )
{
  SetStaticText( iLabelControl, LoadResourceString( iLabelText ));

  Utilities::RegularExpression::Split( std::string( LoadResourceString( iOptions )), "\\|", [&comboBox]( const std::string & sToken ) { comboBox.AddString( sToken.c_str() ); });
  comboBox.SetCurSel( iSetting );
} // SetupComboPreference( comboBox, iLabelControl, iLabelText, iOptions, iSetting )


/**
 * Resizes and sets the text of a static label
 *
 * @param         iStaticID               CStatic id
 * @param         sText                   display text
 */
void PreferencesDlg::DefaultsPage::SetStaticText( UINT iStaticID, CString sText )
{
  CWnd * pItem = GetDlgItem( iStaticID );
  CWnd * pParent = pItem->GetParent();

  pItem->SetWindowText( sText );

  if( pParent && ::IsWindow( pParent->GetSafeHwnd() ))
  {
    CDC * pDC = pParent->GetDC();
    SCOPE_EXIT 
    { 
      pParent->ReleaseDC( pDC ); 
    };

    //Gets the Size of the text
    CSize size = pDC->GetTextExtent( sText ); 

    pItem->SetWindowPos( NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER|SWP_NOMOVE );     
  } 
} // SetStaticText( iStaticID, sText )



/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::ReportPage property page

//IMPLEMENT_DYNCREATE(ReportPage, PreferencesDlgPage)


/**
 * Constructor
 */
PreferencesDlg::ReportPage::ReportPage() : PreferencesDlgPage(PreferencesDlg::ReportPage::IDD)
{
  //{{AFX_DATA_INIT(ReportPage)
  //}}AFX_DATA_INIT
} // ReportPage()


/**
 * Destructor
 */
PreferencesDlg::ReportPage::~ReportPage()
{
} // ~ReportPage()
  

/**
*   Reset the item value to default after user hiting the Reset button
*/
void PreferencesDlg::ReportPage::OnResetPage()
{  
	m_topMarginSpin.SetValue( 1.0F );
	m_bottomMarginSpin.SetValue( 1.0F );
  
	CheckRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED, IDC_REPORTPAGE_HEADER_NONE);
	OnRbHeaderNone();

} // OnResetpage


/**
 * Called by the framework to exchange and validate dialog data.
 *
 * @param pDX   Pointer to CDataExchange object
 */
void PreferencesDlg::ReportPage::DoDataExchange(CDataExchange* pDX)
{
  PreferencesDlgPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ReportPage)
  DDX_Control(pDX, IDC_REPORTPAGE_TRANSFER, m_transferBtn);
  //}}AFX_DATA_MAP
} // DoDataExchange(CDataExchange* pDX)


BEGIN_MESSAGE_MAP(PreferencesDlg::ReportPage, PreferencesDlgPage)
  //{{AFX_MSG_MAP(ReportPage)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_REPORTPAGE_HEADER_NONE, PreferencesDlg::ReportPage::OnRbHeaderNone)
  ON_BN_CLICKED(IDC_REPORTPAGE_HEADER_DEFINED, PreferencesDlg::ReportPage::OnRbHeaderDefined)
  ON_BN_CLICKED(IDC_REPORTPAGE_HEADER_BITMAP, PreferencesDlg::ReportPage::OnRbHeaderBitmap)
  ON_BN_CLICKED(IDC_REPORTPAGE_TRANSFER, PreferencesDlg::ReportPage::OnTransfer)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg::ReportPage message handlers


/**
 * Handles dialog init
 *
 * @return   FALSE if focus set to a control, TRUE otherwise
 */
BOOL PreferencesDlg::ReportPage::OnInitDialog() 
{
  PreferencesDlgPage::OnInitDialog();

  VERIFY(m_font.CreateFont(
  20,
  0,
  0,
  0, 
  FW_NORMAL,
  FALSE,
  FALSE,
  0,
  ANSI_CHARSET,
  OUT_DEFAULT_PRECIS,
  CLIP_DEFAULT_PRECIS,
  DEFAULT_QUALITY,
  DEFAULT_PITCH | FF_SWISS,
  "Arial"));

  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL )); 

  float4 fTopMargin(0.0);   
  state.TryGet<float4>( APP_SETTING_REPORT_TOP_MARGIN, fTopMargin );
  CreateSpinEdit(IDC_REPORTPAGE_TOP_MARGIN, m_topMarginSpin, fTopMargin);

  float4 fBottomMargin(0.0);   
  state.TryGet<float4>( APP_SETTING_REPORT_BOTTOM_MARGIN, fBottomMargin );
  CreateSpinEdit(IDC_REPORTPAGE_BOTTOM_MARGIN, m_bottomMarginSpin, fBottomMargin);
  
  const V3DSystemSettings::ReportHeaderEnum eHeader( state.Get< V3DSystemSettings::ReportHeaderEnum>( APP_SETTING_REPORT_HEADER_SELECTION ));

  std::string sLine; 
  state.TryGet( APP_SETTING_REPORT_HEADER_TEXT, sLine );
  SetDlgItemText(IDC_REPORTPAGE_HEADERTEXT, sLine.c_str());

  switch (eHeader)
  {
    case V3DSystemSettings::eHEADER_BITMAP:
    {
      CheckRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED, IDC_REPORTPAGE_HEADER_BITMAP);
      OnRbHeaderBitmap();
      break;
    }
    case V3DSystemSettings::eHEADER_USER:
    {
      CheckRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED, IDC_REPORTPAGE_HEADER_DEFINED);
      OnRbHeaderDefined();
      break;
    }
    default:
    {
      CheckRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED, IDC_REPORTPAGE_HEADER_NONE);
      OnRbHeaderNone();
      break;
    }
  }

  SetButtonIcon(m_transferBtn, IDB_BUTTON_LARGE);
  
  return TRUE;
} // OnInitDialog()
  

/**
 * Called to apply changes.
 *
 * @return   TRUE
 */
BOOL PreferencesDlg::ReportPage::OnApply() 
{
  SaveValues();
  
  return TRUE;
} // OnApply()


/**
 * IDOK handler
 */
void PreferencesDlg::ReportPage::OnOK() 
{
  SaveValues();
} // OnOK() 


/**
 * Saves changes.
 */
void PreferencesDlg::ReportPage::SaveValues()
{
  int4 iRet = GetCheckedRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED);
  
  CString sHeaderText;
  GetDlgItemText(IDC_REPORTPAGE_HEADERTEXT, sHeaderText);

  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL )); 
  switch (GetCheckedRadioButton(IDC_REPORTPAGE_HEADER_NONE, IDC_REPORTPAGE_HEADER_DEFINED))
  {
    case IDC_REPORTPAGE_HEADER_DEFINED:
    {
      state.SetInt( APP_SETTING_REPORT_HEADER_SELECTION, int4( V3DSystemSettings::eHEADER_USER ));
      break;
    }
    case IDC_REPORTPAGE_HEADER_BITMAP:
    {
      state.SetInt( APP_SETTING_REPORT_HEADER_SELECTION, int4( V3DSystemSettings::eHEADER_BITMAP ));
      break;
    }
    default:
    {
      state.SetInt( APP_SETTING_REPORT_HEADER_SELECTION, int4( V3DSystemSettings::eHEADER_NONE ));
      break;
    }
  }

  state.Set( APP_SETTING_REPORT_HEADER_TEXT, std::string( CT2CA( sHeaderText )));

  state.SetInt( APP_SETTING_REPORT_NUMBERING, int4( V3DSystemSettings::eNUMBERING_BOTTOM ));
  state.SetInt( APP_SETTING_REPORT_TOP_MARGIN, m_topMarginSpin.GetValue() );
  state.SetInt( APP_SETTING_REPORT_BOTTOM_MARGIN, m_bottomMarginSpin.GetValue() );
  state.Save();
} // SaveValues()



/**
 * Creates spin edit control
 *
 * @param uID        ID of control
 * @param wndSpin    On return, newly created spin edit
 * @param fInitial   Initial value
 */
void PreferencesDlg::ReportPage::CreateSpinEdit(const UINT uID, ReportSpinEdit & wndSpin, const float fInitial)
{
  CWnd* pWnd = GetDlgItem(uID);
  ASSERT(pWnd);
  
  CRect rcSpinEdit;
  pWnd->GetWindowRect(rcSpinEdit);
  ScreenToClient(rcSpinEdit);
  
  // pass the placeholders enable/disable state to the spin control
  const BOOL bEnable( pWnd->IsWindowEnabled() );
  pWnd->DestroyWindow();
  
  wndSpin.Create(NULL, "", WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPSIBLINGS|WS_BORDER, rcSpinEdit, this, uID);
  wndSpin.SetFont(&m_font);
  wndSpin.SetDigits(2);
  wndSpin.SetRange(0.5f, 2.0f, false);
  wndSpin.SetStepSize(0.05f);
  wndSpin.SetValue(fInitial);
  wndSpin.SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
  wndSpin.SetColor(RGB(255,255,255));
  wndSpin.EnableWindow( bEnable );
} // CreateSpinEdit(const UINT uID, ReportSpinEdit & wndSpin, const float fInitial)



/**
 * Called when No header option selected
 */
void PreferencesDlg::ReportPage::OnRbHeaderNone()
{
  OnReportHeaderSelectionChanged(IDC_REPORTPAGE_HEADER_NONE);
} // OnRbHeaderNone()


/**
 * called when this option is selected
 */
void PreferencesDlg::ReportPage::OnRbHeaderDefined() 
{
  OnReportHeaderSelectionChanged(IDC_REPORTPAGE_HEADER_DEFINED);
} // OnRbHeaderDefined() 


/**
 * Called when bitmap header option clicked
 */
void PreferencesDlg::ReportPage::OnRbHeaderBitmap() 
{
  OnReportHeaderSelectionChanged(IDC_REPORTPAGE_HEADER_BITMAP);
} // OnRbHeaderBitmap() 


/**
 * Enables/disables controls based on specified report header selection
 *
 * @param iRadioID   Radio button ID of report header selection
 */
void PreferencesDlg::ReportPage::OnReportHeaderSelectionChanged(const int4 iRadioID)
{
  GetDlgItem(IDC_REPORTPAGE_HEADERTEXT)->EnableWindow(iRadioID == IDC_REPORTPAGE_HEADER_DEFINED);
} // OnReportHeaderSelectionChanged(const int4 iRadioID)


/**
 * Called when transfer report header button clicked
 */
void PreferencesDlg::ReportPage::OnTransfer() 
{
  // Make parent directories for the destination
  // Prompt on error
  const std::string reportHeaderFile = V3DSystemSettings::GetReportHeaderFile();
  if (!vx::File(vx::File( reportHeaderFile ).GetParent()).Mkdirs())
  {
    AfxMessageBox(IDS_CONTACT_SERVICE);
    return;
  }

  //get the source file.
  CString  sourceFile="";
  char szFilters[] = { "PNG Files (*.png)|*.png||"  };
  CFileDialog fileDlg (TRUE, "", "",  OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_ENABLESIZING , szFilters, this);
  if (fileDlg.DoModal() == IDOK)
    sourceFile = fileDlg.GetPathName();
  else
    return;

  // Copy the source to the destination
  if ( ! (CopyFile( sourceFile, reportHeaderFile.c_str() , FALSE) == TRUE) )
  {
    CString sErrorPrompt  =   LoadResourceString(IDS_E_HEADER) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    AfxMessageBox(sErrorPrompt, MB_ICONSTOP|MB_OK );
    return;
  }
  AfxMessageBox(IDS_IMAGE_TRANSFER_SUCCESSFUL, MB_ICONINFORMATION|MB_OK);
} // OnTransfer() 



/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg property sheet

IMPLEMENT_DYNAMIC(PreferencesDlg, PropertySheetDlg)


/**
 * Constructor 
 *
 * @param uIDCaption    Resource ID of dialog caption
 * @param pParentWnd    Parent window
 * @param uSelectPage   Initially selected page
 */
PreferencesDlg::PreferencesDlg(UINT uIDCaption, CWnd* pParentWnd, UINT uSelectPage)
  :PropertySheetDlg(uIDCaption, pParentWnd, uSelectPage)
{
  SetupDialog();
} // PreferencesDlg(UINT uIDCaption, CWnd* pParentWnd, UINT uSelectPage)


/**
 * Constructor 
 *
 * @param pszCaption    Dialog caption
 * @param pParentWnd    Parent window
 * @param uSelectPage   Initially selected page
 */
PreferencesDlg::PreferencesDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT uSelectPage)
  :PropertySheetDlg(pszCaption, pParentWnd, uSelectPage)
{
  SetupDialog();
} // PreferencesDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT uSelectPage)


/**
 * Destructor 
 */
PreferencesDlg::~PreferencesDlg()
{
} // ~PreferencesDlg()


/**
 * Initializes the dialog
 */
void PreferencesDlg::SetupDialog()
{
// Enable tabs based on current user rights
// USRTYPE_USER, USRTYPE_ADMIN, USRTYPE_SERVICE

  m_defaultsPage.m_psp.dwFlags &= ~PSP_HASHELP;
  AddPage( &m_defaultsPage );

  const User::UserTypeEnum eUserType( User::GetCurrentUser().GetType() );

  ////////////////////////////////////////////////////////////////////////////////////////////
  // Add the general and report preferences pages if the user is an administrator or service
  /////////////////////////////////////////////////////////////////////////////////////////////
  if ((eUserType == User::UserType_Administrator) || (eUserType == User::UserType_Service))
  {
    m_generalPage.m_psp.dwFlags &= ~PSP_HASHELP;
    AddPage(&m_generalPage);
  
    m_reportPage.m_psp.dwFlags &= ~PSP_HASHELP;
    AddPage(&m_reportPage);
  } // END ... If the user is an administrator or service type

} // SetupDialog()

/**
 * message map
 */
BEGIN_MESSAGE_MAP(PreferencesDlg, PropertySheetDlg)
  //{{AFX_MSG_MAP(PreferencesDlg)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_CTLCOLOR()
  ON_BN_CLICKED( IDC_PREFS_DEFAULTS_RESET, PreferencesDlg::OnResetClickedHandler )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreferencesDlg message handlers


/**
 * Handles dialog init
 *
 * @return   FALSE if focus set to a control, TRUE otherwise
 */
BOOL PreferencesDlg::OnInitDialog() 
{
  BOOL bResult = PropertySheetDlg::OnInitDialog();

	// Get a handle to the Ok button
  CButton * pOk = reinterpret_cast< CButton * >( GetDlgItem( IDOK ));
	
  // Get the location and the dimensions of the Ok button
	CRect rectOk;
  pOk->GetWindowRect( &rectOk );
	ScreenToClient( &rectOk );

  CString sText( LoadResourceString( IDS_PREFS_DEFAULTS_RESET_BUTTON ));
  m_resetButton.Create( sText, WS_CHILD | WS_VISIBLE, CRect( 30, rectOk.top, rectOk.Width(), rectOk.top + rectOk.Height() ), this, IDC_PREFS_DEFAULTS_RESET );

  return bResult;
} // OnInitDialog() 


/**
 * reset click handler
 */
void PreferencesDlg::OnResetClickedHandler()
{
  if( AfxMessageBox( LoadResourceString( IDS_PREFS_DEFAULTS_RESET_QUESTION ), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES )
  {
		if( m_networkPage.IsWindowVisible() )
		  m_networkPage.OnResetPage();
		if( m_generalPage.IsWindowVisible() )
		  m_generalPage.OnResetPage();
		if( m_reportPage.IsWindowVisible() )
		  m_reportPage.OnResetPage();
		m_defaultsPage.OnResetPage();
	} // if
} // OnResetClickedHandler()


/**
 * WM_SIZE handler
 *
 * @param uType   Type of resize event.
 * @param iCX     New width.
 * @param iCY     New height.
 */
void PreferencesDlg::OnSize(UINT uType, int iCX, int iCY) 
{
  PropertySheetDlg::OnSize(uType, iCX, iCY);
  
  m_bkgdBrush.Create( this, IDB_BLUEMETAL );
} // OnSize(UINT uType, int iCX, int iCY) 


/**
 * WM_CTLCOLOR handler
 *
 * @param pDC         Device context for control being colored
 * @param pWnd        Pointer to control
 * @param uCtlColor   Type of control
 * @return            Brush to color control
 */
HBRUSH PreferencesDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 
{
  HBRUSH hbr = PropertySheetDlg::OnCtlColor(pDC, pWnd, uCtlColor);
  
  // TODO: Change any attributes of the DC here
  
  // TODO: Return a different brush if the default is not desired
  hbr = HBRUSH(m_bkgdBrush);
  return hbr;
} // OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 

