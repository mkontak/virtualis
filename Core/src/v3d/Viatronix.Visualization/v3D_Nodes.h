// $Id: v3D_Nodes.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "v3D_NodeIDs.h"
#include "vxNodeDrawing.h"
#include "vxNodeDispatcher.h"
#include "vxNodeLocalController.h"


// namespaces
OPEN_GRAPH_NS


// forward declarations
ref class VisualizationGraph;


// simple nodes
DEFINE_SIMPLE_NODE( NodeDrawing, vxNodeDrawing )
DEFINE_SIMPLE_NODE( NodeDispatcher, vxNodeDispatcher )
DEFINE_SIMPLE_NODE( NodeLocalController, vxNodeLocalController )


CLOSE_GRAPH_NS


// $Log: v3D_Nodes.h,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Nodes.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_Nodes.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
