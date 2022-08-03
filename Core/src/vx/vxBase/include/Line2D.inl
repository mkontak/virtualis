// $Id: Line2D.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)


// includes 
#include "10_math.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Returns vertex of given index
 * @param i
 * @return
 */
template <class T>
inline const T* Line2D<T>::Vertex(const uint4 i) const
//*************************************************************
{
  if (i!=1 && i!=2)
  {
    throw ex::IllegalArgumentException(LogRec("values not 1 or 2 (out of range)", "Line2D", "Vertex(uint4)"));
  }

  switch(i)
  {
  case 1:
    return m_pV1;
  case 2:
    return m_pV2;
  default:
    return m_pV1; // return something anyway
  }
} // Vertex()


#undef FILE_REVISION


// $Log: Line2D.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 1.3  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.2  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 1.1  2002/03/08 17:21:40  michael
// split header into header and inline file
//
// Revision 3.3  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.2  2001/10/23 21:45:22  liwei
// Code cleaned.
//
// Revision 3.1  2001/10/16 21:48:22  liwei
// Added classes Line2D, Edge2D and Intersector.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Line2D.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Line2D.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
