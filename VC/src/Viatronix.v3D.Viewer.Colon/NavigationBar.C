//////////////////////////////////////////////////////////////////////
// $Id: NavigationBar.C,v 1.6 2007/03/12 19:43:46 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:
// Description:
// Created:
// Author: Jeff Meade  jmeade@viatronix.net

// includes
#include "stdafx.h"
#include "V1KMessages.h"
#include "ReaderGlobal.h"

#include "NavigationBar.h"


// namespaces
using namespace ColonViews::Dialogs;
using namespace ReaderLib;
using namespace WinUtils;

/**
 * Constructor
 */
NavigationBar::NavigationBar() 
{
  SetOrientation( TabStrip::LEFT );
} // NavigationBar() 


/**
 * Destructor
 */
NavigationBar::~NavigationBar() 
{
} // ~NavigationBar() 


/// mesage map
BEGIN_MESSAGE_MAP( NavigationBar, TabStrip )
END_MESSAGE_MAP()


/**
 * Called when the item has changed
 *
 * @param   iPrevious   index of previous item
 * @param   iNew        index of new item
 */
void NavigationBar::OnItemChanged( int4 iPrevious, int4 iNew ) 
{
  // add judgement here to ensure we can grab a good report image from VerificationView
  if (rdrGlobal.m_bReportCreated)
    AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(iNew, iPrevious));
} // OnItemChanged( int4 iPrevious, int4 iNew ) 


/**
 * Sets the tabs
 *
 * @param   tabTitles     tabs to add
 */
void NavigationBar::SetNavigationTabs( const std::vector< std::string > & vTabTitles )
{
  TabStrip::BeginInit();

  TabStrip::ClearItems();

  for( int i( 0 ); i < vTabTitles.size(); ++i )
    TabStrip::AddItem( TabStripItem( vTabTitles[ i ] ) );

  TabStrip::EndInit();
  
} // SetNavigationTabs( const std::vector< std::string > & vTabTitles )


/**
 * Gets the index of the selected tab
 *
 * @return    index of current tab
 */
int4 NavigationBar::GetCurrentViewSelection() const
{
  if( !IsWindow( GetSafeHwnd() ) )
    return -1;
  return TabStrip::GetSelectedIndex();
} // GetCurrentViewSelection() const


/**
 * Sets the selected tab
 *
 * @param   iView     index of tab to selected
 */
void NavigationBar::SetCurrentViewSelection( const int4 iView )
{
  TabStrip::SetSelectedIndex( iView );
} // SetCurrentViewSelection( int4 iView )



// Revision history:
// $Log: NavigationBar.C,v $
// Revision 1.6  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.5  2007/03/08 22:22:05  romy
// code review - header files adjusted
//
// Revision 1.4  2005/10/03 12:59:01  geconomos
// derived class from tab strip
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
// Revision 3.2  2002/07/18 20:25:38  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.1  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/06 17:16:33  jmeade
// Cleaned up V1KMessages.h enum, comments.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 03 2001 11:09:20   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar.
// 
//    Rev 2.0   Sep 16 2001 23:41:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:20   ingmar
// Initial revision.
// Revision 1.13  2001/08/21 02:25:57  jmeade
// Enums for view window positions (no hardcoded values)
//
// Revision 1.12  2001/06/14 00:35:32  jmeade
// explicitly call the (non-message generating) CTabCtrl version of SetCurSel()
//
// Revision 1.11  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.10  2001/04/05 18:46:37  jmeade
// tabs to spaces; EnableWindow() override
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NavigationBar.C,v 1.6 2007/03/12 19:43:46 jmeade Exp $
// $Id: NavigationBar.C,v 1.6 2007/03/12 19:43:46 jmeade Exp $
