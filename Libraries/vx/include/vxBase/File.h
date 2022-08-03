// $Id: File.h,v 1.3 2006/10/02 19:59:37 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dave (dave@viatronix.com)

#ifndef File_h
#define File_h

// includes
#include "FileNameFilter.h"

namespace vx 
{
/**
 * Subset of file operations provided by Java jdk1.0, reference that documentation for 
 * good cross-platform ideas or API expansion (java.io.File).  
 * Currently supports WINDOWS, UNIX
 * Behavior in DOS is undefined - 'bout time to get a real file system.
 * QT classes QFile & QDir support more operatrions than vx::File
 *
 * All operations are based on std::string so there is currently no wide-character support.
 */
  class VX_BASE_DLL File
  {
  // functions
  public:

    /// constructor
    File(const std::string & sPathName = std::string());
  
    /// equality comparison for files.
    bool operator==(const File & other) const;

    /// not equal comparison for files.
    bool operator!=(const File & other) const;

    /// returns the absolute pathname string of this abstract pathname.
    std::string GetAbsolutePath() const;

    /// returns the pathname minus the volume or UNC specifier
    std::string GetLocalPath() const;

    /// returns the UNC systen name or IP
    std::string GetUNCSystemName() const;

    /// converts the UNC system name portion of the path to an IP address
    std::string ConvertUNCToIP();

    /// returns the pathname string of this pathname.
    std::string ToString() const { return m_sName; }

    /// returns the name of the file or directory denoted by this abstract pathname.
    std::string GetName() const;

    /// returns the title of the file (the name minus the extension)
    std::string GetTitle() const;

    /// returns the full pathname minus the extension
    std::string GetFullTitle() const;

    /// returns the extension (i.e. string after the last period) of the file.
    std::string GetExtension() const;

    /// returns the pathname string of this pathname's parent, or null if this pathname does not name a parent directory.
    std::string GetParent() const;

    /// returns a File instance constructed from GetParent()
    File GetParentFile() const;

    /// returns the root drive of the path
    std::string GetRoot() const;

    /// tests whether the application can read the file denoted by this abstract pathname.  
    bool CanRead() const;

    /// tests whether the application can modify to the file denoted by this abstract pathname.  
    bool CanWrite() const;

    /// tests whether the file denoted by this abstract pathname exists.  
    bool Exists() const;

    /// Checks the existence of a file
    static bool Exists( std::string fileName );

    /// returns an int specifing the type of drive of the root directory
    uint4 GetDriveType() const;
    
    /// returns the numbee of files in the PATH
    static uint4 GetNumberOfFiles(const std::string & sPath);

    /// get file size
    static uint8 GetFileSize(const std::string & sFile);

    /// get file's attributes
    uint4 GetAttrib() const;

    /// adds the specified attribute(s) to the list of the file's attributes
    void AddAttrib(const uint4 uAttribBits) const;

    /// adds the specified attribute(s) to the list of the file's attributes
    void RemoveAttrib(const uint4 uAttribBits) const;

    /// sets the file's attributes to that specified
    void SetAllAttrib(const uint4 uAllAttribBits) const;

    /// tests whether the file denoted by this abstract pathname is a normal file.
    bool IsFile() const;

    /// tests whether the file denoted by this abstract pathname is a directory.
    bool IsDirectory() const;

    /// tests whether the denoted pathname is a UNC Path.
    bool IsUNCPath() const;

    /// deletes the file or directory denoted by this abstract pathname.
    bool Delete() const;

    /// teturns the length of the specified file
    const int8 FileLength() const;

    /// sets the working directory
    bool SetWorkingDir() const;

    /// returns the current working directory
    static File GetWorkingDir();

    /// creates the directory named by this abstract pathname.
    bool Mkdir() const;

    /// creates the directory named by this abstract pathname, including any necessary but nonexistent parent directories
    bool Mkdirs() const;

    /// renames the file denoted by this abstract pathname.
    bool RenameTo(const File & destFile);

    /// moves the file denoted by this abstract pathname.
    bool MoveTo(const File & destFile, bool bReplace = false) const;

    /// copies the file denoted by the abstract pathname to the new location.
    bool CopyTo(const File & destFile) const;

    /// returns the number of directories in the path (includes the . and .. )
    int4 GetDirectoryCount() const;

    /// returns null list if the directory is not readable or other I/O exceptions occur.
    std::vector<File> ListFiles(const FileNameFilter & filter = AcceptAllFilter()) const;

    /// recursively traverses the directory tree and returns all files that satisfy the given filter.
    std::vector<File> ListFilesRecur(const FileNameFilter & filter = AcceptAllFilter()) const;

    /// returns number of files that match the specified filter (or just the number of files in directory)
    uint4 GetCount(const FileNameFilter & filter = AcceptAllFilter()) const;

    /// atomically creates a new, empty file named by this abstract pathname if and only if a file with this name does not yet exist.  
    bool CreateNewFile() const;

    /// returns the study path for the supplied data
    static std::string GetStudyPath(const std::string & sStudyDir, int4 iStudyType, const std::string & sPatientLastName, const std::string & sPatientFirstName, const std::string & sScanDate, const std::string & sUID );
    
    /// creates an empty file in the default temporary-file directory, using the given prefix, suffix and a generated number to make sure it is unique. 
    static File CreateTempFile(std::string sPrefix, std::string sSuffix = std::string());

    /// List the available filesystem roots. *** NOT YET IMPLEMENTED *** 
    static std::vector<File> ListRoots();

    /// returns the path delimiter used for this OS.
    static std::string GetPathSeparator() { return m_sSeparator; }

    /// returns the local computer name
    static std::string GetComputerName();

    /// serialize the spaces out of the file/directory name by adding replacing it with a '*'
    static std::string SerializeFileName(const std::string sFileName);

    /// deserializes a previously seraialize file/dir name replaces * with spaces
    static std::string DeSerializeFileName(const std::string sFileName);

    /// test all functionality
    static void Test(std::ostream & os = std::cout);

  private:

    /// lists files recursively
    void ListFilesRecurHelper(const FileNameFilter & filter, File curFile, std::vector<File> & vFiles) const;

  // data
  protected:

    /// full path and file/dir name
    std::string m_sName;

    /// "/" for UNIX "\" for WINDOWS
    static std::string m_sSeparator; 

  private:

    /// study types
    static const char * m_StudyTypes[];
  }; // File

  typedef std::vector<vx::File> FileVector;
  typedef std::list<vx::File> FileList;

} // namespace vx


typedef std::vector<vx::File> FILELIST;


#endif // File_h


// $Log: File.h,v $
// Revision 1.3  2006/10/02 19:59:37  frank
// formatting
//
// Revision 1.2  2006/07/06 15:42:46  romy
// added File Exists method
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.23  2003/05/16 13:08:19  frank
// code formatting
//
// Revision 3.22  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.21  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.20  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.19  2002/07/30 16:49:51  mkontak
// Added GetFileSize()
//
// Revision 3.18  2002/07/30 15:36:30  mkontak
// Added GetNumberOfFIles() method for a PATH
//
// Revision 3.17  2002/07/30 15:34:17  mkontak
// Added GetNumberOfFIles() method for a PATH
//
// Revision 3.16  2002/07/18 14:16:45  mkontak
// Minor changes
//
// Revision 3.15  2002/07/02 20:44:12  geconomos
// Exporting classes .
//
// Revision 3.14  2002/05/16 19:24:19  mkontak
// The GetStudyPath now takes an additional parameter UID to
// be appended to the end of the path. If blank it will be ignored and
// the study path will be formatted the original way.
//
// Revision 3.13  2002/04/11 20:04:29  mkontak
// Fixed bug and added a FILELIST typedef
//
// Revision 3.12  2002/03/11 16:02:03  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.11  2002/03/11 15:19:45  mkontak
// Merged VC 1-2/CS-1 into main trunk
//
// Revision 3.10.2.2  2002/03/05 20:10:15  jmeade
// File attribute functions.
//
// Revision 3.10.2.1  2002/02/02 07:27:37  jmeade
// Strings and LogRecs.
//
// Revision 3.10.2.2  2002/03/05 20:10:15  jmeade
// File attribute functions.
//
// Revision 3.10.2.1  2002/02/02 07:27:37  jmeade
// Strings and LogRecs.
//
// Revision 3.10  2002/01/23 07:17:19  mkontak
// Pulled database dependency from GetStudyPath()
//
// Revision 3.9  2002/01/23 05:15:54  mkontak
// Added GetStudyPath() to retreive the built study path
//
// Revision 3.8  2002/01/24 02:16:54  jmeade
// GetParentFile() instance.
//
// Revision 3.7  2002/01/24 02:10:48  jmeade
// GetParentFile() instance.
//
// Revision 3.6  2002/01/17 03:03:05  jmeade
// Added const to function declarations where necessary.
//
// Revision 3.5  2002/01/10 23:03:35  jmeade
// A couple of collection typedefs for convenience.
//
// Revision 3.4  2002/01/10 02:19:58  jmeade
// Method to GetCount() of matching filenames in directory.
//
// Revision 3.3  2002/01/03 19:09:30  jmeade
// Function to get full path title (file title prefixed by full directory path).
//
// Revision 3.2  2001/12/31 20:54:58  mkontak
// Modified the GetComputerName() method to call the SystemInfo::GetNetBiosName()
//
// Revision 3.1  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Sep 28 2001 16:31:52   jmeade
// Removed exception throw for the awfully minute failure of SetWorkingDir
// 
//    Rev 2.2   Sep 20 2001 15:10:28   jmeade
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
// Revision 1.3  2001/09/12 17:06:43  mkontak
// Added MoveTo() method
//
// Revision 1.2  2001/09/07 16:15:24  jmeade
// comments
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.31  2001/08/27 20:52:19  mkontak
// Added two methods to serialize and deserialize file name so that
// spaces are converted to a '*' when serialized and back to a space when
// deserialized.
//
// Revision 1.30  2001/08/06 22:32:33  mkontak
// Added GetUNCSystemName() method
//
// Revision 1.29  2001/06/26 23:26:06  jmeade
// GetTitle() fn; coding conventions
//
// Revision 1.28  2001/06/20 19:52:10  jmeade
// GetDriveType(), GetUNCPath(); temp filename fn returns a long filename
//
// Revision 1.27  2001/06/06 19:43:46  jmeade
// GetComputerName() fn (though I'm not sure this is the place for it, can't think
// of a better for now)
//
// Revision 1.26  2001/06/01 15:34:33  jmeade
// GetRoot()
//
// Revision 1.25  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.24  2001/04/25 19:26:24  dave
// closer standards compliance
//
// Revision 1.23  2001/04/19 20:57:25  dave
// added comparision functions, test, variable names
//
// Revision 1.22  2001/04/13 00:12:29  jmeade
// code standards
//
// Revision 1.21  2001/04/12 18:51:09  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.20  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.19  2001/04/12 15:25:34  jmeade
// GetExtension(): gets the file's extension
//
// Revision 1.18  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.17  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.16  2001/04/03 17:04:21  jmeade
// file length query method
//
// Revision 1.15  2001/03/23 14:59:42  dave
// more standards compliant
//
// Revision 1.14  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.13  2001/03/13 17:25:26  dave
// added GetWorkingDir to complement SetWorkin'Dir
// added CopyTo(), both only work in Win now
//
// Revision 1.12  2001/03/12 16:45:54  jmeade
// Please guys, no aborts for minor failures!!!
//
// Revision 1.11  2001/03/09 22:27:22  soeren
// Added SetAsWorkingDir()
//
// Revision 1.10  2001/02/13 02:57:23  dave
// added FileNameFilter as new file, listRecusive in File.h
//
// Revision 1.9  2001/01/25 16:48:42  dave
// added some File.C stuff for Linux
//
// Revision 1.8  2001/01/23 19:59:54  jcohen
// Declaration of filter
//
// Revision 1.7  2001/01/23 16:37:07  dave
// corrected CreateTempFile again
//
// Revision 1.6  2001/01/23 13:20:33  frank
// Supported the creation of a unique file path.
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
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/File.h,v 1.3 2006/10/02 19:59:37 frank Exp $
// $Id: File.h,v 1.3 2006/10/02 19:59:37 frank Exp $
