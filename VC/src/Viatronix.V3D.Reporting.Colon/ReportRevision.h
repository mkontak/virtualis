// $Id: ReportRevision.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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
// Complete history on bottom of file

#ifndef ReportRevision_h_
#define ReportRevision_h_

#include "ReportHelper.h"

class ReportEntries;
class ReportOtherFindings;
class ReportNonDistendedFindings;
class ReportInfo;

class ATL_NO_VTABLE ReportRevision : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportRevision, &CLSID_ReportRevision>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportRevision, &IID_IReportRevision, &LIBID_ReportLib>,
  public ReportHelper
{
public:

DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportRevision)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportRevision)
	COM_INTERFACE_ENTRY(IReportRevision)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:
  
  /// Called when object is fully constructed
  HRESULT FinalConstruct();
  
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);  
	
  /// returns the minor version of the revision
  STDMETHOD(get_MinorVersion)(long *piMinorVersion);
  
  /// returns the major version of the revision
  STDMETHOD(get_MajorVersion)(long *piMajorVersion);
  
  /// returns the "Colon Findings" collection
  STDMETHOD(get_Entries)(IReportEntries** ppiColon);
  
  /// returns the "Other Findings" collection
  STDMETHOD(get_OtherFindings)(IReportOtherFindings** ppiOther);
  
  /// returns the "Non Distended Findings" collection
  STDMETHOD(get_NonDistendedFindings)(IReportNonDistendedFindings** ppiDistended);
  
  // retuns the "Info" object
  STDMETHOD(get_Info)(IReportInfo** ppiInfo);
 
  /// returns the ReadOnly flag
  STDMETHOD(get_IsReadOnly)(VARIANT_BOOL *pbReadOnly);
  
  /// returns TRUE if the revision is open, FALSE otherwise
  STDMETHOD(get_IsOpen)(VARIANT_BOOL *pbIsOpen);  

  /// returns TRUE if the revision is has been marked completed, FALSE otherwise
  STDMETHOD(get_MarkedCompleted)(VARIANT_BOOL *pbMarkedCompleted);
  
  /// sets the marked completed flag
  STDMETHOD(put_MarkedCompleted)(VARIANT_BOOL bMarkedCompleted);  
  
  /// closes the revision
  STDMETHOD(Close)();
  
  /// saves all changes to the revision file
  STDMETHOD(Save)();
  
  /// returns the storage object 
  CComPtr<IStorage>& GetStorage() {return m_spStg;}

	/// returns the ReadOnly flag(non-property version)
  bool GetReadOnly() const {return m_vbReadOnly == VARIANT_TRUE;}

  /// sets the ReadOnly flag
  void SetReadOnly(VARIANT_BOOL vbReadOnly){m_vbReadOnly = vbReadOnly;}

  /// object setup routine
  HRESULT Setup(CComPtr<IStorage> spRootStg, CComPtr<IStorage> spStg);

  /// used special setup above(poor design I guess)
  HRESULT Setup(CComPtr<IReport>& sp, CComBSTR) {return E_NOTIMPL;}

protected:
  
  /// returns storage smart pointer for revision
  virtual HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);
	
  /// Root Storage smart pointer
  CComPtr<IStorage> m_spRootStg;  
  
  /// Revision Storage smart pointer
  CComPtr<IStorage> m_spStg;
	
  /// Colon Findings smart pointer
  CComPtr<IReportEntries> m_spColonFindings;
  
  /// Other Findings smart pointer
  CComPtr<IReportOtherFindings> m_spOtherFindings;
  
  /// Non Distended Findings smart pointer
  CComPtr<IReportNonDistendedFindings> m_spNonDistendedFindings;
	
  /// Info smart pointer
  CComPtr<IReportInfo> m_spInfo;
	
	/// readOnly flag
  VARIANT_BOOL m_vbReadOnly;
};

#endif

// $Log: ReportRevision.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.2  2001/11/09 04:50:21  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1  2001/10/30 00:44:27  geconomos
// Added methods for getting/setting MarkedComplete flag
//
// Revision 3.0.2.1  2001/10/31 00:27:37  jmeade
// From Revision 3.1  2001/10/30 00:44:27  geconomos:  Added methods for getting/setting MarkedComplete flag
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
//    Rev 1.0   Sep 16 2001 17:27:50   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:43  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.4  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.3  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.2  2001/02/20 17:25:12  kevin
// Fixed bug with crash on opening a report file from CD
//
// Revision 1.1  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.6  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.5  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevision.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportRevision.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
