 // $Id: v3D_Viewer.h,v 1.28 2007/10/29 20:34:27 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

/*
  Viewer class. A control that provides a renderering context and output for the vx
  visualization classes. All implementation is delegated to the unmanged implementation
*/

// pragmas
#pragma once

// includes
#include "v3D_Enums.h"
#include "v3D_Interfaces.h"
#include "vxViewerControl.h"


#ifdef Point
#undef Point
#endif


// forward declarations
class vxInputAdapter;


// forward declarations
OPEN_GRAPH_NS
ref class RenderingNodePlugin;
CLOSE_GRAPH_NS


// namespaces
OPEN_VISUALIZATION_NS


// managed forward declarations
ref class Environment;
ref class Dataset;
ref class Viewerbar;


// class definition
public ref class Viewer : public System::Windows::Forms::Control
{  
// events
public:
  
  /// raised when the popup menu is to be shown
  event System::EventHandler ^ ContextMenu;


// member functions
public:
  
  /// initializer
  Viewer();    

  /// destructor
  virtual ~Viewer();

  /// finalizer
  !Viewer();

  /////////////////////////////////////////////////////
  // general methods

  /// get the vxViewerControl
  System::IntPtr GetViewerPtr() { return System::IntPtr( m_pViewer ); }

  /// initializes the viewer from the specified dataset
  void Initialize( Viatronix::Visualization::Dataset ^ mpDataset );

  /// resets the viewer to a default state
  void Reset();

  /// screen shot
  System::Drawing::Bitmap ^ GetScreenShot( bool bPlaySound );

  /// set this viewer as the active viewer
  void SetActive();

  /////////////////////////////////////////////////////
  // renderer methods
  
  /// adds a renderer to the specified viewer
  void AddRenderer( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// adds a renderer to the specified viewer
  void RemoveRenderer( ViewerTypes eViewerType, System::Type ^ mpFactory );
  
  /// adds a renderer to the specified viewer
  bool HasRenderer( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// removes all renderers
  void RemoveAllRenderers();
  

  /////////////////////////////////////////////////////
  // manipulator methods

  /// adds a manipulator to the specified viewer
  void AddManipulator( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// adds a manipulator to the specified viewer
  void RemoveManipulator( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// adds a manipulator to the specified viewer
  bool HasManipulator( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// removes all manipulators
  void RemoveAllManipulators();


  /////////////////////////////////////////////////////
  // controller methods

  /// adds a controller to the specified viewer
  void AddController( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// adds a controller to the specified viewer
  void RemoveController( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// adds a controller to the specified viewer
  bool HasController( ViewerTypes eViewerType, System::Type ^ mpFactory );

  /// removes all controllers
  void RemoveAllControllers();


  /////////////////////////////////////////////////////
  // viewer type properties

  /// returns the main viewer type
  property Viatronix::Visualization::IViewerType ^ ViewerType
  {
    Viatronix::Visualization::IViewerType ^ get() { return m_mpViewerType; }
    void set( Viatronix::Visualization::IViewerType ^ mpViewerType );
  }

  /// returns the sub viewer type
  property Viatronix::Visualization::IViewerType ^  SubViewerType
  {
    Viatronix::Visualization::IViewerType ^ get() { return m_mpSubViewerType; }
    void set( Viatronix::Visualization::IViewerType ^ mpViewerType );
  }

  /// returns the rendering manager plugin
  property Viatronix::Visualization::Graph::RenderingNodePlugin ^ RenderingNodePlugin
  {
    Viatronix::Visualization::Graph::RenderingNodePlugin ^ get();
  } 


  /////////////////////////////////////////////////////
  // border properties

  /// returns the color of the viewer's border
  property System::Drawing::Color BorderColor
  {
    System::Drawing::Color get();
    void set( System::Drawing::Color borderColor );
  }
  
  /// gets the width of the border in pixels
  property float BorderWidth
  {
    float get() { return m_pViewer->GetBorderWidth(); }
    void set( float fWidth ) { return m_pViewer->SetBorderWidth( fWidth ); }
  }


  /////////////////////////////////////////////////////
  // general properties

  /// returns the environment
  property Viatronix::Visualization::Environment ^ Environment
  {
    Viatronix::Visualization::Environment ^ get() { return m_mpEnvironment; }
  }

  /// return the toolbar
  property Viatronix::Visualization::Viewerbar ^ ToolBar
  {
    Viatronix::Visualization::Viewerbar ^ get() { return m_mpViewerbar; }
  }

  /// returns the AllowSubView flag
  property bool AllowSubView
  {
    bool get() { return m_bAllowSubView; }
    void set( bool bAllowSubView ) { m_bAllowSubView = bAllowSubView; }
  }

  /// returns the enabled state of a specified viewer
  bool GetEnabled( ViewerTypes eViewerType );

  /// sets the enabled state of a specified viewer
  void SetEnabled( ViewerTypes eViewerType, bool bEnabled );

  /// sets the size of the subviewer
  void SetSubViewerSize( int4 x, int4 y );

  /// gets the PopupMenu
  property Viatronix::UI::PopupMenu ^ PopupMenu { Viatronix::UI::PopupMenu ^ get() { return m_mpPopupMenu; }}

  /// returns if the viewer contains a 2D renderer
  property bool Is2D { bool get() { return m_mpViewerType->Is2D; }}

  /// returns if the viewer contains a 3D renderer
  property bool Is3D { bool get() { return m_mpViewerType->Is3D; }}

  // TODO: this is junk, rethink
  /// gets the supported animations
  [ System::ComponentModel::DesignerSerializationVisibilityAttribute( System::ComponentModel::DesignerSerializationVisibility::Hidden ) ]
  property array< System::Type ^ > ^ SupportedAnimations
  {
    array< System::Type ^ > ^ get() { return m_mpSupportedAnimations; }
    void set( array< System::Type ^ > ^ mpSupportedAnimations ) { m_mpSupportedAnimations = mpSupportedAnimations; }
  }

  /// gets the internal vxViewer pointer for mainviewer
  property System::IntPtr InternalViewer { System::IntPtr get() { return System::IntPtr( m_pViewer->GetViewer( vxViewerControl::MainViewer )); }}  

protected:

  /// called when underlying window handle is created
  virtual void OnHandleCreated( System::EventArgs ^ mpArgs ) override;

  /// control's wndproc
  virtual void WndProc( System::Windows::Forms::Message % msg ) override;

  /// called when the mouse is moved over the control during a drag operation
  virtual void OnDragOver( System::Windows::Forms::DragEventArgs ^ mpArgs ) override;

  /// called when a drop operation is performed
  virtual void OnDragDrop( System::Windows::Forms::DragEventArgs ^ mpArgs ) override;

  /// called when the viewer is resized
  virtual void OnResize( System::EventArgs ^ mpArgs ) override;
  
  /// Returns true if the given key is used for input
  virtual bool IsInputKey( System::Windows::Forms::Keys keyData ) override;

  /// called when the viewer bar is clicked
  void OnViewerbarClicked( System::Object ^ mpSender, System::EventArgs ^ mpArgs );

private:

  /// redraw viewer
  void Redraw();

// data
private:
  
  /// unmanged viewer implementation
  vxViewerControl * m_pViewer;

  /// routes all input messages to the contained viewers
  vxInputAdapter * m_pInputAdapter;  

  /// managed environment
  Viatronix::Visualization::Environment ^ m_mpEnvironment;

  /// viewer type
  Viatronix::Visualization::IViewerType ^ m_mpViewerType;

  /// sub viewer type
  Viatronix::Visualization::IViewerType ^ m_mpSubViewerType;

  /// managed toolbar
  Viatronix::Visualization::Viewerbar ^ m_mpViewerbar;

  /// managed rendering manager plugin
  Viatronix::Visualization::Graph::RenderingNodePlugin ^ m_mpRenderingManagerPlugin;

  /// popup menu
  Viatronix::UI::PopupMenu ^ m_mpPopupMenu;
  
  /// supported animations
  array< System::Type ^ > ^ m_mpSupportedAnimations;
  
  /// indicates if the subview can be shown
  bool m_bAllowSubView;

  /// last known mouse position 
  System::Drawing::Point m_lastMousePosition;

  /// indicates if the right mouse button is down
  bool  m_bRightButtonDown;
 
  /// copy of WM_RBUTTONDOWN message
  System::Windows::Forms::Message  m_lastRButtonDownMsg;
 
}; // Viewer

#define Point Point3D

CLOSE_VISUALIZATION_NS


// $Log: v3D_Viewer.h,v $
// Revision 1.28  2007/10/29 20:34:27  gdavidson
// Added a Redraw method
//
// Revision 1.27  2007/10/24 20:08:15  gdavidson
// Moved vxViewerImpl to vxViewer and renamed vxViewerControl
//
// Revision 1.26  2007/05/17 20:05:05  gdavidson
// Added SetSubViewerSize
//
// Revision 1.25  2007/05/03 21:35:39  gdavidson
// Added moved visualizationGraph to Viatronix.Visualization.Graph namespace
//
// Revision 1.24  2007/04/18 17:01:01  gdavidson
// Add IsBeating method
//
// Revision 1.23  2007/02/05 19:29:12  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.22  2007/01/16 17:09:51  gdavidson
// Added RenderingNodePlugin
//
// Revision 1.21  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.20  2006/09/28 21:07:30  romy
// moved undo actions to its on class
//
// Revision 1.19  2006/09/27 17:06:28  geconomos
// updated for class name changes to InutAdapter and ViewerImpl
//
// Revision 1.18  2006/08/03 14:24:11  geconomos
// temporary support for old and new rendering mechanisms
//
// Revision 1.17  2006/07/14 13:32:43  geconomos
// more code cleanup
//
// Revision 1.16  2006/07/12 19:33:56  geconomos
// code clean up
//
// Revision 1.15  2006/04/25 20:22:34  romy
// StackModifiedFuncPtr added
//
// Revision 1.14  2006/04/25 15:16:12  romy
// Undo action exposed
//
// Revision 1.13  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.12  2005/09/26 13:49:00  gdavidson
// Removed Snapshot object from Visualization.
//
// Revision 1.11  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.10.2.1  2005/02/09 15:25:04  geconomos
// Changed ShowViewerPopup event name to "ContextMenu"
//
// Revision 1.10  2001/01/01 16:03:40  frank
// Enabled arrow key usage
//
// Revision 1.9  2004/10/13 16:04:09  geconomos
// made some properties hidden from the forms designer
//
// Revision 1.8  2004/08/17 14:53:49  geconomos
// Reimplemented viewer creatation and destruction to utilize OnHandleCreated and OnHandleDestroyed control methods.
//
// Revision 1.7  2004/07/09 18:40:01  geconomos
// Wired up to the viewerbar's "Click" event in order to set the viewer as selected when the viewerbar is initially clicked.
//
// Revision 1.6  2004/06/15 19:47:39  geconomos
// Moved context menu support from MainForm.cs to this class.
//
// Revision 1.5  2004/05/20 12:47:54  frank
// added controllers
//
// Revision 1.4  2004/04/07 20:32:54  geconomos
// Removed NotifyParent method.
//
// Revision 1.3  2004/03/30 15:11:15  geconomos
// Added property for border width.
//
// Revision 1.2  2004/03/05 22:25:58  geconomos
// Updated unused OnModified method
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.52  2004/02/09 15:35:35  frank
// Added initialization interface support
//
// Revision 1.51  2004/01/10 11:01:58  geconomos
// Added resize methods to reposition viewerbar when viewer is resized or moved.
//
// Revision 1.50  2003/12/16 13:35:24  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.49  2003/12/08 21:21:12  frank
// Exposed WndProc
//
// Revision 1.48  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.47  2003/11/05 13:39:05  frank
// Added the ability to handle the right mouse using manipulators instead of the popup menu
//
// Revision 1.46  2003/10/25 00:38:36  frank
// code review
//
// Revision 1.45  2003/10/15 01:27:31  geconomos
// Added ExcludedRenderingModes property.
//
// Revision 1.44  2003/09/16 16:03:21  frank
// Exposed function to set viewer type active
//
// Revision 1.43  2003/09/05 14:59:03  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.42  2003/08/22 03:45:18  geconomos
// Added supported annotations propery.
//
// Revision 1.41  2003/08/20 14:28:55  michael
// added "mailto:"
//
// Revision 1.40.2.1  2003/08/17 21:50:50  geconomos
// Changed return type of TakeScreenShot() fron Snapshot to System::Dragwing::Bitmap.
//
// Revision 1.40  2003/07/24 13:04:19  michael
// added redraw border instead of causing a full Redraw on the viewer and its
// clients (renderers)
//
// Revision 1.39  2003/07/17 17:34:06  geconomos
// GetScreenShot() now returns a Snaphot object.
//
// Revision 1.38  2003/07/03 16:56:20  geconomos
// Changed property Snapshot to GetScreenShot
//
// Revision 1.37  2003/05/27 19:10:18  geconomos
// Made viewer active when clicking the viewerbar.
//
// Revision 1.36  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.35  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.34  2003/05/13 13:52:11  geconomos
// More coding standards.
//
// Revision 1.33  2003/05/13 13:50:04  geconomos
// Coding standards.
//
// Revision 1.32  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.31  2003/04/25 13:46:26  geconomos
// Added popupMenu property.
//
// Revision 1.30  2003/04/07 18:02:46  michael
// fixed background color for MPR views
//
// Revision 1.29  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.28  2003/03/20 13:44:55  geconomos
// Added ViewerType and SubViewerType properties.
//
// Revision 1.27  2003/03/17 20:45:26  geconomos
// Added a property to allow/disallow the sub view window.
//
// Revision 1.26  2003/03/04 15:35:53  geconomos
// Added RemoveAllRenderers and RemoveAllManipulators methods.
//
// Revision 1.25  2003/02/19 15:23:14  geconomos
// More coding conventions.
//
// Revision 1.24  2002/11/26 14:46:00  geconomos
// Now checking vxInputFeedback list for manipulation completion.
//
// Revision 1.23  2002/11/20 13:48:29  geconomos
// Removed unused Snapshot and Deleted events
//
// Revision 1.22  2002/11/15 21:46:26  geconomos
// Updated to use new ViewType enum value from vxUtils.
//
// Revision 1.21  2002/11/11 20:16:53  geconomos
// Cleaned up header.
//
// Revision 1.20  2002/11/07 13:02:19  geconomos
// Added border color property to viewer.
//
// Revision 1.19  2002/11/06 22:02:36  geconomos
// Added HasRenderer and HasManipulator methods.
//
// Revision 1.18  2002/11/05 20:52:17  geconomos
// Implementation of new viewerbar
//
// Revision 1.17  2002/11/04 14:30:36  geconomos
// new toolbar class.
//
// Revision 1.16  2002/10/21 15:43:33  geconomos
// Removed some keyboarding handling routines to application classes.
//
// Revision 1.15  2002/09/24 15:16:26  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Viewer.h,v 1.28 2007/10/29 20:34:27 gdavidson Exp $
// $Id: v3D_Viewer.h,v 1.28 2007/10/29 20:34:27 gdavidson Exp $
