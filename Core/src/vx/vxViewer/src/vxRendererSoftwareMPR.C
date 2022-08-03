// $Id: vxRendererSoftwareMPR.C,v 1.12 2006/10/03 20:08:13 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSoftwareMPR.h"
#include "vxEnvironment.h"
#include "vxIntensityConverter.h"
#include "vxMathUtils.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.12 $"


/**
 * Default constructor
 * @param environment
 */
vxRendererSoftwareMPR::vxRendererSoftwareMPR(vxEnvironment & environment)
:vxRendererSoftware(environment)
//*******************************************************************
{  
  SetRenderingNecessary(false);
}


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererSoftwareMPR::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false); //vxRendererSoftware::IsRedrawNecessary()); ???

  bool bChangedVolumeIntensity(GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY));
  bool bChangedVolumeLabel(GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL));
  bool bChangedColormap(GetEnvironment().IsModified(vxEnvironment::COLORMAP));
  bool bChangedComponents(GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR));
  bool bChangedCropbox(GetEnvironment().IsModified(vxEnvironment::CROP_BOX) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX));
  bool bChangedPlaneMPR(GetEnvironment().IsModified(vxEnvironment::PLANE_MPR));
  bool bChangedSegmentation(GetEnvironment().IsModified(vxEnvironment::INTERACTIVE_SEGMENTATION) == true && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE));
  bool bChangedSliceShadow(GetEnvironment().IsModified(vxEnvironment::SLICE_SHADOWS) == true && GetEnvironment().GetRenderingMode()->GetSliceShadowMode() != vxRenderingMode::SHADOWS_NONE );
  bool bChangedRenderingMode(GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE) == true);
  bool bChangedCamera(GetEnvironment().IsModified(vxEnvironment::CAMERA) == true);
  bool bChangedWinRect(GetEnvironment().IsModified(vxEnvironment::WIN_RECT) == true);
  bool bChangedBackgroundColor(GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR) == true);
  bool bChangedDataRenderer(GetEnvironment().IsModified(vxEnvironment::DATA_RENDERER_SOFTWARE));
  bool bChangedZoomFactor(GetEnvironment().IsModified(vxEnvironment::ZOOM_FACTOR));

  bRedrawNecessary |= bChangedVolumeIntensity;
  bRedrawNecessary |= bChangedVolumeLabel;
  bRedrawNecessary |= bChangedColormap;
  bRedrawNecessary |= bChangedComponents;
  bRedrawNecessary |= bChangedCropbox;
  bRedrawNecessary |= bChangedPlaneMPR;
  bRedrawNecessary |= bChangedSegmentation;
  bRedrawNecessary |= bChangedSliceShadow;
  bRedrawNecessary |= bChangedRenderingMode;
  bRedrawNecessary |= bChangedCamera;
  bRedrawNecessary |= bChangedWinRect;
  bRedrawNecessary |= bChangedBackgroundColor;
  bRedrawNecessary |= bChangedDataRenderer;
  bRedrawNecessary |= bChangedZoomFactor;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * PreRender routine
 */
int4 vxRendererSoftwareMPR::PreRender()
//*******************************************************************
{
  // call based class
  vxRendererSoftware::PreRender();

  // test if all data is available that is 
  // necessary and if not, throw an exception
  /////////////////////////////////////////////
  if (GetEnvironment().GetColormap() == NULL)
  {
    throw ex::NullPointerException(LogRec("GetEnvironment().GetColormap() is NULL", 
      "vxRendererSoftwareMPR",
      "PreRender"));
  }

  if (GetEnvironment().GetIntensityVolume() == NULL)
  {
    throw ex::NullPointerException(LogRec("GetEnvironment().GetIntensityVolume() is NULL", 
      "vxRendererSoftwareMPR",
      "PreRender"));
  }


  // has the volume changed?
  if (GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY))
  {
    SetRenderingNecessary(true);
  }

  // has the transfer function changed?
  if (GetEnvironment().IsModified(vxEnvironment::COLORMAP))
  {
    SetRenderingNecessary(true);
  }

  // has any of the labels changed?
  if (GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL))
  {
    SetRenderingNecessary(true);
  }

  // has any of the labels changed?
  if (GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL))
  {
    SetRenderingNecessary(true);
  }

  // has the camera changed?
  if (GetEnvironment().IsModified(vxEnvironment::PLANE_MPR))
  {
    SetRenderingNecessary(true);
  }

  return 1;
} // PreRender()


/**
 * Render routine
 */
void vxRendererSoftwareMPR::Render()
//*******************************************************************
{
  // save the OpenGL state
#ifndef VX_VC_DLL
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
#endif

  if (GetRenderingNecessary() == true) // need to re-generate the image?
  {
    Triple<uint4> imageSize(GetImage().GetDim());

    // need to clear because some renderers don't clean up their borders
    GetImage().Clear();

    // subdivide window into tiles
    ////////////////////////////////
    uint2 iIncrement;
    uint2 iBaseX(0), iBaseY(0);
    uint2 iSizeX(imageSize[0]), iSizeY(imageSize[1]);
    const int4 iVolumeBlockSize(16);
    BoundingRect<uint2> window;

    if (GetMultiThreadRendering() == true)
    {
      // subdivide the screen into disjoint windows
      for (int i(0); i<GetNumberOfThreads(); i++)
      { 
        int4 iBaseY0(0), iBaseY1(0);

        // are there more threads than scanlines? --> lets have only one thread!!!!
        if (GetNumberOfThreads() > iSizeY)
        {
          if (i == 0)
            window = BoundingRect<uint2>(iBaseX, iBaseY,iSizeX-1, iSizeY);
          else
            window = BoundingRect<uint2>(iBaseX, iBaseY, iBaseX, iBaseY);
        }
        else
        {  
          iIncrement = iSizeY/(float)GetNumberOfThreads();
          iBaseY = i*iIncrement;

          iBaseY0 = i * iIncrement / iVolumeBlockSize * iVolumeBlockSize;
          iBaseY1 = (i+1) * iIncrement / iVolumeBlockSize * iVolumeBlockSize;
        }
     
        if (i != GetNumberOfThreads()-1)
          window.Set(iBaseX, iBaseY0, iSizeX, iBaseY1);
        else
          window.Set(iBaseX, iBaseY0, iSizeX, iSizeY);

        GetRenderThreadsPtr()[i].SetRenderer(this);
        GetRenderThreadsPtr()[i].SetWindow(window);
      } // for all tiles

      // start all threads
      for (int i(0); i<GetNumberOfThreads(); i++)
      {
        GetRenderThreadsPtr()[i].StartRendering();
      }

      // wait until all threads are done
      for (int i(0); i<GetNumberOfThreads(); i++)
      {
        GetRenderThreadsPtr()[i].WaitForRenderingToComplete();
      }
    } // is multi-threadin on?
    else
    {
      // render in main process
      window = BoundingRect<uint2>(0,  0, iSizeX, iSizeY);
      RenderWindow(window, 0);
    }
    window = BoundingRect<uint2>(0,  0, iSizeX, iSizeY);

#ifndef VX_VC_DLL

    GetEnvironment().SetModified( vxEnvironment::DATA_RENDERER_SOFTWARE, true );

#endif
  } // need to render?
  
#ifndef VX_VC_DLL
  
  if ( GetEnvironment().IsModified( vxEnvironment::DATA_RENDERER_SOFTWARE ) )
  {
    PreCreateTexture( BoundingRect<uint2>( 0, 0, GetImage().GetDim().m_x, GetImage().GetDim().m_y ) );
    CreateTexture();
  }

  // whether rendering was necessary or not, now draw the image
  DrawTexture();
  
  // restore the OpenGL state
  glPopMatrix();
  glPopAttrib();
  
#endif

} // Render()


/**
 * Draw the image stored in the texture tile
 */
void vxRendererSoftwareMPR::DrawTexture()
//*******************************************************************
{
  uint1 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
  GLuint id(GetTextureTile().GetDensityTextureId());

  glBindTexture(GL_TEXTURE_2D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // GL draw textured tile:
  glColor3f(1, 1, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  glBegin(GL_TRIANGLE_FAN);

  for (int i(0); i<4; i++)
  {
    glTexCoord2f(GetTextureTile().GetVertexTexCoord(i)[(uIndex+1)%3], 
                 GetTextureTile().GetVertexTexCoord(i)[(uIndex+2)%3]);
    glVertex3f(GetTextureTile().GetVertex(i)[0], 
               GetTextureTile().GetVertex(i)[1],
               GetTextureTile().GetVertex(i)[2]);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  return;
} // DrawTexture()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererSoftwareMPR.C,v $
// Revision 1.12  2006/10/03 20:08:13  geconomos
// modified to use new SliceShadowMode enumeration
//
// Revision 1.11  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.10  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.6.1  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.8  2004/10/12 12:57:05  frank
// made rendering sensitive to the zoom factor
//
// Revision 1.7  2004/10/05 14:33:40  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.6  2004/10/04 15:02:17  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.5  2004/09/01 12:59:50  frank
// decreased the granularity of component array modified bits
//
// Revision 1.4  2004/04/22 13:39:40  geconomos
// Added temporary ( I hope  ) vc dll preprocessor conditions
//
// Revision 1.3  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/30 18:20:58  frank
// added blending for fusion rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.42  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.41.2.1  2004/02/20 17:45:44  michael
// made renderers sensitive to a change of the background color
//
// Revision 1.41  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.40  2003/12/23 21:35:47  michael
// need to be sensitive to camera change, too.
//
// Revision 1.39  2003/12/18 23:52:46  michael
// added being sensitive to change of rendering mode
//
// Revision 1.38  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.37  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.36  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.35  2003/05/19 14:42:20  frank
// Removed unused return parameter
//
// Revision 1.34  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.33  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.32  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.31  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.30  2003/03/26 19:28:29  michael
// formatting
//
// Revision 1.29  2003/01/22 22:03:13  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.28  2002/11/20 21:38:20  geconomos
// Made rendering threads persitent.
//
// Revision 1.27  2002/11/15 18:12:28  frank
// Fixed border clearing fiasco, re-implemented PreCreateTexture()
//
// Revision 1.26  2002/11/12 21:49:44  ingmar
// added comment
//
// Revision 1.25  2002/11/12 21:26:35  ingmar
// moved default CreateTexture() implementation from vxRendererSoftwareMPR() to vxRendererSoftware()
//
// Revision 1.24  2002/11/08 13:53:18  michael
// removed redundant method
//
// Revision 1.23  2002/11/06 19:35:22  ingmar
// added glPush/pop matrix
//
// Revision 1.22  2002/09/28 02:20:33  frank
// Working on PreCreateTexture - a better way to fix image borders.
//
// Revision 1.21  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.20  2002/09/18 22:06:53  frank
// Fixed flickering.
//
// Revision 1.19  2002/08/21 18:32:57  michael
// formatting
//
// Revision 1.18  2002/08/09 15:38:07  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.17  2002/07/26 23:16:47  michael
// took out redundant rerendering sensitivity for "camera"
//
// Revision 1.16  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.15  2002/07/08 22:03:33  frank
// Removed unnecesary function.
//
// Revision 1.14  2002/07/02 13:43:00  michael
// enable depth test for software renderers
//
// Revision 1.13.4.2  2002/07/09 21:20:44  binli
// multi-thread: subdivide volume along block edge.
//
// Revision 1.13.4.1  2002/07/01 23:16:19  uday
// Enabled depth test.
//
// Revision 1.13  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.12  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.11  2002/04/14 00:51:41  michael
// made GetVolumeType pure virtual. Compile timer error is preferable to run time error
//
// Revision 1.10  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.9  2002/03/29 16:06:54  michael
// added further condition to re-render
//
// Revision 1.8  2002/03/28 17:57:51  geconomos
// removed index lookup since it already is in Tile class
//
// Revision 1.7  2002/03/27 20:19:46  michael
// planeMPR is now included in the modifed/update mechanism
//
// Revision 1.6  2002/03/26 17:05:01  michael
// removed unused code
//
// Revision 1.5  2002/03/22 18:42:00  jenny
// worked on Draw function
//
// Revision 1.4  2002/03/19 17:27:13  dmitry
// fixed 2dMPROrtho rendering
//
// Revision 1.3  2002/03/18 15:46:38  michael
// adjusted 2D renderers to show reg, green, blue pixels ... (debugging)
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:27:55  michael
// added initial version of software renderers (still buggy)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPR.C,v 1.12 2006/10/03 20:08:13 geconomos Exp $
// $Id: vxRendererSoftwareMPR.C,v 1.12 2006/10/03 20:08:13 geconomos Exp $
