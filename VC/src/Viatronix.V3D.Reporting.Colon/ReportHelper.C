#include "stdafx.h"
#include "ReportLib.h"
#include "ReportHelper.h"
#include "ReportRevision.h"

/**
* writes a property to a stream with given length
*
* @param   bsName          name of Stream object
* @param   pvVal           property value
* @param   uSize           size of property to write in bytes
* @return                  S_OK upon success   
*/
HRESULT ReportHelper::WriteProperty(const BSTR bsName, void* pvVal, uint4 uSize)
{
  HRESULT hr;
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // write out the property to the stream
  if(FAILED(hr=spStream->Write(pvVal, uSize, NULL)))
    return hr;
  
  // commit the changes
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

/**
* reads a property from a stream with given length
*
* @param   bsName          name of Stream object
* @param   pvVal           buffer to hold property value
* @param   uSize           size of pvVal
* @return                  S_OK upon success   
*/
HRESULT ReportHelper::ReadProperty(const BSTR bsName, void* pvVal, uint4 uSize)
{
  HRESULT hr;
  
  if(!pvVal)  
    return E_POINTER;
  ZeroMemory(pvVal, uSize);
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // read the property from the stream
  if(FAILED(hr=spStream->Read(pvVal, uSize, NULL)))
    return hr;
  
  return S_OK;
}

/**
* writes a BSTR to a stream
*
* @param   bsName          name of Stream object
* @param   bsVal           BSTR to serialize
* @return                  S_OK upon success
*/
HRESULT ReportHelper::WriteStringProperty(const BSTR bsName, BSTR bsVal)
{
  HRESULT hr(S_OK);
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // use BSTR wrapper for easy serialization
  CComBSTR bs;
  bs.Attach(bsVal);
  hr=bs.WriteToStream(spStream);
  bs.Detach();
  
  // Did we write it out?
  if(FAILED(hr))
    return hr;
  
  // commit the changes
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

/**
* read a BSTR to a stream
*
* @param   bsName          name of Stream object
* @param   bsVal           BSTR to serialize
* @return                  S_OK upon success
*/
HRESULT ReportHelper::ReadStringProperty(const BSTR bsName, BSTR* pbsVal)
{
  HRESULT hr(S_OK);
  
  if(!pbsVal)
    return E_POINTER;
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;

  CComBSTR bs;
  if(SUCCEEDED(hr=GetPropertyStream(bsName, spStg, spStream, false )))
  {
    // read in property value
    if(FAILED(hr=bs.ReadFromStream(spStream)))
        return hr; 
  }
  // check if the stream has not been created
  else if( hr == STG_E_FILENOTFOUND )
    bs = "";
  else 
    return hr;

  // copy to in/out parameter
  if(FAILED(hr=bs.CopyTo(pbsVal)))
    return hr;
  
  return S_OK;
}

/**
* writes a Triple to a stream
*
* @param   bsName          name of Stream object
* @param   vTriple         variant containing triple
* @return                  S_OK upon success   
*/
HRESULT ReportHelper::WriteTripleProperty(const BSTR bsName, VARIANT vTriple)
{
  HRESULT hr(S_OK);
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // access the triple
    
    SafeArrayLock(vTriple.parray);
  
    // triple is a 1Dim array of floats
    float4 f(0.0F);
    LONG iIndex[1];
    
    // for each float
    for(iIndex[0]=0; iIndex[0] < 3; iIndex[0]++)
    {
      // get the element from the array
      SafeArrayGetElement(vTriple.parray, iIndex, (void*) &(f));
      
      // write out the element
      spStream->Write(&f, sizeof(float4), NULL);
    
    } // end for each float
  
    SafeArrayUnlock(vTriple.parray);  
  // unaccess the triple
    
    // commit the changes
    if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
      return hr;
    
    return S_OK;
}

/**
  * reads a Triple from a stream
  *
  * @param   bsName          name of Stream object
  * @param   pvTriple        contains Triple as safearray upon return
  * @return                  S_OK upon success   
  */
HRESULT ReportHelper::ReadTripleProperty(const BSTR bsName, VARIANT* pvTriple)
{
  HRESULT hr(S_OK); 
  
  if(!pvTriple)
    return E_POINTER;
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // create the safe array as 1Dim array of floats
  SAFEARRAYBOUND sab[1] = { {3,0} };
  LPSAFEARRAY psa = SafeArrayCreate(VT_R4, 1, sab);
  
  // lock array and populate
  SafeArrayLock(psa);
  
  float4 f(0.0F);
  LONG iIndex[1];
  
  // for each element in the array
  for(iIndex[0]=0; iIndex[0] < 3; iIndex[0]++)
  {
    // read from the stream
    spStream->Read(&f, sizeof(float4), NULL);
    
    // assign to the array
    SafeArrayPutElement(psa, iIndex, (void*) &(f));
    
  } // end for each element
  
  SafeArrayUnlock(psa);
  
  // set the variant
  pvTriple->vt = VT_ARRAY|VT_R4;
  pvTriple->parray = psa;  
  
  return S_OK;
}

/**
  * writes a Quaternion from a stream
  *
  * @param   bsName         name of Stream object
  * @param   vQuat          contains Quaternion as safearray upon return
  * @return                 S_OK upon success   
  */
HRESULT ReportHelper::WriteQuatProperty(const BSTR bsName, VARIANT vQuat)
{
  HRESULT hr;
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // access the triple
    SafeArrayLock(vQuat.parray);
  
    // quaternion is a 1Dim array of floats
    float f(0.0F);
    LONG iIndex[1];
    
    // for each element in the variant
    for(iIndex[0]=0; iIndex[0] < 4; iIndex[0]++)
    {
      // get the element from tha array
      SafeArrayGetElement(vQuat.parray, iIndex, (void*) &(f));
      
      // write out the element
      spStream->Write(&f, sizeof(float4), NULL);
    
    } // end for each element
  
    SafeArrayUnlock(vQuat.parray);  
  // unaccess the quaternion
  
  // commit the changes
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;
  
  return S_OK;
}

/**
  * reads a Quaternion from a stream
  *
  * @param   bsName        name of Stream object
  * @param   pvQuat        contains Quaternion as safearray upon return
  * @return                S_OK upon success   
  */
HRESULT ReportHelper::ReadQuatProperty(const BSTR bsName, VARIANT* pvQuat)
{
  HRESULT hr(S_OK); 
 
  if(!pvQuat)
    return E_POINTER;  
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // create the safe array as 1Dim array of floats
  SAFEARRAYBOUND sab[1] = { {4,0} };
  LPSAFEARRAY psa = SafeArrayCreate(VT_R4, 1, sab);
  
  // lock array and populate
  SafeArrayLock(psa);
  
  float4 f(0.0F);
  LONG iIndex[1];
  
  // for each element in the array
  for(iIndex[0]=0; iIndex[0] < 4; iIndex[0]++)
  {
    // read in the element from the stream
    spStream->Read(&f, sizeof(float4), NULL);
    
    // assign to the array 
    SafeArrayPutElement(psa, iIndex, (void*) &(f));
  
  } // end for each element
  
  SafeArrayUnlock(psa);
  
  // set the variant
  pvQuat->vt = VT_ARRAY|VT_R4;
  pvQuat->parray = psa;  
  
  return S_OK;
}

/**
  * writes a Quaternion from a stream
  *
  * @param   bsName         name of Stream object
  * @param   vQuat          contains Quaternion as safearray upon return
  * @return                 S_OK upon success   
  */
HRESULT ReportHelper::WriteMatrixProperty(const BSTR bsName, VARIANT newVal)
{
  HRESULT hr(S_OK); 
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // access the matrix
    SafeArrayLock(newVal.parray);
  
    // matrix is a 2Dim array of floats
    float4 f(0.0F);
    LONG iIndex[2];
    
    // for each element in the array
    for(iIndex[1]=0; iIndex[1] < 4; iIndex[1]++)
    {
      for(iIndex[0]=0; iIndex[0] < 4; iIndex[0]++)
      {
        // get the element from tha array
        SafeArrayGetElement(newVal.parray, iIndex, (void*) &(f));
        
        // write out the element
        spStream->Write(&f, sizeof(float), NULL);
      }
    } // end for each element
  
    SafeArrayUnlock(newVal.parray);  
  
   // unaccess the matrix
  
  // commit the changes
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;  
  
  return S_OK;
}

/**
  * reads a Matrix from a stream
  *
  * @param   bsName        name of Stream object
  * @param   pvMatrix      contains Matrix as safearray upon return
  * @return                S_OK upon success   
  */
HRESULT ReportHelper::ReadMatrixProperty(const BSTR bsName, VARIANT* pvMatrix)
{
  HRESULT hr(S_OK);
  if(!pvMatrix)
    return E_POINTER;
  
  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;
  
  // create the safe array as 2Dim array of floats
  SAFEARRAYBOUND sab[2] = { {4,0}, {4,0} };
  LPSAFEARRAY psa = SafeArrayCreate(VT_R4, 2, sab);
  
  // lock array and populate
  SafeArrayLock(psa);
  
  float4 f(0.0F);
  LONG iIndex[2];
  
  // ofr each element in the array
  for(iIndex[1]=0; iIndex[1] < 4; iIndex[1]++)
  {
    for(iIndex[0]=0; iIndex[0] < 4; iIndex[0]++)
    {
      // read in the element from the stream
      spStream->Read(&f, sizeof(float), NULL);
      
      // assgign to the array
      SafeArrayPutElement(psa, iIndex, (void*) &(f));
    }
  } // end for each element
  
  SafeArrayUnlock(psa);
  
  // set the variant
  pvMatrix->vt = VT_ARRAY|VT_R4;
  pvMatrix->parray = psa;
  
  return S_OK;
  
}

/**
* writes a set sized VARIANT to a stream
*
* @param   bsName          name of Stream object
* @param   vVariant        variant containing VARIANT
* @param   iNumElements    number of elements in variant array/to be read
* @return                  S_OK upon success   
*/
HRESULT ReportHelper::WriteVariantProperty(const BSTR bsName, VARIANT vVariant, int iNumElements)
{
  HRESULT hr(S_OK);

  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;

  // lock access the triple
  SafeArrayLock(vVariant.parray);
  
  // triple is a 1Dim array of floats
  float4 f(0.0F);
  LONG iIndex[1];
  
  // for each float
  for(iIndex[0]=0; iIndex[0] < iNumElements; iIndex[0]++)
  {
    // get the element from the array
    SafeArrayGetElement(vVariant.parray, iIndex, (void*) &(f));

    // write out the element
    spStream->Write(&f, sizeof(float4), NULL);

  } // end for each float

  // unlock access to the triple
  SafeArrayUnlock(vVariant.parray);  
    
  // commit the changes
  if(FAILED(hr=spStg->Commit(STGC_DEFAULT)))
    return hr;

  return S_OK;
}

/**
  * reads a set sized VARIANT from a stream
  *
  * @param   bsName          name of Stream object
  * @param   pvVariant       contains VARIANT as safearray upon return
  * @param   iNumElements    number of elements in variant array/to be read
  * @return                  S_OK upon success   
  */
HRESULT ReportHelper::ReadVariantProperty(const BSTR bsName, VARIANT* pvVariant, int iNumElements)
{
  HRESULT hr(S_OK); 

  if(!pvVariant)
    return E_POINTER;

  // get property stream
  CComPtr<IStorage> spStg;
  CComPtr<IStream> spStream;
  if(FAILED(hr=GetPropertyStream(bsName, spStg, spStream)))
    return hr;

  // create the safe array as 1Dim array of floats
  SAFEARRAYBOUND sab[1] = { {iNumElements,0} };
  LPSAFEARRAY psa = SafeArrayCreate(VT_R4, 1, sab);

  // lock array and populate
  SafeArrayLock(psa);

  float4 f(0.0F);
  LONG iIndex[1];

  // for each element in the array
  for(iIndex[0]=0; iIndex[0] < 3; iIndex[0]++)
  {
    // read from the stream
    spStream->Read(&f, sizeof(float4), NULL);
    
    // assign to the array
    SafeArrayPutElement(psa, iIndex, (void*) &(f));
    
  } // end for each element
  
  SafeArrayUnlock(psa);

  // set the variant
  pvVariant->vt = VT_ARRAY|VT_R4;
  pvVariant->parray = psa;  

  return S_OK;
}


/**
 * returns a stream and storage object for a given name
 *
 * @param   psName          name of Stream object
 * @param   spStg           smart pointer to parent storage
 * @param   spStream        smart pointer to return stream object
 * @param   bCreateStream   create stream (do NOT attempt to read from a newly created stream)
 * @return                  S_OK upon success
 */
HRESULT ReportHelper::GetPropertyStream(const CComBSTR& bsName, CComPtr<IStorage>& spStg, CComPtr<IStream>& spStream, bool bCreateStream )
{
  HRESULT hr(S_OK);
  
  if(FAILED(hr=GetEntryStorage(spStg)))
    return hr;
  
  // get revision coclass
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision == NULL)
    return E_FAIL;
  
  // open stream with desried access
  if(SUCCEEDED(hr=OpenStream(spStg, bsName, spStream, pRevision->GetReadOnly())))
    return S_OK;
  
  // should we create a stream? Do NOT read from a stream until you have written to it 
  if( bCreateStream )
  {
    // if the stream doesn't exist, create it
    if(FAILED(hr) && hr == STG_E_FILENOTFOUND)	
      hr = CreateStream(spStg, bsName, spStream); 
    return S_OK;
  }

  return hr;
}

// Revision History:
// $Log: ReportHelper.C,v $
// Revision 1.1.12.1  2009/08/24 22:25:35  gdavidson
// Issue #6322: Cannot read from newly created stream
//
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 02:54:40  jmeade
// Write any n-sized variant to stream.
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:38   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.4  2001/05/08 16:20:03  geconomos
// More coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportHelper.C,v 1.1.12.1 2009/08/24 22:25:35 gdavidson Exp $
// $Id: ReportHelper.C,v 1.1.12.1 2009/08/24 22:25:35 gdavidson Exp $
