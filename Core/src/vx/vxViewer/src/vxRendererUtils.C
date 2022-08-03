// $Id: vxRendererUtils.C,v 1.5.8.1 2009/03/23 15:43:15 gdavidson Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// include declarations
#include "stdafx.h"
#include "vxRendererUtils.h"
#include "GLFontGeorge.h"
#include "PixelRGBA.h"
#include "Box.h"
#include "BaseHistogram.h"
#include "utilities.h"
#include "vxWinRect.h"
#include "vxRenderBuffer.h"
#include "ResourceImage.h"


// define declarations
#define FILE_REVISION "$Revision: 1.5.8.1 $"


/**
 * Draw a point
 * @param point
 * @param color
 * @param size
 */
void vxRendererUtils::GLDrawPoint2f(const Point2D<float4> & point2D, 
                                    const PixelRGBA<float4> & color, 
                                    const float4 & fSize)
{
  glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT);
  glPointSize(fSize);
  glColor3f(color.R(),color.G(),color.B());
  glBegin(GL_POINTS);
  {
    glVertex2f(point2D.m_x, point2D.m_y);
  }
  glEnd();
  glPopAttrib();
} // GLDrawPoint2f()


/**
 * Draw a point with border
 * @param point
 * @param color
 * @param border color
 * @param size
 * @param border size
 */
void vxRendererUtils::GLDrawPointWithBorder2f(const Point2D<float4> & point2D, 
                                              const PixelRGBA<float4> & color,
                                              const PixelRGBA<float4> & colorBorder,
                                              const float4 & fSize, 
                                              const float4 & fSizeBorder)
{
  GLDrawPoint2f(point2D, colorBorder, fSize+fSizeBorder);
  GLDrawPoint2f(point2D, color, fSize);
} // GLDrawPointWithBorder2f()


/**
 * Draw a box
 * @param box
 * @param color
 */
void vxRendererUtils::GLDrawBox(Box<float4> & box, 
                                const PixelRGBA<float4> & color)
{
  glPushAttrib(GL_CURRENT_BIT);
  glColor3f(color.R(),color.G(),color.B());
  glBegin(GL_LINE_LOOP);
  {
    GlVertex(box.GetCorner(0));
    GlVertex(box.GetCorner(1));
    GlVertex(box.GetCorner(3));
    GlVertex(box.GetCorner(2));
  }
  glEnd();
  glBegin(GL_LINE_LOOP);
  {
    GlVertex(box.GetCorner(4));
    GlVertex(box.GetCorner(5));
    GlVertex(box.GetCorner(7));
    GlVertex(box.GetCorner(6));
  }
  glEnd();
  glBegin(GL_LINES);
  {
    GlVertex(box.GetCorner(0));
    GlVertex(box.GetCorner(4));
    GlVertex(box.GetCorner(1));
    GlVertex(box.GetCorner(5));
    GlVertex(box.GetCorner(2));
    GlVertex(box.GetCorner(6));
    GlVertex(box.GetCorner(3));
    GlVertex(box.GetCorner(7));
  }
  glEnd();
  glPopAttrib();
} // GLDrawBox()


/**
 * Projects given 3D point onto 2D window.
 * Utility method assumes that the current OpenGL context is set 
 * correctly. Since this method always reads the necessary values
 * from the OpenGL context which is time cosuming ...
 */
void vxRendererUtils::Project(float4 fObjX, float4 fObjY, float4 fObjZ,
                              float4 & fWinX, float4 & fWinY, float4 & fWinZ)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble modelmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  float8 fWindowX, fWindowY, fWindowZ;
  gluProject(fObjX, fObjY, fObjZ, modelmatrix, projmatrix, viewport, &fWindowX, &fWindowY, &fWindowZ);
  fWinX = fWindowX;
  fWinY = fWindowY;
  fWinZ = fWindowZ;

  return;
} // Project()


/**
 * Projects given 3D point onto 2D window.
 * Utility method assumes that the the viewport and matrices are
 * passed in and not used fromt he current OpenGL context. This
 * might be good to do it after the context has changed or to
 * avoid constant reading of the frame-buffer when calling the
 * method multiple times (performance).
 * @param object x coordinate
 * @param object y coordinate
 * @param object z coordinate
 * @param viewport
 * @param model view matrix
 * @param projection matrix
 * @param window x coordinate
 * @param window y coordinate
 * @param window z coordinate
 */
void vxRendererUtils::Project(float4 fObjX, float4 fObjY, float4 fObjZ,
                              GLint * pViewPort, 
                              GLdouble * pModelmatrix, GLdouble * pProjmatrix,
                              float4 & fWinX, float4 & fWinY, float4 & fWinZ)
{
  GLdouble fWindowX, fWindowY, fWindowZ;
  gluProject(fObjX, fObjY, fObjZ, pModelmatrix, pProjmatrix, pViewPort, &fWindowX, &fWindowY, &fWindowZ);
  fWinX = fWindowX;
  fWinY = fWindowY;
  fWinZ = fWindowZ;

  return;
} // Project()


/**
 * Draws a 2d rectangle using OpenGL
 * @param specifies OpenGL mode
 * @param color red
 * @param color green
 * @param color blue
 * @param coordinate x
 * @param coordinate y
 * @param size x
 * @param size y
 */
void vxRendererUtils::DrawRectangle2i(GLenum mode, 
                                      int4 iRed, int4 iGreen, int4 iBlue,
                                      int4 iX, int4 iY, 
                                      int4 iSizeX, int4 iSizeY)
{
  glBegin(mode);
    glColor3f(iRed/255.0, iGreen/255.0, iBlue/255.0);
    glVertex2i(iX, iY);
    glVertex2i(iX, iY + iSizeY);
    glVertex2i(iX + iSizeX, iY + iSizeY);
    glVertex2i(iX + iSizeX, iY);
  glEnd();

  return;
} // DrawRectangle2i()


/**
 * Render a 3D arrow at some position, direction
 * @param position
 * @param direction
 * @param length
 * @param should it be centered?
 * @param color
 */

void vxRendererUtils::RenderArrow(const Point<float4> & vPosition, 
                                  const Normal<float4> & vDirection, 
                                  const float4 & fLength,
                                  const bool & bCentered, 
                                  const PixelRGBA<float4>& color)
{
  //Uses depth buffer by default
  RenderArrow( vPosition, vDirection, fLength, bCentered, true, color );
}//RenderArrow(const Point<float4> & vPosition, const Normal<float4> & vDirection, const float4 & fLength,const bool & bCentered, const PixelRGBA<float4>& color)


/**
 * Render a 3D arrow at some position, direction
 * @param position
 * @param direction
 * @param length
 * @param should it be centered?
 * @param should it use depth buffer?
 * @param color
 */
void vxRendererUtils::RenderArrow(const Point<float4> & vPosition, 
                                  const Normal<float4> & vDirection, 
                                  const float4 & fLength,
                                  const bool & bCentered, 
                                  const bool & bUseDepthBuffer,
                                  const PixelRGBA<float4>& color)
{
  GLUquadricObj * pQuadric = gluNewQuadric();

  glPushAttrib( GL_ALL_ATTRIB_BITS );
  // determine the proportions
  const float fRadiusToLengthBig(0.3F);
  const float fRadiusToLengthSmall(0.15F);
  const float fRadiusBig(fLength * fRadiusToLengthBig);
  const float fRadiusSmall(fLength * fRadiusToLengthSmall);
  const float fProportionHead(0.5F);
  const float fLengthHead(fLength * fProportionHead);
  const float fLengthTail(fLength * (1.0F - fProportionHead));
  const unsigned int slices(16); // accuracy of cylinder approximation

  GLfloat whiteLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat sourceLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat light1Pos[] = { -128.0f, -128.0f, -128.0f, 1.0f };
  GLfloat light2Pos[] = { 640.0f, 640.0f, 640.0f, 1.0f };

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLightfv(GL_LIGHT2, GL_DIFFUSE, sourceLight);
  glLightfv(GL_LIGHT3, GL_POSITION, light1Pos);
  glEnable(GL_LIGHT2);

  glLightfv(GL_LIGHT3, GL_DIFFUSE, sourceLight);
  glLightfv(GL_LIGHT3, GL_POSITION, light2Pos);
  glEnable(GL_LIGHT3);

  // render with transformations
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glPopMatrix();

  // bright blue.
  GLfloat matEmission[] = {0.3F, 0.0F, 0.5F, 0.0F};
  GLfloat matAmbientAndDiffuse[] = {0.05F, 0.82F, 0.70F, 1.0F};

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);

  if ( bUseDepthBuffer )
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
    // transform to the proper position and direction
    glTranslatef(vPosition.m_x, vPosition.m_y, vPosition.m_z);
    const Normal<float4> nArrowDirection(0,0,-1);
    const float4 degreesPerRadian( static_cast< float4 >( 180.0F / M_PI ) );

    // compute the angle within the x-y plane with x-axis 0 deg and y-axis 90 deg
    const float4 xyAngleRadians(atan2(vDirection.Y(), vDirection.X()));

    // compute the angle it makes away from the z-axis
    const float4 zAxisAngleRadians(acos(nArrowDirection.Dot(vDirection)));

    // first rotate in the x-y plane
    glRotatef(xyAngleRadians * degreesPerRadian, 0.0F, 0.0F, 1.0F);

    // then rotate away from the z-axis toward the x-axis
    glRotatef(-zAxisAngleRadians * degreesPerRadian, 0.0F, 1.0F, 0.0F);

    if (bCentered == true)
    {
      glTranslatef(0.0F, 0.0F, -fLength / 2.0F);
    }

    // render head cone
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, 0.0F);
      gluCylinder(pQuadric, 0.0F, fRadiusBig, fLengthHead, slices, 1);
    } glPopMatrix();

    // make cone base
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, fLength - fLengthHead);
      gluDisk(pQuadric, 0, fRadiusBig, slices, 1);
    } glPopMatrix();

    // make cylinder
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, fLengthHead);
      gluCylinder(pQuadric, fRadiusSmall, fRadiusSmall, fLengthTail, slices, 1);
    } glPopMatrix();

    // make end cap
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, fLength);
      gluDisk(pQuadric, 0, fRadiusSmall, slices, 1);
    }
    glPopMatrix();

  }
  glPopMatrix();

  GLfloat no_matEmission[] = { 0.0F, 0.0F, 0.0F, 1.0F };
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_matEmission);

  glEnable(GL_LIGHT0);
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  gluDeleteQuadric(pQuadric);
  glPopAttrib();

} // RenderArrow()


/**
 * Renders a histogram into an OpenGL window. The context has to be
 * active and the area are defined by min/max and the bins are the
 * active area of the histogram that is to be displayed.
 * @param pHistogram  pointer to histogram
 * @param pImage      pointer to image to be generated
 * @param uLowerBin   lowest bin to be considered
 * @param uUpperBin   uppermost bin to be considered
 * @param color       color for image
 */
void vxRendererUtils::CreateHistogramImage(BaseHistogram * pHistogram,
                                           Image<PixelRGBA<uint1> > & image,
                                           const uint2 & uLowerBin, 
                                           const uint2 & uUpperBin,
                                           PixelRGBA<uint1> & color)
{
  bool bPossible(true);

  if (pHistogram != NULL)
  {
    uint2 uNumBins(pHistogram->GetNumOfBins());
    if (uNumBins != 0)
    {
      const uint1 uRed(color.R());
      const uint1 uGreen(color.G());
      const uint1 uBlue(color.B());
      const uint1 uAlpha(color.A());

      // set image to size
      const uint2 uSizeX(image.GetDim()[0]);
      const uint2 uSizeY(image.GetDim()[1]);

      // find maximum value in histogram
      std::vector<uint4> & vBins(pHistogram->GetBinsVector());
      float4 fMaxCount(0.0f);
      for (uint2 i(1); i<uNumBins; ++i) // ignore the zero since it is usually huge
      {
        if (fMaxCount < vBins[i])
        {
          fMaxCount = vBins[i];
        }
      }

      uint1 * pImagePixels = reinterpret_cast<uint1 *>(const_cast<PixelRGBA<uint1> *>(image.GetDataPtr()));
      uint4 uSum(vBins[0]);
      float4 fSum(vBins[0]);
      float fScale(uSizeY/fMaxCount);
      uint2 uPreviousSourceX(0);
      for (uint4 i(0); i<uSizeX; i++)
      {
        uint2 uSourceX(0.5 + uLowerBin + (uNumBins-1) * (i+1)/(float)uSizeX);
        if (uSourceX > uUpperBin)
          uSourceX = uUpperBin;

        if (uSourceX != uPreviousSourceX)
        {
          uSum = 0;
          for (uint4 j(uPreviousSourceX); j<uSourceX; j++)
          {
            uSum += vBins[j];
          }
          fSum = uSum/(float4)(uSourceX - uPreviousSourceX);
        }

        uint2 uMax(fSum * fScale);
        uint1 * pPixels(pImagePixels + 4*i);
        if (uMax > uSizeY)
          uMax = uSizeY;

        for (uint2 k(0); k<uMax; k++)
        {
          pPixels[0] = uRed;
          pPixels[1] = uGreen;
          pPixels[2] = uBlue;
          pPixels[3] = uAlpha;
          pPixels += 4*uSizeX;
        }
        for (uint2 k(uMax); k<uSizeY; k++)
        {
          pPixels[0] = 0;
          pPixels[1] = 0;
          pPixels[2] = 0;
          pPixels[3] = 0;
          pPixels += 4*uSizeX;
        }
        uPreviousSourceX = uSourceX;
      }

      return;
    }
  }

  // if we were not succesful, make image empty
  image.Clear();

} // CreateHistogram()


/**
 * draws a small rectangle on the screen to indicate non-optimal image quality
 *
 * @param viewerRect the viewport of this viewer
 */
void vxRendererUtils::DrawImageQualityWarningRectangle( const vxWinRect & viewerRect, bool bLowQuality )
{

  // set up identity matrix
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0f, viewerRect.GetViewportWidth() - 1, 0.0f, viewerRect.GetViewportHeight() - 1 );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  // create a solid orange box as a singleton
  static bool bFirstTime = true;
  const int4 iSize = 12;
  static uint1 pLowQualityData[ iSize * iSize * 4 ];
  static uint1 pData[ iSize * iSize * 4 ];
  if ( bFirstTime )
  {
    bFirstTime = false;
    for ( int i=0; i<iSize * iSize; ++i )
    {
      // set to translucent orange (BGRA order) for low quality
      pLowQualityData[ 4 * i + 0 ] = 0;
      pLowQualityData[ 4 * i + 1 ] = 255;
      pLowQualityData[ 4 * i + 2 ] = 255;
      pLowQualityData[ 4 * i + 3 ] = 186;

      // set to green
      pData[ 4 * i + 0 ] = 0;
      pData[ 4 * i + 1 ] = 255;
      pData[ 4 * i + 2 ] = 0;
      pData[ 4 * i + 3 ] = 50;
    }
  }

  // draw a solid box
  glRasterPos2i( 4, viewerRect.GetViewportHeight() * 0.25F + iSize / 2 );
  glPixelStorei( GL_PACK_ALIGNMENT, 1 );
  glDrawPixels( iSize, iSize, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ( bLowQuality ) ? pLowQualityData : pData );

  // restore matrices
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glPopAttrib();

} // DrawImageQualityWarningRectangle( viewerRect )


/**
 * Renders the "Viatronix" logo
 *
 * @param bTranslucent    Render partially translucent
 */
void vxRendererUtils::DrawLogo( const vxWinRect & viewerRect, bool bTranslucent )
{
  // set up identity matrix
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0f, viewerRect.GetViewportWidth() - 1, 0.0f, viewerRect.GetViewportHeight() - 1 );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  if( bTranslucent )
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

  glColor4f( 1, 1, 1, 0.75F );

  // draw the logo as a bitmap
  static Image< PixelRGBA<uint1> > * pLogoBitmap = NULL;
  if ( pLogoBitmap == NULL )
  {
    pLogoBitmap = ResourceImage::FromResource( "LOGO" );
    //pLogoBitmap = GdiplusUtilities::LoadImageFromResource( MAKEINTRESOURCE( IDB_VIATRONIX_LOGO ), "PNG" );

    // flip the image
    for ( int4 y(0); y<pLogoBitmap->GetDim().m_y / 2; ++y )
    {
      for ( int4 x(0); x<pLogoBitmap->GetDim().m_x; ++x )
      {
        const Point2D< uint2 > pos1( x, y );
        const Point2D< uint2 > pos2( x, pLogoBitmap->GetDim().m_y - 1 - y );
        const PixelRGBA< uint1 > tempPixel( pLogoBitmap->GetPixel( pos1 ) );
        pLogoBitmap->SetPixel( pos1, pLogoBitmap->GetPixel( pos2 ) );
        pLogoBitmap->SetPixel( pos2, tempPixel );
      } // for x
    } // for y
  }
  if ( pLogoBitmap )
  {
    Rect<float4> viewPortRect( 0, 0, viewerRect.GetViewportSize().first, viewerRect.GetViewportSize().second );

    glPixelTransferf( GL_ALPHA_SCALE, bTranslucent ? 0.5F : 1.0F );
    const int4 iRasterX = int4( ( viewerRect.GetViewportWidth() - pLogoBitmap->GetDim().m_x ) * 0.5F );
    const int4 iRasterY = viewPortRect.m_top;
    glRasterPos2i( iRasterX, iRasterY );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, pLogoBitmap->GetDim().m_x );
    try
    {
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );
      glDrawPixels( pLogoBitmap->GetDim().m_x, pLogoBitmap->GetDim().m_y, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast< GLubyte * >( pLogoBitmap->GetDataPtr() ) );
      glDisable( GL_BLEND );
    }
    catch ( ... )
    {
    }
  }
  
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

  if( bTranslucent )
    glDisable(GL_BLEND);

  // restore matrices
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glPopAttrib();
} // GLRenderLogo()

// undefines
#undef FILE_REVISION


