// $Id: ReportInfo.C,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Info object for report

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportInfo.h"
#include "ReportRevision.h"


STDMETHODIMP ReportInfo::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportInfo
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/**
 * gets the Patient Name property
 *
 * @param   pbsPatientName      contains Patient Name upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientName(BSTR* pbsPatientName)
{
  return ReadStringProperty(INFO_PROPERTY_PATIENT_NAME, pbsPatientName);
}

/**
 * sets the Patient Name property
 *
 * @param   bsPatientName       Patient Name value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientName(BSTR bsPatientName)
{
  return WriteStringProperty(INFO_PROPERTY_PATIENT_NAME, bsPatientName);
}

/**
 * gets the Patient ID property
 *
 * @param   pbsPatientID        contains Patient ID upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientID(BSTR* pbsPatientID)
{
 	return ReadStringProperty(INFO_PROPERTY_PATIENT_ID, pbsPatientID);
}

/**
 * sets the Patient ID property
 *
 * @param   bsPatientID         Patient ID value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientID(BSTR bsPatientID)
{
  return WriteStringProperty(INFO_PROPERTY_PATIENT_ID, bsPatientID);
}

/**
 * gets the Study Date property
 *
 * @param   pdtStudyDate        contains Study Date upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_StudyDate(DATE* pdtStudyDate)
{
	return ReadProperty(INFO_PROPERTY_STUDY_DATE, pdtStudyDate, sizeof(DATE));
}

/**
 * sets the Study Date property
 *
 * @param   dtStudyDate         new Study Date value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_StudyDate(DATE dtStudyDate)
{
  return WriteProperty(INFO_PROPERTY_STUDY_DATE, &dtStudyDate, sizeof(DATE));
}

/**
 * gets the Endoscopist property
 *
 * @param   pbsEndoscopist      contains Endoscopist upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_Endoscopist(BSTR* pbsEndoscopist)
{
  return ReadStringProperty(INFO_PROPERTY_ENDOSCOPIST, pbsEndoscopist);
}

/**
 * sets the Endoscopist property
 *
 * @param   bsEndoscopist       new Endoscopist value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_Endoscopist(BSTR bsEndoscopist)
{
  return WriteStringProperty(INFO_PROPERTY_ENDOSCOPIST, bsEndoscopist);
}

/**
 * gets the Patient Birthday property
 *
 * @param   pbsPatientBirthday  contains Patient Birthday upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientBirthday(BSTR* pbsPatientBirthday)
{
  return ReadStringProperty(INFO_PROPERTY_PATIENT_BIRTHDAY, pbsPatientBirthday);
}

/**
 * sets the Patient Birthday property
 *
 * @param   pbsPatientBirthday  new Patient Birthday value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientBirthday(BSTR newVal)
{
  return WriteStringProperty(INFO_PROPERTY_PATIENT_BIRTHDAY, newVal);
}

/**
 * gets the Patient Age property
 *
 * @param   pbsPatientAge       contains Patient Age upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientAge(BSTR* pbsPatientAge)
{
  return ReadStringProperty(INFO_PROPERTY_PATIENT_AGE, pbsPatientAge);
}

/**
 * sets the Patient Age property
 *
 * @param   bsPatientAge        new Patient Age value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientAge(BSTR bsPatientAge)
{
  return WriteStringProperty(INFO_PROPERTY_PATIENT_AGE, bsPatientAge);
}

/**
 * gets the Patient Weight property
 *
 * @param   pbsPatientWeight      contains Patient Weight upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientWeight(BSTR* pbsPatientWeight)
{
  return ReadStringProperty(INFO_PROPERTY_PATIENT_WEIGHT, pbsPatientWeight);
}

/**
 * sets the Patient Weight property
 *
 * @param   bsPatientWeight     new Patient Weight value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientWeight(BSTR bsPatientWeight)
{
  return WriteStringProperty(INFO_PROPERTY_PATIENT_WEIGHT, bsPatientWeight);
}

/**
 * gets the Patient Sex property
 *
 * @param   pnPatientSex          contains Patient Sex upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_PatientSex(PatientSexEnum* pnPatientSex)
{
  return ReadProperty(INFO_PROPERTY_PATIENT_SEX, pnPatientSex, sizeof(PatientSexEnum));
}

/**
 * sets the Patient Sex property
 *
 * @param   nPatientSex           new Patient Sex value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_PatientSex(PatientSexEnum nPatientSex)
{
  return WriteProperty(INFO_PROPERTY_PATIENT_SEX, &nPatientSex, sizeof(PatientSexEnum));
}

/**
 * gets the Institution Name property
 *
 * @param   pbsInstitutionName    contains Institution Name upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_InstitutionName(BSTR* pbsInstitutionName)
{
  return ReadStringProperty(INFO_PROPERTY_INSTITUTION_NAME, pbsInstitutionName);
}

/**
 * sets the Institution Name property
 *
 * @param   bsInstitutionName     new Institution Name value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_InstitutionName(BSTR bsInstitutionName)
{
  return WriteStringProperty(INFO_PROPERTY_INSTITUTION_NAME, bsInstitutionName);
}

/**
 * gets the Referring Doctor property
 *
 * @param   pbsReferringDoctor    contains Referring Doctor upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_ReferringDoctor(BSTR* pbsReferringDoctor)
{
  return ReadStringProperty(INFO_PROPERTY_REFERRING, pbsReferringDoctor);
}

/**
 * sets the Referring Doctor property
 *
 * @param   bsReferringDoctor     new Referring Doctor value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_ReferringDoctor(BSTR bsReferringDoctor)
{
  return WriteStringProperty(INFO_PROPERTY_REFERRING, bsReferringDoctor);
}

/**
 * gets the Indication property
 *
 * @param   piIndication          contains Indication upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_Indication(long* piIndication)
{
  return ReadProperty(INFO_PROPERTY_INDICATION, piIndication, sizeof(long));
}

/**
 * sets the Indication property
 *
 * @param   iIndication           new Indication value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_Indication(long iIndication)
{
  return WriteProperty(INFO_PROPERTY_INDICATION, &iIndication, sizeof(long));
}

/**
 * gets the Recommendation property
 *
 * @param   piRecommendation      contains Recommendation upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_Recommendation(long* piRecommendation)
{
  return ReadProperty(INFO_PROPERTY_RECOMMENDATION, piRecommendation, sizeof(long));
}

/**
 * sets the Recommendation property
 *
 * @param   iRecommendation       new Recommendation value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_Recommendation(long iRecommendation)
{
  return WriteProperty(INFO_PROPERTY_RECOMMENDATION, &iRecommendation, sizeof(long));
}

/**
 * gets the Recommendation Type property
 *
 * @param   piRecommendationType  contains Recommendation Type upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_RecommendationType(RecommendationTypeEnum* piRecommendationType)
{
  return ReadProperty(INFO_PROPERTY_RECOMMENDATION_TYPE, piRecommendationType, sizeof(RecommendationTypeEnum));
}

/**
 * sets the Recommendation Type property
 *
 * @param   iRecommendationType   new Recommendation Type value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_RecommendationType(RecommendationTypeEnum iRecommendationType)
{
  return WriteProperty(INFO_PROPERTY_RECOMMENDATION_TYPE, &iRecommendationType, sizeof(RecommendationTypeEnum));
}

/**
 * gets the Recommendation Unit property
 *
 * @param   piRecommendationUnit  contains Recommendation Unit upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_RecommendationUnit(RecommendationUnitEnum* piRecommendationUnit)
{
  return ReadProperty(INFO_PROPERTY_RECOMMENDATION_UNIT, piRecommendationUnit, sizeof(RecommendationUnitEnum));
}

/**
 * sets the Recommendation Unit property
 *
 * @param   iRecommendationUnit   new Recommendation Unit value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_RecommendationUnit(RecommendationUnitEnum iRecommendationUnit)
{
  return WriteProperty(INFO_PROPERTY_RECOMMENDATION_UNIT, &iRecommendationUnit, sizeof(RecommendationTypeEnum));
}

/**
 * gets the Recommendation Time property
 *
 * @param   piRecommendationTime  contains Recommendation Time upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_RecommendationTime(long* piRecommendationTime)
{
  return ReadProperty(INFO_PROPERTY_RECOMMENDATION_TIME, piRecommendationTime, sizeof(long));
}

/**
 * sets the Recommendation Time property
 *
 * @param   iRecommendationTime   new Recommendation Time value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_RecommendationTime(long iRecommendationTime)
{
  return WriteProperty(INFO_PROPERTY_RECOMMENDATION_TIME, &iRecommendationTime, sizeof(long));
}

/**
 * gets the Other Indication property
 *
 * @param   pbsOtherIndication    contains Other Indication upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_OtherIndication(BSTR* pbsOtherIndication)
{
  return ReadStringProperty(INFO_PROPERTY_INDICATION_OTHER, pbsOtherIndication);
}

/**
 * sets the Other Indication property
 *
 * @param   bsOtherIndication     new Other Indication value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_OtherIndication(BSTR bsOtherIndication)
{
  return WriteStringProperty(INFO_PROPERTY_INDICATION_OTHER, bsOtherIndication);
}

/**
 * gets the Other Recommendation property
 *
 * @param   pbsOtherRecommendation    contains Other Recommendation upon return
 * @return                            S_OK upon success
 */
STDMETHODIMP ReportInfo::get_OtherRecommendation(BSTR* pbsOtherRecommendation)
{
  return ReadStringProperty(INFO_PROPERTY_RECOMMENDATION_OTHER, pbsOtherRecommendation);
}

/**
 * sets the Other Recommendation property
 *
 * @param   bsOtherRecommendation new Other Recommendation value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_OtherRecommendation(BSTR bsOtherRecommendation)
{
  return WriteStringProperty(INFO_PROPERTY_RECOMMENDATION_OTHER, bsOtherRecommendation);
}

/**
 * gets the Author property
 *
 * @param   pbsAuthor   contains Author upon return
 * @return               S_OK upon success
 */
STDMETHODIMP ReportInfo::get_Author(BSTR* pbsAuthor)
{
  return ReadStringProperty(INFO_PROPERTY_AUTHOR, pbsAuthor);
}

/**
 * sets the Author property
 *
 * @param   bsAuthor     new Author value
 * @return               S_OK upon success
 */
STDMETHODIMP ReportInfo::put_Author(BSTR bsAuthor)
{
  return WriteStringProperty(INFO_PROPERTY_AUTHOR, bsAuthor);
}

/**
 * gets the Impression property
 *
 * @param   pbsImpression   contains Impression upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportInfo::get_Impression(BSTR *pbsImpression)
{
  return ReadStringProperty(INFO_PROPERTY_IMPRESSION, pbsImpression);
}

/**
 * sets the Impression property
 *
 * @param   bsImpression   new Impression value
 * @return                 S_OK upon success
 */
