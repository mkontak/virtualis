// $Id: MissedRegionsDlg.C,v 1.1.2.5 2010/01/31 02:07:14 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  @jmeade@viatronix.net


#include "stdafx.h"
#include "MissedRegionsDlg.h"
#include "WinUtils.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"
#include "GLChildView.h"
#include "ViatronStudy.h"
#include "ViatronViewWnd.h"
#include "FunctionProgress.h"

using namespace ColonViews::Controls;
using namespace ColonViews::Dialogs;
using namespace ColonViews::ChildViews;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.1.2.5 $"

//////////////////////////////////////////////////////////////////////////
// Missed regions list box

/**
 * Message map
 */
BEGIN_MESSAGE_MAP(MissedRegionsListBox, CListCtrl)
  ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnItemChanged)
  ON_WM_TIMER()
END_MESSAGE_MAP()

/**
 * Constructor
 */
MissedRegionsListBox::MissedRegionsListBox()
 : m_pCurrDataset(NULL)
{
} // Constructor


/**
 * Initialize the listbox
 */
void MissedRegionsListBox::Initialize()
{

  const int4 iListWidth = WinUtils::GetClientRect(this).Width();
  InsertColumn(0, "#", LVCFMT_CENTER, int4(iListWidth * 0.12));
  InsertColumn(1, LoadResourceString(IDS_LOCATION_IN_CM), LVCFMT_CENTER, int4(iListWidth * 0.42));

  CString sqstr; sqstr.Format("%c", 178); // Super-script '2' (squared) character
  CString str; str.Format(IDS_SIZE_FORMAT, LoadResourceString(IDS_MM)+sqstr); str.Remove('.');
  InsertColumn(2, str, LVCFMT_CENTER, int4(iListWidth * 0.40));

  SetExtendedStyle(LVS_EX_FULLROWSELECT);

  UpdateList();

  m_timerUpdate.SetTimer(this, 1000);

} // Initialize()


/**
 * Override to size columns wide enough to display <lpszColumnHeading> text
 *
 * @param iCol                Column index.
 * @param lpszColumnHeading   Column text.
 * @param iFormat             Format options.
 * @param iWidth              Column width.
 * @param iSubItem            Index of sub-item with which the column is associated.
 * @return int                Column inserted.
 */
int MissedRegionsListBox::InsertColumn(int iCol, LPCTSTR lpszColumnHeading, int iFormat, int iWidth, int iSubItem)
{
  return CListCtrl::InsertColumn(iCol, lpszColumnHeading, iFormat,
    max(GetStringWidth(lpszColumnHeading)*1.12, iWidth), iSubItem);
} // InsertColumn()


/**
 * Updates the listbox displaying missed regions
 */
void MissedRegionsListBox::UpdateList()
{
  if (!IsWindow(GetSafeHwnd()))
  {
    return;
  }

  DeleteAllItems();

  CString str;
  for (int4 iCt(0); iCt < rdrGlobal.m_pCurrDataset->m_vPatchList.size(); iCt++)
  {
    // Count
    str.Format("%3d", iCt+1);
    InsertItem(iCt, str);

    // Distance from rectum (computed in mm, displayed in cm, hence division by 10)
    str.Format("%5.1f", rdrGlobal.m_pCurrDataset->m_vPatchList[iCt].m_fDistanceFromRectumMM / 10.0F);
    SetItemText(iCt, 1, str);

    DWORD iErr = GetLastError();

    // Size
    str.Format("%d", rdrGlobal.m_pCurrDataset->m_vPatchList[iCt].m_uSurfaceCount);
    SetItemText(iCt, 2, str);
  }

  m_pCurrDataset = rdrGlobal.m_pCurrDataset;

} // UpdateMissedRegionsList()


/**
 * Set the current missed region selection in the listbox
 * 
 * @param const int4 iSel   0-based selection
 */
void MissedRegionsListBox::SetCurMissedRgn(const int4 iSel)
{
  //  Set the current selection
  SetItemState(iSel, LVIS_SELECTED, LVIS_SELECTED);
  //  Ensure the selection is shown
  if (iSel < GetItemCount() - 1)
  {
    EnsureVisible(iSel+1, TRUE);
    EnsureVisible(iSel, TRUE);
  }
  else
  {
    EnsureVisible(iSel, TRUE);
  }
} // SetCurMissedRgn()


/**
 * Returns the index of the current listbox selection
 * 
 * @return int4   Selection index
 */
int4 MissedRegionsListBox::GetCurMissedRgn()
{
  // Get the current selection
  POSITION pos = GetFirstSelectedItemPosition();
  return (pos != NULL) ? GetNextSelectedItem(pos) : -1;
} // GetCurMissedRgn()


/**
 * Message: LVN_ITEMCHANGED, when the selection state of an item in the IDC_CDB_MISSEDRGN_LIST list changes
 *
 * @param pNMHDR    Notification message information
 * @param pResult   Result
 */
BOOL MissedRegionsListBox::OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  if ((pNMLV->uChanged == LVIF_STATE) && ((pNMLV->uNewState & LVIS_SELECTED)!=LVIS_SELECTED))
    return FALSE;

  const int4 iSelection(pNMLV->iItem);
  if ( (iSelection >= 0) && (iSelection < rdrGlobal.m_pCurrDataset->m_vPatchList.size()) )
  {
    const uint4 iPatch(iSelection);

    // jump to current patch
    Patch & patch = rdrGlobal.m_pCurrDataset->m_vPatchList[iPatch];
    if (patch.m_iSegment > -1)
    {
      rdrGlobal.m_pCurrDataset->m_segmentList.SetCurrentSegmentIndex(patch.m_iSegment);
    }

    Submarine & submarine = rdrGlobal.m_pCurrDataset->m_submarine;
    submarine.FullStop();
    submarine.ExitReplayMode();
    submarine.SetPositionWorld(patch.m_viewPointWorld);
    submarine.SetUpDirection(Normal<float4>(0,1,0));
    submarine.SetOrientation(Normal<float4>(patch.m_vViewDirWorld));
    submarine.EnforceOrthogonality();
    submarine.UpdatePosition();

    // update linked views too
    rdrGlobal.UpdateWorldPosition( * rdrGlobal.m_pCurrDataset, * rdrGlobal.m_pSecondaryDataset );

    SetCurMissedRgn(iPatch);

    // TODO: Kevin Kreeger: Somehow here only calling this function updates marking
    AfxGetMainWnd()->SendMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
    AfxGetMainWnd()->SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

    // stop everything else and update
    if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
    {
      ColonViatronixWnd::ToggleAutoflyMode();
    }
    else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
    {
      ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
      AfxGetMainWnd()->SendMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
    }
  }// endif valid patches yet


  *pResult = 0;

  return FALSE;
} // OnItemChanged()


/**
 * Timer events
 */
void MissedRegionsListBox::OnTimer(UINT_PTR uIDEvent)
{
  if (uIDEvent == m_timerUpdate)
  {
    if ( m_pCurrDataset != rdrGlobal.m_pCurrDataset )
      UpdateList();
  }

} // OnTimer()


/**
 * Window message processing
 *
 * @param uMessage   the message
 * @param wParam     message parameter
 * @param lParam     message parameter
 * @return           message-dependent return value
 */
LRESULT MissedRegionsListBox::WindowProc(UINT_PTR uMessage, WPARAM wParam, LPARAM lParam)
{
  // TODO: Add your specialized code here and/or call the base class
  switch (uMessage)
  {
  case WM_HSCROLL:
    if (LOWORD(wParam) != SB_ENDSCROLL)
      PostMessage(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL,0), LPARAM(GetScrollBarCtrl(SB_HORZ)));
    break;
  case WM_VSCROLL:
    if (LOWORD(wParam) != SB_ENDSCROLL)
      PostMessage(WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL,0), LPARAM(GetScrollBarCtrl(SB_VERT)));
    break;
  }

  return CListCtrl::WindowProc(uMessage, wParam, lParam);
} // WindowProc()


