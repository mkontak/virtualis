// $Id: v3D_FullScreen.h,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $
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
#include "vxFullScreen.h"
#include "vxShareableObject.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


/**
 * Definition for the fullscreen mode
 */
public ref class FullScreen : public IElement, public IObserver
{
// events
public:

  // fired when an annotation is modified
  event System::EventHandler ^ Changed;

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:
  
  /// initializer
  FullScreen();

  /// copy consturctor
  FullScreen( FullScreen ^ mpOther );

  /// destructor
  virtual ~FullScreen();

  /// finalizer
  !FullScreen();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pFullScreen ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

private:

  // common initializtion routine
  void CommonInit();

// event handlers
public:

  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// returns the viewer
  property Viatronix::Visualization::Viewer ^ Viewer
  {
    Viatronix::Visualization::Viewer ^ get();
  } // Viewer


  /// returns the viewer
  property int4 Proportion
  {
    int4 get() { return m_pFullScreen->GetProportion(); }
    void set( int4 iProportion ) { return m_pFullScreen->SetProportion( iProportion ); }
  } // Viewer

// member variables
private:

  // shared colormap object
  vxShareableObject< vxFullScreen > * m_pFullScreen;

  // observer
  vxManagedObserver * m_pObserver;

}; // class FullScreen


CLOSE_VISUALIZATION_NS


// $Log: v3D_FullScreen.h,v $
// Revision 1.1.2.2  2009/07/29 16:12:58  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2008/11/24 17:11:03  gdavidson
// Issue #6171: Initial revision
//
// Revision 1.6  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.4  2006/09/25 20:13:14  geconomos
// added copy constructor
//
// Revision 1.3  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/17 21:00:21  geconomos
// cleaned up ICloneable
//
// Revision 1.2  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.24  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.23  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.22  2003/06/09 12:29:53  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.21  2003/05/27 14:20:01  geconomos
// Made GetName and SetName a propery of the class.
//
// Revision 1.20  2003/05/23 18:30:15  michael
// name is now pulled from/set in unmanaged object.
//
// Revision 1.19  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.18  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.17  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.16  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.15  2003/04/16 12:48:55  michael
// added calls to set the active/full range in the vxColormap objects
//
// Revision 1.14  2003/04/15 17:41:04  michael
// added active range
//
// Revision 1.13  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.12  2003/03/30 21:05:08  michael
// formatting and cosmetics
//
// Revision 1.11  2003/03/21 16:52:08  michael
// formatting
//
// Revision 1.10  2002/11/27 16:31:20  geconomos
// Added CopyFrom method
//
// Revision 1.9  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.8  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.7  2002/11/06 23:53:10  wenli
// Returned System::IntPtr instead of native pointer.
//
// Revision 1.6  2002/10/22 21:11:40  geconomos
// Added copy constructor from v3D Colormap.
//
// Revision 1.5  2002/10/17 15:28:16  geconomos
// Added constructor from native type and created CommonInit() method
//
// Revision 1.4  2002/10/08 19:02:30  michael
// added event handler to get the colormap GUI updated
//
// Revision 1.3  2002/10/01 23:04:23  frank
// Adding FromResource function.  Needs more work.
//
// Revision 1.2  2002/09/25 20:37:02  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.1  2002/09/20 22:16:03  michael
// Added colormap wrapper class
//
// Revision 1.5  2002/09/19 14:26:00  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.4  2002/09/17 14:37:12  geconomos
// Removed property AlphaFromWindow
//
// Revision 1.3  2002/08/22 23:58:48  dmitry
// Serialization implemented.
//
// Revision 1.2  2002/08/02 13:53:31  dmitry
// Color List added.
//
// Revision 1.1  2002/07/31 14:10:29  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_FullScreen.h,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $
// $Id: v3D_FullScreen.h,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $