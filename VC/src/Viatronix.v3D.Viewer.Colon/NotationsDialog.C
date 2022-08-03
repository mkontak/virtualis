// $Id: NotationsDialog.C,v 1.7.2.4 2010/04/11 11:40:22 dongqing Exp $
//
// Copyright © 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
//#include "WinUtils.h"
//#include "GLChildView.h"
//#include "V1KMessages.h"
#include "ReaderGlobal.h"
#include "ViatronStudy.h"

#include "NotationsDialog.h"


// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// namespaces
using namespace ColonViews::ChildViews;
using namespace ColonViews::Dialogs;
using namespace WinUtils;
using namespace ReaderLib;


/**
 * Constructor 
 *
 * @param pParent   Parent window.
 */
NotationsDialog::NotationsDialog(CWnd* pParent /*=NULL*/)
  : ViatronDialog(NotationsDialog::IDD, pParent, IDB_PATIENTINFO_VERT_BKGD), m_iUpdateDlgTimerEvent(1)
{
  //{{AFX_DATA_INIT(NotationsDialog)
  m_iListSel = 0;
  //}}AFX_DATA_INIT
}


/**
 * Dialog data/member initialize
 *
 * @param pDX   Pointer to dialog data exchange instance
 */
void NotationsDialog::DoDataExchange( CDataExchange * pDX )
{
  ViatronDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_NTD_MEASUREMENTS_RADIO, m_btnMeasurements);
  DDX_Control(pDX, IDC_NTD_ANNOTATIONS_RADIO, m_btnAnnotations);
  DDX_Control(pDX, IDC_NTD_BOOKMARKS_RADIO, m_btnBookmarks);
  DDX_Control(pDX, IDC_NTD_DELETEALL_BUTTON, m_btnDeleteAll);
  DDX_Control(pDX, IDC_NTD_SUPINE_COMBO, m_cbxSupine);
  DDX_Control(pDX, IDC_NTD_PRONE_COMBO, m_cbxProne);
  DDX_Control(pDX, IDC_NTD_SUPINE_DELETE_BUTTON, m_btnSupineDelete);
  DDX_Control(pDX, IDC_NTD_PRONE_DELETE_BUTTON, m_btnProneDelete);
  SkinAutoRadioButton::DDX_Radio( pDX, IDC_NTD_BOOKMARKS_RADIO, m_iListSel );
}


BEGIN_MESSAGE_MAP(NotationsDialog, ViatronDialog)
  //{{AFX_MSG_MAP(NotationsDialog)
  ON_CBN_SELENDOK(IDC_NTD_SUPINE_COMBO, OnSelChangeSupineCombo)
  ON_CBN_SELENDOK(IDC_NTD_PRONE_COMBO, OnSelChangeProneCombo)
  ON_CBN_DROPDOWN(IDC_NTD_SUPINE_COMBO, OnDropdownSupineCombo)
  ON_CBN_DROPDOWN(IDC_NTD_PRONE_COMBO, OnDropdownProneCombo)
  ON_BN_CLICKED(IDC_NTD_SUPINE_DELETE_BUTTON, OnClickedSupineDelete)
  ON_BN_CLICKED(IDC_NTD_PRONE_DELETE_BUTTON, OnClickedProneDelete)
  ON_BN_CLICKED(IDC_NTD_DELETEALL_BUTTON, OnDeleteAll)
  ON_WM_CTLCOLOR()
  ON_WM_TIMER()
  ON_CBN_SELENDCANCEL(IDC_NTD_SUPINE_COMBO, OnCloseupCombos)
  ON_CBN_SELENDCANCEL(IDC_NTD_PRONE_COMBO, OnCloseupCombos)
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NotationsDialog message handlers


/**
 * Initialize dialog
 *
 * @return BOOL   FALSE if focus set to a control, TRUE otherwise
 */
BOOL NotationsDialog::OnInitDialog() 
{
  ViatronDialog::OnInitDialog();
  EnableToolTips(TRUE);
  SendMessage(WM_COMMAND, MAKEWPARAM(IDC_NTD_BOOKMARKS_RADIO+m_iListSel, BN_CLICKED), 0);

  m_btnBookmarks.ModifyStyle(0, BS_OWNERDRAW);
  SetButtonIcon(m_btnBookmarks, IDB_BOOKMARKS_BUTTON_SMALL);
  m_btnAnnotations.ModifyStyle(0, BS_OWNERDRAW);
  SetButtonIcon(m_btnAnnotations, IDB_ANNOTATIONS_BUTTON_SMALL);
  m_btnMeasurements.ModifyStyle(0, BS_OWNERDRAW);
  SetButtonIcon(m_btnMeasurements, IDB_MEASUREMENTS_BUTTON_SMALL);

  SetButtonIcon(m_btnDeleteAll, IDB_DELETEALL_BUTTON);

  // Periodically enable/disable controls based on availability of notations
  m_iUpdateDlgTimerEvent = SetTimer(m_iUpdateDlgTimerEvent, 2000, NULL);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog()


/**
 * Menu command handler.
 *
 * @param wParam   Identifies menu command.
 * @param lParam   Unused.
 * @return BOOL    TRUE if command handled, FALSE (for default handler) otherwise.
 */
BOOL NotationsDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  switch (LOWORD(wParam))
  {
    case IDC_NTD_BOOKMARKS_RADIO:
      UpdateData(TRUE);
      UpdateComboList(m_cbxSupine, rdrGlobal.m_supine.m_session.m_bookmarkList);
      UpdateComboList(m_cbxProne, rdrGlobal.m_prone.m_session.m_bookmarkList);
      break;

    case IDC_NTD_ANNOTATIONS_RADIO:
      UpdateData(TRUE);
      UpdateComboList(m_cbxSupine, rdrGlobal.m_supine.m_session.m_2DAnnotationList);
      UpdateComboList(m_cbxProne, rdrGlobal.m_prone.m_session.m_2DAnnotationList);
      break;

    case IDC_NTD_MEASUREMENTS_RADIO:
    {
      UpdateData(TRUE);

      std::list<std::string> itemList;
      ReaderLib::Dataset * vpDataset[2] = { &rdrGlobal.m_supine, &rdrGlobal.m_prone };
      CComboBox * vpCombos[2] = { &m_cbxSupine, &m_cbxProne };

      for ( int4 iDataset(0); iDataset < 2; iDataset++ )
      {
        itemList.clear();
        for (MeasureLineList::const_iterator itLine = vpDataset[iDataset]->m_session.m_measureLineList.begin();
          itLine != vpDataset[iDataset]->m_session.m_measureLineList.end(); ++itLine)
        {
          Box<float4> lineBox(itLine->m_measureEnds.m_start, itLine->m_measureEnds.m_end);
          itemList.push_back(std::string(LPCTSTR(LoadResourceString(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE))) + ToAscii(": ") + std::string(*itLine));
        }

        for (int4 iSRTCt(0); iSRTCt < vpDataset[iDataset]->m_suspiciousRegions.GetCount(); iSRTCt++)
        {
          vxCAD::vxCadFinding rFinding = vpDataset[iDataset]->m_suspiciousRegions.GetFinding(iSRTCt);
          itemList.push_back(std::string(LPCTSTR(LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN))) + ToAscii(" ") + ToAscii(iSRTCt+1));
        }

        UpdateComboList(*(vpCombos[iDataset]), itemList);
      }
    }
    break;

    default:
      return ViatronDialog::OnCommand(wParam, lParam);
  }

  OnTimer(m_iUpdateDlgTimerEvent); // update control visibility

  return ViatronDialog::OnCommand(wParam, lParam);
} // OnCommand()


/**
 * Selection made from supine combobox.
 */
void NotationsDialog::OnSelChangeSupineCombo()
{
  const int4 iSelection(m_cbxSupine.GetCurSel() - eComboCountMin);
  if (iSelection < 0)
  {
    return;
  }

  switch (m_iListSel)
  {
    case eRadio_Annotations:
      if (m_btnSupineDelete.GetCheck() != TRUE)
      {
        rdrGlobal.m_supine.m_session.m_2DAnnotationList.StopAtBookmark(iSelection, rdrGlobal.m_supine);
        ViatronStudy::TurnOffAutoflight();
      }
      else
      {
        DeleteAnnotation(iSelection, m_cbxSupine, rdrGlobal.m_supine.m_session.m_2DAnnotationList, false);
      }
      break;
    case eRadio_Measurements:
      if (m_btnSupineDelete.GetCheck() != TRUE)
      {
        rdrGlobal.SetDatasetOrientation(rdrGlobal.m_supine.GetOrientation());
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_GOTO_3DMEASUREMENTS_FIRST + iSelection, 0), 0);
      }
      else
      {
        if ( iSelection < rdrGlobal.m_supine.m_session.m_measureLineList.size() )
          DeleteMeasurement( iSelection, rdrGlobal.m_supine.m_session.m_measureLineList );
        else
        {
          CString sPromptString;
          sPromptString.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE),
            LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN));
          if (AfxMessageBox(sPromptString, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
          {
            rdrGlobal.m_supine.m_suspiciousRegions.RemoveFinding(rdrGlobal.m_supine,
              iSelection - rdrGlobal.m_supine.m_session.m_measureLineList.size());
          }
        }
      }
      break;
    default:
    case eRadio_Bookmarks:
      if (m_btnSupineDelete.GetCheck() != TRUE)
      {
        rdrGlobal.m_supine.m_session.m_bookmarkList.StopAtBookmark(iSelection, rdrGlobal.m_supine);
        ViatronStudy::TurnOffAutoflight();
      }
      else
      {
        DeleteAnnotation(iSelection, m_cbxSupine, rdrGlobal.m_supine.m_session.m_bookmarkList, true);
      }
      break;
  }

  OnTimer(m_iUpdateDlgTimerEvent); // update control visibility

  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);

  m_cbxSupine.SetCurSel(0);
  m_btnSupineDelete.SetCheck(0);

} // OnSelChangeSupineCombo()


/**
 * Selection made from prone combobox.
 */
void NotationsDialog::OnSelChangeProneCombo() 
{
  const int4 iSelection(m_cbxProne.GetCurSel() - eComboCountMin);
  if (iSelection < 0)
  {
    return;
  }

  switch (m_iListSel)
  {
    case eRadio_Annotations:
      if (m_btnProneDelete.GetCheck() != TRUE)
      {
        rdrGlobal.m_prone.m_session.m_2DAnnotationList.StopAtBookmark(iSelection, rdrGlobal.m_prone);
        ViatronStudy::TurnOffAutoflight();
      }
      else
      {
        DeleteAnnotation(iSelection, m_cbxProne, rdrGlobal.m_prone.m_session.m_2DAnnotationList, false);
      }
      break;
    case eRadio_Measurements:
      if (m_btnProneDelete.GetCheck() != TRUE)
      {
        rdrGlobal.SetDatasetOrientation(rdrGlobal.m_prone.GetOrientation());
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_GOTO_3DMEASUREMENTS_FIRST + iSelection, 0), 0);
      }
      else
      {
        if ( iSelection < rdrGlobal.m_prone.m_session.m_measureLineList.size() )
          DeleteMeasurement( iSelection, rdrGlobal.m_prone.m_session.m_measureLineList );
        else
        {
          CString sPromptString;
          sPromptString.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE),
            LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN));
          if (AfxMessageBox(sPromptString, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
          {
            rdrGlobal.m_prone.m_suspiciousRegions.RemoveFinding(rdrGlobal.m_prone,
              iSelection - rdrGlobal.m_prone.m_session.m_measureLineList.size());
          }
        }
      }
      break;
    default:
    case eRadio_Bookmarks:
      if (m_btnProneDelete.GetCheck() != TRUE)
      {
        rdrGlobal.m_prone.m_session.m_bookmarkList.StopAtBookmark(iSelection, rdrGlobal.m_prone);
        ViatronStudy::TurnOffAutoflight();
      }
      else
      {
        DeleteAnnotation(iSelection, m_cbxProne, rdrGlobal.m_prone.m_session.m_bookmarkList, true);
      }
      break;
  }

  OnTimer(m_iUpdateDlgTimerEvent); // update control visibility

  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);

  m_cbxProne.SetCurSel(0);
  m_btnProneDelete.SetCheck(0);

} // OnSelChangeProneCombo()


/**
 * Deletes the selected measurement
 *
 * @param iSelection      the measurement to delete
 * @param measureLineList the list to delete from
 */
void NotationsDialog::DeleteMeasurement( const int4 iSelection, MeasureLineList & measureLineList )
{

  // find the measurement
  MeasureLineList::iterator iter = measureLineList.begin();
  std::advance( iter, iSelection );

  CString sMsg;
  sMsg.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
    LoadResourceString(IDS_DELETE), LoadResourceString(IDS_MEASURE) + '<'
    + static_cast< std::string >( * iter ).c_str() + '>');

  // consent with the user
  if (AfxMessageBox(sMsg, MB_ICONQUESTION | MB_YESNO) == IDYES)
  {

    // delete!
    measureLineList.erase( iter );

    // update everything
    AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
    Invalidate(FALSE);

  } // if user consents

} // DeleteMeasurement()


/**
 * Refresh combobox when dropdown list is opened.
 */
void NotationsDialog::OnDropdownSupineCombo() 
{
  UpdateData(TRUE);
  switch (m_iListSel)
  {
    case eRadio_Annotations:
      UpdateComboList(m_cbxSupine, rdrGlobal.m_supine.m_session.m_2DAnnotationList);
      break;

    case eRadio_Measurements:
    {
      std::list<std::string> itemList;
      ReaderLib::Dataset * vpDataset[2] = { &rdrGlobal.m_supine, &rdrGlobal.m_prone };
      CComboBox * vpCombos[2] = { &m_cbxSupine, &m_cbxProne };

      for ( int4 iDataset(0); iDataset < 1; iDataset++ )
      {
        itemList.clear();
        for (MeasureLineList::const_iterator itLine = vpDataset[iDataset]->m_session.m_measureLineList.begin();
          itLine != vpDataset[iDataset]->m_session.m_measureLineList.end(); ++itLine)
        {
          Box<float4> lineBox(itLine->m_measureEnds.m_start, itLine->m_measureEnds.m_end);
          itemList.push_front(std::string(LPCTSTR(LoadResourceString(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE))) + ToAscii(": ") + std::string(*itLine));
        }

        for (int4 iSRTCt(0); iSRTCt < vpDataset[iDataset]->m_suspiciousRegions.GetCount(); iSRTCt++)
        {
          vxCAD::vxCadFinding rFinding = vpDataset[iDataset]->m_suspiciousRegions.GetFinding(iSRTCt);
          itemList.push_back(std::string(LPCTSTR(LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN))) + ToAscii(" ") + ToAscii(iSRTCt+1));
        }

        UpdateComboList(*(vpCombos[iDataset]), itemList);
      }
      break;
    }

    default:
    case eRadio_Bookmarks:
      UpdateComboList(m_cbxSupine, rdrGlobal.m_supine.m_session.m_bookmarkList);
      break;
  }

  m_btnDeleteAll.EnableWindow((m_cbxSupine.GetCount()>eComboCountMin) || (m_cbxProne.GetCount()>eComboCountMin));
} // OnDropdownSupineCombo()


/**
 * Refresh combobox when dropdown list is opened.
 */
