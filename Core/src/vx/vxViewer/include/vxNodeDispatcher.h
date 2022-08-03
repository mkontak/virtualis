// $Id: vxNodeDispatcher.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
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


class VX_VIEWER_DLL vxNodeDispatcher : public vxVisualizationGraphNode
{
// functions
public:

  /// executes the node
  void Run();

}; // class vxNodeDispatcher


// $Log: vxNodeDispatcher.h,v $
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeDispatcher.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
// $Id: vxNodeDispatcher.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
