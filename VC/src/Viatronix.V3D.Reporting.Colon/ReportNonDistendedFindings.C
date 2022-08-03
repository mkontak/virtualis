// $Id: ReportNonDistendedFindings.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFindings.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportNonDistendedFindings.h"
#include "ReportRevision.h"
#include "ReportNonDistendedFinding.h"


STDMETHODIMP ReportNonDistendedFindings::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportNonDistendedFindings
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/**
 * returns an interface for a ReportEntry Object by ID or position
 *
 * @param   vIndex          unique ID or position of a specific Entry within the collection
 * @param   ppiEntry        IReportNonDistendedFinding**
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFindings::get_Item(VARIANT vIndex, IReportNonDistendedFinding** ppiEntry)
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
STDMETHODIMP ReportNonDistendedFindings::get__NewEnum(IUnknown** ppiUnknown)
{
  // will implement when needed
  return E_NOTIMPL;
}

/**
 * adds a new empty object to the collection and returns the object
 *
 * @param   ppiEntry        interface of newly created object
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFindings::Add(IReportNonDistendedFinding** ppiEntry)
{
  if(!ppiEntry)
    return E_POINTER;
  *ppiEntry = NULL;
  
  return AddEntry(CreateIndentifier(), ppiEntry);
}

/**
 * removes an object from the collection
 *
 * @param   vIndex         unique ID or position of a specific object within the collection
 * @return                 S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFindings::Remove(VARIANT vIndex)
{
  HRESULT hr;
  
  // verify that the index is valid
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;
  
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spParent);
  if(pRevision==NULL)
    return E_POINTER;

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
STDMETHODIMP ReportNonDistendedFindings::get_Count(long *piCount)
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
HRESULT ReportNonDistendedFindings::Setup(CComPtr<IReportRevision>& spRevision)
{
  m_spParent = spRevision;
  
  HRESULT hr;
  if(FAILED(hr=ReadTOC(TITLE_NON_DISTENDED)))
    return hr;
  
  return S_OK;
}

/**
 * saves all changes to collection to disk
 *
 * @return      S_OK upon success
 */
HRESULT ReportNonDistendedFindings::Save()
{
  HRESULT hr;
  if(FAILED(hr=WriteTOC(TITLE_NON_DISTENDED)))
    return hr;
  
  return S_OK;

}
/**
 * called when final reference to object has been released
 */
void ReportNonDistendedFindings::FinalRelease()
{
  Free();
}

// $Log: ReportNonDistendedFindings.C,v $
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
// Revision 1.8  2001/05/08 16:20:04  geconomos
// More coding standards
//
// Revision 1.7  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.6  2001/03/30 18:11:47  geconomos
// Added error check to ReadTOC() method.
//
// Revision 1.5  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.4  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.3  2001/02/01 16:34:33  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFindings.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportNonDistendedFindings.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
