// $Id: vxIComponentize.h,v 1.1 2004/07/12 18:04:25 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxIComponentize_h
#define vxIComponentize_h


/**
 * This interface should allow any component that contains vector shape information
 * to create a raster version of itself.
 *
 */
class VX_VIEWER_DLL vxIComponentize
{
// member functions
public:

  /// rasterize the existing vector-defined component into a volumetric format
  virtual bool CreateComponent( vxBlockVolume<bool> & outputComponent ) = 0;

}; // interface vxIComponentize


#endif // vxIComponentize_h


// $Log: vxIComponentize.h,v $
// Revision 1.1  2004/07/12 18:04:25  frank
// added component data
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIComponentize.h,v 1.1 2004/07/12 18:04:25 frank Exp $
// $Id: vxIComponentize.h,v 1.1 2004/07/12 18:04:25 frank Exp $
