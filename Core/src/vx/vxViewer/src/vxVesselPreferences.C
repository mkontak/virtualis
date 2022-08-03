// $Id: vxVesselPreferences.C,v 1.1.8.1 2008/11/18 15:13:35 dongqing Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxVesselPreferences.h"


// static member initialization
bool vxVesselPreferences::m_bRotateCurvedMPR = true;


/**
 * Default constructor
 */
vxVesselPreferences::vxVesselPreferences()
{
  Reset();
} // vxVesselPreferences()


/**
 * Reset to defaults
 */
void vxVesselPreferences::Reset()
{
  m_bDisplayCenterline = true;
  m_bDisplayOutline    = false;
  m_bDisplayGraphs     = true;
} // Reset()


// $Log: vxVesselPreferences.C,v $
// Revision 1.1.8.1  2008/11/18 15:13:35  dongqing
// add the initialization for the displaying vessel outline
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVesselPreferences.C,v 1.1.8.1 2008/11/18 15:13:35 dongqing Exp $
// $Id: vxVesselPreferences.C,v 1.1.8.1 2008/11/18 15:13:35 dongqing Exp $
