// $Id: ColonLicense.h,v 1.2.2.3 2008/04/23 20:10:28 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net

#pragma once

#include <string>

/// Wrapper class to ensure proper start-up and release of license client
class ColonLicense
{
public:
  /// Constructor
  ColonLicense();
  
  /// Destructor
  ~ColonLicense();

  /// has license
  bool HasLicense() const { return !m_sContextID.empty(); }
  
  /// logs a user into the system
  bool Login();
  
  /// logs a user into the system
  bool Login( const std::string & username, const std::string & password, const std::string & context  = "" );


  /// Renews a license
  bool Renew();

  /// Release the license
  void Release();

  /// Re-Authenticates a user
  void ReAuthenticate( const std::string & username, const std::string & password );

  /// Gets the context Id
  const std::string & GetContextId() const
  { return m_sContextID; }

public:

  /// time between license renewals
  static const int4 RenewalInterval = 10 * 60 * 1000;

  /// time between failed renewal retries
  static const int4 RenewalRetryInterval = 60 * 1000;

  /// number of retries of failed renewals
  static int4 m_iTimerRetries;
  static const int4 MaxNumberRetries = 20;

private:

  std::string m_sContextID;

}; // class License
