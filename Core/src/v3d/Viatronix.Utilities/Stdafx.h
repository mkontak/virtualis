// $Id: Stdafx.h,v 1.11.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Stdafx.h,v 1.11.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com
//
// Complete history on bottom of file

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

// Pragmas
#pragma once






#include <WinSock2.h>
#include <windows.h>
#include <winhttp.h>
#include "wmsdk.h"
#include <vfw.h>
#include <string>



#include "Logger.h"

// Includes
#include "vxBaseLibrary.h"

// Imports
//#ifdef _DEBUG
//#import "..\..\..\libs\com\ReportLibd.dll" named_guids 
//#else
//#import "..\..\..\libs\com\ReportLib.dll" named_guids 
//#endif

// Undefines
#ifdef GetTempPath
#undef GetTempPath
#endif

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif


/**
 * Returns an unmanaged std::string
 * @param string
 */
inline const std::string GetUnManagedStdString( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // GetUnManagedStdString()


inline const std::string ss( System::String ^ string )
{
  return GetUnManagedStdString( string );
}


/**
 * Returns an unmanaged std::string
 * @param string
 */
inline const std::wstring ws( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalUni( string );

  std::wstring converted = ( const wchar_t * ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ws()




///Logs the unhandled exception messages in to app folder
void ExceptionToFile( System::String ^ message, System::Exception ^ ex );


/*
 * converts the System string to Native String
 * @param System::String
 */
inline const std::string ToNativeString( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;
  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );
  std::string converted = ( const char* ) buffer.ToPointer();
	Marshal::FreeHGlobal( buffer );
  return converted;
} // ToNativeString


/*
 * Converts the GUID to system GUID
 */
inline System::Guid  ToSystemGUID( const GUID & guid  )
{
  return System::Guid( guid.Data1, guid.Data2, guid.Data3, 
                guid.Data4[ 0 ], guid.Data4[ 1 ], 
                guid.Data4[ 2 ], guid.Data4[ 3 ], 
                guid.Data4[ 4 ], guid.Data4[ 5 ], 
                guid.Data4[ 6 ], guid.Data4[ 7 ] );
 // return *__try_cast< System::Guid* >( System::Runtime::InteropServices::Marshal::PtrToStructure( System::IntPtr( ( void * )&guid ), __typeof( System::Guid ) ) );
}//ToSystemGUID( const GUID & guid  )



/*
 * Converts the managed guid to native guid
 * @param System::Guid
 */
//inline GUID ToNativeGUID( System::Guid guid ) 
//{
//   array<System::Byte> ^ guidData =  guid.ToByteArray();
//   System::Byte data = (guidData[0]);
//   return *(GUID *)data;
//}// ToNativeGUID( System::Guid guid ) 

// Defines
#define OPEN_UTILITIES_NS namespace Viatronix {  namespace Utilities {
#define CLOSE_UTILITIES_NS } } 
#define USING_UTILITIES_NS using namespace Viatronix::Utilities;

#define OPEN_LICENSING_NS namespace Viatronix { namespace v3D { namespace Licensing {
#define CLOSE_LICENSING_NS } } }
#define USING_LICENSING_NS using namespace Viatronix::v3D::Licensing;

#define OPEN_REGISTRY_NS namespace Viatronix { namespace Utilities { namespace Registry {
#define CLOSE_REGISTRY_NS } } } 
#define USING_REGISTRY_NS using namespace Viatronix::Utilities::Registry;

#define OPEN_IO_NS namespace Viatronix { namespace Utilities { namespace IO {
#define CLOSE_IO_NS } } } 
#define USING_IO_NS using namespace Viatronix::Utilities::IO;

#define OPEN_TYPES_NS namespace Viatronix { namespace Utilities { namespace Types {
#define CLOSE_TYPES_NS } } } 
#define USING_TYPES_NS using namespace Viatronix::Utilities::Types;

#define OPEN_IMAGING_NS namespace Viatronix {  namespace Imaging {
#define CLOSE_IMAGING_NS  } } 
#define USING_IMAGING_NS  using namespace Viatronix::Imaging;


#define OPEN_THREADING_NS namespace Viatronix {  namespace Threading {
#define CLOSE_THREADING_NS } } 
#define USING_THREADING_NS  using namespace Viatronix::Threading;

#define OPEN_REPORTING_NS namespace Viatronix {  namespace Reporting {
#define CLOSE_REPORTING_NS } } 
#define USING_REPORTING_NS using namespace Viatronix.Reporting;

#define OPEN_EMAILING_NS namespace Viatronix { namespace Emailing {
#define CLOSE_EMAILING_NS } } 
#define USING_EMAILING_NS using namespace Viatronix::Emailing;


// Defines
#define OPEN_PIPELINE_NS namespace Viatronix { namespace Utilities { namespace Pipelining {
#define CLOSE_PIPELINE_NS } } }
#define USING_PIPELINE_NS using namespace Viatronix::Utilities::Pipelining;


OPEN_EMAILING_NS

/**
 *  Impelements an InvalidInstallationException
 */
public ref class InvalidDataException : public System::Exception 
{
public:
  /// Constructor
  InvalidDataException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  InvalidDataException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Destructor
  virtual ~InvalidDataException() { }
};  // class InvalidDataException

CLOSE_EMAILING_NS


OPEN_REGISTRY_NS

/**
 *  Impelements an InvalidInstallationException
 */
public ref class InvalidInstallationException : public System::Exception 
{
public:
  /// Constructor
  InvalidInstallationException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  InvalidInstallationException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Destructor
  virtual ~InvalidInstallationException() { }
};  // class InvalidInstallationException

CLOSE_REGISTRY_NS

OPEN_PIPELINE_NS

/**
* Impelements an Abort Exception
*/
public ref class AbortException : public System::Exception 
{
public:
  /// Constructor
  AbortException() : System::Exception() { }


  /// Constructor
  AbortException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  AbortException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Constructor
  AbortException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : System::Exception(pInfo, context) { }

  /// Destructor
  virtual ~AbortException() { }
};  // class AbortException

CLOSE_PIPELINE_NS



// Revision 1.6  2005/02/10 16:32:44  mkontak
// Fixes and additionas for the Dicom Server Patch
//
// Revision 1.5  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.4.2.2  2005/03/31 18:49:00  mkontak
// no message
//
// Revision 1.4.2.1  2004/11/05 12:31:51  mkontak
// no message
//
// Revision 1.4  2004/05/07 19:17:44  mkontak
// Rolled the Report.Dicom.dll into the utilities
//
// Revision 1.3  2004/03/21 23:10:00  geconomos
// Upgrade to Visual Studio .NET 2003
//
// Revision 1.2  2004/03/02 20:06:32  geconomos
// Merged Viatronix.Logger, Viatronix.Imaging, Viatronix.Threading and Viatronix.Licensing into Viatronix.Utilities.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.15  2003/07/22 18:34:39  mkontak
// Fixes for configuration
//
// Revision 1.14  2003/06/23 15:51:36  dongqing
// code review
//
// Revision 1.13  2003/06/20 18:33:09  mkontak
// Coding standards
//
// Revision 1.12  2003/05/15 12:29:12  mkontak
// Added extensions
//
// Revision 1.11  2003/05/12 14:46:07  mkontak
// Cosmetic
//
// Revision 1.10  2003/05/05 12:23:30  mkontak
// Reorganized registry added new funtionality
//
// Revision 1.9  2003/04/15 14:10:39  mkontak
// Re-Write of CDR
//
// Revision 1.8  2003/04/07 15:20:20  geconomos
// Licensing namespace added.
//
// Revision 1.7  2003/02/13 15:35:02  geconomos
// Added #defines for opening, closing and using the utilities namespace.
//
// Revision 1.6  2002/11/25 22:22:11  mkontak
// Fixed memory leak
//
// Revision 1.5  2002/08/20 16:00:25  mkontak
// Update for V3DSystemSettings
//
// Revision 1.4  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// Revision 1.3  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Stdafx.h,v 1.11.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: Stdafx.h,v 1.11.2.1 2011/05/18 01:56:58 mkontak Exp $
