// $Id: vxVisualizationPreferences.C,v 1.11 2007/06/29 20:20:23 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxVisualizationPreferences.h"


// static member initialization
bool vxVisualizationPreferences::m_bRotateCurvedMPR = false;
float4 vxVisualizationPreferences::m_fFrameRateMin3D = 0.0F;
float4 vxVisualizationPreferences::m_fFrameRateMax3D = 0.0F;
float4 vxVisualizationPreferences::m_fFrameRateMinOblique = 0.0F;
float4 vxVisualizationPreferences::m_fFrameRateMaxOblique = 0.0F;
int4 vxVisualizationPreferences::m_iRenderSizeMin3D = 0;
int4 vxVisualizationPreferences::m_iRenderSizeMax3D = 0;
int4 vxVisualizationPreferences::m_iRenderSizeMinOblique = 0;
int4 vxVisualizationPreferences::m_iRenderSizeMaxOblique = 0;
int4 vxVisualizationPreferences::m_iRenderSizeInitial3D = 0;
int4 vxVisualizationPreferences::m_iRenderSizeInitialOblique = 0;
bool vxVisualizationPreferences::m_bMaintainFrameRate3D = true;
bool vxVisualizationPreferences::m_bMaintainFrameRateOblique = true;

bool vxVisualizationPreferences::m_bShowStatistics = false;
bool vxVisualizationPreferences::m_bShowCoordinates = false;
bool vxVisualizationPreferences::m_bShowHU = false;
bool vxVisualizationPreferences::m_bHighQualityEnabled = true;
int4 vxVisualizationPreferences::m_iMaxRenderThreads = 32;

int4 vxVisualizationPreferences::m_iTileSize2D = 64;
int4 vxVisualizationPreferences::m_iTileSize3D = 32;
int4 vxVisualizationPreferences::m_iTileSizeOblique = 32;


// $Log: vxVisualizationPreferences.C,v $
// Revision 1.11  2007/06/29 20:20:23  geconomos
// added rendering tile size
//
// Revision 1.10  2007/01/19 15:28:50  geconomos
// reorganized visualization preferences
//
// Revision 1.9  2007/01/17 22:11:36  gdavidson
// Added Get and set for HighQualityEnabled
//
// Revision 1.8  2006/11/24 19:51:44  romy
// added shosHU flag
//
// Revision 1.7  2006/10/31 20:52:00  romy
// added show coordinate vizualization preset
//
// Revision 1.6  2006/09/20 15:04:25  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.5  2006/08/23 17:26:47  geconomos
// added "DrawTimingStatistics"
//
// Revision 1.4  2006/08/23 15:53:27  gdavidson
// Added SliceShadowsAllEnabled
//
// Revision 1.3  2006/08/03 14:02:16  geconomos
// added "RenderingManagerEnabled" flag
//
// Revision 1.2  2006/07/19 18:09:11  gdavidson
// Changed the return and parameter types
//
// Revision 1.1  2006/07/19 15:41:54  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVisualizationPreferences.C,v 1.11 2007/06/29 20:20:23 geconomos Exp $
// $Id: vxVisualizationPreferences.C,v 1.11 2007/06/29 20:20:23 geconomos Exp $
