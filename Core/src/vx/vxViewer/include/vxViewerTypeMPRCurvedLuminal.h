// $Id: vxViewerTypeMPRCurvedLuminal.h,v 1.2 2007/03/02 21:40:54 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxViewerTypeMPRCurved.h"


/**
 * a viewer type for the curved MPR luminal views
 */
class VX_VIEWER_DLL vxViewerTypeMPRCurvedLuminal : public vxViewerTypeMPRCurved
{
// functions
public:

  /// constructor
  vxViewerTypeMPRCurvedLuminal( vxEnvironment & environment );

  /// copy constructor
  virtual ~vxViewerTypeMPRCurvedLuminal();

 
  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeMPRCurvedLuminal"; }

}; // class vxViewerTypeMPRCurved


// $Log: vxViewerTypeMPRCurvedLuminal.h,v $
// Revision 1.2  2007/03/02 21:40:54  geconomos
// cleanup
//
// Revision 1.1  2006/09/14 20:11:01  geconomos
// initial revision
//
// $Id: vxViewerTypeMPRCurvedLuminal.h,v 1.2 2007/03/02 21:40:54 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMPRCurvedLuminal.h,v 1.2 2007/03/02 21:40:54 geconomos Exp $
