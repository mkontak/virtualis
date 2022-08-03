// $Id: vxHistogramUpdaterNull.h,v 1.3 2007/03/01 19:04:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declaration
#pragma once

// include declaration
#include "vxBlockVolume.h"

// template class definition
template< typename VoxelType >
class vxHistogramUpdaterNull
{
// member functions
public:


   /**
    * Constructor
    *
    * @param    volume    volume to generate histogram for
    */
  vxHistogramUpdaterNull( vxBlockVolume< VoxelType > & volume ) { }
  
  
  /**
   * Updates the components binary histograms for all blocks contained within the specified volume
   *
   * @param   volume   label volume to update
   */
  inline void Update() { }

  
  /** 
   * Updates the binary histogram for the specified block
   */
  inline void UpdateBlock( vxBlock< VoxelType > & block, int4 iMultiplier = 1 ) { }


  /**
   * Completes the update operation
   */
  inline void Complete() {}

}; // class vxHistogramUpdaterNullLabel


// $Log: vxHistogramUpdaterNull.h,v $
// Revision 1.3  2007/03/01 19:04:37  geconomos
// code review preparation
//
// Revision 1.2  2006/05/31 16:16:42  geconomos
// porting to visual studio 2005
//
// Revision 1.1  2006/04/11 13:37:06  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxHistogramUpdaterNull.h,v 1.3 2007/03/01 19:04:37 geconomos Exp $
// $Id: vxHistogramUpdaterNull.h,v 1.3 2007/03/01 19:04:37 geconomos Exp $
