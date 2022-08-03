// $Id: v3D_SelfadaptiveMRWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dongqing Chen (mailto:dongqing@viatronix.com)

// includes
#include "stdafx.h"
#include "v3D_SelfadaptiveMRWindowLevel.h"
#include "vxVolumeHeader.h"
#include <math.h>
#include <cstdlib>

// namespaces
USING_VISUALIZATION_NS

/**
 * Default constructor
 */
SelfadaptiveMRWindowLevel::SelfadaptiveMRWindowLevel( Viatronix::Visualization::Volume ^ intensityVolume )
{
  m_piVolume = intensityVolume;

  m_fWindow2dGeneral = 0.0F;  
  m_fLevel2dGeneral  = 0.0F;
  m_fWindow3dGeneral = 0.0F;  
  m_fLevel3dGeneral  = 0.0F;

  m_fWindow2dMRA = 0.0F;  
  m_fLevel2dMRA  = 0.0F;
  m_fWindow3dMRA = 0.0F;  
  m_fLevel3dMRA  = 0.0F;

  m_fWindow2dMIP = 0.0F;  
  m_fLevel2dMIP  = 0.0F;
  m_fWindow3dMIP = 0.0F;  
  m_fLevel3dMIP  = 0.0F;

  m_fWindow2dEndoView = 0.0F;  
  m_fLevel2dEndoView  = 0.0F;
  m_fWindow3dEndoView = 0.0F;  
  m_fLevel3dEndoView  = 0.0F;

  m_fWindow2dfMR = 0.0F;  
  m_fLevel2dfMR  = 0.0F;
  m_fWindow3dfMR = 0.0F;  
  m_fLevel3dfMR  = 0.0F;

} // constructor()


/** 
 * compute the window level
 * @return true if window level is modifiable, otherwise false.
 */
bool SelfadaptiveMRWindowLevel::IsAvailable()
{

  Triple<int4> viDim( ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() )->GetHeader().GetVolDim() );
  int4 iX( viDim.m_x );
  int4 iY( viDim.m_y );
  int4 iZ( viDim.m_z );

  float4 fMean   ( 0.0 );
  float8 fSquare ( 0.0 );
  // volume iterator
  vxBlockVolumeIterator< uint2 > iter( * ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() ) );

  int4 iNum( 0 ), x, y;
  uint2 iValue, uMax( 0 );
  // compute mean and std for the center slice
  iter.SetPosZ(iZ/2);
  for ( y=0, iter.SetPosY(0); y<iY; y++, iter.IncY() )
  {
    for ( x=0, iter.SetPosX(0); x<iX; x++, iter.IncX() )
    {
      iValue = iter.GetVoxel();
      if ( iValue > 0 )
      {
        fMean   = fMean   + static_cast<float4>(iValue);
        fSquare = fSquare + static_cast<float8>(iValue)*static_cast<float8>(iValue);
        iNum++;

        if ( iValue > uMax )
          uMax = iValue;
      }
    } // for ( x )
  } // for ( y )

  fMean = fMean / static_cast<float4>( iNum );
  float4 fStd  = sqrt( fSquare/static_cast<float8>(iNum) - static_cast<float8>(fMean)*static_cast<float8>(fMean) );
  
  vxVolumeHeader volHeader( ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() )->GetHeader() );

  //Use the iSignificantBitsPerVoxel value here
  int4 iSignificantBitsPerVoxel = volHeader.GetSignificantBitsPerVoxel();
  if( iSignificantBitsPerVoxel != 0 )
  { 
    float4 fMaximumIntensity( 4095.0 );
    if ( iSignificantBitsPerVoxel >= 13 )
      fMaximumIntensity = 65535.0;
    else if ( iSignificantBitsPerVoxel == 8 )
      fMaximumIntensity = 255;

    // general
    m_fWindow2dGeneral = (fMean + 2.0F*fStd)/fMaximumIntensity;
    m_fLevel2dGeneral  = (fMean + fStd)/fMaximumIntensity;

    m_fWindow3dGeneral = (4.0F*fStd)/fMaximumIntensity;
    m_fLevel3dGeneral  = (fMean + 4.0F*fStd)/fMaximumIntensity;
    
    // MRA
    float fLowBound( 0.0 );
    DetermineLowBoundForMRTOF( fLowBound, static_cast<uint2>(fMean), uMax );

    m_fWindow2dMRA = (3.0*fStd)/fMaximumIntensity;
    m_fLevel2dMRA  = (fMean + 2.8*fStd)/fMaximumIntensity;

    m_fWindow3dMRA = (fStd)/fMaximumIntensity;
    m_fLevel3dMRA  = fLowBound/fMaximumIntensity;

    m_fLevel2dMIP  = 2.0*m_fLevel2dMRA;
    m_fWindow2dMIP = 2.5*m_fLevel2dMIP;

    m_fLevel3dMIP  = 1.5*m_fLevel3dMRA;
    m_fWindow3dMIP = 3.0*m_fLevel3dMIP;

    /*/ fMR
    m_fWindow2dfMR = (fMean + 2.0*fStd)/fMaximumIntensity;
    m_fLevel2dfMR  = (fMean + fStd)/fMaximumIntensity;

    m_fWindow3dfMR = (6.0*fStd)/fMaximumIntensity;
    m_fLevel3dfMR  = (fMean + fStd)/fMaximumIntensity; */

    // endoluminal view
    m_fWindow2dEndoView = (3.0*fStd)/fMaximumIntensity;
    m_fLevel2dEndoView  = (fMean)/fMaximumIntensity;

    m_fWindow3dEndoView = (4.0*fStd)/fMaximumIntensity;
    m_fLevel3dEndoView  = (fMean + 3.5*fStd)/fMaximumIntensity;

    return(true);
  } // if
  else
  {
    return(false);
  } // else 
  
} // IsAvailable()


/** 
 * determine the lower bound for the MRTOF cases
 * @param fLowBound
 * @param uMean
 * @param uMax
 */
void SelfadaptiveMRWindowLevel::DetermineLowBoundForMRTOF( float & fLowBound,
                                                           uint2 uMean, uint2 uMax )
{

  Triple<int4> viDim( ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() )->GetHeader().GetVolDim() );
  int4  iX  ( viDim.m_x );
  int4  iY  ( viDim.m_y );
  int4  iZB ( viDim.m_z/2-3 );
  int4  iZE ( viDim.m_z/2+3 );

  int4   iBinNum ( 200 );    // histogram bin number
  float4 fGap    ( static_cast<float4>( uMax - uMean )/static_cast<float4>(iBinNum) );

  std::vector<int4> viBin;
  int4 iD = 0;

  int4 i;
  // initialize the histogram bins
  for( i=0; i<iBinNum; i++ )
    viBin.push_back( 0 );

  // volume iterator
  vxBlockVolumeIterator< uint2 > iter( * ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() ) );

  int4  x, y, z;
  uint2 uValue( 0 );
  // fill the histogram bin based on the central 7 slices
  for ( z=iZB, iter.SetPosZ(iZB); z<iZE; z++, iter.IncZ() )
  {
    for ( y=0, iter.SetPosY(0); y<iY; y++, iter.IncY() )
    {
      for ( x=0, iter.SetPosX(0); x<iX; x++, iter.IncX() )
      {
        uValue = iter.GetVoxel();
        if ( uValue >= uMean )
        {
          iD = static_cast<int4>( static_cast<float4>(uValue - uMean)/fGap ); 

          if ( iD < iBinNum )
            viBin[iD] = viBin[iD] + 1;
          else
            viBin[iBinNum-1] = viBin[iBinNum-1] + 1;

        } // if ()
      } // for ( x ) 
    } // for ( y )
  } // for ( z )

  int4 iHeighestBinNum   ( -1 );
  int4 iHeighestBinIndex ( iBinNum-1 );
  // determine the heightest bin
  for( i=0; i<iBinNum; i++ )
  {
    if( viBin[i] > iHeighestBinNum )
    {
      iHeighestBinNum   = viBin[i];
      iHeighestBinIndex = i;
    } // if
  } // for(i)

  int4 iLowBinNumber ( static_cast<int4>( static_cast<float4>(iHeighestBinNum)/20.0f ) );
  int4 iLowBinIndex  ( iBinNum-1 );
  // determine the real low bin
  for( i=iHeighestBinIndex+1; i<iBinNum; i++ )
  {
    if( viBin[i] < iLowBinNumber )
    {
      iLowBinIndex = i;
      i = iBinNum;
    } // if
  } // for ()

  // get the lower bound for the MRA case
  fLowBound = static_cast<float4>(iLowBinIndex)*fGap + static_cast<float4>(uMean);

} // DetermineLowBoundForMRTOF()


// Revision History:
// $Log: v3D_SelfadaptiveMRWindowLevel.cpp,v $
// Revision 1.4  2007/02/28 22:16:49  dongqing
// update PT and MRI W/L computation due to the change of using
// significantBits to determine maximum value in volume.
//
// Revision 1.3  2007/02/14 20:48:01  romy
// Threshold values are now using 'iSignificantBitsPerVoxel' instead of  'bitStored' value from the Volume Header. Issue# 5412
//
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/02/06 21:16:30  dongqing
// add MIP preset for MRA
//
// Revision 1.6  2003/08/11 15:34:09  michael
// code review
//
// Revision 1.5  2003/07/11 12:59:53  dongqing
// comments
//
// Revision 1.4  2003/07/10 12:47:22  dongqing
// add automatic lower bound estimation for the MRTOF
//
// Revision 1.3  2003/06/24 14:21:03  dongqing
// change int2 to uint2
//
// Revision 1.2  2003/06/20 14:50:19  dongqing
// adjust MR preset group
//
// Revision 1.1  2003/06/13 18:59:44  dongqing
// initialization
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SelfadaptiveMRWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
// $Id: v3D_SelfadaptiveMRWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
