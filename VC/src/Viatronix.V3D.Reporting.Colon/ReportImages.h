// $Id: ReportImages.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportImages.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Images collection object
// Created: 2/1/2001
// Author: George Economos
//
// Revision History:
// $Log: ReportImages.h,v $
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
//    Rev 2.0   Sep 16 2001 23:40:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:40   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/05/08 16:20:03  geconomos
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
//
// Complete history on bottom of file

#ifndef __REPORTIMAGES_H_
#define __REPORTIMAGES_H_

#include "resource.h"       // main symbols

#include <list>

class ATL_NO_VTABLE ReportImages : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportImages, &CLSID_ReportImages>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportImages, &IID_IReportImages, &LIBID_ReportLib>
{
public:

DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportImages)
	COM_INTERFACE_ENTRY(IReportImages)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
 
END_COM_MAP()

public:
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  STDMETHOD(get_Item)(VARIANT Index,IImage** ppiImage);		
  
  STDMETHOD(get__NewEnum)(IUnknown** ppiUnknown);		
  
  STDMETHOD(get_Count)(long *pnCount);
  
  STDMETHOD(Remove)(VARIANT nIndex);
  
  STDMETHOD(Add)(IImage* piImage);
  
  HRESULT Setup(CComPtr<IReportRevision>& spRevision, CComPtr<IReportEntry>& spEntry);
  
  HRESULT Save();
  
protected:
  
  typedef std::pair<CComBSTR,int> TTocItem;
  
  typedef std::list<TTocItem> TToc;

  TToc m_Toc;
  
  HRESULT ReadTOC();
  
  HRESULT WriteTOC();

  int4 FindItem(CComBSTR bs);

  TTocItem GetEntry(int4 iIndex);
  
  void RemoveEntry(int4 iIndex);
 
  HRESULT GetImage(CComBSTR bsName,IImage** ppiImage);
  
  HRESULT PutImage(CComBSTR bsName, IImage* piImage);

  HRESULT GetImageStorage(CComBSTR bsName, CComPtr<IStorage>& spParentStg, CComPtr<IStorage>& spStg);
  
  int4 VerifyIndex(VARIANT& v);
  
  CComPtr<IReportEntry> m_spEntry;
  
  CComPtr<IReportRevision> m_spRevision;
};

#endif

// $Log: ReportImages.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:56  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/05/08 16:20:03  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportImages.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportImages.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $

