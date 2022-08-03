// $Id: VxReportCreator.C,v 1.16.2.32 2010/02/05 12:41:42 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: PDF file creator
// Owner: George Economos

#include "stdafx.h"
#include "afx.h"
#include <afxdisp.h>
#include "File.h"
#include "VCResource.h"
#include "VxVCReport.h"
#include "VxReportCreator.h"
#include "ReportVariant.h"
#include "Exception.h"
#include "resource.h"
#include "VCResource.h"
#include "Crypt.h"
#include "ApplicationSettings.h"
#include "ReportMiscellaneous.h"
#include "Image.h"
#include "ReaderGlobal.h"

#include <direct.h>


#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B') 


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.16.2.32 $"

using namespace ConsoleLib;
using namespace ReaderLib;

/// 1 inch
const float4 VxReportCreator::FACTOR  = 72.0f;

/// page width in inches
const float4 VxReportCreator::PAGE_WIDTH = 8.5f * FACTOR;

/// page height in inches
const float4 VxReportCreator::PAGE_HEIGHT = 11.0f * FACTOR;

/// right margin x position
const float4 VxReportCreator::RIGHT_MARGIN = PAGE_WIDTH - (1.0f * FACTOR);

/// left margin x position
const float4 VxReportCreator::LEFT_MARGIN = 1.0f * FACTOR;

/// maximum width of line.  no longer used in the code.
//const float4 VxReportCreator::LINE_MAX = PAGE_WIDTH - 100.0f;

/// x offset text within a box.  no longer used in the code.
//const float4 VxReportCreator::BOXED_TEXT_OFFSETX = 5.0f;

/// image width in inches
const float4 VxReportCreator::IMAGE_WIDTH = 2.0f * FACTOR;

/// image height in inches
const float4 VxReportCreator::IMAGE_HEIGHT = 2.0f * FACTOR;

/// spacing between images in inches
const float4 VxReportCreator::IMAGE_SPACE = 0.25f * FACTOR;

/// Other finding output format.
const CString VxReportCreator::g_sOtherFindingsEntryFormat = "%s%s %s %s";
const CString VxReportCreator::g_sLungPneomuthoraxFormat = "%s %s %s %s";
const CString VxReportCreator::g_sRenalScarFormat = "%s %s %s";
const CString VxReportCreator::g_sRenalHydronephrosisFormat = "%s %s %s %s";
const CString VxReportCreator::g_sOtherFindingsVascularAAAEntryFormat = "%s %s %s %s%s";
const CString VxReportCreator::g_sVascularArteralCalcificationFormat = "%s %s %s";
const CString VxReportCreator::g_sGIInguinalHerniaFormat = "%s %s %s";
const CString VxReportCreator::g_sGynecologyUterineFibroidsFormat = "%s %s";

/// output format.
CString VxReportCreator::sOtherExtraFormat;
CString VxReportCreator::sUncertainDistanceFormat;
CString VxReportCreator::sCommentsFormat;
const std::string VxReportCreator::sShowBoxedLineFormat_Center = "center";

int4 VxReportCreator::m_iUncertainty(1);

/**
 * Constructor
 *
 * @param pReportRevision   Pointer to report revision information.
 **/
VxReportCreator::VxReportCreator(ReportLib::IReportRevisionPtr pReportRevision) :
m_pReportRevision(pReportRevision),
m_fCurrentY(0.0f),
m_iTotalPages(-1),
m_iNumPages(1),
m_fTopMargin(0.0f),
m_fBottomMargin(0.0f),
//m_eNumbering(V3DSystemSettings::eNUMBERING_NONE),
m_eHeader(V3DSystemSettings::eHEADER_NONE),
m_globalState( ApplicationSettings::Create( CONFIGURATION_GLOBAL ))
{
  float4 fTopMargin(0.0);   
  m_globalState.TryGet<float4>( APP_SETTING_REPORT_TOP_MARGIN, fTopMargin );
  m_fTopMargin = PAGE_HEIGHT - (fTopMargin * FACTOR);

  float4 fBottomMargin(0.0);   
  m_globalState.TryGet<float4>( APP_SETTING_REPORT_BOTTOM_MARGIN, fBottomMargin );
  m_fBottomMargin = fBottomMargin * FACTOR + 36;

  m_globalState.TryGet< V3DSystemSettings::ReportHeaderEnum>( APP_SETTING_REPORT_HEADER_SELECTION, m_eHeader );
  if (m_eHeader == V3DSystemSettings::eHEADER_USER)
  {
    std::string sHeaderText;
    m_globalState.TryGet( APP_SETTING_REPORT_HEADER_TEXT, sHeaderText );
    m_sHeaderTextList.clear();

    if (!sHeaderText.empty())
    {
      static const int1 sCRLF[] = { 13,10,0 };
      int4 iStartPos(0), iEndPos(0);
			int4 iNumOfLine( 0 );
			/// The text header is for the name of the clinics. It should not go crazy long. If it is too long, it shall be truncated.
			const int4 iMaxLineAllowed( 5 );
			const int4 iMaxCharAllowed( 60 ); 
			std::string ss( "" );
      do
      {
        iEndPos = sHeaderText.find(sCRLF, iStartPos);
				ss = sHeaderText.substr(iStartPos, (iEndPos!=std::string::npos) ? iEndPos-iStartPos : iEndPos);
				if( ss.size() > iMaxCharAllowed )
				{
					ss.erase( iMaxCharAllowed, ss.size() - iMaxCharAllowed );
				}
        m_sHeaderTextList.push_back( ss );
        iStartPos = iEndPos + strlen(sCRLF);
				iNumOfLine++;
      } while ( iEndPos != std::string::npos && iNumOfLine < iMaxLineAllowed );
    } // if header text is not empty
  } // if user requires header display at the report

  m_mapColonSegment[ReportLib::Cecum]          = LoadResourceString(IDS_CECUM);
  m_mapColonSegment[ReportLib::Ascending]      = LoadResourceString(IDS_ASCENDING_COLON);
  m_mapColonSegment[ReportLib::HepaticFlexure] = LoadResourceString(IDS_HEPATIC_FLEXURE);
  m_mapColonSegment[ReportLib::Transverse]     = LoadResourceString(IDS_TRANSVERSE_COLON);
  m_mapColonSegment[ReportLib::SplenicFlexure] = LoadResourceString(IDS_SPLENIC_FLEXURE);
  m_mapColonSegment[ReportLib::Descending]     = LoadResourceString(IDS_DESCENDING_COLON);
  m_mapColonSegment[ReportLib::Sigmoid]        = LoadResourceString(IDS_SIGMOID);
  m_mapColonSegment[ReportLib::Rectum]         = LoadResourceString(IDS_RECTUM);

  m_mapPolypShape[ReportLib::Round] = LoadResourceString(IDS_ROUND);
  m_mapPolypShape[ReportLib::Oval]  = LoadResourceString(IDS_OVAL);
  m_mapPolypShape[ReportLib::Sessile]  = LoadResourceString(IDS_SESSILE);
  m_mapPolypShape[ReportLib::PedunculatedStalkSeen] = LoadResourceString(IDS_PEDUNCULATED);
  m_mapPolypShape[ReportLib::PedunculatedStalkUnseen] = LoadResourceString(IDS_PEDUNCULATED_STALK_UNSEEN);
  m_mapPolypShape[ReportLib::Flat] = LoadResourceString(IDS_FLAT);

  CString densityChange;
  densityChange.Format( LoadResourceString( IDS_DENSITY_CHANGE_FORMAT ), LoadResourceString(IDS_GRADUAL));
  m_mapTranslucency[ReportLib::GradualDensityChange] = densityChange;
  densityChange.Format( LoadResourceString( IDS_DENSITY_CHANGE_FORMAT ), LoadResourceString(IDS_ABRUPT));
  m_mapTranslucency[ReportLib::AbruptDensityChange]  = densityChange;
  densityChange.Format( LoadResourceString( IDS_DENSITY_CHANGE_FORMAT ), LoadResourceString(IDS_INTERMEDIATE));
  m_mapTranslucency[ReportLib::IntermediateDensityChange] = densityChange;
  m_mapTranslucency[ReportLib::SimilarToColonWall]   = LoadResourceString(IDS_SIMILAR_TO_COLON_WALL);
  m_mapTranslucency[ReportLib::Indeterminate + 5]    = LoadResourceString(IDS_INDETERMINATE);

  m_mapIndications[ReportLib::Screening] = LoadResourceString(IDS_SCREENING);
  m_mapIndications[ReportLib::PersonalHistoryOfPolyps] = LoadResourceString(IDS_PERSONAL_HISTORY_OF_POLYPS);
  m_mapIndications[ReportLib::Constipation] = LoadResourceString(IDS_CONSTIPATION);
  m_mapIndications[ReportLib::PersonalHistoryOfCancer] = LoadResourceString(IDS_PERSONAL_HISTORY_OF_COLONCANCER);
  m_mapIndications[ReportLib::FamilyHistoryOfCancer] = LoadResourceString(IDS_FAMILY_HISTORY_OF_CANCER);
  m_mapIndications[ReportLib::Anemia] = LoadResourceString(IDS_ANEMIA);
  m_mapIndications[ReportLib::Hematochezia] = LoadResourceString(IDS_HEMATOCHEZIA);
  m_mapIndications[ReportLib::AbnormalImaging] = LoadResourceString(IDS_ABNORMAL_IMAGING);
  m_mapIndications[ReportLib::AbdominalPain] = LoadResourceString(IDS_ABDOMINAL_PAIN);
  m_mapIndications[ReportLib::IBD] = LoadResourceString(IDS_IBD);
  m_mapIndications[ReportLib::Diarrhea] = LoadResourceString(IDS_DIARRHEA);
  m_mapIndications[ReportLib::PolypOnFlexSigmoid] = LoadResourceString(IDS_POLYP_ON_FLEX_SIGMOID);
  m_mapIndications[ReportLib::FailedColonoscopy] = LoadResourceString(IDS_FAILED_COLONOSCOPY);
  m_mapIndications[ReportLib::FamilyHistoryOfPolyps] = LoadResourceString(IDS_FAMILY_HISTORY_OF_POLYPS);
  m_mapIndications[ReportLib::HighSedationRisk] = LoadResourceString(IDS_HIGH_SEDATION_RISK);

  m_mapOtherFindings[ReportLib::Lung_Nodule]                   = LoadResourceString(IDS_LUNG_NODULE);
  m_mapOtherFindings[ReportLib::Lung_Granuloma]                = LoadResourceString(IDS_LUNG_GRANULOMA);
  m_mapOtherFindings[ReportLib::Lung_Pneomuthorax]             = LoadResourceString(IDS_PNEUMOTHORAX);
  m_mapOtherFindings[ReportLib::Liver_Mass]                    = LoadResourceString(IDS_LIVER_MASS);
  m_mapOtherFindings[ReportLib::Liver_Cyst]                    = LoadResourceString(IDS_LIVER_CYST);
  m_mapOtherFindings[ReportLib::Liver_Gallstones]              = LoadResourceString(IDS_GALLSTONES);
  m_mapOtherFindings[ReportLib::Liver_FattyLiver]              = LoadResourceString(IDS_FATTY_LIVER);
  m_mapOtherFindings[ReportLib::Adrenal_Mass]                  = LoadResourceString(IDS_ADRENAL_MASS);
  m_mapOtherFindings[ReportLib::Renal_Mass]                    = LoadResourceString(IDS_RENAL_MASS);
  m_mapOtherFindings[ReportLib::Renal_Cyst]                    = LoadResourceString(IDS_RENAL_CYST);
  m_mapOtherFindings[ReportLib::Renal_Stones]                  = LoadResourceString(IDS_RENAL_STONES);
  m_mapOtherFindings[ReportLib::Renal_Scar]                    = LoadResourceString(IDS_RENAL_SCAR);
  m_mapOtherFindings[ReportLib::Renal_Hydronephrosis]          = LoadResourceString(IDS_HYDRONEPHROSIS);
  m_mapOtherFindings[ReportLib::Vascular_AAA]                  = LoadResourceString(IDS_AAA);
  m_mapOtherFindings[ReportLib::Vascular_ArteralCalcification] = LoadResourceString(IDS_ARTERAL_CALCIFICATION);
  m_mapOtherFindings[ReportLib::Spleen_Mass]                   = LoadResourceString(IDS_SPLENIC_MASS);
  m_mapOtherFindings[ReportLib::Spleen_Granuloma]              = LoadResourceString(IDS_SPLENIC_GRANULOMA);
  m_mapOtherFindings[ReportLib::GI_InguinalHernia]             = LoadResourceString(IDS_INGUINAL_HERNIA);
  m_mapOtherFindings[ReportLib::GI_HiatalHernia]               = LoadResourceString(IDS_HIATAL_HERNIA);
  m_mapOtherFindings[ReportLib::Gynecology_UterineFibroids]    = LoadResourceString(IDS_UTERINE_FIBROIDS);

  m_mapLungLocation[ReportLib::RightLung_UpperLobe]  = LoadResourceString(IDS_RIGHT_UPPER_LOBE);
  m_mapLungLocation[ReportLib::RightLung_MiddleLobe] = LoadResourceString(IDS_RIGHT_MIDDLE_LOBE);
  m_mapLungLocation[ReportLib::RightLung_LowerLobe]  = LoadResourceString(IDS_RIGHT_LOWER_LOBE);
  m_mapLungLocation[ReportLib::LeftLung_UpperLobe]   = LoadResourceString(IDS_LEFT_UPPER_LOBE);
  m_mapLungLocation[ReportLib::LeftLung_MiddleLobe]  = LoadResourceString(IDS_LEFT_MIDDLE_LOBE);
  m_mapLungLocation[ReportLib::LeftLung_LowerLobe]   = LoadResourceString(IDS_LEFT_LOWER_LOBE);

  m_mapPneumothoraxLocation[ReportLib::RightLung] = LoadResourceString(IDS_RIGHT_LUNG);
  m_mapPneumothoraxLocation[ReportLib::LeftLung]  = LoadResourceString(IDS_LEFT_LUNG);

  m_mapPneumothoraxSize[ReportLib::Small]  = LoadResourceString(IDS_SMALL);
  m_mapPneumothoraxSize[ReportLib::Medium] = LoadResourceString(IDS_MEDIUM);
  m_mapPneumothoraxSize[ReportLib::Large]  = LoadResourceString(IDS_LARGE);

  m_mapLiverLocation[ReportLib::RightLobe_AnteriorSegment]  = LoadResourceString(IDS_RIGHT_LOBE_ANTERIOR_SEGMENT);
  m_mapLiverLocation[ReportLib::RightLobe_PosteriorSegment] = LoadResourceString(IDS_RIGHT_LOBE_POSTERIOR_SEGMENT);
  m_mapLiverLocation[ReportLib::LeftLobe_MedialSegment]     = LoadResourceString(IDS_LEFT_LOBE_MEDIAL_SEGMENT);
  m_mapLiverLocation[ReportLib::LeftLobe_LateralSegment]    = LoadResourceString(IDS_LEFT_LOBE_LATERAL_SEGMENT);

  m_mapAdrenalLocation[ReportLib::Right] = LoadResourceString(IDS_RIGHT);
  m_mapAdrenalLocation[ReportLib::Left]  = LoadResourceString(IDS_LEFT);

  m_mapRenalLocation[ReportLib::Right_UpperPole] = LoadResourceString(IDS_RIGHT_UPPER_POLE);
  m_mapRenalLocation[ReportLib::Right_Middle]    = LoadResourceString(IDS_RIGHT_MIDDLE);
  m_mapRenalLocation[ReportLib::Right_LowerPole] = LoadResourceString(IDS_RIGHT_LOWER_POLE);
  m_mapRenalLocation[ReportLib::Left_UpperPole]  = LoadResourceString(IDS_LEFT_UPPER_POLE);
  m_mapRenalLocation[ReportLib::Left_Middle]     = LoadResourceString(IDS_LEFT_MIDDLE);
  m_mapRenalLocation[ReportLib::Left_LowerPole]  = LoadResourceString(IDS_LEFT_LOWER_POLE);

  m_mapKidneyLocation[ReportLib::RightKidney] = LoadResourceString(IDS_RIGHT_KIDNEY);
  m_mapKidneyLocation[ReportLib::LeftKidney]  = LoadResourceString(IDS_LEFT_KIDNEY);

  m_mapDegree[ReportLib::Mild]     = LoadResourceString(IDS_MILD);
  m_mapDegree[ReportLib::Moderate] = LoadResourceString(IDS_MODERATE);
  m_mapDegree[ReportLib::Severe]   = LoadResourceString(IDS_SEVERE);

  m_mapVascularLocation[ReportLib::Suprarenal] = LoadResourceString(IDS_SUPRARENAL);
  m_mapVascularLocation[ReportLib::Juxtarenal] = LoadResourceString(IDS_JUXTARENAL);
  m_mapVascularLocation[ReportLib::Infrarenal] = LoadResourceString(IDS_INFRARENAL);

  m_mapSplenicLocation[ReportLib::Superior] = LoadResourceString(IDS_SUPERIOR);
  m_mapSplenicLocation[ReportLib::Middle]   = LoadResourceString(IDS_MIDDLE);
  m_mapSplenicLocation[ReportLib::Inferior] = LoadResourceString(IDS_INFERIOR);

  m_mapPatientSex[ReportLib::Male]   = LoadResourceString(IDS_MALE);
  m_mapPatientSex[ReportLib::Female] = LoadResourceString(IDS_FEMALE);

  m_mapRecommendType[ReportLib::Optical] = LoadResourceString(IDS_OPTICAL);
  m_mapRecommendType[ReportLib::Virtual] = LoadResourceString(IDS_3DCT);

  m_mapRecommendUnit[ReportLib::Months] = LoadResourceString(IDS_MONTH);
  m_mapRecommendUnit[ReportLib::Years]  = LoadResourceString(IDS_YEAR);

  sOtherExtraFormat = CString(" - ");// + LoadResourceString(IDS_OTHER) + ") ";
  sUncertainDistanceFormat = " " + LoadResourceString(IDS_DISTANCE_FROM_RECTUM_UNCERTAIN);
  sCommentsFormat = LoadResourceString(IDS_COMMENTS) + ": ";
} // constructor


