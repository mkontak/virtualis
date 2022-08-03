// $Id: vxPolypSegmentation.C,v 1.24.2.5 2010/03/30 20:34:46 dongqing Exp $
//
// Copyright© 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: vxPolypSegmentation.C
// Owner(s): Dongqing Chen dongqing@viatronix.net


/** 
 *  This class is to segment polyp region by an user input. The user will use
 *  the mouse to click a voxel on the surface of polyp to the air colonic 
 *  segment that the polyp belongs to. The class should automatically 
 *  segment out the polyp region and label the region in the label volume. The
 *  class will also determine minimum and maximum diameter, height (of a pedunculated
 *  polyp), and volume of the Polyp.
 */

#include "stdafx.h"
#include "vxPolypSegmentation.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxIntensityConverter.h"
#include "VolumePaint.h"
#include <queue>
#include "Timer.h"

using namespace hdr;
using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.24.2.5 $"

/** 
 *  Constructor
 *
 *  @param ecvVolUC is the 8bits cleansed volume
 *  @param vol is the original/cleansed intensity volume
 *  @param labelVol is the uint2 label volume for 2D MPR rendering
 *  @param uComponentLabel is the label index for this newly found ROI
 *  @param paintVol is the 8bits paint volume for 3D endoluminal view rendering
 *  @param selectPoint is the user select point in world coordinate
 */
vxPolypSegmentation::vxPolypSegmentation( LinVolUC & ecvVolUC, vxBlockVolume<uint2> & vol, 
            vxBlockVolume<uint2> & labelVol, 
            const uint2 & uComponentLabel, vxBlockVolume<uint1> & insVol, 
            LinVolUC & paintVol, const Point<float4> & selectPoint )
: m_ecvVol(ecvVolUC), m_vol(vol), m_labelVol(labelVol), m_uComponentLabel(uComponentLabel), 
  m_insVol(insVol), m_paintVol(paintVol), m_fUserInputPoint(selectPoint)
{
	m_vfMaxDiameterEnd1   = Point<float4>  (0.0F, 0.0F, 0.0F);
	m_vfMaxDiameterEnd2   = Point<float4>  (0.0F, 0.0F, 0.0F);
  m_iClosestTissuePoint = Vector3D<int2> (0,0,0);
  m_iCenterOfSubvolume  = Vector3D<int2> (0,0,0);

	m_fMaximumDiameter = 0.0F;
	m_fVolumeOfPolyp   = 0.0F;

  m_ixl = 0;
  m_ixu = 0;
  m_iyl = 0;
  m_iyu = 0;
  m_izl = 0;
  m_izu = 0;

  m_uSegmentIndex     = 0x00;  // will be determined by user selected point
  m_uTissueLabel      = 0x11;
  m_uWallSurfaceLabel = 0x80;
  m_uPolypLabel       = 0xF0;

  // critical thresholds for polyp volume measurement
	m_uTissueHigh = vxIntensityConverter::ModalityToRaw( 180, m_vol.GetHeader());
  // -500 HU match the measurement taken in the Viewer 3D line measurements
	m_uTissueLow  = vxIntensityConverter::ModalityToRaw(-500, m_vol.GetHeader());

  m_IsFlatPolyp = false;
  
  m_f1stLevelRayLength =  6.0F;    // in millimeter  8 is good for most
  m_f2ndLevelRayLength =  8.0F;    // in millimeter
  m_f3rdLevelRayLength = 16.0F;    // in millimeter

  m_fSquareOfRadiusForSearching1stLevelRayOrigins = 8.0F*8.0F;   // in millimeter
  m_fSquareOfRadiusForSearching2ndLevelRayOrigins = 6.0F*6.0F;   // in millimeter

  m_fHalfWidthOfSubvolume = 20.0F;  // in millimeter

  m_fROISizeRatio = 0.9F;

  m_fSearchRadius = 18.0F;  // in millimeter
} // vxPolypSegmentation( ecvVolUC, vol, labelVol, uComponentLabel, insVol, paintVol, selectPoint )


/** 
 *  destructor
 */
vxPolypSegmentation::~vxPolypSegmentation()
{
} // ~vxPolypSegmentation()


/**
 *  control function
 *
 *  @return -1 if fail; 
 *           0 if normal success; 
 *           1 if flat or diminutive polyp;
 *           3 if the ROI has already been segmented
 */
int4 vxPolypSegmentation::Run()
{
  if( !DetermineColonSegmentIndex() )
  {
    return(-1);
  }

  ExtractSubvolume();  /// it should be rock solid, you bet!

  int4 iPolypSegIndicator( 1 );
  try
  {
    /// setup working mask label volume (size of subvolume)
    if( !SetInitialMask() )
    {
      return(-1);
    }

    iPolypSegIndicator = ExtractPolypRegion();
  }
  catch( ... )
  {
    LogErr("Exception throw in polyp extraction", "vxPolypSegmentation", "Run");
    return(-1);
  }

  if( iPolypSegIndicator == -1 || iPolypSegIndicator == 2 )
  {
    return(-1);
  }
  
  if( iPolypSegIndicator == 3 )
  {
    return(3);
  }

  const Triple<int4> dimV  ( m_vol.GetHeader().GetVolDim()    );
  const Triple<int4> viDim ( m_pmask->GetHeader().GetVolDim() );
  const int4    iSliceStep ( dimV.m_x*dimV.m_y );

  Triple<float4> vfUnits( m_vol.GetHeader().GetVoxelUnits() );

  if( m_fVolumeOfPolyp < 0.01F )
  {
    LogWrn("No polyp ROI found", "vxPolypSegmentation", "Run");
    return(-1);
  }

  const float4 fFlatRatio( m_fMaximumDiameter*m_fMaximumDiameter*(vfUnits.m_x+vfUnits.m_y+vfUnits.m_z)/3.0F/m_fVolumeOfPolyp );

  uint1 * ptVol( m_paintVol.GetDataPtr() );
  vxBlockVolumeIterator<uint1> itm ( *m_pmask   );
  vxBlockVolumeIterator<uint2> itl ( m_labelVol );

  const uint1 uPolypSurfaceLabelInPaintVolume = ReaderLib::SUSPICIOUSROI;
  const uint1 uCADLabelInPaintVolume = ReaderLib::CADROI;

  register int x, y, z, s, t, u;
  s = m_izl*iSliceStep;
  for( z=0, itl.SetPosZ(m_izl), itm.SetPosZ(0); z<viDim.m_z; z++,
            itl.IncZ(), itm.IncZ() )
  {    
    t = s+m_iyl*dimV.m_x;

    for( y=0, itl.SetPosY(m_iyl), itm.SetPosY(0); y<viDim.m_y; y++,
            itl.IncY(), itm.IncY() )
    {
      u = t+m_ixl;

      for( x=0, itl.SetPosX(m_ixl), itm.SetPosX(0); x<viDim.m_x; x++,
            itl.IncX(), itm.IncX() )
      {
        if( itm.GetVoxel() == m_uPolypLabel )
        {
          itl.SetVoxel( m_uComponentLabel );
          if( !(ptVol[x+u] == uCADLabelInPaintVolume) )
            ptVol[x+u] = uPolypSurfaceLabelInPaintVolume;
        }
        else if( itm.GetVoxel() > m_uPolypLabel )
        {
          if( !(ptVol[x+u] == uCADLabelInPaintVolume) )
            ptVol[x+u] = uPolypSurfaceLabelInPaintVolume;
        } // else
      } // for x

      t = t+dimV.m_x;
    } // for y

    s = s+iSliceStep;
  } // for z

  if( fFlatRatio > m_fROISizeRatio || m_fVolumeOfPolyp < 50.0F )
  { /// This is a flat/diminutive lesion
    LogFyi("Flat lesion/polyp was detected", "vxPolypSegmentation", "Run");
    return(1);
  }
   
  return(0);
} // Run


/** 
 *  reset the paint volume per user request
 */
void vxPolypSegmentation::ResetPaintVolPerUserRequest()
{
  const Triple<int4> dimV  ( m_vol.GetHeader().GetVolDim()    );
  const Triple<int4> viDim ( m_pmask->GetHeader().GetVolDim() );
  const int4    iSliceStep ( dimV.m_x*dimV.m_y );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );
  uint1 * ptVol( m_paintVol.GetDataPtr() );

  register int x, y, z, s, t, u;
  s = m_izl*iSliceStep;
  for( z=0, itm.SetPosZ(0); z<viDim.m_z; z++, itm.IncZ() )
  {    
    t = s+m_iyl*dimV.m_x;

    for( y=0, itm.SetPosY(0); y<viDim.m_y; y++, itm.IncY() )
    {
      u = t+m_ixl;

      for( x=0, itm.SetPosX(0); x<viDim.m_x; x++, itm.IncX() )
      {
        if( itm.GetVoxel() >= m_uPolypLabel )
          ptVol[x+u] = 2;
      } // for x

      t = t+dimV.m_x;
    } // for y

    s = s+iSliceStep;
  } // for z

	// TODO this is temporary for RSNA 2006. It should be in ComponentArray
	vxBlockVolumeIterator<uint2> itl(m_labelVol);

	for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
	{
		for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
		{
			if( itl.GetVoxel() == m_uComponentLabel )
				itl.SetVoxel( 0 );
		} // for inside the blocks
	} // for all blocks
	/////////////////////////////////////////////////////////////////////////

} //ResetPaintVolPerUserRequest


/** 
 *  get the maximum diameter of polyp
 *  @param vertex01     is one end of the diameter line
 *  @param vertex02     is another end of the diameter line
 *  @return             the maximum diameter
 */
float4 vxPolypSegmentation::GetMaxDiameter( Point<float4> & vertex01, Point<float4> & vertex02 ) const
{
	vertex01 = m_vfMaxDiameterEnd1;
	vertex02 = m_vfMaxDiameterEnd2;

	return( m_fMaximumDiameter );
} // GetMaxDiameter


/** 
 *  get the volume of polyp in mm^3
 * 
 *  @return   the volume of the polyp
 */
float4 vxPolypSegmentation::GetVolume() const
{
  return ( m_fVolumeOfPolyp > 0.0F ) ? m_fVolumeOfPolyp : 0.0F;
} // GetVolume()


/** get the volume ROI
 *
 *  @param viVolROI is the point list of the ROI in volume coordinate
 */
void vxPolypSegmentation::GetPolypVolumeROI( std::vector< Vector3D<int2> > & viVolROI ) const
{
  if( m_viPolypVoxelContainer.size() > 1 && viVolROI.size() < 1 )
  {
    for( register int i=m_viPolypVoxelContainer.size()-1; i>=0; i-- )
    {
      viVolROI.push_back( m_viPolypVoxelContainer[i] );
    }
  }
  else
  {
    LogWrn("Polyp volume ROI is empty", "vxPolypSegmentation", "GetPolypVolumeROI");
  }
} // GetPolypVolumeROI( viVolROI )


/**
  *  determine the colonic segment index that the polyp locates in
  *
  *  @return true if successfully determination
  */
bool vxPolypSegmentation::DetermineColonSegmentIndex()
{
  Triple<float4> vfUnits( m_vol.GetHeader().GetVoxelUnits() );
  Triple<int4>   viDim  ( m_vol.GetHeader().GetVolDim()     );

  m_iCenterOfSubvolume.m_x = int2(float4(m_fUserInputPoint.m_x)/vfUnits.m_x+0.5F); 
  m_iCenterOfSubvolume.m_y = int2(float4(m_fUserInputPoint.m_y)/vfUnits.m_y+0.5F); 
  m_iCenterOfSubvolume.m_z = int2(float4(m_fUserInputPoint.m_z)/vfUnits.m_z+0.5F); 

  vxBlockVolumeIterator<uint1> iti( m_insVol );
  iti.SetPos( m_iCenterOfSubvolume );

  if( iti.GetVoxel() > 0x00 )
  { // select point in the lumen of the segment
    m_uSegmentIndex = iti.GetVoxel();
  }
  else
  { 
    register int x, y, z, i, j;
    register int minDist(1000);
    register Vector3D<int2> vPoint(0,0,0);
    m_uSegmentIndex = 0x00;

    /// determine the nearest colon wall tissue point
    for( z=-5; z<6; z++ )
    {
      i = abs(z);

      for( y=-5; y<6; y++ )
      {
        j = i+abs(y);

        for( x=-5; x<6; x++ )
        {
          if( iti.PeekRel(x,y,z) > 0 && minDist > x+j )
          {
            m_uSegmentIndex = iti.PeekRel(x,y,z);
            minDist = x+j;
          } // if
        } // for x
      } // for y
    } // for z
  } // else

  if( m_uSegmentIndex == 0x00 )
  {
    LogErr("No segment ponit within 5 voxel range", "vxPolypSegmentation", "DetermineColonSegmentIndex");
    return(false);
  }
  else
  {
    return(true);
  }
}  // DetermineColonSegmentIndex()


/**
 * extract subvolume that encloses the polyp
 */
void vxPolypSegmentation::ExtractSubvolume()
{
  Triple<float4> vfUnits( m_vol.GetHeader().GetVoxelUnits() );
  Triple<int4>   viDim  ( m_vol.GetHeader().GetVolDim()     );
  
  // compute the bounding box vertex
  m_ixl = max( m_iCenterOfSubvolume.m_x - int(m_fHalfWidthOfSubvolume/vfUnits.m_x), 1 );
  m_ixu = min( m_iCenterOfSubvolume.m_x + int(m_fHalfWidthOfSubvolume/vfUnits.m_x), viDim.m_x-2 );
  m_iyl = max( m_iCenterOfSubvolume.m_y - int(m_fHalfWidthOfSubvolume/vfUnits.m_y), 1 );
  m_iyu = min( m_iCenterOfSubvolume.m_y + int(m_fHalfWidthOfSubvolume/vfUnits.m_y), viDim.m_y-2 );
  m_izl = max( m_iCenterOfSubvolume.m_z - int(m_fHalfWidthOfSubvolume/vfUnits.m_z), 1 );
  m_izu = min( m_iCenterOfSubvolume.m_z + int(m_fHalfWidthOfSubvolume/vfUnits.m_z), viDim.m_z-2 );

  Triple<int> viNewDim( Triple<int>(m_ixu-m_ixl+1, m_iyu-m_iyl+1, m_izu-m_izl+1) );

  /// initialize the subvolume
  m_pSubVol	= new vxBlockVolume<uint2>( vxVolumeHeader(viNewDim, vfUnits, 16*sizeof(uint1), 0) );
  if( m_pSubVol == NULL )
  {
    throw ex::OutOfMemoryException(LogRec("The sub-volume is not ready", "vxPolypSegmentation", "ExtractSubvolume"));
    return;
  } // if

  vxBlockVolumeIterator<uint2> itv( m_vol      );
  vxBlockVolumeIterator<uint2> its( *m_pSubVol );

  register int x,y,z;
  /// copy value from original vol to the subvol
  for( z=0, itv.SetPosZ(m_izl), its.SetPosZ(0); z<viNewDim.m_z; z++,
            itv.IncZ(), its.IncZ() )
  {
    for( y=0, itv.SetPosY(m_iyl), its.SetPosY(0); y<viNewDim.m_y; y++,
            itv.IncY(), its.IncY() )
    {
      for( x=0, itv.SetPosX(m_ixl), its.SetPosX(0); x<viNewDim.m_x; x++,
            itv.IncX(), its.IncX() )
      {
        its.SetVoxel( itv.GetVoxel() );
      } // for x
    } // for y
  } // for z

}  // ExtractSubvolume()


/**
  *  initialize the mask for software tissue and label the associated segment
  *  the adjacent wall surface will also be determined
  *
  *  @return true if success
  */
