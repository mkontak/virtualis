// $Id: ReportMiscellaneous.C,v 1.7.2.21 2010/10/26 20:05:03 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for miscellaneous section of report
// Author: George Economos

#include "stdafx.h"
#include "viatron.h"
#include "resource.h"
#include "ReportMiscellaneous.h"
#include "ReaderGlobal.h"
#include "ApplicationSettings.h"

#pragma warning(disable:4800)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace WinUtils;
using namespace ColonReport;
using namespace ReaderLib;

const int4 ReportMiscellaneous::IDD(IDD_RPT_MISC_DUAL);

const char ReportMiscellaneous::m_sHiddenTextDelimiter[3] = { 13,13,0 };
const char ReportMiscellaneous::m_sHiddenTextNewline[3] = { 13,0 };
const std::string ReportMiscellaneous_m_sDualNameDelimeter(" :: ");

#define FILE_REVISION "$Revision: 1.7.2.21 $"

/**
 * constructor
 * @param pParent   parent window
 **/
ReportMiscellaneous::ReportMiscellaneous(CWnd* pParent /*=NULL*/)
  : ReportWndBase(ReportMiscellaneous::IDD, pParent)
{
  //{{AFX_DATA_INIT(ReportMiscellaneous)
  //}}AFX_DATA_INIT
} // constructor


/**
 * Dialog data variable initialization.
 *
 * @param pDX   Variable data.
 */
void ReportMiscellaneous::DoDataExchange(CDataExchange* pDX)
{
  ReportWndBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ReportMiscellaneous)
  DDX_Control(pDX, IDC_UNIT, m_wndUnit);
  DDX_Control(pDX, IDC_TYPE, m_wndType);
  DDX_Control(pDX, IDC_RECOMMENDATIONS, m_wndRecommendations);
  DDX_Control(pDX, IDC_RECOMMENDATIONS2, m_wndRecommendations2);
  DDX_Control(pDX, IDC_INDICATIONS, m_wndIndications);
  DDX_Control(pDX, IDC_RPTMISC_EXAMQUALITY, m_wndExamQuality);
  DDX_Control(pDX, IDC_AGE, m_wndAge);
  DDX_Control(pDX, IDC_PHYSICIAN, m_wndPhysician);
  DDX_Control(pDX, IDC_NAME, m_wndName);
  DDX_Control(pDX, IDC_IMPRESSION, m_wndImpression);
  DDX_Control(pDX, IDC_ID, m_wndID);
  DDX_Control(pDX, IDC_DATE, m_wndDate);
  DDX_Control(pDX, IDC_SEX, m_wndSex);
  DDX_Control(pDX, IDC_YEARS, m_wndYears);
  DDX_Control(pDX, IDC_RPT_COMBO_CRADSCAT, m_wndComboCRADS);

  DDX_Control(pDX, IDC_AGE2, m_wndAge2);
  DDX_Control(pDX, IDC_PHYSICIAN2, m_wndPhysician2);
  DDX_Control(pDX, IDC_NAME2, m_wndName2);
  DDX_Control(pDX, IDC_ID2, m_wndID2);
  DDX_Control(pDX, IDC_DATE2, m_wndDate2);
  DDX_Control(pDX, IDC_SEX2, m_wndSex2);
  //}}AFX_DATA_MAP
} // DoDataExchange()


/**
 * message map
 */
BEGIN_MESSAGE_MAP(ReportMiscellaneous, ReportWndBase)
  //{{AFX_MSG_MAP(ReportMiscellaneous)
  ON_CBN_SELCHANGE(IDC_RPT_COMBO_CRADSCAT, OnSelChangeCRADSCategory)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * WM_INITDIALOG handler
 *
 * @return non-zero to set focus to the first child control
 */