STDMETHODIMP ReportInfo::put_Impression(BSTR bsImpression)
{
  return WriteStringProperty(INFO_PROPERTY_IMPRESSION, bsImpression);
}


/**
 * gets the CRADS Category property
 *
 * @param   piCategory   contains CRADS Category upon return
 * @return               S_OK upon success
 */
STDMETHODIMP ReportInfo::get_CRADSCategory(long* piCategory)
{
  return ReadProperty(INFO_PROPERTY_CRADS_CATEGORY, piCategory, sizeof(long));
}

/**
 * sets the CRADS Category property
 *
 * @param   iCategory   new CRADS Category value
 * @return              S_OK upon success
 */
STDMETHODIMP ReportInfo::put_CRADSCategory(long iCategory)
{
  return WriteProperty(INFO_PROPERTY_CRADS_CATEGORY, &iCategory, sizeof(long));
}


/**
 * gets the Extra-Colonic CRADS Category property
 *
 * @param   piCategory   contains Extra-Colonic CRADS Category upon return
 * @return               S_OK upon success
 */
STDMETHODIMP ReportInfo::get_ExtraColonicCRADSCategory(long* piCategory)
{
  return 1;
} // get_ExtraColonicCRADSCategory


/**
 * gets the ExamQuality property
 *
 * @param   piExamQuality         contains ExamQuality upon return
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::get_ExamQuality(long* piExamQuality)
{
  return ReadProperty(INFO_PROPERTY_EXAM_QUALITY, piExamQuality, sizeof(long));
} // get_ExamQuality()

/**
 * sets the Indication property
 *
 * @param   iIndication           new Indication value
 * @return                        S_OK upon success
 */
STDMETHODIMP ReportInfo::put_ExamQuality(long iExamQuality)
{
  return WriteProperty(INFO_PROPERTY_EXAM_QUALITY, &iExamQuality, sizeof(long));
} // put_ExamQuality()


/**
 * object setup routine
 *
 * @param   spRevision      smart pointer to parent revision object
 * @return                  S_OK upon success
 */
HRESULT ReportInfo::Setup(CComPtr<IReportRevision>& spRevision)
{
  m_spRevision = spRevision;
  
  return S_OK;
}

/**
 * returns the Storage object assoicated with the entry
 *
 * @param   spStg           contains Storage smart pointer upon return
 * @return                  S_OK upon success
 */
HRESULT ReportInfo::GetEntryStorage(CComPtr<IStorage>& spStg)
{
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision == NULL)
    return E_POINTER;
  
  /// try to open storage with desired access
  HRESULT hr;
  hr = OpenStorage(pRevision->GetStorage(), INFO_STORAGE_NAME, spStg, pRevision->GetReadOnly());
  if(SUCCEEDED(hr))
    return S_OK;
  
  /// if storage was not found, create it
  if(FAILED(hr) && hr == STG_E_FILENOTFOUND)
    hr = CreateStorage(pRevision->GetStorage(), INFO_STORAGE_NAME,spStg);
  
  return hr;
}


// $Log: ReportInfo.C,v $
// Revision 1.2.2.4  2010/02/05 09:44:03  dongqing
// C-RADS and Exam Quality
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2.2.3  2008/03/15 01:12:35  jmeade
// moving console and vc report closer in line.
//
// Revision 1.2.2.2  2008/03/12 21:52:05  jmeade
// Issue 6028: Don't store overall extra-colonic c-rads category; that is begging for an out-of-date value.  Compute when needed.
//
// Revision 1.2.2.1  2007/04/27 02:19:34  jmeade
// Issue 5608: one overall extra-colonic c-rads category in the miscellaneous view.
//
// Revision 1.2  2007/02/09 03:59:58  jmeade
// Issue 5219: Implementing C-RADS categories.
//
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:56  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:40   ingmar
// Initial revision.
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.11  2001/05/09 15:21:33  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.10  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.9  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.8  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.7  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.6  2001/02/01 16:34:33  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportInfo.C,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $
// $Id: ReportInfo.C,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $

