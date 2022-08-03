// $Id: VoxelPosition.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "VoxelPosition.h"
#include "vxCadVoxelPosition.h"

/**
 * Default Constructor
 */
VoxelPosition::VoxelPosition() : m_pVoxelPosition ( NULL )
{
} // VoxelPosition()


/**
 * Gets the X component
 *
 * @param   pfx       contains the x component upon return
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::get_X( int * pfx )
{
	if( !pfx)
    return E_INVALIDARG;
  *pfx = m_pVoxelPosition->GetX();

  return S_OK;
} // get_X()


/**
 * Sets the X component
 *
 * @param   fx        x component
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::put_X( int fx )
{
  m_pVoxelPosition->SetX( fx );
  return S_OK;
} // put_X()


/**
 * Gets the Y component
 *
 * @param   pfy       contains the y component upon return
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::get_Y( int * pfy )
{
	if( !pfy )
    return E_INVALIDARG;
  *pfy = m_pVoxelPosition->GetY();

  return S_OK;
} // get_Y()


/**
 * Sets the Y component
 *
 * @param   fy        y component
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::put_Y( int fy )
{
  m_pVoxelPosition->SetY( fy );
  return S_OK;
} // put_Y()


/**
 * Gets the Z component
 *
 * @param   pfz       contains the z component upon return
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::get_Z( int * pfz )
{
	if( !pfz )
    return E_INVALIDARG;
  *pfz = m_pVoxelPosition->GetZ();

  return S_OK;
} // get_Z()


/**
 * Sets the Z component
 *
 * @param   fz        z component
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::put_Z( int fz )
{
  m_pVoxelPosition->SetZ( fz );
  return S_OK;
} // put_Z()


/**
 * Sets the x, y and Z component
 *
 * @param   x        x component
 * @param   y        y component
 * @param   z        z component
 * @return            S_OK for success
 */
STDMETHODIMP VoxelPosition::Set( int x, int y, int z )
{
  m_pVoxelPosition->SetX( x );
  m_pVoxelPosition->SetY( y );
  m_pVoxelPosition->SetZ( z );
  return S_OK;
} // Set( float fx, float fy, float fz )


// $Log: VoxelPosition.C,v $
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VoxelPosition.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
// $Id: VoxelPosition.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
