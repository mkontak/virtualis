// $Id: v3D_SelfadaptivePTWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
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
#include "v3D_SelfadaptivePTWindowLevel.h"
#include "vxVolumeHeader.h"
#include <math.h>

// defines
#define FILE_REVISION "$Revision: 1.4 $"

// namespaces
USING_VISUALIZATION_NS

/**
 * Default constructor
 */
SelfadaptivePTWindowLevel::SelfadaptivePTWindowLevel( Viatronix::Visualization::Volume ^ intensityVolume )
{
  m_piVolume = intensityVolume;

  m_fWindow2d = 0.0F;  
  m_fLevel2d  = 0.0F;
  m_fWindow3d = 0.0F;  
  m_fLevel3d  = 0.0F;

} // constructor()


/** 
*  compute the window level
*  @return true if window level is modifiable, otherwise false.
*/
bool SelfadaptivePTWindowLevel::IsAvailable()
{

  Triple<int4> viDim( ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() )->GetHeader().GetVolDim() );
  int4 iX( viDim.m_x );
  int4 iY( viDim.m_y );
  int4 iZ( viDim.m_z );

  // volume iterator
  vxBlockVolumeIterator< uint2 > iter( * ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() ) );

  uint2 iValue, uMax( 0 );
  // compute mean and std
  for ( iter.SetPos(0,0,0); iter.IsNotAtEnd(); iter.NextBlockZYX() )
  {
    for ( ; iter.IsNotAtEndOfBlock(); iter.NextZYXinsideBlock() )
    {
      iValue = iter.GetVoxel();
      
      if( iValue > uMax )
          uMax = iValue;

    } // for ( inside block )
  } // for ( all blocks ) 

  vxVolumeHeader volHeader( ToNativeVolumeUint2Ptr( m_piVolume->GetVolumePtr() )->GetHeader() );

  //Use the iSignificantBitsPerVoxel value here
  int4 iSignificantBitsPerVoxel = volHeader.GetSignificantBitsPerVoxel();
  if ( iSignificantBitsPerVoxel > 0 )
  { 
    float4 fMaximumIntensity( 4095.0 );
    if ( iSignificantBitsPerVoxel >= 13 )
      fMaximumIntensity = 65535.0;
    else if ( iSignificantBitsPerVoxel == 8 )
      fMaximumIntensity = 255;

    {
      m_fLevel2d = 3.0F*(float)(uMax)/5.0F/fMaximumIntensity;
      m_fWindow2d = 1.0F/3.0F;
      m_fWindow2dInverse = 1.0F/2.0F;

      m_fLevel3d = 2.0F*(float)(uMax)/5.0F/fMaximumIntensity;
      m_fWindow3d = 1.0F/4.5F;
    }

    return(true);
  } // if
  else
  {
    LogErr("Volume header was incorrect", "IsAvailable", "v3D_SelfadaptivePTWindowLevel");
    return(false);
  }

} // IsAvailable()


#undef FILE_REVISION 


// Revision History:
// $Log: v3D_SelfadaptivePTWindowLevel.cpp,v $
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
// Revision 1.3  2003/08/11 15:34:09  michael
// code review
//
// Revision 1.2  2003/08/08 21:24:14  dongqing
// finalize PET preset
//
// Revision 1.1  2003/08/07 19:26:46  dongqing
// new
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SelfadaptivePTWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
// $Id: v3D_SelfadaptivePTWindowLevel.cpp,v 1.4 2007/02/28 22:16:49 dongqing Exp $
