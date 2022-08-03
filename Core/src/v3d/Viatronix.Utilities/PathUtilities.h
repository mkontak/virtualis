// $Id: PathUtilities.h,v 1.13.2.2 2008/10/29 18:30:12 kchalupa Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/PathUtilities.h,v 1.13.2.2 2008/10/29 18:30:12 kchalupa Exp $
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
#include <string.h>
#include <stdlib.h>
#include "UtilityEnums.h"
#include "FileExtensions.h"

// namespaces

OPEN_IO_NS


/**
 * File Size class
 */
public ref class FileSize
{

public:

  /// Constructor
  FileSize(System::String ^ sFilePath, __int64 iSize) : m_sPath(sFilePath), m_iSize(iSize) { }

  /// CompareTo
  int CompareTo(FileSize ^ fs)
  {
   
    return m_iSize.CompareTo(fs->Size);
  }   // CompareTo()

  // ToString
  virtual System::String ^ ToString() override
  {
    return System::String::Format("[FILE={0}] [SIZE={1}]",m_sPath, (m_iSize));
  }

  /// Gets the path
  property System::String ^ FilePath
  {
    String ^ get() { return m_sPath; }
    void set(System::String ^ sValue) { m_sPath = sValue; }
  }

  /// Sets the size
  property __int64 Size
  {
    void set(__int64 iValue) { m_iSize = iValue; }
    __int64 get() { return m_iSize; }
  }

private:

  /// Path
  System::String ^ m_sPath;

  /// Size
  __int64 m_iSize;
};  // class FileSize



/**
 * Implements IO utilities
 */
public ref class PathUtilities
{
private:
  /// Constructor
  PathUtilities() {}

  /// Destructor
  ~PathUtilities() {}


public:

  /// Returns the short path for the specified path (DOS 8.3)
  static System::String ^ GetWindowsShortPathName(System::String ^ sPath);

  /// Returns the long path
  static System::String ^ GetWindowsLongPathName(System::String ^ sPath);

  /// Returns true if the path is a directory 
  static bool IsDirectory(System::String ^ sPath);

  /// Generates a unique name 
  static System::String ^ GenerateUniqueName();

  /// Generates a unique directory name 
  static inline System::String ^ GenerateDirectoryName()
  { return GenerateUniqueName(); }

  /// Generates a unique file name 
  static inline System::String ^ GenerateFileName()
  { return GenerateUniqueName(); }

  ///Excludes all invalid characters from a file name.
  static System::String ^ EliminateInvalidFileNameChars ( System::String ^ fileName );

  /// Sets the attribute for the file/directory specified recursivly
  static void SetAttribute(System::String ^ sFilePath, System::IO::FileAttributes fileAttribute, bool bRecursive);

  /// Sets the attribute for the file/directory specified non-recursivly
  static void SetAttribute(System::String ^ sFilePath, System::IO::FileAttributes fileAttribute ) { SetAttribute(sFilePath, fileAttribute, false); }

  /// Resets the attribute for the file/directory specified recursivly
  static void ResetAttribute(System::String ^ sFilePath, System::IO::FileAttributes fileAttribute, bool bRecursive);

  /// Resets the attribute for the file/directory specified non-recursivly
  static void ResetAttribute(System::String ^ sFilePath, System::IO::FileAttributes fileAttribute ) { ResetAttribute(sFilePath, fileAttribute, false); }

  /// Returns the size of the file or directory (allows ignoring of files, directories and recursive traversing the sub-dirs)
  static __int64 GetSize(array<System::String ^> ^ sFiles );

  /// Returns the size of the file or directory (allows ignoring of files, directories and recursive traversing the sub-dirs)
  static __int64 GetSize(System::String ^ sFilePath, System::Collections::Specialized::StringCollection ^ pExcludeDirectories, System::Collections::Specialized::StringCollection ^ pExcludeFiles) { return GetSize(sFilePath, pExcludeDirectories, pExcludeFiles, true); }

  /// Returns the size of the file or directoryb (recursvly)
  static __int64 GetSize(System::String ^ sFilePath, bool bRecursive) { return GetSize(sFilePath, gcnew System::Collections::Specialized::StringCollection(), gcnew System::Collections::Specialized::StringCollection(), bRecursive); }
  
  /// Returns the size of the file or directory (non-recursive)
  static __int64 GetSize(System::String ^ sFilePath) { return(GetSize(sFilePath, false)); }

  /// Returns the extension for the corresponding image format
  static System::String ^ GetImageFormatExtension(System::Drawing::Imaging::ImageFormat ^ pImageFormat);

  /// Returns the UNC path with an IP instead of a name
  static System::String ^ ConvertUNCToIP(System::String ^ sPath);

  /// Returns the absolute path 
  static System::String ^ GetAbsolutePath(System::String ^ sPath);

  /// Returns the local path (without root)
  static System::String ^ GetLocalPath(System::String ^ sPath);

  /// Returns true of the path is a UNC
  static bool IsUNCPath(System::String ^ sPath);

  /// Returns true if the path is local (! UNC)
  static bool IsLocalPath(System::String ^ sPath);

  /// Returns the extracted system name from a UNC path 
  static System::String ^ GetUNCSystemName(System::String ^ sPath);

  /// Returns a list of file sizes for the files in the path speicifed
  static System::Collections::Generic::List<FileSize ^> ^ GetFileSizes(System::String ^ sPath, bool bRecursive);

  /// Copies an entire directory to the destination, if rename is true the destintaion is the exact path otherwise the source is copied into the destintaion as a sub-directory
  static void CopyDirectory(System::String ^ sSource, System::String ^ sDestination, bool bRename, bool bOverWrite, bool bRecursive);

  /// Returns the number of files in the directory
  static long GetFileCount(System::String ^ sPath, bool bRecursive) { return GetFileCount(sPath, gcnew System::Collections::Specialized::StringCollection(), bRecursive); }

  /// Returns the number of files in the directory
  static long GetFileCount(System::String ^ sPath, System::Collections::Specialized::StringCollection ^ pIgnoreDirectories, bool bRecursive);

  /// Returns the file name with the path minus the extension
  static System::String ^ GetFullTitle(System::String ^ sFilePath);


  /// Returns the UNC path
  static System::String ^ BuildUNCPath(System::String ^ sHostname, System::String ^ sPath);

  /// Returns the PATH with all '/' with '\'
  static System::String ^ UnifySeperators(System::String ^ sPath);

  /// Combines two paths
  static inline System::String ^ CombinePaths(System::String ^ sPath1, System::String ^ sPath2 )
  { return System::IO::Path::Combine( ( sPath1->Length > 0 ? ( sPath1[1] == ':' ? ( sPath1->Length > 2 ? sPath1 : System::String::Concat(sPath1, "\\") ) : sPath1->TrimEnd(m_sDirectorySeperators)) : sPath1 ), sPath2->TrimStart(m_sDirectorySeperators)); }

  /// Combines two paths
  static inline System::String ^ CombinePaths(System::String ^ sPath1, System::String ^ sPath2, System::String ^ sPath3 )
  { return System::IO::Path::Combine(System::IO::Path::Combine(( sPath1->Length > 0 ? ( sPath1[1] == ':' ? ( sPath1->Length > 2 ? sPath1 : System::String::Concat(sPath1, "\\") ) : sPath1->TrimEnd(m_sDirectorySeperators)) : sPath1 ), sPath2->TrimStart(m_sDirectorySeperators))->TrimEnd(m_sDirectorySeperators), sPath3->TrimStart(m_sDirectorySeperators)); }

  /// Copies files
  static void CopyFiles(System::String ^ sDestinationPath, System::String ^ sSourcePath, System::String ^ sFilter, bool bOverwrite);

  /// Rename
  static void Rename(System::String ^ sPath, System::String ^ sFilter, System::String ^ sReplace);

  /// Delete Directory Contents (Recursive)
  static void DeleteDirectoryContents(System::String ^ sDirectory, bool bRecursive);

  /// Delete Directory
  static void DeleteDirectory(System::String ^ sDirectory);

  /// Move Directory
  static void MoveDirectory(System::String ^ sSource, System::String ^ sDestination, bool bOverwrite);

  /// Returns true if  path is relative that is it contains no root
  static bool IsRelative(System::String ^ sFilePath);

  /// Returns true of the path contains a root false if the path is relative
  static bool ContainsRoot(System::String ^ sFilePath);

  /// Returns a list of directory files
  static array<System::String ^> ^ GetDirectoryFiles(System::String ^ sPath, bool bRecursive);

  /// Returns all volume files in the path
  static array<System::String ^> ^ GetVolumeFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Vol)); }

  /// Returns all ecv files in the path
  static array<System::String ^> ^ GetEcvFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Ecv)); }

  /// Returns all dicom files in the path
  static array<System::String ^> ^ GetDicomFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Dicom)); }

  /// Returns all png files in the path
  static array<System::String ^> ^ GetPngFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Png)); }

  /// Returns all png files in the path
  static array<System::String ^> ^ GetMovieFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Avi));  }

  /// Returns all png files in the path
  static array<System::String ^> ^ GetQueueFiles(System::String ^ sPath)  { return System::IO::Directory::GetFiles(sPath, System::String::Concat("*",FileExtensions::Queue)); }

  /// Retruns the volume type
  static System::String ^ GetVolumeType(System::String ^ sVolumeFile);

  /// Retruns the volume data type
  static System::String ^ GetVolumeDataType(System::String ^ sVolumeFile);

private:

  /// Compare
  static int CompareFileSize(FileSize ^ pFS1, FileSize ^ pFS2 ) { return pFS1->CompareTo(pFS2); }  

  ///Returns the folder name from a file path.
  static System::String ^ GetFolderName( System::String ^ fileName );

  /// Returns the size of the file or directory excluding the supplied sup-directories only if recursive
  static __int64 GetSize(System::String ^ sFilePath, System::Collections::Specialized::StringCollection ^ pExcludeDirectories, 
                         System::Collections::Specialized::StringCollection ^ pExcludeFiles, bool bRecursive);  

public:

  /// Directory seperators
  property static array<System::Char> ^ DirectorySeperators
  {
    array<Char> ^ get()  { return m_sDirectorySeperators; }
  }


private:

  ///Invalid File Name Characters
  static array<String ^> ^  m_invalidCharacters = { "\\","/",":","*", "?", "<", ">", "|","\"" };

  /// UNC start "\\"
  static System::String ^ m_sUNCStart = "\\\\";

  /// Seperators
  static array<System::Char> ^  m_sDirectorySeperators = { System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar };

};  // class PathUtilities
      
CLOSE_IO_NS


// Revision History:
// $Log: PathUtilities.h,v $
// Revision 1.13.2.2  2008/10/29 18:30:12  kchalupa
// Issue Annonymization fails.  Had to update the CombinePaths to work around Path.Combine
//
// Revision 1.13.2.1  2007/04/30 11:20:53  mkontak
// new PAC archival changes
//
// Revision 1.13  2006/09/20 12:21:09  mkontak
// Fix for import
//
// Revision 1.12  2006/09/08 19:56:04  mkontak
// Fix to CombinePaths when the first is the argument ia the drive
//
// Revision 1.11  2006/05/09 20:38:38  romy
// fix for GetShort Name
//
// Revision 1.10  2006/02/09 14:10:37  geconomos
// removed using namepace from header
//
// Revision 1.9  2005/11/16 18:58:48  romy
// added EliminateInvalidFileNameChars method
//
// Revision 1.8  2005/11/16 15:32:30  mkontak
// Added VolumeHeader utility
//
// Revision 1.7  2005/11/15 19:51:16  mkontak
// Moved the GetVolumeType() and GetVolumeDataType() out of anonymization
// and into utilties.
//
// Revision 1.6  2005/11/08 17:46:49  mkontak
// Added features
//
// Revision 1.5  2005/11/02 15:00:18  mkontak
// Added CombinePath() with three paths
//
// Revision 1.4  2005/11/02 14:51:13  mkontak
// Added new methods CombinePaths(), ContainsRoot() and IsRelative()
//
// Revision 1.3  2005/09/27 13:43:02  mkontak
// Added GetWondowsShortPathName() amd GetWindowsLongPathName()
//
// Revision 1.2  2005/08/30 13:43:14  mkontak
// Extended the MoveDirectory to account for differing roots
//
// Revision 1.1  2005/08/29 16:58:14  mkontak
// Reorganized and moved the File to PathUtilities and FileExtensions
//
// Revision 1.13  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.12  2005/05/13 18:02:01  mkontak
// Added VrxExt
//
// Revision 1.11  2005/02/22 14:24:22  mkontak
// Removed methods no longer needed
//
// Revision 1.10  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.9  2004/11/01 16:10:51  mkontak
// Pacs Queue Server Changes
//
// Revision 1.8  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.7.2.4  2005/03/21 20:49:48  mkontak
// Fix to return SUCCESS when ignoring DICOM and missing  UID's
//
// Revision 1.7.2.3  2005/02/10 16:00:46  mkontak
// Fixes and additionas for the Dicom Server Patch
//
// Revision 1.10  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.9  2004/11/01 16:10:51  mkontak
// Pacs Queue Server Changes
//
// Revision 1.8  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.7  2004/06/28 17:57:32  mkontak
// Added Rename method
//
// Revision 1.6  2004/05/18 13:08:06  mkontak
// Added LogExt
//
// Revision 1.5  2004/05/17 20:25:04  mkontak
// Minor changes for the new WQ handler
//
// Revision 1.4  2004/05/11 19:13:18  mkontak
// Changes for the WQ
//
// Revision 1.3  2004/04/26 20:31:11  mkontak
// Added config extension
//
// Revision 1.2  2004/04/05 18:26:53  mkontak
// Added GetLocalPath()
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
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
// Revision 1.13  2003/04/22 14:00:21  mkontak
// Used #pragma once
//
// Revision 1.12  2003/04/21 20:40:49  mkontak
// CD Writing/Anonymization
//
// Revision 1.11  2003/03/26 21:50:26  mkontak
// Added CT8 extension
//
// Revision 1.10  2003/03/12 17:10:36  geconomos
// Moved Viatronix.Utilities.File to Viatronix.Utilities.IO.File
//
// Revision 1.9  2003/01/17 21:51:32  mkontak
// Added GetAbsolutePath()
//
// Revision 1.8  2002/11/22 18:30:51  mkontak
// Added extensions and additional methods
//
// Revision 1.7  2002/10/09 20:55:45  mkontak
// Added XmlExt
//
// Revision 1.6  2002/09/30 14:08:04  mkontak
// Added TEXT extension
//
// Revision 1.5  2002/08/27 21:09:48  mkontak
// Added additional file extensions
//
// Revision 1.4  2002/08/23 16:54:49  mkontak
// Added additaionl registry supprt
//
// Revision 1.3  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// Revision 1.2  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/PathUtilities.h,v 1.13.2.2 2008/10/29 18:30:12 kchalupa Exp $
// $Id: PathUtilities.h,v 1.13.2.2 2008/10/29 18:30:12 kchalupa Exp $
