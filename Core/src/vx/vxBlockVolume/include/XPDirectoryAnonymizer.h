// XPDirectoryAnonimizer.h: interface for the XPDirectoryAnonimizer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef XPDIRECTORYANONYMIZER_H
#define XPDIRECTORYANONYMIZER_H

// includes
#include "File.h"
//#include "PersonName.h"


/**
 * Class for creating anonymous datasets one at a time.  Instance of "path/PersonName" will 
 * create new directory at "path/AnonymousName" and move all files in the original directory
 * to the new directory with anonymous file names.
 * Use of this within the V1k or V2k is very likely to cause memory problems under some conditions
 * this class could be removed entirely and the patients could be re-sent running in anonymous mode.
*/
class XPDirectoryAnonymizer
{
public:
  /// Progress callback function; caller can update UI or the like (for e.g. "Creating File X of Y")
  ///  an improvement to this would be with parameters: (const uint8 uBytesCopied, const uint8 uTotalBytes)
  //  Return true to continue, or false to abort
  typedef bool (* ProgressCallbackFunction)(const uint4 uCurrentFile, const uint4 uNumOfFiles, void * pData);

public:
  /// Constructor
  XPDirectoryAnonymizer(const std::string & sPath = "", const std::string & sPrefix = "");

  /// Anonymizes all files in directory "name" and places them in directory "anonName" at the same level
  bool Run(const std::string & sSiteID, const std::string & sStudyID,
           const XPDirectoryAnonymizer::ProgressCallbackFunction pCallbackFunc = XPDirectoryAnonymizer::ProgressCallbackFunction(NULL), 
           void * pUserData = NULL);

  /// Returns the full path of the source directory
  std::string GetSourceDir() const;

  /// Returns the full path of the anonymous directory
  std::string GetAnonymousDir() const;

  /// Returns the title for the files of the anonymous study
  std::string GetAnonymousFileTitle() const { return m_sNewAnonFileTitle; }

  /// Attempts to delete the original directory and files.  Dir must not contain sub-dirs other than "dcm"
  bool DeleteOriginalDirAndFiles() const;

  /// Deletes the Anonymized files created by Run() method.
  bool DeleteAnonymousDirAndFiles();
  


private:
  ///
  void DeleteDirectory(const std::string & sPath);

  /// Call to update progress during anonymization
  void ReportProgress(const uint4 uCurrentFile, const uint4 uNumOfFiles);

  /// a simple solution to anonymize .dcm file.
  bool AnonymizeDcmFile(const std::string & sSrc, const std::string & sDes,
                        const std::string & sFirstName, const std::string & sLastName );

  /// a simple solution to anonymize .txt/.xml file.
  bool AnonymizeTxtOrXmlFile(const std::string & sSrc, const std::string & sDes,
                             const std::string & sFirstName, const std::string & sLastName,
                             const std::string & sPrefix, const std::string & sMedicalID,
                             const std::string & sMI, const std::string & sOldRootFile,
                             const std::string & sNewRootFile, 
                             const std::string & sSiteID, const std::string & sStudyID,
                             bool bCombineSiteAndStudy = true);

  /// test if the patient name is constucted by digit.
  bool IsPatientNameDigitalOrSmallSize(const std::string & sRef);

private:
  /// Source directory (with machine name in UNC protocol)
  vx::File m_sourceDir;
  /// The destination (anonymized study) directory
  vx::File m_anonDir;
  /// New root name for the anonymized study
  std::string m_sNewAnonFileTitle;
  /// File prefix to use to parse files
  std::string m_sPrefix;
  
  /// Progress update callback function
  ProgressCallbackFunction m_pCallBackFunction;
  /// User-defined data for progress function
  void * m_pUserData;

  /// List of report filenames
  std::vector<std::string> m_vrxFileName;
};


#endif // !defined(XPDIRECTORYANONYMIZER_H)
