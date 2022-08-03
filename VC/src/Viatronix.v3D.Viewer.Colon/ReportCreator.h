// $Id: ReportCreator.h,v 1.2 2007/03/10 05:06:25 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: PDF file creator
// Created: 2/1/2001
// Author: George Economos
//


#ifndef ReportCreator_h__
#define ReportCreator_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#import "../../_win/lib/pdf/lib/pdflib_com.dll" named_guids

#if defined(_DEBUG)
# import "../../_win/bin/debug/ReportLib.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "../../_win/bin/release/ReportLib.dll" named_guids
#else
# import "../../_win/bin/debug-release/ReportLib.dll" named_guids
#endif

#include <string>
#include <map>
#include "V3DSystemSettings.h"
#include <list>

class ReportCreator  
{
public:
  /// constructor
	ReportCreator();

  /// generates a PDF document from a report file(.vrx)
  BOOL Create(LPCTSTR pszFileName);

protected:

  /// PDF writer interface
  PDFlib_com::IPDFPtr m_spPdfLib;

  /// starts a new page
  void BeginPage();

  /// ends a page
  void EndPage();

  /// set the current y position to the next value
  float4 NextLine(const int4 iLines = 1);

  /// returns the current y position
  float4 CurrentLine() const;

  /// returns the  current height of a line
  float4 LineHeight() const;

  /// get the required amount of lines for the given string
  float4 GetRequiredLines(const float4 x, const float4 y, LPCTSTR pszString, const float4 fLeftOffset=0.0f) const;

  /// get the required amount of line for the given string written in a box
  float4 GetRequiredBoxedLines(const float4 x, const float4 y, LPCTSTR psz) const;

  /// creates a new page if the needed line falls below the bottom margin
  void CheckPage(const int4 nNeededLines=1);

  /// writes out the patient info and indications
  void WriteHeader();
  
  /// writes out all the non-extra (false) or extra colonic (true) findings.
  void WriteColonFindings(const bool bExtraColonic);

  /// writes out all the NonDistended findings
  void WriteNonDistendedFindings();
  
  /// writes out the overall impression
  void WriteImpression();
 
  /// writes out all the other findings
  void WriteOtherFindings();
  
  /// writes out all the recommendations
  void WriteRecommendations();
  
  /// writes out the author
  void WriteAuthor();

  /// writes out a line of text
  void WriteLine(const float4 x, const float4 y, LPCTSTR psz, const float4 fLeftOffset = 0.0f);

  /// writes out a line of text boxed
  void WriteLineBoxed(const float4 x, const float4 y, LPCTSTR psz);
  
  /// writes out an image
  void WriteImage(const float4 x, const float4 y, const ReportLib::IImagePtr& spImage);

private:
  /// text string preprocessing.
  std::string TextPreprocessing(const std::string & sTextStr, const std::string & sHeaderStr,
                                const std::string & sMarkStr, const bool bNewLine, const int4 iNumOfSpace) const;

  /// test if this is a overview image & extra-colonic.
  bool IsOverviewImgOfExtraColonic(ReportLib::IImagePtr& spImage);

/// data members

protected:

  /// current y position
  float4 m_fCurrentY;
  
  /// number of pages in report
  int4 m_iNumPages;

  /// save this number.
  int4 m_iTotalPages;
  
  /// top margin
  float4 m_fTopMargin;
  
  /// bottom margin
  float4 m_fBottomMargin;
  
  /// footer settings
  V3DSystemSettings::ReportNumberingEnum m_eNumbering;
  
  /// footer settings
  V3DSystemSettings::ReportHeaderEnum m_eHeader;
  
  /// header text
  typedef std::list<std::string> stdstringlist;
  stdstringlist m_sHeaderTextList;
  
  /// header bitmap index
  int4 m_iHeaderImage;
  
  /// dictionary maps
  std::map<long, std::string> m_tColonSegment;
  std::map<long, std::string> m_tPolypShape;
  std::map<long, std::string> m_tOtherFindings;
  std::map<long, std::string> m_tIndications;
  std::map<long, std::string> m_tTranslucency;
  std::map<long, std::string> m_tLungLocation;
  std::map<long, std::string> m_tPneumothoraxLocation;
  std::map<long, std::string> m_tPneumothoraxSize;
  std::map<long, std::string> m_tLiverLocation;
  std::map<long, std::string> m_tAdrenalLocation;
  std::map<long, std::string> m_tRenalLocation;
  std::map<long, std::string> m_tKidneyLocation;
  std::map<long, std::string> m_tDegree;
  std::map<long, std::string> m_tVascularLocation;
  std::map<long, std::string> m_tSplenicLocation;
  std::map<long, std::string> m_tPatientSex;
  std::map<long, std::string> m_tRecommendType;
  std::map<long, std::string> m_tRecommendUnit;

}; // class ReportCreator

#endif

// Revision History:
// $Log: ReportCreator.h,v $
// Revision 1.2  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.8  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.7  2002/04/18 18:31:43  jmeade
// Moved/renamed ViatronSettings class to v3DSystemSettings.
//
// Revision 3.6  2002/03/15 03:31:04  jmeade
// Project configuration changes: Release ==> Debug-Release, Final Release ==> Release.
//
// Revision 3.5.2.1  2002/05/21 01:33:52  jmeade
// Issue 2265:  Implemented free-form text block for report header text.
//
// Revision 3.5  2002/01/09 22:06:45  binli
// Report adjustment: separate display of Non-Extra colon findings & Extra colon findings.
//
// Revision 3.4  2002/01/07 21:43:38  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.3  2002/01/04 18:33:30  binli
// Report adjustment: support of multi-line property in all EditBox
//
// Revision 3.2  2001/12/04 16:23:28  jmeade
// Moved report preferences enums to ViatronSettings class.
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/28 20:50:24  jmeade
// Remainder of settings using the ViatronSettings::Setting class
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 09 2001 09:55:56   binli
// considered the width of output text using WriteLines()
// 
//    Rev 2.0   Sep 16 2001 23:41:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:32   ingmar
// Initial revision.
// Revision 1.22  2001/09/10 08:36:57  soeren
// new dirs
//
// Revision 1.21  2001/08/24 18:19:44  binli
// 590 (track ID: 670/635): page number/patient name.Id,scan date on report pages.
//
// Revision 1.20  2001/08/16 14:23:41  binli
// bug 000486: added 'm_tKidneyLocation', and used it for proper text output in Preview
//
// Revision 1.19  2001/06/07 19:03:58  geconomos
// Code walkthru preparation
//
// Revision 1.18  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.17  2001/05/09 14:14:41  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.16  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.15  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.14  2001/03/17 22:53:34  ingmar
// set path in .dsp files for new directory structure   Part II
//
// Revision 1.13  2001/02/27 20:58:37  geconomos
// Added report header stuff
//
// Revision 1.12  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.11  2001/02/02 21:10:37  jeff
// use std::string instead of LPCTSTR so that strings won't lose scope
//
// Revision 1.10  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportCreator.h,v 1.2 2007/03/10 05:06:25 jmeade Exp $
// $Id: ReportCreator.h,v 1.2 2007/03/10 05:06:25 jmeade Exp $
