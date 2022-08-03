// $Id: vxInputFeedback.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxInputFeedback.h"


/**
 * Destructor
 */
vxInputFeedback::~vxInputFeedback()
//*******************************************************************
{
  m_feedbackList.clear();
} // desctructor


/**
 * Add
 * @param feedback
 */
void vxInputFeedback::Add(vxFeedback & feedback)
//*******************************************************************
{
  m_feedbackList.push_back(feedback);
} // Add()


/**
 * Clear
 */
void vxInputFeedback::Clear()
//*******************************************************************
{
  m_feedbackList.clear();
} // Clear()


/**
 * Returns the feedbackliost,
 * @return feedback list
 */
std::list<vxFeedback> & vxInputFeedback::GetList()
//*******************************************************************
{
  return m_feedbackList;
} // GetList()


// Revision History:
// $Log: vxInputFeedback.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/04/18 19:56:30  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/06/14 20:13:12  geconomos
// Implemented GetList()
//
// Revision 1.1  2002/06/13 23:24:24  michael
// added vxInputFeedback class so that manipulators (input receivers) can send
// a message to a global queue which VB can read to remove them from the list
// of receivers upon completion of operation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxInputFeedback.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxInputFeedback.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $7 20:15:21 dmitry Exp $
