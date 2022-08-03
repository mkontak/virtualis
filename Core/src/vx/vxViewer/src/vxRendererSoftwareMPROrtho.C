// $Id: vxRendererSoftwareMPROrtho.C,v 1.21 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSoftwareMPROrtho.h"
#include "vxBlockVolumeIterator.h"
#include "vxPlaneOrtho.h"
#include "vxUtils.h"
#include "Timer.h"
#include "vxComponentArray.h"
#include "vxCropBox.h"
#include "vxRendererUtils.h"
#include "vxProgressBar.h"
#include "ImageUtilities.h"
#include "vxViewerTypes.h"
#include "vxMathUtils.h"
#include "vxBlockVisibilityTester.h"
#include "vxRenderingMode.h"
#include "vxIntensityConverter.h"


// defines
#define FILE_REVISION "$Revision: 1.21 $"


/**
 * Default constructor
 * @param environment the environment
 */
vxRendererSoftwareMPROrtho::vxRendererSoftwareMPROrtho(vxEnvironment & environment)
:vxRendererSoftwareMPR(environment)
{
} // vxRendererSoftwareMPROrtho()


/**
 * Determines the image size based on the orientation of the PlaneOrtho
 */ 
Triple<uint2> vxRendererSoftwareMPROrtho::GetImageSize()
{

  uint2 uImageWidth(0);
  uint2 uImageHeight(0);
  uint2 uNumberOfImages(0);

  // Compute the slice width, height and total number
  vxPlaneOrtho<float4> * pPlaneOrtho = dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho());
  if (pPlaneOrtho != NULL)
  {
    uint4 uIndex(pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord());
    const Vector3D<int4> dim(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
    uImageWidth     = dim[(uIndex+1) % 3];
    uImageHeight    = dim[(uIndex+2) % 3];
    uNumberOfImages = dim[uIndex];
  }

  return Triple<uint2>(uImageWidth, uImageHeight, uNumberOfImages);

} // GetImageSize()


/**
 * Pre-Render routine
 */
int4 vxRendererSoftwareMPROrtho::PreRender()
{

  SetRenderingNecessary(false);

  // call base class
  vxRendererSoftwareMPR::PreRender();

  // test if all data is available that is 
  // necessary and if not, throw an exception
  /////////////////////////////////////////////
  if (dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho()) == NULL)
  {
    throw ex::AbortOperationException( LogRec( "GetEnvironment().GetPlaneOrtho() is not a PlaneOrtho<float4> *",
                                               "vxRendererSoftwareMPR",
                                               "PreRender()" ) );
  }

  // in case this renderer instance got added to the main view and the pan/zoom
  // (otherwise it would not be in the pan/zoom), simply render generated texture
  /////////////////////////////////////////////////////////////////////////////////
  if (dynamic_cast< vxViewerTypeMPROrthoPanZoom * >(GetEnvironment().GetViewerType()))
  {
    SetRenderingNecessary(false);
    return 1;
  }

  // prepare the image dimensions so that it is a power of two and at least as big as the rendering tile size
  Triple<uint2> desiredImageSize( GetImageSize() );
  const uint4 uMinImageSize( 16 );
  desiredImageSize[0] = Max( uMinImageSize, static_cast< uint4 >( GetNextPowerOfTwo( desiredImageSize[0] ) ) );
  desiredImageSize[1] = Max( uMinImageSize, static_cast< uint4 >( GetNextPowerOfTwo( desiredImageSize[1] ) ) );
  Triple<uint4> actualImageSize( GetImage().GetDim() );

  if ( desiredImageSize[0] != actualImageSize[0] || desiredImageSize[1] != actualImageSize[1] )
  {
    GetImage().Resize( Triple<uint4>( desiredImageSize[0], desiredImageSize[1], desiredImageSize[2] ) );
  }

  // has the rendering mode changed?
  if ( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) )
  {
    SetRenderingNecessary( true );
  }

  // has the crop box changed?
  if ( GetEnvironment().IsModified( vxEnvironment::CROP_BOX ) == true )
  {
    SetRenderingNecessary( true );
  }
  
  // has the win rect changed?
  if ( GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) == true )
    SetRenderingNecessary( true );

  // has the interactive segmentation object changed AND are we showing it?
  if ( GetEnvironment().IsModified( vxEnvironment::INTERACTIVE_SEGMENTATION ) == true &&
       GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) )
  {
    SetRenderingNecessary( true );
  }

  return 1;

} // PreRender()


/**
 * Constructor for the rendering environment
 */
vxRendererSoftwareMPROrtho::RenderEnvironment::RenderEnvironment(vxEnvironment & environment):
m_iterVol( * environment.GetIntensityVolume() ),
m_iterLabel( * environment.GetLabelVolume() )
{

  m_bShowColor      = environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR );
  m_bShowLabels     = environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );
  m_bShowThresholds = environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE );
  m_pPlaneOrtho     = dynamic_cast< vxPlaneOrtho<float4> * >( environment.GetPlaneOrtho() );
  m_uAxisIndex      = m_pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord();
  m_uThresholdLow   = 1200;
  m_uThresholdHigh  = 1400;

  if (m_bShowThresholds == true)
  {
    if ( environment.GetInteractiveSegmentation() != NULL )
    {
      m_uThresholdLow  = vxIntensityConverter::ModalityToRaw( environment.GetInteractiveSegmentation()->GetRegionIntensityLow(), 
        environment.GetIntensityVolume()->GetHeader() );
        
      m_uThresholdHigh = vxIntensityConverter::ModalityToRaw( environment.GetInteractiveSegmentation()->GetRegionIntensityHigh(),
        environment.GetIntensityVolume()->GetHeader() );
    }
  }
  m_pfRGBA = environment.GetColormap()->GetTablePointerRGBA( environment.GetIntensityVolume()->GetHeader() );
  m_pvComponents = & environment.GetComponents()->ArrayByLabel();
  m_volDim = environment.GetIntensityVolume()->GetHeader().GetVolDim();
  m_volUnits = environment.GetIntensityVolume()->GetHeader().GetVoxelUnits();
  m_iNumSlicesInSlab = environment.GetRenderingMode()->GetSlabThickness() / m_volUnits[m_uAxisIndex];
  m_bLabelVolumeAvailable = environment.GetLabelVolume() != NULL;
  m_bNeedLabels = environment.GetComponents()->AreAnyLabelsVisible() || environment.GetComponents()->AreAnyIntensitiesInvisible();

} // RenderEnvironment()

  
/**
 * Render a subset of the image
 *
 * @param window    rectangle within viewport to be rendered 
 * (assumes that the reactangle coordinates correspond to volume coordinates)
 * @param uThreadID the thread ID
 * @return int4     unused
 */
int4 vxRendererSoftwareMPROrtho::RenderWindow(const BoundingRect<uint2> & window, 
                                              const uint1 & uThreadID)
{

  // exit early if ther is no volume loaded
  if ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().X() < 3 )
    return 1;

  // set up the rendering environment
  RenderEnvironment renderEnv( GetEnvironment() );
  renderEnv.m_pImagePixels = reinterpret_cast< uint1 * >( const_cast< PixelRGBA<uint1> * >( GetImage().GetDataPtr() ) );

  renderEnv.m_renderWindow = GetCropBoxWindow( window, renderEnv.m_uAxisIndex ); // does consider crop box
  ExpandWindowToMultipleOfBlockSize( renderEnv.m_renderWindow, renderEnv.m_uBlockSize );

  // get current slab thickness
  renderEnv.m_iPlaneOrthoDistance = vxUtils::CalculateOffsetDistance(renderEnv.m_pPlaneOrtho->GetDistance() / renderEnv.m_volUnits[ renderEnv.m_uAxisIndex ] + 0.5F,
                                                                     renderEnv.m_volDim[renderEnv.m_uAxisIndex], 
                                                                     GetEnvironment().GetOffset(), 
                                                                     GetEnvironment().GetMaximumOffset());
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));
  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox = GetEnvironment().GetCropBox();
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= renderEnv.m_volUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= renderEnv.m_volUnits[renderEnv.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);
    // ensure that nothing is rendered if outside the crop box
    if (minMaxSlicePair.first > int4(fMaxCorner[renderEnv.m_uAxisIndex]))
      return 1;
    if (minMaxSlicePair.second < int4(fMinCorner[renderEnv.m_uAxisIndex]))
      return 1;
    renderEnv.m_minMaxSlicePair = minMaxSlicePair;
  }

  // select the proper renderer
  if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP)
  {
    if (renderEnv.m_bShowColor || (renderEnv.m_bShowLabels && renderEnv.m_bNeedLabels && renderEnv.m_bLabelVolumeAvailable))
      RenderMIP(renderEnv);
    else
      RenderMIPBlock(renderEnv);
  }
  else if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB)
  {
    if ( renderEnv.m_bShowColor || renderEnv.m_bShowThresholds ||
         ( renderEnv.m_bShowLabels && renderEnv.m_bNeedLabels && renderEnv.m_bLabelVolumeAvailable ) )
      RenderThinSlabBlockWithLabelVoxel(renderEnv);
    else
      RenderThinSlabBlock(renderEnv);
  }
  else if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::XRAY)
  {
    RenderXRay(renderEnv);
  }
  else
  {
    RenderSingleSlice(renderEnv);
  }

  return 1;

} // RenderWindow()


/**
 * Call this function to create image of single slice
 *
 * @param renderEnv the rendering environment
 * @return returns 1 if anything was rendered
 */
void vxRendererSoftwareMPROrtho::RenderSingleSlice(RenderEnvironment & renderEnv)
{

  // make sure we are trying to render inside the crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    Vector3D<float4> minCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> maxCorner(pCropBox->GetBox().GetMaxPoint());
    minCorner[renderEnv.m_uAxisIndex] /= renderEnv.m_volUnits[renderEnv.m_uAxisIndex];
    maxCorner[renderEnv.m_uAxisIndex] /= renderEnv.m_volUnits[renderEnv.m_uAxisIndex];
    // do not draw the slice if outside bounding box
    if ( renderEnv.m_iPlaneOrthoDistance < minCorner[renderEnv.m_uAxisIndex] ||
         renderEnv.m_iPlaneOrthoDistance > maxCorner[renderEnv.m_uAxisIndex] )
    {
      return;
    }
  }

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());
  uint4 uPixelOffset(4 * (overallImageSize[0]*renderEnv.m_renderWindow.GetMinY() + renderEnv.m_renderWindow.GetMinX()));

  // declare these outside the loop
  Vector3D<int4> vVoxelPosition(0,0,0);
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4 uRed(0), uGreen(0), uBlue(0), uAlpha(0);
  const bool bLabelVolumeAvailable(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetLabelVolume()) != NULL);
  Image<PixelRGBA<uint1> > & image(GetImage());

  // setup for visible block acceleration
  vxBlockVisibilityTester blockVisibility(GetEnvironment());

  // for all rows of blocks
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      vPixelPosition.m_x = vBlockPosition.m_x;
      vPixelPosition.m_y = vBlockPosition.m_y;
      vVoxelPosition[renderEnv.m_uAxisIndex]         = renderEnv.m_iPlaneOrthoDistance;
      vVoxelPosition[(renderEnv.m_uAxisIndex+1) % 3] = vPixelPosition.m_x;
      vVoxelPosition[(renderEnv.m_uAxisIndex+2) % 3] = vPixelPosition.m_y;
      renderEnv.m_iterVol.SetPos(vVoxelPosition);

      // see if we need to render this block
      if (blockVisibility.IsBlockVisible(renderEnv.m_iterVol.GetPos()) ||
         blockVisibility.IsBlockVisibleSegmentationRange(renderEnv.m_iterVol.GetPos()))
      {
        // for all rows in block
        for (uint4 y(0); y<renderEnv.m_uBlockSize && y+vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); ++y)
        {
          vPixelPosition.m_x = vBlockPosition.m_x;
          vPixelPosition.m_y = vBlockPosition.m_y + y;
          vVoxelPosition[renderEnv.m_uAxisIndex]         = renderEnv.m_iPlaneOrthoDistance;
          vVoxelPosition[(renderEnv.m_uAxisIndex+1) % 3] = vPixelPosition.m_x;
          vVoxelPosition[(renderEnv.m_uAxisIndex+2) % 3] = vPixelPosition.m_y;        
          renderEnv.m_iterVol.SetPos(vVoxelPosition);
          renderEnv.m_iterLabel.SetPos(vVoxelPosition);

          // initialize the index to the start of the next scan line
          // (this is needed for multi-threading where we use screen space subdivision)
          uint4 uPixelOffset(4 * (overallImageSize[0]*vPixelPosition.m_y + vPixelPosition.m_x));

          // for all columns in block
          for (uint4 x(0); x<renderEnv.m_uBlockSize && x+vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); ++x)
          {
            vPixelPosition.m_x = vBlockPosition.m_x + x;

            const int4 iIntensity(renderEnv.m_iterVol.GetVoxel());            
            const uint4 uIndex(iIntensity);
              
            if (renderEnv.m_bShowColor == true)
            {
              const float4 * pColorTableEntry(&renderEnv.m_pfRGBA[4*uIndex]);
              uRed   = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
              uGreen = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
              uBlue  = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
              uAlpha = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
            }
            else
            {
              uRed = uGreen = uBlue = 255;
              uAlpha = (uint2)(renderEnv.m_pfRGBA[4*uIndex+3]) >> GetNumberOfShiftBitsFor8bitAlpha();
            }
            // is there a tag volume?
            bool bComponentDensityVisible(true);

            if (renderEnv.m_bShowLabels && renderEnv.m_bNeedLabels && renderEnv.m_bLabelVolumeAvailable)
            {
              // Get the label value of this voxel
              // Mark the CT by the component colors
              uint2 uLabelVoxel(renderEnv.m_iterLabel.GetVoxel());

              bComponentDensityVisible = (* renderEnv.m_pvComponents)[uLabelVoxel]->IsIntensityVisible();
              if (bComponentDensityVisible == false)
              {
                uRed = uGreen = uBlue = 0;
                uAlpha = 0;
              }
              else if ((* renderEnv.m_pvComponents)[uLabelVoxel]->IsLabelVisible())
              {
                PixelRGBA<uint1> pixelColor((* renderEnv.m_pvComponents)[uLabelVoxel]->GetColor());
                uRed   = vxRendererUtils::InterpolateFixedPoint(uRed  , pixelColor.R(), pixelColor.A());
                uGreen = vxRendererUtils::InterpolateFixedPoint(uGreen, pixelColor.G(), pixelColor.A());
                uBlue  = vxRendererUtils::InterpolateFixedPoint(uBlue , pixelColor.B(), pixelColor.A());
              } // if component LabelVisible

              // increment the label pointer
              switch (renderEnv.m_uAxisIndex)
              {
              case vxUtils::SAGITTAL: renderEnv.m_iterLabel.NextXZY(); break;
              case vxUtils::CORONAL:  renderEnv.m_iterLabel.NextYXZ(); break;
              case vxUtils::AXIAL:    renderEnv.m_iterLabel.NextZYX(); break;
              default: break; // should never happen!
              }

            } // if no tag volume
            // is there a segmentation range to be overlayed?
            if (bComponentDensityVisible == true && renderEnv.m_bShowThresholds == true &&
                iIntensity >= renderEnv.m_uThresholdLow && iIntensity <= renderEnv.m_uThresholdHigh)
            {
              const uint4 iNewR(0);
              const uint4 iNewG(255);
              const uint4 iNewB(0);
              const uint1 uBlendFraction(128);
              uRed   = vxRendererUtils::InterpolateFixedPoint(uRed  , iNewR, uBlendFraction);
              uGreen = vxRendererUtils::InterpolateFixedPoint(uGreen, iNewG, uBlendFraction);
              uBlue  = vxRendererUtils::InterpolateFixedPoint(uBlue , iNewB, uBlendFraction);
            }

            const bool bShowDebug(false);
            if (bShowDebug)
            {
              if (x%16==0 || y%16==0) uBlue = 255;
              if ((x-1)%16<4 && (y-1)%16<4 &&(blockVisibility.IsBlockVisible         (vVoxelPosition))) uRed   = 255;
              if ((x-5)%16<4 && (y-5)%16<4 &&(blockVisibility.IsBlockVisibleIntensity(vVoxelPosition))) uGreen = 255;
              if ((x-9)%16<4 && (y-9)%16<4 &&(blockVisibility.IsBlockVisibleLabel    (vVoxelPosition))) uBlue  = 255;
            }
            // increment the pointers
            switch (renderEnv.m_uAxisIndex)
            {
            case vxUtils::SAGITTAL:
              renderEnv.m_pImagePixels[uPixelOffset++] = uRed;
              renderEnv.m_pImagePixels[uPixelOffset++] = uGreen;
              renderEnv.m_pImagePixels[uPixelOffset++] = uBlue;
              renderEnv.m_pImagePixels[uPixelOffset++] = uAlpha;
              renderEnv.m_iterVol.IncY();
              break;
            case vxUtils::CORONAL:
              image.SetPixelRaw(Point2D<uint2>(vPixelPosition.m_x, vPixelPosition.m_y), PixelRGBA<uint1>(uRed, uGreen, uBlue, uAlpha));
              renderEnv.m_iterVol.IncZ();
              break;
            case vxUtils::AXIAL:
              renderEnv.m_pImagePixels[uPixelOffset++] = uRed;
              renderEnv.m_pImagePixels[uPixelOffset++] = uGreen;
              renderEnv.m_pImagePixels[uPixelOffset++] = uBlue;
              renderEnv.m_pImagePixels[uPixelOffset++] = uAlpha;
              renderEnv.m_iterVol.IncX();
              break;
            default:
              break; // should never happen!
            }
          } // for x in block
        }  // for y in block
      } // if block visible
      else
      {
        // this is filling the block with background color
        int4 color(0);
        if (renderEnv.m_bShowColor && (!renderEnv.m_bShowLabels || blockVisibility.IsBlockVisibleLabel(renderEnv.m_iterVol.GetPos())))
        {
          const int4 iIntensity(renderEnv.m_iterVol.GetVoxel());
          const uint4 uIndex(iIntensity);
          float4 * pfColorTableEntry(&renderEnv.m_pfRGBA[4*uIndex]);
          uint1 * pColorChannel((uint1*) (&color));
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
        }

        // block is not visible
        // for all rows in block
        for (uint4 y(0); y<renderEnv.m_uBlockSize; ++y)
        {
          vPixelPosition.m_x = vBlockPosition.m_x;
          vPixelPosition.m_y = vBlockPosition.m_y + y;

          // initialize the index to the start of the next scan line within block
          uint4 uPixelOffset(4 * (overallImageSize[0]*vPixelPosition.m_y + vPixelPosition.m_x));

          // fill block tile with solid color

          int4 * pPixel((int4*)(renderEnv.m_pImagePixels + uPixelOffset));
          int4 * pLastPixel(pPixel + renderEnv.m_uBlockSize);
          for (; pPixel<pLastPixel; ++pPixel)
          {
            *pPixel = color;
          }
        } // loop over rows in block
      } // block is not visible
    } // for x blocks
  } // for y blocks

} // RenderSingleSlice()


/**
 * Call this function to create image of MIP rendering.
 * @param rendering environment
 * @return int4
 */
void vxRendererSoftwareMPROrtho::RenderMIP(RenderEnvironment & renderEnv)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());

  // get current slab thickness
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    const Vector3D<float4> fVoxelUnits(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ renderEnv.m_uAxisIndex ] ) )
      return;
    if ( minMaxSlicePair.second < int4( fMinCorner[ renderEnv.m_uAxisIndex ] ) )
      return;
  } // show crop box?

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  PixelRGBA<float4> pixelValue[4];

  // for all rows of blocks
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+1)%3] - vBlockPosition.m_x, renderEnv.m_uBlockSize));
      const int4 uThisBlockSizeY(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+2)%3] - vBlockPosition.m_y, renderEnv.m_uBlockSize));

      // for all rows in block
      for (uint4 y(0); y<uThisBlockSizeY; y+=2)
      {
        vPixelPosition.m_y = vBlockPosition.m_y + y;
        vPixelPosition.m_x = vBlockPosition.m_x;

        // initialize the index to the start of the next scan line
        // (this is needed for multi-threading where we use screen space subdivision)
        uint4 uPixelOffset(4 * (overallImageSize.m_x*vPixelPosition.m_y + vPixelPosition.m_x));
        uint4 uNextLinePixelOffset(uPixelOffset +  4 * overallImageSize.m_x);

        // for all columns in block
        for (uint4 x(0); x<uThisBlockSizeX; x+=2)
        {
          vPixelPosition.m_x = vBlockPosition.m_x + x;

          MIPComposition_4Rays(vPixelPosition.m_x, vPixelPosition.m_y, minMaxSlicePair, renderEnv, pixelValue);

          // write the pixel
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].R();
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].G();
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].B();
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].A();
 
          if ((x+1)<uThisBlockSizeX)
          {
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].R();
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].G();
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].B();
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].A();
          }

          if ((y+1)<uThisBlockSizeY)
          {
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].R();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].G();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].B();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].A();

            if ((x+1)<uThisBlockSizeX)
            {
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].R();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].G();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].B();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].A();
            }
          }
        } // for x in block
      } // for y in block
    } // end for all blocks in scanline
  }  // endfor all v

  // set up new slab of rendering
  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderMIP()


/**
 * Call this function to create an x-ray rendered image
 * @param rendering environment
 * @return int4
 */
int4 vxRendererSoftwareMPROrtho::RenderXRay(RenderEnvironment & renderEnv)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    const Vector3D<float4> fVoxelUnits(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ renderEnv.m_uAxisIndex ] ) )
      return 1;
    if ( minMaxSlicePair.second < int4( fMinCorner[ renderEnv.m_uAxisIndex ] ) )
      return 1;
  } // show crop box?

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  PixelRGBA<float4> pixelValue[4];

  // for all rows of blocks
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+1)%3] - vBlockPosition.m_x, renderEnv.m_uBlockSize));
      const int4 uThisBlockSizeY(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+2)%3] - vBlockPosition.m_y, renderEnv.m_uBlockSize));

      // for all rows in block
      for (uint4 y(0); y<uThisBlockSizeY; y+=2)
      {
        vPixelPosition.m_y = vBlockPosition.m_y + y;
        vPixelPosition.m_x = vBlockPosition.m_x;

        // initialize the index to the start of the next scan line
        // (this is needed for multi-threading where we use screen space subdivision)
        uint4 uPixelOffset(4 * (overallImageSize.m_x*vPixelPosition.m_y + vPixelPosition.m_x));
        uint4 uNextLinePixelOffset(uPixelOffset +  4 * overallImageSize.m_x);

        // for all columns in block
        for (uint4 x(0); x<uThisBlockSizeX; x+=2)
        {
          vPixelPosition.m_x = vBlockPosition.m_x + x;

          XRayComposite_4Rays(vPixelPosition.m_x, vPixelPosition.m_y, minMaxSlicePair, renderEnv, pixelValue);

          // write the pixel
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].R();
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].G();
          renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].B();
          renderEnv.m_pImagePixels[uPixelOffset++] = 255;

          if ((x+1)<uThisBlockSizeX)
          {
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].R();
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].G();
            renderEnv.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].B();
            renderEnv.m_pImagePixels[uPixelOffset++] = 255;
          }

          if ((y+1)<uThisBlockSizeY)
          {
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].R();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].G();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].B();
            renderEnv.m_pImagePixels[uNextLinePixelOffset++] = 255;

            if ((x+1)<uThisBlockSizeX)
            {
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].R();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].G();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].B();
              renderEnv.m_pImagePixels[uNextLinePixelOffset++] = 255;
            }
          }
        } // for x in block
      } // for y in block
    } // end for all blocks in scanline
  }  // endfor all v

  // set up new slab of rendering.
  SetMinMaxSlicePair(minMaxSlicePair);

  return 1;

} // RenderXRay()


/**
 * Call this function to create image of MIP rendering
 * @param renderEnv the rendering environment
 */
