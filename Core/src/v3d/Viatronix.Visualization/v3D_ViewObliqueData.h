// $Id: v3D_ViewObliqueData.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// Pragmas
#pragma once


// Includes
#include "v3D_Interfaces.h"
#include "v3D_Enums.h"
#include "vxShareableObject.h"
#include "vxViewObliqueData.h"


// Forward declarations
class vxManagedObserver;


// Namespaces
OPEN_VISUALIZATION_NS


/**
 * Plane oblique class.
 */
public ref class ViewObliqueData :
  public IElement,
  public IObserver
{

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pViewObliqueData ); }

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
  ViewObliqueData();
  
  /// destructor
  virtual ~ViewObliqueData();
    
  /// finalizer
  !ViewObliqueData();

// fields
private:

  /// the unmanaged pointer to the view oblique data
  vxShareableObject< vxViewObliqueData > * m_pViewObliqueData;

  /// the observer
  vxManagedObserver * m_pObserver;

}; // class ViewObliqueData


/**
 * Creates an unmanaged pointer from a managed pointer
 *
 * @param pointer the managed pointer object
 */
inline vxShareableObject< vxViewObliqueData > * ToNativeViewObliqueDataPtr( System::IntPtr pointer )
{
  return _native_ptr_t< vxViewObliqueData >( pointer );
} // ToNativeViewObliqueDataPtr()


CLOSE_VISUALIZATION_NS

// $Log: v3D_ViewObliqueData.h,v $
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:11:46  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/24 20:13:24  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.5  2003/12/16 13:27:52  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.4  2003/05/14 13:27:43  geconomos
// Spelling error.
//
// Revision 1.3  2003/05/13 17:37:40  frank
// code review
//
// Revision 1.2  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.1  2002/11/13 22:08:11  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.2  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewObliqueData.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_ViewObliqueData.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
