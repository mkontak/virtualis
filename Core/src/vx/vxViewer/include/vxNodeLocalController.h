// $Id: vxNodeLocalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// includes
#include "vxVisualizationGraphNode.h"


class VX_VIEWER_DLL vxNodeLocalController : public vxVisualizationGraphNode
{
// functions
public:

  /// does the node requires a flush
  inline bool IsFlushNecessary() const { return true; }

  /// executes the node
  void Run();

}; // class vxNodeLocalController


// $Log: vxNodeLocalController.h,v $
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeLocalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
// $Id: vxNodeLocalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
