// $Id: ReportEngine.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEngine.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Root engine object
// Created: 2/1/2001
// Author: George Economos
//
// Revision History:
// $Log: ReportEngine.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:40   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:36   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.5  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
//
// Complete history on bottom of file

#ifndef __REPORTENGINE_H_
#define __REPORTENGINE_H_

#include "resource.h"       // main symbols
#include "ReportHelper.h"

class CoReportEntries;
class CoReportOtherFindings;
class CoReportNonDistendedFindings;
class CoReportInfo;

/////////////////////////////////////////////////////////////////////////////
// CoReportEngine
class ATL_NO_VTABLE CoReportEngine : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CoReportEngine, &CLSID_Report>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReport,&IID_IReport, &LIBID_ReportLib>,
  public ReportHelper
{
public:
	CoReportEngine();

	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_REPORTENGINE)
DECLARE_NOT_AGGREGATABLE(CoReportEngine)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CoReportEngine)
	COM_INTERFACE_ENTRY(IReport)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
	STDMETHOD(get_IsReadOnly)(VARIANT_BOOL *pVal);
  STDMETHOD(get_MinorVersion)(long *pVal);
  STDMETHOD(get_MajorVersion)(long *pVal);
  STDMETHOD(get_OtherFindings)(IReportOtherFindings* *pVal);
  STDMETHOD(get_NonDistendedFindings)(IReportNonDistendedFindings* *pVal);
  STDMETHOD(get_IsOpen)(VARIANT_BOOL *pVal);
  STDMETHOD(get_Entries)(IReportEntries** ppiEntries);
  STDMETHOD(get_Info)(IReportInfo** ppiInfo);
  STDMETHOD(Save)();
	STDMETHOD(Open)(BSTR bsFileName, VARIANT_BOOL vbReadOnly);
	STDMETHOD(Create)(BSTR bsFileName);
  STDMETHOD(Close)();
 

// CoClass
	
	CComPtr<IStorage>& GetRootStorage(){return m_spRootStg;}

	bool IsReadOnly(){return m_vbReadOnly == VARIANT_TRUE;}

protected:

	HRESULT CommonInit(bool bNewReport);

  HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);

	CComPtr<IStorage> m_spRootStg;
	CComPtr<IReportEntries> m_spEntries;
  CComPtr<IReportOtherFindings> m_spOtherFindings;
  CComPtr<IReportNonDistendedFindings> m_spNonDistendedFindings;
	CComPtr<IReportInfo> m_spInfo;
	
	VARIANT_BOOL m_vbReadOnly;
};

#endif

// $Log: ReportEngine.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.5  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEngine.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportEngine.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
