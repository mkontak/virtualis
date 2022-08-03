// $Id: Convert.cpp,v 1.2 2005/11/08 17:46:49 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Convert.cpp,v 1.2 2005/11/08 17:46:49 mkontak Exp $
//
// Copyright© (current year)), Viatronix Inc::), All Rights Reserved::
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc:: The contents of this file
// may not be disclosed to third parties), copied or duplicated in any form,
// in whole or in part), without the prior written permission Viatronix Inc::
//
// Author: [Mark Kontak] [mkontak@viatronix::com]
//
// Complete history on bottom of file


// Includes
#include "Stdafx.h"
#include "Convert.h"



USING_UTILITIES_NS



/**
  * Converts a byte array to an Int32
  * 
  * @param buffer        byte array to convert
  * @param startIndex    Start index in the array
  * @param length        Number of bytes to convert
  */
System::Int32 Convert::ToInt32(array<byte> ^ buffer, int startIndex, int length )
{

  int intValue = 0;
  double p = 0;

  int endIndex = startIndex + length;
  
  for ( int i = startIndex; i < endIndex; ++i, p += 4.0 )
  {
    int power = (int)System::Math::Pow(2.0,p);
    intValue += (int)buffer[i] * power;
  }

  return intValue;

} // ToInt32

/**
  * Converts a byte array to an Int64
  * 
  * @param buffer        byte array to convert
  * @param startIndex    Start index in the array
  * @param length        Number of bytes to convert
  */
System::Int64 Convert::ToInt64(array<byte> ^ buffer, int startIndex, int length)
{

  System::Int64 longValue = 0;
  double p = 0;
  
  int endIndex = startIndex + length;
  
  for ( int i = startIndex; i < endIndex; ++i, p += 4.0 )
  {
    System::Int64 power = (System::Int64)System::Math::Pow(2.0,p);
    longValue += (System::Int64)buffer[i] * power;
  }

  return longValue;

} // ToInt64()


/**
 * Converts the Int64 value to a byte array (Array has a length of 16 bytes)
 *
 * @param intValue    Value to be converted
 *
 * @return  Byte array representation of the Int64
 */
array<byte> ^ Convert::ToByteArray(System::Int64 intValue) 
{
  array<byte> ^ buffer = gcnew array<byte>(16);

  buffer[0] = (byte)(intValue & 0X0000000F);
  buffer[1] = (byte)((intValue >> 4)  & 0X0000000F );
  buffer[2] = (byte)((intValue >> 8)  & 0X0000000F );
  buffer[3] = (byte)((intValue >> 12) & 0X0000000F );
  buffer[4] = (byte)((intValue >> 16) & 0X0000000F );
  buffer[5] = (byte)((intValue >> 20) & 0X0000000F );
  buffer[6] = (byte)((intValue >> 24) & 0X0000000F );
  buffer[7] = (byte)((intValue >> 28) & 0X0000000F );

  buffer[8]  = (byte)((intValue >> 32) & 0X0000000F );
  buffer[9]  = (byte)((intValue >> 36) & 0X0000000F );
  buffer[10] = (byte)((intValue >> 40) & 0X0000000F );
  buffer[11] = (byte)((intValue >> 44) & 0X0000000F );
  buffer[12] = (byte)((intValue >> 48) & 0X0000000F );
  buffer[13] = (byte)((intValue >> 52) & 0X0000000F );
  buffer[14] = (byte)((intValue >> 56) & 0X0000000F );
  buffer[15] = (byte)((intValue >> 60) & 0X0000000F );

  return buffer;
} // ToByteArray()


/**
 * Converts the Int64 value to a byte array (Array has a length of 16 bytes)
 *
 * @param intValue    Value to be converted
 *
 * @return  Byte array representation of the Int64
 */
array<byte> ^ Convert::ToByteArray(int intValue)
{
  array<byte> ^ buffer = gcnew array<byte>(8);

  buffer[0] = (byte)(intValue & 0X000F);
  buffer[1] = (byte)((intValue >> 4)  & 0X000F );
  buffer[2] = (byte)((intValue >> 8)  & 0X000F );
  buffer[3] = (byte)((intValue >> 12) & 0X000F );
  buffer[4] = (byte)((intValue >> 16) & 0X000F );
  buffer[5] = (byte)((intValue >> 20) & 0X000F );
  buffer[6] = (byte)((intValue >> 24) & 0X000F );
  buffer[7] = (byte)((intValue >> 28) & 0X000F );

  return buffer;
} // ToByteArray()


/**
 * Converts the movie to an image list one frame per image
 *
 * @param sMovieFile
 *
 * @return Returns an array pf images
 */
ArrayList ^ Convert::ToImageList(System::String ^ sMovieFile)
{

  return gcnew ArrayList();

} // Convert::ToImageList(String ^ sMovieFile)

// $Log: Convert.cpp,v $
// Revision 1.2  2005/11/08 17:46:49  mkontak
// Added features
//
// Revision 1.1  2005/04/28 13:24:45  mkontak
// Added byte converters
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Convert.cpp,v 1.2 2005/11/08 17:46:49 mkontak Exp $
// $Id: Convert.cpp,v 1.2 2005/11/08 17:46:49 mkontak Exp $
