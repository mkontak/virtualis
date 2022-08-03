// $Id: vxManagedObserver.h,v 1.2 2006/10/04 18:29:07 gdavidson Exp $
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
#include "vxObserver.h"
#include "v3D_Interfaces.h"

// class declaration
class vxManagedObserver: public vxObserver
{
// functions
public:
    
	/// constructor
  vxManagedObserver( Viatronix::Visualization::IObserver ^ mpObserver )
  { 
    m_pObserver = mpObserver;
  }

	/// implemented virtual function of vxObserver
	virtual void OnModified( vxModifiedInfoList & info ) 
  { 
    // iterate through information
    m_pObserver->OnModified( System::IntPtr( &info ) );
  }

// data
private:

  /// observer
  gcroot< Viatronix::Visualization::IObserver ^ > m_pObserver;
}; // vxManagedObserver


// $Log: vxManagedObserver.h,v $
// Revision 1.2  2006/10/04 18:29:07  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2006/09/27 17:23:33  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.11  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.10  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.9  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.8  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.7  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.6  2003/03/04 15:34:49  geconomos
// Code cleanup.
//
// Revision 1.5  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxManagedObserver.h,v 1.2 2006/10/04 18:29:07 gdavidson Exp $
// $Id: vxManagedObserver.h,v 1.2 2006/10/04 18:29:07 gdavidson Exp $
