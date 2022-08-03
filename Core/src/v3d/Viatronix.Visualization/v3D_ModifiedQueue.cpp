// $Id: v3D_ModifiedQueue.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_ModifiedQueue.h"
#include "vxModifiedQueue.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Flushes the queue and notifies all observers
 */
void ModifiedQueue::Flush()
{
  try
  {
    vxModifiedQueue::Flush();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Flush()


// Revision History:
// $Log: v3D_ModifiedQueue.cpp,v $
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/09/25 12:54:13  michael
// took out unused Clear() function
//
// Revision 1.7  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.6  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.5  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.4  2003/05/06 18:58:02  geconomos
// Coding standards
//
// Revision 1.3  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.2  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// Revision 1.1  2002/07/02 20:30:13  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ModifiedQueue.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_ModifiedQueue.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
