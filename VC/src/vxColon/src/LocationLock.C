// $Id: LocationLock.C,v 1.4 2006/07/06 14:55:22 romy Exp $
//
// Copyright© (current year), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak mkontak@viatronix.com
//
// Complete history on bottom of file

#include "stdafx.h"
#include "LocationLock.h"
#include "File.h"
#include "SystemInfo.h"
#include "FileExtensions.h"
#include "utilities.h" 
#include <lm.h>
#include "Date.h"

#define FILE_REVISION "$REVISION 3.0$"

const std::string LocationLock::m_sLockFileName("Study.lck");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////
LocationLock::LocationLock(const std::string & sStudyPath, const std::string & sDatasetPrefix, uint4 uExpirationTime) : 
          m_sStudyPath(sStudyPath), m_sDatasetPrefix(sDatasetPrefix), m_uExpirationTime(uExpirationTime)
{

  SetLock();
}

LocationLock::~LocationLock()
{
  ClearLock();
}


/**
 * Sets the lock on the specified location path
 *
 * @param const std::string & sStudyPath      Study path to be locked
 * @param uint4               uExpirationTime Number of seconds the lock expires
 * @param const std::string & sDatasetPrefix  Prefix for dataset to be locked
 */
bool LocationLock::SetLock(const std::string & sStudyPath, uint4 uExpirationTime, const std::string & sDatasetPrefix /* = "" */)
{

  bool bResult(false);

  if ( sStudyPath.size() != 0 )
  {
    vx::File studyPath(sStudyPath);

    std::string sSystemName("");
    if ( studyPath.IsUNCPath() )
    {
      sSystemName = studyPath.GetUNCSystemName();
    }
    
    studyPath.ConvertUNCToIP();

    if ( studyPath.Exists() && studyPath.IsDirectory() )
    {

      /////////////////////////////////////////////////////
      // Set up lock file name based on study or dataset
      /////////////////////////////////////////////////////
      std::string sLockFile(studyPath.ToString() + vx::File::GetPathSeparator() + m_sLockFileName);
      if ( ! sDatasetPrefix.empty() )
      {
        const std::string sVolFile(studyPath.ToString() + vx::File::GetPathSeparator() + sDatasetPrefix + vx::VolumeExt);
        const std::string sEcvFile(studyPath.ToString() + vx::File::GetPathSeparator() + sDatasetPrefix + vx::ElecCleansedVolExt);

        /////////////////////////////////////////////////////////
        // Make sure that the study is not already locked
        /////////////////////////////////////////////////////////
        if ( LocationLock::IsLocked(sStudyPath) )
        {
          LogErr("The study is already locked", "LocationLock", "SetLock");
          return(false);
        }

        sLockFile = studyPath.ToString() + vx::File::GetPathSeparator() + sDatasetPrefix + vx::LockExt;

        ///////////////////////////////////////////////////
        // If there is not dataset to lock the dont
        ///////////////////////////////////////////////////
        if ( ! vx::File(sVolFile).Exists() && ! vx::File(sEcvFile).Exists() )
        {
          LogWrn("Could not lock dataset " + sDatasetPrefix + " in path " + sStudyPath + " : no vol or ecv file", "LocationLock", "SetLock");
          return(false);
        }

      }


#ifdef _WIN32

      try
      {
        if ( ! DeleteFile(sLockFile.c_str()) )
        {
          LogWrn("Could not delete lock file " + sLockFile + " [" + ToAscii(GetLastError()) + "]", "LocationLock", "SetLock");
        }
      }
      catch ( ... )
      {
        // Ignore
      }

      HANDLE hFile = CreateFile(sLockFile.c_str(),GENERIC_WRITE | GENERIC_READ, 
                                FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);

      if ( hFile != INVALID_HANDLE_VALUE )
      {

        try
        {
          DWORD dwBytesWritten;

          // Write the expiration seconds into the file
          std::string sBuf(ToAscii(uExpirationTime));

          if ( ! WriteFile(hFile,sBuf.c_str(),sBuf.size(),&dwBytesWritten,NULL) ) 
          {
            LogErr("Could not write out lock file " + sLockFile + " [" + ToAscii(GetLastError()) + "]", "LocationLock", "SetLock");
          }
        }
        catch ( ... )
        {
          // Ignore
        }

        // Close the file handle
        CloseHandle(hFile);

        bResult = true;

      }

      ///////////////////////////
      // Force the ARCHIVE off
      //////////////////////////
      DWORD dwAtt = GetFileAttributes(sLockFile.c_str());
      SetFileAttributes(sLockFile.c_str(), dwAtt & ~FILE_ATTRIBUTE_ARCHIVE );

#else
      FILE * fp;
      fopen_s( &fp, sLockFile.c_str(),"w+");
      if ( fp != (FILE * )NULL )
      {
        fprintf_s(fp,"%d",uExpirationTime);
        fclose(fp);
      }
#endif
    }
  }


  return(bResult);
  
}

