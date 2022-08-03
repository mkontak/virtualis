// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilIO.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxUtilStrings.h"
#include "vxUtilIO.h"
#include "vxLogUtility.h"
#include <Shlobj.h>
#include <ShlGuid.h>


// using
using namespace util;


std::string IO::GetAppDataFolder()
{

   char path[512];

  // ========================================================================
  // Get the COMMON APPLICATION DATA folder
  // 
  //  Windows 7:  C:\ProgramData
  //  Windows XP:  C:\Document and Settings\All Users\Application Data
  // ========================================================================
  ::SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

  return std::string(path);
}

/**
 * Gets the file name from the file path
 *
 * @param sFilePath   File path
 *
 * @return File name
 */
std::string IO::GetFilename(const std::string & sFilePath)
{
  char file[_MAX_DIR];
  char ext[25];
  
  errno_t err = _splitpath_s(sFilePath.c_str(), __nullptr, 0, __nullptr, 0, file, _MAX_DIR, ext, 25);

  std::ostringstream os;

  os << file << ext;

  return os.str();;

} // GetFilename(const std::string & sFilePath)

/**
 * Gets the file name without the extension from the file path
 *
 * @param sFilePath   File path
 *
 * @return File name
 */
std::string IO::GetFilenameWithoutExt(const std::string & sFilePath)
{
  char file[_MAX_DIR];
  char ext[25];
  
  errno_t err = _splitpath_s(sFilePath.c_str(), __nullptr, 0, __nullptr, 0, file, _MAX_DIR, ext, 25);

 
  return std::string(file);

} // GetFilenameWithoutExt(const std::string & sFilePath)

/**
 * Creates the directory specified in the file path
 *
 * @param   sFilePath     File path
 */
void IO::CreateDirectory(const std::string & sFilePath)
{
  char drive[_MAX_DIR];
  char dir[_MAX_DIR];
  char file[_MAX_DIR];
  memset(dir,0,_MAX_DIR);
  memset(file,0,_MAX_DIR);
  
  // Splits the path
  errno_t err = _splitpath_s(sFilePath.c_str(), drive, _MAX_DIR, dir, _MAX_DIR, file, _MAX_DIR, __nullptr, 0);

 
  /// If there was no error in splitting and the directory
  if ( err == 0 )
  {
    char directory[512];
    sprintf_s(directory, 512, "%s%s", drive, dir);


    // Get rid of a trailing directory delimiter
    if ( directory[strlen(directory) - 1] == '\\' || directory[strlen(directory) - 1] == '/' )
      directory[strlen(directory) - 1] = '\0';



    if ( _access(directory, 00) == -1 )
    {
      vxLogUtility::LogMessage(Strings::Format(std::string(""),"Creating directory %s",directory));
      ::CreateDirectory ( directory, NULL );
    }
  } // END ... No error

} // CreateDirectory(const std::string & sFileName)


/**
 * Splits the path into drive, dir, file, ext and returns in a vector
 *
 * @param sFilePath     File to split
 *
 * @return vector of the split values
 */
std::vector<std::string> IO::SplitPath(const std::string & sFilePath)
{
  std::string sLowerCase(const_cast<std::string &>(sFilePath));
  std::transform(sFilePath.begin(), sFilePath.end(), sLowerCase.begin(), ::tolower );

  std::vector<std::string> vpath;

  char drive[_MAX_DRIVE];
  char dir[_MAX_PATH];
  char file[_MAX_DIR];
  char ext[_MAX_EXT];

  memset(drive, 0, _MAX_DRIVE);
  memset(dir,0,_MAX_PATH);
  memset(file,0,_MAX_DIR);
  memset(ext,0,_MAX_EXT);

  errno_t err = _splitpath_s(sLowerCase.c_str(), drive, _MAX_DRIVE, dir, _MAX_PATH, file, _MAX_DIR, ext, _MAX_EXT);

  if ( err == 0 )
  {
    vpath.push_back(drive);
    vpath.push_back(dir);
    vpath.push_back(file);
    vpath.push_back(ext);
  }
  else
    vxLogException(LogRec(Strings::Format("Failed to split path %s [%ld]", sLowerCase.c_str(), err),"IO", "SplitPath")); 


  return vpath;
   

} // SplitPath(const std::string & sFilePath)

