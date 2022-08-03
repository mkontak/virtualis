// $Id: vxBlockVolumeFilterFitLinearZ.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( george@viatronix.com )

#ifndef vxBlockVolumeFilterFitLinearZ_h
#define vxBlockVolumeFilterFitLinearZ_h


// includes
#include "vxBlockVolumeFilter.h"


/**
 * Interpolate slices efficiently (in Z-direction only)
 */
template < class VoxelType >
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFilterFitLinearZ : public vxBlockVolumeFilter<VoxelType>
{
// public member functions
public:

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter( );
  
}; // class vxBlockVolumeFilterLinear


#endif // vxBlockVolumeFilterFitLinearZ_h


// $Log: vxBlockVolumeFilterFitLinearZ.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.4  2003/05/16 13:16:20  frank
// formatting
//
// Revision 1.3  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.2  2002/07/08 13:23:58  geconomos
// exported class
//
// Revision 1.1  2002/06/10 20:55:35  uday
// Initial Version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterFitLinearZ.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterFitLinearZ.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
