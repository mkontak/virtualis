// $Id: RenderContext.C,v 1.2 2006/01/31 14:04:55 frank Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: A class encapsulating data for volume rendering
// Author: Wei Li (liwei@viatronix.net)


// pragmas
#pragma once


// includes
#include "stdafx.h"
#include "RenderContext.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


// namespaces
using namespace ReaderLib;


// Constructor
RenderContext::RenderContext()
{
  m_pDepthBuffer = NULL;
  m_pImageBuffer = NULL;
  m_uDisplayWidth = 0;
  m_uDisplayHeight = 0;
  m_uRenderWidth = 0;
  m_uRenderHeight = 0;
  m_fZoomFactor = 1.0f;
  m_fFov = 45.0f;
  m_fRatio = 1.0f;

  m_TTPTransferFunction.SetTransparentTissueProjection();
  m_ExtViewTransferFunction.SetExternalColonView();
}

// Destructor
RenderContext::~RenderContext()
{
  if(m_pDepthBuffer)
    delete [] m_pDepthBuffer;
  if(m_pImageBuffer)
    delete [] m_pImageBuffer;
  m_pDepthBuffer = NULL;
  m_pImageBuffer = NULL;
}

// Display the image
void RenderContext::DisplayRenderedImage()
{
  int4 iOldUnpackRowLength;
  glGetIntegerv(GL_UNPACK_ROW_LENGTH, &iOldUnpackRowLength);

  float4 uMax, vMax;
  uMax = ((float)m_uDisplayWidth)/m_uRenderWidth;
  vMax = ((float)m_uDisplayHeight)/m_uRenderHeight;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); {

    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); {
      glDisable(GL_LIGHTING);

      glLoadIdentity();

      glOrtho(0, 1, 0, 1, 0, -1);
      float fOffsetX = 0;
      float fOffsetY = 0;
      //if (m_fRatio >1.0f)
      //{
      //  fOffsetX = (1 - 1.0f/m_fRatio) / 2.0f;
      //  fOffsetY = 0.0f;
      //}
      //else
      //{
      //  fOffsetY = (1 - 1.0f*m_fRatio) / 2.0f;
      //  fOffsetX = 0.0f;
      //}

      glPixelStorei(GL_UNPACK_ROW_LENGTH, m_uRenderWidth);
      
      glBindTexture( GL_TEXTURE_2D, 0 );
      glTexImage2D (GL_TEXTURE_2D, 0, 4, m_uRenderWidth, m_uRenderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pImageBuffer);
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_ALPHA_TEST);
      glBegin(GL_QUADS); 
      {
        // aspect ratio scaling is handled now by the raycaster
        glTexCoord2d(0, 0);        glVertex3f(0, 0, 0);
        glTexCoord2d(0, vMax);     glVertex3f(0, 1, 0);
        glTexCoord2d(uMax, vMax);  glVertex3f(1, 1, 0);
        glTexCoord2d(uMax, 0);     glVertex3f(1, 0, 0);
        //glTexCoord2d(0,    0);      glVertex3f(fOffsetX,   fOffsetY, 0);
        //glTexCoord2d(0,    vMax);   glVertex3f(fOffsetX,   1-fOffsetY, 0);
        //glTexCoord2d(uMax, vMax);   glVertex3f(1-fOffsetX, 1-fOffsetY, 0);
        //glTexCoord2d(uMax, 0);      glVertex3f(1-fOffsetX, fOffsetY, 0);
      } 
      glEnd();
      glDisable(GL_TEXTURE_2D);
    } glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  } glPopMatrix();
  glPixelStorei(GL_UNPACK_ROW_LENGTH, iOldUnpackRowLength);
}

// Resize the render buffer
void RenderContext::SetRenderSize(const uint4 uRenderWidth, const uint4 uRenderHeight)
{
  if (m_uRenderWidth != uRenderWidth || m_uRenderHeight != uRenderHeight)
  {
    m_uRenderWidth = uRenderWidth;
    m_uRenderHeight = uRenderHeight;
    if(m_pImageBuffer)
    {
      delete [] m_pImageBuffer;
    }

    if(m_pDepthBuffer)
    {
      delete [] m_pDepthBuffer;
    }

    m_pImageBuffer = new unsigned char[4*m_uRenderWidth*m_uRenderHeight];
    m_pDepthBuffer = new float[m_uRenderWidth*m_uRenderHeight];

    if (m_pImageBuffer == NULL ||  m_pDepthBuffer == NULL)
    {
      throw ex::OutOfMemoryException(LogRec("Error in alloc memory for render context", "RenderContext", "SetRenderSize"));
    }
  }
}

// Return the smallest possible value of power of two that is greater or equal to x
uint4 RenderContext::ExpandToPowerOf2(const uint4 x)
{
  uint4 y = 2;
  while(y < x)
  {
    y = y << 1;
  }
  return y;
}

// Set the size of display
void RenderContext::SetDisplaySize(const uint4 uDisplayWidth, const uint4 uDisplayHeight)
{
  if ((uDisplayWidth == m_uDisplayWidth) && (m_uDisplayHeight == uDisplayHeight))
  {
    return;
  }
  m_uDisplayWidth = uDisplayWidth;
  m_uDisplayHeight = uDisplayHeight;

  SetRenderSize(ExpandToPowerOf2(uDisplayWidth), ExpandToPowerOf2(uDisplayHeight));
}

// Clear the imageBuffer only
// The depth buffer is not cleared for performance reason
void RenderContext::ClearBuffer()
{
  void* pMem = (void*)m_pImageBuffer;
  if(pMem)
    memset((void*)m_pImageBuffer, 0, 4*m_uRenderWidth*m_uRenderHeight * sizeof(unsigned char));
//  pMem = (void*)m_depthBuffer;
//  memset(pMem, 0, m_renderWidth*m_renderHeight* sizeof(float));
}

// $Log: RenderContext.C,v $
// Revision 1.2  2006/01/31 14:04:55  frank
// code review
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.2.16.2  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.2.16.1.2.1  2005/05/25 15:50:21  geconomos
// initial backup
//
// Revision 3.2.16.1  2005/04/06 20:35:23  frank
// Issue #3994: Allowed non-square aspect ratios in volume rendering
//
// Revision 3.2  2002/06/26 17:49:55  jmeade
// Moving more hard-coded strings to the string table; Code standards.
//
// Revision 3.1  2002/01/03 02:30:12  kevin
// Cleanup related to new TTP work when ses, set and sec went away
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 18 2001 14:38:48   binli
// track ID: 000523: transparent rendering of overview: proper w/h ratio. No freeze now
// 
//    Rev 2.0   Sep 16 2001 23:40:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:08   ingmar
// Initial revision.
// Revision 1.2  2001/03/21 21:14:24  liwei
// Updated RenderContext class for coding guideline compliance.
//
// Revision 1.1  2001/03/05 21:48:03  liwei
// Code rearranged to comply with the coding standard
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/RenderContext.C,v 1.2 2006/01/31 14:04:55 frank Exp $
// $Id: RenderContext.C,v 1.2 2006/01/31 14:04:55 frank Exp $
