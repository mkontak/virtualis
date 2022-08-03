// $Id: SystemInfo.h,v 1.7 2006/06/01 19:05:39 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SystemInfo.h,v 1.7 2006/06/01 19:05:39 mkontak Exp $
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

// Namespaces
using namespace System;

// Namespaces
OPEN_UTILITIES_NS

public ref struct DiskSpace
{
  DiskSpace() : TotalBytes(0), TotalGbs(0.0F), FreeBytes(0), FreeGbs(0.0F), UsedBytes(0), UsedGbs(0), PercentUsed(0.0F), PercentFree(0.0F) { }
  __int64 TotalBytes;
  float TotalGbs;
  __int64 FreeBytes;
  float FreeGbs;
  __int64 UsedBytes;
  float UsedGbs;
  float PercentUsed;
  float PercentFree;
};

/**
 * System Information Class Implementation
 */
public ref class SystemInfo
{
public:
  /// Constructor
  SystemInfo(void) { }

  /// Destructor
  virtual ~SystemInfo(void) { }

  /// Returns the amount of free space on the drive 
  static DiskSpace ^ GetDiskSpace(String ^ sDrive);

  /// Returns the amount of free space on the drive 
  static __int64 GetAvailableDiskSpace(String ^ sDrive);

  /// Returns the amount of free space on the drive 
  static __int64 GetTotalDiskSpace(String ^ sDrive);

  ///  Checks the disk thresholds 
  static void CheckDiskThresholds(String ^ sPath, int iDiskWarningThreshold, int iDiskErrorThreshold);

  /// Returns the current thread id
  static int GetCurrentThreadId() { return System::Threading::Thread::CurrentThread->ManagedThreadId; }

private:

  /// Convertion value for GB
  static __int64 m_iDivisorGB = 1073741824;

};  // class SystemInfo


CLOSE_UTILITIES_NS

