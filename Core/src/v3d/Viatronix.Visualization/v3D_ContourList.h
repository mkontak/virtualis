// $Id: v3D_ContourList.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
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
#include "vxContourList.h"


// forward declaration
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class ContourList : public IElement, public IObserver
{
// events
public:

  // raised when the restricted lasso has completed 
  event System::EventHandler ^ Completed;

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pContourList ); }

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
  ContourList();
  
  /// destructor
  virtual ~ContourList();

  /// finalizer
  !ContourList();

  /// returns unmanaged vxContour pointer
  vxShareableObject< vxContourList > * GetContourPtr() { return m_pContourList; }

// data
private:

  // shared restricted lasso object
  vxShareableObject< vxContourList > * m_pContourList;

  // observer
  vxManagedObserver * m_pObserver;
  
}; // class ContourList


CLOSE_VISUALIZATION_NS


// $Log: v3D_ContourList.h,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:07:18  frank
// improved the extended elements modified interface
//
// Revision 1.1  2004/03/12 18:02:33  frank
// working on removing annotation ROI - first renaming contour to contour list
//
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ContourList.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_ContourList.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
