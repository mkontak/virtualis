// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BitConverter.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#include "stdafx.h"
#include "BitConverter.h"
#include "Exception.h"

using namespace text;


/**
 * COnverts the buffer to a bit string
 *
 * @param sValue      String
 * @param compressed  Boolean to determine if the '-' are removed
 *
 * @return String
 */
std::string BitConverter::ToString(const std::string & sValue, bool compressed )
{
  return ToString(sValue.c_str(), sValue.size(), compressed);
} // ToString(const std::string & sValue, bool compressed )


/**
 * COnverts the buffer to a bit string
 *
 * @param value       pointer to buffer
 * @param iSize       Size of buffer
 * @param compressed  Boolean to determine if the '-' are removed
 *
 * @return String
 */
std::string BitConverter::ToString(const void * value, unsigned int iSize, bool compressed )
{
  std::ostringstream os;
  std::string converted;

  std::string sepreator( compressed ? "" : "-" );

  unsigned char * pPtr((unsigned char *)value);

  char buffer [2];
  for ( int i(0); i < iSize; i++ )
  {
    sprintf(buffer,"%02X", (unsigned  char)pPtr[i]);

    os << buffer << sepreator;
  }

  converted = os.str();

  return trim_right(converted,"-");
} // ToString(const void * value, unsigned int iSize, bool compressed )


/**
 * COnverts the buffer to a bit string
 *
 * @param iValue      Value to convert
  *
 * @return String
 */
std::string BitConverter::ToString(const int iValue)
{
  const int iSize(sizeof(int) + 1);
  char buffer[iSize];
  RtlZeroMemory(buffer,iSize);
  char * pPtr((char *)&iValue);

  for ( int i = sizeof(int) - 1, j = 0;  j <= sizeof(int); buffer[j++] = pPtr[i--] );
   

  std::string sValue( trim_left( ToString(buffer, sizeof(int), true),"0") );

  if ( sValue.size() % 2 == 1 )
    sValue.insert(0,"0");

  return sValue;

} // ToString(const int iValue)


/**
 * COnverts the buffer to a bit string
 *
 * @param iValue      Value to convert
  *
 * @return String
 */
int BitConverter::ToInt(const std::string & sValue)
{
  return ToInt(sValue.c_str(), sValue.size());
} // ToString(const int iValue)


/**
 * COnverts the buffer to a bit string
 *
 * @param iValue      Value to convert
  *
 * @return String
 */
int BitConverter::ToInt(const void * value, unsigned int iSize)
{

  assert(iSize  <= sizeof(int) );

  int iValue(0);

  unsigned char * pPtr((unsigned char *)value);
  char buffer[3];
  RtlZeroMemory(buffer,3);

  int iTwo(2);

  for ( int i(0); i < iSize; i++ )
  {
    const int iPower(iSize - i);
    const int iShift( iPower == 1 ? 0 : (int)pow((float)2,(float)(iPower)));
    buffer[0] = pPtr[i];

    int iChar((int)pPtr[i]);
    iChar = (iChar < 65 ?  iChar - 48 : iChar - 55 );
  
    iValue |= ( iChar << iShift );
  }


  return iValue;
  
} // ToString(const int iValue)

/**
 * COnverts the source to the destintaion
 *
 * @param pSource     Source buffer
 * @param iSourceSIze Source size
 * @param pDestBuffer Destintion buffer
 * @param iDestSzie   Destintion buffer Szie
 *
 */
void BitConverter::ToBuffer(const void * pSource, unsigned int iSourceSize, void * pDest, unsigned int iDestSize )
{

  // Make sure there is a dest and it has a size
  if ( pDest == __nullptr || iDestSize == 0  )
    throw ex::IllegalArgumentException(LogRec("Null destintaion", "BitConverter","ToBuffer"));

  /// Destintaion must be at least half the size of the source
  assert( iDestSize >= iSourceSize / 2 );

  unsigned char * pS((unsigned char *)pSource);
  unsigned char * pD((unsigned char *)pDest);

  for ( int i = 0, j = 0; i < iSourceSize; i++ )
  {
    // Convert the first amd second characters 
    int iChar1(((int)pS[i] < 65 ?  (int)pS[i] - 48 : (int)pS[i] - 55 ));
    int iChar2(((int)pS[++i] < 65 ?  (int)pS[i] - 48 : (int)pS[i] - 55 ));

    // Or the character together 
    pD[j++] = (iChar1 << 4) | iChar2;

    // Make sure we have not exceeded the destination
    if ( j > iDestSize )
      throw ex::IndexOutOfBoundsException(LogRec("Destintaion buffer not big enough", "BitConverter","ToBuffer"));

  } // ToBuffer(const void * pSource, unsigned int iSourceSize, void * pDest, unsigned int iDestSize )

  
  
} // ToBuffer(const int iValue)