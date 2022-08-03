// $Id: ReportNonDistendedFinding.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: NonDistended object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#ifndef ReportNonFistendedFinding_h_
#define ReportNonFistendedFinding_h_

#include "ReportHelper.h"

class ReportRevision;

class ATL_NO_VTABLE ReportNonDistendedFinding : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ReportNonDistendedFinding, &CLSID_ReportNonDistendedFinding>,
	public ISupportErrorInfo,
	public IDispatchImpl<IReportNonDistendedFinding, &IID_IReportNonDistendedFinding, &LIBID_ReportLib>,
  public ReportHelper
{
public:

DECLARE_NO_REGISTRY()
DECLARE_NOT_AGGREGATABLE(ReportNonDistendedFinding)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(ReportNonDistendedFinding)
	COM_INTERFACE_ENTRY(IReportNonDistendedFinding)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:
  
  /// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	
  /// gets End Point property
  STDMETHOD(get_EndPoint)(VARIANT* pvEndPoint);
	
  /// sets End Point property
  STDMETHOD(put_EndPoint)(VARIANT vEndPoint);
	
  /// gets Start Point property
  STDMETHOD(get_StartPoint)(VARIANT* pvStartPoint);
	
  /// sets Start Point property
  STDMETHOD(put_StartPoint)(VARIANT vStartPoint);
	
  /// gets Direction property
  STDMETHOD(get_Direction)(VARIANT* pvDirection);
	
  /// sets Direction property
  STDMETHOD(put_Direction)(VARIANT vDirection);
	
  /// gets Position property
  STDMETHOD(get_Position)(VARIANT* pvPosition);
	
  /// sets Position property
  STDMETHOD(put_Position)(VARIANT vPosition);
	
  /// gets Offset property
  STDMETHOD(get_Offset)(float* pfOffset);
	
  /// sets Offset property
  STDMETHOD(put_Offset)(float fOffset);
	
  /// gets DatasetPositionEnumProp property
  STDMETHOD(get_DatasetPositionEnumProp)(DatasetPositionEnumProp* peStudyType);
	
  /// sets DatasetPositionEnumProp property
  STDMETHOD(put_DatasetPositionEnumProp)(DatasetPositionEnumProp eStudyType);
	
  /// gets Image property
  STDMETHOD(get_Image)(IImage** ppiImage);
	
  /// sets Image property
  STDMETHOD(put_Image)(IImage* piImage);
	
  /// gets View Mode property
  STDMETHOD(get_ViewMode)(long* piViewMode);
	
  /// sets View Mode property
  STDMETHOD(put_ViewMode)(long iViewMode);
	
  /// gets TransferY property
  STDMETHOD(get_TransferY)(float* pfTransferY);
	
  /// sets TransferY property
  STDMETHOD(put_TransferY)(float fTransferY);
	
  /// gets TransferX property
  STDMETHOD(get_TransferX)(float* pfTransferX);
	
  /// sets TransferX property
  STDMETHOD(put_TransferX)(float fTransferX);
	
  /// gets Rotation Matrix property
  STDMETHOD(get_RotationMatrix)(VARIANT* pvMatrix);
	
  /// sets Rotation Matrix property
  STDMETHOD(put_RotationMatrix)(VARIANT vMatrix);
	
  /// gets LocationEnum property
  STDMETHOD(get_Location)(LocationEnum* pnLocation);
	
  /// sets LocationEnum property
  STDMETHOD(put_Location)(LocationEnum nLocation);
	
  /// gets Text property
  STDMETHOD(get_Text)(BSTR* pbsText);
	
  /// sets Text property
  STDMETHOD(put_Text)(BSTR bsText);
  
  /// gets ID property
  STDMETHOD(get_ID)(BSTR* pbsText);
	
  /// gets Zoom Factor property
  STDMETHOD(get_ZoomFactor)(float* pfZoomFactor);
	
  /// sets Zoom Factor property
  STDMETHOD(put_ZoomFactor)(float fZoomFactor);
	
  /// gets Quaternion property
  STDMETHOD(get_Quaternion)(VARIANT* pvQuaternion);
	
  /// sets Quaternion property
  STDMETHOD(put_Quaternion)(VARIANT vQuaternion);

  /// object setup routine
  HRESULT Setup(CComPtr<IReportRevision>& spRevision, CComBSTR bsEntryID);

  /// returns the Storage object assoicated with the entry
  HRESULT GetEntryStorage(CComPtr<IStorage>& spStg);

protected:
 
  /// unique identifier for entry
  CComBSTR  m_bsEntryID;
};

#endif

// $Log: ReportNonDistendedFinding.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.1  2001/12/29 00:12:18  jmeade
// Merge from 1.1 branch.
//
// Revision 3.0.2.2  2001/12/03 21:30:59  jmeade
// StudyTypeEnum changed to DatasetPositionEnumProp to eliminate current and future conflicts
//
// Revision 3.0.2.1  2001/12/03 20:48:43  jmeade
// StudyType changed to StudyTypeEnum to eliminate conflicts
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
// Revision 1.3  2001/05/30 14:23:05  jmeade
// coding standards to fix name clashes
//
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:41  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.11  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.10  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.9  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.8  2001/02/05 18:22:15  geconomos
// Added start  and end points in order to draw centerline
//
// Revision 1.7  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.6  2001/02/01 22:28:54  geconomos
// Added postion and direction to non distended areas
//
// Revision 1.5  2001/02/01 16:34:33  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFinding.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportNonDistendedFinding.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
