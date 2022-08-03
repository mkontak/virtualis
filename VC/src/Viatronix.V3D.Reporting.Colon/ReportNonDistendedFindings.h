// $Id: ReportNonDistendedFindings.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFindings.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: NonDistended collection object
// Created: 2/1/2001
// Author: George Economos
//

// Complete history on bottom of file

#ifndef ReportNondistendedFindings_h_
#define ReportNondistendedFindings_h_


#include "ReportCollection.h"
#include "ReportRevision.h"

class ReportNonDistendedFinding;

class ATL_NO_VTABLE ReportNonDistendedFindings : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportNonDistendedFindings, &CLSID_ReportNonDistendedFindings>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportNonDistendedFindings, &IID_IReportNonDistendedFindings, &LIBID_ReportLib>,
  public ReportCollection<ReportRevision, IReportRevision, IReportNonDistendedFinding, ReportNonDistendedFinding, &IID_IReportNonDistendedFinding>
{
public:
  
DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportNonDistendedFindings)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportNonDistendedFindings)
	COM_INTERFACE_ENTRY(IReportNonDistendedFindings)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:
  
  /// called when final reference to object has been released
  void FinalRelease();
  
  /// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// returns an object interface by position or ID
  STDMETHOD(get_Item)(VARIANT vIndex, IReportNonDistendedFinding** ppiFinding);		
  
  /// gets an IEnumXXX interface pointer(used in Visual Basic)
  STDMETHOD(get__NewEnum)(IUnknown** ppiUnknown);		
  
  /// gets number of revisons currently in collection
  STDMETHOD(get_Count)(long *piCount);
  
  /// removes an object from the collection
  STDMETHOD(Remove)(VARIANT vIndex);
  
  /// adds a new empty entry to the collection and returns the object
  STDMETHOD(Add)(IReportNonDistendedFinding** ppiFinding);
  
  /// setup routine called by parent
  HRESULT Setup(CComPtr<IReportRevision>& spRevision);
  
  /// saves all changes to collection to disk
  HRESULT Save();  
};

#endif

// $Log: ReportNonDistendedFindings.h,v $
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
//    Rev 2.0   Sep 16 2001 23:40:48   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:46   ingmar
// Initial revision.
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
// Revision 1.3  2001/02/01 16:34:33  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFindings.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportNonDistendedFindings.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
