// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BitConverter.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

#pragma once



namespace text
{

class VX_BASE_DLL BitConverter
{

public:

  // Converts the string to a bit representation ( ex: 0x81 -> "81" )
  static std::string ToString(const std::string & sValue, bool compressed = true);

  // Converts the buffer to a bit representation
  static std::string ToString(const void * value, unsigned int iSize, bool compressed = true);
 
  /// Converts the int to a bit representation
  static std::string ToString(const int iValue);

  /// Converts the string to an it
  static int ToInt(const std::string & sValue);

  /// Converts the int to a bit representation
  static int ToInt(const void * value, unsigned int iSize);

  /// Converts the source bit representation to the destination buffer
  static void ToBuffer(const void * pSource, unsigned int iSourceSize, void * pDest, unsigned int iDestSize );

private:


}; // class BitCOnverter

};