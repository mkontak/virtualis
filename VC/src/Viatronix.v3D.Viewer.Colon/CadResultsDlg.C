// $Id: CadResultsDlg.C,v 1.48.2.11 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


/**
 * Dialog for displaying CAD results
 */


// includes
#include "stdafx.h"
//#include "ReaderGlobal.h"
//#include "V1KMessages.h"
//#include "GLChildView.h"
//#include "ViatronStudy.h"
//#include "vxCadFinding.h"
//#include "VCSettings.h"
//#include "ColonSession.h"

#include "ViatronViewWnd.h"
#include "GdiplusUtilities.h"
#include "RenderUtil.h"

#include "CadResultsDlg.h"
#include "CadUtils.h"


// namespaces
using namespace ReaderLib;
using namespace ColonViews::Dialogs;
using namespace ColonViews::ChildViews;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace std;
using namespace vxCAD;

// defines
IMPLEMENT_DYNAMIC(CadResultsDlg, ViatronDialog)
#define FILE_REVISION "$Revision: 1.48.2.11 $"


/**
 * Constructor
 *
 * @param pParent   Parent Window
 */
CadResultsDlg::CadResultsDlg( CWnd* pParent /*=NULL*/, const uint4 uBkgdID, const ReaderLib::DatasetOrientations::TypeEnum eDataset )
  : ViatronDialog(CadResultsDlg::IDD, pParent, uBkgdID), m_eIntendedDataset(eDataset), m_pCurrDatasetList( nullptr )
{
  m_pVendorImage = NULL;
} // constructor


/**
 * Destructor
 */
CadResultsDlg::~CadResultsDlg()
{
  if( m_pCurrDatasetList )
    m_pCurrDatasetList->m_cadFindings.Disconnect( this );

  if ( m_pVendorImage )
    delete m_pVendorImage;
} // destructor


void CadResultsDlg::OnNotified( const vxCAD::vxCadFindingsArgs & args )
{
  if( m_pCurrDatasetList != args.m_pDataset )
    return;

  switch( args.m_eAction )
  {
  case vxCadFindingsArgs::CAD_ACTION_SELECTED:
    SendMessage( WM_USER_UPDATE_DISPLAY, CADLIST_SELECT_ITEM, MAKELPARAM( args.m_pDataset->m_cadFindings.GetSelectedIndex(), args.m_pDataset->GetUniqueID() ));
    break;
  case vxCadFindingsArgs::CAD_ACTION_UPDATED:
    SendMessage( WM_USER_UPDATE_DISPLAY, 0, MAKELPARAM( 0, args.m_pDataset->GetUniqueID() ));
    break;
  case vxCadFindingsArgs::CAD_ACTION_VISIBILITY:
    SendMessage( WM_USER_UPDATE_DISPLAY, CADLIST_UPDATE_SHOWALL, MAKELPARAM( 0, args.m_pDataset->GetUniqueID() ));
    break;
  case vxCadFindingsArgs::CAD_ACTION_REFRESH:
    SendMessage( WM_USER_UPDATE_DISPLAY, CADLIST_REINIT, MAKELPARAM( 0, args.m_pDataset->GetUniqueID() ));
    break;
  }
}


/**
 * Dialog data variable initialization.
 *
 *  @param pDX   Dialog data exchange instance.
 */
void CadResultsDlg::DoDataExchange(CDataExchange* pDX)
{
  DDX_Control(pDX, IDC_CDR_RESULTLIST, m_listResults);
  DDX_Control(pDX, IDC_CDR_SHOWFINDINGS, m_btnShowFindings);
  DDX_Control(pDX, IDC_CDR_NEXTFIND, m_btnNextFind);
  DDX_Control(pDX, IDC_CDR_VENDORIMAGE, m_staticVendorImage);
  DDX_Control(pDX, IDC_CDR_SLIDER1, m_slider);
  CDialog::DoDataExchange(pDX);
} // DoDataExchange()


/**
 * message map
 **/
BEGIN_MESSAGE_MAP(CadResultsDlg, ViatronDialog)
  ON_NOTIFY(NM_CLICK, IDC_CDR_RESULTLIST, OnClickCadList)
  ON_BN_CLICKED(IDC_CDR_NEXTFIND, OnNextFindingClicked)
  ON_BN_CLICKED(IDC_CDR_SHOWFINDINGS, OnShowFindingsClicked)
  ON_NOTIFY(HDN_ITEMCLICK, 0, OnColumnHeaderItemClick)
  ON_MESSAGE(CadResultsDlg::WM_USER_UPDATE_DISPLAY, OnMessageUpdateDisplay)
  ON_WM_TIMER()
  ON_WM_PAINT()
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_CDR_RESULTLIST, OnItemChangedResultsList)
  ON_WM_LBUTTONDBLCLK()
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_SETCURSOR()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()


///////////////////////////////////
/// CadResultsDlg message handlers

/**
 * Init dialog procedure
 * 
 * @return   Create success
 */
BOOL CadResultsDlg::OnInitDialog()
{
  try
  {
    ViatronDialog::OnInitDialog();

    EnableToolTips(TRUE);
    EnableMouseTracking();

    // set button icons
    uint4 iIds[][2] = { {IDC_CDR_SHOWFINDINGS, IDB_DETECT_CAD_FINDINGS}, {IDC_CDR_NEXTFIND, IDB_NEXT_CAD_FINDING} } ;
    SetButtonIcons(this, iIds, sizeof(iIds)/sizeof(uint4[2]));

    // set listbox columns
    const int4 iListWidth = WinUtils::GetClientRect(&m_listResults).Width();
    m_listResults.InsertColumn(CadListColumns::eVISIBILITY, "X", LVCFMT_CENTER, int4(0.15 * iListWidth));
    m_listResults.InsertColumn(CadListColumns::eCOUNT, "#", LVCFMT_CENTER, int4(0.15 * iListWidth));
    m_listResults.InsertColumn(CadListColumns::eLOCATION, LoadResourceString(IDS_LOCATION_IN_CM), LVCFMT_CENTER, int4(0.5 * iListWidth));
    m_listResults.InsertColumn(CadListColumns::eSIZE, "Max.Diameter", LVCFMT_CENTER, int4(0.0 * iListWidth));
    m_listResults.InsertColumn(CadListColumns::eID, "GUID", LVCFMT_CENTER, int4(0));
    // create image list
    m_imglistChecks.Create(IDB_STATE_CHECKBOX_24, 16, 1, RGB(255,0,255));
    m_listResults.SetImageList(&m_imglistChecks, LVSIL_SMALL);
    // set listbox style
    m_listResults.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // set font for warning static displays
    CWnd *pStatic = GetDlgItem(IDC_CDR_WARNING);
    CFont *pFont = pStatic->GetFont();
    LOGFONT logFont;
    pFont->GetLogFont(&logFont);
    logFont.lfHeight = -12;
    m_fontWarning.CreateFontIndirect(&logFont);
    pStatic->SetFont(&m_fontWarning);
    pStatic->CenterWindow();

    pStatic = GetDlgItem(IDC_CDR_CADUNAVAILABLE);
    pStatic->SetFont(&m_fontWarning);
    pStatic->CenterWindow();

    // initialize the slider
    SetSliderValue();

    // initialize the list
    InitCadList();
    m_btnShowFindings.SetCheck(rdrGlobal.m_bDisplayCadFindings ? 1 : 0);
    OnShowFindingsClicked();

    m_timerModeCheck.SetTimer(this, 500);

    LoadCadVendorImage();

    // get the position to draw the vendor image
    CRect windowrect; m_staticVendorImage.GetWindowRect( &windowrect );
    ScreenToClient( &windowrect );
    m_vendorImageBounds = Gdiplus::Rect( windowrect.left, windowrect.top, windowrect.Width(), windowrect.Height() );
    m_staticVendorImage.ShowWindow( SW_HIDE );

    return TRUE;
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnInitDialog" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::OnInitDialog" );
  }

  return FALSE;
} // OnInitDialog()


/**
 * Local helper function to determine if sphericity value is part of the cad findings
 *
 * @param rDataset   the dataset to test
 *
 * @return           true sphericity value is part of the cad findings
 */
bool IsSphericityAvailable( const Dataset & rDataset )
{ 
  // first check if the findings are loaded
  bool bVisible = rDataset.IsCadFindingsLoaded();

  if( rDataset.m_cadFindings.GetCount() > 0 &&
      rDataset.m_cadFindings.GetCadVendor() == "Medicsight" &&
      atof(rDataset.m_cadFindings.GetCadApiVersion().c_str()) > 3.9F  )
  { /// This is Medicsight ColonCAD version 4.0 or later. No slider bar display needed
    return false;
  }

  // slider should not be visible if any of the findings do not have the sphericity variable
  for ( int4 i=0; i<rDataset.m_cadFindings.GetCount(); i++ )
  {
    const vxCadFinding::VendorSpecificVariablesMap& vendMap = rDataset.m_cadFindings.GetFinding(i).GetVendorSpecificVariablesMap();
    vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter1 = vendMap.find( "Sphericity" );
    vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter2 = vendMap.find( "Sphericity0" ); // conceivably, for new settings, there must be at least one value....right? :S
    if ( varIter1 == vendMap.end() && varIter2 == vendMap.end() )
    {
      bVisible = false;
    }
  } // for

  return bVisible;

} // IsSphericityAvailable()


/**
 * Window resize handler
 *
 * @param uType   Type of resize event
 * @param iCX     New width of window
 * @param iCY     New height of window
 */
void CadResultsDlg::OnSize( UINT uType, int iCX, int iCY )
{

  try
  {
    // only process valid sizes
    if ( iCX <= 0 && iCY <= 0 )
    {
      return;
    }

    ViatronDialog::OnSize(uType, iCX, iCY);

    // layout the top controls
    const int4 iButtonSize = 62;
    const int4 iGap = 2;
    const int4 iButtonX = iCX - iGap - iButtonSize;
    const int4 iVendorWidth = iCX - iButtonSize - 3 * iGap;
    CWnd * pCtrl = NULL;
    if ( ( pCtrl = GetDlgItem( IDC_CDR_SHOWFINDINGS ) ) != NULL )
      pCtrl->SetWindowPos( NULL, iButtonX, iGap, iButtonSize, iButtonSize, SWP_NOZORDER );
    if ( ( pCtrl = GetDlgItem( IDC_CDR_VENDORIMAGE ) ) != NULL )
      pCtrl->SetWindowPos( NULL, iGap, iGap, iVendorWidth, iButtonSize, SWP_NOZORDER );
    m_vendorImageBounds = Gdiplus::Rect( iGap, iGap, iVendorWidth, iButtonSize );

    // layout the list control
    const int4 iListY = 2 * iGap + iButtonSize;
    const int4 iListWidth = iCX - 2 * iGap;
    const int4 iListHeight = iCY - 4 * iGap - 2 * iButtonSize;
    if ( ( pCtrl = GetDlgItem( IDC_CDR_RESULTLIST ) ) != NULL )
    {
      pCtrl->SetWindowPos( NULL, iGap, iListY, iListWidth, iListHeight, SWP_NOZORDER );
      m_listResults.SetColumnWidth(0, int4(iListWidth * 0.15 * 1.1));
      m_listResults.SetColumnWidth(1, int4(iListWidth * 0.20 * 1.1));
      m_listResults.SetColumnWidth(2, int4(iListWidth * 0.50 * 1.1));
    }

    // layout the next finding button
    const int4 iNextButtonY = iCY - iGap - iButtonSize;
    const int4 iStatsWidth = iCX - 0 * iGap - iButtonSize;
    const int4 iNextButtonX = iStatsWidth + 2 * iGap;
    if ( (pCtrl = GetDlgItem( IDC_CDR_NEXTFIND )) != NULL )
      pCtrl->SetWindowPos( NULL, iNextButtonX, iNextButtonY, iButtonSize, iButtonSize, SWP_NOZORDER );

    // determine if CAD slider should be available
    CWnd * pSlider = GetDlgItem( IDC_CDR_SLIDER1 );
    bool bSlider = ( pSlider != NULL ) &&
                   ( IsSphericityAvailable( *rdrGlobal.m_pCurrDataset ) ); 

    // layout slider control
    const int4 iSliderHeight = 17;
    const int4 iSliderY = iCY - iGap - iSliderHeight;
    if ( bSlider )
    {
      pSlider->SetWindowPos( NULL, iGap, iSliderY, iStatsWidth, iSliderHeight, SWP_NOZORDER );
      pSlider->ShowWindow( SW_SHOW );
    }
    else
    {
      if ( pSlider ) 
      {
        pSlider->ShowWindow( SW_HIDE );
      } // if
    } // else
          
    // layout the statistics
    const int4 iStatsShrink = 5;
    if ( bSlider )
    {
      const int4 iStatsHeight = iButtonSize - iSliderHeight - 0 * iGap - iStatsShrink;
      const int4 iStatsY = iNextButtonY + iStatsShrink;
      if ( ( pCtrl = GetDlgItem( IDC_CDR_STATS ) ) != NULL )
        pCtrl->SetWindowPos( NULL, iGap, iStatsY, iStatsWidth, iStatsHeight, SWP_NOZORDER );
    }
    else
    {
      const int4 iStatsHeight = iButtonSize - 2 * iStatsShrink;
      const int4 iStatsY = iNextButtonY + iStatsShrink;
      if ( ( pCtrl = GetDlgItem( IDC_CDR_STATS ) ) != NULL )
        pCtrl->SetWindowPos( NULL, iGap, iStatsY, iStatsWidth, iStatsHeight, SWP_NOZORDER );
    }

    // layout the warning static displays
    CWnd *pStatic;
    if ( (pStatic = GetDlgItem(IDC_CDR_WARNING)) != NULL )
      pStatic->CenterWindow();

    if ( (pStatic = GetDlgItem(IDC_CDR_CADUNAVAILABLE)) != NULL )
      pStatic->CenterWindow();
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnSize" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::OnSize" );
  }

} // OnSize()


/**
 * WM_PAINT handler
 */ 
void CadResultsDlg::OnPaint()
{
  try
  {
    CPaintDC dc( this );

    if( m_pVendorImage )
    {
      Gdiplus::Graphics gfx( dc.GetSafeHdc() );

      gfx.SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );

      CPoint pt;
      GetCursorPos(&pt);
      ScreenToClient(&pt);
      if ( m_vendorImageBounds.Contains(pt.x, pt.y) )
      {
        GdiplusUtilities::DrawImageScaled( gfx, m_pVendorImage, m_vendorImageBounds );
        //gfx.DrawImage( m_pVendorImage, m_vendorImageBounds.GetLeft(), m_vendorImageBounds.GetTop()  );
        //gfx.DrawString();
        m_bLogoDrawnOpaque = true;
      }
      else
      {
        GdiplusUtilities::DrawImageScaled( gfx, m_pVendorImage, m_vendorImageBounds, 0.5F );
        //GdiplusUtilities::DrawImageTransparent(gfx, m_pVendorImage, m_vendorImageBounds.GetLeft(), m_vendorImageBounds.GetTop(), 0.5F);
        m_bLogoDrawnOpaque = false;
      }
    }
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnPaint" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::OnPaint" );
  }
} // OnPaint()


/**
 * toggle slider visibility
 **/
void CadResultsDlg::OnToggleShowSlider()
{
  if (m_slider.IsWindowVisible())
  {
    m_slider.ShowWindow(SW_HIDE);
  }
  else
  {
    m_slider.ShowWindow(SW_SHOW);
    m_slider.SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
  }
} // OnToggleShowSlider()


/**
 * WM_LBUTTONDBLCLK handler.
 *
 * @param uFlags   Current button-state flags.
 * @param origPt   Point of click.
 */
void CadResultsDlg::OnLButtonDblClk(UINT uFlags, CPoint origPt)
{
  if (m_vendorImageBounds.Contains(origPt.x, origPt.y))
  {
    CWaitCursor crsr;
    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

    // Open the picture file
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;
    sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    sei.lpFile = m_sVendorWebsite.c_str();
    sei.lpVerb = "open";
    sei.lpDirectory = ".";
    sei.nShow = SW_SHOW;

    ShellExecuteEx(&sei);
  }
} // OnLButtonDblClk()


/**
 * WM_MOUSEMOVE handler
 *
 * @param uFlags   Current key flags
 * @param point    Current point of mouse move
 */
void CadResultsDlg::OnMouseMove(UINT uFlags, CPoint point)
{
  ViatronDialog::OnMouseMove(uFlags, point);
  CPoint pt;
  GetCursorPos(&pt);
  ScreenToClient(&pt);
  const bool bInRect( m_vendorImageBounds.Contains(pt.x, pt.y) );
  if (bInRect ^ m_bLogoDrawnOpaque)
    InvalidateRect(CRect(m_vendorImageBounds.GetLeft(), m_vendorImageBounds.GetTop(), m_vendorImageBounds.GetRight(), m_vendorImageBounds.GetBottom()));
} // OnMouseMove()


/**
 * WM_SETCURSOR: Called to set mouse cursor icon
 * 
 * @param pWnd       Pointer to window containing cursor
 * @param uHitTest   Area within dialog where cursor is location (e.g. HT_BORDER)
 * @param uMessage   Mouse message number
 * @return           TRUE if cursor was set
 */
BOOL CadResultsDlg::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  CPoint pt;
  GetCursorPos(&pt);
  ScreenToClient(&pt);
  if ( m_vendorImageBounds.Contains(pt.x, pt.y) )
    SetCursor(AfxGetApp()->LoadCursor(IDC_INDEXHAND));
  else
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

  return TRUE;
} // OnSetCursor()


/**
 * When mouse leaves dialog
 */
void CadResultsDlg::OnMouseLeave()
{
  InvalidateRect(CRect(m_vendorImageBounds.GetLeft(), m_vendorImageBounds.GetTop(), m_vendorImageBounds.GetRight(), m_vendorImageBounds.GetBottom()));
} // OnMouseLeave()


/**
 * Loads the cad vendor image. Parses the vendor string specified in the xml configuration file.
 * 
 * string format: "vendorId1;vendorImage1|vendorId2;vendorImage2|vendorId;vendorImageN|"
 */
void CadResultsDlg::LoadCadVendorImage()
{
  USES_CONVERSION;
  VCSettings::CadVendorMap vendorMap;

  // read in the cad vendor string from the configuration xml
  rdrGlobal.GetSettings()->GetCadVendors(vendorMap);
  if ( vendorMap.size() <= 0 || (!rdrGlobal.m_supine.IsCadFindingsLoaded() && !rdrGlobal.m_prone.IsCadFindingsLoaded()) )
    return;

  // get the vendor id we are looking for
  const std::string & sCadVendor = GetIntendedDatasets().first->IsCadFindingsLoaded() ?
    GetIntendedDatasets().first ->m_cadFindings.GetCadVendor() : 
    GetIntendedDatasets().second->m_cadFindings.GetCadVendor();
  VCSettings::CadVendorMap::const_iterator it = vendorMap.find( sCadVendor );

  // did we find the cad vendor?
  if ( it == vendorMap.end() )
  {
    // log error
    LogErr( "Unable to locate vendor id '" + sCadVendor + "' within vendor map:", "CadResultsDlg", "LoadCadVendorImage" );
    for ( it = vendorMap.begin(); it != vendorMap.end(); ++it )
    {
      LogDbg( std::string( "  " ) + std::string( it->first ) + std::string( " " ) + std::string( it->second.m_sVendor ), "", "" );
    }

    // search for the default one
    it = vendorMap.find( "" );
    if ( it == vendorMap.end() )
    {
      LogErr( "Default vendor id missing!", "CadResultsDlg", "LoadCadVendorImage" );
      return;
    }
  } // if match not found

  // get the path of the executable
  char sExePath[ _MAX_PATH ];
  GetModuleFileName( AfxGetInstanceHandle(), sExePath, _MAX_PATH );

  // get the drive letter and directory of executable
  char sDrive[_MAX_DRIVE];
  char sDirectory[_MAX_DIR ];
  _splitpath( sExePath, sDrive, sDirectory, NULL, NULL );

  // format the image path
  std::string sImagePath = std::string(sDrive) + std::string(sDirectory) + it->second.m_sImagePath;

  // load the cad vendor image
  m_pVendorImage = new Gdiplus::Bitmap( A2W( sImagePath.c_str() ) );
  if( m_pVendorImage->GetLastStatus() != Gdiplus::Ok )
    LogErr( "Unable to load cad vendor image.", "CadResultsDlg", "LoadCadVendorImage" );

  m_sVendorWebsite = it->second.m_sWebsite;

} // LoadCadVendorImage()


/**
 * compares two items
 * @param lParam1      first compare item
 * @param lParam2      second compare item
 * @param lParamSort   sort parameter/data
 * @return             comparison result result
 */
int CALLBACK CadResultsDlg::FindingsListCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  Dataset & rDataset = *(GetIntendedDatasets(ReaderLib::DatasetOrientations::TypeEnum(HIWORD(lParamSort))).first);

  const vxCadFinding& rFind1 = rDataset.m_cadFindings.GetFinding(int4(lParam1));
  const vxCadFinding& rFind2 = rDataset.m_cadFindings.GetFinding(int4(lParam2));

  switch ( CadListColumns::eLISTCOLS(LOWORD(lParamSort)) )
  {
    case CadListColumns::eVISIBILITY:
    {
      ColonSession::FindingInfo::DecisionEnum e1 = rDataset.m_session.m_mapCadFindingInfo[rFind1.GetId()].GetDecision();
      ColonSession::FindingInfo::DecisionEnum e2 = rDataset.m_session.m_mapCadFindingInfo[rFind2.GetId()].GetDecision();
      if      (e1 > e2)                       return -1;
      else if (e1 < e2)                       return +1;
      else if (int4(lParam1) < int4(lParam2)) return -1;
      else if (int4(lParam1) > int4(lParam2)) return +1;
      else return 0;
    }
    default:
    {
      float4 f1 = rDataset.GetNearestSkeletonDFS( rFind1 );
      float4 f2 = rDataset.GetNearestSkeletonDFS( rFind2 );
      return f1 < f2 ? -1 : ((f1 > f2) ? 1 : 0);
    }
  } // switch(lParamSort)

  return int4(lParam1) <= int4(lParam2);
} // FindingsListCompareProc()


