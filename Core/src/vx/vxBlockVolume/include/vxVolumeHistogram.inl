// $Id: vxVolumeHistogram.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


#define FILE_REVISION "$Revision: 1.1.1.1 $"


///////////////////////////////////////////////////////////////////////
// get & set info

/**
 * Returns the count for the bin specified by index.
 */
int4 vxVolumeHistogram::GetCountAt(int4 iIndex) const
{
  return m_vCount[iIndex];
} // GetCountAt()


/**
 * Returns number of bins (how many bins there are).
 */
int4 vxVolumeHistogram::GetNumberOfBins() const
{
  return static_cast< int4 >( m_vCount.size() );
} // GetCountAt()



#undef FILE_REVISION

// $Log: vxVolumeHistogram.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:51:41  michael
// code review
//
// Revision 1.1  2002/10/01 19:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVolumeHistogram.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxVolumeHistogram.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $