// $Id: v3D_Camera.h,v 1.7 2006/10/04 18:29:04 gdavidson Exp $
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
#include "v3D_Volume.h"
#include "vxCamera.h"
#include "vxShareableObject.h"
#include "ColorList.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class Camera : 
  public IElement, 
  public IObserver,
  public System::ICloneable
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pCamera ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:

  /// receives modified information on connected objects
  virtual void OnModified( System::IntPtr modifiedList );
  
// ICloneable implementation
public:

  /// creates a new object that is a copy of the current instance.
  virtual System::Object ^ Clone();

// Serialization
public:

  /// serializes the annotations object to xml
  System::String ^ ToXml();

  /// creates a new camera instance from the specified xml string
  static Camera ^ FromXml( System::String ^ mpXml );

  /// creates a new camera instance from the specified xml resource
  static Camera ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new camera instance from the specified xml file
  static Camera ^ FromFile( System::String ^ mpXmlFile );

// functions
public:
  
  /// initializer
  Camera();
  
  /// destructor
  virtual ~Camera();

  /// finalizer
  !Camera();
  
  /// copies the specified camera settings into this colormap
  void CopyFrom( Camera ^ mpCamera, Viatronix::Visualization::Volume ^ mpVolume );

  /// resets the camera to home position
  void Reset();

  /// assigned color property (READ ONLY)
  property System::Drawing::Color AssignedColor
  {
    System::Drawing::Color get() { return m_color; }
  }

private:

  ///common initialization routine
  void CommonInit();

// data
private:

  // shared camera object
  vxShareableObject< vxCamera > * m_pCamera;
  
  // observer
  vxManagedObserver * m_pObserver;

  // assigned color
  System::Drawing::Color m_color;

  // color list
  static ColorList * m_pColors = new ColorList();

}; // class Camera


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_Camera.h,v $
// Revision 1.7  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/07/29 16:25:00  frank
// removed dead code
//
// Revision 1.4  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.2.1  2005/02/17 21:00:21  geconomos
// cleaned up ICloneable
//
// Revision 1.3  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.2  2004/04/26 19:05:15  michael
// removed ViewAll from camera, need to use utilities
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2003/12/23 21:40:11  michael
// replace typo stemming from copy&paste
//
// Revision 1.20  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.19  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.18  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.17  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.16  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.15  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.14  2003/05/05 17:13:04  geconomos
// Coding standards.
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
// Chaged GetCameraPtr() to return a System::IntPtr for interoperbility.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Camera.h,v 1.7 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Camera.h,v 1.7 2006/10/04 18:29:04 gdavidson Exp $