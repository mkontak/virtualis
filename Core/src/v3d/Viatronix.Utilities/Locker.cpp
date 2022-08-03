// $Id: Locker.cpp,v 1.10.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Locker.cpp,v 1.10.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com
//
// Complete history on bottom of file

// Includes
#include "stdafx.h"
#include "Locker.h"
#include "PathUtilities.h"


#include <lm.h>


// Namespaces
using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace Viatronix;
using namespace Viatronix::Utilities;
using namespace Viatronix::Utilities::IO;
using namespace Viatronix::Logging;



// defines
#define FILE_REVISION "$Revision: 1.10.2.1 $"


/**
 * Constructor to lock a dataset.
 *
 * @param  sPath            Study path of dataset.
 * @param  sPrefix          Prefix for dataset.
 * @param  iExpirationTimeInSeconds  Time for lock to expire in seconds.
 */
Locker::Locker(String * sPath, String * sPrefix, int iExpirationTimeInSeconds) :
/*********************************************************************************************/
m_sPath(sPath), 
m_sPrefix(sPrefix), 
m_iExpirationTimeInSeconds(iExpirationTimeInSeconds)
{
  SetLock();
} //Locker()

/**
 * Constructor to lock a study.
 *
 * @param sPath             Study path to be locked
 * @param iExpuirationTime  Time for lock to expire  
 */
Locker::Locker(String * sPath, int iExpirationTimeInSeconds) :
/**********************************************************************************************/
m_sPath(sPath), 
m_sPrefix(""), 
m_iExpirationTimeInSeconds(iExpirationTimeInSeconds)
{
  // Set the lock immediatly
  SetLock();
} //Locker()

/**
 * Destrcutor
 */
Locker::~Locker()
{
  // Automatically Clear the lock
  ClearLock();
} // ~Locker()


/**
 * Sets the lock on the specified location path
 *
 * @param sStudyPath        Study path to be locked
 * @param sDatasetPrefix    Datasets root 
 * @param iExpirationTimeInSeconds   Locks expiration time
 *
 * @return returns true if the dataset was locked.
 */
bool Locker::SetLock(String * sStudyPath, String * sDatasetPrefix,  int iExpirationTimeInSeconds )
{

  bool bResult(false);

  //////////////////////////////////////////////
  // Make sure the study path was specified
  /////////////////////////////////////////////
  if ( String::Compare(sStudyPath,L"") != 0 && System::IO::Directory::Exists(sStudyPath) )
  {

    //////////////////////////////////////////////////////////
    // Convert the system name in a UNC path to an IP address
    //////////////////////////////////////////////////////////
    String *  sNewPath = PathUtilities::ConvertUNCToIP(sStudyPath);
    
    ///////////////////////////////////////////////////////////
    // Make sure the study path exists and is a directory
    /////////////////////////////////////////////////////////
    if ( PathUtilities::IsDirectory(sNewPath) )
    {
      

      ///////////////////////////////////////
      // Check for the study being locked
      ///////////////////////////////////////
      if ( String::Compare(sDatasetPrefix,L"") != 0 )
      {

        if ( Locker::IsLocked(sStudyPath) )
        {
          return(false);
        } // END ... If the study is locked then all the datasets are locked

       
        String * sVolFile(String::Concat(sNewPath, Path::DirectorySeparatorChar.ToString(), sDatasetPrefix, FileExtensions::Volume));
        String * sEcvFile(String::Concat(sNewPath, Path::DirectorySeparatorChar.ToString(), sDatasetPrefix, FileExtensions::Ecv));

        /////////////////////////////////////////////////////
        // If there is not dataset to lock the return false
        /////////////////////////////////////////////////////
        if ( ! System::IO::File::Exists(sVolFile) && 
             ! System::IO::File::Exists(sEcvFile) )
        {
          throw(new System::IO::FileNotFoundException(L"Could not find any Volume or Exlectronically cleaned volume file in the study path, invalid prefix"));
        }

      } // END ... If the dataset prefix is specified

    
      // Build the lock file
      String * sLockFile = BuildLockFileName(sNewPath, sDatasetPrefix);


      ////////////////////////////////////////////////
      // If the lock file exists try to delete it
      ///////////////////////////////////////////////
      if ( System::IO::File::Exists(sLockFile) )
      {
        System::IO::File::Delete(sLockFile);
      } // END ... If the lock file exists


      try
      {
        WriteExpirationTime(sLockFile, iExpirationTimeInSeconds);

        bResult = true;
      }
      catch ( Exception * e)
      {
        throw e;
      }

      ///////////////////////////
      // Force the ARCHIVE off
      //////////////////////////
      PathUtilities::ResetAttribute(sLockFile, FileAttributes::Archive);

    } // END ... If the path is a directory
    else
    {
      throw new System::IO::IOException(L"Study path specified is not a directory");
    }

  } // END ... If the path is supplied
  else
  {
    throw new System::ArgumentNullException(L"Study path was not specified");
  } // END ... If the path was not specified


  return(bResult);
  
} // SetLock()



/**
 * Clears the location lock.
 *
 * @param sStudyPath      Study path to be cleared (unlocked)
 * @param sDatasetPrefix  Dataset prefix  ( File Name Root )
 */
 void Locker::ClearLock(String * sStudyPath,String * sDatasetPrefix  )
{

  //////////////////////////////////////////
  // Make sure the study path was specified
  //////////////////////////////////////////
  if ( String::Compare(sStudyPath, L"") != 0 && System::IO::Directory::Exists(sStudyPath) )
  {
    
    //////////////////////////////////////////////////////
    // Make sure the study path specified is a directory
    /////////////////////////////////////////////////////
    if ( PathUtilities::IsDirectory(sStudyPath) )
    {

      // Build the lock file
      String * sLockFile = BuildLockFileName(sStudyPath, sDatasetPrefix);

      //////////////////////////////////////////////////////
      // If the lock file exists then we need to delete it
      //////////////////////////////////////////////////////
      if ( System::IO::File::Exists(sLockFile) )
      {
        System::IO::File::Delete(sLockFile);
      } // END ... If the lock file exists
      
    }
    else
    {
      throw new System::IO::IOException(L"Study path specified is not a directory");
    }

  } // END ... If the study path was specified
  else
  {
    throw new System::ArgumentNullException(L"The study path is not set");
  } // END ... else if the study path was not specified
  
} // ClearLock()

/**
* Returns true if the specified study path is locked
*
* @param sStudyPath       Study path to check if locked
* @param sDatasetPrefix   Dataset prefix to be used if we are to check for a dataset lock
*
* @return Returns true of the dataset is locked.
*/
bool Locker::IsLocked(String * sStudyPath, String * sDatasetPrefix)
{
  bool bResult = false;

  ///////////////////////////////////////////
  // Make sure the study path was specifieid
  ////////////////////////////////////////////
  if ( String::Compare(sStudyPath, String::Empty) != 0  && System::IO::Directory::Exists(sStudyPath) )
  {

    ///////////////////////////////////////////
    // Extract the system name from the path
    ///////////////////////////////////////////
    String * sSystemName = PathUtilities::GetUNCSystemName(sStudyPath);

    //////////////////////////////////////////////////////////
    // Convert the system name in a UNC path to an IP address
    //////////////////////////////////////////////////////////
    String *  sNewPath = PathUtilities::ConvertUNCToIP(sStudyPath);
    
    ///////////////////////////////////////////////////////////
    // Make sure the study path exists and is a directory
    /////////////////////////////////////////////////////////
    if ( PathUtilities::IsDirectory(sNewPath) )
    {
      ///////////////////////////////////////
      // Check for the study being locked
      ///////////////////////////////////////
      if ( String::Compare(sDatasetPrefix,String::Empty) != 0 && Locker::IsLocked(sStudyPath) )
      {
        return(true);
      } // END ... If the datset prefix is specified and the study is locked then all the datasets are locked

      String * sLockFile = BuildLockFileName(sNewPath, sDatasetPrefix);

      if ( System::IO::File::Exists(sLockFile) )
      {
        try
        {

          int iExpirationTimeInSeconds = Locker::ReadExpirationTime(sLockFile);


          DateTime fileDateTime = System::IO::File::GetLastWriteTime(sLockFile).ToUniversalTime();
          DateTime offsetDateTime = fileDateTime.AddSeconds((double)iExpirationTimeInSeconds);
            
          System::DateTime sysDateTime;
          try
          {
            sysDateTime = GetSysTime(sSystemName);
          }
          catch ( ... )
          {
            LogErr( "Error retrieving system date and time", "Locker", "IsLocked" );
            sysDateTime = DateTime::get_Now();
          }

          if ( sysDateTime.Date == fileDateTime.Date )
          {
            if ( sysDateTime <= offsetDateTime )
            {
              bResult = true;
            }
          } // END ... If the system and file date stampsa are the same date
   
        }
        catch ( System::Exception * e )
        {
          Log::Error(String::Concat("Failed to calculate lock time : ", e->Message), "Locker", "IsLocked");
          throw(e);
        }

      } // END ... If the lock file exists

    }   // END ... If the study path is valid directory 
    else
    {
      Log::Error(String::Concat("Study path specified [PATH=", sStudyPath, "] is not a directory"), "Locker", "IsLocked"); 
      throw new System::IO::IOException(L"Study path specified is not a directory");
    } // END ... else if the study path is not a valid directory

  } // END ... If the study path was specified
  else
  {
    Log::Error(String::Concat("Study path specified [PATH=", sStudyPath, "] does not exist"), "Locker", "IsLocked"); 
    //throw new System::ArgumentNullException(L"The study path is not set");
  } // END ... else if the study path was not specified

  
  return(bResult);
  
} // IsLocked()

/**
 * Returns the lock file name.
 *
 * @param sStudyPath          Locks path
 * @param sDatasetPrefix      Blank if study filled in with datasets root if dataset
 *
 * @return Returns the lock files name with the full path.
 */
String * Locker::BuildLockFileName(String * sStudyPath, String * sDatasetPrefix)
{

  String * sLockFileName = L"";

  if ( String::Compare(sStudyPath,L"") != 0 )
  {
    if (  String::Compare(sDatasetPrefix,L"") != 0 )
    {
      sLockFileName = String::Concat(sStudyPath, Path::DirectorySeparatorChar.ToString(), sDatasetPrefix, FileExtensions::Lock);
    } // END ... If the dataset prefix is set
    else
    {
      sLockFileName = String::Concat(sStudyPath, Path::DirectorySeparatorChar.ToString(), m_sLockFileName);
    } // END ... If the dataset prefix is not set
  } // END ... If the study path is blank

  return(sLockFileName);

} // BuildLockFileName()



/**
 * Returns the system time for the system supplied
 *
 * @param sSystemName     System to get time for
 *
 * @return Returns the DateTime of the suuplied system
 */
System::DateTime Locker::GetSysTime( String * sSystemName )
{
  TIME_OF_DAY_INFO * pBuf = NULL;
  

  System::DateTime currentSystemDateTime(System::DateTime::Now);

  try
  {
    if ( String::Compare(sSystemName,L"") != 0 && String::Compare(System::Windows::Forms::SystemInformation::ComputerName, sSystemName, true) != 0 )
    {
      WCHAR szSystemName[512];
      szSystemName[0] = '\\';
      szSystemName[1] = '\\';

	  int i(0);
      for ( ; i < sSystemName->Length; i++)
      { 
        szSystemName[i + 2] = sSystemName->Chars[i];
      }
      szSystemName[i + 2] = '\0';

      if ( NetRemoteTOD(szSystemName, (LPBYTE *)&pBuf) == NERR_Success )
      {
        if ( pBuf != NULL )
        {
          System::DateTime sysDateTime(pBuf->tod_year, pBuf->tod_month, pBuf->tod_day, pBuf->tod_hours, pBuf->tod_mins, pBuf->tod_secs);
          NetApiBufferFree(pBuf);
          return(sysDateTime);
        } // END ... If a valid buffer with the date is returned
        else
        {
          Log::Error(String::Concat("Failed to get system time [SYSTEM=", sSystemName, "]  : no TOD returned"), "Locker", "GetSysTime");
        }
      } // END ... If the retreiving of the remote systems time succeeds
      else
      {
        Log::Error(String::Format("Failed to get system time [SYSTEM={0}] [ERROR={1}]", sSystemName, __box((int)GetLastError())), "Locker", "GetSysTime");
      }

    } // END ... If the system name is nothing
  }
  catch ( Exception * ex )
  {
    Log::Error(String::Concat("Failed to get system time [SYSTEM=", sSystemName, "]  : " , ex->Message), "Locker", "GetSysTime");
    throw ex;
  }

  return(currentSystemDateTime);
  
} // GetSysTime()

/** 
 * Returns the expiration time from the lock file
 *
 * @param sLockFile       Lock file to be read
 *
 * @return Returns the expiration time in seconds.
 */
int Locker::ReadExpirationTime(String * sLockFile)
{
  int iExpirationTimeInSeconds(0);

  FileStream * pFileStream = NULL;
  StreamReader * pStreamReader = NULL;

  try
  {
    pFileStream = new FileStream(sLockFile, FileMode::Open, FileAccess::Read, FileShare::ReadWrite);

    pStreamReader = new StreamReader(pFileStream);

    String * sEx = pStreamReader->ReadToEnd();

    iExpirationTimeInSeconds = System::Convert::ToInt32(sEx);

    return iExpirationTimeInSeconds;

  }
  catch ( Exception * e)
  {
    Log::Error(String::Concat("Failed to read the expiration time from the lock file : ", e->Message), "Locker", "ReadExpirationTime");
    throw e;
  }
  __finally
  {
    if ( pStreamReader != NULL )
    {
      pStreamReader->Close();
    }
    else if ( pFileStream != NULL )
    {
      pFileStream->Close();
    }


  }


} // ReadExpirationtime()

/** 
 * Writes the expiration time to the lock file
 *
 * @param sLockFile       Lock file to written to.
 * @param iExpirationTimeInSeconds Expiration time
 */
void Locker::WriteExpirationTime(String * sLockFile, int iExpirationTimeInSeconds )
{

  FileStream * pFileStream = NULL;
  StreamWriter * pStreamWriter = NULL;


  try
  {

    pFileStream = new FileStream(sLockFile, FileMode::Create, FileAccess::ReadWrite, FileShare::ReadWrite);

    pStreamWriter = new StreamWriter(pFileStream);

    pStreamWriter->Write(System::Convert::ToString(iExpirationTimeInSeconds));

    pStreamWriter->Close();

  }
  catch ( Exception * e)
  {

    if ( pStreamWriter != NULL )
      pStreamWriter->Close();
    else if ( pFileStream != NULL )
      pStreamWriter->Close();

    throw e;
  }


} // WriteExpirationtime()


// undefines
#undef FILE_REVISION


// $Log: Locker.cpp,v $
// Revision 1.10.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.9  2006/02/06 20:05:40  mkontak
// no message
//
// Revision 1.8  2005/11/18 21:09:16  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7  2005/11/16 19:59:23  mkontak
// Locker fix from 2.1
//
// Revision 1.6  2005/08/29 16:58:14  mkontak
// Reorganized and moved the File to PathUtilities and FileExtensions
//
// Revision 1.5  2005/08/05 11:40:44  mkontak
// Deleted old logger include lines
//
// Revision 1.4  2005/07/01 12:39:08  vxconfig
// Added new Logger.
//
// Revision 1.3  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.2.2.2  2005/06/23 17:17:13  mkontak
// Issue 4209
//
// Revision 1.2.2.1  2005/06/10 14:45:30  mkontak
// Issue 3937
//
// Revision 1.2.12.1  2005/11/07 16:52:04  frank
// Issue #4472: Patched problem retrieving date/time causing XP viewer to faile on close.
//
// Revision 1.2  2004/03/29 21:57:05  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.16  2004/02/09 21:36:06  mkontak
// no message
//
// Revision 1.15  2004/01/20 19:54:23  mkontak
// Added check to make sure the path exists before doing anything
//
// Revision 1.14  2003/07/10 17:56:03  mkontak
// Coding standards, additions to DcmServerService registry
//
// Revision 1.13  2003/06/20 15:09:36  mkontak
// Coding standards
//
// Revision 1.12  2003/05/16 14:35:52  mkontak
// V3D_File
//
// Revision 1.11  2003/03/12 17:10:36  geconomos
// Moved Viatronix.Utilities.File to Viatronix.Utilities.IO.File
//
// Revision 1.10  2002/11/25 22:03:23  mkontak
// Made backward compatible
//
// Revision 1.9  2002/08/12 19:57:11  mkontak
// Added additional exceptions to indicate failures.
//
// Revision 1.8  2002/08/05 21:00:45  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Locker.cpp,v 1.10.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: Locker.cpp,v 1.10.2.1 2011/05/18 01:56:58 mkontak Exp $


