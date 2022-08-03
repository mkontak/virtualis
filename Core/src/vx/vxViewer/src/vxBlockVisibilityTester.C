// $Id: vxBlockVisibilityTester.C,v 1.9 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxBlockVisibilityTester.h"
#include "vxEnvironment.h"
#include "vxIntensityConverter.h"
#include "vxUtils.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxBlockVisibilityTester::vxBlockVisibilityTester( vxEnvironment & environment )
: m_pVolIterIntensity( NULL ),
m_pVolIterLabel( NULL )
{

  // don't initialize if the data is not ready yet
  if ( environment.GetIntensityVolume() != NULL && environment.GetLabelVolume() != NULL )
  {
    m_pVolIterIntensity = new vxBlockVolumeIterator<uint2>( *environment.GetIntensityVolume() );
    m_pVolIterLabel     = new vxBlockVolumeIterator<uint2>( *environment.GetLabelVolume() );
  }
  else
  {
    throw ex::VException( LogRec( "Intensity or label volume histogram is null", "vxBlockVisibilityTester", "constructor" ) );
  }

  m_bShowLabel = environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );

  // set up intentsity mask
  vxVolumeHeader & header( environment.GetIntensityVolume()->GetHeader() );
  int4 iMaxValue( vxIntensityConverter::GetMaxValue( header, false ) );
  m_iMaskIntensity = vxUtils::GenerateBitCodeForTransferFunction( environment.GetColormap()->GetTablePointerRGBA( header ), iMaxValue );

  // set up label masks

  // is the intensity visible for the labels
  m_iMaskLabel = vxUtils::GenerateBitCodeComponentsVisible(*environment.GetComponents());

  // is the label color visible for the labels
  m_iMaskLabelColor = vxUtils::GenerateBitCodeLabelColorVisible(*environment.GetComponents());

  // check for intensity range
  m_iMaskSegmentationIntensityRange = 0;

  if ( environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) )
  {
    int4 iLowRaw  = vxIntensityConverter::ModalityToRaw(  environment.GetInteractiveSegmentation()->GetRegionIntensityLow (), header );
    int4 iHighRaw = vxIntensityConverter::ModalityToRaw( environment.GetInteractiveSegmentation()->GetRegionIntensityHigh(), header );

    m_iMaskSegmentationIntensityRange = vxUtils::GenerateBitCodeForInterval( iLowRaw, iHighRaw );
  }

} // vxBlockVisibilityTester()


/**
 * Constructor
 *
 * @param 
 */
vxBlockVisibilityTester::vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                                                  vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                                                  vxComponentArray * pComponentArray,
                                                  InteractiveSegmentation * pInteractiveSegmentation )
: m_pVolIterIntensity( NULL ),
m_pVolIterLabel( NULL )
{

  // don't initialize if the data is not ready yet
  if ( pIntensityVolume != NULL && pLabelVolume != NULL )
  {
    m_pVolIterIntensity = new vxBlockVolumeIterator<uint2>( * pIntensityVolume );
    m_pVolIterLabel     = new vxBlockVolumeIterator<uint2>( * pLabelVolume );
  }
  else
  {
    throw ex::VException( LogRec( "Intensity or label volume histogram is null", "vxBlockVisibilityTester", "constructor" ) );
  }

  m_bShowLabel = pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );

  // set up intentsity mask
  vxVolumeHeader & header( pIntensityVolume->GetHeader() );
  int4 iMaxValue( vxIntensityConverter::GetMaxValue( header, false ) );
  m_iMaskIntensity = vxUtils::GenerateBitCodeForTransferFunction( pColormap->GetTablePointerRGBA( header ), iMaxValue );

  // set up label masks

  // is the intensity visible for the labels
  m_iMaskLabel = vxUtils::GenerateBitCodeComponentsVisible( * pComponentArray );

  // is the label color visible for the labels
  m_iMaskLabelColor = vxUtils::GenerateBitCodeLabelColorVisible( * pComponentArray );

  // check for intensity range
  m_iMaskSegmentationIntensityRange = 0;

  if ( pRenderingMode->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) )
  {
    int4 iLowRaw  = vxIntensityConverter::ModalityToRaw( pInteractiveSegmentation->GetRegionIntensityLow (), header );
    int4 iHighRaw = vxIntensityConverter::ModalityToRaw( pInteractiveSegmentation->GetRegionIntensityHigh() , header );

    m_iMaskSegmentationIntensityRange = vxUtils::GenerateBitCodeForInterval( iLowRaw, iHighRaw );
  }

} // vxBlockVisibilityTester()

