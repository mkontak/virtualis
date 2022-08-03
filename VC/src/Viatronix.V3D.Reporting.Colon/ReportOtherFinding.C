// $Id: ReportOtherFinding.C,v 1.2 2007/02/09 00:11:49 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportOtherFinding.C,v 1.2 2007/02/09 00:11:49 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Other Finding object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportOtherFinding.h"
#include "ReportRevision.h"


STDMETHODIMP ReportOtherFinding::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportOtherFinding
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/**
 * gets the ID of the entry item
 *
 * @param   pbsID           contains entry ID upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_ID(BSTR* pbsID)
{
  if(!pbsID)
    return E_POINTER;
  
  return m_bsEntryID.CopyTo(pbsID);
}

/**
 * gets the Other Findings property
 *
 * @param   pnOtherFinding  contains Other Finidings property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_Finding(OtherFindingsEnum* pnOtherFinding)
{
  return ReadProperty(FINDING_PROPERTY_FINDING, pnOtherFinding, sizeof(OtherFindingsEnum));
}

/**
 * sets the Finding property
 *
 * @param   nOtherFinding     new Finding value
 * @return                    S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Finding(OtherFindingsEnum nOtherFinding)
{
  return WriteProperty(FINDING_PROPERTY_FINDING, &nOtherFinding, sizeof(OtherFindingsEnum));
}

/**
 * gets the Location property
 *
 * @param   pnLocation      contains Location property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_Location(long* pnLocation)
{
	return ReadProperty(FINDING_PROPERTY_LOCATION, pnLocation, sizeof(long));
}

/**
 * sets the Location property
 *
 * @param   iLocation        new Location value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Location(long iLocation)
{
  return WriteProperty(FINDING_PROPERTY_LOCATION, &iLocation, sizeof(long));
}

/**
 * gets the Maximum Size property
 *
 * @param   pfMaximumSize   contains Maximum Size property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_MaximumSize(float* pfMaximumSize)
{
  return ReadProperty(FINDING_PROPERTY_MAX_SIZE, pfMaximumSize, sizeof(float));
}

/**
 * sets the Maximum Size property
 *
 * @param   fMaximumSize     new Maximum Size value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_MaximumSize(float fMaximumSize)
{
  return WriteProperty(FINDING_PROPERTY_MAX_SIZE, &fMaximumSize, sizeof(float));
}

/**
 * gets the Perpendicular Size property
 *
 * @param   pfPerpSize      contains Perpendicular Size property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_PerpendicularSize(float* pfPerpSize)
{
  return ReadProperty(FINDING_PROPERTY_PERP_SIZE, pfPerpSize, sizeof(float));
}

/**
 * sets the Perpendicular Size property
 *
 * @param   fPerpSize        new Perpendicular Size value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_PerpendicularSize(float fPerpSize)
{
  return WriteProperty(FINDING_PROPERTY_PERP_SIZE, &fPerpSize, sizeof(float));
}

/**
 * gets the Length property
 *
 * @param   pfLength        contains Length property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_Length(float* pfLength)
{
  return ReadProperty(FINDING_PROPERTY_LENGTH, pfLength, sizeof(float));
}

/**
 * sets the Length property 
 *
 * @param   fLength          new Length value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Length(float fLength)
{
  return WriteProperty(FINDING_PROPERTY_LENGTH, &fLength, sizeof(float));
}

/**
 * gets the Degree property
 *
 * @param   piDegree        contains Degree property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_Degree(long* piDegree)
{
  return ReadProperty(FINDING_PROPERTY_DEGREE, piDegree, sizeof(long));
}

/**
 * sets the Degree property
 *
 * @param   iDegree          new Degree value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Degree(long iDegree)
{
  return WriteProperty(FINDING_PROPERTY_DEGREE, &iDegree, sizeof(long));
}

/**
 * gets the Size property
 *
 * @param   piSize          contains Size property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_Size(long* piSize)
{ 
  return ReadProperty(FINDING_PROPERTY_SIZE, piSize, sizeof(long));
}

/**
 * sets the Size property
 *
 * @param   iSize            new Size value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Size(long iSize)
{
  return WriteProperty(FINDING_PROPERTY_SIZE, &iSize, sizeof(long));
}


/**
 * gets the CRADS Category property
 *
 * @param   piCat           contains CRADS category property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::get_CRADSCategory(long* piCat)
{ 
  return ReadProperty(FINDING_PROPERTY_CRADS_CATEGORY, piCat, sizeof(long));
}

/**
 * sets the CRADS Category property
 *
 * @param   iCat             new CRADS category value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_CRADSCategory(long iCat)
{
  return WriteProperty(FINDING_PROPERTY_CRADS_CATEGORY, &iCat, sizeof(long));
}


/**
 * gets the Comment property
 *
 * @param   pbsComment      contains Comment property upon return
 * @return                  S_OK upon success
 */

STDMETHODIMP ReportOtherFinding::get_Comment(BSTR* pbsComment)
{
  return ReadStringProperty(FINDING_PROPERTY_COMMENT, pbsComment);
}

/**
 * sets the Comment property
 *
 * @param   bsComment        new Comment value
 * @return                   S_OK upon success
 */
STDMETHODIMP ReportOtherFinding::put_Comment(BSTR bsComment)
{
  return WriteStringProperty(FINDING_PROPERTY_COMMENT, bsComment);
}

/**
 * object setup routine
 *
 * @param   spRevision      smart pointer to parent revision object
 * @param   bsEntryID       unique entry ID
 * @return                  S_OK upon success
 */
HRESULT ReportOtherFinding::Setup(CComPtr<IReportRevision>& spRevision, CComBSTR  bsEntryID)
{
  m_spRevision = spRevision;
  m_bsEntryID = bsEntryID;
  
  return S_OK;
}

/**
 * returns the Storage object assoicated with the entry
 *
 * @param   spStg           contains Storage smart pointer upon return
 * @return                  S_OK upon success
 */
HRESULT ReportOtherFinding::GetEntryStorage(CComPtr<IStorage>& spStg)
{
	HRESULT hr(S_OK);
  
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision==NULL)
    return E_POINTER;
	
	// try to open the storage with desired access
  hr = OpenStorage(pRevision->GetStorage(), m_bsEntryID, spStg, pRevision->GetReadOnly());
	if(SUCCEEDED(hr))
		return S_OK;
	
	// if it doesn't exist, create it
  if(FAILED(hr) && hr == STG_E_FILENOTFOUND)
    hr = CreateStorage(pRevision->GetStorage(), m_bsEntryID, spStg);

  return hr;
}

// $Log: ReportOtherFinding.C,v $
// Revision 1.2  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
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
//    Rev 2.0   Sep 16 2001 23:40:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:48   ingmar
// Initial revision.
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:42  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/05/08 16:20:04  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportOtherFinding.C,v 1.2 2007/02/09 00:11:49 jmeade Exp $
// $Id: ReportOtherFinding.C,v 1.2 2007/02/09 00:11:49 jmeade Exp $
