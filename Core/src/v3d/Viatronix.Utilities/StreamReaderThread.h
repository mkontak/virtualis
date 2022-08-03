
// $Id: StreamReaderThread.h,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once

// Includes
#include "ThreadBase.h"
#include "SystemInfo.h"


// Namespaces
OPEN_PIPELINE_NS 


/**
 * Implements a stream reader to read stdin and stderr 
 */
 public ref class StreamReaderThread : public Viatronix::Threading::ThreadBase, public System::IDisposable
{
public:

  /// Construtcor
  StreamReaderThread( String ^ sStreamName, System::IO::StreamReader * pReader) : 
      ThreadBase(String::Format("{0}_STREAM_READER_{1}",sStreamName, (Viatronix::Utilities::SystemInfo::GetCurrentThreadId()))), \
      m_pReader(pReader), 
      m_sStream(System::String::Empty), 
      m_bDisposed(false),
      m_sStreamName(sStreamName) { }

  /// Destructor
  ~StreamReaderThread() 
  { Dispose(false);  }

  /// Dispose of object
  void Dispose() 
  { Dispose(true); System::GC::SuppressFinalize(this); }

  /// Runs the thread
  void Run() { m_sStream = m_pReader->ReadToEnd(); }

protected:

  /// Dispose of object
  void Dispose(bool bDisposing)
  { 
    if ( ! m_bDisposed ) 
    { 
      Viatronix::Logging::Log::Debug("Dispoing of StreamReaderThread", "StreamReaderThread", "Dispose");
      ThreadBase::Dispose(bDisposing);
    } 
    m_bDisposed = true;
  } // Dispose()

public:

  /// Returns the stream name
  property System::String ^ get_StreamName() { return m_sStreamName; }

  /// Returns the stream as a string
  property System::String ^ get_Stream() { return m_sStream; }

private:

  /// Disposed flag
  bool m_bDisposed;

  /// Stream name
  System::String ^ m_sStreamName;

  /// Stream Reader
  System::IO::StreamReader * m_pReader;

  /// Read stream
  System::String ^ m_sStream;

};  // class StreamReaderThread

CLOSE_PIPELINE_NS

// $Log: StreamReaderThread.h,v $
// Revision 1.2.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2007/03/09 21:07:03  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/StreamReaderThread.h,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: StreamReaderThread.h,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
