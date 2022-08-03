// $Id: vxNodeDispatcher.C,v 1.2 2007/10/24 20:13:10 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxNodeDispatcher.h"
#include "vxVisualizationGraphNode.h"
#include "vxVisualizationGraph.h"
#include "vxEnvironment.h"
#include "vxViewer.h"
#include "vxNodeRendering.h"
#include "vxWindowHandle.h"
#include "vxRenderingContextDIB.h"


/**
 * Dispatch redraw to all viewers
 */
void vxNodeDispatcher::Run()
{
  //// drawing
  //std::vector< vxViewer * > & viewers = GetVisualizationGraph()->GetViewers();
  //for( std::vector< vxViewer * >::iterator iter = viewers.begin(); iter != viewers.end(); ++iter )
  //{
  //  if( (*iter)->GetEnvironment().IsModified( vxEnvironment::ALL ) || (*iter)->GetEnvironment().GetRenderBuffer().IsModified() )
  //  {
  //    // get associated HWND handle for viewer
  //    HWND hWnd = dynamic_cast< vxWindowHandle * >( (*iter)->GetEnvironment().GetElement( vxIDs::WindowHandle ) )->GetHandle();
  //    
  //    //PostMessage( hWnd, vxNodeRendering::VX_REDRAW, 0, 0 );


  //    PAINTSTRUCT ps;
  //    BeginPaint( hWnd, &ps );

  //    HDC hdc = GetDC( hWnd );

  //    vxRenderingContextDIB & context = * dynamic_cast< vxRenderingContextDIB * >( (*iter)->GetEnvironment().GetContext() );

  //    const std::pair< uint2, uint2 > & size = (*iter)->GetEnvironment().GetWinRect().GetWindowSize();
  //    BitBlt( hdc, 0, 0, size.first, size.second, context.GetHDC(), 0, context.GetDib().GetHeight() - size.second, SRCCOPY );


  //    (*iter)->GetEnvironment().SetModified( vxEnvironment::ALL, false );

  //    ReleaseDC( hWnd, hdc );

  //    EndPaint( hWnd, &ps );
  //  }
  //} 
} // Run()


// $Log: vxNodeDispatcher.C,v $
// Revision 1.2  2007/10/24 20:13:10  gdavidson
// Moved dispatch code into vxNodeDrawing
//
// Revision 1.1  2007/05/03 21:42:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeDispatcher.C,v 1.2 2007/10/24 20:13:10 gdavidson Exp $
// $Id: vxNodeDispatcher.C,v 1.2 2007/10/24 20:13:10 gdavidson Exp $
