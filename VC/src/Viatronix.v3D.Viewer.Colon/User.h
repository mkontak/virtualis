// $Id: User.h,v 1.4 2007/03/08 18:34:09 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#pragma once


// includes
#include <map>


// forward declaration
class vxDOMElement;


// class declaration
class User
{
// enums
public:

  /// user type
  typedef enum UserTypeEnum
  {
    UserType_Unknown       = 000,
    UserType_Administrator = 100,
    UserType_Service       = 200,
    UserType_Physician     = 300,
    UserType_Operator      = 400,
    //UserType_System      = 500,
  } UserTypeEnum;

// member functions
public:
  
  /// constructor
  User();
  
  /// destructor
  virtual ~User();

  /// gets the login 
  const std::string & GetLogin() const { return m_sLogin; }

  /// sets the login
  void SetLogin( const std::string & sLogin ) { m_sLogin = sLogin; }

  /// gets the full name
  const std::string & GetFullName() const { return m_sFullname; }

  /// sets the full name
  void SetFullName( const std::string & sFullname ) { m_sFullname = sFullname; }

  /// gets the user role
  const std::string &  GetRole() const { return m_sRole; }

  /// sets the users role
  void SetRole(const std::string & sRole)
  { m_sRole = sRole; }

  /// Gets the type/role as a enum
  UserTypeEnum GetType();

  /// Gets the rights mask
  int4 GetRightsMask() const
  { return m_iRightsMask; }

  /// Sets the rights mask
  void SetRightsMask(int4 iMask) 
  { m_iRightsMask = iMask; }

  /// gets the settings path
  const std::string & GetSettingsPath() const { return m_sSettingsPath; }

  /// sets the settings path
  void SetSettingsPath( const std::string & sSettingsPath ) { m_sSettingsPath = sSettingsPath; }

  /// determines if a user has a particular right
  bool IsAllowed( const std::string & sRight );

  /// gets the current user
  static User & GetCurrentUser();

  /// Adds right
  void AddRight(const std::string & sName, int4 mask )
  { if ( ! sName.empty() ) m_rights.insert(std::pair<std::string,int4>(sName, mask)); }

// member variables
private:

  /// users login (encrypted)
  std::string m_sLogin;

  /// users full name
  std::string m_sFullname;

  /// users role/type
  std::string m_sRole;

  /// users settings path
  std::string m_sSettingsPath;

  /// mapping of rights to bitmasks
  std::map< std::string, int4 > m_rights;

  /// users rights mask
  int4 m_iRightsMask;

  
}; // class User

// $Log: User.h,v $
// Revision 1.4  2007/03/08 18:34:09  jmeade
// code standards.
//
// $Id: User.h,v 1.4 2007/03/08 18:34:09 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/User.h,v 1.4 2007/03/08 18:34:09 jmeade Exp $