bool vxPolypSegmentation::SetInitialMask()
{
  Triple<int4>   viDim   ( m_pSubVol->GetHeader().GetVolDim()     );
  Triple<float4> vfUnits ( m_pSubVol->GetHeader().GetVoxelUnits() );

  // initialize the working submask
  m_pmask	= new vxBlockVolume<uint1>( vxVolumeHeader(viDim, vfUnits, 8*sizeof(uint1), 0) );
  if( m_pmask == NULL )
  {
    LogErr("No memory for the sub-mask", "vxPolypSegmentation", "SetInitialMask");
    return(false);
  } // if

  vxBlockVolumeIterator<uint1> iti( m_insVol );
  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  register int x, y, z;
  // copy the associate lumen index to the submask
  for( z=0, iti.SetPosZ(m_izl), itm.SetPosZ(0); z<viDim.m_z; z++,
            iti.IncZ(), itm.IncZ() )
  {
    for( y=0, iti.SetPosY(m_iyl), itm.SetPosY(0); y<viDim.m_y; y++,
            iti.IncY(), itm.IncY() )
    {
      for( x=0, iti.SetPosX(m_ixl), itm.SetPosX(0); x<viDim.m_x; x++,
            iti.IncX(), itm.IncX() )
      {
        if( iti.GetVoxel() > 0x00 )
        {
          itm.SetVoxel( iti.GetVoxel() );
        }
      } // for x
    } // for y
  } // for z

  const uint1 uTmpLabel01( 0xFE );
  //const uint1 uTmpLabel02( 0xFE-1 );

  const Vector3D<int2> viCenterSubVol( viDim.m_x/2+1, viDim.m_y/2+1, viDim.m_z/2+1 );
  Vector3D<int2> viSeedOfLumen(0,0,0);
  itm.SetPos( viCenterSubVol );
  register int i, j, minDist(1000);
  /// determine the nearest lumen point
  for( z=-5; z<6; z++ )
  {
    i = abs(z);

    for( y=-5; y<6; y++ )
    {
      j = i+abs(y);

      for( x=-5; x<6; x++ )
      {
        if( itm.PeekRel(x,y,z) == m_uSegmentIndex && minDist > x+j )
        {
          viSeedOfLumen = viCenterSubVol+Vector3D<int2>(x,y,z);
          minDist = x+j;
        } // if
      } // for x
    } // for y
  } // for z

  if( viSeedOfLumen != Vector3D<int2>(0,0,0) )
  { // grow out the air lumen in which the polyp lies in
    GrowLumen( viSeedOfLumen, m_uSegmentIndex, uTmpLabel01 );

    // synchronize the iterator after volume changes
    itm.SetPos( itm.GetPos() );
  } // if
  else
  {
    LogErr("No air lumen near the selected point", "vxPolypSegmentation", "SetInitialMask");
    return(false);
  }

  vxBlockVolumeIterator<uint2> its( *m_pSubVol );

  /// label the soft tissue and muscle voxels
  for( its.SetPos(0,0,0), itm.SetPos(0,0,0); its.IsNotAtEnd(); its.NextBlockZYX(), itm.NextBlockZYX() )
  {
    for( ; its.IsNotAtEndOfBlock(); its.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
    {
      if( itm.GetVoxel() == 0x00 )
      {
        if( its.GetVoxel() >= m_uTissueLow && its.GetVoxel() <= m_uTissueHigh )
        {
          itm.SetVoxel( m_uTissueLabel );
        }
        else if( its.GetVoxel() < m_uTissueLow )  
        {  /// this will catch air that enclosed by the polyp. But, it may include small bowel too.
          itm.SetVoxel( m_uWallSurfaceLabel );
        } // else if()
      } // if

      else if( itm.GetVoxel() == uTmpLabel01 )
      {
        itm.SetVoxel( m_uSegmentIndex );
      }
      else 
      {
        itm.SetVoxel( 0x00 );
      }
    } // for inside the block
  } // for all blocks

  // label associated wall surface point = one layer dilation of the selected lumen
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
    {
      if( itm.GetVoxel() == m_uSegmentIndex )
      {
        if( itm.PeekRel(1,0,0) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,0,0) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,1,0) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,-1,0) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,0,1) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,0,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,1,0) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,-1,0) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,-1,0) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,1,0) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,0,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,0,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,-1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,0,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,0,-1) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,1,-1) == m_uTissueLabel ) 
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(0,-1,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,-1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,-1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,1,1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,1,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(1,-1,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,-1,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
        else if( itm.PeekRel(-1,1,-1) == m_uTissueLabel )
        {
          itm.SetVoxel( m_uWallSurfaceLabel );
        }
      } // if
    } // for inside block
  } // for all blocks

  return(true);
} // SetInitialMask()


/**
 *  grow out lumen segment
 *
 *  @param viSeedOfLumen is the seed point
 *  @param uROIlabel is the old label index for the ROI
 *  @param uNewLabel is the new label index for the grown area
 */
void vxPolypSegmentation::GrowLumen( const Vector3D<int2> & viSeedOfLumen, 
                        const uint1 & uROIlabel, const uint1 & uNewLabel )
{
  const Triple<int4>  viDim   ( m_pmask->GetHeader().GetVolDim()    );
  const Triple<float4> vfUnits ( m_pmask->GetHeader().GetVoxelUnits());
  const int4 iX ( viDim.m_x );
  const int4 iY ( viDim.m_y );
  const int4 iZ ( viDim.m_z );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );
  vxBlockVolumeIterator<uint2> its( *m_pSubVol );

  // queue for region growing
  register    Vector3D<int2>   vi(viSeedOfLumen);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	if( its.GetVoxel() >= m_uTissueLow )
  {
		return;
  }
  itm.SetVoxel(uNewLabel);

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
	  itm.SetPos(vi);
    its.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX && itm.Peek1px0py0pz() != uNewLabel && its.PeekRel(1,0,0) < m_uTissueLow )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncX();
      itm.SetVoxel(uNewLabel);
			itm.DecX();
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY && itm.Peek0px1py0pz() != uNewLabel && its.PeekRel(0,1,0) < m_uTissueLow )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncY();
      itm.SetVoxel(uNewLabel);
			itm.DecY();
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x >= 0 && itm.Peek1nx0py0pz() != uNewLabel && its.PeekRel(-1,0,0) < m_uTissueLow )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.DecX();
      itm.SetVoxel(uNewLabel);
			itm.IncX();
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y >= 0 && itm.Peek0px1ny0pz() != uNewLabel && its.PeekRel(0,-1,0) < m_uTissueLow )
		{
      viRegQueue.push( vi );
			itm.DecY();
      itm.SetVoxel(uNewLabel);
			itm.IncY();
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z >= 0 && itm.Peek0px0py1nz() != uNewLabel && its.PeekRel(0,0,-1) < m_uTissueLow )
		{
      viRegQueue.push( vi );
			itm.DecZ();
      itm.SetVoxel(uNewLabel);
			itm.IncZ();
		} // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ && itm.Peek0px0py1pz() != uNewLabel && its.PeekRel(0,0,1) < m_uTissueLow )
		{
      viRegQueue.push( vi );
			itm.IncZ();
      itm.SetVoxel(uNewLabel);
			itm.DecZ();
		} // if

	  viRegQueue.pop();
  }  // end of while() 
} // GrowLumen( viSeedOfLumen, uROIlabel, uNewLabel )


/**
 *  extract polyp region
 *
 *  @return -1 if fail 
 *           0 if success
 *           1 if very small area
 *           2 if no region was found
 *           3 if it is segmented ROI
 */
int4 vxPolypSegmentation::ExtractPolypRegion()
{
  const Triple<int4>     viDim    ( m_pmask->GetHeader().GetVolDim()     );
  const Triple<float4>   vfUnits  ( m_pmask->GetHeader().GetVoxelUnits() );
  const Vector3D<int2>   viCenter ( viDim.m_x/2+1, viDim.m_y/2+1, viDim.m_z/2+1 );
  const Vector3D<float4> vfSquare ( vfUnits.m_x*vfUnits.m_x, vfUnits.m_y*vfUnits.m_y, vfUnits.m_z*vfUnits.m_z );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );
  itm.SetPos( viCenter );

  register int i, x, y, z;
  register int4 minDist(1000);
  register float4 ft, fDist(99999.0F);
  m_iClosestTissuePoint = Vector3D<int2>(0,0,0);
  /// determine the nearest colon wall tissue point
  for( z=-4; z<5; z++ )
  {
    for( y=-4; y<5; y++ )
    {
      for( x=-4; x<5; x++ )
      {
        if( itm.PeekRel(x,y,z) == m_uTissueLabel )
        {
          ft = float(x*x)*vfSquare.m_x + float(y*y)*vfSquare.m_y +float(z*z)*vfSquare.m_z;

          if( ft < fDist )
          {
            m_iClosestTissuePoint = viCenter+Vector3D<int2>(x,y,z);
            fDist = ft;
          }
        } // if
      } // for x
    } // for y
  } // for z

  if( m_iClosestTissuePoint == Vector3D<int2>(0,0,0) )
  {
    LogWrn("No colonic wall tissue was found near the user selected point", "vxPolypSegmentation", "ExtractPolypRegion");
    return(-1);
  }
  
  /// check if this ROI has already segmented
  vxBlockVolumeIterator<uint2> itLabel( m_labelVol );
  itLabel.SetPos(m_iClosestTissuePoint.m_x+m_ixl, m_iClosestTissuePoint.m_y+m_iyl, m_iClosestTissuePoint.m_z+m_izl);
  if( itLabel.GetVoxel() > 0 )
  {
    return(3);  /// It has already been segmented!
  }

  /// get all possible of tissue neighbors as a tissue cluster for seed of polyp
  std::vector< Vector3D<int2> > viTissueSeedList;
  viTissueSeedList.push_back( m_iClosestTissuePoint );
  SearchForNearestTissuePos( m_iClosestTissuePoint, viTissueSeedList );

  std::vector< Vector3D<int2> > viInitialSurfacePos;
  /// determine all surface points within m_fSearchRadius
  ComputeInitialSurfacePos( viInitialSurfacePos );
  
  const int iN( viInitialSurfacePos.size() );
  if( iN < 9 )
  {
    LogWrn("No adjacent surface point was found", "vxPolypSegmentation", "ExtractPolypRegion");
    return(-1);
  }

  std::vector<uint1> viPatchIndicator;
  // initialize the indicator
  for( i=iN-1; i>=0; i-- )
  {
    viPatchIndicator.push_back( 0 );
  }

  /// compute the 1st level polyp surface patch
  for( i=0; i<viTissueSeedList.size(); i++ )
  {
    ComputingPolypSurfacePatch( viInitialSurfacePos, viPatchIndicator, viTissueSeedList[i], m_f1stLevelRayLength, 1 );
  }
    
  ComputingPolypSurfacePatch( viInitialSurfacePos, viPatchIndicator, m_iClosestTissuePoint, m_f3rdLevelRayLength, 1 );

  GrowOutTopSurfacePatch( viInitialSurfacePos, viPatchIndicator, m_iClosestTissuePoint );

  itm.SetPos( itm.GetPos() );  // synchronize iterator after other function call another iterator
  
  std::vector< Vector3D<int2> > viTruePatch;     /// the patch point container
  /// create final patch array
  for( i=iN-1; i>=0; i-- )
  {
    if( viPatchIndicator[i] > 0 )
    {
      viTruePatch.push_back( viInitialSurfacePos[i] );
    }
  } // for i
  /// release memory
  viInitialSurfacePos.clear();
  viPatchIndicator.clear();

  if( viTruePatch.size() < 9 )
  {
    LogWrn("Too few surface point near the selected point", "vxPolypSegmentation", "ExtractPolypRegion");
    return(2);
  }

  const uint1 uTmpLabel_1(m_uPolypLabel-10);
  /// line fill algorithm for generating initial polyp ROI (usually a bit over-estimated)
  ComputeEncloseROIbyLineFilling( viTruePatch, uTmpLabel_1);

  for( i=viTissueSeedList.size()-1; i>=0; i-- )
  {
    itm.SetPos(viTissueSeedList[i]);
    itm.SetVoxel(uTmpLabel_1);
  }
  viTissueSeedList.clear();  // release memory

  const uint1 uTmpLabel_2( uTmpLabel_1+1 );
  /// grow out the single connected polyp ROI
  RegionGrow6NB( m_iClosestTissuePoint, uTmpLabel_1, uTmpLabel_2 );

  vxBlockVolumeIteratorNeighbors<uint1> it18( *m_pmask, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18 );

  const uint1 uTmpLabel_3( uTmpLabel_2+1 );
  /// One layer of erosion to the single connected polyp ROI
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
    {
      if( itm.GetVoxel() == uTmpLabel_2 )
      {
		    for( it18.CenterAt(itm.GetPos()); !it18.IsAtEnd(); ++it18 )
        {
          if( it18.GetVoxel() < uTmpLabel_2 || it18.GetVoxel() > uTmpLabel_3 )
          {
            itm.SetVoxel( uTmpLabel_3 );
            break;
          } // if
        } // for all neighbors
      } // if 
    } // for inside the block
  } // for all blocks

  Vector3D<int2> viNewSeed(0,0,0);
  itm.SetPos( m_iClosestTissuePoint );
  for( z=-2; z<3; z++ )
  {
    for( y=-2; y<3; y++ )
    {
      for( x=-2; x<3; x++ )
      {
        if( itm.PeekRel(x,y,z) == uTmpLabel_2 )
        {
          viNewSeed = m_iClosestTissuePoint + Vector3D<int2>(x,y,z);
          y = 3;
          z = 3;
          break;
        }
      } // for x
    } // for y
  } // for z

  if( viNewSeed == Vector3D<int2>(0,0,0) )
  { /// erosion too much for small ROI, put it back
    LogWrn("Found ROI is too small for erosion", "vxPolypSegmentation", "ExtractPolypRegion");

    // This ROI should be within 1.6 mm radius ball region
    const float fSquareRadius( 6.0F );  /// in square of millimeter

    register Vector3D<float> vf;
    for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
      {
        if( itm.GetVoxel() == uTmpLabel_2 || itm.GetVoxel() == uTmpLabel_3 )
        {
          vf    = Vector3D<float>( itm.GetPos()-m_iClosestTissuePoint );
          fDist = vf.m_x*vf.m_x*vfSquare.m_x + vf.m_y*vf.m_y*vfSquare.m_y + vf.m_z*vf.m_z*vfSquare.m_z;

          if( fDist <= fSquareRadius )
          {
            m_viPolypVoxelContainer.push_back( itm.GetPos() );

            itm.SetVoxel( m_uPolypLabel );
          } // if within the range
        } // if 
      } // for inside block
    } // for all blocks
  } // if

  else
  { /// regular ROI apply the thin plate/edge removal
    RegionGrow6NB( viNewSeed, uTmpLabel_2, m_uPolypLabel );

    const uint1 uTmpLabel_4( uTmpLabel_3+1 );
    // dilation for recovering eroded layer
    for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
      {
        if( itm.GetVoxel() == m_uPolypLabel )
        {
		      for( it18.CenterAt(itm.GetPos()); !it18.IsAtEnd(); ++it18 )
          {
            if( it18.GetVoxel() == uTmpLabel_2 || it18.GetVoxel() == uTmpLabel_3 )
            {
              it18.SetVoxel( uTmpLabel_4 );
            }
          } // for all neighbors
        } // if 
      } // for inside the block
    } // for all blocks

    vxBlockVolumeIteratorNeighbors<uint1> it6( *m_pmask, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
    const uint1 uTmpLabel_5( uTmpLabel_4+1 );
    // dilation for recovering eroded layer
    for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
      {
        if( itm.GetVoxel() == uTmpLabel_4 )
        {
		      for( it6.CenterAt(itm.GetPos()); !it6.IsAtEnd(); ++it6 )
          {
            if( it6.GetVoxel() == uTmpLabel_2 || it6.GetVoxel() == uTmpLabel_3 )
            {
              it6.SetVoxel( uTmpLabel_5 );
            }
          } // for all neighbors
        } // if 
      } // for inside the block
    } // for all blocks

    if( m_viPolypVoxelContainer.size() > 0 )
    {
      LogErr("Polyp ROI container initialization error", "vxPolypSegmentation", "ExtractPolypRegion");
      return(-1);
    }

    for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
      {
        if( itm.GetVoxel() == m_uPolypLabel )
        {
          m_viPolypVoxelContainer.push_back( itm.GetPos() );
        }
        else if( itm.GetVoxel() == uTmpLabel_4 || itm.GetVoxel() == uTmpLabel_5 )
        {
          itm.SetVoxel( m_uPolypLabel );
          m_viPolypVoxelContainer.push_back( itm.GetPos() );
        }
      } // for inside block
    } // for all blocks
  } // else

  if( m_viPolypVoxelContainer.size() < 9 )
  {
    LogErr("Fail to grow out polyp volume", "vxPolypSegmentation", "ExtractPolypRegion");
    return(2);
  }

  bool bGetMeasurement(true);
  try
  {
    bGetMeasurement = ComputePolypMeasurements();
  }
  catch( ... )
  {
    LogErr("Fail to compute the measurements", "vxPolypSegmentation", "ExtractPolypRegion");
    return(-1);
  }

  if( !bGetMeasurement )
  {
    return(-1);
  }

  return(0);
} // ExtractPolypRegion()


