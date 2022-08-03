// $Id: Crypt.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Crypt.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ 
// 
// Copyright© 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Dima (dmitry@viatronix.com)

#include "stdafx.h"
#include "Crypt.h"

using namespace std; 

/**
 * Demo run with a test string value.
 */
bool Crypt::Test()
{
  bool bSuccess = true;

  // test for encrypt followed by decrypt
  Crypt crypt;

  string sDataString = "This is a test! -> | <-  13615643616  !@#$%^&()*";
  string sDataStringOrig = sDataString;
  std::cout << "Example of encryption with Crypt" << endl;
  std::cout << "\nData to Encrypt :  " << sDataString << endl;
  // test for encryption / decryption with different keyIndex  
  for (uint4 index(0); index < 6; ++index)
  {
    Crypt crypt(index);
    std::cout << "\nKey Index Value :  " << index << endl;
    crypt.Encrypt16bit(sDataString);
    std::cout << "Above Encrypted :  " << sDataString << endl;
    crypt.Decrypt16bit(sDataString);
    std::cout << "Decrypted Data  :  " << sDataString << endl;
    bSuccess &= (sDataString == sDataStringOrig);

  }

  // Test for encryption and Decryption with pointer parameters
  crypt.Encrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sDataString.c_str())), 48);
  std::cout << "\nEncrypted w/ pointer reference :  " << sDataString << endl;
  crypt.Decrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sDataString.c_str())), 48);
  std::cout << "Decrypted w/ pointer reference :  " << sDataString << endl;
  bSuccess &= (sDataString == sDataStringOrig);


  // test for data length < 8 bytes
  string sSmallStr = "12345";
  string sOrigSmallStr = static_cast<const string>(sSmallStr);
  std::cout << "\nOriginal  Small String :  " << sSmallStr << endl;
  crypt.Encrypt16bit(sSmallStr);
  std::cout << "Encrypted Small String :  " << sSmallStr << endl;
  bSuccess &= (sSmallStr == sOrigSmallStr);


  // test for data length not multiple of 8 bytes
  string sTestStr = "test me here 123456789";
  string sOrigStr = static_cast<const string>(sTestStr);

  std::cout << "\nOriginal  String (Length MOD 8 != 0) :  " << sTestStr << endl;
  crypt.Encrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sTestStr.c_str())), static_cast< uint4 >( sTestStr.length()) );
  std::cout <<   "Encrypted String (Length MOD 8 != 0) :  " << sTestStr << endl;
  crypt.Decrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sTestStr.c_str())), static_cast< uint4 >( sTestStr.length()) );
  std::cout <<   "Decrypted String (Length MOD 8 != 0) :  " << sTestStr << endl;
  bSuccess &= (sTestStr == sOrigStr);

  // test encryption /  decrytpion with readable results
  std::cout << "\nOriginal  String            : |" << sTestStr << "|" << endl;
  crypt.EncryptReadable(sTestStr);
  std::cout <<   "Encrypted String (Readable) : |" << sTestStr << "|" << endl;
  crypt.DecryptReadable(sTestStr);
  std::cout <<   "Decrypted String            : |" << sTestStr << "|" << endl;

  bSuccess &= (sTestStr == sOrigStr);
  return bSuccess;
} // Test

// Revision History:
// $Log: Crypt.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:42:40  michael
// code review
//
// Revision 3.1  2003/05/15 12:37:14  mkontak
// Added include for iostream
//
// Revision 3.0  2001/10/14 23:02:14  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:52   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:28   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.11  2001/08/31 18:53:38  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.10  2001/04/20 17:54:12  dmitry
// Altered to reflect coding standards.
//
// Revision 1.9  2001/04/10 17:31:32  dmitry
// Code Standardization, Test for Data Length < 8 bytes Added
//
// Revision 1.8  2001/04/09 18:02:57  dmitry
// Removed Revision History from Top of File
//
// Revision 1.7  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.6  2001/03/19 17:18:05  dongqing
// Revised datatypes to those defined in Typedef.h
//
// Revision 1.5  2001/02/22 15:54:43  dmitry
// Deep String Copy example added.
//
// Revision 1.4  2001/02/22 15:44:01  dmitry
// Encryption to Readable Character / Decryption Added
// warning for char * param functions added
//
// Revision 1.3  2001/01/22 20:37:16  dmitry
// Added Encrypt / Decrypt methods w/ pointer to data and byte size of data parameters.
// Modified the original Encrypt / Decrypt methods containing  string parameter to utilize new methods.
//
// Revision 1.2  2001/01/19 20:27:00  dmitry
// Improved test function and added m_ to member var
//
// Revision 1.1  2001/01/19 15:48:57  dmitry
// Initial version of Crypt class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Crypt.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Crypt.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $

