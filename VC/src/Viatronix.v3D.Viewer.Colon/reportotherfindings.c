// $Id: reportotherfindings.c,v 1.4.2.5 2008/12/20 00:42:26 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for other findings
// Owner: George Economos

#include "stdafx.h"
#include "viatron.h"
#include "ReportOtherFindings.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace WinUtils;
using namespace ColonReport;

/**
 * constructor
 * @param pParent   parent window
 **/
ReportOtherFindings::ReportOtherFindings(CWnd* pParent): ReportWndBase(ReportOtherFindings::IDD, pParent),
 m_pSubFinding(NULL),
 m_bInitComplete(false),
 m_iFocusID(-1)
{
  //{{AFX_DATA_INIT(ReportOtherFindings)
  //}}AFX_DATA_INIT

  FillGroupMaps(m_mapGroupNames, m_mapSubFindings);
} // constructor


/**
 * fills in map information
 *
 * @param TOrganGroupMap   organs
 * @param TSubFindingMap   finding information
 */
void ReportOtherFindings::FillGroupMaps(TOrganGroupMap& mapGroupNames, TSubFindingMap& mapSubFindings)
{
  mapGroupNames[GID_Lung] = LoadResourceString(IDS_LUNG);
  mapGroupNames[GID_Liver] = LoadResourceString(IDS_LIVER);
  mapGroupNames[GID_Renal] = LoadResourceString(IDS_RENAL);
  mapGroupNames[GID_Adrenal] = LoadResourceString(IDS_ADRENAL);
  mapGroupNames[GID_Spleen] = LoadResourceString(IDS_SPLEEN);
  mapGroupNames[GID_GI] = LoadResourceString(IDS_GI);
  mapGroupNames[GID_Gynecology] = LoadResourceString(IDS_GYNECOLOGY);
  mapGroupNames[GID_Vascular] = LoadResourceString(IDS_VASCULAR);
  mapGroupNames[GID_Miscellaneous] = LoadResourceString(IDS_MISCELLANEOUS);

  mapSubFindings[ReportLib::Lung_Nodule] = CreateMapEntry(GID_Lung, LoadResourceStringWithUpperHead(IDS_LUNG_NODULE), IDD_RPT_SUB_LUNG1, IDM_LUNG_NODULE);
  mapSubFindings[ReportLib::Lung_Granuloma] = CreateMapEntry(GID_Lung, LoadResourceStringWithUpperHead(IDS_LUNG_GRANULOMA), IDD_RPT_SUB_LUNG1, IDM_LUNG_GRANULOMA);
  mapSubFindings[ReportLib::Lung_Pneomuthorax] = CreateMapEntry(GID_Lung, LoadResourceStringWithUpperHead(IDS_PNEUMOTHORAX), IDD_RPT_SUB_LUNG2, IDM_LUNG_PNEUMOTHORAX);
  mapSubFindings[ReportLib::Liver_Mass] = CreateMapEntry(GID_Liver, LoadResourceStringWithUpperHead(IDS_LIVER_MASS), IDD_RPT_SUB_LIVER1, IDM_LIVER_MASS);
  mapSubFindings[ReportLib::Liver_Cyst] = CreateMapEntry(GID_Liver, LoadResourceStringWithUpperHead(IDS_LIVER_CYST), IDD_RPT_SUB_LIVER1, IDM_LIVER_CYST);
  mapSubFindings[ReportLib::Liver_FattyLiver] = CreateMapEntry(GID_Liver, LoadResourceStringWithUpperHead(IDS_FATTY_LIVER), IDD_RPT_SUB_OTHER, IDM_LIVER_FATTY);
  mapSubFindings[ReportLib::Liver_Gallstones] = CreateMapEntry(GID_Liver, LoadResourceStringWithUpperHead(IDS_GALLSTONES), IDD_RPT_SUB_OTHER, IDM_LIVER_GALLSTONES);
  mapSubFindings[ReportLib::Adrenal_Mass] = CreateMapEntry(GID_Adrenal, LoadResourceStringWithUpperHead(IDS_ADRENAL_MASS), IDD_RPT_SUB_ADRENAL1, IDM_ADRENAL_MASS);
  mapSubFindings[ReportLib::Renal_Mass] = CreateMapEntry(GID_Renal, LoadResourceStringWithUpperHead(IDS_RENAL_MASS), IDD_RPT_SUB_RENAL1, IDM_RENAL_MASS);
  mapSubFindings[ReportLib::Renal_Cyst] = CreateMapEntry(GID_Renal, LoadResourceStringWithUpperHead(IDS_RENAL_CYST), IDD_RPT_SUB_RENAL1, IDM_RENAL_CYST);
  mapSubFindings[ReportLib::Renal_Stones] = CreateMapEntry(GID_Renal, LoadResourceStringWithUpperHead(IDS_RENAL_STONES), IDD_RPT_SUB_RENAL1, IDM_RENAL_STONES);
  mapSubFindings[ReportLib::Renal_Scar] = CreateMapEntry(GID_Renal, LoadResourceStringWithUpperHead(IDS_RENAL_SCAR), IDD_RPT_SUB_RENAL2, IDM_RENAL_SCAR);
  mapSubFindings[ReportLib::Renal_Hydronephrosis] = CreateMapEntry(GID_Renal, LoadResourceStringWithUpperHead(IDS_HYDRONEPHROSIS), IDD_RPT_SUB_RENAL3, IDM_RENAL_HYDRONEPHROSIS);
  mapSubFindings[ReportLib::Vascular_AAA] = CreateMapEntry(GID_Vascular, LoadResourceString(IDS_AAA), IDD_RPT_SUB_VASCULAR1, IDM_VASCULAR_AAA);
  mapSubFindings[ReportLib::Vascular_ArteralCalcification] = CreateMapEntry(GID_Vascular, LoadResourceStringWithUpperHead(IDS_ARTERAL_CALCIFICATION), IDD_RPT_SUB_VASCULAR2, IDM_VASCULAR_CALCIFICATION);
  mapSubFindings[ReportLib::GI_HiatalHernia] = CreateMapEntry(GID_GI, LoadResourceStringWithUpperHead(IDS_HIATAL_HERNIA), IDD_RPT_SUB_OTHER, IDM_GI_HIATAL);
  mapSubFindings[ReportLib::GI_InguinalHernia] = CreateMapEntry(GID_GI, LoadResourceStringWithUpperHead(IDS_INGUINAL_HERNIA), IDD_RPT_SUB_GI1, IDM_GI_INGUINAL);
  mapSubFindings[ReportLib::Gynecology_UterineFibroids] = CreateMapEntry(GID_Gynecology, LoadResourceStringWithUpperHead(IDS_UTERINE_FIBROIDS), IDD_RPT_SUB_OTHER, IDM_GYNECOLOGY_FIBROIDS);
  mapSubFindings[ReportLib::Spleen_Mass] = CreateMapEntry(GID_Spleen, LoadResourceStringWithUpperHead(IDS_SPLENIC_MASS), IDD_RPT_SUB_SPLEEN1, IDM_SPLEEN_MASS);
  mapSubFindings[ReportLib::Spleen_Granuloma] = CreateMapEntry(GID_Spleen, LoadResourceStringWithUpperHead(IDS_SPLENIC_GRANULOMA), IDD_RPT_SUB_SPLEEN1, IDM_SPLEEN_GRANULOMA);
  mapSubFindings[ReportLib::Misc_Other] = CreateMapEntry(GID_Miscellaneous, LoadResourceStringWithUpperHead(IDS_OTHER), IDD_RPT_SUB_OTHER, IDM_MISC_OTHER);
} // FillGroupMaps()


/**
 * helper function for adding findings info
 * @param eGroupID     identifies finding group
 * @param pszName      name for finding
 * @param uDlgID       ID of corresponding control
 * @param uCommandID   ID of finding command
 **/
OtherFindingData* ReportOtherFindings::CreateMapEntry(const OrganGroupIDEnum eGroupID, LPCTSTR pszName, const UINT uDlgID, const UINT uCommandID)
{
  OtherFindingData* pfd = new OtherFindingData;
  ASSERT(pfd);

  pfd->eGroupID = eGroupID;
  pfd->sName = pszName;
  pfd->uDlgID = uDlgID;
  pfd->uCommandID = uCommandID;

  return pfd;
} // CreateMapEntry()


/**
 * destructor
 **/
ReportOtherFindings::~ReportOtherFindings()
{
  for(TSubFindingMap::const_iterator it = m_mapSubFindings.begin(); it != m_mapSubFindings.end(); it++)
  {
    OtherFindingData* pfd = it->second;
    delete pfd;
  }
} // destructor


/**
 * Dialog data variable initialization.
 *
 *  @param pDX   Dialog data exchange instance.
 */
void ReportOtherFindings::DoDataExchange(CDataExchange* pDX)
{
  ReportWndBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ReportOtherFindings)
  DDX_Control(pDX, IDC_OTHERFINDING_LIST, m_wndGroup);
  //}}AFX_DATA_MAP
} // DoDataExchange()


/**
 * message map
 */
BEGIN_MESSAGE_MAP(ReportOtherFindings, ReportWndBase)
  //{{AFX_MSG_MAP(ReportOtherFindings)
  ON_WM_DESTROY()
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_OTHERFINDING_LIST, OnItemChanged)
  ON_NOTIFY(LVN_DELETEITEM, IDC_OTHERFINDING_LIST, OnDeleteItem)
  ON_NOTIFY(NM_RCLICK, IDC_OTHERFINDING_LIST, OnRClickItem)
  ON_WM_SHOWWINDOW()
  ON_WM_MOUSEMOVE()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDOWN()
  ON_COMMAND(IDM_RPT_DELETE_ENTRY, OnRptDeleteItem)
  ON_COMMAND(IDM_RPT_DELETE_ALL_ENTRIES, OnRptDeleteAllItems)
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDM_LUNG_NODULE, IDM_MISC_OTHER, OnAddFinding)
 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ReportOtherFindings message handlers

/**
 * Init dialog procedure
 * 
 * @return   Create success
 */
BOOL ReportOtherFindings::OnInitDialog() 
{
  ReportWndBase::OnInitDialog();

  GetDlgItem(IDC_STATIC_LUNG)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_LIVER)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_ADRENAL)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_RENAL)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_VASCULAR)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_SPLEEN)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_GI)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_GYNECOLOGY)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_VASCULAR)->SetFont(&m_fontBold);
  GetDlgItem(IDC_STATIC_MISC)->SetFont(&m_fontBold);
   
  m_wndGroup.SetBackgroundImage(GetChildBackground(m_wndGroup.GetDlgCtrlID()));
  m_wndGroup.SetUseGroups(TRUE);
  
  m_bInitComplete = true;

  return TRUE;
} // OnInitDialog()


/**
 * set the report data
 * 
 * @param spRevision   report revision
 */
void ReportOtherFindings::SetReport(ReportLib::IReportRevisionPtr& spRevision)
{
  ReportWndBase::SetReport(spRevision);

  try
  {
    long iNumFindings = m_spRevision->OtherFindings->Count;
    for(long iFinding(0); iFinding < iNumFindings; iFinding++)
    {
      ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(iFinding);
      OtherFindingData* pfd = m_mapSubFindings[spFinding->GetFinding()];

      ItemData* pid = new ItemData;
      pid->sID = static_cast<LPCTSTR>(spFinding->ID);

      if(m_wndGroup.FindItem(pfd->eGroupID) == -1)
        m_wndGroup.InsertGroup(pfd->eGroupID, m_mapGroupNames[pfd->eGroupID]);

      m_wndGroup.InsertGroupItem(pfd->eGroupID, pfd->sName, reinterpret_cast<DWORD>(pid));
    }
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_OTHER_FINDING, e.Error());
  }
} // SetReport()


/**
 * view going out of scope
 */
void ReportOtherFindings::OnReportViewUnactivate()
{
  if (m_spRevision == NULL || m_spRevision->IsOpen == VARIANT_FALSE)
    return;

  if (m_spRevision->IsReadOnly == VARIANT_FALSE)
  {
    if (m_pSubFinding)
      m_pSubFinding->OnReportViewUnactivate();
  }
} // OnReportViewUnactivate()


/**
 * adds a new finding
 * 
 * @param uID   id of finding
 */
void ReportOtherFindings::OnAddFinding(UINT uID)
{
  if (m_spRevision == NULL || m_spRevision->IsOpen == VARIANT_FALSE)
    return;

  bool bFound(false);

  TSubFindingMap::const_iterator it = m_mapSubFindings.begin();
  while (it != m_mapSubFindings.end())
  {
    if (it->second->uCommandID == uID)
    {
      bFound = true;
      break;
    }
    
    it++;
  }
  if (!bFound)
    return;

  ItemData* pid = new ItemData;

  try
  {
    ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->Add();
    pid->sID = static_cast<LPCTSTR>(spFinding->ID);
    spFinding->Finding = it->first;
  }
  catch(_com_error& e)
  { 
    DisplayReportError(IDS_ADD_FINDING, e.Error());
    delete pid;
    return;
  }

  if (m_wndGroup.FindItem(it->second->eGroupID) == -1)
    m_wndGroup.InsertGroup(it->second->eGroupID, m_mapGroupNames[it->second->eGroupID]);

  m_wndGroup.InsertGroupItem(it->second->eGroupID, it->second->sName, reinterpret_cast<DWORD>(pid));
  m_wndGroup.SelectItem(reinterpret_cast<DWORD>(pid));
} // OnAddFinding()


/**
 * extra handling on show window call
 * 
 * @param bShow     Show or hide
 * @param uStatus   status of window
 */
void ReportOtherFindings::OnShowWindow(BOOL bShow, UINT uStatus) 
{
  ReportWndBase::OnShowWindow(bShow, uStatus);
  
  if(m_pSubFinding)
    m_pSubFinding->ShowWindow(bShow? SW_SHOW:SW_HIDE);
} // OnShowWindow()


/**
 * WM_DESTROY handler
 */
void ReportOtherFindings::OnDestroy() 
{
  if(m_pSubFinding)
  {
    m_pSubFinding->DestroyWindow();
    delete m_pSubFinding;
    m_pSubFinding = NULL;
  } 

  ReportWndBase::OnDestroy();
} // OnDestroy()


/**
 * Called when an item is deleted from the list.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Set to 0 on success.
 */
void ReportOtherFindings::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
  *pResult = 0;
  
  if(m_wndGroup.IsGroup(plv->iItem))
    return;

  ItemData* pid = reinterpret_cast<ItemData*>(plv->lParam);
  delete pid;

} // OnDeleteItem


/**
 * Handles changes in list view selections.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Set to 0 on success.
 */
void ReportOtherFindings::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
  
  if( !(plv->uOldState & LVIS_SELECTED) && plv->uNewState&LVIS_SELECTED)
  {
    if(m_wndGroup.IsGroup(plv->iItem))
       return;
     
    ItemData* pid = reinterpret_cast<ItemData*>(plv->lParam);
    
    ShowSubFinding(pid->sID);
  }
  
  *pResult = 0;
} // OnItemChanged


/**
 * displays sub-finding dialog
 *
 * @param pszID   identifies finding
 * @return        success/failure
 */
BOOL ReportOtherFindings::ShowSubFinding(LPCTSTR pszID)
{
  HideSubFinding();

  CRect rc;
  GetWindowRect(rc);
  GetParent()->ScreenToClient(rc);

  CPoint ptFinal;

  ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(pszID);
  
  if(m_mapSubFindings.find(spFinding->GetFinding()) == m_mapSubFindings.end())
    return TRUE;
  
  ReportSubFinding* pSubFinding = new ReportSubFinding;
  if(pSubFinding == NULL)
    return FALSE;

  pSubFinding->Create(m_mapSubFindings[spFinding->GetFinding()]->uDlgID, GetParent());
  
  pSubFinding->SetFinding(spFinding);

  CRect rcSubFinding;
  pSubFinding->GetClientRect(rcSubFinding);

  ptFinal.x = rc.right - 7;
  ptFinal.y = rc.top + ((rc.Height() - rcSubFinding.Height()) / 2);
    
  m_pSubFinding = pSubFinding;
  pSubFinding->SlideOut(ptFinal, this);

  return TRUE;
} // ShowSubFinding()


/**
 * hides sub-finding dialog
 */
void ReportOtherFindings::HideSubFinding()
{
  CRect rc;
  GetWindowRect(rc);
  GetParent()->ScreenToClient(rc);
  
  CPoint ptFinal;
  
  if(m_pSubFinding)
  {
    
    CRect rcSubFinding;
    m_pSubFinding->GetClientRect(rcSubFinding);
    
    ptFinal.x = rc.right - rcSubFinding.Width();
    ptFinal.y = rc.top + ((rc.Height() - rcSubFinding.Height()) / 2);      
    
    m_pSubFinding->SlideIn(ptFinal, this);
    m_pSubFinding->OnReportViewUnactivate();
    
    m_pSubFinding->DestroyWindow();
    GetParent()->RedrawWindow(rcSubFinding);
    delete m_pSubFinding;
    m_pSubFinding = NULL;
    Sleep(75);
  }
} // HideSubFinding()


/**
 * Mouse move handler; displays help text if available for button, tracks mouse for mouse leave event
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse move point
 */
void ReportOtherFindings::OnMouseMove(UINT uFlags, CPoint point) 
{
  CWnd* pWnd = ChildWindowFromPoint(point);
  if(pWnd)
  {
    int4 iID = pWnd->GetDlgCtrlID();
    
    if(iID < IDM_LUNG_NODULE || iID > IDM_MISC_OTHER)
    {
      if(m_iFocusID != -1)
      {
        CWnd* pWnd = GetDlgItem(m_iFocusID);
        pWnd->SetFont(&m_fontNormal);
        m_iFocusID = -1;
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
      }

      return;      
    }

    if(iID != m_iFocusID)
    {
      if(m_iFocusID != -1)
      {
        CWnd* pWnd = GetDlgItem(m_iFocusID);
        pWnd->SetFont(&m_fontNormal);
      }
      
      pWnd->SetFont(&m_fontUnderline);
      m_iFocusID = iID; 
      SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
    }
  }
  
  ReportWndBase::OnMouseMove(uFlags, point);

} // OnMouseMove( UINT uFlags, CPoint point ) 


/**
 * WM_LBUTTONDOWN down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void ReportOtherFindings::OnLButtonDown(UINT uFlags, CPoint point) 
{
  CWnd* pWnd = ChildWindowFromPoint(point);
  if(pWnd)
  {
    int4 iID = pWnd->GetDlgCtrlID();

    if(iID >= IDM_LUNG_NODULE || iID <= IDM_MISC_OTHER)
      PostMessage(WM_COMMAND, iID);
  }
      
  ReportWndBase::OnLButtonDown(uFlags, point);
} // OnLButtonDown()


/**
 * WM_SETCURSOR msg
 *
 * @param pWnd       pointer to this window
 * @param uHitTest   identifies where the cursor is
 * @param uMessage   message that triggered set cursor msg
 */
BOOL ReportOtherFindings::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  return TRUE;
} // OnSetCursor()


/**
 * NM_RCLICK handler
 *
 * @param   pNMHDR      NMHDR contains information related to this message
 * @param   pResult     resultant code
 */
void ReportOtherFindings::OnRClickItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
  if (m_wndGroup.GetItemCount() <= 0)
  {
    return;
  }

  DWORD dwPos = GetMessagePos();

  CPoint pt(LOWORD(dwPos), HIWORD(dwPos));
  
  CPoint ptHitTest(pt) ;
  m_wndGroup.ScreenToClient(&ptHitTest);
  
  CMenu menu;
  VERIFY(menu.LoadMenu(IDM_REPORT_POPUPS));
  
  CMenu* pMenu = menu.GetSubMenu(2);

  int4 iItem = m_wndGroup.HitTest(ptHitTest);
  if ((iItem < 0) || m_wndGroup.IsGroup(iItem))
  {
    pMenu->EnableMenuItem(IDM_RPT_DELETE_ENTRY, MF_BYCOMMAND | MF_GRAYED);
  }

  pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);  
  
  *pResult = 0;
} // OnRClickItem()


/**
 * delete a finding
 * @param iItem   item to delete
 */
void ReportOtherFindings::DeleteItem(const int4 iItem)
{
  if (m_wndGroup.IsGroup(iItem))
  {
    return;
  }

  ItemData* pid = reinterpret_cast<ItemData*>(m_wndGroup.GetItemData(iItem));
  ASSERT(pid);

  try
  {
    ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(static_cast<LPCTSTR>(pid->sID));
   
    spFinding = NULL;

    m_spRevision->OtherFindings->Remove(static_cast<LPCTSTR>(pid->sID));
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_DELETE_FINDING, e.Error());
    return;
  }
  m_wndGroup.DeleteItem(iItem);
} // DeleteItem()


/**
 * delete item message handler
 **/
void ReportOtherFindings::OnRptDeleteItem()
{
  CString str; str.Format(IDS_VERIFY_USER_OPERATION_FORMAT, 
    LoadResourceString(IDS_DELETE), LoadResourceString(IDS_FINDING));
  if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDNO)
  {
    return;
  }
  
  int4 iItem = m_wndGroup.GetNextItem(-1, LVNI_SELECTED);
  if (iItem == -1)
  {
    return;
  }

  HideSubFinding();
  DeleteItem(iItem);

  int4 iNextItem = iItem;
  int4 iPrevItem = iItem - 1;

  if (iNextItem < m_wndGroup.GetItemCount() && !m_wndGroup.IsGroup(iNextItem))
  {
    m_wndGroup.SetItemState(iNextItem, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
  }
  else if (iPrevItem > 0 && !m_wndGroup.IsGroup(iPrevItem))
  {
    m_wndGroup.SetItemState(iPrevItem, LVIS_SELECTED|LVIS_FOCUSED, 0X000F);
  }
  else
  {
    m_wndGroup.DeleteItem(iPrevItem);  
  }

  CRect rc;
  m_wndGroup.GetWindowRect(rc);
  ScreenToClient(rc);
  RedrawWindow(rc);
} // OnRptDeleteItem()


/**
 * delete all items message handler
 **/
void ReportOtherFindings::OnRptDeleteAllItems()
{
  CString str; str.Format(IDS_VERIFY_USER_OPERATION_ALL_FORMAT, 
    LoadResourceString(IDS_DELETE), LoadResourceString(IDS_OTHER_FINDINGS));
  if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDNO)
  {
    return;
  }

  HideSubFinding();

  int4 i(0);

  // Find all non-group items
  std::list<int4> itemIndexList;
  for (i = 0; i < m_wndGroup.GetItemCount(); i++)
  {
    if (!m_wndGroup.IsGroup(i))
    {
      itemIndexList.push_front(i);
    }
  }
  // Delete all non-group items
  for (std::list<int4>::const_iterator it = itemIndexList.begin(); it != itemIndexList.end(); it++)
  {
    DeleteItem(*it);
  }
  // Delete any remaining items (which will all be groups)
  m_wndGroup.DeleteAllItems();

  CRect rc;
  m_wndGroup.GetWindowRect(rc);
  ScreenToClient(rc);
  RedrawWindow(rc);
} // OnRptDeleteAllItems()


/**
 * query for item in group
 * @param iGroupID   item to query
 * @return           true if item belongs to group
 */
bool ReportOtherFindings::IsItemInGroup(const int4 iGroupID)
{
  for (int4 iItem(0); iItem < m_wndGroup.GetItemCount(); iItem++)
  {
    if (m_wndGroup.GetItemData(iItem) == iGroupID)
      return true;
  }

  return false;
} // IsItemInGroup(const int4 iGroupID)

// Revision History:
// $Log: reportotherfindings.c,v $
// Revision 1.4.2.5  2008/12/20 00:42:26  jmeade
// changed an id.
//
// Revision 1.4.2.4  2008/03/12 20:37:19  jmeade
// Issue 6028: Don't store overall extra-colonic c-rads category; that is begging for an out-of-date value.  Compute when needed.
//
// Revision 1.4.2.3  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.4.2.2  2007/09/12 15:46:06  jmeade
// fixed crash.
//
// Revision 1.4.2.1  2007/04/27 02:19:34  jmeade
// Issue 5608: one overall extra-colonic c-rads category in the miscellaneous view.
//
// Revision 1.4  2007/03/10 03:19:30  jmeade
// code standards.
//
// Revision 1.3  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.2.2.2  2003/04/09 22:31:35  jmeade
// Hide subheading, proper delete all call.
//
// Revision 3.2.2.1  2003/04/09 21:44:01  jmeade
// Delete all other findings option; Coding standards.
//
// Revision 3.2  2003/01/21 19:40:34  jmeade
// Duplicate: Revision : 3.1.2.1: Strings to resource.
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/04 05:14:01  jmeade
// Code standards.
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 20 2001 18:42:56   jmeade
// Popup menu naming
// 
//    Rev 2.0   Sep 16 2001 23:41:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:38   ingmar
// Initial revision.
// Revision 1.15  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.14  2001/05/09 14:14:41  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.13  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.12  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.11  2001/02/05 20:52:07  jeff
// more strings to resource table
//
// Revision 1.10  2001/02/05 14:32:12  geconomos
// Implementation of ReportVariant
//
// Revision 1.9  2001/02/02 08:06:49  jeff
// Moving strings to resource table
//
// Revision 1.8  2001/02/01 22:29:22  geconomos
// Fixed bug with not saving current view in other findings
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/reportotherfindings.c,v 1.4.2.5 2008/12/20 00:42:26 jmeade Exp $
// $Id: reportotherfindings.c,v 1.4.2.5 2008/12/20 00:42:26 jmeade Exp $