void vxRendererSoftwareMPROrtho::RenderMIPBlock( RenderEnvironment & renderEnv )
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    const Vector3D<float4> fVoxelUnits(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ renderEnv.m_uAxisIndex ] ) )
      return;
    if ( minMaxSlicePair.second < int4( fMinCorner[ renderEnv.m_uAxisIndex ] ) )
      return;
  } // show crop box?

  // first and last blocks
  std::pair<int4, int4> MinMaxBlock(minMaxSlicePair.first>>4, minMaxSlicePair.second>>4);

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  const uint4 iNewG(255);
  const float fAlpha(0.5F);
  const float4 fOneMinusAlpha(1.0F - fAlpha);
  int4 iZPos[renderEnv.m_uBlockSize*renderEnv.m_uBlockSize];
  int4 iMaxIntensity[renderEnv.m_uBlockSize*renderEnv.m_uBlockSize];
  int4 iMaxPosition[renderEnv.m_uBlockSize*renderEnv.m_uBlockSize];

  for (int4 y(0); y<renderEnv.m_uBlockSize*renderEnv.m_uBlockSize; y++)
  {
    iZPos[y] = minMaxSlicePair.second - minMaxSlicePair.first + 1;
    iMaxIntensity[y] = 0;
    iMaxPosition[y]  = 0;
  }

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch (renderEnv.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    iXStep = renderEnv.m_uBlockSize;
    iYStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iYStep = 1;
    iZStep = renderEnv.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = renderEnv.m_uBlockSize;
    iZStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    break;
  }

  // setup for visible block acceleration
  vxBlockVisibilityTester blockVisibility(GetEnvironment());

  // for all rows of blocks
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+1)%3] - vBlockPosition.m_x, renderEnv.m_uBlockSize));
      const int4 uThisBlockSizeY(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+2)%3] - vBlockPosition.m_y, renderEnv.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        switch (renderEnv.m_uAxisIndex)
        {
        case vxUtils::SAGITTAL: 
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(block*renderEnv.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          break;
        case vxUtils::CORONAL:
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(vBlockPosition.m_y, block*renderEnv.m_uBlockSize, vBlockPosition.m_x));
          break;
        case vxUtils::AXIAL:
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*renderEnv.m_uBlockSize));
          break;
        }

        if (blockVisibility.IsBlockVisible(renderEnv.m_iterVol.GetPos()))
        {
          int4 iImageOffset(0);
          int4 iStartZ((block*renderEnv.m_uBlockSize<minMaxSlicePair.first) ? minMaxSlicePair.first : block*renderEnv.m_uBlockSize);
          int4 iEndZ(((block+1)*renderEnv.m_uBlockSize>minMaxSlicePair.second) ? minMaxSlicePair.second : (block+1)*renderEnv.m_uBlockSize);
          if (iEndZ == iStartZ)
          {
            iEndZ = iStartZ + 1;
          }

          for (int4 y(0), iYOffset(0); y<uThisBlockSizeY; y++, iYOffset+=iYStep)
          {
            for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset++, iXOffset+=iXStep)
            {
              for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%renderEnv.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
              {
                const int4 iCurrInt(renderEnv.m_iterVol.QuickPeekInsideBlock(iZOffset));
                if (iCurrInt > iMaxIntensity[iImageOffset])
                {
                  iMaxIntensity[iImageOffset] = iCurrInt;
                  iMaxPosition[iImageOffset] = z;
                }
              } // for (z)
            } // for (x)
          } // for (y)
        } // block visbile?
      } // for (block)

      int4 iImageOffset(0);

      if ( renderEnv.m_bShowThresholds )
      {
        for ( int4 y=0; y<uThisBlockSizeY; y++ )
        {
          const uint4 uPixelOffset( 4 * ( overallImageSize.m_x * ( vBlockPosition.m_y + y ) + vBlockPosition.m_x ) );
          uint1 * puImg( renderEnv.m_pImagePixels + uPixelOffset );

          for ( int4 x(0); x<uThisBlockSizeX; x++ )
          {
            iMaxPosition[ iImageOffset ] = minMaxSlicePair.second - iMaxPosition[ iImageOffset ];
            const uint4 uIndex( iMaxIntensity[ iImageOffset ] );
            const float4 fMIPOpacity( (uint2) ( renderEnv.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> GetNumberOfShiftBitsFor8bitAlpha());

            if (uIndex >= renderEnv.m_uThresholdLow && uIndex <= renderEnv.m_uThresholdHigh)
            {
              * puImg++ = uint1( 255.0F * fAlpha );
              * puImg++ = uint1( 255.0F * fAlpha + fOneMinusAlpha * iNewG );
              * puImg++ = uint1( 255.0F * fAlpha );
              * puImg++ = uint1( fMIPOpacity );
            }
            else
            {
              * puImg++ = uint1( 255 );
              * puImg++ = uint1( 255 );
              * puImg++ = uint1( 255 );
              * puImg++ = uint1( fMIPOpacity );
            }
            iZPos[iImageOffset] = minMaxSlicePair.second - minMaxSlicePair.first + 1;
            iMaxIntensity[iImageOffset] = 0;
            iMaxPosition[iImageOffset]  = 0;
            iImageOffset++;
          } // for all pixels
        } // for all scanlines
      } // show thresholds?
      else
      {
        for (int4 y=0; y<uThisBlockSizeY; y++)
        {
          const uint4 uPixelOffset(4 * (overallImageSize.m_x*(vBlockPosition.m_y+y) + vBlockPosition.m_x));
          uint1 * puImg(renderEnv.m_pImagePixels + uPixelOffset);

          for (int4 x(0); x<uThisBlockSizeX; x++)
          {
            iMaxPosition[iImageOffset] = minMaxSlicePair.second - iMaxPosition[iImageOffset];
            const uint4 uIndex(iMaxIntensity[iImageOffset]);
            const float4 fMIPOpacity((uint2)(renderEnv.m_pfRGBA[(uIndex<<2)+3]) >> GetNumberOfShiftBitsFor8bitAlpha());

            * puImg++ = uint1( fMIPOpacity );
            * puImg++ = uint1( fMIPOpacity );
            * puImg++ = uint1( fMIPOpacity );
            * puImg++ = uint1( fMIPOpacity );

            iZPos[iImageOffset] = minMaxSlicePair.second - minMaxSlicePair.first + 1;
            iMaxIntensity[iImageOffset] = 0;
            iMaxPosition[iImageOffset]  = 0;
            iImageOffset ++;

          } // for all pixels
        } // for all scanlines
      } // end-else (bDisplayThresholds)
    } // end for all blocks in scanline
  }  // endfor all v

  // set up new slab of rendering.
  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderMIPBlock()


/**
 * Volume render a thin slab
 * @param renderEnv rendering environment
 */
void vxRendererSoftwareMPROrtho::RenderThinSlabBlock(RenderEnvironment & renderEnv)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());
  uint4 uPixelOffset(4 * (overallImageSize[0]*renderEnv.m_renderWindow.GetMinY() + renderEnv.m_renderWindow.GetMinX()));

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    const Vector3D<float4> fVoxelUnits(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);
  }
  // first and last blocks.
  std::pair<int4, int4> MinMaxBlock(minMaxSlicePair.first>>4, minMaxSlicePair.second>>4);

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4  uIndex(0);
  float4 fAccuInt[ renderEnv.m_uBlockSize*renderEnv.m_uBlockSize ];
  float4 fAccuOpa[ renderEnv.m_uBlockSize*renderEnv.m_uBlockSize ];

  // initialize...
  for (int4 y(0); y<renderEnv.m_uBlockSize*renderEnv.m_uBlockSize; y++)
  {
    *(fAccuInt+y) = 0.0F;
    *(fAccuOpa+y) = 0.0F;
  }
  float4 fSampleAlpha;

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch (renderEnv.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    iXStep = renderEnv.m_uBlockSize;
    iYStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iYStep = 1;
    iZStep = renderEnv.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = renderEnv.m_uBlockSize;
    iZStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    break;
  }

  // setup for visible block acceleration
  vxBlockVisibilityTester blockVisibility( GetEnvironment() );

  // for all scanlines
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all pixels in scanline
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+1)%3] - vBlockPosition.m_x, renderEnv.m_uBlockSize));
      const int4 uThisBlockSizeY(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+2)%3] - vBlockPosition.m_y, renderEnv.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        Vector3D<int4> pos; 
        pos[ ( renderEnv.m_uAxisIndex     )     ] = block * renderEnv.m_uBlockSize;
        pos[ ( renderEnv.m_uAxisIndex + 1 ) % 3 ] = vBlockPosition.m_x;
        pos[ ( renderEnv.m_uAxisIndex + 2 ) % 3 ] = vBlockPosition.m_y;
        renderEnv.m_iterVol.SetPos( pos );

        const bool bBlockVisible( blockVisibility.IsBlockVisible( renderEnv.m_iterVol.GetPos() ) );
        int4 iStartZ( ( block * renderEnv.m_uBlockSize < minMaxSlicePair.first ) ? minMaxSlicePair.first : block * renderEnv.m_uBlockSize );
        int4 iEndZ( ( ( block + 1 ) * renderEnv.m_uBlockSize > minMaxSlicePair.second ) ? minMaxSlicePair.second : ( block + 1 ) * renderEnv.m_uBlockSize );
        if ( iEndZ == iStartZ ) iEndZ = iStartZ + 1;

        int4 iImageOffset(0);
        for (int4 y(0), iYOffset(0); (bBlockVisible && y<uThisBlockSizeY); y++, iYOffset+=iYStep)
        {
          for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset++, iXOffset+=iXStep)
          {
            const float4 fOpacityThreshold( 0.99F );
            if ( fAccuOpa[ iImageOffset ] < fOpacityThreshold )
            {
              for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%renderEnv.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
              {
                uIndex = renderEnv.m_iterVol.QuickPeekInsideBlock( iZOffset );
                fSampleAlpha = ( uint2( renderEnv.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> GetNumberOfShiftBitsFor8bitAlpha()) / 255.0F;
                if ( fSampleAlpha > 0.0F )
                {
                  float4 fSampleIntensity( 255.0F * fSampleAlpha );
                  float4 fAccuBuffer( ( 1.0F - fAccuOpa[ iImageOffset ] ) * fSampleAlpha );
                  fAccuInt[ iImageOffset ] += fSampleIntensity * fAccuBuffer;
                  fAccuOpa[ iImageOffset ] += fAccuBuffer;
                }
              } // for (z)
            } // opacity below threshold?
          } // for(x)
        } // for(y)
      } // for (block)

      int4 iImageOffset(0);
      for (int4 y=0; y<uThisBlockSizeY; y++)
      {
        uint4 uPixelOffset(4 * (overallImageSize.m_x*(vBlockPosition.m_y+y) + vBlockPosition.m_x));
        uint1 * puImg(renderEnv.m_pImagePixels + uPixelOffset);

        for (int4 x(0); x<uThisBlockSizeX; x++)
        {
          * puImg++ = 255;
          * puImg++ = 255;
          * puImg++ = 255;
          * puImg++ = uint1( fAccuInt[ iImageOffset ] );

          fAccuInt[ iImageOffset ] = 0.0F;
          fAccuOpa[ iImageOffset ] = 0.0F;
          iImageOffset++;
        }
      }
    } // end-for all volume(u)
  }  // end-for all volume(v)

  // set up new slab of rendering
  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderThinSlabBlock() : Block-based acceleration.


/**
 * Call this function to create image for ThinSlab rendering (with labeled voxls)
 * @param renderEnv render environment
 */
void vxRendererSoftwareMPROrtho::RenderThinSlabBlockWithLabelVoxel( RenderEnvironment & renderEnv )
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(GetImage().GetDim());
  uint4 uPixelOffset(4 * (overallImageSize[0]*renderEnv.m_renderWindow.GetMinY() + renderEnv.m_renderWindow.GetMinX()));

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(renderEnv.m_volDim, renderEnv.m_volUnits, renderEnv.m_iNumSlicesInSlab, renderEnv.m_uAxisIndex, renderEnv.m_iPlaneOrthoDistance));
  // get boundary slices considering crop box extent
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
    const Vector3D<float4> fVoxelUnits(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Vector3D<float4> fMinCorner(pCropBox->GetBox().GetMinPoint());
    Vector3D<float4> fMaxCorner(pCropBox->GetBox().GetMaxPoint());
    fMinCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    fMaxCorner[renderEnv.m_uAxisIndex] /= fVoxelUnits[renderEnv.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[renderEnv.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[renderEnv.m_uAxisIndex]);
  }
  // first and last blocks.
  std::pair<int4, int4> MinMaxBlock(minMaxSlicePair.first>>4, minMaxSlicePair.second>>4);

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4 uIndex(0);
  uint2 uVoxelLabel(0);
  float4 fSampleAlpha(0), fSampleRed(0), fSampleGreen(0), fSampleBlue(0);
  const bool bShowColor( GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR) );

  // initialize...
  float4 fAccuRGBA[ renderEnv.m_uBlockSize * renderEnv.m_uBlockSize * 4 ];
  for ( int4 y(0); y<renderEnv.m_uBlockSize * renderEnv.m_uBlockSize * 4; y++ )
  {
    * ( fAccuRGBA + y ) = 0.0F;
  }

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch ( renderEnv.m_uAxisIndex )
  {
  case vxUtils::SAGITTAL:
    iXStep = renderEnv.m_uBlockSize;
    iYStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    iYStep = 1;
    iZStep = renderEnv.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = renderEnv.m_uBlockSize;
    iZStep = renderEnv.m_uBlockSize * renderEnv.m_uBlockSize;
    break;
  }

  // setup for visible block acceleration
  vxBlockVisibilityTester blockVisibility( GetEnvironment() );

  // for all scanlines
  for (vBlockPosition.m_y=renderEnv.m_renderWindow.GetMinY(); vBlockPosition.m_y<renderEnv.m_renderWindow.GetMaxY(); vBlockPosition.m_y += renderEnv.m_uBlockSize)
  {
    // for all pixels in scanline
    for (vBlockPosition.m_x=renderEnv.m_renderWindow.GetMinX(); vBlockPosition.m_x<renderEnv.m_renderWindow.GetMaxX(); vBlockPosition.m_x += renderEnv.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+1)%3] - vBlockPosition.m_x, renderEnv.m_uBlockSize));
      const int4 uThisBlockSizeY(min(renderEnv.m_volDim[(renderEnv.m_uAxisIndex+2)%3] - vBlockPosition.m_y, renderEnv.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        bool bBlockVisible(true);
        switch (renderEnv.m_uAxisIndex)
        {
        case vxUtils::SAGITTAL: 
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(block*renderEnv.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          bBlockVisible = blockVisibility.IsBlockVisible(renderEnv.m_iterVol.GetPos());
          if (bBlockVisible) renderEnv.m_iterLabel.SetPos(Vector3D<int4>(block*renderEnv.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          break;
        case vxUtils::CORONAL:
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(vBlockPosition.m_y, block*renderEnv.m_uBlockSize, vBlockPosition.m_x));
          bBlockVisible = blockVisibility.IsBlockVisible(renderEnv.m_iterVol.GetPos());
          if (bBlockVisible) renderEnv.m_iterLabel.SetPos(Vector3D<int4>(vBlockPosition.m_y, block*renderEnv.m_uBlockSize, vBlockPosition.m_x));
          break;
        case vxUtils::AXIAL:
          renderEnv.m_iterVol.SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*renderEnv.m_uBlockSize));
          bBlockVisible = blockVisibility.IsBlockVisible(renderEnv.m_iterVol.GetPos());
          if (bBlockVisible) renderEnv.m_iterLabel.SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*renderEnv.m_uBlockSize));
          break;
        }

        if (bBlockVisible == true)
        {
          int4 iStartZ((block*renderEnv.m_uBlockSize<minMaxSlicePair.first) ? minMaxSlicePair.first : block*renderEnv.m_uBlockSize);
          int4 iEndZ(((block+1)*renderEnv.m_uBlockSize>minMaxSlicePair.second) ? minMaxSlicePair.second : (block+1)*renderEnv.m_uBlockSize);
          if (iEndZ == iStartZ) iEndZ = iStartZ+1;
          int4 iImageOffset_A(3);

          for (int4 y(0), iYOffset(0); y<uThisBlockSizeY; y++, iYOffset+=iYStep)
          {
            for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset_A+=4, iXOffset+=iXStep)
            {
              if (fAccuRGBA[iImageOffset_A] < 0.95F)
              {
                for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%renderEnv.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
                {
                  uIndex = (renderEnv.m_iterVol.QuickPeekInsideBlock(iZOffset));
                  float4 * pColorTableEntry(&(renderEnv.m_pfRGBA[uIndex<<2]));
                  fSampleAlpha = ((uint2)(pColorTableEntry[3]) >> GetNumberOfShiftBitsFor8bitAlpha()) / 255.0F;
                  if (fSampleAlpha>0.0F)
                  {
                    uVoxelLabel = (renderEnv.m_iterLabel.QuickPeekInsideBlock(iZOffset));
                    // if the voxel is invisible, no contribution, therefore ...
                    const bool bComponentDensityVisible((* renderEnv.m_pvComponents)[uVoxelLabel]->IsIntensityVisible());
                    if (bComponentDensityVisible == true)
                    {
                      bool bComponentLabelVisible( ( uVoxelLabel > 0 ) && ( ( * renderEnv.m_pvComponents )[ uVoxelLabel ]->IsLabelVisible() ) );
                      if ( bComponentLabelVisible )
                      {
                        // if labeled, use label color...
                        const PixelRGBA<uint1> & labelVoxelColor = ( * renderEnv.m_pvComponents )[ uVoxelLabel ]->GetColor();
                        float4 fAlphaLabelVoxel( labelVoxelColor.A() / 255.0F );
                        float4 fOneMinusAlphaLabelVoxel( ( 1.0F - fAlphaLabelVoxel ) * 255.0F );
                        float4 fLabelVoxelRed  ( labelVoxelColor.R() * fAlphaLabelVoxel + fOneMinusAlphaLabelVoxel );
                        float4 fLabelVoxelGreen( labelVoxelColor.G() * fAlphaLabelVoxel + fOneMinusAlphaLabelVoxel );
                        float4 fLabelVoxelBlue ( labelVoxelColor.B() * fAlphaLabelVoxel + fOneMinusAlphaLabelVoxel );

                        fSampleRed   = fLabelVoxelRed   * fSampleAlpha;
                        fSampleGreen = fLabelVoxelGreen * fSampleAlpha;
                        fSampleBlue  = fLabelVoxelBlue  * fSampleAlpha;
                      }
                      else
                      {
                        if (bShowColor == true)
                        {
                          fSampleRed   = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
                          fSampleGreen = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
                          fSampleBlue  = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
                        }
                        else
                        {
                          // if no color, use shades of gray ...
                          fSampleRed = fSampleGreen = fSampleBlue = fSampleAlpha * 255;
                        }
                      }

                      // possibly blend in some threshold color
                      if ( renderEnv.m_bShowThresholds && uIndex >= renderEnv.m_uThresholdLow && uIndex <= renderEnv.m_uThresholdHigh )
                      {
                        const uint4 iNewG( 255 );
                        const float fAlpha( 0.5F );
                        const float4 fOneMinusAlpha( 1.0F - fAlpha );
                        fSampleRed   =                  fOneMinusAlpha * fSampleRed  ;
                        fSampleGreen = fAlpha * iNewG + fOneMinusAlpha * fSampleGreen;
                        fSampleBlue  =                  fOneMinusAlpha * fSampleBlue ;
                      }

                      // accumulate
                      float4 fAccuBuffer( ( 1.0F - fAccuRGBA[ iImageOffset_A ] ) * fSampleAlpha );
                      fAccuRGBA[ iImageOffset_A - 3 ] += fSampleRed   * fAccuBuffer;
                      fAccuRGBA[ iImageOffset_A - 2 ] += fSampleGreen * fAccuBuffer;
                      fAccuRGBA[ iImageOffset_A - 1 ] += fSampleBlue  * fAccuBuffer;
                      fAccuRGBA[ iImageOffset_A     ] += fAccuBuffer;

                    } // end-if (bComponentLabelVisible)
                  } // end-if (fSampleAlpha>0.0F)
                } // for (z)
              }
            } // for(x)
          } // for(y)
        } // end-if (bBlockVisible)
      } // for (block)

      int4 iImageOffset(0);
      float4 * pfAccuRGBAPtr = fAccuRGBA;
      for ( int4 y=0; y<uThisBlockSizeY; y++ )
      {
        uint4 uPixelOffset( 4 * ( overallImageSize.m_x * ( vBlockPosition.m_y + y ) + vBlockPosition.m_x ) );
        uint1 * puImg( renderEnv.m_pImagePixels + uPixelOffset );
        for ( int4 x(0); x<uThisBlockSizeX; x++ )
        {
          * puImg++ = uint1( * pfAccuRGBAPtr );  * pfAccuRGBAPtr++ = 0.0F;
          * puImg++ = uint1( * pfAccuRGBAPtr );  * pfAccuRGBAPtr++ = 0.0F;
          * puImg++ = uint1( * pfAccuRGBAPtr );  * pfAccuRGBAPtr++ = 0.0F;
          * puImg++ = uint1( * pfAccuRGBAPtr * 255 );  * pfAccuRGBAPtr++ = 0.0F;
        }
      }
    } // end-for all volume(u)
  }  // end-for all volume(v)

  // set up new slab of rendering.
  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderThinSlabBlock() : Parallel + Block


