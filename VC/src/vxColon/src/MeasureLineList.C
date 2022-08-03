// $Id: MeasureLineList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "MeasureLineList.h"


/**
 * Constructor
 */
MeasureLineList::MeasureLineList() : std::list<MeasureLine>(), m_currentIter(end())
{
}


/**
 * Override of clear operator, which in addition resets iterator.
 */
void MeasureLineList::clear()
{
  typedef std::list<MeasureLine> msltype;
  msltype::clear(); m_currentIter = end();
}


/**
 * Copy operator.
 *
 * @param src                Source.
 * @return MeasureLineList   *this.
 */
MeasureLineList &MeasureLineList::operator=(const MeasureLineList & src)
{
  clear();
  MeasureLineList::iterator listCurr = begin();
  MeasureLineList::iterator listEnd = end();
  for (; listCurr != listEnd; listCurr++)
  {
    push_back(*listCurr);
  }
  return *this;
}


/**
 * Returns true if <p2> is within <hw> pixels if <p1>
 *
 * @param p1      Point 1.
 * @param p2      Point 2.
 * @param hw      Nearness distance threshold.
 * @return bool   returns true if <p2> is within <hw> pixels if <p1>.
 */
template <class T>
inline bool PtNearPt2D(const Point2D<T>& p1, const Point2D<T>& p2, const T& hw)
{
  return (Rect<T>(p1.m_x - hw, p1.m_y - hw, p1.m_x + hw, p1.m_y + hw).PtInRect(p2));
}


/**
 * Returns a position to place text that will not obscure that of another line's text.
 *
 * @param currLineIter             Line to test.
 * @return MeasureLine::POSITION   Line position where text should be drawn:
 *                                  (1) START if currLine->m_start is not near another endpoint in the list
 *                                  (2) END if (1) is false, and currLine->m_end is not near another endpoint
 *                                  (3) MID if neither (1) nor (2) are true
 */
MeasureLine::POSITION MeasureLineList::GetUniquePos(const MeasureLineList::iterator & currLineIter)
{
  if (empty())
  {
    return MeasureLine::START;
  }

  // Whether or not each end is still valid
  bool bStart(true), bEnd(true);

  Point2D<int4> currLineStartPt = currLineIter->m_drawEnds.m_start, currLineEndPt = currLineIter->m_drawEnds.m_end;

  MeasureLineList::iterator listCurr = begin();
  MeasureLineList::iterator listEnd = end();

  for (; listCurr != listEnd; listCurr++)
  {
    if (currLineIter == listCurr)
    {
      continue;
    }

    if (bStart && (PtNearPt2D(currLineStartPt, listCurr->m_drawEnds.m_start, 25) || PtNearPt2D(currLineStartPt, listCurr->m_drawEnds.m_end, 25)) )
    {
      bStart = false;

      if (!bEnd)
      {
        return MeasureLine::MID;
      }
    }

    if (bEnd && (PtNearPt2D(currLineEndPt, listCurr->m_drawEnds.m_start, 25) || PtNearPt2D(currLineEndPt, listCurr->m_drawEnds.m_end, 25)) )
    {
      bEnd = false;

      if (!bStart)
      {
        return MeasureLine::MID;
      }
    }
  }

  if (bStart)     { return MeasureLine::START;  }
  else if (bEnd)  { return MeasureLine::END;    }
  else            { return MeasureLine::MID;    }
}


// $Log: MeasureLineList.C,v $
// Revision 1.3  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.2  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.1  2006/01/31 14:19:11  frank
// code review
//
// Revision 1.2  2005/11/19 12:05:08  vxconfig
// coding standards
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.4.2.2.4.1.6.1  2005/06/20 17:06:31  jmeade
// Issue 4197: Finally completing the port to Visual Studio 2k3.
//
// Revision 1.4.2.2.4.1  2005/03/16 17:12:50  frank
// Issue #3992: removed unused data fields
//
// Revision 1.4.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.4.2.1  2003/04/14 16:35:53  jmeade
// Issue 2841:  MeasureLine string operator; Comments, code standards.
//
// Revision 1.4  2003/01/22 23:06:37  ingmar
// added missing includes
//
// Revision 1.3  2002/12/24 03:46:54  jmeade
// Use std::list stream i/o.
//
// Revision 1.2  2002/09/12 18:02:41  jmeade
// Check-in from defunct/unused VC_1-2-3 branch:  Code standards.
//
// Revision 1.1.2.1  2002/07/03 17:42:50  jmeade
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/MeasureLineList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
// $Id: MeasureLineList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
