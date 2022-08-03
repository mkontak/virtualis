// $Id: ReportRevisions.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevisions.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Report Revisions Collection
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#ifndef ReportRevisions_h__
#define ReportRevisions_h__

#include "ReportCollection.h"
#include "ReportRevision.h"
#include "Report.h"

/////////////////////////////////////////////////////////////////////////////
// ReportRevisions
class ATL_NO_VTABLE ReportRevisions : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportRevisions, &CLSID_ReportRevisions>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportRevisions, &IID_IReportRevisions, &LIBID_ReportLib>,
  public ReportCollection<Report, IReport, IReportRevision, ReportRevision, &IID_IReportRevision>
{
public:
  
DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportRevisions)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportRevisions)
	COM_INTERFACE_ENTRY(IReportRevisions)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

  /// Called when final reference to object has been released
  void FinalRelease();

  /// required interface method for ISupportErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// returns a Revison object interface by position or ID
  STDMETHOD(get_Item)(VARIANT Index, IReportRevision** ppiRevision);		
  
  /// gets an IEnumXXX interface pointer(used in Visual Basic)
  STDMETHOD(get__NewEnum)(IUnknown** ppiUnknown);		
  
  /// gets number of revisons currently in collection
  STDMETHOD(get_Count)(long *pnCount);
  
  /// removes an object from the collection
  STDMETHOD(Remove)(VARIANT nIndex);
  
  /// adds a new empty revision to the collection and returns the object
  STDMETHOD(Add)(IReportRevision** ppiEntry);
  
  /// setup routine called by parent
  HRESULT Setup(CComPtr<IReport>& spReport, VARIANT_BOOL vbReadOnly);
  
  /// adds an entry to the table of contents
  virtual HRESULT AddEntry( CComBSTR bsName, IReportRevision** ppiReportEntry);

private:
  
  /// Readonly indicator
  VARIANT_BOOL m_vbReadOnly;
};

#endif

// Revision History:
// $Log: ReportRevisions.h,v $
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
//    Rev 2.0   Sep 16 2001 23:40:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:52   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:43  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.3  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.2  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevisions.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportRevisions.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $