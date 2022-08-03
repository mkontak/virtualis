// $Id: Base64.h,v 1.2 2008/04/24 21:48:29 jmeade Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declarations
#pragma once



/// Forward declarations
class VX_BASE_DLL Buffer;

/**
 *
 */
class VX_BASE_DLL Base64
{
// member funtions
public:

  // encodes the specified data 
  static std::string Encode( const std::string & s );

  // encodes the specified data 
  static std::shared_ptr<Buffer> Encode( const uint1 * pData , uint4 uDataLength );


  /// Encodes the buffer
  static std::shared_ptr<Buffer> Encode( const std::shared_ptr<Buffer> & buffer );

  // decodes the specified data 
  static std::string Decode( const std::string & s );

private:

  // private constructor
  Base64();

}; // class Base64


// $Log: Base64.h,v $
// Revision 1.2  2008/04/24 21:48:29  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.1.2.1  2008/01/18 18:25:46  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Base64.h,v 1.2 2008/04/24 21:48:29 jmeade Exp $
// $Id: Base64.h,v 1.2 2008/04/24 21:48:29 jmeade Exp $
