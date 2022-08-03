// $Id: PathUtilities.cpp,v 1.17.2.3 2008/09/08 18:24:24 kchalupa Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/PathUtilities.cpp,v 1.17.2.3 2008/09/08 18:24:24 kchalupa Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com

// Includes
#include "stdafx.h"
#include "PathUtilities.h"
#include "SystemSettings.h"
#include "Descriptions.h"

// Namespaces
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Drawing::Imaging;
using namespace System::IO;
using namespace System::Collections::Specialized;
using namespace Viatronix;
using namespace Viatronix::Logging;


USING_UTILITIES_NS
USING_REGISTRY_NS
USING_TYPES_NS
USING_IO_NS


/**
 * Retrieves the folder name from the file path
 *
 * @param   sFileName   Filename to get folder
 *
 * @return Folder
 */
System::String ^ PathUtilities::GetFolderName( System::String ^ sFileName )
{
  //Do this way becasue System::IO::Path::GetDirectoryName crashes for long path names > 256 

  return sFileName->Substring( 0, sFileName->LastIndexOf( System::IO::Path::DirectorySeparatorChar ) );
}//GetFolderName( System::String ^ sFileName )


/**
 * Retrieves the short name for the the path supplied
 * 
 * @param sPath       Path to return the short name for
 *
 * @return Windows short path name
 */
String ^ PathUtilities::GetWindowsShortPathName(String ^ sPath)
{

  String ^ sDirectory(sPath);
  String ^ sFileName(String::Empty);

  char  shortPathName[512];

  if ( ! IsDirectory(sPath) )
  {
    sFileName  = System::IO::Path::GetFileName(sPath);
    sDirectory = GetFolderName ( sPath ) ; //System::IO::Path::GetDirectoryName(sPath);
  }

  int rc = ::GetShortPathName(GetUnManagedStdString(sDirectory).c_str(), shortPathName, 512);

  String ^ sShortPathName = sDirectory;

  if ( rc == 0 )
  {
    LogWrn( util::Strings::Format("Failed to get short name [%ld]", GetLastError()), "PathUtilities","GetWindowsShortPathName");
  }
  else
  {
    sShortPathName = gcnew String(&shortPathName[0]);
  }
 
  return (sFileName->Length > 0 ? System::IO::Path::Combine(sShortPathName, sFileName) : sShortPathName);

} // GetWindowsShortPathName(String ^ sPath)

/**
 * Retrieves the long name for the the path supplied
 * 
 * @param sPath       Path to return the short name for
 *
 * @return Windows long ath name
 */
String ^ PathUtilities::GetWindowsLongPathName(String ^ sPath)
{
  char  longPathName[512];

  int rc = ::GetLongPathName(GetUnManagedStdString(sPath).c_str(), longPathName, 512);

 
  return gcnew String(&longPathName[0]);

} // GetWindowsLongPathName(String ^ sPath)

/**
 * Converts the speicifed UNC file from a system name to an IP address
 *
 * @param sPath     Path to convert
 *
 * @return Returns the converted path
 */
String ^ PathUtilities::ConvertUNCToIP(String ^ sPath)
{

  String ^ sNewPath = sPath;

  ////////////////////////////////////
  // Only convert if it is a UNC path
  ///////////////////////////////////
  if ( IsUNCPath(sPath) )
  {

    int iSlashPos = sPath->IndexOf(Path::DirectorySeparatorChar.ToString(),2);

    if (iSlashPos < (sPath->Length - 2))
    {
      String ^ sHostName(sPath->Substring(2,iSlashPos - 2));

      String ^ sUpperHostName(sHostName->ToUpper());

      if ( String::Compare(sUpperHostName, System::Windows::Forms::SystemInformation::ComputerName->ToUpper()) == 0 ||
           String::Compare(sUpperHostName, "LOCALHOST") == 0 )
      {
        sHostName = "127.0.0.1";
      }
      else
      {

        /// 2005 changes
        /// Changed GetHostByName() to GetHostEntry()
        System::Net::IPHostEntry ^ pHost = System::Net::Dns::GetHostEntry(sHostName);

        Net::IPAddress ^ ipAddr = pHost->AddressList[0];
          
        sHostName = ipAddr->ToString();
      }

      sNewPath = String::Concat(m_sUNCStart, sHostName, sPath->Substring(iSlashPos));
               
    } // END ... If the slash position is less then the total length of the path

  } // END ... If the path is a UNC

  return GetAbsolutePath(sNewPath);

} // ConvertUNCToIP()

/**
 * Gets a list of files sizes for the path specified
 *
 * @param sPath         Path
 * @param bRecursive    Recursive
 */
System::Collections::Generic::List<FileSize ^> ^ PathUtilities::GetFileSizes(String ^ sPath, bool bRecursive)
{
  System::Collections::Generic::List<FileSize ^> ^ pList(gcnew System::Collections::Generic::List<FileSize ^>);

  array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sPath);

  for ( int i(0); i < sFiles->Length; ++i )
  {

    System::IO::FileInfo ^ pInfo(gcnew FileInfo(sFiles[i]));

    pList->Add(gcnew FileSize(sFiles[i], pInfo->Length));

  } // END .. For each file

  if ( bRecursive )
  {
    array<String ^> ^ sDirs  = System::IO::Directory::GetDirectories(sPath);

    for ( int i(0); i < sDirs->Length; ++i )
    {
      pList->AddRange(GetFileSizes(sDirs[i],bRecursive));
    } // END ... FOr each directory
  }

  pList->Sort(gcnew Comparison<FileSize ^>(CompareFileSize));

  return pList;

} // GetFileSizes(String ^ sPath, bool bRecursive)


/**
 * Returns the local path
 *
 * @param sPath       Path to return the absolute path of.
 *
 * @return Returns the local path for the path specified.
 */
String ^ PathUtilities::GetLocalPath(String ^ sPath)
{

  String ^ sLocalPath(UnifySeperators(sPath));

  if ( ! IsLocalPath(sLocalPath) )
  {
    String ^ sPathRoot(String::Empty);

    if ( IsUNCPath(sLocalPath) )
    {
      array<Char> ^ sep  = { '\\' };

      array<String ^> ^ sFields  = sLocalPath->Split(sep);

      sLocalPath = ( sFields->Length > 3 ? String::Concat("\\",String::Join("\\",sFields,3,sFields->Length - 3)) : "" );
    }
  }

  return sLocalPath;

}  // GetLocalPath()


/**
 * Returns the absolute path
 *
 * @param sPath       Path to return the absolute path of.
 *
 * @return Returns the absolute path for the path specified.
 */
String ^ PathUtilities::GetAbsolutePath(String ^ sPath)
{
  return System::IO::Path::GetFullPath(sPath);
}  // GetAbsolutePath()

/**
 * Generates a unique directory name 
 *
 * @return Unique name
 */
String ^ PathUtilities::GenerateUniqueName()
{

  //System::Globalization::DateTimeFormatInfo * pFormatter = gcnew System::Globalization::DateTimeFormatInfo();

  /// 'T'
  //pFormatter->LongTimePattern = "HHmmssff";

  /// 'F'
  //pFormatter->FullDateTimePattern = "yyyyMMdd_HHmmssff";

  return   System::Guid::NewGuid().ToString();

} // GenerateUniqueName()

/**
 * Returns true if the path is a directory false otherwise.
 *
 * @param sPath         Path to check.
 *
 * @return Returns true if the path is a directory.
 */
bool PathUtilities::IsDirectory(String ^ sPath)
{

  bool bReturn(false);

  //////////////////////////////////
  // Make the path was specified
  /////////////////////////////////
  if ( String::Compare(sPath,"") != 0 )
  {
    try
    {
      bReturn = ( ( System::IO::File::GetAttributes(sPath) & System::IO::FileAttributes::Directory) == System::IO::FileAttributes::Directory ? true : false);  
    }
    catch ( System::Exception ^  )
    {
      // Ignore
    }
  } // END ... If path was specified
  
  return bReturn;

} // IsDirectory()


/**
 * Returns true if the PATH is a UNC falase if it is local.
 *
 * @param sPath         Path to be checked.
 *
 * @return Returns True if the path contains UNC path specifiers
 */
bool PathUtilities::IsUNCPath(String ^ sPath)
{

  bool bReturn(false);

  /////////////////////////////////////
  // Make sure the path was specified
  ////////////////////////////////////
  if ( String::Compare(sPath,L"") != 0 )
  {
    String ^ sFirstTwo = sPath->Substring(0,2);

    bReturn = (String::Compare(sFirstTwo,L"//") == 0  || String::Compare(sFirstTwo,m_sUNCStart) == 0);
  } // END ... If the path ws specified

  return bReturn;

} // IsUNCPath()

/**
* Returns true if the PATH is local to this system false otherwise.
*
* @param sPath         Path to be checked.
*
* @return Returns True if the path contains X: root specifiers
*/
bool PathUtilities::IsLocalPath(String ^ sPath)
{

  bool bReturn(false);

  /////////////////////////////////////
  // Make sure the path was specified
  ////////////////////////////////////
  if ( String::Compare(sPath,L"") != 0 )
  {

    if ( ! IsUNCPath(sPath) )
    {
      if ( System::IO::Path::GetPathRoot(sPath)->IndexOf(":") != -1 )
      {
        bReturn = true;
      }
    }
  }
    
  return bReturn;

} // IsLocalPath()

/**
 * Sets the attribute of the file or directory.
 *
 * If a directory is specified and recursive is set then all files in the directory and all files 
 * in all sub-driectories will be set to with the specified file attributes. This is accomplished
 * by calling ourself recursivly.
 *
 * @param sFilePath       File/Path to set attributes for.
 * @param fileAttributes  Attributes to be set
 * @param bRecursive      Ignored if sFilePath is a file, but if sFilePath is a directory then
 *                        the operation will be perfromed on all files in the directory and recursivly
 *                        on all files in all sub-directories.
 *
 * @return void.
 */
void PathUtilities::SetAttribute(String ^ sFilePath, System::IO::FileAttributes fileAttribute, bool bRecursive )
{
 
  // Get current attributes
  System::IO::FileAttributes currentAttributes = System::IO::File::GetAttributes(sFilePath);

  // Set attributes on file/path
  System::IO::File::SetAttributes(sFilePath, (System::IO::FileAttributes)((int)currentAttributes | (int)fileAttribute));

  //////////////////////////////////////////////////////////////////////////////////////////
  // If the sFilePath is a directory we need to set the attirbutee on all the directory 
  // files.
  /////////////////////////////////////////////////////////////////////////////////////////
  if ( IsDirectory(sFilePath) )
  {

    /////////////////////////////////////////////
    // If we are to perform this recursivly then 
    /////////////////////////////////////////////
    if ( bRecursive )
    {

      ///////////////////////////////////////
      // Set for each file in the directory
      ///////////////////////////////////////
      array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sFilePath);
      for ( int i(0); i < sFiles->Length; i++ )
      {
        // Call ourself
        SetAttribute(sFiles[i], fileAttribute, false);
      } // END ... for each file in the directory

      ///////////////////////////////
      // Set for each sub-directory
      ///////////////////////////////
      array<String ^> ^ dirs = System::IO::Directory::GetDirectories(sFilePath);
      for ( int i(0); i < dirs->Length; i++ )
      {

        // Ignore the '.' and '..'
        if ( dirs[i] == L"." || dirs[i] == L".." )
          continue;

        // Call ourself
        SetAttribute(dirs[i], fileAttribute, bRecursive);
      } // END ... for each directory in the list
    } // END ... If we are to set all files/sub-dirs attribute as well
  } // END ... If this file path is a directory

} // SetAttribute()


/**
 * Reset the attribute of the file or directory.
 *
 * If a directory is specified and recursive is set then all files in the directory and all files 
 * in all sub-driectories will be reset to with the specified file attributes. This is accomplished
 * by calling ourself recursivly.
 *
 * @param sFilePath       File/Path to set attributes for.
 * @param fileAttributes  Attributes to be reset (unset)
 * @param bRecursive      Ignored if sFilePath is a file, but if sFilePath is a directory then
 *                        the operation will be perfromed on all files in the directory and recursivly
 *                        on all files in all sub-directories.
 *
 * @return void.
 */
void PathUtilities::ResetAttribute(String ^ sFilePath, System::IO::FileAttributes fileAttribute, bool bRecursive )
{

  // Current attributes
  System::IO::FileAttributes currentAttributes = System::IO::File::GetAttributes(sFilePath);

  // Unset 
  System::IO::File::SetAttributes(sFilePath, (System::IO::FileAttributes)((int)currentAttributes & ~(int)fileAttribute));


  //////////////////////////////////////////////////////////////////////////////////////////
  // If the sFilePath is a directory we need to set the attirbutee on all the directory 
  // files.
  /////////////////////////////////////////////////////////////////////////////////////////
  if ( IsDirectory(sFilePath) )
  {

    /////////////////////////////////////////////
    // If we are to perform this recursivly then 
    /////////////////////////////////////////////
    if ( bRecursive )
    {

      ///////////////////////////////////////
      // Reset for each file in the directory
      ///////////////////////////////////////
      array<String ^> ^ files = Directory::GetFiles(sFilePath);
      for ( int i(0); i < files->Length; i++ )
      {
        // Call ourself
        ResetAttribute(files[i], fileAttribute, false);
      } // END ... for each file in the directory

      ////////////////////////////////
      // Reset for each directory
      //////////////////////////////
      array<String ^> ^ dirs = System::IO::Directory::GetDirectories(sFilePath);
      for ( int i(0); i < dirs->Length; i++ )
      {
        // Ignore '.' and '..'
        if ( dirs[i] == L"." || dirs[i] == L".." )
          continue;

        // Call ourself
        ResetAttribute(dirs[i], fileAttribute, bRecursive);
      } // END ... for each directory in the list
    } // END ... If we are to set all files/sub-dirs attribute as well
  } // END ... If this file path is a directory

} // ResetAttribute()



/**
 * Returns the system name from the UNC path.
 *
 * @param  sPath        Path to extract the system name from
 *
 * @return Returns the system name from the UNC path.
 */
String ^ PathUtilities::GetUNCSystemName(String ^ sPath)
{

  String ^ sUNCSystemName = L"";

  //////////////////////////////////////////////////////////////////
  // We only need to extract the name if the path is a UNC path
  /////////////////////////////////////////////////////////////////
  if ( IsUNCPath(sPath) ) 
  {
    String ^ sDels("/\\");

    array<String ^> ^ sFields = sPath->Split(sDels->ToCharArray());

    sUNCSystemName = sFields[2];
  } // END ... If the PATH is a UNC

  return sUNCSystemName;

} // GetUNCSystenName()


/** 
 * Returns the size of all files specified in the list
 *
 * @param sFiles      Files to get the size of
 *
 * @return Size of all files
 */
__int64 PathUtilities::GetSize(array<String ^> ^ sFiles )
{

  __int64 iSize(0);

  //////////////////////////////////////////////////////////
  // Get the files size for each file in the directory
  /////////////////////////////////////////////////////////
  for ( int i(0); i < sFiles->Length; i++ )
  {

    if ( ! IsDirectory(sFiles[i]) )
    {
      // Get the files size
      iSize += GetSize(sFiles[i]);
    }
    else
    {
      Log::Warning(String::Format("Directory specified [{0}], ignoring",sFiles[i]), "PathUtilities" , "GetSize");
    }

  } // END ... for each file in the directory

  return iSize;

} // GetSize(String ^ sFiles [] )

/**
 * Returns the size of the file or directory. 
 *
 * If the recursive flag is set to true and the sFilePath parameter is a directory then we will
 * recursivly call ourselve for each sub-directory that is not to be excluded.The exclusion list 
 * can contain extact sub-directory/file names or you may use expression characters:
 *
 * example:
 *    File List:
 *      *.dcm     All dicom files 
 *  
 *    Directory List:
 *      *_DCM     All directories that have a _DCM at the end of their name.
 *
 * @param sFilePath           File/Directory to get the size of
 * @param pExcludeDirectories List of sub-directories that are to excluded from the size.
 * @param pExcludeFiles       List of files that are to be excluded from the size.
 * @param bRecursive          Determines if in a directory we are to traverse the 
 *                            sub-directories
 *
 * @returns Returns the size of the supplied file or directory.
 */
__int64 PathUtilities::GetSize(String ^ sFilePath, StringCollection ^ pExcludeDirectories, StringCollection ^ pExcludeFiles, bool bRecursive)
{

  __int64 iSize = 0;

  if ( IsDirectory(sFilePath) )
  {
    
    array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sFilePath);

    // Set up a collection for all files that match theignore file criteria
    StringCollection ^ pExFiles = gcnew StringCollection();

    //////////////////////////////////////////////////////////////////////////
    // Loop through all the ignore file criteria an find files that match
    /////////////////////////////////////////////////////////////////////////
    for ( int i(0); i < pExcludeFiles->Count; i++ ) 
    {
      array<String ^> ^ sExcludeFiles = System::IO::Directory::GetFiles(sFilePath, pExcludeFiles[i]);
        
      pExFiles->AddRange(sExcludeFiles);
    } // END ... for each file to be ignored


    //////////////////////////////////////////////////////////
    // Get the files size for each file in the directory
    /////////////////////////////////////////////////////////
    for ( int i(0); i < sFiles->Length; i++ )
    {

      /////////////////////////////////////////////////////////
      // If the file is to be excluded then ignore th size
      ////////////////////////////////////////////////////////
      if ( pExFiles->Contains(sFiles[i]) )
      {
        continue;
      } // END ... If the file is to be excluded

      // Get the files size
      iSize += GetSize(sFiles[i]);

    } // END ... for each file in the directory


    ///////////////////////////////////////////////////////////////////////////
    // If the recursize flag is set then we will drill down into the directory
    ///////////////////////////////////////////////////////////////////////////
    if ( bRecursive )
    {

      // Get the directory list
      array<String ^> ^ dirs = System::IO::Directory::GetDirectories(sFilePath);

      // Collection for all directories that match the ignore criteria
      StringCollection ^ pExDirectories = gcnew StringCollection();

      /////////////////////////////////////////////////////////////////////////////////
      // Loop through all the ignore sub-directory criteria to make a list of 
      // directories in the current that match
      /////////////////////////////////////////////////////////////////////////////////
      for ( int i(0); i < pExcludeDirectories->Count; i++ ) 
      {
        array<String ^> ^ sExcludeDirs = System::IO::Directory::GetDirectories(sFilePath, pExcludeDirectories[i]);
          
        pExDirectories->AddRange(sExcludeDirs);
      } // END ... for each sub directory to ignore


      ////////////////////////////////////////////////////
      // For all the directories recursivly call myself
      /////////////////////////////////////////////////////
      for ( int i(0); i < dirs->Length; i++ )
      {

        //////////////////////////////////////////////////////////////////////
        // If the directory is '.', ".." or is to be excluded then continue
        /////////////////////////////////////////////////////////////////////
        if (  String::Compare(dirs[i],L".") == 0  || 
              String::Compare(dirs[i],L"..") == 0 || 
              pExDirectories->Contains(dirs[i]) )
        {
          continue;
        } // END ... If the directory is to be excluded from the size

        iSize += GetSize(dirs[i], pExcludeDirectories, pExcludeFiles, bRecursive);

      } // END ... for each directory in the list

    } // END ... If Recursive

  } // END ... If this file path is a directory
  else
  {
    FileInfo ^ fileInfo = gcnew FileInfo(sFilePath);
    iSize = fileInfo->Length;
  }

  return iSize;

} // GetSize(String ^ sFilePath, StringCollection ^ pExcludeDirectories, StringCollection ^ pExcludeFiles, bool bRecursive)

/**
 * Copies the source diretory to the destination directory.
 *
 * @param sSource         Source directory
 * @param sDestination    Destination directory
 * @param bRename         Rename flag indicates whether the destination is an exact path or a parent to the sources
 *                        path. If true then it means we are looking to rename the directory if false we are looking to 
 *                        copy the source into the destination as a sub-directory.
 * @param bOverwrite      Flag indicating you want to overwrite the destination if it exists.
 * @param bRecursive      Copies all files and sub-directories recursivly.
 *
 * @return void.
 */
void PathUtilities::CopyDirectory(String ^ sSource, String ^ sDestination, bool bRename, bool bOverWrite, bool bRecursive)
{

  if ( System::IO::Directory::Exists(sSource) )
  {
    Log::Debug(String::Format("Copying directory [SOURCE={0}] [DEST={1}]", sSource, sDestination), "PathUtilities", "CopyDirectory");

    String ^ sDestPath( ( bRename ? sDestination : String::Concat(sDestination, System::IO::Path::DirectorySeparatorChar.ToString(), System::IO::Path::GetFileName(sSource)) ) );
 
    /////////////////////////////////////////////////////
    // Make sure the destination does not already exist
    /////////////////////////////////////////////////////
    if ( System::IO::Directory::Exists(sDestPath) )
    {
      /////////////////////////////////////////////
      // If overwrite is set delete the destination
      /////////////////////////////////////////////
      if ( bOverWrite )
      {
        DeleteDirectoryContents(sDestPath, true);
      } // END ... If we are to overwrite
      else
      {
        Log::Error(String::Concat(L"Path already exists [", sDestPath, "] and overwrite was not specified"), "PathUtilities", "CopyDirectory");
        throw  gcnew System::IO::IOException(L"Path already exists");
      } // END ... Else not overwrite
    } // END ... If the paths exists
    else
    {
      Log::Debug(String::Concat("Creating destination directory [", sDestPath, "]"), "PathUtilities", "CopyDirectory");
      System::IO::Directory::CreateDirectory(sDestPath);
    }

    Log::Debug(String::Concat("Creating destination directory [", sDestPath, "]"), "PathUtilities", "CopyDirectory");
    System::IO::Directory::CreateDirectory(sDestPath);

    // Get all files in the source
    array<String ^> ^ files = System::IO::Directory::GetFiles(sSource);

    //////////////////
    // Copy each  file
    //////////////////
    for ( int i(0); i < files->Length; i++ )
    {
      String ^ sDestFile = System::IO::Path::Combine(sDestPath, System::IO::Path::GetFileName(files[i]));
      System::IO::File::Copy(files[i], sDestFile, true);
    } // END ... for each file

    //////////////////////////////////////////
    // Only copy sub-directories is recursive
    //////////////////////////////////////////
    if ( bRecursive )
    {
      array<String ^> ^ dirs = System::IO::Directory::GetDirectories(sSource);

      ///////////////////////////////////////
      // For each sub-directory call ourself
      ///////////////////////////////////////
      for ( int i(0); i < dirs->Length; i++ )
      {
        // Call ourself
        CopyDirectory(dirs[i], sDestPath, false, bOverWrite, bRecursive);
      } // END ... for each directory

    } // END ... If recursive
  }

} // CopyDirectory(String ^ sSource, String ^ sDestination, bool bRename, bool bOverWrite, bool bRecursive)


/**
 * Returns the number of files in the path.
 *
 * @param sPath                  Path to parse
 * @param sIgnoreDirectories     List of directories to ignore.
 * @param bRecursive             Determines if we are to recursive perform operation or not.
 *
 * @return the number of files in the path
 */ 
long PathUtilities::GetFileCount(String ^ sPath, StringCollection ^ pIgnoreDirectories, bool bRecursive)
{

  ////////////////////////////////
  // Make sure the path exists
  ///////////////////////////////
  if ( ! IsDirectory(sPath) )
  {
    throw(gcnew System::IO::IOException(String::Concat(L"Path is not a directory : ", sPath)));
  } // END ... If the path specified is not a directory

  array<String ^> ^ Files = System::IO::Directory::GetFiles(sPath);

  long lFileCount = Files->Length;

  ///////////////////////////////////////////////
  // If we to perform this operation recursivly
  ///////////////////////////////////////////////
  if ( bRecursive )
  {

    // Get the directory list for this path
    array<String ^> ^ dirs = System::IO::Directory::GetDirectories(sPath);

    if ( dirs->Length > 0 )
    {

      // Collection for all directories that match the ignore criteria
      StringCollection ^ pDirectories = gcnew StringCollection();

      /////////////////////////////////////////////////////////////////////////////////
      // Loop through all the ignore sub-directory criteria to make a list of 
      // directories of the surent path to ignore.
      /////////////////////////////////////////////////////////////////////////////////
      for ( int i(0); i < pIgnoreDirectories->Count; i++ ) 
      {
        array<String ^> ^ sIgnoreDirs = System::IO::Directory::GetDirectories(sPath, pIgnoreDirectories[i]);
          
        pDirectories->AddRange(sIgnoreDirs);
      } // END ... for each sub directory to ignore


      /////////////////////////////////////////////////////////////
      // Loop through all the sub-directoroes of the source path
      /////////////////////////////////////////////////////////////
      for ( int i(0); i < dirs->Length; i++ )
      {

        /////////////////////////////////////////////////
        // Check if the directory is to be ignored
        /////////////////////////////////////////////////
        if ( pDirectories->Contains(dirs[i]) )
        {
          continue;
        } // END ... If the directory is to be ignored.

        // Recursivly call ourselves 
        lFileCount += GetFileCount(dirs[i], pIgnoreDirectories, bRecursive);

      } // END ... for each sub-directory

    } // END ... If there are sub-directories

  } // END ... If recursive


  return(lFileCount);

} // GetFileCount(String ^ sPath, StringCollection ^ pIgnoreDirectories, bool bRecursive)



/**
 * Returns the images format extension.
 *
 * @param pImageFormat      Imaneg format to check.
 *
 * @return Returns the extension to gho along with the image format.
 */
String ^ PathUtilities::GetImageFormatExtension(ImageFormat ^ pImageFormat)
{
  String ^ sExt = L"";

  if ( pImageFormat->Equals(ImageFormat::Bmp) )
  {
    sExt = FileExtensions::Bitmap;
  }
  else if ( pImageFormat->Equals(ImageFormat::Gif) )
  {
    sExt = FileExtensions::Gif;
  }
  else if ( pImageFormat->Equals(ImageFormat::Jpeg) )
  {
    sExt = FileExtensions::Jpeg;
  }
  else if ( pImageFormat->Equals(ImageFormat::Icon) )
  {
    sExt = FileExtensions::Icon;
  }
  else if ( pImageFormat->Equals(ImageFormat::Emf) )
  {
    sExt = L".emf";
  }
  else if ( pImageFormat->Equals(ImageFormat::Exif) )
  {
    sExt = L".xxif";
  }
  else if ( pImageFormat->Equals(ImageFormat::Png) )
  {
    sExt = FileExtensions::Png;
  }
  else if ( pImageFormat->Equals(ImageFormat::Tiff) )
  {
    sExt = FileExtensions::Tiff;
  }
  else if ( pImageFormat->Equals(ImageFormat::Wmf) )
  {
    sExt = L".wmf";
  }

  return(sExt);

} // GetImageFormatExtension(ImageFormat * pImageFormat)

/**
 * Returns the fule name with path minus the extension
 *
 * @param sFilePath       File and path to perform operation on.
 *
 * @return Returns the filename with path minus the extension
 */
String ^ PathUtilities::GetFullTitle(String ^ sFilePath)
{

  String ^ sDir = System::IO::Path::GetDirectoryName(sFilePath);

  if ( sDir->Length > 0 )
  {
    sDir = String::Concat(sDir,System::IO::Path::DirectorySeparatorChar.ToString());
  }

  return String::Concat(sDir,System::IO::Path::GetFileNameWithoutExtension(sFilePath));

} // GetFullTitle(String ^ sFilePath)



/** 
 * Builds the UNC path
 *
 * @param sHostname     System name
 * @param sPath         Path
 *
 * @return Returns the UNC path
 */
String ^ PathUtilities::BuildUNCPath(String ^ sHostname, String ^ sPath)
{
  StringBuilder ^ sUNCPath = gcnew StringBuilder();

  sUNCPath->Append(System::IO::Path::DirectorySeparatorChar.ToString());
  sUNCPath->Append(System::IO::Path::DirectorySeparatorChar.ToString());
  sUNCPath->Append(sHostname);
  sUNCPath->Append(System::IO::Path::DirectorySeparatorChar.ToString());

  array<Char> ^ cDel = { '/', '\\' };
  sUNCPath->Append(sPath->TrimStart(cDel));

  return UnifySeperators(sUNCPath->ToString());
} // BuildUNCPath(String ^ sHostname, String ^ sPath)

/** 
 * Unifies the seperaptors 
 *
 * @param sPath
 *
 * @return Returns the seperators for the system running
 */
String ^ PathUtilities::UnifySeperators(String ^ sPath)
{
  String ^ sSearch("/");

  if ( System::IO::Path::DirectorySeparatorChar == '/' )
  {
    sSearch = "\\";
  }

  return sPath->Replace(sSearch, System::IO::Path::DirectorySeparatorChar.ToString());
    
} // UnifySeperators(String ^ sPath)


/** 
 * Copies files from source to dest. If the dest does not exist it will be created.
 *
 * @param sDestinationPath      Destintaion for files
 * @param sSourcePath           Source location for files
 * @param sFilter               File filter
 * @param bOverwrite            Overwrite files flag
 */
void PathUtilities::CopyFiles(String ^ sDestinationPath, String ^ sSourcePath, String ^ sFilter, bool bOverwrite)
{

  if ( ! System::IO::Directory::Exists(sDestinationPath) )
  {
    Log::Debug(String::Concat("Creating destination path ", sDestinationPath), "PathUtilities", "CopyFiles");
    System::IO::Directory::CreateDirectory(sDestinationPath);
  }

  array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sSourcePath, sFilter);

  for ( int i(0); i < sFiles->Length; i++ )
  {
    String ^ sNewFile = String::Concat(sDestinationPath,System::IO::Path::DirectorySeparatorChar.ToString(),System::IO::Path::GetFileName(sFiles[i]));

    Log::Debug(String::Concat("Copying ", sFiles[i], " to ", sNewFile), "PathUtilities", "CopyFiles");

    System::IO::File::Copy(sFiles[i], sNewFile, bOverwrite);
  }

} // CopyFiles(String ^ sDestinationPath, String ^ sSourcePath, String ^ sFilter, bool bOverwrite)

/** 
 * Copies files from source to dest. If the dest does not exist it will be created.
 *
 * @param sPath      Path of files
 * @param sOld       Source location for files
 * @param sNew       File filter
 */
void PathUtilities::Rename(String ^ sPath, String ^ sFilter, String ^ sReplace)
{

  if ( ! System::IO::Directory::Exists(sPath) )
  {
    throw gcnew System::IO::DirectoryNotFoundException(String::Concat("Path does not exists : ", sPath));
  }

  array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sPath, sFilter);

  String ^ sSearch(sFilter->Replace("*",""));

  for ( int i(0); i < sFiles->Length; i++ )
  {

    String ^ sNewFilename(System::IO::Path::GetFileName(sFiles[i])->Replace(sSearch, sReplace));

    String ^ sNewFile = String::Concat(sPath,System::IO::Path::DirectorySeparatorChar.ToString(),sNewFilename);

    Log::Debug(String::Concat("Renaming ", sFiles[i], " to ", sNewFile), "PathUtilities", "Rename");

    System::IO::File::Move(sFiles[i], sNewFile);
  }

} // Rename(String ^ sPath, String ^ sFilter, String ^ sReplace)


/** 
 * Deletes all contents in the directory incuding sub-directories
 *
 * @param sDirectory                  Directrory to be purged
 */
void PathUtilities::DeleteDirectoryContents(String ^ sDirectory, bool bRecursive )
{
  // Check to make sure the directory exists.
  if ( System::IO::Directory::Exists(sDirectory) )
  {
    Log::Debug(String::Concat("Deleting directory contents [", sDirectory, "]"), "PathUtilities", "DeleteDirectoryContents");

    // Delete all files in the current level directory.
    array<String ^> ^ sFiles = System::IO::Directory::GetFiles(sDirectory);
    for ( int i(0); i < sFiles->Length; System::IO::File::Delete(sFiles[i++]) );

    // Delete the contents in the subdirectories if desired.
    if ( bRecursive )
    {
      array<String ^> ^ sDirs = System::IO::Directory::GetDirectories(sDirectory);

      for ( int i(0); i < sDirs->Length; i++)
      {
        DeleteDirectoryContents(sDirs[i], true);

        System::IO::Directory::Delete(sDirs[i]);
      } // for ( int i(0); i < sDirs->Length; i++)
    } // if ( bRecursive )
  } // if ( System::IO::Directory::Exists(sDirectory) )
} // DeleteDirectoryContents(String ^ sDirectory, bool bRecursive )

/** 
 * Deletes all contents in the directory incuding sub-directories
 *
 * @param sDirectory                  Directrory to be purged
 */
void PathUtilities::DeleteDirectory(String ^ sDirectory )
{

  if ( System::IO::Directory::Exists(sDirectory) )
  {
    Log::Debug(String::Concat("Deleting directory [", sDirectory, "]"), "PathUtilities", "DeleteDirectory");

    DeleteDirectoryContents(sDirectory, true);

    // Deletes the directory
    System::IO::Directory::Delete(sDirectory);
  }


} // DeleteDirectory(String ^ sDirectory )

/** 
 * Deletes all contents in the directory incuding sub-directories
 *
 * @param sSource      Source Directrory to be moved
 * @param sDestination        Destination directory
 * @param bOverwrite            Overwrite flag
 */
void PathUtilities::MoveDirectory(String ^ sSource, String ^ sDestination, bool bOverwrite )
{

  if ( System::IO::Directory::Exists(sSource) )
  {
    if ( String::Compare(System::IO::Path::GetPathRoot(sSource), System::IO::Path::GetPathRoot(sDestination), true) == 0 )
    {
      Log::Debug(String::Format("Moving directory [SOURCE={0}] [DEST={1}]", sSource, sDestination), "PathUtilities", "MoveDirectory");

      if ( System::IO::Directory::Exists(sDestination) && ! bOverwrite )
      {
        throw gcnew System::IO::IOException("Destination directory already exists");
      }
      else
      {
        DeleteDirectory(sDestination);

        // Deletes the directory
        System::IO::Directory::Move(sSource, sDestination);
      }
    }
    else
    {
      Log::Debug("Different path roots must perform a copy", "PathUtilities", "MoveDirectory");


      if ( System::IO::Directory::Exists(sDestination) && ! bOverwrite )
      {
        throw gcnew System::IO::IOException("Destination directory already exists");
      }
      else
      {
        DeleteDirectory(sDestination);

        // Deletes the directory
        CopyDirectory(sSource, sDestination, true, true, true);

      
      }
    }

  }


} // MoveDirectory(String ^ sSource, String ^ sDestination, bool bOverwrite )

/** 
 * Returns all the files of the specified directory
 *
 * @param sPath       Directory to search
 * @param bRecursive  Flag to determine if we are to search all sub-directories
 *
 * @return list of diretories 
 */
array<String ^> ^ PathUtilities::GetDirectoryFiles(String ^ sPath, bool bRecursive)
{
  StringCollection ^ pFiles = gcnew StringCollection();

  array<String ^> ^ sFiles  = System::IO::Directory::GetFiles(sPath);

  for (int i(0); i < sFiles->Length; pFiles->Add(sFiles[i++]) );

  if ( bRecursive )
  {
    array<String ^> ^ sDirs = System::IO::Directory::GetDirectories(sPath);

    for ( int i(0); i < sDirs->Length; ++i )
    {
      array<String ^> ^ sDirFiles = GetDirectoryFiles(sDirs[i], bRecursive);

      for (int j(0); j < sDirFiles->Length; pFiles->Add(sDirFiles[j++]) );

    }

  }

  array<String ^> ^ sDirectoryFiles = gcnew array<String ^>(pFiles->Count);

  pFiles->CopyTo(sDirectoryFiles, 0);

  return sDirectoryFiles;
} // GetDirectoryFiles(String ^ sPath, bool bRecursive)


/**
 * Returns true if the path contains a root, false if it is relative
 *
 * @param sFilePath   File or Path to check
 *
 * @return TRUE if it contains a root, false if relative.
 */
bool PathUtilities::ContainsRoot(String ^ sFilePath)
{

  String ^ sRoot(Path::GetPathRoot(sFilePath));
  return (sRoot->Length > 2 ? true: false );
} // ContainsRoot(String ^ sFilePath)

/**
 * Returns true if the path is relative
 *
 * @param sFilePath   File or Path to check
 *
 * @return TRUE if  relative.
 */
bool PathUtilities::IsRelative(String ^ sFilePath)
{

  bool bContainsRoot(ContainsRoot(sFilePath));
  return (! bContainsRoot);
} // IsRelative(String ^ sFilePath)

/** 
 * Returns the volume data type as a string
 *
 * @param sVolumeFile     Source file to extract the type from.
 *
 * @return Returns the string LINEAR or BLOCK
 */
String ^ PathUtilities::GetVolumeType( String ^ sVolumeFile )
{

  String ^ sType( "LINEAR" );
  System::IO::StreamReader ^ pReader(__nullptr);
  array<System::Char> ^ buffer = gcnew array<System::Char>(600);
  try
  {
    pReader = gcnew System::IO::StreamReader( sVolumeFile );   
    pReader->Read(buffer, 0, 600);
    String ^ pBuffer = gcnew String(buffer);
    int index = pBuffer->IndexOf( "sMemoryLayout" );

    if ( index > -1 )
    {
      int iStart(index + 14);
      int iEnd(pBuffer->IndexOf("#", iStart) - 1);

      sType = pBuffer->Substring( iStart, iEnd - iStart )->Trim();
    }

  }
  catch ( System::Exception ^ ex )
  {
    Log::Error( String::Format("Could not get the volumes type [{0}] : {1}", sVolumeFile, ex->Message), "PathUtilities", "GetVolumeType");
    throw gcnew System::Exception( String::Concat("Could not get the volumes type : ", ex->Message) );
  }
  __finally
  {

    // Cleanup resources
    if ( pReader != __nullptr ) 
    {
      pReader->Close();
    }

    delete [] buffer;

  }
  return sType->ToUpper();

} // GetVolumeDatatype(String ^ sVolumeFile)


/** 
 * Returns the volume data type as a string
 *
 * @param sVoluemFile     Source file to extract the type from.
 *
 * @return Returns the data type of the volume UINT2, INT2, ....
 */
String ^ PathUtilities::GetVolumeDataType(String ^ sVolumeFile)
{

  String ^ sDataType("UINT2");
  System::IO::StreamReader ^ pReader(__nullptr);
  array<System::Char> ^ buffer = gcnew array<System::Char>(600);

  try
  {
    pReader = gcnew System::IO::StreamReader(sVolumeFile);   
    pReader->Read(buffer, 0, 600);
    String ^ pBuffer = gcnew String(buffer);
    int iStart(pBuffer->IndexOf("sDataType") + 10);
    int iEnd(pBuffer->IndexOf("#", iStart) - 1);
    sDataType = pBuffer->Substring(iStart, iEnd - iStart)->Trim();
  }
  catch ( System::Exception ^ ex )
  {
    Log::Error(String::Format("Could not get the volumes datatype [{0}] : {1}", sVolumeFile, ex->Message), "PathUtilities", "GetVolumeDataType");
    throw gcnew System::Exception( String::Concat("Could not get the volumes datatype : ", ex->Message), ex );
  }
  __finally
  {
    // Cleanup resources
    if ( pReader != __nullptr ) 
    {
      pReader->Close();
    }

    delete [] buffer;

  }

  return sDataType->ToUpper()->Trim();

} // GetVolumeDatatype(String ^ sSourceFile)


/**
 * Eliminates all invalid file name charactes from a string.
 * @param    sFileName  string which contains invalid characters
 * @returns  A string object which is a copied from fileName and replaced all invalid chars with empty chars.
 */
String ^ PathUtilities::EliminateInvalidFileNameChars ( String ^ sFileName )
{
  String ^ validFileName = String::Copy( sFileName );
  for ( int index=0; index < m_invalidCharacters->Length; index++ )
  {
    int invalidIndex = sFileName->IndexOf( m_invalidCharacters[index] );
    if ( invalidIndex != -1 )
      validFileName = validFileName->Replace( m_invalidCharacters[index] , String::Empty );
  }
  return validFileName;
}//EliminateInvalidFileNameChars ( String ^ fileName )



// Revision History:
// $Log: PathUtilities.cpp,v $
// Revision 1.17.2.3  2008/09/08 18:24:24  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
// Revision 1.17.2.3  2008/09/08 11:25:47  kchalupa
// Fixed a bug in the DeleteDirectoryContents to make it
// correctly descend into the subdirectories.
//
// Revision 1.17.2.2  2007/09/12 13:49:39  mkontak
// Change the GetUniqueFileName to return a guid instead of a date/time permutation
//
// Revision 1.17.2.1  2007/04/30 11:20:53  mkontak
// new PAC archival changes
//
// Revision 1.17  2007/03/09 21:07:03  mkontak
// Coding standards
//
// Revision 1.16  2006/10/06 13:55:41  mkontak
// no message
//
// Revision 1.15  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.14  2006/05/09 20:38:38  romy
// fix for GetShort Name
//
// Revision 1.13  2006/02/09 14:10:37  geconomos
// removed using namepace from header
//
// Revision 1.12  2005/11/23 17:54:55  mkontak
// Minor change
//
// Revision 1.11  2005/11/16 18:58:48  romy
// added EliminateInvalidFileNameChars method
//
// Revision 1.10  2005/11/16 15:32:30  mkontak
// Added VolumeHeader utility
//
// Revision 1.9  2005/11/15 19:51:16  mkontak
// Moved the GetVolumeType() and GetVolumeDataType() out of anonymization
// and into utilties.
//
// Revision 1.8  2005/11/02 18:07:48  mkontak
// Minor fix
//
// Revision 1.7  2005/11/02 14:51:13  mkontak
// Added new methods CombinePaths(), ContainsRoot() and IsRelative()
//
// Revision 1.6  2005/09/29 14:26:45  mkontak
// Only shorten the directory name leaving the file name intact
//
// Revision 1.5  2005/09/27 13:43:02  mkontak
// Added GetWondowsShortPathName() amd GetWindowsLongPathName()
//
// Revision 1.4  2005/09/02 11:36:32  mkontak
// Fix to the CopyDirectory() method
//
// Revision 1.3  2005/09/01 19:29:31  mkontak
// Fix CopyDirectiory
//
// Revision 1.2  2005/08/30 13:43:14  mkontak
// Extended the MoveDirectory to account for differing roots
//
// Revision 1.1  2005/08/29 16:58:14  mkontak
// Reorganized and moved the File to PathUtilities and FileExtensions
//
// Revision 1.17  2005/07/01 12:39:08  vxconfig
// Added new Logger.
//
// Revision 1.16  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.15  2005/02/22 14:24:22  mkontak
// Removed methods no longer needed
//
// Revision 1.14  2005/02/10 16:32:44  mkontak
// Fixes and additionas for the Dicom Server Patch
//
// Revision 1.13  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.12  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.10.2.2  2005/02/10 16:00:46  mkontak
// Fixes and additionas for the Dicom Server Patch
//
// Revision 1.13  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.12  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.10  2004/06/28 17:57:32  mkontak
// Added Rename method
//
// Revision 1.9  2004/06/10 17:52:43  mkontak
// Fix process
//
// Revision 1.8  2004/05/24 19:26:09  mkontak
// Minor fixes
//
// Revision 1.7  2004/05/17 20:25:04  mkontak
// Minor changes for the new WQ handler
//
// Revision 1.6  2004/05/11 19:13:18  mkontak
// Changes for the WQ
//
// Revision 1.5  2004/04/20 17:54:46  mkontak
// Chaged the way the installed modules interfaces with the registry
//
// Revision 1.4  2004/04/12 14:21:53  mkontak
// Reorg of registry and split out to seperate files
//
// Revision 1.3  2004/04/05 18:26:53  mkontak
// Added GetLocalPath()
//
// Revision 1.2  2004/03/02 20:06:54  geconomos
// Merged Viatronix.Logger, Viatronix.Imaging, Viatronix.Threading and Viatronix.Licensing into Viatronix.Utilities.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/02/09 21:35:55  mkontak
// Slight mod to ConvertUNCtoIP so that 127.0.0.1 is used if the hostname in the UNC is the current
// host name.
//
// Revision 1.7  2003/10/31 15:58:52  mkontak
// Fixed CopyFiles
//
// Revision 1.6  2003/10/31 15:17:45  mkontak
// Added CopyFiles method to IO::File
//
// Revision 1.5  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.4  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.3  2003/06/20 15:09:36  mkontak
// Coding standards
//
// Revision 1.2  2003/05/22 18:41:47  dongqing
// code review
//
// Revision 1.1  2003/05/15 14:24:32  mkontak
// Cosmetic
//
// Revision 1.15  2003/05/15 12:29:12  mkontak
// Added extensions
//
// Revision 1.14  2003/05/05 12:23:30  mkontak
// Reorganized registry added new funtionality
//
// Revision 1.13  2003/04/21 20:40:49  mkontak
// CD Writing/Anonymization
//
// Revision 1.12  2003/03/26 15:43:53  mkontak
// Returns the absolute path in convertunctoip()
//
// Revision 1.11  2003/03/12 17:10:36  geconomos
// Moved Viatronix.Utilities.File to Viatronix.Utilities.IO.File
//
// Revision 1.10  2003/01/17 21:51:32  mkontak
// Added GetAbsolutePath()
//
// Revision 1.9  2002/11/22 18:30:51  mkontak
// Added extensions and additional methods
//
// Revision 1.8  2002/10/09 20:55:45  mkontak
// Added XmlExt
//
// Revision 1.7  2002/09/13 19:35:45  mkontak
// Issue 2628: Can only set read access for the ODBC and not write. Use open
//                    before create.
//
// Revision 1.6  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// Revision 1.5  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/PathUtilities.cpp,v 1.17.2.3 2008/09/08 18:24:24 kchalupa Exp $
// $Id: PathUtilities.cpp,v 1.17.2.3 2008/09/08 18:24:24 kchalupa Exp $
