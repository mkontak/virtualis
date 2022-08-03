// $Id: Report.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Report.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// Copyright 2001, Viatronix Inc, All Rights Reserved
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos geconomos@viatronix.net
//
// Complete history on bottom of file

#ifndef __REPORT_H_
#define __REPORT_H_

#include "resource.h"

class ATL_NO_VTABLE Report : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Report, &CLSID_Report>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReport, &IID_IReport, &LIBID_ReportLib>
{
public:
	
DECLARE_REGISTRY_RESOURCEID(IDR_REPORT)
DECLARE_NOT_AGGREGATABLE(Report)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(Report)
	COM_INTERFACE_ENTRY(IReport)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

  /// Called when object is fully constructed 
  HRESULT FinalConstruct();

  /// Called when final reference to object has been released
  void FinalRelease();
  
  /// required interface method for ISupportErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// releases all held objects
  STDMETHOD(Close)();
  
  /// gets IReportRevisions interface
  STDMETHOD(get_Revisions)(IReportRevisions** ppiVal);
  
  /// opens a new report file object
  STDMETHOD(Open)(BSTR bsFileName);
  
  /// creates a new report file object
  STDMETHOD(Create)(BSTR bsFileName);

  /// returns the root IStorage interface
  CComPtr<IStorage> GetStorage(){return m_spRootStg;}

  /// return readonly property
  bool GetReadOnly() const {return m_vbReadOnly == VARIANT_TRUE;}
  
protected:

  /// common initialization routines for Report object
  HRESULT CommonInit();

  /// root IStorage interface
  CComPtr<IStorage> m_spRootStg;
  
  /// IReportRevisions collection interface 
  CComPtr<IReportRevisions> m_spRevisions;
  
  /// Read only indictaor
  VARIANT_BOOL m_vbReadOnly;
};

#endif

// Revision History:
// $Log: Report.h,v $
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
//    Rev 2.0   Sep 16 2001 23:40:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:32   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:37  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.3  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.2  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Report.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: Report.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
