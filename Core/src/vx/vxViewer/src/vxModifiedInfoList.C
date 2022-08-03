// $Id: vxModifiedInfoList.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxModifiedInfoList.h"
#include "vxModifiedInfo.h"
#include <algorithm>


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 */
vxModifiedInfoList::vxModifiedInfoList() :
 m_iter(m_messages.begin())
   //*******************************************************************
{
} // constructor


/**
 * Destructor
 */
vxModifiedInfoList::~vxModifiedInfoList()
//*******************************************************************
{
  ReleaseObjects();
} // destructor


/**
 * Returns the element at the specified position
 * @param position
 * @return modified info
 */
const vxModifiedInfo * vxModifiedInfoList::At(uint4 uPos)
//*******************************************************************
{
  if (uPos >= m_messages.size()) 
  {
    throw ex::IndexOutOfBoundsException(LogRec("List index out of bound.","vxModifiedInfoList","At"));
  }

  m_iter = m_messages.begin();
  std::advance(m_iter, uPos);

  return *m_iter;  
} // At()


/**
 * Returns the first element
 * @return modified info
 */
const vxModifiedInfo * vxModifiedInfoList::Begin()
//*******************************************************************
{
  if (m_messages.empty())
  {
    return NULL;
  }

  m_iter = m_messages.begin();

  return *m_iter;
} // Begin()


/**
 * Returns the next element from the current position
 * @return modified info
 */
const vxModifiedInfo * vxModifiedInfoList::Next()
//*******************************************************************
{  
  if (m_iter == m_messages.end() || ++m_iter == m_messages.end())
  {
    return NULL;
  }

  return *m_iter;
} // Next()


/**
 * Returns the number of elements in the list
 * @return size
 */
int4 vxModifiedInfoList::Size() const
//*******************************************************************
{
  return m_messages.size();
} // Size()


/**
 * Adds an element to the list
 * @param modified info
 */
void vxModifiedInfoList::PushBack(const vxModifiedInfo & info)
//*******************************************************************
{
  vxModifiedInfo * pInfo(NULL);

  info.Clone(&pInfo);
  m_messages.push_back(pInfo);
} // PushBack()


/**
 * Releases all objects in the list.
 */
void vxModifiedInfoList::ReleaseObjects() 
//*******************************************************************
{ 
  std::for_each(m_messages.begin(), m_messages.end(), ReleaseObject()); 
  m_messages.clear(); 
  m_iter = m_messages.begin(); 
} // ReleaseObjects()


#undef FILE_REVISION


// Revision History:
// $Log: vxModifiedInfoList.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.6  2002/07/25 17:45:33  dmitry
// vxModifiedInfo copied and managed as an internal pointer.
//
// Revision 1.5  2002/07/24 16:54:10  ingmar
// ok, my fault. it is just fine to delete a "const int * pNumber"
// only "int const * pNumber" would have been a problem.
//
// Revision 1.4  2002/07/15 23:09:32  ingmar
// removed pointer delete to allow compolation NEED TO FIX THIS SO THERE IS NO MEMORY LEAK
//
// Revision 1.3  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.2  2002/07/10 11:44:30  geconomos
// exported classes.
//
// Revision 1.1  2002/07/08 15:24:53  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxModifiedInfoList.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxModifiedInfoList.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
