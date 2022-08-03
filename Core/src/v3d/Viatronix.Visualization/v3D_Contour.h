// $Id: v3D_Contour.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
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
#include "vxShareableObject.h"
#include "vxContour.h"


// forward declaration
class Observer;


// namespaces
OPEN_VISUALIZATION_NS

// class definition
public __gc class Contour : public IElement, public IObserver
{
// events
public:

  // raised when the restricted lasso has completed 
  __event System::EventHandler * Completed;

// IElement implementation
public:

  /// returns a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pContour ); }

  /// connects an observer to the object
  void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  void Disconnect( System::IntPtr observer );

// IObserver implementation
public:

  /// processes modified queue events
  void OnModified( System::IntPtr modifiedList );
  
// functions
public:
  
  /// initializer
  Contour();
  
  /// finalizer
  ~Contour();

  /// returns unmanaged vxContour pointer
  vxShareableObject< vxContour > * GetContourPtr() { return m_pContour; }

// data
private:

  // shared restricted lasso object
  vxShareableObject< vxContour > * m_pContour;

  // observer
  Observer * m_mpObserver;
}; // Contour


CLOSE_VISUALIZATION_NS


// $Log: v3D_Contour.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.6  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.5  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.4  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.3  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.2  2002/11/08 23:58:23  frank
// Added a contour element to the environment.
//
// Revision 1.1  2002/11/06 22:19:41  frank
// Added Contour renderer, manipulator, and asociated classes.
//
// Revision 1.6  2002/09/30 14:39:16  dmitry
// EventCompleted added
//
// Revision 1.5  2002/09/26 14:51:52  frank
// Moved InitializeData into C++.
//
// Revision 1.4  2002/09/24 18:59:01  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_Contour.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id: v3D_Contour.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
