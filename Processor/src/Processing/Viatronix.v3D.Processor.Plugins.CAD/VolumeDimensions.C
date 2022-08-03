// $Id: VolumeDimensions.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "VolumeDimensions.h"

/**
 * Gets the x dimensions
 *
 * @param   pix       contains the x dimensions upon return
 * @return            S_OK for success
 */
STDMETHODIMP VolumeDimensions::get_X( long * pix )
{
	if( !pix )
    return E_INVALIDARG;
  *pix = m_dimensions.m_x;

  return S_OK;
} // get_X()


/**
 * Gets the y dimensions
 *
 * @param   piy       contains the y dimensions upon return
 * @return            S_OK for success
 */
STDMETHODIMP VolumeDimensions::get_Y( long * piy )
{
	if( !piy )
    return E_INVALIDARG;
  *piy = m_dimensions.m_y;

  return S_OK;
} // get_Y()


/**
 * Gets the z dimensions
 *
 * @param   piz       contains the z dimensions upon return
 * @return            S_OK for success
 */
STDMETHODIMP VolumeDimensions::get_Z( long * piz )
{
	if( !piz )
    return E_INVALIDARG;
  *piz = m_dimensions.m_z;

  return S_OK;
} // get_Y()


// $Log: VolumeDimensions.C,v $
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VolumeDimensions.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
// $Id: VolumeDimensions.C,v 1.1 2005/09/16 14:48:15 geconomos Exp $