/**
 * Create report file.
 *
 * @param sFileName   Name of report file to create.
 * @return            Success/failure.
 */
BOOL VxReportCreator::Create(LPCTSTR sFileName)
{
  try
  {
    CreatePdf();
		m_pPdf->Save( sFileName ); 
    m_pPdf->ClosePath();

    return TRUE;
  }
  catch ( pdf::PdfException & )
	{
		AfxMessageBox( "Could not save PDF file." );
    return FALSE;
	} 
  catch( ex::VException & e )
  {
    AfxMessageBox( e.GetLogRecord().GetTextMessage().c_str() );
    return FALSE;
  }
} // Create()


/**
 * Create report file.
 *
 * @param sFileName   Name of report file to create.
 * @return            Success/failure.
 */
std::shared_ptr<pdf::Pdf> VxReportCreator::CreatePdf()
{
  try
  {
    if (m_pReportRevision == NULL || m_pReportRevision->IsOpen == VARIANT_FALSE)
    {
      return FALSE;
    }

    m_pPdf = pdf::Pdf::Create();

		/// add font with font embeded
    m_pPdf->AddTrueTypeFont( "Times New Roman", true);
    m_pPdf->SetTextSize(11);
    m_fCurrentY = PAGE_HEIGHT - m_pPdf->GetTextHeight();


    try
    { // The header logo set up for the first page
      if( m_eHeader == V3DSystemSettings::eHEADER_BITMAP )
      {
        std::string reportHeader = V3DSystemSettings::GetReportHeaderFile();

        try
        {
          m_pPdf->AddImageFromFile( reportHeader );

          float4 fImgWidth      = m_pPdf->ImageWidth();
			    float4 fImgHeight     = m_pPdf->ImageHeight();
			    float4 fHeaderWidth   = RIGHT_MARGIN - LEFT_MARGIN;
			    float4 fHeaderHeight  = PAGE_HEIGHT - m_fTopMargin - 2;
			    float4 fActualWidth   = fHeaderHeight * fImgWidth / fImgHeight;

          m_pPdf->FitImage( LEFT_MARGIN + (fHeaderWidth-fActualWidth)/2.0f, PAGE_HEIGHT - 2.0f, fActualWidth, fHeaderHeight );
        }
        catch(pdf::PdfException & )
        {
          m_eHeader = V3DSystemSettings::eHEADER_NONE;
          AfxMessageBox("VC Report Header image missing.  Report will have a blank header.", MB_OK);
        }
      }
			else if( m_eHeader == V3DSystemSettings::eHEADER_USER )
			{
				if( !m_sHeaderTextList.empty() )
				{           
          // set text align at the center
          m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_CENTER );

					m_fCurrentY = PAGE_HEIGHT - m_pPdf->GetTextHeight() - 4;

					for( stdstringlist::const_iterator it = m_sHeaderTextList.begin(); it != m_sHeaderTextList.end(); ++it )
					{
            m_pPdf->DrawText( PAGE_WIDTH / 2.0F, m_fCurrentY,  *it );
            m_fCurrentY -= m_pPdf->GetTextHeight();
          } // for iterator

          m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT );
				} // if header text is not empty
			} // else

      m_fCurrentY = m_fTopMargin - m_pPdf->GetTextHeight();
    }
    catch ( pdf::PdfException & ex )
    {
      LogErr(util::Strings::Format("Failed to create PDF : %s", ex.GetTextMessage().c_str()), "VxReportCreator", "Create");

#ifndef FINAL_RELEASE
      AfxMessageBox("PDF library is not available.\nPlease contact Viatronix service.");
#endif

      throw ex::VException( ex.GetLogRecord() );
    }
    catch( ... )
    {
			throw ex::VException( LogRec("Could not find report header file.", "VxReportCreator", "Create") );
    }

		uint4 uErrorCode = 0;

    // bit setting:
		try { BeginPage( true ); }                  catch( const _com_error&e ) { uErrorCode |= (0x01   ); throw e; }
    try { WriteHeader(); }                      catch( const _com_error&e ) { uErrorCode |= (0x01<<1); throw e; }
    try { WriteColonFindings( false ); }        catch( const _com_error&e ) { uErrorCode |= (0x01<<2); throw e; }
    try { WriteColonFindings( true  ); }        catch( const _com_error&e ) { uErrorCode |= (0x01<<3); throw e; }
    try { WriteImpression(); }                  catch( const _com_error&e ) { uErrorCode |= (0x01<<6); throw e; }
    try { WriteRecommendations(); }             catch( const _com_error&e ) { uErrorCode |= (0x01<<7); throw e; }
    try { WriteAuthor(); }                      catch( const _com_error&e ) { uErrorCode |= (0x01<<8); throw e; }
    try { EndPage(); }                          catch( const _com_error&e ) { uErrorCode |= (0x01<<9); throw e; }
		try { WritePageNumber(); }                  catch( ... ) { LogErr("Fail to add page number to report", "Create", "VxReportCreator"); }

    return m_pPdf;
  }

  catch(_com_error& e)
  {
    CString sErrorMsg;
#ifndef FINAL_RELEASE
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
#else
    sErrorMsg = LoadResourceString(IDS_ERROR_CREATING_REPORT) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    e;
#endif
    CT2CA msg( sErrorMsg );
    throw ex::VException( LogRec( std::string( msg ), "vxReportCreator", "CreatePdf" ));
  }
} // CreatePdf()


/**
 * Puts PDF headers at start of every page.
 *
 * @param bIsFirstPage   True if this is the first page in the PDF document
 */
void VxReportCreator::BeginPage( const bool & bIsFirstPage )
{
  try
  { // start a new page
		if( !bIsFirstPage )
		{
			m_pPdf->NewPage();
		}
		else
		{
			return;
		} // else if it is the first page

		m_pPdf->SetPageSize( "Letter" );

		// if user has selected header logo image from Preference Dialog
    if( m_eHeader == V3DSystemSettings::eHEADER_BITMAP )
    {
      std::string reportHeader = V3DSystemSettings::GetReportHeaderFile();

			m_pPdf->AddImageFromFile( reportHeader  );

			float4 fImgWidth      = m_pPdf->ImageWidth();
			float4 fImgHeight     = m_pPdf->ImageHeight();
			float4 fHeaderWidth   = RIGHT_MARGIN - LEFT_MARGIN;
			float4 fHeaderHeight  = PAGE_HEIGHT - m_fTopMargin - 2;
			float4 fActualWidth   = fHeaderHeight * fImgWidth / fImgHeight;

			/// fit the image into the header area
			m_pPdf->FitImage( LEFT_MARGIN + (fHeaderWidth-fActualWidth)/2.0f, PAGE_HEIGHT - 2.0f, fActualWidth, fHeaderHeight );
		} // if header logo is applicable
		else if( m_eHeader == V3DSystemSettings::eHEADER_USER )
		{
			if( !m_sHeaderTextList.empty() )
			{
				// set text align at the center
				m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_CENTER );

				m_fCurrentY = PAGE_HEIGHT - m_pPdf->GetTextHeight() - 4;

				for( stdstringlist::const_iterator it = m_sHeaderTextList.begin(); it != m_sHeaderTextList.end(); ++it )
				{
					m_pPdf->DrawText( PAGE_WIDTH / 2.0F, m_fCurrentY,  *it );
					m_fCurrentY -= m_pPdf->GetTextHeight();
				} // for iterator

        m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT );
			} // if header text is not empty
		} // else

     m_fCurrentY = m_fTopMargin - m_pPdf->GetTextHeight();
  } // try
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format( "Error creating PDF report.0x%08X", e.Error() );
    LogRec( std::string(sErrorMsg), "VxReportCreator", "BeginPage" );
    throw e;
  }
} // BeginPage()


/**
 * Puts PDF footers at end of every page.
 */
void VxReportCreator::EndPage()
{
  try
  {
		m_pPdf->SetLineWidth( 1.5f );
		m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fBottomMargin - 40, RIGHT_MARGIN - 2.0f, m_fBottomMargin - 40 );

    // format the footer and output
    CString sPatientInfo;
    ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;
    COleDateTime dt(spInfo->StudyDate);
    sPatientInfo.Format( LoadResourceString( IDS_REPORT_FOOTER_PATIENTDATA_FORMAT ),
      static_cast<LPCTSTR>(spInfo->PatientName.length()>0 ? spInfo->PatientName : _bstr_t()),
      static_cast<LPCTSTR>(spInfo->PatientID.length()>0 ? spInfo->PatientID : _bstr_t()),
      static_cast<LPCTSTR>(dt.Format(VAR_DATEVALUEONLY, LANG_USER_DEFAULT)));

    std::string sDisclaimerText;  
    m_globalState.TryGet( APP_SETTING_REPORT_DISCLAIMER, sDisclaimerText );
    if( !sDisclaimerText.empty() )
    {
      Crypt crypt;
      crypt.DecryptReadable( sDisclaimerText );
      
      m_pPdf->SetTextSize(14);
			m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_CENTER );
			m_pPdf->DrawText( PAGE_WIDTH / 2.0F, m_fBottomMargin - 60.0f - (m_pPdf->GetTextHeight()*1.5f), sDisclaimerText );
      m_pPdf->SetTextSize(12);
    }

		sDisclaimerText = sPatientInfo;
		m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT  );
		m_pPdf->DrawText( LEFT_MARGIN, m_fBottomMargin - 40.0f - m_pPdf->GetTextHeight(),  sDisclaimerText  );

   }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "EndPage");
    throw e;
  }
} // EndPage()


/**
 * Writes all patient report header info.
 */
void VxReportCreator::WriteHeader()
{
  try
  {
    float4 xPos = 200.0f;
    ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

    // output the patient information title and underline
    CString str( LoadResourceString( IDS_PATIENTINFORMATION ));
    str.MakeUpper();
		std::string sTemp = str;
		/// patient information header
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT  );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );

		m_fCurrentY -= m_pPdf->GetTextHeight();
		m_pPdf->SetLineWidth( 1.5f );
		/// section separation line
		m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY + 4.0f, RIGHT_MARGIN - 2.0f, m_fCurrentY + 4.0f);

		m_fCurrentY -= 1.0f*m_pPdf->GetTextHeight();
		/// patient name
		sTemp = LoadResourceString(IDS_PATIENT);
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT  );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );
		sTemp = spInfo->PatientName;
		m_pPdf->DrawText( xPos, m_fCurrentY, sTemp );

		m_fCurrentY -= m_pPdf->GetTextHeight();
		/// patient ID
		sTemp = LoadResourceString(IDS_PATIENTID);
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );
		sTemp = spInfo->PatientID;
		m_pPdf->DrawText( xPos, m_fCurrentY,  sTemp  );

		m_fCurrentY -= m_pPdf->GetTextHeight();
		/// study date
		sTemp = LoadResourceString(IDS_STUDYDATE);
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment(  pdf::Pdf::ALIGN_LEFT );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp  );
		COleDateTime dt(spInfo->StudyDate);
		sTemp = static_cast<LPCTSTR>(dt.Format(VAR_DATEVALUEONLY, LANG_USER_DEFAULT));
		m_pPdf->DrawText( xPos, m_fCurrentY, sTemp );

		m_fCurrentY -= m_pPdf->GetTextHeight();
		/// referring physican
		sTemp = LoadResourceString(IDS_REFERRINGPHYSICIAN);
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment(  pdf::Pdf::ALIGN_LEFT );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );
		sTemp = spInfo->ReferringDoctor;
		m_pPdf->DrawText( xPos, m_fCurrentY, sTemp );

		m_fCurrentY -= 2.0f * m_pPdf->GetTextHeight();
		/// output indications title and underline
    str = LoadResourceString( IDS_INTRODUCTION );
    str.MakeUpper();
		sTemp = str;
		sTemp = sTemp + " : ";
	  m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );
		m_fCurrentY -= m_pPdf->GetTextHeight();
		m_pPdf->SetLineWidth( 1.5f );
		/// section separation line
		m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY+4.0f, RIGHT_MARGIN - 2.0f, m_fCurrentY+4.0f );
		m_fCurrentY -= m_pPdf->GetTextHeight();
    
    // new xPos value for indications
    xPos = LEFT_MARGIN + 20;

    // indications is a bitmask
    int4 iIndications  = spInfo->Indication;

    // determine if any indication have been selected or entered as custom 
    bool bHasIndication = ( iIndications > 0 || spInfo->OtherIndication.length() > 0 );
    UINT iResourceID = ( !bHasIndication ) ? IDS_INDICATION_ELECTIVE_COLONSCOPY_FORMAT : IDS_INDICATION_GENERAL_FORMAT;
    CString s( LoadResourceString( iResourceID ));
    s.Replace("#<age>", static_cast<LPCSTR>(spInfo->PatientAge));
    s.Replace("#<gender>", m_mapPatientSex[spInfo->GetPatientSex()].c_str());
		sTemp = s;
	  m_pPdf->SetTextAlignment(  pdf::Pdf::ALIGN_LEFT );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );
		m_fCurrentY -= m_pPdf->GetTextHeight();

    if( bHasIndication ) //  output if there are indications
    {
      using namespace ReportLib;
      const IndicationsEnum veInd[] = { Screening, PersonalHistoryOfPolyps, PersonalHistoryOfCancer, FamilyHistoryOfPolyps, FamilyHistoryOfCancer,
        Constipation, Anemia, Hematochezia, AbnormalImaging, AbdominalPain, IBD, Diarrhea, FailedColonoscopy, HighSedationRisk } ;

      for( int4 iInd(0); iInd <= sizeof(veInd)/sizeof(IndicationsEnum); iInd++ )
      {
        if (iIndications & veInd[iInd])
				{
					sTemp = m_mapIndications[ReportLib::IndicationsEnum(veInd[iInd])];
	        m_pPdf->SetTextAlignment(  pdf::Pdf::ALIGN_LEFT );
		      m_pPdf->DrawText( xPos, m_fCurrentY, sTemp );
					m_fCurrentY -= m_pPdf->GetTextHeight();
				} // if
      } // for all selected indications

      // don't forget to write out other indications.
      if( spInfo->OtherIndication.length() > 0 )
      {
        std::string sNormalOtherIndicationStr = LPCTSTR(LoadResourceString(IDS_OTHER_INDICATIONS_LABEL_ABBREV)) + spInfo->OtherIndication;
        sNormalOtherIndicationStr = TextPreprocessing( sNormalOtherIndicationStr, "", "", false, 10);
				float4 fLineNum = GetRequiredLines( xPos, m_fCurrentY, sNormalOtherIndicationStr, 34.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( xPos, m_fCurrentY, RIGHT_MARGIN - xPos - 34, sNormalOtherIndicationStr  );
				m_fCurrentY -= (fLineNum + 1.0F) * m_pPdf->GetTextHeight();
      }   // if    
    } // if bHasIndication

    if( m_globalState.Get<bool>( APP_SETTING_REPORT_ADD_CRADS ) )
    { // C-RADS Category
      if(!bHasIndication)
      {
        m_fCurrentY -= m_pPdf->GetTextHeight();
      }

      if( spInfo->CRADSCategory >= 0 && spInfo->CRADSCategory <7 )
      {
        s = (LPCTSTR(LoadResourceString(IDS_CRADS_OUTPUT)) + ToAscii(spInfo->CRADSCategory)).c_str();
				sTemp = s;
				float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sTemp, 0.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
      }
      else
      {
        LogDbg("Invalid CRADS category", "VxReportCreator.C", "WriteHeader");
      }

      ReportLib::IReportOtherFindingsPtr spOtherFindings = m_pReportRevision->OtherFindings;
      long lCat(-1);
      for (long lCurr(0), iCount(spOtherFindings->Count); lCurr < iCount; lCurr++)
      {
        if (lCat < spOtherFindings->GetItem(lCurr)->CRADSCategory)
          lCat = spOtherFindings->GetItem(lCurr)->CRADSCategory;
      }
      if( (lCat >= 0) && (lCat != 1) )
			{
				sTemp = LPCTSTR(LoadResourceString(IDS_EXTRA_COLONIC_CRADS) + ToAscii(lCat).c_str());
				float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sTemp, 0.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN, sTemp );
				m_fCurrentY -= (fLineNum + 1.0F) * m_pPdf->GetTextHeight();
			} // if
    } // if
    
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "WriteHeader");
    throw e;
  }
} // WriteHeader().


/**
 * Writes out all colon findings or extra-colonic findings.
 *
 * @param bExtraColonic   Whether to output extra-colonic (if true) or normal colonic findings.
 */
