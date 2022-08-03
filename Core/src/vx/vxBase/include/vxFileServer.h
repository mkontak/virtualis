// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileServer.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "FileTable.h"
#include "FileServerHeader.h"
#include "Stream.h"


// pragma
#pragma warning(push)
#pragma warning(disable:4482)

/// Forward declaration
class Task;

namespace io
{



/**
 * Implements a file server
 */
class VX_BASE_DLL FileServer
{
private:

  FileServer(const uint4 uPort);


public:

  
  ~FileServer();

  /// Creates the file server
  static std::shared_ptr<FileServer> Create( const uint4 uPort );

  //Runs the server synchronously
  void RunSynchrounously();

  /// Starts the server asynchronously
  void Start();

  /// Aborts the server
  void Abort();

  /// Request handler
  void RequestHandler(const std::shared_ptr<Stream> & stream);

private:

    /// Sends the response header back
  void SendResponse(const std::shared_ptr<Stream> & stream, const FILESERVERHEADER & header, const int4 iStatusCode, const uint4 uSize = 0, const bool bEof = false );


#pragma region command handlers

private:

  /// Open Command Handler
  void Open(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Query command handler
  void Query(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Delete command handler
  void Delete(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Exists command handler
  void Exists(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// GetFiles command handler
  void GetFiles(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// GetDirs command handler
  void GetDirs(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Ping command handler
  void Ping(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Write command handler
  void Write(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Read command handler
  void Read(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Close command handler
  void Close(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Size command handler
  void Size(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Seek command handler
  void Seek(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Flush command handler
  void Flush(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Flush command handler
  void Eof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

  /// Flush command handler
  void SmartEof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream);

#pragma endregion

public:

  /// Get header size
  static unsigned int GetHeaderSize()
  { return (unsigned int)FILESERVERHEADER_SIZE; }

  // Gets the file server port
  uint4 GetPort() 
  { return m_uPort; }

  // Returns true if started
  bool IsStarted()
  { return ( m_socketHandle != INVALID_SOCKET ); }
  
private:

  // Port to listen on
  uint4 m_uPort;

  /// Socket Handle
  SOCKET m_socketHandle;

  // Initialized flag
  bool m_bInitialized;


  // Abort requested flag
  bool m_bAbortRequested;


  // File table 
  FileTable m_fileTable;

 
  /// Task group for processing files operations
  std::shared_ptr<::Task> m_task; 

  /// Started flag
  bool m_bStarted;

 
};  // class FileServer


} /// namespace io

#pragma warning(pop)