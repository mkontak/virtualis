// $Id: ReportEntry.C,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Report entry object(Colon Finding)
// Created: 2/1/2001
// Author: George Economos
//

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportRevision.h"
#include "ReportEntry.h"
#include "ReportImages.h"


STDMETHODIMP ReportEntry::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportEntry
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
STDMETHODIMP ReportEntry::get_ID(BSTR* pbsID)
{
  if(!pbsID)
    return E_POINTER;

  return m_bsEntryID.CopyTo(pbsID);
}

/**
 * gets the Version property for the entry
 *
 * @param   piVersion   contains version property upon return
 * @return              S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Version(int *piVersion)
{
  return ReadProperty(ENTRY_PROPERTY_VERSION, piVersion, sizeof(int));
}

/**
 * sets the Version property for the entry
 *
 * @param   iVersion   new version value
 * @return             S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Version(int iVersion)
{
  return WriteProperty(ENTRY_PROPERTY_VERSION, &iVersion, sizeof(int));
}


/**
 * gets the images collection for the entry item
 *
 * @param   ppiImages       contains images collection pointer upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Images(IReportImages** ppiImages)
{
	if(!ppiImages)
    return E_INVALIDARG;

  return m_spImages.CopyTo(ppiImages);
}

/**
 * gets the name property for the entry
 *
 * @param   pbsName         contains name property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Name(BSTR *pbsName)
{
  return ReadStringProperty(ENTRY_PROPERTY_NAME, pbsName);
}

/**
 * sets the name property for the entry
 *
 * @param   bsName          new name value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Name(BSTR bsName)
{
  return WriteStringProperty(ENTRY_PROPERTY_NAME, bsName);
}


/**
 * gets the comments property for the entry
 *
 * @param   pbsComments     contains comments property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Comments(BSTR* pbsComments)
{
  return ReadStringProperty(ENTRY_PROPERTY_COMMENTS, pbsComments);
}

/**
 * sets the comments property for the entry
 *
 * @param   bsComments      new comments value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Comments(BSTR bsComments)
{
  return WriteStringProperty(ENTRY_PROPERTY_COMMENTS, bsComments);
}

/**
 * gets the ColonFindingEnum property for the entry
 *
 * @param   pnFinding       contains ColonFindingEnum property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_ColonFinding(ColonFindingEnum *pnFinding)
{
  return ReadProperty(ENTRY_PROPERTY_COLON_FINDING, pnFinding, sizeof(ColonFindingEnum));
}

/**
 * sets the ColonFindingEnum property for the entry
 *
 * @param   nFinding       new ColonFindingEnum property
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_ColonFinding(ColonFindingEnum nFinding)
{
  return WriteProperty(ENTRY_PROPERTY_COLON_FINDING, &nFinding, sizeof(ColonFindingEnum));
}

/**
 * gets the LocationEnum property for the entry
 *
 * @param   pnLocation      contains LocationEnum property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Location(LocationEnum *pnLocation)
{
  return ReadProperty(ENTRY_PROPERTY_LOCATION, pnLocation, sizeof(LocationEnum));
}

/**
 * sets the LocationEnum property for the entry
 *
 * @param   nLocation       new LocationEnum value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Location(LocationEnum nLocation)
{
  return WriteProperty(ENTRY_PROPERTY_LOCATION, &nLocation, sizeof(LocationEnum));
}

/**
 * gets the ShapeEnum property for the entry
 *
 * @param   pnShape         contains ShapeEnum property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Shape(ShapeEnum *pnShape)
{
  return ReadProperty(ENTRY_PROPERTY_SHAPE, pnShape, sizeof(ShapeEnum));
}

/**
 * sets the ShapeEnum property for the entry
 *
 * @param   nShape          new ShapeEnum value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Shape(ShapeEnum nShape)
{
  return WriteProperty(ENTRY_PROPERTY_SHAPE, &nShape, sizeof(ShapeEnum));
}

/**
 * gets the SizeComparisonEnum property for the entry
 *
 * @param   pnSizeComparison    contains SizeComparisonEnum property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_SizeComparison(SizeComparisonEnum *pnSizeCompare)
{
  return ReadProperty(ENTRY_PROPERTY_SIZE_COMPARISON, pnSizeCompare, sizeof(SizeComparisonEnum));
}

/**
 * sets the SizeComparisonEnum property for the entry
 *
 * @param   nSizeComparison     new SizeComparisonEnum value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::put_SizeComparison(SizeComparisonEnum nSizeCompare)
{
  return WriteProperty(ENTRY_PROPERTY_SIZE_COMPARISON, &nSizeCompare, sizeof(SizeComparisonEnum));
}

/**
 * gets the TranslucencyEnum property for the entry
 *
 * @param   pnTranslucency      contains TranslucencyEnum property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Translucency(TranslucencyEnum *pnTranslucency)
{
  return ReadProperty(ENTRY_PROPERTY_TRANSLUCENCY, pnTranslucency, sizeof(TranslucencyEnum));
}

/**
 * sets the TranslucencyEnum property for the entry
 *
 * @param   nTranslucency       new TranslucencyEnum value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Translucency(TranslucencyEnum nTranslucency)
{
  return WriteProperty(ENTRY_PROPERTY_TRANSLUCENCY, &nTranslucency, sizeof(TranslucencyEnum));
}

/**
 * gets the Maximum Size property for the entry
 *
 * @param   pfMaximumSize       contains Maximum Size property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Maxiumum(float *pfMaximumSize)
{
  return ReadProperty(ENTRY_PROPERTY_MAXIMUM, pfMaximumSize, sizeof(float));
}

/**
 * sets the Maximum Size property for the entry
 *
 * @param   fMaximumSize        new Maximum value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Maxiumum(float fMaximumSize)
{
  return WriteProperty(ENTRY_PROPERTY_MAXIMUM, &fMaximumSize, sizeof(float));
}

/**
 * gets the Perpendicular Size property for the entry
 *
 * @param   pfPerpSize          contains Perpendicular Size property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_Perpendicular(float *pfPerpSize)
{
  return ReadProperty(ENTRY_PROPERTY_PERPENDICULAR, pfPerpSize, sizeof(float));
}

/**
 * puts the Perpendicular Size property for the entry
 *
 * @param   fPerpSize           new Perpendicular Size value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::put_Perpendicular(float fPerpSize)
{
  return WriteProperty(ENTRY_PROPERTY_PERPENDICULAR, &fPerpSize, sizeof(float));  
}


/**
 * gets the VolumeLocation coordinates property for the entry
 *
 * @param   pvCoordinates   contains VolumeLocation property upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::get_CameraCoordinates(VARIANT *pvCoordinates)
{
  return ReadVariantProperty(ENTRY_PROPERTY_CAMERACOORDINATES, pvCoordinates, 9);
}

/**
 * sets the camera's volume location property for the entry
 *
 * @param   nLocation       new LocationEnum value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportEntry::put_CameraCoordinates(VARIANT vCoordinates)
{
  return WriteVariantProperty(ENTRY_PROPERTY_CAMERACOORDINATES, vCoordinates, 9);
}

/**
 * gets the original distance to rectum property for the entry
 *
 * @param   pfPerpSize          contains original distance to rectum property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_OrigDistanceToRectum(float *pfOrigDist)
{
  return ReadProperty(ENTRY_PROPERTY_ORIGDISTANCETORECTUM, pfOrigDist, sizeof(float));
}

/**
 * puts the original distance to rectum property for the entry
 *
 * @param   fOrigDist           new original distance to rectum value
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::put_OrigDistanceToRectum(float fOrigDist)
{
  return WriteProperty(ENTRY_PROPERTY_ORIGDISTANCETORECTUM, &fOrigDist, sizeof(float));
}


/**
 * gets the OtherFindingID property for the entry
 *
 * @param   pbsOtherFindingID   contains name property upon return
 * @return                      S_OK upon success
 */
STDMETHODIMP ReportEntry::get_OtherFindingID(BSTR *pbsOtherFindingID)
{
  return ReadStringProperty(ENTRY_PROPERTY_OTHERFINDINGID, pbsOtherFindingID);
}

/**
 * sets the name property for the entry
 *
 * @param   bsOtherFindingID   new name value
 * @return                     S_OK upon success
 */
STDMETHODIMP ReportEntry::put_OtherFindingID(BSTR bsOtherFindingID)
{
  return WriteStringProperty(ENTRY_PROPERTY_OTHERFINDINGID, bsOtherFindingID);
}


/**
 * called when final reference to object has been released
 */
void ReportEntry::FinalRelease()
{
  m_spImages = NULL;
}

/**
 * saves all changes made to the entry to disk
 */
HRESULT ReportEntry::Save()
{
  ReportImages* pImages = GetCoClass<ReportImages>(m_spImages);
  if(pImages == NULL)
    return E_POINTER;
  
  return pImages->Save();
}

/**
 * object setup routine
 *
 * @param   spRevision      smart pointer to parent revision object
 * @param   bsEntryID       unique entry ID
 * @return                  S_OK upon success
 */
HRESULT ReportEntry::Setup(CComPtr<IReportRevision>& spRevision, CComBSTR bsEntryID)
{
	m_spRevision = spRevision;
	m_bsEntryID = bsEntryID;

  HRESULT hr;
  
  // create images collection
  CComObject<ReportImages>* pImages;
  if(FAILED(hr=CComObject<ReportImages>::CreateInstance(&pImages)))
    return hr;
  
  // grab a smart pointer to this object
  CComPtr<IReportEntry> spEntry;
  if(FAILED(hr=QueryInterface(IID_IReportEntry, reinterpret_cast<void**>(&spEntry))))
    return hr;
  
  // setup the images collection
  if(FAILED(hr=pImages->Setup(m_spRevision, spEntry)))
    return hr;
  
  // grab a smart pointer to the images collection
  if(FAILED(hr=pImages->QueryInterface(IID_IReportImages, reinterpret_cast<void**>(&m_spImages))))
    return hr;
 
  return S_OK;
}

/**
 * returns the Storage object assoicated with the entry
 *
 * @param   spStg           contains Storage smart pointer upon return
 * @return                  S_OK upon success
 */
HRESULT ReportEntry::GetEntryStorage(CComPtr<IStorage>& spStg)
{
  HRESULT hr(S_OK);
 
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision == NULL)
    return E_POINTER;

	// try to open the storage with desired access
  hr=OpenStorage(pRevision->GetStorage(), m_bsEntryID, spStg, pRevision->GetReadOnly());
	if(SUCCEEDED(hr))
		return S_OK;
	
	// if it doesn't exist, create it
  if(FAILED(hr) && hr == STG_E_FILENOTFOUND)
		hr = CreateStorage(pRevision->GetStorage(), m_bsEntryID, spStg);
	
	return hr;
}


// $Log: ReportEntry.C,v $
// Revision 1.1.12.2  2008/11/14 20:16:18  jmeade
// give version info to each report entry;
// flip camera coordinates for report entries of old prone datasets.
//
// Revision 1.1.12.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 02:54:59  jmeade
// Camera (location) coordinates and original distance to rectum for each colon finding entry.
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
//    Rev 1.0   Sep 16 2001 17:27:38   ingmar
// Initial revision.
// Revision 1.2  2001/05/29 23:12:34  jmeade
// code conventions: changed all enum ??? to enum ???Enum
//
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.7  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.6  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.5  2001/02/07 16:24:51  geconomos
// Added revisions to report file
//
// Revision 1.4  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.3  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportEntry.C,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
// $Id: ReportEntry.C,v 1.1.12.2 2008/11/14 20:16:18 jmeade Exp $
