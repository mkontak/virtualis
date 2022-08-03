// $Id: vxRenderingInfo.h,v 1.1 2006/08/28 14:50:02 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once

// forward delcation
class vxViewer;
class vxRenderingEngine;

// struct declaration
struct VX_VIEWER_DLL vxRenderingInfo
{
  /// associated viewer
  vxViewer * m_pViewer;

  /// time taken to render last image
  float4 m_fRenderTime;

  /// last render size
  Triple< uint4 > m_renderSize;

  /// indicates if rendering is necessary
  bool m_bNeedsRendering;

}; // struct vxRenderingInfo


// $Log: vxRenderingInfo.h,v $
// Revision 1.1  2006/08/28 14:50:02  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingInfo.h,v 1.1 2006/08/28 14:50:02 geconomos Exp $
// $Id: vxRenderingInfo.h,v 1.1 2006/08/28 14:50:02 geconomos Exp $
