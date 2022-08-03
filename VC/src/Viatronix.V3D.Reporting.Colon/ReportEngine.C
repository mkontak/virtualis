// $Id: ReportEngine.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEngine.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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
// $Log: ReportEngine.C,v $
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
//    Rev 1.0   Sep 16 2001 17:27:36   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.8  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.7  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportEngine.h"
#include "ReportEntries.h"
#include "ReportOtherFindings.h"
#include "ReportnonDistendedFindings.h"
#include "ReportInfo.h"


CoReportEngine::CoReportEngine() : 
 m_vbReadOnly(VARIANT_FALSE)
{
}


/////////////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CoReportEngine::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReport
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	} 
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////
// IReportEngine methods and properties
/////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CoReportEngine::get_MajorVersion(long *pVal)
{
  return ReadProperty(HEADER_MAJOR_VERSION, pVal, sizeof(long));
}


STDMETHODIMP CoReportEngine::get_MinorVersion(long *pVal)
{
  return ReadProperty(HEADER_MINOR_VERSION, pVal, sizeof(long));
}

STDMETHODIMP CoReportEngine::get_Info(IReportInfo** ppiInfo)
{
	if(!m_spRootStg)
		return STG_E_INVALIDHANDLE;
	
	if(!ppiInfo)
		return E_POINTER;
	*ppiInfo = NULL;
	
	return m_spInfo.CopyTo(ppiInfo);
}

STDMETHODIMP CoReportEngine::get_Entries(IReportEntries** ppiEntries)
{
	if(!m_spRootStg)
		return STG_E_INVALIDHANDLE;
	
	if(!ppiEntries)
		return E_POINTER;
	*ppiEntries = NULL;
	
	return m_spEntries.CopyTo(ppiEntries);
}

STDMETHODIMP CoReportEngine::get_OtherFindings(IReportOtherFindings **pVal)
{
  if(!m_spRootStg)
    return STG_E_INVALIDHANDLE;
  
  if(!pVal)
    return E_POINTER;
  *pVal = NULL;
  
  return m_spOtherFindings.CopyTo(pVal);
}

STDMETHODIMP CoReportEngine::get_NonDistendedFindings(IReportNonDistendedFindings **pVal)
{
  if(!m_spRootStg)
    return STG_E_INVALIDHANDLE;
  
  if(!pVal)
    return E_POINTER;
  *pVal = NULL;
  
  return m_spNonDistendedFindings.CopyTo(pVal);
}


STDMETHODIMP CoReportEngine::get_IsOpen(VARIANT_BOOL *pVal)
{
  if(!pVal)  
    return E_POINTER;
  *pVal = !m_spRootStg? VARIANT_FALSE : VARIANT_TRUE;
  return S_OK;
}


STDMETHODIMP CoReportEngine::get_IsReadOnly(VARIANT_BOOL *pVal)
{
  if(!pVal)  
    return E_POINTER;
  *pVal = m_vbReadOnly;
  
  return S_OK;
}

STDMETHODIMP CoReportEngine::Create(BSTR bsFileName)
{
	DWORD dwFlags(STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_DENY_WRITE);
	
	HRESULT hr;
	if(FAILED(hr=StgCreateDocfile(bsFileName, dwFlags, 0,&m_spRootStg)))
		return hr;

	if(FAILED(hr=CommonInit(true)))
		return hr;

  WriteProperty(HEADER_MAJOR_VERSION, (void*)(&CURRENT_MAJOR_VERSION), sizeof(long));
  WriteProperty(HEADER_MINOR_VERSION, (void*)(&CURRENT_MINOR_VERSION), sizeof(long));
	
  return S_OK;
}


STDMETHODIMP CoReportEngine::Open(BSTR bsFileName, VARIANT_BOOL vbReadOnly)
{
	m_vbReadOnly = vbReadOnly;
	
	DWORD dwFlags(0);
  if(m_vbReadOnly==VARIANT_TRUE)
    dwFlags = STGM_TRANSACTED|STGM_READ|STGM_SHARE_DENY_NONE;
  else
    dwFlags = STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_DENY_WRITE;

	HRESULT hr; 
	if(FAILED(hr=StgOpenStorage(bsFileName, NULL, dwFlags, NULL, 0, &m_spRootStg)))
		return hr;

	if(FAILED(hr=CommonInit(false)))
		return hr;
	
	return S_OK;
}

STDMETHODIMP CoReportEngine::Save()
{
	HRESULT hr;
	
	if(!m_spRootStg.p)
		return STG_E_INVALIDHANDLE;

	CoReportEntries* pEntries = GetCoClass<CoReportEntries>(m_spEntries);
	if(FAILED(hr=pEntries->Save()))
		return hr;

  CoReportOtherFindings* pFindings = GetCoClass<CoReportOtherFindings>(m_spOtherFindings);
  if(FAILED(hr=pFindings->Save()))
    return hr;
  
  
  CoReportNonDistendedFindings* pNon = GetCoClass<CoReportNonDistendedFindings>(m_spNonDistendedFindings);
  if(FAILED(hr=pNon->Save()))
    return hr;
  
  m_spRootStg->Commit(STGC_DEFAULT|STGC_CONSOLIDATE);

	return S_OK;
}

STDMETHODIMP CoReportEngine::Close()
{
  m_spEntries = NULL;
  m_spOtherFindings = NULL;
  m_spNonDistendedFindings = NULL;
  m_spInfo = NULL;
  m_spRootStg = NULL;
  
  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////////////
// Implementation
/////////////////////////////////////////////////////////////////////////////////////

HRESULT CoReportEngine::FinalConstruct()
{
	HRESULT hr;

	// Create the entries collection object
	CComObject<CoReportEntries>* pEntries;
	if(FAILED(hr=CComObject<CoReportEntries>::CreateInstance(&pEntries)))
		return hr;

	hr = pEntries->QueryInterface(IID_IReportEntries, reinterpret_cast<void**>(&m_spEntries));
	if(FAILED(hr))
		return hr;
	
  // Create the entries collection object
  CComObject<CoReportOtherFindings>* pFindings;
  if(FAILED(hr=CComObject<CoReportOtherFindings>::CreateInstance(&pFindings)))
    return hr;
  
  hr = pFindings->QueryInterface(IID_IReportOtherFindings, reinterpret_cast<void**>(&m_spOtherFindings));
  if(FAILED(hr))
    return hr;
  
  // Create the supine non distended areas collection object
  CComObject<CoReportNonDistendedFindings>* pNonDistendedFindings;
  if(FAILED(hr=CComObject<CoReportNonDistendedFindings>::CreateInstance(&pNonDistendedFindings)))
    return hr;
  
  hr = pNonDistendedFindings->QueryInterface(IID_IReportNonDistendedFindings, reinterpret_cast<void**>(&m_spNonDistendedFindings));
  if(FAILED(hr))
    return hr;
 
  // Create the info object
	CComObject<CoReportInfo>* pInfo;
	if(FAILED(hr=CComObject<CoReportInfo>::CreateInstance(&pInfo)))
		return hr;
	
	hr = pInfo->QueryInterface(IID_IReportInfo, reinterpret_cast<void**>(&m_spInfo));
	if(FAILED(hr))
		return hr;

  m_spEngine = this;
	
	return S_OK;
}


void CoReportEngine::FinalRelease()
{
  Close();
}

HRESULT CoReportEngine::GetEntryStorage(CComPtr<IStorage>& spStg)
{
  spStg = m_spRootStg;

  return S_OK;
}

HRESULT CoReportEngine::CommonInit(bool bNewReport)
{
	HRESULT hr;

	CComPtr<IReportEngine> spEngine;
	QueryInterface(IID_IReportEngine, reinterpret_cast<void**>(&spEngine));
	
	CoReportEntries* pEntries = GetCoClass<CoReportEntries>(m_spEntries);
	if(FAILED(hr=pEntries->Setup(spEngine, bNewReport)))
		return hr;
	
  CoReportOtherFindings* pFindings = GetCoClass<CoReportOtherFindings>(m_spOtherFindings);
  if(FAILED(hr=pFindings->Setup(spEngine, bNewReport)))
    return hr;
 
  CoReportNonDistendedFindings* pNon = GetCoClass<CoReportNonDistendedFindings>(m_spNonDistendedFindings);
  if(FAILED(hr=pNon->Setup(spEngine, bNewReport)))
    return hr; 
 
  CoReportInfo* pInfo = GetCoClass<CoReportInfo>(m_spInfo);
	if(FAILED(hr=pInfo->Setup(spEngine, bNewReport)))
		return hr;
	
	return S_OK;
}

// $Log: ReportEngine.C,v $
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
// Revision 1.8  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.7  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEngine.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportEngine.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $

