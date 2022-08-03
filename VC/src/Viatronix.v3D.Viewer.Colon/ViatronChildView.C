// $Id: ViatronChildView.C,v 1.6.2.4 2009/12/09 19:44:42 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronChildView.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "ViatronChildView.h"
#include "NavigationView.h"
#include "OverView.h"
#include "SliceChildView.h"
#include "VFrameManager.h"
#include "FullObliqueView.h"

#define FILE_REVISION "$Revision: 1.6.2.4 $"

using namespace ColonViews::Layouts;
using namespace ReaderLib;

////////////////////////////////////////////////////////////////////////
// ViatronChildView, derivative of GLChildView custom control

/**
 * Constructor 
 *
 * @param eGLViewType           Type of GL child view.
 * @param hParent               Parent window.
 * @param eDatasetOrientation   Dataset orientation.
 */
ViatronChildView::ViatronChildView(const GLChildView::ViewWndTypeEnum eGLViewType, HWND hParent,
                                   const DatasetOrientations::TypeEnum eDatasetOrientation)
: GLChildView(eGLViewType), m_dataset(GetDatasetRef(eDatasetOrientation)),
m_oppositeDataset(GetDatasetRef(GetOppositeOrientation(eDatasetOrientation)))
{
  switch (eDatasetOrientation) 
	{
    case DatasetOrientations::eSUPINE:
    case DatasetOrientations::ePRONE:
      m_pDataset = m_pOppositeDataset = NULL;
      break;

    case DatasetOrientations::ePRIMARY:
      m_pDataset = &rdrGlobal.m_pCurrDataset;
      m_pOppositeDataset = &rdrGlobal.m_pSecondaryDataset;
      break;
    
    case DatasetOrientations::eSECONDARY:
      m_pDataset = &rdrGlobal.m_pSecondaryDataset;
      m_pOppositeDataset = &rdrGlobal.m_pCurrDataset;
      break;
    
    default:
      // Illegal argument
      ex::IllegalArgumentException(LogRec("Invalid DatasetOrientationsEnum case","ViatronChildView", "ViatronChildView"));
      m_pDataset = m_pOppositeDataset = NULL;
      break;
  }
} // ViatronChildView()


/**
 * Destructor 
 */
ViatronChildView::~ViatronChildView()
{
} // Destructor()


/**
 * WM_Command-message handler.
 *
 * @param wParam   Message-dependent parameter.
 * @param lParam   Message-dependent parameter.
 * @return         command result
 */
BOOL ViatronChildView::OnCommand( WPARAM wParam, LPARAM lParam )
{
  switch ( LOWORD(wParam) )
  {
    case IDM_VIEWWINDOW_MAINWINDOW:
    {
      AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_VIEW_WINDOW, GetClassInstanceID(), 0);
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL ), 0 );
      return TRUE;
    }

    case IDM_VIEWWINDOW_FULLSCREEN:
    {
      FullObliqueLayout::m_eViewWndType = GetViewWndType();
      FullObliqueLayout::m_eDatasetOrientation = GetDatasetRef().GetOrientation();

      AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE,
        MAKELPARAM(ColonViews::FramesManager::VFrameManager::NavFrameManager::ePOS_FULLVIEW, 0));
      return TRUE;
    }
  }
  return FALSE;
} // OnCommand()

////////////////////////////////////////////////////////////////////////////////////
// ViatronixChildWnd, generic view wnd with (typically) a single ViatronChildView


/**
 * Constructor 
 *
 * @param uViewTemplateID   Resource ID of the layout definition for the view instance.
 * @param uBkgdBmpID        Resource ID of the background image.
 */
ViatronixChildWnd::ViatronixChildWnd(const uint4 uViewLayoutResourceID)
  : ViatronixWnd(IDB_ENDOVIEW_VERT_DLGBAR), m_uViewLayoutResID(uViewLayoutResourceID)
{
} // constructor