BOOL ReportMiscellaneous::OnInitDialog() 
{
  ReportWndBase::OnInitDialog();

  m_wndYears.SetDigits(0);
  m_wndYears.SetRange(1, 99);
  m_wndYears.SetValue(5);
  m_wndYears.SetColor(m_clrBack);

  m_wndAge.SetDigits(0);
  m_wndAge.SetRange(0, 10000);
  m_wndAge.SetFont(&m_fontNormal);

  m_wndDate.SetMonthCalColor(MCSC_BACKGROUND, m_clrBack);
  m_wndDate.SetMonthCalColor(MCSC_MONTHBK, m_clrBack); 
  m_wndDate.SetMonthCalColor(MCSC_TRAILINGTEXT, RGB(0,0,255)); 
 
  m_wndDate.SetMonthCalFont(m_fontNormal);

  m_wndIndications.SetBackgroundImage(GetChildBackground(m_wndIndications.GetDlgCtrlID()));
  m_wndIndications.SetSelectedBold(FALSE);
  m_wndIndications.SetCheckboxes(TRUE);
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SCREENING),
    static_cast<DWORD>(ReportLib::Screening));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_PERSONAL_HISTORY_OF_POLYPS),
    static_cast<DWORD>(ReportLib::PersonalHistoryOfPolyps));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_PERSONAL_HISTORY_OF_COLONCANCER),
    static_cast<DWORD>(ReportLib::PersonalHistoryOfCancer));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_FAMILY_HISTORY_OF_POLYPS),
    static_cast<DWORD>(ReportLib::FamilyHistoryOfPolyps));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_FAMILY_HISTORY_OF_CANCER),
    static_cast<DWORD>(ReportLib::FamilyHistoryOfCancer));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_CONSTIPATION),
    static_cast<DWORD>(ReportLib::Constipation));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ANEMIA),
    static_cast<DWORD>(ReportLib::Anemia));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_HEMATOCHEZIA),
    static_cast<DWORD>(ReportLib::Hematochezia));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ABNORMAL_IMAGING),
    static_cast<DWORD>(ReportLib::AbnormalImaging));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ABDOMINAL_PAIN),
    static_cast<DWORD>(ReportLib::AbdominalPain));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_IBD),
    static_cast<DWORD>(ReportLib::IBD));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_DIARRHEA),
    static_cast<DWORD>(ReportLib::Diarrhea));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_FAILED_COLONOSCOPY),
    static_cast<DWORD>(ReportLib::FailedColonoscopy));
  m_wndIndications.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_HIGH_SEDATION_RISK),
    static_cast<DWORD>(ReportLib::HighSedationRisk));

  m_wndExamQuality.SetBackgroundImage(GetChildBackground(m_wndExamQuality.GetDlgCtrlID()));
  m_wndExamQuality.SetSelectedBold(FALSE);
  m_wndExamQuality.SetCheckboxes(TRUE);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_FECAL_TAGGING), ReportLib::FecalTagging);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_CO2), ReportLib::CO2);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ROOMAIR), ReportLib::RoomAir);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_BARIUM), ReportLib::Barium);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_IODINE), ReportLib::Iodine);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_GLUCAGON), ReportLib::Glucagon);
  m_wndExamQuality.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_BUSCOPAN), ReportLib::Buscopan);


  m_wndRecommendations.SetBackgroundImage(GetChildBackground(m_wndRecommendations.GetDlgCtrlID()));
  m_wndRecommendations.SetSelectedBold(FALSE);
  m_wndRecommendations.SetCheckboxes(TRUE);
  m_wndRecommendations.InsertGroupItem(ReportGroupCtrl::NO_GROUP, "", static_cast<DWORD>(ReportLib::FollowUp));

  m_wndRecommendations2.SetBackgroundImage(GetChildBackground(m_wndRecommendations2.GetDlgCtrlID()));
  m_wndRecommendations2.SetSelectedBold(FALSE);
  m_wndRecommendations2.SetCheckboxes(TRUE);
  m_wndRecommendations2.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SCHEDULE_FOR_POLYP_REMOVAL),
    static_cast<DWORD>(ReportLib::Removal));
  
  m_wndType.InsertString(0, LoadResourceString(IDS_VIRTUAL_COLONOSCOPY_IN));
  m_wndType.SetItemData(0, ReportLib::Virtual);
  m_wndType.InsertString(1, LoadResourceString(IDS_OPTICAL_COLONOSCOPY_IN));
  m_wndType.SetItemData(1, ReportLib::Optical);
  m_wndType.SetCurSel(0);
  
  m_wndUnit.InsertString(0, LoadResourceString(IDS_YEAR));
  m_wndUnit.SetItemData(0, ReportLib::Years);
  m_wndUnit.InsertString(1, LoadResourceString(IDS_MONTH));
  m_wndUnit.SetItemData(1, ReportLib::Months);
  m_wndUnit.SetCurSel(0);
	
  m_wndSex.InsertString(0, LoadResourceString(IDS_MALE));     m_wndSex2.InsertString(0, LoadResourceString(IDS_MALE));
  m_wndSex.SetItemData(0, ReportLib::Male);                   m_wndSex2.SetItemData(0, ReportLib::Male);
  m_wndSex.InsertString(1, LoadResourceString(IDS_FEMALE));   m_wndSex2.InsertString(1, LoadResourceString(IDS_FEMALE));
  m_wndSex.SetItemData(1, ReportLib::Female);                 m_wndSex2.SetItemData(1, ReportLib::Female);
  m_wndSex.SetCurSel(0);                                      m_wndSex2.SetCurSel(-1);

  // Colonic and Xtra-Colonic C-RADS
  DWORD iCat(0);
  for (iCat = 0; iCat <= 4; iCat++)
  {
    m_wndComboCRADS.AddString(("C"+ToAscii(iCat)).c_str());
  }

  m_staticCRADS.SubclassDlgItem(IDC_RPTMISC_CRDAS_LINK_STATIC, this);
  m_staticXCRADS.SubclassDlgItem(IDC_RPTMISC_XCRDAS_LINK_STATIC, this);
  m_staticCRADS.m_sTarget = LoadResourceString(IDS_CRADS_WEBLINK);
  m_staticXCRADS.m_sTarget = LoadResourceString(IDS_CRADS_WEBLINK);

  return TRUE; 
} // OnInitDialog()