/**
 * Detect missed regions button clicked
 */
void MissedRegionsListBox::OnDetectMissedRgnsBtn() 
{
  try
  {
    // Check for greater than 90% to allow this to compute (otherwise
    // it would take too long to analyze really large patches). The
    // number 0.895 represents 89.5% and it is utilized because
    // the percent printed in the UI is rounded to no decimal places.
    //if( rdrGlobal.m_pCurrDataset->GetPercentExamined() > 0.895F )
    {
      rdrGlobal.m_pCurrDataset->m_fields.m_paintVol.Transfer3DMarking();

      CWaitCursor waitCrsr;
      AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT, IDS_DETECTING_MISSED_REGIONS, 0);
      rdrGlobal.m_pCurrDataset->m_fields.m_paintVol.CreateMissedPatchList(*(rdrGlobal.m_pCurrDataset));
      AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT, 0, 0);

      UpdateList();

      AfxGetMainWnd()->SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_OVERVIEW), 0);

      // if the system is configured to export the listbox data to a file
      const bool bExportDataToFile( rdrGlobal.GetSettings()->GetBoolValue( VCSettings::ViewerXML(), VCSettings::AllowDataExport() ) );

      // output the missed regions to disk, if data export is enabled
      if ( bExportDataToFile )
      {

        // the output stream
        std::ofstream dataOutput( "C:\\MissedRegionsData.csv", std::ios::app );

        // output the header
        if ( rdrGlobal.m_pCurrDataset->m_vPatchList.size() > 0 )
        {
          dataOutput << std::endl << "Missed Regions" << std::endl;
          dataOutput << "#, Distance from Rectum (cm), Size (mm^2)" << std::endl;
        }

        // loop over all the patches
        CString str;
        for (int4 iCt(0); iCt < rdrGlobal.m_pCurrDataset->m_vPatchList.size(); iCt++)
        {
          // Count
          str.Format("%3d", iCt+1);
          dataOutput << (LPCSTR) str << ", ";

          // Distance from rectum (computed in mm, displayed in cm, hence division by 10)
          str.Format("%5.1f", rdrGlobal.m_pCurrDataset->m_vPatchList[iCt].m_fDistanceFromRectumMM / 10.0F);
          dataOutput << (LPCSTR) str << ", ";

          // Size
          str.Format("%d", rdrGlobal.m_pCurrDataset->m_vPatchList[iCt].m_uSurfaceCount);
          dataOutput << (LPCSTR) str << std::endl;
        }

        dataOutput.close();
      }
    }
  }
  catch ( ... )
  {
    AfxMessageBox( "Unhandled exception in OnAppDetectMissedRegions", MB_ICONERROR );
    LogErr( "unhandled exception", "ColonViatronixWnd", "OnAppDetectMissedRegions" );
  }

} // OnDetectMissedRgnsBtn()


/**
 * Next missed regions button clicked
 */
void MissedRegionsListBox::OnNextMissedRgnsBtn()
{
  if ( rdrGlobal.m_pCurrDataset->m_vPatchList.size() > 0 )
  {
    const int4 iPatch((GetCurMissedRgn() + 1) % rdrGlobal.m_pCurrDataset->m_vPatchList.size());
    SetCurMissedRgn(iPatch);

    // jump to current patch
    Patch & patch = rdrGlobal.m_pCurrDataset->m_vPatchList[iPatch];
    if (patch.m_iSegment > -1)
    {
      rdrGlobal.m_pCurrDataset->m_segmentList.SetCurrentSegmentIndex(patch.m_iSegment);
    }

    Submarine & submarine = rdrGlobal.m_pCurrDataset->m_submarine;
    submarine.FullStop();
    submarine.ExitReplayMode();
    submarine.SetPositionWorld(patch.m_viewPointWorld);
    submarine.SetUpDirection(Normal<float4>(0,1,0));
    submarine.SetOrientation(Normal<float4>(patch.m_vViewDirWorld));
    submarine.EnforceOrthogonality();
    submarine.UpdatePosition();

    // update linked views too
    rdrGlobal.UpdateWorldPosition( * rdrGlobal.m_pCurrDataset, * rdrGlobal.m_pSecondaryDataset );

    // TODO: While here calling this method is sufficient to update marking.  Why is that?
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
    AfxGetMainWnd()->SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

    // stop everything else and update
    if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
    {
      ColonViatronixWnd::ToggleAutoflyMode();
    }
    else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
    {
      ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
      AfxGetMainWnd()->SendMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
    }
  }
} // OnNextMissedRgnsBtn()


///////////////////////////////////////////////////////////////////////////////////////////
// MissedRegionsDlg dialog

IMPLEMENT_DYNAMIC(MissedRegionsDlg, ViatronDialog)

/**
 * message mapping
 */
BEGIN_MESSAGE_MAP(MissedRegionsDlg, ViatronDialog)
  ON_BN_CLICKED(IDC_CDB_DETECTMISSEDRGNS_BTN, OnDetectMissedRgnsBtn)
  ON_BN_CLICKED(IDC_CDB_NEXTMISSEDRGN_BTN, OnNextMissedRgnsBtn)
  ON_WM_SIZE()
END_MESSAGE_MAP()


/**
 * Constructor
 */
MissedRegionsDlg::MissedRegionsDlg(CWnd* pParent /*=NULL*/)
	: ViatronDialog(MissedRegionsDlg::IDD, pParent, IDB_ENDOVIEW_HORIZ_DLGBAR)
{

} // constructor


/**
 * Destructor
 */
MissedRegionsDlg::~MissedRegionsDlg()
{
} // destructor


/**
 * dialog initialization
 * @return BOOL  Create success
 */
BOOL MissedRegionsDlg::OnInitDialog()
{
  ViatronDialog::OnInitDialog();
  m_missedRegionList.Initialize();

  // {{ Set button icons
  const uint4 vuBtnsNicons[][2] =
  {
    // Missed region buttons
    { IDC_CDB_DETECTMISSEDRGNS_BTN, IDB_MISSEDREGIONS_DETECT },
    { IDC_CDB_NEXTMISSEDRGN_BTN, IDB_MISSEDREGIONS_NEXT },
  };
  SetButtonIcons(this, vuBtnsNicons, sizeof(vuBtnsNicons)/(sizeof(uint4[2])));

  const BOOL bTipsEnabled(EnableToolTips(TRUE));

// TODO: for OnSize() method
//   GetDlgItem( IDC_CDB_MISSEDRGN_LIST )->SetWindowPos( NULL, iGapH, iListY, iListWidth, iListHeight, SWP_NOZORDER );
//   m_missedRegionList.SetColumnWidth(0, int4(iListWidth * 0.12 ));
//   m_missedRegionList.SetColumnWidth(1, int4(iListWidth * 0.42 ));
//   m_missedRegionList.SetColumnWidth(2, int4(iListWidth * 0.40 ));

  return TRUE;
} // OnInitDialog()


/**
 * Initialize child control variables
 * 
 * @param CDataExchange* pDX   
 */
void MissedRegionsDlg::DoDataExchange(CDataExchange* pDX)
{
  ViatronDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CDB_MISSEDRGN_LIST, m_missedRegionList);
  DDX_Control(pDX, IDC_CDB_NEXTMISSEDRGN_BTN, m_nextMissedBtn);
  DDX_Control(pDX, IDC_CDB_DETECTMISSEDRGNS_BTN, m_detectMissedBtn);
} // DoDataExchange(CDataExchange* pDX)


/**
 * Empty override so that user cannot dismiss dialog
 */
void MissedRegionsDlg::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
} // OnOK()


/**
 * Empty override so that user cannot dismiss dialog
 */
void MissedRegionsDlg::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * update the missed regions
 */
void MissedRegionsDlg::UpdateState()
{

  // Missed Regions List
  if (m_eCurrOrient != rdrGlobal.m_pCurrDataset->GetOrientation())
  {
    m_missedRegionList.UpdateList();
    m_eCurrOrient = rdrGlobal.m_pCurrDataset->GetOrientation();
  }
  // (end) Missed Regions

  // Missed Regions Buttons
  if (rdrGlobal.m_pCurrDataset->GetPercentExamined() >= 0.0)//0.895F)
  {
    if (!m_detectMissedBtn.IsWindowEnabled())
    {
      m_detectMissedBtn.EnableWindow(TRUE);
    }

    if (m_missedRegionList.GetItemCount() > 0)
    {
      if (!m_nextMissedBtn.IsWindowEnabled())
      {
        m_nextMissedBtn.EnableWindow(TRUE);
      }
    }
    else if (m_nextMissedBtn.IsWindowEnabled())
    {
      m_nextMissedBtn.EnableWindow(FALSE);
    }
  }
  else
  {
    if (m_detectMissedBtn.IsWindowEnabled())
    {
      m_detectMissedBtn.EnableWindow(FALSE);
    }

    if (m_nextMissedBtn.IsWindowEnabled())
    {
      m_nextMissedBtn.EnableWindow(FALSE);
    }
  }
  // (end) Missed Regions Buttons

} // UpdateState()


/**
 * Calculate the missed regions
 */
void MissedRegionsDlg::OnDetectMissedRgnsBtn()
{
  m_missedRegionList.OnDetectMissedRgnsBtn();
} // OnDetectMissedRgnsBtn()


/**
 * Jump to the next missed region
 */
void MissedRegionsDlg::OnNextMissedRgnsBtn()
{
  m_missedRegionList.OnNextMissedRgnsBtn();
} // OnNextMissedRgnsBtn()


/**
 * Window resize handler
 *
 * @param uType   Type of resize event
 * @param iCX     New width of window
 * @param iCY     New height of window
 */
void MissedRegionsDlg::OnSize(UINT uType, int iCX, int iCY)
{
  ViatronDialog::OnSize(uType, iCX, iCY);
  CRect rectWin(WinUtils::GetClientRect(this));
 
  if (IsWindow(m_missedRegionList) && IsWindow(m_nextMissedBtn) && IsWindow(m_detectMissedBtn) )
  {
    CRect rectList(WinUtils::GetChildRect(m_missedRegionList));
    CRect rectMissedBtn(WinUtils::GetChildRect(m_nextMissedBtn));
    CRect rectDetectBtn(WinUtils::GetChildRect(m_detectMissedBtn));

    rectMissedBtn.MoveToX(iCX - rectMissedBtn.Width() - 10);
    rectDetectBtn.MoveToX(iCX - rectDetectBtn.Width() - 10);

    rectList.bottom = iCY - 10 - rectList.top;
    rectList.right = rectMissedBtn.left - 10;
    m_missedRegionList.SetWindowPos(NULL,0,0, rectList.Width(), rectList.Height(), SWP_NOMOVE | SWP_NOZORDER);

    m_nextMissedBtn.SetWindowPos(NULL, rectMissedBtn.left,rectMissedBtn.top, 0,0, SWP_NOSIZE | SWP_NOZORDER);
    m_detectMissedBtn.SetWindowPos(NULL, rectDetectBtn.left,rectDetectBtn.top, 0,0, SWP_NOSIZE | SWP_NOZORDER);
  }

} // OnSize()


// Revision History:
// $Log: MissedRegionsDlg.C,v $
// Revision 1.1.2.5  2010/01/31 02:07:14  dongqing
// make sure dialog doesn't disappear
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.4  2010/01/18 11:01:23  dongqing
// 1280x1024 provisions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.3  2010/01/17 18:29:16  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.2  2009/12/17 16:16:38  dongqing
// tooltips
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MissedRegionsDlg.C,v 1.1.2.5 2010/01/31 02:07:14 dongqing Exp $
// $Id: MissedRegionsDlg.C,v 1.1.2.5 2010/01/31 02:07:14 dongqing Exp $