/**
 * User clicks a column header for sorting
 *
 * @param pNMHDR    Notification message information
 * @param pResult   Out result
 */
void CadResultsDlg::OnColumnHeaderItemClick( NMHDR * pNMHDR, LRESULT * pResult )
{
  // NOTE: Disabling column-sort for now.  As is, it is buggy and seemingly not very useful regardless.
  return;

  NMHEADER* pLV = (NMHEADER*) pNMHDR;
/*  if (pLV->iItem == CadListColumns::eVISIBILITY)
    rdrGlobal.m_pCurrDataset->m_cadFindings.SortFindings( Dataset::SortByVisibility );
  else
    rdrGlobal.m_pCurrDataset->m_cadFindings.SortFindings( Dataset::SortBySkeletonIndex );
  InitCadList();*/

  m_listResults.SortItems(FindingsListCompareProc, MAKELONG(pLV->iItem, m_eIntendedDataset));

  *pResult = 0;
} // OnColumnHeaderItemClick()


/**
 * Message: WM_USER_UPDATE_DISPLAY
 * 
 * @param wParam   Update message
 * @param lParam   Message-specific parameter
 * @return         Unused
 */
LRESULT CadResultsDlg::OnMessageUpdateDisplay(WPARAM wParam, LPARAM lParam)
{
  Dataset & rDataset = *(GetIntendedDatasets().first);
  switch (wParam)
  {
    case CADLIST_UPDATE_STATS:
    case CADLIST_UPDATE_SHOWALL:
    {
      LVITEM item;
      memset(&item, 0, sizeof(LVITEM));
      item.mask = LVIF_TEXT;
      const int4 iIDSize(1024);
      char sText[iIDSize+1];
      item.cchTextMax = 1024; item.pszText = sText;
      item.iSubItem = CadListColumns::eID;

      for (item.iItem = 0; item.iItem < m_listResults.GetItemCount(); item.iItem++)
      {
        memset(sText, 0, sizeof(char)*(iIDSize+1));
        m_listResults.GetItem(&item);

        map< string, ColonSession::FindingInfo >::iterator it = rDataset.m_session.m_mapCadFindingInfo.find(sText);
        if (it != rDataset.m_session.m_mapCadFindingInfo.end())
        {
          m_listResults.SetItem(item.iItem, 0, LVIF_IMAGE | LVIF_PARAM, 0, int4(it->second.GetDecision()),
            0,0, LPARAM((it->second.GetDecision() != ColonSession::FindingInfo::eDecisionDenied) ? 1 : 0), 0);
        }
      }

      // refresh displays
      //rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
      //AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

      if ( (m_btnShowFindings.GetCheck() == 1) != rdrGlobal.m_bDisplayCadFindings )
        m_btnShowFindings.SetCheck( rdrGlobal.m_bDisplayCadFindings ? 1 : 0 );

      if (wParam == CADLIST_UPDATE_SHOWALL)
      {
        OnShowFindingsClicked();
      }

      break;
    }

    case CADLIST_REINIT: // Re-initializes the CAD list
    {
      InitCadList();
      break;
    }

    case CADLIST_SELECT_ITEM:
      if ( GetIntendedDatasets().first->GetUniqueID() == HIWORD(lParam) )
        SetCurrentFindingSel( LocateFinding(rDataset.m_cadFindings.GetFinding(LOWORD(lParam)).GetId() ) );
      break;

    default:
      break;
  }

  SetStatsText();
  return 0;
} // OnMessageUpdateDisplay()


/**
 * WM_TIMER message
 */
void CadResultsDlg::OnTimer(UINT_PTR uIDEvent)
{

  try
  {
    // NOTE: If more than one timer is installed, then enable the following check
    // if (uIDEvent == m_timerModeCheck)
    {
      SyncToCurrentDataset();

      // disable the slider if concurrent read or CAD mark visibility is not enabled
      if ( GetDlgItem( IDC_CDR_SLIDER1 ) != NULL )
      {
        Dataset & rDataset = *(GetIntendedDatasets().first);
        const vxCadFindings& findings = rDataset.m_cadFindings;

        if( findings.GetCount() <= 0 || (findings.GetCount() > 0 &&
            rDataset.m_cadFindings.GetCadVendor() == "Medicsight" &&
            atof(rDataset.m_cadFindings.GetCadApiVersion().c_str()) > 3.9F)  )
        {
          GetDlgItem( IDC_CDR_SLIDER1 )->ShowWindow( SW_HIDE );
        }
        else
        {
          const vxCadFinding::VendorSpecificVariablesMap& vendMap = findings.GetFinding(0).GetVendorSpecificVariablesMap();
          vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter1 = vendMap.find( "Sphericity" );
          vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter2 = vendMap.find( "Sphericity0" ); // conceivably, for new settings, there must be at least one value....right? :S
          const bool bShowSlider = ( varIter1 != vendMap.end() || varIter2 != vendMap.end() );

          GetDlgItem( IDC_CDR_SLIDER1 )->ShowWindow( bShowSlider ? SW_SHOW : SW_HIDE );

          const bool bEnableSlider = rdrGlobal.m_bDisplayCadFindings || rdrGlobal.m_bAlwaysShowCadArrows;
          GetDlgItem( IDC_CDR_SLIDER1 )->EnableWindow( bEnableSlider );
        }
      }
    }
  } // try
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnTimer" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::OnTimer" );
  } // catch

} // OnTimer( uIDEvent )


/**
 * Empty override so that user cannot dismiss dialog
 */
void CadResultsDlg::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
} // OnOK()


/**
 * Empty override so that user cannot dismiss dialog
 */
void CadResultsDlg::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * Message: LVN_ITEMCHANGED, when the selection state of an item in the IDC_CDB_MISSEDRGN_LIST list changes
 *
 * @param pNMHDR    Notification message information
 * @param pResult   Result
 */
void CadResultsDlg::OnItemChangedResultsList(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  if ((pNMLV->uChanged == LVIF_STATE) && ((pNMLV->uNewState & LVIS_SELECTED)==LVIS_SELECTED))
  {
    if (!WinUtils::IsKeyStateDown(VK_CONTROL))
      JumpToFinding(pNMLV->iItem);
  }
  *pResult = 0;
} // OnItemChangedResultsList()

//////////////////////////////////////////////////////////////////////////

/**
 * initialize cad list control
 */
void CadResultsDlg::InitCadList()
{

  try
  {
    Dataset & rDataset = *(GetIntendedDatasets().first);
    m_listResults.DeleteAllItems();
    int4 iActualItemCount = 0;
    for (int iFinding(0); iFinding < rDataset.m_cadFindings.GetCount(); iFinding++)
    {
      const vxCadFinding & rFind = rDataset.m_cadFindings.GetFinding(iFinding);
      ASSERT(rFind.GetCentroidMM().GetUnits() == vxCadVector::MILLIMETER);

      // see if this finding has a slider threshold and if it meets the current criteria
      if ( RenderUtil::IsFindingVisible( rFind, rDataset.m_session.m_mapCadFindingInfo,
        rDataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings) )
      {
        // Item, selection image(image parameter), LPARAM(visibility)
        const ColonSession::FindingInfo & info = rDataset.m_session.m_mapCadFindingInfo[ rFind.GetId() ];
        const int4 iImage = int4( info.GetDecision() );
        const int4 iListItem = m_listResults.InsertItem(LVIF_IMAGE | LVIF_PARAM, iActualItemCount++, NULL, 0, UINT(0), iImage%3,
          LPARAM((iImage%3!= int4(ColonSession::FindingInfo::eDecisionDenied)) ? 1 : 0));
        char sItem[101];

        // Count
        sprintf(sItem, "%d", iListItem+1);
        m_listResults.SetItem(iListItem, CadListColumns::eCOUNT, LVIF_TEXT, sItem, 0,0,0,0,0);

        // Location
        sprintf(sItem, "%.1f", rDataset.GetNearestSkeletonDFS( rFind ) * 0.1F);
        m_listResults.SetItem(iListItem, CadListColumns::eLOCATION, LVIF_TEXT, sItem, 0,0,0,0,0);

        // Size: (endpoint - stpt).GetLength();
        const vxCadDimension& dim = rFind.GetDimensions().GetLargestDimension();
        sprintf(sItem, "%d.%d", (dim.GetEndPoint().GetVector() - dim.GetStartPoint().GetVector()).GetLength());
        m_listResults.SetItem(iListItem, CadListColumns::eSIZE, LVIF_TEXT, sItem, 0,0,0,0,0);

        // ID
        m_listResults.SetItem(iListItem, CadListColumns::eID, LVIF_TEXT, rFind.GetId().c_str(), 0,0,0,0,0);
      } // if showing this finding
    }

    // disconnect old one
    if( m_pCurrDatasetList )
      m_pCurrDatasetList->m_cadFindings.Disconnect( this );
   
    // connect new one
    m_pCurrDatasetList = &rDataset;
    m_pCurrDatasetList->m_cadFindings.Connect( this );

    SetStatsText();

  } // try
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "InitCadList" );
  } // catch

} // InitCadList()


/**
 * sets the stats display
 */
void CadResultsDlg::SetStatsText()
{

  try
  {

    // count the number of accepted findings
    int4 iNumAcceptedFindings( 0 );
    int4 iNumFindingsTotal( 0 );
    Dataset & rDataset = *(GetIntendedDatasets().first);
    bool bSphericityValid = false;
    for ( int i=0; i<rDataset.m_cadFindings.GetCount(); ++i )
    {

      // get the finding
      const vxCadFinding & finding = rDataset.m_cadFindings.GetFinding( i );

      // see if the finding is visible (if applicable)
      bool bFindingVisible = true;
      const vxCadFinding::VendorSpecificVariablesMap& vendMap = finding.GetVendorSpecificVariablesMap();
      vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter1 = vendMap.find( "Sphericity" );
      vxCadFinding::VendorSpecificVariablesMap::const_iterator varIter2 = vendMap.find( "Sphericity0" ); // conceivably, for new settings, there must be at least one value....right? :S
      if ( varIter1 != vendMap.end() || varIter2 != vendMap.end() )
      {
        bFindingVisible = RenderUtil::IsFindingVisible( finding,rDataset.m_session.m_mapCadFindingInfo, rDataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings);
        bSphericityValid = true;
      }
      if ( bFindingVisible )
      {
        iNumFindingsTotal++;

        // see if the finding is accepted or undecided (implicitly accepted)
        if ( RenderUtil::GetFindingDecision( finding, rDataset.m_session.m_mapCadFindingInfo ) != ColonSession::FindingInfo::eDecisionDenied )
        {
          iNumAcceptedFindings ++;
        }
      }
      
    } // for all findings in the primary dataset

    const bool bSupine(rDataset.GetOrientation() == DatasetOrientations::eSUPINE);
    CString sText;

    if ( rdrGlobal.m_bDisplayCadFindings )
    {
      if ( rdrGlobal.IsDualDataLoaded() )
      {
        Dataset & rDsetOpp = * (GetIntendedDatasets().second);

        // count the total number of findings in the other dataset
        int4 iNumTotalSecondary( 0 );
        for ( int i=0; i<rDsetOpp.m_cadFindings.GetCount(); ++i )
        {

          // get the finding
          const vxCadFinding & finding = rDsetOpp.m_cadFindings.GetFinding( i );

          // see if the finding is above the threshold (if applicable)
          if ( RenderUtil::IsFindingVisible( finding, rDsetOpp.m_session.m_mapCadFindingInfo, rDsetOpp.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings) )
          {
            iNumTotalSecondary++;
          }
          
        } // for all findings in the primary dataset

        sText.LoadString( IDS_CADFINDINGS_STATS_DUAL );
        sText.Replace("#<total2>", ToAscii(iNumTotalSecondary).c_str());
        sText.Replace("#<dataset2>", ColonViatronixWnd::PatientOrientationToString(rDsetOpp.GetPatientOrientation()).c_str());
      }
      else
      {
        sText.LoadString( IDS_CADFINDINGS_STATS_SINGLE );
      }

      sText.Replace("#<count>", ToAscii(iNumAcceptedFindings).c_str());
      sText.Replace("#<total>", ToAscii(iNumFindingsTotal).c_str());
      sText.Replace("#<dataset>", ColonViatronixWnd::PatientOrientationToString(rDataset.GetPatientOrientation()).c_str());

      if ( bSphericityValid )
      {
        CString sSpherText;
        sSpherText.Format(IDS_CADFINDINGS_SPHERICITY_STATS, rDataset.m_cadFindings.m_iCadThresholdValue);
        sText += sSpherText;
      }
    } // if (rdrGlobal.bDisplayCadFindings)

    SetDlgItemText(IDC_CDR_STATS, sText);

  } // try
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "SetStatsText" );
  } // catch

} // SetStatsText()


/**
 * Returns the current list control index of the finding
 *
 * @param sID   GUID of the finding to select
 * @return      Index of the finding if found, -1 otherwise
 */
int4 CadResultsDlg::LocateFinding( const std::string & sID )
{
  LVITEM item;
  memset(&item, 0, sizeof(LVITEM));
  item.mask = LVIF_TEXT;
  const int4 iIDSize(1024);
  char sText[iIDSize+1];
  item.cchTextMax = 1024; item.pszText = sText;
  item.iSubItem = CadListColumns::eID;

  for (item.iItem = 0; item.iItem < m_listResults.GetItemCount(); item.iItem++)
  {
    memset(sText, 0, sizeof(char)*(iIDSize+1));
    m_listResults.GetItem(&item);

    if (sID == sText)
    {
      return item.iItem;
    }
  }

  return -1;
}


/**
 * Selects the given finding
 *
 * @param sID   GUID of the finding to select
 */
void CadResultsDlg::SelectFinding( const std::string & sID )
{
  int4 iFinding(LocateFinding(sID));
  if (iFinding >= 0)
  {
    m_listResults.SetItemState(iFinding, LVIS_SELECTED, LVIS_SELECTED);
  }
}


/**
 * Called when results listbox is clicked
 *
 * @param pNMHDR    Pointer to notification message structure
 * @param pResult   Return result
 */
void CadResultsDlg::OnClickCadList( NMHDR * pNMHDR, LRESULT * pResult )
{
  NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
  *pResult = 0;

  if (pLV->iSubItem == CadListColumns::eVISIBILITY)
  {
    LVITEM item;
    memset(&item, 0, sizeof(LVITEM));
    item.mask = LVIF_IMAGE | LVIF_TEXT;
    const int4 iIDSize(1024);
    char sText[iIDSize+1]; memset(sText, 0, sizeof(char)*(iIDSize+1));
    item.iItem = pLV->iItem; item.cchTextMax = 1024; item.pszText = sText;
    item.iSubItem = CadListColumns::eID;
    m_listResults.GetItem(&item);

    Dataset & rDataset = *(GetIntendedDatasets().first);

    map< string, ColonSession::FindingInfo >::iterator it = rDataset.m_session.m_mapCadFindingInfo.find( sText );
    if ( it != rDataset.m_session.m_mapCadFindingInfo.end() )
    {
      // changed from...to...: Undecided (0) ==> Denied (2) ==> Accepted (1)
      it->second.SetDecision( ColonSession::FindingInfo::DecisionEnum((int4(it->second.GetDecision())+2) % 3) );
      m_listResults.SetItem(item.iItem, 0, LVIF_IMAGE | LVIF_PARAM, 0, int4(it->second.GetDecision()),
        0,0, LPARAM((it->second.GetDecision() != ColonSession::FindingInfo::eDecisionDenied) ? 1 : 0), 0);

      SetStatsText();

      // refresh displays
      rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
      AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
    }
  }
} // OnClickCadList()


/**
 * Jump the viewpoint to a finding
 *
 * @param iFinding   index of the finding to which to jump
 */
void CadResultsDlg::JumpToFinding( const int4 iFinding )
{
  try
  {
    SyncToCurrentDataset();

    LVITEM item;
    memset(&item, 0, sizeof(LVITEM));
    item.mask = LVIF_TEXT;
    const int4 iIDSize(1024);
    char sIDText[iIDSize+1]; 
    memset(sIDText, 0, sizeof(char)*(iIDSize+1));
    item.iItem = iFinding; 
    item.cchTextMax = 1024; 
    item.pszText = sIDText;
    item.iSubItem = CadListColumns::eID;
    if( iFinding >= 0 && iFinding < m_listResults.GetItemCount() )
    {
      m_listResults.GetItem(&item);
      Dataset * pDataset = GetIntendedDatasets().first;
      Utilities::CAD::JumpTo( *pDataset, sIDText );
    }
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "JumpToFinding(int)" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::JumpToFinding(int)" );
  }
} // JumpToFinding()


/**
 * Jump to next finding
 */
void CadResultsDlg::OnNextFindingClicked()
{
  try
  {
    if (!rdrGlobal.m_bDisplayCadFindings)
      return;

    // Get the current selection
    POSITION pos = m_listResults.GetFirstSelectedItemPosition();
    int4 iCurr = (pos != NULL) ? m_listResults.GetNextSelectedItem(pos) : m_listResults.GetItemCount()-1;

    LVFINDINFO findInfo; memset(&findInfo, 0, sizeof(LVFINDINFO));
    findInfo.lParam = 1; findInfo.flags = LVFI_PARAM | LVFI_WRAP;
    iCurr = m_listResults.FindItem(&findInfo, iCurr);

    if (iCurr >= 0)
    {
      SetCurrentFindingSel(iCurr);
      JumpToFinding(iCurr);
    }
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnNextFindingClicked" );
    AfxMessageBox( "Unhandled exception in CadResultsDlg::OnNextFindingClicked" );
  }
} // OnNextFindingClicked


/**
 * Sets value of slider to that of current session cad value.
 */
void CadResultsDlg::SetSliderValue()
{
  Dataset & rDataset = *(GetIntendedDatasets().first);

  if( rDataset.m_cadFindings.GetCount() > 0 &&
      rDataset.m_cadFindings.GetCadVendor() == "Medicsight" &&
      atof(rDataset.m_cadFindings.GetCadApiVersion().c_str()) > 3.9F  )
  { /// This is Medicsight ColonCAD version 4.0 or later. Default value should be 50
    rDataset.m_cadFindings.m_iCadThresholdValue = 50;
  } //

  // set the proper range
  const int4 iSliderMin(0), iSliderMax(100);
  m_slider.SetRange(iSliderMin, iSliderMax);

  // interpolate
  const int4 iCadMin(0), iCadMax(100);
  const float4 fPos = Interpolate(
    static_cast< float4 >( iCadMin ),
    static_cast< float4 >( rDataset.m_cadFindings.m_iCadThresholdValue ),
    static_cast< float4 >( iCadMax ),
    static_cast< float4 >( iSliderMin  ),
    static_cast< float4 >( iSliderMax ) );

  // set slider position
  m_slider.SetPos( Bound( iSliderMin, static_cast< int4 >( fPos ), iSliderMax ) );

} // SetSliderValue()


/**
 * Searches for an instance of the cad dialog and posts message to it
 *
 * @param childCaller   child window invoking message
 * @param uMsg          message
 * @param wp            message-dependent parameter
 * @param lp            message-dependent parameter
 * @return              message-dependent value, return from PostMessage
 */
BOOL CadResultsDlg::SendMessageToDlgInstance(const ColonViews::ChildViews::ViatronChildView & childCaller, const UINT_PTR uMsg, const WPARAM wp, const LPARAM lp)
{
  LRESULT lRtnVal(0);
  int4 vIds[] = { IDC_TEMPLATE_CADRESULTSDLG, IDC_TEMPLATE_CADRESULTSDLG1 };
  for ( int4 i(0); i < 2; i++ )
  {
    // get the current instance of the cad dialog...
    CWnd* pCadDlg = childCaller.GetParent()->GetDlgItem(vIds[i]);

    // ...check if the given child is nested down a level...
    if (!pCadDlg)
    {
      pCadDlg = childCaller.GetParent()->GetParent()->GetDlgItem(vIds[i]);
    }

    // ...send message to cad dialog to update its display
    if ( pCadDlg && pCadDlg->IsWindowVisible() )
    {
      lRtnVal = pCadDlg->SendMessage(uMsg, wp, lp);
    }
  }

  return TRUE;
} // SendMessageToDlgInstance()


/**
 * Toggles display of findings
 */
void CadResultsDlg::OnShowFindingsClicked()
{
  try
  {

    if ( GetIntendedDatasets().first->IsCadFindingsLoaded() )
    {
      rdrGlobal.m_bDisplayCadFindings = m_btnShowFindings.GetCheck() != 0;
      m_listResults.ShowWindow( rdrGlobal.m_bDisplayCadFindings ? SW_SHOW : SW_HIDE );
      GetDlgItem( IDC_CDR_WARNING )->ShowWindow( rdrGlobal.m_bDisplayCadFindings ? SW_HIDE : SW_SHOW );
      m_btnNextFind.EnableWindow( rdrGlobal.m_bDisplayCadFindings );
      AfxGetMainWnd()->PostMessage( WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM( GLChildView::eGVT_ALL ), 0 );
      GetDlgItem( IDC_CDR_CADUNAVAILABLE )->ShowWindow( SW_HIDE );
    }
    else
    {
      m_listResults.ShowWindow( SW_HIDE );
      GetDlgItem( IDC_CDR_CADUNAVAILABLE )->ShowWindow( SW_HIDE );
      GetDlgItem( IDC_CDR_WARNING )->ShowWindow( SW_HIDE );
      m_btnNextFind.EnableWindow( FALSE );
    }

    SetStatsText();

  } // try
  catch ( ... )
  {
    LogErr( "Unhandled exception", "CadResultsDlg", "OnShowFindingsClicked" );
  } // catch

} // OnShowFindingsClicked()


/**
 * Sets the current finding selection
 *
 * @param iSel   Finding to be selected
 */
void CadResultsDlg::SetCurrentFindingSel(const int4 iSel)
{
  m_listResults.SetItemState(iSel, LVIS_SELECTED, LVIS_SELECTED);
  if (iSel < m_listResults.GetItemCount() - 1)
  {
    m_listResults.EnsureVisible(iSel+1, TRUE);
    m_listResults.EnsureVisible(iSel, TRUE);
  }
  else
  {
    m_listResults.EnsureVisible(iSel, TRUE);
  }
} // SetCurrentFindingsSel()


/**
 * Event handler for scrolling, handles IDC_CDR_
 * 
 * @param uSBCode      Scroll event code
 * @param uPos         Current slider position, validity depends on nSBCode
 * @param pScrollBar   Pointer to control receiving the event
 */
void CadResultsDlg::OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar)
{
  const int4 iCtrlID(pScrollBar->GetDlgCtrlID());

  if (iCtrlID != IDC_CDR_SLIDER1)
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
  case TB_THUMBPOSITION:  //while mouse wheel is on action
  case TB_ENDTRACK:       // called at the end of all movements
    // NOTE: Same code to get the value for all scroll events
    fPercent = float4( pSlider->GetPos() ) / float4( pSlider->GetRangeMax() - pSlider->GetRangeMin() );
    break;

  default: // don't handle any other notification codes
    break;
  }

  Dataset * pDataset = GetIntendedDatasets().first;

  // fPercent is the value of the slider between 0.0 and 1.0
  // convert it to a valid slider value
  pDataset->m_cadFindings.m_iCadThresholdValue = Bound( 0, static_cast< int4 >( 100 * fPercent ), 100 );

  // refresh findings list
  InitCadList();

  // refresh display
  pDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

  CDialog::OnHScroll(uSBCode, uPos, pScrollBar);
} // OnHScroll()


/**
 * ensure the cad list being shown is for the currently-selected dataset
 **/
void CadResultsDlg::SyncToCurrentDataset()
{

  if ( m_pCurrDatasetList != GetIntendedDatasets().first )
  {
    SetSliderValue();
    InitCadList();
    OnShowFindingsClicked();
  }

} // SyncToCurrentDataset()


/// returns the dataset indicated by m_eIntendedDataset
std::pair<ReaderLib::Dataset *,ReaderLib::Dataset *> CadResultsDlg::GetIntendedDatasets()
{
  switch ( m_eIntendedDataset )
  {
    case DatasetOrientations::eSUPINE:
      return std::make_pair(&rdrGlobal.m_supine,&rdrGlobal.m_prone);
      break;

    case DatasetOrientations::ePRONE:
      return std::make_pair(&rdrGlobal.m_prone,&rdrGlobal.m_supine);
      break;

    case DatasetOrientations::eSECONDARY:
      return std::make_pair(rdrGlobal.m_pSecondaryDataset,rdrGlobal.m_pCurrDataset);
      break;

    case DatasetOrientations::ePRIMARY:
    default:
      return std::make_pair(rdrGlobal.m_pCurrDataset,rdrGlobal.m_pSecondaryDataset);
      break;
  }
} // GetIntendedDatasets()


/// returns the dataset indicated by eOrient
std::pair<ReaderLib::Dataset *,ReaderLib::Dataset *> CadResultsDlg::GetIntendedDatasets(const ReaderLib::DatasetOrientations::TypeEnum eOrient)
{
  switch ( eOrient )
  {
    case DatasetOrientations::eSUPINE:
      return std::make_pair(&rdrGlobal.m_supine,&rdrGlobal.m_prone);
      break;

    case DatasetOrientations::ePRONE:
      return std::make_pair(&rdrGlobal.m_prone,&rdrGlobal.m_supine);
      break;

    case DatasetOrientations::eSECONDARY:
      return std::make_pair(rdrGlobal.m_pSecondaryDataset,rdrGlobal.m_pCurrDataset);
      break;

    case DatasetOrientations::ePRIMARY:
    default:
      return std::make_pair(rdrGlobal.m_pCurrDataset,rdrGlobal.m_pSecondaryDataset);
      break;
  }
} // GetIntendedDatasets()


// undefines
#undef FILE_REVISION


// $Log: CadResultsDlg.C,v $
// Revision 1.48.2.11  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.48.2.10  2010/04/16 19:29:43  dongqing
// cad panes
//
// Revision 1.48.2.9  2010/02/16 11:07:40  dongqing
// don't send messages to dialog when hidden
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.48.2.8  2009/11/25 22:15:09  dongqing
// widescreen
//
// Revision 1.48.2.7  2009/11/13 17:37:06  dongqing
// Issue Cad09-03: second cad pane to registration-cad layout
//
// Revision 1.48.2.6  2009/11/02 21:52:04  dongqing
// Issue Cad09-01: order of toggling visibility of cad marks: undecided->show->hide-> (swapping show/hide)
//
// Revision 1.48.2.5  2009/04/07 21:26:01  dongqing
// forget to check if the CAD finding list is empty or not. It was fixed now.
//
// Revision 1.48.2.4  2009/04/02 18:24:47  dongqing
// check if Medicsight CAD version is 4.0 or later. If it is, don't display slider
//
// Revision 1.48.2.3  2009/01/30 19:14:22  jmeade
// Issue 6238: ensure the cad of the current dataset is listed when jumping to findings
//
// Revision 1.48.2.2  2007/06/22 00:08:36  jmeade
// spacing, typos.
//
// Revision 1.48.2.1  2007/03/22 20:40:45  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.48  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.47  2007/03/10 04:10:43  jmeade
// code standards.
//
// Revision 1.46  2007/03/08 22:21:49  romy
// code review - header files adjusted
//
// Revision 1.45  2007/03/05 23:17:13  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.44  2007/02/26 19:16:02  jmeade
// Issue 5428: typo in cad pane stats, introduced in fixing Issue 5315.
//
// Revision 1.43  2007/02/23 18:53:05  jmeade
// Issue 5315: formatting/fitting cad findings statistics.
//
// Revision 1.42  2007/02/16 22:00:16  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.41  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.40  2007/01/24 19:52:41  romy
// fixed the mousewheel issue with Sphericity  issue
//
// Revision 1.39  2006/11/10 00:19:15  jmeade
// Issue 5097: Ensure cad list sorting will use intended dataset.
//
// Revision 1.38  2006/10/04 20:49:10  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.37  2006/09/14 17:41:23  jmeade
// Issue 4959: select finding on jump-to-finding.
//
// Revision 1.36  2006/09/01 22:56:21  jmeade
// Issue 4955: refresh cad findings list when toggling extra-colonic findings view.
//
// Revision 1.35  2006/08/02 23:05:55  jmeade
// Issue 4609: 'c' key toggles visibility of cad findings.
//
// Revision 1.34  2006/07/28 15:14:50  jmeade
// ensure default cursor returned when leaving the vendor image area.
//
// Revision 1.33  2006/06/02 21:49:35  jmeade
// Issue 4814: sizing dialog elements.
//
// Revision 1.32  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.31  2006/01/31 18:18:39  romy
// code review-rerranged include files
//
// Revision 1.30  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.29  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.28.2.37  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.28.2.36  2006/10/20 21:10:20  jmeade
// Issue 4883: removed erroneous setting of item data.
//
// Revision 1.28.2.35  2006/09/18 16:21:28  jmeade
// Issue 4959: select finding on jump-to-finding.
//
// Revision 1.28.2.34.2.6  2007/02/01 17:16:46  jmeade
// Issue 4933: use correct dataset for findings.
//
// Revision 1.28.2.34.2.5  2007/01/31 16:56:55  jmeade
// Issue 5306: using mouse wheel to adjust sphericity slider needed another case in scroll handler.
//
// Revision 1.28.2.34.2.4  2007/01/26 19:54:05  jmeade
// sphericity values start at zero.
//
// Revision 1.28.2.34.2.3  2007/01/22 20:36:54  jmeade
// Issue 5325: implemented updating of CAD display.
//
// Revision 1.28.2.34.2.2  2007/01/19 19:44:00  jmeade
// fixed check for at least 1 sphericity value per cad finding.
//
// Revision 1.28.2.34.2.1  2007/01/18 22:04:26  jmeade
// reflecting changes to medisight cad.
//
// Revision 1.28.2.34  2006/04/14 18:06:40  jmeade
// Issue 4698: implemented list-based sort (instead of data-based sort); disabled sorting until bugs are worked out.
//
// Revision 1.28.2.33  2006/03/29 20:52:23  frank
// read the segment number from the cad findings file
//
// Revision 1.28.2.32  2006/03/28 20:08:37  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.28.2.31  2006/03/27 18:31:07  jmeade
// Issue 4669: don't load vendor images when cad is not loaded.
//
// Revision 1.28.2.30  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.28.2.29  2006/03/22 21:10:38  jmeade
// Issue 4665: tooltip and buddy value for cad slider.
//
// Revision 1.28.2.28  2006/03/22 20:57:10  frank
// moved the finding visibility and other information into a class
//
// Revision 1.28.2.27  2006/02/28 17:56:55  frank
// fixed display of slider in some instances
//
// Revision 1.28.2.26  2006/02/23 12:57:06  frank
// bounded slider position
//
// Revision 1.28.2.25  2006/02/01 15:26:56  jmeade
// Set orthogonal point to intersect slices directly on the finding's centroid.
//
// Revision 1.28.2.24  2006/01/25 23:57:39  jmeade
// Issue 4607: Set the slider value when toggling between datasets.
//
// Revision 1.28.2.23  2006/01/12 20:54:59  frank
// removed debug messages
//
// Revision 1.28.2.22  2006/01/12 19:56:26  frank
// laidout/showed/hid/enabled/disabled the slider when appropriate
//
// Revision 1.28.2.21  2006/01/11 18:36:34  jmeade
// handling when cad is not available for an individual dataset.
//
// Revision 1.28.2.20  2006/01/11 18:18:20  frank
// Issue #4598: Fixed crashing of viewer when no cad findings exist
//
// Revision 1.28.2.19  2006/01/11 00:56:24  jmeade
// handling when cad is not available for an individual dataset.
//
// Revision 1.28.2.18  2006/01/10 16:46:34  frank
// Fixed display of CAD vendor image when only 1 of 2 cad files exists
//
// Revision 1.28.2.17  2006/01/10 14:15:11  frank
// Added Medicsight default slider value
//
// Revision 1.28.2.16  2006/01/09 17:11:23  frank
// fixed statistics text
//
// Revision 1.28.2.15  2006/01/09 16:48:47  frank
// decreased the status text window height
//
// Revision 1.28.2.14  2006/01/09 16:37:37  frank
// Issue #4593 and Issue #4595: hid and disabled CAD slider when appropriate
//
// Revision 1.28.2.13  2006/01/07 04:36:10  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.28.2.12  2006/01/07 02:23:00  jmeade
// init cad slider to current value on dlginit.
//
// Revision 1.28.2.11  2006/01/06 16:49:25  jmeade
// horizontal sphericity slider.
//
// Revision 1.28.2.10  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.28.2.9  2006/01/05 19:15:48  jmeade
// removed message beep
//
// Revision 1.28.2.8  2006/01/05 19:14:36  jmeade
// scrollbar for adjustment.
//
// Revision 1.28.2.7  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.28.2.6  2005/12/20 19:47:29  jmeade
// highlight logo to indicate "clickability" when mouse hovers over.
//
// Revision 1.28.2.5  2005/11/16 02:29:39  frank
// Issue #4538: Synchronized the S/P datasets after jumping to cad finding
//
// Revision 1.28.2.4  2005/11/15 19:41:54  frank
// allowed reversing through the cad findings
//
// Revision 1.28.2.3  2005/11/15 14:21:03  frank
// Issue #4514: Fixed crash jumping to a finding not in the current dataset
// Issue #4534: Fixed incorrect counting of cad findings total
//
// Revision 1.28.2.2  2005/11/11 02:39:44  jmeade
// Shift-Space in EndoView jumps to previous cad finding.
//
// Revision 1.28.2.1  2005/11/09 02:15:00  jmeade
// cad pane on supine/prone view.
//
// Revision 1.28  2005/10/27 21:32:58  jmeade
// double-click on logo invokes website.
//
// Revision 1.27  2005/10/27 01:26:09  jmeade
// split cad vendors into separate xml node/attribute sets.
//
// Revision 1.26  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.25  2005/10/21 15:08:30  geconomos
// corrected vendor image loading
//
// Revision 1.24  2005/10/14 19:31:05  jmeade
// jump to cad finding on selection changes.
//
// Revision 1.23  2005/10/13 18:11:32  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.22  2005/10/07 16:17:23  geconomos
// reworked cad vendor image
//
// Revision 1.21  2005/10/07 14:51:40  vxconfig
// hit other findings if only a single study is loaded
//
// Revision 1.20  2005/10/07 12:59:30  vxconfig
// bigger buttons
//
// Revision 1.19  2005/10/07 12:56:06  vxconfig
// bigger buttons
//
// Revision 1.18  2005/10/06 19:27:03  vxconfig
// prevented crash when double-clicking on an emty line
//
// Revision 1.17  2005/10/06 17:43:52  jmeade
// ignore double-click (i.e. don't jump to item) on the visibility checkbox.
//
// Revision 1.16  2005/10/06 14:49:16  vxconfig
// cleared stats when findings are hidden
//
// Revision 1.15  2005/10/06 02:14:03  jmeade
// cad vendor image.
//
// Revision 1.14  2005/10/05 22:49:27  jmeade
// update w/ plenty changes.
//
// Revision 1.13  2005/10/04 18:05:55  jmeade
// update cad list after changes.
//
// Revision 1.12  2005/09/30 22:20:52  jmeade
// background bitmap for cad dialog; sorting.
//
// Revision 1.11  2005/09/28 17:00:01  vxconfig
// name change: element -> index
//
// Revision 1.10  2005/09/28 00:35:49  jmeade
// context menu functionality in overview.
//
// Revision 1.9  2005/09/27 21:41:47  jmeade
// show/hide all findings; hide on 1st open.
//
// Revision 1.8  2005/09/27 01:08:20  jmeade
// enums for cad display constants.
//
// Revision 1.7  2005/09/23 21:01:01  jmeade
// jump to next finding; summary text.
//
// Revision 1.6  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.5  2005/09/22 18:26:42  jmeade
// jump to cad finding.
//
// Revision 1.4  2005/09/22 16:56:36  jmeade
// store cad finding locations along centerline.
//
// Revision 1.3  2005/09/21 17:16:55  jmeade
// read, display cad findings list info.
//
// Revision 1.2  2005/09/20 20:00:03  jmeade
// updated
//
// Revision 1.1  2005/09/13 21:45:14  jmeade
// cad results dialog, initial checkin.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/CadResultsDlg.C,v 1.48.2.11 2010/09/20 18:41:34 dongqing Exp $
// $Id: CadResultsDlg.C,v 1.48.2.11 2010/09/20 18:41:34 dongqing Exp $
