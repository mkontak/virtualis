// $Id: MeasureAreaList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
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
#include "MeasureAreaList.h"



/**
 * Constructor
 */
MeasureAreaList::MeasureAreaList() :
std::list<MeasureArea>(),
m_currentIter( end() )
{
}


/**
 * Copy operator.
 *
 * @param other                Source.
 * @return MeasureAreaList &   *this.
 */
MeasureAreaList & MeasureAreaList::operator=(const MeasureAreaList & other)
{
  clear();
  for (MeasureAreaList::iterator listCurr = begin(); listCurr != end(); listCurr++)
  {
    push_back(*listCurr);
  }
  m_currentIter = end();
  return *this;
} // operator ==


/**
 * Selects the nearest proximal handle of a measure area in the list.
 *
 * @param position   Point to test.
 * @return bool      True if a handle proximal handle found (and thus selected), false otherwise.
 */
bool MeasureAreaList::SelectHandle(const Point<float4> & position)
{
  m_currentIter = end();
  for (MeasureAreaList::iterator listCurr = begin(); listCurr != end(); listCurr++)
  {
    bool bResult = listCurr->SelectHandle(position);
    if (bResult == true)
    {
      m_currentIter = listCurr;
    }
  }

  return (m_currentIter != end());
} // SelectHandle


/**
 * Overridden to reset the measure area list, reset the current iterator.
 */
void MeasureAreaList::clear()
{
  m_currentIter = end();
  typedef std::list<MeasureArea> MeasureAreaListBaseClass;
  MeasureAreaListBaseClass::clear();
}


// $Log: MeasureAreaList.C,v $
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
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/MeasureAreaList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
// $Id: MeasureAreaList.C,v 1.3 2006/06/01 20:28:29 frank Exp $
