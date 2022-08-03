// $Id: ReportEntry.h,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//


#ifndef ReportEntry_h__
#define ReportEntry_h__

#include "ReportLib.h"
#include "ReportHelper.h"

class ATL_NO_VTABLE ReportEntry : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportEntry, &CLSID_ReportEntry>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportEntry, &IID_IReportEntry, &LIBID_ReportLib>,
  public ReportHelper
{
public:

DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportEntry)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportEntry)
	COM_INTERFACE_ENTRY(IReportEntry)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:
  
  /// called when final reference to object has been released
  void FinalRelease();

  /// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// gets finding property
  STDMETHOD(get_ColonFinding)(ColonFindingEnum *pnFinding);

  /// sets finding property
  STDMETHOD(put_ColonFinding)(ColonFindingEnum nFinding);

  /// gets perpendicular size property
  STDMETHOD(get_Perpendicular)(float *pfPerpSize);
	
  /// sets perpendicular size property
  STDMETHOD(put_Perpendicular)(float fPerpSize);

  /// gets maximum size property
  STDMETHOD(get_Maxiumum)(float *pfMaxSize);
	
  /// sets maximum size property
  STDMETHOD(put_Maxiumum)(float fMaxSize);
	
  /// gets translucency property
  STDMETHOD(get_Translucency)(TranslucencyEnum *pnTranslucency);
	
  /// sets translucency property
  STDMETHOD(put_Translucency)(TranslucencyEnum nTranslucency);
	
  /// gets size comparison property
  STDMETHOD(get_SizeComparison)(SizeComparisonEnum *pnSizeCompare);
	
  /// sets size comparison property
  STDMETHOD(put_SizeComparison)(SizeComparisonEnum nSizeCompare);
	
  /// gets shape value
  STDMETHOD(get_Shape)(ShapeEnum *pnShape);
	
  /// sets shape value
  STDMETHOD(put_Shape)(ShapeEnum nShape);
	
  /// gets location property
  STDMETHOD(get_Location)(LocationEnum *pnLocation);
	
  /// sets location property
  STDMETHOD(put_Location)(LocationEnum nLocation);
  
  /// gets name property
  STDMETHOD(get_Name)(BSTR *pbsName);
  
  /// sets name property
  STDMETHOD(put_Name)(BSTR bsName);
  
  /// gets comment property
  STDMETHOD(get_Comments)(BSTR *pbsComments);
  
  /// sets comment property
  STDMETHOD(put_Comments)(BSTR bsComments);

  /// gets Camera coordinates property
  STDMETHOD(get_CameraCoordinates)(VARIANT* pvDirection);

  /// sets Direction property
  STDMETHOD(put_CameraCoordinates)(VARIANT vDirection);

  /// gets entry's original distance to rectum property
  STDMETHOD(get_OrigDistanceToRectum)(float *pfOrigDist);
	
  /// sets entry's original distance to rectum property
  STDMETHOD(put_OrigDistanceToRectum)(float fOrigDist);

  /// gets the ID for the corresponding other finding (for extra-colonic findings)
  STDMETHOD(get_OtherFindingID)(BSTR *pbsOtherFindingID);

  /// sets the ID for the corresponding other finding (for extra-colonic findings)
  STDMETHOD(put_OtherFindingID)(BSTR bsOtherFindingID);
 
  /// gets ID property
  STDMETHOD(get_ID)(BSTR *pbsID);

  /// gets entry's version number property
  STDMETHOD(get_Version)(int *piVersion);

  /// sets entry's version property
  STDMETHOD(put_Version)(int iVersion);
  
  /// gets images collection
  STDMETHOD(get_Images)(IReportImages** ppiImages);
  
	/// object setup routine
  HRESULT Setup(CComPtr<IReportRevision>& spRevision, CComBSTR bsEntryID);

  /// returns the Storage object associated with the entry
  virtual HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);
  
  /// saves all changes made to the entry to disk
  HRESULT Save();
	
private:
	
  /// unique identifier for entry
  CComBSTR m_bsEntryID;

  /// images collection smart pointer
  CComPtr<IReportImages> m_spImages;
}; // ReportEntry

#endif

// $Log: ReportEntry.h,v $
// Revision 1.1.12.2  2008/11/14 20:16:18  jmeade
// give version info to each report entry;
// flip camera coordinates for report entries of old prone datasets.
//
// Revision 1.1.12.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 02:54:59  jmeade
// Camera (location) coordinates and original distance to rectum for each colon finding entry.
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:38   ingmar
// Initial revision.
// Revision 1.3  2001/05/30 14:23:05  jmeade
// coding standards to fix name clashes
//
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.7  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.6  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.5  2001/02/07 16:24:51  geconomos
// Added revisions to report file
//
// Revision 1.4  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.3  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEntry.h,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
// $Id: ReportEntry.h,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
