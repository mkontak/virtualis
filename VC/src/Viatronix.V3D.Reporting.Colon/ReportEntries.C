// $Id: ReportEntries.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEntries.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Entries collection object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportRevision.h"
#include "ReportEntries.h"
#include "ReportEntry.h"

STDMETHODIMP ReportEntries::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportEntries
	};
	
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++){
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	
	return S_FALSE;
}

/**
 * returns an interface for a ReportEntry Object by ID or position
 *
 * @param   vIndex          unique ID or position of a specific Entry within the collection
 * @param   ppiEntry        IReportEntry**
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntries::get_Item(VARIANT vIndex, IReportEntry** ppiEntry)
{
	// verify that the index is valid
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;
 
	// return the object's interface
  return GetEntry(iIndex).second.CopyTo(ppiEntry);
}

/**
 * returns an interface for an IEnumXXX interface
 *
 * @param   IUnknown        IUnknown pointer
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntries::get__NewEnum(IUnknown** ppiUnknown)
{
	// will implement when needed
  return E_NOTIMPL;
}

/**
 * adds a new empty entry to the collection and returns the object
 *
 * @param   ppiEntry        interface of newly created Entry object
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntries::Add(IReportEntry** ppiEntry)
{
	if(!ppiEntry)
		return E_POINTER;
	*ppiEntry = NULL;

	return AddEntry(CreateIndentifier(), ppiEntry);
}

/**
 * removes an object from the collection
 *
 * @param   vIndex         unique ID or position of a specific Entry within the collection
 * @return                 S_OK upon success
 */
STDMETHODIMP ReportEntries::Remove(VARIANT vIndex)
{
  HRESULT hr(S_OK);

  // verify that the index is valid
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;
  
	ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spParent);
  if(pRevision == NULL)
    return E_FAIL;
	
  // remove the entry from the compound file
  if(FAILED(hr=pRevision->GetStorage()->DestroyElement(GetEntry(iIndex).first)))
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
STDMETHODIMP ReportEntries::get_Count(long* piCount)
{
	if(!piCount)
		return E_POINTER;
	
	*piCount = m_Toc.size();

	return S_OK;
}

/**
 * setup routine called by parent
 *
 * @param   spRevision      smart pointer to parent Revision object
 * @return                  S_OK upon success
 */
HRESULT ReportEntries::Setup(CComPtr<IReportRevision>& spRevision)
{
	m_spParent = spRevision;

	HRESULT hr;
	if(FAILED(hr=ReadTOC(TITLE_COLON_FINDINGS)))
		return hr;

  return S_OK;
}

/**
 * saves all changes to collection to disk
 *
 * @return      S_OK upon success
 */
HRESULT ReportEntries::Save()
{
	HRESULT hr(S_OK);
	if(FAILED(hr=WriteTOC(TITLE_COLON_FINDINGS)))
		return hr;

  int4 iNumEntries = m_Toc.size();

  // for each item in the collection
  for(int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  {
    TTocItem ti = GetEntry(iEntry);
    if(ti.second.p)
    {
      ReportEntry* pEntry = GetCoClass<ReportEntry>(ti.second);
      if(pEntry==NULL)
        return E_POINTER;

      // save the item to disk
      if(FAILED(hr=pEntry->Save()))
        return hr;
    }
  } // end for each item

	return S_OK;
}

/**
 * called when final reference to object has been released
 */
void ReportEntries::FinalRelease()
{
  Free();  
}


// $Log: ReportEntries.C,v $
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
//    Rev 2.0   Sep 16 2001 23:40:40   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:36   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.9  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.8  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.7  2001/03/30 18:11:47  geconomos
// Added error check to ReadTOC() method.
//
// Revision 1.6  2001/02/07 16:24:51  geconomos
// Added revisions to report file
//
// Revision 1.5  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.4  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEntries.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportEntries.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
