// $Id: v3D_Cursor3d.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)

#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_Volume.h"
#include "vxCursor3d.h"
#include "vxShareableObject.h"
#include "ColorList.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class Cursor3d: public IElement, public IObserver
{
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pCursor3d ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:

  /// receives modified information on connected objects
  virtual void OnModified( System::IntPtr ptrInfo );

// Serialization
public:

  /// serializes the annotations object to xml
  System::String ^ ToXml();

  /// creates a new Cursor3d object from the specified xml string
  static Cursor3d ^ FromXml( System::String ^ mpXml );

  /// creates a new Cursor3d object from the specified xml resource
  static Cursor3d ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new Cursor3d object from the specified xml file
  static Cursor3d ^ FromFile( System::String ^ mpXmlFile );

// properties
public:

  /// returns the position
  property Triplet ^ Position
  {
    Triplet ^ get();
    void set( Triplet ^ mpPosition );
  }

  /// returns the view reference point - not the view direction
  property Triplet ^ Reference
  {
    Triplet ^ get();
    void set( Triplet ^ mpViewReferencePoint );
  }

  /// returns the view direction
  property Triplet ^ Direction
  {
    Triplet ^ get();
    void set( Triplet ^ mpViewDirection );
  }

// functions
public:
  
  /// constructor
  Cursor3d();

  /// constructor from vxCursor3d
  Cursor3d( vxCursor3d * pCursor3d );
  
  // constructor from v3D Colormap
  Cursor3d( Cursor3d ^ mpCursor3d );

  /// destructor
  virtual ~Cursor3d();

  /// finalize
  !Cursor3d();

  /// returns the shared Cursor3d object
  System::IntPtr GetCursor3dPtr() { return System::IntPtr( m_pCursor3d ); }

private:

  /// common initialization routine
  void CommonInit();

// data
private:

  // shared Cursor3d object
  vxShareableObject<vxCursor3d> * m_pCursor3d;
  
  // observer
  vxManagedObserver * m_pObserver;

}; // Cursor3d


CLOSE_VISUALIZATION_NS


// $Log: v3D_Cursor3d.h,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:07:56  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.6  2003/08/25 15:52:58  frank
// fixed function descriptions
//
// Revision 1.5  2003/05/16 13:09:37  frank
// code formatting
//
// Revision 1.4  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.3  2003/05/06 15:32:22  geconomos
// More coding standards.
//
// Revision 1.2  2003/05/06 15:14:53  michael
// code review
//
// Revision 1.1  2003/04/24 14:01:29  michael
// added vxCursor3d class
//
// Revision 1.13  2003/04/16 19:22:52  geconomos
// Issue #3100: Cemar not resetting properly.
//
// Revision 1.12  2002/11/27 16:31:19  geconomos
// Added CopyFrom method
//
// Revision 1.11  2002/11/11 19:19:06  geconomos
// Reworked serialization.
//
// Revision 1.10  2002/11/07 17:23:38  geconomos
// Added ViewAll methods.
//
// Revision 1.9  2002/11/02 03:12:27  frank
// spelling
//
// Revision 1.8  2002/10/31 12:49:57  geconomos
// Chaged GetCursor3dPtr() to return a System::IntPtr for interoperbility.
//
// Revision 1.7  2002/10/24 01:24:17  michael
// added additional constructor
//
// Revision 1.6  2002/10/18 20:30:17  frank
// Added IElement interface.
//
// Revision 1.5  2002/10/17 15:28:15  geconomos
// Added constructor from native type and created CommonInit() method
//
// Revision 1.4  2002/10/16 22:20:00  michael
// added initial serialization from resource code ... needs work :-)
//
// Revision 1.3  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// Revision 1.2  2002/08/02 19:40:25  dmitry
// added the AssignedColor property
//
// Revision 1.1  2002/08/02 13:53:56  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Cursor3d.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Cursor3d.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $