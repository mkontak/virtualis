// $Id: Base64.C,v 1.2 2008/04/24 21:48:29 jmeade Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "Base64.h"
#include "vxBuffer.h"

// possible base64 characters
static const std::string sBase64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/**
 * Encode the string
 *
 * @param s   String to be encoded
 */
std::string Base64::Encode( const std::string & s ) 
{ 
  return Encode( reinterpret_cast< const byte * >( s.c_str() ), static_cast< uint4 >( s.length() ))->ToString(); 
}

/**
 * Encodes the plain text buffer
 *
 * @param buffer 
 */
std::shared_ptr<Buffer> Base64::Encode( const uint1 * pData, uint4 uDataLength )
{



  std::shared_ptr<Buffer> encoded = Buffer::Create(uDataLength + 3);

  // tracks where we are in the original triad
  register int4 i = 0;
  
  /// original data to encode
  uint1 v3[ 3 ];
  
  /// result of encoding original data
  uint1 v4[ 4 ];

  // process all characters
  while( uDataLength-- ) 
  {
    v3[ i++ ] = *( pData++ );
    
    // every third octet ( byte )
    if( i == 3 ) 
    {
      // encode
      v4[ 0 ] = (  v3[ 0 ] & 0xfc ) >> 2;
      v4[ 1 ] = (( v3[ 0 ] & 0x03 ) << 4 ) + ( ( v3[ 1 ] & 0xf0 ) >> 4 );
      v4[ 2 ] = (( v3[ 1 ] & 0x0f ) << 2 ) + ( ( v3[ 2 ] & 0xc0 ) >> 6 );
      v4[ 3 ] =    v3[ 2 ] & 0x3f;

      // update return buffer
      for( i = 0; ( i < 4 ); i++ )
        encoded->Append(sBase64Characters[ v4[ i ] ]);

      i = 0;
    }
  } // while( uDataLength-- ) 

  
  // handle any leftover
  if( i )
  {
    // add null to leftover
    for( int4 j = i; j < 3; j++ )
      v3[ j ] = '\0';

    // encode
    v4[ 0 ] =  ( v3[ 0 ] & 0xfc ) >> 2;
    v4[ 1 ] = (( v3[ 0 ] & 0x03 ) << 4) + ( ( v3[ 1 ] & 0xf0 ) >> 4 );
    v4[ 2 ] = (( v3[ 1 ] & 0x0f ) << 2) + ( ( v3[ 2 ] & 0xc0 ) >> 6 );
    v4[ 3 ] =    v3[ 2 ] & 0x3f;

    // update return buffer
    for( int4 j = 0; (j < i + 1); j++ )
      encoded->Append(sBase64Characters[ v4[ j ] ]);

    // pad remaining
    while( i++ < 3 )
      encoded->Append('=');
  }

  return encoded;
} // Encode( const std::shred_ptr<Buffer> & buffer )

/**
 * Encodes the specified data to Base64
 *
 * @param   pData           data to encode
 * @param   uDataLength     size of data to encode
 * @return  encoded string
 */
std::shared_ptr<Buffer> Base64::Encode( const std::shared_ptr<Buffer> & buffer ) 
{
  return Encode(buffer->GetData(), buffer->GetLength());
} // Encode( const uint1 * pData, uint4 uDataLength )


/**
 * Decodes the specified encoded string
 *
 * @param   sEncoded    encoded string
 * @return  decoded string
 */
std::string Base64::Decode( const std::string & sEncoded ) 
{
  // decoded string
  std::string sDecoded;

  // tracks where we are inside the encoded buffer
  int iEncodedBufferIndex = 0;

  // tracks where we are in the encoded quad
  int4 i = 0;
  
   /// orginal encoded data
  uint1 v4[ 4 ];

  /// resultant decoded data
  uint1 v3[ 3 ];
  
  // process all input except for any padding at the end
  int4 iDataLength = sEncoded.size();
  while( iDataLength-- && sEncoded[ iEncodedBufferIndex ] != '=' ) 
  {
    v4[ i++ ] = sEncoded[ iEncodedBufferIndex++ ]; 
    //iEncodedBufferIndex++;
    if( i == 4 ) 
    {
      for( i = 0; i < 4; i++ )
        v4[ i ] = sBase64Characters.find( v4[ i ] );

      // decode
      v3[ 0 ] = (   v4[ 0 ] << 2)          + ( ( v4[ 1 ] & 0x30 ) >> 4 );
      v3[ 1 ] = ( ( v4[ 1 ] & 0xf ) << 4 ) + ( ( v4[ 2 ] & 0x3c ) >> 2 );
      v3[ 2 ] = ( ( v4[ 2 ] & 0x3 ) << 6 ) + v4[ 3 ];

      // update return buffer 
      for( i = 0; i < 3; i++ )
        sDecoded += v3[ i ];
      i = 0;
    }
  } //   while( iDataLength-- && sEncoded[ iEncodedBufferIndex ] != '=' ) 

  // handle any extra
  if( i ) 
  {
    // add 0 to leftover
    for( int4 j = i; j < 4; j++ )
      v4[ j ] = 0;

    for( int4 j = 0; j <4; j++ )
      v4[ j ] = sBase64Characters.find( v4[ j ] );

    // decode
    v3[ 0 ] = (   v4[ 0 ] << 2)          + ( ( v4[ 1 ] & 0x30 ) >> 4 );
    v3[ 1 ] = ( ( v4[ 1 ] & 0xf ) << 4 ) + ( ( v4[ 2 ] & 0x3c ) >> 2 );
    v3[ 2 ] = ( ( v4[ 2 ] & 0x3 ) << 6 ) + v4[ 3 ];

    // update return buffer
    for( int4 j = 0; j < i - 1; j++ ) 
      sDecoded += v3[ j ];
  }
  return sDecoded;
} // std::string Base64::Decode( const std::string & sEncoded ) 


// $Log: Base64.C,v $
// Revision 1.2  2008/04/24 21:48:29  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.1.2.1  2008/01/18 18:25:56  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Base64.C,v 1.2 2008/04/24 21:48:29 jmeade Exp $
// $Id: Base64.C,v 1.2 2008/04/24 21:48:29 jmeade Exp $
