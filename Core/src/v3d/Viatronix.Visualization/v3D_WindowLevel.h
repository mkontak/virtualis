// $Id: v3D_WindowLevel.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
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
#include "vxWindowLevel.h"
#include "vxShareableObject.h"


// foreward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class WindowLevel: public Viatronix::Visualization::IObserver
{
// IObserver
public:

  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

//  functions
public:
  
  /// initializer
  WindowLevel();
  
  /// destructor
  virtual ~WindowLevel();

  /// finalizer
  !WindowLevel();

  /// gets a vxWindowLevel pointer
  vxShareableObject< vxWindowLevel > * GetWindowLevelPtr() { return m_pWindowLevel; }
  
  /// gets or sets the window value
  property float Window
  {
    float get();
    void set( float fWindow );
  }

  /// gets or sets the level value
  property float Level
  {
    float get();
    void set( float fLevel );
  }
  
  /// gets or sets the associated title
  property System::String ^ Title
  {
    System::String ^ get() { return m_mpTitle; }
    void set( System::String ^ mpTitle ) { m_mpTitle = mpTitle; }
  }
  
// member variables
private:

  // shared rendering mode object
  vxShareableObject< vxWindowLevel > * m_pWindowLevel;
  
  // observer
  vxManagedObserver * m_pObserver;

  // associated title
  System::String ^ m_mpTitle;

}; // WindowLevel


CLOSE_VISUALIZATION_NS 


// Revision history:
// $Log: v3D_WindowLevel.h,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/12/16 13:27:53  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.11  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.10  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.9  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.8  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.7  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.6  2003/05/13 13:28:48  michael
// code review
//
// Revision 1.5  2003/03/04 15:36:30  geconomos
// Code cleanup.
//
// Revision 1.4  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.3  2002/09/24 14:34:37  geconomos
// Exception handling and formatting.
//
// Revision 1.2  2002/08/22 23:58:48  dmitry
// Serialization implemented.
//
// Revision 1.1  2002/07/30 20:23:26  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_WindowLevel.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_WindowLevel.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $