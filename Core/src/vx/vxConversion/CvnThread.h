// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "CvnCallback.h"

namespace cvn 
{

class CVN_DLL CvnThread : public util::Thread, public CvnCallback 
{
protected:

  /// Constructor
  CvnThread();

  /// Destructor
  virtual ~CvnThread() {}
  

public: 
 
  /// Starts the server. 
  virtual HANDLE Start(std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID ) > callbackFunction = __nullptr, LPVOID callbackData = __nullptr );



}; // class CvnThread

} // namespace cvn