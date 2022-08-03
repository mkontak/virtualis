// $Id: VxVCReport.C,v 1.15.2.11 2010/03/05 08:54:34 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxVCReport.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net
#include "stdafx.h"
#include "afxdisp.h"
#include "ShellApi.h"
#include "VxVCReport.h"
#include "VxReportCreator.h"
#include "VxConsole.h"
#include "PersonName.h"
#include "File.h"
#include "VCResource.h"
#include "FileExtensions.h"
#include "User.h"
#include "ReaderGlobal.h"

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

#include "ReportVariant.h"


#define FILE_REVISION "$Revision: 1.15.2.11 $"

ReportLib::IReportRevisionPtr VxVCReport::m_spRevision;
std::string VxVCReport::m_sReportFile;
std::string VxVCReport::m_sXMLFilename;
ReportLib::IReportPtr VxVCReport::m_spReport;
bool VxVCReport::m_bReportMarkedComplete(false);

using namespace ConsoleLib;
using namespace ReaderLib;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Returns whether or not the specified report is marked as complete.
 *
 * @param sFile   The report pathname.
 * @return        Whether or not the specified report is marked as complete.
 */
bool VxVCReport::IsReportMarkedCompleted(const std::string & sFile) throw(ex::IOException)
{
  bool bMarkedCompeted(false);
  try
  {
    if (!vx::File(sFile.c_str()).Exists())
    {
      throw ex::FileNotFoundException(LogRec(sFile, "VxVCReport", "IsReportMarkedCompleted"));
    }

    ReportLib::IReportPtr spReport;
    if (FAILED(spReport.CreateInstance( __uuidof(ReportLib::Report))))
    {
      throw ex::IOException(LogRec("Failed to create IReportPtr instance", "VxVCReport", "IsReportMarkedCompleted"));
    }
    spReport->Open(_bstr_t(sFile.c_str()));

    ReportLib::IReportRevisionPtr spRevision;
    spRevision = spReport->Revisions->GetItem(0L);

    bMarkedCompeted = spRevision->MarkedCompleted == VARIANT_TRUE ? true : false;

    spRevision->Close();
    spRevision = NULL;

    spReport->Close();
    spReport = NULL;
  }
  catch(_com_error& e)
  {
    //ReportWndBase::DisplayReportError(IDS_OPEN, e.Error());
    CString sMsg; sMsg.Format("com error %d while attempting to read report.", e.Error());
    throw ex::IOException(LogRec(LPCTSTR(sMsg), "VxVCReport", "IsReportMarkedCompleted"));
  }
  return bMarkedCompeted;
} // IsReportMarkedCompleted()


/**
 * Returns whether or not the currently open report is marked as complete.
 *
 * @return        Whether or not the specified report is marked as complete.
 */
bool VxVCReport::IsReportMarkedCompleted()
{
  return m_bReportMarkedComplete;
} // IsReportMarkedCompleted()


/**
 * Sets the 'Marked Complete' flag for the study.
 *
 * @param bCompleted   Indicates whether or not the report is complete.
 * @return             Indicates success/failure
 */
bool VxVCReport::SetMarkedCompleteFlag(const bool bCompleted)
{
  try
  {
    if (m_spRevision==NULL)
    {
      return false;
    }
    m_spRevision->MarkedCompleted = bCompleted ? VARIANT_TRUE : VARIANT_FALSE;
		
    // Save the revision after changing the flag
    if( S_OK != m_spRevision->Save() )
		  LogErr("Fail to save m_spRevision", "VxVCReport", "SetMarkedCompleteFlag");

    m_bReportMarkedComplete = bCompleted;
  }
  catch(_com_error&)
  {
    throw;
  }

  return true;
} // SetMarkedCompleteFlag()


/**
 * Opens the specified report file
 *
 * @param hdrVol            Volume header instance, used for reading demographic and other info.
 * @param sReportFilename   Report file to open
 * @return                  Success of opening the report file
 */
bool VxVCReport::OpenReportPreview(vxVolumeHeader& hdrVol, const std::string & sReportFilename)
{

  if (!vx::File(sReportFilename).Exists())
  {
    CString sLoadError;
    VERIFY(sLoadError.LoadString(IDS_E_LOAD_REPORT));
    return false;
  }

  const std::string sPDFName(GeneratePDFReport(hdrVol, sReportFilename));
  vx::File pdfFile(vx::File(sPDFName).ConvertUNCToIP());
  if (sPDFName.empty() || !pdfFile.Exists())
  {
    LogErr("Unable to create temporary file for report preview.", "VxVCReport", "OpenReport");
    const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    return false;
  }

  return (ShellExecute(NULL, "open", pdfFile.ToString().c_str(), NULL, ".", SW_SHOWMAXIMIZED) > HINSTANCE(32));
} // OpenReportPreview()


/**
 * Query whether a report file is open.
 *
 * @param eStudyType          Type of study for the report file.
 * @param sFilePathAndTitle   File path and title to the study.
 * @return                    True if the report is open.
 */
bool VxVCReport::IsReportOpen(const std::string & sFilePathAndTitle)
{
  vx::File studyFile(sFilePathAndTitle);
  return IsReportOpen(studyFile.GetParent(), studyFile.GetTitle());
} // IsReportOpen()


/**
 * Query whether a report file is open.
 *
 * @param eStudyType   Type of study for the report file.
 * @param sFilePath    File path and title to the study.
 * @param sFileTitle   File path and title to the study.
 * @return             True if the report is open.
 */
bool VxVCReport::IsReportOpen(const std::string & sFilePath, const std::string & sFileTitle)
{
  std::string sReportFile = GenerateReportFilename(sFilePath + sFileTitle);
  CFile sFile;
  CFileException ex;

  // check if the file exists  
  if ( vx::File(sReportFile).Exists() )
  {
    // if the report exists check if can be opened
		if (!sFile.Open(sReportFile.c_str(), CFile::modeRead | CFile::shareExclusive, &ex))
    {
      return true;
    }

    sFile.Close();
  }

  return false;
} // IsReportOpen()


/**
 * Returns the report filename from the specified study filename.
 *
 * @param sStudyFilename   The report's study.
 * @return                 Report filename for the study.
 */
std::string VxVCReport::GenerateReportFilename(const std::string & sStudyFilename)
{
  if (sStudyFilename.empty())
    return std::string();

  const vx::File    studyFile(sStudyFilename);
  const std::string sParent(studyFile.GetParent());
  
	// Include the parent path if non-empty
	if (!sParent.empty())
	  m_sXMLFilename = sParent + vx::File::GetPathSeparator() + "vxrpt.xml";
	else
	  m_sXMLFilename = "vxrpt.xml";

	/// set the working vrx file path
	int1 sTempDir[MAX_PATH + 1];
	const int4 iPathStringLen(GetTempPath(MAX_PATH, sTempDir));
	GetLongPathName(sTempDir, sTempDir, MAX_PATH);

	std::string sReportFile = std::string(sTempDir);

	if (sReportFile.size() > 0)
	{
		sReportFile = sReportFile + "vxrpt" + std::string(vx::ReportExt);
		return sReportFile;
	}
	else
		return std::string();

} // GenerateReportFilename()


/**
 * Generates PDF report
 *
 * @param hdrVol            Volume header instance, used for reading demographic and other info.
 * @param sReportFilename   Name of the report file (.vrx)
 * @return                  Name of the newly generated PDF file
 */
std::string VxVCReport::GeneratePDFReport(vxVolumeHeader& hdrVol, std::string sReportFilename)
{
  if (!vx::File(sReportFilename).Exists())
  {
    LogErr("Report file doesn't exist.", "VxVCReport", "GeneratePDFReport");
    return std::string();
  }

  // If the file is read-only, copy it to a temp file to use before continuing
  DWORD dwFileAttribs(GetFileAttributes(sReportFilename.c_str()));
  if (dwFileAttribs & FILE_ATTRIBUTE_READONLY)
  {
    try
    {
      const std::string sTempRpt(vx::File::CreateTempFile(std::string(vx::ReportExt).substr(1,3),
        std::string(vx::ReportExt).substr(1,3)).ToString());

      if (CopyFile(sReportFilename.c_str(), sTempRpt.c_str(), FALSE) == FALSE)
      {
        throw ex::IOException(LogRec("Unable to copy .vrx file.", "VxVCReport", "GeneratePDFReport"));
      }

      // Unset read-only flag
      SetFileAttributes(sTempRpt.c_str(), dwFileAttribs & ~FILE_ATTRIBUTE_READONLY);
      sReportFilename = sTempRpt;
    }
    catch(ex::IOException &)
    {
      LogErr("Unable to create temporary .vrx file.", "VxVCReport", "GeneratePDFReport");
      return std::string();
    }
  }

  if (OpenReportFile(hdrVol, sReportFilename.c_str()))
  {
    // create temporary file name for PDF document
    CString sPDFFilename;
    try
    {
      sPDFFilename = vx::File::CreateTempFile(
        std::string(vx::ReportExt).substr(1,3), std::string(vx::AdobePDFExt).substr(1,3)).ToString().c_str();
    }
    catch(ex::IOException &)
    {
      LogErr("Unable to create temporary .pdf file.", "VxVCReport", "GeneratePDFReport");
      return std::string();
    }

    // create the PDF document
    VxReportCreator rptCreator(VxVCReport::m_spRevision);
    BOOL bSuccess = rptCreator.Create(sPDFFilename);

    // issue 3594: 2nd attempt to create file if it failed by success was reported by report creator
    if (!vx::File(LPCTSTR(sPDFFilename)).Exists())
    {
      LogErr("2nd report preview attempt: " + sReportFilename, "GeneratePDFReport", "VXVCReport");
      bSuccess = rptCreator.Create(sPDFFilename);
    }

    CloseReport(false, true);

    if (bSuccess)
    {
      return LPCTSTR(sPDFFilename);
    }
  }

  return std::string();
} // GeneratePDFReport()//


/**
 * Opens a report.
 *
 * @param hdrVol            Volume header instance, used for reading report info.
 * @param sStudyTitle       Study filename (path and) title.
 * @param bReadOnly         Whether to open report as read-only.
 * @param bHasPermissions   Whether current user has permission to modify report.
 * @param bReportCreated    Returns true if a (temporary) report file was generated (e.g. when bReadOnly is true)
 * @return                  Success/failure of report open.
 */
BOOL VxVCReport::OpenStudyReport(vxVolumeHeader& hdrVol, const std::string& sStudyTitle, const bool bReadOnly,
                                 const bool bHasPermissions, bool& bReportCreated)
{
  m_sReportFile = VxVCReport::GenerateReportFilename(sStudyTitle).c_str();

	/// Is there an vrx file in the session directory? If there is, move to the working directory
	{
		const vx::File    studyFile(sStudyTitle);
	  const std::string sParent(studyFile.GetParent());

	  // Go to the parent path if non-empty
	  if (!sParent.empty())
	    _chdir(sParent.c_str());

	  struct _finddata_t c_file;
	  intptr_t hFile;

	  std::string sOldVersionRptName = "*" + vx::ReportExt;

	  /// search in the session directory to find existing VRX file. If it exists, move to the user temporary directory
	  if ((hFile = _findfirst(sOldVersionRptName.c_str(), &c_file)) != -1)
	  {
	    sOldVersionRptName = c_file.name;

			if (!MoveFileEx(sOldVersionRptName.c_str(), m_sReportFile.c_str(), MOVEFILE_REPLACE_EXISTING))
			{
				LogErr("Could not setup temporary report file: " + m_sReportFile, "VxVCReport", "GenerateReportFilename");
			}
			else
			{
				if (!rdrGlobal.m_bReportCreated)
					rdrGlobal.m_bReportCreated = true;
			} // else
	  } // if

	  _findclose(hFile);

	} // if

	vx::File reportXMLFile(m_sXMLFilename);
	vx::File reportVRXFile(m_sReportFile);

  // Create a temporary report file if one doesn't exist *and* (the study is read-only or user cannot modify the report)
  const bool bCreateTempPDF( !reportXMLFile.Exists() && !reportVRXFile.Exists() && (bReadOnly || !bHasPermissions) );

	/// we need to create a temporary .PDF file
	if (bCreateTempPDF || bReadOnly || !bHasPermissions)
	{
		const std::string sTempReportFile =
			vx::File::CreateTempFile(reportVRXFile.GetTitle(), std::string(vx::ReportExt).substr(1, 3)).ToString();

		if (reportVRXFile.Exists())
		{
			if (!CopyFile(reportVRXFile.ToString().c_str(), sTempReportFile.c_str(), FALSE)) 
				return FALSE;
		  
			/// remove read-only attribute
		  vx::File(sTempReportFile).RemoveAttrib(FILE_ATTRIBUTE_READONLY);
		} // if

		m_sReportFile = sTempReportFile.c_str();
	} // if

  const bool bTmpOpened(OpenReportFile(hdrVol, m_sReportFile.c_str(), bCreateTempPDF) == TRUE);

  // unlock NavigationBar.
  bReportCreated = true;

  return bTmpOpened;
} // OpenStudyReport()