/** search for nearest tissue voxel within 26-neighborhood
 *
 *  @param viSeed
 *  @param vTissuePointList
 */
void vxPolypSegmentation::SearchForNearestTissuePos( const Vector3D<int2> & viSeed, 
                                                     std::vector< Vector3D<int2> > & vTissuePointList )
{
  vxBlockVolumeIterator<uint1> itm( *m_pmask );
  /// determine the nearest lumen voxel
  itm.SetPos( viSeed );

  /// collect all tissue neighbor for robust surface patch searching
  if( itm.PeekRel(1,0,0)   == m_uTissueLabel ) 
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(1,0,0) );
  }
  //if( itm.PeekRel(1,1,0)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,1,0) );
  //if( itm.PeekRel(1,-1,0)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,-1,0) );
  if( itm.PeekRel(-1,0,0)  == m_uTissueLabel )
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,0,0) );
  }
  //if( itm.PeekRel(-1,1,0)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,1,0) );
  //if( itm.PeekRel(-1,-1,0) == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,-1,0) );
  if( itm.PeekRel(0,1,0)   == m_uTissueLabel )
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(0,1,0) );
  }
  if( itm.PeekRel(0,-1,0)  == m_uTissueLabel )
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(0,-1,0) );
  }

  if( itm.PeekRel(0,0,1)   == m_uTissueLabel )
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(0,0,1) );
  }
  //if( itm.PeekRel(1,0,1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,0,1) );
  //if( itm.PeekRel(1,1,1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,1,1) );
  //if( itm.PeekRel(1,-1,1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,-1,1) );
  //if( itm.PeekRel(-1,0,1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,0,1) );
  //if( itm.PeekRel(-1,1,1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,1,1) );
  //if( itm.PeekRel(-1,-1,1) == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,-1,1) );
  //if( itm.PeekRel(0,1,1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(0,1,1) );
  //if( itm.PeekRel(0,-1,1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(0,-1,1) );

  if( itm.PeekRel(0,0,-1)   == m_uTissueLabel ) 
  {
    vTissuePointList.push_back( viSeed+Vector3D<int2>(0,0,-1) );
  }
  //if( itm.PeekRel(1,0,-1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,0,-1) );
  //if( itm.PeekRel(1,1,-1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,1,-1) );
  //if( itm.PeekRel(1,-1,-1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(1,-1,-1) );
  //if( itm.PeekRel(-1,0,-1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,0,-1) );
  //if( itm.PeekRel(-1,1,-1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,1,-1) );
  //if( itm.PeekRel(-1,-1,-1) == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(-1,-1,-1) );
  //if( itm.PeekRel(0,1,-1)   == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(0,1,-1) );
  //if( itm.PeekRel(0,-1,-1)  == m_uTissueLabel ) vTissuePointList.push_back( viSeed+Vector3D<int2>(0,-1,-1) );
} // SearchForNearestTissuePosviSeed, vTissuePointList )


/**
 *  determine all surface point within fSearchRadius
 *
 *  @param viInitialSurfacePos the container for the initial surface position
 */
void vxPolypSegmentation::ComputeInitialSurfacePos( std::vector< Vector3D<int2> > & viInitialSurfacePos )
{
  const Triple<int>   viDim  ( m_pmask->GetHeader().GetVolDim()     );
  const Triple<float> vfUnits( m_pmask->GetHeader().GetVoxelUnits() );
  const int ixl( max(viDim.m_x/2+1-int(m_fSearchRadius/vfUnits.m_x), 1) );
  const int ixu( min(viDim.m_x/2+1+int(m_fSearchRadius/vfUnits.m_x), viDim.m_x-2) );
  const int iyl( max(viDim.m_y/2+1-int(m_fSearchRadius/vfUnits.m_y), 1) );
  const int iyu( min(viDim.m_y/2+1+int(m_fSearchRadius/vfUnits.m_y), viDim.m_y-2) );
  const int izl( max(viDim.m_z/2+1-int(m_fSearchRadius/vfUnits.m_z), 1) );
  const int izu( min(viDim.m_z/2+1+int(m_fSearchRadius/vfUnits.m_z), viDim.m_z-2) );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  register int x, y, z;
  for( z=izl, itm.SetPosZ(izl); z<=izu; z++, itm.IncZ() )
  {
    for( y=iyl, itm.SetPosY(iyl); y<=iyu; y++, itm.IncY() )
    {
      for( x=ixl, itm.SetPosX(ixl); x<=ixu; x++, itm.IncX() )
      {
        if( itm.GetVoxel() == m_uWallSurfaceLabel )
        {
          viInitialSurfacePos.push_back( Vector3D<int2>(x,y,z) );
        }
      } // for x
    } // for y
  } // for z
} // ComputeInitialSurfacePos( viInitialSurfacePos )


/**
 *  ray detection algorithm for the polyp surface patch
 *
 *  @param viInitialSurfacePoint    the container of all possible surface point
 *  @param viSurfaceIndicator       the indicator to tell if it is true polyp patch point
 *  @param rayOrigin                the tissue point that the ray sent from
 *  @param fRayLength               is the maximum length that the ray will reaches
 *  @param iNewIndicatorValue       is the indicator index for the found surface
 */
void vxPolypSegmentation::ComputingPolypSurfacePatch( const std::vector< Vector3D<int2> > & viInitialSurfacePos,
          std::vector<uint1> & viPatchIndicator, const Vector3D<int2> & rayOrigin, 
          const float & fRayLength, const uint1 & iNewIndicatorValue )
{
  const Triple<float>   vfUnits ( m_vol.GetHeader().GetVoxelUnits() );
  const Vector3D<float> vff     ( Vector3D<float>(vfUnits.m_x*vfUnits.m_x, vfUnits.m_y*vfUnits.m_y, vfUnits.m_z*vfUnits.m_z) ); 
  const int             iN      ( viInitialSurfacePos.size() );
  const float           fSquareOfRayLength ( fRayLength*fRayLength );

  register Vector3D<float> vfDirection;
  register Vector3D<float> vfCurrentPos;
  register Vector3D< int2> vi;
  register float           f; /// fx, fy, fz;

  vxBlockVolumeIterator<uint1> itm( *m_pmask );
  itm.SetPos( rayOrigin );
  if( itm.GetVoxel() != m_uTissueLabel )
  {
    throw ex::AbortOperationException(LogRec("The ray origin is not tissue voxel", "vxPolypSegmentation", "ComputingPolypSurfacePatch"));
    return;
  }

  register int i, j, m, x, y, z;
  register bool bIsTruePolypSurface;

  /// ray detection algorithm for polyp surface patch
  for( i=0; i<iN; i++ )
  {
    if( viPatchIndicator[i] == 0 )
    {
      /// heading direction in voxel grid
      vi = viInitialSurfacePos[i] - rayOrigin;

      // current ray direction in floating point value
      vfDirection = Vector3D<float>( float(vi.m_x), float(vi.m_y), float(vi.m_z) );
      f = sqrt(vfDirection.m_x*vfDirection.m_x+vfDirection.m_y*vfDirection.m_y+vfDirection.m_z*vfDirection.m_z);
      vfDirection = Vector3D<float>( vfDirection.m_x/f, vfDirection.m_y/f, vfDirection.m_z/f );

      float ff = vfDirection.m_x*vfDirection.m_x*vff.m_x +
                 vfDirection.m_y*vfDirection.m_y*vff.m_y + 
                 vfDirection.m_z*vfDirection.m_z*vff.m_z;

      if( ff <= fSquareOfRayLength )
      { /// the current surface point within the patch range

        // ray length
        j = int(f)+1;

        bIsTruePolypSurface = true;

        vfCurrentPos = rayOrigin;
        /// searching along the ray
        for( m=0; m<j; m++ )
        {
          vfCurrentPos = vfCurrentPos + vfDirection;

          /// round up to the nearest voxel grid point
          x = int(vfCurrentPos.m_x);
          if( vfCurrentPos.m_x - float(x) > 0.5F )
            x++;
          y = int(vfCurrentPos.m_y);
          if( vfCurrentPos.m_y - float(y) > 0.5F )
            y++;
          z = int(vfCurrentPos.m_z);
          if( vfCurrentPos.m_z - float(z) > 0.5F )
            z++;

          itm.SetPos(x, y, z);

          /// check the current ray situation
          if( itm.GetVoxel() == m_uWallSurfaceLabel )
          { // hit a surface point other than the current one
            if( abs(viInitialSurfacePos[i].m_x-x) > 1 || abs(viInitialSurfacePos[i].m_y-y) > 1 || 
                abs(viInitialSurfacePos[i].m_z-z) > 1 )
            {
              bIsTruePolypSurface = false;
            }
            break;
          } // else

          else if( itm.GetVoxel() != m_uTissueLabel )
          { // hit non-tissue point
            bIsTruePolypSurface = false;
            break;
          } // else
        } // for m

        if( bIsTruePolypSurface )
        {
          viPatchIndicator[i] = iNewIndicatorValue;
        }
      } // if the surface point within the defined range

    } // if the surface point has not been determined yet.
  } // for i all initial surface points within certain range
} // ComputingPolypSurfacePatch( viInitialSurfacePos, viPatchIndicator, rayOrigin, fRayLength, iNewIndicatorValue )


/**
 *  computing the enclosed region by line filling from surface patch
 *
 *  @param viSurfacePos the container for the surface voxels
 *  @param uPolyLabel
 */
void vxPolypSegmentation::ComputeEncloseROIbyLineFilling( const std::vector< Vector3D<int2> > & viSurfacePos,
                                                          const uint1 & uPolypLabel )
{
  const Triple<float> vfUnits( m_vol.GetHeader().GetVoxelUnits() );
  const int           iNum   ( viSurfacePos.size() );

  register Vector3D<float> vfDirection;
  register Vector3D<float> vfDD;
  register Vector3D<float> vfCurrentPos;
  register Vector3D< int2> vi;

  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  register int   i, j, m, n, x, y, z;
  register float f;
  /// line filing the polyp body within the detected surface patch
  for( i=iNum-1; i>0; i-- )
  {
    for( j=i-1; j>=0; j-- )
    {
      /// line direction
      vi = viSurfacePos[j] - viSurfacePos[i];
      vfDirection = Vector3D<float>( float(vi.m_x), float(vi.m_y), float(vi.m_z) );
      vfDD = Vector3D<float>(vfDirection.m_x*vfDirection.m_x, vfDirection.m_y*vfDirection.m_y, vfDirection.m_z*vfDirection.m_z);
      f  = sqrt( vfDD.m_x + vfDD.m_y + vfDD.m_z );
      vfDirection = Vector3D<float>( vfDirection.m_x/f, vfDirection.m_y/f, vfDirection.m_z/f );

      // length of the line segment 
      n = int(f)+1;

      vfCurrentPos = viSurfacePos[i];
      /// searching along the line
      for( m=0; m<n; m++ )
      { /// round up to the nearest voxel grid point
        x = int(vfCurrentPos.m_x);
        if( vfCurrentPos.m_x - float(x) > 0.5F )
        {
          x++;
        }
        y = int(vfCurrentPos.m_y);
        if( vfCurrentPos.m_y - float(y) > 0.5F )
        {
          y++;
        }
        z = int(vfCurrentPos.m_z);
        if( vfCurrentPos.m_z - float(z) > 0.5F )
        {
          z++;
        }

        itm.SetPos( x, y, z );
        if( itm.GetVoxel() == m_uTissueLabel )
        {
          itm.SetVoxel( uPolypLabel );
        }

        vfCurrentPos = vfCurrentPos + vfDirection;
      } // for m

    } // for j
  } // for i
} // ComputeEncloseROIbyLineFilling( viSurfacePos, uPolypLabel )


/** 
 *   compute the necessary measurements for the polyp features
 *   
 *   @return true if successful
 */
bool vxPolypSegmentation::ComputePolypMeasurements()
{
  const Triple<float> vfUnits (m_vol.GetHeader().GetVoxelUnits());
  const Triple<float> vff     (vfUnits.m_x*vfUnits.m_x, vfUnits.m_y*vfUnits.m_y, vfUnits.m_z*vfUnits.m_z); 

  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  register int i, j, iN( m_viPolypVoxelContainer.size() );

  m_fVolumeOfPolyp = float(iN)*vfUnits.m_x*vfUnits.m_y*vfUnits.m_z;

  std::vector< Vector3D<int2> > viSurfaceTissue;
  // find all surface voxel of the polyp
  vxBlockVolumeIteratorNeighbors<uint1> it18( *m_pmask, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18 );
  for( i=iN-1; i>=0; i-- )
  {
		for( it18.CenterAt(m_viPolypVoxelContainer[i]); !it18.IsAtEnd(); ++it18 )
		{
      if( it18.GetVoxel() == m_uWallSurfaceLabel )
      {
        viSurfaceTissue.push_back(m_viPolypVoxelContainer[i]);
        break;
      } // if
    } // for neighbors
  } // for i

  iN = viSurfaceTissue.size();
  if( iN < 6 )
  {
    LogWrn("Polyp surface is too small", "vxPolypSegmentation", "ComputePolypMeasurement");
    return(false);
  }

  register Vector3D<float> vfTmp;
  register int ii, jj;
  register float fDistance, fMaxDistance(0.0F);
  /// determine the maximum diameter
  for( i=iN-1; i>=0; i-- )
  {
    for( j=i-1; j>=0; j-- )
    {
      vfTmp = Vector3D<float>( float(viSurfaceTissue[i].m_x-viSurfaceTissue[j].m_x),
                               float(viSurfaceTissue[i].m_y-viSurfaceTissue[j].m_y),
                               float(viSurfaceTissue[i].m_z-viSurfaceTissue[j].m_z) );
      fDistance = vfTmp.m_x*vfTmp.m_x*vff.m_x + vfTmp.m_y*vfTmp.m_y*vff.m_y + vfTmp.m_z*vfTmp.m_z*vff.m_z;

      if( fDistance > fMaxDistance )
      {
        fMaxDistance = fDistance;
        ii = i;  // record the end point index
        jj = j;  // record the end point index
      } // if
    } // for j
  } // for i

  m_vfMaxDiameterEnd1.m_x = float(viSurfaceTissue[ii].m_x+m_ixl)*vfUnits.m_x;
  m_vfMaxDiameterEnd1.m_y = float(viSurfaceTissue[ii].m_y+m_iyl)*vfUnits.m_y;
  m_vfMaxDiameterEnd1.m_z = float(viSurfaceTissue[ii].m_z+m_izl)*vfUnits.m_z;

  m_vfMaxDiameterEnd2.m_x = float(viSurfaceTissue[jj].m_x+m_ixl)*vfUnits.m_x;
  m_vfMaxDiameterEnd2.m_y = float(viSurfaceTissue[jj].m_y+m_iyl)*vfUnits.m_y;
  m_vfMaxDiameterEnd2.m_z = float(viSurfaceTissue[jj].m_z+m_izl)*vfUnits.m_z;

  m_fMaximumDiameter = sqrt(fMaxDistance);

  std::stringstream st;
  st << "Polyp volume = " << m_fVolumeOfPolyp << ";  Maximum diameter = " << m_fMaximumDiameter;
  std::string ss( st.str() );
  LogFyi( ss.c_str(), "vxPolypSegmentation", "ComputePolypMeasurement" );

  /// dilation of the surface for 3D endoluminal color display of the ROI
  const uint1 uTmpLabel01( m_uPolypLabel+1 );
  for( i=iN-1; i>=0; i-- )
  {
		for( it18.CenterAt(viSurfaceTissue[i]); !it18.IsAtEnd(); ++it18 )
		{
      if( it18.GetVoxel() == m_uWallSurfaceLabel || it18.GetVoxel() == m_uSegmentIndex )
      {
        it18.SetVoxel( uTmpLabel01 );
      }
    } // for neighbors
  } // for i
  
  vxBlockVolumeIteratorNeighbors<uint1> it6( *m_pmask, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
  const uint1 uTmpLabel02( uTmpLabel01+1 );
  /// dilation of the associated air surface again
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
    {
      if( itm.GetVoxel() == uTmpLabel01 )
      {
				for( it6.CenterAt(itm.GetPos()); !it6.IsAtEnd(); ++it6 )
				{
          if( it6.GetVoxel() == m_uSegmentIndex )
          {
            it6.SetVoxel( uTmpLabel02 );
          }
        } // for neighbors
      } // if

    } // for inside the block
  } // for all blocks

  return(true);
} // ComputePolypMeasurements()


