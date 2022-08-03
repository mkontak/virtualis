// $Id: vxManipulatorVessel.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorVessel.h"
#include "vxElementFloat4.h"
#include "vxIntensityConverter.h"
#include "Image.h"
#include "vxIntensityStatistics.h"
#include "vxEnvironment.h"
#include "vxIDs.h"
#include "vxVessel.h"
#include "vxVesselUtilities.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 * @param environment the environment
 */
vxManipulatorVessel::vxManipulatorVessel( vxEnvironment & environment ):
vxManipulator( environment )
{
} // Constructor


/**
 * Destructor
 */
vxManipulatorVessel::~vxManipulatorVessel()
{
  GetEnvironment().GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CROP_BOX, false );
} // Destructor


/**
 * Retrieve the current section index
 *
 * @param environment the environment
 */
float4 vxManipulatorVessel::GetVesselSectionIndex( vxEnvironment & environment )
{
  // get the current index pointer
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( environment.GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogWrn( "no current vessel section: ", "vxManipulatorVesselLuminal", "GetVesselSectionIndex" );
    return 0;
  }

  return pCurrentVesselSection->GetValue();

} // GetVesselSectionIndex()


/**
 * Set the current section index
 *
 * @param environment   the environment
 * @param fSectionIndex the desired section index
 */
void vxManipulatorVessel::SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex )
{
  // get the current index pointer
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( environment.GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogWrn( "no current vessel section: ", "vxManipulatorVesselLuminal", "SetVesselSectionIndex" );
    return;
  }

  // get the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
  if (pVessel == NULL)
    throw ex::NullPointerException( LogRec( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROblique", "SetVesselSectionIndex" ) );

  // bounds check the section index
  if ( fSectionIndex < 0.0F || fSectionIndex > static_cast< float4 >( pVessel->size()-1 ) )
    return;

  // make sure it's actually changing
  if ( pCurrentVesselSection->GetValue() == fSectionIndex )
    return;

  // set the current vessel section index
  const float4 fOldValue( pCurrentVesselSection->GetValue() );
  pCurrentVesselSection->SetValue( fSectionIndex );
  if ( fSectionIndex != fOldValue )
    pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );

} // SetVesselSectionIndex()


/**
 * Get the offset distance of the detail volume
 *
 * @return the offset of the detail volume
 */
Point3Df vxManipulatorVessel::GetDetailVolumeOffset( vxEnvironment & environment )
{
  vxBlockVolume<uint2> * pVesselDetailVolume = dynamic_cast<vxShareableObject< vxBlockVolume<uint2> > *>( environment.GetElement( vxIDs::DetailVolume ) );
  if (pVesselDetailVolume == NULL)
  {
    //LogWrn( "Vessel detail volume pointer is NULL", "vxManipulatorVesselLuminal", "GetDetailVolumeOffset" );
    return Point3Df();
  }

  // get the detail volume world position
  VarMap varmap(pVesselDetailVolume->GetHeader().GetVariableVolumeHeader());  
  Point<float4> volCenter;
  varmap.GetHdrVar( "VolumeCenter", volCenter );
  const Vector3D<float4> units( pVesselDetailVolume->GetHeader().GetVoxelUnits() );
  const Vector3D<int4> dim( pVesselDetailVolume->GetHeader().GetVolDim() );
  Vector3Df halfDim( 0.5f*(dim.X()-1)*units.X(),
                     0.5f*(dim.Y()-1)*units.Y(),
                     0.5f*(dim.Z()-1)*units.Z() );
  return volCenter - halfDim;

} // GetDetailVolumeOffset()


void ErodeImage( Image<bool> & mask )
{
  Image<bool> temp( mask.GetDim(), false );

  for ( int y=1; y<mask.GetDim().m_y-1; ++y )
  {
    for ( int x=1; x<mask.GetDim().m_x-1; ++x )
    {
      if ( mask.GetPixel( Point2D<int4>( x, y ) ) == true )
      {
        uint4 uNeighbors( 0 );
        uNeighbors += mask.GetPixel( Point2D<int4>( x-1, y ) );
        uNeighbors += mask.GetPixel( Point2D<int4>( x+1, y ) );
        uNeighbors += mask.GetPixel( Point2D<int4>( x, y-1 ) );
        uNeighbors += mask.GetPixel( Point2D<int4>( x, y+1 ) );
        if ( uNeighbors > 2 )
        {
          temp.SetPixel( Point2D<int4>( x, y ), true );
        }
      }
    }
  }

  mask = temp;
}


void DilateImage( Image<bool> & mask )
{
  Image<bool> temp( mask.GetDim(), false );

  for ( int y=0; y<mask.GetDim().m_y; ++y )
  {
    for ( int x=0; x<mask.GetDim().m_x; ++x )
    {
      if ( mask.GetPixel( Point2D<int4>( x, y ) ) == true )
      {
        const int4 iRadius( 4 );
        for ( int u=-iRadius; u<=iRadius; ++u )
        {
          for ( int v=-iRadius; v<=iRadius; ++v )
          {
            if ( x + u >= 0 && x + u < mask.GetDim().m_x &&
                 y + v >= 0 && y + v < mask.GetDim().m_y )
            {
              temp.SetPixel( Point2D<int4>( x + u, y + v ), true );
            }
          }
        }
      }
    }
  }

  mask = temp;
}


/**
 * sculpt out the table
 *
 * @param environment the environment
 */
void vxManipulatorVessel::SculptTable( vxEnvironment & environment )
{

  vxBlockVolume< uint2 > outputVolume( environment.GetIntensityVolume()->GetHeader() );
  vxBlockVolumeIterator< uint2 > volIter( * environment.GetIntensityVolume() );
  vxBlockVolumeIterator< uint2 > outputIter( outputVolume );
  const uint2 uThresholdRaw( vxIntensityConverter::ModalityToRaw( 100, environment.GetIntensityVolume()->GetHeader() ) );
  int4 iSampleWidth( outputVolume.GetHeader().GetVolDim().m_z / 3 );
  int4 iStartZ( outputVolume.GetHeader().GetVolDim().m_z / 2 - iSampleWidth / 2 );
  int4 iEndZ( outputVolume.GetHeader().GetVolDim().m_z / 2 + iSampleWidth / 2 );
  Image<bool> maskImage( Triple<int4>( outputVolume.GetHeader().GetVolDim().m_x, outputVolume.GetHeader().GetVolDim().m_y, 0 ), false );
  const float4 fHeightThreshold( 0.5F );

  // find seeds that meet a minimum HU and have small difference in the z-direction
  for ( int y=outputVolume.GetHeader().GetVolDim().m_y * fHeightThreshold; y<outputVolume.GetHeader().GetVolDim().m_y; ++y )
  {
    for ( int x=0; x<outputVolume.GetHeader().GetVolDim().m_x; ++x )
    {

      // sample a column in the z-direction
      vxIntensityStatistics stats;
      volIter.SetPos( x, y, iStartZ );
      for ( int4 z(iStartZ); z<=iEndZ; ++z, volIter.IncZ() )
        stats.Add( volIter.GetVoxel() );

      // choose those voxels with low standard deviation
      stats.Compute();
      const int4 uThresholdStdDev( 50 );
      if ( stats.GetAverage() > uThresholdRaw && stats.GetStandardDeviation() < uThresholdStdDev )
      {
        maskImage.SetPixel( Point2D<int4>( x, y ), true );
      }
    }
  }

  ErodeImage( maskImage );
  DilateImage( maskImage );

  // copy volume back to intensity volume
  for ( int y=0; y<outputVolume.GetHeader().GetVolDim().m_y; ++y )
  {
    for ( int x=0; x<outputVolume.GetHeader().GetVolDim().m_x; ++x )
    {
      if ( maskImage.GetPixel( Point2D<int4>( x, y ) ) == true )
      {
        volIter.SetPos( x, y, 0 );
        for ( int z=0; z<outputVolume.GetHeader().GetVolDim().m_z; ++z, volIter.IncZ() )
        {
          volIter.SetVoxel( 0 );
        }
      }
    }
  }
  //for ( outputIter.FirstNonBackgroundBlockZYX(); outputIter.IsNotAtEnd(); outputIter.NextNonBackgroundBlockZYX() )
  //{
  //  for ( outputIter.FirstNonBackgroundZYXinsideBlock(); outputIter.IsNotAtEndOfBlock(); outputIter.NextNonBackgroundZYXinsideBlock() )
  //  {
  //    volIter.SetPos( outputIter.GetPos() );
  //    volIter.SetVoxel( outputIter.GetVoxel() );
  //  }
  //}



  environment.GetIntensityVolume()->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_INTENSITY_INITIALIZED ) );

} // SculptTable()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVessel.C,v $
// Revision 1.2  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2006/06/05 15:06:52  frank
// updated for visual studio 2005
//
// Revision 1.4  2005/06/08 14:59:51  vxconfig
// added easy select object
//
// Revision 1.3  2005/06/07 18:08:09  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/09/02 17:58:09  frank
// removed dead code
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/25 14:58:27  frank
// formatting
//
// Revision 1.3  2003/12/22 18:54:44  frank
// Set the current vessel section extended element modified
//
// Revision 1.2  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/04/30 13:43:44  frank
// code review
//
// Revision 1.3  2003/03/24 14:29:33  frank
// Moved some common code to a base class
//
// Revision 1.2  2003/03/14 17:28:11  frank
// Added a GetDetailVolumeOffset()
//
// Revision 1.1  2003/03/14 15:57:00  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVessel.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $
// $Id: vxManipulatorVessel.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $
