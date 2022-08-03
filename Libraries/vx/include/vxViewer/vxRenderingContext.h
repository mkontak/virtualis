// $Id: vxRenderingContext.h,v 1.1 2004/03/30 17:23:03 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

#ifndef vxRenderingContext_h
#define vxRenderingContext_h

// includes
#include "vxDib.h"

// class declaration
class VX_VIEWER_DLL vxRenderingContext
{
// member functions
public:
  
  // default consturctor
  vxRenderingContext();
  
  // virtual desturctor
  virtual ~vxRenderingContext();
  
  // creates a rendering context with the spcified width and height 
 	void Create( uint4 uWidth, uint4 uHeight );
 	
 	// destroys the context rendering object
 	void Destroy();
  
  // sets the context as active
  void Activate();
  
  // gets the associated dib
  const vxDib & GetDib() const { return m_dib; }

// member variables
private:

  // opengl rendering context
  HGLRC	m_hrc;
  
  // dib rendered into
  vxDib m_dib;
  
}; // class vxRenderingContext

#endif // #ifndef vxRenderingContext_h

// $Log: vxRenderingContext.h,v $
// Revision 1.1  2004/03/30 17:23:03  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingContext.h,v 1.1 2004/03/30 17:23:03 geconomos Exp $
// $Id: vxRenderingContext.h,v 1.1 2004/03/30 17:23:03 geconomos Exp $
