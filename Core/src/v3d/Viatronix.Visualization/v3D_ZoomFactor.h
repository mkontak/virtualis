// $Id: v3D_ZoomFactor.h,v 1.4 2006/10/04 18:29:07 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxZoomFactor.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class ZoomFactor : public IObserver, public IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pZoomFactor ); }

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

/// functions
public:

  /// initializer
  ZoomFactor();

  /// destructor
  virtual ~ZoomFactor();

  /// finalizer
  !ZoomFactor();

// data
private:

  vxShareableObject<vxZoomFactor> * m_pZoomFactor;

}; // ZoomFactor


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_ZoomFactor.h,v $
// Revision 1.4  2006/10/04 18:29:07  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:12:02  frank
// improved the extended elements modified interface
//
// Revision 1.1  2004/04/26 19:28:08  michael
// Added managed zoom factor class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ZoomFactor.h,v 1.4 2006/10/04 18:29:07 gdavidson Exp $
// $Id: v3D_ZoomFactor.h,v 1.4 2006/10/04 18:29:07 gdavidson Exp $
