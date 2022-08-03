// $Id: vxBlockVolumeFilter.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilter.h"


// namespaces
using namespace std;


// partial template instantiations
template class vxBlockVolumeFilter< int1>;
template class vxBlockVolumeFilter< int2>;
template class vxBlockVolumeFilter< int4>;
template class vxBlockVolumeFilter<uint1>;
template class vxBlockVolumeFilter<uint2>;
template class vxBlockVolumeFilter<uint4>;
template class vxBlockVolumeFilter<bool>;


/**
 * Default constructor
 */
template <class VoxelType> 
vxBlockVolumeFilter<VoxelType>::vxBlockVolumeFilter()
: m_pSrcVolume( NULL ),
m_pDstVolume( NULL )
{
} // default constructor


/**
 * Copy constructor
 */
template <class VoxelType> 
vxBlockVolumeFilter<VoxelType>::vxBlockVolumeFilter( vxBlockVolumeFilter<VoxelType> & other )
: m_pSrcVolume( other.m_pSrcVolume ),
m_pDstVolume( other.m_pDstVolume )
{
} // copy constructor


/**
 * Sets the source Volume
 *
 * @param other the other filter
 */
template <class VoxelType> 
vxBlockVolumeFilter<VoxelType> const & vxBlockVolumeFilter<VoxelType>::operator =( const vxBlockVolumeFilter<VoxelType> & other )
{
  if ( & other == this )
    return * this;

  m_pSrcVolume = other.m_pSrcVolume;
  m_pDstVolume = other.m_pDstVolume;

  return *this;

} // assignment operator


// $Log: vxBlockVolumeFilter.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
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
// Revision 1.7  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.6  2003/05/16 10:44:51  michael
// code review
//
// Revision 1.5  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.4  2002/05/14 17:55:33  ingmar
// SetDestVolume() -> SetDstVolume()
//
// Revision 1.3  2002/05/13 21:45:18  ingmar
// Tri-Linear filtering now working
//
// Revision 1.2  2002/04/25 17:13:34  ingmar
// removed some const modifiers because we do not yet have const iterators
//
// Revision 1.1  2002/04/24 19:52:12  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilter.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilter.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
