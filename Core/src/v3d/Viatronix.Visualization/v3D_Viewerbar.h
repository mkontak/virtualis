// $Id: v3D_Viewerbar.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"

// namespaces 
OPEN_VISUALIZATION_NS

// forward managed declarations
ref class Viewer;
ref class ViewerbarButtonCollection;

// class definition
public ref class Viewerbar : public System::Windows::Forms::Control
{
// functions
public:
    
  /// initializer
  Viewerbar( Viatronix::Visualization::Viewer ^ mpViewer );

  /// returns the Buttons collection
  property Viatronix::Visualization::ViewerbarButtonCollection ^ Buttons 
  { 
    Viatronix::Visualization::ViewerbarButtonCollection ^ get() { return m_mpButtons; }
  }

  /// recalculates the required size of the viewerbar, the size and location of the buttons
  void RecalculateLayout();

protected:

  /// called when the viewerbar needs to paint itself
  virtual void OnPaint( System::Windows::Forms::PaintEventArgs ^ mpArgs ) override;

  /// mouse move handler for child label controls
  virtual void OnMouseMoveHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs );
  
  /// mouse down handler for child label controls
  virtual void OnMouseDownHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs );

  /// mouse up handler for child label controls
  virtual void OnMouseUpHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs );
  
  /// key down handler for child label controls
  virtual void OnKeyDownHandler( System::Object ^ mpSender, System::Windows::Forms::KeyEventArgs ^ mpArgs );
  
  virtual void OnMouseDown( System::Windows::Forms::MouseEventArgs ^ mpArgs ) override;

  /// thread timer callback function
  void OnTimerCallbackHandler( System::Object ^ mpState );

private:

  /// delegate instance for timer callback
  void TimerCallbackDelegate( System::Object ^ mpSender, System::EventArgs ^ mpArgs );

// properties
public:

  /// sets the expanded flag (but does not explicitly recalculate the layout)
  property bool Expanded { void set( bool bExpanded ) { m_bExpanded = bExpanded; }}


// member variables
private:

  /// parent viewer
  Viatronix::Visualization::Viewer ^ m_mpViewer;

  /// collection of viewerbar buttons
  Viatronix::Visualization::ViewerbarButtonCollection ^ m_mpButtons;
  
  /// tooltip control
  System::Windows::Forms::ToolTip ^ m_mpToolTip;

  /// thread timer
  System::Threading::Timer ^ m_mpTimer;
  
  // indicates if the toolbar is expanded
  bool m_bExpanded;

  /// the item that is currently beging tracked
  int m_iTrackItem;

  /// timer callback instance
  System::EventHandler ^ m_mpTimerDelgate;
  
  /// left toolbar image
  static System::Drawing::Bitmap ^ m_mpToolBarLeftImage = nullptr;
  
  /// middle toolbar image
  static System::Drawing::Bitmap ^ m_mpToolBarMiddleImage = nullptr;
  
  /// right toolbar image
  static System::Drawing::Bitmap ^ m_mpToolBarRightImage = nullptr;

  /// expand toolbar image
  static System::Drawing::Bitmap ^ m_mpToolBarExpandImage = nullptr;

  /// static constructor
  static Viewerbar()
  {
    System::Resources::ResourceManager resources( L"Viatronix.Visualization", System::Reflection::Assembly::GetExecutingAssembly() );

    m_mpToolBarLeftImage    = safe_cast< System::Drawing::Bitmap ^ >( resources.GetObject( L"VIEWERBAR_LEFT" ) );
    m_mpToolBarMiddleImage  = safe_cast< System::Drawing::Bitmap ^ >( resources.GetObject( L"VIEWERBAR_MIDDLE" ) );
    m_mpToolBarRightImage   = safe_cast< System::Drawing::Bitmap ^ >( resources.GetObject( L"VIEWERBAR_RIGHT" ) );
    m_mpToolBarExpandImage  = safe_cast< System::Drawing::Bitmap ^ >( resources.GetObject( L"VIEWERBAR_EXPAND" ) );
  }

}; // ToolBar

CLOSE_VISUALIZATION_NS

// $Log: v3D_Viewerbar.h,v $
// Revision 1.7  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/01/31 17:07:54  geconomos
// corrected access of control from within timer
//
// Revision 1.5  2005/05/25 15:30:37  frank
// worked on displaying the full viewerbar skin image - it was being cut off
//
// Revision 1.4  2005/01/06 19:25:14  frank
// enabled expansion of viewerbar panes
//
// Revision 1.3  2004/10/13 16:07:19  geconomos
// Added the ability to collapse the viewerbar.
//
// Revision 1.2  2004/10/12 00:14:07  geconomos
// now skinning the toolbar
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/01/26 12:37:28  geconomos
// Passed along key presses to viewer when mouse is over the viewer bar.
//
// Revision 1.9  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.8  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.7  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.6  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.5  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.4  2003/04/16 18:50:22  geconomos
// Issue #3110:  Buttons not refreshing after drop event.
//
// Revision 1.3  2003/04/08 14:54:52  geconomos
// Issue #2398: Tooltips are now available for viewerbar buttons.
//
// Revision 1.2  2002/11/20 13:49:11  geconomos
// implemented click on IViewerbarButton
//
// Revision 1.1  2002/11/18 19:59:22  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Viewerbar.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Viewerbar.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
