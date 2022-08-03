// $Id: v3D_SyncData.h,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Cheng Qian ( cqian@viatronix.net )


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "v3D_Environment.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS

// class declaration for ElementInt4
public ref class SyncData :
  public IObserver,
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  SyncData( );

  /// destructor
  virtual ~SyncData();

  /// finalize
  !SyncData();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

  /// initialize
  void CommonInit( );

  //add an environment
  void AddEnvironment(  Viatronix::Visualization::Environment ^ mpEnvironment ); 

  //remove a viewer 
  void RemoveEnvironment( Viatronix::Visualization::Environment ^ mpEnvironment );

/// properties
public:

  /// enables or disables the linking
  property bool Enabled
  {
    bool get();
    void set( bool bEnabled );
  } // Enabled

// member variables
private:

  /// Sharable object.
  vxShareableObject< vxSyncData > * m_pSyncData;

  /// observer
  vxManagedObserver  * m_pObserver;

}; // class SyncData

CLOSE_VISUALIZATION_NS


#pragma region reivison history

// $Log: v3D_SyncData.h,v $
// Revision 1.1.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/02/19 14:42:57  kchalupa
// Coding Standards
//
// Revision 1.1  2008/05/02 05:01:54  cqian
// check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SyncData.h,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_SyncData.h,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $

#pragma endregion
