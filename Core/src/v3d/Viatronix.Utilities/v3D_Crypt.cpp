// $Id: v3D_Crypt.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: george economos ( george@viatronix.net )

// Includes
#include "stdafx.h"
#include "v3D_Crypt.h"
#include "Crypt.h"

// Namespaces
USING_UTILITIES_NS
using namespace System;


/**
 * Encrypts the input string using the specified key.
 *
 * @param iKey encryption key
 * @param mpInput string to encrypt
 *
 * @return the encryped string
 */
System::String * Viatronix::Utilities::Crypt::EncryptReadable( long iKey, System::String * mpInput )
{
  ::Crypt crypt( iKey );  
  
  std::string sEncrypted = GetUnManagedStdString( mpInput );  
  crypt.EncryptReadable( sEncrypted );
  
  return sEncrypted.c_str();
} // EncryptReadable()


/**
 * Decrypts the input string using the specified key.
 *
 * @param iKey decryption key
 * @param mpInput string to decrypt
 *
 * @return the decrypted string
 */
System::String * Viatronix::Utilities::Crypt::DecryptReadable( long iKey, System::String * mpInput )
{
  ::Crypt crypt( iKey );  
  
  std::string sDecrypted = GetUnManagedStdString( mpInput );  
  
  crypt.DecryptReadable( sDecrypted );

  return sDecrypted.c_str();
} // DecryptReadable()


/**
 * Unit test.
 */
void Viatronix::Utilities::Crypt::UnitTest()
{  
  System::String * mpDataString = S"This is a test! -> | <-  13615643616  !@#$%^&()*";

  Console::WriteLine( "Crypt Unit Test" );
  Console::WriteLine( "===============" );
  Console::WriteLine();  

  Console::WriteLine( "Original String: {0}", mpDataString );  
  
  const long iKey = 6;
  System::String * mpEncrypted = Viatronix::Utilities::Crypt::EncryptReadable( iKey, mpDataString );
  Console::WriteLine( "Encrypted String: {0}", mpEncrypted );  

  System::String * mpDecrypted = Viatronix::Utilities::Crypt::DecryptReadable( iKey, mpEncrypted );
  Console::WriteLine( "Decrypted String: {0}", mpDecrypted );  

  Console::WriteLine();  
  if( !System::String::Compare( mpDataString, mpDecrypted ) )
    Console::WriteLine( "Unit test passed." );  
  else
    Console::WriteLine( "Unit test failed." );  
  
} // UnitTest


// $Log: v3D_Crypt.cpp,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/04 14:46:20  mkontak
// Merge with XP_1_1_09042003
//
// Revision 1.4.2.1  2003/08/25 20:12:31  mkontak
// Change to ThreadBase
//
// Revision 1.4  2003/05/01 19:21:46  frank
// code review
//
// Revision 1.3  2003/05/01 17:19:05  geconomos
// Coding standards.
//
// Revision 1.2  2003/04/15 14:10:39  mkontak
// Re-Write of CDR
//
// Revision 1.1  2003/03/13 14:50:46  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/v3D_Crypt.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: v3D_Crypt.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
