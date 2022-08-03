// $Id: XPDirectoryAnonymizer.C,v 1.4 2006/01/31 14:03:32 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: David dave@viatronix.com


// includes
#include "stdafx.h"
#include "XPDirectoryAnonymizer.h"
#include "SerializedDicom.h"
#include "Crypt.h"
#include "PersonName.h"
#include "PatientAnonymizer.h"
#include "Exception.h"
#include "vxBlockVolume.h"
#include "FileExtensions.h"
#include "Date.h"


// defines
#define FILE_REVISION  "$Revision: 1.4 $"


/**
 * Constructor.
 */
XPDirectoryAnonymizer::XPDirectoryAnonymizer(const std::string & sPath, const std::string & sPrefix) : 
  m_sourceDir(sPath), m_anonDir(""), m_sPrefix(sPrefix)
{
    m_sourceDir.ConvertUNCToIP();
}
  

/**
 * JRM:
 * Note:  This function -- and class -- is exceedingly overcoded and mangled far beyond necessity.  It needs 
 *          to be fully rethought -- clearly, with Requirements and Design -- and re-implementation from scratch.
 *
 * Anonymizes all files in directory "name" and places them in directory "anonName" at the same level
 * Dicom files are not currently copied or given much thought, as they won't fit on 1 cd and are a pain to
 * anonymize, we should re-create the dicom files from the vol if needed
 */
bool XPDirectoryAnonymizer::Run(const std::string& sSiteID, const std::string& sStudyID,
                              const XPDirectoryAnonymizer::ProgressCallbackFunction pCallbackFunc, void* pUserData)
                              
{

  m_pCallBackFunction = pCallbackFunc;
  m_pUserData = pUserData;


  uint4 uProgress(1);
 
  const int4 uTotalFiles = static_cast< int4>( m_sourceDir.ListFiles().size() - 2 + 10 );
  const std::string sSep(vx::File::GetPathSeparator());

  PersonName pnPatient;
  std::string sPatientID;

  std::string sNewRootFile("");
  std::string sOldRootFile("");



  vx::FileVector vVolumeFiles = m_sourceDir.ListFiles(SuffixFilter(vx::VolumeExt));
      
    
  //////////////////////////////////////////////////////
  // If not VOL files were found then we are in trouble
  /////////////////////////////////////////////////////    
  if ( vVolumeFiles.empty() ) // no .ecv files found.
  {
    throw ex::FileNotFoundException(LogRec("Study directory/files not found or not accessible: " + m_sourceDir.ToString(),
                                            "XPDirectoryAnonymizer", "Run"));
  }


  if (m_anonDir.ToString().empty())
  {
    vx::Date dt;
    const std::string sDirName = sSiteID + "_" +  sStudyID + "." + dt.DateAsString();
    std::string sRootName = std::string(getenv("TMP")) + sSep + sDirName;
    if (sRootName[0] != 'C' && sRootName[0] != 'D' && sRootName[0] != '/')
    {
      sRootName[0] = 'D'; // cheesy hack to correct error in writing PNG files to mapped network drives
    }
          
    m_anonDir = vx::File(sRootName);
  
    int1 sTempAnonPath[MAX_PATH+1];
    if (GetTempFileName(m_anonDir.GetParent().c_str(), m_anonDir.GetName().c_str(), 0, sTempAnonPath) != 0)
    {
      DeleteFile(sTempAnonPath);
      m_anonDir = vx::File(sTempAnonPath);
    }

    m_anonDir.Mkdirs();
           
    LogFyi("Create Directory : " + m_anonDir.ToString(), "XPDirectoryAnonymizer","Run");
        
  }

  m_sNewAnonFileTitle = m_anonDir.ToString() + sSep + sSiteID + "_" + sStudyID + "_";


  try
  {

    for (uint4 i(0); i < vVolumeFiles.size(); ++i )
    {
      
      std::string sVolumeFile(vVolumeFiles[i].ToString());

      const std::string sFileName(vVolumeFiles[i].GetName());

      // If the volume file is a session then do not anonymize
      if ( sFileName.find("-",0) != std::string::npos )
      {
        const std::string sDest(vx::File(m_sNewAnonFileTitle).GetParent() + vx::File::GetPathSeparator() + sFileName);

        LogDbg("Copying session volume " + sFileName, "XPDirectoryAnonimizer", "Run");

        CopyFile(sVolumeFile.c_str(), sDest.c_str(), false);
        continue;
      }

      LogDbg("Anonymizing VOL File: " + sFileName, "XPDirectoryAnonymizer","Run");
        
      vxBlockVolume<uint2> hVol;

      hVol.Read(sVolumeFile, std::pair< float4, float4 >( 0.0F, 100.0F ) );

      VarMap map;
      map.CreateMap(hVol.GetHeader().GetVariableVolumeHeader().ToString_1());

      std::string sVarHeader;
      if ( map.GetHdrVar("varHeader", sVarHeader) )
      {

        VarMap varHdr(sVarHeader);
        std::string sPatientName;

        if ( varHdr.GetHdrVar("patientName",sPatientName) )
        {
          varHdr.SetHdrVar("patientName", sSiteID + "-" + sStudyID, "Anonymous Patient Name");
        }
        else
        {
          hVol.GetHeader().GetHeaderEntry(vxVolumeHeader::PatientName, sPatientName);
        }

        if ( varHdr.GetHdrVar("patientID",sPatientID) )
        {
          varHdr.SetHdrVar("patientID", " ", "Anonymous Patient ID");
        }
        else
        {
          hVol.GetHeader().GetHeaderEntry(vxVolumeHeader::PatientID, sPatientID);
        }
      
        
        pnPatient.SetDicomName(sPatientName);
        

        std::string sDicomHeader;
        if ( varHdr.GetHdrVar("dicomHeader", sDicomHeader) )
        {
          SerializedDicom serDicom;

          std::istrstream ss(sDicomHeader.c_str());
          ss >> serDicom;

          if ( ! serDicom.GetFieldValue("0010", "0010", "" ).empty() )
          {
            serDicom.SetFieldValue("0010", "0010", sSiteID + "-" + sStudyID);
          }

          if ( ! serDicom.GetFieldValue("0010", "0020", "" ).empty() )
          {
            serDicom.SetFieldValue("0010", "0020", " ");
          }

          varHdr.SetHdrVar("dicomHeader", serDicom.ToString(), "Serialized Dicom Header"  );

       }

        hVol.GetHeader().SetVariableVolumeHeader(varHdr);
      }

      ReportProgress(uProgress, uTotalFiles);

      const int4 iFirstUB = static_cast< int4>( vVolumeFiles[i].GetName().find_first_of("_") );
      const int4 iLastUB = static_cast< int4>( vVolumeFiles[i].GetName().find_last_of("_"));
      std::string sDest("");

      if ( iLastUB != iFirstUB )
      {

        const int4 iStartPos =  iLastUB - 2;

        sDest = (iStartPos > 0 ? m_sNewAnonFileTitle : std::string(vx::File(m_sNewAnonFileTitle).GetParent() + vx::File::GetPathSeparator()) ) + ( iStartPos >= 0 ? vVolumeFiles[i].GetName().substr(iStartPos) : vVolumeFiles[i].GetName());
      }
      else
      {
        sDest = m_sNewAnonFileTitle + vVolumeFiles[i].GetName().substr(iLastUB + 1);
      }

      sOldRootFile = vVolumeFiles[i].GetTitle();
      sNewRootFile = vx::File(sDest).GetTitle();

      hVol.Write(sDest);

            
      uProgress++;

              
    } // END ... For each volume file

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// R  S  P   F  I  L  E  /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
      vx::FileVector vRspFiles = m_sourceDir.ListFiles(SuffixFilter(".rsp"));

      for ( uint4 j(0); j < vRspFiles.size(); j++ )
      {

        const int4 iLastUB = static_cast< int4>( vRspFiles[j].GetName().find_last_of("_"));

        const std::string sDest = m_sNewAnonFileTitle + vRspFiles[j].GetName().substr(iLastUB + 1);

        CopyFile(vRspFiles[j].ToString().c_str(), sDest.c_str(), true);

        ReportProgress(uProgress, uTotalFiles);

        uProgress++;

      } // END ... FOr each RSP file in the directoyr

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// D  C  M   F  I  L  E  /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
      vx::FileVector vDicomFiles = m_sourceDir.ListFiles(SuffixFilter(vx::DicomExt));

      for ( uint4 j(0); j < vDicomFiles.size(); j++ )
      {
        
        const int4 iFirstUB = static_cast< int4>( vDicomFiles[j].GetName().find_first_of("_"));
        const int4 iLastUB = static_cast< int4>( vDicomFiles[j].GetName().find_last_of("_"));
        std::string sDcmSuffix("");

        if ( iLastUB != iFirstUB )
        {

          const int4 iStartPos =  iLastUB - 2;

          sDcmSuffix = (iStartPos > 0 ? vDicomFiles[j].GetName().substr(iStartPos) : vDicomFiles[j].GetName());

        }
        else
        {

          std::string sDcmSuffix = vDicomFiles[j].GetName().substr(iLastUB + 1);

        }

        ReportProgress( uProgress, uTotalFiles);

        bool bDcmSuccess = AnonymizeDcmFile(vDicomFiles[j].ToString(), m_sNewAnonFileTitle + sDcmSuffix, pnPatient.GetFirst(), pnPatient.GetLast());

        uProgress ++;
      
      } // END ... for each DICOM file
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// X  M  L   F  I  L  E  /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
      vx::FileVector vXmlFiles = m_sourceDir.ListFiles(SuffixFilter(vx::XMLExt));

      for ( uint4 j(0); j < vXmlFiles.size(); j++ )
      {

        const int4 iStartPos = static_cast< int4>( vXmlFiles[j].GetName().find_last_of("_") - 2);

        ReportProgress( uProgress, uTotalFiles);

        const std::string sDestXmlFile(m_sNewAnonFileTitle + vXmlFiles[j].GetName().substr(iStartPos));

        const std::string sPrefix = (iStartPos > 0 ? vXmlFiles[j].GetName().substr(0, iStartPos-1) : vXmlFiles[j].GetName() );

        if ( !AnonymizeTxtOrXmlFile(vXmlFiles[j].ToString(), sDestXmlFile, pnPatient.GetFirst(), pnPatient.GetLast(),
                                    sPrefix, sPatientID, pnPatient.GetMiddle(), sOldRootFile, sNewRootFile, sSiteID, sStudyID) )
        {
          CopyFile(vXmlFiles[j].ToString().c_str(), sDestXmlFile.c_str(), FALSE);
        }

        uProgress ++;
      } // END ... for each XMP file
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// T  X  T   F  I  L  E  /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
      vx::FileVector vTxtFiles = m_sourceDir.ListFiles(SuffixFilter(vx::TXTExt));

      for ( uint4 j(0); j < vTxtFiles.size(); j++)
      {

        const int4 iStartPos = static_cast< int4>( vTxtFiles[j].GetName().find_last_of("_") - 2 );

        ReportProgress( uProgress, uTotalFiles);


        const std::string sDestTxtFile = m_sNewAnonFileTitle + vTxtFiles[j].GetName().substr(iStartPos);
        const std::string sPrefix = (iStartPos > 0 ? vTxtFiles[j].GetName().substr(0, iStartPos-1) : vTxtFiles[j].GetName() );

        if ( !AnonymizeTxtOrXmlFile(vTxtFiles[j].ToString(), sDestTxtFile, pnPatient.GetFirst(), pnPatient.GetLast(),
                                    sPrefix, sPatientID, pnPatient.GetMiddle(), sOldRootFile, sNewRootFile, sSiteID, sStudyID) )
        {
          CopyFile(vTxtFiles[j].ToString().c_str(), sDestTxtFile.c_str(), FALSE);
        }

        uProgress ++;
       
      } // END ... for each text file
    }
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////// X M L   D I R E C T O R Y   /////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////     
    {
      vx::File dirXML(m_sourceDir.ToString() + sSep + "Xml");
    
      if ( dirXML.Exists() )
      {

        LogDbg("Copying xml directory to " + m_anonDir.ToString(), "DirectoryAnonimizer", "Run");        

        vx::File dirAnonXML(m_anonDir.ToString() + sSep + "Xml");

        if ( dirAnonXML.Mkdir() )
        {

          vx::FileVector vFiles = dirXML.ListFiles();
          
          for ( uint4 j(0); j < vFiles.size(); j++ )
          {
            

            ///////////////////////////////////////////////////////////////////////////////////
            // If the file is the '.', '..' or a REPORT or MODULES file then ignore it
            ///////////////////////////////////////////////////////////////////////////////////
            if ( vFiles[j].GetName()  == "." || vFiles[j].GetName() == ".." || 
                 vFiles[j].GetName().find("REPORT") != std::string::npos  ||
                 vFiles[j].GetName().find("MODULES") != std::string::npos )
              continue;

            ReportProgress( uProgress, uTotalFiles); uProgress++;

            const std::string sDestXmlFile = dirAnonXML.ToString() + sSep + vFiles[j].GetName();
            
            if ( !AnonymizeTxtOrXmlFile(vFiles[j].ToString(), sDestXmlFile, pnPatient.GetFirst(), pnPatient.GetLast(),
                                        vFiles[j].GetName(), sPatientID, pnPatient.GetMiddle(), sOldRootFile, sNewRootFile, 
                                        sSiteID, sStudyID, false) )
            {
              CopyFile(vFiles[j].ToString().c_str(), sDestXmlFile.c_str(), FALSE);
            }
            
        
          } // END ... For each XMP file

        } // END ... If the directories were made

      } // END ... If the XMP directory exists

    }   

    ReportProgress(uTotalFiles, uTotalFiles);
      
  } 
  catch ( ex::AbortOperationException & )
  {
    throw;
  } // catch AbortOperationException
  catch ( ex::VException &  )
  {
    throw;
  } // catch VException
  catch ( ... )
  {
    throw ex::IOException(LogRec("Could not anonymize study", "XPDirectoryAnonymizer", "Run"));
  }

  return true;

} // Run