void NotationsDialog::OnDropdownProneCombo() 
{
  UpdateData(TRUE);
  switch (m_iListSel)
  {
    case eRadio_Annotations:
      UpdateComboList(m_cbxProne, rdrGlobal.m_prone.m_session.m_2DAnnotationList);
      break;

    case eRadio_Measurements:
    {
      std::list<std::string> itemList;
      ReaderLib::Dataset * vpDataset[2] = { &rdrGlobal.m_supine, &rdrGlobal.m_prone };
      CComboBox * vpCombos[2] = { &m_cbxSupine, &m_cbxProne };

      for ( int4 iDataset(1); iDataset < 2; iDataset++ )
      {
        itemList.clear();
        for (MeasureLineList::const_iterator itLine = vpDataset[iDataset]->m_session.m_measureLineList.begin();
          itLine != vpDataset[iDataset]->m_session.m_measureLineList.end(); ++itLine)
        {
          Box<float4> lineBox(itLine->m_measureEnds.m_start, itLine->m_measureEnds.m_end);
          itemList.push_front(std::string(LPCTSTR(LoadResourceString(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE))) + ToAscii(": ") + std::string(*itLine));
        }

        for (int4 iSRTCt(0); iSRTCt < vpDataset[iDataset]->m_suspiciousRegions.GetCount(); iSRTCt++)
        {
          vxCAD::vxCadFinding rFinding = vpDataset[iDataset]->m_suspiciousRegions.GetFinding(iSRTCt);
          itemList.push_back(std::string(LPCTSTR(LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN))) + ToAscii(" ") + ToAscii(iSRTCt+1));
        }

        UpdateComboList(*(vpCombos[iDataset]), itemList);
      }
      break;
    }

    default:
    case eRadio_Bookmarks:
      UpdateComboList(m_cbxProne, rdrGlobal.m_prone.m_session.m_bookmarkList);
      break;
  }

  m_btnDeleteAll.EnableWindow((m_cbxSupine.GetCount()>eComboCountMin) || (m_cbxProne.GetCount()>eComboCountMin));
} // OnDropdownProneCombo()


/**
 * Called when delete supine notation button is clicked
 */
void NotationsDialog::OnClickedSupineDelete()
{
  m_btnSupineDelete.SetCheck(1);
  m_cbxSupine.ShowDropDown(TRUE);
} // OnClickedSupineDelete()


/**
 * Called when delete prone notation button is clicked
 */
void NotationsDialog::OnClickedProneDelete()
{
  m_btnProneDelete.SetCheck(1);
  m_cbxProne.ShowDropDown(TRUE);
} // OnClickedProneDelete()


/**
 * Deletes an item.
 *
 * @param bBookmark true if a bookmark, false if an annotation
 */
void NotationsDialog::DeleteAnnotation(const int4 iDel, const CComboBox& combo, BookmarkList& theList, const bool bBookmark)
{
  CString sPromptString;
  if (iDel < 0)
  {
    return;
  }

  if ( bBookmark )
  {
    sPromptString.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE),
      LoadResourceString(IDS_BOOKMARK) + '<'
      + theList[iDel].m_sName.c_str() + '>');
  }
  else
  {
    sPromptString.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE),
      LoadResourceString(IDS_ANNOTATION) + '<'
      + theList[iDel].m_sName.c_str() + '>');
  }

  if (AfxMessageBox(sPromptString, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES)
  {
    return;
  }

  theList.DeleteBookmark(iDel);
  OnDropdownProneCombo();
  OnTimer(m_iUpdateDlgTimerEvent);
} // DeleteAnnotation()


/**
 * Called when either combobox's list is hidden
 */
void NotationsDialog::OnCloseupCombos() 
{
  m_btnSupineDelete.SetCheck(0);
  m_btnProneDelete.SetCheck(0);
} // OnCloseupCombos()


/**
 * Deletes all items in current list.
 */
void NotationsDialog::OnDeleteAll()
{
  int4 iDelCommand(0);
  switch (m_iListSel)
  {
    case eRadio_Annotations:
      iDelCommand = IDM_NOTATIONS_ANNOTATIONS_DELETEALLANNOTATIONS;
      break;
    case eRadio_Measurements:
      iDelCommand = IDM_MEASUREMENTS_DELETEALL;
      break;
    default:
    case eRadio_Bookmarks:
      iDelCommand = IDM_BOOKMARKS_DELETEALLBOOKMARKS;
      break;
  }

  AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(iDelCommand, 0), 0);
  SendMessage(WM_COMMAND, MAKEWPARAM(IDC_NTD_BOOKMARKS_RADIO+m_iListSel, BN_CLICKED), 0);
  OnTimer(m_iUpdateDlgTimerEvent); // update control visibility
} // OnDeleteAll()


