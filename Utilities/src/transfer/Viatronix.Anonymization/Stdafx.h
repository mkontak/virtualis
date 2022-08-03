
// $Id: Stdafx.h,v 1.6 2007/03/12 14:45:43 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Romy Sreedhana] (mailto:romy@viatrinix.com)


// Pragmas
#pragma once

#define _SECURE_SCL 0
//#define _HAS_ITERATOR_DEBUGGING  0


// includes

#include "Logger.h"
#include "vxBlockVolumeLibrary.h"

#include <string>
#include <memory>
#include "windows.h"

#ifdef _DEBUG
#import "..\..\..\Libraries\com\ReportLibd.dll" named_guids 
#else
#import "..\..\..\Libraries\com\ReportLib.dll" named_guids 

#endif

// defines
#define OPEN_ANMZN_NS namespace Viatronix { namespace Anonymization {
#define CLOSE_ANMZN_NS } }
#define USING_ANMZN_NS using namespace Viatronix::Anonymization;

/**
 ^ Returns an unmanaged std::string
 ^ @param string
 */
inline const std::string GetUnManagedStdString( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;
  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );
  std::string converted = ( const char* ) buffer.ToPointer();
  Marshal::FreeHGlobal( buffer );
  return converted;
} // GetUnManagedStdString()

/**
 ^ Returns an unmanaged std::string
 ^ @param string
 */
inline const std::string ss( System::String ^ string )
{
  return GetUnManagedStdString(string);
} // ss()

// $Log: Stdafx.h,v $
// Revision 1.6  2007/03/12 14:45:43  mkontak
// coding standards
//
// Revision 1.5  2007/03/12 14:44:45  mkontak
// coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/Stdafx.h,v 1.6 2007/03/12 14:45:43 mkontak Exp $
// $Id: Stdafx.h,v 1.6 2007/03/12 14:45:43 mkontak Exp $
