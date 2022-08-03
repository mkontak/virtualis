// $Id: ColonLicense.C,v 1.2.2.7 2010/09/21 20:44:18 dongqing Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


#include "StdAfx.h"
#include "ColonLicense.h"
#include "ViatronSplash.h"
#include "User.h"
#include "ServiceLocator.h"


// globals
int4 ColonLicense::m_iTimerRetries(0);


std::string Format( const std::string & username, const std::string & pwd, const std::string & id, bool release = false );


/**
 * Constructor
 */
ColonLicense::ColonLicense()
{
}


/**
 * Destructor
 */
ColonLicense::~ColonLicense()
{
  Release();
}


bool ColonLicense::Login()
{
#ifndef BYPASS_AUTHENTICATION

  bool bLoginSuccessful( false );

  // attempts to login to the system when OK button is clicked
  ColonViews::Dialogs::ViatronSplash::FunctionPtr okPtr = [&]( ColonViews::Dialogs::ViatronSplash & dlg ) -> bool
  {
    try
    {

      std::string sUser(dlg.GetUsername());

      LogDbg(util::Strings::Format("Attempting to login ( user: %s )",sUser.c_str()), "ColonLicense", "Login" );

      m_sContextID = ServiceLocator::GetLoginService()->Login( sUser, "", dlg.GetPassword() );

      bLoginSuccessful = true;

      return true;

    }
    catch( ex::VException & e )
    {
      LogDbg( "Login Failed [ERROR=" + e.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Login" );

      // prompt user to try again and reset the dialog
      std::stringstream builder;
      builder << "Login Failed" << std::endl << "Do you want to try again?";

      dlg.Reset();
      m_sContextID = "";
      return ( AfxMessageBox( builder.str().c_str(), MB_YESNO ) != IDYES );
    }
  };

  // prompt for confirmation before closing
  ColonViews::Dialogs::ViatronSplash::FunctionPtr cancelPtr = []( ColonViews::Dialogs::ViatronSplash & dlg ) -> bool
  {
    return ( AfxMessageBox( "Are you sure you want to exit?", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES );
  };

  ColonViews::Dialogs::ViatronSplash dlg( true, okPtr, cancelPtr );
  return ( dlg.DoModal() == IDOK ) && bLoginSuccessful;

#else
  // bypass licensing
  return true;
#endif
}


bool ColonLicense::Login( const std::string & username, const std::string & password, const std::string & context)
{
#ifndef BYPASS_AUTHENTICATION

  bool bResult = false;

  try
  {

    LogDbg(util::Strings::Format("Attempting to login ( user: %s ) (id = %s)",username.c_str(), context.c_str()), "ColonLicense", "Login" );

    m_sContextID = ServiceLocator::GetLoginService()->Login( username, context, password );

    bResult = true;
  }
  catch( ex::IOException & )
  {
    m_sContextID = "";
    throw;
  }
  catch( ex::VException & ex )
  {
    m_sContextID = "";

    LogDbg( "Login Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Login" );

    std::stringstream builder;
    builder << "Unable to renew context [" << ex.GetLogRecord().GetTextMessage() << "]";
    LogDbg( builder.str(), "ColonLicense", "Login" );
  }

  return bResult;

#else

  // bypass licensing
  return true;

#endif
} // Login( username, password )



/**
 * Attempts to re-authenticate the user
 *
 * @param username      User
 * @param password     Users password
 */
void ColonLicense::ReAuthenticate( const std::string & username, const std::string & password )
{
#ifndef BYPASS_AUTHENTICATION

  try
  {
    ServiceLocator::GetLoginService()->Authenticate( m_sContextID, username, password );

  }
  catch( ex::VException & ex )
  {

    LogErr( "Re-Authentication Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Login" );

    throw;
  }


#else

  // bypass licensing
  return;

#endif
} // ReAuthenticate( username, id )



bool ColonLicense::Renew()
{
#ifndef BYPASS_AUTHENTICATION

  bool bResult = false;

  CString closeViewer;
  closeViewer.LoadString(IDS_CLOSE_VIEWER);

  try
  {
    ServiceLocator::GetLoginService()->Renew( m_sContextID );
    bResult = true;
  }
  catch ( ex::AccountExpiredException & ex )
  {
    m_sContextID = "";
    LogErr( "Renew Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Renew" );

    throw;
  }
  catch ( ex::LicenseExpiredException & ex )
  {
    m_sContextID = "";
    LogErr( "Renew Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Renew" );

    throw; 
  }
  catch( ex::RuntimeException & ex )
  {
    m_sContextID = "";

    LogErr( "Renew Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Renew" );

    throw;
  }

  return bResult;
  
#else

  // bypass licensing
  return true;

#endif
}


void ColonLicense::Release()
{
#ifndef BYPASS_AUTHENTICATION

  SCOPE_EXIT { m_sContextID = ""; };

  try
  {
    if( !m_sContextID.empty() )
      ServiceLocator::GetLoginService()->Logout( m_sContextID );
  }
  catch( ex::VException & ex )
  {
    LogErr( "Release Failed [ERROR=" + ex.GetLogRecord().GetTextMessage() + "]", "ColonLicense", "Release" );

  }

#endif
} // Release


std::string Format( const std::string & username, const std::string & pwd, const std::string & id, bool release )
{
  std::stringstream stream;
  stream << "<context ";
  if( !username.empty() )
    stream << "login=\"" << username << "\" ";
  if( !pwd.empty() )
    stream << "pwd=\"" << pwd << "\" ";
  if( !id.empty() )
    stream << "id=\"" << id << "\" ";
  if( release )
    stream << "release=\"true\" ";

  stream << "/>";

  return stream.str();
}