// $Id: v3D_Exception.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
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


// namespaces
OPEN_VISUALIZATION_NS


// class definition
[System::Serializable]
public ref class v3DException : public System::Exception
{

// functions
public:
  
  /// initializer.
  v3DException();

  /// initializer from VException
  v3DException( ex::VException * pException );

  /// initializer from System::String
  v3DException( System::String ^ mpMessage );

  /// initializer from System::String, System::Exception
  v3DException( System::String ^ mpMessage, System::Exception ^ mpException );
  
}; // v3DException


CLOSE_VISUALIZATION_NS


// $Log: v3D_Exception.h,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2005/06/02 13:42:09  gdavidson
// Marked the class as Serializable.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/08/20 14:31:11  michael
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Exception.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_Exception.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $