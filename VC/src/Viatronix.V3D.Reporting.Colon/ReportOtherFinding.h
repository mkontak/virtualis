// $Id: ReportOtherFinding.h,v 1.2 2007/02/09 00:11:49 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportOtherFinding.h,v 1.2 2007/02/09 00:11:49 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Other Finding object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#ifndef ReportOtherFinding_h__
#define ReportOtherFinding_h__

#include "ReportHelper.h"

class ATL_NO_VTABLE ReportOtherFinding : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportOtherFinding, &CLSID_ReportOtherFinding>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportOtherFinding, &IID_IReportOtherFinding, &LIBID_ReportLib>,
  public ReportHelper
{
public:

DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportOtherFinding)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportOtherFinding)
	COM_INTERFACE_ENTRY(IReportOtherFinding)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:

  /// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	/// gets ID property
  STDMETHOD(get_ID)(BSTR* pbsID);

  /// gets finding property
  STDMETHOD(get_Finding)(OtherFindingsEnum* pnFinding);

  /// sets finding property
  STDMETHOD(put_Finding)(OtherFindingsEnum nFinding);

  /// gets comment property
  STDMETHOD(get_Comment)(BSTR* pbsComment);

  /// sets comment property
  STDMETHOD(put_Comment)(BSTR bsComment);

  /// gets Degree property
  STDMETHOD(get_Degree)(long* piDegree);

  /// sets Degree property
  STDMETHOD(put_Degree)(long iDegree);

  /// gets Length property
  STDMETHOD(get_Length)(float* pfLength);

  /// sets Length property
  STDMETHOD(put_Length)(float fLength);

  /// gets PerpendicularSize property
  STDMETHOD(get_PerpendicularSize)(float* pfSize);

  /// sets PerpendicularSize property
  STDMETHOD(put_PerpendicularSize)(float fSize);

  /// gets MaximumSize property
  STDMETHOD(get_MaximumSize)(float* pfSize);

  /// sets MaximumSize property
  STDMETHOD(put_MaximumSize)(float fSize);

  /// gets Location property
  STDMETHOD(get_Location)(long* piLocation);

  /// sets Location property
  STDMETHOD(put_Location)(long iLocation);

  /// gets Size property
  STDMETHOD(get_Size)(long* pfSize);

  /// sets Size property
  STDMETHOD(put_Size)(long iSize);

  /// gets CRARSCategory property
  STDMETHOD(get_CRADSCategory)(long* iCat);

  /// sets CRARSCategory property
  STDMETHOD(put_CRADSCategory)(long iCat);

  /// object setup routine
  HRESULT Setup(CComPtr<IReportRevision>& spRevision, CComBSTR  bsEntryID);

  /// returns the Storage object associated with the entry
  HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);

protected:

  /// unique identifier for entry
  CComBSTR  m_bsEntryID;
};

#endif

// $Log: ReportOtherFinding.h,v $
// Revision 1.2  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
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
//    Rev 2.0   Sep 16 2001 23:40:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:48   ingmar
// Initial revision.
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:42  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.5  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.4  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.3  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.2  2001/02/01 16:34:33  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportOtherFinding.h,v 1.2 2007/02/09 00:11:49 jmeade Exp $
// $Id: ReportOtherFinding.h,v 1.2 2007/02/09 00:11:49 jmeade Exp $
