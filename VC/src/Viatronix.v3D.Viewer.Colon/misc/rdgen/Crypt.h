// $Id: Crypt.h,v 1.1 2006/08/01 18:17:37 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/rdgen/Crypt.h,v 1.1 2006/08/01 18:17:37 geconomos Exp $ 
// 
// Copyright© 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Dima (dmitry@viatronix.com)


#ifndef Crypt_h
#define Crypt_h

/**
 * Class implementation of the GHOST encryption / decryption algorithm.
 * "GHOST is a 64-bit block algorithm with a 256-bit key. If you add in
 * the secret S-Box permutation, GHOST has a total of about 610 bits of
 * secret data." [Schneier 333]
 * 
 * Crypt implementation processes 64-bit (4 chars) of data at a time. If
 * the block of data is not a multiple of 64-bits, blank spaces (ie. " ")
 * will be concatinated to the end of the stream. The data to be encrypted / 
 * decrypted is passed by reference and will be replaced by its complement.
 * 
 * For further information on GHOST encryption see:
 * Schneier, Bruce. "Applied Cryptography". John Wiley & Sons, Inc. 1996. [331, 643]
 *
 * WARNING: When using encryption and decryption with (char *) param all string
 *          variables set to the passed string will be altered due to initial 
 *          std::string data referencing.
 * To Create Deep Copy use (const string) cast.  Ex: std::string strcp = (const string)str;
 * $Id: Crypt.h,v 1.1 2006/08/01 18:17:37 geconomos Exp $ <a href="../../vxBase/1_utils/Spec/CryptDesignSpec.htm">Design Specification</a>
 */
class Crypt 
{
public:
  /// Constructor function, calls method to initialize S-Box var,
  /// depending on keyIndex value deffines key
  inline Crypt(uint4 uKeyIndex=0);

  /// Destructor function, dets key values to NULL
  inline ~Crypt();

  /// Public encryption function, concatinates string with blank space to ensure
  /// blocks of eight chars, sends blocks of eight chars to be encrypted.
  inline void Encrypt16bit(std::string & sIoString) const;

  /// Sequentially reads data passed via pointer encrypting blocks of eight bytes.
  inline void Encrypt16bit(uint1 * puDataPtr, uint4 uDataByteLength) const;

  /// Public decryption function, concatinates string with blank space to ensure
  /// blocks of eight chars, sends blocks of eight chars to be decrypted.
  inline void Decrypt16bit(std::string & sIoString) const;

  /// Sequentially reads data passed via pointer decrypting blocks of eight bytes.
  inline void Decrypt16bit(uint1 * puDataPtr, uint4 uDataByteLength) const;

  /// Encrypts strings to readable characters
  inline void EncryptReadable(std::string & sIoString) const;

  /// Decrypt readable encryption, result uchar string
  inline void DecryptReadable(std::string & sIoString) const;

  /// Demo run with a test string value.
  static bool Test();

  /// Returns the encrypted string.
  inline std::string GetEncrypt16bit(const std::string & sDataString) const;

  /// Returns the decrypted string.
  inline std::string GetDecrypt16bit(const std::string & sDataString) const;

  /// Returns the encrypted readable string.
  inline std::string GetEncryptReadable(const std::string & sIoString) const;

  /// Returns the decrypted string.
  inline std::string GetDecryptReadable(const std::string & sIoString) const;

private:
  /// Initiates S-Box entries and creates four blocks of a 256 bit key.
  inline void SBoxInit();

  /// Uses bit shifting to encrypt one register value.
  inline uint4 KeyCrypt(uint4 uValue) const;

  /// Splits data value into left and right component, implements substituiion and calls 
  /// KeyCrypt to encrypt data.
  inline void GostCrypt(uint4 * puDataInOut) const;

  /// Splits data value into left and right component, implements substituiion and calls 
  /// KeyCrypt to decrypt data.
  inline void GostDecrypt(uint4 * puDataInOut) const;

private:
  /// Primary key defined by user through index value in the constructor
  uint4 m_uKey[8];

  /// Blocks of a 256 bit key generated with respect to the S-Box values
  int1 m_iKey87[256], m_iKey65[256], m_iKey43[256], m_iKey21[256];
};

#include "Crypt.inl" 
#endif

// Revision History:
// $Log: Crypt.h,v $
// Revision 1.1  2006/08/01 18:17:37  geconomos
// initial revision
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:42:40  michael
// code review
//
// Revision 3.1  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
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
// Revision 1.13  2001/08/31 18:53:38  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.12  2001/04/20 17:54:12  dmitry
// Altered to reflect coding standards.
//
// Revision 1.11  2001/04/10 17:29:50  dmitry
// Code Standardization
//
// Revision 1.10  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.9  2001/03/19 17:18:05  dongqing
// Revised datatypes to those defined in Typedef.h
//
// Revision 1.8  2001/03/02 21:29:03  jmeade
// convenience functions for readable encryption
//
// Revision 1.7  2001/02/22 15:54:13  dmitry
// String Copy suggestion added.
//
// Revision 1.6  2001/02/22 15:14:36  dmitry
// Encryption to Readable Character / Decryption Added
// warning for char * param functions added
//
// Revision 1.5  2001/02/21 01:00:46  jeff
// no message
//
// Revision 1.4  2001/02/21 00:52:06  jeff
// global convenience functions
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/rdgen/Crypt.h,v 1.1 2006/08/01 18:17:37 geconomos Exp $
// $Id: Crypt.h,v 1.1 2006/08/01 18:17:37 geconomos Exp $

