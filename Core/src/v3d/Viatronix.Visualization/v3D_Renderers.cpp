// $Id: v3D_Renderers.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "stdafx.h"
#include "v3D_renderers.h"

// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"

// namespaces
USING_VISUALIZATION_NS

/**
 * Factory for vxRendererTexMapMPR
 * @param environment environment pointer
 * @return vxRendererTexMap *
 */
System::IntPtr RendererTexMapMPR::CreateRenderer( System::IntPtr environment ) 
{ 
  // detect whether we got a GeForce3/4 so that 
  // dependent textures are available for the lookup
  ////////////////////////////////////////////////////
  GLuint error;

  while ((error = glGetError()) != GL_NO_ERROR)
  {
    ; // clear out error queue
  }
  glEnable(GL_TEXTURE_SHADER_NV);

  if (glGetError() == GL_NO_ERROR)
  {
    LogDbg("GeForce3/4: Using texture mapping", "RendererTexMapMPR", "CreateRenderer")
    glDisable(GL_TEXTURE_SHADER_NV);
    return System::IntPtr( new vxRendererTexMap2DMPRNew( *reinterpret_cast< vxEnvironment * >( environment.ToPointer() ) ) ); 
  }
  else
  {
    LogDbg("GeForce2: Using software rendering", "RendererTexMapMPR", "CreateRenderer")
    return System::IntPtr( new vxRendererSoftwareMPROrtho( *reinterpret_cast< vxEnvironment * >( environment.ToPointer() ) ) );
  }
} // CreateRenderer()


// Revision History:
// $Log: v3D_Renderers.cpp,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.28  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.27  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.26  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Renderers.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id: v3D_Renderers.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
