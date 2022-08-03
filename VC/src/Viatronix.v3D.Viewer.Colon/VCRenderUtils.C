// $Id: VCRenderUtils.C,v 1.5 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.net)

/**
 * Class created just for VC so that it can still use the old volume reference
 * which is being removed from vxBase. Yes, this class is for the interim until
 * we completely remove the old volumes. 
 *
 */

#include "stdafx.h"
#include "VCRenderUtils.h"
#include "MeasureLineList.h"


using namespace ReaderLib;

/**
 * Draw the profile of the last measurement.
 */
void VCRenderUtils::RenderMeasurementProfile(MeasureLineList & measureList, const Rect<uint4> & drawAreaRect,
                                          TransferFunctionVC & transferFunction, LinVolUC & volData,
                                          const Matrix<float4> & mWorldToVolume)

{
  // early return if there is no measurement to render
  if (measureList.size() == 0)
    return;

  // prepare the fiery colored texture
  transferFunction.PrepareOpenGLTextureEnvironment();

  // get a pointer to the current or first measurement line
  MeasureLineList::iterator currline;
  if (measureList.m_currentIter == measureList.end())
    currline = measureList.begin();
  else
    currline = measureList.m_currentIter;

  // prepare the OpenGL environment
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  {
    glLineWidth(2.0);
    const float4 fOpacity(0.5F);
    glColor4f(1,1,1,fOpacity);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // prepare a few coordinates for rendering
    const Point<float4>  startWorldCoordPt  = currline->m_measureEnds.m_start;
    const Point<float4>   endWorldCoordPt   = currline->m_measureEnds.m_end;
    const Point<float4>  startVolumeCoordPt = mWorldToVolume * startWorldCoordPt;
    const Point<float4>   endVolumeCoordPt  = mWorldToVolume *  endWorldCoordPt;
    const Vector<float4> diffWorldCoordVec(endWorldCoordPt - startWorldCoordPt);
    const Vector<float4> diffVolumeCoordVec(endVolumeCoordPt - startVolumeCoordPt);
    const float4 fDisplayHeight = 350.0F;

    // Determine which orientation to draw the graph.
    // Choose horizontal if the slope is within 45 deg of horizontal, else vertical.
    const bool bHorizontal(abs(currline->m_drawEnds.m_start.m_y - currline->m_drawEnds.m_end.m_y)
                          < abs(currline->m_drawEnds.m_start.m_x - currline->m_drawEnds.m_end.m_x));
    
    // Determine if it is horizontal or vertical depending on the slope
    // (compare the slope to 45 degrees).
    unsigned int iDrawMin, iDrawMax;
    bool bStartToEnd;
    if (bHorizontal)
    {
      // horizontal
      if (currline->m_drawEnds.m_start.m_x < currline->m_drawEnds.m_end.m_x)
      {
        // startX < endX
        iDrawMin = currline->m_drawEnds.m_start.m_x;
        iDrawMax = currline->m_drawEnds.m_end.m_x;
        bStartToEnd = true;
      }
      else
      {
        // endX <= startX
        iDrawMin = currline->m_drawEnds.m_end.m_x;
        iDrawMax = currline->m_drawEnds.m_start.m_x;
        bStartToEnd = false;
      } // if startX < endX
    }
    else
    {
      // vertical
      if (currline->m_drawEnds.m_start.m_y < currline->m_drawEnds.m_end.m_y)
      {
        // startX < endX
        iDrawMin = currline->m_drawEnds.m_start.m_y;
        iDrawMax = currline->m_drawEnds.m_end.m_y;
        bStartToEnd = true;
      }
      else
      {
        // endX <= startX
        iDrawMin = currline->m_drawEnds.m_end.m_y;
        iDrawMax = currline->m_drawEnds.m_start.m_y;
        bStartToEnd = false;
      } // if startX < endX
    } // if horizontal
    
    // Determine if we should draw it at the top or bottom of the screen
    // (or on the left or the right if vertically oriented).
    // Get the graph away from the line as much as possible by
    // placing it on the opposite side of the window from the middle of the line.
    bool bLowSide;
    const Point<float> middlePt(
      (currline->m_drawEnds.m_start.m_x + currline->m_drawEnds.m_end.m_x) / 2.0F,
      (currline->m_drawEnds.m_start.m_y + currline->m_drawEnds.m_end.m_y) / 2.0F, 0.0F);
    if (bHorizontal)
    {
      // horizontal
      if (middlePt.m_y < drawAreaRect.Height() / 2.0F)
        bLowSide = true;
      else 
        bLowSide = false;
    }
    else
    {
      // vertical
      if (middlePt.m_x >= drawAreaRect.Width() / 2.0F)
        bLowSide = true;
      else
        bLowSide = false;
    } // if horizontal

    // draw the profile
    glEnable(GL_TEXTURE_1D);
    const uint4 uDrawWidth(iDrawMax - iDrawMin);
    const float4 fLength(diffWorldCoordVec.GetLength());
    glBegin(GL_QUAD_STRIP);
    {
      // loop over the width of the measurement
      for (unsigned int i=0; i < uDrawWidth; i++)
      {
        const float4 fPercent(float4(i) / float4(uDrawWidth));
        const Point<float4> positionPt(startVolumeCoordPt + diffVolumeCoordVec * fPercent);
        const float4 fValue(volData.GetResampledValue(positionPt) / 255.0F);

        float fDrawCoordMajor; // the major axis coordinate (e.g., x for horizontal)
        if (bStartToEnd)
          fDrawCoordMajor = iDrawMin + uDrawWidth * fPercent;
        else
          fDrawCoordMajor = iDrawMax - uDrawWidth * fPercent;
        const float4 fDistance(fLength * fPercent);
        glTexCoord1f(fValue);
        if (bHorizontal)
        {
          // horizontal
          if (bLowSide)
            glVertex2i(fDrawCoordMajor, 0);
          else
            glVertex2i(fDrawCoordMajor, drawAreaRect.Height());
        }
        else
        {
          // vertical
          if (bLowSide)
            glVertex2i(0, drawAreaRect.Height() - fDrawCoordMajor);
          else
            glVertex2i(drawAreaRect.Width(), drawAreaRect.Height() - fDrawCoordMajor);
        } // if horizontal
        const float fDrawCoordMinor(fValue * fDisplayHeight);
        if (bHorizontal)
        {
          // horizontal
          if (bLowSide)
            glVertex2i(fDrawCoordMajor, fDrawCoordMinor);
          else
            glVertex2i(fDrawCoordMajor, drawAreaRect.Height() - fDrawCoordMinor);
        }
        else
        {
          // vertical
          if (bLowSide)
            glVertex2i(fDrawCoordMinor, drawAreaRect.Height() - fDrawCoordMajor);
          else
            glVertex2i(drawAreaRect.Width() - fDrawCoordMinor, drawAreaRect.Height() - fDrawCoordMajor);
        } // if horizontal
      } // loop over width
    } // glBegin
    glEnd();
    glDisable(GL_TEXTURE_1D);

    // outline the top of the graph for clarity
    glBegin(GL_LINE_STRIP);
    {
      for (unsigned int i=0; i < uDrawWidth; i++)
      {
        const float4 fPercent(float(i) / float(uDrawWidth));
        const Point<float4> positionPt(startVolumeCoordPt + diffVolumeCoordVec * fPercent);
        const float4 fValue(volData.GetResampledValue(positionPt) / 255.0F);
        float fDrawCoordMajor;
        if (bStartToEnd)
          fDrawCoordMajor = iDrawMin + uDrawWidth * fPercent;
        else
          fDrawCoordMajor = iDrawMax - uDrawWidth * fPercent;
        const float fDrawCoordMinor(fValue * fDisplayHeight);
        if (bHorizontal)
        {
          // horizontal
          if (bLowSide)
            glVertex2i(fDrawCoordMajor, fDrawCoordMinor);
          else
            glVertex2i(fDrawCoordMajor, drawAreaRect.Height() - fDrawCoordMinor);
        }
        else
        {
          // vertical
          if (bLowSide)
            glVertex2i(fDrawCoordMinor, drawAreaRect.Height() - fDrawCoordMajor);
          else
            glVertex2i(drawAreaRect.Width() - fDrawCoordMinor, drawAreaRect.Height() - fDrawCoordMajor);
        } // if horizontal
      } // loop over width
    } // glBegin
    glEnd();
    
    // draw mm markings
    // loop over each mm of the length
    for (unsigned int i=0; i < fLength; i++)
    {
      // 1mm markings have the default height
      float4 fDrawHeight(fDisplayHeight * 0.035F);
      
      // if this is a 5mm marking make it taller
      if (i%5 == 0)
      {
        fDrawHeight *= 2.0F;
      }

      // draw the line
      const float4 fPercent(float4(i) / float4(fLength));
      glLineWidth(1.0F);
      glBegin(GL_LINES);
      {
        float fDrawCoordMajor;
        if (bStartToEnd)
          fDrawCoordMajor = iDrawMin + uDrawWidth * fPercent;
        else
          fDrawCoordMajor = iDrawMax - uDrawWidth * fPercent;

        // first vertex
        if (bHorizontal)
        {
          // horizontal
          if (bLowSide)
            glVertex2i(fDrawCoordMajor, 0);
          else
            glVertex2i(fDrawCoordMajor, drawAreaRect.Height());
        }
        else
        {
          // vertical
          if (bLowSide)
            glVertex2i(0, drawAreaRect.Height() - fDrawCoordMajor);
          else
            glVertex2i(drawAreaRect.Width(), drawAreaRect.Height() - fDrawCoordMajor);
        } // if horizontal

        // second vertex
        if (bHorizontal)
        {
          // horizontal
          if (bLowSide)
            glVertex2i(fDrawCoordMajor, fDrawHeight);
          else
            glVertex2i(fDrawCoordMajor, drawAreaRect.Height() - fDrawHeight);
        } // if horizontal
        else
        {
          // vertical
          if (bLowSide)
            glVertex2i(fDrawHeight, drawAreaRect.Height() - fDrawCoordMajor);
          else
            glVertex2i(drawAreaRect.Width() - fDrawHeight, drawAreaRect.Height() - fDrawCoordMajor);
        } // if horizontal
      } // glBegin
      glEnd();
      
    } // iterate over all the mm of the length
    glDisable(GL_BLEND);

  } // glPushAttrib
  glPopAttrib();

} // RenderMeasurementProfile()



// Revision History:
// $Log: VCRenderUtils.C,v $
// Revision 1.5  2007/03/08 18:29:00  jmeade
// code standards.
//
// Revision 1.4  2006/06/01 20:23:13  frank
// updated to visual studio 2005
//
// Revision 1.3  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.2  2005/08/16 21:58:20  jmeade
// namespaces for ReaderLib.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1  2002/05/03 21:24:48  jaddonisio
// New files created for the interim while we start deleteing old volumes.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VCRenderUtils.C,v 1.5 2007/03/08 18:29:00 jmeade Exp $
// $Id: VCRenderUtils.C,v 1.5 2007/03/08 18:29:00 jmeade Exp $
