// $Id: vxCurvetracerSettings.C,v 1.4 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxCurvetracerSettings.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"

#define FILE_REVISION "$Revision: 1.4 $"

/**
 * Constructor
 */
vxCurvetracerSettings::vxCurvetracerSettings() : 
  m_iShiftFor8bitAlpha( 0 )
{
  m_pfRGBA = new float4[ 4*65536 ];
} // vxCurvetracerSettings()


/**
 * Destructor
 */
vxCurvetracerSettings::~vxCurvetracerSettings()
{
  delete[] m_pfRGBA;
} // ~vxCurvetracerSettings()


/**
 * Sets the intenisty volume. (MAKES A COPY)
 *
 * @param   volume    intenisty volume
 */
void vxCurvetracerSettings::SetIntensityVolume( vxBlockVolume< uint2 > & volume )
{
  try
  {
    m_intensityVolume.SetHeader( volume.GetHeader() );
    m_intensityVolume.Clear();

    vxBlockVolumeIterator<uint2> iterIntensityVolOrig( volume );
    vxBlockVolumeIterator<uint2> iterIntensityVolRender( m_intensityVolume );

    for(; iterIntensityVolOrig.IsNotAtEnd();  iterIntensityVolOrig.NextBlockZYX(), iterIntensityVolRender.NextBlockZYX() )
      iterIntensityVolRender.SetCurrentBlock( iterIntensityVolOrig.GetCurrentBlock() );

  }
  catch(...)
  {
    throw ex::VException( LogRec( "SetIntensityVolume failed.", "vxCurvetracerSettings", "SetIntensityVolume"));
  }
} // SetIntensityVolume( vxBlockVolume< uint2 > & volume )


/**
 * Sets the transfer function
 *
 * @param   volume      volume
 * @param   colomrap    colormap
 */
void vxCurvetracerSettings::SetTransferFunction( vxBlockVolume< uint2 > & volume , vxColormap & colormap )
{
  m_colormap = colormap;

  const int4 iMaxIndex( vxIntensityConverter::GetMaxValue( volume.GetHeader(), false ) );

  memcpy( m_pfRGBA, colormap.GetTablePointerRGBA( volume.GetHeader() ), iMaxIndex * 16 );

  // compute shift factor
  m_iShiftFor8bitAlpha = log(static_cast<float4>(GetNextPowerOfTwo(iMaxIndex+1)))/log(2.0F) - 8.0F; 
} // SetTransferFunction( vxBlockVolume< uint2 > & volume, vxColormap & colormap )


// $Log: vxCurvetracerSettings.C,v $
// Revision 1.4  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.3  2007/03/02 18:11:55  geconomos
// further cleanup
//
// Revision 1.2  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCurvetracerSettings.C,v 1.4 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxCurvetracerSettings.C,v 1.4 2007/03/02 19:17:42 geconomos Exp $
