// $Id: vxPlanetracerSettings.C,v 1.8.8.2 2010/08/05 14:17:19 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)
//         

// incldues
#include "StdAfx.h"
#include "vxPlanetracerSettings.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"

// defines
#define FILE_REVISION "$Revision: 1.8.8.2 $"

/**
 * Constructor
 */
vxPlanetracerSettings::vxPlanetracerSettings() : 
  m_bShowLabels(false),
  m_bNeedLabels( false ),
  m_bShowColor(false),
  m_bShowThresholds(false),
  m_uThresholdLow(1200),
  m_uThresholdHigh(1400),
  m_iOffset(0),
  m_iMaximumOffset(0),
  m_iShiftFor8bitAlpha( 0 )
{
  m_pfRGBA = new float4[ 4*65536 ];
} // vxPlanetracerSettings()


/**
 * Destructor
 */
vxPlanetracerSettings::~vxPlanetracerSettings()
{
  if( m_pfRGBA )
    delete[] m_pfRGBA;

} // ~vxPlanetracerSettings()


/**
 * Sets the intenisty volume. (MAKES A COPY)
 *
 * @param   volume    intenisty volume
 */
void vxPlanetracerSettings::SetIntensityVolume( vxBlockVolume< uint2 > & volume )
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
    throw ex::VException(LogRec("SetIntensityVolume failed.", "vxPlanetracerSettings", "SetIntensityVolume"));
  }
} // SetIntensityVolume( vxBlockVolume< uint2 > & volume )


/**
 * Sets the label volume. (MAKES A COPY)
 *
 * @param   labelVolume   label volume
 */
void vxPlanetracerSettings::SetLabelVolume( vxBlockVolume< uint2 > & labelVolume )
{
  try
  {
    m_labelVolume.SetHeader( labelVolume.GetHeader() );
    m_labelVolume.Clear();
    vxBlockVolumeIterator<uint2> iterLabelOrig( labelVolume );
    vxBlockVolumeIterator<uint2> iterLabelRender( m_labelVolume );
    
    for (; iterLabelOrig.IsNotAtEnd();  iterLabelOrig.NextBlockZYX(), iterLabelRender.NextBlockZYX())
      iterLabelRender.SetCurrentBlock(iterLabelOrig.GetCurrentBlock());
  }
  catch(...)
  {
    throw ex::VException(LogRec("SetLabelVolume failed.", "vxPlanetracerSettings", "SetLabelVolume"));
  }
} // SetLabelVolume( vxBlockVolume< uint2 > & labelVolume )


/**
 * Sets the components. (MAKES A COPY)
 *
 * @param   components    components
 */
void vxPlanetracerSettings::SetComponents( const vxComponentArray & components )
{
  m_components.resize( components.ArrayByLabel().size() );
  for( int4 k( 0 ); k < m_components.size(); ++k )
    components.ArrayByLabel()[ k ]->CopyTo( m_components[ k ], false );
} // SetComponents( const vxComponentArray & components )


/**
 * Sets the transfer function
 *
 * @param   volume      intenisty volume
 * @param   colormap    colormap
 */
void vxPlanetracerSettings::SetTransferFunction( vxBlockVolume< uint2 > & volume , vxColormap & colormap )
{
  m_colormap = colormap;

  const int4 iMaxIndex =  vxIntensityConverter::GetMaxValue( volume.GetHeader(), false );

  memcpy( m_pfRGBA, colormap.GetTablePointerRGBA( volume.GetHeader() ), iMaxIndex * 16 );

  // compute shift factor
  m_iShiftFor8bitAlpha = log(static_cast<float4>(GetNextPowerOfTwo(iMaxIndex+1)))/log(2.0F) - 8.0F; 

} // SetTransferFunction( vxBlockVolume< uint2 > & volume , vxColormap & colormap )


// $Log: vxPlanetracerSettings.C,v $
// Revision 1.8.8.2  2010/08/05 14:17:19  dongqing
// fix the memory leak in vxColormap = operator. and roll back the previous
// fix in vxPlanetracerSetting.C/h
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.8.8.1  2010/08/05 13:50:30  dongqing
// fix the colormap memory leak (m_colormap)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.8  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.7  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.6  2007/02/22 16:39:20  geconomos
// code cleanup
//
// Revision 1.5  2007/01/25 21:53:09  romy
// Fixed the issue of loosing the component data between Tab switching. Calcium scoring was suffering from this with black blocks on Plaques
//
// Revision 1.4  2006/12/14 22:02:49  romy
// component data changes incorporated
//
// Revision 1.3  2006/09/21 14:58:33  geconomos
// switch datatype for components from vxComponentArray to std::vector<vxComponent>
//
// Revision 1.2  2006/08/03 14:16:30  geconomos
// cosmetics
//
// Revision 1.1  2006/07/21 14:59:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxPlanetracerSettings.C,v 1.8.8.2 2010/08/05 14:17:19 dongqing Exp $
// $Id: vxPlanetracerSettings.C,v 1.8.8.2 2010/08/05 14:17:19 dongqing Exp $
