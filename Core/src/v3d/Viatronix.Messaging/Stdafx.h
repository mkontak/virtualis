// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Stdafx.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once


#include <string>
#include <memory>

#include "Logger.h"
#include "vxMessagingLibrary.h"


#pragma warning(disable : 4251)
#pragma warning(disable : 4482)

#define _CRT_SECURE_NO_DEPRECATE

#define USING_MESSAGE_NS  using namespace Viatronix::Messaging;
#define OPEN_MESSAGE_NS   namespace Viatronix { namespace Messaging {
#define CLOSE_MESSAGE_NS  } } 



/*
 * Converts a managed System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string representation of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ss()



