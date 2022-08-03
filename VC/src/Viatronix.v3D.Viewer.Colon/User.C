// $Id: User.C,v 1.6.2.1 2007/10/30 17:49:54 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#include "StdAfx.h"
#include "User.h"
#include <map>
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxDOMNodeList.h"
#include "vxDOMNode.h"


//// Static one-and-only service user account information
#define S_SERVICE_USERNAME    "nelomfendbgeejlkai"    // Encrypted Readable "vxservice"
#define S_SERVICE_USERPASS    "gokdnkafnmnfokcdef"    // Encrypted Readable "vicon2000"
#define S_SERVICE_FULLNAME    "Service User"          // Service user fullname

/**
 * Constructor
 */
User::User() : 
  m_sLogin( S_SERVICE_USERNAME ),
  m_sFullname( S_SERVICE_FULLNAME ),
  m_sRole( "Service" ),
  m_iRightsMask( 0xffffffff ),
  m_sSettingsPath( "C:\\Documents and Settings\\All Users\\Application Data\\Viatronix\\vxservice" )
{
} // User()


/**
 * Destructor
 */
User::~User()
{
} // ~User()


/**
 * Determines if a right is allowed for this user.
 *
 * @param     sRight        A users right
 * @return                  true if the user has the right; otherwise false
 */
bool User::IsAllowed( const std::string & sRight ) 
{

  if ( GetType() == UserType_Service )
    return true;

  std::map< std::string, int4 >::const_iterator iter = m_rights.find( tolower(sRight) );
  if( iter == m_rights.end() )
    return false;

  return ( iter->second & m_iRightsMask ) ? true : false;
} // IsAllowed( sRight )


/**
 * Gets the usder type as a enum
 */
User::UserTypeEnum User::GetType()
{
  std::string sRole(tolower(m_sRole));

  if ( strcmp(sRole.c_str(),"administrator") == 0 )
    return UserType_Administrator;
  else if ( strcmp(sRole.c_str(),"service") == 0 )
    return UserType_Service;
  else if ( strcmp(sRole.c_str(),"physician") == 0 )
    return UserType_Physician;
  else if ( strcmp(sRole.c_str(),"operator") == 0 )
    return UserType_Operator;
  else if ( strcmp(sRole.c_str(),"op-admin") == 0 )
    return UserType_Administrator;
  else if ( strcmp(sRole.c_str(),"phy-admin") == 0 )
    return UserType_Administrator;
  else 
    return UserType_Unknown;

}  // GetType()



/**
 * Gets the current user
 */
User & User::GetCurrentUser()
{
  static User currentUser;
  return currentUser;
} // GetCurrentUser()


// $Log: User.C,v $
// Revision 1.6.2.1  2007/10/30 17:49:54  jmeade
// Issue 5848: get service login and fullname from console.
//
// Revision 1.6  2007/03/09 00:11:04  jmeade
// code standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/User.C,v 1.6.2.1 2007/10/30 17:49:54 jmeade Exp $
// $Id: User.C,v 1.6.2.1 2007/10/30 17:49:54 jmeade Exp $
