// $Id: reportnondistendedareas.c,v 1.13 2007/03/10 03:19:30 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for Non Distended findings
// Author: George Economos

#include "stdafx.h"
#include "viatron.h"
#include "ReportNonDistendedAreas.h"
#include "ReportViewWnd.h"
#include "WinUtils.h"
#include "Overview.h"
#include "readerglobal.h"
#include "ReportVariant.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ReaderLib;
using namespace WinUtils;
using namespace ColonReport;


/**
 * Constructor
 *
 * @param pParent   Parent window.
 */
ReportNonDistendedAreas::ReportNonDistendedAreas(CWnd* pParent) : ReportWndBase(ReportNonDistendedAreas::IDD, pParent)
{
  //{{AFX_DATA_INIT(ReportNonDistendedAreas)
  //}}AFX_DATA_INIT

  // save segment list information
  SegmentList& supSegList = rdrGlobal.m_supine.m_segmentList;
  m_wndOverview.m_vSupineSegmentOrder = supSegList.GetSegmentOrder();
  m_wndOverview.m_iSavedSupineSegment = supSegList.GetSortedSegmentIndexFromIdentifier(supSegList.GetCurrentSegment().GetIdentifier());
  SegmentList& prnSegList = rdrGlobal.m_prone.m_segmentList;
  m_wndOverview.m_vProneSegmentOrder = prnSegList.GetSegmentOrder();
  m_wndOverview.m_iSavedProneSegment = prnSegList.GetSortedSegmentIndexFromIdentifier(prnSegList.GetCurrentSegment().GetIdentifier());
} // constructor



/**
 * Initializes dialog control and data variables.
 *
 * @param pDX   Pointer to CData exchange instance.
 */
void ReportNonDistendedAreas::DoDataExchange(CDataExchange* pDX)
{
  ReportWndBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ReportNonDistendedAreas)
  DDX_Control(pDX, IDC_OVERVIEW_FRAME, m_wndFrame);
  DDX_Control(pDX, IDC_ENTRIES, m_wndEntries);
  DDX_Control(pDX, IDC_GROUP_LOCATION, m_wndGroupLocation);
  //}}AFX_DATA_MAP
} // DoDataExchange


BEGIN_MESSAGE_MAP(ReportNonDistendedAreas, ReportWndBase)
  //{{AFX_MSG_MAP(ReportNonDistendedAreas)
  ON_NOTIFY(LVN_DELETEITEM, IDC_ENTRIES, OnDeleteItem)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_ENTRIES, OnItemChanged)
  ON_COMMAND(IDM_RPT_DELETE_NONDISTENDED, OnRptDeleteNondistended)
  ON_COMMAND(IDM_RPT_DELETE_ALL_NONDISTENDED, OnRptDeleteAllNondistended)
  ON_COMMAND(IDM_RPT_RELOAD_NONDISTENDED, OnRptReloadNondistended)
  ON_COMMAND(IDM_RPT_NONDISTENDED_UNCERTAIN, OnRptNondistendedAreaUncertain)
  ON_NOTIFY(NM_RCLICK, IDC_ENTRIES, OnRclickEntries)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ReportNonDistendedAreas message handlers


/**
 * Called on dialog init.
 *
 * @return   TRUE if focus set to a control.
 */
BOOL ReportNonDistendedAreas::OnInitDialog()
{
  ReportWndBase::OnInitDialog();

  GetDlgItem(IDC_ENTRY_NAME)->SetFont(&m_fontName);

  CRect rcOverview;
  m_wndFrame.GetWindowRect(rcOverview);
  ScreenToClient(rcOverview);

  m_wndOverview.Create(NULL, "", WS_CHILD, rcOverview, this, 1000);
  (dynamic_cast<OverView*>(m_wndOverview.GetOverview()))->SetDisplayDistanceFromRectum(false);
  m_wndOverview.OnInitialized();

  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_CECUM),
    static_cast<DWORD>(ReportLib::Cecum));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ASCENDING),
    static_cast<DWORD>(ReportLib::Ascending));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_HEPATIC_FLEXURE),
    static_cast<DWORD>(ReportLib::HepaticFlexure));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP,LoadResourceString(IDS_TRANSVERSE),
    static_cast<DWORD>(ReportLib::Transverse));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP,LoadResourceString(IDS_SPLENIC_FLEXURE),
    static_cast<DWORD>(ReportLib::SplenicFlexure));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP,LoadResourceString(IDS_DESCENDING),
    static_cast<DWORD>(ReportLib::Descending));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP,LoadResourceString(IDS_SIGMOID),
    static_cast<DWORD>(ReportLib::Sigmoid));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_RECTUM),
    static_cast<DWORD>(ReportLib::Rectum));

  m_wndGroupLocation.SelectItem(static_cast<DWORD>(ReportLib::Cecum));

  m_wndEntries.SetBkColor(m_clrBack);

  return TRUE;
} // OnInitDialog


/**
 * idle routine handler
 */
void ReportNonDistendedAreas::GLIdle()
{
  m_wndOverview.GLRenderView( 0 );
} // GLIdle


/**
 * Sets report revision pointer.
 *
 * @param spRevision   Report revision pointer.
 */
void ReportNonDistendedAreas::SetReport(ReportLib::IReportRevisionPtr& spRevision)
{
  ReportWndBase::SetReport(spRevision);
  LoadReport();
} // SetReport


/**
 * Called when report view is activated.
 */
void ReportNonDistendedAreas::OnReportViewActivate()
{
  if (m_wndOverview.m_bDisplayAllSegments)
  {  rdrGlobal.m_supine.m_segmentList.SelectAllSegments(); }

  if (m_wndOverview.m_bDisplayAllSegments)
  {  rdrGlobal.m_prone.m_segmentList.SelectAllSegments(); }

  m_wndOverview.ShowWindow(rdrGlobal.m_pCurrDataset->m_segmentList.IsLoaded());
  m_wndFrame.ShowWindow(!rdrGlobal.m_pCurrDataset->m_segmentList.IsLoaded());
  if(m_sCurrentEntry.IsEmpty())
  {
    m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000f);
  }
  else
  {
    OnEntryChanged(m_sCurrentEntry);
  }
} // OnReportViewActivate


/**
 * Called when view is deactivated.
 */
void ReportNonDistendedAreas::OnReportViewUnactivate()
{
  OnEntryChanging(m_sCurrentEntry);

  rdrGlobal.m_supine.m_segmentList.SetSegmentOrder(m_wndOverview.m_vSupineSegmentOrder);
  rdrGlobal.m_supine.m_segmentList.SetCurrentSegmentIndex(m_wndOverview.m_iSavedSupineSegment);

  rdrGlobal.m_prone.m_segmentList.SetSegmentOrder(m_wndOverview.m_vProneSegmentOrder);
  rdrGlobal.m_prone.m_segmentList.SetCurrentSegmentIndex(m_wndOverview.m_iSavedProneSegment);
} // OnReportViewUnactivate


/**
 * Called when an item is deleted from the list.
 *
 * @param pNMHDR    Notificaition message header.
 * @param pResult   Set to 0 on success.
 */
void ReportNonDistendedAreas::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  ItemData* pItemData = reinterpret_cast<ItemData*>(plv->lParam);
  if (pItemData)
  {
    delete pItemData;
  }

  *pResult = 0;
} // OnDeleteItem


/**
 * Handles changes in list view selections.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Set to 0 on success.
 */
void ReportNonDistendedAreas::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  if(plv->uChanged & LVIF_STATE)
  {
    if(!(plv->uOldState&LVIS_SELECTED) && (plv->uNewState & LVIS_SELECTED))
    {
      ItemData* pItemData = reinterpret_cast<ItemData*>(plv->lParam);
      ASSERT(pItemData);
      OnEntryChanging(m_sCurrentEntry);
      m_sCurrentEntry = pItemData->m_sID;
      OnEntryChanged(m_sCurrentEntry);
    }
  }

  *pResult = 0;
} // OnItemChanged


/**
 * Called when an item is losing focus.
 *
 * @param pszEntryID   ID of the entry.
 * @return             Success.
 */
BOOL ReportNonDistendedAreas::OnEntryChanging(LPCTSTR pszEntryID)
{
  CWaitCursor cursor;

  try
  {
    if(*pszEntryID == '\0' || m_spRevision->IsReadOnly == VARIANT_TRUE)
      return TRUE;

    ReportLib::IReportNonDistendedFindingPtr spFinding = m_spRevision->NonDistendedFindings->GetItem(pszEntryID);

    if(rdrGlobal.m_pCurrDataset->m_segmentList.IsLoaded())
    {
      {
        int4 iWidth(0), iHeight(0), iWidthAlignedDWORD( 0 );

        LPBYTE pBuffer = LPBYTE(m_wndOverview.GetOverview()->GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE, true));
        ReportLib::IImagePtr spImage(__uuidof(ReportLib::Image));
        spImage->Create(iWidth, iHeight, 24);
        COleSafeArray sa;
				DWORD dwElements = iWidthAlignedDWORD*iHeight;
        sa.CreateOneDim(VT_UI1, dwElements, pBuffer);
        spImage->SetPixels(sa);
				if (rdrGlobal.m_bNormalizeSnapshotDim)
          spImage->Resize( 512, 512 );
        spFinding->Image = spImage;
      }

      spFinding->RotationMatrix = ReportVariant(rdrGlobal.m_pCurrDataset->m_trackballStandard.GetCurrentMatrix());
      spFinding->Quaternion = ReportVariant(rdrGlobal.m_pCurrDataset->m_trackballStandard.GetCurrentQuat());
      spFinding->TransferX = rdrGlobal.m_pCurrDataset->m_trackballStandard.GetTransferX();
      spFinding->TransferY = rdrGlobal.m_pCurrDataset->m_trackballStandard.GetTransferY();
      spFinding->ZoomFactor = rdrGlobal.m_pCurrDataset->m_trackballStandard.GetZoomFactor();
      spFinding->Location = static_cast<ReportLib::LocationEnum>(m_wndGroupLocation.GetSelectedItem());
    }
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_READ_NONDISTENDED_FINDING, e.Error());
  }

  return TRUE;
} // OnEntryChanging


/**
 * Called when an entry is gaining focus.
 *
 * @param pszEntryID   ID of the entry.
 * @return             Success.
 */
BOOL ReportNonDistendedAreas::OnEntryChanged(LPCTSTR pszEntryID)
{
  try
  {
    if (*pszEntryID == '\0')
    {
      return TRUE;
    }

    ReportLib::IReportNonDistendedFindingPtr spFinding = m_spRevision->NonDistendedFindings->GetItem(pszEntryID);

    extern SerializedDicom::PatientOrientation DatasetPositionToDicomPatientOrientation(const ReportLib::DatasetPositionEnumProp);
    const SerializedDicom::PatientOrientation ePatOrt = DatasetPositionToDicomPatientOrientation(spFinding->_DatasetPositionEnumProp);
    CString sEntryName(ColonViews::Layouts::ColonViatronixWnd::PatientOrientationToString(ePatOrt).c_str());

    std::string strArea(spFinding->Text);
    if (strArea[0] == '-')
    {
      strArea = LowerCaseString(LoadResourceString(IDS_COLONFINDING_AREA_UNCERTAIN));
    }

    if (!sEntryName.IsEmpty())
    {
      SetDlgItemText(IDC_ENTRY_NAME, "(" + sEntryName + ") " + strArea.c_str());
    }
    else
    {
      SetDlgItemText(IDC_ENTRY_NAME, strArea.c_str());
    }

    if(rdrGlobal.m_pCurrDataset->m_segmentList.IsLoaded())
    {
      // TODO: How to map dataset position to a dataset orientation
      int WHAT_TO_DO_HERE;
      rdrGlobal.SetDatasetOrientation(spFinding->_DatasetPositionEnumProp == ReportLib::Supine
        ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE);

      ReportVariant  matrix;
      matrix.Attach(spFinding->RotationMatrix.Detach());
      rdrGlobal.m_pCurrDataset->m_trackballStandard.SetCurrentMatrix(matrix.asMatrix());

      ReportVariant quat;
      quat.Attach(spFinding->Quaternion.Detach());
      rdrGlobal.m_pCurrDataset->m_trackballStandard.SetCurrentQuat(quat.asQuaternion());
      rdrGlobal.m_pCurrDataset->m_trackballStandard.SetTransfer(Point2D<int4>(spFinding->TransferX, spFinding->TransferY));
      rdrGlobal.m_pCurrDataset->m_trackballStandard.SetZoomFactor(spFinding->ZoomFactor);

      OverView* pOverView = dynamic_cast<OverView*>(m_wndOverview.GetOverview());
      ASSERT(pOverView);
      pOverView->SetRenderReportArrow(true);

      ReportVariant pos;
      pos.Attach(spFinding->Position);

      ReportVariant dir;
      dir.Attach(spFinding->Direction);

      pOverView->SetReportArrow(pos.asPoint(), dir.asNormal());

      ReportVariant begin;
      begin.Attach(spFinding->StartPoint);

      ReportVariant end;
      end.Attach(spFinding->EndPoint);

      pOverView->SetReportSegment(begin.asPoint(), end.asPoint());

      m_wndGroupLocation.SelectItem(static_cast<DWORD>(spFinding->Location));
      m_wndOverview.RedrawWindow();
    }
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_READ_NONDISTENDED_FINDING, e.Error());
    return FALSE;
  }
  return TRUE;
} // OnEntryChanged


/**
 * Loads the report.
 */
void ReportNonDistendedAreas::LoadReport()
{
  CWaitCursor cursor;

  m_wndEntries.SetRedraw(FALSE);
  m_wndEntries.DeleteAllItems();

  m_sCurrentEntry.Empty();

  m_wndEntries.SetRedraw(TRUE);
  m_wndEntries.RedrawWindow();
  m_wndEntries.SetRedraw(FALSE);

  ReportLib::IReportNonDistendedFindingsPtr spFindings = m_spRevision->NonDistendedFindings;

  long lNumFindingCount = spFindings->Count;
  for(long lFinding(0); lFinding < lNumFindingCount; lFinding++)
  {
    ReportLib::IReportNonDistendedFindingPtr spFinding = spFindings->GetItem(lFinding);

    ItemData* pid = new ItemData;
    pid->m_sID = static_cast<LPCTSTR>(spFinding->ID);
    pid->m_eDatasetPos = spFinding->_DatasetPositionEnumProp;

    CString s = static_cast<LPCTSTR>(spFinding->Text);
    if (s.GetAt(0) == '-')
    {
      s = LoadResourceStringWithUpperHead(IDS_COLONFINDING_AREA_UNCERTAIN);
    }

    LVITEM lvi;
    lvi.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvi.iItem     = m_wndEntries.GetItemCount();
    lvi.iSubItem  = 0;
    lvi.pszText   = (LPTSTR)(LPCTSTR)s;

    switch ( pid->m_eDatasetPos )
    {
      case ReportLib::Supine:
        lvi.iImage = 1;
        break;
      case ReportLib::Prone:
        lvi.iImage = 2;
        break;
      default:
        lvi.iImage = 0;
        break;
    }

    lvi.lParam    = reinterpret_cast<LPARAM>(pid);

    m_wndEntries.InsertItem(&lvi);
  }

  m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
  m_wndEntries.SetRedraw(TRUE);
  m_wndEntries.RedrawWindow();
} // LoadReport


/**
 * Deletes a non-distended area entry.
 */
void ReportNonDistendedAreas::OnRptDeleteNondistended()
{
  CString msg;
  msg.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
    LoadResourceString(IDS_DELETE), LoadResourceString(IDS_NON_DISTENDED_AREAS));
  if(AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION)  == IDNO)
    return;

  int4 nItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if(nItem == -1)
    return;

  ItemData* pld = reinterpret_cast<ItemData*>(m_wndEntries.GetItemData(nItem));
  try
  {
    m_spRevision->NonDistendedFindings->Remove(static_cast<LPCTSTR>(pld->m_sID));
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_DELETE_NONDISTENDED_AREA, e.Error());
  }

  m_sCurrentEntry.Empty();
  m_wndEntries.DeleteItem(nItem);
  m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
  if  ( m_wndEntries.GetItemCount() == 0 )
    SetDlgItemText(IDC_ENTRY_NAME, "" );
} // OnRptDeleteNondistended


/**
 * Deletes all non-distended areas.
 */
void ReportNonDistendedAreas::OnRptDeleteAllNondistended()
{
  CString msg;
  msg.Format(IDS_VERIFY_USER_OPERATION_ALL_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_NON_DISTENDED_AREAS));
  if (AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION)   == IDNO)
  {
    return;
  }

  int4 nItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if (nItem == -1)
    return;

  ItemData* pld = reinterpret_cast<ItemData*>(m_wndEntries.GetItemData(nItem));
  try
  {
    long nCount = m_spRevision->NonDistendedFindings->Count;
    for (long nItem(0);  nItem < nCount; nItem++)
      m_spRevision->NonDistendedFindings->Remove(0L);
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_DELETE_NONDISTENDED_AREA, e.Error());
  }

  m_sCurrentEntry.Empty();
  m_wndEntries.DeleteAllItems();
  m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
  SetDlgItemText(IDC_ENTRY_NAME, "" );
} // OnRptDeleteAllNondistended


/**
 * Reloads all non-distended areas.
 */
void ReportNonDistendedAreas::OnRptReloadNondistended()
{
  try
  {
    ReportLib::IReportNonDistendedFindingsPtr spFindings = m_spRevision->NonDistendedFindings;

    const long iNumNonDistended(spFindings->Count);

    if (iNumNonDistended > 0)
    {
      CString msg;
      msg.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
        LoadResourceString(IDS_RELOAD), LoadResourceString(IDS_NON_DISTENDED_AREAS));
      msg = LoadResourceString(IDS_DISCARD_CHANGES_PROMPT) + '\n' + msg;
      if(AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION) == IDNO)
      {
        return;
      }      
    }

    CWaitCursor cursor;

    for(long iFinding(0); iFinding < iNumNonDistended; iFinding++)
    {
      spFindings->Remove(0L);
    }

    // load non-distended areas in supine and prone datasets (and return the current dataset orientation)
    DatasetOrientations::TypeEnum eCurrDS = rdrGlobal.m_pCurrDataset->GetOrientation();

    OverView * pOverview = dynamic_cast<OverView*>(m_wndOverview.GetOverview());
    rdrGlobal.SetDatasetOrientation(DatasetOrientations::eSUPINE);
    pOverview->AddNonDistendedAreas(rdrGlobal.m_supine, spFindings, true);
    rdrGlobal.SetDatasetOrientation(DatasetOrientations::ePRONE);
    pOverview->AddNonDistendedAreas(rdrGlobal.m_prone, spFindings, false);

    // recover.
    rdrGlobal.SetDatasetOrientation(eCurrDS);
  }
  catch(_com_error&)
  {
    return;
  }

  CWaitCursor cursor;
  LoadReport();
} // OnRptReloadNondistended


/**
 * Called when user has marked/unmarked a nondistended area's distance from rectum as uncertain.
 */
void ReportNonDistendedAreas::OnRptNondistendedAreaUncertain()
{
  int4 nItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if(nItem == -1)
    return;

  try
  {
    // modify 'm_spRevision'
    ItemData* pld = reinterpret_cast<ItemData*>(m_wndEntries.GetItemData(nItem));
    ReportLib::IReportNonDistendedFindingPtr spFinding = m_spRevision->NonDistendedFindings->GetItem(static_cast<LPCTSTR>(pld->m_sID));

    std::string strName(spFinding->Text);
    std::string strArea;
    bool bUndeterminedDistance = false;
    if (strName[0] == '-')
    {
      strName.erase(0,1);
      strArea = strName;
      spFinding->Text = strName.c_str();
      bUndeterminedDistance = false;
    }
    else
    {
      strArea = LoadResourceStringWithUpperHead(IDS_COLONFINDING_AREA_UNCERTAIN);
      strName.insert(0, "-");
      spFinding->Text = strName.c_str();
      bUndeterminedDistance = true;
    }

    // remove the old entry from the list control.
    m_wndEntries.DeleteItem(nItem);

    // add new entry to the list control.
    ItemData* pid = new ItemData;
    pid->m_sID = static_cast<LPCTSTR>(spFinding->ID);

    extern SerializedDicom::PatientOrientation DatasetPositionToDicomPatientOrientation(const ReportLib::DatasetPositionEnumProp);
    const SerializedDicom::PatientOrientation ePatOrt = DatasetPositionToDicomPatientOrientation(spFinding->_DatasetPositionEnumProp);
    CString sEntryName(ColonViews::Layouts::ColonViatronixWnd::PatientOrientationToString(ePatOrt).c_str());

    LVITEM lvi;
    lvi.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvi.iItem    = nItem;
    lvi.iSubItem = 0;
    lvi.pszText  = (LPTSTR)(LPCTSTR)strArea.c_str();

    switch ( pid->m_eDatasetPos )
    {
      case ReportLib::Supine:
        lvi.iImage = 1;
        break;
      case ReportLib::Prone:
        lvi.iImage = 2;
        break;
      default:
        lvi.iImage = 0;
        break;
    }

    lvi.lParam   = reinterpret_cast<LPARAM>(pid);
    m_wndEntries.InsertItem(&lvi);

    // set up new 'Entry Name'
    CString sTitle;
    sTitle = sEntryName;

    if (!sTitle.IsEmpty())
    {
      SetDlgItemText(IDC_ENTRY_NAME, "(" + sTitle + ") " + LowerCaseString(strArea.c_str()));
    }
    else
    {
      SetDlgItemText(IDC_ENTRY_NAME, strArea.c_str());
    }
    // end setup new 'Entry Name'.
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_DELETE_NONDISTENDED_AREA, e.Error());
  }
} // OnRptNondistendedAreaUncertain()


/**
 * Right-mouse click, shows popup menu.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Set to 0 on success.
 */
void ReportNonDistendedAreas::OnRclickEntries(NMHDR* pNMHDR, LRESULT* pResult)
{
  DWORD dwPos = GetMessagePos();
  CPoint pt(LOWORD(dwPos), HIWORD(dwPos));

  CMenu menu;
  VERIFY(menu.LoadMenu(IDM_REPORT_POPUPS));

  CMenu* pMenu = menu.GetSubMenu(3);

  CPoint ptHitTest(pt) ;
  m_wndEntries.ScreenToClient(&ptHitTest);

  // Enable/disable/check menu items as appropriate
  int4 iItem = m_wndEntries.HitTest(ptHitTest);
  if (iItem < 0)
  {
    pMenu->EnableMenuItem(IDM_RPT_DELETE_NONDISTENDED, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
    pMenu->EnableMenuItem(IDM_RPT_NONDISTENDED_UNCERTAIN , MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
  }
  else
  {
    int4 iItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
    ItemData* pld = reinterpret_cast<ItemData*>(m_wndEntries.GetItemData(iItem));
    ReportLib::IReportNonDistendedFindingPtr spFinding = m_spRevision->NonDistendedFindings->GetItem(static_cast<LPCTSTR>(pld->m_sID));

    // check label.
    std::string strArea = spFinding->Text;
    if (strArea[0] == '-')
    {
      pMenu->CheckMenuItem(IDM_RPT_NONDISTENDED_UNCERTAIN, MF_CHECKED);
    }
    else
    {
      pMenu->CheckMenuItem(IDM_RPT_NONDISTENDED_UNCERTAIN, MF_UNCHECKED);
    }
    // end check label.
  }

  if (m_wndEntries.GetItemCount() <= 0)
  {
    pMenu->EnableMenuItem(IDM_RPT_DELETE_ALL_NONDISTENDED, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
  }
  // (end) Enable/disable/check menu items as appropriate


  pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);
  *pResult = 0;
} // OnRclickEntries


////////////////////////////////////////////////////////////////////////////////////

#include "OverView.h"

namespace ColonViews
{
  namespace ChildViews
  {
    class ReportOverView : public OverView
    {
    public:
      /// constructor
      ReportOverView(HWND hParent, const ReaderLib::DatasetOrientations::TypeEnum eOrtn,
        const bool bBufferImg = false) : OverView(hParent, eOrtn, bBufferImg) {}
        /// gets the resource ID of the popup menu for this view
        virtual uint4 GetPopupMenuID(UINT, CPoint) const;
    };
  }
}

/**
 * Constructor
 */
ReportNonDistendedAreas::OverviewChildWnd::OverviewChildWnd(const bool bDisplayAllSegments)
: ViatronixChildWnd(0), m_savedOverViewModes(ViatronStudy::m_modeOverView), m_bDisplayAllSegments(bDisplayAllSegments)
{
  ViatronStudy::m_modeOverView -= ViewModes::TRANSLUCENT;
  ViatronStudy::m_modeOverView -= ViewModes::ORTHOGONAL;
  ViatronStudy::m_modeOverView -= ViewModes::SLICE;
} // constructor


/**
 * Destructor
 */
ReportNonDistendedAreas::OverviewChildWnd::~OverviewChildWnd()
{
  ViatronStudy::m_modeOverView = m_savedOverViewModes;

  rdrGlobal.m_supine.m_segmentList.SetSegmentOrder(m_vSupineSegmentOrder);
  rdrGlobal.m_supine.m_segmentList.SetCurrentSegmentIndex(m_iSavedSupineSegment);

  rdrGlobal.m_prone.m_segmentList.SetSegmentOrder(m_vProneSegmentOrder);
  rdrGlobal.m_prone.m_segmentList.SetCurrentSegmentIndex(m_iSavedProneSegment);
} // destructor


/**
 * Creates the child views.
 *
 * @param hWndThis   Handle of this window.
 */
void ReportNonDistendedAreas::OverviewChildWnd::CreateChildViews(HWND hWndThis)
{
  std::list<GLChildViewPtr> viewList;
  viewList.push_back(new ReportOverView(hWndThis, DatasetOrientations::ePRIMARY, true));
  SetChildViews(viewList);
  OverView & rOverview = (OverView &) GetChildViewByPos(0);
  rOverview.GetDatasetRef().m_pTrackball = & (rOverview.GetDatasetRef().m_trackballStandard);
  rOverview.GetOppositeDatasetRef().m_pTrackball = & (rOverview.GetOppositeDatasetRef().m_trackballStandard);
  rOverview.SetDisplayListRender(false);
  rOverview.SetPermitSlices( false );
  rOverview.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 1, 1 ), this, 2001 );

} // CreateChildViews


BEGIN_MESSAGE_MAP( ReportNonDistendedAreas::OverviewChildWnd, ColonViews::Layouts::ViatronixChildWnd )
  ON_WM_SIZE()
END_MESSAGE_MAP()


void ReportNonDistendedAreas::OverviewChildWnd::OnSize( UINT uType, int4 iCX, int4 iCY )
{
  ColonViews::Layouts::ViatronixChildWnd::OnSize( uType, iCX, iCY );
 
  CRect rc;
  GetClientRect(rc);
  SetViewRect(0, Rect<int4>(rc.left,rc.top,rc.right,rc.bottom));
} // OnSize

/**
 * Calculates the view rectangles.
 *
 * @param const int4 iWidth    Width of window.
 * @param const int4 iHeight   Height of window.
 */
void ReportNonDistendedAreas::OverviewChildWnd::CalculateViewRects(const int4 iWidth, const int4 iHeight)
{
  /*CRect rc;
  GetClientRect(rc);
  SetViewRect(0, Rect<int4>(rc.left,rc.top,rc.right,rc.bottom));*/
} // CalculateViewRects


/**
 * Renders the view.
 */
void ReportNonDistendedAreas::OverviewChildWnd::GLRenderViews()
{
  if (rdrGlobal.m_pCurrDataset->m_segmentList.IsLoaded())
  {
    GLRenderView( 0 );
  }
  else
  {
    ViatronixChildWnd::GLRenderViews();
  }
} // GLRenderViews
 

/**
 * ON_WM_MOUSEMOVE handler, overriden to prevent base class calls to SetChildViewMisc(...)
 *
 * @param uFlags   Current key flags
 * @param point    Current point of mouse move
 */
void ReportNonDistendedAreas::OverviewChildWnd::OnMouseMove(UINT uFlags, CPoint point)
{
  GLChildViewRef prevView = GetCurrentView();
  GLRenderViews();
  GLWnd::OnMouseMove(uFlags, point);
} // OnMouseMove


/**
 * gets the resource ID of the popup menu for this view
 *
 * @return   ID of menu resource
 */
uint4 ReportOverView::GetPopupMenuID(UINT, CPoint) const
{
  return IDM_OVERVIEW_RESET;
} // GetPopupMenuID


