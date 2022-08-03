// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: vxLoggerServer.cpp
//
// Description:
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#include "stdafx.h"
#include "vxLogEnums.h"
#include "vxLoggerServer.h"
#include "vxLogRecord.h"
#include "vxLogExceptions.h"
#include "vxLogMutex.h"
#include "vxUtilIO.h"
#include "vxUtilStrings.h"
#include "vxTaskGroup.h"

/**
 * Copy Constructor
 *
 * @param logger   source
 */
vxLoggerServer::vxLoggerServer ( const vxLoggerInfo & loggerInfo ) :
vxLoggerClassic(loggerInfo),
m_bInitialized(false)
{
  m_tasks = std::make_shared<TaskGroup>();

  Initialize();

}//vxLoggerClassic ( const vxLogger & logger )


/**
 * Destructor
 */
vxLoggerServer::~vxLoggerServer()
{
  Release();
}//~vxLoggerServer()


/**
 * Creates the logger
 */
std::shared_ptr<vxLogger> vxLoggerServer::Create( const vxLoggerInfo & loggerInfo )
{

  std::shared_ptr<vxLogger> pLogger(new vxLoggerServer(loggerInfo));

  if ( pLogger == __nullptr )
	throw vxLogOutOfMemoryException(LogRec("Failed to allocation vxLogger", "vxLogger", "Create"));

  /// Creates the nutex
  pLogger->CreateMutex();

  return pLogger;

} // Create()



/**
 * Initializes the logger
 */
void vxLoggerServer::Initialize()
{

  if ( ! m_bInitialized )
  {


	int error = WSAStartup(0x0202, &m_wsaData);


	if (error)
	{
	  TraceClassic(LogRec(util::Strings::Format("Failed to start Windows Sockets [%ld]",error),"vxLoggerServer","Initialize"));
	  return;
	}


	if ( m_wsaData.wVersion != 0x0202 )
	{
	  WSACleanup(); // Clean up Winsock

	  TraceClassic(LogRec(util::Strings::Format("Invalid Windows Sockets version [%4x]",m_wsaData.wVersion),"vxLogServer","Run"));
	}

	m_bInitialized = true;

  }


} // Initialize()

/**
 * Releases the resources
 */
void vxLoggerServer::Release()
{

  if ( m_wsaData.wVersion == 0x0000 )
  {
	WSACleanup();
	m_wsaData.wVersion = 0x0000;
  }

} // Release();


/**
 * Disconnects from the specified  socket
 */
void vxLoggerServer::Disconnect(SOCKET & socketHandle)
{

  if ( socketHandle != INVALID_SOCKET )
  {

	if ( shutdown(socketHandle, SD_BOTH) == SOCKET_ERROR )
	  TraceClassic(LogRec(util::Strings::Format("Failed to shutdown the socket (%d)",WSAGetLastError()),"vxLogServer","Run"));

	closesocket(socketHandle);
  }

  socketHandle = INVALID_SOCKET;

} // Disconnect()

/**
 * Attempts to connect to the server
 */
SOCKET vxLoggerServer::Connect()
{

  SOCKET socketHandle = INVALID_SOCKET;

  try
  {
	// ============================================================================================
	// If winsock initialized correctly and we are not already connected then attempt to connect
	// ============================================================================================
	if ( m_bInitialized  )
	{


	  // Make sure the socket was created
	  if ((socketHandle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		throw vxLogIOException(LogRec(util::Strings::Format("Failed to create socket (%d)",WSAGetLastError()),"vxLogServer","Run"), false);



	  hostent * pHostEntry(__nullptr);
	  unsigned long addr = inet_addr(m_sServer.c_str());

	  if( addr == INADDR_NONE)
		pHostEntry = gethostbyname(m_sServer.c_str());
	  else
		pHostEntry = gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);

	  if( pHostEntry == __nullptr)
		throw vxLogException(LogRec(util::Strings::Format("Failed to get servers host (%d)",WSAGetLastError()),"vxLoggerServer","Connect"), false);

	  SOCKADDR_IN server;
	  server.sin_addr.s_addr  = *((unsigned long*)pHostEntry->h_addr);
	  server.sin_family       = AF_INET;
	  server.sin_port         = htons(m_iPort);

	  // Attempt to bind to the socket
	  if ( connect(socketHandle, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR )
		throw vxLogException(LogRec(util::Strings::Format("Failed to connect to socket (%d)",WSAGetLastError()),"vxLoggerServer","Connect"), false);

	} // END ... If not connected yet



  }
  catch ( vxLogException & ex )
  {
	TraceClassic(ex.GetLogRecord());

	Disconnect(socketHandle);

   }
  catch ( ... )
  {
	Disconnect(socketHandle);

	TraceClassic(LogRec("Failed to connect the logger service : unspecified error","vxLogServer","Connect"));

   }

  return socketHandle;

} // Connect()

/**
 * Writes the message out to the log file
 *
 * @param record   vxLogRecord object which contains all the log data.
 *
 */
void vxLoggerServer::Trace( const vxLogRecord & record )
{

  /// Run the server
  m_tasks->Group.run( [this, record] ()
  {


	SOCKET socketHandle = INVALID_SOCKET;

	try
	{

	  // =======================
	  // Connect to log server
	  // =======================
	  if ( (socketHandle =  Connect()) != INVALID_SOCKET )
	  {

		// LINGER on close if there is unsent data
		u_long iMode=1;
		  ioctlsocket(socketHandle,SO_LINGER,&iMode);

		std::string sFilename = util::IO::GetFilenameWithoutExt(m_sFilename);

		if ( sFilename == m_sHandle )
		  sFilename = "";
		else
		  sFilename = m_sFilename;

		std::ostringstream os;




		os  << "<logger handle=\""
			<< m_sHandle
			<< "\" sys=\""
			<< m_sSystem
			<< "\" lvl=\""
			<< (int)m_eLogLevel
			<< "\" fmt=\""
			<< (int)m_eFormat
			<< "\" file=\""
			<< sFilename
			<< "\" >"
			<< const_cast<vxLogRecord &>(record).ToXml()
			<< " /logger>";

		std::string sXml(os.str());

		REQUEST rq;
		strcpy(rq.Tag,"RQ");
		rq.Size = (int)sXml.size();


		// Send the request
		if ( send(socketHandle, (char *)&rq, sizeof(REQUEST),0) < 0 )
		  throw vxLogException(LogRec(util::Strings::Format("Failed to send request header (%d)", WSAGetLastError()), "vxLoggerServer","Trace"), false);

		if ( send(socketHandle, sXml.c_str(), (int)sXml.size(), 0) < 0 )
		  throw vxLogException(LogRec(util::Strings::Format("Failed to send request data (%d)", WSAGetLastError()), "vxLoggerServer","Trace"), false);

		// ===============================================================================================================================================
		// I do not require a response back from the server indicating that ot received and logged the message. This would make the process a bit slower
		// so I disabled it.
		// ==============================================================================================================================================
		//recv(m_socketHandle, (char *)&rsp, sizeof(RESPONSE), 0 );


	  } // END ... If we connected
	  else
		TraceClassic(record);
	}
	catch ( vxLogException & ex )
	{
	  Disconnect(socketHandle);

	  TraceClassic(record);

	  TraceClassic(ex.GetLogRecord());
	}
	catch ( ... )
	{
	   Disconnect(socketHandle);

	  TraceClassic(record);

	  TraceClassic(LogRec("Failed to send record to logger : unspecified error","vxLogRecord","Trace"));

	}




   }  );


}

/**
 * Writes the message out to the log file
 *
 * @param record   vxLogRecord object which contains all the log data.
 *
 */
void vxLoggerServer::TraceClassic( const vxLogRecord & record )
{

  vxLoggerClassic::Trace(record);

} // Trace( const vxLogRecord & record )


