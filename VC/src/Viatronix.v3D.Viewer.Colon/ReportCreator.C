// $Id: ReportCreator.C,v 1.2.2.1 2009/06/16 13:50:32 dongqing Exp $
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
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportCreator.h"
#include "ReportViewWnd.h"
#include "resource.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.2.2.1 $"

/// 1 inch
const float4 FACTOR  = 72.0f;

/// page width in inches
const float4 PAGE_WIDTH = 8.5f * FACTOR;

/// page height in inches
const float4 PAGE_HEIGHT = 11.0f * FACTOR;

/// right margin x position
const float4 RIGHT_MARGIN = PAGE_WIDTH - (1.0f * FACTOR);

/// left margin x position
const float4 LEFT_MARGIN = 1.0f * FACTOR;

/// maximum width of line
const float4 LINE_MAX = PAGE_WIDTH - 100.0f;

/// x offset text within a box
const float4 BOXED_TEXT_OFFSETX = 5.0f;

/// image width in inches
const float4 IMAGE_WIDTH = 2.0f * FACTOR;

/// image height in inches
const float4 IMAGE_HEIGHT = 2.0f * FACTOR;

/// spacing bewteen images in inches
const float4 IMAGE_SPACE = 0.25f * FACTOR;

/// PDF writer license key
const BSTR PDFLIB_KEY = L"0030-1300-2578-5088";

/// Other finding output format.
const CString sOtherFindingsEntryFormat = " - %s%s %s %s";
const CString sLungPneomuthoraxFormat = " - %s %s %s %s";
const CString sRenalScarFormat = " - %s %s %s";
const CString sRenalHydronephrosisFormat = " - %s %s %s %s";
const CString sOtherFindingsVascularAAAEntryFormat = " - %s %s %s %s%s";
const CString sVascularArteralCalcificationFormat = " - %s %s %s";
const CString sGIInguinalHerniaFormat = " - %s %s %s";
const CString sGynecologyUterineFibroidsFormat = " - %s %s"; 

/// output format.
CString sOtherExtraFormat;
CString sUncertainDistanceFormat;
CString sCommentsFormat;
const std::string sShowBoxedLineFormat_Center = "center";

/**
 * constructor
 */
