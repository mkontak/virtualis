// Copyrightę 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseLoginService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ILoginService.h"


class DatabaseLoginService : public ILoginService
{
public:

  // destructor
  virtual ~DatabaseLoginService();
  
  // Acquires a context
  std::string Login( const std::string & sUser, const std::string & sContextId, const std::string & sPassword = "" );

  // Release the context
  void Logout( const std::string & sContextId );

  // Renew context
  void Renew( const std::string & sContextId );

  // Re-Authenticates the user
  void Authenticate(  const std::string & sContextId, const std::string & sUser, const std::string & sPassword = "" );

}; //class DatabaseLoginService

