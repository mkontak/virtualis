// $Id: ReportRevision.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevision.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Revision Object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportRevision.h"
#include "ReportEntries.h"
#include "ReportOtherFindings.h"
#include "ReportnonDistendedFindings.h"
#include "ReportInfo.h"


STDMETHODIMP ReportRevision::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportRevision
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	} 
	return S_FALSE;
}

/**
 * returns the major version of the revision
 *
 * @param   piMajorVersion  contains major version upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevision::get_MajorVersion(long* piMajorVersion)
{
  return ReadProperty(HEADER_MAJOR_VERSION, piMajorVersion, sizeof(long));
}

/**
 * returns the minor version of the revision
 *
 * @param   piMinorVersion  contains minor version upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevision::get_MinorVersion(long* piMinorVersion)
{
  return ReadProperty(HEADER_MINOR_VERSION, piMinorVersion, sizeof(long));
}

/**
 * returns the "Colon Findings" collection
 *
 * @param   ppiEntries      contains entries interface upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevision::get_Entries(IReportEntries** ppiEntries)
{
	if(!ppiEntries)
		return E_POINTER;
	
	return m_spColonFindings.CopyTo(ppiEntries);
}

/**
 * returns the "Other Findings" collection
 *
 * @param   ppiOther        contains Other findings collection interface upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevision::get_OtherFindings(IReportOtherFindings** ppiOther)
{
  if(!ppiOther)
    return E_POINTER;
  
  return m_spOtherFindings.CopyTo(ppiOther);
}


/**
 * returns the "Non Distended Findings" collection
 *
 * @param   ppiNonDistended   contains Non Distended findings collection interface upon return
 * @return                    S_OK upon success
 */
STDMETHODIMP ReportRevision::get_NonDistendedFindings(IReportNonDistendedFindings** ppiNonDistended)
{
  if(!ppiNonDistended)
    return E_POINTER;
  
  return m_spNonDistendedFindings.CopyTo(ppiNonDistended);
}

/**
 * returns the info object
 *
 * @param   ppiNonDistended   contains info interface upon return
 * @return                    S_OK upon success
 */
STDMETHODIMP ReportRevision::get_Info(IReportInfo** ppiInfo)
{
  if(!ppiInfo)
    return E_POINTER;
  
  return m_spInfo.CopyTo(ppiInfo);
}

/**
 * returns the IsOpen flag
 *
 * @param   pbIsOpen    contains FileOpen flag upon return
 * @return              S_OK upon success
 */
STDMETHODIMP ReportRevision::get_IsOpen(VARIANT_BOOL* pbIsOpen)
{
  if(!pbIsOpen)  
    return E_POINTER;
  *pbIsOpen = !m_spRootStg? VARIANT_FALSE : VARIANT_TRUE;
  return S_OK;
}

/**
 * returns the ReadOnly flag
 *
 * @param   pbIsReadOnly  contains ReadOnly flag upon return
 * @return                S_OK upon success
 */
STDMETHODIMP ReportRevision::get_IsReadOnly(VARIANT_BOOL* pbIsReadOnly)
{
  if(!pbIsReadOnly)  
    return E_POINTER;
  *pbIsReadOnly = m_vbReadOnly;
  
  return S_OK;
}

/**
 * returns the Marked Completed flag
 *
 * @param   pbIsReadOnly  contains MarkedCompleted flag upon return
 * @return                S_OK upon success
 */
STDMETHODIMP ReportRevision::get_MarkedCompleted(VARIANT_BOOL* pbMarkedCompleted)
{
  *pbMarkedCompleted = VARIANT_FALSE;
  return ReadProperty(HEADER_COMPLETED, pbMarkedCompleted, sizeof(VARIANT_BOOL));
}

/**
 * sets the Marked Completed flag
 *
 * @param   bIsReadOnly   MarkedCompleted
 * @return                S_OK upon success
 */
STDMETHODIMP ReportRevision::put_MarkedCompleted(VARIANT_BOOL bMarkedCompleted)
{
  return WriteProperty(HEADER_COMPLETED, &bMarkedCompleted, sizeof(VARIANT_BOOL));
}

/**
 * saves all changes meade to the revision to disk
 *
 * @return S_OK upon success
 */
STDMETHODIMP ReportRevision::Save()
{
  HRESULT hr;
  
  // do we have a valid state (all collections created and both storage open)
  if (!m_spColonFindings || !m_spOtherFindings || !m_spNonDistendedFindings || !m_spStg || !m_spRootStg)
    return S_OK;

  // Save the colon findings
  ReportEntries* pColon = GetCoClass<ReportEntries>(m_spColonFindings);
  if(pColon == NULL)
    return E_POINTER;  
  
  if(FAILED(hr=pColon->Save()))
    return hr;

  // Save the other findings
  ReportOtherFindings* pOther = GetCoClass<ReportOtherFindings>(m_spOtherFindings);
  if(pOther==NULL)
    return E_POINTER;
  
  if(FAILED(hr=pOther->Save()))
    return hr;
  
  // Save the Non Distended findings
  ReportNonDistendedFindings* pNonDistended = GetCoClass<ReportNonDistendedFindings>(m_spNonDistendedFindings);
  if(pNonDistended == NULL)
    return E_POINTER;
  
  if(FAILED(hr=pNonDistended->Save()))
    return hr;

  // commit changes to revision storage
  if(FAILED(hr=m_spStg->Commit(STGC_DEFAULT)))
    return hr;

  // commit changes to root storage
  if(FAILED(hr=m_spRootStg->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

/**
 * releases all collections and storage objects
 *
 * @return S_OK upon success
 */
STDMETHODIMP ReportRevision::Close()
{
  m_spColonFindings = NULL;
  m_spOtherFindings = NULL;
  m_spNonDistendedFindings = NULL;
  m_spInfo = NULL;
  m_spRootStg = NULL;
  m_spStg = NULL;
  
  return S_OK;
}

/**
 * object setup routine
 *
 * @param spRootStg     smart pointer to Root Storage
 * @param spStg         smart pointer to Revision Storage
 *
 * @return S_OK upon success
 */
HRESULT ReportRevision::Setup(CComPtr<IStorage> spRootStg, CComPtr<IStorage> spStg)
{
  HRESULT hr;
  
  m_spRootStg = spRootStg;
  m_spStg = spStg;

  CComPtr<IReportRevision> spRevision;
  QueryInterface(IID_IReportRevision, reinterpret_cast<void**>(&spRevision));
  
  ReportEntries* pEntries = GetCoClass<ReportEntries>(m_spColonFindings);
  if(FAILED(hr=pEntries->Setup(spRevision)))
    return hr;
  
  ReportOtherFindings* pFindings = GetCoClass<ReportOtherFindings>(m_spOtherFindings);
  if(FAILED(hr=pFindings->Setup(spRevision)))
    return hr;
  
  ReportNonDistendedFindings* pNon = GetCoClass<ReportNonDistendedFindings>(m_spNonDistendedFindings);
  if(FAILED(hr=pNon->Setup(spRevision)))
    return hr; 
  
  ReportInfo* pInfo = GetCoClass<ReportInfo>(m_spInfo);
  if(FAILED(hr=pInfo->Setup(spRevision)))
    return hr;

  return S_OK;
}

/**
 * Called when object is fully constructed
 */
HRESULT ReportRevision::FinalConstruct()
{
	HRESULT hr;

	// Create the "Colon Findings" collection object
	  CComObject<ReportEntries>* pEntries;
	  if(FAILED(hr=CComObject<ReportEntries>::CreateInstance(&pEntries)))
		  return hr;

    hr = pEntries->QueryInterface(IID_IReportEntries, reinterpret_cast<void**>(&m_spColonFindings));
	  if(FAILED(hr))
		  return hr;
  // end create "Colon Findings"	
  
    
  // Create the "Other Findings" collection object
    CComObject<ReportOtherFindings>* pFindings;
    if(FAILED(hr=CComObject<ReportOtherFindings>::CreateInstance(&pFindings)))
      return hr;
  
    hr = pFindings->QueryInterface(IID_IReportOtherFindings, reinterpret_cast<void**>(&m_spOtherFindings));
    if(FAILED(hr))
      return hr;
  // end create "Other Findings"
  
    
  // Create the Non Distended Findings collection object
    CComObject<ReportNonDistendedFindings>* pNonDistendedFindings;
    if(FAILED(hr=CComObject<ReportNonDistendedFindings>::CreateInstance(&pNonDistendedFindings)))
      return hr;
  
    hr = pNonDistendedFindings->QueryInterface(IID_IReportNonDistendedFindings, reinterpret_cast<void**>(&m_spNonDistendedFindings));
    if(FAILED(hr))
      return hr;
  // end create Non Distended Findings  
  
    
  // Create the Non Distended Findings collection object
    CComObject<ReportInfo>* pInfo;
	  if(FAILED(hr=CComObject<ReportInfo>::CreateInstance(&pInfo)))
		  return hr;
	
	  hr = pInfo->QueryInterface(IID_IReportInfo, reinterpret_cast<void**>(&m_spInfo));
	  if(FAILED(hr))
		  return hr;
  // end create info object

  m_spRevision = this;
	
	return S_OK;
}

/**
 * returns storage smart pointer for revision
 *
 * @param spStg         smart pointer to Revision Storage
 *
 * @return S_OK upon success
 */
HRESULT ReportRevision::GetEntryStorage(CComPtr<IStorage>& spStg) 
{
  spStg = m_spStg;

  return S_OK;
}


// $Log: ReportRevision.C,v $
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
// Revision 1.5  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.4  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.3  2001/02/20 17:25:12  kevin
// Fixed bug with crash on opening a report file from CD
//
// Revision 1.2  2001/02/16 18:04:21  jeff
// check null pointers (in case report was already closed) before saving
//
// Revision 1.1  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.8  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.7  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevision.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportRevision.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $


