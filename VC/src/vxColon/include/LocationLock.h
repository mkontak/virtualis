// $Id: LocationLock.h,v 1.2 2006/01/31 14:04:26 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak (mkontak@viatronix.com)


// pragmas
#pragma once


class  LocationLock  
{
public:

  /// Constructor
  LocationLock(const std::string & sStudyPath, const std::string & sDatasetPrefix = "", uint4 uExpiratiionTime = 0);
  
  /// Destructor
	virtual ~LocationLock();
  
  /// Set a lock on the study in the specified path
  static bool SetLock(const std::string & sStudyPath, uint4 uExpirationTime, const std::string & sDatasetPrefix = "");
  bool  SetLock() { return(SetLock(m_sStudyPath,m_uExpirationTime)); }

  /// Returns true if the study in the path specified is locked
  static bool IsLocked(const std::string & sStudyPath, const std::string & sDatasetPrefix = "");
  bool IsLocked() { return(IsLocked(m_sStudyPath)); }

  /// Clears the lock of the study
  static void ClearLock(const std::string & sStudyPath, const std::string & sDatasetPrefix = "");
  void ClearLock() { ClearLock(m_sStudyPath); }
  
  ///
  void SetExpiratiionTime(uint4 uExpirationTime) { m_uExpirationTime = uExpirationTime; }

private:

  /// Retrieves the remote time
  static void GetSysTime(std::string & sSystemName, LPSYSTEMTIME lpSysTime);

  /// Returns the time in seconds
  static uint8 GetTimeInSeconds(LPSYSTEMTIME lpSysTime)
  {
    uint8 uSeconds = (lpSysTime->wHour * 3600) + (lpSysTime->wMinute * 60) + (lpSysTime->wSecond);
    return(uSeconds);
  }
  
  std::string m_sStudyPath;
  std::string m_sDatasetPrefix;
  uint4 m_uExpirationTime;

  static const std::string m_sLockFileName;


};  // class LocationLock


// $Log: LocationLock.h,v $
// Revision 1.2  2006/01/31 14:04:26  frank
// code review
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.6  2002/07/18 15:11:55  mkontak
// Add VXDLL to class definitions
//
// Revision 1.5  2002/07/18 14:17:58  mkontak
// Added dataset locking capabilities
//
// Revision 1.4  2002/03/11 16:02:03  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 1.3  2002/03/11 15:19:45  mkontak
// Merged VC 1-2/CS-1 into main trunk
//
// Revision 1.2.2.1  2002/02/26 01:20:53  jmeade
// Fixed calculation error in GetTimeInSeconds().
//
// Revision 1.2.2.1  2002/02/26 01:20:53  jmeade
// Fixed calculation error in GetTimeInSeconds().
//
// Revision 1.2  2001/11/21 16:35:19  mkontak
// Merge 11/21/2001
//
// Revision 1.1.2.3  2001/11/14 14:11:02  mkontak
// Fix return code for SetLock()
//
// Revision 1.1.2.2  2001/11/12 19:11:03  mkontak
// Added functinality to get remote system time.
//
// Revision 1.1.2.1  2001/11/12 05:40:01  mkontak
// Initial check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/LocationLock.h,v 1.2 2006/01/31 14:04:26 frank Exp $
// $Id: LocationLock.h,v 1.2 2006/01/31 14:04:26 frank Exp $
