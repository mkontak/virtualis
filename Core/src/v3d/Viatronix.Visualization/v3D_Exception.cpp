// $Id: v3D_Exception.cpp,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
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
#include "v3D_Exception.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a v3DException object with default values.
 */
v3DException::v3DException()
{
}


/**
 * Initializes a v3DException object with the specified VException.
 * @param pException VException containing information.
 */
v3DException::v3DException( ex::VException * pException ) 
: System::Exception( gcnew System::String( pException->GetLogRecord().GetTextMessage().c_str() ))
{
} // v3DException( pException ) :


/**
 * Initializes a v3DException object with the specified message string.
 * @param mpMessage Message string.
 */
v3DException::v3DException( System::String ^ mpMessage ) 
  : System::Exception( mpMessage )
{
} // v3DException( mpMessage ) 


/**
 * Initializes a v3DException object with the specified message and exception.
 * @param mpMessage Message string.
 * @param mpException Inner exception.
 */
v3DException::v3DException( System::String ^ mpMessage, System::Exception ^ mpException ) 
  : System::Exception( mpMessage, mpException )
{
} // v3DException( SmpMessage, mpException ) 


// Revision History:
// $Log: v3D_Exception.cpp,v $
// Revision 1.2  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/24 20:11:08  michael
// took out code that was commented out
//
// Revision 1.5  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.4  2003/05/16 13:10:14  frank
// code formatting
//
// Revision 1.3  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.2  2003/05/06 17:31:33  geconomos
// Coding standards
//
// Revision 1.1  2002/09/24 14:32:39  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Exception.cpp,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Exception.cpp,v 1.2 2006/10/04 18:29:04 gdavidson Exp $