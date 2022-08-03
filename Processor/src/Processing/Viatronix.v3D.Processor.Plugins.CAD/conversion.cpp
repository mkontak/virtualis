// $Id: conversion.cpp,v 1.2 2006/06/02 13:01:49 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

#include "StdAfx.h"
#include "conversion.h"

#define FILE_REVISION "$Revision: 1.2 $"



/** 
 * Converts a UNC path to a local one (if possible)
 */
std::string Conversion::UNC2Local( const std::string & sUNCPath )
{
  USES_CONVERSION;

  // make sure that the supplied path is unc
  if( !PathIsUNC( sUNCPath.c_str() ) )
    return sUNCPath;

  // upon return will contain the converted path
  std::string sLocalPath;

  // grab the share name from the unc path ( maybe includes sub directories )
  const char * sTempShareName = strstr( sUNCPath.c_str() + 2, "\\" ) + 1;

  // will contain share name
  char sShareName[ 255 ];
  
  // check to see if any subdirectories were specified in the original unc path
  char * sShareDirectory( NULL );
  if( ( sShareDirectory = const_cast< char * >( strstr( sTempShareName, "\\" ) ) ) != NULL )
  {
    // remove the additional sub directoreis from the share name
    lstrcpyn( sShareName, sTempShareName, static_cast< int4 >( sShareDirectory - sTempShareName + 1) );
  }
  else
  {
    // no subdirectories specified, just copy
    lstrcpy( sShareName, sTempShareName );
  }

  // get the info for the hare, we are assuming that the local machine owns the share
  PSHARE_INFO_502 pShareInfo( NULL );
  if( NetShareGetInfo( NULL, A2W( sShareName ), 502, reinterpret_cast< LPBYTE * >( &pShareInfo ) ) == ERROR_SUCCESS )
  {
    // get the absolute path
    sLocalPath = W2A( pShareInfo->shi502_path );
    
    // was a subdirectory specified in the original unc path
    if( sShareDirectory != NULL )
    {
      // append the subdirectory to the absolute path
      sLocalPath += sShareDirectory; 
    }
    
    // free resources
    NetApiBufferFree( pShareInfo );
  }
  else
  {
    LogErr( "Unable to convert unc path: " + sUNCPath, "", "UNC2Local" );
  }
  return sLocalPath;
} // UNC2Local( const std::string & sUNCPath )


// $Log: conversion.cpp,v $
// Revision 1.2  2006/06/02 13:01:49  geconomos
// corrected compilation error porting to 2005
//
// Revision 1.1  2006/02/09 19:27:54  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/conversion.cpp,v 1.2 2006/06/02 13:01:49 geconomos Exp $
// $Id: conversion.cpp,v 1.2 2006/06/02 13:01:49 geconomos Exp $