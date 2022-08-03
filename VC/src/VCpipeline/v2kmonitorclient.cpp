//////////////////////////////////////////////////////////////////////
// $Id: v2kmonitorclient.cpp,v 1.4 2006/04/27 14:06:22 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/_deprecated/v2kmonitorclient.cpp,v 1.4 2006/04/27 14:06:22 mkontak Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file 
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: V2kMonitorClient.c
// Description: implementation of the V2kMonitorClient clas
// Created: 10/10/00
// Author: Soeren Grimm soeren@viatronix.net

#include "stdafx.h"
#include "V2kMonitorClient.h"
#include "utilities.h"

bool V2kMonitorClient::m_bIsConnected = false;
Mutex V2kMonitorClient::m_mutex;
SOCKET V2kMonitorClient::m_socket = INVALID_SOCKET;

#define FILE_REVISION "$Revision: 1.4 $"

/** 
 * Constructor.
*/
V2kMonitorClient::V2kMonitorClient()
{
  
}

/** 
 * Destructor. Closes the socket
*/
V2kMonitorClient::~V2kMonitorClient()
{
  if(m_socket != INVALID_SOCKET) closesocket(m_socket);
}

/** 
 * Connect to the vc2k Gui.
 * @param iPort Port Number (8020,8021,8022)
 * @param sHost hostname 
*/
void V2kMonitorClient::ConnectToHost(int4 iPort, const std::string sHost)
{  
  WSADATA WSAData;
  if (WSAStartup(MAKEWORD(1,1), &WSAData)) // Funny windows stuff, to initialize windows socket.
  {
    throw ex::VException(LogRec( "V2kMonitorClient", "ConnectToHost", "WSAStartup failed"));


  }
  m_socket = socket(AF_INET, SOCK_STREAM, 0); /// create socket
  if (m_socket == INVALID_SOCKET)
  {
    throw ex::VException(LogRec( "V2kMonitorClient", "ConnectToHost" , "ConnectToHost socket failed" ));
  }   
  SOCKADDR_IN sin;
  sin.sin_family = AF_INET; /// set socket family
  sin.sin_port = htons(iPort); /// Set port number
  PHOSTENT hent = gethostbyname(sHost.c_str()); /// Set host to localhost 
  memcpy((char *)&(sin.sin_addr), hent->h_addr, hent->h_length);    
  if(connect((SOCKET) m_socket, (struct sockaddr *) &sin, sizeof(struct sockaddr)) == 0)  // connect to v2kGui
  {
    m_bIsConnected = true;
  }
  else
  {
    
    throw ex::VException(LogRec( "V2kMonitorClient", "ConnectToHost", "ConnectToHost connection failed" )); 
  }
}

/** 
 * Send message to vc2k Gui.
 * @param sMessage message to send.
*/
void V2kMonitorClient::Send(const std::string & sMessage)
{ 
  LogTmp("Sending message to WQ Handler [" + sMessage + "]", "V2kMonitorClient", "Send");

  int iSize(sMessage.size());

  char buffer [8];
  buffer[0] = (byte)(iSize & 0X000F);
  buffer[1] = (byte)((iSize >> 4)  & 0X000F );
  buffer[2] = (byte)((iSize >> 8)  & 0X000F );
  buffer[3] = (byte)((iSize >> 12) & 0X000F );
  buffer[4] = (byte)((iSize >> 16) & 0X000F );
  buffer[5] = (byte)((iSize >> 20) & 0X000F );
  buffer[6] = (byte)((iSize >> 24) & 0X000F );
  buffer[7] = (byte)((iSize >> 28) & 0X000F );

  send(m_socket, buffer, 8, 0); 
  send(m_socket, sMessage.c_str(), sMessage.size(), 0); 

}



void V2kMonitorClient::SendState( const int4 iPort,
                                  const std::string & sHost,
                                  const std::string & sModuleType, 
                                  const std::string & sStage, 
                                  const int4 & iStageProgress, 
                                  const int4 & iOverallProgress )
{
  m_mutex.Lock(); // Make sure that only one Thread is in the code.
  
  if ( ! m_bIsConnected )
  {
    try
    {
      ConnectToHost(iPort, sHost);
    }
    catch ( ex::VException )
    {
      LogWrn("Could not establish connection (" + sHost + ", " + ToAscii(iPort) + "), can not update progress monitor.", "V2kApp", "SendInformation");
    }
  }

  if(m_bIsConnected) /// Check if connected to vc2k Gui.
  {      
    std::strstream sBuilder;
    
    sBuilder << "<Status moduleType =\"" << sModuleType << "\" stage=\"" << sStage << "\" stageProgress=\"" << iStageProgress
      << "\" overallProgress=\"" << iOverallProgress << "\" messageType=\"0\"  message=\"\" />";


    Send(std::string(sBuilder.str(), sBuilder.pcount()));/// Finally send message to v2kGui.

  }  
  m_mutex.Unlock();
}

void V2kMonitorClient::SendInformation( const int4 iPort,
                                        const std::string & sHost,
                                        const std::string & sModuleType, 
                                        const std::string & sStage, 
                                        const std::string & sMessage)
{
  m_mutex.Lock(); // Make sure that only one Thread is in the code.
    
  if ( ! m_bIsConnected )
  {
    try
    {
      ConnectToHost(iPort, sHost);
    }
    catch ( ex::VException )
    {
      LogWrn("Could not establish connection (" + sHost + ", " + ToAscii(iPort) + "), can not update progress monitor.", "V2kApp", "SendInformation");
    }
  }
  
  if(m_bIsConnected) /// Check if connected to vc2k Gui.
  {      
    std::strstream sBuilder;
    
    sBuilder << "<Status moduleType =\"" << sModuleType << "\" stage=\"" << sStage << "\" stageProgress=\"0\" overallProgress=\"0\" messageTpe=\"2\" message=\"" << sMessage << "\" \\>";


    Send(std::string(sBuilder.str(), sBuilder.pcount()));/// Finally send message to v2kGui.
  }  
  m_mutex.Unlock();  
}

#undef FILE_REVISION

// $Log: v2kmonitorclient.cpp,v $
// Revision 1.4  2006/04/27 14:06:22  mkontak
// Fixed messaging
//
// Revision 1.3  2005/11/16 11:41:26  mkontak
// no message
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.4.10.1  2003/04/25 18:22:34  mkontak
// Added port/host to sendmessage and sendinformation so that if the
// connection is down a new one can be attempted.
//
// Revision 3.5  2003/04/25 18:21:31  mkontak
// Added port/host to sendmessage and sendinformation so that if the
// connection is down a new one can be attempted.
//
// Revision 3.4  2002/05/09 20:17:40  mkontak
// Issue 2178: Fixed message ending
//
// Revision 3.3  2002/05/01 17:21:41  mkontak
// Added study type to broadcast message
//
// Revision 3.2  2002/02/28 18:12:51  geconomos
// Removal of QT.
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
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
// Revision 1.5  2001/08/13 20:05:52  tushar
// removed errors due to the new logger
//
// Revision 1.4  2001/07/30 14:15:29  soeren
// changed enum name
//
// Revision 1.3  2001/07/26 14:49:54  soeren
// renamed function
//
// Revision 1.2  2001/07/25 23:34:26  soeren
// changed to new exceptions
//
// Revision 1.1  2001/07/25 17:21:25  soeren
// new version of monitor client
//
// Revision 1.1  2001/07/09 14:15:13  soeren
// $Id: v2kmonitorclient.cpp,v 1.4 2006/04/27 14:06:22 mkontak Exp $