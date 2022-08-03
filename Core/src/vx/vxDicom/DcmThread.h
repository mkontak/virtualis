// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmThread.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmCallback.h"

#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)


namespace dcm 
{

 /**
  * Implements the Threaded object used within the DICOM library. This gives all objects that are threaded to 
  * same look and feel. Since all threads implements callback functionality in the Start() I derive from the 
  * DcmCallback class. The util::Thread contains all the functionality.
  */
class DCM_DLL DcmThread : public util::Thread, public DcmCallback
{
protected:

  /// Constructor
  DcmThread(const std::string & sStartLogMessage = "");

  /// Destructor
  virtual ~DcmThread() {}
  

public:


  const std::string & GetThreadId() const
  { return m_sId; }

public: 
 
  /// Starts the server. 
  virtual HANDLE Start(LPVOID callbackData, std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction );

  /// Starts the server. 
  virtual HANDLE Start( std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )
  { return Start(__nullptr, callbackFunction); }

  /// Waits forever
private:

  /// Thread Id
  std::string m_sId;

  /// Start log message
  std::string m_sStartLogMessage;

}; // class DcmThread


} // namespace dcm

#pragma warning(pop)