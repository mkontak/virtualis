// $Id: File.C,v 1.2 2006/07/06 15:42:33 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dave (dave@viatronix.com)


// includes
#include "stdafx.h"
#include "File.h"
#include "SystemInfo.h"
#include "Utilities.h" 

// namespaces
using namespace std;
using namespace vx;


// globals?
const char * File::m_StudyTypes[] = { "UNKNOWN", "VC", "AAA", "CS", "XP", "VAS", "ANGIO" };


// more includes
#ifdef _WIN32
  #include <io.h>
  #include <direct.h>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
string File::m_sSeparator = "\\";
#else // __linux__ 
  #include <cstdio>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <dirent.h>
  #include <unistd.h>
  #include <pwd.h>
  #include <grp.h>
string File::m_sSeparator = "/";
#endif // _WIN32

#include <errno.h>


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
File::File(const std::string & sPathName) : m_sName(sPathName)
{
  // Convert all slashes to OS depend. type, remove all trailing slashes.
#ifdef _WIN32
  string::iterator it = m_sName.begin();

  for ( ; it != m_sName.end(); ++it )
  {
    if ( *it == '/' )
      *it = '\\';
  }
#else // __linux__ or UNIX
  string::iterator it = m_sName.begin();

  for ( ; it != m_sName.end(); ++it )
  {
    if ( *it == '\\' )
      *it = '/';
  }
#endif
  if ( ! m_sName.empty() )
  {
    if ( m_sName[m_sName.size()-1] == m_sSeparator[0] )
    {
      m_sName = m_sName.substr(0, m_sName.size() - 1);
    }
  }
} // constructor


/**
 * equality comparison for files.
 * @param File object
 * @return equal?
 */
bool File::operator==(const File & other) const
{
  if (GetAbsolutePath() == other.GetAbsolutePath())
  {
    return true;
  }
  else
  {
    return false;
  }
} // operator==()


/**
 * not equal comparison for files
 * @param File object
 * @return equal?
 */

bool File::operator!=(const File & other) const
{
  if (GetAbsolutePath() == other.GetAbsolutePath())
  {
    return false;
  }
  else
  {
    return true;
  }
} // operator!=()


/**
 * Get absolut path
 * @return absolute pathname string of this abstract pathname.
 */
string File::GetAbsolutePath() const
{
  std::string sAbsPath;
  
#ifdef _WIN32
  
  char sAbsolutePath[_MAX_PATH + 1];
  RtlZeroMemory(sAbsolutePath, _MAX_PATH + 1);

  if ( _fullpath(sAbsolutePath, m_sName.c_str(), _MAX_PATH) != __nullptr )
    sAbsPath = sAbsolutePath;

#else

  int Someone_Please_Implement_Me;

#endif
  
  return toupper(sAbsPath);
} // GetAbsolutePath()


/**
 * Get local path
 * @return pathname minus the volume or UNC specifier
 */
string File::GetLocalPath() const
{
  if (IsUNCPath())
  {
    // Jump ahead past the UNC name
    const int4 iPosFirstDir( static_cast< int4 >( strcspn(m_sName.substr(2).c_str(), "\\/")));

    if( static_cast< uint4 >( iPosFirstDir ) >= m_sName.length())
    {
      return string();
    }
  
    return m_sName.substr(iPosFirstDir+2);
  }

  const int2 iLocalPathStart(m_sName[1] == ':' ? 2 : 0);

  if (strchr("\\/", m_sName[iLocalPathStart]))
  {
    return m_sName.substr(2);
  }
  else
  {
    return GetPathSeparator() + m_sName.substr(2);
  }
} // GetLocalPath()


/**
 * Get name
 * @return The name of the file or directory denoted by this abstract pathname,
 *         or the empty string if this pathname's name sequence is empty.
 */
string File::GetName() const
{
  const string::size_type iPos(m_sName.find_last_of(m_sSeparator));

  if ( iPos == string::npos )
  {
    return m_sName;
  }
  else
  {
    return m_sName.substr(iPos+1, m_sName.size());
  }
} // GetName()


/**
 * Get file extension
 * @return The n-character extension of the file (all characters following the last period)
 *         or the empty string if this pathname doesn't contain a period
 */
string File::GetExtension() const
{
  const string::size_type iPos(m_sName.find_last_of('.'));

  if ( iPos == string::npos )
  {
    return string();
  }
  else
  {
    return (m_sName.substr(iPos+1, m_sName.size()));
  }
} // GetExtension()


/**
 * Get title of file
 * @return  The title of the file (name without the extension), or the filename if no extension
 */
string File::GetTitle() const
{
  const string sFilename(GetName());
  const string::size_type iPos(sFilename.find_last_of('.'));

  if ( iPos == string::npos )
  {
    return sFilename;
  }
  else
  {
    return sFilename.substr(0, iPos);
  }
} // GetTitle()


/**
 * Get full title
 * @return full pathname minus the extension
 */
string File::GetFullTitle() const
{
  return GetParent()+GetPathSeparator()+GetTitle();
} // GetFullTitle()


/**
 * Get parent
 * @return pathname string of the parent directory named by this abstract pathname, 
 *         or null if this pathname does not name a parent.
 */
string File::GetParent() const
{
  const string::size_type iPos(m_sName.find_last_of(m_sSeparator));
  if ( iPos == string::npos )
  {
    return string();
  }
  else
  {
    return (m_sName.substr(0, iPos));
  }
} // GetParent()

  
/**
 * Get parent file
 * @return file instance constructed from GetParent()
 */
File File::GetParentFile() const
{
  return File(GetParent());
} // GetParentFile()


/**
 * Get root
 * @return root drive of the pathname
 */
string File::GetRoot() const
{
  const string::size_type iPos(m_sName.find_first_of("/\\"));

  return ( iPos == string::npos ) ? string() : (m_sName.substr(0, iPos));
} // GetRoot()


/**
 * Test whether the application can read from the file
 * @return true if and only if the file specified by this abstract pathname exists and 
 *         can be read by the application; false otherwise.
 */
bool File::CanRead() const
{
#ifdef _WIN32
  return(_access(m_sName.c_str(), 4) == 0);
#else // __linux__
  return(access(m_sName.c_str(), R_OK) == 0);
#endif
} // CanRead()


/**
 * Tests whether the application can modify to the file denoted by this abstract pathname.
 * @return true if the file is write enabled.
 */
bool File::CanWrite() const
{
#ifdef _WIN32
  return(_access(m_sName.c_str(), 2) == 0);
#else // __linux__
  return(access(m_sName.c_str(), W_OK) == 0);
#endif
} // CanWrite()


/**
 * Does file exist?
 * @return true if and only if the file denoted by this abstract pathname exists; false otherwise.
 */
bool File::Exists() const
{
#ifdef _WIN32
  return(_access(m_sName.c_str(), 0) == 0);
#else
  return(access(m_sName.c_str(), F_OK) == 0);
#endif
} // Exists()

/**
 * Does file exist?
 * @return true if and only if the file denoted by this abstract pathname exists; false otherwise.
 */
bool File::Exists( std::string fileName ) 
{
#ifdef _WIN32
  return(_access( fileName.c_str(), 0) == 0);
#else
  return(access( fileName.c_str(), F_OK) == 0);
#endif
} // Exists()


/**
 * Get the drive type
 * @return uint4  An integer specifier of the type of drive on which the file/directory is stored
 */
uint4 File::GetDriveType() const
{
#ifdef _WIN32
  const string sFirstTwo(m_sName.substr(0,2));
  // the Win32 GetDriveType fn won't recognize UNC paths
  return (IsUNCPath()) ? DRIVE_REMOTE : (::GetDriveType(GetRoot().c_str()));
#else
  return 0;
#endif
} // GetDriveType()


/**
 * Get file's attributes
 * @return file's set of attributes
 */
uint4 File::GetAttrib() const
{
#ifdef _WIN32
  return GetFileAttributes(ToString().c_str());
#else
  throw ex::UnsupportedOperationException(LogRec("Only Win32 implemented","File","GetAttrib"));
#endif
} // GetAttrib()


/**
 * Adds the specified attribute(s) to the list of the file's attributes
 * @param attributes to add
 */
void File::AddAttrib(const uint4 uAttribBits) const
{
#ifdef _WIN32
  SetAllAttrib(GetAttrib() | uAttribBits);
#else
  throw ex::UnsupportedOperationException(LogRec("Only Win32 implemented","File","AddAttrib"));
#endif
} // AddAttrib()


/**
 * Adds the specified attribute(s) to the list of the file's attributes
 * @param attributes to remove
 */
void File::RemoveAttrib(const uint4 uAttribBits) const
{
#ifdef _WIN32
  SetAllAttrib(GetAttrib() & ~uAttribBits);
#else
  throw ex::UnsupportedOperationException(LogRec("Only Win32 implemented","File","RemoveAttrib"));
#endif
} // RemoveAttrib


/**
 * Sets the file's attributes to that specified
 * @param attributes to set
 */
void File::SetAllAttrib(const uint4 uAllAttribBits) const
{
#ifdef _WIN32
  SetFileAttributes(ToString().c_str(), uAllAttribBits);
#else
  throw ex::UnsupportedOperationException(LogRec("Only Win32 implemented","File","SetAllAttrib"));
#endif
} // SetAllAttrib()


/**
 * Is it a file?
 * @return true if and only if the file denoted by this abstract pathname exists and is a normal file; false otherwise.
 */
bool File::IsFile() const
{
#ifdef  _WIN32
  
  DWORD dwResult = GetFileAttributes((m_sName).c_str());
  if (dwResult == -1)
  {
    return false;
  }
  if (dwResult & FILE_ATTRIBUTE_DIRECTORY)
  {
    return false;
  }
  else
  {
    return true;
  }
  
#else // __linux__
  
  struct stat stbuf;
  if ( !stat(m_sName.c_str(), & stbuf) )
  {
    return !((stbuf.st_mode & S_IFMT) == S_IFDIR );
  }
  else
  {
    return false;
  }

#endif
} // IsFile()


/**
 * Is it a directory?
 * @return true if and only if the file denoted by this abstract pathname exists and is a directory; false otherwise
 */
bool File::IsDirectory() const
{
#ifdef  _WIN32

  DWORD dwResult = GetFileAttributes((m_sName).c_str());
  if (dwResult == -1)
  {
    return false;
  }
  if (dwResult & FILE_ATTRIBUTE_DIRECTORY)
  {
    return true;
  }
  else
  {
    return false;
  }
  
#else // __linux__
  
  struct stat stbuf;
  if ( !stat(m_sName.c_str(), & stbuf) )
  {
    return ( (stbuf.st_mode & S_IFMT) == S_IFDIR );
  }
  else
  {
    return false;
  }

#endif
} // IsDirectory()


/**
 * Is it a UNC path
 * @return true if the path contains UNC path specifiers
 */
bool File::IsUNCPath() const
{
  const string sFirstTwo(m_sName.substr(0,2));

  return ((sFirstTwo == "//") || (sFirstTwo == "\\\\"));
} // IsUNCPath


/**
 * Returns the UNC system name
 * @param system name
 */
std::string File::GetUNCSystemName() const
{
  std::string sUNCSystemName;

  if ( IsUNCPath() ) 
  {
    std::string::size_type idx(m_sName.substr(2,m_sName.size() - 2).find("\\"));
    if ( idx != std::string::npos )
    {
      sUNCSystemName = m_sName.substr(2,idx);
    }
    else
    {
      idx = m_sName.substr(2,m_sName.size() - 2).find("/");
      if ( idx != std::string::npos )
      {
        sUNCSystemName = m_sName.substr(2,idx);      
      }
    }
  }
  
  return toupper(sUNCSystemName);
} // GetUNCSystemName()


/**
 * Convert UNC to IP address
 * @return file
 */
string File::ConvertUNCToIP()
{
#ifdef _WIN32
  if (IsUNCPath())
  {
    WSADATA WSAData;
    if ( WSAStartup(MAKEWORD(1,1), &WSAData) == 0 )
    {
      HOSTENT * pHost;
      IN_ADDR inAdr;
      const int4 iSlashPos( static_cast< int4 >( strcspn(m_sName.substr(2).c_str(), "/\\")) );

      if ( static_cast< uint4 >( iSlashPos ) < m_sName.length() - 2)
      {
        std::string sHostName(m_sName.substr(2,iSlashPos));

        if ((pHost = gethostbyname(sHostName.c_str())) != NULL)
        {
          memcpy((char *)&(inAdr), pHost->h_addr, pHost->h_length);
          
          sHostName = inet_ntoa(inAdr);
        }

        // Build the UNC pathname
        const string sTmp(string("\\\\") + sHostName + GetPathSeparator() + m_sName.substr(iSlashPos+2+1));
        m_sName = sTmp;
      }

      WSACleanup();
    }
  }
#endif

  return m_sName;
} // ConvertUNCToIP()


/**
 * Delete
 * @return true if and only if the file or directory is successfully deleted; false otherwise
 */
bool File::Delete() const
{
#ifdef _WIN32
  int4 iResult(-1);

  if (IsDirectory())
  {
    iResult = _rmdir(m_sName.c_str());
    if (iResult != SYSTEM_RETURN_SUCCESS)
    {
      if (errno == ENOTEMPTY)      
      {
        LogWrn(m_sName + ": Directory is not empty; or directory is either current working directory or root directory.","File","Delete");
      }
      if (errno == ENOENT)      
      {
        LogWrn(m_sName + ": Path is invalid.","File","Delete");
      }      
    }
  }
  else if (IsFile())
  {
    iResult = remove(m_sName.c_str()); 
    if (iResult != SYSTEM_RETURN_SUCCESS)
    {           
      if (errno == EACCES)
      {
        LogWrn(m_sName + ": Read only.","File","Delete");
      }
      if (errno == ENOENT )
      {
        LogWrn(m_sName + ": Filename or path was not found.","File","Delete");
      }
    }
  }
  else
  {
    LogWrn(m_sName + " is not a file or directory","File","Delete");
  }
  return (iResult != -1 );
  
#else // __linux__
  return ( remove(m_sName.c_str()) != -1 );
#endif
} // Delete()


/**
 * Returns the lenght of the file
 * @return length 
 */
const int8 File::FileLength() const
{
  int8 iFileLen(0);
#if defined(_WIN32)
  const int4 ihFile(_open(m_sName.c_str(), _O_RDONLY));

  if (ihFile != -1)
  {
    iFileLen = _filelengthi64(ihFile);
    _close(ihFile);
  }
#else
  LogWrn("File::FileLength not implemented in this OS","File","FileLength");

#endif

  return iFileLen;
} // FileLength()


/**
 * Set working directory
 * @return true if the working directory was set; false otherwise
 */
bool File::SetWorkingDir() const
{
#if defined(_WIN32)
  return ( _chdir(m_sName.c_str()) == 0 );
#else
  LogWrn("<File::SetWorkingDir> NOT IMPLEMENTED","File","SetWorkingDir");
#endif
} // SetWorkingDir()


/**
 * Returns the current working directory
 * @return directory name
 */
File File::GetWorkingDir()
{
#if defined(_WIN32)

  char vCwdBuff[_MAX_PATH + 1];
  RtlZeroMemory(vCwdBuff, _MAX_DIR + 1);

  if (  _getcwd( vCwdBuff, _MAX_PATH ) != __nullptr )
    LogWrn("Failed to get the current working directory, will return blank", "File", "GetWorkingDir");

  const string sCwd(vCwdBuff);
  return File(sCwd);

#else

  LogWrn("GETWORKIN' DIR NOT IMPLEMENTED IN THE OS","File","GetWorkingDir");

  throw ex::VException();
  return false;
#endif
} // GetWorkingDir()


/**
 * Make directory
 * @return true if the directory was created or already exists; false otherwise
 */
bool File::Mkdir() const
{
#ifdef _WIN32
  return (_mkdir(m_sName.c_str()) == 0) || (errno == EEXIST);
#else // __linux__
  return mkdir(m_sName.c_str(), 0xFFFF);
#endif
} // Mkdir()


/**
 * Make directories
 * @return true if and only if the directory was created, along with all necessary parent directories; false  otherwise
 */
bool File::Mkdirs() const
{
  if ( !GetParent().empty() )
  {
    File par(GetParent());
    par.Mkdirs();
  }

  return Mkdir();
} // Mkdirs()


/**
 * Rename to
 * @param abstract pathname for the named file
 * @return true if and only if the renaming succeeded; false otherwise
 */
bool File::RenameTo(const File & destFile)
{
  const int4 iResult(rename(m_sName.c_str(), destFile.ToString().c_str()));
  if ( iResult != 0 )
  {
    // fail
    return false;
  }
  else
  {
    *this = destFile;
    return true;
  }
} // RenameTo()


/**
 * Move to
 * @param abstract pathname for the named file
 * @param replacing existing files?
 * @return true if and only if the move succeeded; false otherwise
 */
bool File::MoveTo(const File & destFile, bool bReplace /* = false */) const
{
#ifdef _WIN32
  if ( bReplace == true)
  {
    if ( MoveFileEx(m_sName.c_str(),destFile.ToString().c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) )
    {
      return true;
    }
    else
    {
      DWORD dwError = GetLastError();      
      LogWrn("Error [" + ToAscii(dwError) + "] : Could not move file " + m_sName.c_str() + " to " + destFile.ToString().c_str(),"File","MoveTo");
      return false;
    }
  }
  else
  {
    if ( MoveFileEx(m_sName.c_str(),destFile.ToString().c_str(), MOVEFILE_COPY_ALLOWED) )
    {
      return true;
    }
    else
    {
      DWORD dwError = GetLastError();      
      LogWrn("Error [" + ToAscii(dwError) + "] : Could not move file " + m_sName.c_str() + " to " + destFile.ToString().c_str(),"File","MoveTo");
      return false;
    }
  }

#else

  // Delete the destination file first
  if ( bReplace == true)
  {
    if ( destFile.Exists() )
    {
      destFile.Delete();
    }
  }

  // Perform a rename
  return RenameTo(destFile);
#endif
} // Moveto()


/**
 * Copy to
 * @param destination
 * @return true if and only if the move succeeded; false otherwise
 */
bool File::CopyTo(const File & destFile) const
{
  if ( destFile.Exists() )
  {
    return false;
  }
#ifdef _WIN32
  if ( CopyFile(m_sName.c_str(), destFile.ToString().c_str(), FALSE) )
  {
    return true;
  }
  else
  {
    DWORD dwError = GetLastError();      
    LogWrn("Error [" + ToAscii(dwError) + "] : Could not copy file " + m_sName.c_str() + " to " + destFile.ToString().c_str(),"File","CopyTo");
    return false;
  }
#else
  tt << "COPY TO NOT IMPLEMENTED "<<endl;
  return false;
#endif

  return false;
} // CopyTo()


/** 
 * Get the directory count
 * @return number of files?
 */
int4 File::GetDirectoryCount() const
{
  int4 iDirCount(0);

  if ( IsDirectory() )
  {
#ifdef _WIN32
    struct _finddata_t fData;
    const intptr_t iFileHandle(_findfirst((m_sName + "\\*").c_str(), &fData));
  
    if ( iFileHandle != -1 )
    { 
      perror("_findfirst() - Invalid directory name");
 
      do
      {
        if ( fData.attrib & _A_SUBDIR )
          iDirCount++;
          
      } while ( _findnext(iFileHandle, &fData) == 0 );
  
      _findclose(iFileHandle);
    } // END ... if the file hanle is valid
#endif
  }

  return iDirCount;
} // GetDirectoryCount()


/**
 * List files
 * @param file filter
 * @return list of files
 */
vector<File> File::ListFiles(const FileNameFilter & filter) const
{
  vector<File> vFiles;

  // if there is not file then no files can be listed
  if ( m_sName.empty() )
  {
    return vFiles;
  }

  if ( IsDirectory() )
  {
#ifdef _WIN32
    struct _finddata_t fData;
    const intptr_t iFileHandle(_findfirst((m_sName + '\\' + filter.GetNameWildcard()).c_str(), &fData));

    if ( iFileHandle != -1 )
    { 
      do
      {
        vFiles.push_back(File(m_sName + m_sSeparator + fData.name));
      } while ( _findnext(iFileHandle, &fData) == 0 );

      _findclose(iFileHandle);
    }
    else
    {
      perror("_findfirst() - Invalid directory name");
    }
#else // __linux__
    DIR * dfd = opendir(m_sName.c_str());

    if ( dfd != NULL )
    {
      dirent *dp;
      while ( (dp = readdir(dfd)) != NULL )
      {
        if ( filter.Accept(m_sName, dp->d_name) )
        {
          vFiles.push_back(File(m_sName + m_sSeparator + dp->d_name));
        }
      }
      closedir(dfd);
    }
    else
    {
      cerr<<"<File::List> Can't oper directory"<<endl;
    }
#endif
  }
 
  return vFiles;
} // ListFiles()


/**
 * Returns number of files that match the specified filter (or just the number of files in directory)
 * @param filter for file count
 * @return number of files matching specified filter
 */
uint4 File::GetCount(const FileNameFilter & filter) const
{
  return static_cast< uint4 >( ListFiles(filter).size() );
}


/**
 * List files recursively
 * @param file filter
 * @return vector of files
 */
vector<File> File::ListFilesRecur(const FileNameFilter & filter) const
{
  // if there is not file then no files can be listed
  if ( m_sName.empty() )
  {
    return (vector<File>());
  }

  vector<File> vResult;

  if ( IsDirectory() )
  {
    ListFilesRecurHelper(filter, m_sName, vResult);
  }

  return vResult;
} // ListFilesRecur()


/**
 * Create a new file
 * @return success?
 */
bool File::CreateNewFile() const
{
  ofstream fileOFStream(m_sName.c_str());

  if ( fileOFStream.good() != 0 )
  {
    return true;
  }
  else
  {
    return false;
  }
} // CreateNewFile()


/**
 * Returns the study path for the supplied arguements
 * @param studies directory
 * @param type of study  
 * @param patients last name
 * @param patients first name
 * @param studies scan date
 * @param study UID
 * @return  sStudyDir + "\" + eStudyType(VC,CS,AAA) + "_" + sPatientLastName + "_" + sPatientFirstName + "." + sScanDate + "-" + sUID
 */
std::string File::GetStudyPath(const std::string & sStudyDir, 
                               int4 iStudyType, 
                               const std::string & sPatientLastName, 
                               const std::string & sPatientFirstName, 
                               const std::string & sScanDate, 
                               const std::string & sUID )
{
  std::string sStudyPath;
  std::string sLastChar(sStudyDir.substr(sStudyDir.size() - 1, 1));
  
  if ( sLastChar == "/" || sLastChar == "\\" )
  {
    sStudyPath = sStudyDir + File::m_StudyTypes[iStudyType] + "_" + 
                  sPatientLastName + "_" + sPatientFirstName + "." + sScanDate;    
  }
  else
  {
    sStudyPath = sStudyDir + vx::File::GetPathSeparator() + File::m_StudyTypes[iStudyType] + "_" + 
                  sPatientLastName + "_" + sPatientFirstName + "." + sScanDate;
  }

  // if the study id was specified the use it in the study path otherwise ignore it
  if ( ! sUID.empty() )
  {
    sStudyPath += "-" + sUID;
  }
  
  return sStudyPath;
} // GetStudyPath()



/**
 * Creates and returns a unique temporary file
 * @param prefix string for generating the file's name; must be at least three characters long
 * @param suffix string for generating the file's name; may be null, in which case the suffix ".tmp" will be used
 * @return abstract pathname denoting a newly-created empty file
 * @exception vx::IOException
 */
File File::CreateTempFile(string sPrefix, string sSuffix)
{
#ifdef _WIN32 // NOTE : Even easier to create a temp file in Windows...have a look!

  int1 sTempDir[MAX_PATH+1];
  const int4 iPathStringLen(GetTempPath(MAX_PATH, sTempDir));
  GetLongPathName(sTempDir, sTempDir, MAX_PATH);
  const uint4 uPrefixLen( static_cast< uint4 >( sPrefix.length() )); // Pad the prefix if it isn't long enough

  if ( uPrefixLen < 3 )
  {
    sPrefix += string("TMP").substr(0, 3-uPrefixLen);
  }

  int1 sTempBuffer[MAX_PATH+1];
  const uint4 uUniqueIdUsed( GetTempFileName(sTempDir, sPrefix.c_str(), 0, sTempBuffer) );

  if (uUniqueIdUsed == 0)
  {
    return File();
  }

  string sFilename(sTempBuffer);

  // Replace the .tmp if necessary
  if (sSuffix.length() > 0)
  {
    File(sFilename).Delete();

    int4 iDotPos = static_cast< int4 >( sFilename.find_last_of(".TMP") );
    if (iDotPos != string::npos)
    {
      sFilename = sFilename.substr(0, iDotPos) + '.' + sSuffix;
    }

    File fileWithSuffix(sFilename);
    fileWithSuffix.CreateNewFile();

    return fileWithSuffix;
  }

  return File(sFilename);

#else 
  int TODO__implement_use_of_the_sPrefix_and_sSuffix_arguments;

  string sName;
  int1 *sTempFileName = "/tmp/v2kXXXXXX";
  int4 fd = mkstemp(sTempFileName);
  if ( fd != -1 )
  {
    // close the file since it gets opened
    close(fd);
    sName = sTempFileName;
  }
  return File sName;

#endif
} // CreateTempFile()


/**
 * List roots
 * @return abstract pathname denoting a newly-created empty file.
 */
vector<File> File::ListRoots()
{
  return (vector<File>()); // check out _getdcwd() for example
} // ListRoots()


/**
 * @return string  The local computer's name
 */
string File::GetComputerName()
{
  return(SystemInfo::GetNetBiosComputerName());
}

/**
 * Serializes the file/directory name by replacing the spaces with '*'
 * @param file name
 * @return new file name
 */
std::string File::SerializeFileName(const std::string sFileName)
{
  std::string sNewFile(sFileName);
  std::string sReplace("*");
  std::string sSearch(" ");
  std::string::size_type idx(sNewFile.find(sSearch));

  while ( idx != std::string::npos )
  {
    sNewFile.replace(idx, sSearch.size(), sReplace);
    idx = sNewFile.find(sSearch);     
  }

  return sNewFile;  
} // SerializeFileName()


/**
 * Deserializes the file/directory name by replacing the  '*' with a space
 * @param file name
 * @return new file name
 */
std::string File::DeSerializeFileName(const std::string sFileName)
{
  std::string sNewFile(sFileName);
  std::string sReplace(" ");
  std::string sSearch("*");
  std::string::size_type idx(sNewFile.find(sSearch));

  while ( idx != std::string::npos )
  {
    sNewFile.replace(idx, sSearch.size(), sReplace);
    idx = sNewFile.find(sSearch);     
  }

  return sNewFile;
} // DeSerializeFileName()



/**
 * List files recursive helper
 * @param file filter
 * @param current file
 * @return vector of files
 */
void File::ListFilesRecurHelper(const FileNameFilter & filter, 
                                File curFile, 
                                vector<File> & vOutFiles) const
{
  vector<File> vNewFiles = curFile.ListFiles();
  vector<File>::iterator fileIter = vNewFiles.begin();

  for ( ; fileIter != vNewFiles.end(); ++fileIter )
  {
    if ( fileIter->IsDirectory() )
    {  // check if it is directory && not /. or /..
      string sCurDir(m_sSeparator + ".");
      string sParent(m_sSeparator + "..");
      string sFileStr(fileIter->ToString());
      int4 iLen( static_cast< int4 >( sFileStr.size()));

      if ( sFileStr.substr(iLen-2) != sCurDir && sFileStr.substr(iLen-3) != sParent )
      {
        ListFilesRecurHelper(filter, *fileIter, vOutFiles);
      }
    }

    // now check if we should Accept the file
    if ( filter.Accept(fileIter->GetParent(), fileIter->GetName()) )
    {
      vOutFiles.push_back(*fileIter);
    }
  } // for files
} // ListFilesRecurHelper()


/**
 * Get number of files
 * @param path
 * @return count
 */
uint4 File::GetNumberOfFiles(const std::string & sPath)
{
  uint4 iNumberOfFiles(0);

  if ( vx::File(sPath).IsDirectory() )
  {
    FileVector vFileList = vx::File(sPath).ListFiles();

    for ( uint4 i(0); i < vFileList.size(); i++ )
    {
      if ( vFileList[i].GetName() == "." || vFileList[i].GetName() == ".." )
        continue;

      if ( vFileList[i].IsDirectory() )
      {
        iNumberOfFiles += vx::File::GetNumberOfFiles(vFileList[i].ToString());
      }
      else
      {
        iNumberOfFiles++;
      }
    }
  }
  else
  {
    iNumberOfFiles++;
  }

  return iNumberOfFiles;
} // GetNumberOfFiles()


/**
 * Get file size
 * @param file name
 * @return file size
 */
uint8 File::GetFileSize(const std::string & sFile)
{
  uint8 iSize(0);

  if ( vx::File(sFile).IsDirectory() )
  {
    FileVector vFiles = vx::File(sFile).ListFiles();

    for ( uint4 i(0); i < vFiles.size(); i++ )
    {
      if ( vFiles[i].GetName() == "." || vFiles[i].GetName() == ".." )
        continue;

      iSize += vx::File::GetFileSize(vFiles[i].ToString());
    }
  }
  else
  {
    WIN32_FILE_ATTRIBUTE_DATA fileData;

    if ( GetFileAttributesEx(sFile.c_str(), GetFileExInfoStandard, &fileData) == TRUE )
    {
      iSize = fileData.nFileSizeLow;
    } 
  }

  return iSize;

} // GetFileSize()


/**
 * Test function
 * @param ostream
 */
void File::Test(ostream & os)
{
  File cwd(GetWorkingDir());

  if (!cwd.IsDirectory()) throw ex::VException(LogRec("Error getting current directory","File","Test"));

  File A = File::CreateTempFile("A");
  //if (!A.Exists()) throw ex::VException(LogRec("Error creating temp file A","File","Test"));
  File B = File::CreateTempFile("B");
  if (A == B) throw ex::VException(LogRec("Temp file error, two temp files equal","File","Test"));
  if (!A.CanRead()) throw ex::VException(LogRec("Temp file A non-readable","File","Test"));
  if (!A.CanWrite()) throw ex::VException(LogRec("Temp file A non-writable","File","Test"));
  if (!A.Delete())  throw ex::VException(LogRec("Error deleting file","File","Test"));

  // only on Windows w/C
  File newCwd("C:/newDir");
  newCwd.Mkdir();
  newCwd.SetWorkingDir();

  File C(".././.");
  File D("..");
  if (C != D) throw ex::VException(LogRec("C and D directories non-equal","File","Test"));

  cwd.SetWorkingDir();
  newCwd.Delete();
} // Test()


#undef FILE_REVISION


// $Log: File.C,v $
// Revision 1.2  2006/07/06 15:42:33  romy
// added File Exists method
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.30  2003/09/29 20:00:40  mkontak
// Angio processing
//
// Revision 3.29  2003/07/07 18:41:57  mkontak
// Change CTA to VAS
//
// Revision 3.28  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.27  2003/03/21 14:50:55  mkontak
// Fix for CTA
//
// Revision 3.26  2002/09/30 20:45:08  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.25  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.24  2002/07/30 16:49:51  mkontak
// Added GetFileSize()
//
// Revision 3.23  2002/07/30 15:34:17  mkontak
// Added GetNumberOfFIles() method for a PATH
//
// Revision 3.22  2002/07/18 14:16:45  mkontak
// Minor changes
//
// Revision 3.21  2002/06/26 15:08:48  mkontak
// Fixe for Tower, Added DeletEvent() to Events class
//
// Revision 3.20  2002/06/05 21:25:15  mkontak
// Issue 2335: Fixed serialize/deserialize file name.
//
// Revision 3.19  2002/05/23 15:22:00  mkontak
// In ConvertUNCtoIP() we called WSAStart() without WSACleanup() this could
// be causing a small memory leak.
//
// Revision 3.18  2002/05/21 19:06:28  mkontak
// Issue 2258: If the ListFiles() was called and the filename was not set it would
//                    list all files and directories in the C:" File.C
//
// Revision 3.17  2002/05/16 19:24:19  mkontak
// The GetStudyPath now takes an additional parameter UID to
// be appended to the end of the path. If blank it will be ignored and
// the study path will be formatted the original way.
//
// Revision 3.16  2002/04/11 20:04:29  mkontak
// Fixed bug and added a FILELIST typedef
//
// Revision 3.15  2002/03/14 21:48:19  mkontak
// Fixed GetAbsolutePath().
//
// Revision 3.14  2002/03/11 16:02:03  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.13  2002/03/11 15:19:53  mkontak
// Merged VC 1-2/CS-1 into main trunk
//
// Revision 3.12  2002/02/28 18:12:50  geconomos
// Removal of QT.
//
// Revision 3.11.2.2  2002/03/05 20:10:15  jmeade
// File attribute functions.
//
// Revision 3.11.2.1  2002/02/02 07:27:37  jmeade
// Strings and LogRecs.
//
// Revision 3.11.2.3  2002/07/10 18:50:07  mkontak
// Fixed bug in serialize file name
//
// Revision 3.11.2.2  2002/03/05 20:10:15  jmeade
// File attribute functions.
//
// Revision 3.11.2.1  2002/02/02 07:27:37  jmeade
// Strings and LogRecs.
//
// Revision 3.11  2001/01/01 07:17:19  mkontak
// Pulled database dependency from GetStudyPath()
//
// Revision 3.10  2001/01/01 05:15:55  mkontak
// Added GetStudyPath() to retreive the built study path
//
// Revision 3.9  2002/01/24 02:10:48  jmeade
// GetParentFile() instance.
//
// Revision 3.8  2002/01/17 22:24:48  jmeade
// Use system commands for file name (filter) matching.
//
// Revision 3.7  2002/01/17 03:03:05  jmeade
// Added const to function declarations where necessary.
//
// Revision 3.6  2002/01/12 00:14:21  jmeade
// Bug, CreateTempFile():  Was cutting off last letter of file title when suffix specified.
//
// Revision 3.5  2002/01/10 02:19:58  jmeade
// Method to GetCount() of matching filenames in directory.
//
// Revision 3.4  2002/01/03 19:09:30  jmeade
// Function to get full path title (file title prefixed by full directory path).
//
// Revision 3.3  2001/12/31 20:54:58  mkontak
// Modified the GetComputerName() method to call the SystemInfo::GetNetBiosName()
//
// Revision 3.2  2001/12/29 00:10:23  jmeade
// Merge from 1.1 branch.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.1  2001/11/14 22:29:16  mkontak
// Modified MoveTo() to work on shares.
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.4   Oct 04 2001 14:29:38   jmeade
// Should use backslashes in UNC/IP pathnames
// 
//    Rev 2.3   Sep 28 2001 16:31:52   jmeade
// Removed exception throw for the awfully minute failure of SetWorkingDir
// 
//    Rev 2.2   Sep 20 2001 15:10:10   jmeade
// UNC to IP conversion methods
// 
//    Rev 2.1   Sep 19 2001 14:29:56   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:32   ingmar
// Initial revision.
// Revision 1.13  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.12  2001/09/12 17:06:48  mkontak
// Added MoveTo() method
//
// Revision 1.11  2001/09/11 23:49:19  jmeade
// Use proper boolean constants instead of type-casted integers
//
// Revision 1.10  2001/09/11 21:06:13  soeren
// fixed deleting problem of directories in UNC paths
//
// Revision 1.9  2001/09/11 21:04:02  soeren
// fixed deleting problem of directories in UNC paths
//
// Revision 1.8  2001/09/11 16:50:43  soeren
// added some logger warnings in delete funtion
//
// Revision 1.7  2001/09/11 16:49:36  soeren
// added some logger warnings in delete funtion
//
// Revision 1.6  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.5  2001/09/07 21:21:17  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.4  2001/09/07 17:59:07  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.3  2001/09/07 16:15:24  jmeade
// comments
//
// Revision 1.2  2001/09/05 17:19:50  jmeade
// Mkdir should return true if the directory already exists
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.53  2001/08/27 20:52:13  mkontak
// Added two methods to serialize and deserialize file name so that
// spaces are converted to a '*' when serialized and back to a space when
// deserialized.
//
// Revision 1.52  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.51  2001/08/07 17:02:11  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.50  2001/08/06 22:32:26  mkontak
// Added GetUNCSystemName() method
//
// Revision 1.49  2001/07/24 22:32:38  tushar
// fixed a byg
//
// Revision 1.48  2001/06/26 23:26:06  jmeade
// GetTitle() fn; coding conventions
//
// Revision 1.47  2001/06/20 19:52:10  jmeade
// GetDriveType(), GetUNCPath(); temp filename fn returns a long filename
//
// Revision 1.46  2001/06/13 16:08:52  jmeade
// no message
//
// Revision 1.45  2001/06/06 19:43:45  jmeade
// GetComputerName() fn (though I'm not sure this is the place for it, can't think
// of a better for now)
//
// Revision 1.44  2001/06/01 15:34:33  jmeade
// GetRoot()
//
// Revision 1.43  2001/05/28 20:17:59  dave
// moving to UNC paths, new database
//
// Revision 1.42  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.41  2001/05/01 21:10:47  soeren
// acivated qt
//
// Revision 1.40  2001/04/25 19:26:24  dave
// closer standards compliance
//
// Revision 1.39  2001/04/20 17:33:11  dave
// commented out GetAbsolutePath
//
// Revision 1.38  2001/04/19 20:57:25  dave
// added comparision functions, test, variable names
//
// Revision 1.37  2001/04/19 15:53:22  jmeade
// removed unreferenced var
//
// Revision 1.36  2001/04/13 15:18:00  jmeade
// check return value from GetTempFileName()
//
// Revision 1.35  2001/04/13 00:11:59  jmeade
// Proper Win32 coding in CreateTempFile() function (erroneously coded to be
// "harder" than Linux version); NOTE:  Linux implementation is incorrect & incomplete
//
// Revision 1.34  2001/04/12 18:50:58  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.33  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.32  2001/04/12 15:25:34  jmeade
// GetExtension(): gets the file's extension
//
// Revision 1.31  2001/04/10 21:31:50  dave
// updated some comments
//
// Revision 1.30  2001/04/10 15:02:37  dave
// updated Delete for dirs to not chdir to parent
//
// Revision 1.29  2001/04/09 22:23:49  dave
// removed Abort call
//
// Revision 1.28  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.27  2001/04/05 17:59:53  dmitry
// updated datatypes
//
// Revision 1.26  2001/04/03 17:04:20  jmeade
// file length query method
//
// Revision 1.25  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.24  2001/03/23 14:59:42  dave
// more standards compliant
//
// Revision 1.23  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.22  2001/03/13 18:29:49  dave
// corrected return value for CopyTo
//
// Revision 1.21  2001/03/13 17:25:25  dave
// added GetWorkingDir to complement SetWorkin'Dir
// added CopyTo(), both only work in Win now
//
// Revision 1.20  2001/03/12 16:45:54  jmeade
// Please guys, no aborts for minor failures!!!
//
// Revision 1.19  2001/03/09 22:27:22  soeren
// Added SetAsWorkingDir()
//
// Revision 1.18  2001/02/13 02:57:23  dave
// added FileNameFilter as new file, listRecusive in File.h
//
// Revision 1.17  2001/02/12 16:11:26  sarang
// added some comments
//
// Revision 1.16  2001/02/05 22:58:51  sarang
// Added missing linux system calls.
//
// Revision 1.15  2001/01/25 16:48:42  dave
// added some File.C stuff for Linux
//
// Revision 1.14  2001/01/24 15:24:48  dave
// changed Delete()
//
// Revision 1.13  2001/01/23 21:00:31  dave
// ss
//
// Revision 1.12  2001/01/23 20:49:26  dave
// ss
//
// Revision 1.11  2001/01/23 20:46:56  dave
// ss
//
// Revision 1.10  2001/01/23 20:23:26  dave
// changed Delete()
//
// Revision 1.9  2001/01/23 20:00:41  dave
// compiles for UNIX
//
// Revision 1.8  2001/01/23 16:37:07  dave
// corrected CreateTempFile again
//
// Revision 1.7  2001/01/23 13:20:33  frank
// Supported the creation of a unique file path.
//
// Revision 1.6  2001/01/22 22:27:45  dave
// attemted to compile in UNIX
//
// Revision 1.5  2001/01/22 19:42:11  dave
// corrected CreateTempFile
//
// Revision 1.4  2001/01/22 17:58:05  dave
// additional documentation added, some debuggin done
//
// Revision 1.3  2001/01/22 05:16:16  dave
// expanded functionality, need to write Test()
//
// Revision 1.2  2001/01/20 00:04:34  dave
// revision 1.1.3.2.3
//
// Revision 1.1  2001/01/19 17:21:16  dave
// first pass at File.h added, additions shortly
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/File.C,v 1.2 2006/07/06 15:42:33 romy Exp $
// $Id: File.C,v 1.2 2006/07/06 15:42:33 romy Exp $
