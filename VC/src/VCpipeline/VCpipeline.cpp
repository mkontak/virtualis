// $Id: VCpipeline.cpp,v 1.5 2006/08/31 20:57:57 dongqing Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Created:  
// Author: Soeren Grimm soeren@viatronix.net
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "V2kApp.h"
#include "ExceptionSetup.h"
#include "vxDOMDocument.h"

using namespace std;

/**
*The one and only main.
*/
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _WIN32
  SetThreadPriority(GetCurrentThread (),THREAD_PRIORITY_BELOW_NORMAL);
  SetErrorMode(SEM_NOGPFAULTERRORBOX);
  CoInitialize( NULL );  /// This is only for Licensing purpose
#endif  

  ExceptionSetup::SetNewHandler();
  ExceptionSetup::SetTerminateHandler();
	vxDOMDocument::Initialize();

	try
  {
    V2kApp app(argc, argv);    // Create pipeline application.
		int i = app.Run(); // Start pipeline application.
    return( i );
  }
	catch( ... )
	{
		return( 1 );
	}

  CoUninitialize();   /// This is only for Licensing purpose
} // main

// $Log: VCpipeline.cpp,v $
// Revision 1.5  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.4  2006/08/29 18:38:29  gchin
// Changes from Dongqing for Dr. Cadi
//
// Revision 1.3  2006/06/09 12:35:39  dongqing
// add series version XML
//
// Revision 1.2  2005/11/15 13:12:19  mkontak
// Fix to work with the new WQ and Console 3.0
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0.22.1  2005/02/15 17:41:01  jmeade
// merge from vc1.4 branch.
//
// Revision 3.0.28.1  2005/01/12 17:14:31  dongqing
// Using new licensing mechanism
//
// Revision 3.0  2001/10/14 23:01:51  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:28   ingmar
// Initial revision.
// Revision 1.4  2001/08/20 17:21:26  soeren
// decreased priority of pipeline under windows
//
// Revision 1.3  2001/08/16 14:34:37  ingmar
// added header and Id
//
// Revision 1.2  2001/08/07 17:05:28  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.1  2001/07/26 16:29:59  soeren
// new main for vcpipeline
//
// Revision 1.5  2001/05/28 20:18:18  dave
// moving to UNC paths, new database
//
// Revision 1.4  2001/05/23 00:57:06  dave
// moving to new database classes
//
// Revision 1.3  2001/05/20 20:27:13  soeren
// More Coding standard complient
//
// Revision 1.3  2001/05/20 20:27:13  soeren
// More Coding standard complient
//
// Revision 1.2  2001/05/20 20:23:47  soeren
// More Coding standard complient
//
// Revision 1.1  2001/04/17 19:54:06  soeren
// new main
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/VCpipeline.cpp,v 1.5 2006/08/31 20:57:57 dongqing Exp $
// $Id: VCpipeline.cpp,v 1.5 2006/08/31 20:57:57 dongqing Exp $