/**
 * Refreshes the combobox's list with the given list items.
 *
 * @param combo     Combobox.
 * @param theList   List items.
 */
template<class Type>
void NotationsDialog::UpdateComboList(CComboBox& combo, const std::list<Type>& theList)
{
  combo.ResetContent();

  uint4 uTitleIDS(0);
  switch (m_iListSel)
  {
    case eRadio_Annotations:
      uTitleIDS = IDS_ANNOTATION;
      break;
    case eRadio_Measurements:
      uTitleIDS = IDS_MEASURE;
      break;
    default:
    case eRadio_Bookmarks:
      uTitleIDS = IDS_BOOKMARK;
  }

  CString sPrompt;
  sPrompt.Format(IDS_SELECT_FROM_LIST_FORMAT, LoadResourceString(uTitleIDS));
  combo.AddString(sPrompt);

  combo.SetCurSel(0);
  for (std::list<Type>::const_iterator it = theList.begin(); it != theList.end(); it++)
  {
    combo.AddString(std::string(*it).c_str());
  }

  combo.EnableWindow(combo.GetCount() > eComboCountMin);
} // UpdateComboList()


/**
 * Colors dialog's controls.
 *
 * @param pDC         Device context for painting controls.
 * @param pWnd        Handle to the control being drawn.
 * @param uCtlColor   Identifies the type of control being drawn.
 * @return HBRUSH     Brush to paint control.
 */
HBRUSH NotationsDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 
{
  // The listboxes of the combo controls will wrap the dialog's background, producing an undesirable look
  HBRUSH hbr = (uCtlColor == CTLCOLOR_LISTBOX)
    ? CDialog::OnCtlColor(pDC, pWnd, uCtlColor)
    : ViatronDialog::OnCtlColor(pDC, pWnd, uCtlColor);

  return hbr;
} // OnCtlColor()


/**
 * WM_TIMER handler.
 *
 * @param uIDEvent   ID of timer event.
 */
void NotationsDialog::OnTimer(UINT_PTR uIDEvent) 
{
  // The sole timer event, to ensure controls are enabled (disabled) for non-empty (empty) lists
  if (m_iUpdateDlgTimerEvent)
  {
    bool bAnySupine(false), bAnyProne(false);
    switch (m_iListSel)
    {
      case eRadio_Annotations:
      {
        m_cbxSupine.EnableWindow(bAnySupine = !rdrGlobal.m_supine.m_session.m_2DAnnotationList.empty());
        m_cbxProne .EnableWindow(bAnyProne  = !rdrGlobal.m_prone .m_session.m_2DAnnotationList.empty());
        break;
      }
      case eRadio_Measurements:
      {
        m_cbxSupine.EnableWindow(bAnySupine
          = (!rdrGlobal.m_supine.m_session.m_measureLineList.empty() || !rdrGlobal.m_supine.m_suspiciousRegions.IsEmpty()) );
        m_cbxProne .EnableWindow(bAnyProne
          = (!rdrGlobal.m_prone .m_session.m_measureLineList.empty() || !rdrGlobal.m_prone.m_suspiciousRegions.IsEmpty()) );
        break;
      }

      default:
      case eRadio_Bookmarks:
      {
        m_cbxSupine.EnableWindow(bAnySupine = !rdrGlobal.m_supine.m_session.m_bookmarkList.empty());
        m_cbxProne .EnableWindow(bAnyProne  = !rdrGlobal.m_prone .m_session.m_bookmarkList.empty());
        break;
      }
    }

    // enable/disable delete buttons
    const bool bAnyAtAll = bAnySupine || bAnyProne;
    if ( static_cast< bool >( m_btnDeleteAll   .IsWindowEnabled() ) != bAnyAtAll  ) m_btnDeleteAll   .EnableWindow( bAnyAtAll  );
    if ( static_cast< bool >( m_btnSupineDelete.IsWindowEnabled() ) != bAnySupine ) m_btnSupineDelete.EnableWindow( bAnySupine );
    if ( static_cast< bool >( m_btnProneDelete .IsWindowEnabled() ) != bAnyProne  ) m_btnProneDelete .EnableWindow( bAnyProne  );

  }

  ViatronDialog::OnTimer(uIDEvent);
} // OnTimer()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void NotationsDialog::OnSize( UINT uType, int iCX, int iCY )
{
  ViatronDialog::OnSize(uType, iCX, iCY);

  if ( iCX == 0 || iCY == 0 )
  {
    return;
  }

  CenterChildControls(IDC_CONTAINER_GROUPBOX);

} // OnSize()


// $Log: NotationsDialog.C,v $
// Revision 1.7.2.4  2010/04/11 11:40:22  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.3  2009/12/17 16:17:08  dongqing
// comment
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.2  2009/01/31 00:38:35  jmeade
// Issue 6237: volume measurements in the notations pane.
//
// Revision 1.7.2.1  2008/11/13 02:17:18  jmeade
// fix bug where go-to-bookmark functionality failed when prone loaded as 1st dataset.
//
// Revision 1.7  2007/03/08 22:22:12  romy
// code review - header files adjusted
//
// Revision 1.6  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.5  2006/01/31 14:26:44  frank
// code review
//
// Revision 1.4.4.1  2006/02/28 17:25:32  geconomos
// formatting
//
// Revision 1.4  2005/09/13 13:06:21  geconomos
// Making calls to ViatronStudy::StopAutoFlight()
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
// Revision 1.1.2.12.2.2  2005/03/26 03:15:23  frank
// Issue #4063: added measurement deletion, fixed button enabling
//
// Revision 1.1.2.12.2.1  2005/03/25 13:58:51  frank
// Issue #3990: Fixed bookmark and annotation jumping with linked datasets
//
// Revision 1.1.2.12  2005/02/15 19:43:21  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.11.2.1  2004/12/06 22:42:25  jmeade
// Stop flight upon jumping to bookmark.
//
// Revision 1.1.2.11  2004/03/22 21:57:45  jmeade
// Issue 3776:  Delete button in notations dialog.
//
// Revision 1.1.2.10  2003/04/14 20:35:02  jmeade
// Issue 2841: Proper enabling of delete all button.
//
// Revision 1.1.2.9  2003/04/14 16:31:38  jmeade
// Issue 2841:  Entries for measurements list.
//
// Revision 1.1.2.8  2003/04/09 16:15:11  jmeade
// Issue 2841: Enable delete-all if *either* list (not both) is non-empty.
//
// Revision 1.1.2.7  2003/03/21 19:27:18  jmeade
// Issue 2841: Ensure via timer that combo enable state always current.
//
// Revision 1.1.2.6  2003/03/12 20:36:24  jmeade
// Issue 2841: Disable combo for empty list.
//
// Revision 1.1.2.5  2003/03/10 21:54:17  jmeade
// Issue 2841: Basic look and functionality implemented.
//
// Revision 1.1.2.4  2003/03/07 02:50:27  jmeade
// Issue 2882: Use default brush to prevent wrapping in background brush for combobox list.
//
// Revision 1.1.2.3  2003/02/28 20:47:14  jmeade
// Comments.
//
// Revision 1.1.2.2  2003/02/27 23:54:49  jmeade
// Issue 2841:  Lists now made functional.
//
// Revision 1.1.2.1  2003/02/27 04:04:19  jmeade
// Issue 2841: Notations pane (annotations and bookmarks).
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NotationsDialog.C,v 1.7.2.4 2010/04/11 11:40:22 dongqing Exp $
// $Id: NotationsDialog.C,v 1.7.2.4 2010/04/11 11:40:22 dongqing Exp $
