// $Id: ReportInfo.h,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Info object for report

#ifndef ReportInfo_h__
#define ReportInfo_h__


#include "ReportHelper.h"

class ReportRevision;

class ATL_NO_VTABLE ReportInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportInfo, &CLSID_ReportInfo>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportInfo, &IID_IReportInfo, &LIBID_ReportLib>,
  public ReportHelper
{
public:

DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportInfo)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportInfo)
	COM_INTERFACE_ENTRY(IReportInfo)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:

	/// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// gets Impression property  
  STDMETHOD(get_Impression)(/*[out, retval]*/ BSTR *pVal);

  /// sets Impression property  
  STDMETHOD(put_Impression)(/*[in]*/ BSTR newVal);

  /// gets Author property
  STDMETHOD(get_Author)(BSTR* pbsAuthor);

  /// sets Author property
  STDMETHOD(put_Author)(BSTR bsAuthor);

  /// gets Recommendation Time property
  STDMETHOD(get_RecommendationTime)(long* piRecommendationTime);

  /// sets Recommendation Time property
  STDMETHOD(put_RecommendationTime)(long iRecommendationTime);

  /// gets Recommendation Unit property
  STDMETHOD(get_RecommendationUnit)(RecommendationUnitEnum* pnRecommendationUnit);

  /// sets Recommendation Unit property
  STDMETHOD(put_RecommendationUnit)(RecommendationUnitEnum nRecommendationUnit);

  /// gets Recommendation Type property
  STDMETHOD(get_RecommendationType)(RecommendationTypeEnum *pnRecommendationType);

  /// sets Recommendation Type property
  STDMETHOD(put_RecommendationType)(RecommendationTypeEnum nRecommendationType);

  /// gets Recommendation property
  STDMETHOD(get_Recommendation)(long* piRecommendation);

  /// sets Recommendation property
  STDMETHOD(put_Recommendation)(long iRecommendation);

  /// gets Other Recommendation property
  STDMETHOD(get_OtherRecommendation)(BSTR* pbsOtherRecommendation);

  /// sets Other Recommendation property
  STDMETHOD(put_OtherRecommendation)(BSTR bsOtherRecommendation);

  /// gets Indication property
  STDMETHOD(get_Indication)(long* piIndication);

  /// sets Indication property
  STDMETHOD(put_Indication)(long iIndication);

  /// gets Other Indication property
  STDMETHOD(get_OtherIndication)(BSTR* pbsOtherIndication);

  /// sets Other Indication property
  STDMETHOD(put_OtherIndication)(BSTR bsOtherIndication);

  /// gets Referring Doctor property
  STDMETHOD(get_ReferringDoctor)(BSTR* pbsReferringDoctor);

  /// sets Referring Doctor property
  STDMETHOD(put_ReferringDoctor)(BSTR bsReferringDoctor);

  /// gets Endoscopist property
  STDMETHOD(get_Endoscopist)(BSTR* pbsEndoscopist);

  /// sets Endoscopist property
  STDMETHOD(put_Endoscopist)(BSTR bsEndoscopist);

  /// gets Study Date property
  STDMETHOD(get_StudyDate)(DATE *pdtStudyate);

  /// sets Study Date property
  STDMETHOD(put_StudyDate)(DATE dtStudyDate);

  /// gets Patient ID property
  STDMETHOD(get_PatientID)(BSTR* pbsPatientID);

  /// sets Patient ID property
  STDMETHOD(put_PatientID)(BSTR bsPatientID);

  /// gets Patient Name property
  STDMETHOD(get_PatientName)(BSTR* pbsPatientName);

  /// sets Patient Name property
  STDMETHOD(put_PatientName)(BSTR bsPatientName);  

  /// gets Patient Sex property
  STDMETHOD(get_PatientSex)(PatientSexEnum* pnPatientSex);

  /// sets Patient Sex property
  STDMETHOD(put_PatientSex)(PatientSexEnum nPatientSex);

  /// gets Patient Weight property
  STDMETHOD(get_PatientWeight)(BSTR* pbsPatientWeight);

  /// sets Patient Weight property
  STDMETHOD(put_PatientWeight)(BSTR bsPatientWeight);

  /// gets Patient Age property
  STDMETHOD(get_PatientAge)(BSTR* pbsPatientAge);

  /// sets Patient Age property
  STDMETHOD(put_PatientAge)(BSTR bsPatientAge);

  /// gets Patient Birthday property
  STDMETHOD(get_PatientBirthday)(BSTR* pbsPatientBirthday);

  /// sets Patient Birthday property
  STDMETHOD(put_PatientBirthday)(BSTR bsPatientBirthday);

  /// gets Institution Name property
  STDMETHOD(get_InstitutionName)(BSTR* pbsInstitutionName);

  /// sets Institution Name property
  STDMETHOD(put_InstitutionName)(BSTR bsInstitutionName);

  /// gets CRADS Category property
  STDMETHOD(get_CRADSCategory)(long* piCategory);

  /// sets CRADS Category property
  STDMETHOD(put_CRADSCategory)(long iCategory);

  /// gets Extra-Colonic CRADS Category property
  STDMETHOD(get_ExtraColonicCRADSCategory)(long* piCategory);

  /// gets ExamQuality property
  STDMETHOD(get_ExamQuality)(long* piExamQuality);

  /// sets ExamQuality property
  STDMETHOD(put_ExamQuality)(long ExamQuality);

  /// object setup routine
  HRESULT Setup(CComPtr<IReportRevision>& spRevision);

private:

  /// returns the Storage object assoicated with the entry
  virtual HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);
};

#endif

// $Log: ReportInfo.h,v $
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
// Revision 1.10  2001/05/09 15:21:33  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.9  2001/05/08 16:20:03  geconomos
// More coding standards
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportInfo.h,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $
// $Id: ReportInfo.h,v 1.2.2.4 2010/02/05 09:44:03 dongqing Exp $

