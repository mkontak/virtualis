// $Id: FindingDimension.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "FindingDimension.h"
#include "Vector.h"
#include "vxCadDimensions.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
FindingDimension::FindingDimension() : m_pDimension ( NULL )
{
} // Vector()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT FindingDimension::FinalConstruct()
{
  CComCreator< CComObject< Vector > >::CreateInstance( NULL, __uuidof( IVector ), reinterpret_cast< void ** >( &m_spStartPoint ) );
  CComCreator< CComObject< Vector > >::CreateInstance( NULL, __uuidof( IVector ), reinterpret_cast< void ** >( &m_spEndPoint ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void FindingDimension::FinalRelease()
{ 
  m_spStartPoint = NULL;
  m_spEndPoint = NULL;
} // FinalRelease()


/**
 * Sets the internal vxCadDimension pointer ( class does NOT assume ownership )
 *
 * @param   pDimension   vxCadDimension pointer
 */
void FindingDimension::SetInternalPtr( vxCadDimension * pDimension )
{ 
  m_pDimension = pDimension;
  GetCoClassPtr< Vector >( m_spStartPoint )->SetInternalPtr( const_cast< vxCadVector * >( &m_pDimension->GetStartPoint() ) );
  GetCoClassPtr< Vector >( m_spEndPoint )->SetInternalPtr( const_cast< vxCadVector * >( &m_pDimension->GetEndPoint() ) );
} // FinalRelease()


/**
 * Gets the start point
 *
 * @param   ppiStartPoint   upon return contains the start point
 * @return                  S_OK for success
 */
STDMETHODIMP FindingDimension::get_StartPoint( IVector ** ppiStartPoint )
{
	if( !ppiStartPoint )
    return E_INVALIDARG;
  return m_spStartPoint.QueryInterface( ppiStartPoint );
} // get_StartPoint()


/**
 * Gets the end point
 *
 * @param   ppiEndPoint   upon return contains the end point
 * @return                S_OK for success
 */
STDMETHODIMP FindingDimension::get_EndPoint( IVector ** ppiEndPoint )
{
	if( !ppiEndPoint )
    return E_INVALIDARG;
  return m_spEndPoint.QueryInterface( ppiEndPoint );
} // get_StartPoint()


// $Log: FindingDimension.C,v $
// Revision 1.2  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingDimension.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingDimension.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
