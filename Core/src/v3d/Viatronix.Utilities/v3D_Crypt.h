// $Id: v3D_Crypt.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// Pragmas
#pragma once

// Namespaces
OPEN_UTILITIES_NS

/// class declaration
public __gc class Crypt
{
// member functions
public:

  // encrypts the input string using the specified key
  static System::String * EncryptReadable( long iKey, System::String * mpInput );

  // encrypts the input string using the specified key
  inline static System::String * EncryptReadable( System::String * mpInput )
  { return EncryptReadable(0, mpInput); }

  // decrypts the input string using the specified key
  static System::String * DecryptReadable( long iKey, System::String * mpInput );

  // decrypts the input string using the specified key
  inline static System::String * DecryptReadable( System::String * mpInput )
  { return DecryptReadable(0, mpInput); }

  // unit test
  static void UnitTest();

}; // class Crypt

CLOSE_UTILITIES_NS

// $Log: v3D_Crypt.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/04 14:46:41  mkontak
// Merge with XP_1_1_09042003
//
// Revision 1.4  2003/08/20 14:24:08  michael
// added "mailto:"
//
// Revision 1.3.2.1  2003/08/19 17:39:28  mkontak
// Added defaults to key
//
// Revision 1.3  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.2  2003/05/01 17:19:06  geconomos
// Coding standards.
//
// Revision 1.1  2003/03/13 14:50:46  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/v3D_Crypt.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: v3D_Crypt.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