void VxReportCreator::WriteColonFindings( const bool bExtraColonic )
{
  try
  { //get number of total findings
		const int4 iNumEntries( m_pReportRevision->Entries->Count );

		// if there aren't any find, return
		if (iNumEntries <= 0)
		  return;

		// get the number of colon findings, except 'extra-colonic'
		bool bNoSuchFinding = true;
		int4 iEntry=0;
		for( ; iEntry < iNumEntries; iEntry++ )
		{ // grab finding from report
			ReportLib::IReportEntryPtr spEntry = m_pReportRevision->Entries->GetItem( (long)iEntry );
			bNoSuchFinding = bExtraColonic ^ ( spEntry->ColonFinding == ReportLib::Other_Extracolonic );
			if( bNoSuchFinding == false ) 
				break;
		}

		// if all are 'extra' ...
		if ( bNoSuchFinding )
		  return;

    // output the colon findings title and underline
    CString str( LoadResourceString( bExtraColonic ? IDS_EXTRA_COLONIC_FINDINGS : IDS_COLON_FINDINGS ) );
    str.MakeUpper();

		m_fCurrentY -= m_pPdf->GetTextHeight();
		std::string sTemp = (LPCTSTR(str + ':'));
		/// Finding title
	  m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_LEFT  );
		m_pPdf->DrawText( LEFT_MARGIN, m_fCurrentY, sTemp );

		m_fCurrentY -= m_pPdf->GetTextHeight();
		m_pPdf->SetLineWidth( 1.5f );
		/// section separation line
		m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY+10.0f, RIGHT_MARGIN - 2.0f, m_fCurrentY+10.0f );

		m_fCurrentY -= m_pPdf->GetTextHeight();

    // TODO: Maybe better to add an entry field identifying the following case
    // if an entry has an asterisk at the end of the entry name, then it's recorded distance to rectum doesn't match current segment order
    bool bPrintUncertainDistanceWarning(false);

    // for each colon finding
    for (iEntry=0; iEntry < iNumEntries; iEntry++)
    { // grab finding from report
      ReportLib::IReportEntryPtr spEntry = m_pReportRevision->Entries->GetItem( (long)iEntry );
      if( bExtraColonic == (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
        continue;

      // extract Supine/Prone marks from 'spEntry->Name'.
      CString csTitle;
      std::string sName(spEntry->Name);
      std::string sSubName;
      std::string sStudyOrientation;
      const bool bDistanceUndetermined = (sName[0]=='-');
      if (bDistanceUndetermined) 
				sSubName = sUncertainDistanceFormat;

      if( (sName.find(LoadResourceString(IDS_SUPINE_EXTRA)) != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen(LoadResourceString(IDS_SUPINE_EXTRA)));
        }
        sStudyOrientation = LoadResourceString(IDS_SUPINE_EXTRA);
      }
      else if( (sName.find(LoadResourceString(IDS_PRONE_EXTRA)) != std::string::npos ) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr( strlen(LoadResourceString(IDS_PRONE_EXTRA)) );
        }
        sStudyOrientation = LoadResourceString( IDS_PRONE_EXTRA );
      }
			/// ===================================
			/// The following block is for English viewer load French report or vias verse. It had not completely solve the problem.
			/// So, by design the viewer does not support across language session load. DQ 20120111
			else if( (sName.find("(Supine)") != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen("(Supine)"));
        }
        sStudyOrientation = LoadResourceString(IDS_SUPINE_EXTRA);
      } 
			else if( (sName.find("(Prone)") != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen("(Prone)"));
        }
        sStudyOrientation = LoadResourceString( IDS_PRONE_EXTRA );
      } 
			else if( (sName.find("(Décubitus)") != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen("(Décubitus)"));
        }
        sStudyOrientation = LoadResourceString(IDS_SUPINE_EXTRA);
      } 
			else if( (sName.find("(Procubitus)") != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other_Extracolonic) )
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen("(Procubitus)"));
        }
        sStudyOrientation = LoadResourceString( IDS_PRONE_EXTRA );
      } 
			/// End of the ugly code block
			/// ====================================
      else
      {
        if (!bDistanceUndetermined)
        {
          sSubName = spEntry->Name;
        }
      } // ELSE

      if( sName[sName.length()-1] == '*' )
      {
        bPrintUncertainDistanceWarning = true;
      }

      // pre-processing 'comments': remove the last '\n'(s).
      _bstr_t bsComments = spEntry->Comments;
      std::string sComments = TextPreprocessing( std::string(bsComments.length() > 0 ? bsComments : "" ),
                                    std::string(sCommentsFormat), std::string(""), /* new-line*/true, 19 );

      //format textual portion of finding based on type
      switch( spEntry->ColonFinding )
      {
        case ReportLib::Polyp:
        {
          CString strQuantity;
          /// if (maximum==0.0 && Perpendicular==0.0) -> no display of these info.
          if( spEntry->Maxiumum != 0.0F )
          {
            CString sMM( LoadResourceString(IDS_MM)); 
						sMM.Remove('.');
            if( spEntry->Perpendicular != 0.0F )
              strQuantity.Format( "%.1f x %.1f %s ", spEntry->Maxiumum, spEntry->Perpendicular, sMM );
            else
              strQuantity.Format( "%.1f %s ", spEntry->Maxiumum, sMM );
          }

          if (spEntry->Translucency == ReportLib::Indeterminate)
          {
            csTitle = LoadResourceString( IDS_COLONFINDING_NOTRANSLUCENCY_FORMAT );
            csTitle.Replace("#<dimensions>", strQuantity);
            csTitle.Replace("#<shape>", (strQuantity.GetLength()>0 ? LowerCaseString(m_mapPolypShape[spEntry->GetShape()].c_str()) : m_mapPolypShape[spEntry->GetShape()].c_str()));
            csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
            csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
            csTitle.Replace("#<flexure>",
              ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
              ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
            csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          }
          else
          {
            csTitle = LoadResourceString( IDS_COLONFINDING_TRANSLUCENCY_FORMAT );
            csTitle.Replace("#<dimensions>", strQuantity);
            csTitle.Replace("#<shape>", (strQuantity.GetLength()>0 ? LowerCaseString(m_mapPolypShape[spEntry->GetShape()].c_str()) : m_mapPolypShape[spEntry->GetShape()].c_str()));
            csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
            csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
            csTitle.Replace("#<flexure>",
              ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
              ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
            csTitle.Replace("#<translucency>", LowerCaseString(m_mapTranslucency[spEntry->GetTranslucency()].c_str()));
            csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          }
          break;
        }
        case ReportLib::Diverticulum:
        {
          csTitle = LoadResourceString( IDS_COLONFINDING_OTHER_FORMAT );

          csTitle.Replace("#<finding>", LoadResourceString(IDS_DIVERTICULUM));
          csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
          csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
          csTitle.Replace("#<flexure>",
            ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
            ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
          csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          break;
        }
        case ReportLib::Stenosis:
        {
          csTitle = LoadResourceString( IDS_COLONFINDING_OTHER_FORMAT );

          csTitle.Replace("#<finding>", LoadResourceString(IDS_STENOSIS));
          csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
          csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
          csTitle.Replace("#<flexure>",
            ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
            ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
          csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          break;
        }
        case ReportLib::Other_Normal: // Normal
        {
          csTitle = LoadResourceString( IDS_NORMAL_FINDING_FORMAT );
          csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
          csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
          csTitle.Replace("#<flexure>",
            ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
            ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
          csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          break;
        }
        case ReportLib::Other_Colonic: // Colonic Other
        {
          csTitle = LoadResourceString( IDS_OTHER_LOCATION_FORMAT );
          csTitle.Replace("#<segment>", LowerCaseString(m_mapColonSegment[spEntry->GetLocation()&~ReportLib::NearFlexure].c_str()));
          csTitle.Replace("#<location>", LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())));
          csTitle.Replace("#<flexure>",
            ((spEntry->GetLocation()&ReportLib::NearFlexure)==ReportLib::NearFlexure)
            ? LowerCaseString(LoadResourceString(IDS_NEAR_FLEXURE))+" " : "");
          csTitle.Replace("#<comments>", static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()));
          break;
        }
        case ReportLib::Other_Extracolonic: // Colonic Extra.
        {
          //csTitle.Format( IDS_EXTRA_COLONIC_FINDING_FORMAT, (static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString())) );
          break;
        }
      }

			std::string sTemp;
      if ( spEntry->ColonFinding != ReportLib::Other_Extracolonic )
      { // add Supine/Prone mark.
        CString sLine = CString(sStudyOrientation.c_str()) + ' ' + csTitle;

        // check to see if there is enough room to begin to output the images
				sTemp = sLine;

				float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sTemp, 0.0f );

        // check if we need a new page
        if( m_fCurrentY - (fLineNum * m_pPdf->GetTextHeight()) < m_fBottomMargin )
        { // start a new page
          EndPage();
          BeginPage( false );

          // Need to put a space in, the text box is spaced by the 0.25" image spacing.  So, on new page need to manually space it.
          m_fCurrentY -= 0.20 * FACTOR;
        }
				
			  m_pPdf->SetLineWidth( 1.0f );
		    /// draw gray box for finding statement area
				m_pPdf->SetFillColor( 0.8F, 0.8F, 0.8F );
				m_pPdf->DrawBox( LEFT_MARGIN, m_fCurrentY + m_pPdf->GetTextHeight(), RIGHT_MARGIN - LEFT_MARGIN, fLineNum * m_pPdf->GetTextHeight() + 5.0F, 2 );
				/// draw statement text inside the box
				m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0f, m_fCurrentY + 2.0F, RIGHT_MARGIN - LEFT_MARGIN - 2.0f, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
      }
      else
      {
        try
        {
          ReportLib::IReportOtherFindingPtr spFinding =  m_pReportRevision->OtherFindings->GetItem( spEntry->OtherFindingID );
          WriteOtherFinding( spFinding );
        }
        catch(_com_error& )
        {
          LogErr( "Likely extra-colonic finding without a child.  This should've been taken care of when the report was being opened",
                  "VxReportCreator", "WriteColonFindings" );
        } //catch
      } // else

      // get number of images in the finding
      int4 iNumImages = spEntry->Images->Count;

      // for each image in the finding
      for (int4 iImage(0), iActualImg(0); iImage < iNumImages; iImage++)
      {
        if (iImage == 0 && bExtraColonic) // skip overview in extra-colonic findings
          continue;

        // grab the image from the colon finding
        ReportLib::IImagePtr spImage = spEntry->Images->GetItem( (long)iImage );

        // calculate the x value(3 images per line)
        int4 xPos = LEFT_MARGIN + (iActualImg%3) * ( IMAGE_WIDTH + IMAGE_SPACE );
        // if this is the first image on a line calculate the y value
        if( iActualImg % 3 == 0 && iImage > 0 )
        {
					if(!(bExtraColonic && iImage == 1))
          {
            m_fCurrentY -= (IMAGE_HEIGHT + IMAGE_SPACE);
          }
        }

        // check if we need a new page
        if( m_fCurrentY - IMAGE_HEIGHT < m_fBottomMargin )
        { // start a new page
          EndPage();
          BeginPage( false );
        } // if

        // output the image
        WriteImage( xPos, m_fCurrentY, spImage );

        iActualImg ++;
      } // end for each image

			m_fCurrentY -= (IMAGE_HEIGHT + 2.0f * m_pPdf->GetTextHeight() );
    } // end for each colon finding
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "WriteColonFindings");
    throw e;
  }
} // WriteColonFindings()


/**
 * Writes physician recommendations to report.
 */
void VxReportCreator::WriteRecommendations()
{
  try
  { // get the info object
    ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

    CheckPage();
    m_fCurrentY -= m_pPdf->GetTextHeight();

    // grab the recommendations bitmask
    int4 iRecommendations = spInfo->Recommendation;

    bool bIncludeCRADS( m_globalState.Get<bool>( APP_SETTING_REPORT_ADD_CRADS ));
    if (iRecommendations > 0 || spInfo->OtherRecommendation.length() > 0 || bIncludeCRADS )
    {
      // get the other recommendation string
      CString sOtherRecommendation(static_cast<LPCTSTR>(spInfo->OtherRecommendation.length()>0?spInfo->OtherRecommendation:_bstr_t()));
      std::string sNormalOtherRecommStr = TextPreprocessing(std::string(sOtherRecommendation), std::string(" "), std::string(sOtherExtraFormat), false, 1);

      // output crads
      CString sCRADS_C;
      CString sCRADS_E;
      if( bIncludeCRADS )
      { // KLUDGE: Add text for C-RADS and XC C-RADS
        sCRADS_C += LoadResourceString(IDS_CRADS_OUTPUT) + (ToAscii(spInfo->CRADSCategory)).c_str();
        ColonReport::ReportMiscellaneous::GetExtraColonicCRADSCat( m_pReportRevision, sCRADS_E );
      }

      CString titleStr( LoadResourceString( IDS_RECOMMENDATIONS ) );

      // output the "Recommendations" title and underline
      titleStr.MakeUpper();
			std::string sTemp = LPCTSTR(titleStr + ':');
			float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sTemp, 0.0f );
			CheckPage( fLineNum + 1.0F );
			m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
			m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
			m_pPdf->SetLineWidth( 1.5f );
			m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY + 4.0f, RIGHT_MARGIN - 2.0, m_fCurrentY + 4.0f );
			m_fCurrentY -= m_pPdf->GetTextHeight();
				
      // check for the existence of each bit value if it exists print of text
      if (iRecommendations&ReportLib::FollowUp)
      {
        CString s;
        s.Format( LoadResourceString( IDS_OTHER_FINDINGS_RECOMMENDATIONS_FOLLOWUP_FORMAT ),
          m_mapRecommendType[spInfo->GetRecommendationType()].c_str(),
          spInfo->RecommendationTime,
          m_mapRecommendUnit[spInfo->GetRecommendationUnit()].c_str()
          );
				sTemp = s;
			  fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
      } // if

      if (iRecommendations&ReportLib::Removal)
      {
        CheckPage();
        CString s( LoadResourceString( IDS_OTHER_FINDINGS_RECOMMENDATIONS_POLYP_REMOVAL));
				sTemp = s;
			  fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
      }

      // if the other string has a value print it out
      if (spInfo->OtherRecommendation.length())
      {
        ColonReport::ReportMiscellaneous::RemoveLegacyCRADSCategory( sOtherRecommendation );
        std::string sNormalOtherRecommStr = TextPreprocessing(std::string(sOtherRecommendation), " ", std::string(sOtherExtraFormat), false, 1);
			  fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sNormalOtherRecommStr, 0.0f );
				CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sNormalOtherRecommStr );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
      }

      // output crads
			if( sCRADS_C.GetLength() > 0 || sCRADS_E.GetLength() > 0 )
      {
				int4 iLines = (iRecommendations > 0 && spInfo->OtherRecommendation.length() == 0) ? 2 : 1;
				m_fCurrentY -= float4(iLines) * m_pPdf->GetTextHeight();
			}

      if( sCRADS_C.GetLength() > 0 )
      { // need extra new line if only contains recommendations
				sTemp = sCRADS_C;
        std::string sDefinition = LPCTSTR(LoadResourceString(IDS_CRADS_VERBOSE_C0 + spInfo->CRADSCategory));

			  fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f ) + 1 + GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sDefinition, 0.0f );
				CheckPage( fLineNum );
				
        fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f );
        m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();

        fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sDefinition, 0.0f );
        m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sDefinition );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
			}

      if( sCRADS_E.GetLength() > 0 )
			{ // Put a line space between the C-RADS and E-RADS.
        if(sCRADS_C.GetLength() > 0)
        {
          CheckPage();
				  m_fCurrentY -= m_pPdf->GetTextHeight();
        }

        sTemp = sCRADS_E;
        std::string sDefinition = "";

        int4 position = sTemp.find("|");

        if(sTemp.size() > position + 1)
        {
          sDefinition = sTemp.substr(position + 1);
        }

        sTemp = sTemp.substr(0, position);

			  fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f );
        if(sDefinition.size() > 0)
        {
          fLineNum += 1 + GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sDefinition, 0.0f );
        }

        CheckPage( fLineNum );
				
        fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sTemp, 0.0f );
        m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
				m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();

        if(sDefinition.size() > 0)
        {
          fLineNum = GetRequiredLines( LEFT_MARGIN + 2.0f, m_fCurrentY, sDefinition, 0.0f );
          m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sDefinition );
				  m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
        }
      }
    } // if
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "WriteRecommendations");
    throw e;
  }
} // WriteRecommendations


/**
 * Writes physician impression info to report.
 */
void VxReportCreator::WriteImpression()
{
  try
  {
    ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

    if (spInfo->Impression.length()<=0)
      return;

    // Space out from previous section.
    m_fCurrentY -= m_pPdf->GetTextHeight();

    bool bShowImpression = true;
    bool bShowParameters = false;

		std::string sIndicator( std::string( LPCSTR(LoadResourceString( IDS_EXAMINATION_QUALITY ))) );
		sIndicator = sIndicator + ":";

    std::string sImpression( spInfo->Impression );
    std::string sExaminationParameters = "";

    int position = sImpression.find( sIndicator );
    if(position != std::string::npos && sImpression.length() > position + 1)
    {
      bShowParameters = true;
      sExaminationParameters = sImpression.substr(position, sImpression.length() - position);
    }

		if(sImpression.compare(1, 23, sIndicator.c_str()) == 0)
    {
      bShowImpression = false;
    }
    else
    {
      sImpression = sImpression.substr(0, position);
    }

    CString s( LoadResourceString( IDS_IMPRESSION ) );
    s.MakeUpper();
		CString sLine = static_cast<LPCTSTR>(s + ':');

		std::string sTemp = sLine;
		float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sTemp, 0.0f );
		CheckPage( fLineNum + 1.0F );
		m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
		m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
		m_pPdf->SetLineWidth( 1.5f );
		m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY + 4.0f, RIGHT_MARGIN - 2.0f, m_fCurrentY + 4.0f );
		m_fCurrentY -= m_pPdf->GetTextHeight();
		
    if(bShowImpression)
    {
      std::string sNormalStr = TextPreprocessing( sImpression, std::string(" "), std::string(" - "), false, 1);
      fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sNormalStr, 0.0f );
      CheckPage( fLineNum );
				m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0f, m_fCurrentY + 2.0F, RIGHT_MARGIN - LEFT_MARGIN - 2.0f, sNormalStr );
      m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
    }

    if(bShowParameters)
    {
      if(bShowImpression)
      {
        CheckPage();
        m_fCurrentY -= m_pPdf->GetTextHeight();
      }

      std::string sNormalStr = TextPreprocessing( sExaminationParameters, std::string(" "), std::string(" - "), false, 1);
      fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, sNormalStr, 0.0f );
      CheckPage( fLineNum );
			m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0f, m_fCurrentY + 2.0F, RIGHT_MARGIN - LEFT_MARGIN - 2.0f, sNormalStr );
      m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
    }
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "WriteImpression");
    throw e;
  }
} // WriteImpression()


/**
 * Writes author info (diagnosing doctor) to report.
 */
void VxReportCreator::WriteAuthor()
{
  try
  { // get the info object
    ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

    // if blank than return
    if (spInfo->Author.length() == 0)
    {
      return;
    }

    // check if we need a new page
    CheckPage( 4 );

		m_fCurrentY -= 3.0f * m_pPdf->GetTextHeight();
    m_pPdf->SetLineWidth( 1.5f );
	  m_pPdf->DrawLine( LEFT_MARGIN + 2.0f, m_fCurrentY - 1.0F, PAGE_WIDTH/2.0f, m_fCurrentY - 1.0F);
		m_fCurrentY -= m_pPdf->GetTextHeight();

	  std::string sTemp = spInfo->Author;
    m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0F, m_fCurrentY, RIGHT_MARGIN - LEFT_MARGIN - 2.0F, sTemp );
		m_fCurrentY -= (GetRequiredLines(  LEFT_MARGIN + 2.0F, m_fCurrentY, sTemp, 0.0f ) + 1.0f) * m_pPdf->GetTextHeight();
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "VxReportCreator", "WriteAuthor");
    throw e;
  }
} // WriteAuthor()


/**
 * Returns the number of lines required to write the given string.
 *
 * @param x             X position to output text.
 * @param y             Y position to output text.
 * @param sText         Text to query.
 * @param fRightOffset   Rightt margin offset.
 * @return              Number of lines required to output string.
 */
float4 VxReportCreator::GetRequiredLines( float4 x, float4 y, std::string sText, float4 fRightOffset )
{
  float4 fHeight(0.0f);

	fHeight = m_pPdf->GetWrappedTextHeight( RIGHT_MARGIN - x - fRightOffset,  sText );

	return fHeight / m_pPdf->GetTextHeight();
} // GetRequiredLines()


/**
 * Outputs an image to report.
 *
 * @param x         X position for image.
 * @param y         Y position for image.
 * @param spImage   Image to output.
 */
void VxReportCreator::WriteImage( const float4 x, const float4 y, ReportLib::IImagePtr& spImage )
{
 
  try
  {
    int iSize = spImage->ImageSize;

    m_pPdf->AddImage(std::shared_ptr<Gdiplus::Bitmap>( Gdiplus::Bitmap::FromHBITMAP((HBITMAP)spImage->Bitmap, HPALETTE(0)) ));

    m_pPdf->FitImage( x, y, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 0 );
  }
  catch ( pdf::PdfException & )
  {
    LogErr("Failed to load image to PDF object", "WriteImage", "VxReportCreator");
  }

} // WriteImage()


/**
 * Start a new page in report if necessary (if current page will be overrun)
 *  prior to adding n new lines.
 *
 * @param iNeededLines   Number of lines that will be output.
 */
void VxReportCreator::CheckPage(const int4 iNeededLines)
{
  // see if adding the needed lines will go into the bottom margin
	if( m_fCurrentY - (iNeededLines * m_pPdf->GetTextHeight()) < m_fBottomMargin )
  { // start a new page if it does
    EndPage();
    BeginPage( false );
  } // if
} // CheckPage()


/**
 * Pre-process string text prior to output to report.
 *
 * @param sTextStr              Text to pre-process.
 * @param sHeaderStr            Header to add to string.
 * @param sMarkStr              Text to put a start of each newline (in place of '\n')
 * @param bNewLineAfterHeader   Whether to start a newline after the header
 * @param iNumOfSpace           Number of spaces to prefix to each new line.
 * @return                      Preprocessed text.
 */
std::string VxReportCreator::TextPreprocessing(const std::string & sTextStr, const std::string & sHeaderStr,
                                             const std::string & sMarkStr, bool bNewLineAfterHeader, int4 iNumOfSpace)
{
  // pre-processing 1: remove the last '\n'(s).
  std::string sNewText(sTextStr);
  if (sNewText.length())
  {
    int4 iRPos = sNewText.rfind("\n");
    while (iRPos !=  std::string::npos)
    {
      if (iRPos < sNewText.length()-1) break;

      sNewText.erase(iRPos-1, 2);
      iRPos = sNewText.rfind("\n");
    }
  }

  // preprocessing 2: add space before each new line.
  if (sNewText.length())
  {
    int4 iPos = sNewText.find_first_of("\n");
    while (true)
    {
      if (iPos == std::string::npos) break;

      std::string sSpaceStr = sMarkStr;
      for (int4 i=0; i<iNumOfSpace; i++) sSpaceStr.insert(0, " ");

      sNewText.insert(iPos+1, sSpaceStr);
      iPos = sNewText.find_first_of("\n", iPos+1);
    }
  }

  // preprocessing 3: add a header to the string.
  if (sNewText.length())
  {
    sNewText.insert(0, sHeaderStr+sMarkStr);
    if (bNewLineAfterHeader) sNewText.insert(0, "\n");
  }

  return sNewText;
} // TextPreprocessing()


/**
 * write out a line of other-findings info
 *
 * @param spFinding   The finding to output
 */
void VxReportCreator::WriteOtherFinding( const ReportLib::IReportOtherFindingPtr spFinding )
{
  // build textual representation
  CString s;
  CString sQuantity;
  if (spFinding->MaximumSize != 0.0F)
  {
    if (spFinding->PerpendicularSize != 0.0F)
      sQuantity.Format("%.1f x %.1f %s ", spFinding->MaximumSize, spFinding->PerpendicularSize, LoadResourceString(IDS_CM));
    else
      sQuantity.Format("%.1f %s ", spFinding->MaximumSize, LoadResourceString(IDS_CM));
  }

  std::string sComment(spFinding->Comment);
  if (sComment.length())
  {
    int4 iPos = sComment.find_first_of("\n");
    while (iPos != std::string::npos)
    {
      sComment.replace(iPos-1, 2, " / ");
      iPos = sComment.find_first_of("\n", iPos);
    }
    if (sComment.length()) sComment.insert(0, ": ");
  }

  switch(spFinding->GetFinding())
  {
  case ReportLib::Lung_Nodule:
  case ReportLib::Lung_Granuloma:
    s.Format(g_sOtherFindingsEntryFormat,
      sQuantity,
      (sQuantity.GetLength()>0 ? m_mapLungLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_mapLungLocation[spFinding->GetLocation()].c_str())),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Lung_Pneomuthorax:
    s.Format(g_sLungPneomuthoraxFormat,
      UpperHeadString(m_mapPneumothoraxSize[spFinding->GetSize()].c_str()),
      m_mapPneumothoraxLocation[spFinding->GetLocation()].c_str(),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.c_str())
      );
    break;

  case ReportLib::Liver_Mass:
  case ReportLib::Liver_Cyst:
    s.Format(g_sOtherFindingsEntryFormat,
      sQuantity,
      (sQuantity.GetLength()>0 ? m_mapLiverLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_mapLiverLocation[spFinding->GetLocation()].c_str())),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Adrenal_Mass:
    s.Format(g_sOtherFindingsEntryFormat,
      sQuantity,
      (sQuantity.GetLength()>0 ? m_mapAdrenalLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_mapAdrenalLocation[spFinding->GetLocation()].c_str())),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Renal_Mass:
  case ReportLib::Renal_Cyst:
  case ReportLib::Renal_Stones:
    s.Format(g_sOtherFindingsEntryFormat,
      sQuantity,
      (sQuantity.GetLength()>0 ? m_mapRenalLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_mapRenalLocation[spFinding->GetLocation()].c_str())),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Renal_Scar:
    s.Format(g_sRenalScarFormat,
      UpperHeadString(m_mapRenalLocation[spFinding->GetLocation()].c_str()),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Renal_Hydronephrosis:
    s.Format(g_sRenalHydronephrosisFormat,
      UpperHeadString(m_mapDegree[spFinding->GetDegree()].c_str()),
      m_mapKidneyLocation[spFinding->GetLocation()].c_str(),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Vascular_AAA:
    {
      CString sLength;
      if (spFinding->Length != 0.0F)
      {
        sLength.Format( LoadResourceString( IDS_VASCULAR_FINDING_LENGTH_FORMAT ), spFinding->Length);
      }

      s.Format(g_sOtherFindingsVascularAAAEntryFormat,
        UpperHeadString(m_mapVascularLocation[spFinding->GetLocation()].c_str()),
        sQuantity,
        m_mapOtherFindings[spFinding->GetFinding()].c_str(),
        sLength,
        static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
        );
    }
    break;

  case ReportLib::Vascular_ArteralCalcification:
    s.Format(g_sVascularArteralCalcificationFormat,
      UpperHeadString(m_mapDegree[spFinding->GetDegree()].c_str()),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Spleen_Mass:
  case ReportLib::Spleen_Granuloma:
    s.Format(g_sOtherFindingsEntryFormat,
      sQuantity,
      (sQuantity.GetLength() > 0 ? m_mapSplenicLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_mapSplenicLocation[spFinding->GetLocation()].c_str())),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::GI_InguinalHernia:
    s.Format(g_sGIInguinalHerniaFormat,
      UpperHeadString(m_mapAdrenalLocation[spFinding->GetLocation()].c_str()),
      m_mapOtherFindings[spFinding->GetFinding()].c_str(),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Liver_Gallstones:
  case ReportLib::Liver_FattyLiver:
  case ReportLib::GI_HiatalHernia:
  case ReportLib::Gynecology_UterineFibroids:
    s.Format(g_sGynecologyUterineFibroidsFormat,
      UpperHeadString(m_mapOtherFindings[spFinding->GetFinding()].c_str()),
      static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
      );
    break;

  case ReportLib::Misc_Other:
    s.Format( LoadResourceString( IDS_COLONFINDING_MISCELLANEOUS_OTHER_FORMAT ), static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString()));
    break;

  default:
    ;
  }

	float4 fLineNum = GetRequiredLines( LEFT_MARGIN, m_fCurrentY, s.GetBuffer(), 0.0f );

  // check if we need a new page
  if( m_fCurrentY - (fLineNum * m_pPdf->GetTextHeight()) < m_fBottomMargin )
  { // start a new page
    EndPage();
    BeginPage( false );

    // Need to put a space in, the text box is spaced by the 0.25" image spacing, from the previous finding's images.  So, on new page need to manually space it.
    m_fCurrentY -= 0.20 * FACTOR;
  }
				
	m_pPdf->SetLineWidth( 1.0f );
	/// draw gray box for finding statement area
	m_pPdf->SetFillColor( 0.8F, 0.8F, 0.8F );
	m_pPdf->DrawBox( LEFT_MARGIN, m_fCurrentY+m_pPdf->GetTextHeight(), RIGHT_MARGIN - LEFT_MARGIN, fLineNum * m_pPdf->GetTextHeight() + 5.0F, 2 );
	/// draw statement text inside the box
	m_pPdf->DrawWrappedText( LEFT_MARGIN + 2.0f, m_fCurrentY + 2.0F, RIGHT_MARGIN - LEFT_MARGIN - 2.0f, s.GetBuffer() );

	m_fCurrentY -= fLineNum * m_pPdf->GetTextHeight();
} // WriteOtherFinding()


/**
 *  write out page number
 */
void VxReportCreator::WritePageNumber()
{
	const int4 iNumOfPages = m_pPdf->PageCount();

	std::string s;
  CString st = LoadResourceString( IDS_PAGE_NUMBERING_FORMAT );

	for( int4 i=1; i<= iNumOfPages; i++ )
	{
		m_pPdf->SelectPage( i );
    m_pPdf->SetTextSize(8);

		CString sPage;
		sPage.Format( st, i, iNumOfPages );
		s = sPage; 
		m_pPdf->SetTextAlignment( pdf::Pdf::ALIGN_RIGHT );
		m_pPdf->DrawTextA( RIGHT_MARGIN - 2.0f, 36.0f,  s );
	} // for i
} // WritePageNumber()


/**
* write out the xml report file based on the current report Revision
*/
void VxReportCreator::WriteXMLReport()
{
	try
	{
		CoInitialize(NULL);

		vxDOMDocument::Initialize();

		MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
		HRESULT hRes = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pXmlDoc->preserveWhiteSpace = VARIANT_TRUE;

		/// add xml data
		MSXML2::IXMLDOMProcessingInstructionPtr pInstr;
		pInstr = pXmlDoc->createProcessingInstruction("xml", "version='1.0' encoding='utf-8'");
		pXmlDoc->appendChild(pInstr);

		MSXML2::IXMLDOMElementPtr pElement, pReportHeader, pEltop, pElnest, pElinner;

		ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

		/// set report version
		pReportHeader = AppendAttributeChild(pXmlDoc, pXmlDoc, "V3DColonReport", "version", variant_t(1.0));

		std::string sss;
		long jj;

		/// report information block ---------------------------------
		float4 fVersion(1.0f);
		pEltop = AppendAttributeChild(pXmlDoc, pReportHeader, "Info", "version", variant_t(ToAscii(fVersion).c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientName", "string", variant_t(spInfo->GetPatientName()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientID", "string", variant_t(spInfo->GetPatientID()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientAge", "string", variant_t(spInfo->GetPatientAge()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientBirthday", "string", variant_t(spInfo->GetPatientBirthday()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientSex", "enum", variant_t(ToAscii(spInfo->GetPatientSex()).c_str()));
		sss = (spInfo->GetPatientSex() == 0) ? "male" : "female";
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientSexString", "string", variant_t(sss.c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "PatientWeight", "string", variant_t(spInfo->GetPatientWeight()));
		DATE dd = spInfo->GetStudyDate();
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "StudyDate", "double", variant_t(ToAscii(dd).c_str()));
		sss = ToAscii(COleDateTime(dd).GetYear()) + "-" + ToAscii(COleDateTime(dd).GetMonth()) + "-" + ToAscii(COleDateTime(dd).GetDay()) + ":" + ToAscii(COleDateTime(dd).GetHour());
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "StudyDateString", "string", variant_t(sss.c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "ReferringPhysician", "string", variant_t(spInfo->GetReferringDoctor()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "ReportAuthor", "string", variant_t(spInfo->GetAuthor()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "InstitutionName", "string", variant_t(spInfo->GetInstitutionName()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "Indication", "long", variant_t(ToAscii(spInfo->GetIndication()).c_str()));

		const ReportLib::IndicationsEnum veInd[] = { ReportLib::Screening, ReportLib::PersonalHistoryOfPolyps, 
			ReportLib::PersonalHistoryOfCancer, ReportLib::FamilyHistoryOfPolyps, ReportLib::FamilyHistoryOfCancer, 
			ReportLib::Constipation, ReportLib::Anemia, ReportLib::Hematochezia, ReportLib::AbnormalImaging, 
			ReportLib::AbdominalPain, ReportLib::IBD, ReportLib::Diarrhea, ReportLib::FailedColonoscopy, ReportLib::HighSedationRisk };
		sss = "";
		jj = spInfo->GetIndication();
		for (int4 n = 0; n <= sizeof(veInd) / sizeof(ReportLib::IndicationsEnum); n++)
		{
			if (jj & veInd[n])
				sss = sss + m_mapIndications[ReportLib::IndicationsEnum(veInd[n])] + ",";
		} // for n
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "IndicationString", "string", variant_t(sss.c_str()));

		pElement = AppendAttributeChild(pXmlDoc, pEltop, "Impression", "string", variant_t(spInfo->GetImpression()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "OtherIndication", "string", variant_t(spInfo->GetOtherIndication()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "OtherRecommendation", "string", variant_t(spInfo->GetOtherRecommendation()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "CRADSCategoryColon", "long", variant_t(ToAscii(spInfo->GetCRADSCategory()).c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "CRADSCategoryColonString", "string", 
			         variant_t(LoadResourceString(IDS_CRADS_VERBOSE_C0 + spInfo->CRADSCategory)));

		pElement = AppendAttributeChild(pXmlDoc, pEltop, "Recommendation", "long", variant_t(ToAscii(spInfo->GetRecommendation()).c_str()));
		jj = spInfo->GetRecommendation();
		if (jj & ReportLib::Removal)
		{
			pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationString", "string",
				variant_t(LoadResourceString(IDS_OTHER_FINDINGS_RECOMMENDATIONS_POLYP_REMOVAL)));
		} // if

		pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationTime", "long", variant_t(ToAscii(spInfo->GetRecommendationTime()).c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationType", "enum", variant_t(ToAscii(spInfo->GetRecommendationType()).c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationTypeString", "string", 
			                              variant_t(m_mapRecommendType[spInfo->GetRecommendationType()].c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationUnit", "enum", variant_t(ToAscii(spInfo->GetRecommendationUnit()).c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "RecommendationUnitString", "string", 
			                              variant_t(m_mapRecommendUnit[spInfo->GetRecommendationUnit()].c_str()));
		pElement = AppendAttributeChild(pXmlDoc, pEltop, "ExamQuality", "long", variant_t(ToAscii(spInfo->GetExamQuality()).c_str()));

		/// report entries ----------------------------------------------------------------------
		ReportLib::IReportEntriesPtr spEntries = m_pReportRevision->Entries;

		const int4 iNumOfEntry(spEntries->GetCount());

		if (iNumOfEntry > 0)
		{
			pEltop = AppendAttributeChild(pXmlDoc, pReportHeader, "Entries", "version", variant_t(1.0));

		  register int4 i;
			/// work on each entry
		  for (i = 0; i < iNumOfEntry; i++)
		  {
			  ReportLib::IReportEntryPtr spEntry = spEntries->GetItem(long(i));

			  pElnest = AppendAttributeChild(pXmlDoc, pEltop, "Entry", "id", variant_t(i));
			  pElinner = AppendAttributeChild(pXmlDoc, pElnest, "Colonic", "version", variant_t(1.0));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Name", "string", variant_t(spEntry->Name));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Comment", "string", variant_t(spEntry->Comments));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Maximum", "float", variant_t(spEntry->GetMaxiumum()));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Perpendicular", "float", variant_t(spEntry->GetPerpendicular()));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "OrigDistanceToRectum", "float", variant_t(spEntry->GetOrigDistanceToRectum()));

				ColonReport::ReportVariant dist;
				dist.Attach(spEntry->CameraCoordinates.Detach());
				Point<float4> pos; Normal3D<float4> orientation; Normal3D<float4> up;
				dist.asCameraCoordinates(pos, orientation, up);
				sss = "pos=\"" + ToAscii(pos.m_x) + "," + ToAscii(pos.m_y) + "," + ToAscii(pos.m_z) + "\" normal=\"" + 
					    ToAscii(orientation.X()) + "," + ToAscii(orientation.Y()) + "," + ToAscii(orientation.Z()) +
					    "\" up=\"" + ToAscii(up.X()) + "," + ToAscii(up.Y()) + "," + ToAscii(up.Z());
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "CameraLocationString", "string", variant_t(sss.c_str()));

				pElement = AppendAttributeChild(pXmlDoc, pElinner, "FindingType", "enum", variant_t(spEntry->GetColonFinding()));
				switch (spEntry->GetColonFinding())
			  {
			  case ReportLib::Polyp:
				  sss = "Polyp";
				  break;
			  case ReportLib::Diverticulum:
				  sss = "Diverticulum";
				  break;
			  case ReportLib::Stenosis:
				  sss = "Stenosis";
				  break;
			  case ReportLib::Other_Normal:
				  sss = "Normal";
				  break;
			  case ReportLib::Other_Colonic:
				  sss = "Other Colonic";
				  break;
			  case ReportLib::Other_Extracolonic:
				  sss = "Extracolonic";
				  break;
			  } // switch
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "FindingTypeString", "string", variant_t(sss.c_str()));

				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Location", "enum", variant_t(spEntry->GetLocation()));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "LocationString", "string", 
					variant_t(m_mapColonSegment[spEntry->GetLocation() & ~ReportLib::NearFlexure].c_str()));

				if ((spEntry->GetLocation() & ReportLib::NearFlexure) == ReportLib::NearFlexure)
					pElement = AppendAttributeChild(pXmlDoc, pElinner, "NearFlexure", "string", variant_t("yes"));
				else
					pElement = AppendAttributeChild(pXmlDoc, pElinner, "NearFlexure", "string", variant_t("no"));

				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Shape", "enum", variant_t(spEntry->GetShape()));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "ShapeString", "string", 
					                              variant_t(m_mapPolypShape[spEntry->GetShape()].c_str()));

				pElement = AppendAttributeChild(pXmlDoc, pElinner, "Translucency", "enum", variant_t(spEntry->GetTranslucency()));
				pElement = AppendAttributeChild(pXmlDoc, pElinner, "TranslucencyString", "string", 
					                              variant_t(m_mapTranslucency[spEntry->GetTranslucency()].c_str()));

				/// extra colonic finding information ----------------------------------------------
				if (ToAscii(spEntry->GetOtherFindingID()).size() > 1)
				{
					pElinner = AppendAttributeChild(pXmlDoc, pElnest, "ExtraColonic", "version", variant_t(1.0));
					pElement = AppendAttributeChild(pXmlDoc, pElinner, "Flag", "enum", variant_t("Other extracolonic"));

					ReportLib::IReportOtherFindingsPtr spOthers = m_pReportRevision->GetOtherFindings();
					if (spOthers == NULL)
						throw ex::IOException(LogRec("Other findings list was empty.", "VxReportCreator", "WriteXMLReport"));

					ReportLib::IReportOtherFindingPtr spFinding = spOthers->GetItem(spEntry->OtherFindingID);
					if (spFinding != NULL)
					{
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Comment", "string", variant_t(spFinding->GetComment()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Degree", "long", variant_t(spFinding->GetDegree()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "DegreeString", "string", 
							                              variant_t(ToAscii(m_mapDegree[spFinding->GetDegree()]).c_str()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Size", "long", variant_t(spFinding->GetSize()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Location", "long", variant_t(spFinding->GetLocation()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Length", "float", variant_t(spFinding->GetLength()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "PerpendicularSize", "float", variant_t(spFinding->GetPerpendicularSize()));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "MaximumSize", "float", variant_t(spFinding->GetMaximumSize()));
						jj = spFinding->GetCRADSCategory();
						sss = LoadResourceString(IDS_EXTRA_COLONIC_CRADS) + (ToAscii(jj) + "|").c_str() +
							    LoadResourceString(IDS_CRADS_VERBOSE_E0 + jj);
						pElement = AppendAttributeChild(pXmlDoc, pElinner, "CRADSCategoryExtraColonic", "long", variant_t(jj));
					  pElement = AppendAttributeChild(pXmlDoc, pElinner, "CRADSCategoryExtraColonicString", "string", variant_t(sss.c_str()));

						pElement = AppendAttributeChild(pXmlDoc, pElinner, "ExtraColonicFindingType", "enum", variant_t(spFinding->GetFinding()));
						jj = spFinding->GetFinding();
						switch (jj)
						{
						case ReportLib::Lung_Nodule:
						case ReportLib::Lung_Granuloma:
							sss = m_mapLungLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Lung_Pneomuthorax:
							sss = m_mapPneumothoraxSize[spFinding->GetSize()] + "," + m_mapPneumothoraxLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Liver_Mass:
						case ReportLib::Liver_Cyst:
							sss = m_mapLiverLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Adrenal_Mass:
							sss = m_mapAdrenalLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Renal_Mass:
						case ReportLib::Renal_Cyst:
						case ReportLib::Renal_Stones:
							sss = m_mapRenalLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Renal_Scar:
							sss = m_mapRenalLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Renal_Hydronephrosis:
							sss = m_mapDegree[spFinding->GetDegree()] + "," + m_mapKidneyLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Vascular_AAA:
							sss = m_mapVascularLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Vascular_ArteralCalcification:
							sss = m_mapDegree[spFinding->GetDegree()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Spleen_Mass:
						case ReportLib::Spleen_Granuloma:
							sss = m_mapSplenicLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::GI_InguinalHernia:
							sss = m_mapAdrenalLocation[spFinding->GetLocation()] + "," + m_mapOtherFindings[jj];
							break;

						case ReportLib::Liver_Gallstones:
						case ReportLib::Liver_FattyLiver:
						case ReportLib::GI_HiatalHernia:
						case ReportLib::Gynecology_UterineFibroids:
							sss = m_mapOtherFindings[jj];
							break;

						case ReportLib::Misc_Other:
							sss = "Miscellaneous other";
							break;

						default:
							;
						}

						pElement = AppendAttributeChild(pXmlDoc, pElinner, "ExtraColonicFinding", "string", variant_t(sss.c_str()));

					} // 
				} // if extra colonic finding info available

				pElinner = AppendAttributeChild(pXmlDoc, pElnest, "Snapshots", "version", variant_t(1.0));

				// get number of images in the finding
				int4 iNumImages = spEntry->Images->Count;

				for (int4 iImage(0); iImage < iNumImages; ++iImage)
				{
					sss = "finding_" + ToAscii(i) + "_" + ToAscii(iImage) + ".png";
				  pElement = AppendAttributeChild(pXmlDoc, pElinner, "Filename", "string", variant_t(sss.c_str()));
				} /// for iImage

			} // for i
		} // if there exist at least one entry


		/// save XML file to the file system ----------------------------------------------------
		if (FAILED(pXmlDoc->save(VxVCReport::m_sXMLFilename.c_str())))
		  throw ex::IOException(LogRec("Fail to save XML report", "VxReportCreator", "WriteXMLReport"));

		CoUninitialize();
	}
	catch (...)
	{
		throw ex::IOException(LogRec("Error occured while writing XML report file.", "VxReportCreator", "WriteXMLReport"));
	}
} // WriteXMLReport()


/**
* Appends an attribute child node to a given parent node
*
* @param pXmlDoc          The xml document
* @param pParentElement   parent node
* @param sElement         name of child node
* @param sAttrName        attribute for child
* @param value            attribute value
* @return                 newly-created child node
*/
MSXML2::IXMLDOMElementPtr VxReportCreator::AppendAttributeChild(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, MSXML2::IXMLDOMNodePtr pParentElement, 
	const std::string& sElement, const std::string& sAttrName, const variant_t& value)
{
	MSXML2::IXMLDOMElementPtr pElement = pXmlDoc->createElement(sElement.c_str());

	MSXML2::IXMLDOMAttributePtr pAttr = pXmlDoc->createAttribute(sAttrName.c_str());
	pAttr->value = value;
	pElement->setAttributeNode(pAttr);

	pParentElement->appendChild(pElement);
	return pElement;
} // AppendAttributeChild()


/**
*  Read the xml report file and create the report revision
*/
void VxReportCreator::ReadXMLReport()
{
	try
	{
		CoInitialize(NULL);

		vxDOMDocument::Initialize();

		MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
		HRESULT hRes = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pXmlDoc->preserveWhiteSpace = VARIANT_FALSE;

		if (pXmlDoc->load(VxVCReport::m_sXMLFilename.c_str()) != VARIANT_TRUE)
			throw ex::IOException(LogRec("Fail to read XML report", "VxReportCreator", "ReadXMLReport"));

		MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;

		std::string ss;
		variant_t vValue;

		/// report infomation block ---------------------------------------------------------
		ReportLib::IReportInfoPtr spInfo = m_pReportRevision->Info;

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientName");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientName = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientID");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientID = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientAge");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientAge = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientBirthday");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientBirthday = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientSex");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientSex = ReportLib::PatientSexEnum(int4(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("enum")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/PatientWeight");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->PatientWeight = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/StudyDate");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->StudyDate = atof(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("double")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/ReferringPhysician");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->ReferringDoctor = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/ReportAuthor");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->Author = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/InstitutionName");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->InstitutionName = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/Indication");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->Indication = atol(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("long")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/Impression");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		{
			ss = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());
			uint4 uFound = ss.find("Examination Parameters:", 0);
			ss = ss.substr(0, uFound);
			spInfo->Impression = ss.c_str();
		} // if

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/OtherIndication");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->OtherIndication = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/OtherRecommendation");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->OtherRecommendation = _bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("string")->GetnodeValue());

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/CRADSCategoryColon");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->CRADSCategory = atol(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("long")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/Recommendation");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->Recommendation = atol(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("long")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/RecommendationTime");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->RecommendationTime = atol(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("long")->GetnodeValue()));

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/RecommendationType");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		{
			vValue = pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("enum")->GetnodeValue();
			spInfo->RecommendationType = ReportLib::RecommendationTypeEnum(atoi(_bstr_t(vValue)));
		} // if

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/RecommendationUnit");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		{
			vValue = pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("enum")->GetnodeValue();
			spInfo->RecommendationUnit = ReportLib::RecommendationUnitEnum(atoi(_bstr_t(vValue)));
		} // if

		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Info/ExamQuality");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		  spInfo->ExamQuality = atol(_bstr_t(pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("long")->GetnodeValue()));

	  /// read all report entries -----------------------------------------------------------
		pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//V3DColonReport/Entries/Entry");
		if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
		{			
			/// number of report entry
			const int4 iEntryCount = pIDOMNodeList->Getlength();

			if (iEntryCount > 0)
			{
				ReportLib::IReportEntriesPtr spEntries = m_pReportRevision->Entries;

				/// read over all report entry
				for (int4 iEntryIndex = 0; iEntryIndex < iEntryCount; iEntryIndex++)
				{
					ReportLib::IReportEntryPtr spEntry = spEntries->Add();

					/// get the current Entry item 
					MSXML2::IXMLDOMNodeListPtr pEntryList = pIDOMNodeList->Getitem(iEntryIndex)->GetchildNodes();

					const int4 iItemCount = pEntryList->Getlength();
					if (pEntryList != NULL && iItemCount > 0 && iItemCount < 4)
					{
						MSXML2::IXMLDOMNodeListPtr pItemList;

						for (int4 iItem = 0; iItem<iItemCount; iItem++)
						{
							/// get Colonic element
							pItemList = pEntryList->Getitem(iItem)->GetchildNodes();
							if (pItemList != NULL && pItemList->Getlength() > 0 &&
								pEntryList->Getitem(iItem)->GetnodeName() == _bstr_t("Colonic"))
							{ /// Colonic element
								const int4 iN = pItemList->Getlength();

								for (int4 i = 0; i<iN; i++)
								{
									if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Name"))
									{
										spEntry->Name = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("string"))->GetnodeValue());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Comment"))
									{
										spEntry->Comments = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("string"))->GetnodeValue());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Maximum"))
									{
										ss = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue());
										spEntry->Maxiumum = atof(ss.c_str());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Perpendicular"))
									{
										ss = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue());
										spEntry->Perpendicular = atof(ss.c_str());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("OrigDistanceToRectum"))
									{
										ss = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue());
										spEntry->OrigDistanceToRectum = atof(ss.c_str());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("CameraLocationString"))
									{
										ss = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("string"))->GetnodeValue());
										Point<float4> pos(0, 0, 0); Normal3D<float4> orientation(0, 0, 0);
										uint4 us = ss.find("pos=\"", 0) + 5;
										uint4 ue = ss.find(",", 0);
										std::string sx = ss.substr(us, ue - us);
										pos.m_x = atof(sx.c_str());
										us = ue + 1;
										ue = ss.find(",", us);
										sx = ss.substr(us, ue - us);
										pos.m_y = atof(sx.c_str());
										us = ue + 1;
										ue = ss.find("\"", us);
										sx = ss.substr(us, ue - us);
										pos.m_z = atof(sx.c_str());
										spEntry->CameraCoordinates = ColonReport::ReportVariant(pos, orientation, orientation);
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("FindingType"))
									{
										spEntry->ColonFinding = ReportLib::ColonFindingEnum(int4(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("enum"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Location"))
									{
										spEntry->Location = ReportLib::LocationEnum(int4(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("enum"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Shape"))
									{
										spEntry->Shape = ReportLib::ShapeEnum(int4(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("enum"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Translucency"))
									{
										spEntry->Translucency = ReportLib::TranslucencyEnum(int4(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("enum"))->GetnodeValue()));
										continue;
									}
								} // for each internal node
							} // if

							/// get ExtraColonic element
							else if (pItemList != NULL && pItemList->Getlength() > 0 &&
								pEntryList->Getitem(iItem)->GetnodeName() == _bstr_t("ExtraColonic"))
							{ /// ExtraColonic element
								const int4 iN = pItemList->Getlength();

								ReportLib::IReportOtherFindingsPtr spOthers = m_pReportRevision->GetOtherFindings();
								ReportLib::IReportOtherFindingPtr spFinding = spOthers->Add();
								/// refresh the other finding ID
								spEntry->OtherFindingID = spFinding->ID;

								for (int4 i = 0; i < iN; i++)
								{
									if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Comment"))
									{
										spFinding->Comment = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("string"))->GetnodeValue());
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Degree"))
									{
										spFinding->Degree = atol(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("long"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Size"))
									{
										spFinding->Size = atol(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("long"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Length"))
									{
										spFinding->Length = atof(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("PerpendicularSize"))
									{
										spFinding->PerpendicularSize = atof(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Maximum"))
									{
										spFinding->MaximumSize = atof(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("float"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("CRADSCategoryExtraColonic"))
									{
										spFinding->CRADSCategory = atol(_bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("long"))->GetnodeValue()));
										continue;
									}
									else if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("ExtraColonicFindingType"))
									{
										spFinding->Finding = ReportLib::OtherFindingsEnum(int4(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("enum"))->GetnodeValue()));
										continue;
									}
								} // for
							}

							/// get Snapshot element
							else if (pItemList != NULL && pItemList->Getlength() > 0 &&
								pEntryList->Getitem(iItem)->GetnodeName() == _bstr_t("Snapshots"))
							{ /// Snapshot element
								const int4 iN = pItemList->Getlength();

								const vx::File    studyFile(VxVCReport::m_sXMLFilename);
								const std::string sParent(studyFile.GetParent());
								if (!sParent.empty())
									_chdir(sParent.c_str());

								for (int4 i = 0; i < iN; i++)
								{
									if (pItemList->Getitem(i)->GetnodeName() == _bstr_t("Filename"))
									{
										ss = _bstr_t(pItemList->Getitem(i)->Getattributes()->getNamedItem(_bstr_t("string"))->GetnodeValue());
										ss = sParent + "\\Snapshots\\" + ss;

										Gdiplus::Bitmap * pImg = Gdiplus::Bitmap::FromFile((wchar_t*)widestring(ss).c_str(), FALSE);

										const int4 iWidth = pImg->GetWidth();
										const int4 iHeight = pImg->GetHeight();

										/// The bitmap data has to be the multiple of sizeof(DWORD) for an image row
										int4 iBytesPerLine = (((iWidth * 24) + 31) / 32 * 32) / 8;
										DWORD dwElements(iBytesPerLine*iHeight);
										int4 iWidth32((((iWidth * 24) % 32) == 0) ? (iWidth * 3) : iBytesPerLine);

										BYTE *pBuffer = new BYTE[dwElements];
										if (pBuffer == NULL)
											throw ex::OutOfMemoryException(LogRec("Out of memory for bitmap", "VxReportCreator", "ReadXMLReport"));

										register Gdiplus::Color gColor;
										register int4 x, y, m = 0, n = 0;

										for (y = iHeight - 1; y >= 0; y--)
										{
											for (x = 0; x < iWidth; x++)
											{
												pImg->GetPixel(x, y, &gColor);
												pBuffer[m] = static_cast<BYTE>(gColor.GetBlue());  m++;
												pBuffer[m] = static_cast<BYTE>(gColor.GetGreen()); m++;
												pBuffer[m] = static_cast<BYTE>(gColor.GetRed());   m++;
											} // for x

											n = n + iWidth32;
											m = n;
										} // for y

										COleSafeArray sa01;
										sa01.CreateOneDim(VT_UI1, dwElements, pBuffer);

										ReportLib::IImagePtr spImage01(__uuidof(ReportLib::Image));
										spImage01->Create(iWidth, iHeight, 24);
										spImage01->SetPixels(sa01);

										spEntry->Images->Add(spImage01);

										delete pBuffer;
										pBuffer = NULL;
										pImg = NULL;
									} // if
								} // for
							} // if
						} // for each item in the Entry
					} // node is not empty
				} // for iEntryIndex
			} // if there is an entry
		} // if ENtries element exists

		CoUninitialize();
	}
	catch (...)
	{
		throw ex::IOException(LogRec("Error occured while writing XML report file.", "VxReportCreator", "WriteXMLReport"));
	}
} // ReadXMLReport()

#undef FILE_REVISION

// $Log: VxReportCreator.C,v $
// Revision 1.16.2.32  2010/02/05 12:41:42  dongqing
// moved globals into class as private static variables
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.31  2010/02/05 09:49:00  dongqing
// log text
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.30  2010/02/05 01:38:02  dongqing
// remove c-rads sample output
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.29  2010/01/27 16:09:05  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.28  2009/09/15 17:55:15  dongqing
// Fix the CRADs catagory output to PDF error.
//
// Revision 1.16.2.27  2009/06/16 13:50:32  dongqing
// fix the report text "atuncertain" to correct "at uncertain"
//
// Revision 1.16.2.26  2008/07/30 16:40:01  jmeade
// confusion between personal history of cancer/colon-cancer
//
// Revision 1.16.2.25  2008/04/19 01:15:54  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.24  2008/04/17 16:08:49  jmeade
// virtual ==> 3d ct
//
// Revision 1.16.2.23  2008/04/15 02:17:33  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.22  2008/04/08 23:16:04  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.21  2008/04/07 22:53:33  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.20  2008/04/04 19:35:19  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.19  2008/04/04 00:22:32  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.18  2008/04/03 17:18:28  jmeade
// no heading for other indications, as requested by a user.
//
// Revision 1.16.2.17  2008/04/02 21:17:13  jmeade
// Issue 6017: aligning console and vc printed reports.
//
// Revision 1.16.2.16  2008/03/29 00:45:19  jmeade
// Issue 6051: only print perpendicular measurement when necessary.
//
// Revision 1.16.2.15  2008/03/25 21:21:48  jmeade
// Issue 6028: ensure c-rads value printed in header.
//
// Revision 1.16.2.14  2008/03/20 18:30:42  jmeade
// moving console and vc report closer in line.
//
// Revision 1.16.2.13  2008/03/15 01:12:36  jmeade
// moving console and vc report closer in line.
//
// Revision 1.16.2.12  2008/03/12 21:03:52  jmeade
// tabs to spaces.
//
// Revision 1.16.2.11  2008/03/12 20:37:19  jmeade
// Issue 6028: Don't store overall extra-colonic c-rads category; that is begging for an out-of-date value.  Compute when needed.
//
// Revision 1.16.2.10  2008/03/12 18:12:33  jmeade
// Issue 6028: display colon c-rads in header.
//
// Revision 1.16.2.9  2008/02/14 10:06:10  jmeade
// Issue 5924: tightening loop on shouldn't-happen-anyway errors.
//
// Revision 1.16.2.8  2008/02/14 09:48:33  jmeade
// Issue 5924: print other findings info with extra-colonic findings.
//
// Revision 1.16.2.7  2008/02/14 07:01:48  jmeade
// Issue 5924: identify xc findings properly.
//
// Revision 1.16.2.6  2007/10/30 21:20:08  jmeade
// since we won't always have access to the voxel spacing (via the dataset), just remove the disclaimer and put it in the report.
//
// Revision 1.16.2.5  2007/10/26 00:23:22  jmeade
// Issue 5845: uncertainty measurement.
//
// Revision 1.16.2.4  2007/06/05 21:12:43  jmeade
// language independent output of c-rads category
//
// Revision 1.16.2.3  2007/04/27 22:02:04  jmeade
// removing repetition from code ouputting c-rads data.
//
// Revision 1.16.2.2  2007/04/27 02:19:34  jmeade
// Issue 5608: one overall extra-colonic c-rads category in the miscellaneous view.
//
// Revision 1.16.2.1  2007/04/03 20:43:36  jmeade
// extra checks on location property.
//
// Revision 1.16  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.15  2007/02/21 02:03:16  jmeade
// formatting of c-rads text.
//
// Revision 1.14  2007/02/15 20:26:03  jmeade
// Issue 5219: text for Normal findings.
//
// Revision 1.13  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.12  2007/02/09 05:18:43  jmeade
// Issue 5219: Implementing C-RADS: optional (near) flexure field.
//
// Revision 1.11  2007/02/09 03:59:58  jmeade
// Issue 5219: Implementing C-RADS categories.
//
// Revision 1.10  2007/02/09 00:17:43  jmeade
// Issue 5219: Implementing C-RADS: failed colonoscopy indication.
//
// Revision 1.9  2006/10/18 23:50:42  jmeade
// Issue 4921: Removed unnecessary and risky code/check that used 1st pixel to identify overview image.
//
// Revision 1.8  2006/09/28 22:49:03  romy
// removed the hardcoded v1k path for report
//
// Revision 1.7  2006/08/01 18:15:57  geconomos
// implementation of "report disclaimer"
//
// Revision 1.6  2006/06/01 20:23:33  frank
// updated to visual studio 2005
//
// Revision 1.5  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4  2005/11/21 23:16:49  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.3.2.5  2006/08/01 18:18:03  geconomos
// implementation of "report disclaimer"
//
// Revision 1.3.2.4  2006/03/17 22:52:36  jmeade
// format string changes for language-dependent text.
//
// Revision 1.3.2.3  2006/03/16 01:21:24  jmeade
// format string changes for language-dependent text.
//
// Revision 1.3.2.2  2006/03/13 16:09:41  jmeade
// Ensure non-null comment block prior to accessing.
//
// Revision 1.3.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.3  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.2.2.1  2005/09/21 13:28:26  geconomos
// changed to use variant_t instead of COleVariant in WriteImage
//
// Revision 1.2  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.3.2.12.8.1  2005/06/29 00:08:09  jmeade
// Issue 4180: Skip overview image for extra-colonic findings.
//
// Revision 1.3.2.12  2005/02/15 18:04:25  jmeade
// merge from vc1.4 branch.
//
// Revision 1.3.2.11.2.1  2005/01/11 18:31:07  jmeade
// check for zero-length bstrings before accessing.
//
// Revision 1.3.2.11  2004/07/26 18:02:48  jmeade
// string resources.
//
// Revision 1.3.2.10  2004/07/22 17:13:19  jmeade
// Issue 3630: Print message indicating which distance-to-rectum values may be inconsistent with current segment selection.
//
// Revision 1.3.2.9  2004/07/21 20:28:06  jmeade
// Report lib enum.
//
// Revision 1.3.2.8  2004/06/17 18:40:04  vxconfig
// enum error.
//
// Revision 1.3.2.7  2004/05/14 17:23:23  jmeade
// Undefined report lib enum.
//
// Revision 1.3.2.6  2004/05/06 18:30:36  mkontak
// Fixed issue with older anonymized CD's
//
// Revision 1.3.2.5  2004/03/27 00:43:59  jmeade
// Issue 3590: New polyp shape classifications; replaced for location radios with ReportGroupCtrl.
//
// Revision 1.3.2.4  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.3.2.3  2004/02/03 19:01:37  mkontak
// Missing #undef FILE_REVISION
//
// Revision 1.3.2.2  2003/06/06 22:55:11  jmeade
// code review prep.
//
// Revision 1.3.2.1  2003/03/07 04:09:37  jmeade
// Issue 2886: Correct syntax in code for loading format strings.
//
// Revision 1.3  2003/01/21 19:43:29  jmeade
// Duplicate from VC_1-2_CS_1 branch: Text to resource.
//
// Revision 1.2  2002/07/26 22:56:20  jmeade
// Load resource string function will fail before application resource map is initialized.
//
// Revision 1.1  2002/07/18 19:58:41  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.28  2002/06/25 23:06:12  jmeade
// Moving more hard-coded strings to the string table.
//
// Revision 3.27  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.26  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.25  2002/04/18 18:31:30  jmeade
// Moved/renamed ViatronSettings class to v3DSystemSettings.
//
// Revision 3.24  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.23  2002/02/04 20:18:14  binli
// Improved image quality in report file
//
// Revision 3.22.2.6  2002/05/21 01:33:52  jmeade
// Issue 2265:  Implemented free-form text block for report header text.
//
// Revision 3.22.2.5  2002/04/11 20:35:47  jmeade
// Issue 2040:  Misspellings; Code standards.
//
// Revision 3.22.2.4  2002/03/28 22:35:57  jmeade
// Correcting spellng errors in report.
//
// Revision 3.22.2.3  2002/02/26 19:08:04  binli
// added new item "family history of polyps" to the Miscellancenous::Indications.
//
// Revision 3.22.2.2  2002/02/20 22:27:13  binli
// issue 1902: increased bottom margin by 0.5cm(36Pts) in case of our of range.
//
// Revision 3.22.2.1  2002/02/11 16:37:19  binli
// Improved image quality in report file
//
// Revision 3.22  2002/01/30 22:50:32  jmeade
// Log PDF initialization error.
//
// Revision 3.21  2002/01/28 19:11:52  binli
// image header of report file: if image size is too bigger, scaled.
//
// Revision 3.20  2002/01/17 18:55:38  binli
// 1> adjustment of captions
// 2> hardcoding of some const strings.
//
// Revision 3.19  2002/01/17 02:23:16  jmeade
// Log records and TODO comments.
//
// Revision 3.18  2002/01/15 21:02:06  binli
// tiny correction: no show of Supine/Prone when finding is extra (as required by Linda)
//
// Revision 3.17  2002/01/15 20:49:14  binli
// issue 1424: option to show/not show distance in report
//
// Revision 3.16  2002/01/09 22:06:45  binli
// Report adjustment: separate display of Non-Extra colon findings & Extra colon findings.
//
// Revision 3.15  2002/01/07 21:43:38  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.14  2002/01/04 20:26:27  binli
// little adjustment of report output.
//
// Revision 3.13  2002/01/04 18:33:30  binli
// Report adjustment: support of multi-line property in all EditBox
//
// Revision 3.12  2002/01/03 18:57:56  binli
// Report adjustment: export of extra-colonic
//
// Revision 3.11  2002/01/02 22:26:48  binli
// Report adjustment: added 'Extra-Colonic'
//
// Revision 3.10  2002/01/02 15:32:58  binli
// Adjustment: Added Other_Location/Clear_Location
//
// Revision 3.9  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.8  2001/12/26 20:51:42  binli
// adjustment of Report.
//
// Revision 3.7  2001/12/11 18:02:44  binli
// if there is a error in one section of .vrx file, no display of it in .PDF.
//
// Revision 3.6  2001/12/10 19:23:42  binli
// removed the duplicated 'WriteRecommendations()'
//
// Revision 3.5  2001/12/04 16:23:28  jmeade
// Moved report preferences enums to ViatronSettings class.
//
// Revision 3.4  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.3.2.5  2001/12/03 21:31:37  jmeade
// StudyType changed to DatasetPositionEnumProp to eliminate current and future conflicts
//
// Revision 3.3.2.4  2001/11/05 22:08:47  jmeade
// Coding standards.
//
// Revision 3.3.2.3  2001/10/30 17:11:43  binli
// issue 1236: small erro.
//
// Revision 3.3.2.2  2001/10/28 20:54:25  jmeade
// Remainder of settings using the ViatronSettings::Setting class; Coding standards
//
// Revision 3.3.2.1  2001/10/25 15:43:52  binli
// issue 1236: temporay error while busy. wait a little and try should be fine.
// (a> removed the temporay file  b> efficient)
//
// Revision 3.3  2001/10/24 00:20:10  jmeade
// String corrections
//
// Revision 3.2  2001/10/18 16:42:36  jmeade
// Coding standards (removing unused resource items)
//
// Revision 3.1  2001/10/17 16:01:35  binli
// issue 1187: always show PatientInfo on the bottom of the report file.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 09 2001 09:55:44   binli
// considered the width of output text using WriteLines()
// 
//    Rev 2.1   Oct 05 2001 17:19:58   binli
// report PDF file: adjust the distance between images and titles (NonDistendedAreas)
// 
//    Rev 2.0   Sep 16 2001 23:41:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:32   ingmar
// Initial revision.
// Revision 1.34  2001/09/10 22:39:29  binli
// track ID: 000672: added (Supine)/(Prone) to caption in Colon-Findings and NonDistendedAreas.
//
// Revision 1.33  2001/09/10 17:14:25  binli
// track ID: 000636: changed the section order: 'impression' section before 'Recommedation'
//
// Revision 1.32  2001/09/04 18:02:20  binli
// task 173 (track ID 000485): added new option: report inderterminate translucent feature or not.
//
// Revision 1.31  2001/08/24 18:19:44  binli
// 590 (track ID: 670/635): page number/patient name.Id,scan date on report pages.
//
// Revision 1.30  2001/08/22 17:15:58  binli
// bug pt1000: (better) write otherindications when it is not blank (WriteHeader())
//
// Revision 1.29  2001/08/22 16:55:02  binli
// bug pt1000: write otherindications when it is not blank (WriteHeader())
//
// Revision 1.28  2001/08/16 14:23:41  binli
// bug 000486: added 'm_tKidneyLocation', and used it for proper text output in Preview
//
// Revision 1.27  2001/06/07 19:03:58  geconomos
// Code walkthru preparation
//
// Revision 1.26  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.25  2001/05/09 14:14:41  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.24  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.23  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.22  2001/03/07 20:04:11  geconomos
// Changed OtherColonFinding to Normal
//
// Revision 1.21  2001/02/27 20:58:37  geconomos
// Added report header stuff
//
// Revision 1.20  2001/02/26 22:00:08  geconomos
// Added Other to ColonFinding enum
//
// Revision 1.19  2001/02/16 16:59:10  geconomos
// Fxied bug with displaying other recommendations string
//
// Revision 1.18  2001/02/07 21:22:25  geconomos
// Fixed bug in non distended finding.
//
// Revision 1.17  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.16  2001/02/05 18:34:55  geconomos
// Ensured that that c_str()  is being called from all maps.
//
// Revision 1.15  2001/02/05 16:17:20  jeff
// strings to resource table
//
// Revision 1.14  2001/02/02 21:10:37  jeff
// use std::string instead of LPCTSTR so that strings won't lose scope
//
// Revision 1.13  2001/02/02 08:09:24  jeff
// Moving strings to resource table
//
// Revision 1.12  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxReportCreator.C,v 1.16.2.32 2010/02/05 12:41:42 dongqing Exp $
// $Id: VxReportCreator.C,v 1.16.2.32 2010/02/05 12:41:42 dongqing Exp $