/**
 * when the CRADS combo box selection changes
 **/
void ReportMiscellaneous::OnSelChangeCRADSCategory()
{
  SetDlgItemText(IDC_RPTMISC_CRADS_VERBOSE, LoadResourceString(IDS_CRADS_VERBOSE_C0 + m_wndComboCRADS.GetCurSel()));
} // OnSelChangeCRADSCategory()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ReportMiscellaneous::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
} // OnOK()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ReportMiscellaneous::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * set report data
 * @param spRevision   report info
 **/
void ReportMiscellaneous::SetReport(ReportLib::IReportRevisionPtr& spRevision)
{
  ReportWndBase::SetReport(spRevision);

  try
  {
    ReportLib::IReportInfoPtr spInfo = spRevision->Info;

    std::string sPatientName(spInfo->PatientName.length()>0? spInfo->PatientName : _bstr_t());
    int4 iDelimPos(-1);
    if ( (iDelimPos = sPatientName.find(ReportMiscellaneous_m_sDualNameDelimeter)) == std::string::npos )
    {
      if (rdrGlobal.m_supine.m_studyinfo.m_sPatientID != rdrGlobal.m_prone.m_studyinfo.m_sPatientID)
      {
        // put supine patient in 1 and prone patient in 2, or put sPatientName in 1 and prone patient name in 2
        m_wndName .SetWindowText((true?sPatientName:rdrGlobal.m_supine.m_studyinfo.m_sPatientName).c_str());
        m_wndName2.SetWindowText(rdrGlobal.m_prone.m_studyinfo.m_sPatientName.c_str());

        m_wndID2.SetWindowText(rdrGlobal.m_prone.m_studyinfo.m_sPatientID.c_str());
        m_wndPhysician2.SetWindowText(rdrGlobal.m_prone.m_studyinfo.m_sPhysicianName.c_str());
        m_wndDate2.SetTime(rdrGlobal.m_prone.m_studyinfo.m_dateExam);

        if ( rdrGlobal.m_prone.m_studyinfo.m_sPatientData.length() >= 3 )
        {
          SelectComboItem(m_wndSex2, tolower(rdrGlobal.m_prone.m_studyinfo.m_sPatientData[0])=='m'?0:1);
          m_wndAge2.SetWindowText(&rdrGlobal.m_prone.m_studyinfo.m_sPatientData[1]);
        }
      }
      else
      {
        // put sPatientName in 1 and TODO: hide/disable 2
        m_wndName .SetWindowText(sPatientName.c_str());
        m_wndName2.EnableWindow(FALSE);
        m_wndAge2.EnableWindow(FALSE);
        m_wndDate2.SetTime(rdrGlobal.m_supine.m_studyinfo.m_dateExam);
        //m_wndDate2.EnableWindow(FALSE);
        m_wndID2.EnableWindow(FALSE);
        m_wndPhysician2.EnableWindow(FALSE);
        m_wndSex2.EnableWindow(FALSE);
      }
    }
    else
    {
      m_wndName .SetWindowTextA(sPatientName.substr(0, iDelimPos).c_str());
      m_wndName2.SetWindowTextA(sPatientName.substr(iDelimPos + ReportMiscellaneous_m_sDualNameDelimeter.length()).c_str());

      m_wndID2.SetWindowText(rdrGlobal.m_prone.m_studyinfo.m_sPatientID.c_str());
      m_wndPhysician2.SetWindowText(rdrGlobal.m_prone.m_studyinfo.m_sPhysicianName.c_str());
      m_wndDate2.SetTime(rdrGlobal.m_prone.m_studyinfo.m_dateExam);

      if ( rdrGlobal.m_prone.m_studyinfo.m_sPatientData.length() >= 3 )
      {
        SelectComboItem(m_wndSex2, tolower(rdrGlobal.m_prone.m_studyinfo.m_sPatientData[0])=='m'?0:1);
        m_wndAge2.SetWindowText(&rdrGlobal.m_prone.m_studyinfo.m_sPatientData[1]);
      }
    }

    if ( !rdrGlobal.m_bDualDataLoaded )
    {
      std::string sss("");
      m_wndName2.SetWindowTextA(sss.c_str());

      m_wndID2.SetWindowText(sss.c_str());
      m_wndPhysician2.SetWindowText(sss.c_str());
      m_wndDate2.SetTime(rdrGlobal.m_supine.m_studyinfo.m_dateExam);

      //m_wndName2.EnableWindow(FALSE);
      //m_wndAge2.EnableWindow(FALSE);
      //m_wndDate2.EnableWindow(FALSE);
      //m_wndID2.EnableWindow(FALSE);
      //m_wndPhysician2.EnableWindow(FALSE);
      //m_wndSex2.EnableWindow(FALSE);
    }

    // KLUDGE: Strip out Examination Quality text and set checkboxes accordingly
    CString sImpression = spInfo->Impression.length()>0 ? static_cast<LPCTSTR>(spInfo->Impression) : CString();
    int4 iCharImp = sImpression.Find(m_sHiddenTextDelimiter);
    if (iCharImp >= 0) sImpression = sImpression.Left(iCharImp);
    m_wndImpression.SetWindowText(sImpression);

    m_wndID.SetWindowText(spInfo->PatientID.length()>0?spInfo->PatientID:_bstr_t());
    m_wndPhysician.SetWindowText(spInfo->ReferringDoctor.length()>0?spInfo->ReferringDoctor:_bstr_t());
    m_wndAge.SetValue(atof(spInfo->PatientAge.length()>0?spInfo->PatientAge:"0"));
    m_wndDate.SetTime(spInfo->StudyDate);

    long nIndications = spInfo->Indication;
    for ( int4 iInd(1); iInd <= int4(ReportLib::IndicationsEnumLast); iInd *= 2 )
    {
      m_wndIndications.SetCheck(iInd, nIndications&iInd);
    }

    try
    {
      long nExamQualityParams = spInfo->ExamQuality;
      for ( int4 iEQ(1); iEQ <= int4(ReportLib::ExamQualityEnumLast); iEQ *= 2 )
      {
        m_wndExamQuality.SetCheck(iEQ, nExamQualityParams&iEQ);
      }
    }
    catch(...)
    {
      LogFyi("An earlier report revision, does not have examination quality parameters.  Continuing.", "ReportMiscellaneous", "SetReport");
    }

    long nRecommendations = spInfo->Recommendation;
    m_wndRecommendations.SetCheck(ReportLib::FollowUp, nRecommendations&ReportLib::FollowUp);
    m_wndRecommendations2.SetCheck(ReportLib::Removal, nRecommendations&ReportLib::Removal);

    m_wndYears.SetValue(spInfo->RecommendationTime > 0? spInfo->RecommendationTime : 5);

    SetDlgItemText(IDC_INDICATION_OTHER, static_cast<LPCTSTR>(spInfo->OtherIndication.length()>0?spInfo->OtherIndication:_bstr_t()));

    CString sOtherRecmdtn = spInfo->OtherRecommendation.length()>0 ? static_cast<LPCTSTR>(spInfo->OtherRecommendation) : CString();
    // KLUDGE: Strip out C-RADS and XC-C-RADS
    RemoveLegacyCRADSCategory( sOtherRecmdtn );

    SetDlgItemText(IDC_RECOMMEND_OTHER, sOtherRecmdtn);

    SetDlgItemText(IDC_AUTHOR, static_cast<LPCTSTR>(spInfo->Author.length()>0?spInfo->Author:_bstr_t()));

    SelectComboItem(m_wndType, spInfo->RecommendationType);
    SelectComboItem(m_wndUnit, spInfo->RecommendationUnit);
    SelectComboItem(m_wndSex, spInfo->PatientSex);

    m_wndComboCRADS.SetCurSel(spInfo->CRADSCategory);
    SetDlgItemText(IDC_RPTMISC_CRADS_VERBOSE, LoadResourceString(IDS_CRADS_VERBOSE_C0 + (spInfo->CRADSCategory > 0 ? spInfo->CRADSCategory : 1) ));

    long lCat(GetExtraColonicCRADSCat( spRevision ));      
    if ( lCat < 0 ) lCat = 1;
    SetDlgItemText(IDC_RPT_OTHER_CRADS_CAT, ("E"+ToAscii(lCat)).c_str());
    SetDlgItemText(IDC_RPTMISC_ERADS_VERBOSE, LoadResourceString(IDS_CRADS_VERBOSE_E0 + lCat));
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_STUDY_INFORMATION, e.Error());
  }
} // SetReport


/**
 * called when view opened
 **/
void ReportMiscellaneous::OnReportViewActivate()
{
  try
  {
    if (m_spRevision == NULL || m_spRevision->IsOpen == VARIANT_FALSE || m_spRevision->IsReadOnly == VARIANT_TRUE)
      return;

    const long lCat(GetExtraColonicCRADSCat( m_spRevision ));
    SetDlgItemText(IDC_RPT_OTHER_CRADS_CAT, ("E"+ToAscii((lCat >= 0) ? lCat : 1)).c_str());
    SetDlgItemText(IDC_RPTMISC_ERADS_VERBOSE, LoadResourceString(IDS_CRADS_VERBOSE_E0 + lCat));
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_STUDY_INFORMATION, e.Error());
  }
} // OnReportViewActivate()


/**
 * called when view closed
 **/
void ReportMiscellaneous::OnReportViewUnactivate()
{
  try
  {
    if( m_spRevision == NULL || m_spRevision->IsOpen == VARIANT_FALSE || m_spRevision->IsReadOnly == VARIANT_TRUE)
      return;

    ReportLib::IReportInfoPtr spInfo = m_spRevision->Info;
    
    char szText[1024*2];
    
    m_wndName.GetWindowText(szText, 255);
    CString scsText;
    m_wndName2.GetWindowText(scsText);
    if ( !scsText.IsEmpty() )
    {
      spInfo->PatientName = (szText + ReportMiscellaneous_m_sDualNameDelimeter + LPCTSTR(scsText)).c_str();
    }
    else
      spInfo->PatientName = szText;

    m_wndImpression.GetWindowText(szText, 1024*2-1);
    spInfo->Impression = szText;

    m_wndID.GetWindowText(szText, 255);
    spInfo->PatientID = szText;

    m_wndPhysician.GetWindowText(szText, 255);
    spInfo->ReferringDoctor = szText;
   
    m_wndAge.GetWindowText(szText, 255);
    spInfo->PatientAge = szText;

    COleDateTime dt;
    m_wndDate.GetTime(dt);
    spInfo->StudyDate = dt;
    long nIndications = 0;
    for ( int4 iInd(1); iInd <= int4(ReportLib::IndicationsEnumLast); iInd *= 2 )
    {
      nIndications |= m_wndIndications.GetCheck(iInd)? iInd : 0;
    }
    spInfo->Indication = nIndications;


    long nExamQualityParams = 0;
    CString sExamQ;
    for ( int4 iEQ(1); iEQ <= int4(ReportLib::ExamQualityEnumLast); iEQ *= 2 )
    {
      if ( m_wndExamQuality.GetCheck(iEQ) )
      {
        nExamQualityParams |= iEQ;
        sExamQ += m_wndExamQuality.GetItemText(m_wndExamQuality.FindItem(iEQ), 0) + ", ";
      }
    }
    spInfo->ExamQuality = nExamQualityParams;

    // KLUDGE: Check Examination Quality Checkboxes and set text in impression accordingly
    if (!sExamQ.IsEmpty())
      spInfo->Impression += LPCTSTR(m_sHiddenTextDelimiter + LoadResourceString(IDS_EXAMINATION_QUALITY) + ": " + sExamQ.Left(sExamQ.GetLength()-2));


    long nRecommendations =  0;
    nRecommendations |= m_wndRecommendations.GetCheck(ReportLib::FollowUp)? ReportLib::FollowUp : 0;
    nRecommendations |= m_wndRecommendations2.GetCheck(ReportLib::Removal)? ReportLib::Removal : 0;
    spInfo->Recommendation = nRecommendations;    
    spInfo->RecommendationType = static_cast<ReportLib::RecommendationTypeEnum>(m_wndType.GetItemData(m_wndType.GetCurSel()));
    spInfo->RecommendationUnit = static_cast<ReportLib::RecommendationUnitEnum>(m_wndUnit.GetItemData(m_wndUnit.GetCurSel()));
    spInfo->PatientSex = static_cast<ReportLib::PatientSexEnum>(m_wndSex.GetItemData(m_wndSex.GetCurSel()));
    spInfo->RecommendationTime = static_cast<long>(m_wndYears.GetValue());
    spInfo->CRADSCategory = m_wndComboCRADS.GetCurSel();

    CString s;
    GetDlgItemText(IDC_INDICATION_OTHER, s);
    spInfo->OtherIndication = static_cast<LPCTSTR>(s);    

    GetDlgItemText(IDC_RECOMMEND_OTHER, s);
    spInfo->OtherRecommendation = static_cast<LPCTSTR>(s);

    GetDlgItemText(IDC_AUTHOR, s);
    spInfo->Author = static_cast<LPCTSTR>(s);
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_STUDY_INFORMATION, e.Error());
  }
} // OnReportViewUnactivate()


/**
 * update the E# text corresponding to the most relevant extra-colonic finding's C-RADS classification
 * @param spRevision   report to update
 */
void ReportMiscellaneous::UpdateKludgedExtraColonicCRadsText(ReportLib::IReportRevisionPtr& spRevision)
{ 
  ReportLib::IReportInfoPtr spInfo = spRevision->Info;

  // find and remove any current extra-colonic text
  CString sOtherRecmdtn = spInfo->OtherRecommendation.length()>0 ? static_cast<LPCTSTR>(spInfo->OtherRecommendation) : CString();
  int4 iCharPos = sOtherRecmdtn.Find(m_sHiddenTextDelimiter);
  if (iCharPos >= 0)
  {
    // found the Colonic text, now search for any possibe extra-colonic
    iCharPos = sOtherRecmdtn.Find(m_sHiddenTextDelimiter, iCharPos+1);
    if (iCharPos >= 0)
    {
      sOtherRecmdtn = sOtherRecmdtn.Left(iCharPos);
    }
  }

  // find the highest category among all other findings
  // add extra-colonic text for all valid categories besides E1
  long lCat( GetExtraColonicCRADSCat( spRevision, sOtherRecmdtn ));  
  spInfo->OtherRecommendation = LPCTSTR(sOtherRecmdtn);

} // UpdateKludgedExtraColonicCRadsText(ReportLib::IReportRevisionPtr& spRevision)


/**
 * returns the highest c-rads category among the extra-colonic findings
 * @return   the highest c-rads category among the extra-colonic findings
 */
const long ReportMiscellaneous::GetExtraColonicCRADSCat( ReportLib::IReportRevisionPtr & spRevision, CString & sOutput )
{
  // Set the overall Extra-Colonic CRADS Category to the highest category found
  ReportLib::IReportOtherFindingsPtr spOtherFindings = spRevision->OtherFindings;
  if( spOtherFindings->GetCount() < 1 )
    return(1);

  long lCat(-1);
  for (long lCurr(0), iCount(spOtherFindings->Count); lCurr < iCount; lCurr++)
  {
    if (lCat < spOtherFindings->GetItem(lCurr)->CRADSCategory)
      lCat = spOtherFindings->GetItem(lCurr)->CRADSCategory;
  }

  if( (lCat == 0) || (lCat > 1) )
  {
    sOutput += m_sHiddenTextDelimiter + LoadResourceString(IDS_EXTRA_COLONIC_CRADS) + (ToAscii(lCat) + "|").c_str();
    sOutput += LoadResourceString(IDS_CRADS_VERBOSE_E0 + lCat);
  }

  return lCat;
} // GetExtraColonicCRADSCat()


/**
 * removes legacy crads category from string
 *
 * @param     sMessage                string containing possible legacy crads
 */
void ReportMiscellaneous::RemoveLegacyCRADSCategory( CString & sMessage )
{
  int4 iCharRcm = sMessage.Find(m_sHiddenTextDelimiter);
  if (iCharRcm >= 0) 
    sMessage = sMessage.Left(iCharRcm);
} // RemoveLegacyCRADSCategory( sMessage )

