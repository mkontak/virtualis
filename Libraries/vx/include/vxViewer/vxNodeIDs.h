// $Id: vxNodeIDs.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

#pragma once


// type definitions
typedef std::string vxNodeID;


struct VX_VIEWER_DLL vxNodeIDs
{
 
  static const vxNodeID NodeRendering;
  static const vxNodeID NodeGlobalController;
  static const vxNodeID NodeLocalController;
  static const vxNodeID NodeDrawing;
  static const vxNodeID NodeDatasets;
  static const vxNodeID NodeDispatcher;

}; // class vxNodeIDs


// $Log: vxNodeIDs.h,v $
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeIDs.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
// $Id: vxNodeIDs.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
