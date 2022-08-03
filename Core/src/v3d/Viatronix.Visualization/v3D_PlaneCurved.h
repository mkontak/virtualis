// $Id: v3D_PlaneCurved.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
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
#include "v3D_Enums.h"
#include "vxShareableObject.h"
#include "vxPlaneCurved.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class declaration
public ref class  PlaneCurved : public IElement, public IObserver
{
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pPlaneCurved ); }

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

// functions
public:

  /// initializer
  PlaneCurved();

  /// destructor
  virtual ~PlaneCurved();

  /// finalizer
  !PlaneCurved();

private:

  /// initializes the object
  void Initialize();

// data
private:

  // vxPlaneCurved pointer
  vxShareableObject< vxPlaneCurved  > * m_pPlaneCurved;

  // observer
  vxManagedObserver * m_pObserver;

}; // PlaneCurved


CLOSE_VISUALIZATION_NS


// $Log: v3D_PlaneCurved.h,v $
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
// Revision 1.6  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.5  2003/10/29 20:54:48  frank
// Removed double rendering in curved MPR
//
// Revision 1.4  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.3  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.2  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.1  2002/11/18 15:54:15  michael
// added renamed version fitting to vxBase objet's name
//
// Revision 1.2  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.1  2002/10/21 22:19:55  frank
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneCurved.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_PlaneCurved.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