// $Log: vxRendererUtils.C,v $
// Revision 1.5.8.1  2009/03/23 15:43:15  gdavidson
// Added code to renderer a viatronix logo
//
// Revision 1.5  2007/01/25 22:41:39  gdavidson
// Added a flag that allows rendering for two warning levels
//
// Revision 1.4  2006/12/12 16:50:23  romy
// Disables depth buffer during the arrow rendering to bring it to the front of Slice shadows.
//
// Revision 1.3  2006/10/05 21:15:13  geconomos
// changed color of low quality warning indicator
//
// Revision 1.2  2006/09/20 20:34:43  frank
// Issue #4963: Added a utility function to display a small orange box on the view to indicate that the image is not fully rendered
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.28  2003/12/18 21:22:19  frank
// Removed compile warning
//
// Revision 1.27  2003/10/03 12:13:25  michael
// took out log messages that were used for debugging
//
// Revision 1.26  2003/08/20 15:48:18  michael
// using M_PI instead of typed in values for it
//
// Revision 1.25  2003/08/06 13:25:40  michael
// changed histogram texture creation so that bin 0 is ignored (air)
//
// Revision 1.24  2003/07/30 18:43:41  michael
// added utility to create a histogram image
//
// Revision 1.23  2003/07/29 11:05:08  michael
// replaced redundant code by one centralized piece that draws the histogram.
// However, it is still actively drawn but should use a texture that is only updated
// if the BaseHistogram has changed ...
//
// Revision 1.22  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.21  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.20  2003/05/14 17:28:25  michael
// code review
//
// Revision 1.19  2003/05/08 21:06:56  geconomos
// Fixed Michael's stuff.
//
// Revision 1.18  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.17  2002/11/15 18:10:15  frank
// Removed unused parameter (iOpacity)
//
// Revision 1.16  2002/08/22 13:50:38  michael
// fixed cpy/past comment in doxygen text
//
// Revision 1.15  2002/08/21 18:30:40  michael
// extended utility method for drawing a rectangle
//
// Revision 1.14  2002/08/07 14:37:07  michael
// Added utility to draw a rectangle in OpenGL
//
// Revision 1.13  2002/08/02 14:51:37  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.12  2002/07/30 17:17:11  michael
// Removed redundant methods
//
// Revision 1.11  2002/07/29 22:38:25  michael
// Fixed 32 bit encoding and evaluation whether a block is visible. There were
// several thigns wrong. only 31 bits were used and the comparison of the
// logical AND was compared with >0 butneeds to be !=0
//
// Revision 1.10  2002/07/28 14:43:04  michael
// fixed bug: Ingmar moved from 16 to 32 bins but forgot to the NUMBER_OF_BINS to 32, too.
//
// Revision 1.9  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.8  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.7  2002/07/18 15:22:49  ingmar
// changed bins from 16 to 32
//
// Revision 1.6  2002/06/27 19:55:39  soeren
// fixed GenerateBitCodeFor12BitTransferFunction
//
// Revision 1.5.2.8  2002/07/18 14:57:13  jaddonisio
// Typecast initialization of const iBinSize to int4. This avoids a serious conversion problem on my machine.
//
// Revision 1.5.2.7  2002/07/17 20:49:09  frank
// Made block visibility more convenient to use.
//
// Revision 1.5.2.6  2002/07/17 17:06:02  frank
// Added utility class to manage block visibility complexity.
//
// Revision 1.5.2.5  2002/07/09 20:08:37  michael
// cosmetics
//
// Revision 1.5.2.4  2002/07/08 15:22:53  frank
// Added label visible utility function.
//
// Revision 1.5.2.3  2002/07/02 22:24:20  michael
// fixed bug in bit encoding for intervals
//
// Revision 1.5.2.2  2002/07/02 13:47:50  michael
// added 32 bit encoding method for interval and fixed 12 bit encoding code
//
// Revision 1.5.2.1  2002/07/01 21:23:15  frank
// Added utility function to traverse histograms.
//
// Revision 1.5  2002/06/27 16:02:21  soeren
// made parameter const
//
// Revision 1.4  2002/06/27 15:42:54  soeren
// added GenerateBitCodeFor12BitTransferFunction
//
// Revision 1.3  2002/06/27 14:07:32  michael
// using 16 bins for transfer function and 31 bins for labels and moved helper
// methods to vxRendererUtils
//
// Revision 1.2  2002/03/20 19:46:02  michael
// moved OpenGl back-projection stuff to 74_vxUtils
//
// Revision 1.1  2002/03/20 19:19:16  michael
// extended vxRendererUtils to perform the OpenGL backprojection
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererUtils.C,v 1.5.8.1 2009/03/23 15:43:15 gdavidson Exp $
// $Id: vxRendererUtils.C,v 1.5.8.1 2009/03/23 15:43:15 gdavidson Exp $
