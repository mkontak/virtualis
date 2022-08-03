// $Id: v3D_CycleSettings.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// includes
#include "vxCycleSettings.h"
#include "v3D_Interfaces.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


public ref class CycleSettings : public IElement, public IObserver
{
public:

  /// default constructor
  CycleSettings();

  /// destructor
  virtual ~CycleSettings();

  /// finalizer
  !CycleSettings();
  
  /// gets sets if camera flying is enabled
  property bool Enabled
  {
    bool get() { return m_pCycleSettings->IsEnabled(); }
    void set( bool bEnabled ) { m_pCycleSettings->SetEnabled( bEnabled ); }
  } // Enabled

// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pCycleSettings ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

// members
private:

  /// unmanaged settings
  vxShareableObject< vxCycleSettings > * m_pCycleSettings;

    // observer 
  vxManagedObserver * m_pObserver;

}; // class CycleSettings


CLOSE_VISUALIZATION_NS


// $Log: v3D_CycleSettings.h,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CycleSettings.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_CycleSettings.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $

