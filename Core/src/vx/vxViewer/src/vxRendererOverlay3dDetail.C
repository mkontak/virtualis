// $Id: vxRendererOverlay3dDetail.C,v 1.4 2006/01/27 15:47:45 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererOverlay3dDetail.h"
#include "vxEnvironment.h"
#include "GLEnv.h"
#include "GLFontGeorge.h"
#include "vxOverlayStrings.h"
#include "Vector3D.h"
#include "vxDataRendererSoftware.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 *
 * @param environment the environemnt
 */
vxRendererOverlay3dDetail::vxRendererOverlay3dDetail(vxEnvironment & environment)
: vxRendererOverlay3D(environment)
//******************************************************************* 
{
} // vxRendererOverlay3dDetail()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererOverlay3dDetail::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(vxRendererOverlay3D::IsRedrawNecessary());

  bool bChangedDataRenderer(GetEnvironment().IsModified(vxEnvironment::DATA_RENDERER_SOFTWARE));

  bRedrawNecessary |= bChangedDataRenderer;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Render 3D Overlay
 */  
void vxRendererOverlay3dDetail::Render()
//******************************************************************* 
{
  // call base class
  vxRendererOverlay3D::Render();

  vxOverlayStrings * pOverlayStrings = dynamic_cast<vxOverlayStrings *>(GetEnvironment().GetElement(vxIDs::OverlayStrings));

  if(GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_IMAGE_INFORMATION))
  {
    vxShareableObject<vxDataRendererSoftware> * pData = dynamic_cast<vxShareableObject<vxDataRendererSoftware> *>(GetEnvironment().GetDataRendererSoftware());

    float4 fFrameTime(pData->GetFrameTime());
    std::string sFps;
    sFps = "Fps: " + ToAscii(1.0/fFrameTime);

    int4 iSizeX(pData->GetImage().GetDim()[0]);
    int4 iSizeY(pData->GetImage().GetDim()[1]);
    std::string sImageSize;
    sImageSize = "Size: " + ToAscii(iSizeX) + "x" + ToAscii(iSizeY);
    pOverlayStrings->AddElement(vxOverlayStrings::vxOverlayElement(sFps, vxOverlayStrings::vxOverlayElement::UpperRight));
    pOverlayStrings->AddElement(vxOverlayStrings::vxOverlayElement(sImageSize, vxOverlayStrings::vxOverlayElement::UpperRight));
  }
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererOverlay3dDetail.C,v
// $Id: vxRendererOverlay3dDetail.C,v 1.4 2006/01/27 15:47:45 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlay3dDetail.C,v 1.4 2006/01/27 15:47:45 geconomos Exp $