/**
 * Opens the specified report.
 *
 * @param hdrVol             Volume header instance, used for reading demographic and other info.
 * @param pszReport          Report pathname.
 * @param bEmptyReportFile   Whether or not to erase all current report information upon opening.
 * @return                   Success of report open.
 */
BOOL VxVCReport::OpenReportFile(vxVolumeHeader& hdrVol, LPCTSTR pszReport, bool bEmptyReportFile)
{
  CloseReport(true, false);
  LogDbg("CreateInstance", "VxVCReport", "OpenReportFile");
  if (FAILED(m_spReport.CreateInstance( __uuidof(ReportLib::Report))))
  {
    return FALSE;
  }

  m_bReportMarkedComplete = false;

  try
  {
		if(!vx::File(pszReport).Exists() && vx::File(m_sXMLFilename).Exists() && !bEmptyReportFile)
		{
			/// create VRX from XML report
			m_spReport->Create(_bstr_t(pszReport));
			m_spRevision = m_spReport->Revisions->Add();

			// create the PDF document
			VxReportCreator rptCreator(m_spRevision);

			/// raed the XML report file from the v3D Enterprise system
			rptCreator.ReadXMLReport();
		}
		else if (vx::File(pszReport).Exists() && !bEmptyReportFile)
    {
      m_spReport->Open(_bstr_t(pszReport));
      m_spRevision = m_spReport->Revisions->GetItem(0L);

      //{{ for compatibility... 
      {
        ReportLib::IReportInfoPtr spInfo = m_spRevision->Info;
        // PatientName
        if (spInfo->PatientName.length() == 0)
        {
          spInfo->PatientName = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientName").c_str();
        }

        // PatientID
        if (spInfo->PatientID.length() == 0)
        {
          spInfo->PatientID = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientID").c_str();
        }

        // ReferringDoctor
        if (spInfo->ReferringDoctor.length() == 0)
        {
          spInfo->ReferringDoctor = VxConsole::GetVolumeHeaderVariable(hdrVol, "physicianName").c_str();
        }

        if (spInfo->Author.length() == 0)
        {
          spInfo->Author = User::GetCurrentUser().GetFullName().c_str();
        }

        // InstitutionName
        if (spInfo->InstitutionName.length() == 0)
        {
          spInfo->InstitutionName = VxConsole::GetVolumeHeaderVariable(hdrVol, "institutionName").c_str();
        }

        // PatientWeight/Sex/StudyDate
        if (spInfo->PatientWeight.length() == 0)
        {
          // Weight
          spInfo->PatientWeight = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientWeight").c_str();

          // Sex
          CString sSex = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientSex").c_str();
          if ( sSex == "M")
          {
            spInfo->PatientSex = ReportLib::Male;
          }
          else if( sSex == "F")
          {
            spInfo->PatientSex = ReportLib::Female;
          }

          // StudyDate
          CString sStudyDate = VxConsole::GetVolumeHeaderVariable(hdrVol, "studyDate").c_str();
          if (!sStudyDate.IsEmpty())
          {
            spInfo->StudyDate = StringToDate(sStudyDate);
          }

          // PatientAge
          if (spInfo->PatientAge.length() == 0)
          {
            CString sAge = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientAge").c_str();
            spInfo->PatientAge = static_cast<LPCTSTR>(sAge);
          }
        } // PatientWeight/Sex/StudyDate

        // marked complete flag
        m_bReportMarkedComplete = m_spRevision->MarkedCompleted == VARIANT_TRUE;
      } //{{ end-compatibility

      //{{ backward compatibility of extra-colonic findings
      {
        for ( long lOtherFinding(0); lOtherFinding < m_spRevision->OtherFindings->Count; lOtherFinding++ )
        {
          ReportLib::IReportOtherFindingPtr spOtherFdg = m_spRevision->OtherFindings->GetItem(lOtherFinding);
          bool bOrphaned = true;
          for ( long lColonFinding(0); lColonFinding < m_spRevision->Entries->Count; lColonFinding++ )
          {
            ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(lColonFinding);
            if ( spOtherFdg->ID == spEntry->OtherFindingID )
            {
              bOrphaned = false;
              break;
            }
          } // for: thru colon findings

          if ( bOrphaned )
          {
            ReportLib::IReportEntryPtr spNewEntry = m_spRevision->Entries->Add();
            spNewEntry->OtherFindingID = spOtherFdg->ID;
            CString sname; sname.Format(LoadResourceString(IDS_EXTRA_COLONIC_FINDING_FORMAT), " ");
            spNewEntry->Name = static_cast<LPCTSTR>(sname);
            spNewEntry->ColonFinding = ReportLib::Other_Extracolonic;
            // TODO: Add images!!!  Or just leave image-less.
          }
        } // for: thru other findings
      }
      //}} backward compatibility of extra-colonic findings

    } // end-exist()
    else
    {
      m_spReport->Create(_bstr_t(pszReport));
      m_spRevision = m_spReport->Revisions->Add();
      ReportLib::IReportInfoPtr spInfo = m_spRevision->Info;

      spInfo->PatientName = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientName").c_str();
      spInfo->PatientID = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientID").c_str();

      spInfo->PatientWeight = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientWeight").c_str();

      switch (VxConsole::GetVolumeHeaderVariable(hdrVol, "patientSex")[0])
      {
        case 'M':  spInfo->PatientSex = ReportLib::Male;    break;
        case 'F':  spInfo->PatientSex = ReportLib::Female;  break;
        default:   break;
      }

      spInfo->Author = User::GetCurrentUser().GetFullName().c_str();
      spInfo->ReferringDoctor = VxConsole::GetVolumeHeaderVariable(hdrVol, "physicianName").c_str();
      spInfo->InstitutionName = VxConsole::GetVolumeHeaderVariable(hdrVol, "institutionName").c_str();

      spInfo->CRADSCategory = (long)1;
			
      // Scan Date
      CString sStudyDate = VxConsole::GetVolumeHeaderVariable(hdrVol, "studyDate").c_str();
      if (!sStudyDate.IsEmpty())
      {
        spInfo->StudyDate = StringToDate(sStudyDate);        
      }

      // Patient Age
      CString sAge = VxConsole::GetVolumeHeaderVariable(hdrVol, "patientAge").c_str();
      spInfo->PatientAge = static_cast<LPCTSTR>(sAge);

      m_spRevision->Save();
			if (m_spReport != NULL)
			{
				m_spRevision->Close();
				m_spRevision = NULL;
				m_spReport->Close();
				m_spReport = NULL;
			}
    }
  }
  catch (_com_error&)
  {
    if (m_spReport != NULL)
    {
      m_spReport->Close();
      m_spReport = NULL;
    }

		return FALSE;
  }

  return TRUE;
} // OpenReportFile()


/**
 * Closes the report.
 *
 * @param bSave       true to save file, false otherwise
 * @param bReadOnly   true if a read only report and should be deleted after closing, false otherwise
 * @return            Success/failure.
 */
BOOL VxVCReport::CloseReport(const bool bSave, const bool bReadOnly)
{
#ifdef FINAL_RELEASE
  try
#endif
  {
    if (m_spRevision==NULL)
    {
      return FALSE;
    }

    if (bSave)
    {
      m_spRevision->Save();
    }

    m_spRevision->Close();
    m_spRevision = NULL;

    if ( m_spReport != NULL )
    {
      m_spReport->Close();
      m_spReport = NULL;
    }

    if (bReadOnly)
    {
      DeleteFile(m_sReportFile.c_str());
    }
  }
#ifdef FINAL_RELEASE
  catch(_com_error&)
  {
//    ReportWndBase::DisplayReportError(IDS_SAVE, e.Error());
    return FALSE;
  }
#endif

  return TRUE;  
} // CloseReport()


/**
 * Converts a ColonSegment value to a LocationEnum value
 *
 * @param eSegment   Value to convert.
 * @return           Converted value.
 */
ReportLib::LocationEnum VxVCReport::MapColonSegment(const ColonSegmentEnum eSegment)
{
  ReportLib::LocationEnum eLocation(ReportLib::Rectum);
  switch (eSegment)
  {
    case eRECTUM:           eLocation = ReportLib::Rectum;          break;
    case eSIGMOID:          eLocation = ReportLib::Sigmoid;         break;
    case eDESCENDING:       eLocation = ReportLib::Descending;      break;
    case eSPLENIC_FLEXURE:  eLocation = ReportLib::SplenicFlexure;  break;
    case eTRANSVERSE:       eLocation = ReportLib::Transverse;      break;
    case eHEPATIC_FLEXURE:  eLocation = ReportLib::HepaticFlexure;  break;
    case eASCENDING:        eLocation = ReportLib::Ascending;       break;
    case eCECUM:            eLocation = ReportLib::Cecum;           break;
    default: break;
  }
  
  return eLocation; 
} // MapColonSegment()


/**
 * Converts a string to a date
 *
 * @param s   String to convert
 * @return    Date
 */
COleDateTime VxVCReport::StringToDate(const CString& s)
{
  if (s.GetLength() < 6)
    return COleDateTime(0,0,0,0,0,0);

  int4 iYear, iMonth, iDay;
  iYear = atoi(s.Left(4));
  iDay = atoi(s.Right(2));

  if (s.GetLength() == 8)
  {
    iMonth = atoi(s.Mid(4, 2));
  }
  else
  {
    iMonth = atoi(s.Mid(5, 2));
  }

  return COleDateTime(iYear, iMonth, iDay, 0, 0, 0);
} // StringToDate()


/**
 *  Write out PDF report file and snapshot to the file system only at user 
 *  exit the application. It may take some time if there are more than 6 
 *  snapshots.
 *
 *  @param bOutputPDF: the flag indicating if write out the PDF file
 */
void VxVCReport::WritePDFandSnapshots(const bool & bOutputPDF)
{
	if (m_sReportFile.empty())
	{
		LogErr("Missing report file name for session storage.", "VxVCReport", "WritePDFandSnapshots");
		return;
	}

	if (!vx::File(m_sReportFile).Exists())
	{
		LogErr("Report file doesn't exist.", "VxVCReport", "WritePDFandSnapshots");
		return;
	}

	// create temporary file name for PDF document
	int iFound = m_sXMLFilename.find_last_of('/\\');
	const std::string sSessionPath = m_sXMLFilename.substr(0, iFound);
	const std::string sPDFFilename = sSessionPath + "\\Reports\\report.pdf";

	try
	{
	  // create the PDF document
	  VxReportCreator rptCreator(VxVCReport::m_spRevision);
		if (bOutputPDF)
		{
			if (!rptCreator.Create(sPDFFilename.c_str()))
				LogErr("Fail to write pdf report file.", "VxVCReport", "WritePDFandSnapshots");
		}

		/// create and save the XML report file to the v3D Enterprise system
		rptCreator.WriteXMLReport();

		const std::string sSnapshotPath = sSessionPath + "\\Snapshots";

		std::string ss;

		/// remove old snapshots
		CFileFind cfd;
		BOOL bFound = cfd.FindFile(sSnapshotPath.c_str() + CString("\\*.png"));
		while (bFound)
		{
			bFound = cfd.FindNextFile();
			ss = cfd.GetFilePath();
			vx::File vf(ss);
			vf.RemoveAttrib(FILE_ATTRIBUTE_READONLY);
			vf.Delete();
		} // while

		// extract entries
		const int4 iNumEntries(m_spRevision->Entries->Count);
		for (int4 iEntry = 0; iEntry < iNumEntries; ++iEntry)
		{
			// grab finding from report
			ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem((long)iEntry);

			// get number of images in the finding
			int4 iNumImages = spEntry->Images->Count;

			for (int4 iImage(0); iImage < iNumImages; ++iImage)
			{
				Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)spEntry->Images->GetItem((long)iImage)->Bitmap, HPALETTE(0));

				if (pImage != nullptr)
				{
					ss = sSnapshotPath + "\\finding_" + ToAscii(iEntry) + "_" + ToAscii(iImage) + ".png";

					CLSID imgCLSID;
					if (ReaderGlobal::GetCodecClsid(L"image/png", imgCLSID))
					  pImage->Save((wchar_t*)widestring(ss).c_str(), &imgCLSID, NULL);
 				} // if

				delete pImage;
			} // for iImage
		} // for iEntry
			
		// extract "Non-Distended" findings
		const int4 iNumFindings = m_spRevision->NonDistendedFindings->Count;

		for (int4 iFinding(0); iFinding <iNumFindings; ++iFinding)
		{
			// grab the "Non-Distended" finding
			ReportLib::IReportNonDistendedFindingPtr spFinding = m_spRevision->NonDistendedFindings->GetItem((long)iFinding);

			Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)spFinding->Image->Bitmap, HPALETTE(0));

			if (pImage != nullptr)
			{
				ss = sSnapshotPath + "\\NDfinding_" + ToAscii(iFinding) + ".png";

				CLSID imgCLSID;
				if (ReaderGlobal::GetCodecClsid(L"image/png", imgCLSID))
				  pImage->Save((wchar_t*)widestring(ss).c_str(), &imgCLSID, NULL);
 			} // if

			delete pImage;
		} // for iFindings
	}
	catch (...)
	{
		throw ex::VException(LogRec("Error occurred while save PDF report file and snapshots.", "VxVCReport", "ExtractSnapshots"));
	}

} // WritePDFandSnapshots


/**
 *  Remove all temporary report vrx files
 */
void VxVCReport::RemoveTemporaryReportFiles()
{
	if (m_sReportFile.empty())
		return;

	const vx::File    studyFile(m_sReportFile);
	const std::string sParent(studyFile.GetParent());

	// Include the parent path if non-empty
	if (!sParent.empty())
	  _chdir(sParent.c_str());

	struct _finddata_t c_file;
  intptr_t hFile;

	std::string sss = "*" + vx::ReportExt;

	std::vector<std::string> sv;
  if ((hFile = _findfirst(sss.c_str(), &c_file)) != -1)
	{ /// remove all .vrx file in the current directory
		do
		{
			sv.push_back(c_file.name);
		} while ( _findnext(hFile, &c_file) == 0);

	  _findclose(hFile);

		for (int4 i = 0; i < sv.size(); i++)
		  remove(sv[i].c_str());
	} // if
		
} // RemoveTemporaryReportFiles()


// $Log: VxVCReport.C,v $
// Revision 1.15.2.11  2010/03/05 08:54:34  dongqing
// spaces
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.15.2.10  2009/08/24 22:01:59  dongqing
// Since Gerg finds out the correct fix to the report libray for Microsoft ATL
// update, we need to roll back Dongqing's temporary fix.
//
// Revision 1.15.2.9  2009/08/10 20:32:21  dongqing
// Add initialization for string fields of the report.
// That should fix the malfunction at report tab initialization.
//
// Revision 1.15.2.8  2008/04/01 18:02:43  jmeade
// Issue 6058: save new report only after all info has been set.
//
// Revision 1.15.2.7  2008/03/25 21:20:42  jmeade
// Issue 6028: ensure default c-rads value (C1) is stored in new vrx files.
//
// Revision 1.15.2.6  2008/02/15 01:48:17  jmeade
// Issue 5924: header name for extra-colonic finding.
//
// Revision 1.15.2.5  2008/02/14 07:04:01  jmeade
// Issue 5924: create parents for orphaned otherfindings.
//
// Revision 1.15.2.4  2007/11/07 03:36:58  jmeade
// Issue 5771: author field re-upped.
//
// Revision 1.15.2.3  2007/09/14 20:39:42  jmeade
// Issue 5776: removed double-processing of patient name.
//
// Revision 1.15.2.2  2007/08/22 17:09:39  jmeade
// Issue 5771: re-read report information because newly created reports are not storing the info.
// not a bug "fix" per say, more like a bug correction.
//
// Revision 1.15.2.1  2007/06/27 00:59:34  jmeade
// Issue 5705: must close report revision, then close report, then reopen both, for newly-created reports.
//
// Revision 1.15  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.14  2007/02/16 21:47:37  jmeade
// fixed potential crash with empty age field.
//
// Revision 1.13  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.12  2006/10/03 02:09:25  jmeade
// cvs string.
//
// Revision 1.11  2006/06/27 17:32:28  jmeade
// Read patient position from volume header.
//
// Revision 1.10  2005/11/22 16:42:34  frank
// fixed bad merge
//
// Revision 1.9  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.8  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.7  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.6  2005/11/07 20:40:05  geconomos
// removed unused catch parameters
//
// Revision 1.5.2.2  2006/07/10 19:50:30  mkontak
// Fixed no patient name when report is open from CD
//
// Revision 1.5.2.1  2005/11/18 14:10:22  geconomos
// Issue #4532: Author of report is not being set
//
// Revision 1.5  2005/10/12 00:33:11  jmeade
// Issue 4468: Fixed logic error updating report complete flag .
//
// Revision 1.4  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.2  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.5.4.7.2.2.6.1  2005/07/01 16:51:13  jmeade
// Issue 4169: Store value of marked-complete flag to avoid checking file on each query.
//
// Revision 1.5.4.7.2.2  2005/05/03 00:28:48  jmeade
// Issue 3980: handle either date format.
//
// Revision 1.5.4.7.2.1  2005/04/28 23:40:36  jmeade
// Issue 3980: read patientBirthDate if patientAge is 0 or empty.
//
// Revision 1.5.4.7  2005/02/15 18:04:25  jmeade
// merge from vc1.4 branch.
//
// Revision 1.5.4.6.2.1  2005/01/12 00:44:20  jmeade
// check for zero-length bstrings before accessing.
//
// Revision 1.5.4.6  2004/02/24 23:15:03  jmeade
// Issue 3530: Fill in author during new report creation.
//
// Revision 1.5.4.5  2004/02/13 20:54:42  jmeade
// Issue 3594: remove debug sound.
//
// Revision 1.5.4.4  2004/02/13 19:48:58  jmeade
// Issue 3530:  Ensure author is filled properly, and not empty and not == "Empty name".
//
// Revision 1.5.4.3  2003/12/02 17:59:15  jmeade
// Issue 3530:  Report creation code check for a blank entry for author's name
//  to decide whether to fill in based on the current user.
//
// Revision 1.5.4.2  2003/06/06 22:55:11  jmeade
// code review prep.
//
// Revision 1.5.4.1  2003/06/06 20:55:38  jmeade
// code review prep.
//
// Revision 1.5  2002/10/07 23:02:19  jmeade
// Unncessary includes, constructor, and destructor.
//
// Revision 1.4  2002/09/25 19:35:01  jmeade
// IsReportOpen override taking in a full study (title) pathname.
//
// Revision 1.3  2002/09/18 23:13:17  jmeade
// Log messages, comments.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxVCReport.C,v 1.15.2.11 2010/03/05 08:54:34 dongqing Exp $
// $Id: VxVCReport.C,v 1.15.2.11 2010/03/05 08:54:34 dongqing Exp $
