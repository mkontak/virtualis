// $Id: ReportCollection.inl,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ReportCollection.inl
// Description:  
// Created:  
// Author: George geconomos@viatronix.net
//
// Complete History at bottom of file.
#include "stdafx.h"
#include "ReportLib.h"
#include "ReportCollection.h"


/**
 * read table of contents from stream object into list
 *
 * @param   bsName          name of stream that contains tableof contents
 * @return                  S_OK upon success
 */
template<class P,class PI, class T, class U,const IID* piid>
HRESULT ReportCollection<P,PI,T,U,piid>::ReadTOC(CComBSTR bsName)
{
  HRESULT hr(S_OK);
  
  // access CoClass of parent
  P* pParent = GetCoClass<P>(m_spParent);
  if(pParent==NULL)
    return E_POINTER;
  
  // try to open contents stream
  CComPtr<IStream> spStream;
  hr=OpenStream(pParent->GetStorage(), bsName, spStream, pParent->GetReadOnly());
  
  // if it failed for any other reason than "not found" report the error
  if(FAILED(hr) && hr!= STG_E_FILENOTFOUND)
    return hr;
  
  // it's okay if it's not found
  else if(hr == STG_E_FILENOTFOUND)
    return S_OK;
  
  // how many items are in the table of contents
  int4 iNumEntries(0);	
  spStream->Read(&iNumEntries,sizeof(iNumEntries), NULL);
  
  // read in each entry identifer
  for(int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  {    
    CComBSTR bs;
    if(FAILED(hr=bs.ReadFromStream(spStream)))
      return hr;
    
    // add it to the list
    if(FAILED(hr=AddEntry(bs, NULL)))
      return hr;    
  }
  
  return S_OK;
}


/**
 * writes out table of contents list to stream object
 *
 * @param   bsName          name of stream to contain table of contents
 * @return                  S_OK upon success
 */
template<class P,class PI, class T, class U,const IID* piid>
HRESULT ReportCollection<P,PI,T,U,piid>::WriteTOC(CComBSTR bsName)
{
  HRESULT hr(S_OK);
  
  // access CoClass of parent
  P* pParent = GetCoClass<P>(m_spParent);
  if(pParent==NULL)
    return E_POINTER;
  
  // create table of contents stream, overwriting existing
  CComPtr<IStream> spStream;
  if(FAILED(hr=CreateStream(pParent->GetStorage(), bsName, spStream)))
    return hr;
  
  // how many items do we have?  
  int4 iNumEntries = m_Toc.size();
  
  // write out count to disk
  if(FAILED(hr=spStream->Write(&iNumEntries, sizeof(iNumEntries), NULL)))
    return hr;
  
  // iterate through the list and write out each entry identifier
  for(TToc::iterator it = m_Toc.begin(); it != m_Toc.end(); it++)
   it->first.WriteToStream(spStream);

  // commit the changes at revison level
  if(FAILED(hr=pParent->GetStorage()->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

/**
 * adds an entry to the table of contents
 *
 * @param   bsEntryID         unique ID of collection item
 * @param   ppiEntry          collection item interface pointer
 * @return                    S_OK upon success
 */
template<class P,class PI, class T, class U,const IID* piid>
HRESULT ReportCollection<P,PI,T,U,piid>::AddEntry(CComBSTR bsEntryID, T** ppiEntry)
{
  HRESULT hr;
  
  // create a new instance of the collection item
  CComObject<U>* pEntry;
  if(FAILED(hr=CComObject<U>::CreateInstance(&pEntry)))
    return hr;
  
  // perform setup on the item
  if(FAILED(hr=pEntry->Setup(m_spParent, bsEntryID)))
    return hr;
  
  // query for smart pointer
  CComPtr<T> spEntry;
  if(FAILED(hr=pEntry->QueryInterface(*piid, reinterpret_cast<void**>(&spEntry))))
    return hr;
  
  // if requested, return interface
  if(ppiEntry)
    spEntry.CopyTo(ppiEntry);
  
  // add to list
  m_Toc.push_back(TTocItem(bsEntryID, spEntry));	
  
  return S_OK;
}


/**
 * verifies that an index into table of contents is valid
 *
 * @param   vIndex         unique ID of collection item
 * @return                 absoulut position within list if valid, -1 otherwise
 */
template<class P,class PI, class T, class U,const IID* piid>
int4 ReportCollection<P,PI,T,U,piid>::VerifyIndex(VARIANT& vIndex)
{
  int4 iIndex(-1);
  
  // index as an absolute position
  if(vIndex.vt == VT_I4)
  {
    iIndex = vIndex.lVal;
    
    // check that index is within bounds of the list
    if(iIndex < 0 || iIndex > m_Toc.size())
      return -1; // out of range
  }
  // index as ID
  else if(vIndex.vt == VT_BSTR)
  {
    // find the ID in the list
    iIndex = FindItem(vIndex.bstrVal);
    if(iIndex == -1)
      return -1;  // not found
  }
  
  return iIndex;
}

/**
 * returns list item based on absolute position
 *
 * @param   iIndex         unique ID of collection item
 * @return                 list item
 */
template<class P, class PI, class T, class U,const IID* piid>
typename ReportCollection<P,PI,T,U,piid>::TTocItem ReportCollection<P,PI,T,U,piid>::GetEntry(int4 iIndex)
{
  // advance and get desired list item
  TToc::iterator it = m_Toc.begin();
  
  std::advance(it, iIndex);
  
  return *it;
}

/**
 * returns list item based on Item ID
 *
 * @param   bsItemID       unique ID of collection item
 * @return                 list item
 */
template<class P,class PI, class T, class U,const IID* piid>
int4 ReportCollection<P,PI,T,U,piid>::FindItem(CComBSTR  bsItemID)
{
  TToc::iterator it = m_Toc.begin();

  // for each item in the list
  for(int4 iEntry(0); it != m_Toc.end(); iEntry++, it++)
  {
    // do the IDs mattch?
    if(it->first == bsItemID)
      return iEntry; // return the absolute position
  
  } // end for each item
  
  return -1;
}

/**
 * returns list item based on absolute position
 *
 * @param   iIndex         unique ID of collection item
 * @return                 list item
 */
template<class P,class PI, class T, class U,const IID* piid>
void ReportCollection<P,PI,T,U,piid>::RemoveEntry(int4 iIndex)
{
  // advance and get desired list item
  TToc::iterator it = m_Toc.begin();
  
  std::advance(it, iIndex);
  
  // remove it
  m_Toc.erase(it);
}

/**
 * frees the smart pointer for each item in the list and empties it
 */
template<class P,class PI, class T, class U,const IID* piid>
void ReportCollection<P,PI,T,U,piid>::Free()
{
  // free all items in list
  TToc::iterator it = m_Toc.begin();
  while(it != m_Toc.end())
  {
    it->second = NULL;
    it++;
  }

  m_Toc.clear();
}

// $Log: ReportCollection.inl,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/03/22 21:46:22  geconomos
// Migrated all projects to Visual Studio 2003!
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
//    Rev 1.0   Sep 16 2001 17:27:34   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.2  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.1  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportCollection.inl,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportCollection.inl,v 1.1 2005/08/05 12:55:55 geconomos Exp $