BEGIN_MESSAGE_MAP(ViatronixChildWnd, ViatronixWnd)
  //{{AFX_MSG_MAP(ViatronixChildWnd)
  ON_WM_NCLBUTTONDBLCLK()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_SIZING, OnMessageSizingQuery)
END_MESSAGE_MAP()

/**
 * query wnd size
 *
 * @param wProposedSize   proposed size
 * @param LPARAM          unused
 */
LRESULT ViatronixChildWnd::OnMessageSizingQuery(WPARAM wProposedSize, LPARAM)
{
  if (LOWORD(wProposedSize) < HIWORD(wProposedSize))
  {
    return MAKELRESULT(LOWORD(wProposedSize), LOWORD(wProposedSize));
  }
  else
  {
    return MAKELRESULT(HIWORD(wProposedSize), HIWORD(wProposedSize));
  }
} // OnMessageSizingQuery()


/**
 * non-client area double click
 *
 * @param uHitTest   identifies type of area clicked
 * @param point      point clicked
 **/
void ViatronixChildWnd::OnNcLButtonDblClk(UINT uHitTest, CPoint point) 
{
  GetParent()->PostMessage(WM_NCLBUTTONDBLCLK, WPARAM(HWND(*this)), MAKELPARAM(point.x, point.y));
} // OnNcLButtonDblClk()


/**
 * mouse move
 * @param uFlags   keyboard/mouse state flags
 * @param point    point of mouse cursor
 **/
void ViatronixChildWnd::OnMouseMove(UINT uFlags, CPoint point)
{
  ViatronixWnd::OnMouseMove( uFlags, point );
} // OnMouseMove()


// Revision History:
// $Log: ViatronChildView.C,v $
// Revision 1.6.2.4  2009/12/09 19:44:42  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.6.2.3  2007/09/28 01:25:40  jmeade
// set current dataset in fullview mode.
//
// Revision 1.6.2.2  2007/09/25 22:45:16  jmeade
// Issue 5359: set the child view type.
//
// Revision 1.6.2.1  2007/09/20 23:07:49  jmeade
// full screen view
//
// Revision 1.6  2007/03/07 23:07:03  jmeade
// code standards.
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.1  2005/12/12 20:33:51  geconomos
// Issue# 4574: Child views not refreshing when setting main veiw from context menu
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
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
// Revision 3.11.2.2.4.2  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.11.2.2.4.1.2.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.11.2.2.4.1  2005/04/01 23:33:22  jmeade
// Painting in the s/p panes.
//
// Revision 3.11.2.2  2003/07/25 21:45:45  jmeade
// Issue 3401: Common childview handler for main view window toggle.
//
// Revision 3.11.2.1  2003/02/22 12:23:51  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.11  2002/11/25 23:28:12  jmeade
// No internal setting of pane text (let windows handle it themselves).
//
// Revision 3.10  2002/11/15 00:41:47  jmeade
// No hard-coding of # of child views.
//
// Revision 3.9  2002/09/05 17:34:04  jmeade
// Window title for child windows.
//
// Revision 3.8  2002/08/27 18:28:23  jmeade
// Sliding pane incremental checkin (using resource for child wind layout); Unused app messages.
//
// Revision 3.7  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.5  2002/07/26 22:42:33  jmeade
// ViatronChildWnd, new control for a single GLChildView.
//
// Revision 3.4  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.3  2002/04/12 02:17:57  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2002/03/25 21:20:25  jmeade
// Cleanup of log record code.
//
// Revision 3.1.2.1  2002/03/28 22:32:16  jmeade
// Cleanup of log record code.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 1.1   Oct 08 2001 16:54:06   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronChildView.C,v 1.6.2.4 2009/12/09 19:44:42 dongqing Exp $
// $Id: ViatronChildView.C,v 1.6.2.4 2009/12/09 19:44:42 dongqing Exp $
