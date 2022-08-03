// $Id: v3D_PlaneOrtho.h,v 1.4.8.2 2009/07/29 19:54:06 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
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
#include "v3D_Enums.h"
#include "vxShareableObject.h"
#include "vxPlaneOrtho.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// forward managed declarations
ref class Volume;


// class definition
public ref class  PlaneOrtho : public IElement, public IObserver
{
// events
public:

  /// raised when the PlaneOrtho object is modified
  event System::EventHandler ^ Changed;

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:
  
  /// initializer
  PlaneOrtho();
  
  // initializer from orientation.
  PlaneOrtho( Viatronix::Visualization::Orientations eOrientation );

  // initializer from orientation and volume
  PlaneOrtho( Viatronix::Visualization::Orientations eOrientation, Viatronix::Visualization::Volume ^ mpVolume );
 
  /// destructor
  virtual ~PlaneOrtho();

  /// finalizer
  !PlaneOrtho();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pPlane ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr mpObserver);

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr mpObserver );

  /// Initializes the PlaneOrtho object from the specified volume.
  void Initialize( Viatronix::Visualization::Volume ^ mpVolume ); 
  
  /// increments one slice
  float Increment();

  /// decrements one slice
  float Decrement();

  /// gets a shareable vxPlaneOrtho<float4> pointer
  System::IntPtr GetPlaneOrthoPtr() { return System::IntPtr( m_pPlane ); }

private:

  // initializes the PlaneOrtho object from the specified orientation
  void CommonInit( Viatronix::Visualization::Orientations eOrientation );

  // sets the plane's normal to the specified orientation
  void SetNormalToOrientation( Viatronix::Visualization::Orientations eOrientation );

// event handlers
public:
  
  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// sets the orientation
  property Viatronix::Visualization::Orientations Orientation 
  {
    Viatronix::Visualization::Orientations get();
    void set( Viatronix::Visualization::Orientations eOrientation );
  } // Orientation

  /// sets the orientation
  property float Distance 
  {
    float get();
    void set( float fDistance );
  } // Distance

// member variables
private:

  /// vxPlaneOrtho<float4> pointer
  vxShareableObject< vxPlaneOrtho<float4>  > * m_pPlane;

  /// observer
  vxManagedObserver * m_pObserver;

}; // class PlaneOrtho 


CLOSE_VISUALIZATION_NS


// $Log: v3D_PlaneOrtho.h,v $
// Revision 1.4.8.2  2009/07/29 19:54:06  kchalupa
// Code Walkthrough
//
// Revision 1.4.8.1  2008/11/18 15:17:26  gdavidson
// Determine orientation enum from plane in native pointer
//
// Revision 1.4  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:10:45  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.17  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.16  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.15  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.14  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.13  2002/11/18 17:51:48  michael
// additional properties
//
// Revision 1.12  2002/11/18 16:36:02  michael
// completed plane object renaming
//
// Revision 1.11  2002/11/13 15:03:10  geconomos
// Removed EVENT_FLOAT_DISTANCE
//
// Revision 1.10  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.9  2002/11/06 22:03:40  geconomos
// Changed GetPlaneOrthoPtr() to return System::IntPtr
//
// Revision 1.8  2002/10/03 19:40:42  frank
// Added Increment() and Decrement().
//
// Revision 1.7  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// Revision 1.6  2002/08/26 21:04:59  dmitry
// Not very good comments added to the properties.
//
// Revision 1.5  2002/08/07 21:27:57  geconomos
// Added events.
//
// Revision 1.4  2002/07/19 12:49:30  manju
// Moved SetDistance to cpp file.
//
// Revision 1.3  2002/07/15 23:03:41  frank
// Added distance property.
//
// Revision 1.2  2002/07/03 13:15:55  geconomos
// Continued effort!
//
// Revision 1.1  2002/07/02 20:30:13  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneOrtho.h,v 1.4.8.2 2009/07/29 19:54:06 kchalupa Exp $
// $Id: v3D_PlaneOrtho.h,v 1.4.8.2 2009/07/29 19:54:06 kchalupa Exp $