ReportCreator::ReportCreator() : 
m_fCurrentY(0.0f),
m_iTotalPages(-1),
m_iNumPages(1),
m_fTopMargin(0.0f),
m_fBottomMargin(0.0f),
m_eNumbering(V3DSystemSettings::eNUMBERING_NONE),
m_eHeader(V3DSystemSettings::eHEADER_NONE),
m_iHeaderImage(-1)
{
  float4 fTopMargin(0.0);   V3DSystemSettings::m_reportTopMargin.Get(fTopMargin);
  m_fTopMargin = PAGE_HEIGHT - (fTopMargin * FACTOR);
  float4 fBottomMargin(0.0);   V3DSystemSettings::m_reportBottomMargin.Get(fBottomMargin);
  m_fBottomMargin = fBottomMargin * FACTOR + 36;

  int4 iNumbering(0);   V3DSystemSettings::m_reportNumbering.Get(iNumbering);
  m_eNumbering = V3DSystemSettings::ReportNumberingEnum(iNumbering);
  
  int4 iHeaderSel(0);   V3DSystemSettings::m_reportHeaderSelection.Get(iHeaderSel);
  m_eHeader = V3DSystemSettings::ReportHeaderEnum(iHeaderSel);

  if (m_eHeader == V3DSystemSettings::eHEADER_USER)
  {
    std::string sHeaderText;
    V3DSystemSettings::m_reportHeaderText.Get(sHeaderText);
    m_sHeaderTextList.clear();

    if (!sHeaderText.empty())
    {
      static const int1 sCRLF[] = { 13,10,0 };
      int4 iStartPos(0), iEndPos(0);
      do
      {
        iEndPos = sHeaderText.find(sCRLF, iStartPos);
        m_sHeaderTextList.push_back(sHeaderText.substr(iStartPos, (iEndPos!=std::string::npos) ? iEndPos-iStartPos : iEndPos));
        iStartPos = iEndPos + strlen(sCRLF);
      } while (iEndPos != std::string::npos);
    }
  }

  m_tColonSegment[ReportLib::Cecum]          = LoadResourceString(IDS_CECUM);
  m_tColonSegment[ReportLib::Ascending]      = LoadResourceString(IDS_ASCENDING_COLON);
  m_tColonSegment[ReportLib::HepaticFlexure] = LoadResourceString(IDS_HEPATIC_FLEXURE);
  m_tColonSegment[ReportLib::Transverse]     = LoadResourceString(IDS_TRANSVERSE_COLON);
  m_tColonSegment[ReportLib::SplenicFlexure] = LoadResourceString(IDS_SPLENIC_FLEXURE);
  m_tColonSegment[ReportLib::Descending]     = LoadResourceString(IDS_DESCENDING_COLON);
  m_tColonSegment[ReportLib::Sigmoid]        = LoadResourceString(IDS_SIGMOID);
  m_tColonSegment[ReportLib::Rectum]         = LoadResourceString(IDS_RECTUM);

  m_tPolypShape[ReportLib::Round] = LoadResourceString(IDS_ROUND);
  m_tPolypShape[ReportLib::Oval]  = LoadResourceString(IDS_OVAL);
  m_tPolypShape[ReportLib::Flat]  = LoadResourceString(IDS_FLAT);
  
	CString densityChange;
	densityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_GRADUAL));
	m_tTranslucency[ReportLib::GradualDensityChange] = densityChange;
	densityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_ABRUPT));
  m_tTranslucency[ReportLib::AbruptDensityChange]  = densityChange;
	densityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_INTERMEDIATE));
  m_tTranslucency[ReportLib::IntermediateDensityChange] = densityChange;
  m_tTranslucency[ReportLib::SimilarToColonWall]   = LoadResourceString(IDS_SIMILAR_TO_COLON_WALL);
  m_tTranslucency[ReportLib::Indeterminate + 5]    = LoadResourceString(IDS_INDETERMINATE);
  
  m_tIndications[ReportLib::Screening] = LoadResourceString(IDS_SCREENING);
  m_tIndications[ReportLib::PersonalHistoryOfPolyps] = LoadResourceString(IDS_PERSONAL_HISTORY_OF_POLYPS);
  m_tIndications[ReportLib::Constipation] = LoadResourceString(IDS_CONSTIPATION);
  m_tIndications[ReportLib::PersonalHistoryOfCancer] = LoadResourceString(IDS_PERSONAL_HISTORY_OF_CANCER);
  m_tIndications[ReportLib::FamilyHistoryOfCancer] = LoadResourceString(IDS_FAMILY_HISTORY_OF_CANCER);
  m_tIndications[ReportLib::Anemia] = LoadResourceString(IDS_ANEMIA);
  m_tIndications[ReportLib::Hematochezia] = LoadResourceString(IDS_HEMATOCHEZIA);
  m_tIndications[ReportLib::AbnormalImaging] = LoadResourceString(IDS_ABNORMAL_IMAGING);
  m_tIndications[ReportLib::AbdominalPain] = LoadResourceString(IDS_ABDOMINAL_PAIN);
  m_tIndications[ReportLib::IBD] = LoadResourceString(IDS_IBD);
  m_tIndications[ReportLib::Diarrhea] = LoadResourceString(IDS_DIARRHEA);
  m_tIndications[ReportLib::PolypOnFlexSigmoid] = LoadResourceString(IDS_POLYP_ON_FLEX_SIGMOID);

  m_tIndications[ReportLib::PolypOnFlexSigmoid+ReportLib::PolypOnFlexSigmoid] = LoadResourceString(IDS_FAMILY_HISTORY_OF_POLYPS);

  m_tOtherFindings[ReportLib::Lung_Nodule]                   = LoadResourceString(IDS_LUNG_NODULE);
  m_tOtherFindings[ReportLib::Lung_Granuloma]                = LoadResourceString(IDS_LUNG_GRANULOMA);
  m_tOtherFindings[ReportLib::Lung_Pneomuthorax]             = LoadResourceString(IDS_PNEUMOTHORAX);
  m_tOtherFindings[ReportLib::Liver_Mass]                    = LoadResourceString(IDS_LIVER_MASS);
  m_tOtherFindings[ReportLib::Liver_Cyst]                    = LoadResourceString(IDS_LIVER_CYST);
  m_tOtherFindings[ReportLib::Liver_Gallstones]              = LoadResourceString(IDS_GALLSTONES);
  m_tOtherFindings[ReportLib::Liver_FattyLiver]              = LoadResourceString(IDS_FATTY_LIVER);
  m_tOtherFindings[ReportLib::Adrenal_Mass]                  = LoadResourceString(IDS_ADRENAL_MASS);
  m_tOtherFindings[ReportLib::Renal_Mass]                    = LoadResourceString(IDS_RENAL_MASS);
  m_tOtherFindings[ReportLib::Renal_Cyst]                    = LoadResourceString(IDS_RENAL_CYST);
  m_tOtherFindings[ReportLib::Renal_Stones]                  = LoadResourceString(IDS_RENAL_STONES);
  m_tOtherFindings[ReportLib::Renal_Scar]                    = LoadResourceString(IDS_RENAL_SCAR);
  m_tOtherFindings[ReportLib::Renal_Hydronephrosis]          = LoadResourceString(IDS_HYDRONEPHROSIS);
  m_tOtherFindings[ReportLib::Vascular_AAA]                  = LoadResourceString(IDS_AAA);
  m_tOtherFindings[ReportLib::Vascular_ArteralCalcification] = LoadResourceString(IDS_ARTERAL_CALCIFICATION);
  m_tOtherFindings[ReportLib::Spleen_Mass]                   = LoadResourceString(IDS_SPLENIC_MASS);
  m_tOtherFindings[ReportLib::Spleen_Granuloma]              = LoadResourceString(IDS_SPLENIC_GRANULOMA);
  m_tOtherFindings[ReportLib::GI_InguinalHernia]             = LoadResourceString(IDS_INGUINAL_HERNIA);
  m_tOtherFindings[ReportLib::GI_HiatalHernia]               = LoadResourceString(IDS_HIATAL_HERNIA);
  m_tOtherFindings[ReportLib::Gynecology_UterineFibroids]    = LoadResourceString(IDS_UTERINE_FIBROIDS);

  m_tLungLocation[ReportLib::RightLung_UpperLobe]  = LoadResourceString(IDS_RIGHT_UPPER_LOBE);
  m_tLungLocation[ReportLib::RightLung_MiddleLobe] = LoadResourceString(IDS_RIGHT_MIDDLE_LOBE);
  m_tLungLocation[ReportLib::RightLung_LowerLobe]  = LoadResourceString(IDS_RIGHT_LOWER_LOBE);
  m_tLungLocation[ReportLib::LeftLung_UpperLobe]   = LoadResourceString(IDS_LEFT_UPPER_LOBE);
  m_tLungLocation[ReportLib::LeftLung_MiddleLobe]  = LoadResourceString(IDS_LEFT_MIDDLE_LOBE);
  m_tLungLocation[ReportLib::LeftLung_LowerLobe]   = LoadResourceString(IDS_LEFT_LOWER_LOBE);

  m_tPneumothoraxLocation[ReportLib::RightLung] = LoadResourceString(IDS_RIGHT_LUNG);
  m_tPneumothoraxLocation[ReportLib::LeftLung]  = LoadResourceString(IDS_LEFT_LUNG);

  m_tPneumothoraxSize[ReportLib::Small]  = LoadResourceString(IDS_SMALL);
  m_tPneumothoraxSize[ReportLib::Medium] = LoadResourceString(IDS_MEDIUM);
  m_tPneumothoraxSize[ReportLib::Large]  = LoadResourceString(IDS_LARGE);
  
  m_tLiverLocation[ReportLib::RightLobe_AnteriorSegment]  = LoadResourceString(IDS_RIGHT_LOBE_ANTERIOR_SEGMENT);
  m_tLiverLocation[ReportLib::RightLobe_PosteriorSegment] = LoadResourceString(IDS_RIGHT_LOBE_POSTERIOR_SEGMENT);
  m_tLiverLocation[ReportLib::LeftLobe_MedialSegment]     = LoadResourceString(IDS_LEFT_LOBE_MEDIAL_SEGMENT);
  m_tLiverLocation[ReportLib::LeftLobe_LateralSegment]    = LoadResourceString(IDS_LEFT_LOBE_LATERAL_SEGMENT);
  
  m_tAdrenalLocation[ReportLib::Right] = LoadResourceString(IDS_RIGHT);
  m_tAdrenalLocation[ReportLib::Left]  = LoadResourceString(IDS_LEFT);
  
  m_tRenalLocation[ReportLib::Right_UpperPole] = LoadResourceString(IDS_RIGHT_UPPER_POLE);
  m_tRenalLocation[ReportLib::Right_Middle]    = LoadResourceString(IDS_RIGHT_MIDDLE);
  m_tRenalLocation[ReportLib::Right_LowerPole] = LoadResourceString(IDS_RIGHT_LOWER_POLE);
  m_tRenalLocation[ReportLib::Left_UpperPole]  = LoadResourceString(IDS_LEFT_UPPER_POLE);
  m_tRenalLocation[ReportLib::Left_Middle]     = LoadResourceString(IDS_LEFT_MIDDLE);
  m_tRenalLocation[ReportLib::Left_LowerPole]  = LoadResourceString(IDS_LEFT_LOWER_POLE);
  
  m_tKidneyLocation[ReportLib::RightKidney] = LoadResourceString(IDS_RIGHT_KIDNEY);
  m_tKidneyLocation[ReportLib::LeftKidney]  = LoadResourceString(IDS_LEFT_KIDNEY);

  m_tDegree[ReportLib::Mild]     = LoadResourceString(IDS_MILD);
  m_tDegree[ReportLib::Moderate] = LoadResourceString(IDS_MODERATE);
  m_tDegree[ReportLib::Severe]   = LoadResourceString(IDS_SEVERE);

  m_tVascularLocation[ReportLib::Suprarenal] = LoadResourceString(IDS_SUPRARENAL);
  m_tVascularLocation[ReportLib::Juxtarenal] = LoadResourceString(IDS_JUXTARENAL);
  m_tVascularLocation[ReportLib::Infrarenal] = LoadResourceString(IDS_INFRARENAL);

  m_tSplenicLocation[ReportLib::Superior] = LoadResourceString(IDS_SUPERIOR);
  m_tSplenicLocation[ReportLib::Middle]   = LoadResourceString(IDS_MIDDLE);
  m_tSplenicLocation[ReportLib::Inferior] = LoadResourceString(IDS_INFERIOR);
  
  m_tPatientSex[ReportLib::Male]   = LoadResourceString(IDS_MALE);
  m_tPatientSex[ReportLib::Female] = LoadResourceString(IDS_FEMALE);

  m_tRecommendType[ReportLib::Optical] = LoadResourceString(IDS_OPTICAL);
  m_tRecommendType[ReportLib::Virtual] = LoadResourceString(IDS_VIRTUAL);

  m_tRecommendUnit[ReportLib::Months] = LoadResourceString(IDS_MONTH);
  m_tRecommendUnit[ReportLib::Years]  = LoadResourceString(IDS_YEAR);  

  // Initialize globals that use resource string table
  sOtherExtraFormat = CString(" - (") + LoadResourceString(IDS_OTHER) + ") ";
  sUncertainDistanceFormat = " " + LoadResourceString(IDS_DISTANCE_FROM_RECTUM_UNCERTAIN);
  sCommentsFormat = LoadResourceString(IDS_COMMENTS) + ": ";
} // constructor


/**
 * create report file
 *
 * @param pszFileName   name of file to create
 * @return              succes/failure
 **/
BOOL ReportCreator::Create(LPCTSTR pszFileName)
{
  try
  {
    if (ReportViewWnd::GetReport() == NULL || ReportViewWnd::GetReport()->IsOpen == VARIANT_FALSE)
    {
      return FALSE;
    }

    if (FAILED(m_spPdfLib.CreateInstance(PDFlib_com::CLSID_PDF)))
    {
      LogErr("Could not initialize the PDF library.", "ReportCreator", "Create");
#ifndef FINAL_RELEASE
      AfxMessageBox("Could not initialize the PDF library.\n\nDid you register the necessary DLLs?");
#endif
      return FALSE;
    }

    uint4 uErrorCode = 0;
    // run this to get the total number of pages.
    if(m_eNumbering == V3DSystemSettings::eNUMBERING_BOTTOM)
    {
      m_spPdfLib->open_file(pszFileName);
      m_spPdfLib->set_parameter("serial", PDFLIB_KEY);

      try
      {
        if(m_eHeader == V3DSystemSettings::eHEADER_BITMAP)
        {
          m_iHeaderImage = m_spPdfLib->open_image_file("png", "c:\\v1k\\RptHdr.png", _bstr_t(), 0);
        }
      }
      catch(...)
      {
      }

      // bit setting:
      try { BeginPage(); }                  catch(_com_error) { uErrorCode |= (0x01   ); }
      try { WriteHeader(); }                catch(_com_error) { uErrorCode |= (0x01<<1); }
      try { WriteColonFindings(false); }    catch(_com_error) { uErrorCode |= (0x01<<2); }
      try { WriteColonFindings(true);  }    catch(_com_error) { uErrorCode |= (0x01<<3); }
      try { WriteNonDistendedFindings(); }  catch(_com_error) { uErrorCode |= (0x01<<4); }
      try { WriteOtherFindings(); }         catch(_com_error) { uErrorCode |= (0x01<<5); } 
      try { WriteImpression(); }            catch(_com_error) { uErrorCode |= (0x01<<6); }
      try { WriteRecommendations(); }       catch(_com_error) { uErrorCode |= (0x01<<7); }
      try { WriteAuthor(); }                catch(_com_error) { uErrorCode |= (0x01<<8); }
      try { EndPage(); }                    catch(_com_error) { uErrorCode |= (0x01<<9); }
      
      if(m_iHeaderImage != -1)
      {
        m_spPdfLib->close_image(m_iHeaderImage);
      }
    
      m_spPdfLib->close();
      remove(pszFileName);
    } // endif (NUMBERING_BOTTOM)
   
    // open again:
    // resume the page number.
    m_iTotalPages = m_iNumPages - 1;
    m_iNumPages = 1;
    m_spPdfLib->open_file(pszFileName);
    m_spPdfLib->set_parameter("serial", PDFLIB_KEY);
    
    try
    {
      if (m_eHeader == V3DSystemSettings::eHEADER_BITMAP)
      {
        m_iHeaderImage = m_spPdfLib->open_image_file("png", "c:\\v1k\\RptHdr.png", _bstr_t(), 0);
      }
    }
    catch(...)
    {
    }
    
    if (!(uErrorCode & (0x01)))     BeginPage();
    if (!(uErrorCode & (0x01<<1)))  WriteHeader();
    if (!(uErrorCode & (0x01<<2)))  WriteColonFindings(false);
    if (!(uErrorCode & (0x01<<3)))  WriteColonFindings(true);
    if (!(uErrorCode & (0x01<<4)))  WriteNonDistendedFindings();
    if (!(uErrorCode & (0x01<<5)))  WriteOtherFindings();
    if (!(uErrorCode & (0x01<<6)))  WriteImpression();
    if (!(uErrorCode & (0x01<<7)))  WriteRecommendations();
    if (!(uErrorCode & (0x01<<8)))  WriteAuthor();
    if (!(uErrorCode & (0x01<<9)))  EndPage();

    if (m_iHeaderImage != -1)
    {
      m_spPdfLib->close_image(m_iHeaderImage);
    }
    
    m_spPdfLib->close();
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
    AfxMessageBox(sErrorMsg);
   
    return FALSE;
  }

  return TRUE;
} // Create()


/**
 * initialize a new page
 **/
void ReportCreator::BeginPage()
{
  try
  {
    // start a new page
    m_spPdfLib->begin_page(PAGE_WIDTH, PAGE_HEIGHT);
    m_fCurrentY = m_fTopMargin;
  
    // select font for new page
    int4 iFont = m_spPdfLib->findfont("Times-Roman", "winansi", 0);
    m_spPdfLib->setfont(iFont, 12);

    // if user has selected Header from Preferences dialog
    if (m_eHeader == V3DSystemSettings::eHEADER_USER)
    {
      int4 y = PAGE_HEIGHT - LineHeight() - 5;
    
      if (!m_sHeaderTextList.empty())
      {
        for (stdstringlist::const_iterator sLine = m_sHeaderTextList.begin(); sLine != m_sHeaderTextList.end(); ++sLine)
        {
          m_spPdfLib->show_boxed(sLine->c_str(), LEFT_MARGIN, y, RIGHT_MARGIN-LEFT_MARGIN, LineHeight(),
            sShowBoxedLineFormat_Center.c_str(), _bstr_t());
          y -= LineHeight();
        }
      }
    }
    // if user has selected a bitmap header from Preferences dialog
    else if ( (m_eHeader == V3DSystemSettings::eHEADER_BITMAP) && (m_iHeaderImage != -1) )
    {
      // calculate acceptable ranges
      float4 fMaxLine = RIGHT_MARGIN - LEFT_MARGIN;
      float4 fMaxHeight = PAGE_HEIGHT - m_fTopMargin - 10;

      // bitmap dimensions
      float4 fWidth = m_spPdfLib->get_value("imagewidth", m_iHeaderImage);
      float4 fHeight = m_spPdfLib->get_value("imageheight", m_iHeaderImage);
      float4 fScaleX, fScaleY;
      float4 fHeightNew = fHeight;

      m_spPdfLib->save();
      {
        // scale if necessary
        if (fWidth > fMaxLine)
        {
          fScaleX = fMaxLine / fWidth;
          fHeightNew *= fScaleX;
          fScaleY = ((fHeightNew>fMaxHeight) ? fMaxHeight : fHeightNew)/fHeight;
        }
        else
        {
          fScaleX = 1.0F;
          fScaleY = (fHeight>fMaxHeight) ? fMaxHeight/fHeight : 1.0F;
        }

        // center
        float4 x = (fMaxLine - fWidth*fScaleX) / 2.0F + LEFT_MARGIN;

        // flip coordinate system
        m_spPdfLib->scale(fScaleX, fScaleY);
        m_spPdfLib->place_image(m_iHeaderImage, x/fScaleX, (m_fTopMargin+5) / fScaleY, 1.0F);  
      }
      // restore coordinate system
      m_spPdfLib->restore();
    }
    
    // if 'image' header, shift down a little bit.
    m_fCurrentY -= 16;
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "BeginPage");
    throw e;
  }
} // BeginPage()


/**
 * close a page
 **/
void ReportCreator::EndPage()
{
  try
  {
    // don't mind the fudge factors
    m_spPdfLib->setlinewidth(1.0F);
    m_spPdfLib->moveto(LEFT_MARGIN-10, m_fBottomMargin - 40);
    m_spPdfLib->lineto(PAGE_WIDTH - LEFT_MARGIN + 10, m_fBottomMargin - 40);
    m_spPdfLib->stroke();

    // format the footer and output
    CString sPage;
    CString sPatientInfo;
    ReportLib::IReportInfoPtr spInfo = ReportViewWnd::GetReport()->Info;
    COleDateTime dt(spInfo->StudyDate);
    sPatientInfo.Format(IDS_REPORT_FOOTER_PATIENTDATA_FORMAT,
      static_cast<LPCTSTR>(spInfo->PatientName), static_cast<LPCTSTR>(spInfo->PatientID), 
      static_cast<LPCTSTR>(dt.Format(VAR_DATEVALUEONLY, LANG_USER_DEFAULT)));

    // if user selected footer in Preferences dialog
    if (m_eNumbering == V3DSystemSettings::eNUMBERING_BOTTOM)
    {
      // format the footer and output
      sPage.Format( ((m_iTotalPages <= 0) ? IDS_FIRST_PAGE_NUMBERING_FORMAT : IDS_PAGE_NUMBERING_FORMAT),
        m_iNumPages++, m_iTotalPages);
    }

    // select font for new page
    int4 iFont = m_spPdfLib->findfont("Times-Roman", "winansi", 0);
    m_spPdfLib->setfont(iFont, 10);
    m_spPdfLib->show_boxed(static_cast<LPCTSTR>(sPage), 0, m_fBottomMargin - 40 - (LineHeight()*1.5), PAGE_WIDTH-LEFT_MARGIN+10, LineHeight(), "right", _bstr_t());
    m_spPdfLib->show_boxed(static_cast<LPCTSTR>(sPatientInfo), LEFT_MARGIN-10, m_fBottomMargin - 40 - (LineHeight()*1.5), PAGE_WIDTH-LEFT_MARGIN, LineHeight(), "left", _bstr_t());
    m_spPdfLib->setfont(iFont, 12);
  
    // end the page
    m_spPdfLib->end_page();
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "EndPage");
    throw e;
  }
} // EndPage()


/**
 * advance forward <iLines> number of lines
 *
 * @param iLines   number of lines to skip
 * @return         new line number
 **/
float4 ReportCreator::NextLine(const int4 iLines)
{
  // calculate the y value of the next line
  m_fCurrentY -=  iLines * LineHeight();
  
  return m_fCurrentY;
} // NextLine()


/**
 * query text line height
 *
 * @return    line height
 **/
float4 ReportCreator::LineHeight() const
{
  // line height is equal to  font height
  return m_spPdfLib->get_value("fontsize", 0.0f);
} // LineHeight()


/**
 * return the current y position
 * @return   the current y position
 */
float4 ReportCreator::CurrentLine() const
{
  return m_fCurrentY;
} // CurrentLine()


/**
 * write page header
 */
void ReportCreator::WriteHeader()
{
  try
  {
    float4 x = 200.0f;
    ReportLib::IReportInfoPtr spInfo = ReportViewWnd::GetReport()->Info;
    CString str;

    // output the patient information title and underline
    str.LoadString(IDS_PATIENTINFORMATION); str.MakeUpper();
    m_spPdfLib->show_xy(LPCTSTR(str + ':'), LEFT_MARGIN, CurrentLine());
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
  
    // patient name
    m_spPdfLib->show_xy(LPCTSTR(LoadResourceString(IDS_PATIENT) + ':'), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->show_xy(spInfo->PatientName.length()? spInfo->PatientName : _bstr_t(), x, CurrentLine());
  
    // patient ID
    m_spPdfLib->show_xy(LPCTSTR(LoadResourceString(IDS_PATIENTID) + ':'), LEFT_MARGIN, NextLine());
    m_spPdfLib->show_xy(spInfo->PatientID.length()? spInfo->PatientID : _bstr_t(), x, CurrentLine());
  
    // study date
    COleDateTime dt(spInfo->StudyDate);
    m_spPdfLib->show_xy(LPCTSTR(LoadResourceString(IDS_STUDYDATE) + ':'), LEFT_MARGIN, NextLine());
    m_spPdfLib->show_xy(static_cast<LPCTSTR>(dt.Format(VAR_DATEVALUEONLY, LANG_USER_DEFAULT)), x, CurrentLine());
  
    // referring physician
    m_spPdfLib->show_xy(LPCTSTR(LoadResourceString(IDS_REFERRINGPHYSICIAN) + ':'), LEFT_MARGIN, NextLine());
    m_spPdfLib->show_xy(spInfo->ReferringDoctor.length()? spInfo->ReferringDoctor : _bstr_t(), x, CurrentLine());
 
    // output indications title and underline
    str.LoadString(IDS_INTRODUCTION); str.MakeUpper();
    m_spPdfLib->show_xy(LPCTSTR(str + ':'), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
  
    // new x value for indications
    x = LEFT_MARGIN + 20;
  
    // indications is a bitmask
    int4 iIndications  = spInfo->Indication;
  
    CString s;
    if(iIndications == 0) // default output if there aren't indications
    {
      s.Format(IDS_INDICATION_ELECTIVE_COLONSCOPY_FORMAT, static_cast<LPCSTR>(spInfo->PatientAge),
        m_tPatientSex[spInfo->GetPatientSex()].c_str());
      m_spPdfLib->show_xy(static_cast<LPCTSTR>(s), LEFT_MARGIN, NextLine(2));

      /// not to forget to write out other indications.
      std::string str = spInfo->OtherIndication;
      if (str.size() != 0)
      {
        std::string sOtherIndicationStr(str);
        if (sOtherIndicationStr.length())
        {
          int4 iPos = sOtherIndicationStr.find_first_of("\n");
          while (iPos != std::string::npos)
          {
            sOtherIndicationStr.replace(iPos-1, 2, " / ");
            iPos = sOtherIndicationStr.find_first_of("\n", iPos);
          }
        }
        std::string strNormal = LPCTSTR(LoadResourceString(IDS_OTHER_INDICATIONS_LABEL_FULL)) + sOtherIndicationStr;
        // CString sOtherIndication(strNormal.c_str());
        CheckPage(GetRequiredLines(x, NextLine(2), strNormal.c_str(), 36));
        WriteLine(x, CurrentLine(), strNormal.c_str(), 56);
      }
    }
    else // check each bit value and output string if present
    {
      s.Format(IDS_INDICATION_GENERAL_FORMAT, static_cast<LPCSTR>(spInfo->PatientAge), 
        m_tPatientSex[spInfo->GetPatientSex()].c_str());
      m_spPdfLib->show_xy(static_cast<LPCTSTR>(s), LEFT_MARGIN, NextLine(2));
    
      NextLine();

      if(iIndications&ReportLib::Screening)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::Screening].c_str(), x, NextLine());
      }
   
      if(iIndications&ReportLib::PersonalHistoryOfPolyps)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::PersonalHistoryOfPolyps].c_str(), x, NextLine());
      }

      if(iIndications&ReportLib::PersonalHistoryOfCancer)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::PersonalHistoryOfCancer].c_str(), x, NextLine());
      }
          
      if(iIndications&(ReportLib::PolypOnFlexSigmoid+ReportLib::PolypOnFlexSigmoid))
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::PolypOnFlexSigmoid+ReportLib::PolypOnFlexSigmoid].c_str(), x, NextLine());
      }

      if(iIndications&ReportLib::FamilyHistoryOfCancer)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::FamilyHistoryOfCancer].c_str(), x, NextLine());
      }

      if(iIndications&ReportLib::Constipation)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::Constipation].c_str(), x, NextLine());
      }

   
      if(iIndications&ReportLib::Anemia)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::Anemia].c_str(), x, NextLine());
      }
    
      if(iIndications&ReportLib::Hematochezia)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::Hematochezia].c_str(), x, NextLine());
      }

      if(iIndications&ReportLib::AbnormalImaging)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::AbnormalImaging].c_str(), x, NextLine());
      }
    
      if(iIndications&ReportLib::AbdominalPain)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::AbdominalPain].c_str(), x, NextLine());
      }
   
      if(iIndications&ReportLib::IBD)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::IBD].c_str(), x, NextLine());
      }
    
      if(iIndications&ReportLib::Diarrhea)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::Diarrhea].c_str(), x, NextLine());
      }

      if(iIndications&ReportLib::PolypOnFlexSigmoid)
      {
        m_spPdfLib->show_xy(m_tIndications[ReportLib::PolypOnFlexSigmoid].c_str(), x, NextLine());
      }
   
      std::string str = spInfo->OtherIndication;
      if (str.size() != 0)
      {
        std::string sOtherIndicationStr(str);
        if (sOtherIndicationStr.length())
        {
          int4 iPos = sOtherIndicationStr.find_first_of("\n");
          while (iPos != std::string::npos)
          {
            sOtherIndicationStr.replace(iPos-1, 2, " / ");
            iPos = sOtherIndicationStr.find_first_of("\n", iPos);
          }
        }
        std::string strNormal = LPCTSTR(LoadResourceString(IDS_OTHER_INDICATIONS_LABEL_ABBREV)) + sOtherIndicationStr;
        // CString sOtherIndication(strNormal.c_str());
        CheckPage(GetRequiredLines(x, NextLine(2), strNormal.c_str(), 36));
        WriteLine(x, CurrentLine(), strNormal.c_str(), 56);
      }
    }
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteHeader");
    throw e;
  }
} // WriteHeader().


/**
 * writes out all the non-extra (false) or extra colonic (true) findings.
 * @param bExtraColonic   whether to output colonic or external findings
 */
void ReportCreator::WriteColonFindings(const bool bExtraColonic)
{
  try
  {
    // get number of colon findings
    int4 iNumEntries = ReportViewWnd::GetReport()->Entries->Count;
  
    // if there aren't any return
    if (iNumEntries == 0)
    {
      return;
    }

    // get the number of colon findings, except 'extra-colonic'
    bool bNoSuchFinding = true;
    for (int4 iEntry=0; iEntry < iNumEntries; iEntry++)
    {
      // grab finding from report
      ReportLib::IReportEntryPtr spEntry = ReportViewWnd::GetReport()->Entries->GetItem((long)iEntry);
      bNoSuchFinding = bExtraColonic ^ (spEntry->ColonFinding == ReportLib::Other+2);
      if (bNoSuchFinding == false) break;
    }
    // if all are 'extra' ...
    if ( bNoSuchFinding )
    {
      return;
    }

    // output the colon findings title and underline
    CString str;
    
    if (bExtraColonic)
    {
      str.LoadString(IDS_EXTRA_COLONIC_FINDINGS);
    }
    else
    {
      str.LoadString(IDS_COLON_FINDINGS);
    }
    
    str.MakeUpper();
    m_spPdfLib->show_xy(LPCTSTR(str + ':'), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
  
    NextLine(1);

    // for each colon finding
    for (iEntry=0; iEntry < iNumEntries; iEntry++)
    {
      // grab finding from report
      ReportLib::IReportEntryPtr spEntry = ReportViewWnd::GetReport()->Entries->GetItem((long)iEntry);
      if (bExtraColonic)
      {
        if (spEntry->ColonFinding != ReportLib::Other+2) continue;
      }
      else
      {
        if (spEntry->ColonFinding == ReportLib::Other+2) continue;
      }

      // extract Supine/Prone marks from 'spEntry->Name'.
      CString csTitle, s;
      std::string sName(spEntry->Name);
      std::string sSubName;
      std::string sStudyOrientation;
      const bool bDistanceUndetermined = (sName[0]=='-');
      if (bDistanceUndetermined) sSubName = sUncertainDistanceFormat;
      if ((sName.find(LoadResourceString(IDS_SUPINE_EXTRA)) != std::string::npos) && (spEntry->ColonFinding != ReportLib::Other+2))
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen(LoadResourceString(IDS_SUPINE_EXTRA)));
        }
        sStudyOrientation = LoadResourceString(IDS_SUPINE_EXTRA);
      }
      else if ((sName.find(LoadResourceString(IDS_PRONE_EXTRA)) != std::string::npos ) && (spEntry->ColonFinding != ReportLib::Other+2))
      {
        if (!bDistanceUndetermined)
        {
          sSubName = sName.substr(strlen(LoadResourceString(IDS_PRONE_EXTRA)));
        }
        sStudyOrientation = LoadResourceString(IDS_PRONE_EXTRA);
      }
      else
      {
        if (!bDistanceUndetermined)
        {
          sSubName = spEntry->Name;
        }
      }

      /// pre-processing 'comments': remove the last '\n'(s).
      std::string sComments = TextPreprocessing(std::string(spEntry->Comments), std::string(sCommentsFormat), 
                                                std::string(" - "), /* new-line*/true, 19);

      // format textual portion of finding based on type
      if (spEntry->ColonFinding == ReportLib::Polyp)
      {
        CString strQuantity;
        /// if (maxiumum==0.0 && Perpendicular==0.0) -> no display of these info.
        if ((spEntry->Maxiumum != 0.0F) || (spEntry->Perpendicular != 0.0F))
        {
          strQuantity.Format("%.1f x %.1f mm ", spEntry->Maxiumum, spEntry->Perpendicular);
        }

        if(spEntry->Translucency == ReportLib::Indeterminate)
        {
          csTitle.Format(IDS_COLONFINDING_NOTRANSLUCENCY_FORMAT,
            strQuantity,
            (strQuantity.GetLength()>0 ? LowerCaseString(m_tPolypShape[spEntry->GetShape()].c_str()) : m_tPolypShape[spEntry->GetShape()].c_str()),
            LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()),
            LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
            static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString())
          );
        }
        else
        {
          csTitle.Format(IDS_COLONFINDING_TRANSLUCENCY_FORMAT,
            strQuantity,
            (strQuantity.GetLength()>0 ? LowerCaseString(m_tPolypShape[spEntry->GetShape()].c_str()) : m_tPolypShape[spEntry->GetShape()].c_str()),
            LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()), 
            LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
            LowerCaseString(m_tTranslucency[spEntry->GetTranslucency()].c_str()),
            static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString())
          );
        }
      }
      else if(spEntry->ColonFinding == ReportLib::Diverticulum)
      {
        CString sFinding;
        VERIFY(sFinding.LoadString(IDS_DIVERTICULUM));
      
        csTitle.Format(IDS_COLONFINDING_OTHER_FORMAT, 
          sFinding,
          LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()), 
          LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
          static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString())
        );
      }
      else if (spEntry->ColonFinding == ReportLib::Stenosis)
      {
        CString sFinding;
        VERIFY(sFinding.LoadString(IDS_STENOSIS));
     
        csTitle.Format(IDS_COLONFINDING_OTHER_FORMAT, 
          sFinding,
          LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()), 
          LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
          static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString())
        );
      }
      else if (spEntry->ColonFinding == ReportLib::Other) // Normal
      {
        csTitle.Format(IDS_NORMAL_FINDING, 
          LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()),
          LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
          (static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()))
        );
      }
      else if (spEntry->ColonFinding == ReportLib::Other+1) // Colonic Other
      {
        csTitle.Format(IDS_OTHER_LOCATION_FORMAT,
          LowerCaseString(m_tColonSegment[spEntry->GetLocation()].c_str()),
          LowerCaseString(static_cast<LPCTSTR>(sSubName.c_str())),
          (static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()))
        );
      }
      else if (spEntry->ColonFinding == ReportLib::Other+2) // Colonic Extra.
      {
        csTitle.Format(IDS_EXTRA_COLONIC_FINDING_FORMAT,
          (static_cast<LPCTSTR>(spEntry->Comments.length() ? sComments.c_str() : CString()))
        );
      }

      // add Supine/Prone mark.
      s = CString(sStudyOrientation.c_str()) + ' ' + csTitle;
 
      // check to see if there is enough room to begin to output the images
      float4 fRequiredHeight = GetRequiredBoxedLines(LEFT_MARGIN, CurrentLine(), s) * LineHeight();
      if((CurrentLine() - fRequiredHeight - (2*IMAGE_SPACE) - IMAGE_HEIGHT) < m_fBottomMargin)
      {
        // start a new page
        EndPage();
        BeginPage();
      }

      // write out text
      WriteLineBoxed(LEFT_MARGIN, CurrentLine(), s);

      // get number of images in the finding
      int4 iNumImages = spEntry->Images->Count;
    
      // for each image in the finding
      for(int4 iImage(0), iActualImg(0); iImage < iNumImages; iImage++)
      {
        // grab the image from the colon finding
        ReportLib::IImagePtr spImage = spEntry->Images->GetItem((long)iImage);

        // not to write out overview image if this is a extra-colonic finding.
        bool bIsExtraColonicFinding = false;
        if (spEntry->ColonFinding == ReportLib::Other+2)
        {
          bIsExtraColonicFinding = IsOverviewImgOfExtraColonic(spImage);
          if (bIsExtraColonicFinding) continue;
        }

        // calculate the x value(3 images per line)
        int x = LEFT_MARGIN + (iActualImg%3)*(IMAGE_WIDTH+IMAGE_SPACE);
        // if this is the first image on a line calculate the y value
        if (iActualImg%3 == 0)
        {
          m_fCurrentY -= IMAGE_HEIGHT + IMAGE_SPACE;
        }
      
        // check if we need a new page
        if(CurrentLine() < m_fBottomMargin)
        {
          // start a new page
          EndPage();
          BeginPage();
        
          // offset current y by image height
          m_fCurrentY -= IMAGE_HEIGHT;
        }

        // output the image
        WriteImage(x, PAGE_HEIGHT - CurrentLine() - IMAGE_HEIGHT, spImage);
        if ( !bIsExtraColonicFinding ) iActualImg ++;
      } // end for each image
    
      m_fCurrentY -= IMAGE_SPACE;
    } // end for each colon finding
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteColonFindings");
    throw e;
  }
} // WriteColonFindings()


/**
 * output non-distended areas
 */
void ReportCreator::WriteNonDistendedFindings()
{
  try
  {
    // get number of "Non-Distended" findings
    int4 iNumFindings = ReportViewWnd::GetReport()->NonDistendedFindings->Count;
  
    // if there aren't any return
    if(iNumFindings == 0)
    {
      return;
    }
  
    // check that the title will fit on the current page
    if((CurrentLine() - (LineHeight()*6) - IMAGE_HEIGHT) < m_fBottomMargin)
    {
      // start a new page
      EndPage();
      BeginPage();
    }
  
    // output the "Non-Distended" title and underline
    m_spPdfLib->show_xy(LPCTSTR(LoadResourceString(IDS_NONDISTENDEDAREAS_HEADING)), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();  
    NextLine(6);
  
    // for each "Non-Distended" area
    for(int4 iFinding(0); iFinding <iNumFindings; iFinding++)
    {    
      // grab the "Non-Distended" finding
      ReportLib::IReportNonDistendedFindingPtr spFinding = 
        ReportViewWnd::GetReport()->NonDistendedFindings->GetItem((long)iFinding);
    
      // grab the image from the Non-Distended finding
      ReportLib::IImagePtr spImage = spFinding->Image;
    
      // calculate the x value(3 Non-Distended areas per line)
      int x = LEFT_MARGIN + (iFinding%3)*(IMAGE_WIDTH+IMAGE_SPACE);
    
      // if this is the first image on a line calculate the y value
      if (iFinding%3 == 0 && iFinding != 0) 
      {
        m_fCurrentY = m_fCurrentY - IMAGE_HEIGHT - IMAGE_SPACE - (LineHeight()*4.5f);
      }
    
      // check if we need a new page
      if(CurrentLine() - IMAGE_HEIGHT < m_fBottomMargin)
      {
        // start a new a page
        EndPage();
        BeginPage();
        NextLine(3);
      }

      // draw box for text output and fill
      m_spPdfLib->setlinewidth(1.0f);
      m_spPdfLib->setrgbcolor_fill(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
      m_spPdfLib->rect(x, CurrentLine(),IMAGE_WIDTH, 4 * LineHeight());
      m_spPdfLib->fill_stroke();    
      m_spPdfLib->setrgbcolor_fill(0.0f, 0.0f, 0.0f);    
    
      // output the text
      CString sStudyOrientation;
      sStudyOrientation.Format("(%s) ", 
        LoadResourceString((spFinding->DatasetPositionEnumProp == ReportLib::Supine) ? IDS_SUPINE : IDS_PRONE));

      std::string strText(spFinding->Text);
      if (strText[0] == '-')
      {
        strText = LoadResourceString(IDS_COLONFINDING_AREA_UNCERTAIN);
      }

      CString sSegment;
      sSegment.Format("%s %s", LoadResourceString(IDS_SEGMENT), m_tColonSegment[spFinding->GetLocation()].c_str());
      m_spPdfLib->show_boxed(static_cast<LPCTSTR>(sSegment), x, CurrentLine() + (LineHeight() * 2.7f), IMAGE_WIDTH, LineHeight(),
        sShowBoxedLineFormat_Center.c_str(), _bstr_t());
      m_spPdfLib->show_boxed(strText.c_str(), x, CurrentLine() + (LineHeight() * 1.7f), IMAGE_WIDTH, LineHeight(),
        sShowBoxedLineFormat_Center.c_str(), _bstr_t());
      m_spPdfLib->show_boxed(static_cast<LPCTSTR>(sStudyOrientation), x, CurrentLine() + (LineHeight() * 0.7f), IMAGE_WIDTH, LineHeight(),
        sShowBoxedLineFormat_Center.c_str(), _bstr_t());

      // output the image
      WriteImage(x, PAGE_HEIGHT  - CurrentLine() + (IMAGE_SPACE / 2.5f) , spImage);
    } // end for each "Non-Distended" finding

    // the current y position
    m_fCurrentY = m_fCurrentY - IMAGE_HEIGHT - IMAGE_SPACE - (LineHeight()*3.5f);
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteNonDistendedFindings");
    throw e;
  }
} // WriteNonDistendedFindings()


/**
 * output other findings
 */
void ReportCreator::WriteOtherFindings()
{
  try
  {
    // get number of "Other" findings
    int4 iNumOtherFindings = ReportViewWnd::GetReport()->OtherFindings->Count;
    if(iNumOtherFindings == 0)
    {
      return;
    }

    // adjust the current y position
    CheckPage(3);

    CString str;
    // output the "Other Findings" title and underline
    str.LoadString(IDS_OTHER_FINDINGS); str.MakeUpper();    
    m_spPdfLib->show_xy(LPCTSTR(str + ':'), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
    NextLine(2);
   
    for(int4 iFinding(0); iFinding < iNumOtherFindings; iFinding++)
    {
      // grab the other finding
      ReportLib::IReportOtherFindingPtr spFinding =  ReportViewWnd::GetReport()->OtherFindings->GetItem((long)iFinding);

      // build textual representation
      CString s;
      CString sQuantity;
      if ((spFinding->MaximumSize != 0.0F) || (spFinding->PerpendicularSize != 0.0F))
      {
        sQuantity.Format("%.1f x %.1f cm ", spFinding->MaximumSize, spFinding->PerpendicularSize);
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
        s.Format(sOtherFindingsEntryFormat,
          sQuantity,
          (sQuantity.GetLength()>0 ? m_tLungLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_tLungLocation[spFinding->GetLocation()].c_str())),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
        );
        break;
    
      case ReportLib::Lung_Pneomuthorax:
        s.Format(sLungPneomuthoraxFormat, 
          UpperHeadString(m_tPneumothoraxSize[spFinding->GetSize()].c_str()),
          m_tPneumothoraxLocation[spFinding->GetLocation()].c_str(),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.c_str()) 
          );
        break;
    
      case ReportLib::Liver_Mass:
      case ReportLib::Liver_Cyst:        
        s.Format(sOtherFindingsEntryFormat,
          sQuantity,
          (sQuantity.GetLength()>0 ? m_tLiverLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_tLiverLocation[spFinding->GetLocation()].c_str())),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
        );
        break;

      case ReportLib::Adrenal_Mass:
        s.Format(sOtherFindingsEntryFormat,
          sQuantity,
          (sQuantity.GetLength()>0 ? m_tAdrenalLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_tAdrenalLocation[spFinding->GetLocation()].c_str())),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;
   
      case ReportLib::Renal_Mass:
      case ReportLib::Renal_Cyst:
      case ReportLib::Renal_Stones:
        s.Format(sOtherFindingsEntryFormat,
          sQuantity,
          (sQuantity.GetLength()>0 ? m_tRenalLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_tRenalLocation[spFinding->GetLocation()].c_str())),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;

      case ReportLib::Renal_Scar:
        s.Format(sRenalScarFormat, 
          UpperHeadString(m_tRenalLocation[spFinding->GetLocation()].c_str()),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;
      
      case ReportLib::Renal_Hydronephrosis:
        s.Format(sRenalHydronephrosisFormat,
          UpperHeadString(m_tDegree[spFinding->GetDegree()].c_str()),
          m_tKidneyLocation[spFinding->GetLocation()].c_str(),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;

      case ReportLib::Vascular_AAA:
      {
        CString sLength;
        if (spFinding->Length != 0.0F)
        {
          sLength.Format("with %.1f cm length ", spFinding->Length);
        }

        s.Format(sOtherFindingsVascularAAAEntryFormat,
          UpperHeadString(m_tVascularLocation[spFinding->GetLocation()].c_str()),
          sQuantity,
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          sLength,
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        }
        break;


      case ReportLib::Vascular_ArteralCalcification:
        s.Format(sVascularArteralCalcificationFormat,
          UpperHeadString(m_tDegree[spFinding->GetDegree()].c_str()),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
        );
        break;
    
      case ReportLib::Spleen_Mass:
      case ReportLib::Spleen_Granuloma:
        s.Format(sOtherFindingsEntryFormat,
          sQuantity,
          (sQuantity.GetLength() > 0 ? m_tSplenicLocation[spFinding->GetLocation()].c_str() : UpperHeadString(m_tSplenicLocation[spFinding->GetLocation()].c_str())),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;
  
      case ReportLib::GI_InguinalHernia:
        s.Format(sGIInguinalHerniaFormat,
          UpperHeadString(m_tAdrenalLocation[spFinding->GetLocation()].c_str()),
          m_tOtherFindings[spFinding->GetFinding()].c_str(), 
          static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;
  
      case ReportLib::Liver_Gallstones:
      case ReportLib::Liver_FattyLiver:        
      case ReportLib::GI_HiatalHernia:
      case ReportLib::Gynecology_UterineFibroids:
        s.Format(sGynecologyUterineFibroidsFormat,
           UpperHeadString(m_tOtherFindings[spFinding->GetFinding()].c_str()),
           static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString())
          );
        break;
      
      case ReportLib::Misc_Other:
        s.Format(IDS_COLONFINDING_MISCELLANEOUS_OTHER_FORMAT, static_cast<LPCTSTR>(sComment.length() ? sComment.c_str() : CString()));
        break;
    
      default:
        continue;        
      }

      // check if we need a new page
      CheckPage(GetRequiredLines(LEFT_MARGIN, NextLine(), s, 34));
      // output the text
      WriteLine(LEFT_MARGIN, CurrentLine(), s, 34);
    } // end for each "Other" finding
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteOtherFindings");
    throw e;
  }  
} // WriteOtherFindings() 


/**
 * output doctor recommendations
 */
void ReportCreator::WriteRecommendations()
{
  try
  {
    // grap the info object
    ReportLib::IReportInfoPtr spInfo = ReportViewWnd::GetReport()->Info;

    // grab the recommendations bitmask
    int4 iRecommendations = spInfo->Recommendation;
  
    // grab the other recommendation string
    CString sOtherRecommendation = static_cast<LPCTSTR>(spInfo->OtherRecommendation);
  
    // if there aren't any return
    if(iRecommendations == 0 && sOtherRecommendation.IsEmpty())
    {
      return;
    }
  
    // sOtherRecommendation.Insert(0, "- ");
    if (iRecommendations == 0)
    {
      std::string sNormalOtherRecommStr = TextPreprocessing(std::string(sOtherRecommendation), 
        std::string(" "), std::string(sOtherExtraFormat), false, 1);
      CheckPage(3 + GetRequiredLines(LEFT_MARGIN, CurrentLine(), sNormalOtherRecommStr.c_str(), 34));
    }
    else
    {
      CheckPage(4);
    }

    CString titleStr;

    // output the "Recommendations" title and underline
    titleStr.LoadString(IDS_RECOMMENDATIONS); titleStr.MakeUpper();
    m_spPdfLib->show_xy(LPCTSTR(titleStr + ':'), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
    NextLine();

    // check for the existence of each bit value if it exists print of text
    if(iRecommendations&ReportLib::FollowUp)
    {
      CString s;
      s.Format(IDS_OTHER_FINDINGS_RECOMMENDATIONS_FOLLOWUP_FORMAT,
        m_tRecommendType[spInfo->GetRecommendationType()].c_str(),
        spInfo->RecommendationTime,
        m_tRecommendUnit[spInfo->GetRecommendationUnit()].c_str()
        );
      m_spPdfLib->show_xy(LPCTSTR(s), LEFT_MARGIN, NextLine());
    }
  
    if (iRecommendations&ReportLib::Removal)
    { 
      CheckPage();
      CString s; s.LoadString(IDS_OTHER_FINDINGS_RECOMMENDATIONS_POLYP_REMOVAL);
      m_spPdfLib->show_xy(LPCTSTR(s), LEFT_MARGIN, NextLine());
    }
  
    // if the other string has a value print it out
    if(spInfo->OtherRecommendation.length())
    {
      std::string sNormalOtherRecommStr = TextPreprocessing(std::string(sOtherRecommendation), 
        " ", std::string(sOtherExtraFormat), false, 1);
      CheckPage(GetRequiredLines(LEFT_MARGIN, NextLine(), sNormalOtherRecommStr.c_str(), 34));
      WriteLine(LEFT_MARGIN, NextLine(), sNormalOtherRecommStr.c_str());
    }
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteRecommendations");
    throw e;
  }
} // WriteRecommendations


/**
 * output impressions
 */
void ReportCreator::WriteImpression()
{
  try
  {
    ReportLib::IReportInfoPtr spInfo = ReportViewWnd::GetReport()->Info;
    CString sImpression = static_cast<LPCTSTR>(spInfo->Impression);
    
    std::string strImpression(spInfo->Impression);

    if(strImpression.size() == 0)
      return;
  
    CheckPage(3);
  
    CString s;
    s.LoadString(IDS_IMPRESSION); 
    s.MakeUpper();
    m_spPdfLib->show_xy(static_cast<LPCTSTR>(s), LEFT_MARGIN, NextLine(2));
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, CurrentLine()-3);
    m_spPdfLib->lineto(RIGHT_MARGIN, CurrentLine()-3);
    m_spPdfLib->stroke();
  
    if (spInfo->Impression.length())
    {
      std::string sNormalStr = TextPreprocessing(strImpression, std::string(" "), std::string(" - "), false, 1);
      NextLine();
      CheckPage(GetRequiredLines(LEFT_MARGIN, NextLine(2), sNormalStr.c_str(), 34));
      WriteLine(LEFT_MARGIN, CurrentLine(), sNormalStr.c_str(), 34);
    }
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteImpression");
    throw e;
  }
} // WriteImpression()


/**
 * output report author name
 */
void ReportCreator::WriteAuthor()
{
  try
  {
    // grab the info object
    ReportLib::IReportInfoPtr spInfo = ReportViewWnd::GetReport()->Info;
  
    // if blank than return
    if(spInfo->Author.length() == 0)
    {
      return;
    }

    // check if we need a new page
    CheckPage(4);

    // write out 1 line and write out author name underneath
    m_spPdfLib->setlinewidth(2.0f);
    m_spPdfLib->moveto(LEFT_MARGIN, NextLine(3));
    m_spPdfLib->lineto(LEFT_MARGIN + 150, CurrentLine());
    m_spPdfLib->stroke();
    m_spPdfLib->show_xy(spInfo->Author, LEFT_MARGIN, NextLine());  
  }
  catch(_com_error& e)
  {
    CString sErrorMsg;
    sErrorMsg.Format("Error creating PDF report.0x%08X", e.Error());
    LogRec(std::string(sErrorMsg), "ReportCreator", "WriteAuthor");
    throw e;
  }
} // WriteAuthor()


/**
 * get the required amount of lines for the given string
 * @param x              x-location for string
 * @param y              y-location for string
 * @param sz             the string
 * @param fLeftOffset    string indent
 * @return               how many lines required for string display
 */
float4 ReportCreator::GetRequiredLines(const float4 x, const float4 y, LPCTSTR psz, const float4 fLeftOffset) const
{
  float4 cy = LineHeight();
  float4 fNumLines(1.0f);
  
  while(true)
  {
    int4 iRet = m_spPdfLib->show_boxed(psz, x + fLeftOffset, y, RIGHT_MARGIN - x, cy, "left", "blind");
    if(iRet == 0)
    {
      break;
    }
    
    // cy *= 2; // why double the lines?
    cy += LineHeight();
    y -= LineHeight();
    fNumLines++;
  }  

  return fNumLines;
} // GetRequiredLines()


/**
 * output a line of text
 * @param x              x-location for string
 * @param y              y-location for string
 * @param sz             the string
 * @param fLeftOffset    string indent
 */
void ReportCreator::WriteLine(const float4 x, const float4 y, LPCTSTR psz, const float4 fLeftOffset)
{
  float4 fNumLines = GetRequiredLines(x, y, psz, fLeftOffset);
  
  if(fNumLines > 1)
  {
    NextLine(fNumLines-1);
  }

  fNumLines ++;

  m_spPdfLib->show_boxed(psz, x, CurrentLine(), LINE_MAX-fLeftOffset, fNumLines*LineHeight(), "left", _bstr_t());
} // WriteLine()


/**
 * write a line with a border
 * @param x    x-location for string
 * @param y    y-location for string
 * @param sz   the string
 */
void ReportCreator::WriteLineBoxed(const float4 x, const float4 y, LPCTSTR psz)
{
  // get required line for text and offset y position
  float4 nNumLines = GetRequiredLines(x+BOXED_TEXT_OFFSETX, y, psz) + 1;
  NextLine(nNumLines);

  // draw a box
  m_spPdfLib->setlinewidth(1.0f);
  m_spPdfLib->setrgbcolor_fill(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
  m_spPdfLib->rect(x, CurrentLine(), RIGHT_MARGIN-x, nNumLines * LineHeight());
  m_spPdfLib->fill_stroke();
  
  // write text within box
  m_spPdfLib->setrgbcolor_fill(0.0f, 0.0f, 0.0f);
  m_spPdfLib->show_boxed(psz, x+BOXED_TEXT_OFFSETX, CurrentLine() + (LineHeight() / 2.0f) , RIGHT_MARGIN-x-BOXED_TEXT_OFFSETX, (nNumLines-1)*LineHeight() , "left", _bstr_t());
} // WriteLineBoxed()


/**
 * output an image
 * @param x         x-location for string
 * @param y         y-location for string
 * @param spImage   the image
 */
void ReportCreator::WriteImage(const float4 x, const float4 y, const ReportLib::IImagePtr& spImage)
{
  // spImage->Resize(IMAGE_WIDTH, IMAGE_HEIGHT);

  // access the pixel data
  COleVariant vPixels = spImage->Pixels;
  
  LPBYTE pPixels;
  SafeArrayAccessData(vPixels.parray, reinterpret_cast<void**>(&pPixels));
  
  // convert from RGB to BGR
  LPBYTE nMax = pPixels + spImage->ImageSize;
  BYTE temp;
  while(pPixels <  nMax)
  {
    temp = pPixels[0];
    pPixels[0] = pPixels[2];
    pPixels[2] = temp;
    pPixels +=3;
  }
  
  SafeArrayUnaccessData(vPixels.parray);
  
  // read the image from memory
  const int4 iImageID = 
    m_spPdfLib->open_image(
    "raw",
    "memory",
    &vPixels,
    spImage->ImageSize,
    spImage->Width,
    spImage->Height,
    3,
    spImage->BitsPerPixel/3,
    _bstr_t()
    );
  
  // flip coordinate system  
  const float4 fScaleX = IMAGE_WIDTH / spImage->Width;
  const float4 fScaleY = IMAGE_HEIGHT / spImage->Height;
  
  m_spPdfLib->save();
  m_spPdfLib->translate(0.0f, PAGE_HEIGHT);
  m_spPdfLib->scale(fScaleX, -fScaleY);
  m_spPdfLib->place_image(iImageID, x/fScaleX, y/fScaleY, 1.0f);
  // restore coordinate system and free image
  m_spPdfLib->restore();
  m_spPdfLib->close_image(iImageID);

} // WriteImage()


/**
 * get the required amount of lines for the given string plus enclosing box
 * @param x              x-location for string
 * @param y              y-location for string
 * @param sz             the string
 * @return               how many lines required for string display
 */
float4 ReportCreator::GetRequiredBoxedLines(const float4 x, const float4 y, LPCTSTR psz) const
{
  // get required line for boxed text
  return  GetRequiredLines(x+BOXED_TEXT_OFFSETX, y, psz) + 1.0f;
} // GetRequiredBoxedLines()


/**
 * creates new page if number of lines exceeds current
 * @param iNeededLines   number of lines needed
 */
void ReportCreator::CheckPage(const int4 iNeededLines)
{
  // see if adding the needed lines will go past the bottom margin
  if(CurrentLine() - (iNeededLines*LineHeight()) < m_fBottomMargin)
  {
    // start a new page if it does
    EndPage();
    BeginPage();
  }
} // CheckPage()


/**
 * preprocessing of string text.
 * @param sTextStr      start string
 * @param sHeaderStr    header string
 * @param sMarkStr      ???
 * @param bNewLine      whether to start a new line
 * @param iNumOfSpace   number of spaces to indent lines
 * @return              post-processed string
 */
std::string ReportCreator::TextPreprocessing(const std::string & sTextStr, const std::string & sHeaderStr,
                                             const std::string & sMarkStr, const bool bNewLine, const int4 iNumOfSpace) const
{
  /// pre-processing 1: remove the last '\n'(s).
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

  /// preprocessing 2: add space before each new line.
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

  /// preprocessing 3: add a header to the string.
  if (sNewText.length())
  {
    sNewText.insert(0, sHeaderStr+sMarkStr);
    if (bNewLine) sNewText.insert(0, "\n");
  }

  return sNewText;
} // TextPreprocessing()

/**
 * ARCHANE, ARCHAIC test if this is a overview image & extra-colonic.
 * @param spImage   image to test
 * @return          true for extra-colonic finding
 */
bool ReportCreator::IsOverviewImgOfExtraColonic(ReportLib::IImagePtr& spImage)
{
  // access the pixel data
  LPBYTE pPixels;
  COleVariant vPixels = spImage->Pixels;
  SafeArrayAccessData(vPixels.parray, reinterpret_cast<void**>(&pPixels));  

  if ((pPixels[0]==0x0F && pPixels[2]==0x0F) || (pPixels[0]==26 && pPixels[2]==26))
  {
    SafeArrayUnaccessData(vPixels.parray);
    return true;
  }
    
  SafeArrayUnaccessData(vPixels.parray);
  return false;
} // IsOverviewImgOfExtraColonic()


// $Log: ReportCreator.C,v $
// Revision 1.2.2.1  2009/06/16 13:50:32  dongqing
// fix the report text "atuncertain" to correct "at uncertain"
//
// Revision 1.2  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.29  2002/09/12 18:09:25  jmeade
// Fixed debug assertion.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportCreator.C,v 1.2.2.1 2009/06/16 13:50:32 dongqing Exp $
// $Id: ReportCreator.C,v 1.2.2.1 2009/06/16 13:50:32 dongqing Exp $
