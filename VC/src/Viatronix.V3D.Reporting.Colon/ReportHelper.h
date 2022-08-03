// $Id: ReportHelper.h,v 1.1.12.1 2009/08/24 22:25:43 gdavidson Exp $
// Copyright 2001, Viatronix Inc, All Rights Reserved
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.

#ifndef ReportHelper_h__
#define ReportHelper_h__

class ReportHelper  
{
protected:
  
  /// returns the Storage object assoicated with the entry
  virtual HRESULT GetEntryStorage(CComPtr<IStorage>& spStg) = 0;

  /// read a property from a stream with given length
  HRESULT ReadProperty(const BSTR bsPropName, void* pVal, uint4 uSize);
  
  /// writes a property to a stream with given length
  HRESULT WriteProperty(const BSTR bsPropName, void* pVal, uint4 uSize);

  /// writes a BSTR to a stream
  HRESULT WriteStringProperty(const BSTR bsPropName, BSTR bsVal);
  
  /// reads a BSTR from a stream
  HRESULT ReadStringProperty(const BSTR bsPropName, BSTR* pVal);

  /// writes a triple to a stream
  HRESULT WriteTripleProperty(const BSTR bsPropName, VARIANT newVal);
  
  /// reads a triple from a stream
  HRESULT ReadTripleProperty(const BSTR bsPropName, VARIANT* pVal);

  /// writes a quaternion to a stream
  HRESULT WriteQuatProperty(const BSTR bsPropName, VARIANT newVal);
  
  /// reads a quaternion from a stream
  HRESULT ReadQuatProperty(const BSTR bsPropName, VARIANT* pVal);
 
  /// writes a variant to a stream
  HRESULT WriteVariantProperty(const BSTR bsPropName, VARIANT newVal, int iNumElements);
  
  /// reads a variant from a stream
  HRESULT ReadVariantProperty(const BSTR bsPropName, VARIANT* pVal, int iNumElements);

  /// writes a matrix to a stream
  HRESULT WriteMatrixProperty(const BSTR bsPropName, VARIANT newVal);
  
  /// reads a matrix from a stream
  HRESULT ReadMatrixProperty(const BSTR bsPropName, VARIANT* pVal);  
  
  /// smart pointer to parent Revision object
  CComPtr<IReportRevision> m_spRevision;

private:
  
  /// Opens or creates a stream
  HRESULT GetPropertyStream(const CComBSTR& bsName, CComPtr<IStorage>& spStg, CComPtr<IStream>& spStream, bool bCreateStream = true );

};

#endif

// $Log: ReportHelper.h,v $
// Revision 1.1.12.1  2009/08/24 22:25:43  gdavidson
// Issue #6322: Cannot read from newly created stream
//
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 02:54:40  jmeade
// Write any n-sized variant to stream.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportHelper.h,v 1.1.12.1 2009/08/24 22:25:43 gdavidson Exp $
// $Id: ReportHelper.h,v 1.1.12.1 2009/08/24 22:25:43 gdavidson Exp $