/**
 * Return the source directory as a string
 */
std::string XPDirectoryAnonymizer::GetSourceDir() const
{
  return (m_sourceDir.ToString());
}


/**
 * Return the anonymous directory as a string
 */
std::string XPDirectoryAnonymizer::GetAnonymousDir() const
{
  return (m_anonDir.ToString());
}


/**
 * Attempts to delete the original directory and files.  Dir must not contain sub-dirs other than "dcm"
 */
bool XPDirectoryAnonymizer::DeleteOriginalDirAndFiles() const
{
  LogDbg("Deleting original directory : " + m_sourceDir.ToString(), "XPDirectoryAnonymizer", "DeleteOriginalDirAndFiles");

  vx::File dcmDir(m_sourceDir.ToString() + "/dcm");

  LogTmp("Attempting to delete dcm sub-directory : " + dcmDir.ToString(), "XPDirectoryAnonymizer", "DeleyteOriginalDirAndFiles");
    
  if ( dcmDir.Exists() && dcmDir.IsDirectory() )
  {
    vx::FileVector vDcmFiles = dcmDir.ListFiles(); // not SuffixFilter b/c SLICECOUNT file
    for(uint4 ii(0); ii < vDcmFiles.size(); ++ii)
    {
      vDcmFiles[ii].Delete();
    }
  }
  dcmDir.Delete();

  vx::FileVector vFiles = m_sourceDir.ListFiles();
  for(uint4 iCurrFile(0); iCurrFile < vFiles.size(); ++iCurrFile)
  {
    LogTmp("Deleting : " + vFiles[iCurrFile].GetName(), "XPDirectoryAnonymizer", "DeleyteOriginalDirAndFiles");
    
    vFiles[iCurrFile].Delete();
  }

  return (m_sourceDir.Delete());
}


/**
* Deletes the Anonymized files created by Run() method.
*/
bool XPDirectoryAnonymizer::DeleteAnonymousDirAndFiles()
{
  // could check for dir "dcm" and delete the contents
  
  LogDbg("Deleting Anonymous Directory : " + m_anonDir.ToString(), "DirectoryAnonimyzer", "DeleteAnonymousDirAndFiles");
  
  vx::FileVector vFiles = m_anonDir.ListFiles();
  for(uint4 i(0); i < vFiles.size(); ++i)
  {
    
    if ( vFiles[i].GetName() == "." || vFiles[i].GetName() == ".." )
      continue;
    
    
    
    if ( vFiles[i].IsDirectory() )
    {
      DeleteDirectory(vFiles[i].ToString());
    }
    else
    {
      LogTmp("Deleting Anonymous File : " + vFiles[i].GetName() , "DirectoryAnonimyzer", "DeleteAnonymousDirAndFiles");
      
      vFiles[i].Delete();
    }
  }
  
  return (m_anonDir.Delete());
  
}


/**
* Deletes a directory recursivly
*/
void XPDirectoryAnonymizer::DeleteDirectory(const std::string & sPath)
{

  LogDbg("Deleting Anonymous Directory : " + sPath, "DirectoryAnonimyzer", "DeleteDirectory");
  
  vx::File path(sPath);

  vx::FileVector vFiles = path.ListFiles();
  for(uint4 i(0); i < vFiles.size(); ++i)
  {
   
    
    if ( vFiles[i].GetName() == "." || vFiles[i].GetName() == ".." )
      continue;
      
    
    if ( vFiles[i].IsDirectory() )
    {
      DeleteDirectory(vFiles[i].ToString());
    }
    else
    {
      LogTmp("Deleting Anonymous File : " + vFiles[i].GetName() , "DirectoryAnonimyzer", "DeleteAnonymousDirAndFiles");      
      vFiles[i].Delete();
    }
  }

  path.Delete();
  
}





/**
 * Report progress back to calling procedure
 */
void XPDirectoryAnonymizer::ReportProgress(const uint4 uCurrentFile, const uint4 uNumOfFiles)
{
  ///////////////////////////////////////////////
  // Only update if the callback function was set
  //////////////////////////////////////////////////
  if (m_pCallBackFunction != NULL)
  {
    if (m_pCallBackFunction(uCurrentFile, uNumOfFiles, m_pUserData) == false)
    {
      throw ex::AbortOperationException(LogRec("User aborted operation","XPDirectoryAnonymizer","Run"));
    } // END ... if callback function returns false

  } // END ... If the callback function was defined
  
  
}


/*
 * replace the patient first name with iStudyID, last name with iSiteID
 * (processing .dcm file.)
 * 
 * @param sSrc  source file
 * @param sDex  destination file
 * @return true when success transfer
 */
bool XPDirectoryAnonymizer::AnonymizeDcmFile(const std::string & sSrc, const std::string & sDes,
                                            const std::string & sFirstName, const std::string & sLastName )
{
  std::vector<char> vTmpBuf;
  char ch;

  std::ifstream input(sSrc.c_str(), std::ios::in | std::ios::binary);
  vTmpBuf.clear();
  while (input)
  {
    input.get(ch);
    vTmpBuf.push_back(ch);
  }
  input.close();

  bool bFindFirst = false;
  bool bFindSecond = false; 
  bool bFindBoth = false;
  int4 iFirstPos = 0;
  int4 iSecondPos = 0;

  LogDbg("Anonymizing DCM File: [" + sSrc + "] To [" + sDes + "]" , "XPDirectoryAnonymizer","AnonymizeDcmFile");
  
  for (uint4 i=0; i < (vTmpBuf.size()-10); i++)
  {
    if (vTmpBuf[i] == sFirstName[0] && (bFindFirst == false))
    {
      bool bFind = true;

      // match the first name.
      for (uint4 j=1; j<sFirstName.size(); j++)
      {
        if (sFirstName[j] != vTmpBuf[i+j])
        {
          bFind = false;
          break;
        }
      }
      if (bFind == true)
      {
        // iFirstPos = i;
        for (uint4 k=0; k<sFirstName.size(); k++) vTmpBuf[i+k] = 'X';
        bFindFirst = true;
      }
    }
    
    // match the last name.
    if (vTmpBuf[i] == sLastName[0] && bFindSecond == false)
    {
      bool bFind = true;
      for (uint4 j=1; j<sLastName.size(); j++)
      {
        if (sLastName[j] != vTmpBuf[i+j])
        {
          bFind = false;
          break;
        }
      }
      if (bFind == true)
      {
        for (uint4 k=0; k<sLastName.size(); k++) vTmpBuf[i+k] = 'X';
        bFindSecond = true;
        // iSecondPos = i;
      }
    }

    if (bFindFirst && bFindSecond)
    {
      bFindBoth = true;
      break;
    }
  } // find the last and first patient name.

  // if not find ...
  if ((bFindFirst == false) && (bFindSecond == false)) 
  {
    return false;
  }

  std::ofstream output(sDes.c_str(), std::ios::out | std::ios::binary);
  for (uint4 k=0; k<vTmpBuf.size()-1; k++)
  {
    output << vTmpBuf[k];
  }

  vTmpBuf.clear();
  output.close();

  return true;
} // XPDirectoryAnonymizer()