/**
 * Clears the borders of the image that are not a mutiple of 16.
 *
 * @param window   size of the displayed rectangle.
 * @param cropRect size of the actual rendered part.
 */
void vxRendererSoftwareMPROrtho::ClearBorders(const BoundingRect<uint2> & window, 
                                              const BoundingRect<uint2> & cropRect)
{
  Image<PixelRGBA<uint1> > & image(GetImage());

  // need to set the clear color to be fully transparent
  PixelRGBA<uint1> color(0, 0, 0, 0);
  image.SetClearColor(color);

  // rectangle below the cropped area
  if (cropRect.GetMinY() > 0)
  {
    image.ClearPixelsInRectangle(Point2D<uint2>(window.GetMinX(), window.GetMinY() ), Point2D<uint2>(window.GetMaxX()-1, cropRect.GetMinY()-1));
  }

  // rectangle above the cropped area
  if (cropRect.GetMaxY() < window.GetMaxY())
  {
    image.ClearPixelsInRectangle(Point2D<uint2>(window.GetMinX(), cropRect.GetMaxY()+1), Point2D<uint2>(window.GetMaxX()-1, window.GetMaxY()-1));
  }

  // rectangle to the left of the cropped region
  if (cropRect.GetMinX() > 0)
  {
    image.ClearPixelsInRectangle(Point2D<uint2>(window.GetMinX(), cropRect.GetMinY()), Point2D<uint2>(cropRect.GetMinX()-1, cropRect.GetMaxY()));
  }

  // rectangle to the right of the cropped region
  if (cropRect.GetMaxX() < window.GetMaxX())
  {
    image.ClearPixelsInRectangle(Point2D<uint2>(cropRect.GetMaxX()+1, cropRect.GetMinY()), Point2D<uint2>( window.GetMaxX()-1, cropRect.GetMaxY()));
  }

} // ClearBorders()


/**
 * Called right before CreateTexture()
 * This is called by a single thread
 * Clear out the borders of the image
 * @param window screen space render area
 */
void vxRendererSoftwareMPROrtho::PreCreateTexture(const BoundingRect<uint2> & window)
{

  // plane pointer
  vxPlaneOrtho<float4> * pPlaneOrtho(dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho()));
  uint1 uAxisIndex(pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord());

  // get the crop window
  BoundingRect<uint2> cropWindow(GetCropBoxWindow(window, uAxisIndex));

  ClearBorders(window, cropWindow);

} // PreCreateTexture()


/**
 * Creates the texture tile parameters
 */
void vxRendererSoftwareMPROrtho::CreateTexture()
{

  // call base class to generate the OpenGL texture
  vxRendererSoftware::CreateTexture();

  // Now compute the texture coordinates for the texture tile
  const Vector3D<int4> size(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
  const Vector3D<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
  vxPlaneOrtho<float4> * pPlane(dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho()));
  uint4 uIndex(pPlane->GetNormal().GetMaxAbsComponentCoord());
  uint2 i1((uIndex + 0)%3);
  uint2 i2((uIndex + 1)%3);
  uint2 i3((uIndex + 2)%3);
  Vector<float4> lowerLeftPoint(0,0,0);
  Vector<float4> upperRightPoint(size[0] * units[0], size[1] * units[1], size[2] * units[2]);

  float4 fDistance(vxUtils::CalculateOffsetDistance(pPlane->GetDistance() / units[uIndex] + 0.5F,
                                                    size[uIndex], 
                                                    GetEnvironment().GetOffset(), 
                                                    GetEnvironment().GetMaximumOffset()) * units[uIndex]);
  
  lowerLeftPoint[i1]  = fDistance;
  lowerLeftPoint[i2]  = 0;
  lowerLeftPoint[i3]  = 0;
  upperRightPoint[i1] = fDistance;
  upperRightPoint[i2] = units[i2] * max(1,(size[i2]-1));
  upperRightPoint[i3] = units[i3] * max(1,(size[i3]-1));

  GetTextureTile().SetOrigin(lowerLeftPoint);
  GetTextureTile().SetSize(upperRightPoint);

  Vector<float4> lowerLeftCoord(0.0, 0.0, 0.0);
  Vector<float4> upperRightCoord(1.0, 1.0, 1.0);
  upperRightCoord[i3] = (size[i3] > 1) ? (size[i3]-1)/(float)(GetNextPowerOfTwo(size[i3])-1) : 1.0;
  upperRightCoord[i2] = (size[i2] > 1) ? (size[i2]-1)/(float)(GetNextPowerOfTwo(size[i2])-1) : 1.0;

  GetTextureTile().SetTexCoordLowerLeft(lowerLeftCoord);
  GetTextureTile().SetTexCoordUpperRight(upperRightCoord);

  Vector<float4> direction(0,0,0);
  direction[i1] = 1.0F;
  GetTextureTile().SetNormal(Normal3Df(direction));

} // CreateTexture()


/**
 * Accumulation along dedicate view direction (with shading effect)
 * @param current accumulated intensity
 * @param current accumulated opacity
 * @param reference to volume interator
 * @param shade on first sample?
 * @param pointer to shading function
 * @return  false when reached threshold.
 */
bool vxRendererSoftwareMPROrtho::AccumulationOfIntensityAndOpacityWithShading(float4 & fAccuIntensity, 
                                                                              float4 & fAccuOpacity,
                                                                              vxBlockVolumeIterator<uint2> & volIter,
                                                                              bool & bNoShadeOnFirstSample,
                                                                              CalcL1dotGFuncPtr pCalcL1DotGFunc)
{
  const float4 m_fKA(0.5F);
  const float4 m_fKD(0.5F);

  const uint4 uIndex(volIter.GetVoxel());
  // this is inefficient and should use the RendererEnvironment's pointer instead FIX ME
  float4 * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));
  float4 fSampleValue(pfRGBA[(uIndex<<2)+3]/4096.0F);

  // if no contribution, no further calculation.
  if ( fSampleValue <= 0.0F ) return true;

  // 1> calculate gradient, 2> calculate dot product of light (L1) and gradient.
  float4 fL1dotG((this->*pCalcL1DotGFunc)(volIter));
  if (fL1dotG<0)
  {
    fL1dotG = 0;
  }
  float4 fDiffuse(fL1dotG * m_fKD);
  float4 fSampleAlpha(fSampleValue);
  float4 fSampleIntensity;

  if (bNoShadeOnFirstSample == true)
  {
    fSampleIntensity = 255.0F * fSampleValue;
    bNoShadeOnFirstSample = false;
  }
  else
  {
    fSampleIntensity = 255.0F * fSampleValue * m_fKA + 255.0F * fSampleValue * fDiffuse; // + fSpecular;
  }

  // accumulate.
  float4 fAccuBuffer((1.0F - fAccuOpacity) * fSampleAlpha);
  fAccuIntensity += fSampleIntensity   * fAccuBuffer;
  fAccuOpacity += fAccuBuffer;

  // stop here.
  if (fAccuOpacity >= 0.95F) return false;

  return true;
} // AccumulationOfIntensityAndOpacityWithShading()


/**
 * Orthographic ray casting with shading (Axial/Coronal/Sagittal - use iterator)
 * 
 * @param volIter         reference to blockvolume iterator
 * @param minMaxSlicePair slab pair.
 * @param uAxialIndex     enum of axial.
 * @param bInverseDir     flag: viewing from positive/negative direction.
 * @return                RGBA color.
 */
PixelRGBA<float4> vxRendererSoftwareMPROrtho::SimpleRayCastingWithShading( const uint2 vx,                                                                     const uint2 vy, 
                                                                          vxBlockVolumeIterator<uint2> & volIter,
                                                                          const std::pair<uint2, uint2> minMaxSlicePair,
                                                                          const uint1 uAxisIndex, 
                                                                          const bool bInverseDir)
{
  float4 fAccuIntensity(0.0F);
  float4 fAccuOpacity(0.0F);
  bool bNoShadeOnFirstSample(true);

  switch (uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    if (bInverseDir == false)
    {
      volIter.SetPos(Vector3D<int4>(minMaxSlicePair.first, vx, vy));
      for (int2 x=minMaxSlicePair.first; x<=minMaxSlicePair.second; x++, volIter.IncX())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Sagittal) == true) continue;
        break;
      }
    }
    else
    {
      volIter.SetPos(Vector3D<int4>(minMaxSlicePair.second, vx, vy));
      for (int2 x=minMaxSlicePair.second; x>=minMaxSlicePair.first; x--, volIter.DecX())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Sagittal) == true) continue;
        break;
      }
    }
    break;

  case vxUtils::CORONAL:
    if (bInverseDir == false)
    {
      volIter.SetPos(Vector3D<int4>(vy, minMaxSlicePair.first, vx));
      for (int2 y=minMaxSlicePair.first; y<=minMaxSlicePair.second; y++, volIter.IncY())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Coronal) == true) continue;
        break;
      }
    }
    else
    {
      volIter.SetPos(Vector3D<int4>(vy, minMaxSlicePair.second, vx));
      for (int2 y=minMaxSlicePair.second; y>=minMaxSlicePair.first; y--, volIter.DecY())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Coronal) == true) continue;
        break;
      }
    }
    break;

  case vxUtils::AXIAL:
    if (bInverseDir == false)
    {
      volIter.SetPos(Vector3D<int4>(vx, vy, minMaxSlicePair.first));
      for (int2 z=minMaxSlicePair.first; z<=minMaxSlicePair.second; z++, volIter.IncZ())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Axial) == true) continue;
        break;
      }
    }
    else
    {
      volIter.SetPos(Vector3D<int4>(vx, vy, minMaxSlicePair.second));
      for (int2 z=minMaxSlicePair.second; z>=minMaxSlicePair.first; z--, volIter.DecZ())
      {
        if (AccumulationOfIntensityAndOpacityWithShading(fAccuIntensity, fAccuOpacity, volIter, bNoShadeOnFirstSample, &vxRendererSoftwareMPROrtho::CalcL1dotG_Axial) == true) continue;
        break;
      }
    }
    break;

  default: break;

  } // end-switch().

  return PixelRGBA<float4>(fAccuIntensity, fAccuIntensity, fAccuIntensity, fAccuOpacity);
} // SimpleRayCastingWithShading()


/**
 * Dot product of Light (L1(1,1,1)) and gradient: Sagittal(X), using volume iterator.
 *    1> normal calculation(gradient).
 *    2> dot product.
 * @param reference of volume iterator
 * @return dot product
 */
float4 vxRendererSoftwareMPROrtho::CalcL1dotG_Sagittal(vxBlockVolumeIterator<uint2> & volIter)
{
  const float4 fOneOverSqua3(0.57736F); // 1.0F/1.732
  // this is inefficient and should use the RendererEnvironment's pointer instead FIX ME
  float4 * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

  // 1> calculate gradient (coronal)
  uint4 uIndex(volIter.GetVoxel());
  float4 fOpacity000(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek0px1py0pz());
  float4 fOpacity001(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek0px0py1nz());
  float4 fOpacity010(pfRGBA[(uIndex<<2)+3]/4096.0F);

  float4 fx(fOpacity001 - fOpacity000);
  float4 fy(fOpacity010 - fOpacity000);

  // 2> dot product.
  // grad_x = 0.0F, grad_y = fx*root; grad_z = fy*root;;
  // dot_product = grad_x * fOneOverSqua3 + grad_y * fOneOverSqua3 + grad_z * fOneOverSqua3;
  if (fx < 1e-6)
  {
    if (fy < 1e-6)
      return 0.0F; // grad_x=0.0F; grad_y=0.0F; grad_z=0.0F;
    else
      return fOneOverSqua3;  // grad_x=0.0F; grad_y=0.0F; grad_z=1.0F;
  }
  else
  {
    if (fy < 1e-6)
      return fOneOverSqua3;  // grad_x=0.0F; grad_y=1.0F; grad_z=0.0F;
    else
    {
      float4 fRoot = 1.0F / sqrt(fx*fx + fy*fy);
      return (fx*fRoot + fy*fRoot) * fOneOverSqua3; // grad_x=0.0F; grad_y=fx*root; grad_z=fy*root;
    }
  }
} // CalcL1dotG_Sagittal()


/**
 * Dot product of Light (L1(1,1,1)) and gradient: Coronal(Y), using volume iterator.
 *    1> normal calculation(gradient).
 *    2> dot product.
 * @param reference of volume iterator
 * @return dot product
 */
float4 vxRendererSoftwareMPROrtho::CalcL1dotG_Coronal(vxBlockVolumeIterator<uint2> & volIter)
{
  const float4 fOneOverSqua3(0.57736F); // 1.0F/1.732
  // this is inefficient and should use the RendererEnvironment's pointer instead FIX ME
  float4 * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

  // 1> calculate gradient (coronal)
  uint4 uIndex(volIter.GetVoxel());
  float4 fOpacity000(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek1px0py0pz());
  float4 fOpacity001(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek0px0py1nz());
  float4 fOpacity100(pfRGBA[(uIndex<<2)+3]/4096.0F);

  float4 fx(fOpacity001 - fOpacity000);
  float4 fy(fOpacity100 - fOpacity000);

  // 2> dot product.
  // grad_x = fx*root; grad_y = 0.0F; grad_z = fy*root;;
  // dot_product = grad_x * fOneOverSqua3 + grad_y * fOneOverSqua3 + grad_z * fOneOverSqua3;
  if (fx < 1e-6)
  {
    if (fy < 1e-6)
      return 0.0F; // grad_x=0.0F; grad_y=0.0F; grad_z=0.0F;
    else
      return fOneOverSqua3;  // grad_x=0.0F; grad_y=0.0F; grad_z=1.0F;
  }
  else
  {
    if (fy < 1e-6)
      return fOneOverSqua3;  // grad_x=1.0F; grad_y=0.0F; grad_z=0.0F;
    else
    {
      float4 fRoot(1.0F / sqrt(fx*fx + fy*fy));
      return (fx*fRoot + fy*fRoot) * fOneOverSqua3;  // grad_x=fx*root; grad_y=0.0F; grad_z=fy*root;
    }
  }
} // CalcL1dotG_Coronal()


/**
 * Dot product of Light (L1(1,1,1)) and gradient: Axial(Z), using volume iterator
 *    1> normal calculation(gradient)
 *    2> dot product
 * @param reference of volume iterator
 * @return dot product
 */
float4 vxRendererSoftwareMPROrtho::CalcL1dotG_Axial(vxBlockVolumeIterator<uint2> & volIter)
{
  const float4 fOneOverSqua3(0.57736F); // 1.0F/1.732
  // this is inefficient and should use the RendererEnvironment's pointer instead FIX ME
  float4 * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

  // 1> calculate gradient (axial)
  uint4 uIndex(volIter.GetVoxel());
  float4 fOpacity000(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek1px0py0pz());
  float4 fOpacity001(pfRGBA[(uIndex<<2)+3]/4096.0F);

  uIndex = uint4(volIter.Peek0px1py0pz());
  float4 fOpacity010(pfRGBA[(uIndex<<2)+3]/4096.0F);

  float4 fx(fOpacity001 - fOpacity000);
  float4 fy(fOpacity010 - fOpacity000);

  // 2> dot product.
  // grad_x = fx*root; grad_y = fy*root; grad.m_z = 0.0F;
  // dot_product = grad_x * fOneOverSqua3 + grad_y * fOneOverSqua3 + grad_z * fOneOverSqua3;
  if (fx < 1e-6)
  {
    if (fy < 1e-6)
      return 0.0F; // grad_x = 0.0F; grad_y = 0.0F; grad.m_z = 0.0F;
    else
      return fOneOverSqua3;  // grad_x = 0.0F; grad_y = 1.0F; grad_z = 0.0F;
  }
  else
  {
    if (fy < 1e-6)
      return fOneOverSqua3;  // grad_x = 1.0F; grad_y = 0.0F; grad_z = 0.0F;
    else
    {
      float4 fRoot(1.0F / sqrt(fx*fx + fy*fy));
      return (fx*fRoot + fy*fRoot) * fOneOverSqua3;  // grad_x = fx*root; grad_y = fy*root; grad_z = 0.0F;
    }
  }
} // CalcL1dotG_Axial()


/**
 * MIP rendering: process 4(2x2) pixels per loop
 * @param increment in x
 * @param increment in y
 * @param min max slab pair
 * @param render environment
 * @param pixel color
 */