/**
 * Gets the default log directory
 */
std::string IO::GetDefaultLogDirectory()
{

  std::string sDirectory(GetDefaultViatronixDirectory() + "Logs");

  // ======================================================
  // If the directory does not exist attempt to create it
  // =====================================================
  if ( ! Exists(sDirectory) )
    ::CreateDirectory(sDirectory.c_str(), NULL);


  return sDirectory + "\\";

} // GetDefaultLogDirectory()

/**
 * Gets the default log directory
 */
std::string IO::GetDefaultViatronixDirectory()
{

  std::string sDirectory(Combine(GetAppDataFolder(),"Viatronix"));

  // ======================================================
  // If the directory does not exist attempt to create it
  // =====================================================
  if ( ! Exists(sDirectory) )
    ::CreateDirectory(sDirectory.c_str(), NULL);

  return sDirectory + "\\";

} // GetDefaultViatronixDirectory()


/**
 * Determines if the file/path exists
 *
 * @param sFilePath     File or Path
 */
bool IO::Exists(const std::string & sFilePath)
{

  return ( _access(sFilePath.c_str(), 0 ) == 0 );
} // Exists(const std::string & sFilePath)

/**
 * Returns the list of files for the directory specified
 *
 * @param sDirectory      Directory
 * @param sPattern        File pattern
 *
 * @return Vector of file names
 */
std::vector<std::string> IO::GetFiles(const std::string & sDirectory, const std::string & sPattern)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;

   std::vector<std::string> files;

   std::string sSearch = sDirectory + "\\" + sPattern;


   hFind = FindFirstFile(sSearch.c_str(), &FindFileData);

   if (hFind != INVALID_HANDLE_VALUE) 
   {
     do
     {

        if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY )
          files.push_back(FindFileData.cFileName);

     } while ( FindNextFile(hFind, &FindFileData ) > 0 );
          
   }

   FindClose(hFind);
  

   return files;

} // GetFiles(const std::string & sDirectory, const std::string & sPattern)

/**
 * Returns the list of files for the directory specified
 *
 * @param sDirectory      Directory
 * @param sPattern        File pattern
 *
 * @return Vector of file names
 */
std::vector<std::string> IO::GetDirs(const std::string & sDirectory, const std::string & sPattern)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;

   std::vector<std::string> dirs;

   std::string sSearch = sDirectory + "\\" + sPattern;


   hFind = FindFirstFile(sSearch.c_str(), &FindFileData);

   if (hFind != INVALID_HANDLE_VALUE) 
   {

     do
     {
        const size_t length = strlen(FindFileData.cFileName);
        if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY &&  
                strncmp(FindFileData.cFileName,".", length) != 0 && 
                  strncmp(FindFileData.cFileName,"..",length) != 00 )
          dirs.push_back(FindFileData.cFileName);

     } while ( FindNextFile(hFind, &FindFileData ) > 0 );
   }

   FindClose(hFind);
  

   return dirs;

} // GetFiles(const std::string & sDirectory, const std::string & sPattern)


/*
 * Combines two paths
 *
 * @param sPath     Path 1
 * @param sPath     Path 2
 *
 * @return Combined path
 */
std::string IO::Combine(const std::string & sPath1, const std::string & sPath2 )
{
  
 
  size_t end = sPath1.size() - 1;
  for ( ; end >= 0 && sPath1[end] == '\\' ; --end );

  std::string combined = sPath1.substr(0,end + 1); 

  unsigned int start = 0;
  for ( ; start < sPath2.size() && sPath2[start] == '\\' ; start++ );

  combined.append("\\");
  combined.append(sPath2.substr(start, sPath2.size() - start));

  return combined;

} // Combine(const std::string & sPath1, const std::string & sPath2 )