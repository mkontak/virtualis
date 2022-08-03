// $Id: vxRenderingNodeThreadArgs.h,v 1.1 2007/04/12 14:15:35 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#pragma once

#include "Rect.h"

class vxRenderingNodePlugin;

struct VX_VIEWER_DLL vxRenderingNodeThreadArgs
{
  /// rendering node plugin
  vxRenderingNodePlugin * m_pPlugin;
  
  /// total number of tiles to render
  LONG m_iNumberOfTiles;
  
  /// next tile to render
  LONG m_iNextTile;

  /// number of tiles in a single row
  int4 m_iNumberOfTilesInRow;

  /// number of rows
  int4 m_iNumberOfRows;
  
  /// tile size
  Triple< uint2 > m_tileSize;

}; // struct vxRenderingNodeThreadArgs

// $Log: vxRenderingNodeThreadArgs.h,v $
// Revision 1.1  2007/04/12 14:15:35  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodeThreadArgs.h,v 1.1 2007/04/12 14:15:35 geconomos Exp $
// $Id: vxRenderingNodeThreadArgs.h,v 1.1 2007/04/12 14:15:35 geconomos Exp $