/** grow out the top surface patch
 *
 *  @param viInitialSurfacePos    is the container for the initial surface points
 *  @param viPatchIndicator       is the container for the indicator of true patch points
 *  @param closestTissuePoint     is the nearest tissue point to the user selected point
 *  @return                       number of grown ___?
 */
int4 vxPolypSegmentation::GrowOutTopSurfacePatch( std::vector< Vector3D<int2> > & viInitialSurfacePos, 
             std::vector<uint1> & viPatchIndicator, const Vector3D<int2> & closestTissuePoint )
{
  const int   iN( viInitialSurfacePos.size() );
  const uint1 uTmpLabel01( m_uWallSurfaceLabel+1 );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  register int i, j, k, min;
  k = -1;
  min = 30000;
  /// determine the nearest surface point to the closestTissuePoint
  for( i=iN-1; i>=0; i-- )
  {
    if( viPatchIndicator[i] > 0 )
    {
      itm.SetPos( viInitialSurfacePos[i] );
      itm.SetVoxel( uTmpLabel01 );

      j = abs( viInitialSurfacePos[i].m_x-closestTissuePoint.m_x ) +
          abs( viInitialSurfacePos[i].m_z-closestTissuePoint.m_z ) +
          abs( viInitialSurfacePos[i].m_y-closestTissuePoint.m_y );
         
      if( j < min )
      {
        min = j;
        k = i;
      } // if
    } // if this pos is in the patch
  } // for i

  if( k == -1 )
  {
    LogErr("Could not get seed for the initial patch", "vxPolypSegmentation", "GrowOutTopSurfacePatch");
    return(-1);
  }

  // queue for region growing
  register    Vector3D<int2>   vi( viInitialSurfacePos[k] );
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  
  register int iCount(1);

  const uint1 uTmpLabel02( uTmpLabel01+1 );
  itm.SetPos(vi);
  itm.SetVoxel(uTmpLabel02);

  vxBlockVolumeIteratorNeighbors<uint1> it26( *m_pmask, Vector3D<int>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );

  // start 26-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();

		for( it26.CenterAt(vi); !it26.IsAtEnd(); ++it26 )
    { 
      if( it26.GetVoxel() == uTmpLabel01 )
      {
        viRegQueue.push( it26.GetPos() );
        it26.SetVoxel( uTmpLabel02 );
        //iCount++;
      } // if
    } // neighbors

	  viRegQueue.pop();
  }  // end of while() 

  for( i=iN-1; i>=0; i-- )
  {
    if( viPatchIndicator[i] > 0 )
    {
      itm.SetPos( viInitialSurfacePos[i] );

      if( itm.GetVoxel() == uTmpLabel01 )
      {
        viPatchIndicator[i] = 0;
      }

      itm.SetVoxel( m_uWallSurfaceLabel );
    } // if this pos is in the patch
  } // for i

  return(iCount);
} // GrowOutTopSurfacePatch( viInitialSurfacePos, viPatchIndicator, closestTissuePoint )


/**
 * Curve surface thinning algorithm for determine the center of the polyp patch
 *
 *  @param viPolypPatchCenter
 *  @param viInitialSurfacePos
 *  @param viPatchIndicator
 *  @return true if center was found
 */
bool vxPolypSegmentation::DeterminePolypPatchCenter( Vector3D<int2> & viPolypPatchCenter, 
        const std::vector<Vector3D<int2>> & viInitialSurfacePos, const std::vector<uint1> & viPatchIndicator )
{
  const Triple<int> dim( m_pmask->GetHeader().GetVolDim() );
  const int iX( dim.m_x );
  const int iY( dim.m_y );
  const int iZ( dim.m_z );
  const int iS( iX*iY );
  const int iN( iS*iZ );

  uint1 * puDat = new uint1[iN];
  
  if( puDat == NULL )
  {
    throw ex::OutOfMemoryException(LogRec("Out of memory for patch center determination", "vxPolypSegmentation", "DeterminePolypPatchCenter"));
  }

  const uint1 uB(0xFF);
  register int x, y, z;

  /// label surface
  for( x=viInitialSurfacePos.size()-1; x>=0; x-- )
  {
    if( viPatchIndicator[x] > 0 )
    {
      puDat[viInitialSurfacePos[x].m_x + iX*viInitialSurfacePos[x].m_y + iS*viInitialSurfacePos[x].m_z] = uB;
    }
  } // for x

  // 26-connected neighbors
  const int viNB[26] =
  {
    1, -1, iX, -iX, iX-1, iX+1, -iX-1, -iX+1,
    iS, iS+1, iS-1, iS+iX, iS-iX, iS+iX-1, iS+iX+1, iS-iX-1, iS-iX+1,
    -iS, -iS+1, -iS-1, -iS+iX, -iS-iX, -iS+iX-1, -iS+iX+1, -iS-iX-1, -iS-iX+1
  };

  Vector3D<float> vfCenterWhole(0.0F,0.0F,0.0F);
  Vector3D<float> vfCenterEdge (0.0F,0.0F,0.0F);
  register int iWhole(0);
  register int iEdge(0);
  register int s, t, i, j, k;

  /// compute the center of all surface points and edge points
  s = 0;
  for( z=0; z<iZ; z++ )
  {
    t = s;
    for( y=0; y<iY; y++ )
    {
      for( x=0; x<iX; x++)
      {
        if( puDat[x+t] == uB )
        {
          vfCenterWhole.m_x = vfCenterWhole.m_x + float(x);
          vfCenterWhole.m_y = vfCenterWhole.m_y + float(y);
          vfCenterWhole.m_z = vfCenterWhole.m_z + float(z);
          iWhole++;

          j = x+t;
          k = 0;
          for( i=0; i<26; i++ )
          {
            if( puDat[j+viNB[i]] == uB )
            {
              k++;
            }
          } // for 26 neighbors

          if( k < 8 )
          {
           vfCenterEdge.m_x = vfCenterEdge.m_x + float(x);
           vfCenterEdge.m_y = vfCenterEdge.m_y + float(y);
           vfCenterEdge.m_z = vfCenterEdge.m_z + float(z);
           iEdge++;
         } // if it is edge point
        } // if 
      } // for x

      t = t+iX;
    } // for y

    s = s + iS;
  } // for z

  if( iEdge == 0 || iWhole == 0 || vfCenterWhole == vfCenterEdge )
  {
    LogWrn("Error occur in center computation", "vxPolypSegmentation", "DeterminePolypPatchCenter");
    return(false);
  }

  vfCenterEdge  = vfCenterEdge /float(iEdge);
  vfCenterWhole = vfCenterWhole/float(iWhole);

  Vector3D<float> vfDirection = vfCenterWhole-vfCenterEdge;
  
  vfDirection = vfDirection.GetNormalized();

  register Vector3D<float> vfTmp(vfCenterWhole);
  for( i=0; i<200; i++ )
  {
    vfTmp = vfTmp+vfDirection;

    t = int(vfTmp.m_x) + iX*int(vfTmp.m_y) + iS*int(vfTmp.m_z);


    if( t > 1 && t < iN && puDat[t] > 0 )
    {
      viPolypPatchCenter.m_x = int(vfTmp.m_x);
      viPolypPatchCenter.m_y = int(vfTmp.m_y);
      viPolypPatchCenter.m_z = int(vfTmp.m_z);

      break;
    }
    else if( t < 1 || t > iN-1 )
    {
      break;
    }
  } // for i

  if( i == 199 )
  {
    LogWrn("Could not find the center of the patch", "vxPolypSegmentation", "DeterminePolypPathcCenter");
    return(false);
  }

  return(true);
} // DeterminePolypPatchCenter( viPolypPatchCenter, viInitialSurfacePos, viPatchIndicator )


/**
 *  general 6-connected region grow
 *
 *  @param viSeedOfLumen is the seed point
 *  @param uROIlabel is the old label index for the ROI
 *  @param uNewLabel is the new label index for the grown area
 */
void vxPolypSegmentation::RegionGrow6NB( const Vector3D<int2> & viSeedOfLumen, 
                        const uint1 & uROIlabel, const uint1 & uNewLabel )
{
  const Triple<int4>  viDim   ( m_pmask->GetHeader().GetVolDim()    );
  const Triple<float> vfUnits ( m_pmask->GetHeader().GetVoxelUnits());
  const int4 iX ( viDim.m_x );
  const int4 iY ( viDim.m_y );
  const int4 iZ ( viDim.m_z );

  vxBlockVolumeIterator<uint1> itm( *m_pmask );

  // queue for region growing
  register    Vector3D<int2>   vi(viSeedOfLumen);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	if( itm.GetVoxel() != uROIlabel )
  {
		return;
  }
  itm.SetVoxel(uNewLabel);

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
	  itm.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX && itm.Peek1px0py0pz() == uROIlabel )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncX();
      itm.SetVoxel(uNewLabel);
			itm.DecX();
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY && itm.Peek0px1py0pz() == uROIlabel )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncY();
      itm.SetVoxel(uNewLabel);
			itm.DecY();
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x >= 0 && itm.Peek1nx0py0pz() == uROIlabel )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.DecX();
      itm.SetVoxel(uNewLabel);
			itm.IncX();
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y >= 0 && itm.Peek0px1ny0pz() == uROIlabel )
		{
      viRegQueue.push( vi );
			itm.DecY();
      itm.SetVoxel(uNewLabel);
			itm.IncY();
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z >= 0 && itm.Peek0px0py1nz() == uROIlabel )
		{
      viRegQueue.push( vi );
			itm.DecZ();
      itm.SetVoxel(uNewLabel);
			itm.IncZ();
		} // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ && itm.Peek0px0py1pz() == uROIlabel )
		{
      viRegQueue.push( vi );
			itm.IncZ();
      itm.SetVoxel(uNewLabel);
			itm.DecZ();
		} // if

	  viRegQueue.pop();
  }  // end of while() 
} // RegionGrow6NB( viSeedOfLumen, uROIlabel, uNewLabel )


#undef FILE_REVISION


// Revision History:
// $Log: vxPolypSegmentation.C,v $
// Revision 1.24.2.5  2010/03/30 20:34:46  dongqing
// the SRT surface area should not eat out the CAD surface area. Fixed
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.24.2.4  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.24.2.3  2008/03/03 20:40:46  dongqing
// standardize the Vol to World coordinate conversion
//
// Revision 1.24.2.2  2008/01/16 23:12:43  dongqing
// minor change on the float to int convertion
//
// Revision 1.24.2.1  2008/01/03 04:27:37  jmeade
// comments and standards.
//
// Revision 1.24  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.23  2007/01/23 20:19:13  dongqing
// set the label for the paint volume SRT ROI
//
// Revision 1.22  2007/01/11 15:03:33  dongqing
// adjust the range from 1.6 -> 2.4 mm for diminutive polyp
//
// Revision 1.21  2007/01/05 16:57:34  dongqing
// improve the duminitive polyp segmentation
//
// Revision 1.20  2007/01/05 16:29:46  dongqing
// remove GrowOutPolyp()
//
// Revision 1.19  2007/01/05 15:55:50  dongqing
// add GetPolypVolumeROI()
//
// Revision 1.18  2007/01/05 15:38:19  dongqing
// optimize the polyp surface patch computation
// The 2nd round of patch computation was removed
//
// Revision 1.17  2006/12/20 16:24:05  dongqing
// Fix the crash bug due to pointer out of bound in PatchCenter determination
//
// Revision 1.16  2006/12/15 23:11:34  dongqing
// determine the center of polyp patch based on the vector from center of edge
// ring to the center of the patch
//
// Revision 1.15  2006/12/13 22:04:33  dongqing
// add determination of the center of patch
//
// Revision 1.14  2006/11/20 19:38:37  dongqing
// add manually fix labelVolume after droping small flat lesion.
// This is temporily solution. It should fix in the ComponentArray.
//
// Revision 1.13  2006/10/13 18:54:32  dongqing
// fix the ends of max diameter coordinate problem
//
// Revision 1.12  2006/10/10 18:11:36  dongqing
// 1st algorithm optimization for the VCS7
// also, it is the 1st version that S&P match reasonable
//
// Revision 1.11  2006/10/03 15:50:36  dongqing
// 3 level patch detection check-in
//
// Revision 1.10  2006/09/29 17:41:44  dongqing
// change the ray length back to 9 mm
//
// Revision 1.9  2006/09/29 16:08:26  dongqing
// change the ray detection to 2-level iteration.
//
// Revision 1.8  2006/09/26 19:34:28  dongqing
// Switch back to work on the labelVolume from sliceRenderData
//
// Revision 1.7  2006/09/26 15:55:36  dongqing
// pass in the bool label volume
//
// Revision 1.6  2006/09/21 21:17:23  dongqing
// allow to output flat polyp
//
// Revision 1.5  2006/09/20 18:52:13  dongqing
// 1st code adjustment for the Suspicious region tool
//
// Revision 1.4  2006/09/14 19:57:30  dongqing
// add flat/diminish polyp detection
//
// Revision 1.3  2006/09/14 18:25:31  dongqing
// tune up the maximum diameter measurement
//
// Revision 1.2  2006/09/13 19:33:29  dongqing
// switch WallSurface from tissue to air near tissue
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/vxPolypSegmentation.C,v 1.24.2.5 2010/03/30 20:34:46 dongqing Exp $
// $Id: vxPolypSegmentation.C,v 1.24.2.5 2010/03/30 20:34:46 dongqing Exp $