// $Id: ViatronMenus.C,v 1.26.2.20 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronMenus.C  
// Description: Menu handlers for CViatronApp, MainAppFrame, ViatronViewWnd
// Owner: Jeff Meade  @jmeade@viatronix.net


#include "stdafx.h"
#include "Viatron.h"
#include "MainAppFrame.h"
#include "WinUtils.h"
#include "Crypt.h"

#include "ViatronViewWnd.h"
#include "ViatronChildView.h"
#include "OverView.h"
#include "SliceChildView.h"
#include "NavigationView.h"
#include "CadUtils.h"
#include "vcUtils.h"
#include "Pdf.h"
#include "ApplicationSettings.h"
#include "vxPacsLibrary.h"
#include "ServiceLocator.h"
#include "ProgressDlg.h"
#include "ThreadTask.h"
#include <mmsystem.h>
#include "Exception.h"
#include "Database.h"
#include "User.h"

#define FILE_REVISION "$Revision: 1.26.2.20 $"

using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;


/////////////////////////////////////////////////////////////////////////////
// CViatronApp menu items

/////////////////////////////////////////////////////////////////////////////
// MainAppFrame user account menu items


/**
 * Displays User account admininstration dialog
 */
void MainAppFrame::OnUsersMenu()
{
} // OnUsersMenu()


/**
 * Logs out the current user
 */
void MainAppFrame::OnLogoutUser()
{
  // re-run the program, which will bring up the splash-login screen
  char szFilename[_MAX_PATH+1];
  GetModuleFileName(NULL, szFilename, _MAX_PATH);
  SendMessage(WM_CLOSE, 0, 0);
  ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", szFilename, NULL, NULL, SW_SHOWNORMAL);
} // OnLogoutUser()


/**
 * Changes the current user password
 */
void MainAppFrame::OnChangeUserPassword()
{
} // OnChangeUserPassword()


/**
 * Enables the logout user menu item if login is enabled
 *
 * @param pCmdUI   Pointer to command update instance
 */
void MainAppFrame::OnUpdateUsrLogoutMenu(CCmdUI* pCmdUI)
{
  //CMenu * pMenu = (pCmdUI->m_pSubMenu != (CMenu *) NULL) ? pCmdUI->m_pSubMenu : pCmdUI->m_pMenu;
  //if (!pMenu)
  //{
  //  return;
  //}

  //// Add the current user's name to the Logout menu item
  //CString sLogout;
  //sLogout.Format(IDS_LOGOUT_USER_FORMAT, VxUserAdmin::GetCurrentUserFullName().c_str());
  //pMenu->ModifyMenu(IDM_USR_LOGOUT, MF_BYCOMMAND | MF_STRING, pCmdUI->m_nID, sLogout);

  //// Enable item only if login is enabled
  //pCmdUI->Enable(VxUserAdmin::IsUserCheckEnabled());
} // OnUpdateUsrLogoutMenu()


/**
 * Enables account mgmt for service and admin users
 *
 * @param pCmdUI   Pointer to command update instance
 */
void MainAppFrame::OnUpdateUserAdminMenu(CCmdUI* pCmdUI)
{
  //const UserTypeEnum eUserType(VxUserAdmin::GetCurrentUserType());
  //pCmdUI->Enable((eUserType == UserType_Administrator) || (eUserType == UserType_Service));
} // OnUpdateUserAdminMenu()


/**
 * Enables the change password menu item if login is enabled
 *
 * @param pCmdUI   Pointer to command update instance
 */
void MainAppFrame::OnUpdateUsrPasschangeMenu(CCmdUI* pCmdUI)
{
//  pCmdUI->Enable((VxUserAdmin::IsUserCheckEnabled() && (VxUserAdmin::GetCurrentUserID() >= 0)) ? TRUE : FALSE);
} // OnUpdateUsrPasschangeMenu()


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bookmarks, annotations, measurements

/**
 * IDM_BOOKMARKS_ADDBOOKMARK: Menu handler, adds a new bookmark at current position
 */
void ColonViatronixWnd::OnBookmarksAddBookmark()
{
  GLChildView::GLVID idMainID(SendMessage(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID,0,0));
  if (idMainID <= 0) { idMainID = GetCurrentView().GetClassInstanceID(); }

  GLChildViewRef childView = GetChildView(idMainID);
  if (childView.GetViewWndType() != GLChildView::eGVT_NONE)
  {
    Dataset& currDataset = ((ViatronChildView&)childView).GetDatasetRef();

    CString bknm;
    bknm.Format(IDS_BOOKMARK_NAME_FORMAT, currDataset.GetCurrentDistanceFromRectumMM() / 10.0f);
    Bookmark bm;
    bm.LoadCurrentPos(currDataset, LPCTSTR(bknm));
    currDataset.m_session.m_bookmarkList.AddBookmark(bm);
  }
  else
  {
    Dataset& currDataset = *rdrGlobal.m_pCurrDataset;

    // add supine bkmk
    if (currDataset.IsLoadingComplete())
    {
      CString bknm;
      bknm.Format(IDS_BOOKMARK_NAME_FORMAT, currDataset.GetCurrentDistanceFromRectumMM() / 10.0f);
      currDataset.m_session.m_bookmarkList.AddBookmark(Bookmark().LoadCurrentPos(currDataset, LPCTSTR(bknm)));
    }
  }

  PlaySound(LPCTSTR(IDW_BOOKMARK_DROP), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);

  // Add temporary (quiet) notification that a bookmark been added
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_DISPLAY_HELP_TEXT, IDS_BOOKMARK_ADDED, 1);
  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
} // OnBookmarksAddBookmark()


/**
 * IDM_BOOKMARKS_ADDBOOKMARK: Updates Add Bookmark menu item
 *
 * @param pCmdUI   Menu command update handler instance
 */
void ColonViatronixWnd::OnUpdateBookmarksAddBookmark(CCmdUI* pCmdUI)
{
  GLChildView::GLVID idMainID(SendMessage(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID,0,0));
  if (idMainID <= 0) { idMainID = GetCurrentView().GetClassInstanceID(); }
  GLChildViewRef childView = GetChildView(idMainID);
  Dataset& currDataset = (childView.GetClassTypeInfo() == typeid(ViatronChildView))
    ? ((ViatronChildView&)childView).GetDatasetRef() : *rdrGlobal.m_pCurrDataset;

  // Enable if a study is loaded and an overview child exists in this view
  if (currDataset.IsLoadingComplete())
  {
    for (int4 i(0); i < NumGLViews(); i++)
    {
      if (typeid(GetChildViewByPos(i)) == typeid(OverView))
      {
        pCmdUI->Enable(TRUE);
        return;
      }
    }

    for (CWndPtrList::const_iterator it = m_controlPtrList.begin(); it != m_controlPtrList.end(); it++)
    {
      if ( (typeid(*(*it)) == typeid(ViatronixChildWnd)) && (typeid(GLChildViewRef((ViatronixChildWnd&)(*(*it)))) == typeid(OverView)) )
      {
        pCmdUI->Enable(TRUE);
        return;
      }
    }
  }

  pCmdUI->Enable(FALSE);
} // OnUpdateBookmarksAddBookmark()


/**
 * Returns eBOTH if both datasets are visible (on the current view), or indicates which sole dataset is visible
 */
DatasetOrientations::TypeEnum ColonViatronixWnd::GetVisibleDatasets(ViatronixWnd& parentWnd)
{
  std::set<DatasetOrientations::TypeEnum> eDSetPosSet;
  for (uint4 i(0); i < parentWnd.NumGLViews(); i++)
  {
    ViatronChildView& childView = (ViatronChildView&) parentWnd.GetChildViewByPos(i);
    if (childView.GetViewWndType() != GLChildView::eGVT_NONE)
    {
      eDSetPosSet.insert(childView.GetDatasetRef().GetOrientation());
    }
  }

  for (CWndPtrList::const_iterator it = parentWnd.m_controlPtrList.begin(); it != parentWnd.m_controlPtrList.end(); it++)
  {
    const type_info& controlType = typeid(*(*it));
    if (controlType == typeid(ViatronixChildWnd))
    {
      DatasetOrientations::TypeEnum eType = GetVisibleDatasets((ViatronixChildWnd&) **it);
      if (eType != DatasetOrientations::eDEFAULT)
      {
        eDSetPosSet.insert(eType);
      }
    }
  }

  switch (eDSetPosSet.size())
  {
    case 1:
      return *eDSetPosSet.begin();
    case 0:
      return DatasetOrientations::eDEFAULT;
    default:
      return DatasetOrientations::eBOTH;
  }
} // GetVisibleDatasets()


/**
 * IDM_BOOKMARKS_DELETEALLBOOKMARKS: Menu handler, deletes all bookmarks
 */
void ColonViatronixWnd::OnBookmarksDeleteAllBookmarks()
{
  DeleteAllBookmarks(GetVisibleDatasets(*this));

  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
} // OnBookmarksDeleteAllBookmarks()


/**
 * Prompts user and deletes list of bookmarks from the specified dataset(s)
 *
 * @param eDatasetPos   Dataset(s) to delete bookmark list
 * @return              Success/failure
 */
bool ColonViatronixWnd::DeleteAllBookmarks(const DatasetOrientations::TypeEnum eDatasetPos)
{

  // form the confirmation message
  CString sMsg;
  if ( ( eDatasetPos & DatasetOrientations::eSUPINE ) &&
       ( eDatasetPos & DatasetOrientations::ePRONE  ) )
  {
    sMsg.Format( IDS_BOTHDATASETS_DELETEALL_FORMAT, LoadResourceString(IDS_BOOKMARK_LIST).MakeLower() );
  }
  else
  {
    sMsg.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_BOOKMARK_LIST));
  }

  // confirm
  if (AfxMessageBox(sMsg, MB_ICONQUESTION | MB_YESNO) != IDYES)
  {
    return false;
  }

  // delete
  if ((eDatasetPos & DatasetOrientations::eSUPINE) && rdrGlobal.m_supine.IsLoadingComplete())
  {
    rdrGlobal.m_supine.m_session.m_bookmarkList.Reset();
    AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
  }
  if ((eDatasetPos & DatasetOrientations::ePRONE) && rdrGlobal.m_prone.IsLoadingComplete())
  {
    rdrGlobal.m_prone.m_session.m_bookmarkList.Reset();
    AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
  }

  return true;
} // DeleteAllBookmarks()


/**
 * IDM_BOOKMARKS_DELETEALLBOOKMARKS: Updates Delete All Bookmarks menu item
 *
 * @param pCmdUI   Menu command update handler instance
 */
void ColonViatronixWnd::OnUpdateBookmarksDeleteAllBookmarks(CCmdUI* pCmdUI)
{
  DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  BOOL bEnable(FALSE);
  if (eVisOrient & DatasetOrientations::eSUPINE)
  { bEnable |= (rdrGlobal.m_supine.m_session.m_bookmarkList.size() > 0) ? TRUE : FALSE;  }
  if (eVisOrient & DatasetOrientations::ePRONE)
  { bEnable |= (rdrGlobal.m_prone.m_session.m_bookmarkList.size() > 0) ? TRUE : FALSE;  }

  pCmdUI->Enable(bEnable);
} // OnUpdateBookmarksDeleteAllBookmarks()


/**
 * Menu handler, deletes all measurements
 */
void ColonViatronixWnd::OnMeasurementsDeleteAll()
{
  if ( rdrGlobal.m_pCurrDataset->m_session.m_measureLineList.empty() && (rdrGlobal.m_pCurrDataset->m_suspiciousRegions.GetCount() <= 0) )
  {
    return;
  }

  // form the confirmation message
  CString sMsg;
  DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  if ( ( eVisOrient & DatasetOrientations::eSUPINE ) &&
       ( eVisOrient & DatasetOrientations::ePRONE  ) )
  {
    sMsg.Format( IDS_BOTHDATASETS_DELETEALL_FORMAT, LoadResourceString(IDS_MEASUREMENT_LIST).MakeLower() );
  }
  else
  {
    sMsg.Format(IDS_VERIFY_USER_OPERATION_ALL_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_MEASUREMENT_LIST));
  }

  // confirm
  if (AfxMessageBox(sMsg, MB_ICONQUESTION | MB_YESNO) != IDYES)
  {
    return;
  }

  // delete!
  if (eVisOrient & DatasetOrientations::eSUPINE)
  {
    rdrGlobal.m_supine.m_session.m_measureLineList.clear();
    rdrGlobal.m_supine.m_suspiciousRegions.ClearFindings(rdrGlobal.m_supine);
  }
  if (eVisOrient & DatasetOrientations::ePRONE)
  {
    rdrGlobal.m_prone.m_session.m_measureLineList.clear();
    rdrGlobal.m_prone.m_suspiciousRegions.ClearFindings(rdrGlobal.m_prone);
  }

  // update
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnMeasurementsDeleteAll()


/**
 * Updates Delete All Measurements menu item
 *
 * @param pCmdUI   Menu command update handler instance
 */
void ColonViatronixWnd::OnUpdateMeasurementsDeleteAll(CCmdUI* pCmdUI)
{
  DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  BOOL bEnable(FALSE);
  if (eVisOrient & DatasetOrientations::eSUPINE)
  {
    bEnable |= (rdrGlobal.m_supine.m_session.m_measureLineList.size() > 0) ? TRUE : FALSE;
    bEnable |= (rdrGlobal.m_supine.m_suspiciousRegions.GetCount() > 0) ? TRUE : FALSE;
  }
  if (eVisOrient & DatasetOrientations::ePRONE)
  {
    bEnable |= (rdrGlobal.m_prone.m_session.m_measureLineList.size() > 0) ? TRUE : FALSE;
    bEnable |= (rdrGlobal.m_prone.m_suspiciousRegions.GetCount() > 0) ? TRUE : FALSE;
  }

  pCmdUI->Enable(bEnable);
} // OnUpdateMeasurementsDeleteAll()


/**
 * IDM_NOTATIONS_ANNOTATIONS_DELETEALLANNOTATIONS: Menu item handler:  Deletes all annotations.
 */
void ColonViatronixWnd::OnAnnotationsDeleteAllAnnotations()
{
  // form the confirmation message
  CString sMsg;
  DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  if ( ( eVisOrient & DatasetOrientations::eSUPINE ) &&
       ( eVisOrient & DatasetOrientations::ePRONE  ) )
  {
    sMsg.Format( IDS_BOTHDATASETS_DELETEALL_FORMAT, LoadResourceString(IDS_ANNOTATIONS_LIST).MakeLower() );
  }
  else
  {
    sMsg.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_ANNOTATIONS_LIST));
  }

  // confirm
  if (AfxMessageBox(sMsg, MB_ICONQUESTION | MB_YESNO) != IDYES)
  {
    return;
  }

  // delete
  if (eVisOrient & DatasetOrientations::eSUPINE) { rdrGlobal.m_supine.m_session.m_2DAnnotationList.clear(); }
  if (eVisOrient & DatasetOrientations::ePRONE)  { rdrGlobal.m_prone.m_session.m_2DAnnotationList.clear(); }

  // update
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnAnnotationsDeleteAllAnnotations()


/**
 * Edit the user-preferred name for quick drop annotations
 */
void ColonViatronixWnd::On2DAnnotationsEditQuickDropName()
{
  SliceChildView::EditQuickDropAnnotationName();
} // On2DAnnotationsEditQuickDropName()


/**
 * IDM_NOTATIONS_ANNOTATIONS_DELETEALLANNOTATIONS: Updates Delete All annotations menu.
 *
 * @param pCmdUI   Command update instance.
 */
void ColonViatronixWnd::OnUpdateAnnotationsDeleteAllAnnotations(CCmdUI * pCmdUI)
{
  DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  BOOL bEnable(FALSE);
  if (eVisOrient & DatasetOrientations::eSUPINE)
  { bEnable |= (rdrGlobal.m_supine.m_session.m_2DAnnotationList.size() > 0) ? TRUE : FALSE;  }
  if (eVisOrient & DatasetOrientations::ePRONE)
  { bEnable |=  (rdrGlobal.m_prone.m_session.m_2DAnnotationList.size() > 0) ? TRUE : FALSE;  }

  pCmdUI->Enable(bEnable);
} // OnUpdateAnnotationsDeleteAllAnnotations()


/**
 * IDM_GOTO_BOOKMARK_FIRST: Menu item handler:  Jump to bookmark.
 *
 * @param uCmdID   Menu item ID of bookmark item.
 */
