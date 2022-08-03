// $Id: AssemblyInfo.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/AssemblyInfo.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
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

// Includes
#include "stdafx.h"

// Namespaces
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;

///////////////////////////////////////////////////////////////////////////////////
// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//////////////////////////////////////////////////////////////////////////////////
[assembly:AssemblyTitleAttribute("V3D Utilities")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Viatronix Inc.")];
[assembly:AssemblyProductAttribute("V3D Utilities")];
[assembly:AssemblyCopyrightAttribute("Copyright 2003")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];		

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers 
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute("1.1.0.0")];

//
// In order to sign your assembly you must specify a key to use. Refer to the 
// Microsoft .NET Framework documentation for more information on assembly signing.
//
// Use the attributes below to control which key is used for signing. 
//
// Notes: 
//   (*) If no key is specified, the assembly is not signed.
//   (*) KeyName refers to a key that has been installed in the Crypto Service
//       Provider (CSP) on your machine. KeyFile refers to a file which contains
//       a key.
//   (*) If the KeyFile and the KeyName values are both specified, the 
//       following processing occurs:
//       (1) If the KeyName can be found in the CSP, that key is used.
//       (2) If the KeyName does not exist and the KeyFile does exist, the key 
//           in the KeyFile is installed into the CSP and used.
//   (*) In order to create a KeyFile, you can use the sn.exe (Strong Name) utility.
//        When specifying the KeyFile, the location of the KeyFile should be
//        relative to the project directory.
//   (*) Delay Signing is an advanced option - see the Microsoft .NET Framework
//       documentation for more information on this.
//
[assembly:AssemblyDelaySignAttribute(false)];
[assembly:AssemblyKeyFileAttribute("")];
[assembly:AssemblyKeyNameAttribute("")];


// Revision History:
// $Log: AssemblyInfo.cpp,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/06/26 14:26:26  mkontak
// Coding standards
//
// Revision 1.5  2003/04/21 20:40:50  mkontak
// CD Writing/Anonymization
//
// Revision 1.4  2003/04/14 15:31:12  geconomos
// Updated the version from 1.0.0.0 to 1.1.0.0
//
// Revision 1.3  2002/09/13 13:48:38  geconomos
// Set version to 1.0.0.0
//
// Revision 1.2  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/AssemblyInfo.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: AssemblyInfo.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

