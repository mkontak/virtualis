// $Id: v3D_Interfaces.h,v 1.10 2008/01/28 19:53:25 cqian Exp $
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
#pragma warning (disable : 4803) // event handlers in an interface generates warnings


/// includes
#include "v3D_Enums.h"


/// forward declarations
class vxEnvironment;
class vxManipulator;
class vxRenderer;
class vxElement;
class vxModifiedInfoList;
class vxModifiedInfo;



// namespaces
OPEN_VISUALIZATION_NS


// forward managed declarations
ref class Viewer;


// interface definition: IManipulatorFactory
//////////////////////////////////////////////
public interface class IManipulatorFactory
{
  /// creates a vxManipulator
  System::IntPtr CreateManipulator( System::IntPtr env );

}; // interface IManipulatorFactory  


// interface definition: IControllerFactory
//////////////////////////////////////////////
public interface class IControllerFactory
{
  /// creates a vxController
  System::IntPtr CreateController( System::IntPtr env );

}; // interface IControllerFactory


// interface definition: IControllerFactory
//////////////////////////////////////////////
public interface class IGlobalControllerFactory
{
  /// creates a vxController
  System::IntPtr CreateController();

}; // interface IControllerFactory


// interface definition: IRendererFactory
///////////////////////////////////////////
public interface class IRendererFactory
{
  /// creates a vxRenderer
  System::IntPtr CreateRenderer( System::IntPtr env );

}; // interface IRenderer  


// interface definition: IAnnotationFactory
/////////////////////////////////////////////
public interface class IAnnotationFactory
{
  /// creates a vxAnnotation
  System::IntPtr CreateAnnotation();

}; // interface IManipulatorFactory  


// interface definition: IElement
///////////////////////////////////
public interface class IElement
{
  /// returns an unmanaged pointer to implementing type
  virtual System::IntPtr GetElementPtr();

  /// connects to the specified vxObserver
  virtual void Connect( System::IntPtr observer );
  
  /// disconnects from the specified vxObserver
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

}; // interface IElement  


// interface definition: IObserver
////////////////////////////////////
public interface class IObserver
{
  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );
}; // interface IObserver


// interface definition: IViewerType
//////////////////////////////////////
public interface class IViewerType
{
  /// gets the implementing vxViewerType pointer
  System::IntPtr GetViewerTypePtr();

  /// initializes the view
  void InitializeView();

  /// gets if the viewer is 2D
  property bool Is2D { bool get(); }

  /// gets if the viewer is 3D
  property bool Is3D { bool get(); }

}; // interface IViewerType


// interface definition: IDropSource
//////////////////////////////////////
public interface class IDropSource
{
  /// gets the drop's data type
  property Viatronix::Visualization::DropType DropType { Viatronix::Visualization::DropType get(); }

  /// called when drop occurs on a viewer
  void Drop( Viatronix::Visualization::Viewer ^ mpViewer, bool bCopy  );

  /// Is supports Sharing
  bool IsSupportShare(); 

  /// Is supports copying
  bool IsSupportCopy(); 

}; // interface IDropSource



CLOSE_VISUALIZATION_NS


// $Log: v3D_Interfaces.h,v $
// Revision 1.10  2008/01/28 19:53:25  cqian
// Add a cut plane for LV segmentation
//
// Revision 1.9  2007/03/23 15:36:26  romy
// Extended IDropSource interfaces
//
// Revision 1.8  2006/11/09 15:02:06  gdavidson
// Added IGlobalControllerFactory
//
// Revision 1.7  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2004/10/13 15:52:05  geconomos
// Removed IViewerbar interface.
//
// Revision 1.5  2004/10/04 15:12:27  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.4  2004/07/09 18:38:28  geconomos
// Added "Bounds" property to IViewerbarButton.
//
// Revision 1.3  2004/05/20 12:42:58  frank
// added the controller factory
//
// Revision 1.2  2004/03/02 19:19:17  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.34  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.33  2003/10/22 14:40:22  frank
// Added camera initialization to the viewer type
//
// Revision 1.32  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.31  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.30  2003/05/06 18:51:41  geconomos
// Coding standards
//
// Revision 1.29  2003/04/16 18:50:22  geconomos
// Issue #3110:  Buttons not refreshing after drop event.
//
// Revision 1.28  2003/04/08 14:54:52  geconomos
// Issue #2398: Tooltips are now available for viewerbar buttons.
//
// Revision 1.27  2003/03/20 13:44:06  geconomos
// Removed inclusion of v3D_Viewer.h
//
// Revision 1.26  2003/03/17 19:48:01  geconomos
// Issue #2914. Implemented DropType property from vxViewer.
//
// Revision 1.25  2003/03/06 16:01:57  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.24  2003/01/13 14:46:12  geconomos
// Added IAnnotationFactory interface.
//
// Revision 1.23  2002/12/18 15:46:11  geconomos
// added IViewerType interface
//
// Revision 1.22  2002/11/20 13:48:01  geconomos
// implemented click on IViewerbarButton
//
// Revision 1.21  2002/11/18 20:39:40  geconomos
// New viewerbar implementation
//
// Revision 1.20  2002/11/12 21:31:19  geconomos
// Removed IPlane.
//
// Revision 1.19  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.18  2002/11/06 23:44:10  geconomos
// Cahnged IToolbarButton to have a MenuItem[] instead of ContextMenu
//
// Revision 1.17  2002/11/06 15:01:06  geconomos
// Changed definition of IToolbarButton
//
// Revision 1.16  2002/11/05 23:17:05  ingmar
// removed obsolete include
//
// Revision 1.15  2002/11/05 20:50:42  geconomos
// Added methods to IToolbarButton and added IDropSource.
//
// Revision 1.14  2002/11/04 14:30:36  geconomos
// new toolbar class.
//
// Revision 1.13  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.12  2002/09/24 18:17:54  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Interfaces.h,v 1.10 2008/01/28 19:53:25 cqian Exp $
// $Id: v3D_Interfaces.h,v 1.10 2008/01/28 19:53:25 cqian Exp $
