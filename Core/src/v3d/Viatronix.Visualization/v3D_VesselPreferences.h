// $Id: v3D_VesselPreferences.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once
#pragma warning (disable : 4803) // event handlers in an interface generates warnings


// includes
#include "vxVesselPreferences.h"
#include "v3D_Interfaces.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


public ref class VesselPreferences : public Viatronix::Visualization::IElement,
                                     public Viatronix::Visualization::IObserver
{

// IElement implementation
public:

  /// Get a pointer to the IElement
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_mpVesselPreferences ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr object );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr object );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedInfoList );

// events
public:

  /// When the object is modified
  event System::EventHandler ^ Changed;

public:

  /// constructor
  VesselPreferences();

  /// destructor
  virtual ~VesselPreferences();

  /// finalizer
  !VesselPreferences();

  /// reset
  void Reset();

// properties
public:

  /// get the display of vessel outline
  property bool DisplayOutline
  {
    bool get();
    void set( bool bDisplayOutline );
  }

  /// get the display of vessel centerline
  property bool DisplayCenterline
  {
    bool get();
    void set( bool bDisplayCenterline );
  }

  /// get the display of vessel graphs
  property bool DisplayGraphs
  {
    bool get();
    void set( bool bDisplayGraphs );
  }

  /// get the rotating curved MPR mode
  property static bool RotatingCurvedMPR
  {
    bool get();
    void set( bool bRotatingCurvedMPR );
  }

private:

  /// the preferences
  vxShareableObject< vxVesselPreferences > * m_mpVesselPreferences;

  /// vxManagedObserver
  vxManagedObserver * m_pObserver;

}; // class VesselPreferences

CLOSE_VISUALIZATION_NS

// $Log: v3D_VesselPreferences.h,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/08/17 14:07:06  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VesselPreferences.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_VesselPreferences.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
