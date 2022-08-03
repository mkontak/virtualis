// $Id: v3D_PlaneOblique.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.ncom)


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_Enums.h"
#include "vxShareableObject.h"
#include "vxPlaneOblique.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// forward managed declarations
ref class Volume;


/**
 * Plane oblique class.
 */

public ref class  PlaneOblique :
  public IElement,
  public IObserver
{
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pPlaneOblique ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// constructors and finalizers
public:
  
  /// default constructor
  PlaneOblique();

  // constructor from orientation.
  PlaneOblique( Viatronix::Visualization::Orientations eOrientation );

  // constructor from orientation and volume
  PlaneOblique( Viatronix::Visualization::Orientations eOrientation, Viatronix::Visualization::Volume ^ mpVolume );

  /// destructor
  virtual ~PlaneOblique();

  /// finalizer
  !PlaneOblique();

  /// Initialize the data on an orientation and volume
  void Initialize( Viatronix::Visualization::Orientations eOrientation, Viatronix::Visualization::Volume ^ mpVolume ); 

private:

  // initialize the object
  void CommonInit( Viatronix::Visualization::Orientations eOrientation );

  // sets the plane's normal to the specified orientation
  void SetNormalToOrientation( Viatronix::Visualization::Orientations eOrientation );

// fields
private:

  /// the native oblique plane
  vxShareableObject< vxPlaneOblique<float4> > * m_pPlaneOblique;

  /// the observer
  vxManagedObserver * m_pObserver;

// properties
public:

    /// sets the orientation
  property float Distance 
  {
    float get();
    void set( float fDistance );
  }

}; // class PlaneOblique


/**
 * Convert a managed point to an unmanaged pointer
 *
 * @param pointer the managed pointer
 *
 * @return 
 */
inline vxShareableObject< vxPlaneOblique<float4> > * ToNativePlaneObliquePtr( System::IntPtr pointer )
{
  return _native_ptr_t< vxPlaneOblique<float4> >( pointer );
} // ToNativePlaneObliquePtr()


CLOSE_VISUALIZATION_NS


// $Log: v3D_PlaneOblique.h,v $
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
// Revision 1.10  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.2.1  2004/02/24 20:09:48  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.9  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.8  2003/05/13 17:37:40  frank
// code review
//
// Revision 1.7  2003/05/13 17:19:22  frank
// code review
//
// Revision 1.6  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.5  2002/11/18 20:15:58  michael
// added distance
//
// Revision 1.4  2002/11/18 17:51:48  michael
// additional properties
//
// Revision 1.3  2002/11/18 16:36:03  michael
// completed plane object renaming
//
// Revision 1.2  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneOblique.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_PlaneOblique.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
