// $Id: Locker.h,v 1.2 2004/03/29 21:57:05 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Locker.h,v 1.2 2004/03/29 21:57:05 mkontak Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com
//

// Pragmas
#pragma once

// Includes
#include <basetsd.h>
#include <certcli.h>
#include <windef.h>
#include <winbase.h>
#include <windows.h>


// Namespaces
using namespace System;

OPEN_UTILITIES_NS

/**
  * Implements study location locking as well as dataset locking.
  */
__sealed public __gc class Locker
{
public:

  /// Constructor (Locks a dataset)
  Locker(String * sPath, String * sPrefix, int iExpirationTimeInSeconds);

  /// Constructor (Locks a study)
  Locker(String * sPath, int iExpirationTimeInSeconds);

  /// Destructors
  ~Locker();

  /// Sets the lock on the members
  bool SetLock() { return(SetLock(m_sPath, m_sPrefix, m_iExpirationTimeInSeconds)); }

  /// Sets lock on the study path specified 
  inline static bool SetLock(String * sStudyPath, int iExpirationTime)
  { return Locker::SetLock(sStudyPath, System::String::Empty, iExpirationTime);  }

  /// Sets lock on a dataset 
  static bool SetLock(String * sStudyPath, String * sDatasetPrefix, int iExpirationTimeInSeconds);

  /// Clears lock on the members
  inline void ClearLock() 
  { Locker::ClearLock(m_sPath, m_sPrefix); }

  /// Clears a study lock
  inline static void ClearLock(String * sStudyPath)
  { Locker::ClearLock(sStudyPath, System::String::Empty); } 


  /// Clears a datset lock
  static void ClearLock(String * sStudyPath, String * sDatasetPrefix);

  /// Returns true if the study or the dataset is locked (Used against the members)
  inline bool IsLocked() 
  { return IsLocked(m_sPath, m_sPrefix); }

  /// Returns true of the study is locked
  inline static bool IsLocked(String * sStudyPath)
  { return Locker::IsLocked(sStudyPath, String::Empty); } 

  /// Returns true if the study or the dataset is locked
  static bool IsLocked(String * sStudyPath, String * sDatasetPrefix);


private:    // Private Methods

  /// Builds the Lock File Name with the full path
  static String * BuildLockFileName(String * sStudyPath, String * sDatasetPrefix);

  /// Returns the System Time regardless of whether the system is local or remote
  static System::DateTime GetSysTime( String * sSystemName );

  /// Reads the expiration time
  static int ReadExpirationTime(String * sLockFile);

  /// Writes the expiration time
  static void WriteExpirationTime(String * sLockFile, int iExpirationTimeInSeconds);

private:    // Private Members

  /// Location lock file name
  static String * m_sLockFileName = "Study.lck";

  /// Path to be locked
  String * m_sPath;

  /// Dataset prefix to be locked
  String * m_sPrefix;

  /// Expiration time
  int m_iExpirationTimeInSeconds;

};  // class Locker

CLOSE_UTILITIES_NS



// Revision History:
// $Log: Locker.h,v $
// Revision 1.2  2004/03/29 21:57:05  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.10  2004/02/09 21:36:06  mkontak
// no message
//
// Revision 1.9  2003/06/20 15:09:36  mkontak
// Coding standards
//
// Revision 1.8  2003/04/22 14:00:21  mkontak
// Used #pragma once
//
// Revision 1.7  2003/04/15 14:10:38  mkontak
// Re-Write of CDR
//
// Revision 1.6  2002/11/25 22:03:23  mkontak
// Made backward compatible
//
// Revision 1.5  2002/11/07 21:33:01  mkontak
// Changed lcoation.lck to study.lck
//
// Revision 1.4  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// Revision 1.3  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Locker.h,v 1.2 2004/03/29 21:57:05 mkontak Exp $
// $Id: Locker.h,v 1.2 2004/03/29 21:57:05 mkontak Exp $
