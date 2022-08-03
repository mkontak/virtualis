// $Id: FindingDimensions.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "FindingDimensions.h"
#include "FindingDimension.h"
#include "vxCadDimensions.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
FindingDimensions::FindingDimensions() : m_pDimensions ( NULL )
{
} // Vector()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT FindingDimensions::FinalConstruct()
{
  CComCreator< CComObject< FindingDimension > >::CreateInstance( NULL, __uuidof( IFindingDimension ), reinterpret_cast< void ** >( &m_spLargestDimension ) );
  CComCreator< CComObject< FindingDimension > >::CreateInstance( NULL, __uuidof( IFindingDimension ), reinterpret_cast< void ** >( &m_spPerpendicularDimension ) );
  CComCreator< CComObject< FindingDimension > >::CreateInstance( NULL, __uuidof( IFindingDimension ), reinterpret_cast< void ** >( &m_spDepthDimension ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void FindingDimensions::FinalRelease()
{ 
  m_spLargestDimension = NULL;
  m_spPerpendicularDimension = NULL;
  m_spDepthDimension = NULL;
} // FinalRelease()


/**
 * Sets the internal vxCadDimensions pointer ( class does NOT assume ownership )
 *
 * @param   pDimension   vxCadDimensions pointer
 */
void FindingDimensions::SetInternalPtr( vxCadDimensions * pDimensions )
{ 
  m_pDimensions = pDimensions;
  GetCoClassPtr< FindingDimension >( m_spLargestDimension )->SetInternalPtr( const_cast< vxCadDimension * >( &m_pDimensions->GetLargestDimension() ) );
  GetCoClassPtr< FindingDimension >( m_spPerpendicularDimension )->SetInternalPtr( const_cast< vxCadDimension * >( &m_pDimensions->GetPerpendicularDimension() ) );
  GetCoClassPtr< FindingDimension >( m_spDepthDimension )->SetInternalPtr( const_cast< vxCadDimension * >( &m_pDimensions->GetDepthDimension() ) );
} // FinalRelease()


/**
 * Gets the largest dimension
 *
 * @param   ppiLargest      upon return contains the largest dimension
 * @return                  S_OK for success
 */
STDMETHODIMP FindingDimensions::get_Largest( IFindingDimension ** ppiLargest )
{
	if( !ppiLargest )
    return E_INVALIDARG;
  *ppiLargest = NULL;
  return m_spLargestDimension.QueryInterface( ppiLargest );
} // get_Largest()


/**
 * Gets the perpendicular dimension
 *
 * @param   ppiPerpendicular  upon return contains the perpendicular dimension
 * @return                    S_OK for success
 */
STDMETHODIMP FindingDimensions::get_Perpendicular( IFindingDimension ** ppiPerpendicular )
{
	if( !ppiPerpendicular )
    return E_INVALIDARG;
  *ppiPerpendicular = NULL;
  return m_spPerpendicularDimension.QueryInterface( ppiPerpendicular );
} // get_Largest()


/**
 * Gets the depth dimension
 *
 * @param   ppiDepth      upon return contains the depth dimension
 * @return                S_OK for success
 */
STDMETHODIMP FindingDimensions::get_Depth( IFindingDimension ** ppiDepth )
{
	if( !ppiDepth )
    return E_INVALIDARG;
  *ppiDepth = NULL;
  return m_spDepthDimension.QueryInterface( ppiDepth );
} // get_Depth()


// $Log: FindingDimensions.C,v $
// Revision 1.2  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingDimensions.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingDimensions.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
