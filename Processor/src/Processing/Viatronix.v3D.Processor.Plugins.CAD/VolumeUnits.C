// $Id: VolumeUnits.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "VolumeUnits.h"

/**
 * Returns the units in x
 *
 * @param   pfx       upon return contains units in x
 * @return            S_OK for success
 */
STDMETHODIMP VolumeUnits::get_X( float * pfx )
{
	if( !pfx )
    return E_INVALIDARG;
  *pfx = m_units.m_x;

  return S_OK;
} // get_X()


/**
 * Returns the units in y
 *
 * @param   pfy       upon return contains units in y
 * @return            S_OK for success
 */
STDMETHODIMP VolumeUnits::get_Y( float * pfy )
{
	if( !pfy )
    return E_INVALIDARG;
  *pfy = m_units.m_y;

  return S_OK;
} // get_Y()


/**
 * Returns the units in z
 *
 * @param   pfz       upon return contains units in z
 * @return            S_OK for success
 */
STDMETHODIMP VolumeUnits::get_Z( float * pfz )
{
	if( !pfz )
    return E_INVALIDARG;
  *pfz = m_units.m_z;

  return S_OK;
} // get_Y()


// $Log: VolumeUnits.C,v $
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VolumeUnits.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
// $Id: VolumeUnits.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
