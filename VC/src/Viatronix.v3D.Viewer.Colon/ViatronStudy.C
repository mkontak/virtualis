// $Id: ViatronStudy.C,v 1.9 2007/03/07 22:33:53 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronStudy.C  
// Description: Implementation of class/instances to handle various user modes in Colon application.
// Owner: Jeff Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "ViatronStudy.h"
#include "MainAppFrame.h"
#include "Viatron.h"
#include "ApplicationSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ViewModes;
using namespace ReaderLib;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///// Static member variable declarations ////////////////////////////

VtxViewModes ViatronStudy::m_modeOverView;
VtxViewModesWithNotify<ViatronStudy::OnSetViewModeProc> ViatronStudy::m_modeEndoView;
VtxViewModesWithNotify<ViatronStudy::OnSetViewModeProc> ViatronStudy::m_modeOrthogView;
VtxViewModesWithNotify<ViatronStudy::OnSetViewModeProc> ViatronStudy::m_modeObliqueView;
VtxViewModesWithNotify<ViatronStudy::OnSetViewModeProc> ViatronStudy::m_modeSegments3D;

VtxViewModes *ViatronStudy::m_peCurrentViewModes;
GLChildView::ViewWndTypeEnum ViatronStudy::m_eCurrentViewType;
DatasetOrientations::TypeEnum ViatronStudy::m_eCurrentOrientation;

ViatronStudy::structVWMDS ViatronStudy::ViewWndModes;

bool ViatronStudyInitialReset = ViatronStudy::Reset( false );

//////////////////////////////////////////////////////////////////////


/**
 * Resets all view modes.
 *
 * @return   Always true.
 */
bool ViatronStudy::Reset( bool bUserSpecific )
{
  ViewModes::VIEWMODES centerlineMode( CENTERLINE );
  if( bUserSpecific )
    centerlineMode = ApplicationSettings::UserSettings().Get<bool>( APP_SETTING_SHOW_CENTERLINE ) ? CENTERLINE : static_cast< ViewModes::VIEWMODES >( 0 );

  m_modeOverView = VtxViewModes((CENTERLINE | BOOK_MARK | DRAW_DISTANCE),
    (MISSED_REGIONS | CENTERLINE | BOOK_MARK | SLICE | TRANSLUCENT | DRAW_DISTANCE));

  m_modeEndoView.SetModes(centerlineMode, ViewModes::VIEWMODES(MISSED_REGIONS | CENTERLINE | BOOK_MARK | SLICE | TRANSLUCENT |
    BIOPSY_VALID | LINEMEASURE_VALID | LIGHTING_VALID | AUTOFLY_VALID | ANNOTATION_VALID | SEGMENTPOLYP_VALID));

  m_modeOrthogView.SetModes(ViewModes::VIEWMODES(0),
    ViewModes::VIEWMODES(LINEMEASURE_VALID | ROIMEASURE_VALID | AUTOFLY2D_VALID | ANNOTATION_VALID | PSEUDOCOLOR_VALID | SEGMENTPOLYP_VALID));

  m_modeObliqueView.SetModes(ViewModes::VIEWMODES(0),
    ViewModes::VIEWMODES(LINEMEASURE_VALID | ROIMEASURE_VALID | AUTOFLY2D_VALID | PSEUDOCOLOR_VALID | SEGMENTPOLYP_VALID));

  m_modeSegments3D.SetModes(ViewModes::VIEWMODES(0), ViewModes::VIEWMODES(PSEUDOCOLOR_VALID));

  m_peCurrentViewModes = &ViatronStudy::m_modeEndoView;
  m_eCurrentViewType = GLChildView::eGVT_NAVIGATION;
  m_eCurrentOrientation = DatasetOrientations::eSUPINE;

  ViewWndModes.m_modeStandardLayout = VtxViewModes(0);
  ViewWndModes.m_modeEndoOrthoLayout = VtxViewModes(0);
  ViewWndModes.m_modeSPOrthoLayout = VtxViewModes(0);
  ViewWndModes.m_modeSPOverViewLayout = VtxViewModes(0);

  return true;
} // Reset()


/**
 * Returns the view modes of the currently selected view.
 *
 * @param eViewType       Child view type whose modes will be returned.
 * @return VtxViewModes   View modes for the given childview type.
 */
VtxViewModes &ViatronStudy::GetViewModes(const GLChildView::ViewWndTypeEnum eViewType)
{
  static VtxViewModes nullModes;

  switch (eViewType)
  {
    case GLChildView::eGVT_OVERVIEW:
      return m_modeOverView;

    case GLChildView::eGVT_NAVIGATION:
      return m_modeEndoView;

    case GLChildView::eGVT_AXIAL:
    case GLChildView::eGVT_SAGITTAL:
    case GLChildView::eGVT_CORONAL:
      return m_modeOrthogView;

    case GLChildView::eGVT_CROSS_SECTION:
      return m_modeObliqueView;

    case GLChildView::eGVT_3DSEGMENTS:
      return m_modeSegments3D;

    default:
      return nullModes;
  }
} // GetViewModes()


/**
 * Sets the currently selected view.
 *
 * @param eViewType      Type of the child view.
 * @param eStudyOrient   Now-current dataset orientation.
 */
void ViatronStudy::SetCurrentView(const GLChildView::ViewWndTypeEnum eViewType, const DatasetOrientations::TypeEnum eStudyOrient)
{
  m_eCurrentViewType = eViewType;
  m_eCurrentOrientation = eStudyOrient;

  switch (eViewType)
  {
    case GLChildView::eGVT_OVERVIEW:
      m_peCurrentViewModes = &m_modeOverView;
      break;
    case GLChildView::eGVT_NAVIGATION:
      m_peCurrentViewModes = &m_modeEndoView;
      break;

    case GLChildView::eGVT_AXIAL:
    case GLChildView::eGVT_SAGITTAL:
    case GLChildView::eGVT_CORONAL:
    case GLChildView::eGVT_ORTHOGONAL:
      m_peCurrentViewModes = &m_modeOrthogView;
      break;

    case GLChildView::eGVT_CROSS_SECTION:
      m_peCurrentViewModes = &m_modeObliqueView;
      break;

    case GLChildView::eGVT_3DSEGMENTS:
      m_peCurrentViewModes = &m_modeSegments3D;
      break;

    default:
      break;
  }
} // SetCurrentView()


/**
 * Method invoked when view mode is changed.
 *
 * @param uOldMode   Previous set of view modes.
 * @param uNewMode   New set of view modes after update.
 */
void ViatronStudy::OnSetViewModeProc(const ViewModesValue& uOldMode, const ViewModesValue& uNewMode)
{
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_VIEWMODE_CHANGED, uOldMode, uNewMode);
} // OnSetViewModeProc()


/**
 * Turns off autoflight
 */
void ViatronStudy::TurnOffAutoflight()
{
  // turn off autoflight
  ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
  ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY2D;
  ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY;
  ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY2D;
  ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY;
  ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY2D;
  ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
  ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
} // TurnOffAutoflight()



// Revision History:
// $Log: ViatronStudy.C,v $
// Revision 1.9  2007/03/07 22:33:53  jmeade
// code standards.
//
// Revision 1.8  2007/02/02 22:28:52  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.7  2007/02/02 21:29:36  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.6  2006/10/04 20:51:36  frank
// Issue #5042: Made centerline display default a configuration setting
//
// Revision 1.5  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4.2.1  2006/03/03 01:56:53  jmeade
// Issue 4620: Add annotations in 3D.
//
// Revision 1.4  2005/11/01 16:13:22  jmeade
// Issue 4491: Slice views trigger on-mode-changed processing.
//
// Revision 1.3  2005/09/13 13:05:46  geconomos
// Added StopAutoFlight() static member
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.6.2.5.2.2  2005/04/11 15:04:07  jmeade
// Issue 2815: Disable annotations for oblique views.
//
// Revision 3.6.2.5.2.1  2005/03/25 15:46:46  frank
// Issue #3990: Disabled autofly when flight is not possible on the dual ortho view
//
// Revision 3.6.2.5  2004/09/30 15:51:31  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.6.2.4  2003/06/02 21:47:44  jmeade
// Issue 3262: Route and handle view mode change message thru MainAppFrame to ensure it'll always get handled; comments.
//
// Revision 3.6.2.3  2003/02/27 03:33:46  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.6.2.2  2003/02/22 05:57:01  jmeade
// Issue 2837:  Recognize any generic orthogonal view for m_orthoModes.
//
// Revision 3.6.2.1  2003/02/12 01:08:50  jmeade
// Issue 2763: Global mode/valid option for pseudo-color view.
//
// Revision 3.6  2002/12/12 22:39:58  jmeade
// Unused or unneeded view modes code cleanup; Comments, code standards.
//
// Revision 3.5  2002/06/10 22:39:23  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.4  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.3  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.2.2.3  2002/06/06 15:59:33  jmeade
// Issue 2407:  Allow autofly in overview.
//
// Revision 3.2.2.2  2002/05/23 17:07:35  jmeade
// Issue 2270:  Support for 2D annotations mode.
//
// Revision 3.2.2.1  2002/04/24 01:02:19  jmeade
// Issue 2044:  Set current view *dataset orientation* as well as type.
//
// Revision 3.2  2001/11/12 21:16:10  binli
// GUI: use autobtn to control 2d auto-fly
//
// Revision 3.1  2001/10/17 19:20:32  jmeade
// All Orthogonal views now share same view modes.  Issue 1183.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:28   ingmar
// Initial revision.
// Revision 1.15  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.14  2001/05/07 21:43:43  binli
// Added control to toggle display bookmarks in Overview
// (TrackRecord ID: 000375 & 000342 )
//
// Revision 1.13  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.12  2001/02/22 01:37:57  jeff
// removed unnecessary modes
//
// Revision 1.11  2001/02/09 00:10:20  jeff
// biopsy_valid mode for endoscopic view
//
// Revision 1.10  2001/02/05 21:24:35  jeff
// measure ==> line measure and roi measure; removed CL_GRADIENT mode
//
// Revision 1.9  2001/02/05 18:51:01  frank
// Removed the centerline gradient in all-but-verify view.
//
// Revision 1.8  2001/01/30 02:12:44  jeff
// changed PICKINFO and MARKING mode to MISSED_REGIONS
//
// Revision 1.7  2001/01/26 19:10:25  geconomos
// Removed CONNECT_SEGS view mode and combined with CENTERLINE.
// Added DRAW_DISTANCE view mode to enable drawing dictance from rectum
//
// Revision 1.6  2000/12/27 23:11:35  jeff
// Store view modes for each navigation view (Standard, Endo-Ortho, etc)
// eGVT_TRANSVERSE ==> eGVT_AXIAL;
// eGVT_MPR ==> eGVT_CROSS_SECTION;
//
// Revision 1.5  2000/11/20 03:33:43  kevin
// Removed Hidden Centerline removal as default
//
// Revision 1.4  2000/11/16 19:46:40  binli
// added 'CENTERLINE2' to OverViewModes to control centerline rendering.
//
// Revision 1.3  2000/10/31 23:51:47  jeff
// ViatronStudy::Reset() function, to reset modes and the like
//
// Revision 1.2  2000/10/02 23:01:39  jeff
// Moved bookmark list to ReaderGlobal (instance)
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 8     9/01/00 9:14p Jeff
// Mode changes implemented throughout interface
// 
// 7     9/01/00 3:41p Jeff
// No marking in overview, No orthogonal in cross-section view
// 
// 6     9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 5     8/30/00 6:32p Jeff
// One standardarized enum value class for all mode windows
// 
// 4     8/29/00 6:37p Jeff
// Added endoscopic modes
// 
// 3     8/29/00 4:14p Jeff
// Overview mode variables added
// 
// 2     8/28/00 1:54p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/14/00  Time:  1:53p
// Created ViatronStudy.cpp
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronStudy.C,v 1.9 2007/03/07 22:33:53 jmeade Exp $
// $Id: ViatronStudy.C,v 1.9 2007/03/07 22:33:53 jmeade Exp $
