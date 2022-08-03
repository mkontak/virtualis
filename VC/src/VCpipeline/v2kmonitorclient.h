// $Id: v2kmonitorclient.h,v 1.2 2005/11/16 11:41:26 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/_deprecated/v2kmonitorclient.h,v 1.2 2005/11/16 11:41:26 mkontak Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file 
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: V2kMonitorClient.h
// Description: interface for the V2kMonitorClient clas
// Created: 10/10/00
// Author: Soeren Grimm soeren@viatronix.net
//

#ifndef V2kMonitorClient_h
#define V2kMonitorClient_h


#include "Mutex.h"

#include "DatabaseConst.h"

/**
 * Frontend to the vc2k Gui. With this class, it is possible to connect to the v2kGui and write into to the fields of the
 * first tab of the v2k Gui.
 * $Id: v2kmonitorclient.h,v 1.2 2005/11/16 11:41:26 mkontak Exp $ <a href="../../src/u_gui/spec/V2kMonitorClientInterface.htm">Design Specification</a>
*/
class V2kMonitorClient
{
public:
  V2kMonitorClient();  /// Constructor.
  ~V2kMonitorClient(); /// Destructor.
  static void ConnectToHost(const int4 iPort,const std::string sHost = "localhost");      /// Connect to the vc2k Gui on specified port.  
  static void SendState(const int4 iPort,const std::string & sHost, const std::string & sModuleType, const std::string & sStage, const int4 & iStageProgress, const int4 & iOverallProgress);
  static void SendInformation(const int4 iPort,const std::string & sHost, const std::string & sModuleType, const std::string & sStage, const std::string & sMessage);  
  
private:
  /// Convertrs int to byte array
  static void Send(const std::string & sMessage);  /// Send Message to vc2k Gui.


  /// Fields
private:

  static bool m_bIsConnected;            /// If connected to Gui 'bIsConnected' is set to true.
  static Mutex m_mutex;               /// Mutex, to be Thread safe
  static SOCKET m_socket;              /// Socket

};

#endif

// $Log: v2kmonitorclient.h,v $
// Revision 1.2  2005/11/16 11:41:26  mkontak
// no message
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.2.12.1  2003/04/25 18:22:34  mkontak
// Added port/host to sendmessage and sendinformation so that if the
// connection is down a new one can be attempted.
//
// Revision 3.3  2003/04/25 18:21:31  mkontak
// Added port/host to sendmessage and sendinformation so that if the
// connection is down a new one can be attempted.
//
// Revision 3.2  2002/05/01 17:21:41  mkontak
// Added study type to broadcast message
//
// Revision 3.1  2002/02/28 18:12:51  geconomos
// Removal of QT.
//
// Revision 3.0  2001/10/14 23:02:29  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:06   ingmar
// Initial revision.
// Revision 1.7  2001/08/22 23:31:35  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.6  2001/07/30 14:15:29  soeren
// changed enum name
//
// Revision 1.5  2001/07/26 14:49:54  soeren
// renamed function
//
// Revision 1.4  2001/07/25 17:21:25  soeren
// new version of monitor client
//
// $Id: v2kmonitorclient.h,v 1.2 2005/11/16 11:41:26 mkontak Exp $