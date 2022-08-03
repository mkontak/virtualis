// $Id: v3D_EnvironmentList.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxEnvironmentList.h"


// forward unmanaged declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// forward declaration
ref class Environment;


// class definition
public ref class EnvironmentList : public IElement, public IObserver
{

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pEnvironmentList ); }

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
  
// functions
public:
  
  /// initializer
  EnvironmentList();
  
  /// destructor
  virtual ~EnvironmentList();

  /// finalizer
  !EnvironmentList();

  /// returns unmanaged vxEnvironment pointer
  vxShareableObject< vxEnvironmentList > * GetEnvironmentListPtr() { return m_pEnvironmentList; }

  /// adds the environment to the list
  void Add( Environment ^ mpEnvironment );

  /// removes the environment from the list
  void Remove( Environment ^ mpEnvironment );

  /// clears all the environments from the list
  void Clear();

// data
private:

  // shared restricted lasso object
  vxShareableObject< vxEnvironmentList > * m_pEnvironmentList;

  // observer
  vxManagedObserver * m_pObserver;
  
}; // class EnvironmentList


CLOSE_VISUALIZATION_NS


// $Log: v3D_EnvironmentList.h,v $
// Revision 1.3  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/03/27 19:41:49  gdavidson
// new build
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_EnvironmentList.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_EnvironmentList.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
