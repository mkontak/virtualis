// $Id: FindingCollection.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "FindingCollection.h"
#include "Finding.h"
#include "Dictionary.h"
#include "vxCadFindings.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
FindingCollection::FindingCollection() 
{
} // Finding()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT FindingCollection::FinalConstruct()
{
  // vendor specific data
  CComCreator< CComObject< Dictionary > >::CreateInstance( NULL, __uuidof( IDictionary ), reinterpret_cast< void ** >( &m_spVendorSpecificData ) );
  GetCoClassPtr< Dictionary >( m_spVendorSpecificData )->SetInternalPtr( const_cast< Dictionary::DictionaryMap * >( &m_findings.GetVendorSpecificVariablesMap() ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void FindingCollection::FinalRelease()
{
  m_spVendorSpecificData = NULL;
} // FinalRelease()


/**
 * Gets the number of findings in the collection component
 *
 * @param     piCount   contains the count upon return
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::get_Count( int * piCount )
{
	if( !piCount )
    return E_INVALIDARG;
  *piCount = m_findings.GetCount();
  return S_OK;
} // get_Count( int * piCount )


/**
 * Gets the cad vendor
 *
 * @param     psVendor   upon return contains the vendor name
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::get_Vendor( BSTR * psVendor )
{
	if( !psVendor )
    return E_INVALIDARG;
  *psVendor = NULL;
  return CComBSTR( m_findings.GetCadVendor().c_str() ).CopyTo( psVendor ) ;
} // get_Vendor( BSTR * psVendor )


/**
 * Sets the cad vendor
 *
 * @param     sVendor  cad vendor
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::put_Vendor( BSTR  sVendor )
{
  std::string s( static_cast< const char * >( _bstr_t( sVendor ) ) );
  m_findings.SetCadVendor( s );
  return S_OK;
} // get_Vendor( BSTR * psVendor )


/**
 * Gets the cad api vendor version
 *
 * @param     psVersion   upon return contains the vendor api version
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::get_Version( BSTR * psVersion )
{
	if( !psVersion )
    return E_INVALIDARG;
  *psVersion = NULL;
  return CComBSTR( m_findings.GetCadApiVersion().c_str() ).CopyTo( psVersion ) ;
} // get_Version( BSTR * psVersion )


/**
 * Sets the cad api vendor version
 *
 * @param     sVersion  cad vendor
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::put_Version( BSTR  sVersion )
{
  std::string s( static_cast< const char * >( _bstr_t( sVersion ) ) );
  m_findings.SetCadApiVersion( s );
  return S_OK;
} // put_Version( BSTR  sVersion )


/**
 * Gets the vendor specific data
 *
 * @param   ppiVendorSpecificData   upon return contains an IDictionary interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP FindingCollection::get_VendorSpecificData( IDictionary ** ppiVendorSpecificData )
{
  if( !ppiVendorSpecificData )
    return E_INVALIDARG;
  *ppiVendorSpecificData= NULL;
  return m_spVendorSpecificData.QueryInterface( ppiVendorSpecificData );
} // get_VendorSpecificData( IDictionary ** ppiVendorSpecificData )


/**
 * Adds the voxel position to the collection
 *
 * @param     piFinding    finding
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::Add(  IFinding * piFinding  )
{
  CComPtr< IFinding > spFinding( piFinding );  
  m_findings.AddFinding( *GetCoClassPtr< Finding >( spFinding )->GetInternalPtr() );
  return S_OK;
} // Add(  IFinding * piPosition  )


/**
 * Adds the voxel position to the collection
 *
 * @param     iIndex       index of finding to return
 * @param     ppiFinding   upon return contains the voxel position
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::Get( int iIndex, IFinding ** ppiFinding )
{	
  return E_NOTIMPL;
} // Get( int iIndex, IFinding ** ppiFinding );


/**
 * Clears out the collection
 *
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::Clear()
{
  return E_NOTIMPL;
} // Clear()


/**
 * Removes the voxel position at the specified index
 *
 * @param     iIndex         index of voxel position to remove
 * @return    S_OK for success
 */
STDMETHODIMP FindingCollection::Remove( int iIndex  )
{
  return E_NOTIMPL;
} // Remove( int iIndex  );


/**
 * Creates a new Finding object and returns it's IFinding interface pointer
 *
 * @param   ppiFinding    upon return contains a IFinding interface pointer
 * @return                S_OK for success
 */
STDMETHODIMP FindingCollection::CreateFinding( IFinding ** ppiFinding )
{
	if( !ppiFinding )
    return E_INVALIDARG;
  *ppiFinding = NULL;
  return CComCreator< CComObject< Finding > >::CreateInstance( NULL, __uuidof( IFinding ), reinterpret_cast< void ** >( ppiFinding ) );
} // get_Depth()


/**
 * Writes out the findings in xml to the specified file
 *
 * @param   bsFilename  output filename
 * @return  S_OK for success
 */
STDMETHODIMP FindingCollection::ToFile( BSTR bsFilename )
{
  _bstr_t tmp( bsFilename );
  std::string sFilename( static_cast< const char * >( tmp ) ); 
  m_findings.ToFile( sFilename );
  return S_OK;
} // ToFile( BSTR sFilename )


// $Log: FindingCollection.C,v $
// Revision 1.5  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.2.1  2006/01/09 20:01:26  geconomos
// added vendor specific data to findings collection
//
// Revision 1.3  2005/10/11 13:23:11  geconomos
// added api version property
//
// Revision 1.2  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingCollection.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingCollection.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
