// $Id: VxVCReport.h,v 1.7 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

/**
 * interface for the VxVCReport class.
 */

#if !defined(AFX_VXVCREPORT_H__52AF4429_4901_4BD4_9605_8C53D56349F1__INCLUDED_)
#define AFX_VXVCREPORT_H__52AF4429_4901_4BD4_9605_8C53D56349F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "Exception.h"
#include "DatabaseConst.h"
#include "SegmentList.h"
#include "vxVolumeHeader.h"

#include "atlcomtime.h"

#if defined(_DEBUG)
# import "..\..\..\..\Libraries\com\ReportLibd.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "..\..\..\..\Libraries\com\ReportLib.dll" named_guids
#else
# import "..\..\..\..\Libraries\com\ReportLibdr.dll" named_guids
#endif

/// helper functions and variables for VC Reporting
class VxVCReport  
{
public:
  /// Returns true if the report is marked complete, false otherwise
  static bool IsReportMarkedCompleted(const std::string & sFile) throw(ex::IOException);

  /// Returns true if the currently-open report is marked complete, false otherwise
  static bool IsReportMarkedCompleted();

  /// Sets the 'Marked Complete' flag for the study.
  static bool SetMarkedCompleteFlag(const bool bCompleted);

  /// Opens the study's report file
  static BOOL OpenStudyReport(vxVolumeHeader& hdrVol, const std::string& sStudyTitle, const bool bReadOnly,
    const bool bHasPermissions, bool& bReportCreated);

  /// Opens the named report file
  static BOOL OpenReportFile(vxVolumeHeader& hdrVol, LPCTSTR pszReport, bool bEmptyReportFile = false);

  /// Closes the report file
  static BOOL CloseReport(const bool bSave, const bool bReadOnly);

  /// Opens the specified report file
  static bool OpenReportPreview(vxVolumeHeader& hdrVol, const std::string& sReportFilename);

  /// Checks if a report file is open
  static bool IsReportOpen(const std::string & sFilePathAndTitle);
  /// Checks if a report file is open
  static bool IsReportOpen(const std::string & sFilePath, const std::string & sFileTitle);

  /// Generates PDF report
  static std::string GeneratePDFReport(vxVolumeHeader& hdrVol, std::string sReportFilename);

  /// Generates the name of the report file for the study with the specified pathname
  static std::string GenerateReportFilename(const std::string & sStudyFilename);

  /// Maps a colon segment to a location enum
  static ReportLib::LocationEnum VxVCReport::MapColonSegment(const ColonSegmentEnum eSegment);

  /// Converts a string to a date
  static COleDateTime StringToDate(const CString& s);

	/// write out PDF report file and snapshot to the file system
	static void WritePDFandSnapshots(const bool & bOutputPDF);

	///Remove all temporary report vrx files
	static void RemoveTemporaryReportFiles();

public:
  /// Report revision pointer
  static ReportLib::IReportRevisionPtr m_spRevision;

  /// Report filename
  static std::string m_sReportFile;

	/// Temporary local VRX filename
	static std::string m_sXMLFilename;

  /// Report pointer
  static ReportLib::IReportPtr m_spReport;

private:
  /// Stores report complete flag for quick access
  static bool m_bReportMarkedComplete;
}; // VxVCReport()

#endif // !defined(AFX_VXVCREPORT_H__52AF4429_4901_4BD4_9605_8C53D56349F1__INCLUDED_)

// $Log: VxVCReport.h,v $
// Revision 1.7  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.6  2006/06/27 17:32:28  jmeade
// Read patient position from volume header.
//
// Revision 1.5  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/05 14:14:10  geconomos
// updated #import for location of reporting library
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.4.4.3.4.1.6.1  2005/07/01 16:51:13  jmeade
// Issue 4169: Store value of marked-complete flag to avoid checking file on each query.
//
// Revision 1.4.4.3.4.1  2005/04/28 23:40:36  jmeade
// Issue 3980: read patientBirthDate if patientAge is 0 or empty.
//
// Revision 1.4.4.3  2004/02/24 23:15:03  jmeade
// Issue 3530: Fill in author during new report creation.
//
// Revision 1.4.4.2  2003/07/03 17:55:18  jmeade
// Comments.
//
// Revision 1.4.4.1  2003/06/06 20:55:38  jmeade
// code review prep.
//
// Revision 1.4  2002/10/07 23:02:19  jmeade
// Unncessary includes, constructor, and destructor.
//
// Revision 1.3  2002/09/25 19:35:01  jmeade
// IsReportOpen override taking in a full study (title) pathname.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxVCReport.h,v 1.7 2007/03/07 22:34:05 jmeade Exp $
// $Id: VxVCReport.h,v 1.7 2007/03/07 22:34:05 jmeade Exp $
