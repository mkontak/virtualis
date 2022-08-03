// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpSession.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once

#include "HttpHandle.h"


#pragma warning(push)
#pragma warning(disable:4251)


namespace http
{


/**
 * Implements an HTTP session
 */
class VX_BASE_DLL  HttpSession : public HttpHandle
{

protected:

  // Constructor
  HttpSession() : HttpHandle() {}

private:

  /// Copy constructor
  HttpSession(const HttpSession & session);

public:

  static std::shared_ptr<HttpSession> Create();


};

} // namespace http

#pragma warning(pop)