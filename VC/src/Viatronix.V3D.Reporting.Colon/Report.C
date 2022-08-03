// $Id: Report.C,v 1.2 2006/05/12 18:34:30 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Report.C,v 1.2 2006/05/12 18:34:30 gdavidson Exp $
// Copyright 2001, Viatronix Inc, All Rights Reserved
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos geconomos@viatronix.net
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "Report.h"
#include "ReportRevisions.h"

STDMETHODIMP Report::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReport
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/**
 * accessor for ReportRevisions collection interface
 *
 * @param   ppiRevisions    IRepportRevisons**
 * @return                  S_OK upon success
 */
STDMETHODIMP Report::get_Revisions(IReportRevisions** ppiRevisions)
{
  if(!ppiRevisions)
    return E_POINTER;
  
  return m_spRevisions.CopyTo(ppiRevisions);
}

/**
 * creates a new report file object
 *
 * @param   bsFileName      name of file to create
 * @return                  S_OK upon success
 */
STDMETHODIMP Report::Create(BSTR bsFileName)
{
  DWORD_PTR dwFlags(STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_DENY_WRITE);
  
  HRESULT hr;
  if(FAILED(hr=StgCreateDocfile(bsFileName, dwFlags, 0,&m_spRootStg)))
    return hr;

  m_vbReadOnly = VARIANT_FALSE;
  
  if(FAILED(hr=CommonInit()))
    return hr;
  
  return S_OK;
}


/**
 * opens a new report file object
 *
 * @param   bsFileName    name of file to open
 * @return                S_OK upon success
 */
STDMETHODIMP Report::Open(BSTR bsFileName)
{
  m_vbReadOnly = VARIANT_FALSE;
  
  // try to open read/write
  HRESULT hr = StgOpenStorage(bsFileName, NULL, STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_DENY_NONE, NULL, 0, &m_spRootStg); 
  
  // check to see if we failed because of permissioning
  if(FAILED(hr) && (hr == STG_E_LOCKVIOLATION || hr == STG_E_ACCESSDENIED))
  {
      // try to open just read
      hr=StgOpenStorage(bsFileName, NULL, STGM_TRANSACTED|STGM_READ|STGM_SHARE_DENY_NONE, NULL, 0, &m_spRootStg); 
      
      m_vbReadOnly = VARIANT_TRUE;
  }
  
  if(FAILED(hr))
    return hr; // Houston we have a problem

  
  if(FAILED(hr=CommonInit()))
    return hr;
  
  return S_OK;
}


/**
 * releases all held objects
 * @return    S_OK upon success
 */
STDMETHODIMP Report::Close()
{
  m_spRevisions  = NULL;

  if( m_spRootStg != NULL )
    m_spRootStg.Release();

  m_spRootStg = NULL;
  
  return S_OK;
}

/**
 * common initialization routines for Report object
 * @return    S_OK upon success
 */
HRESULT Report::CommonInit()
{
  HRESULT hr;
  
  // query for CoClass
  ReportRevisions* pRevisions = GetCoClass<ReportRevisions>(m_spRevisions);
  if(pRevisions == NULL)
    return E_FAIL;

  // convert CoClass to interface pointer
  CComPtr<IReport> spReport;
  if(FAILED(hr=QueryInterface(IID_IReport, reinterpret_cast<void**>(&spReport))))
    return hr;
  
  // setup the Revisions object
  if(FAILED(hr=pRevisions->Setup(spReport, m_vbReadOnly)))
    return hr;  
  
  return S_OK;
}

/**
 * called when object is fully constructed
 * @return  S_OK upon success
 */
HRESULT Report::FinalConstruct()
{
  HRESULT hr;

  // create the Revisions collection object
  CComObject<ReportRevisions>* pRevisions;
  if(FAILED(hr=CComObject<ReportRevisions>::CreateInstance(&pRevisions)))
    return hr;
  
  // query for IReportRevisions interface 
  if(FAILED(hr = pRevisions->QueryInterface(IID_IReportRevisions, reinterpret_cast<void**>(&m_spRevisions))))
    return hr;
  
  return S_OK;
}

/**
 * called when final reference to object has been released
 */
void Report::FinalRelease()
{
  // Release revsions collection
  m_spRevisions = NULL;
}

// Revision History:
// $Log: Report.C,v $
// Revision 1.2  2006/05/12 18:34:30  gdavidson
// Issue #  4745 : Changed the read write access
//
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
//    Rev 2.0   Sep 16 2001 23:40:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:32   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:37  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.4  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.3  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Report.C,v 1.2 2006/05/12 18:34:30 gdavidson Exp $
// $Id: Report.C,v 1.2 2006/05/12 18:34:30 gdavidson Exp $