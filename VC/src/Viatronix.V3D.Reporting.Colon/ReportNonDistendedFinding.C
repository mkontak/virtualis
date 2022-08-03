// $Id: ReportNonDistendedFinding.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportNonDistendedFinding.h"
#include "ReportRevision.h"
#include "Image.h"


STDMETHODIMP ReportNonDistendedFinding::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportNonDistendedFinding
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
STDMETHODIMP ReportNonDistendedFinding::get_ID(BSTR* pbsID)
{
  if(!pbsID)
    return E_POINTER;

  return m_bsEntryID.CopyTo(pbsID);
}

/**
 * gets the Quaternion property for the entry
 *
 * @param   pvQuaternion    contains Quaternion property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Quaternion(VARIANT* pvQuaternion)
{
  return ReadQuatProperty(NONDISTENDED_PROPERTY_QUATERNION, pvQuaternion);
}

/**
 * sets the Quaternion property for the entry
 *
 * @param   vQuaternion     new Quaternion value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Quaternion(VARIANT vQuaternion)
{
  return WriteQuatProperty(NONDISTENDED_PROPERTY_QUATERNION, vQuaternion);
}


/**
 * gets the Rotation Matrix property for the entry
 *
 * @param   pvMatrix        contains Rotation Matrix property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_RotationMatrix(VARIANT* pvMatrix)
{
  return ReadMatrixProperty(NONDISTENDED_PROPERTY_MATRIX, pvMatrix);
}


/**
 * sets the Rotation Matrix property for the entry
 *
 * @param   vMatrix         new Rotation Matrix value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_RotationMatrix(VARIANT vMatrix)
{
  return WriteMatrixProperty(NONDISTENDED_PROPERTY_MATRIX, vMatrix);
}

/**
 * gets the Position property for the entry
 *
 * @param   pvPosition      contains Position property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Position(VARIANT* pvPosition)
{
  return ReadTripleProperty(NONDISTENDED_PROPERTY_POSITION, pvPosition);
}

/**
 * sets the Position property for the entry
 *
 * @param   vPosition       new Position value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Position(VARIANT vPosition)
{
  return WriteTripleProperty(NONDISTENDED_PROPERTY_POSITION, vPosition);
}

/**
 * gets the Direction property for the entry
 *
 * @param   pvDirection     contains Direction property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Direction(VARIANT* pvDirection)
{
  return ReadTripleProperty(NONDISTENDED_PROPERTY_DIRECTION, pvDirection);
}

/**
 * sets the Direction property for the entry
 *
 * @param   vDirection      new Direction value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Direction(VARIANT vDirection)
{
  return WriteTripleProperty(NONDISTENDED_PROPERTY_DIRECTION, vDirection);
}

/**
 * gets the Zoom Factor property for the entry
 *
 * @param   pfZoomFactor    contains Zoom Factor property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_ZoomFactor(float* pfZoomFactor)
{
  return ReadProperty(NONDISTENDED_PROPERTY_ZOOM, pfZoomFactor, sizeof(float));
}

/**
 * sets the Zoom Factor property for the entry
 *
 * @param   fZoomFactor     new Zoom Factor value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_ZoomFactor(float fZoomFactor)
{
  return WriteProperty(NONDISTENDED_PROPERTY_ZOOM, &fZoomFactor, sizeof(float));
}

/**
 * gets the ViewMode property for the entry
 *
 * @param   pnViewMode      contains View mode property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_ViewMode(long* pnViewMode)
{
  return ReadProperty(NONDISTENDED_PROPERTY_VIEWMODE, pnViewMode, sizeof(long));
}


/**
 * sets the View Mode property for the entry
 *
 * @param   nViewMode       new View Mode value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_ViewMode(long nViewMode)
{
  return WriteProperty(NONDISTENDED_PROPERTY_VIEWMODE, &nViewMode, sizeof(long));
}

/**
 * gets the Text property for the entry
 *
 * @param   pbsText         contains Text property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Text(BSTR* pbsText)
{
  return ReadStringProperty(NONDISTENDED_PROPERTY_TEXT, pbsText);
}

/**
 * sets the text property for the entry
 *
 * @param   bsText          new Text value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Text(BSTR bsText)
{
  return WriteStringProperty(NONDISTENDED_PROPERTY_TEXT, bsText);
}

/**
 * gets the LocationEnum property for the entry
 *
 * @param   pnLocation      contains LocationEnum property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Location(LocationEnum* pnLocation)
{
  return ReadProperty(NONDISTENDED_PROPERTY_LOCATION, pnLocation, sizeof(LocationEnum));
}

/**
 * sets the LocationEnum property for the entry
 *
 * @param   nLocation       new LocationEnum value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Location(LocationEnum nLocation)
{
  return WriteProperty(NONDISTENDED_PROPERTY_LOCATION, &nLocation, sizeof(LocationEnum));
}

/**
 * gets the TransferX property for the entry
 *
 * @param   pfTransferX     contains TransferX property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_TransferX(float* pfTransferX)
{
  return ReadProperty(NONDISTENDED_PROPERTY_TRANSFERX, pfTransferX, sizeof(float));
}

/**
 * sets the TransferX property for the entry
 *
 * @param   fTransferX      new TransferX value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_TransferX(float fTransferX)
{
  return WriteProperty(NONDISTENDED_PROPERTY_TRANSFERX, &fTransferX, sizeof(float));
}

/**
 * gets the TransferY property for the entry
 *
 * @param  pfTransferY      contains TransferY property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_TransferY(float* pfTransferY)
{
  return ReadProperty(NONDISTENDED_PROPERTY_TRANSFERY, pfTransferY, sizeof(float));
}

/**
 * sets the TransferY property for the entry
 *
 * @param   fTransferY      new TransferY value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_TransferY(float fTransferY)
{
  return WriteProperty(NONDISTENDED_PROPERTY_TRANSFERY, &fTransferY, sizeof(float));
}

/**
 * gets the Study Type property for the entry
 *
 * @param   pnStudyType     contains Study Type property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_DatasetPositionEnumProp(DatasetPositionEnumProp* peStudyType)
{
  return ReadProperty(NONDISTENDED_PROPERTY_TYPE, peStudyType, sizeof(DatasetPositionEnumProp));
}

/**
 * sets the Study Type property for the entry
 *
 * @param   nStudyType      new Study Type value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_DatasetPositionEnumProp(DatasetPositionEnumProp eStudyType)
{
  return WriteProperty(NONDISTENDED_PROPERTY_TYPE, &eStudyType, sizeof(DatasetPositionEnumProp));
}

/**
 * gets the Offset property for the entry
 *
 * @param   pfOffset    ontains Offset property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Offset(float* pfOffset)
{
  return ReadProperty(NONDISTENDED_PROPERTY_OFFSET, pfOffset, sizeof(float));
}

/**
 * sets the Offset property for the entry
 *
 * @param   fOffset         new Offset value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Offset(float fOffset)
{
  return WriteProperty(NONDISTENDED_PROPERTY_OFFSET, &fOffset, sizeof(float));
}

/**
 * gets the Start Point property for the entry
 *
 * @param   pvStartPoint    contains Start Point property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_StartPoint(VARIANT* pvStartPoint)
{
  return ReadTripleProperty(NONDISTENDED_PROPERTY_START_POINT, pvStartPoint);
}

/**
 * sets the Start Point property for the entry
 *
 * @param   vStartPoint     new Start Point value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_StartPoint(VARIANT vStartPoint)
{
  return WriteTripleProperty(NONDISTENDED_PROPERTY_START_POINT, vStartPoint);
}

/**
 * gets the Quaternion property for the entry
 *
 * @param   pvQuaternion    ontains Quaternion property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_EndPoint(VARIANT *pVal)
{
  return ReadTripleProperty(NONDISTENDED_PROPERTY_END_POINT, pVal);
}

/**
 * sets the End Point property for the entry
 *
 * @param   vEndPoint       new End Point value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_EndPoint(VARIANT vEndPoint)
{
  return WriteTripleProperty(NONDISTENDED_PROPERTY_END_POINT, vEndPoint);
}

/**
 * gets the Image property for the entry
 *
 * @param   ppiImage        contains Image property upon return 
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::get_Image(IImage** ppiImage)
{
  HRESULT hr;
  
  if(!ppiImage)
    return E_POINTER;
  *ppiImage = NULL;
  
  CComObject<Image>* pImage;
  if(FAILED(hr=CComObject<Image>::CreateInstance(&pImage)))
    return hr;

  if(FAILED(hr=pImage->Setup(m_spRevision,CComBSTR("Image"))))
    return hr;

  CComPtr<IStorage> spStg;
  if(FAILED(hr=GetEntryStorage(spStg)))
    return hr;

  if(FAILED(hr=pImage->ReadBitmap(spStg)))
    return hr;
  
  if(FAILED(hr=pImage->QueryInterface(IID_IImage, reinterpret_cast<void**>(ppiImage))))
    return hr;  
  
  return S_OK;
}

/**
 * sets the Image property for the entry
 *
 * @param   piImage         new Image value
 * @return                  S_OK upon success
 */
STDMETHODIMP ReportNonDistendedFinding::put_Image(IImage* piImage)
{
  HRESULT hr;
  
  CComPtr<IStorage> spStg;
  if(FAILED(hr=GetEntryStorage(spStg)))
    return hr;
  
  CComPtr<IImage> spImage(piImage);
  Image* pImage = GetCoClass<Image>(spImage);
  if(pImage==NULL)
    return E_POINTER;
  
  if(FAILED(hr=pImage->WriteBitmap(spStg)))
    return hr;
  
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;

  return S_OK;
}

/**
 * object setup routine
 *
 * @param   spRevision      smart pointer to parent revision object
 * @param   bsEntryID       unique entry ID
 * @return                  S_OK upon success
 */
HRESULT ReportNonDistendedFinding::Setup(CComPtr<IReportRevision>& spRevision, CComBSTR  bsEntryID)
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
HRESULT ReportNonDistendedFinding::GetEntryStorage(CComPtr<IStorage>& spStg)
{
  HRESULT hr(S_OK);
  
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision==NULL)
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

// $Log: ReportNonDistendedFinding.C,v $
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
//    Rev 1.0   Sep 16 2001 17:27:44   ingmar
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportNonDistendedFinding.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportNonDistendedFinding.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $


