// $Id: vxRendererSoftwareMPROblique.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSoftwareMPROblique.h"
#include "vxEnvironment.h"
#include "vxViewObliqueData.h"
#include "Quadrilateral.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeRayIterator.h"
#include "Vector4d.h"
#include "vxRendererUtils.h"
#include "vxMathUtils.h"
#include "vxRenderingMode.h"
#include "vxIntensityConverter.h"


// defines
#define FILE_REVISION "$Revision: 1.16 $"


/**
 * Constructor
 * @param environment the environment
 */
vxRendererSoftwareMPROblique::vxRendererSoftwareMPROblique( vxEnvironment & environment )
:vxRendererSoftwareMPR( environment )
//*******************************************************************
{
}


/**
 * Determines the image size based on the orientation of the PlaneOrtho
 * @return the image size
 */ 
Triple<uint2> vxRendererSoftwareMPROblique::GetImageSize()
//*******************************************************************
{
  const int2 iMinScreenSize(Min(GetEnvironment().GetWinRect().GetViewportWidth(),
                                GetEnvironment().GetWinRect().GetViewportHeight()));
  const int2 iImageSize(GetNextPowerOfTwo(iMinScreenSize));

  return Triple<uint4>(iImageSize, iImageSize, 0);
} // GetImageSize()


/**
 * Pre-render routine
 * @return 1 upon success
 */
int4 vxRendererSoftwareMPROblique::PreRender()
//*******************************************************************
{ 
  try
  {
  //LogDbg("vxRendererSoftwareMPROblique: the following caused this: " + GetEnvironment().GetModifiedElementsString(), "", "");

  SetRenderingNecessary(false);

  // call based class
  vxRendererSoftwareMPR::PreRender();

  // start setup
  ////////////////  
  Triple<uint2> desiredImageSize(GetImageSize());
  Triple<uint4> actualImageSize(GetImage().GetDim());
  if (desiredImageSize[0] != actualImageSize[0] || desiredImageSize[1] != actualImageSize[1])
  {
    GetImage().Resize(Triple<uint4>(desiredImageSize[0], desiredImageSize[1], desiredImageSize[2]));
  }

  // has the camera changed?
  if ( GetEnvironment().IsModified( vxEnvironment::CAMERA ) == true )
  {
    ComputeFourCornerPoints();
    SetRenderingNecessary( true );
  }

  // has the rendering mode changed?
  if (GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE))
    SetRenderingNecessary(true);

  // has the interactive segmentation object changed AND are we showing it?
  if (GetEnvironment().IsModified(vxEnvironment::INTERACTIVE_SEGMENTATION) == true &&
      GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE))
  {
    SetRenderingNecessary(true);
  }

  // has the crop box changed?
  if (GetEnvironment().IsModified(vxEnvironment::CROP_BOX) == true)
    SetRenderingNecessary(true);

  // has the oblique view data changed?
  if (GetEnvironment().IsModified(vxEnvironment::PLANE_MPR) == true)
  {
    ComputeFourCornerPoints();
    SetRenderingNecessary(true);
  }
  
  // has the win rect changed?
  if (GetEnvironment().IsModified(vxEnvironment::WIN_RECT) == true)
  {
    ComputeFourCornerPoints();
    SetRenderingNecessary(true);
  }

  // has the win rect changed?
  if (GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR) == true)
  {
    SetRenderingNecessary(true);
  }
  }
  catch (...)
  {
    LogDbg("PreRender bad","","");
  }
  return 1;
} // PreRender()


/** 
 * Computes the four corner points from the camera and the plane oblique
 * @return true if successful
 */
bool vxRendererSoftwareMPROblique::ComputeFourCornerPoints()
//*******************************************************************
{
  bool bResult(true);
  try
  {
  const Triple<float> & units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());

  for (int i(0); i<4; ++i)
  {
    int x(GetEnvironment().GetWinRect().GetReferencePoint().first);
    int y(GetEnvironment().GetWinRect().GetReferencePoint().second);
    if (i == 2 || i == 3) x += GetEnvironment().GetWinRect().GetViewportWidth()-1; // add width  if on right
    if (i == 1 || i == 2) y += GetEnvironment().GetWinRect().GetViewportHeight()-1; // add height if on top

    Point<float4> worldPos;
    Point2D<int2> screenPos(x, y);
    bResult &= GetEnvironment().GetViewerType()->ConvertScreenToWorld(screenPos, worldPos, false);
    m_cornerIntersection[i] = worldPos;

    // convert to volume integer (index) coordinates
    m_cornerIntersection[i].m_x /= units.m_x;
    m_cornerIntersection[i].m_y /= units.m_y;
    m_cornerIntersection[i].m_z /= units.m_z;
  }
/*
  vxPlaneOblique<float4> plane(*GetEnvironment().GetPlaneOblique()); //normal, Point3Df(m_cornerIntersection[0][0], m_cornerIntersection[0][1], m_cornerIntersection[0][2]));
  Box<float4> box(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())));
  float4 fMin(fabs(plane.GetDistance(box.GetCorner(0))));
  for (int4 i(1); i<8; i++)
  {
    const float4 fMinLocal(fabs(plane.GetDistance(box.GetCorner(i))));
    if (fMinLocal < fMin)
      fMin = fMinLocal;
  }
  LogDbg("Min is: " + ToAscii(fMin), "", "");
*/
  }
    catch (...)
  {
    LogDbg("PreRender bad","","");
  }

  return bResult;
} // ComputeFourCornerPoints()


/**
 * Render a sub-window of the image (could be multi-threaded)
 * @param window the window to render
 * @return 1 if successful
 */
int4 vxRendererSoftwareMPROblique::RenderWindow( const BoundingRect<uint2> & window, const uint1 & uThreadID )
{
  try
  {
  if (GetRenderingNecessary() == false)
  {
    return 1;
  }

  // declare these outside the loop
  ///////////////////////////////////
  Point<uint2> vPixelPosition(0, 0, 0);
  uint1 * pImagePixels = reinterpret_cast< uint1 * >( GetImage().GetDataPtr() );

  // set up the ray iterators
  RaySegment raySegment( m_cornerIntersection[1], m_cornerIntersection[2] );
  Vector4Df sampleIncrementVec( raySegment.GetDirection() );
  sampleIncrementVec /= float4( window.GetMaxX() - window.GetMinX() );
  vxBlockVolumeRayIterator<uint2> iterRayIntensity( * GetEnvironment().GetIntensityVolume(), raySegment, sampleIncrementVec );
  vxBlockVolumeRayIterator<uint2> iterRayLabel   ( * GetEnvironment().GetLabelVolume(), raySegment, sampleIncrementVec );
  vxBlockVolumeRayIterator<uint2> iterRayIntensityInner( * GetEnvironment().GetIntensityVolume(), raySegment, sampleIncrementVec );

  // the components in convenient sorted order
  const std::vector< vxComponent * > & vpComponentInfo(GetEnvironment().GetComponents()->ArrayByLabel());

  // some variables used in the inner loop
  uint1 uRed(0), uGreen(0), uBlue(0), uAlpha(0);
  const Vector3D<int4> size(GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim());

  // set up the crop box
  const Triplei & dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  const Triplef & units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  Box<float4> visibleBox;
  if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) )
  {
    const Triplef worldToVolume( 1 / units.m_x, 1 / units.m_y, 1 / units.m_z );
    visibleBox.SetMinPoint( GetEnvironment().GetCropBox()->GetBox().GetMinPoint().GetScaledAnIso( worldToVolume ) );
    visibleBox.SetMaxPoint( GetEnvironment().GetCropBox()->GetBox().GetMaxPoint().GetScaledAnIso( worldToVolume ) );
  }
  else
  {
    visibleBox = Box<float4>( Point3Df( 0, 0, 0 ), Point3Df( dim.m_x, dim.m_y, dim.m_z ) );
  }

  // loop over all the image pixels
  for ( vPixelPosition.m_y = window.GetMinY();
        vPixelPosition.m_y < window.GetMaxY();
        ++vPixelPosition.m_y )
  {
    
    // setup each scanline - first interpolate the scanline ends
    const float4 dv( float4( vPixelPosition.m_y ) / ( GetImageSize().m_y - 1 ) );
    const Point3Df leftPoint ( m_cornerIntersection[0] + ( m_cornerIntersection[1] - m_cornerIntersection[0] ) * dv );
    const Point3Df rightPoint( m_cornerIntersection[3] + ( m_cornerIntersection[2] - m_cornerIntersection[3] ) * dv );
    
    // initialize the ray iterators
    RaySegment scanlineRaySegment( leftPoint, rightPoint );
    iterRayIntensity.SetRaySegment( scanlineRaySegment );
    iterRayLabel    .SetRaySegment( scanlineRaySegment );

    // loop over all the pixels
    vPixelPosition.m_x = window.GetMinX();

    for ( ; vPixelPosition.m_x < window.GetMaxX(); ++vPixelPosition.m_x, ++iterRayIntensity, ++iterRayLabel )
    {
      const Vector3D<int4> position(iterRayIntensity.GetPosInt());

      // filter pixels outside the volume
      if ( IsInsideBox( iterRayIntensity, visibleBox ) == false )
      {
        uint1 * pPixels( pImagePixels + 4 * ( vPixelPosition.m_y * GetImage().GetDim().m_x + vPixelPosition.m_x ) );
        *pPixels++ = GetEnvironment().GetBackgroundColor()->R();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->G();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->B();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->A();
      }
      else
      {
        break;
      }
    }

    const float * pfRGBA(GetTablePointerRGBA());

    for ( ; IsInsideBox( iterRayIntensity, visibleBox ) && vPixelPosition.m_x < window.GetMaxX(); ++vPixelPosition.m_x, ++iterRayIntensity, ++iterRayLabel )
    {
      /////////////////////////////////////////////////////////////////
      // AT THIS POINT, THERE IS NO THIN SLAB MODE IMPLEMENTED!!!!!!!
      /////////////////////////////////////////////////////////////////

      // sample the intensity (single slice mode by default)
      uint4 uIntensity(iterRayIntensity.GetSample());
      uIntensity = Bound( uint4(0), uIntensity, uint4(4095) );

      // if MIP mode then scan trough slab for maximum sample value
      const bool bMipMode( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP );
      if ( bMipMode )
      {
        const Vector3Df viewDirWorld( GetEnvironment().GetCamera()->GetViewDirection() );
        const float4 fSlabThickness( GetEnvironment().GetRenderingMode()->GetSlabThickness() );
        const Vector3Df sampleVectorWorld( viewDirWorld * fSlabThickness );
        const Vector3Df sampleVectorVoxels( sampleVectorWorld.GetScaledAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );
        Point3Df start( Point3Df( iterRayIntensity.GetPosFloat() ) - sampleVectorVoxels / 2 );
        Point3Df end  ( Point3Df( iterRayIntensity.GetPosFloat() ) + sampleVectorVoxels / 2 );
        RaySegment innerRaySegment( start, end );
        iterRayIntensityInner.SetRaySegment( innerRaySegment );
        const float4 fIntersampleDistance( 0.5F );
        Vector4Df raySampleIncVec( innerRaySegment.GetDirection().GetNormalizedXYZ() * fIntersampleDistance );
        iterRayIntensityInner.SetSampleIncrementVector( raySampleIncVec );

        uint4 uMax(0);
        while ( iterRayIntensityInner.IsAtEnd() == false )
        {
          if ( iterRayIntensityInner.IsInsideVolume() )
          {
            uint4 uSample = iterRayIntensityInner.GetSample();
            if ( uSample > uMax )
              uMax = uSample;
          }
          ++iterRayIntensityInner;
        }
        uIntensity = uMax;
      }

      // apply the transfer function
      const bool bShowColor(GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR));
      if (bShowColor == true)
      {
        // color
        const float4 * pColorTableEntry(pfRGBA + 4 * uIntensity);
        uRed   = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
        uGreen = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
        uBlue  = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
        uAlpha = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
      }
      else
      {
        // grayscale
        uRed = uGreen = uBlue = 255;
        uAlpha = (uint2)(pfRGBA[4 * uIntensity + 3]) >> GetNumberOfShiftBitsFor8bitAlpha();
      }

      // get a pointer to the pixels
      uint1 * pPixels( pImagePixels + 4 * ( vPixelPosition.m_y * GetImage().GetDim().m_x + vPixelPosition.m_x ) );

      // check for a visible label
      const bool bLabelsVisible( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );
      if (bLabelsVisible == true)
      {
        // sample the label volume
        uint2 uLabel(iterRayLabel.GetSampleNearest());

        // hide hidden density
        if (vpComponentInfo[uLabel]->IsIntensityVisible() == false)
        {
          uAlpha = 0;
        }

        // possibly blend label color into the sample
        else if (vpComponentInfo[ uLabel ]->IsLabelVisible())
        {
          // blend the label at this sample
          PixelRGBA<uint1> pixelColor(vpComponentInfo[ uLabel ]->GetColor());
          uRed   = vxRendererUtils::InterpolateFixedPoint(uRed  , pixelColor.R(), pixelColor.A());
          uGreen = vxRendererUtils::InterpolateFixedPoint(uGreen, pixelColor.G(), pixelColor.A());
          uBlue  = vxRendererUtils::InterpolateFixedPoint(uBlue , pixelColor.B(), pixelColor.A());
        }

      } // if labels used

      // possibly blend intensity range into sample
      const bool bShowThresholds(GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE));

      /// this part is not used in VC codes. This file is only for VC purpose. So, for performance speed up
      /// I set the Low and High as fixed number here. Dongqing 2007/04/26
      const uint4 uThresholdLow(1); //GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow());
      const uint4 uThresholdHigh(8000); //GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh());

      if (bShowThresholds == true && uIntensity >= uThresholdLow && uIntensity <= uThresholdHigh )
      {
        uRed   = vxRendererUtils::InterpolateFixedPoint( uRed  , 0  , 128 );
        uGreen = vxRendererUtils::InterpolateFixedPoint( uGreen, 255, 128 );
        uBlue  = vxRendererUtils::InterpolateFixedPoint( uBlue , 0  , 128 );
      }

      // write out the pixel RGB
      *pPixels++ = uRed;
      *pPixels++ = uGreen;
      *pPixels++ = uBlue;
      *pPixels++ = uAlpha;

    } // for all u

    // filter pixels outside the volume
    for ( ; vPixelPosition.m_x < window.GetMaxX(); ++vPixelPosition.m_x, ++iterRayIntensity, ++iterRayLabel )
    {
      if ( IsInsideBox( iterRayIntensity, visibleBox ) == false )
      {
        uint1 * pPixels(pImagePixels + 4 * (vPixelPosition.m_y * GetImage().GetDim().m_x + vPixelPosition.m_x));
        *pPixels++ = GetEnvironment().GetBackgroundColor()->R();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->G();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->B();
        *pPixels++ = GetEnvironment().GetBackgroundColor()->A();
      }
      else
      {
        // something is wrong
        LogDbg("Error, we are outside volume but get inside sample", "vxRendererSoftwareMPROblique", "RenderWindow");
        break;
      }
    }

  } // for all v
}
catch (...)
{
  LogDbg("Oblique CRAPPED OUT","","");
}
  return 1;
} // RenderWindow()


/**
 * helper function to determine if the ray position is within a box
 *
 * @param iter the ray iterator
 * @param box the box to test against
 *
 * @return if ray position is inside the box
 */
bool vxRendererSoftwareMPROblique::IsInsideBox( vxBlockVolumeRayIterator<uint2> & iter, Box<float> & box ) const
{
  const Point3Df & boxMin( box.GetMinPoint() );
  const Point3Df & boxMax( box.GetMaxPoint() );
  const Vector4Df pos( iter.GetPosFloat() );
  return
    pos.m_x >= boxMin.m_x && pos.m_x <= boxMax.m_x &&
    pos.m_y >= boxMin.m_y && pos.m_y <= boxMax.m_y &&
    pos.m_z >= boxMin.m_z && pos.m_z <= boxMax.m_z;
} // IsInsideBox()


/**
 * Draw the image stored in the texture tile
 */
void vxRendererSoftwareMPROblique::DrawTexture()
//*******************************************************************
{
  try
  {
  // Simply set the OpenGL texture environment and draw the slice
  GLuint id(GetTextureTile().GetDensityTextureId());
  glBindTexture(GL_TEXTURE_2D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // GL draw textured tile:
  glColor3f(1, 1, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS); 

  // loop over four corners
  for (int i(0); i<4; i++)
  {
    Point2D<float4> texCoord;
    texCoord.m_x = 0.0F + ((i == 2 || i == 3) ? 1.0F : 0.0F);
    texCoord.m_y = 0.0F + ((i == 1 || i == 2) ? 1.0F : 0.0F);
    glTexCoord2f(texCoord[0], texCoord[1]);

    const Triple<float4> & units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    Point<float4> point(m_cornerIntersection[i].m_x * units.m_x,
                        m_cornerIntersection[i].m_y * units.m_y,
                        m_cornerIntersection[i].m_z * units.m_z);
    glVertex3f(point[0], point[1], point[2]);
  }

  glEnd();
  glDisable(GL_TEXTURE_2D);

  }
  catch (...)
  {
    LogDbg("DrawTexture bad","","");
  }

} // DrawTexture()


// undefines
#undef FILE_REVISION


// $Log: vxRendererSoftwareMPROblique.C,v $
// Revision 1.16  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.15.2.1  2007/04/26 21:16:22  dongqing
// Reset the intensityLow and intensityHigh as constant.
// No no longer use both InteractiveSegmentation bounds and
// vxIntensityConverter bounds. This speed up a lot on oblique view
//
// Revision 1.15  2007/02/13 22:32:57  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.14  2006/04/20 12:32:54  geconomos
// using instance vairables instead of indexers when accessing corner points
//
// Revision 1.13  2006/03/27 19:33:48  gdavidson
// Readded
//
// Revision 1.11  2006/02/09 14:08:16  geconomos
// 64-bit conditionals
//
// Revision 1.10  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8  2005/07/14 20:16:13  vxconfig
// no longer need special compile for VC
//
// Revision 1.7.2.1  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.7  2005/03/12 18:32:51  michael
// Added switch for VC to deliver values with opacity = 255 and color values
// already opacity adjusted.
//
// Revision 1.6  2004/10/05 14:33:53  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.5  2004/05/19 20:24:44  frank
// fixed opacity blending
//
// Revision 1.4  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.3  2004/04/22 13:39:40  geconomos
// Added temporary ( I hope  ) vc dll preprocessor conditions
//
// Revision 1.2  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.67  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.66.2.1  2004/02/20 17:46:14  michael
// since oblique creates a full viewer image, pixels outside the volume need to
// be filled with background color and not black!!!
//
// Revision 1.66  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.65  2004/01/14 14:18:20  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.64  2003/11/20 21:10:03  frank
// Fixed problem with the segmentation range not being displayed
//
// Revision 1.63  2003/11/10 21:04:19  frank
// Added MIP mode
//
// Revision 1.62  2003/11/06 02:58:04  michael
// Fixed display of slice number and number of total slices for the oblique MPR,
// added text to be displayed if after rotation the slice is outside the volume,
// and added mechanism so that axis aligned obliques can reach the very first
// and very last slice/facs of volume (numerical precision problem).
//
// Revision 1.61  2003/11/04 15:48:10  michael
// crop box needs to be checked precisely, not +1 in the size!!!
//
// Revision 1.60  2003/09/25 15:13:06  frank
// Expanded the crop box to show the top slice
//
// Revision 1.59  2003/09/25 13:32:47  frank
// Added crop box handling in oblique view
//
// Revision 1.58  2003/09/25 03:06:35  frank
// Merged oblique views into one
//
// Revision 1.34  2003/09/04 17:29:45  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.33  2003/08/29 11:01:10  michael
// Changed first to second since this was a copy and paste error, noticed by Dongqing
//
// Revision 1.32  2003/08/27 14:35:43  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.31  2003/08/20 15:41:51  michael
// merged with branch to get the improvements in that prevent the multiple
// images when being zoomed far off
//
// Revision 1.30  2003/08/20 14:11:35  michael
// removed empty for loop
//
// Revision 1.29.2.2  2003/08/11 19:17:14  michael
// fixed issue #3261
//
// Revision 1.29.2.1  2003/08/11 19:08:16  michael
// fixed issue #3261
//
// Revision 1.29  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.28  2003/07/15 12:55:38  frank
// Issue #3372: Added intensity range
//
// Revision 1.27  2003/06/18 15:19:59  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.26  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.25  2003/05/29 17:03:40  michael
// took our redundant debug code and added timing for rendering to be logged
//
// Revision 1.24  2003/05/28 16:13:30  michael
// fixed issue 3261
//
// Revision 1.23  2003/05/27 14:12:00  michael
// completed code review
//
// Revision 1.22  2003/05/23 15:48:31  michael
// minor changes
//
// Revision 1.21  2003/05/23 14:12:52  frank
// code review complete
//
// Revision 1.20  2003/05/23 13:43:48  frank
// recalled version 1.17 - some problems rendering in 1.19
//
// Revision 1.19  2003/05/22 13:16:27  frank
// code review
//
// Revision 1.18  2003/05/21 18:26:04  frank
// code review
//
// Revision 1.17  2003/05/21 17:38:52  frank
// Back to original projection
//
// Revision 1.16  2003/05/21 17:38:05  frank
// Example of manual projection instead of OpenGL corner point projection
//
// Revision 1.15  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.14  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.13  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.12  2003/04/24 13:59:49  michael
// added newline at end (better when merging)
//
// Revision 1.11  2003/03/21 17:12:48  frank
// Issue #2936: Improved resolution of oblique rendering
//
// Revision 1.10  2003/02/28 19:42:53  frank
// Added label support
//
// Revision 1.9  2003/01/22 22:04:49  ingmar
// dimension and units are now Triples
//
// Revision 1.8  2002/12/17 18:43:47  frank
// Moved oblique corner computation into shared area so others can use it.
//
// Revision 1.7  2002/12/09 15:32:30  michael
// Added comments and code clean-up
//
// Revision 1.6  2002/12/06 21:29:00  michael
// removed old fashioned Set/GetRGBATablePtr(); it was only copy of what is in
// the colormap in the environment.
//
// Revision 1.5  2002/11/28 02:31:30  michael
// switched off multi-threading to avoid crash ... Revisit after RSNA 2002
//
// Revision 1.4  2002/11/26 18:12:35  ingmar
// now using Vector4d<float4> in the rayIterator
//
// Revision 1.3  2002/11/22 17:24:30  michael
// really drawing the quad in 3D
//
// Revision 1.2  2002/11/20 21:15:10  michael
// removed warnings
//
// Revision 1.1  2002/11/19 21:25:15  michael
// added initial version of this renderer
//
// Revision 1.27  2002/11/14 16:33:04  michael
// changed vxCamera::GetRightDirection() to vxCamera::GetViewRight()
//
// Revision 1.26  2002/11/13 22:08:45  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.25  2002/11/13 20:11:36  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.24  2002/11/13 01:09:10  frank
// Removed a dead variable.
//
// Revision 1.23  2002/11/12 16:16:46  frank
// Used Quadrilateral from helper function.
//
// Revision 1.22  2002/11/08 17:50:58  frank
// Rendered from Quadrilateral
//
// Revision 1.21  2002/11/05 19:54:12  frank
// Created helper function for producing the Quadrilateral for the oblique slice plane.
//
// Revision 1.20  2002/10/23 22:03:30  ingmar
// removed references that VC++6.0 does not like
//
// Revision 1.19  2002/10/23 22:02:14  frank
// ext dll is so much faster...
//
// Revision 1.18  2002/10/23 21:44:33  frank
// Rendering works, but slowly
//
// Revision 1.17  2002/10/18 21:05:34  frank
// Don't want stdafx.h
//
// Revision 1.16  2002/10/18 20:31:04  frank
// Improving speed.
//
// Revision 1.15  2002/10/17 16:14:49  frank
// Removed Quadrilateral template parameter.
//
// Revision 1.14  2002/10/17 15:34:40  frank
// Cleaning up oblique renderer.
//
// Revision 1.13  2002/09/25 20:36:42  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.12  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.11  2002/09/05 22:37:47  jaddonisio
// New vxComponent.
//
// Revision 1.10  2002/08/09 15:39:55  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.9  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.8  2002/05/04 10:46:20  michael
// cosmetics, removed old commented out code
//
// Revision 1.7  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.6  2002/05/01 00:04:13  michael
// Use state of rendering mode instead of environment
//
// Revision 1.5  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.4  2002/04/25 01:39:48  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 1.3  2002/04/24 17:36:35  jenny
// vxBlockVolume stuff
//
// Revision 1.2  2002/04/24 15:08:52  jenny
// vxBlockVolume stuff
//
// Revision 1.1  2002/04/23 21:39:14  jenny
// new software .C file
//
// Revision 1.3  2002/04/23 15:41:28  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.2  2002/04/11 14:38:30  michael
// added missing private copy constructor and assignment operator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPROblique.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxRendererSoftwareMPROblique.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
