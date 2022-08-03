// $Id: SystemInfo.h,v 1.4 2006/02/02 20:02:05 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Soeren Grimm (soeren@viatronix.com)

// pragama declarations
#pragma once

// incldue declarations
#include "CpuInfo.h"

// class declaration
class VX_BASE_DLL SystemInfo
{ 
// member functions
public:

  /// constructor
  SystemInfo() {}

  /// returns the total size of of physical memory
  static uint8 GetTotalPhysicalMemory();

  /// returns the current available amount of physical memory
  static uint8 GetAvailablePhysicalMemory();

  /// returns the total size of the pagefile
  static uint8 GetTotalPageFile();

  /// returns the current available amount of the pagefile
  static uint8 GetAvailablePageFile();

  /// returns the total size of virtual memory
  static uint8 GetTotalVirtualMemory();

  /// returns the current available amount of virtual memory
  static uint8 GetAvailableVirtualMemory();

  /// returns the computer NetBios name
  static std::string GetNetBiosComputerName();

  /// gets the cpu info
  static CpuInfo & GetCpuInfo() { return m_cpuInfo; }

  /// Returns true if the process is running, false otherwsie
  static const uint4 GetProcessCount(const std::string & sApplicationName);

  // Get Module Name
  static std::string GetModuleName();

private:

  /// initializes all the static member variables
  static bool Init();

 
// memebr variables
private:

  /// used for initializing the static member variables
  static bool m_bStaticInitDummy;

  /// windows system information struct
  static SYSTEM_INFO m_systemInfo;

  /// windows memory information struct
  static MEMORYSTATUS m_memoryStatus;

  /// cpu information
  static CpuInfo m_cpuInfo;

}; // SystemInfo




// $Log: SystemInfo.h,v $
// Revision 1.4  2006/02/02 20:02:05  geconomos
// CpuInfo class added
//
// Revision 1.3  2005/01/07 21:58:27  geconomos
// added method to return physical processors for ht enabled chips
//
// Revision 1.2  2005/01/07 20:27:15  michael
// Added method to return the actual number of physical processors
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.5  2002/07/18 15:11:55  mkontak
// Add VXDLL to class definitions
//
// Revision 3.4  2002/03/26 23:01:35  ingmar
// added GetProcessorSerialNumber() function
//
// Revision 3.3  2002/03/22 16:49:12  ingmar
// added GetProcessorSerialNumber function
//
// Revision 3.2  2001/12/31 20:54:24  mkontak
// Added GetIPAddress
//
// Revision 3.1  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.23  2001/07/02 19:22:00  soeren
// Added Warning, that SystemInfo is not thread save
//
// Revision 1.22  2001/07/02 18:25:10  soeren
// changed something
//
// Revision 1.21  2001/07/02 18:00:39  soeren
// added GetCurrentDate
//
// Revision 1.20  2001/06/14 02:13:20  soeren
// working queue
//
// Revision 1.19  2001/06/06 14:16:14  soeren
// changed default values of GetTotal and GetAvailalbeDiskspace
//
// Revision 1.18  2001/06/05 17:08:27  soeren
// whatever
//
// Revision 1.17  2001/05/21 13:31:34  soeren
// More Coding standard compliant
//
// Revision 1.16  2001/05/21 12:45:23  soeren
// More Coding standard compliant
//
// Revision 1.15  2001/05/07 23:08:11  soeren
// added comment
//
// Revision 1.14  2001/05/07 22:34:48  soeren
// rewrote the whole SystemInfo class
//
// Revision 1.13  2001/05/07 18:02:11  soeren
// removed some functions
//
// Revision 1.12  2001/05/07 15:18:43  dmitry
// System Information functionality extended.
//
// Revision 1.11  2001/04/12 18:09:30  dmitry
// modified base class for documentation
//
// Revision 1.10  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.9  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.8  2001/04/03 17:07:42  jmeade
// GetDiskSpace() and GetFreeDiskSpace(): accept a root directory,
// return a uint8 (now that we have that type)
//
// Revision 1.7  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.6  2001/02/02 09:06:29  jeff
// SystemInfo functions made static to ease accessibility, and also since no info
// is instance specific (i.e. system information)
//
// Revision 1.5  2001/02/02 09:01:43  jeff
// Not sure why SystemInfo had to be overhauled, but in any case,
// GetMemoryStatus() returns nothing if MemoryStatus class has no copy operator(s)
//
// Revision 1.4  2001/01/18 16:33:08  dave
// changed SystemInfo to inline GetCPUCount()
//
// Revision 1.3  2001/01/18 16:17:39  dave
// added diskSpace functions
//
// Revision 1.2  2000/11/17 00:27:56  dave
// added VX header, footer, rm tabs, made compile in linux
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/SystemInfo.h,v 1.4 2006/02/02 20:02:05 geconomos Exp $
// $Id: SystemInfo.h,v 1.4 2006/02/02 20:02:05 geconomos Exp $

