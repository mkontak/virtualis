// $Id: vxZoomFactor.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxZoomFactor.h"


/**
 * Copy constructor
 * @param colormap
 */
vxZoomFactor::vxZoomFactor(const vxZoomFactor & other)
//*******************************************************************
{
  *this = other;
} // copy constructor()


/**
 * Assignment operator
 * @param other 
 * @return const reference to this
 */
const vxZoomFactor & vxZoomFactor::operator=(const vxZoomFactor & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_center = other.m_center;
    m_fSize  = other.m_fSize;
  }

  return *this;
} // operator=()


// Revision History:
// $Log: vxZoomFactor.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2004/02/20 23:32:50  michael
// added new zoom factor class
//
// $Id: vxZoomFactor.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxZoomFactor.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