/*
 * replace the patient first name with iStudyID, last name with iSiteID
 * (processing .txt and .xml log files)
 *
 * @param sSrc  source file
 * @param sDex  destination file
 * @param sSite Site ID
 * @param sStudyID  Study ID
 * @return true when success transfer
 */
bool XPDirectoryAnonymizer::AnonymizeTxtOrXmlFile(const std::string & sSrc, const std::string & sDes,
                                                const std::string & sFirstName, const std::string & sLastName, 
                                                const std::string & sPrefix, const std::string & sMedicalID, 
                                                const std::string & sMI, const std::string & sOldRootFile, 
                                                const std::string & sNewRootFile,
                                                const std::string & sSiteID, const std::string & sStudyID, 
                                                bool bCombineSiteAndStudy )
{

  LogDbg("Anonymizing TXT/XML File: [" + sSrc + "] To [" + sDes + "]" , "XPDirectoryAnonymizer","AnonymizeTxtOrXmlFile");
    
  // if First/Last is digital string, simple copy.
  bool bAllDight = IsPatientNameDigitalOrSmallSize(sFirstName);
  if (bAllDight==false) bAllDight = IsPatientNameDigitalOrSmallSize(sLastName);
  if (bAllDight==false) bAllDight = IsPatientNameDigitalOrSmallSize(sPrefix);

  // simple copy applied.
  if ( bAllDight == true )
  {
    CopyFile(sSrc.c_str(), sDes.c_str(), FALSE);
    return true;
  }

  std::ifstream input(sSrc.c_str(), std::ios::in | std::ios::binary);
  std::ofstream output(sDes.c_str(), std::ios::out | std::ios::binary);
  const int4 uMaxLength(4096);
  int1 sTempLine[uMaxLength+1];
  std::string sRepStr = sSiteID + "-" + sStudyID;
  std::string sNewPrefix = sStudyID;

  while (input)
  {
    input.getline(sTempLine, uMaxLength);
    std::string sStrLine(sTempLine);


    // replace the first name.
    int4 iSize = 0;

    // Replace Root File Name
    if ( ! sNewRootFile.empty() && ! sOldRootFile.empty() )
    {
      iSize = 0;
      do
      {
        iSize = static_cast< int4>( sStrLine.find(sOldRootFile, iSize) );
        if (iSize >= 0)
        {
          sStrLine.replace(iSize, sOldRootFile.size(), sNewRootFile, 0, sNewRootFile.size());
          iSize += 1;
        }
      } while (iSize != std::string::npos);
    }
    
    iSize = 0;
    do
    {
      iSize = static_cast< int4>( sStrLine.find(sFirstName, iSize) );
      if (iSize >= 0)
      {
        if ( bCombineSiteAndStudy )
        {
          sStrLine.replace(iSize, sFirstName.size(), sRepStr, 0, sRepStr.size());
          iSize += static_cast< int4>( sRepStr.size() );
        }
        else
        {
          sStrLine.replace(iSize, sFirstName.size(), sStudyID, 0, sStudyID.size());
          iSize += static_cast< int4>(  sStudyID.size() );          
        }
      }
    } while (iSize != std::string::npos);

    // replace the last name.
    iSize = 0;
    do
    {
      iSize = static_cast< int4>( sStrLine.find(sLastName, iSize) );
      if (iSize >= 0)
      {
        if ( bCombineSiteAndStudy )
        {
          sStrLine.replace(iSize, sLastName.size(), sRepStr, 0, sRepStr.size());
          iSize += static_cast< int4>( sRepStr.size() );
        }
        else
        {
          sStrLine.replace(iSize, sLastName.size(), sSiteID, 0, sSiteID.size());
          iSize += static_cast< int4>( sSiteID.size() );          
        }
      }
    } while (iSize != std::string::npos);

    // replace the Medical ID
    if ( ! sMedicalID.empty()  )
    {
      iSize = 0;
      do
      {
        iSize = static_cast< int4>( sStrLine.find(sMedicalID, iSize) );
        if (iSize >= 0)
        {
          sStrLine.replace(iSize, sMedicalID.size(), " ", 0, 1);
          iSize += 1;
        }
      } while (iSize != std::string::npos);
    }
    

    if ( ! sMI.empty() )
    {
      // replace the Middle name
      iSize = 0;
      do
      {
        iSize = static_cast< int4>( sStrLine.find(sMI, iSize) );
        if (iSize >= 0)
        {
          sStrLine.replace(iSize, sMI.size(), " ", 0, 1);
          iSize += 1;
        }
      } while (iSize != std::string::npos);
    }
    
    if ( ! sPrefix.empty() )
    {
      // replace the prefix.
      iSize = 0;
      do
      {
        iSize = static_cast< int4>( sStrLine.find(sPrefix, iSize) );
        if (iSize >= 0)
        {
          sStrLine.replace(iSize, sPrefix.size(), sNewPrefix, 0, sNewPrefix.size());
          iSize += static_cast< int4>( sNewPrefix.size() );
        }
      } while (iSize != std::string::npos);
    }

    output << sStrLine << std::endl;
  }

  input.close();
  output.close();

  return true;
} // AnonymizeTxtOrXmlFile


/**
 * Test if the patient name consists only of numeric characters (digits).
 *
 * @param const std::string sRef   Patient name to test
 * @return bool                    True if all in sRef are digits, false otherwise.
 */
bool XPDirectoryAnonymizer::IsPatientNameDigitalOrSmallSize(const std::string & sRef)
{
  // too small to be danger to do replacement.
  if ( sRef.size() < 3 )
  {
    return true;
  }

  // are all digits ?
  for (uint4 i=0; i<sRef.size(); i++)
  {
    if ( !isdigit(sRef[i]) ) 
    {
      return false;
    }
  }

  return true;
} // IsPatientNameDigitalOrSmallSize()


// undefines
#undef FILE_REVISION


// $Log: XPDirectoryAnonymizer.C,v $
// Revision 1.4  2006/01/31 14:03:32  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/XPDirectoryAnonymizer.C,v 1.4 2006/01/31 14:03:32 frank Exp $
// $Id: XPDirectoryAnonymizer.C,v 1.4 2006/01/31 14:03:32 frank Exp $