void ColonViatronixWnd::OnBookmarksGotoBookmark(UINT uCmdID)
{
  const int4 iBookmarkToGoTo(uCmdID - IDM_GOTO_BOOKMARK_FIRST);
  rdrGlobal.m_pCurrDataset->m_session.m_bookmarkList.StopAtBookmark(iBookmarkToGoTo, *rdrGlobal.m_pCurrDataset);
  ViatronStudy::TurnOffAutoflight();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnBookmarksGotoBookmark()


/**
 * IDM_BOOKMARKS_BOOKMARK_LIST: Updates list of bookmarks in menu
 *
 * @param pCmdUI   Menu command update handler instance
 */
void ColonViatronixWnd::OnUpdateBookmarksList(CCmdUI* pCmdUI)
{
  UpdateMenuList(rdrGlobal.m_pCurrDataset->m_session.m_bookmarkList, *pCmdUI,
    IDM_GOTO_BOOKMARK_FIRST, IDM_GOTO_BOOKMARK_LIST_MAX);
} // OnUpdateBookmarksList()


/**
 * IDM_GOTO_ANNOTATION_FIRST: Menu item:  Jump to Annotation
 *
 * @param uCmdID   Menu item ID of annotation item.
 */
void ColonViatronixWnd::OnNotationsAnnotationGotoAnnotation(UINT uCmdID)
{
  const int4 iAnnotationToGoTo(uCmdID - IDM_GOTO_ANNOTATION_FIRST);
  rdrGlobal.m_pCurrDataset->m_session.m_2DAnnotationList.StopAtBookmark(iAnnotationToGoTo, *rdrGlobal.m_pCurrDataset);
  ViatronStudy::TurnOffAutoflight();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnNotationsAnnotationGotoAnnotation()


/**
 * IDM_GOTO_ANNOTATION_FIRST: Update the menu item list of annotations.
 *
 * @param pCmdUI   Command update handler.
 */
void ColonViatronixWnd::OnUpdateNotationsAnnotationList(CCmdUI* pCmdUI) 
{
  UpdateMenuList(rdrGlobal.m_pCurrDataset->m_session.m_2DAnnotationList, *pCmdUI,
    IDM_GOTO_ANNOTATION_FIRST, IDM_GOTO_BOOKMARK_LIST_MAX);
} // OnUpdateNotationsAnnotationList()


/**
 * Gets a display string for a bookmark menu item.
 *
 * @param uIndex   Index of the bookmark.
 * @param sText    Text to display along with bookmark number.
 * @return         Bookmark menu item string for display.
 */
std::string GetBkmDisplayStr(const uint1 uIndex, const std::string& sText)
{
  static const int1 sCRLF[3] = { 13,10,0 };
  int1 sDisplay[111];
  enum { eMaxDisplayStringLen = 90 };
  std::string::size_type iEndPos(sText.find_first_of(sCRLF));
  if (iEndPos == std::string::npos)   iEndPos = eMaxDisplayStringLen;

  sprintf(sDisplay, "&%d %s", uIndex, sText.substr(0, iEndPos).c_str());
  if (sText.length() > iEndPos)   strcat(sDisplay, "...");
  return sDisplay;
} // GetBkmDisplayStr()


/**
 * IDM_NOTATIONS_3DMEASUREMENTS_LIST_FIRST: list update
 *
 * @param pCmdUI   Command update handler.
 */
void ColonViatronixWnd::OnUpdateNotations3DMeasurements(CCmdUI * pCmdUI)
{
  std::list<std::string> itemList;
  Point<float> ptFindingWorld;
  Point<float4> ptViewWorld; Normal<float4> viewDir; Vector<float4> upDir;
  ReaderLib::Dataset & rDataset = *rdrGlobal.m_pCurrDataset;

  for (MeasureLineList::const_iterator itLine = rDataset.m_session.m_measureLineList.begin();
    itLine != rDataset.m_session.m_measureLineList.end(); ++itLine)
  {
    itemList.push_front(std::string(LPCTSTR(LoadResourceString(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE))) + ToAscii(": ") + std::string(*itLine));
  }
	
  for (int4 iSRTCt(0); iSRTCt < rDataset.m_suspiciousRegions.GetCount(); iSRTCt++)
  {
    vxCAD::vxCadFinding rFinding = rDataset.m_suspiciousRegions.GetFinding(iSRTCt);
    itemList.push_back(std::string(LPCTSTR(LoadResourceString(IDC_CDB_REGIONSEGMENT_BTN))) + ToAscii(" ") + ToAscii(iSRTCt+1));
  }

  UpdateMenuList(itemList, *pCmdUI, IDM_GOTO_3DMEASUREMENTS_FIRST, IDM_GOTO_BOOKMARK_LIST_MAX);
  pCmdUI->Enable(!itemList.empty() ? TRUE : FALSE);
} // OnUpdateNotations3DMeasurements()


/**
 * IDM_NOTATIONS_3DMEASUREMENTS_LIST_FIRST: Menu item:  Jump to measure
 *
 * @param uCmdID   Menu item ID of annotation item.
 */
void ColonViatronixWnd::OnNotations3DMeasurementsGotoMeasurement(UINT uCmdID)
{
  Dataset & rDataset = *rdrGlobal.m_pCurrDataset;
  uCmdID -= IDM_GOTO_3DMEASUREMENTS_FIRST;

  Point<float> ptFindingWorld;

	bool bIsSessionVerNewer4_2(rDataset.m_session.GetVersion() > 4.1F);

	if (uCmdID < rDataset.m_session.m_measureLineList.size())
	{
		int4 k, K = rDataset.m_session.m_measureLineList.size();
		MeasureLineList::const_iterator it = rDataset.m_session.m_measureLineList.end();
		for (--it, k=0; (uCmdID > 0) && (k < K); k++, --it, --uCmdID)
		{
		} //for

		if (bIsSessionVerNewer4_2)
		{
			if (rDataset.IsLoadingComplete())
			{
				rDataset.SelectSegment(it->m_position);
				rDataset.m_submarine.SetPositionWorld(it->m_position);
				rDataset.m_submarine.SetUpDirection(it->m_upDirection);
				rDataset.m_submarine.SetOrientation(it->m_orientation);
				rDataset.m_submarine.UpdatePosition();
				rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
			} // if

			// get the other dataset
			Dataset & rSecondary = (&rDataset == &rdrGlobal.m_supine) ? rdrGlobal.m_prone : rdrGlobal.m_supine;

			if (rdrGlobal.m_bSPLocked)
			{
				rdrGlobal.UpdateWorldPosition(rDataset, rSecondary);
			}

			// stop both datasets
			rDataset.m_submarine.FullStop();
			rSecondary.m_submarine.FullStop();

			ViatronStudy::TurnOffAutoflight();
			InvalidateViews(WPARAM(GLChildView::eGVT_ALL), 0);
		}
		else
		{
			Box<float4> lineBox(it->m_measureEnds.m_start, it->m_measureEnds.m_end);

			ptFindingWorld = lineBox.GetMidPoint();
		}
	}
  else
  {
    uCmdID -= rDataset.m_session.m_measureLineList.size();

    if ( uCmdID < rDataset.m_suspiciousRegions.GetCount() )
    {
      vxCAD::vxCadFinding rFinding = rDataset.m_suspiciousRegions.GetFinding(uCmdID);
      ptFindingWorld = Point<float4>(rFinding.GetCentroidMM().GetVector());
    }
    else
    {
      return;
    }
  
		bIsSessionVerNewer4_2 = false;
  }

	if (!bIsSessionVerNewer4_2)
	{
		Point<float4> ptViewWorld; Normal<float4> viewDir; Vector<float4> upDir;
    GetCameraAngleFromWorldPt(rDataset, ptFindingWorld, ptViewWorld, viewDir, upDir);

    rDataset.m_submarine.SetCameraWorld(ptViewWorld, viewDir, Normal<float4>(upDir));
    rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();

    // scroll the orthogonal slices to the centroid
    rDataset.m_vOrthogonalPtWorld = ptFindingWorld;
    rDataset.m_vLastUpdatedOrthoPtWorld = rDataset.m_submarine.GetPositionWorld();
    // update the cross-section sliding bar position
    rDataset.m_iXsectionScrollBarLocationIndex = rDataset.m_joinedSkeleton.ConvertToJoinedIndex( rDataset.m_segmentList.GetCurrentSegment(), 
    rDataset.m_fields.GetClosestSkeletonIndex(rDataset.m_submarine.GetPositionVolume()) );
	} // if

} // OnNotations3DMeasurementsGotoMeasurement()


#include "RenderUtil.h"
/**
 * get camera location and orientation from which to view a given point
 *
 * @param rDataset        intended dataset
 * @param ptWorld         world location of point to look at
 * @param outCamLoc       (output) world location for camera
 * @param outCamOrient    (output) orientation for camera
 * @param outCamUp        (output) up direction for camera
 */
void ColonViatronixWnd::GetCameraAngleFromWorldPt(Dataset & rDataset, const Point<float4> & ptWorld, 
                                                  Point<float4> & outCamLoc, Normal<float4> & outCamOrient, Vector<float4> & outCamUp)
{

  Point<uint2> ptFindingVol = rDataset.m_fields.m_volumeRender.GetWorldToVolumeMatrix() * ptWorld;

  Point<uint2> ptViewVol;

  bool bFound = rDataset.m_fields.FindClosestValidCSI(ptFindingVol, 10, 100, ptViewVol);
  vxBlockVolumeIterator<uint2> insIter( rDataset.m_fields.m_csi );
  uint2 iSkelNode = insIter.Peek( ptViewVol.m_x, ptViewVol.m_y, ptViewVol.m_z );

  Segment rSeg = rDataset.m_segmentList.GetSegment(rDataset.GetSortedSegmentIndex(ptViewVol));
  const SkeletonNode & rSkelNode = rSeg.GetCurrentSkeleton().GetSkelNode(iSkelNode);

  outCamLoc = rDataset.m_fields.m_volumeRender.GetVolumeToWorldMatrix() * ptViewVol;
  outCamOrient = Normal<float4>( ptWorld - outCamLoc );

  Normal<float4> testUp(1,0,0);
  float4 fDot = Vector<float4>(outCamOrient).Dot(testUp);
  // TODO test for -1 and 1 dot products (unlikely case)
  outCamUp  = Vector<float4>(testUp) - Vector<float4>(outCamOrient) * fDot;

} // GetCameraAngleFromWorld()


/**
 * Updates the given menu with the specified list of bookmarks.
 *
 * @param itemList       List of bookmarks/annotations.
 * @param cmdUI          Menu update handler.
 * @param uFirstMenuID   ID of the first menu item that holds the list of bookmarks.
 * @param uMaxNumItems   Maximum number of bookmark menu items.
 * @return               The number of items added.
 */
template<class T>
int4 ColonViatronixWnd::UpdateMenuList(const std::list<T>& itemList, CCmdUI& cmdUI, 
                                    const uint4 uFirstMenuID, const uint4 uMaxNumItems)
{
  CMenu *pMenu = (cmdUI.m_pSubMenu != (CMenu *) NULL) ? cmdUI.m_pSubMenu : cmdUI.m_pMenu;
  if (pMenu == NULL)
  {
    return 0;
  }

  // Delete all previous bookmark items EXCEPT the first (position 0)
  for (int4 iCt(1); iCt < uMaxNumItems; iCt++)
  {
    if (pMenu->GetMenuState(cmdUI.m_nID + iCt,0) == 0xFFFFFFFF)
    {
      // Stop when you reach end of menu's list of bookmarks
      break;
    }

    pMenu->DeleteMenu(cmdUI.m_nID + iCt, MF_BYCOMMAND);
  } // Deleting previous bookmark list menu items

  if (itemList.empty())
  {
    pMenu->ModifyMenu(cmdUI.m_nID, MF_STRING | MF_BYCOMMAND, uFirstMenuID, LoadResourceString(IDS_EMPTY_LIST));
    cmdUI.Enable(FALSE);
    return 0;
  }

  std::list<T>::const_reverse_iterator bmIt = itemList.rbegin();
  const std::list<T>::size_type uListSize(itemList.size());
  const UINT uFlags(MF_STRING | MF_BYCOMMAND);
  UINT uCurrItemID(uFirstMenuID + uListSize - 1);

  pMenu->ModifyMenu(cmdUI.m_nID, uFlags, uCurrItemID, GetBkmDisplayStr(uListSize, std::string(*bmIt)).c_str());

  int4 iItemCount(0);
	int4 iCt(0);
  for (iCt = uListSize-1, --uCurrItemID, ++bmIt; bmIt != itemList.rend(); --iCt, --uCurrItemID, ++bmIt, ++iItemCount)
  {
    // Insert menu item for the bookmark prior to the previously-added item ID
    pMenu->InsertMenu(uCurrItemID + 1, uFlags, uCurrItemID, GetBkmDisplayStr(iCt, std::string(*bmIt)).c_str());
  } // Adding menu items for current bookmark list

  // Add a menubar break every 20 items
  CString sMenuItem;
  for (iCt = 21; iCt <= uListSize; iCt += 20)
  {
    uCurrItemID = uFirstMenuID+iCt-1;
    pMenu->GetMenuString(uCurrItemID, sMenuItem, MF_BYCOMMAND);
    pMenu->ModifyMenu(uCurrItemID, uFlags | MF_MENUBARBREAK, uCurrItemID, sMenuItem);
  }

  return iItemCount;
} // UpdateMenuList()


/**
 * Do the final job to mark the report complete:
 * session will be marked as read only too
 *
 * @param return true if save session successful
 */
bool ColonViatronixWnd::CompleteReport()
{ 
	/// check user right for marking report complete
  if( !User::GetCurrentUser().IsAllowed( "CompleteReport" ) )
	{
		return false;
	} 

  std::string sOriginalAuthorName = VxVCReport::m_spRevision->Info->Author;

	/// check user right for marking report complete
	if( User::GetCurrentUser().GetFullName() != sOriginalAuthorName )
	{
    // TODO: Need to add prompt into the string table
		CString sPrompt("The name of the report author does not match the \nfull name of the current user.\n\nDo you want to proceed?");

		if( AfxMessageBox( sPrompt, MB_ICONQUESTION|MB_YESNO) == IDNO )
			return false;

		VxVCReport::m_spRevision->Info->Author = User::GetCurrentUser().GetFullName().c_str();
		VxVCReport::m_spRevision->Save();
		VxVCReport::WritePDFandSnapshots(true);
	} // if
		
	// the com initialization is necessary since the method will be 
	// called in a child-thread. The child-thread does not initialize
	// com for the xml output. 20120125
	LPVOID pv(NULL);
	::CoInitialize( pv );
	try
	{ // flag the session to read only
		rdrGlobal.m_supine.m_session.m_bIsReadOnly = true;
		if( rdrGlobal.IsDualDataLoaded() )
			rdrGlobal.m_prone.m_session.m_bIsReadOnly = true;

		rdrGlobal.m_bReadOnly = true;
		  
		Utilities::Reporting::MarkComplete( true ); 

		::CoUninitialize();

	} // try
	catch ( ... )
	{
	  LogErr("Exception thrown while saving session.", "ColonViatronixWnd", "ComleteReport" );
		::CoUninitialize();

		return false;
	} // catch

	return true;
} // CompleteReport()


/**
 *  Remark the completed report as incomplete
 *  The session state in database must be flipped as well.
 *
 * @param return true if save session successful
 */
bool ColonViatronixWnd::FlipCompleteReport()
{ 
  std::string sOriginalAuthorName = VxVCReport::m_spRevision->Info->Author;

	/// check user right for marking report complete
	if( User::GetCurrentUser().GetFullName() == sOriginalAuthorName )
	{
		CString sPrompt;
    sPrompt.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString( IDS_REMOVE_MARK ),
      LoadResourceString( IDS_REPORT ));

		if( AfxMessageBox( sPrompt, MB_ICONQUESTION|MB_YESNO) == IDNO )
			return false;

		// the com initialization is necessary since the method will be 
		// called in a child-thread. The child-thread does not initialize
		// com for the xml output. 20120125
		LPVOID pv(NULL);
		::CoInitialize( pv );
		try
		{ // flag the session to read only
			rdrGlobal.m_supine.m_session.m_bIsReadOnly = false;
			if( rdrGlobal.IsDualDataLoaded() )
				rdrGlobal.m_prone.m_session.m_bIsReadOnly = false;		

			rdrGlobal.m_bReadOnly = false;

			Utilities::Reporting::MarkComplete( false );

		  ::CoUninitialize();
		
		} // try
		catch ( ... )
		{
			LogErr("Exception thrown while saving session.", "ColonViatronixWnd", "FlipComleteReport" );
			::CoUninitialize();

			return false;
		} // catch

	} 
	else
	{
		AfxMessageBox("You are not the original author for this report.\n\nOnly the original author is allowed to change a completed report.");
		return false;
	}
	
	return true;
} // FlipCompleteReport()



/**
 * Message: ID_REPORT_MARKCOMPLETE: Toggles the report complete state.
 */
void ColonViatronixWnd::OnToggleReportCompleteMark()
{			
  try
  {
    const bool bComplete(VxVCReport::IsReportMarkedCompleted(ReportLayout::GetReportFileName()) || rdrGlobal.m_bReadOnly );

 	  if( bComplete )
		{

 			bool bIsAllowToUnmark = false;
			if( rdrGlobal.IsLoadingComplete() )
			{
				bIsAllowToUnmark = FlipCompleteReport();
			} // if loading is complete.

			try
			{
				// Complete the Session
				if ( bIsAllowToUnmark && !rdrGlobal.m_supine.GetSessionUID().empty() ) 
            ServiceLocator::GetRetrievalService()->ReopenSession( ViatronApp::GetViatronApp()->GetContextId(), rdrGlobal.m_supine.GetSessionUID() );
			}
			catch ( ... )
			{ // Log Error
				LogErr( "Failed to change session state from Complete to Incomplete", "OnToggleReportCompleteMark", "ViatronMenus" );
			} // END ... catch      

		} // if ( bComplete )
		else
		{

	
			CString sLockInfo( "Report and Session will be PERMANENTLY locked as Read Only.\nNo further change will be allowed." );

			{ // Check for percentage of colon's surface covered, take the highest covered among both datasets
				const float4 fPercCovg = (Max(rdrGlobal.m_supine.GetPercentExamined(), rdrGlobal.m_prone.GetPercentExamined())+0.005f) * 100.0f;
				if (fPercCovg < 95.0f)
				{
					CString sLumenCvg;
					sLumenCvg.Format(IDS_LUMEN_COVERAGE_WARNING_FORMAT, int4(fPercCovg));

					sLockInfo = sLumenCvg + "\n\n" + sLockInfo;
				}
			} // if

			Utilities::Reporting::PacsSendOptions eSendOptions( Utilities::Reporting::PACS_SEND_NONE );
			ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL ));     
			eSendOptions = Utilities::Reporting::PacsSendOptions( state.Get<int4>( APP_SETTING_SEND_TO_PACS ) );


			CString sPACSSentInfo("");
			if( eSendOptions != Utilities::Reporting::PACS_SEND_NONE )
			{
				sPACSSentInfo = "\n\nReport information will be sent to PACS.";
			} // if
			
			CString sPrompt;
			sPrompt.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(bComplete ? IDS_REMOVE_MARK : IDS_MARK),
										 LoadResourceString(bComplete ? IDS_REPORT : IDS_STUDY_AS_COMPLETED));

			sPrompt = sLockInfo + sPACSSentInfo + "\n\n" + sPrompt;

			if ( !rdrGlobal.IsLoadingComplete() || AfxMessageBox(sPrompt, MB_ICONQUESTION|MB_YESNO) == IDNO )
			{
				return;
			} // if

			{ 
				if( !CompleteReport() )
				{
					return;
				} // if save session fail, don't sent anything to PACS.

				// ==========================================================================================================
				// Wrap the Database::CompleteSession in a try catch so that we continue with the store to PACS even if the 
				// sessions flag could not be updated in the database
				// ==========================================================================================================
				try
				{
					// Complete the Session
					if ( !rdrGlobal.m_supine.GetSessionUID().empty()  ) 
            ServiceLocator::GetRetrievalService()->CompleteSession( ViatronApp::GetViatronApp()->GetContextId(), rdrGlobal.m_supine.GetSessionUID() );
				}
				catch ( ... )
				{ // Log Error
					LogErr( "Failed to complete the session", "ColonViatronixWnd", "ViatronMenu" );
				} // END ... catch      

				if( !rdrGlobal.m_supine.GetSessionUID().empty() && eSendOptions != Utilities::Reporting::PACS_SEND_NONE )
				{
					ColonViews::Dialogs::ProgressDlg progressDlg( LoadResourceString( IDS_PROGRESS_DLG_LABEL ));
					ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL )); 

					// pacs store thread task
					ThreadTask task( [&progressDlg, &state, eSendOptions]
					{ // wait for the progress dialog to be displayed
						progressDlg.Wait();

						Utilities::Reporting::SendToPacs( VxVCReport::m_spRevision, rdrGlobal.m_supine.GetSessionUID(), eSendOptions );
					});

					// start pacs store and complete report on thread end
					task.Begin( [&progressDlg, &task]
					{
						vxLogTimer timer;

						SCOPE_EXIT
						{
							progressDlg.CloseDialog();
						};

						try
						{
							task.End();
							LogFyi(util::Strings::Format("Transfer took [%ld ms]", timer.GetElapsedTime()), "ColonViatronixWnd", "OnToggleReportCompleteMark");  
						}
						catch( ... )
						{
							LogErr( "Unable to store report and images to PACS.", "ColonViatronixWnd", "OnToggleReportCompleteMark" );
							AfxMessageBox("An error occurred while transferring report to PACS. \nYou can send it from V3D-Console later.");
						}   // catch
          
					});

					progressDlg.DoModal();
				}
			}
		} // else if report is not complete

  }
  catch (...)
  {
    CString sMsg;
    sMsg.Format(IDS_ERROR_GENERIC_FORMAT, LoadResourceString(IDS_OPEN), LoadResourceString(IDS_REPORT));
    AfxMessageBox(sMsg, MB_ICONEXCLAMATION);
    return;
  }
} // OnToggleReportCompleteMark()


/**
 * Message: ID_REPORT_MARKCOMPLETE: Updates the report complete menu item.
 *
 * @param pCmdUI   Command update instance.
 */
void ColonViatronixWnd::OnUpdateToggleReportCompleteMark(CCmdUI* pCmdUI)
{
  try
  {
		pCmdUI->SetCheck(VxVCReport::IsReportMarkedCompleted() ? 1 : 0);
		pCmdUI->Enable(ReportLayout::HasPermissions() && ReportLayout::CanOpenReport()  );
  }
  catch (...)
  {
    pCmdUI->Enable(FALSE);
  }
} // OnUpdateToggleReportCompleteMark()


/**
 * Message: ID_OPTIONS_DISPLAYRENDERSPEED: Toggle display of rendering speed.
 */
void ColonViatronixWnd::OnOptionsDisplayRenderSpeed()
{
#if defined(FINAL_RELEASE)
  EndoluminalView::m_bDisplayRenderSpeed = !EndoluminalView::m_bDisplayRenderSpeed;
#endif
} // OnOptionsDisplayRenderSpeed()


/**
 * UpdateMessage: ID_OPTIONS_DISPLAYRENDERSPEED: Update render speed menu item.
 *
 * @param pCmdUI   Menu update command handler.
 */
void ColonViatronixWnd::OnUpdateOptionsDisplayRenderSpeed(CCmdUI* pCmdUI)
{
#if defined(FINAL_RELEASE)
  pCmdUI->SetCheck(EndoluminalView::m_bDisplayRenderSpeed == true);
#endif
} // OnUpdateOptionsDisplayRenderSpeed()


/**
 * Message: ID_VIEWWINDOW_PSEUDOCOLORDISPLAY: Toggle pseudocolor display.
 */
void ColonViatronixWnd::OnViewWindowPseudoColorDisplay()
{
  SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SliceChildView::SHOW_BIOPSY)
    ? SliceChildView::SHOW_BIOPSY : SliceChildView::SHOW_NONE;
  InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
} // OnViewWindowPseudoColorDisplay()


/**
 * Update Message: ID_VIEWWINDOW_PSEUDOCOLORDISPLAY: Updates pseudocolor display menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateViewWindowPseudoColorDisplay(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(SliceChildView::m_eDbgShow == SliceChildView::SHOW_BIOPSY ? 1 : 0);
  if (rdrGlobal.m_pCurrDataset->GetModality() == Dataset::MR)
  {
    pCmdUI->Enable(FALSE);
  }
} // OnUpdateViewWindowPseudoColorDisplay()


/**
 * Message: ID_VIEWWINDOW_SHOWCLEANSED: Toggles show cleansed option.
 */
void ColonViatronixWnd::OnViewWindowShowCleansed()
{
  SliceViewer::m_bShowUncleansed = !SliceViewer::m_bShowUncleansed;
  InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
} // OnViewWindowShowCleansed()


/**
 * Update Message: ID_VIEWWINDOW_SHOWCLEANSED: Updates show cleansed menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateViewWindowShowCleansed(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(SliceViewer::m_bShowUncleansed ? 1 : 0);

  bool bSEnable = false, bPEnable;
  const DatasetOrientations::TypeEnum eVisOrient = GetVisibleDatasets(*this);
  const Fields & fdS = rdrGlobal.m_supine.m_fields;
  const Fields & fdP = rdrGlobal.m_prone.m_fields;

  bSEnable = (eVisOrient & DatasetOrientations::eSUPINE) &&
    (fdS.m_sliceRendererData.IsValid() ? fdS.IsHQDiffVolLoaded() && fdS.IsElecCleansed() : fdS.DoesDiffVolExist());
  bPEnable = (eVisOrient & DatasetOrientations::ePRONE) &&
    (fdP.m_sliceRendererData.IsValid() ? fdP.IsHQDiffVolLoaded() && fdP.IsElecCleansed() : fdP.DoesDiffVolExist());

  pCmdUI->Enable((bSEnable == true) || (bPEnable == true));

  if (rdrGlobal.m_pCurrDataset->GetModality() == Dataset::MR)
  {
    pCmdUI->SetText(LoadResourceString(IDS_SHOWSUBTRACTED));
  }
} // OnUpdateViewWindowShowCleansed()


/**
 * Message: ID_2DDISPLAYOPTIONS_VERTICALSCROLLBARS
 */
void ColonViatronixWnd::OnToggleScrollbarOrientation()
{
  SliceChildView::SetScrollBarLocationEnum((SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT)
    ? SliceChildView::SBO_BOTTOM : SliceChildView::SBO_RIGHT);

  // force the framework to reload the layout
  MainAppFrame::GetMainFrame()->ReloadCurrentLayout();

} // OnToggleScrollbarOrientation()


/**
 * Update Message: ID_2DDISPLAYOPTIONS_VERTICALSCROLLBARS
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT);
} // OnUpdateToggleScrollbarOrientation()


/**
 * Message: ID_VIEWWINDOW_ARROWINSLICECENTER: Toggles display of arrow in center of 2D slice during flight.
 */
void ColonViatronixWnd::OnViewWindowArrowInSliceCenter()
{
  SliceViewer::m_bArrowInSliceCenter = !SliceViewer::m_bArrowInSliceCenter;
} // OnViewWindowArrowInSliceCenter()


/**
 * Update Message: ID_VIEWWINDOW_ARROWINSLICECENTER: Updates arrow in 2D slice center menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(SliceViewer::m_bArrowInSliceCenter ? 1 : 0);
  pCmdUI->Enable(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()) == true);
} // OnUpdateViewWindowArrowInSliceCenter()



/**
 * Message: ID_VIEWWINDOW_SWITCHFLIGHTDIRECTION: Toggles flight direction for both studies.
 */
void ColonViatronixWnd::OnViewWindowSwitchFlightDirection()
{
  rdrGlobal.m_supine.m_submarine.ToggleFlightDirection();
  rdrGlobal.m_prone.m_submarine.ToggleFlightDirection();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnViewWindowSwitchFlightDirection()


/**
 * Update Message: ID_VIEWWINDOW_SWITCHFLIGHTDIRECTION: Updates switch flight direction menu option.
 *
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(SliceViewer::m_bFlightAlongCenterline ? 1 : 0);
  pCmdUI->Enable(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()) == true);
} // OnUpdateViewWindowSwitchFlightDirection()


/**
 * Message: ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW: Toggles option to update of navigation view during 2D flight.
 */
void ColonViatronixWnd::OnViewWindowUpdateEndoluminalView() 
{
  SliceChildView::m_bUpdateEndoluminalView = !SliceChildView::m_bUpdateEndoluminalView;
} // OnViewWindowUpdateEndoluminalView()


/**
 * Message: ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW: Updates navigation view update during 2D flight option menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void ColonViatronixWnd::OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(SliceChildView::m_bUpdateEndoluminalView ? 1 : 0);
  pCmdUI->Enable(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()) == true);
} // OnUpdateViewWindowUpdateEndoluminalView()


/**
 * Message: ID_OPTIONS_ALWAYSSHOWCADINDICATORS: Toggle display of cad indicators.
 */
void ColonViatronixWnd::OnOptionsAlwaysShowCadIndicators()
{
  rdrGlobal.m_bAlwaysShowCadArrows = !rdrGlobal.m_bAlwaysShowCadArrows;
} // OnOptionsAlwaysShowCadIndicators()


/**
 * Update Message: ID_OPTIONS_ALWAYSSHOWCADINDICATORS: Update always show cad arrows menu option.
 *
 * @param pCmdUI   Menu update command handler.
 */
void ColonViatronixWnd::OnUpdateOptionsAlwaysShowCadIndicators(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(rdrGlobal.m_bAlwaysShowCadArrows == true);
  pCmdUI->Enable((rdrGlobal.m_supine.m_cadFindings.GetCount() > 0) || (rdrGlobal.m_prone.m_cadFindings.GetCount() > 0));

  CString sFmt(LoadResourceString(IDS_FMT_ALWAYSSHOWCADINDICATORS));
  sFmt.Replace("#<cadname>", rdrGlobal.GetSettings()->GetValue(VCSettings::ViewerXML(), VCSettings::CADDisplayName()).c_str());
  pCmdUI->SetText(sFmt);
} // OnUpdateOptionsAlwaysShowCadIndicators()


/**
 * Message: ID_CADOPTIONS_SHOWEXTRACOLONIC: Toggle display of extra colonic cad findings
 */
void ColonViatronixWnd::OnOptionsShowExtraColonicFindings()
{
  rdrGlobal.m_bShowExternalCadFindings = !rdrGlobal.m_bShowExternalCadFindings;

  if (GetChildViewByPos(0) != GetNullEventView())
    Utilities::CAD::Refresh();
} // OnOptionsShowExtraColonicFindings


/**
 * Update Message: ID_CADOPTIONS_SHOWEXTRACOLONIC: Update menu.
 *
 * @param pCmdUI   Menu update command handler.
 */
void ColonViatronixWnd::OnUpdateOptionsShowExtraColonicFindings(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(rdrGlobal.m_bShowExternalCadFindings == true);
  pCmdUI->Enable((rdrGlobal.m_supine.m_cadFindings.GetCount() > 0) || (rdrGlobal.m_prone.m_cadFindings.GetCount() > 0));

  CString sFmt(LoadResourceString(IDS_FMT_SHOWEXTRACOLONICCAD));
  sFmt.Replace("#<cadname>", rdrGlobal.GetSettings()->GetValue(VCSettings::ViewerXML(), VCSettings::CADDisplayName()).c_str());
  pCmdUI->SetText(sFmt);
} // OnUpdateOptionsShowExtraColonicFindings


/**
 * Message: ID_SRTOPTIONS_DISPLAYREGIONSOFINTEREST: Toggle display of extra colonic cad findings
 */
void ColonViatronixWnd::OnSrtOptionsDisplayRegionsOfInterest()
{
  if( ViatronStudy::m_modeOrthogView != ViewModes::SEGMENTPOLYP &&
		  ViatronStudy::m_modeObliqueView != ViewModes::SEGMENTPOLYP &&
			ViatronStudy::m_modeEndoView != ViewModes::SEGMENTPOLYP )
	{
		rdrGlobal.m_bDisplaySRTroi = !rdrGlobal.m_bDisplaySRTroi;
		rdrGlobal.m_bDisplaySRTroi2D = !rdrGlobal.m_bDisplaySRTroi2D;
	} // if

	rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnSrtOptionsDisplayRegionsOfInterest()


/**
 * Update Message: ID_SRTOPTIONS_DISPLAYREGIONSOFINTEREST: Update menu.
 *
 * @param pCmdUI   Menu update command handler.
 */
void ColonViatronixWnd::OnUpdateSrtOptionsDisplayRegionsOfInterest(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) ? TRUE : FALSE);
  pCmdUI->SetCheck(rdrGlobal.m_bDisplaySRTroi == true);
} // OnUpdateSrtOptionsDisplayRegionsOfInterest()


/**
 * Message: IDM_JUMP_PREVSEGMENT: Jumps to the next segment towards the cecum.
 */
void ColonViatronixWnd::OnJumpPrevSegment()
{
  rdrGlobal.m_pCurrDataset->GoToNextSegmentToCecum();
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnJumpPrevSegment()


/**
 * Message: IDM_JUMP_NEXTSEGMENT: Jumps to the next segment towards the rectum.
 */
void ColonViatronixWnd::OnJumpNextSegment()
{
  rdrGlobal.m_pCurrDataset->GoToNextSegmentToRectum();
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnJumpNextSegment()


/**
 * Message: IDM_TOGGLE_SUPINE_PRONE: Switches between supine and prone datasets.
 */
void ColonViatronixWnd::OnToggleSupineProne()
{
  if( rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked() ) // For Linked actions
    rdrGlobal.UpdateWorldPosition( *rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset );

  AfxGetMainWnd()->PostMessage(WM_APP_SET_MAIN_DATASET, WPARAM(DatasetOrientations::eSWAP), 0);
} // OnToggleSupineProne()


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void ColonViatronixWnd::OnToggleAutofly()
{
  ColonViatronixWnd::ToggleAutoflyMode();
} // OnToggleAutofly()


/**
 * Message: ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE: Toggles Line Measure Mode.
 */
void ColonViatronixWnd::OnToggleLineMeasure()
{
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  const bool bBiopsy(BitCheck(ViatronStudy::GetCurrentViewModes(), ViewModes::BIOPSY));

  if (ViatronStudy::GetCurrentViewModes().GetValid() & ViewModes::LINEMEASURE_VALID)
  {
    ViatronStudy::GetCurrentViewModes().Toggle(ViewModes::LINEMEASURE);
  }

  if (bBiopsy)
  {
    rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
    rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnToggleLineMeasure()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE: Toggles ROI Measure mode.
 */
void ColonViatronixWnd::OnToggleRoiMeasure()
{
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  if (ViatronStudy::GetCurrentViewModes().GetValid() & ViewModes::ROIMEASURE_VALID)
  {
    ViatronStudy::GetCurrentViewModes().Toggle(ViewModes::ROIMEASURE);
  }
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnToggleRoiMeasure()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING: Toggles lighting mode.
 */
void ColonViatronixWnd::OnToggleLighting()
{
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  const bool bBiopsy(BitCheck(ViatronStudy::GetCurrentViewModes(), ViewModes::BIOPSY));

  if (ViatronStudy::GetCurrentViewModes().GetValid() & ViewModes::LIGHTING_VALID)
  {
    ViatronStudy::GetCurrentViewModes().Toggle(ViewModes::LIGHTING);
  }

  if (bBiopsy)
  {
    rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
    rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
  InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
} // OnToggleLighting()


/**
 * Message: ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY: Toggles Translucent mode.
 */
void ColonViatronixWnd::OnToggleTranslucent()
{
  const bool bAutoFly(ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY);
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (rdrGlobal.m_pCurrDataset->GetModality() == Dataset::MR)
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  const ViewModesValue validModes(ViatronStudy::GetCurrentViewModes().GetValid());

  if ((validModes & ViewModes::BIOPSY_VALID) == ViewModes::BIOPSY_VALID)
  {
    if ( (rdrGlobal.m_supine.GetModality() != Dataset::MR) || (rdrGlobal.m_prone.GetModality() != Dataset::MR) )
    {
      rdrGlobal.m_bBiopsyMode = ViatronStudy::GetCurrentViewModes().Toggle(ViewModes::BIOPSY);

      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
      InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
    }
  }
  else if ((validModes & ViewModes::PSEUDOCOLOR_VALID) == ViewModes::PSEUDOCOLOR_VALID)
  {
    SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY,0), 0);

    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  }
} // OnToggleTranslucent()


/**
 * Message: ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION: Toggles Add annotation mode.
 */
void ColonViatronixWnd::OnToggleAnnotationMode()
{
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  //if (ViatronStudy::GetCurrentViewModes().GetValid() & ViewModes::ANNOTATION_VALID)
  {
    ToggleViewMode(ViewModes::ANNOTATION);
  }
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnToggleAnnotationMode()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION: Toggles segment polyp mode.
 */
void ColonViatronixWnd::OnToggleSegmentPolypMode()
{
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  const bool bBiopsy(BitCheck(ViatronStudy::GetCurrentViewModes(), ViewModes::BIOPSY));

  if (ViatronStudy::GetCurrentViewModes().GetValid() & ViewModes::SEGMENTPOLYP_VALID)
  {
    ViatronStudy::GetCurrentViewModes().Toggle(ViewModes::SEGMENTPOLYP);
  }

  if (bBiopsy)
  {
    rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
    rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnToggleSegmentPolypMode()


/**
 * IDM_TOGGLE_VIEW_DIRECTION: Toggles the current flight direction.
 */
void ColonViatronixWnd::OnToggleViewDirection()
{
  rdrGlobal.m_pCurrDataset->m_submarine.ToggleFlightDirection();
} // OnToggleViewDirection()


/**
 * IDM_NEXT_MISSED_REGION: Jumps to the next missed region.
 */
void ColonViatronixWnd::OnNextMissedRegion()
{
  SendMessage(WM_APP_JUMP_NEXT_MISSED_REGION, WPARAM(1), LPARAM(0));
} // OnNextMissedRegion()


/**
 * Displays flight controls
 */
void ColonViatronixWnd::OnControlPadFlightControls()
{
  CWnd *pAdjust = GetDlgItem(IDC_TEMPLATE_ADJUSTTRANSLUCENCY);
  CWnd *pControls = GetDlgItem(IDC_TEMPLATE_CONTROLS);
  if (pControls)    pControls->ShowWindow(SW_SHOW);
  if (pAdjust)      pAdjust->ShowWindow(SW_HIDE);
} // OnControlPadFlightControls()


/**
 * Updates menu item
 */
void ColonViatronixWnd::OnUpdateControlPadFlightControls(CCmdUI* pCmdUI) 
{
  CWnd *pControls = GetDlgItem(IDC_TEMPLATE_CONTROLS);
  if (pControls)    pCmdUI->SetCheck(pControls->IsWindowVisible());
} // OnUpdateControlPadFlightControls()


/**
 * Displays translucency adjustment controls
 */
void ColonViatronixWnd::OnControlPadAdjustTranslucency()
{
  CWnd *pAdjust = GetDlgItem(IDC_TEMPLATE_ADJUSTTRANSLUCENCY);
  CWnd *pControls = GetDlgItem(IDC_TEMPLATE_CONTROLS);
  if (pAdjust)      pAdjust->ShowWindow(SW_SHOW);
  if (pControls)    pControls->ShowWindow(SW_HIDE);
} // OnControlPadAdjustTranslucency()


/**
 * Updates menu item
 *
 * @param pCmdUD   Command update handler
 */
void ColonViatronixWnd::OnUpdateControlPadAdjustTranslucency(CCmdUI* pCmdUI) 
{
  CWnd *pAdjust = GetDlgItem(IDC_TEMPLATE_ADJUSTTRANSLUCENCY);
  if (pAdjust)      pCmdUI->SetCheck(pAdjust->IsWindowVisible());

  if (rdrGlobal.m_pCurrDataset->GetModality() == Dataset::MR)
    pCmdUI->Enable(FALSE);
} // OnUpdateControlPadAdjustTranslucency()


// $Log: ViatronMenus.C,v $
// Revision 1.26.2.20  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.19  2010/04/16 19:30:40  dongqing
// prevent the flash, or moment of blank space, when toggling btw dialogs
//
// Revision 1.26.2.18  2010/04/01 06:46:32  dongqing
// fixed crash when accessing cad options from report view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.17  2010/02/01 16:37:00  dongqing
// Helper function to reload the current layout
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.16  2009/12/10 20:07:50  dongqing
// cleaner reloading of layouts, for e.g. when changing scrollbar orientation
//
// Revision 1.26.2.15  2009/10/07 16:13:17  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.26.2.14  2009/01/31 00:38:35  jmeade
// Issue 6237: volume measurements in the notations pane.
//
// Revision 1.26.2.13  2009/01/23 19:21:54  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.26.2.12  2009/01/16 01:46:37  jmeade
// delete suspicious regions with measurements
//
// Revision 1.26.2.11  2009/01/15 20:35:01  jmeade
// jump slices to measurement
//
// Revision 1.26.2.10  2009/01/15 04:10:51  jmeade
// jump to 3D measurements implemented
//
// Revision 1.26.2.9  2009/01/09 03:10:09  jmeade
// volume measure interface changes.
//
// Revision 1.26.2.8  2009/01/07 01:39:02  jmeade
// volume measure interface changes.
//
// Revision 1.26.2.7  2008/11/13 02:30:08  jmeade
// fix bug where go-to-bookmark functionality failed when prone loaded as 1st dataset.
//
// Revision 1.26.2.6  2008/10/30 22:24:32  jmeade
// Issue 6129: disable only the SRT option when SRT functionality is disallowed.
//
// Revision 1.26.2.5  2008/02/04 22:35:39  jmeade
// comment.
//
// Revision 1.26.2.4  2008/01/16 01:21:18  jmeade
// Issue 5917: support for vertical scrollbars throughout application.
//
// Revision 1.26.2.3  2008/01/11 02:18:54  jmeade
// issue 5907.
//
// Revision 1.26.2.2  2007/10/03 19:59:01  jmeade
// spacing.
//
// Revision 1.26.2.1  2007/04/04 20:04:53  jmeade
// politically-correct names for CAD in menus.
//
// Revision 1.26  2007/03/01 01:18:57  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.25  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.24  2007/02/02 22:39:52  romy
// added permissions to the Menu toggling
//
// Revision 1.23  2007/01/11 02:44:36  jmeade
// comments.
//
// Revision 1.22  2007/01/10 18:37:45  jmeade
// srt option applies to both datasets.
//
// Revision 1.21  2007/01/03 05:40:32  jmeade
// Issue 5217: menu for toggle on/off display of SRT areas.
//
// Revision 1.20  2006/10/04 20:50:54  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.19  2006/09/11 14:25:47  jmeade
// Issue 4980: Ensure a valid childview with call to UpdateCad display.
//
// Revision 1.18  2006/09/07 23:55:16  jmeade
// enable segment polyp mode.
//
// Revision 1.17  2006/09/01 22:56:22  jmeade
// Issue 4955: refresh cad findings list when toggling extra-colonic findings view.
//
// Revision 1.16  2006/07/11 22:12:19  jmeade
// Issue 4868: 2d flight options enabled only if flight enabled.
//
// Revision 1.15  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.14  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.13  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.12  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.11  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.10  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.9.2.9  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.9.2.8.2.1  2007/01/22 20:36:55  jmeade
// Issue 5325: implemented updating of CAD display.
//
// Revision 1.9.2.8  2006/03/24 23:45:36  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.9.2.7  2006/03/22 22:54:58  jmeade
// language-independent strings.
//
// Revision 1.9.2.6  2006/03/14 01:06:07  jmeade
// Issue 4645: Logic error when determining when to load/display low-quality uncleansed.
//
// Revision 1.9.2.5  2006/03/08 00:17:33  jmeade
// Issue 4620: Shortcut key for annotation mode.
//
// Revision 1.9.2.4  2005/12/30 20:58:21  jmeade
// fixed bug: not deleting measurement list upon request for prone-only studies.
//
// Revision 1.9.2.3  2005/12/09 03:54:52  jmeade
// comments.
//
// Revision 1.9.2.2  2005/11/21 19:39:18  jmeade
// disable always-show-cad-findings when no cad available.
//
// Revision 1.9.2.1  2005/11/09 01:41:16  jmeade
// Issue 4042: Check cdv file for flag indicating if study processed with EC.
//
// Revision 1.9  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.8  2005/10/12 22:38:37  jmeade
// option to always show cad findings arrows in 3D
//
// Revision 1.7  2005/10/12 00:34:28  jmeade
// Issue 4468: Check internal report complete flag (instead of slow disk access).
//
// Revision 1.6  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.5.2.2  2005/09/22 13:55:40  geconomos
// Issue# 4451: Screen flashes when jumping to annotation
//
// Revision 1.5.2.1  2005/09/20 16:54:25  geconomos
// Issue# 4444: Entire screen flashes when selecting jumo to bookmark
//
// Revision 1.5  2005/09/13 13:06:21  geconomos
// Making calls to ViatronStudy::StopAutoFlight()
//
// Revision 1.4  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.7.2.14.2.14.2.6  2005/07/19 21:07:10  jmeade
// Issue 4284: Disabe translucency for MR studies.
//
// Revision 3.7.2.14.2.14.2.5  2005/07/08 19:00:07  jmeade
// privatize dataset modality variable.
//
// Revision 3.7.2.14.2.14.2.4  2005/07/06 20:57:06  jmeade
// Issue 4199: subtracted text in main menu.
//
// Revision 3.7.2.14.2.14.2.3  2005/06/30 23:58:11  jmeade
// Issue 4200: Disable translucency.
//
// Revision 3.7.2.14.2.14.2.2  2005/06/28 22:20:58  jmeade
// Issue 4200: Disable translucency adjust for MR.
//
// Revision 3.7.2.14.2.14.2.1  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.7.2.14.2.14  2005/06/10 18:20:39  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.7.2.14.2.13  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.7.2.14.2.12  2005/04/25 13:57:06  frank
// Issue #4148: Updated display after toggling pseudocolor and painting
//
// Revision 3.7.2.14.2.11  2005/04/22 19:04:10  frank
// forced an update after mode changes
//
// Revision 3.7.2.14.2.10.2.1  2005/06/08 13:59:27  geconomos
// added some refresh mechanisms
//
// Revision 3.7.2.14.2.10  2005/04/08 22:42:02  jmeade
// Issue 4042: Check whether hq diff volume loads.
// Issue 4100: Update window after toggling uncleansed option.
//
// Revision 3.7.2.14.2.9  2005/04/08 18:23:40  frank
// Issue #4113: Disabled menu items in verification view
//
// Revision 3.7.2.14.2.8  2005/04/01 18:46:53  jmeade
// removed duplicate message.
//
// Revision 3.7.2.14.2.7  2005/04/01 03:56:40  frank
// Issue #4096: fixed toggling pseudocolor display from context menu
//
// Revision 3.7.2.14.2.6  2005/03/29 15:00:48  jmeade
// Issue 3997: No 2D flight in slice verification.
//
// Revision 3.7.2.14.2.5  2005/03/28 12:10:01  frank
// Issue #4067: Changed message when deleting both series' annotation/bookmarks/measurements - still needs to be turned in into a resource.
//
// Revision 3.7.2.14.2.4  2005/03/25 13:58:52  frank
// Issue #3990: Fixed bookmark and annotation jumping with linked datasets
//
// Revision 3.7.2.14.2.3  2005/03/24 16:36:33  frank
// Issue #3986: Jumped to annotation on both supine and prone at the same time if linked.
//
// Revision 3.7.2.14.2.1  2005/03/12 14:43:06  jmeade
// Removed beta code.
//
// Revision 3.7.2.14  2005/02/15 20:11:34  jmeade
// merge from vc1.4 branch.
//
// Revision 3.7.2.13  2004/09/30 15:51:31  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.7.2.12.2.2  2004/12/06 22:42:25  jmeade
// Stop flight upon jumping to bookmark.
//
// Revision 3.7.2.12.2.1  2004/10/23 00:45:51  jmeade
// consolidate code for marking study complete
//
// Revision 3.7.2.12  2004/09/23 18:48:13  jmeade
// compile error.
//
// Revision 3.7.2.11  2004/09/23 18:37:40  jmeade
// Toggle between adjust translucency and flight controls in menu.
//
// Revision 3.7.2.10  2004/05/05 23:18:52  jmeade
// Incremental update.
//
// Revision 3.7.2.9  2003/08/11 19:44:48  jmeade
// Issue 3401: Update 2D views after switching supine/prone.
//
// Revision 3.7.2.8  2003/07/25 21:46:30  jmeade
// Issue 3401: Update 2D views after switching supine/prone.
//
// Revision 3.7.2.7  2003/04/14 16:52:23  jmeade
// Issue 2841: Handle delete all measurements msg for any visible datasets; Removed unneeded BookmarkList override.
//
// Revision 3.7.2.6  2003/03/13 00:42:45  jmeade
// Issue 2890:  Flush accum buffer after exiting biopsy mode (to other modes).
//
// Revision 3.7.2.5  2003/03/07 05:33:36  jmeade
// Issue 2878:  Delete all for bookmarks and annotations based on which datasets are visible.
//
// Revision 3.7.2.4  2003/02/27 03:34:11  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.7.2.3  2003/02/22 12:23:51  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.7.2.2  2003/02/21 18:08:41  jmeade
// Removed an essential identical app message; Method to update bookmark menu generalized to work for any std::list.
//
// Revision 3.7.2.1  2003/02/19 07:24:35  jmeade
// Issue 2848:  Localizing view mode changing code to ViatronViewWnd.
//
// Revision 3.7  2003/02/04 22:43:31  jmeade
// Cleaned up code and bugs in OnBookmarksAddBookmark().
//
// Revision 3.6  2003/01/21 21:55:08  jmeade
// Add bookmark menu item update to recognize ViatronChildWnd-embedded Overview.
//
// Revision 3.5  2002/12/24 03:45:56  jmeade
// Bug: Measure list not saved in session b/c there were two copies of it!
//
// Revision 3.4  2002/12/23 22:09:07  ingmar
// missing include.
//
// Revision 3.3  2002/12/12 22:39:16  jmeade
// Moved menu methods.
//
// Revision 3.2  2002/10/14 15:57:22  jmeade
// Call to ChangePassword must also supply previous password for authentication.
//
// Revision 3.1  2002/09/18 20:07:37  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:24   ingmar
// Initial revision.
// Revision 1.9  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.8  2001/03/07 15:09:32  jmeade
// bookmarks menu only for standard view
//
// Revision 1.7  2001/02/02 08:05:21  jeff
// Moving strings to resource table
//
// Revision 1.6  2000/11/14 18:56:21  frank
// Changed study reference to pointer
//
// Revision 1.5  2000/11/09 19:09:05  jeff
// BookmarkList::GetBookmark() is now a private method; use operator[] instead
//
// Revision 1.4  2000/11/08 21:17:47  jeff
// Corrected a char buffer overrun
//
// Revision 1.3  2000/11/08 17:39:18  frank
// Added m_study to ReaderGlobal
//
// Revision 1.2  2000/10/02 23:01:39  jeff
// Moved bookmark list to ReaderGlobal (instance)
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 6     8/28/00 1:51p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  8/14/00  Time:  2:49p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Created a new class ViatronStudy for study-specific info (bookmarks,
// narvigation)
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  8/01/00  Time:  7:03p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Don't delete the "Bookmark List" menu item; modify it with the first
// bookmark, or set grayed "Bookmark List" text (latter when no bookmarks)
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  7/28/00  Time:  5:53p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added functions for serializing bookmark list and for adding
// bookmarks thru GUI
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/24/00  Time:  7:52p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added bookmark class and associated hooks,
// (initially) only for jumping to fixed precomputed positions
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/24/00  Time:  6:21p
// Created ViatronMenus.C
// Comment:
//   Initial revision; for menu handlers
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronMenus.C,v 1.26.2.20 2010/09/20 18:41:34 dongqing Exp $
// $Id: ViatronMenus.C,v 1.26.2.20 2010/09/20 18:41:34 dongqing Exp $
