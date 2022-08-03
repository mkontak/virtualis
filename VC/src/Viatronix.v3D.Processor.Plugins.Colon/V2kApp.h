// $Id: V2kApp.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: [Name] [e-mail]
//
// Complete History at bottom of file.

#ifndef V2kAPP_H
#define V2kAPP_H

#include "CmdLineSwitches.h"

/// add for using the new license mechanism
//#import "..\..\..\Libraries\tlb\include\Viatronix.v3D.Licensing.tlb" no_namespace named_guids raw_interfaces_only

/**
 * This is the V2k Application class. 
 */
class V2kApp
{
public:
  /// Constructor.
  V2kApp(int iArgc, char * psArgv[]); 
  /// Run the v2k application
  int4 Run(); 


private:
  /// Initialize the streams
  void InitializeStreams();  

private:
  CmdLineSwitches m_cmdLine;
  int m_iArgc;
  char ** m_psArgv;


};



#endif

// $Log: V2kApp.h,v $
// Revision 1.3  2006/06/20 19:48:03  gdavidson
// Changed import for Viatronix.v3D.Licensing
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
// Revision 3.0.28.1  2005/01/12 17:14:32  dongqing
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
//    Rev 1.0   Sep 16 2001 17:26:30   ingmar
// Initial revision.
// Revision 1.1  2001/07/26 16:29:59  soeren
// new main for vcpipeline
//
// Revision 1.8  2001/05/28 20:18:18  dave
// moving to UNC paths, new database
//
// Revision 1.7  2001/05/20 20:26:30  soeren
// More Coding standard complient
//
// Revision 1.6  2001/05/20 20:23:47  soeren
// More Coding standard complient
//
// Revision 1.5  2001/05/02 19:15:46  dave
// converted to new stream classes
//
// Revision 1.4  2001/04/25 14:30:52  soeren
// fixed path bug
//
// Revision 1.3  2001/04/21 05:24:26  soeren
// no message
//
// Revision 1.2  2001/04/17 22:47:40  soeren
// removed enum
//
// Revision 1.1  2001/04/17 19:54:06  soeren
// new main
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/V2kApp.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
// $Id: V2kApp.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