/**
 * Clears the location lock.
 *
 * @param const std::string & sStudyPath      Study path to be cleared (unlocked)
 * @param const std::string & sDatasetPrefix  Dataset prefix  ( File Name Root )
 */
 void LocationLock::ClearLock(const std::string & sStudyPath, const std::string & sDatasetPrefix /* = "" */ )
{

  if ( sStudyPath.size() != 0 )
  {
    vx::File studyPath(sStudyPath);
    
    if ( studyPath.Exists() && studyPath.IsDirectory() )
    {
      std::string sLockFile(sStudyPath + vx::File::GetPathSeparator() + m_sLockFileName);

      if ( ! sDatasetPrefix.empty() )
      {
        sLockFile = sStudyPath + vx::File::GetPathSeparator() + sDatasetPrefix + vx::LockExt;
      } // END ... If we are to check for a dataset lock
      
      vx::File lockFile(sLockFile);

      if ( lockFile.Exists() )
      {
        if ( ! DeleteFile(sLockFile.c_str()) )
        {
          if ( ! lockFile.Delete() )
          {
            LogErr("Could not delete lock file " + sLockFile, "LocationLock", "ClearLock");
          }
        }
      }
      
    }
  }
  
} // ClearLock()


/**
* Returns true if the specified study path is locked
*
* @param const std::string & sStudyPath       Study path to check if locked
* @param const std::string & sdatasetPrefix   Dataset frefix to be used if we are to check for a dataset lock
*/
 bool LocationLock::IsLocked(const std::string & sStudyPath, const std::string & sDatasetPrefix /* = "" */)
{
  bool bIsLocked(false);

  LogTmp("Checking if " + sStudyPath + " is locked", "LocationLock", "IsLocked");
  
  if ( ! sStudyPath.empty() )
  {
    vx::File studyPath(sStudyPath);

    std::string sSystemName("");
    if ( studyPath.IsUNCPath() )
    {
      sSystemName = studyPath.GetUNCSystemName();
    }

    studyPath.ConvertUNCToIP();
    
    if ( studyPath.Exists() && studyPath.IsDirectory() )
    {
      std::string sLockFile(sStudyPath + vx::File::GetPathSeparator() + m_sLockFileName);

      if ( ! sDatasetPrefix.empty() )
      {
        /////////////////////////////////////////////////////////
        // Make sure that the study is not already locked
        /////////////////////////////////////////////////////////
        if ( LocationLock::IsLocked(sStudyPath) )
        {
          LogErr("The study is already locked", "LocationLock", "SetLock");
          return(true);
        }

        sLockFile = sStudyPath + vx::File::GetPathSeparator() + sDatasetPrefix + vx::LockExt;
      } // END ... If we are to check for a dataset lock
      
      

#ifdef _WIN32

      HANDLE hFile = CreateFile(sLockFile.c_str(), GENERIC_READ, 
                                FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  
      if ( hFile != INVALID_HANDLE_VALUE  )
      {
  
        FILETIME fileTime;
        if ( GetFileTime(hFile,NULL,NULL,&fileTime) )
        {

          TCHAR szBuf[100];
          DWORD dwBytesToRead = 100;
          DWORD dwBytesRead = 0;
          uint4 uExpirationTime(0);
          if ( ReadFile(hFile, szBuf, dwBytesToRead, &dwBytesRead,NULL ) )
          {
            szBuf[dwBytesRead] = '\0';
            uExpirationTime = atoi(szBuf);
          }
            
        
          SYSTEMTIME fileSysTime;
          LPSYSTEMTIME lpSysTime = &fileSysTime;

          FileTimeToSystemTime(&fileTime, lpSysTime);
          uint8 uFileTime = GetTimeInSeconds(&fileSysTime) + uExpirationTime;
          
          SYSTEMTIME sysTime;
          lpSysTime = &sysTime;
          if ( sSystemName.size() > 0  )
          {
            GetSysTime(sSystemName, lpSysTime);
          }
          else
          {
            GetSystemTime(lpSysTime);
          }

          uint8 uSysTime = GetTimeInSeconds(lpSysTime);
          
          vx::Date sysDate(lpSysTime->wYear, lpSysTime->wMonth, lpSysTime->wDay);
          vx::Date fileDate(fileSysTime.wYear, fileSysTime.wMonth, fileSysTime.wDay);
          
          // Must be the same day
          if ( vx::Date::DateDiff(sysDate,fileDate) == 0 )
          {

            LogTmp("System Time: " + ToAscii(uSysTime) + "  File Time: " + ToAscii(uFileTime), "LocationLock", "IsLocked");
            if ( uSysTime <= uFileTime )
            {
              bIsLocked = true;
            }

          } // END ... DateDiff()

        } // END ... If the GetFileTime() was successful

        CloseHandle(hFile);

      } // END ... If open file was successful
      else
      {
        DWORD dwStatus = GetLastError();
        bIsLocked = false;
      }

#else
      FILE * fp;
      fopen_s( &fp, sLockFile.c_str(),"r");
      if ( fp != (FILE * )NULL )
      {
        uint4 uExpirationTime(0); 
        fscanf_s( fp,"%d",&uExpirationTime);

        // iostat()
        fclose(fp);
            
      }
      else
      {
        bIsLocked = false;
      }
    
#endif
      
    } // END ...  If the study path exists and is a directory
    
  } // END ... If the study path was specified
  
  return(bIsLocked);
  
} // IsLocked()


/**
 * 
 */
void LocationLock::GetSysTime( std::string & sSystemName, LPSYSTEMTIME lpSysTime )
{
  LPTIME_OF_DAY_INFO pBuf = NULL;
  
  // Default to the local system time
  GetSystemTime(lpSysTime);

  WCHAR szSystemName[512];
  int4 i = 0;
  for ( ; i < sSystemName.size(); i++)
  { 
    szSystemName[i] = sSystemName[i];
  }
  szSystemName[i] = '\0';

  if ( NetRemoteTOD(szSystemName, (LPBYTE *)&pBuf) == NERR_Success )
  {
    if ( pBuf != NULL )
    {
      lpSysTime->wHour = pBuf->tod_hours;
      lpSysTime->wMinute = pBuf->tod_mins;
      lpSysTime->wSecond = pBuf->tod_secs;
      lpSysTime->wDay = pBuf->tod_day;
      lpSysTime->wDayOfWeek = pBuf->tod_weekday;
      lpSysTime->wMilliseconds = pBuf->tod_msecs;
      lpSysTime->wMonth = pBuf->tod_month;
      lpSysTime->wYear = pBuf->tod_year;

      LogTmp("Remote System: [" + ToAscii(lpSysTime->wHour) + ":" + 
             ToAscii(lpSysTime->wMinute) + ":" + ToAscii(lpSysTime->wSecond) + "]", 
             "LocationLock", "GetSysTime");

      NetApiBufferFree(pBuf);
    }
  }
  
}

#undef FILE_REVISION

// $Log: LocationLock.C,v $
// Revision 1.4  2006/07/06 14:55:22  romy
// fopen to fopen_s Migration
//
// Revision 1.3  2006/06/01 20:26:57  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.2  2006/01/31 14:04:26  frank
// code review
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.8  2002/09/30 20:46:02  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 1.7  2002/08/14 16:11:36  wenli
// initial version - Vascular
//
// Revision 1.6  2002/07/18 14:17:58  mkontak
// Added dataset locking capabilities
//
// Revision 1.5  2002/03/11 16:02:03  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 1.4  2002/03/11 15:19:45  mkontak
// Merged VC 1-2/CS-1 into main trunk
//
// Revision 1.3.2.2  2002/02/26 22:19:50  mkontak
// Force the lock files attributes to not be ARCHIVE
//
// Revision 1.3.2.1  2002/02/06 23:20:19  mkontak
// Added VxInstalledModules class to get the default module
//
// Revision 1.3.2.2  2002/02/26 22:19:50  mkontak
// Force the lock files attributes to not be ARCHIVE
//
// Revision 1.3.2.1  2002/02/06 23:20:19  mkontak
// Added VxInstalledModules class to get the default module
//
// Revision 1.3  2001/12/29 00:10:23  jmeade
// Merge from 1.1 branch.
//
// Revision 1.2  2001/11/21 16:35:14  mkontak
// Merge 11/21/2001
//
// Revision 1.1.2.6  2001/11/15 16:03:12  mkontak
// Problems with file times corrected in SetLock().
//
// Revision 1.1.2.5  2001/11/15 14:37:35  mkontak
// Fixed IsLocked(), the file handle was not being closed.
//
// Revision 1.1.2.4  2001/11/14 14:11:02  mkontak
// Fix return code for SetLock()
//
// Revision 1.1.2.3  2001/11/13 02:15:16  mkontak
// Modified SHARE parameter in CreateFile() API function
//
// Revision 1.1.2.2  2001/11/12 19:11:03  mkontak
// Added functinality to get remote system time.
//
// Revision 1.1.2.1  2001/11/12 05:40:19  mkontak
// Initial check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/LocationLock.C,v 1.4 2006/07/06 14:55:22 romy Exp $
// $Id: LocationLock.C,v 1.4 2006/07/06 14:55:22 romy Exp $
