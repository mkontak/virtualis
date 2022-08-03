//////////////////////////////////////////////////////////////////////
// $Id: NavigationBar.h,v 1.4 2007/03/12 19:43:46 jmeade Exp $
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

// pragmas
#pragma once

//include desclations
#include "TabStrip.h"

OPEN_DIALOGS_NS

// class declaration
class NavigationBar : public WinUtils::TabStrip
{
// member functions
public:

  // constructor
  NavigationBar();

  // destructor
  ~NavigationBar();

  // sets the tabs
  void SetNavigationTabs( const std::vector< std::string > & vTabTitles );

  /// sets the selected tab
  void SetCurrentViewSelection( const int4 iView );

  /// gets the selected tab
  int4 GetCurrentViewSelection() const;

protected:

  /// mfc message map
  DECLARE_MESSAGE_MAP()

  /// called after the current item changes
  virtual void OnItemChanged( int4 iPrevious, int4 iNew );
  
}; // class NavigationBar 

CLOSE_DIALOGS_NS


// Revision History:
// $Log: NavigationBar.h,v $
// Revision 1.4  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.3  2005/10/03 12:59:01  geconomos
// derived class from tab strip
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:20   ingmar
// Initial revision.
// Revision 1.7  2001/04/13 02:34:11  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NavigationBar.h,v 1.4 2007/03/12 19:43:46 jmeade Exp $
// $Id: NavigationBar.h,v 1.4 2007/03/12 19:43:46 jmeade Exp $
