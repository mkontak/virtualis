// $Id: Crypt.inl,v 1.1 2006/08/01 18:17:37 geconomos Exp $ 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/rdgen/Crypt.inl,v 1.1 2006/08/01 18:17:37 geconomos Exp $ 
// 
// Copyright© 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Dima (dmitry@viatronix.com)


/**
 * Constructor function, calls method to initialize S-Box var,
 * depending on keyIndex value deffines m_uKey
 *
 * @param uKeyIndex		Reference value for m_uKey deffinition.
 */
Crypt::Crypt(uint4 uKeyIndex)
{
  SBoxInit();
  for ( uint4 k(0); k<=uKeyIndex; ++k )
  {
    for ( uint4 j(0); j<8; ++j )
    {
      m_uKey[j] = (k+1)*(k+(j+1))-1;
    }   
  }
} // constructor


/**
 * Destructor function, sets m_uKey values to zero
 */
Crypt::~Crypt()
{
} // destructor


/**
 * Initiates S-Box entries and creates four blocks of a 256 bit m_uKey.
 */
void Crypt::SBoxInit()
{
  uint1 s8[16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};
  uint1 s7[16] = {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10};
  uint1 s6[16] = {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8};
  uint1 s5[16] = {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15};
  uint1 s4[16] = {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9};
  uint1 s3[16] = {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11};
  uint1 s2[16] = {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1};
  uint1 s1[16] = {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7};

  for ( int4 i(0); i < 256; ++i )
  {
    m_iKey87[i] = s8[i >> 4] << 4 | s7[i & 15];
    m_iKey65[i] = s6[i >> 4] << 4 | s5[i & 15];
    m_iKey43[i] = s4[i >> 4] << 4 | s3[i & 15];
    m_iKey21[i] = s2[i >> 4] << 4 | s1[i & 15];
  }
} // SBoxInit


/**
 * Public encryption function, concatinates string with blank space to ensure
 * blocks of eight chars, sends blocks of eight chars to be encrypted.
 * 
 * @param sIoString  String to be encrypted.
 */
void Crypt::Encrypt16bit(std::string & sIoString) const
{
  // disable std reference counting (force seperate copy)
  sIoString += "a";  
  sIoString.erase(sIoString.length()-1);

  // now change the string data ( c_str() circumvents the std reference count )
  Encrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sIoString.c_str())), static_cast< uint4 >( sIoString.length() ));
} // Encrypt16bit


/**
 * Sequentially reads data passed via pointer encrypting blocks of eight bytes.
 *
 * WARNING: When using encryption with (char *) param all string variables
 *          set to the passed string will be altered due to initial 
 *          std::string data referencing.
 * To Create Deep Copy use (const string) cast.  Ex: std::string strcp = (const string)str;
 *
 * @param puDataPtr         Pointer to the data to be encrypted.
 * @param uDataByteLength   Byte length of the data to be encrypted.
 */
void Crypt::Encrypt16bit(uint1 * puDataPtr, uint4 uDataByteLength) const
{
  if (uDataByteLength < 8) return;
  for (uint4 k(0); k < (uDataByteLength/8); ++k)
  {
    GostCrypt(reinterpret_cast<uint4 *>(puDataPtr + (8*k)));
  }

  if ((uDataByteLength % 8) != 0)
  {
    GostCrypt(reinterpret_cast<uint4 *>(puDataPtr + (uDataByteLength-8)));
  }
} // Encrypt16bit


/**
 * Passes a string to be encrypted with no alteration to the original string.
 * 
 * @param   sDataString  String to be encrypted.
 * @return               Encrypted string.
 */
std::string Crypt::GetEncrypt16bit(const std::string & sDataString) const
{
  std::string sReturnStr(sDataString);
  Crypt crypt;
  crypt.Encrypt16bit(sReturnStr);
  return sReturnStr;
} // Decrypt16bit


/**
 * Public decryption function, concatinates string with blank space to ensure
 * blocks of eight chars, sends blocks of eight chars to be decrypted.
 * 
 * @param sIoString   String to be decrypted.
 */
void Crypt::Decrypt16bit(std::string & sIoString) const
{
  // disable std reference counting (force seperate copy)
  sIoString += "a";  
  sIoString.erase(sIoString.length()-1);

  // now change the string data ( c_str() circumvents the std reference count )
  Decrypt16bit(reinterpret_cast<uint1 *>(const_cast<char *>(sIoString.c_str())), static_cast< uint4 >( sIoString.length() ));
} // Decrypt16bit


/**
 * Passes a string to be decrypted with no alteration to the original string.
 * 
 * @param   sDataString  String to be decrypted.
 * @return               Encrypted string.
 */
std::string Crypt::GetDecrypt16bit(const std::string & sDataString) const
{
  std::string sReturnStr(sDataString);
  Crypt crypt;
  crypt.Decrypt16bit(sReturnStr);
  return sReturnStr;
} // Decrypt16bit


/**
 * Sequentially reads data passed via pointer decrypting blocks of eight bytes.
 *
 * WARNING: When using decryption with (char *) param all string variables 
 *          set to the passed string will be altered due to initial 
 *          std::string data referencing.
 * To Create Deep Copy use (const string) cast.  Ex: std::string strcp = (const string)str;
 *
 * @param puDataPtr        Pointer to the data to be decrypted.
 * @param uDataByteLength  Byte length of the data to be decrypted.
 */
void Crypt::Decrypt16bit(uint1 * puDataPtr, uint4 uDataByteLength) const
{
  if (uDataByteLength < 8) return;
  if ((uDataByteLength % 8) != 0)
  {
    GostDecrypt(reinterpret_cast<uint4 *>(puDataPtr + (uDataByteLength-8)));
  }
  for (uint4 k(0); k < (uDataByteLength/8); ++k)
  {
    GostDecrypt(reinterpret_cast<uint4 *>(puDataPtr + (8*k)));
  } 
} // Decrypt16bit




/**
 * Uses bit shifting to encrypt one register value.
 * 
 * @param uValue      Single integer value to be encrypted.
 * @return            Encrypted value.
 */
uint4 Crypt::KeyCrypt(uint4 uValue) const
{
  uValue = m_iKey87[uValue>>24 & 255] << 24 | m_iKey65[uValue>>16 & 255] << 16 | 
           m_iKey43[uValue>> 8 & 255] <<  8 | m_iKey21[uValue & 255];

  return uValue<<11 | uValue>>(32-11);
} // KeyCrypt


/**
 * Splits data value into left and right component, implements substituiion and calls 
 * KeyCrypt to encrypt data.
 * 
 * @param puDataInOut Integer value to be encrypted.
 */
void Crypt::GostCrypt(uint4 * puDataInOut) const
{
  register uint4 n1(puDataInOut[0]), n2(puDataInOut[1]);

  n2 ^= KeyCrypt(n1+m_uKey[0]); n1 ^= KeyCrypt(n2+m_uKey[1]);
  n2 ^= KeyCrypt(n1+m_uKey[2]); n1 ^= KeyCrypt(n2+m_uKey[3]);
  n2 ^= KeyCrypt(n1+m_uKey[4]); n1 ^= KeyCrypt(n2+m_uKey[5]);
  n2 ^= KeyCrypt(n1+m_uKey[6]); n1 ^= KeyCrypt(n2+m_uKey[7]);

  n2 ^= KeyCrypt(n1+m_uKey[0]); n1 ^= KeyCrypt(n2+m_uKey[1]);
  n2 ^= KeyCrypt(n1+m_uKey[2]); n1 ^= KeyCrypt(n2+m_uKey[3]);
  n2 ^= KeyCrypt(n1+m_uKey[4]); n1 ^= KeyCrypt(n2+m_uKey[5]);
  n2 ^= KeyCrypt(n1+m_uKey[6]); n1 ^= KeyCrypt(n2+m_uKey[7]);

  n2 ^= KeyCrypt(n1+m_uKey[0]); n1 ^= KeyCrypt(n2+m_uKey[1]);
  n2 ^= KeyCrypt(n1+m_uKey[2]); n1 ^= KeyCrypt(n2+m_uKey[3]);
  n2 ^= KeyCrypt(n1+m_uKey[4]); n1 ^= KeyCrypt(n2+m_uKey[5]);
  n2 ^= KeyCrypt(n1+m_uKey[6]); n1 ^= KeyCrypt(n2+m_uKey[7]);

  n2 ^= KeyCrypt(n1+m_uKey[7]); n1 ^= KeyCrypt(n2+m_uKey[6]);
  n2 ^= KeyCrypt(n1+m_uKey[5]); n1 ^= KeyCrypt(n2+m_uKey[4]);
  n2 ^= KeyCrypt(n1+m_uKey[3]); n1 ^= KeyCrypt(n2+m_uKey[2]);
  n2 ^= KeyCrypt(n1+m_uKey[1]); n1 ^= KeyCrypt(n2+m_uKey[0]);

  puDataInOut[0] = n2; puDataInOut[1] = n1;
} // GostCrypt


/**
 * Splits data value into left and right component, implements substituiion and calls 
 * KeyCrypt to decrypt data.
 * 
 * @param puDataInOut Integer value to be decrypted.
 */