// Revision History:
// $Log: reportnondistendedareas.c,v $
// Revision 1.13  2007/03/10 03:19:30  jmeade
// code standards.
//
// Revision 1.12  2007/02/06 16:44:57  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.11  2006/10/03 18:14:08  jmeade
// Issue 4946: display of decubitus patient positions.
//
// Revision 1.10  2006/09/22 21:50:29  jmeade
// Issue 4946: display of decubitus patient positions in report.
//
// Revision 1.9  2006/08/31 20:05:51  jmeade
// Issue 4946: text in non-distended regions.
//
// Revision 1.8  2006/02/07 21:40:28  dongqing
// code walk through
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.5.2.3  2006/01/05 16:04:00  geconomos
// Issue# 4589: Overview not being sized correctly
//
// Revision 1.5.2.2  2005/12/14 20:16:57  jmeade
// Issue 4563: Only 'reset display' for non-distended overview image.
//
// Revision 1.5.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.5  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.17.2.11.2.3.2.1  2005/07/18 19:54:31  geconomos
// Issue #4288: fixed overview image
//
// Revision 3.17.2.11.2.3  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.17.2.11.2.2  2005/04/22 15:48:55  jmeade
// Issue 4134: Disable mouse help.
//
// Revision 3.17.2.11.2.1.2.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.17.2.11.2.1  2005/04/05 14:47:17  frank
// Issue #4055: added ability to disable slice indicators on overview
//
// Revision 3.17.2.11  2005/02/15 19:46:33  jmeade
// merge from vc1.4 branch.
//
// Revision 3.17.2.10.2.1  2005/01/20 21:32:49  jmeade
// allow drawing of overview text.
//
// Revision 3.17.2.10  2004/08/11 16:46:31  geconomos
// Report images are always at least 512x512
//
// Revision 3.17.2.9  2004/07/21 20:24:00  jmeade
// Report lib enum.
//
// Revision 3.17.2.8  2004/06/17 18:41:38  vxconfig
// enum error.
//
// Revision 3.17.2.7  2004/06/09 15:13:04  jmeade
// Issue 3709: Must save and reset the current segment before changing segment selections.
//
// Revision 3.17.2.6  2004/03/30 23:45:47  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.17.2.5  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.17.2.4  2003/04/09 21:49:16  jmeade
// Coding standards.
//
// Revision 3.17.2.3  2003/04/08 16:12:51  jmeade
// Issue 3080:  Don't display distance-from-rectum in overview image, only prompt to discard changes when there are entries.
//
// Revision 3.17.2.2  2003/03/07 04:11:33  jmeade
// Issue 2886:  Replace unused delete format strings.
//
// Revision 3.17.2.1  2003/02/22 11:58:21  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.17  2002/12/12 22:07:07  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.16  2002/11/22 00:27:39  jmeade
// Coding standard on m_trackball* variables.
//
// Revision 3.15  2002/11/15 00:34:49  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.14  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.12  2002/07/18 20:25:39  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.11  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.10  2002/04/18 18:32:01  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.9  2002/04/12 02:14:56  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.8  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7  2002/02/04 20:17:30  binli
// improved image quality in report file
//
// Revision 3.6.2.3  2002/07/02 20:47:53  jmeade
// Issue 2062:  Reset segment ordering for displaying non-distended areas.
//
// Revision 3.6.2.2  2002/06/12 02:26:52  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.6.2.1  2002/02/11 16:37:19  binli
// Improved image quality in report file
//
// Revision 3.6  2002/01/17 18:55:38  binli
// 1> adjustment of captions
// 2> hardcoding of some const strings.
//
// Revision 3.5  2002/01/15 20:49:14  binli
// issue 1424: option to show/not show distance in report
//
// Revision 3.4  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.3  2001/12/22 00:10:08  jmeade
// Code standards.
//
// Revision 3.2  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1.2.7  2001/12/03 21:31:37  jmeade
// StudyType changed to DatasetPositionEnumProp to eliminate current and future conflicts
//
// Revision 3.1.2.6  2001/11/06 01:13:20  jmeade
// Coding standards.
//
// Revision 3.1.2.5  2001/11/06 00:50:30  jmeade
// Coding standards.
//
// Revision 3.1.2.4  2001/11/05 22:09:00  jmeade
// Coding standards.
//
// Revision 3.1.2.3  2001/11/05 18:02:48  jmeade
// No hard-coding strings.
//
// Revision 3.1.2.2  2001/11/04 05:13:41  jmeade
// Issue 1287, Item 4; Code standards.
//
// Revision 3.1.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1  2001/10/20 10:31:54  jmeade
// Issue 1226:  Toggle off translucency option when viewing non-distended areas
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.4   Oct 08 2001 17:04:14   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.3   Sep 25 2001 17:11:58   binli
// ID 1009 (part2): 1> correct position of arrow; 2> always show up of arrow.
// 
//    Rev 2.2   Sep 25 2001 13:46:18   binli
// ID 1009: (part 1): correct study images in PDF file.
// 
//    Rev 2.1   Sep 20 2001 18:42:56   jmeade
// Popup menu naming
// 
//    Rev 2.0   Sep 16 2001 23:41:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:36   ingmar
// Initial revision.
// Revision 1.33  2001/09/11 18:01:52  binli
// small error in bug fix (ID 000672)
//
// Revision 1.32  2001/09/10 22:39:35  binli
// track ID: 000672: added (Supine)/(Prone) to caption in Colon-Findings and NonDistendedAreas.
//
// Revision 1.31  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.30  2001/05/17 20:42:04  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.29  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.28  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.27  2001/05/04 15:34:04  binli
// reflected works of display lists.
//
// Revision 1.26  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.25  2001/04/16 18:43:07  binli
// removed unused variable: m_bImageCreated in Overview.
//
// Revision 1.24  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.23  2001/04/05 20:43:00  binli
// reflected change in TrackBall: private matrix.
//
// Revision 1.22  2001/04/03 22:09:24  binli
// reflected changes in TrackBall.
//
// Revision 1.21  2001/03/14 16:43:44  geconomos
// Added delete all to NonDistended areas
//
// Revision 1.20  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.19  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.18  2001/02/12 16:44:38  geconomos
// Fixe supine and prone dilema in non distended areas
//
// Revision 1.17  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.16  2001/02/05 20:51:47  jeff
// more strings to resource table
//
// Revision 1.15  2001/02/05 18:23:38  geconomos
// Added start  and end points in order to draw centerline
//
// Revision 1.14  2001/02/05 14:32:12  geconomos
// Implementation of ReportVariant
//
// Revision 1.13  2001/02/02 08:06:16  jeff
// Moving strings to resource table
//
// Revision 1.12  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/reportnondistendedareas.c,v 1.13 2007/03/10 03:19:30 jmeade Exp $
// $Id: reportnondistendedareas.c,v 1.13 2007/03/10 03:19:30 jmeade Exp $