/**
 * Constructor
 *
 * @param 
 */
vxBlockVisibilityTester::vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                                                  vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                                                  vxComponentArray * pComponentArray,
                                                  int4 iSegmentationLow, int4 iSegmentationHigh )
: m_pVolIterIntensity( NULL ),
m_pVolIterLabel( NULL )
{

  // don't initialize if the data is not ready yet
  if ( pIntensityVolume != NULL && pLabelVolume != NULL )
  {
    m_pVolIterIntensity = new vxBlockVolumeIterator<uint2>( * pIntensityVolume );
    m_pVolIterLabel     = new vxBlockVolumeIterator<uint2>( * pLabelVolume );
  }
  else
  {
    throw ex::VException( LogRec( "Intensity or label volume histogram is null", "vxBlockVisibilityTester", "constructor" ) );
  }

  m_bShowLabel = pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );

  // set up intentsity mask
  vxVolumeHeader & header( pIntensityVolume->GetHeader() );
  int4 iMaxValue( vxIntensityConverter::GetMaxValue( header, false ) );
  m_iMaskIntensity = vxUtils::GenerateBitCodeForTransferFunction( pColormap->GetTablePointerRGBA( header ), iMaxValue );

  // set up label masks

  // is the intensity visible for the labels
  m_iMaskLabel = vxUtils::GenerateBitCodeComponentsVisible( * pComponentArray );

  // is the label color visible for the labels
  m_iMaskLabelColor = vxUtils::GenerateBitCodeLabelColorVisible( * pComponentArray );

  // check for intensity range
  m_iMaskSegmentationIntensityRange = 0;

  if ( pRenderingMode->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) )
  {
    int4 iLow ( iSegmentationLow );
    int4 iHigh( iSegmentationHigh );

    m_iMaskSegmentationIntensityRange = vxUtils::GenerateBitCodeForInterval( iLow, iHigh );
  }

} // vxBlockVisibilityTester()


/**
 * Constructor
 *
 * @param 
 */
vxBlockVisibilityTester::vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                                                  vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                                                  const std::vector< vxComponent > & components,
                                                  int4 iSegmentationLow, int4 iSegmentationHigh )
: m_pVolIterIntensity( NULL ),
m_pVolIterLabel( NULL )
{

  // don't initialize if the data is not ready yet
  if ( pIntensityVolume != NULL && pLabelVolume != NULL )
  {
    m_pVolIterIntensity = new vxBlockVolumeIterator<uint2>( * pIntensityVolume );
    m_pVolIterLabel     = new vxBlockVolumeIterator<uint2>( * pLabelVolume );
  }
  else
  {
    throw ex::VException( LogRec( "Intensity or label volume histogram is null", "vxBlockVisibilityTester", "constructor" ) );
  }

  m_bShowLabel = pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );

  // set up intentsity mask
  vxVolumeHeader & header( pIntensityVolume->GetHeader() );
  int4 iMaxValue( vxIntensityConverter::GetMaxValue( header, false ) );
  m_iMaskIntensity = vxUtils::GenerateBitCodeForTransferFunction( pColormap->GetTablePointerRGBA( header ), iMaxValue );

  // set up label masks

  // is the intensity visible for the labels
  m_iMaskLabel = vxUtils::GenerateBitCodeComponentsVisible( components );

  // is the label color visible for the labels
  m_iMaskLabelColor = vxUtils::GenerateBitCodeLabelColorVisible( components );

  // check for intensity range
  m_iMaskSegmentationIntensityRange = 0;

  if ( pRenderingMode->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) )
  {
    int4 iLow ( iSegmentationLow );
    int4 iHigh( iSegmentationHigh );

    m_iMaskSegmentationIntensityRange = vxUtils::GenerateBitCodeForInterval( iLow, iHigh );
  }

} // vxBlockVisibilityTester()


/**
 * Destructor
 */
vxBlockVisibilityTester::~vxBlockVisibilityTester()
{
  delete m_pVolIterIntensity;
  delete m_pVolIterLabel;
} // ~vxBlockVisibilityTester()


/**
 * Determine if the intensity of any voxel in the block is visible.
 * Can be invisible because of current transfer function 
 * or because of all labels in the block being set to invisible intensity
 *
 * @param volumePosition volume location (voxel) of interest
 * @return               visibility
 */
bool vxBlockVisibilityTester::IsBlockVisibleIntensity( const Vector3D<int4> & volumePosition )
{
  // get block information
  //uint4 x( volumePosition.X() >> 4 );
  //uint4 y( volumePosition.Y() >> 4 );
  //uint4 z( volumePosition.Z() >> 4 );
  //m_pVolIterHistIntensity->SetPos( x, y, z );
  //m_pVolIterHistLabel    ->SetPos( x, y, z );
  //int4 iHistVoxelIntensity( m_pVolIterHistIntensity->GetVoxel() );
  //int4 iHistVoxelLabel    ( m_pVolIterHistLabel    ->GetVoxel() );

  // get block information
  uint4 x( volumePosition.X() );
  uint4 y( volumePosition.Y() );
  uint4 z( volumePosition.Z() );
  m_pVolIterIntensity->SetPos( x, y, z );
  m_pVolIterLabel    ->SetPos( x, y, z );
  int4 iHistVoxelIntensity( m_pVolIterIntensity->GetCurrentBlock().GetBinaryHistogram() );
  int4 iHistVoxelLabel    ( m_pVolIterLabel->GetCurrentBlock().GetBinaryHistogram() );

  // apply visibility logic
  return ( iHistVoxelIntensity & m_iMaskIntensity ) && ( ! m_bShowLabel || ( iHistVoxelLabel & m_iMaskLabel ) );

} // IsBlockVisibleIntensity()


/**
 * Block visible with respect to segmentation range?
 *
 * @param volumePosition volume location (voxel) of interest
 * @return               visibility
 */
bool vxBlockVisibilityTester::IsBlockVisibleSegmentationRange( const Vector3D<int4> & volumePosition )
{

  // get block information
  //uint4 x( volumePosition.X() >> 4 );
  //uint4 y( volumePosition.Y() >> 4 );
  //uint4 z( volumePosition.Z() >> 4 );
  //m_pVolIterHistIntensity->SetPos( x, y, z );
  //int4 iHistVoxelIntensity( m_pVolIterHistIntensity->GetVoxel() );

  // get block information
  uint4 x( volumePosition.X() );
  uint4 y( volumePosition.Y() );
  uint4 z( volumePosition.Z() );
  m_pVolIterIntensity->SetPos( x, y, z );
  int4 iHistVoxelIntensity( m_pVolIterIntensity->GetCurrentBlock().GetBinaryHistogram() );

  // apply visibility logic
  return ( iHistVoxelIntensity & m_iMaskSegmentationIntensityRange ) > 0;

} // IsBlockVisibleIntensity()


/**
 * Determine if the any voxel in the block is visible.
 * Can be invisible because of all labels in the block beeing set to invisible intensity
 *
 * @param volumePosition volume location (voxel) of interest
 * @return               visibility
 */
bool vxBlockVisibilityTester::IsBlockVisibleLabel( const Vector3D<int4> & volumePosition )
{

  // get block information
  //uint4 x( volumePosition.X() >> 4 );
  //uint4 y( volumePosition.Y() >> 4 );
  //uint4 z( volumePosition.Z() >> 4 );
  //m_pVolIterHistLabel->SetPos( x, y, z );
  //int4 iHistVoxelLabel( m_pVolIterHistLabel->GetVoxel() );

// get block information
  uint4 x( volumePosition.X());
  uint4 y( volumePosition.Y());
  uint4 z( volumePosition.Z());
  m_pVolIterLabel->SetPos( x, y, z );
  int4 iHistVoxelLabel( m_pVolIterLabel->GetCurrentBlock().GetBinaryHistogram() );

  // apply visibility logic
  return m_bShowLabel && ( iHistVoxelLabel & m_iMaskLabel );

} // IsBlockVisibleLabel()


/**
 * Determine if a given block is visible
 *
 * @param volumePosition   volume location (voxel) of interest
 * @return                 visibility
 */
bool vxBlockVisibilityTester::IsBlockVisible(const Vector3D<int4> & volumePosition)
{

  // get block information
  //uint4 x( volumePosition.X() >> 4 );
  //uint4 y( volumePosition.Y() >> 4 );
  //uint4 z( volumePosition.Z() >> 4 );
  //m_pVolIterHistIntensity->SetPos( x, y, z );
  //m_pVolIterHistLabel    ->SetPos( x, y, z );
  //int4 iHistVoxelIntensity( m_pVolIterHistIntensity->GetVoxel() );
  //int4 iHistVoxelLabel    ( m_pVolIterHistLabel    ->GetVoxel() );

  // get block information
  uint4 x( volumePosition.X() );
  uint4 y( volumePosition.Y() );
  uint4 z( volumePosition.Z() );
  m_pVolIterIntensity->SetPos( x, y, z );
  m_pVolIterLabel    ->SetPos( x, y, z );
  int4 iHistVoxelIntensity( m_pVolIterIntensity->GetCurrentBlock().GetBinaryHistogram() );
  int4 iHistVoxelLabel    ( m_pVolIterLabel->GetCurrentBlock().GetBinaryHistogram() );

  return IsBlockVisibleRaw( iHistVoxelIntensity, iHistVoxelLabel );

} // IsBlockVisible()

/**
 * Is raw block visible?
 *
 * @param iHistVoxelIntensity intensity volume histogram
 * @param iHistVoxelLabel     label volume histogram
 * @return                    visibility
 */
bool vxBlockVisibilityTester::IsBlockVisibleRaw( int4 iHistVoxelIntensity, int4 iHistVoxelLabel )
{

  return

    // is the intensity visible
    ( ( iHistVoxelIntensity & m_iMaskIntensity ) && ( ! m_bShowLabel || ( iHistVoxelLabel & m_iMaskLabel ) ) )

    ||

    // or is the label visible
    ( m_bShowLabel && ( iHistVoxelLabel & m_iMaskLabel ) );

} // IsBlockVisibleRaw()


#undef FILE_REVISION


// $Log: vxBlockVisibilityTester.C,v $
// Revision 1.9  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.8  2007/02/13 22:32:11  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.7  2006/09/21 14:56:55  geconomos
// added new consturctor that takes std::vector<vxComponent> as argument
//
// Revision 1.6  2006/07/21 15:02:19  geconomos
// added yet another constructor
//
// Revision 1.5  2006/06/23 19:18:07  frank
// breaking out utility functions to take individual elements instead of just the environment
//
// Revision 1.4  2006/03/29 20:02:34  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.3  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.2.16.1  2006/05/19 14:12:44  geconomos
// +added virtual destructor
// + deletion of created iterators
//
// Revision 1.2  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/31 01:59:51  michael
// reference to vxVolumeHeader is sufficient
//
// Revision 1.1  2003/08/04 14:39:24  frank
// Separated class out from vxUtils
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxBlockVisibilityTester.C,v 1.9 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxBlockVisibilityTester.C,v 1.9 2007/02/15 00:08:23 jmeade Exp $