void Crypt::GostDecrypt(uint4 * puDataInOut) const
{
  register uint4 n1(puDataInOut[0]), n2(puDataInOut[1]);

  n2 ^= KeyCrypt(n1+m_uKey[0]); n1 ^= KeyCrypt(n2+m_uKey[1]);
  n2 ^= KeyCrypt(n1+m_uKey[2]); n1 ^= KeyCrypt(n2+m_uKey[3]);
  n2 ^= KeyCrypt(n1+m_uKey[4]); n1 ^= KeyCrypt(n2+m_uKey[5]);
  n2 ^= KeyCrypt(n1+m_uKey[6]); n1 ^= KeyCrypt(n2+m_uKey[7]);

  n2 ^= KeyCrypt(n1+m_uKey[7]); n1 ^= KeyCrypt(n2+m_uKey[6]);
  n2 ^= KeyCrypt(n1+m_uKey[5]); n1 ^= KeyCrypt(n2+m_uKey[4]);
  n2 ^= KeyCrypt(n1+m_uKey[3]); n1 ^= KeyCrypt(n2+m_uKey[2]);
  n2 ^= KeyCrypt(n1+m_uKey[1]); n1 ^= KeyCrypt(n2+m_uKey[0]);

  n2 ^= KeyCrypt(n1+m_uKey[7]); n1 ^= KeyCrypt(n2+m_uKey[6]);
  n2 ^= KeyCrypt(n1+m_uKey[5]); n1 ^= KeyCrypt(n2+m_uKey[4]);
  n2 ^= KeyCrypt(n1+m_uKey[3]); n1 ^= KeyCrypt(n2+m_uKey[2]);
  n2 ^= KeyCrypt(n1+m_uKey[1]); n1 ^= KeyCrypt(n2+m_uKey[0]);

  n2 ^= KeyCrypt(n1+m_uKey[7]); n1 ^= KeyCrypt(n2+m_uKey[6]);
  n2 ^= KeyCrypt(n1+m_uKey[5]); n1 ^= KeyCrypt(n2+m_uKey[4]);
  n2 ^= KeyCrypt(n1+m_uKey[3]); n1 ^= KeyCrypt(n2+m_uKey[2]);
  n2 ^= KeyCrypt(n1+m_uKey[1]); n1 ^= KeyCrypt(n2+m_uKey[0]);

  puDataInOut[0] = n2; puDataInOut[1] = n1; 
} // GostDecrypt

/**
 * Encrypts a string to a readable character string.
 * 
 * @param sIoString String to encrypt.
 */
void Crypt::EncryptReadable(std::string & sIoString) const
{
  Encrypt16bit(sIoString);
  std::string sOutString;   
  for (uint4 t(0); t < sIoString.length(); ++t)
  {
    sOutString += 'a' + (static_cast<uint1>(sIoString[t]) >> 4);
    sOutString += 'a' + (static_cast<uint1>(sIoString[t]) & 15);
  }
  sIoString = sOutString;
} // EncryptReadable


/**
 * Encrypts a string to a readable character string with no alteration to the original string.
 * 
 * @param sDataString String to encrypt.
 * @return          Encrypted string.
 */
std::string Crypt::GetEncryptReadable(const std::string & sDataString) const
{
  std::string sReturnStr(sDataString);
  Crypt crypt;
  crypt.EncryptReadable(sReturnStr);
  return sReturnStr;
} // GetEncryptReadable


/**
 * Decrypts readable encryption strings to original string in uchar.
 * 
 * @param sIoString String to decrypt.
 */
void Crypt::DecryptReadable(std::string & sIoString) const
{
  std::string sOutString;
  for (uint4 t(0); t < sIoString.length(); t += 2)
  {
    sOutString += (static_cast<uint1>(sIoString[t] - 'a') << 4) | static_cast<uint1>(sIoString[t+1] - 'a');
  }
  Decrypt16bit(sOutString);
  sIoString = sOutString;
} // DecryptReadable


/**
 * Decrypts readable encryption strings to original string in uchar with no alteration to the original string.
 * 
 * @param sDataString String to decrypt.
 * @return            Decrypted string.
 */
std::string Crypt::GetDecryptReadable(const std::string & sDataString) const
{
  std::string sReturnStr(sDataString);
  Crypt crypt;
  crypt.DecryptReadable(sReturnStr);
  return sReturnStr;
} // GetDecryptReadable


// Revision History:
// $Log: Crypt.inl,v $
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
// Revision 3.1  2002/03/07 22:14:24  dmitry
// Removed code from the destructor.
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
// Revision 1.11  2001/04/20 17:54:12  dmitry
// Altered to reflect coding standards.
//
// Revision 1.10  2001/04/10 17:29:50  dmitry
// Code Standardization
//
// Revision 1.9  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.8  2001/03/19 17:18:05  dongqing
// Revised datatypes to those defined in Typedef.h
//
// Revision 1.7  2001/03/02 21:29:03  jmeade
// convenience functions for readable encryption
//
// Revision 1.6  2001/02/22 15:54:13  dmitry
// String Copy suggestion added.
//
// Revision 1.5  2001/02/22 15:14:36  dmitry
// Encryption to Readable Character / Decryption Added
// warning for char * param functions added
//
// Revision 1.4  2001/02/21 00:52:07  jeff
// global convenience functions
//
// Revision 1.3  2001/01/22 20:37:15  dmitry
// Added Encrypt / Decrypt methods w/ pointer to data and byte size of data parameters.
// Modified the original Encrypt / Decrypt methods containing  string parameter to utilize new methods.
//
// Revision 1.2  2001/01/19 20:27:00  dmitry
// Improved test function and added m_ to member var
//
// Revision 1.1 2001/01/19 15:48:57 dmitry
// Initial version of Crypt class
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/rdgen/Crypt.inl,v 1.1 2006/08/01 18:17:37 geconomos Exp $
// $Id: Crypt.inl,v 1.1 2006/08/01 18:17:37 geconomos Exp $
