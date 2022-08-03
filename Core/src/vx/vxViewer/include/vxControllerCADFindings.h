// $Id: vxControllerCADFindings.h,v 1.1 2007/04/11 20:49:39 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Romy Sreedharan (romy@viatronix.com)

#ifndef vxControllerCADFindings_h
#define vxControllerCADFindings_h


// includes
#include "vxController.h"


/**
 * Controller Class which processes the Selection of CAD Findings.
 */
class VX_VIEWER_DLL vxControllerCADFindings : public vxController
{
// functions
public:

  /// constructor
  vxControllerCADFindings( vxEnvironment & environment );

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

}; // class vxControllerCADFindings


#endif // vxControllerCADFindings_h


// Revision History:
// $Log: vxControllerCADFindings.h,v $
// Revision 1.1  2007/04/11 20:49:39  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerCADFindings.h,v 1.1 2007/04/11 20:49:39 romy Exp $
// $Id: vxControllerCADFindings.h,v 1.1 2007/04/11 20:49:39 romy Exp $

