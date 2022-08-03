// $Id: vxBlockVolumeFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef vxBlockVolumeFilter_h
#define vxBlockVolumeFilter_h


// includes
#include "vxBlockVolume.h"


/**
 * Block volume server. Can make and return volumes of any resolution.
 */
template <class VoxelType>
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFilter
{
// member functions
public:

  /// default contructor
  vxBlockVolumeFilter();

  /// copy constructor
  vxBlockVolumeFilter( vxBlockVolumeFilter<VoxelType> & other );

  /// assignment operator
  vxBlockVolumeFilter<VoxelType> const & operator =( const vxBlockVolumeFilter<VoxelType> & other );

  /// default contructor
  virtual ~vxBlockVolumeFilter() {}

  /// sets the source Volume
  void SetSrcVolume( vxBlockVolume<VoxelType> & rSrcVolume ) { m_pSrcVolume = & rSrcVolume; }

  /// sets the destination Volume
  void SetDstVolume( vxBlockVolume<VoxelType> & rDstVolume ) { m_pDstVolume = & rDstVolume; }

  /// filters the source volume and writes the result into the desination volume
  virtual void ApplyFilter( ) { } // = 0; ToDo: go back to =0 when moving to VC++ 7.0
  
// data members
protected:

  /// source volume
  vxBlockVolume<VoxelType> * m_pSrcVolume;

  /// destination volume
  vxBlockVolume<VoxelType> * m_pDstVolume;

}; // class vxBlockVolumeFilter


#endif // vxBlockVolumeFilter_h


// Revision History:
// $Log: vxBlockVolumeFilter.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.9  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.8  2003/05/16 13:16:20  frank
// formatting
//
// Revision 1.7  2003/05/16 10:44:51  michael
// code review
//
// Revision 1.6  2002/07/08 13:23:58  geconomos
// exported class
//
// Revision 1.5  2002/05/14 17:55:33  ingmar
// SetDestVolume() -> SetDstVolume()
//
// Revision 1.4  2002/05/13 21:45:18  ingmar
// Tri-Linear filtering now working
//
// Revision 1.3  2002/05/09 22:23:17  ingmar
// removed =0 to avoid compiler warnings
//
// Revision 1.2  2002/04/25 17:13:34  ingmar
// removed some const modifiers because we do not yet have const iterators
//
// Revision 1.1  2002/04/24 19:52:12  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
