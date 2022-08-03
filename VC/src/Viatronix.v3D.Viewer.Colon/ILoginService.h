// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ILoginService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net


#pragma once

#include <string>

/**
 * Login service implementation
 */
class ILoginService
{
public:

  // destructor
  virtual ~ILoginService() = 0 {}
  
  // Acquires a context
  virtual std::string Login( const std::string & sUser, const std::string & sContextId, const std::string & sPassword = "" ) = 0;

  // Acquires a context
  virtual void Authenticate(  const std::string & sContextId, const std::string & sUser, const std::string & sPassword = "" ) = 0;

  // Release the context
  virtual void Logout( const std::string & sContextId ) = 0;

  // Renew context
  virtual void Renew( const std::string & sContextId ) = 0;

}; //class ILoginService

