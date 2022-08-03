// $Id: vxRaytracerSettings.C,v 1.12 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxRaytracerSettings.h"
#include "vxIntensityConverter.h"
#include "vxBlockVolumeIterator.h"
#include "vxUtils.h"
#include "SpaceLeapInfoMaker.h"
#include "vxRayGenerator.h"
#include "vxComponentArray.h"

#define FILE_REVISION "$Revision: 1.12 $"

const int4   Global_iSldBlockWidth = 16;
const float4 Global_fInterSampleDistance = 0.5;
const float4 Global_fKA = 0.20f;
const float4 Global_fKD = 0.70f;
const float4 Global_fKS = 0.30f;
const float4 Global_fSpecularLight1Power = 8.0f;


/**
 * Constructor
 */
vxRaytracerSettings::vxRaytracerSettings() : 
  m_bShadingEnabled(false ),
  m_bShadowingEnabled( false ),
  m_bJitteringEnabled( false ),
  m_bPreIntegrationEnabled( false ),
  m_bSampleEveryPixel( false ),
  m_bShowCutsShaded( false ),
  m_bShowCutsGrayscale( false ),
  m_bSliceShadowsEnabled( false ),
  m_sliceShadowColor( 255, 255, 0, 255 ),
  m_bClipSliceShadow( false ),
  m_bShowIntensityRange( false ),
  m_iIntensityRangeLow( 0 ),
  m_iIntensityRangeHigh( 0 ),
  m_iShift8Bit( 4 ),
  m_iVisibleMaskIntensity( 0 ),
  m_iVisibleMaskLabel( 0 ),
  m_bCropBoxActive( false ),
  m_bBoxOfInterestActive( NULL ),
  m_fQualitySampleDistanceScaleResolutionDependent( 1.0f ),
  m_bUseSpaceLeaping( true )
{
  m_illuminator.SetPhongConstants( Global_fKA, Global_fKD, Global_fKS, Global_fSpecularLight1Power );
  
  // initialize 1D transfer function
  // TODO: encapsulate color tables
  m_pfColormap1d                          = new float4[ 4*65536 ];
  m_pfColormap1dAlphaCorrected            = new float4[ 4*65536 ];
  m_pfColormap1d8bit                      = new float4[ 4*256 ];
  m_pfColormap1d8bitPreIntegrated         = new float4[ 5*256 ];
  m_pfColormap1d8bitAlphaCorrected        = new float4[ 4*256 ];
  m_pfColormap2d8bitPreIntegrated         = new float4[ 5*256*256 ];
  m_pfColormap2d8bitPreIntegratedOutOf1d  = new float4[ 5*256*256 ];

} // vxRaytracerSettings()


/**
 * Destructor
 */
vxRaytracerSettings::~vxRaytracerSettings()
{
  if( m_pfColormap1d != NULL )
    delete [] m_pfColormap1d;

  if( m_pfColormap1dAlphaCorrected != NULL )
    delete [] m_pfColormap1dAlphaCorrected;

  if( m_pfColormap1d8bit != NULL )
    delete [] m_pfColormap1d8bit;

  if( m_pfColormap1d8bitPreIntegrated != NULL )
    delete [] m_pfColormap1d8bitPreIntegrated;

  if( m_pfColormap1d8bitAlphaCorrected != NULL )
    delete [] m_pfColormap1d8bitAlphaCorrected;

  if( m_pfColormap2d8bitPreIntegrated != NULL )
    delete [] m_pfColormap2d8bitPreIntegrated;

  if( m_pfColormap2d8bitPreIntegratedOutOf1d != NULL )
    delete [] m_pfColormap2d8bitPreIntegratedOutOf1d;

} // ~vxRaytracerSettings()



/**
 * Sets the transfer function
 *
 * @param   volume      volume
 * @param   colomrap    colormap
 */
void vxRaytracerSettings::SetTransferFunction( vxBlockVolume< uint2 > & volume, vxColormap & colormap )
{
  const float4 fSampleDistance = Global_fInterSampleDistance * m_fQualitySampleDistanceScaleResolutionDependent;

  const int4 iMaxIndex( vxIntensityConverter::GetMaxValue( volume.GetHeader(), false ) );

  // create a copy of the original 1D lookup table and correct alpha values
  //////////////////////////////////////////////////////////////////////////////
  memcpy( m_pfColormap1d, colormap.GetTablePointerRGBA( volume.GetHeader() ), iMaxIndex * 16 );
  
  float4 * pfSrc( m_pfColormap1d );
  float4 * pfDest( m_pfColormap1dAlphaCorrected );

  for (int4 i(0); i<=iMaxIndex; i++)
  {
    *pfDest++ = *pfSrc++; // red
    *pfDest++ = *pfSrc++; // green
    *pfDest++ = *pfSrc++; // blue
    *pfDest++ = iMaxIndex * (1 - pow(1-(*pfSrc++/(float4)iMaxIndex), Global_fInterSampleDistance/fSampleDistance ) ); // alpha
  }

  // create a 8bit lookup table as well as an alpha corrected one
  /////////////////////////////////////////////////////////////////
  
  memcpy( m_pfColormap1d8bit, colormap.GetTablePointerRGBA(255), 256*16 );
  pfSrc  = m_pfColormap1d8bit;
  pfDest = m_pfColormap1d8bitAlphaCorrected;

  for( int4 i(0); i<=255; i++ )
  {
    *pfDest++ = *pfSrc++; // red
    *pfDest++ = *pfSrc++; // green
    *pfDest++ = *pfSrc++; // blue
    *pfDest++ = 1 - pow(1-(*pfSrc++/255.0F), Global_fInterSampleDistance/fSampleDistance); // alpha
  }

  for (int4 j(255); j>=0; --j)
  {
    for (int4 i(j-1); i>=0; --i)  FileScope_PreIntegrate2D8bit(j-1,i-1, -1, 1.0F/(j-i), m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + i)*5);
                                  FileScope_PreIntegrate2D8bit(j,  j+1, +1, 1.0F/(1),   m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + j)*5);
    for (int4 k(j+1); k<256; ++k) FileScope_PreIntegrate2D8bit(j+1,k+1, +1, 1.0F/(k-j), m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + k)*5);
  }

  /// This is for PET, SPECT, or MRI just in case the maximum value > 2^12
  m_iShift8Bit = log(static_cast<float4>(GetNextPowerOfTwo(iMaxIndex+1)))/log(2.0F) - 8.0F; 

} // UpdateTFunc2D()


/**
 * Sets the intenisty volume. (MAKES A COPY)
 *
 * @param   volume    intenisty volume
 */
void vxRaytracerSettings::SetIntensityVolume( vxBlockVolume< uint2 > & volume )
{
  try
  {
    m_intensityVolume.SetHeader( volume.GetHeader() );
    m_intensityVolume.Clear();

    vxBlockVolumeIterator<uint2> iterIntensityVolOrig( volume );
    vxBlockVolumeIterator<uint2> iterIntensityVolRender( m_intensityVolume );

    for (; iterIntensityVolOrig.IsNotAtEnd();  iterIntensityVolOrig.NextBlockZYX(), iterIntensityVolRender.NextBlockZYX() )
      iterIntensityVolRender.SetCurrentBlock( iterIntensityVolOrig.GetCurrentBlock() );
  }
  catch (...)
  {
    throw ex::VException(LogRec("SetIntensityVolume failed.", "vxRaytracer", "vxRaytracerSettings"));
  }
} // SetIntensityVolume( vxBlockVolume< uint2 > & volume )


/**
 * Sets the label volume. (MAKES A COPY)
 *
 * @param   labelVolume   label volume
 */
void vxRaytracerSettings::SetLabelVolume( vxBlockVolume< uint2 > & labelVolume )
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
  catch (...)
  {
    throw ex::VException(LogRec("UpdateRenderVolumeLabel failed.", "vxRaytracerSettings", "SetLabelVolume"));
  }
} // SetLabelVolume( vxBlockVolume< uint2 > & labelVolume )


/**
 * Sets the components. (MAKES A COPY)
 *
 * @param   components    components
 */
void vxRaytracerSettings::SetComponents( const vxComponentArray & components )
{
    m_components.resize( components.ArrayByLabel().size() );
    for( int4 k( 0 ); k < m_components.size(); ++k )
      components.ArrayByLabel()[ k ]->CopyTo( m_components[ k ], false );
} // SetComponents( const vxComponentArray & components )


/**
 * Calculates the box of interest based on the crop box and intensity volume
 *
 * @param   intensityVolume   volume
 * @param   cropbox           crop box  
 */
void vxRaytracerSettings::SetBoxOfInterest( vxBlockVolume< uint2 > & intensityVolume, vxCropBox & cropbox )
{
  try
  {
    const Box< float4 > & box( cropbox.GetBox() );

    // make sure we did not expand past the volume boundaries
    const Vector3D<float4> units( intensityVolume.GetHeader().GetVoxelUnits() );
    Point<float4> minPointVol, maxPointVol;
    minPointVol.m_x = box.GetMinX()/units.X();
    minPointVol.m_y = box.GetMinY()/units.Y();
    minPointVol.m_z = box.GetMinZ()/units.Z();
    maxPointVol.m_x = box.GetMaxX()/units.X();
    maxPointVol.m_y = box.GetMaxY()/units.Y();
    maxPointVol.m_z = box.GetMaxZ()/units.Z();

    m_fBoxOfInterest.SetMinPoint( minPointVol );
    m_fBoxOfInterest.SetMaxPoint( maxPointVol );
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateBoxOfInterest failed.", "vxRaytracer", "UpdateBoxOfInterest"));
  }
} // SetBoxOfInterest( vxBlockVolume< uint2 > & intensityVolume, vxCropBox & cropbox )


