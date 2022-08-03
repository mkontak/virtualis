// $Id: v3D_NodeIDs.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)


#include "StdAfx.h"
#include "v3D_NodeIDs.h"
#include "vxNodeIDs.h"


// namespaces
USING_GRAPH_NS

System::String ^ NodeIDs::NodeRendering::get          () { return gcnew System::String( vxNodeIDs::NodeRendering          .c_str() ); }
System::String ^ NodeIDs::NodeDatasets::get           () { return gcnew System::String( vxNodeIDs::NodeDatasets           .c_str() ); }
System::String ^ NodeIDs::NodeGlobalController::get   () { return gcnew System::String( vxNodeIDs::NodeGlobalController   .c_str() ); }
System::String ^ NodeIDs::NodeLocalController::get    () { return gcnew System::String( vxNodeIDs::NodeLocalController    .c_str() ); }
System::String ^ NodeIDs::NodeDrawing::get            () { return gcnew System::String( vxNodeIDs::NodeDrawing            .c_str() ); }
System::String ^ NodeIDs::NodeDispatcher::get         () { return gcnew System::String( vxNodeIDs::NodeDispatcher         .c_str() ); }


// $Log: v3D_NodeIDs.cpp,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeIDs.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_NodeIDs.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
