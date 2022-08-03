// $Id: VxReportCreator.h,v 1.4.2.3 2010/02/05 12:41:43 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: PDF file creator
// Owner: George Economos

#ifndef ReportCreator_h__
#define ReportCreator_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if defined(_DEBUG)
# import "..\..\..\..\Libraries\com\ReportLibd.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "..\..\..\..\Libraries\com\ReportLib.dll" named_guids
#else
# import "..\..\..\..\Libraries\com\ReportLibdr.dll" named_guids
#endif

#pragma warning(disable:4786)

#include "v3dsystemsettings.h"
#include "ApplicationSettings.h"
#include "vxDOMNodeList.h"
#include "vxDomNamedNodeMap.h"

#import "msxml3.dll"


namespace ConsoleLib
{
  /// creates a printable report
  class VxReportCreator
  {
  public:
    /// Constructor
	  VxReportCreator(ReportLib::IReportRevisionPtr pReportRevision);

    /// generates a PDF document from a report file(.vrx)
    BOOL Create(LPCTSTR pszFileName);

    /// creates a pdf
    std::shared_ptr<pdf::Pdf> CreatePdf();

		/// write out the xml report file 
		void WriteXMLReport();

		/// Read the xml report file and create the report revision
		void ReadXMLReport();
	
	protected:

    /// starts a new page
    void BeginPage( const bool & bIsFirstPage );

    /// ends a page
    void EndPage();

    /// get the required amount of line for the given string
    float4 GetRequiredLines( float4 x, float4 y, std::string sText, float4 fRightOffset );

    /// creates a new page if the needed line falls below the bottom margin
    void CheckPage(const int4 iNeededLines = 1);

    /// writes out the patient info and indications
    void WriteHeader();

    /// writes out all the non-extra (false) or extra colonic (true) findings.
    void WriteColonFindings(const bool bExtraColonic);

    /// writes out all the recommendations
    void WriteRecommendations();

    /// writes out the overall impression
    void WriteImpression();

    /// writes out the author
    void WriteAuthor();

    /// writes out an image
    void WriteImage(const float4 x, const float4 y, ReportLib::IImagePtr& spImage);

		/// write out page number
		void WritePageNumber();

  private:
    /// text string preprocessing.
    std::string TextPreprocessing(const std::string & sTextStr, const std::string & sHeaderStr,
                                  const std::string & sMarkStr, bool bNewLine, int4 iNumOfSpace);

    /// write out a line of otherfindings info
    void WriteOtherFinding(const ReportLib::IReportOtherFindingPtr spFinding);
		
		/// Appends an attribute child node to a given parent node
		MSXML2::IXMLDOMElementPtr AppendAttributeChild(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, MSXML2::IXMLDOMNodePtr pParentElement, const std::string& sElement,
			const std::string& sAttrName, const variant_t& value);


  /// data members

  public:
    /// uncertainty value for measurements; will be set by callers
    static int4 m_iUncertainty;

  protected:
		/// COM report object 
    ReportLib::IReportRevisionPtr m_pReportRevision;

		/// PDF object from vx wrapper to the Quick PDF libaray
    std::shared_ptr<pdf::Pdf> m_pPdf;

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

    /// footer settings  Always display.
    //V3DSystemSettings::ReportNumberingEnum m_eNumbering;

    /// footer settings
    V3DSystemSettings::ReportHeaderEnum m_eHeader;

    /// header text
    typedef std::list<std::string> stdstringlist;
    stdstringlist m_sHeaderTextList;

    /// dictionary maps
    std::map<long, std::string> m_mapColonSegment;
    std::map<long, std::string> m_mapPolypShape;
    std::map<long, std::string> m_mapOtherFindings;
    std::map<long, std::string> m_mapIndications;
    std::map<long, std::string> m_mapTranslucency;
    std::map<long, std::string> m_mapLungLocation;
    std::map<long, std::string> m_mapPneumothoraxLocation;
    std::map<long, std::string> m_mapPneumothoraxSize;
    std::map<long, std::string> m_mapLiverLocation;
    std::map<long, std::string> m_mapAdrenalLocation;
    std::map<long, std::string> m_mapRenalLocation;
    std::map<long, std::string> m_mapKidneyLocation;
    std::map<long, std::string> m_mapDegree;
    std::map<long, std::string> m_mapVascularLocation;
    std::map<long, std::string> m_mapSplenicLocation;
    std::map<long, std::string> m_mapPatientSex;
    std::map<long, std::string> m_mapRecommendType;
    std::map<long, std::string> m_mapRecommendUnit;

    /// global application state
    ApplicationSettings::ApplicationState m_globalState;

  private:
    /// 1 inch
    static const float4 FACTOR;

    /// page width in inches
    static const float4 PAGE_WIDTH;

    /// page height in inches
    static const float4 PAGE_HEIGHT;

    /// right margin x position
    static const float4 RIGHT_MARGIN;

    /// left margin x position
    static const float4 LEFT_MARGIN;

    /// maximum width of line.  No longer used.
    //static const float4 LINE_MAX;

    /// x offset text within a box.  No longer used.
    //static const float4 BOXED_TEXT_OFFSETX;

    /// image width in inches
    static const float4 IMAGE_WIDTH;

    /// image height in inches
    static const float4 IMAGE_HEIGHT;

    /// spacing between images in inches
    static const float4 IMAGE_SPACE;

    /// Other finding output format.
    static const CString g_sOtherFindingsEntryFormat;
    static const CString g_sLungPneomuthoraxFormat;
    static const CString g_sRenalScarFormat;
    static const CString g_sRenalHydronephrosisFormat;
    static const CString g_sOtherFindingsVascularAAAEntryFormat;
    static const CString g_sVascularArteralCalcificationFormat;
    static const CString g_sGIInguinalHerniaFormat;
    static const CString g_sGynecologyUterineFibroidsFormat;

    /// output format.
    static CString sOtherExtraFormat;
    static CString sUncertainDistanceFormat;
    static CString sCommentsFormat;
    static const std::string sShowBoxedLineFormat_Center;

  }; // VxReportCreator

} // namespace ConsoleLib

#endif

// $Log: VxReportCreator.h,v $
// Revision 1.4.2.3  2010/02/05 12:41:43  dongqing
// moved globals into class as private static variables
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.2  2008/02/14 09:48:33  jmeade
// Issue 5924: print other findings info with extra-colonic findings.
//
// Revision 1.4.2.1  2007/10/26 00:23:22  jmeade
// Issue 5845: uncertainty measurement.
//
// Revision 1.4  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.3  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.2  2005/08/05 14:14:10  geconomos
// updated #import for location of reporting library
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.4.1  2003/06/06 22:55:11  jmeade
// code review prep.
//
// Revision 1.1  2002/07/18 19:58:41  jmeade
// Moved report functionality to ConsoleLib.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxReportCreator.h,v 1.4.2.3 2010/02/05 12:41:43 dongqing Exp $
// $Id: VxReportCreator.h,v 1.4.2.3 2010/02/05 12:41:43 dongqing Exp $
