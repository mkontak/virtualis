// $Id: SliceViewerDBG.C,v 1.4 2005/09/29 15:01:35 vxconfig Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)


/**
 * A class to support viewing of volume slices
 */


// includes
#include "stdafx.h"
#include <GL/gl.h>
#include "SliceViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "vxBlockVolumeIterator.h"


// namespaces
using namespace ReaderLib;
using namespace SliceOrientations;


// globals
const uint4 MAXVALUE = 9999;


/**
 * render .ins
 *
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderIns(const float4 fLeft, const float4 fBottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();
  
  uint4 uFullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    return;
  }

  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + fLeft, fBottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(fLeft, (m_uDisplayHeight-viewportHeight)/2 + fBottom, viewportWidth, viewportHeight);
  } // end-if (displayAspectRatio < sliceAspectRatio).

  // Make the image
  vxBlockVolumeIterator<uint1> insIter( myDataset.m_fields.m_ins );
  for (int4 y=0,i=0; y<512; y++)
  {
    for (int4 x=0; x<512; x++,i++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      int4 value = insIter.Peek( pos.m_x, pos.m_y, pos.m_z );
      if( value == 0 ) 
      {
        m_uRGBSegImage[i*3  ] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+1] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+2] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      } 
      else 
      {
        m_uRGBSegImage[i*3  ] = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_x;
        m_uRGBSegImage[i*3+1] = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_y;
        m_uRGBSegImage[i*3+2] = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_z;
      } // end-if.
    } // end-for(x)
  } // end-for(y)..

  // draw the image
  glPushMatrix(); {
    // gluOrtho2D(0, m_volumeWidth-1, m_volumeHeight-1, 0);
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);

    if ( m_bBlockedPixel ) 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } 
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } // end-if (m_bBlockedPixel)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);

    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }
    
    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();

    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {      
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE ) 
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).

  } glPopMatrix();
  
  glFlush();

  m_iSliceNumber = uFullVolumeSlice;
} // RenderIns().


/**
 * render .paint
 *
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderPaint(const float4 left, const float4 bottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();
  unsigned char * pPaintVol = (unsigned char *) myDataset.m_fields.m_paintVol.GetDataPtr();
  
  uint4 fullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    return;
  }

  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + left, bottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(left, (m_uDisplayHeight-viewportHeight)/2 + bottom, viewportWidth, viewportHeight);
  } // endif (displayAspectRatio < sliceAspectRatio).

  // Make the image
  int4 i = 0;
  for(int4 y=0;y<512;y++) 
  {
    for(int4 x=0;x<512;x++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      const uint4 xDim = myDataset.m_vDim.m_x;
      const uint4 yDim = myDataset.m_vDim.m_y;
      const uint4 zDim = myDataset.m_vDim.m_y;
      const uint4 yStep = xDim;
      const uint4 zStep = xDim*yDim;
      uint1 value = 0;
      if (x<xDim && y<yDim) 
      {
        value = pPaintVol[int4(myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z)*zStep+y*yStep+x];
      }
      if( value == 0 ) 
      {
        m_uRGBSegImage[i*3  ] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+1] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+2] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      } 
      else if( value == 128 ) 
      {
        m_uRGBSegImage[i*3  ] = 0;
        m_uRGBSegImage[i*3+1] = 255;
        m_uRGBSegImage[i*3+2] = 0;
      } 
      else 
      {
        m_uRGBSegImage[i*3  ] = value;
        m_uRGBSegImage[i*3+1] = 0;
        m_uRGBSegImage[i*3+2] = 0;
      } // endif (x<xDim && y<yDim)
      i++;
    } // endfor (x).
  } // endfor (y).

  // draw the image
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);

    if ( m_bBlockedPixel ) 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } 
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);

    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }

    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();

    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE ) 
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).

  } glPopMatrix();
  
  glFlush();

  m_iSliceNumber = fullVolumeSlice;
} // RenderPaint().


/**
 * render .DDB
 *
 * @param scale   scalor.
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderDdb(const float4 scale, const float4 left, const float4 bottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();

  uint4 fullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
    return;

  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + left, bottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(left, (m_uDisplayHeight-viewportHeight)/2 + bottom, viewportWidth, viewportHeight);
  } // endif (displayAspectRatio < sliceAspectRatio).


  // Make the image
  int4 i = 0;
  vxBlockVolumeIterator<uint1> insIter( myDataset.m_fields.m_ins );
  for(int4 y=0;y<512;y++) 
  {
    for(int4 x=0;x<512;x++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      int4 value = insIter.Peek( pos.m_x, pos.m_y, pos.m_z );
      if(value == 0) 
      {
        m_uRGBSegImage[i*3  ] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+1] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
        m_uRGBSegImage[i*3+2] = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      } 
      else 
      {
        int4 color = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_x;
        color *= *(myDataset.m_fields.m_sldInterior.GetDataPtr()+fullVolumeSlice*512*512+i);
        color /= 255;
        color *= scale;
        if(color>255) color = 255;
        m_uRGBSegImage[i*3  ] = color;
        color = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_y;
        color *= *(myDataset.m_fields.m_sldInterior.GetDataPtr()+fullVolumeSlice*512*512+i);
        color /= 255;
        color *= scale;
        if(color>255) color = 255;
        m_uRGBSegImage[i*3+1] = color;
        color = RenderUtil::m_vTranslucentSliceViewColors[value-1].m_z;
        color *= *(myDataset.m_fields.m_sldInterior.GetDataPtr()+fullVolumeSlice*512*512+i);
        color /= 255;
        color *= scale;
        if(color>255) color = 255;
        m_uRGBSegImage[i*3+2] = color;
      } // endif (value).
      i++;
    } // endfor (x).
  } // endfor (y).

  // draw the image
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);

    if ( m_bBlockedPixel ) 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } 
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }
    
    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();
    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE ) 
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).

  } glPopMatrix();
  
  glFlush();
  m_iSliceNumber = fullVolumeSlice;
} // RenderDdb().


/**
 * render .CSI
 *
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderCsi(const float4 left, const float4 bottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();

  uint4 fullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    return;
  }

  // should we use the current viewpoint
  switch (m_eSliceOrientation)
  {
  case eAXIAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + left, bottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(left, (m_uDisplayHeight-viewportHeight)/2 + bottom, viewportWidth, viewportHeight);
  }

  // Make the image
  int4 i = 0;
  vxBlockVolumeIterator<uint1> insIter( myDataset.m_fields.m_ins );
  vxBlockVolumeIterator<uint2> csiIter( myDataset.m_fields.m_csi );
  for (int4 y=0;y<512;y++) 
  {
    for (int4 x=0;x<512;x++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      int4 insValue = insIter.Peek( pos.m_x, pos.m_y, pos.m_z );
      uint1 pixelValue = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      if (insValue != 0) 
      {
        pixelValue = csiIter.Peek( pos.m_x, pos.m_y, pos.m_z );
      } // inside the volume
      m_uRGBSegImage[i*3  ] = pixelValue;
      m_uRGBSegImage[i*3+1] = pixelValue;
      m_uRGBSegImage[i*3+2] = pixelValue;
      if (insValue != 0 && pixelValue == 0) {
        m_uRGBSegImage[i*3  ] = 255;
        m_uRGBSegImage[i*3+1] = 0;
        m_uRGBSegImage[i*3+2] = 0;
      } // endif (insValue).
      i++;
    } // endfor (x).
  } // endfor (y).

  // draw the image
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);

    if ( m_bBlockedPixel ) 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } 
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }
    
    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();
    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE ) 
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).
  }
  glPopMatrix();
  
  glFlush();

  m_iSliceNumber = fullVolumeSlice;
} // RenderCsi().


/**
 * render .PDR
 *
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderPdr(const float4 left, const float4 bottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();

  uint4 fullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    return;
  }

  // should we use the current viewpoint
  switch (m_eSliceOrientation)
  {
  case eAXIAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + left, bottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(left, (m_uDisplayHeight-viewportHeight)/2 + bottom, viewportWidth, viewportHeight);
  }

  // Make the image
  int4 i = 0;
  vxBlockVolumeIterator<uint1> insIter( myDataset.m_fields.m_ins );
  vxBlockVolumeIterator<float4> dfsIter( myDataset.m_fields.m_dfs );
  for (int4 y=0;y<512;y++) 
  {
    for (int4 x=0;x<512;x++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      int4 insValue = insIter.Peek( pos.m_x, pos.m_y, pos.m_z );;
      uint1 pixelValue = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      if (insValue != 0) 
      {
        pixelValue = dfsIter.Peek( pos.m_x, pos.m_y, pos.m_z );
      } // inside the volume
      m_uRGBSegImage[i*3  ] = pixelValue;
      m_uRGBSegImage[i*3+1] = pixelValue;
      m_uRGBSegImage[i*3+2] = pixelValue;
      if (insValue != 0 && pixelValue == 0)
      {
        m_uRGBSegImage[i*3  ] = 255;
        m_uRGBSegImage[i*3+1] = 0;
        m_uRGBSegImage[i*3+2] = 0;
      } // endif
      i++;
    } // endfor (x).
  } // endfor (y).

  // draw the image
  glPushMatrix(); {
    // gluOrtho2D(0, m_volumeWidth-1, m_volumeHeight-1, 0);
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage)
    {
      ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage)
    {
      ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    }

    if ( m_bBlockedPixel )
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } 
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } // endif (m_bBlockedPixel)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    
    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++)
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++)
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }
    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();

    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE )
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).

  } glPopMatrix();
  
  glFlush();

  m_iSliceNumber = fullVolumeSlice;
} // RenderPdr().


/**
 * render Biopsy
 *
 * @param left    left of the viewport.
 * @param bottom  bottom of the viewport.
 */
void SliceViewer::RenderBiopsy(const float4 left, const float4 bottom, const bool bInverseImage)
{
  Dataset & myDataset = * GetDataset();

  uint4 fullVolumeSlice;

  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
    return;

  // should we use the current viewpoint
  switch (m_eSliceOrientation)
  {
  case eAXIAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z;
    break;
  case eSAGITTAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x;
    break;
  case eCORONAL:
    fullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  // setup the viewport so the aspect ratio is correct
  float4 sliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 displayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (displayAspectRatio < sliceAspectRatio) 
  {
    // tall and narrow slice
    int4 viewportHeight = m_uDisplayHeight;
    int4 viewportWidth = viewportHeight / sliceAspectRatio;
    glViewport((m_uDisplayWidth-viewportWidth)/2 + left, bottom, viewportWidth, viewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 viewportWidth = m_uDisplayWidth;
    int4 viewportHeight = viewportWidth * sliceAspectRatio;
    glViewport(left, (m_uDisplayHeight-viewportHeight)/2 + bottom, viewportWidth, viewportHeight);
  }

  // Make the image
  int4 i = 0;
  float4 * pRed   = myDataset.m_biopsyTransferFunction.m_pfRed;
  float4 * pGreen = myDataset.m_biopsyTransferFunction.m_pfGreen;
  float4 * pBlue  = myDataset.m_biopsyTransferFunction.m_pfBlue;
  for(int4 y=0;y<512;y++) 
  {
    for(int4 x=0;x<512;x++) 
    {
      Point<uint2> pos(x,y,myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z);
      uint1 pixelValue = myDataset.m_fields.m_volumeRender.GetVoxel(pos);
      m_uRGBSegImage[i*3  ] = pRed[pixelValue];
      m_uRGBSegImage[i*3+1] = pGreen[pixelValue];
      m_uRGBSegImage[i*3+2] = pBlue[pixelValue];
      i++;
    } // endfor (y).
  } // endfor (x)

  // draw the image
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 1, 0);
    ZoomAdjust();

    // inverse.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, m_uRGBSegImage);
    // restore.
    if (bInverseImage) ReverseVideoImage(eRVI_RGB, 512, 512, m_uRGBSegImage);

    if ( m_bBlockedPixel )
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else 
    {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    Point<float4> corners[4];
    corners[0] = Point<float4>(0,0,0);
    corners[1] = Point<float4>(1,0,0);
    corners[2] = Point<float4>(1,1,0);
    corners[3] = Point<float4>(0,1,0);
    // we need to flip the axial, prone view vertically and horizontally
    if (GetDataset()->GetOrientation() == DatasetOrientations::ePRONE)
    {
      if (m_eSliceOrientation == eAXIAL)
      {
        for (uint4 i=0; i<4; i++)
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if axial, prone view
      // we need to flip the coronal, prone view horizontally
      if (m_eSliceOrientation == eCORONAL)
      {
        for (uint4 i=0; i<4; i++) 
        {
          corners[i].m_x = 1.0F - corners[i].m_x;
        } // for all 4 corners
      } // if coronal, prone view
      // we need to flip the sagittal, prone view vertically
      if (m_eSliceOrientation == eSAGITTAL)
      {
        for (uint4 i=0; i<4; i++)
        {
          corners[i].m_y = 1.0F - corners[i].m_y;
        } // for all 4 corners
      } // if sagittal, prone view
    }
    
    glBegin(GL_QUADS);
      glTexCoord2fv(&corners[0].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_top);
      glTexCoord2fv(&corners[1].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_top);
      glTexCoord2fv(&corners[2].m_x); glVertex2f(m_rZoom.m_right, m_rZoom.m_bottom);
      glTexCoord2fv(&corners[3].m_x); glVertex2f(m_rZoom.m_left,  m_rZoom.m_bottom);
    glEnd();
    // display 2D thumbnail, presumably only when panning (and not on select)
    if ( m_bDisplayOverviewSliceGrasp ) 
    {
      // if zoom factor <= 1.0; centering slice in the zoom selection area.
      if (((m_rZoom.m_bottom-m_rZoom.m_top) <= 1.0F) || ((m_rZoom.m_right-m_rZoom.m_left) <= 1.0F))
      {
        // render the 2D overview slice.
        float4 x0 = (0.25F - (m_rZoom.m_right - m_rZoom.m_left)/4.0F)/2.0F;
        float4 x1 = x0 + (m_rZoom.m_right - m_rZoom.m_left)/4.0F;
        float4 y0 = (0.75F + (0.25F-(m_rZoom.m_bottom - m_rZoom.m_top)/4.0F)/2.0);
        float4 y1 = y0 + (m_rZoom.m_bottom - m_rZoom.m_top)/4.0F;
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame(Rect<float4>(x0, y0, x1, y1));
      }
      else 
      {
        // render the 2D overview slice.
        glBegin(GL_QUADS); 
          glTexCoord2fv(&corners[0].m_x); glVertex2f(0.00F, 0.75F);
          glTexCoord2fv(&corners[1].m_x); glVertex2f(0.25F, 0.75F);
          glTexCoord2fv(&corners[2].m_x); glVertex2f(0.25F, 1.00F);
          glTexCoord2fv(&corners[3].m_x); glVertex2f(0.00F, 1.00F);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        // display a frame that around the slice.
        Draw2DOverviewSliceFrame();
      } // (zoomfactor <1.0F)
    } 
    else 
    {
      glDisable(GL_TEXTURE_2D);
    } // end-if (m_bDisplayOverviewSlice)

    // frame to select an area for zooming.
    if ( m_rSelect.m_left < MAXVALUE )
    {
      DrawSelectionFrame();
    } // end-if (m_rSelect.m_left).
  } glPopMatrix();
  
  glFlush();

  m_iSliceNumber = fullVolumeSlice;
} // RenderBiopsy().



// $Log: SliceViewerDBG.C,v $
// Revision 1.4  2005/09/29 15:01:35  vxconfig
// switched dfs and dfe volumes to new vx block volume
//
// Revision 1.3  2005/09/29 14:11:46  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.2  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.3.10.2  2005/02/15 18:42:39  jmeade
// merge from vc1.4 branch.
//
// Revision 3.3.10.1.2.2  2005/02/04 23:55:52  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.3.10.1.2.1  2004/10/07 00:30:08  jmeade
// Function name change.
//
// Revision 3.3.10.1  2004/09/22 21:56:49  jmeade
// Reverse-video imaging for new slice renderers.
//
// Revision 3.3  2002/04/12 02:15:09  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.1.2.1  2002/02/26 16:54:32  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.1  2001/12/21 03:53:26  jmeade
// CODING STANDARDS!
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:24   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:16   ingmar
// Initial revision.
// Revision 1.36  2001/07/05 14:38:33  binli
// used Rect in Draw2DOverviewSliceFrame()
//
// Revision 1.35  2001/07/03 15:20:38  binli
// better 2D slice zooming.
//
// Revision 1.34  2001/05/29 17:26:03  jmeade
// slice viewer dbg colors
//
// Revision 1.33  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.32  2001/05/06 22:58:14  kevin
// Test for CVS checkin since it has broken on two people this week.
//
// Revision 1.31  2001/05/02 20:12:12  ingmar
// BookmarkList.C
//
// Revision 1.30  2001/04/30 12:32:07  frank
// Reflected changes in Study class.
//
// Revision 1.29  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.28.2.1  2001/05/01 23:45:15  jmeade
// tabs to spaces, m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.28  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.27  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.26  2001/04/05 21:11:04  binli
// code standard: bracks/short/const...
//
// Revision 1.25  2001/04/05 16:27:06  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.24  2001/04/03 21:13:14  binli
// 1st code walkthrough
//
// Revision 1.23  2001/03/27 21:33:20  binli
// continued code walkthrough.
//
// Revision 1.22  2001/03/21 14:59:49  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.21  2001/03/20 18:50:31  binli
// code walkthrough to reflect new code standard:
// 1> moved some inline functions to .inl
// 2> changed some variables name;
// 3> changed some variables properties from public to protected or private.
// 4> new bracket rules.
//
// Revision 1.20  2001/03/16 23:16:26  binli
// renamed some variables according to new code standard.
//
// Revision 1.19  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.18  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.17  2001/03/13 15:49:08  binli
// code walkthrough
//
// Revision 1.16  2001/03/07 16:36:34  binli
// coordinate display of original slice and .ins/.csi/.pdi ... slices.
//
// Revision 1.15  2001/02/28 21:03:44  jmeade
// ScrollTo() method (for xsec/view); tabs to spaces; const & and Rect<int4> where there should be
//
// Revision 1.14  2001/02/27 23:43:16  jmeade
// hid m_eOrientation for safekeeping (more hiding to come)
//
// Revision 1.13  2001/02/09 22:49:48  binli
// flip the 2D Prone Pdr/Ins/Csi/Paint images.
//
// Revision 1.12  2001/01/25 19:41:12  frank
// Removed pointless caching.
//
// Revision 1.11  2001/01/25 16:04:46  frank
// Removed m_pStudy
//
// Revision 1.10  2001/01/24 18:19:04  binli
// added control to switch display 'blocked' or 'smoothed' slice images.
//
// Revision 1.9  2001/01/17 14:20:43  frank
// Added ability to display biopsy transfer function on slices.
//
// Revision 1.8  2001/01/08 21:47:22  binli
// put all 'CSI', 'INS', 'Paint', ' Pdr', under the zooming control.
//
// Revision 1.7  2001/01/08 21:01:17  binli
// did 'blocked'(nearest), not linear interpolation within the  texel after zooming.
//
// Revision 1.6  2001/01/08 20:10:07  binli
// zooming DDB image
//
// Revision 1.5  2001/01/03 19:48:35  kevin
// Fixed bug in xyzScale
//
// Revision 1.4  2001/01/03 19:44:37  kevin
// put ins debugging back in
//
// Revision 1.3  2001/01/03 15:04:07  frank
// Used Math library classes to eliminate variables.
//
// Revision 1.2  2000/12/27 23:03:35  jeff
// TRANSVERSE ==> AXIAL, CROSSECT ==> CROSS_SECTION
//
// Revision 1.1  2000/12/20 18:55:07  kevin
// no message
//
// Revision 1.40  2000/12/18 13:55:15  frank
// Cleared up paint volume size discrepancy
//
// Revision 1.39  2000/12/15 18:10:54  jeff
// store number of slices (based on orientation)
//
// Revision 1.38  2000/12/12 21:38:37  jeff
// Added SliceViewer::GetOrientation() method
//
// Revision 1.37  2000/11/24 15:15:25  kevin
// Added R/L/A/P/I/S marking to 2D images
//
// Revision 1.36  2000/11/22 16:43:24  frank
// Shrunk 3D arrow
//
// Revision 1.35  2000/11/21 22:58:02  liwei
// 3D arrow is blended to slices if it is facing inside-out
//
// Revision 1.34  2000/11/21 05:24:00  jeff
// Added m_bRenderCamera
//
// Revision 1.33  2000/11/20 19:45:27  frank
// Fixed slice arrow rendering
//
// Revision 1.32  2000/11/20 15:57:26  frank
// Worked on 3D arrow rendering
//
// Revision 1.31  2000/11/20 03:35:53  kevin
// Painting Debug
//
// Revision 1.30  2000/11/17 21:15:55  kevin
// FIxed some errors from changing the rendersize to be non-power-of-two
//
// Revision 1.29  2000/11/17 19:13:21  frank
// Improved arrow rendering
//
// Revision 1.28  2000/11/16 22:14:00  kevin
// I did NOT make any changes
//
// Revision 1.27  2000/11/16 18:39:53  kevin
// Added display of counting surface
//
// Revision 1.26  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.25  2000/11/13 17:12:48  liwei
// Minor changes
//
// Revision 1.24  2000/11/10 17:53:14  liwei
// Added a pointer to Study in SliceViewer
//
// Revision 1.23  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.22  2000/11/09 19:31:04  liwei
// A bug of hybrid view fixed
//
// Revision 1.21  2000/11/09 02:55:04  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.20  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.19  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.18  2000/11/07 18:21:38  liwei
// Modified for hybrid view
//
// Revision 1.17  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
// Revision 1.16  2000/11/03 19:34:52  frank
// Added PDR and CSI examiners
//
// Revision 1.15  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.14  2000/10/31 23:35:18  liwei
// Hybrid view added
//
// Revision 1.13  2000/10/31 14:37:40  frank
// Merged in new segment structure
//
// Revision 1.12  2000/10/27 01:12:45  kevin
// Added Texture Mag filter
//
// Revision 1.11  2000/10/25 14:05:43  ingmar
// put GetDataPtr back
//
// Revision 1.10  2000/10/23 16:11:29  liwei
// Camera drawn as an arrow
//
// Revision 1.9.4.1  2000/10/28 20:39:14  ingmar
// merged v1k-newSegments branch and new iterators
//
// Revision 1.9  2000/10/14 18:54:56  kevin
// Added IsAxial and bounded GetVoxelCoord if point
// is outside window
//
// Revision 1.8  2000/10/04 14:26:43  kevin
// Added  rdrGlobal units and dim for fast
// reading for study load speed
//
// Revision 1.7  2000/10/03 17:35:23  jeff
// Bug fixes in GetDisplayVoxelCoord()
//
// Revision 1.6  2000/10/02 21:56:59  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.5  2000/09/30 02:57:55  jeff
// GetDisplayedVoxelCoord function
//
// Revision 1.4  2000/09/26 02:58:14  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.3.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.3  2000/09/16 01:52:33  jeff
// Changed slice access
//
// Revision 1.2  2000/09/12 20:31:27  kevin
// CHanged ins vol to use getvoxel so it can become non-linear storage format
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 24    8/28/00 11:59a Antonio
// updated header and footer
//
//*****************  Version 23  *****************
//User: Kevin           Date:  8/23/00  Time: 10:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed the viewcolor in orthogonal images
//
//*****************  Version 22  *****************
//User: Kevin           Date:  8/23/00  Time:  1:41a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Really, Really fixed otrthogonalPoint to work with viewpoint
//color displaying whether point matches the viewpoint
//
//*****************  Version 21  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 20  *****************
//User: Kevin           Date:  8/21/00  Time: 12:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added ddb debug display in overview images
//
//*****************  Version 19  *****************
//User: Kevin           Date:  8/20/00  Time:  8:12p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed the ins axial viewers to show colonr AND the original slice
//
//*****************  Version 18  *****************
//User: Frank           Date:  8/17/00  Time: 10:44a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added orthogonal slice shadows
//Fixed cross-section slice shadow location
//
//*****************  Version 17  *****************
//User: Jeff            Date:  8/15/00  Time:  5:38p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  SetOrientation() needed to call InitVolumeLoaded() to generalize it
//(i.e. enable it to be called repeatedly)
//
//*****************  Version 16  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 15  *****************
//User: Jeff            Date:  8/09/00  Time:  1:37p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  
//
//*****************  Version 14  *****************
//User: Kevin           Date:  8/08/00  Time: 10:31a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Positional jumps and positional dragging on the 2D axial views
//
//*****************  Version 13  *****************
//User: Kevin           Date:  8/04/00  Time:  5:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed some slice scrolling problems
//
//*****************  Version 12  *****************
//User: Frank           Date:  8/01/00  Time:  9:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed camera position display
//
//*****************  Version 11  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/26/00  Time: 11:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Made the OldVolume interface just like Volume
//
//*****************  Version 9  *****************
//User: Frank           Date:  7/26/00  Time:  9:40a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wored OldVolume variables in
//anticipation of new Volume class
//
//*****************  Version 8  *****************
//User: Kevin           Date:  7/21/00  Time:  8:37p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added CDC drawing to place symbology over the axial slices and the
//distance measurements
//
//*****************  Version 7  *****************
//User: Kevin           Date:  7/21/00  Time:  6:38p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added initial axial slice scrolling
//
//*****************  Version 6  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 6
//Comment:
//  
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/18/00  Time:  1:36p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/13/00  Time:  4:37a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Fixed axis drawing
//
//*****************  Version 3  *****************
//User: Frank           Date:  7/13/00  Time:  4:16a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Testing out keyword expansion
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/13/00  Time: 11:49p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Reversed the slice orientation
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created SliceViewer.C
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SliceViewerDBG.C,v 1.4 2005/09/29 15:01:35 vxconfig Exp $
// $Id: SliceViewerDBG.C,v 1.4 2005/09/29 15:01:35 vxconfig Exp $
