// $Id: vxMomentum.C,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: george economos (george@viatronix.com)

#include "stdafx.h"
#include "vxMomentum.h"


/**
 * Default constructor
 */
vxMomentum::vxMomentum() : 
  m_bEnabled( false ),
  m_bInitiated( false ),
  m_fUpAngle( 0.0F ),
  m_fRightAngle( 0.0F ),
  m_fViewDirectionRightAngle( 0.0F ),
  m_fViewDirectionUpAngle( 0.0F ),
  m_iFramesPerSecond( 15 )
{
} // vxMomentum()


/**
 * Destructor
 */
vxMomentum::~vxMomentum()
{
} // ~vxMomentum()


/**
 * Resets the momentum instance to a deafult state
 */
void vxMomentum::Reset()
{
  m_bEnabled = false;
  m_bInitiated= false;

  m_fUpAngle      = 0.0F;
  m_fRightAngle   = 0.0F;
  m_fViewDirectionRightAngle = 0.0F;
  m_fViewDirectionUpAngle = 0.0F;
} // Reset();




// $Log: vxMomentum.C,v $
// Revision 1.3  2007/01/16 17:03:48  gdavidson
// Moved angle information from manuipulator to data object
//
// Revision 1.2  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.2.2  2005/08/15 15:28:58  geconomos
// Added Reset method to return instance to default state
//
// Revision 1.1.2.1  2005/08/12 17:20:07  geconomos
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMomentum.C,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
// $Id: vxMomentum.C,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
