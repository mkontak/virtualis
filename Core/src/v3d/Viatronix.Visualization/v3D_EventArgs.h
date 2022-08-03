// $Id: v3D_EventArgs.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
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
#include "v3D_Enums.h"

// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class IntegerArg : public System::EventArgs
{
// functions
public:
  
  /// initializes an IntegerArg object with the specified value.
  IntegerArg( int iValue ) { m_iValue = iValue; }
  
  /// gets the value.
  property int Value 
  { 
    int get() { return m_iValue; }
  }

// data
private:
  
  /// value
  int m_iValue;
}; // IntegerArg


CLOSE_VISUALIZATION_NS


// $Log: v3D_EventArgs.h,v $
// Revision 1.2  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.8  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.7  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.6  2003/05/06 17:23:31  geconomos
// Coding standards
//
// Revision 1.5  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.4  2002/09/27 19:25:46  dmitry
// ViewArg added.
//
// Revision 1.3  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.2  2002/08/21 16:02:35  dmitry
// Image property renamed
//
// Revision 1.1  2002/08/21 14:56:35  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_EventArgs.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_EventArgs.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
