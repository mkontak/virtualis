// $Id: ReportImages.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportImages.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Images collection object
// Created: 2/1/2001
// Author: George Economos
//
// Revision History:
// $Log: ReportImages.C,v $
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
//    Rev 2.0   Sep 16 2001 23:40:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:40   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.7  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.6  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.5  2001/03/30 18:11:47  geconomos
// Added error check to ReadTOC() method.
//
// Revision 1.4  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.3  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.2  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "ReportRevision.h"
#include "ReportEntry.h"
#include "ReportImages.h"
#include "Image.h"

/////////////////////////////////////////////////////////////////////////////
// ReportImages


STDMETHODIMP ReportImages::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IReportImages
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////
// IReportEntries methods and properties
/////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP ReportImages::get_Item(VARIANT vIndex, IImage** ppiImage)
{
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;
  
  return GetImage( GetEntry(iIndex).first, ppiImage);
}

STDMETHODIMP ReportImages::get__NewEnum(IUnknown** ppiUnknown)
{
  return E_NOTIMPL;
}


STDMETHODIMP ReportImages::Add(IImage* piImage)
{
  if(!piImage)
    return E_POINTER;

  CComBSTR  bs = CreateIndentifier();

  HRESULT hr;
  if(FAILED(hr=PutImage(bs, piImage)))
    return hr;

  m_Toc.push_back( TTocItem(bs, 0) );
  
  return S_OK;
}

STDMETHODIMP ReportImages::Remove(VARIANT vIndex)
{
  HRESULT hr;
  
  int4 iIndex = VerifyIndex(vIndex);
  if(iIndex == -1)
    return E_INVALIDARG;

  ReportEntry* pEntry = GetCoClass<ReportEntry>(m_spEntry);
  if(pEntry == NULL)
    return E_POINTER;

  CComPtr<IStorage> spStg;
  if(FAILED(hr=pEntry->GetEntryStorage(spStg)))
    return hr;

  if(FAILED(hr=spStg->DestroyElement(GetEntry(iIndex).first)))
    return hr;
  
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;

  RemoveEntry(iIndex);
  
  return S_OK;
}


STDMETHODIMP ReportImages::get_Count(long *piCount)
{
  if(!piCount)
    return E_POINTER;
  
  *piCount = m_Toc.size();

  return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
// Implementation
/////////////////////////////////////////////////////////////////////////////////////

HRESULT ReportImages::Setup(CComPtr<IReportRevision>& spRevision, CComPtr<IReportEntry>& spEntry)
{
  m_spRevision = spRevision;
  m_spEntry = spEntry;

  HRESULT hr;
  if(FAILED(hr=ReadTOC()))
    return hr;
  
  return S_OK;
}

HRESULT ReportImages::Save()
{
  HRESULT hr;
  if(FAILED(hr=WriteTOC()))
    return hr;
  
  return S_OK;
}

HRESULT ReportImages::PutImage(CComBSTR  bsName, IImage* piImage)
{
  if(!piImage)
    return E_INVALIDARG;
  
  HRESULT hr;
  ReportEntry* pEntry = GetCoClass<ReportEntry>(m_spEntry);
  if(pEntry == NULL)
    return E_POINTER;
  
  CComPtr<IStorage> spParentStg;
  if(FAILED(hr=pEntry->GetEntryStorage(spParentStg)))
    return hr;
  
  CComPtr<IStorage> spStg;
  if(FAILED(hr=GetImageStorage(bsName, spParentStg, spStg)))
    return hr;
  
  CComPtr<IImage> spImage(piImage);
  Image* pImage = GetCoClass<Image>(spImage);
  if(pImage == NULL)
    return E_POINTER;
  
  if(FAILED(hr=pImage->Setup(m_spRevision, bsName)))
    return hr;
  
  if(FAILED(hr=pImage->WriteBitmap(spStg)))
    return hr;
  
  if(FAILED(hr=spParentStg->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

HRESULT ReportImages::GetImage(CComBSTR bsName, IImage** ppiImage)
{
  HRESULT hr;
  
  if(!ppiImage)
    return E_POINTER;
  *ppiImage = NULL;
  
  CComObject<Image>* pImage;
  if(FAILED(hr=CComObject<Image>::CreateInstance(&pImage)))
    return hr;
  
  if(FAILED(hr=pImage->Setup(m_spRevision, bsName)))
    return hr;
  
  ReportEntry* pEntry = GetCoClass<ReportEntry>(m_spEntry);
  if(pEntry==NULL)
    return E_POINTER;
  
  CComPtr<IStorage> spParentStg;
  if(FAILED(hr=pEntry->GetEntryStorage(spParentStg)))
    return hr;
  
  CComPtr<IStorage> spStg;
  if(FAILED(hr=GetImageStorage(bsName, spParentStg, spStg)))
    return hr;
  
  if(FAILED(hr=pImage->ReadBitmap(spStg)))
    return hr;
  
  if(FAILED(hr=pImage->QueryInterface(IID_IImage, reinterpret_cast<void**>(ppiImage))))
    return hr;
  
  return S_OK;
}

HRESULT ReportImages::GetImageStorage(CComBSTR bsName, CComPtr<IStorage>& spParentStg, CComPtr<IStorage>& spStg)
{
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision==NULL)
    return E_POINTER;
  
  HRESULT hr;
  hr = OpenStorage(spParentStg, bsName, spStg, pRevision->GetReadOnly());
  if(SUCCEEDED(hr))
    return S_OK;
  
  if(FAILED(hr) && hr == STG_E_FILENOTFOUND)	
    hr = CreateStorage(spParentStg, bsName, spStg);
  
  return hr;
}


HRESULT ReportImages::ReadTOC()
{
  HRESULT hr;
     
  ReportEntry* pEntry = GetCoClass<ReportEntry>(m_spEntry);
  if(pEntry == NULL)
    return E_POINTER;

  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision == NULL)
    return E_POINTER;
	
	CComPtr<IStorage> spEntryStg;
  if(FAILED(hr=pEntry->GetEntryStorage(spEntryStg)))
    return hr;

  CComPtr<IStream> spStream;
  hr = OpenStream(spEntryStg, TITLE_COLON_FINDINGS, spStream, pRevision->GetReadOnly());
	
  if(hr==STG_E_FILENOTFOUND)
    return S_OK;
  
  if(FAILED(hr))
		return hr;
	
	int4 iNumEntries(0);
	spStream->Read(&iNumEntries, sizeof(iNumEntries), NULL);

	for(int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  {		
		CComBSTR bs;
    if(FAILED(hr=bs.ReadFromStream(spStream)))
      return hr;
    
    m_Toc.push_back(TTocItem(bs, 0));    
	}
	
	return S_OK;
}


HRESULT ReportImages::WriteTOC()
{
  HRESULT hr;
  ReportEntry* pEntry = GetCoClass<ReportEntry>(m_spEntry);
  if(pEntry == NULL)
    return E_POINTER;
  
  CComPtr<IStorage> spEntryStg;
  if(FAILED(hr=pEntry->GetEntryStorage(spEntryStg)))
    return hr;
  
  CComPtr<IStream> spStream;
  if(FAILED(hr=CreateStream(spEntryStg, TITLE_COLON_FINDINGS, spStream)))
    return hr;
  
	long nNumEntries = m_Toc.size();
	spStream->Write(&nNumEntries,sizeof(nNumEntries),NULL);

  for(TToc::iterator it = m_Toc.begin(); it != m_Toc.end(); it++)
    it->first.WriteToStream(spStream);
    

  if(FAILED(hr=spEntryStg->Commit(STGC_DEFAULT)))
    return hr;
	
	return S_OK;
}
int4 ReportImages::VerifyIndex(VARIANT& v)
{
  int4 iIndex(-1);
  
  if(v.vt == VT_I4)
  {
    iIndex = v.lVal;
    
    if(iIndex < 0 || iIndex > m_Toc.size())
      return -1;
  }
  else if(v.vt == VT_BSTR)
  {
    iIndex = FindItem(v.bstrVal);
    if(iIndex == -1)
      return -1;
  }
  
  return iIndex;
}

ReportImages::TTocItem ReportImages::GetEntry(int4 iIndex)
{
  TToc::iterator it = m_Toc.begin();
  for(int4 iEntry(0); iEntry < iIndex; iEntry++, it++);
  
  return *it;
}

void ReportImages::RemoveEntry(int4 iIndex)
{
  TToc::iterator it = m_Toc.begin();
  for(int4 iEntry(0); iEntry < iIndex; iEntry++, it++);
  
  m_Toc.erase(it);
}

int4 ReportImages::FindItem(CComBSTR bs)
{
  TToc::iterator it = m_Toc.begin();
  for(int iEntry(0); it != m_Toc.end(); iEntry++, it++)
  {
    if(it->first == bs)
      return iEntry;
  }
  
  return -1;
}

// $Log: ReportImages.C,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:56  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.7  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.6  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// Revision 1.5  2001/03/30 18:11:47  geconomos
// Added error check to ReadTOC() method.
//
// Revision 1.4  2001/02/07 16:24:52  geconomos
// Added revisions to report file
//
// Revision 1.3  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.2  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportImages.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportImages.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
