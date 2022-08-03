// $Id: SystemInfo.C,v 1.6 2006/02/02 20:01:46 geconomos Exp $
//
// Copyright © 2001,2002 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Soeren Grimm (soeren@viatronix.com)


// includes
#include "stdafx.h"
#include "SystemInfo.h"
#include "utilities.h" 
#include "Date.h"

#include <TlHelp32.h>

// namespaces
using namespace std;


// static member initializations
SYSTEM_INFO SystemInfo::m_systemInfo;
MEMORYSTATUS SystemInfo::m_memoryStatus;
bool SystemInfo::m_bStaticInitDummy = SystemInfo::Init();
CpuInfo SystemInfo::m_cpuInfo;


// defines
#define FILE_REVISION "$Revision: 1.6 $"



/**
 * Returns the computer name
 * @return computer name string
 */
std::string SystemInfo::GetNetBiosComputerName()
{
  DWORD dwSize(512);
  char szName[513];
  GetComputerName(szName, &dwSize);
  std::string sComputerName(szName);
  return(toupper(sComputerName));
} // GetNetBiosComputerName()




/**
 * This functions initializes all the static member variables.
 * @return success
 */
bool SystemInfo::Init()
{

#ifdef _WIN32
  GetSystemInfo(&m_systemInfo);
  GlobalMemoryStatus(&m_memoryStatus);
#endif
  return 1;

} // Init()




/**
 * Returns the total size of of physical memory
 * @return size
 */
uint8 SystemInfo::GetTotalPhysicalMemory()
{

  #ifdef _WIN32
    return (uint8)m_memoryStatus.dwTotalPhys;
  #else
    return 0;
  #endif

} // GetTotalPhysicalMemory()


/**
 * Returns the current available amount of physical memory
 * @return size
 */
uint8 SystemInfo::GetAvailablePhysicalMemory()
{

  #ifdef _WIN32
    GlobalMemoryStatus(&m_memoryStatus);
    return (uint8)m_memoryStatus.dwAvailPhys;
  #else
    return 0;
  #endif

} // GetAvailablePhysicalMemory()


/**
 * Returns the total size of the pagefile
 * @return size
 */
uint8 SystemInfo::GetTotalPageFile()
{

  #ifdef _WIN32
    return (uint8)m_memoryStatus.dwTotalPageFile;
  #else
    return 0;
  #endif

} // GetTotalPageFile()


/**
 * Returns the current available amount of the pagefile
 * @return size
 */
uint8 SystemInfo::GetAvailablePageFile()
{

  #ifdef _WIN32
    GlobalMemoryStatus(&m_memoryStatus);
    return (uint8)m_memoryStatus.dwAvailPageFile;
  #else
    return 0;
  #endif

} // GetAvailablePageFile()


/**
 * Returns the total size of virtual memory
 * @return size
 */
uint8 SystemInfo::GetTotalVirtualMemory()
{

  #ifdef _WIN32
    return (uint8)m_memoryStatus.dwTotalVirtual;
  #else
    return 0;
  #endif

} // GetTotalVirtualMemory()


/**
 * Returns the current available amount of virtual memory
 * @return size
 */
uint8 SystemInfo::GetAvailableVirtualMemory()
{

  #ifdef _WIN32
    GlobalMemoryStatus(&m_memoryStatus);
    return (uint8)m_memoryStatus.dwAvailVirtual;
  #else
    return 0;
  #endif

} // GetAvailableVirtualMemory()


/**
 * Determines of the process is already running
 */
const uint4 SystemInfo::GetProcessCount(const std::string & sApplicationName)
{

    uint4 uCount(0);
    std::string sUpperApp(toupper(sApplicationName));

    HANDLE handle = INVALID_HANDLE_VALUE;


    SCOPE_EXIT
    { 
      if ( handle != INVALID_HANDLE_VALUE )
        CloseHandle(handle);
    };

    try
    {
        // Create snapshot of the processes
        handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if( handle == INVALID_HANDLE_VALUE )
           throw ex::RuntimeException(LogRec(util::Strings::Format("Failed to take process table snapshot (%d)", GetLastError()), "SystemInfo", "IsProcessRunning"));

         PROCESSENTRY32 info;

        // Get the first process
        if ( !Process32First(handle, &info)  )
            throw ex::RuntimeException(LogRec(util::Strings::Format("Failed to get the first procee from list (%d)", GetLastError()), "SystemInfo", "IsProcessRunning"));
            

        // While there's another process, retrieve it
        do
        {
          if ( toupper(info.szExeFile) == sUpperApp )
            uCount++;
          
        } while ( Process32Next(handle, &info) );
    }
    catch ( ex::VException )
    {
      throw;
    }
    catch( ... )
    {
      throw ex::RuntimeException(LogRec(util::Strings::Format("Failed to check for process : unspecified", GetLastError()), "SystemInfo", "IsProcessRunning"));
    }

    return uCount;
}

/**
 * Returns the module name 
 */
std::string SystemInfo::GetModuleName() 
{
  std::vector<char> executablePath(MAX_PATH);

  // Try to get the executable path with a buffer of MAX_PATH characters.
  DWORD result = ::GetModuleFileNameA( nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size()));

  // As long the function returns the buffer size, it is indicating that the buffer
  // was too small. Keep enlarging the buffer by a factor of 2 until it fits.
  while (result == executablePath.size()) 
  {
    executablePath.resize(executablePath.size() * 2);
    result = ::GetModuleFileNameA(nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size()));
  }

  // If the function returned 0, something went wrong
  if (result == 0) {
    throw std::runtime_error("GetModuleFileName() failed");
  }

  // We've got the path, construct a standard string from it
  std::vector<std::string> folders = util::IO::SplitPath(std::string(executablePath.begin(), executablePath.begin() + result));

  return folders[util::IO::FILE];

}

// $Log: SystemInfo.C,v $
// Revision 1.6  2006/02/02 20:01:46  geconomos
// updated for changes to SystemInfo
//
// Revision 1.5  2005/07/01 12:30:07  vxconfig
// Added new Logger.
//
// Revision 1.4  2005/01/07 21:58:27  geconomos
// added method to return physical processors for ht enabled chips
//
// Revision 1.3  2005/01/07 20:28:31  michael
// dawn, had (copy & paste) code snippet in there ... took it out
//
// Revision 1.2  2005/01/07 20:27:15  michael
// Added method to return the actual number of physical processors
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.14.2.1  2003/08/11 11:06:53  mkontak
// moved Date into vx namespace
//
// Revision 3.14  2003/05/16 13:08:20  frank
// code formatting
//
// Revision 3.13  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.12  2003/03/12 15:29:24  geconomos
// Added inclusion of WinSock.h
//
// Revision 3.11  2002/09/30 20:46:33  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.10  2002/03/26 23:01:35  ingmar
// added GetProcessorSerialNumber() function
//
// Revision 3.9  2002/03/22 16:53:58  ingmar
// changed format of GetProcessorSerialNumber function
//
// Revision 3.8  2002/03/22 16:49:12  ingmar
// added GetProcessorSerialNumber function
//
// Revision 3.7  2002/03/05 19:50:02  ingmar
// added missing return 0 that Intel C++ compiler complained about
//
// Revision 3.6  2002/02/28 18:12:50  geconomos
// Removal of QT.
//
// Revision 3.5  2002/01/03 17:44:13  mkontak
// Fixed GetIPAddress
//
// Revision 3.4  2001/12/31 20:54:17  mkontak
// Added GetIPAddress
//
// Revision 3.3  2001/12/29 00:10:23  jmeade
// Merge from 1.1 branch.
//
// Revision 3.2  2001/11/14 01:58:39  wenli
// Add area selection of plaques
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.4  2001/11/09 15:31:44  mkontak
// Additional error trapping on GetAvailableDiskSpace().
//
// Revision 3.0.2.3  2001/11/09 15:07:15  mkontak
// Added additional exception if we fail to get available disk space
//
// Revision 3.0.2.2  2001/11/09 15:06:34  mkontak
// Added additional exception if we fail to get available disk space
//
// Revision 3.0.2.1  2001/11/09 14:34:31  mkontak
// Fixed computer name length problem.
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.3   Oct 12 2001 11:55:54   mkontak
// Added error checking in the GetAvailavleDiskSpace to throw a proper IOException.Resolution for 1119: System Info for  unshared 'studies' Folder  is inappropriate.  [processor ]
// 
//    Rev 2.2   Oct 12 2001 11:52:56   mkontak
// Added error checking in the GetAvailavleDiskSpace to throw a proper IOException.
// 
//    Rev 2.1   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.25  2001/08/10 18:29:48  mkontak
// Changed GetDiskFreeSpace to GetDiskFreeSpaceEx so that large drives will
// report the correct number of free bytes.
//
// Revision 1.24  2001/07/03 14:56:49  soeren
// removed stdafx.c and .h
//
// Revision 1.23  2001/07/02 18:25:11  soeren
// changed something
//
// Revision 1.22  2001/07/02 18:00:39  soeren
// added GetCurrentDate
//
// Revision 1.21  2001/06/14 02:13:20  soeren
// working queue
//
// Revision 1.20  2001/06/06 15:15:34  soeren
// removed a bug
//
// Revision 1.19  2001/06/06 13:44:29  soeren
// fixed getDiskSize bug
//
// Revision 1.18  2001/05/21 13:31:34  soeren
// More Coding standard compliant
//
// Revision 1.17  2001/05/21 12:45:23  soeren
// More Coding standard compliant
//
// Revision 1.16  2001/05/07 22:57:12  soeren
// beautified
//
// Revision 1.15  2001/05/07 22:34:48  soeren
// rewrote the whole SystemInfo class
//
// Revision 1.14  2001/05/07 18:02:11  soeren
// removed some functions
//
// Revision 1.13  2001/05/07 15:18:43  dmitry
// System Information functionality extended.
//
// Revision 1.12  2001/04/09 18:02:56  dmitry
// Removed Revision History from Top of File
//
// Revision 1.11  2001/04/03 17:07:42  jmeade
// GetDiskSpace() and GetFreeDiskSpace(): accept a root directory,
// return a uint8 (now that we have that type)
//
// Revision 1.10  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.9  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.8  2001/02/05 21:45:31  sarang
// Added linux system calls to get information about the system under linux.
//
// Revision 1.7  2001/02/02 09:06:29  jeff
// SystemInfo functions made static to ease accessibility, and also since no info
// is instance specific (i.e. system information)
//
// Revision 1.6  2001/02/02 09:01:43  jeff
// Not sure why SystemInfo had to be overhauled, but in any case,
// GetMemoryStatus() returns nothing if MemoryStatus class has no copy operator(s)
//
// Revision 1.5  2001/01/22 22:27:45  dave
// attemted to compile in UNIX
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
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/SystemInfo.C,v 1.6 2006/02/02 20:01:46 geconomos Exp $
// $Id: SystemInfo.C,v 1.6 2006/02/02 20:01:46 geconomos Exp $