void vxRendererSoftwareMPROrtho::MIPComposition_4Rays( const uint2 vx, const uint2 vy, const std::pair< uint2, uint2 > minMaxSlicePair,
                                                       RenderEnvironment & renderEnv, PixelRGBA< float4 > pixelColor[] )
{

  int4 z(minMaxSlicePair.second - minMaxSlicePair.first);
  int4 iMaxIntensity[4] = { 0};
  int4 iMaxPosition[4] = { 0};

  switch (renderEnv.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    renderEnv.m_iterVol.SetPos(minMaxSlicePair.first, vx, vy);

    for (; z>=0; --z, renderEnv.m_iterVol.IncX())
    {
      const int4 iCurrIntensityY0Z0(renderEnv.m_iterVol.GetVoxel()  ); 
      if (iCurrIntensityY0Z0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityY0Z0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityY1Z0(renderEnv.m_iterVol.Peek0px1py0pz()); 
      if (iCurrIntensityY1Z0 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityY1Z0; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityY0Z1(renderEnv.m_iterVol.Peek0px0py1pz()); 
      if (iCurrIntensityY0Z1 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityY0Z1; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityY1Z1(renderEnv.m_iterVol.Peek0px1py1pz()); 
      if (iCurrIntensityY1Z1 > iMaxIntensity[3])
      {
        iMaxIntensity[3] = iCurrIntensityY1Z1; iMaxPosition[3] = z;
      } 
    }
    break;

  case vxUtils::CORONAL:
    renderEnv.m_iterVol.SetPos(vy, minMaxSlicePair.first, vx);

    for (; z>=0; --z, renderEnv.m_iterVol.IncY())
    {
      const int4 iCurrIntensityX0Z0(renderEnv.m_iterVol.GetVoxel()  ); if (iCurrIntensityX0Z0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityX0Z0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityX0Z1(renderEnv.m_iterVol.Peek0px0py1pz()); if (iCurrIntensityX0Z1 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityX0Z1; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityX1Z0(renderEnv.m_iterVol.Peek1px0py0pz()); if (iCurrIntensityX1Z0 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityX1Z0; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityX1Z1(renderEnv.m_iterVol.Peek1px0py1pz()); if (iCurrIntensityX1Z1 > iMaxIntensity[3])
      {
        iMaxIntensity[3] = iCurrIntensityX1Z1; iMaxPosition[3] = z;
      }
    }
    break;

  case vxUtils::AXIAL:
    renderEnv.m_iterVol.SetPos(vx, vy, minMaxSlicePair.first);

    for (; z>=0; --z, renderEnv.m_iterVol.IncZ())
    {
      const int4 iCurrIntensityX0Y0(renderEnv.m_iterVol.GetVoxel()       ); if (iCurrIntensityX0Y0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityX0Y0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityX1Y0(renderEnv.m_iterVol.QuickPeek1px0py0pz()); if (iCurrIntensityX1Y0 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityX1Y0; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityX0Y1(renderEnv.m_iterVol.QuickPeek0px1py0pz()); if (iCurrIntensityX0Y1 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityX0Y1; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityX1Y1(renderEnv.m_iterVol.QuickPeek1px1py0pz()); if (iCurrIntensityX1Y1 > iMaxIntensity[3])
      {
        iMaxIntensity[3] = iCurrIntensityX1Y1; iMaxPosition[3] = z;
      }
    }
    break;

  default: break;
  } // end-switch

  for (int4 k(0); k<4; k++)
  {
    iMaxPosition[ k ] = minMaxSlicePair.second - iMaxPosition[ k ];
    bool bComponentDensityVisible( true );
    uint4 uIndex( iMaxIntensity[ k ] );
    const bool bShowColor( GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR) == true );
    if ( bShowColor )
    {
      float4 * pfLutEntry( & renderEnv.m_pfRGBA[ ( uIndex << 2 ) ] );
      pixelColor[ k ] = PixelRGBA<float4>( (uint2) ( * ( pfLutEntry     ) ) >> GetNumberOfShiftBitsFor8bitAlpha(),
                                           (uint2) ( * ( pfLutEntry + 1 ) ) >> GetNumberOfShiftBitsFor8bitAlpha(),
                                           (uint2) ( * ( pfLutEntry + 2 ) ) >> GetNumberOfShiftBitsFor8bitAlpha(),
                                           (uint2) ( * ( pfLutEntry + 3 ) ) >> GetNumberOfShiftBitsFor8bitAlpha() );
    }
    else
    {
      float4 fMIPOpacity( (uint2) ( renderEnv.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> GetNumberOfShiftBitsFor8bitAlpha() );
      pixelColor[ k ] = PixelRGBA<float4>( 255.0F, 255.0F, 255.0F, fMIPOpacity );
    }

    if ( renderEnv.m_bShowLabels )
    {
      // if the voxel with maximum intensity is a label one.
      int4 iXOffset(k%2), iYOffset(k/2);
      switch (renderEnv.m_uAxisIndex)
      {
      case vxUtils::SAGITTAL: renderEnv.m_iterLabel.SetPos(iMaxPosition[k], vx+iXOffset, vy+iYOffset); break;
      case vxUtils::CORONAL:  renderEnv.m_iterLabel.SetPos(vy+iYOffset, iMaxPosition[k], vx+iXOffset); break;
      case vxUtils::AXIAL:    renderEnv.m_iterLabel.SetPos(vx+iXOffset, vy+iYOffset, iMaxPosition[k]); break;
      default: break;
      }
      uint2 uVoxelLabel(renderEnv.m_iterLabel.GetVoxel());

      bool bComponentLabelVisible(false); 
      bComponentDensityVisible = (* renderEnv.m_pvComponents)[uVoxelLabel]->IsIntensityVisible();

      if (uVoxelLabel > 0)
      {
        if (bComponentDensityVisible == true)
        {
          bComponentLabelVisible = (* renderEnv.m_pvComponents)[uVoxelLabel]->IsLabelVisible();
        }
        else
        {
          // find a voxel that is <a> with maximum intensity AND <b> density visible AND <c> label visible.
          pixelColor[ k ] = MIPAuxiliary( vx + iXOffset, vy + iYOffset, minMaxSlicePair, renderEnv );
        }
      } //  end-if(uVoxelLabel>0) 

      if ( bComponentLabelVisible )
      {
        const PixelRGBA<uint1> & LabelVoxelColor = ( * renderEnv.m_pvComponents ) [ uVoxelLabel ]->GetColor();
        float4 fAlpha( LabelVoxelColor.A() / 255.0F );
        float4 fOneMinusAlpha( 1.0F - fAlpha );

        // Mix...
        pixelColor[k].R() = pixelColor[k].R() * fOneMinusAlpha + LabelVoxelColor.R() * fAlpha;
        pixelColor[k].G() = pixelColor[k].G() * fOneMinusAlpha + LabelVoxelColor.G() * fAlpha;
        pixelColor[k].B() = pixelColor[k].B() * fOneMinusAlpha + LabelVoxelColor.B() * fAlpha;
        pixelColor[k].A() = pixelColor[k].A();
      }
    } // if show label

    // is there a segmentation range to be overlayed?
    if (bComponentDensityVisible && renderEnv.m_bShowThresholds)
    {
      if (uIndex >= renderEnv.m_uThresholdLow && uIndex <= renderEnv.m_uThresholdHigh)
      {
        const uint4 iNewG( 255 );
        const float fAlpha( 0.5F );
        const float4 fOneMinusAlpha( 1.0F - fAlpha );
        pixelColor[k].R() =                  fOneMinusAlpha * pixelColor[k].R();
        pixelColor[k].G() = fAlpha * iNewG + fOneMinusAlpha * pixelColor[k].G();
        pixelColor[k].B() =                  fOneMinusAlpha * pixelColor[k].B();
      }
    } 
  } // end-for(k)

} // MIPComposition_4Rays()


/**
 * Accumulate RGB data into a pixel
 *
 * @param destinationPixel the desintation pixel
 * @param iIntensity       the intensity
 * @param renderEnv        the rendering environment
 * @param voxelOffset      the offset of the current voxel from the voxel pos in the vol iter
 */
void vxRendererSoftwareMPROrtho::SumPixel( PixelRGBA<float4> & destinationPixel, int2 iIntensity,
                                           vxRendererSoftwareMPROrtho::RenderEnvironment & renderEnv,
                                           const Vector3Di & voxelOffset )
{
  // get intensity sample color
  PixelRGBAf sample;
  if ( renderEnv.m_bShowColor )
  {
    float4 * pfTfunc( & renderEnv.m_pfRGBA[ iIntensity << 2] );
    sample.R() += uint2( pfTfunc[0] ) >> GetNumberOfShiftBitsFor8bitAlpha();
    sample.G() += uint2( pfTfunc[1] ) >> GetNumberOfShiftBitsFor8bitAlpha();
    sample.B() += uint2( pfTfunc[2] ) >> GetNumberOfShiftBitsFor8bitAlpha();
  }
  else
  {
    float4 * pfTfunc( & renderEnv.m_pfRGBA[ iIntensity << 2 ] );
    const float4 fColor( uint2( pfTfunc[3] ) >> GetNumberOfShiftBitsFor8bitAlpha() );
    sample.R() += fColor;
    sample.G() += fColor;
    sample.B() += fColor;
  }

  // possibly blend label color
  if ( renderEnv.m_bShowLabels && renderEnv.m_bNeedLabels && renderEnv.m_bLabelVolumeAvailable )
  {
    renderEnv.m_iterLabel.SetPos( renderEnv.m_iterVol.GetPos() );
    renderEnv.m_iterLabel.SetPosRel( voxelOffset );
    uint2 uLabelVoxel( renderEnv.m_iterLabel.GetVoxel() );
    bool bComponentDensityVisible( ( * renderEnv.m_pvComponents )[ uLabelVoxel ]->IsIntensityVisible() );
    if ( bComponentDensityVisible == false )
    {
      sample.R() = sample.G() = sample.B() = 0;
    }
    else if ( ( * renderEnv.m_pvComponents )[ uLabelVoxel ]->IsLabelVisible() )
    {
      PixelRGBA<uint1> pixelColor = ( * renderEnv.m_pvComponents )[ uLabelVoxel ]->GetColor();
      PixelRGBAf labelColorF( pixelColor.R(), pixelColor.G(), pixelColor.B(), 0.5F );
      sample.R() = sample.R() * (1.0F - labelColorF.A()) + labelColorF.R() * labelColorF.A();
      sample.G() = sample.G() * (1.0F - labelColorF.A()) + labelColorF.G() * labelColorF.A();
      sample.B() = sample.B() * (1.0F - labelColorF.A()) + labelColorF.B() * labelColorF.A();
    } // if component LabelVisible
  }

  // possibly blend in threshold color
  if ( renderEnv.m_bShowThresholds && iIntensity >= renderEnv.m_uThresholdLow && iIntensity <= renderEnv.m_uThresholdHigh )
  {
    const uint4 iNewG( 255 );
    const float fAlpha( 0.5F );
    const float4 fOneMinusAlpha( 1.0F - fAlpha );
    sample.R() =                  fOneMinusAlpha * sample.R();
    sample.G() = fAlpha * iNewG + fOneMinusAlpha * sample.G();
    sample.B() =                  fOneMinusAlpha * sample.B();
  }

  // sum the pixel
  destinationPixel.R() += sample.R();
  destinationPixel.G() += sample.G();
  destinationPixel.B() += sample.B();

} // SumPixel()


/**
 * X-ray rendering: process 4(2x2) pixels per loop.
 *
 * @param vx              the first voxel location that is not the ray direction
 * @param vx              the second voxel location that is not the ray direction
 * @param minMaxSlicePair the slab bounds
 * @param env             the rendering environment
 * @param pixelColor      the output final pixel color
 */
void vxRendererSoftwareMPROrtho::XRayComposite_4Rays( const uint2 vx, const uint2 vy, const std::pair< uint2, uint2 > minMaxSlicePair,
                                                      RenderEnvironment & renderEnv, PixelRGBA< float4 > pixelColor[] )
{
  // clear the pixel
  int4 z( minMaxSlicePair.second - minMaxSlicePair.first );
  for( int4 i(0); i<4; ++i )
  {
    pixelColor[i].R() = 0.0F;
    pixelColor[i].G() = 0.0F;
    pixelColor[i].B() = 0.0F;
    pixelColor[i].A() = 0.0F;
  }

  switch ( renderEnv.m_uAxisIndex )
  {
  case vxUtils::SAGITTAL:
    renderEnv.m_iterVol.SetPos(minMaxSlicePair.first, vx, vy);

    for (; z>=0; --z, renderEnv.m_iterVol.IncX())
    {
      SumPixel( pixelColor[0], renderEnv.m_iterVol.GetVoxel     (), renderEnv, Vector3Di( 0, 0, 0 ) );
      SumPixel( pixelColor[1], renderEnv.m_iterVol.Peek0px1py0pz(), renderEnv, Vector3Di( 0, 1, 0 ) );
      SumPixel( pixelColor[2], renderEnv.m_iterVol.Peek0px0py1pz(), renderEnv, Vector3Di( 0, 0, 1 ) );
      SumPixel( pixelColor[3], renderEnv.m_iterVol.Peek0px1py1pz(), renderEnv, Vector3Di( 0, 1, 1 ) );
    }
    break;

  case vxUtils::CORONAL:
    renderEnv.m_iterVol.SetPos(vy, minMaxSlicePair.first, vx);

    for (; z>=0; --z, renderEnv.m_iterVol.IncY())
    {
      SumPixel(pixelColor[0], renderEnv.m_iterVol.GetVoxel     (), renderEnv, Vector3Di( 0, 0, 0 ) );
      SumPixel(pixelColor[1], renderEnv.m_iterVol.Peek0px0py1pz(), renderEnv, Vector3Di( 0, 0, 1 ) );
      SumPixel(pixelColor[2], renderEnv.m_iterVol.Peek1px0py0pz(), renderEnv, Vector3Di( 1, 0, 0 ) );
      SumPixel(pixelColor[3], renderEnv.m_iterVol.Peek1px0py1pz(), renderEnv, Vector3Di( 1, 0, 1 ) );
    }
    break;

  case vxUtils::AXIAL:
    renderEnv.m_iterVol.SetPos(vx, vy, minMaxSlicePair.first);

    for (; z>=0; --z, renderEnv.m_iterVol.IncZ())
    {
      SumPixel(pixelColor[0], renderEnv.m_iterVol.GetVoxel          (), renderEnv, Vector3Di( 0, 0, 0 ) );
      SumPixel(pixelColor[1], renderEnv.m_iterVol.QuickPeek1px0py0pz(), renderEnv, Vector3Di( 1, 0, 0 ) );
      SumPixel(pixelColor[2], renderEnv.m_iterVol.QuickPeek0px1py0pz(), renderEnv, Vector3Di( 0, 1, 0 ) );
      SumPixel(pixelColor[3], renderEnv.m_iterVol.QuickPeek1px1py0pz(), renderEnv, Vector3Di( 1, 1, 0 ) );
    }
    break;

  default: break;
  } // orientation switch

  // average the pixel colors
  float fReciprocal(1.0F / (minMaxSlicePair.second - minMaxSlicePair.first + 1.0F));
  for(int4 i(0); i<4; ++i)
    pixelColor[i].ScaleRGB(fReciprocal);

} // XRayComposite_4Rays()


/**
 * Auxiliary to MIP rendering: composite with labled voxels.
 * @param increment in x
 * @param increment in y
 * @param min max slab pair
 * @param render envrionment
 * @return pixel color
 */
PixelRGBA<float4> vxRendererSoftwareMPROrtho::MIPAuxiliary(const uint2 vx, 
                                                           const uint2 vy, 
                                                           const std::pair<uint2, uint2> minMaxSlicePair,
                                                           RenderEnvironment & renderEnv )
{

  int4 iMaxIntensity(0);
  int4 iMaxPosition(0);
  uint2 uVoxelLabelAtMaxPos(0);

  switch (renderEnv.m_uAxisIndex)
  {
    case vxUtils::SAGITTAL: renderEnv.m_iterVol.SetPos(minMaxSlicePair.first, vx, vy); break;
    case vxUtils::CORONAL: renderEnv.m_iterVol.SetPos(vy, minMaxSlicePair.first, vx); break;
    case vxUtils::AXIAL: renderEnv.m_iterVol.SetPos(vx, vy, minMaxSlicePair.first); break;
    default: break;
  }

  for (int2 z(minMaxSlicePair.first); z<=minMaxSlicePair.second; z++)
  {
    // find the voxel with maximum intensity.
    int4 iCurrIntensity(renderEnv.m_iterVol.GetVoxel());
    if (iCurrIntensity > iMaxIntensity)
    {
      switch (renderEnv.m_uAxisIndex)
      {
        case vxUtils::SAGITTAL: renderEnv.m_iterLabel.SetPos(z, vx, vy); break;
        case vxUtils::CORONAL:  renderEnv.m_iterLabel.SetPos(vy, z, vx); break;
        case vxUtils::AXIAL:    renderEnv.m_iterLabel.SetPos(vx, vy, z); break;
        default: break;
      } // switch()

      uint2 uVoxelLabel(renderEnv.m_iterLabel.GetVoxel());

      bool bComponentDensityVisible = (* renderEnv.m_pvComponents)[uVoxelLabel]->IsIntensityVisible();
      if (bComponentDensityVisible==true)
      {
        iMaxIntensity = iCurrIntensity;
        iMaxPosition = z;
        uVoxelLabelAtMaxPos = uVoxelLabel;
      }
    } // if()

    switch (renderEnv.m_uAxisIndex)
    {
      case vxUtils::SAGITTAL: renderEnv.m_iterVol.IncX(); break;
      case vxUtils::CORONAL:  renderEnv.m_iterVol.IncY(); break;
      case vxUtils::AXIAL:    renderEnv.m_iterVol.IncZ(); break;
      default: break;
    } // switch()
  } // end-for(z)

  // original color.
  const uint4 uIndex( iMaxIntensity );
  float4 fMIPOpacity( (uint2) ( renderEnv.m_pfRGBA[ 4 * uIndex + 3 ] ) >> GetNumberOfShiftBitsFor8bitAlpha() );
  PixelRGBA<float4> pixelColor( 255.0F, 255.0f, 255.0F, fMIPOpacity );

  if ( renderEnv.m_bShowLabels )
  {
    bool bComponentLabelVisible = ( * renderEnv.m_pvComponents ) [ uVoxelLabelAtMaxPos ]->IsLabelVisible();
    if ( bComponentLabelVisible )
    {
      const PixelRGBA<uint1> & LabelVoxelColor( ( * renderEnv.m_pvComponents ) [ uVoxelLabelAtMaxPos ]->GetColor() );
      float4 fAlpha( LabelVoxelColor.A() / 255.0F );
      float4 fOneMinusAlpha( 1.0F - fAlpha );

      // blend
      pixelColor.R() = fMIPOpacity * fOneMinusAlpha + LabelVoxelColor.R() * fAlpha;
      pixelColor.G() = fMIPOpacity * fOneMinusAlpha + LabelVoxelColor.G() * fAlpha;
      pixelColor.B() = fMIPOpacity * fOneMinusAlpha + LabelVoxelColor.B() * fAlpha;
      pixelColor.A() = fMIPOpacity;
    }
  } // if show label

  // is there a segmentation range to be overlayed?
  bool bComponentDensityVisible((* renderEnv.m_pvComponents)[uVoxelLabelAtMaxPos]->IsIntensityVisible());

  if (bComponentDensityVisible && renderEnv.m_bShowThresholds)
  {
    if (uIndex >= renderEnv.m_uThresholdLow && uIndex <= renderEnv.m_uThresholdHigh)
    {
      const uint4 iNewG(255);
      const float fAlpha = (128.0F / 255.0F);
      const float4 fOneMinusAlpha(1.0F - fAlpha);
      pixelColor.R() =                          fAlpha * pixelColor.R();
      pixelColor.G() = fOneMinusAlpha * iNewG + fAlpha * pixelColor.G();
      pixelColor.B() =                          fAlpha * pixelColor.B();
    }
  }

  return pixelColor;

} // MIPAuxiliary()


/**
 * Return the bounding rectangle of the crop box in image coordinates
 *
 * @return the bounding rectnagle of the crop box in image coordinates
 */
BoundingRect<uint2> vxRendererSoftwareMPROrtho::GetCropRectImage()
{
  // get the crop box
  vxShareableObject<vxCropBox> * pCropBox(GetEnvironment().GetCropBox());
  vxPlaneOrtho<float4> * pPlane = dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho());
  uint4 uAxisIndex(pPlane->GetNormal().GetMaxAbsComponentCoord());
  BoundingRect<float4> fCropRect( pCropBox->GetCropRect( uAxisIndex ) );

  // project
  uint2 i1((uAxisIndex + 0) % 3);
  uint2 i2((uAxisIndex + 1) % 3);
  uint2 i3((uAxisIndex + 2) % 3);

  // convert to volume coordinates
  BoundingRect<uint2 > uCropRect;
  const Vector3D<float4> volUnits( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  const Triple<int4> dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  uCropRect.SetMinX( uint2( Bound( int4(0), int4( ceil ( fCropRect.GetMinX() / volUnits[i2] - 0.5F ) ), int4( dim[i2]-1 ) ) ) );
  uCropRect.SetMaxX( uint2( Bound( int4(0), int4( floor( fCropRect.GetMaxX() / volUnits[i2] + 0.5F ) ), int4( dim[i2]-1 ) ) ) );
  uCropRect.SetMinY( uint2( Bound( int4(0), int4( ceil ( fCropRect.GetMinY() / volUnits[i3] - 0.5F ) ), int4( dim[i3]-1 ) ) ) );
  uCropRect.SetMaxY( uint2( Bound( int4(0), int4( floor( fCropRect.GetMaxY() / volUnits[i3] + 0.5F ) ), int4( dim[i3]-1 ) ) ) );

  return uCropRect;

} // GetCropRectImage()


/**
 * Returns the window to be rendered after cropping with the cropbox
 * @param initial window
 * @param viewing direction
 */
BoundingRect<uint2> vxRendererSoftwareMPROrtho::GetCropBoxWindow(const BoundingRect<uint2> & window, 
                                                                 const uint1 & uAxisIndex)
{
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    BoundingRect<uint2> uCropRect(GetCropRectImage());
    return window.GetIntersection(uCropRect);
  }
  else
  {
    return window;
  }
} // GetCropBoxWindow()


/**
 * Returns the window to be rendered after cropping with the cropbox
 *
 * @param window      Initial window
 * @param uAxisIndex  Viewing direction
 */
void vxRendererSoftwareMPROrtho::ExpandWindowToMultipleOfBlockSize(BoundingRect<uint2> & rect, const uint4 uBlockSize)
{
  rect.SetMinX( (  rect.GetMinX()                / uBlockSize) * uBlockSize );
  rect.SetMinY( (  rect.GetMinY()                / uBlockSize) * uBlockSize );
  rect.SetMaxX( ( (rect.GetMaxX()+uBlockSize-1)  / uBlockSize) * uBlockSize );
  rect.SetMaxY( ( (rect.GetMaxY()+uBlockSize-1)  / uBlockSize) * uBlockSize );
} // ExpandWindowToMultipleOfBlockSize()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererSoftwareMPROrtho.C,v $
// Revision 1.21  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.20  2007/02/13 22:33:14  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.19  2006/05/31 16:18:22  geconomos
// porting to visual studio 2005
//
// Revision 1.18  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.17  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.16.2.3  2005/08/02 15:53:21  michael
// Fixed comment (getting label, not intensity!!! somebody copy&pasted)
//
// Revision 1.16.2.2  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.16.2.1  2005/07/15 17:34:22  michael
// Fixed clear color bug: with crop box opaque black was filled in instead of a
// transparent color.
//
// Revision 1.16.6.1  2006/05/24 14:03:01  geconomos
// removed dead code in VX_VC_DLL blocks
//
// Revision 1.16  2005/05/16 13:49:08  frank
// made the crop box easier to handle
//
// Revision 1.15  2005/03/12 18:32:33  michael
// Added switch for VC to deliver values with opacity = 255 and color values
// already opacity adjusted.
//
// Revision 1.14  2004/12/09 15:27:07  michael
// took out test for 12 bit data since it doesn't belong there and causes slow
// down for PET and MR data. Someone left it in there after a debugging session.
//
// Revision 1.13  2004/10/05 14:33:53  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.12  2004/10/04 14:59:58  frank
// removed unused return values
//
// Revision 1.10  2004/08/30 23:59:08  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.9  2004/05/19 13:22:08  vxconfig
// added code to make opacity (alpha) full for the VC compilation
//
// Revision 1.8  2004/05/19 13:12:06  geconomos
// Added a preprocessor directive to set the alpha channel to full when generating images in the vc dll build.
//
// Revision 1.7  2004/05/17 13:02:03  frank
// fixed various problems with label volumes and components under different rendering modes
//
// Revision 1.6  2004/05/14 20:48:51  vxconfig
// misc
//
// Revision 1.5  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.4  2004/04/22 13:39:40  geconomos
// Added temporary ( I hope  ) vc dll preprocessor conditions
//
// Revision 1.3  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/30 18:21:29  frank
// added alpha blending for fusion rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.163  2004/02/11 14:06:01  frank
// removed dead code
//
// Revision 1.162  2004/02/09 18:11:22  frank
// fixed threshold display to blend green over instead of add with saturation
//
// Revision 1.161  2004/02/05 12:59:45  michael
// Added the offset integration into the location of where the plane is rendered
// so that annotations can show up properly (#3609)
//
// Revision 1.160  2004/02/05 03:00:00  frank
// fixed offset plane distance for 3x3 and 4x4 configurations
//
// Revision 1.159  2004/02/02 13:45:26  frank
// Issue #3648: Fixed problem with small studies accessing image pixels out of bounds
//
// Revision 1.158  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.157  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.156  2003/11/20 21:10:03  frank
// Fixed problem with the segmentation range not being displayed
//
// Revision 1.155  2003/11/10 13:53:02  frank
// Adjusted the crop box bounds and missing image text to be coordinated
//
// Revision 1.154  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.153  2003/11/05 01:18:48  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.152  2003/08/27 14:35:43  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.151  2003/08/04 14:41:30  frank
// Separated visibility tester class out from vxUtils
//
// Revision 1.150  2003/08/01 18:50:17  frank
// Issue #3411: Rounded crop box instead of truncated so that for really small datasets (e.g., 10 slices) it looks better.
//
// Revision 1.149  2003/07/29 11:52:09  michael
// took out CineSeries stuff since it does not belong to this class/object and will
// be provided by the movie tool.
//
// Revision 1.148  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.147  2003/06/25 15:36:53  frank
// Issue #3312: Threshold display missing from x-ray and this slab mode - added
//
// Revision 1.146  2003/06/18 15:19:59  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.145  2003/06/10 13:30:16  michael
// fixed bug if thin slab and x-ray
//
// Revision 1.144  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.143  2003/05/30 17:56:09  michael
// fixed issue #3268
//
// Revision 1.142  2003/05/30 15:23:25  michael
// initialization in constructor instead of assignment operator
//
// Revision 1.141  2003/05/22 13:04:30  frank
// code review
//
// Revision 1.140  2003/05/19 14:43:30  frank
// additional code review
//
// Revision 1.139  2003/05/16 20:46:41  dongqing
// code review
//
// Revision 1.138  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.137  2003/05/15 20:10:23  frank
// code review of BoundingBox and Box
//
// Revision 1.136  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.135  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.134  2003/04/14 16:38:08  frank
// Separated progress percent and progress message interface
//
// Revision 1.133  2003/04/07 20:12:29  frank
// Issue #3063: Bad crop drawing in coronal on top of image - fixed
//
// Revision 1.132  2003/04/07 20:06:15  dongqing
// Fix missing renderEnv.m_iterVol.IncX() in  the function MIPAuxiliary(...)
//
// Revision 1.131  2003/03/26 19:28:29  michael
// formatting
//
// Revision 1.130  2003/03/21 15:40:40  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.129  2003/03/07 13:42:57  frank
// Added ability to create a new cine series
//
// Revision 1.128  2003/02/25 22:24:08  frank
// minor performance optimization (SetPixelRaw)
//
// Revision 1.127  2003/01/24 18:22:49  frank
// Updated modified event checking
//
// Revision 1.126  2003/01/22 22:04:18  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.125  2001/01/02 15:29:17  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.124  2002/12/24 18:11:09  frank
// Blended labels in x-ray mode
//
// Revision 1.123  2002/12/23 17:06:48  frank
// Removed even more redundant code
//
// Revision 1.122  2002/12/23 15:11:03  frank
// Removed more redundant code
//
// Revision 1.121  2002/12/23 14:32:36  frank
// Removed redundant code
//
// Revision 1.120  2002/12/23 13:54:59  frank
// Encapsulating the common rendering environment variables and constants.
//
// Revision 1.119  2002/12/20 16:14:11  frank
// Added x-ray projection
//
// Revision 1.118  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.117  2002/12/19 20:45:22  ingmar
// PixelRGBA member data is now private -> need to use access functions
//
// Revision 1.116  2002/12/17 14:34:19  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.115  2002/12/11 23:07:01  ingmar
// revived fast RenderMIPBlock() and added consideration of crop box to it
//
// Revision 1.114  2002/12/11 22:38:34  ingmar
// removed obsolete RenderThinSlab() and dependent functions
//
// Revision 1.113  2002/12/11 22:31:22  ingmar
// removed obsolete RenderThinSlab() and dependent functions
//
// Revision 1.112  2002/12/11 22:24:25  ingmar
// added crop box consideration for z-extent to all thin slab rendering
//
// Revision 1.111  2002/12/11 21:14:15  ingmar
// removed window resizing to multiple of blocksize from crop box compute into its own function ExpandWindowToMultipleOfBlockSize()
// also fixed clearing of border
//
// Revision 1.110  2002/12/11 15:38:59  ingmar
// color is now also possible in thin slab mode
//
// Revision 1.109  2002/12/09 22:03:15  ingmar
// adding color in a uniform block only if IsBlockVisibleLabel()
//
// Revision 1.108  2002/12/09 15:32:29  michael
// Added comments and code clean-up
//
// Revision 1.107  2002/12/06 23:12:32  ingmar
// added possibility to apply color to MIP rendering
//
// Revision 1.106  2002/12/06 21:59:17  ingmar
// code beautify
//
// Revision 1.105  2002/12/06 21:58:47  ingmar
// simplified logic in RenderWindow()
//
// Revision 1.104  2002/12/06 21:28:59  michael
// removed old fashioned Set/GetRGBATablePtr(); it was only copy of what is in
// the colormap in the environment.
//
// Revision 1.103  2002/12/06 19:21:37  ingmar
// vxRenderingMode::SHOW_LABEL -> SHOW_COMPONENT_IN_MPR
// and added SHOW_COLORMAP_IN_MPR
//
// Revision 1.102  2002/11/27 21:34:47  frank
// fixed thin slab with labels slab location
//
// Revision 1.101  2002/11/27 20:13:42  ingmar
// removed superfluous rage from interface to IsBlockVisibleSegmentationRange()
// (the range is automatically determined in the constructor)
//
// Revision 1.100  2002/11/27 19:15:16  ingmar
// now considering IsBlockVisibleSegmentationRange()
//
// Revision 1.99  2002/11/26 16:23:37  frank
// Removed #defines and local function definitions
//
// Revision 1.98  2002/11/26 00:10:36  frank
// Fixed MIP/ThinSlab slab width
//
// Revision 1.97  2002/11/25 15:42:54  michael
// checking for crop box
//
// Revision 1.96  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.95  2002/11/15 23:10:55  ingmar
// reenabled skipping of empty blocks
//
// Revision 1.94  2002/11/15 21:06:09  michael
// changed view types
//
// Revision 1.93  2002/11/15 18:12:28  frank
// Fixed border clearing fiasco, re-implemented PreCreateTexture()
//
// Revision 1.92  2002/11/12 21:26:35  ingmar
// moved default CreateTexture() implementation from vxRendererSoftwareMPR() to vxRendererSoftware()
//
// Revision 1.91  2002/11/12 20:48:15  ingmar
// moved GetNextPowerOfTwo() to 10_math.h
//
// Revision 1.90  2002/11/12 17:06:24  michael
// cosmetics
//
// Revision 1.89  2002/11/08 13:53:18  michael
// removed redundant method
//
// Revision 1.88  2002/11/08 03:50:44  michael
// fixed issue with slice number display (for single slice rendering)
//
// Revision 1.87  2002/11/07 00:03:57  ingmar
// code beautify and
// now no longer ignoring crop box
//
// Revision 1.86  2002/11/05 20:25:59  frank
// re-enabled crop box
//
// Revision 1.85  2002/11/02 03:50:06  frank
// working on crop box
//
// Revision 1.84  2002/11/02 03:09:31  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.83  2002/10/24 22:33:12  ingmar
// fixed comment
//
// Revision 1.82  2002/10/07 15:19:28  frank
// Enable full slice thin slab rendering again.
//
// Revision 1.81  2002/10/05 02:38:35  frank
// Cleaned up variables and structure.
//
// Revision 1.80  2002/10/03 15:59:24  frank
// Issue #2689: Fixed crop box rendering.
//
// Revision 1.79  2002/10/01 19:10:19  frank
// What changed outside of here to suddenly make slice rendering bad?
//
// Revision 1.78  2002/09/28 02:19:36  frank
// Fixed border area.
//
// Revision 1.77  2002/09/27 17:07:42  jaddonisio
// Changed uLabelVoxel from uint1 to uint2. Fixes wrap around.
//
// Revision 1.76  2002/09/27 16:06:22  frank
// Completed merge from XP_1.0 branch.
//
// Revision 1.75  2002/09/25 20:36:42  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.74  2002/09/20 19:37:37  geconomos
// MIP rendering bug.
//
// Revision 1.73  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.72  2002/09/20 16:09:01  michael
// changed name to GetWorldCoordinateOpenGL()
//
// Revision 1.71  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.70  2002/09/18 22:06:10  frank
// Fixed flickering.
//
// Revision 1.69  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.68  2002/09/06 22:56:37  frank
// Removed extra #include.
//
// Revision 1.67  2002/09/06 21:29:02  frank
// Made fixed-point an inline function.
//
// Revision 1.66  2002/09/05 23:01:46  jaddonisio
// New vxComponent.
//
// Revision 1.65  2002/08/28 20:32:07  frank
// Removed warning
//
// Revision 1.64  2002/08/14 22:45:44  wenli
// initial version - Vascular
//
// Revision 1.63  2002/08/14 22:28:17  uday
// MIP and VR mode updates for CropBox.
//
// Revision 1.62  2002/08/09 15:53:18  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.61  2002/08/08 15:56:49  uday
// Added SetBorders(), cropBox stuff.
//
// Revision 1.60  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.59  2002/07/29 14:57:49  michael
// Removed viewer type from vxViewer and moved it to vxRenderer
//
// Revision 1.58  2002/07/26 23:16:15  michael
// added a view type to the renderers
//
// Revision 1.57  2002/07/10 21:11:13  ingmar
// enabled Slice Rendering for slice sizes that are not a multiple of 16 (such are the initial default image of size 1x1)
//
// Revision 1.56  2002/07/08 17:33:30  binli
// Cropbox
//
// Revision 1.55  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.54.2.17  2002/07/17 20:49:51  frank
// Fixed green threshold display.
//
// Revision 1.54.2.16  2002/07/15 17:32:01  binli
// another fix to correct to render MIP & ThanSlab.
//
// Revision 1.54.2.15  2002/07/12 23:10:13  uday
// Corrected loop limits in new mip and thinslab renderers.
//
// Revision 1.54.2.14  2002/07/12 21:55:17  uday
// Corrected blocksize for volumes with less than 16 slices.
//
// Revision 1.54.2.13  2002/07/10 21:34:02  binli
// MIP rendering (no label voxel): block-based method, almost double the speed!
//
// Revision 1.54.2.12  2002/07/10 15:16:34  binli
// little more efficient of ThinSlab rendering with labeled voxles.
//
// Revision 1.54.2.11  2002/07/09 21:24:34  binli
// applied same approach as that in Revision 1.54.2.8 to ThinSlab rendering with labeled voxles.
//
// Revision 1.54.2.10  2002/07/09 18:41:48  frank
// Removed someone's debug info.
//
// Revision 1.54.2.9  2002/07/09 13:18:07  michael
// IsBlockVisible12Bit() --> IsBlockVisible()
//
// Revision 1.54.2.8  2002/07/08 21:18:09  binli
// volume rendering: block-based accumulation
// (+double the speed!)
//
// Revision 1.54.2.7  2002/07/08 16:04:31  frank
// Name change.
//
// Revision 1.54.2.6  2002/07/08 15:27:49  frank
// Fixed thin slab problem.
//
// Revision 1.54.2.5  2002/07/03 15:17:18  binli
// forget check in the correct code to fix 'black streaks'
//
// Revision 1.54.2.4  2002/07/03 15:11:42  binli
// 1> CropBox stuff
// 2> fixed 'black streaks' problem.
//
// Revision 1.54.2.3  2002/07/02 22:07:58  frank
// Applied invisible block slipping speedup to axial slices only.
//
// Revision 1.54.2.2  2002/07/02 15:55:50  uday
// Moved GetSlabPair() to vxUtils.
//
// Revision 1.54.2.1  2002/07/01 21:24:25  frank
// Accelerated slice rendering by 20% on average using empty visiblity block space leaping.
//
// Revision 1.54  2002/06/25 16:29:18  binli
// a little acceleration: 5%
//
// Revision 1.53  2002/06/21 17:20:35  binli
// ThinSlab: called QuickPeek functions to get another 10% or so acceleration.
//
// Revision 1.52  2002/06/20 19:17:59  binli
// faster MIP: cast 4 rays per loop. (quickpeek in vxBlockVolume)
//
// Revision 1.51  2002/06/20 17:27:31  binli
// issue 2455: fixed in MIPComposition3.
//
// Revision 1.50  2002/06/17 17:03:40  binli
// test for invisible densities in ThinSlab: same as that in SingleSlice
//
// Revision 1.49  2002/06/17 16:29:13  binli
// 1> better fix to process 'bad voxel'
// 2> replaced multi-op with shift-op
//
// Revision 1.48  2002/06/16 18:54:17  jaddonisio
// Check for bad voxel values, avert crash when rendering 900 slice dataset.
//
// Revision 1.47  2002/06/13 22:23:07  jaddonisio
// Changed name on Get/Set region thresholds.
//
// Revision 1.46  2002/06/13 20:39:38  binli
// test of ThinSlabRendering: 4 methods:
// 1> original
// 2> 'block'
// 3> parallel: 4 rays per loop
// 4> parallel + block.
//
// Revision 1.45  2002/06/13 15:24:01  binli
// Parallel ThinSlab rendering: cast 4 rays per loop.
//
// Revision 1.44  2002/06/12 23:21:21  frank
// Added a test for invisible densities.
//
// Revision 1.43  2002/06/12 22:13:53  frank
// Fixed coronal.
//
// Revision 1.42  2002/06/12 17:01:51  frank
// Sped up performance by more than 30%.
// Moved to 16^2 tiles for rendering.
// Automatically determined if we need to include the label volume.
//
// Revision 1.41  2002/06/11 21:46:46  frank
// Speeding up MIP rendering.
//
// Revision 1.40  2002/06/11 17:18:34  binli
// MIP w/ Threshold display: efficiency
//
// Revision 1.39  2002/06/11 16:25:12  binli
// issue 2739: added a post-processing.
//
// Revision 1.38  2002/06/11 14:31:47  frank
// spelling
//
// Revision 1.37  2002/06/11 04:26:59  frank
// Fixed threshold display in MIP mode.
//
// Revision 1.36  2002/06/10 15:17:42  dmitry
// Working on MIP w/ Threshold display?
//
// Revision 1.35  2002/06/05 17:46:27  binli
// MIP & Single Slice rendering: show component or not.
//
// Revision 1.34  2002/06/04 22:47:27  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.33  2002/05/17 21:37:33  binli
// issue 2230: MIP rendering, invisible component
//
// Revision 1.32  2002/05/16 17:28:25  michael
// changed density invisible to also make label invisible
//
// Revision 1.31  2002/05/14 15:40:09  binli
// little improvement of rendering efficiency: use function pointer avoid 'switch'
//
// Revision 1.30  2002/05/13 21:53:29  ingmar
// completed the task of prependinf SHOW_ to all enums in vxEnderingMode
//
// Revision 1.29  2002/05/13 21:29:28  binli
// MIP composion method 1,2,3
//
// Revision 1.28  2002/05/13 20:55:34  dmitry
// Enum naming changed in RenderingMode.
//
// Revision 1.27  2002/05/10 21:17:24  michael
// fixed thin slab rendering to show at least one slice ...
//
// Revision 1.26  2002/05/10 19:32:03  binli
// code cleaned
//
// Revision 1.25  2002/05/10 19:00:17  binli
// 1> MIP rendering, new rule
// 2> code adjusted
//
// Revision 1.24  2002/05/09 17:36:56  uday
// Changed SliceOrientationEnum scope
//
// Revision 1.23  2002/05/07 20:30:14  binli
// minor change: efficiency
//
// Revision 1.22  2002/05/07 19:57:00  michael
// renamed uTagVoxel to uLabelVoxel
//
// Revision 1.21  2002/05/07 17:57:22  binli
// MIP/ThinSlab rendering: show lable
//
// Revision 1.20  2002/05/06 14:51:55  binli
// fixed a bug when rendering ThinSlab: new transfer function(float & int)
//
// Revision 1.19  2002/05/04 10:46:20  michael
// cosmetics, removed old commented out code
//
// Revision 1.18  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.17  2002/05/03 22:12:27  soeren
// adjusted to new transferfunction that returns int instead of float now
// uRed = uGreen = uBlue = 255 * GetTFTableRGBA()[4*uIndex+3];
// uRed = uGreen = uBlue = GetTFTableRGBA()[4*uIndex+3]/16;
//
// Revision 1.16  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.15  2002/05/01 12:12:05  michael
// Commente dout non existing crop box code
//
// Revision 1.14  2002/04/30 21:11:30  binli
// 1> viewing from positive/negative (with shading)
// 2> switch between MIP/ThinSlab
//
// Revision 1.13  2002/04/30 20:21:42  michael
// Simplified case statements
//
// Revision 1.12  2002/04/29 18:24:32  binli
// 1> code adjusted
// 2> SlidingThinSlab: viewing from positive/nagative direction.
//
// Revision 1.11  2002/04/29 16:07:45  michael
// Now uses slab thickness as defined in vxRenderingMode
//
// Revision 1.10  2002/04/26 19:42:37  michael
// started on adjusting the opacity based on unit length ...
//
// Revision 1.9  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.8  2002/04/25 18:27:43  michael
// fixed texture coordinate for coronal and sagittal (division by int required float cast)
//
// Revision 1.7  2002/04/25 01:39:48  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 1.6  2002/04/24 18:08:07  manju
// Removed the Divide by zero error.
//
// Revision 1.5  2002/04/24 17:36:59  jenny
// vxBlockVolume stuff
//
// Revision 1.4  2002/04/24 15:09:25  jenny
// vxBlockVolume stuff
//
// Revision 1.3  2002/04/24 13:59:50  binli
// code cleaned/comments
//
// Revision 1.2  2002/04/24 03:05:49  michael
// no rendering necessary/performed if no volume available
//
// Revision 1.1  2002/04/23 21:39:14  jenny
// new software .C file
//
// Revision 1.32  2002/04/23 20:52:27  binli
// SlidingThinSlab: version of no shading.
//
// Revision 1.31  2002/04/23 20:12:22  michael
// replaced Density by Intensity
//
// Revision 1.30  2002/04/23 20:10:25  binli
// code cleaned
//
// Revision 1.29  2002/04/23 16:17:58  binli
// optimization for SlidingThinSlab volume rendering.
//
// Revision 1.28  2002/04/23 15:54:44  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.27  2002/04/22 20:36:00  binli
// slab thickness control
//
// Revision 1.26  2002/04/22 19:05:01  binli
// First time MIP: resume calculation.
//
// Revision 1.25  2002/04/22 17:57:50  binli
// SlidingThinSlab: Coronal & Sagittal
//
// Revision 1.24  2002/04/22 13:24:04  michael
// adjusted thin slab rendering to use RenderingMode in environment
//
// Revision 1.23  2002/04/20 18:01:49  michael
// Removed the burden of generating the block volume when MIP is not
// used. Lazy mechanism generates in on demand. Also replaced include
// by forward declarations
//
// Revision 1.22  2002/04/20 15:41:09  michael
// Adjusted code of MIP rendering
//
// Revision 1.21  2002/04/19 22:45:12  frank
// Toggled MIP mode with M key.
//
// Revision 1.20  2002/04/19 16:16:43  michael
// fixed wrong method name
//
// Revision 1.19  2002/04/19 15:28:40  binli
// MIP rendering/Sliding Thin Slice
//
// Revision 1.18  2002/04/09 20:25:14  michael
// included removal of component densities
//
// Revision 1.17  2002/04/09 15:27:59  geconomos
// Added a check to see if a component's label should be rendered.
//
// Revision 1.16  2002/04/08 19:25:34  michael
// fixed display of labels
//
// Revision 1.15  2002/04/08 18:04:02  jaddonisio
// Corrected render mode enums to all caps.
//
// Revision 1.14  2002/04/08 17:42:28  michael
// cosmetics
//
// Revision 1.13  2002/04/06 00:17:45  michael
// fixed sample intensity range
//
// Revision 1.12  2002/04/04 00:59:44  jaddonisio
// Set rendering necessary flag if Label Volume modified.
//
// Revision 1.11  2002/04/01 16:30:25  dmitry
// Changed to use vxRenderingSettings.
//
// Revision 1.10  2002/03/29 16:07:20  michael
// renamed Tag to Label
//
// Revision 1.9  2002/03/29 00:10:24  uday
// Normal for texture tile (Michael)
//
// Revision 1.8  2002/03/26 17:05:37  michael
// removed unused code and fixed sagittal rendering
//
// Revision 1.7  2002/03/25 16:42:06  jenny
// worked on sagittal and coronal images
//
// Revision 1.6  2002/03/20 16:46:11  michael
// fixed NULL pointer by updating tables if modifed OR EMPTY
//
// Revision 1.5  2002/03/20 15:34:33  dmitry
// Call to vxEnvironment to get and set the plane changed.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPROrtho.C,v 1.21 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRendererSoftwareMPROrtho.C,v 1.21 2007/02/15 00:08:23 jmeade Exp $