/**
 * Updates the volume for space leaping.
 */
void vxRaytracerSettings::UpdateSpaceLeapVolume()
{
  try
  {
    const int4 iSldBlockWidth = Global_iSldBlockWidth;

    /// make new space leap volume
    Triplei newDim( Triplei((Vector3Di( m_intensityVolume.GetHeader().GetVolDim()) - Vector3Di(1,1,1)) / iSldBlockWidth + Vector3Di(1,1,1))  );
    m_spaceLeapVolume.GetHeader().SetVolDim(newDim);
    m_spaceLeapVolume.Clear();
    SpaceLeapInfoMaker< int2 > slim;
    slim.ComputeInt1SpaceLeapRadius( m_intensityVolume, m_labelVolume, m_spaceLeapVolume, m_iVisibleMaskIntensity, m_iVisibleMaskLabel ); 
  }
  catch(...)
  {
    throw ex::VException(LogRec("UpdateVolumeSpaceLeap failed.", "vxRaytracerSettings", "UpdateVolumeSpaceLeap"));
  }
} // UpdateSpaceLeapVolume()


/**
 * Update the mask for intensities
 *
 * @param   intensity volume        volume
 * @param   colormap                colormap
 * @return whether mask has changed
 */
bool vxRaytracerSettings::UpdateMaskForIntensities( vxBlockVolume< uint2 > & intensityVolume, vxColormap & colormap )
{
  bool bUpdated = false;
  try
  {
    const int4 iMaxIndex( vxIntensityConverter::GetMaxValue( intensityVolume.GetHeader() ) );

    // update the class member mask
    const int4 iMaskIntensity = vxUtils::GenerateBitCodeForTransferFunction( colormap.GetTablePointerRGBA( intensityVolume.GetHeader() ), iMaxIndex );
    if (m_iVisibleMaskIntensity != iMaskIntensity)
    {
      m_iVisibleMaskIntensity = iMaskIntensity;
      bUpdated  = true;
    }
  }
  catch(...)
  {
    throw ex::VException(LogRec("UpdateMaskForIntensities failed.", "vxRaytracerSettings", "UpdateMaskForIntensities"));
  }
  return bUpdated;
} // UpdateMaskForIntensities()


/**
 * Updates the voxel mask for labels
 *
 * @param   components    component array
 */
bool vxRaytracerSettings::UpdateMaskForLabels( vxComponentArray & components )
{
  bool bUpdated = false;
  try
  {
    const int4 iMaskLabel = vxUtils::GenerateBitCodeComponentsVisible( components );
    if( m_iVisibleMaskLabel != iMaskLabel )
    {
      m_iVisibleMaskLabel = iMaskLabel;
      bUpdated =  true;
    }
  }
  catch(...)
  {
    throw ex::VException(LogRec("UpdateMaskForLabels failed.", "vxRaytracerSettings", "UpdateMaskForLabels"));
  }
  return bUpdated;
} // UpdateMaskForLabels( vxComponentArray & components )



// $Log: vxRaytracerSettings.C,v $
// Revision 1.12  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.11  2007/02/28 22:19:19  dongqing
// In SetTransferFunction, m_iShift8Bit equal to significantBits in volume
// this is for PET and MRI modalities.
//
// Revision 1.10  2007/01/25 21:53:09  romy
// Fixed the issue of loosing the component data between Tab switching. Calcium scoring was suffering from this with black blocks on Plaques
//
// Revision 1.9  2006/12/14 22:02:49  romy
// component data changes incorporated
//
// Revision 1.8  2006/09/21 14:59:06  geconomos
// switch datatype for components from td::vector<vxComponent*>to std::vector<vxComponent>
//
// Revision 1.7  2006/08/03 14:15:42  geconomos
// vxRayGenerator related changes
//
// Revision 1.6  2006/07/18 16:20:34  geconomos
// removed unused "SampleDistanceScale" instance field
//
// Revision 1.5  2006/07/12 19:52:09  geconomos
// + added segmentation intenisty range properties
// + added slice shadow related properties
//
// Revision 1.4  2006/07/07 13:43:52  geconomos
// added "render size" and "ray generator" class variables
//
// Revision 1.3  2006/06/27 20:14:32  geconomos
// making true copies of color tables from colormap
//
// Revision 1.2  2006/06/21 18:47:01  geconomos
// added vxRayGenerator instance pointer
//
// Revision 1.1  2006/06/20 20:06:09  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRaytracerSettings.C,v 1.12 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxRaytracerSettings.C,v 1.12 2007/03/02 19:17:42 geconomos Exp $
