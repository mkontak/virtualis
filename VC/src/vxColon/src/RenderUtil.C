// $Id: RenderUtil.C,v 1.14.2.3 2010/02/08 22:52:34 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Kevin Kreeger (kevin@viatronix.net)


/**
 * Utilities for rendering
 */


// includes
#include "stdafx.h"
#include "RenderUtil.h"
#include "GLEnv.h"
#include "vxBlockVolumeIterator.h"
#include "Box.h"


// defines
#define FILE_REVISION "$Revision: 1.14.2.3 $"

// namespaces
using namespace vxCAD;


// static member initialization
Triple<uint1> RenderUtil::m_vTranslucentSliceViewColors[16] =
{
  Triple<uint1>( 255,   0,   0 ), // Red
  Triple<uint1>(   0, 255,   0 ), // Green
  Triple<uint1>(   0,   0, 255 ), // Blue
  Triple<uint1>( 255, 255,   0 ), // Yellow
  Triple<uint1>( 255,   0, 255 ), // Magenta
  Triple<uint1>(   0, 255, 255 ), // Cyan
  Triple<uint1>( 102,  51,   0 ), // Brown
  Triple<uint1>( 255,  51,   0 ), // Orange
  Triple<uint1>( 128, 128, 128 ), // Grey
  Triple<uint1>( 128,   0,   0 ), // Half Red
  Triple<uint1>(   0, 128,   0 ), // Half Green
  Triple<uint1>(   0,   0, 128 ), // Half Blue
  Triple<uint1>( 128, 128,   0 ), // Half Yellow
  Triple<uint1>( 128,   0, 128 ), // Half Magenta
  Triple<uint1>(   0, 128, 128 ), // Half Cyan
  Triple<uint1>( 255, 255, 255 ), // White
};


/**
 * Constructor.
 */
RenderUtil::RenderUtil()
{
} // RenderUtil()



/**
 * Destructor.
 */
RenderUtil::~RenderUtil()
{
} // ~RenderUtil()


/**
 * Render a 3D arrow at some position, direction.
 */
void RenderUtil::RenderArrow( const Point<float4> & vPosition, const Normal<float4> & vDirection, const float4 & fLength,
                              const bool & bCentered, const int4 iMark, const PixelRGBA<float4> & color,
                              const bool & bExtraLights )
{
  GLUquadricObj * pQuadric = gluNewQuadric();
  
  // determine the proportions
  const float bigRadiusToLength = 0.3F;
  const float smallRadiusToLength = 0.15F;
  const float bigRadius = fLength * bigRadiusToLength;
  const float smallRadius = fLength * smallRadiusToLength;
  const float proportionHead = 0.5F;
  const float lengthHead = fLength * proportionHead;
  const float lengthTail = fLength * (1.0F - proportionHead);
  const unsigned int slices = 16; // accuracy of cylinder approximation
  
  GLfloat  whiteLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat  sourceLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat  light1Pos[] = { -128.0f, -128.0f, -128.0f, 1.0f };
  GLfloat  light2Pos[] = { 640.0f, 640.0f, 640.0f, 1.0f };
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glLightfv(GL_LIGHT2, GL_DIFFUSE, sourceLight);
  glLightfv(GL_LIGHT3, GL_POSITION, light1Pos);
  glEnable(GL_LIGHT2);
  
  if ( bExtraLights )
  {
    glLightfv(GL_LIGHT3, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT3, GL_POSITION, light2Pos);
    glEnable(GL_LIGHT3);
  }
  
  // render with transformations
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glPopMatrix();
  
  // SliceViewer
  if ( iMark == 3 ) 
  { 
    // bright blue.
    GLfloat matEmission[] = {0.3F, 0.0F, 0.5F, 0.0F};
    GLfloat matAmbientAndDiffuse[] = {0.05F, 0.82F, 0.70F, 1.0F};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    
  } 
  // bright blue.
  else 
  {
    GLfloat matEmission[] = {0.3F, 0.0F, 0.5F, 0.0F};
    GLfloat matDiffuse[] = {0.2F, 0.0F, 0.8F, 1.0F};
    GLfloat matAmbientAndDiffuse[] = {0.05F, 0.82F, 0.70F, 1.0F};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuse);

    if(iMark == -1) // used to indicate different color has been specified.
    {
      GLfloat matColor[] = {color.R(), color.G(), color.B(), color.A()};
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matColor);
    }
    else
    {
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    }
  }
  
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
    
    // transform to the proper position and direction
    glTranslatef(vPosition.m_x, vPosition.m_y, vPosition.m_z);
    const Normal<float> nArrowDirection(0,0,-1);
    const float4 degreesPerRadian = static_cast< float4 >( 180.0F / M_PI );
    // compute the angle within the x-y plane with x-axis 0 deg and y-axis 90 deg
    const float4 xyAngleRadians = atan2(vDirection.Y(), vDirection.X());
    // compute the angle it makes away from the z-axis
    const float4 zAxisAngleRadians = acos(nArrowDirection.Dot(vDirection));
    // first rotate in the x-y plane
    glRotatef(xyAngleRadians * degreesPerRadian, 0.0F, 0.0F, 1.0F);
    // then rotate away from the z-axis toward the x-axis
    glRotatef(-zAxisAngleRadians * degreesPerRadian, 0.0F, 1.0F, 0.0F);
    if (bCentered)
    {
      glTranslatef(0.0F, 0.0F, -fLength / 2.0F);
    }
    
    // render head cone
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, 0.0F);
      gluCylinder(pQuadric, 0.0F, bigRadius, lengthHead, slices, 1);
    } glPopMatrix();
    
    // make cone base
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, fLength - lengthHead);
      gluDisk(pQuadric, 0, bigRadius, slices, 1);
    } glPopMatrix();
    
    // make cylinder
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, lengthHead);
      gluCylinder(pQuadric, smallRadius, smallRadius, lengthTail, slices, 1);
    } glPopMatrix();
    
    // make end cap
    glPushMatrix();
    {
      glTranslatef(0.0F, 0.0F, fLength);
      gluDisk(pQuadric, 0, smallRadius, slices, 1);
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
} // RenderArrow()

  
/**
 * Draw the profile of a histogram using as a line graph.
 *
 * @param vHistogram   Histogram to render.
 * @param vDrawArea    Area in which to draw in the current OpenGL context.
 * @param vRGBA256     Colors for rendered histogram values.
 */
void RenderUtil::RenderHistogram(const std::vector<uint4> & vHistogram, const Rect<uint4> & vDrawArea,
                                 const std::vector<float4> & vRGB256)
{
  if (vRGB256.size() != 256*3)
  {
    return;
  }
  
  int4 i(0);
  // find the maximum
  uint4 uMax(0);
  const uint4 uSize(vHistogram.size());
  for (i=0; i<uSize; i++) 
  {
    if (vHistogram[i] > uMax) 
    {
      uMax = vHistogram[i];
    }
  }

  // early return if no data
  if (uMax == 0)
  {
    return;
  }

  // prepare OpenGL
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint id(0);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_1D, id);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, (void*) &vRGB256[0]);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  const float4 fOpacity(0.5F);
  glColor4f(1.0F, 1.0F, 1.0F, fOpacity);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_1D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // draw the profile area
  const float4 fHeight = 100.0F;
  glBegin(GL_QUAD_STRIP);
  {
    for (i=0; i<256; i++)
    {    
      float4 fPercentValue(float(i) / float(uSize));
      float4 fPositionX(fPercentValue * vDrawArea.Width() + vDrawArea.m_left);
			uint4 uIndex(Bound(static_cast<uint4>(0), static_cast<uint4>(fPercentValue * (uSize-1)), uSize-1));
      float4 fPositionY(fHeight * vHistogram[uIndex] / uMax);
      glTexCoord1f(fPercentValue);
      glVertex2f(fPositionX, 0.0F);
      glVertex2f(fPositionX, fPositionY);
    }
  } // glBegin
  glEnd();
  glDisable(GL_TEXTURE_1D);
  glDeleteTextures(1, &id);

  // draw the profile as a white line for clarity
  glBegin(GL_LINE_STRIP);
  {
    for (i=0; i<256; i++)
    {
      float4 fPercentValue(float(i) / float(uSize));
      float4 fPositionX(fPercentValue * vDrawArea.Width() + vDrawArea.m_left);
			uint4 uIndex(Bound(static_cast<uint4>(0), static_cast<uint4>(fPercentValue * (uSize-1)), uSize-1));
      float4 fPositionY(fHeight * vHistogram[uIndex] / uMax);
      glVertex2f(fPositionX, fPositionY);
    }
  } // glBegin
  glEnd();

  glDisable(GL_BLEND);

} // RenderHistogram()


/**
 * Draw the profile of a histogram as a line graph.
 *
 * @param vHistogram   Histogram to render.
 * @param vDrawArea    Area in which to draw in the current OpenGL context.
 * @param pRGBA256     Colors for rendered histogram values.
 */
void RenderUtil::RenderHistogram(const std::vector<uint4> & vHistogram, 
                                 const Rect<uint4> & vDrawArea,
                                 const float * pRGBA256)
{
  int4 i(0);
  // find the maximum
  uint4 uMax(0);
  const uint4 uSize(vHistogram.size());
  for (i=0; i<uSize; i++) 
  {
    if (vHistogram[i] > uMax) 
    {
      uMax = vHistogram[i];
    }
  }

  // early return if no data
  if (uMax == 0)
  {
    return;
  }

  // prepare OpenGL
  GLubyte * pData = new GLubyte[3*256];
  for (int4 i3(0), i4(0); i3<256*3; i3+=3, i4+=4)
  {
    pData[i3 + 0] = pRGBA256[i4 + 0];
    pData[i3 + 1] = pRGBA256[i4 + 1];
    pData[i3 + 2] = pRGBA256[i4 + 2];
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint id(0);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_1D, id);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, pData);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  delete[] pData;

  const float4 fOpacity(0.5F);
  glColor4f(1.0F, 1.0F, 1.0F, fOpacity);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_1D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // draw the profile area
  const float4 fHeight = 100.0F;
  glBegin(GL_QUAD_STRIP);
  {
    for (i=0; i<256; i++)
    {    
      float4 fPercentValue(float(i) / float(uSize));
      float4 fPositionX(fPercentValue * vDrawArea.Width() + vDrawArea.m_left);
			uint4 uIndex(Bound(static_cast<uint4>(0), static_cast<uint4>(fPercentValue * (uSize-1)), uSize-1));
      float4 fPositionY(fHeight * vHistogram[uIndex] / uMax);
      glTexCoord1f(fPercentValue);
      glVertex2f(fPositionX, 0.0F);
      glVertex2f(fPositionX, fPositionY);
    }
  } // glBegin
  glEnd();
  glDisable(GL_TEXTURE_1D);
  glDeleteTextures(1, &id);

  // draw the profile as a white line for clarity
  glBegin(GL_LINE_STRIP);
  {
    for (i=0; i<256; i++)
    {
      float4 fPercentValue(float(i) / float(uSize));
      float4 fPositionX(fPercentValue * vDrawArea.Width() + vDrawArea.m_left);
			uint4 uIndex(Bound(static_cast<uint4>(0), static_cast<uint4>(fPercentValue * (uSize-1)), uSize-1));
      float4 fPositionY(fHeight * vHistogram[uIndex] / uMax);
      glVertex2f(fPositionX, fPositionY);
    }
  } // glBegin
  glEnd();

  glDisable(GL_BLEND);

} // RenderHistogram()


/*
 * Draw a histogram as bars.
 *
 * @param vHistogram        
 * @param vDrawArea         
 * @param transferFunction  The transfer funtion.
 * @param GLFontGeorge      
 * @param minMaxPairHU      
 */
void RenderUtil::RenderHistogramBars(std::vector<uint4> & vHistogram, const Rect<uint4> & vDrawArea,
                                     TransferFunctionVC & transferFunction, GLFontGeorge & font,
                                     const std::pair<uint4, uint4> & minMaxPairHU)
{
  // early return if no data
  if (vHistogram.size() == 0)
    return;
  
  // find the maximum
  std::vector<uint4>::iterator maxPosIter = std::max_element(vHistogram.begin(), vHistogram.end());
  
  // prepare OpenGL
  //transferFunction.PrepareOpenGLTextureEnvironment();
  const float4 fOpacity(0.5F);
  const Triple<float4> colorTriple(1.0F, 1.0F, 1.0F);
  glColor4f(colorTriple.m_x, colorTriple.m_y, colorTriple.m_z, fOpacity);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // prepare to draw the profile area
  const float4 fPercentBarWidth(0.9F); // the width of the bar as a percentage of the max bar width
  const float4 fPercentHeight(0.3F); // the maximum height of the bars as a percentage of the given drawing area
  const float4 fBarWidth(fPercentBarWidth * float4(vDrawArea.Width()) / float4(vHistogram.size()));
  const float4 fRaiseBottom(70.0F); // raise the bottom of the graph up so we can display x axis

  // loop over all the bins
  for (int i(0); i<vHistogram.size(); i++)
  {   
    const float4 fPercentValue(float(i) / float(vHistogram.size()));
    const float4 fPositionMidX(fPercentValue * vDrawArea.Width() + vDrawArea.GetMinX());
    const float4 fPositionMinX(fPositionMidX - fBarWidth * 0.5F);
    const float4 fPositionMaxX(fPositionMidX + fBarWidth * 0.5F);
    const float4 fPositionMinY(vDrawArea.GetMinY() + fRaiseBottom);
    const float4 fPositionMaxY(Interpolate(0.0F, float4(vHistogram[i]), float4(*maxPosIter), float4(vDrawArea.GetMinY() + fRaiseBottom), float4(vDrawArea.GetMaxY()) * fPercentHeight));
    const float4 fTextureMin(Interpolate(float4(vDrawArea.GetMinX()), fPositionMinX, float4(vDrawArea.GetMaxX()), 0.0F, 1.0F));
    const float4 fTextureMax(Interpolate(float4(vDrawArea.GetMinX()), fPositionMaxX, float4(vDrawArea.GetMaxX()), 0.0F, 1.0F));
    
    // fill the bars
    glEnable(GL_TEXTURE_1D);
    glBegin(GL_QUADS);
    {
      glTexCoord1f(fTextureMin);
      glVertex2f(fPositionMinX, fPositionMinY);
      glVertex2f(fPositionMinX, fPositionMaxY);
      glTexCoord1f(fTextureMax);
      glVertex2f(fPositionMaxX, fPositionMaxY);
      glVertex2f(fPositionMaxX, fPositionMinY);
    }
    glEnd();
    glDisable(GL_TEXTURE_1D);
    
    // draw the outline of the bars
    glBegin(GL_LINE_STRIP);
    {
      glVertex2f(fPositionMinX, fPositionMinY);
      glVertex2f(fPositionMinX, fPositionMaxY);
      glVertex2f(fPositionMaxX, fPositionMaxY);
      glVertex2f(fPositionMaxX, fPositionMinY);
    }
    glEnd();
    
    // draw the number of voxels above the bar
    font.PutFormattedString(ToAscii(vHistogram[i]), GLFontGeorge::ALIGN_CENTER, Point2D<int4>(fPositionMidX, fPositionMaxY + 10), colorTriple);
    
  } // for all bins

  // draw the min and max values

  // draw the left side tick mark
  const float4 fInset(20.0F);
  glBegin(GL_LINE_STRIP);
  {
    glColor3f(1.0F, 1.0F, 1.0F);
    glVertex2f(vDrawArea.GetMinX()         , vDrawArea.GetMinY() + fRaiseBottom         );
    glVertex2f(vDrawArea.GetMinX()         , vDrawArea.GetMinY() + fRaiseBottom - fInset);
    glVertex2f(vDrawArea.GetMinX() + fInset, vDrawArea.GetMinY() + fRaiseBottom - fInset);
  }
  glEnd();

  // draw the right side tick mark
  glBegin(GL_LINE_STRIP);
  {
    glColor3f(1.0F, 1.0F, 1.0F);
    glVertex2f(vDrawArea.GetMaxX()         , vDrawArea.GetMinY() + fRaiseBottom         );
    glVertex2f(vDrawArea.GetMaxX()         , vDrawArea.GetMinY() + fRaiseBottom - fInset);
    glVertex2f(vDrawArea.GetMaxX() - fInset, vDrawArea.GetMinY() + fRaiseBottom - fInset);
  }
  glEnd();

  // draw the min value
  const float4 fVerticalCenter(4.0F);
  font.PutFormattedString(ToAscii(minMaxPairHU.first), GLFontGeorge::ALIGN_LEFT,
    Point2D<int4>(vDrawArea.GetMinX() + fInset, vDrawArea.GetMinY() + fRaiseBottom - fInset - fVerticalCenter), colorTriple);

  // draw the max value
  font.PutFormattedString(ToAscii(minMaxPairHU.second), GLFontGeorge::ALIGN_RIGHT,
    Point2D<int4>(vDrawArea.GetMaxX() - fInset, vDrawArea.GetMinY() + fRaiseBottom - fInset - fVerticalCenter), colorTriple);
  
  glDisable(GL_BLEND);
} // RenderHistogramBars()


/* 
 * Draw a Rhombus.
 *
 * @param center const Point2D<T>
 * @param radius const T &
 * @param fLineWidth
 * @param bFill const bool
 */
void RenderUtil::GLDrawRhombus( const Point2D<float4> &center, const float4 &radius, const float4 & fLineWidth, const bool &bFill )
{
  // make a rumbus
  static Point2D<float4> vOrigPts[4];
  
  vOrigPts[0] = Point2D<float4>(0.0F, 0.0F);
  vOrigPts[1] = Point2D<float4>(-radius, radius);
  vOrigPts[2] = Point2D<float4>(0.0F, 2.0F*radius);
  vOrigPts[3] = Point2D<float4>(radius, radius);

  glPushMatrix();
  glTranslated(center.m_x, center.m_y-radius, 0);

  if (!bFill)	
  {
    glLineWidth( fLineWidth );
  }
  
  glBegin(bFill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
  
  /// drawing dark line of rhombus
  for (int4 i=0; i<=3; i++)
  {
	  glVertex2f( vOrigPts[i].m_x, vOrigPts[i].m_y );
  }
	glVertex2f( 0.0F, 0.0F );

  glColor3ub( 255, 255, 255);
  /// draw the white diagonal line
	glVertex2f( 0.0F, 0.0F );
  glVertex2f( 0.0F, 2.0F*radius );
  glVertex2f( 0.0F, radius );
	glVertex2f( -radius, radius );
	glVertex2f( radius, radius );
  glVertex2f( 0.0F, radius );
	glVertex2f( 0.0F, 0.0F );

  glEnd();
  glPopMatrix();
} // GLDrawRhombus()


/* 
 * Draw a circle.
 *
 * @param center const Point2D<T>
 * @param radius const T &
 * @param bFill const bool
 */
void RenderUtil::GLDrawCircle(const Point2D<float4> &center, const float4 &radius, const bool &bFill)
{
  // make a circle
  const int4 iCircleDiv = 8;		
  const int4 iCircleDivx2 = iCircleDiv*2;
  static Point2D<float4> vOrigPts[iCircleDiv*2+1];
  static bool bInited = false;
  if (!bInited)
  {
    int4 iCt = 0;
    const float8 fPI_VALUE = M_PI;
	  for (float8 d = -fPI_VALUE; d <= fPI_VALUE; d+=fPI_VALUE/iCircleDiv)
	  {
		  vOrigPts[iCt++] = Point2D<float4>(static_cast<float4>(sin(d)), static_cast<float4>(cos(d)));
	  }
	  bInited = true;
  } // end if (!bInited)

  // Store a couple of arrays of points to reduce duplicated multiplications
  static const int4 iNumStored = 5;		
  static int4 iNextStore = 0;
  static int4 viLastSize[iNumStored] = {0,0,0};
  static Point2D<float4> mCurrPts[iNumStored][iCircleDiv*2+1];

  int4 iWhich=iNumStored-1;
  for (; iWhich>=0; iWhich--)
  {
	  if ( (static_cast<float4>(radius * 1000.0f)) != viLastSize[iWhich])
    {
      continue;
    }
  } // end for

  if (iWhich < 0)
  {
    iWhich = iNextStore;
	  iNextStore = (iNextStore+1) % iNumStored;
	  for (int4 i=0; i<=iCircleDiv*2; i++)
	  {
  	  mCurrPts[iWhich][i] = vOrigPts[i] * radius;
  	}
	  viLastSize[iWhich] = (int4) (radius * 1000.0f);
  } // end if(iWhich < 0)

  glPushMatrix();
  glTranslated(center.m_x, center.m_y, 0);

  if (!bFill)	
  {
    glLineWidth(radius * 0.25f);
  }
  
  glBegin(bFill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
  
  for (int4 i=0; i<=iCircleDivx2; i++)
  {
	  glVertex2f(mCurrPts[iWhich][i].m_x, mCurrPts[iWhich][i].m_y);
  }
  glEnd();
  glPopMatrix();
} // GLDrawCircle()


/**
 * Draw a  point
 * @param   point2D
 * @param   color
 * @param   colorBorder
 * @param   fSize
 * @param   fSizeBorder
 */
void RenderUtil::GLDrawPointWithBorder2f(const Point2D<float4> & point2D, 
                                         const PixelRGBA<float4> & color,
                                         const PixelRGBA<float4> & colorBorder,
                                         const float4 & fSize, 
                                         const float4 & fSizeBorder)
//*******************************************************************
{
  GLDrawPoint2f(point2D, colorBorder, fSize+fSizeBorder);
  GLDrawPoint2f(point2D, color, fSize);
} // GLDrawPointWithBorder2f()



/**
 * InvertColor. Simple helper method for drawing a border that sticks out.
 * @param color
 * @return inverted color
 */
PixelRGBA<float4> RenderUtil::InvertColor(const PixelRGBA<float4> & color)
//*******************************************************************
{
  return PixelRGBA<float4>(1.0, 1.0F-0.5*color.G(), 1.0F-0.5*color.B(), color.A());
} // InvertColor( color )



/**
 * GLDrawPoint
 * -----------
 * Public function to draw a point.
 *
 * @param   point2D 
 * @param   color 
 * @param   fSize 
 */
void RenderUtil::GLDrawPoint2f(const Point2D<float4> & point2D, 
                               const PixelRGBA<float4> & color, 
                               const float4 & fSize)
//*******************************************************************
{
  glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT);
    glPointSize(fSize);
    glColor3f(color.R(),color.G(),color.B());
    glBegin(GL_POINTS);
    {
      GlVertex2f(point2D);
    }
    glEnd();
  glPopAttrib();
} // end GLDrawPoint2f().



/**
 * GLDrawPoint
 * -----------
 * Public function to draw a point.
 *
 * @param   point 
 * @param   color 
 * @param   fSize 
 */
void RenderUtil::GLDrawPoint(const Point<float4> & point, 
                             const PixelRGBA<float4> & color, 
                             const float4 & fSize)
//*******************************************************************
{
  glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT);
    glColor3f(color.R(),color.G(),color.B());
    glPointSize(fSize);
    glBegin(GL_POINTS);
    {
      GlVertex(point);
    }
    glEnd();
  glPopAttrib();
} // end GLDrawPoint().



/**
 * GLDrawAxis
 * ----------
 * Public function to draw a single polygon.
 *
 * @param origin - (x,y,z) of origin.
 * @param uLineWidth - Thickness of axis lines.
 * @param uLineLength - Length of axis lines.
 */
void RenderUtil::GLDrawAxis(const Point<uint2> & origin, const uint2 & uLineWidth, const uint2 & uLineLength)
{
  glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glLineWidth(uLineWidth);

    glColor3f(1,0,0);
    glBegin(GL_LINES);
    {
      glVertex3f( origin.m_x, origin.m_y, origin.m_z);
      glVertex3f(uLineLength, origin.m_y, origin.m_z);
    }
    glEnd();

    glColor3f(0,1,0);
    glBegin(GL_LINES);
    {
      glVertex3f(origin.m_x,  origin.m_y, origin.m_z);
      glVertex3f(origin.m_x, uLineLength, origin.m_z);
    }
    glEnd();

    glColor3f(0,0,1);
    glBegin(GL_LINES);
    {
      glVertex3f(origin.m_x, origin.m_y,  origin.m_z);
      glVertex3f(origin.m_x, origin.m_y, uLineLength);
    }
    glEnd();
  glPopAttrib();
} // end GLDrawAxis().


/**
 * GLDrawAxis
 * ----------
 * Public function to draw a single polygon.
 *
 * @param box - (x,y,z) of box corners.
 * @param color - RGB of color.
 */
void RenderUtil::GLDrawBox( Box<float4> & box, const PixelRGBA<float4> & color ) //TexMap3dBrick & brick)
//*******************************************************************
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
}


/**
 * gets a matrix to convert from world to volume coordinates
 *
 * @param header the volume header
 *
 * @return a conversion matrix
 */
Matrix<float4> RenderUtil::GetWorldToVolumeMatrix( const vxVolumeHeader & header )
{

  // if you later need volume to world...
  //Matrix<float4> volumeToWorld;
  //volumeToWorld.Scale( Vector<float4>( header.GetVoxelUnits().m_x, header.GetVoxelUnits().m_y, header.GetVoxelUnits().m_z ) );
  //return volumeToWorld;

  Matrix<float4> worldToVolume;
  worldToVolume.Scale( Vector<float4>( 1.0F / header.GetVoxelUnits().m_x,
                                       1.0F / header.GetVoxelUnits().m_y,
                                       1.0F / header.GetVoxelUnits().m_z ) );
  return worldToVolume;

} // GetWorldToVolumeMatrix()


/**
 * is a particular finding visible
 *
 * @param finding               the finding
 * @param cadFindInfo           map of cad finding session information
 * @param iCadSliderThreshold   display threshold value
 * @param bShowExternal         whether or not extra-colonic findings should be displayed
 * @return                      whether the finding is visible
 */
bool RenderUtil::IsFindingVisible( const vxCadFinding & finding, const ColonSession::FindingInfoMap & cadFindInfo,
                                   const int4 iCadSliderThreshold, const bool bShowExternal)
{
  // try to find the finding
  ColonSession::FindingInfoMap::const_iterator it = cadFindInfo.find( finding.GetId() );
  if ( it == cadFindInfo.end() )
  {
    // could not find it...bad
    LogErr( "finding not in map", "RenderUtil", "IsFindingVisible" );
    return false;
  }

  // check whether it is within the lumen and/or should be visible
  if ( !it->second.GetInLumen() && !bShowExternal)
  {
    return false;
  }

  // check the sphericity
  if (iCadSliderThreshold >= 0)
  {
    // backward compatibility check
    std::map< std::string, std::string >::const_iterator varIter = finding.GetVendorSpecificVariablesMap().find( "Sphericity" );
    if ( varIter != finding.GetVendorSpecificVariablesMap().end() )
    {
      const int4 iSphericity = atoi( varIter->second.c_str() );
      return ! ( iSphericity < iCadSliderThreshold );
    }
    else
    {
      // check the sphericity

      // version 1.1 check (even though the file still says version 1.0
      static const std::string sSph("Sphericity");
      for ( varIter = finding.GetVendorSpecificVariablesMap().begin(); varIter != finding.GetVendorSpecificVariablesMap().end(); varIter++ )
      {

        if (sSph.compare(0, sSph.length(), varIter->first, 0, sSph.length()) != 0)
          continue;

        const int4 iSphericity = atoi( varIter->second.c_str() );
        if ( iSphericity == iCadSliderThreshold )
        {
          return true;
        }
      }

    }

    return false;
  } // if cad threshold is valid

  return true;

} // IsFindingVisible()


/**
 * Get the decision setting for a cad finding
 *
 * @param finding       the finding
 * @param cadFindInfo   the map of visibilities for the cad findings
 * @param eDecide       decision value to test against
 * @return              whether the finding has been marked with the given decision
 */
ColonSession::FindingInfo::DecisionEnum RenderUtil::GetFindingDecision( const vxCadFinding & finding, const ColonSession::FindingInfoMap & cadFindInfo )
{
  // try to find the finding
  std::map< std::string, ColonSession::FindingInfo >::const_iterator it = cadFindInfo.find( finding.GetId() );
  if ( it == cadFindInfo.end() )
  {
    // could not find it...bad
    LogErr( "finding not in map", "RenderUtil", "IsFindingVisible" );
    return ColonSession::FindingInfo::eDecisionUndecided;
  }
  else
  {
    return it->second.GetDecision();
  }
} // GetDecision()



// undefines
#undef FILE_REVISION


// $Log: RenderUtil.C,v $
// Revision 1.14.2.3  2010/02/08 22:52:34  dongqing
// Draw the location indicator rhombus line thinner and add
// diagnoal line with white color
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.14.2.2  2010/01/29 22:54:39  dongqing
// add GLDrawRhombus
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.14.2.1  2009/01/15 04:13:03  jmeade
// const modifier on parameter variable
//
// Revision 1.14  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.13  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.12  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.11  2007/02/06 16:44:29  jmeade
// fixes in showcadfinding.
//
// Revision 1.10  2006/10/02 20:01:55  frank
// removed compile warning
//
// Revision 1.9  2006/09/26 19:34:30  frank
// replaced hardcoded numbers with constants
//
// Revision 1.8  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.7  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.9  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.4.2.8.2.1  2007/01/18 22:04:45  jmeade
// reflecting changes to medisight cad.
//
// Revision 1.4.2.8  2006/03/29 14:17:16  frank
// name change for finding info property
//
// Revision 1.4.2.7  2006/03/28 20:22:14  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.4.2.6  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.4.2.5  2006/03/23 18:43:09  frank
// working on hiding CAD findings outside of the lumen
// still needs one more piece to allow the user to decide whether to show/not
//
// Revision 1.4.2.4  2006/03/22 20:53:44  frank
// moved the finding visibility and other information into a class
//
// Revision 1.4.2.3  2006/01/07 04:33:23  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.4.2.2  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.4.2.1  2005/12/12 14:59:15  frank
// Issue #4561: Changed 2D arrows to circles
//
// Revision 1.4  2005/09/29 18:30:34  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.3  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.2  2005/09/23 03:19:39  vxconfig
// working on display of cad findings
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.29.2.2  2004/03/22 21:46:19  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.29.2.1  2003/04/22 18:18:52  jmeade
// Issue 3123: RenderHistogram override with (safer) std::vector<> for colormap data.
//
// Revision 3.29  2003/02/05 22:31:53  jmeade
// Push/pop attribute stack in some functions where needed.
//
// Revision 3.28  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 3.27  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 3.26  2002/12/19 20:44:43  ingmar
// PixelRGBA member data is now private -> need to use access functions
//
// Revision 3.25  2002/11/14 16:57:05  ingmar
// fixed color of box
//
// Revision 3.24  2002/09/26 01:17:08  michael
// Added code to "invert" a color which sticks out if drawn on top of something with the original color
//
// Revision 3.23  2002/09/25 20:37:24  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 3.22  2002/09/19 14:25:25  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 3.21  2002/09/09 21:31:13  frank
// Cleaning out dead code.
//
// Revision 3.20  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 3.19  2002/06/04 18:37:11  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.18  2002/06/03 19:14:32  manju
// Reduced the height of histogram being displaye for the annotations.
//
// Revision 3.17  2002/05/10 14:05:23  manju
// Changed display height of linear annotation profile to 100.
//
// Revision 3.16  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.15  2002/05/01 19:43:52  michael
// cosmetics
//
// Revision 3.14  2002/05/01 00:00:12  michael
// Commented out SetAgatson helper function ... to be revisited later
//
// Revision 3.13  2002/04/26 19:52:47  jmeade
// Code standards.
//
// Revision 3.12  2002/04/23 15:27:25  jenny
// switched to vxBlockVolume<int2>
//
// Revision 3.11  2002/04/23 15:02:39  jenny
// switched to vxBlockVolume<int2>
//
// Revision 3.10  2002/04/08 20:26:03  manju
// Removed the logger statement from RenderHistogram.
//
// Revision 3.9  2002/04/08 13:55:30  manju
// Using TransferFunction instead of TransferFunctionVC in RenderHistogram.
// The function has been modified accordingly.
// Added a new method to draw profile for linear annotation and moved
// SetAgatston method here.
//
// Revision 3.8  2002/03/14 22:54:01  jaddonisio
// New functions:
//  i. GLDrawPoint
//  ii. GLDrawBox
// and placed DrawPolygon back in vxRenderTexMap
// because it had multitexture specific things.
//
// Revision 3.7  2002/03/13 20:03:28  jaddonisio
// New drawing utilities.
//
// Revision 3.6  2002/03/11 20:06:44  michael
// formatting
//
// Revision 3.5  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.4  2002/02/12 19:39:09  frank
// Added min/max histogram limits.
//
// Revision 3.3  2002/02/06 00:48:07  frank
// Improved the histogram drawing.
//
// Revision 3.2.2.3  2002/05/29 18:20:27  jmeade
// Enable alpha blending of RenderArrow color (though fn doesn't yet support this).
//
// Revision 3.2.2.2  2002/03/07 19:53:55  binli
// moved the global GL render function GLDrawCircle() from 10-math::Point2D to RenderUtil
//
// Revision 3.2.2.1  2002/02/12 23:20:18  frank
// Added a new histogram drawing function.
//
// Revision 3.2  2001/11/13 22:15:58  manju
// Added color to RenderArrow.
//
// Revision 3.1  2001/11/02 19:44:28  jenny
// worked on RenderHistogram()
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 09 2001 10:48:58   manju
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:50:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:18  ingmar
// new directory structure
//
// Revision 1.6  2001/07/30 16:28:38  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.5  2001/07/03 19:14:23  liwei
// Removed the inclusion of stdafx.h
//
// Revision 1.4  2001/06/29 15:36:15  jmeade
// fixed a divide by zero (line 217) with better determination of a horizontal line; coding conventions
//
// Revision 1.3  2001/06/27 18:51:18  geconomos
// no message
//
// Revision 1.2  2001/05/29 17:27:00  jmeade
// slice viewer dbg colors
//
// Revision 1.1  2001/05/15 17:50:53  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/RenderUtil.C,v 1.14.2.3 2010/02/08 22:52:34 dongqing Exp $
// $Id: RenderUtil.C,v 1.14.2.3 2010/02/08 22:52:34 dongqing Exp $
