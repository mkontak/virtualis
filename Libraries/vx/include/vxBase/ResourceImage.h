// $Id: ResourceImage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

#ifndef ResourceImage_h
#define ResourceImage_h


// includes
#include "Image.h"
#include "Typedef.h"


/**
 * Loads an image from the resource file.
 */
class VX_BASE_DLL ResourceImage
{
// member functions
public:

  /// loads an image from the resource for the specified string
  static Image< PixelRGBA< uint1 > > * FromResource( const std::string & sResource, HMODULE hModule = NULL );

  /// sets the module that contains the resources
  static void SetModuleHandle( HMODULE hModuleHandle ) { m_hModuleHandle = hModuleHandle; }

// data members
private:

  // module that contains the resources
  static HMODULE  m_hModuleHandle;
  
}; // class ResourceImage


#endif 


// $Log: ResourceImage.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/16 16:47:54  frank
// code review
//
// Revision 1.8  2003/05/16 11:44:23  michael
// code review
//
// Revision 1.7  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 1.6  2003/02/26 19:26:40  geconomos
// Added HMODULE parameter to FromResource
//
// Revision 1.5  2002/09/19 18:21:24  geconomos
// Changed module handle type from uint4 to HMODULE.
//
// Revision 1.4  2002/03/21 16:00:58  geconomos
// Added Gdiplus initialization.
//
// Revision 1.3  2002/03/20 14:49:22  geconomos
// Changed class to contain single static method for loading images from the resource.
//
// Revision 1.2  2002/01/22 05:24:40  michael
// added orientation cube images (png)
//
// Revision 1.1  2002/01/21 22:01:25  dmitry
// Initial Version
//
// $Id: ResourceImage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ResourceImage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $