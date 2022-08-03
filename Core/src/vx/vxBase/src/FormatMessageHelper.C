// $Id: FormatMessageHelper.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "FormatMessageHelper.h"

#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Gets the message asscociated with the specified last error code
 *
 * @param   dwLastError   last error code
 * @return  message associated with last error code
 */
std::string FormatMessageHelper::Format( DWORD dwLastError )
{
  std::string sMsg;

  DWORD dwFlags( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS );

  LPVOID pMsgBuf( NULL );
  if( ::FormatMessage( dwFlags, NULL, dwLastError, 0, reinterpret_cast< LPTSTR >( &pMsgBuf ), 0, NULL ) == 0 )
    LogErr( "Unable to retrieve message for id = " + ToAscii( dwLastError ), "FormatMessageHelper", "FormatMessage" )
  
  if( pMsgBuf )
  {
    sMsg = reinterpret_cast< const char * >( pMsgBuf );
    LocalFree( pMsgBuf );
  }
  return sMsg;
} // FormatMessageHelper::FormatMessage( DWORD dwLastError )


// $Log: FormatMessageHelper.C,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/FormatMessageHelper.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: FormatMessageHelper.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
