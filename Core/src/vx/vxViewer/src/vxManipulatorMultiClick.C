// $Id: vxManipulatorMultiClick.C,v 1.1.2.1 2009/02/20 22:28:52 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeff Meade (mailto:jmeade@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorMultiClick.h"


// defines
#define FILE_REVISION "$Revision: 1.1.2.1 $"


/**
 * Returns the mouse position
 * @param environment
 */ 
vxManipulatorMultiClick::vxManipulatorMultiClick(vxEnvironment & environment):
m_pEnvironment(&environment)
//*******************************************************************
{
  m_mousePosition     = Point2D<uint2>(0,0);
  m_lastMousePosition = Point2D<uint2>(0,0);
}


/**
 * Get the multiplier based on the shift/control keys when manipulating a linear variable (e.g., slice number)
 * @param uKeyboardState state of the keyboard
 * @return the multiplier
 */
int4 vxManipulatorMultiClick::GetKeyMultiplier(const uint4 uKeyboardState)
//*******************************************************************
{
  uint4 uMultiplier(1);

  // if CTRL is pressed, go four times faster
  if (uKeyboardState & STATE_CTRL)
    uMultiplier *= 4;

  // if SHIFT is pressed, go two times faster
  if (uKeyboardState & STATE_SHIFT)
    uMultiplier *= 2;

  return uMultiplier;
} // GetKeyMultiplier()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorMultiClick.C,v $
// Revision 1.1.2.1  2009/02/20 22:28:52  jmeade
// initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorMultiClick.C,v 1.1.2.1 2009/02/20 22:28:52 jmeade Exp $
// $Id: vxManipulatorMultiClick.C,v 1.1.2.1 2009/02/20 22:28:52 jmeade Exp $
