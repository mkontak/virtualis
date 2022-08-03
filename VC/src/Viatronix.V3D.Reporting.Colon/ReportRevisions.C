// $Id: ReportRevisions.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportRevisions.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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
#include "ReportRevisions.h"
#include "Report.h"
#include "ReportRevision.h"


STDMETHODIMP ReportRevisions::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportRevisions
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/**
 * returns a interface for a ReportRevison Object by ID or position
 *
 * @param   vIndex          unique ID or position of a specific Revision within the collection
 * @param   ppiRevision     IRepportRevisons**
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevisions::get_Item(VARIANT vIndex, IReportRevision** ppiRevision)
{
	// verify that the index is valid
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;
 
	// return the object's interface
  return GetEntry(iIndex).second.CopyTo(ppiRevision);
}

/**
 * returns an interface for an IEnumXXX interface
 *
 * @param   IUnknown        IUnknown pointer
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevisions::get__NewEnum(IUnknown** ppiUnknown)
{
	// will implement when needed
  return E_NOTIMPL;
}

/**
 * adds a new empty revision to the collection and returns the object
 *
 * @param   ppiRevision     interface of newly created Revision object
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportRevisions::Add(IReportRevision** ppiRevision)
{
	if(!ppiRevision)
		return E_POINTER;
	*ppiRevision = NULL;

	return AddEntry(CreateIndentifier(), ppiRevision);
}

/**
 * removes an object from the collection
 *
 * @param   vIndex         unique ID or position of a specific Revision within the collection
 * @return                 S_OK upon success
 */
STDMETHODIMP ReportRevisions::Remove(VARIANT vIndex)
{
  HRESULT hr;

  // verify that the index is valid
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;

  Report* pReport = GetCoClass<Report>(m_spParent);
  if(pReport==NULL)
    return E_FAIL;
  
	// remove the entry from the compound file
  if(FAILED(hr=pReport->GetStorage()->DestroyElement(GetEntry(iIndex).first)))
		return hr;

	// remove the entry from the table of contents
  RemoveEntry(iIndex);
	
	return S_OK;
}

/**
 * gets number of items in the collection
 *
 * @param   piCount        pointer of variable that is update with the collection count
 * @return                 S_OK upon success
 */
STDMETHODIMP ReportRevisions::get_Count(long *piCount)
{
	if(!piCount)
		return E_POINTER;	
	*piCount = m_Toc.size();

	return S_OK;
}

/**
 * setup routine called by parent
 *
 * @param   spReport        smart pointer to parent Report object
 * @param   vbReadOnly      read only indicator
 * @return                  S_OK upon success
 */
HRESULT ReportRevisions::Setup(CComPtr<IReport>& spReport, VARIANT_BOOL vbReadOnly)
{
	m_spParent = spReport;
  m_vbReadOnly = vbReadOnly;
  
  HRESULT hr;
	if(FAILED(hr=ReadTOC(TITLE_TABLE_OF_REVISIONS)))
		return hr;
	
	return S_OK;
}

/**
 * called when final reference to object has been released
 */
void ReportRevisions::FinalRelease()
{
  /// release all held objects in the table of contents
  Free();

  /// free the Report object
  m_spParent = NULL;
}

/**
 * adds an entry to the report file
 *
 * @param   bsEntryID       unique ID of Revision object
 * @param   ppiRevision     returns pointer to Revision object
 * @return                  S_OK upon success
*/
HRESULT ReportRevisions::AddEntry(CComBSTR bsEntryID, IReportRevision** ppiRevision)
{
  HRESULT hr;
  
  Report* pReport = GetCoClass<Report>(m_spParent);
  if(pReport == NULL)
    return FALSE;
  
  // try to open the storage for desired access
  CComPtr<IStorage> spStg;
  hr = OpenStorage(pReport->GetStorage(), bsEntryID,  spStg, m_vbReadOnly == VARIANT_TRUE);
  
  // if it does exist create it
  if(FAILED(hr) && hr == STG_E_FILENOTFOUND)
    hr = CreateStorage(pReport->GetStorage(), bsEntryID,  spStg);
  
  // check for errors
  if(FAILED(hr))
    return hr;
  
  // create new Revision object
  CComObject<ReportRevision>* pRevision;
  if(FAILED(hr=CComObject<ReportRevision>::CreateInstance(&pRevision)))
    return hr;
  
  // set op the objects properties
  pRevision->SetReadOnly(m_vbReadOnly);
  
  if(FAILED(hr=pRevision->Setup(pReport->GetStorage(), spStg)))
    return hr;
  
  CComPtr<IReportRevision> spRevision;
  if(FAILED(hr=pRevision->QueryInterface(IID_IReportRevision, reinterpret_cast<void**>(&spRevision))))
    return hr;
  
  // only for new objects
  if(ppiRevision)
    spRevision.CopyTo(ppiRevision);
  
  // add to the table of contengts
  m_Toc.push_back(TTocItem(bsEntryID, spRevision));	
  
  WriteTOC(TITLE_TABLE_OF_REVISIONS);
  
  return S_OK;
}

