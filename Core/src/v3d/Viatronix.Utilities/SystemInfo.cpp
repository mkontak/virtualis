// $Id: SystemInfo.cpp,v 1.9.12.1 2011/05/18 01:56:58 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SystemInfo.cpp,v 1.9.12.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com
//

// Includes
#include "StdAfx.h"
#include "SystemInfo.h"


// Usings
using namespace Viatronix::Logging;

// Namespaces
USING_UTILITIES_NS

/**
 * Returns the total disk space for the root directory specified-
 *
 * If the device is a full path it will attempt to retieve the root before 
 * calling the WIN32 GetDiskFreeSpaceEx()
 *
 * @param String ^ sPath     The path to be used iin retreiving the available disk space.
 *
 * @return  The amount of available space in bytes
 *
 */
DiskSpace ^ SystemInfo::GetDiskSpace(String ^ sPath)
{

  DiskSpace ^ pDiskSpace = gcnew DiskSpace();


  if ( String::Compare(sPath,L"") != 0 && sPath->Length >= 2 )
  {

    String ^ sRoot = String::Concat(System::IO::Path::GetPathRoot(sPath), System::Convert::ToString(System::IO::Path::DirectorySeparatorChar));
    DWORD iSectorsPerCluster, iBytesPerSector, iNumberOfFreeClusters, iTotalNumberOfClusters;

    // 
    if ( GetDiskFreeSpace(GetUnManagedStdString(sRoot).c_str(), &iSectorsPerCluster, &iBytesPerSector, &iNumberOfFreeClusters, &iTotalNumberOfClusters) )
    {

      pDiskSpace->TotalBytes  = (__int64)iSectorsPerCluster * (__int64)iTotalNumberOfClusters * (__int64)iBytesPerSector;
      pDiskSpace->TotalGbs    = (float)((double)pDiskSpace->TotalBytes / (double)m_iDivisorGB);
      pDiskSpace->FreeBytes   = (__int64)iSectorsPerCluster * (__int64)iNumberOfFreeClusters * (__int64)iBytesPerSector;
      pDiskSpace->FreeGbs     = (float)((double)pDiskSpace->FreeBytes / (double)m_iDivisorGB);
      pDiskSpace->UsedBytes   = pDiskSpace->TotalBytes - pDiskSpace->FreeBytes;
      pDiskSpace->UsedGbs     = (float)((double)pDiskSpace->UsedBytes / (double)m_iDivisorGB);
      pDiskSpace->PercentUsed = (float)((double)pDiskSpace->UsedBytes / (double)pDiskSpace->TotalBytes) * 100.0F;
      pDiskSpace->PercentFree = (float)((double)pDiskSpace->FreeBytes / (double)pDiskSpace->TotalBytes) * 100.0F;

    } // END ... If the getDiskFreeSpace() was successful
    else
    {

      ////////////////////////////////////
      // Throw a meaningful error
      //////////////////////////////////
      DWORD dwError = GetLastError();
      switch (dwError)
      {
      case ERROR_ACCESS_DENIED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access path ",sPath)));
        break;
      case ERROR_BAD_NETPATH:
      case ERROR_NETNAME_DELETED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access network path ",sPath)));
        break;
      case ERROR_BAD_NET_NAME:
        throw(gcnew System::IO::IOException(String::Concat(L"Invalid network/share name ", sPath)));
        break;        
      case ERROR_REQ_NOT_ACCEP:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not connect to share ", sPath)));
        break;
      default:
        throw(gcnew System::IO::IOException(String::Concat(L"Error checking disk space [", Convert::ToString((int)dwError), L"]"))); 
        break;
      }
    } // END ... If the GetDiskFreSpaceEx() failed

  } // END ... If the path was specfied and is valid 
  

  return pDiskSpace;

} // GetAvaiableDiskSpace()

/**
 * Returns the total disk space for the root directory specified.
 *
 * If the device is a full path it will attempt to retieve the root before 
 * calling the WIN32 GetDiskFreeSpaceEx()
 *
 * @param String ^ sPath     The path to be used iin retreiving the available disk space.
 *
 * @return  The amount of available space in bytes
 *
 */
__int64 SystemInfo::GetTotalDiskSpace(String ^ sPath)
{

  __int64 iTotalSpace(0);

  if ( String::Compare(sPath,L"") != 0 && sPath->Length >= 2 )
  {

    String ^ sRoot = String::Concat(System::IO::Path::GetPathRoot(sPath), System::Convert::ToString(System::IO::Path::DirectorySeparatorChar));
    DWORD iSectorsPerCluster, iBytesPerSector, iNumberOfFreeClusters, iTotalNumberOfClusters;

    // 
    if ( GetDiskFreeSpace(GetUnManagedStdString(sRoot).c_str(), &iSectorsPerCluster, &iBytesPerSector, &iNumberOfFreeClusters, &iTotalNumberOfClusters) )
    {

      iTotalSpace = (__int64)iSectorsPerCluster * (__int64)iTotalNumberOfClusters * (__int64)iBytesPerSector;

    } // END ... If the getDiskFreeSpace() was successful
    else
    {

      ////////////////////////////////////
      // Throw a meaningful error
      //////////////////////////////////
      DWORD dwError = GetLastError();
      switch (dwError)
      {
      case ERROR_ACCESS_DENIED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access path ",sPath)));
        break;
      case ERROR_BAD_NETPATH:
      case ERROR_NETNAME_DELETED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access network path ",sPath)));
        break;
      case ERROR_BAD_NET_NAME:
        throw(gcnew System::IO::IOException(String::Concat(L"Invalid network/share name ", sPath)));
        break;        
      case ERROR_REQ_NOT_ACCEP:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not connect to share ", sPath)));
        break;
      default:
        throw(gcnew System::IO::IOException(String::Concat(L"Error checking disk space [", Convert::ToString((int)dwError), L"]"))); 
        break;
      }
    } // END ... If the GetDiskFreSpaceEx() failed

  } // END ... If the path was specfied and is valid 
  

  return(iTotalSpace);

} // GetAvaiableDiskSpace()

/**
 * Returns the number of available bytes on the disk device specified.
 *
 * If the device is a full path it will attempt to retieve the root before 
 * calling the WIN32 GetDiskFreeSpaceEx()
 *
 * @param String ^ sPath     The path to be used iin retreiving the available disk space.
 *
 * @return  The amount of available space in bytes
 *
 */
__int64 SystemInfo::GetAvailableDiskSpace(String ^ sPath)
{

  __int64 iFreeSpace(0);

  if ( String::Compare(sPath,L"") != 0 && sPath->Length >= 2 )
  {


    String ^ sRoot = String::Concat(System::IO::Path::GetPathRoot(sPath), System::Convert::ToString(System::IO::Path::DirectorySeparatorChar));
    DWORD iSectorsPerCluster, iBytesPerSector, iNumberOfFreeClusters, iTotalNumberOfClusters;
    // 
    if ( GetDiskFreeSpace(GetUnManagedStdString(sRoot).c_str(), &iSectorsPerCluster, &iBytesPerSector, &iNumberOfFreeClusters, &iTotalNumberOfClusters) )
    {

      iFreeSpace = (__int64)iSectorsPerCluster * (__int64)iNumberOfFreeClusters * (__int64)iBytesPerSector;

    } // END ... If the getDiskFreeSpace() was successful
    else
    {

      ////////////////////////////////////
      // Throw a meaningful error
      //////////////////////////////////
      DWORD dwError = GetLastError();
      switch (dwError)
      {
      case ERROR_ACCESS_DENIED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access path ",sPath)));
        break;
      case ERROR_BAD_NETPATH:
      case ERROR_NETNAME_DELETED:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not access network path ",sPath)));
        break;
      case ERROR_BAD_NET_NAME:
        throw(gcnew System::IO::IOException(String::Concat(L"Invalid network/share name ", sPath)));
        break;        
      case ERROR_REQ_NOT_ACCEP:
        throw(gcnew System::IO::IOException(String::Concat(L"Could not connect to share ", sPath)));
        break;
      default:
        throw(gcnew System::IO::IOException(String::Concat(L"Error checking disk space [", Convert::ToString((int)dwError), L"]"))); 
        break;
      }
    } // END ... If the GetDiskFreSpaceEx() failed

  } // END ... If the path was specfied and is valid 
  

  return(iFreeSpace);

} // GetAvaiableDiskSpace()


/**
 * Checks the disk thresholds 
 *
 * @param sPath                   Path to be checked for disk availability
 * @param iDiskWarningThreshold   % of disk available before a warning is generated
 * @param iDiskErrorThreshold     % of disk available before an error is generated
 *
 */
void SystemInfo::CheckDiskThresholds(String ^ sPath, int iDiskWarningThreshold, int iDiskErrorThreshold )
{

  Log::Information("Checking disk threshold", "SystemInfo", "CheckDiskThreshold");

  // Get the disk space
  DiskSpace ^ pDiskSpace = GetDiskSpace(sPath);

  /////////////////////////////////////////////////////////////////
  // Check the disk space against the error threshold and the 
  // warning threshold.
  ////////////////////////////////////////////////////////////////
  if ( pDiskSpace->PercentUsed >= iDiskErrorThreshold )
  {
    Log::Error("NOT ENOUGH DISK SPACE TO PROCESS TASK, PLEASE CLEANUP", "SystemInfo", "CheckDiskThresholds");
    throw gcnew System::IO::IOException("Out of disk space");
  } // END ... If the disk space <= the disk error threshold
  else if ( pDiskSpace->PercentUsed >= iDiskWarningThreshold )
  {
    Log::Warning("Disk space is getting low, please cleanup", "SystemInfo", "CheckDiskThresholds");
  } // END ... Else if the disk space <= the disk warning threshold

} // CheckDiskThresholds(String ^ sPath, int iDiskWarningThreshold, int iDiskErrorThreshold )


// $Log: SystemInfo.cpp,v $
// Revision 1.9.12.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9  2005/08/29 12:37:51  mkontak
// Added CheckDiskThreadholds()
//
// Revision 1.8  2005/03/23 19:17:20  mkontak
// Fixed GetDiskSpace
//
// Revision 1.7  2005/02/22 14:23:33  mkontak
// Added percentages to DiskSpace struct
//
// Revision 1.6  2004/07/21 17:25:38  mkontak
// Fix
//
// Revision 1.5  2004/07/20 13:31:45  mkontak
// no message
//
// Revision 1.4  2004/07/19 20:42:35  mkontak
// Fixed systeminfo
//
// Revision 1.3  2004/07/19 18:47:32  mkontak
// Fixed systeminfo
//
// Revision 1.2  2004/07/19 18:36:03  mkontak
// Fixed systeminfo
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/16 20:05:58  mkontak
// Fixed SystemInfo
//
// Revision 1.4  2003/07/15 14:49:39  mkontak
// Coding standards
//
// Revision 1.3  2003/06/20 18:33:09  mkontak
// Coding standards
//
// Revision 1.2  2002/11/25 22:22:11  mkontak
// Fixed memory leak
//
// Revision 1.1  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SystemInfo.cpp,v 1.9.12.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: SystemInfo.cpp,v 1.9.12.1 2011/05/18 01:56:58 mkontak Exp $
//