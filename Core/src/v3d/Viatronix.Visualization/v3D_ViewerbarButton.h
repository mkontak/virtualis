// $Id: v3D_ViewerbarButton.h,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
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


/**
 * Represents a viewer bar button
 */
public ref class ViewerbarButton
{
// construction
public:
  
  /// default constructor
  ViewerbarButton();

// member functions
public:

  /// gets the drop source for the button
  virtual IDropSource ^ GetDropSource() { return nullptr; }
  
  /// gets the color to identify with the drop source button
  virtual System::Drawing::Image ^ GetDropSourceImage() { return nullptr; }
  
  /// gets the context menu to display when the button is clcked
  virtual Viatronix::UI::ContextMenu ^ GetContextMenu() { return nullptr; } 

// properties
public:

  /// gets or setsthe parent viewer
  property virtual Viatronix::Visualization::Viewer ^ Viewer
  { 
    Viatronix::Visualization::Viewer ^ get() { return m_mpViewer; }
    void set( Viatronix::Visualization::Viewer ^ mpViewer ) { m_mpViewer = mpViewer; } 
  } // Viewer
    
  /// gets the button image
  property System::Drawing::Bitmap ^ Image
  {
    System::Drawing::Bitmap ^ get() { return m_mpImage; }
    void set( System::Drawing::Bitmap ^ mpImage );
  } // Image

  /// gets the id
  property System::String ^ ID
  {
    System::String ^ get() { return m_mpID; }
    void set( System::String ^ mpID ) { m_mpID = mpID; }
  } // ID

  /// gets the tooltip
  property System::String ^ Tooltip
  {
    System::String ^ get() { return m_mpTooltip; }
    void set( System::String ^ mpTooltip ) { m_mpTooltip = mpTooltip; }
  } // Tooltip

  /// gets the tooltip
  property System::Drawing::Rectangle Bounds
  {
    System::Drawing::Rectangle get() { return m_mpBounds; }
    void set( System::Drawing::Rectangle bounds ) { m_mpBounds = bounds; }
  } // Bounds

  /// indicates if the button acts as a drag and drop source
  property bool IsDropSource
  {
    bool get() { return m_bIsDropSource; }
    void set( bool bIsDropSource ) { m_bIsDropSource = bIsDropSource; }
  } // IsDropSource

  /// Gets or sets the visibility
  property virtual bool Visible
  {
    bool get() { return m_bVisible; }
    void set( bool bVisible ) { m_bVisible = bVisible; }
  } // Visible

  /// indicates if the button displays a context menu when clicked
  property bool HasContextMenu
  {
    bool get() { return m_bHasContextMenu; }
    void set( bool bHasContextMenu ) { m_bHasContextMenu = bHasContextMenu; }
  } // HasContextMenu

// event handlers
public:

  /// called when the user clicks the button.
  virtual void OnClick( System::Windows::Forms::MouseButtons button ) { }

// member variables
private:

  /// parent viewer
  Viatronix::Visualization::Viewer ^ m_mpViewer;

  /// button image
  System::Drawing::Bitmap ^ m_mpImage;

  /// toolip text
  System::String ^ m_mpTooltip;

  /// bounding rectangle
  System::Drawing::Rectangle  m_mpBounds;
     
  /// indicates if the button acts as a drop source
  bool m_bIsDropSource;
  
  /// indicates if the button has a context menu
  bool m_bHasContextMenu;

  /// indicates the button's visibility
  bool m_bVisible;

  /// id
  System::String ^ m_mpID;

}; // class ViewerbarButton

CLOSE_VISUALIZATION_NS

// $Log: v3D_ViewerbarButton.h,v $
// Revision 1.5.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.5.2.1  2009/03/23 15:13:44  gdavidson
// Added the ablility to changed the visibility of a viewerbar button
//
// Revision 1.5  2007/07/17 21:06:19  gdavidson
// Added ID property
//
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/06/16 18:39:21  frank
// invalidated the button when the image is set
//
// Revision 1.2  2004/10/13 16:04:49  geconomos
// Added new properties.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerbarButton.h,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_ViewerbarButton.h,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
