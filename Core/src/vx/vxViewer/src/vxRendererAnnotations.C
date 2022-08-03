// $Id: vxRendererAnnotations.C,v 1.9 2007/10/12 19:00:25 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererAnnotations.h"
#include "vxEnvironment.h"
#include "vxViewerType3D.h"
#include "vxDataAnnotationCreate.h"


/**
 * Constructor
 * @param environment
 */ 
vxRendererAnnotations::vxRendererAnnotations(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
} // vxRendererAnnotations()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererAnnotations::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false);

  const bool bChangedAnnotation(GetEnvironment().IsModified(vxEnvironment::HANDLESET));
  const bool bChangedDataAnnotationCreate(GetEnvironment().IsModified(vxIDs::DataAnnotationCreate));
  const bool bShowAnnotations( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_ANNOTATIONS ));

  bRedrawNecessary |= bChangedAnnotation;
  bRedrawNecessary |= bChangedDataAnnotationCreate;
  bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);
  bRedrawNecessary |= bShowAnnotations;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Any preparation necessary?
 * @param void.
 * @return void.
 */
int4 vxRendererAnnotations::PreRender()
//*******************************************************************
{
  if (dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType()) == NULL)
  {
    // initialize the static members of the annotations/handlesObjects
    if (GetEnvironment().GetAnnotations() != NULL)
    {
      vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
      for (int i(0); i<pAnnotations->GetCount(); i++)
      {
        pAnnotations->Get(i)->PreRender(GetEnvironment());
      }
    }

    vxDataAnnotationCreate * pDataAC = static_cast<vxDataAnnotationCreate *>(dynamic_cast<vxShareableObject<vxDataAnnotationCreate> *>(GetEnvironment().GetElement(vxIDs::DataAnnotationCreate)));
    if (pDataAC != NULL)
    {
      if (pDataAC->m_pAnnotation != NULL)
      {
        pDataAC->m_pAnnotation->PreRender(GetEnvironment());
      }
    }
  }

  return true;
} // PreRender()


/**
 * Draw the annotations
 */
void vxRendererAnnotations::Render()
//*******************************************************************
{
  const bool bShowAnnotations( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_ANNOTATIONS ));
  if (dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType()) == NULL && bShowAnnotations)
  {
    // annotations belong to a slice and we do not show them in slabs
    if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::DEFAULT)
    {
      Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

      // save the OpenGL state
      glPushAttrib(GL_ALL_ATTRIB_BITS);

      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);

      if( GetEnvironment().GetViewerType()->GetRenderCoordinateSystem()  == vxViewerType::eScreenSpace )
      {
        // save the OpenGL state
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // new settings
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

      }

      // determines if the 
      const bool bIsActiveDataset = vxEnvironment::GetActiveEnvironment() != NULL &&
                                           vxEnvironment::GetActiveEnvironment()->GetDataset() == GetEnvironment().GetDataset();
      
      // draw all annotations in 3D
      if (GetEnvironment().GetAnnotations() != NULL)
      {
        vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
        for (int i(0); i<pAnnotations->GetCount(); i++)
        {
          pAnnotations->Get(i)->Render(GetEnvironment());
        }

        if( bIsActiveDataset )
        {
          vxDataAnnotationCreate * pDataAC = static_cast<vxDataAnnotationCreate *>(dynamic_cast<vxShareableObject<vxDataAnnotationCreate> *>(GetEnvironment().GetElement(vxIDs::DataAnnotationCreate)));
          if (pDataAC != NULL)
          {
            if (pDataAC->m_pAnnotation != NULL )
            {
              pDataAC->m_pAnnotation->Render(GetEnvironment());
            }
          }
        }
      }

      if( GetEnvironment().GetViewerType()->GetRenderCoordinateSystem()  == vxViewerType::eScreenSpace )
      {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();  
      }

      // save the OpenGL state
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // draw text of annotations in 2D
      ///////////////////////////////////
      GLint viewport[4];
      GLdouble mvmatrix[16], projmatrix[16];
      glGetIntegerv(GL_VIEWPORT, viewport);
      glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
      glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

      // new settings
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
      glDisable(GL_DEPTH_TEST);

      if (GetEnvironment().GetAnnotations() != NULL)
      {
        vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
        for (int i(0); i<pAnnotations->GetCount(); i++)
        {
          pAnnotations->Get(i)->RenderText(GetEnvironment(), viewport, mvmatrix, projmatrix);
        }

        if( bIsActiveDataset )
        {
          vxDataAnnotationCreate * pDataAC = static_cast<vxDataAnnotationCreate *>(dynamic_cast<vxShareableObject<vxDataAnnotationCreate> *>(GetEnvironment().GetElement(vxIDs::DataAnnotationCreate)));
          if (pDataAC != NULL)
          {
            if (pDataAC->m_pAnnotation != NULL )
            {
              pDataAC->m_pAnnotation->RenderText(GetEnvironment(), viewport, mvmatrix, projmatrix);
            }
          }
        }
      }

      // restore the OpenGL state
      glPopAttrib();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();  
    }
  }
} // Render()


// Revision History:
// $Log: vxRendererAnnotations.C,v $
// Revision 1.9  2007/10/12 19:00:25  gdavidson
// Issue #5837: Only show the annotation's creation in the currently active dataset
//
// Revision 1.8  2007/08/21 19:42:07  gdavidson
// Add check of the rendering mode before rendering annotations
//
// Revision 1.7  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.6.2.1  2007/04/04 16:28:18  geconomos
// Issue# 5552: Removed problematic glPopAttrib()
//
// Revision 1.6  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2004/10/05 14:31:07  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/04/06 16:26:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.92  2004/02/03 13:45:51  michael
// fixed issue #3714
//
// Revision 1.91  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.90  2004/01/23 04:00:28  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.89  2004/01/16 22:42:11  michael
// moved OpenGL state push AFTER the rendering of 3D so that it is more logical
//
// Revision 1.88  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.87  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.86  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.85  2003/11/05 01:18:46  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.84  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.82.2.1  2003/08/12 19:59:43  michael
// fixed issue #3450
//
// Revision 1.82  2003/06/18 15:19:57  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.81  2003/05/19 14:37:44  frank
// Removed unused return parameter
//
// Revision 1.80  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.79  2003/05/17 15:06:27  michael
// fixed issue #3192 and #3194
//
// Revision 1.78  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.77  2003/05/14 18:30:03  dongqing
// code review
//
// Revision 1.76  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.75  2003/04/18 15:15:55  frank
// Cleaned up includes
//
// Revision 1.74  2003/04/16 14:41:21  michael
// removed redundant include
//
// Revision 1.73  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.72  2003/03/04 17:58:12  frank
// Cleaned out unused functions
//
// Revision 1.71  2003/03/04 01:43:06  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.70  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.69  2003/02/27 23:12:28  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.68  2003/01/22 22:00:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.67  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.66  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.65  2003/01/15 16:30:13  michael
// calling PreRender() to determine the handle size in world space
//
// Revision 1.64  2003/01/13 23:20:33  michael
// made first annotation to work in new structure (vxAnnotations)
//
// Revision 1.63  2003/01/09 14:55:19  michael
// removed redundant code
//
// Revision 1.62  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.61  2002/11/22 17:24:47  michael
// fixed drawing of arrows and points in oblique
//
// Revision 1.60  2002/11/22 13:15:06  michael
// fixed distance tolerance from index to world coordinates (0.5*units[uIndex])
//
// Revision 1.59  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.58  2002/11/12 00:10:30  michael
// cosmetics
//
// Revision 1.57  2002/11/08 23:23:57  michael
// fixed world/index coordinates
//
// Revision 1.56  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.55  2002/09/25 20:36:42  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.54  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.53  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.52  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.51  2002/06/28 21:36:43  jenny
// added annotation splitter for rendering
//
// Revision 1.50  2002/06/12 13:30:33  manju
// Moved GetScreenCoordinates method completely to vxUtils.
//
// Revision 1.49  2002/06/07 19:37:16  manju
// Modified rendering of ellipse and rectangle annotation.
//
// Revision 1.48  2002/06/04 13:10:05  manju
// Issue 2359 : The text now appears at the end of the arrow rather than above
// the tip.
//
// Revision 1.47  2002/06/03 20:44:45  manju
// Made the render functions virtual, to be derived by
// vxRendererAnnotations3D.
//
// Revision 1.46  2002/05/29 19:28:46  manju
// Coding standard.
//
// Revision 1.45  2002/05/22 18:21:27  manju
// Point annotation can also now be seen properly in the 3x3 and 4x4 views.
//
// Revision 1.44  2002/05/21 15:28:06  manju
// Using screen coordinates to position labels and statistics for annotations.
//
// Revision 1.43  2002/05/20 15:49:04  manju
// Using the world coordinates of the seed point for Volume Annotation.
//
// Revision 1.42  2002/05/17 00:48:03  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.41  2002/05/13 18:21:02  manju
// Changed type of handle position form uint4 to int2 in DrawHandlePoints
// method.
//
// Revision 1.40  2002/05/09 20:46:39  manju
// Not displaying the statistics in the image for ellipse, lasso, rectangle
// and volume.
//
// Revision 1.39  2002/05/09 19:41:38  manju
// Modification in the position of label for volume annotation.
//
// Revision 1.38  2002/05/08 21:21:59  manju
// point and arrow annotation can now be seen in all orientations.
//
// Revision 1.37  2002/05/06 19:37:57  manju
// Modified length of arc of angle annotation.
//
// Revision 1.36  2002/05/03 18:28:43  manju
// Modified RenderLassoStatistcis and RenderEllipseAnnotation methods.
//
// Revision 1.35  2002/05/01 21:38:55  michael
// Adjusted GetDataModality() to GetDataModalityEnum()
//
// Revision 1.34  2002/05/01 20:23:08  manju
// Minor changes.
//
// Revision 1.33  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.32  2002/04/29 18:32:11  manju
// Added rectangle annotation.
//
// Revision 1.31  2002/04/26 13:39:56  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.30  2002/04/25 21:28:46  manju
// Modified positioning of statistics.
//
// Revision 1.29  2002/04/25 19:50:39  manju
// Modified drawing of cubes for handle points
// Modified positions of statistics for ellipse and lasso annotation, to look better
// for large datasets and also when zoomed.
//
// Revision 1.28  2002/04/25 19:19:43  manju
// Modified drawing of arc of angle annotation and labels and text for linear
// and angle annotation, to look better for large datasets and also when
// zoomed.
//
// Revision 1.27  2002/04/24 20:16:21  manju
// Using PixelRGBA to represent color, in RenderArrow method.
//
// Revision 1.26  2002/04/24 15:00:15  jenny
// vxBlockVolume stuff
//
// Revision 1.25  2002/04/24 03:03:02  michael
// cosmetics
//
// Revision 1.24  2002/04/23 20:12:22  michael
// replaced Density by Intensity
//
// Revision 1.23  2002/04/23 15:39:57  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.22  2002/04/19 14:40:03  manju
// Modified IsInSlice method to account for a slight slice width.
//
// Revision 1.21  2002/04/17 18:18:22  frank
// Added implementation note.
//
// Revision 1.20  2002/04/15 15:05:49  manju
// Changed units from uint2 to float4.
//
// Revision 1.19  2002/04/14 00:18:00  michael
// adjusted method name
//
// Revision 1.18  2002/04/12 19:15:44  manju
// Displaying seedpoint as a handle point for volume annotation.
//
// Revision 1.17  2002/04/10 20:35:04  manju
// Using CalcOffsetDistance to get the current slice number.
//
// Revision 1.16  2002/04/10 15:03:09  manju
// Modified positions of labels and text depending on slice orientation.
//
// Revision 1.15  2002/04/08 21:20:41  manju
// Modified rendering of annotations to take into account all three orientations,
// Axial, Sagittal and Coronal.
//
// Revision 1.14  2002/04/08 13:58:37  manju
// Added rendering of volume annotation and of histogrmas for volume, ellipse,
// lasso and linear annotations.
//
// Revision 1.13  2002/04/02 22:17:21  manju
// Displaying statistics of annotations.
//
// Revision 1.12  2002/04/01 20:36:17  manju
// Restricting the rendering of an annotation to the slice on which it was created
// in. Modified positions of labels and statsitics.
//
// Revision 1.11  2002/04/01 19:04:04  manju
// Removed passing of font as argument. Also changed GetAnnotationLabel to
// GetAnnotationLabelText.
//
// Revision 1.10  2002/04/01 16:33:17  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.9  2002/03/28 21:44:14  geconomos
// Disabled the creation of fonts.
//
// Revision 1.8  2002/03/27 18:15:51  manju
// The annotations list is now made of HandlesObj and not Annotation objects.
//
// Revision 1.7  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.6  2002/03/26 18:43:00  dmitry
// Renamed GetContext to GetContextPtr.
//
// Revision 1.5  2002/03/25 21:29:35  michael
// fixed one remaining dataModality error
//
// Revision 1.4  2002/03/25 16:29:12  manju
// Added creation of font for the annotation text to appear. This will be
// removed later.
//
// Revision 1.3  2002/03/21 18:34:28  manju
// cleaned the includes.
//
// Revision 1.2  2002/03/21 16:16:18  manju
// Modified rendering of annotations.
//
// Revision 1.1  2002/03/15 18:52:56  manju
// Renderer for Annotations.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererAnnotations.C,v 1.9 2007/10/12 19:00:25 gdavidson Exp $
// $Id: vxRendererAnnotations.C,v 1.9 2007/10/12 19:00:25 gdavidson Exp $
