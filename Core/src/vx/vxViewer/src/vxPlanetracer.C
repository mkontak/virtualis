// $Id: vxPlanetracer.C,v 1.15.2.1 2008/11/21 00:22:37 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: Frank Dachille  (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxPlanetracer.h"
#include "vxUtils.h"
#include "vxComponent.h"
#include "ImageUtilities.h"
#include "vxMathUtils.h"
#include "vxBlockVisibilityTester.h"
#include "vxRendererUtils.h"
#include "vxRenderBuffer.h"
#include "BoundingRect.h"


const bool   Global_bShowThreadingPattern2D         = false;

const PixelRGBA< uint1 > Global_debugColors2D[8] = { 
  PixelRGBA< uint1 >( 255,    0,     0,   255 ), 
  PixelRGBA< uint1 >(   0,  255,     0,   255 ), 
  PixelRGBA< uint1 >(   0,    0,   255,   255 ), 
  PixelRGBA< uint1 >( 255,  255,     0,   255 ),  
  PixelRGBA< uint1 >(   0,  255,   255,   255 ),  
  PixelRGBA< uint1 >( 255,  165,     0,   255 ),  
  PixelRGBA< uint1 >( 160,   32,   240,   255 ), 
  PixelRGBA< uint1 >( 255,  255,   255,   255 )  
};    



/**
 * Constructor
 */
vxPlanetracer::vxPlanetracer() : m_pRenderBuffer( new vxRenderBuffer() )
{
	m_iMaxVoxelValue = 4094;
} // vxPlanetracer()


/**
 * Destructor
 */
vxPlanetracer::~vxPlanetracer()
{
  if( m_pRenderBuffer )
    delete m_pRenderBuffer;
} // ~vxPlanetracer()


/**
 * Performs initialization prior to rendering
 */
void vxPlanetracer::PreRender()
{
  SetImageSize();

  InitializeRenderArguments();

  ClearBorders( m_arguments );
} // PreRender()


/**
 * Constructor for the rendering environment
 */
void vxPlanetracer::InitializeRenderArguments()
{
  m_arguments.m_pRenderingMode = const_cast< vxRenderingMode * >( &m_settings.GetRenderingMode() );
  m_arguments.m_bShowColor = m_settings.GetShowColor();

  m_arguments.m_bShowThresholds = m_settings.GetShowThreshold();
  m_arguments.m_uThresholdHigh = m_settings.GetThresholdHigh();
  m_arguments.m_uThresholdLow = m_settings.GetThresholdLow();

  m_arguments.m_cropbox = m_settings.GetCropBox();
  m_arguments.m_croprect = m_settings.GetCropRect();

  m_arguments.m_iMaximumOffset = m_settings.GetMaximumOffset();
  m_arguments.m_iOffset = m_settings.GetOffset();

  m_arguments.m_bNeedLabels = m_settings.GetNeedLabels();
  m_arguments.m_bShowLabels = m_settings.GetShowLabels();
  
  
  m_arguments.m_pvComponents = &m_settings.GetComponents();
  
  m_arguments.m_volDim = m_settings.GetIntensityVolume().GetHeader().GetVolDim();
  m_arguments.m_volUnits = m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits();

  m_arguments.m_pPlaneOrtho = const_cast< vxPlaneOrtho< float4 > * >( &m_settings.GetPlaneOrtho() );
  m_arguments.m_uAxisIndex = m_settings.GetPlaneOrtho().GetNormal().GetMaxAbsComponentCoord();

  m_arguments.m_iNumSlicesInSlab = m_settings.GetRenderingMode().GetSlabThickness() / m_arguments.m_volUnits[m_arguments.m_uAxisIndex];
  
  m_arguments.m_imageSize = m_pRenderBuffer->GetColorImage().GetDim();
  m_arguments.m_pImagePixels = reinterpret_cast< uint1*>(m_pRenderBuffer->GetColorImage().GetDataPtr());
  
  m_arguments.m_pfRGBA = const_cast< float4*>(m_settings.GetColorTable());
  m_arguments.m_iShiftBitsFor8bitAlpha = m_settings.GetNumberOfShiftBitsFor8bitAlpha();

  std::string sModality("");
  sModality = m_settings.GetIntensityVolume().GetHeader().GetDicomFieldValue("0008", "0060", sModality);
  if( sModality != "CT" )
	{
	  int4 iHeaderVersion(0);
	  m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("version", iHeaderVersion);

	  if( iHeaderVersion >= 5 )
	  {
      m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("maxValueRange", m_iMaxVoxelValue);
		  if( m_iMaxVoxelValue > 4096 )
				m_iMaxVoxelValue = 65534;
		}
		else
		{
		  m_iMaxVoxelValue = 65534;
		}
	}  // non CT modality	
 
} // InitializeRenderArguments()


/**
 * Determines the image size based on the orientation of the PlaneOrtho
 */ 
void vxPlanetracer::SetImageSize()
{
  uint2 uImageWidth(0);
  uint2 uImageHeight(0);
  uint2 uNumberOfImages(0);

  // Compute the slice width, height and total number
  uint4 uIndex = GetSettings().GetPlaneOrtho().GetNormal().GetMaxAbsComponentCoord();
  const Vector3D<int4> dim( GetSettings().GetIntensityVolume().GetHeader().GetVolDim());
  uImageWidth     = dim[(uIndex+1) % 3];
  uImageHeight    = dim[(uIndex+2) % 3];
  uNumberOfImages = dim[uIndex];

  Triple< uint4 > desiredImageSize( uImageWidth, uImageHeight, uNumberOfImages );

  const uint4 uMinImageSize( 16 );
  desiredImageSize[0] = Max( uMinImageSize, static_cast< uint4 >( GetNextPowerOfTwo( desiredImageSize[0] ) ) );
  desiredImageSize[1] = Max( uMinImageSize, static_cast< uint4 >( GetNextPowerOfTwo( desiredImageSize[1] ) ) );
  
  Triple<uint4> actualImageSize( GetRenderBuffer().GetBufferSize() );

  if ( desiredImageSize[0] != actualImageSize[0] || desiredImageSize[1] != actualImageSize[1] )
  {
    GetRenderBuffer().SetBufferSize( desiredImageSize );
  }
} // SetImageSize()

  
/**
 * Render a subset of the image
 *
 * @param window    rectangle within viewport to be rendered 
 * (assumes that the reactangle coordinates correspond to volume coordinates)
 * @param uThreadID the thread ID
 * @return int4     unused
 */
void vxPlanetracer::RenderWindow(const BoundingRect<uint2> & window, const uint1 & uThreadID)
{
  // set up the rendering environment
  vxPlanetracerRenderArgs args = m_arguments;

  args.m_uThreadId = uThreadID;
  args.m_debugColor = Global_debugColors2D[ uThreadID ];

  // 
  vxBlockVolumeIterator<uint2> iterVol( m_settings.GetIntensityVolume() );
  vxBlockVolumeIterator<uint2> iterLabel( m_settings.GetLabelVolume() );
  args.m_pIterVol = &iterVol;
  args.m_pIterLabel = &iterLabel;

  vxBlockVisibilityTester blockVisibility( 
    &m_settings.GetIntensityVolume(), 
    &m_settings.GetLabelVolume(),
    const_cast< vxRenderingMode * >( &m_settings.GetRenderingMode() ),
    const_cast< vxColormap * >( &m_settings.GetColormap() ),
    m_settings.GetComponents(),
    m_settings.GetThresholdLow(), 
    m_settings.GetThresholdHigh() );
  args.m_pBlockVisibility = &blockVisibility;


  args.m_renderWindow = GetCropBoxWindow( window, args ); // does consider crop box
  
  // TODO: keep an eye on this
  ExpandWindowToMultipleOfBlockSize( args.m_renderWindow, args.m_uBlockSize );

  // get current slab thickness
  args.m_iPlaneOrthoDistance = vxUtils::CalculateOffsetDistance(args.m_pPlaneOrtho->GetDistance() / args.m_volUnits[ args.m_uAxisIndex ] + 0.5F,
                                                                     args.m_volDim[args.m_uAxisIndex], 
                                                                     args.m_iOffset, 
                                                                     args.m_iMaximumOffset);
  
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));
  
  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= args.m_volUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= args.m_volUnits[args.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);
    // ensure that nothing is rendered if outside the crop box
    if (minMaxSlicePair.first > int4(fMaxCorner[args.m_uAxisIndex]))
      return;
    if (minMaxSlicePair.second < int4(fMinCorner[args.m_uAxisIndex]))
      return;
    args.m_minMaxSlicePair = minMaxSlicePair;
  }

  // select the proper renderer
  if (args.m_pRenderingMode->GetRenderingAlgorithm() == vxRenderingMode::MIP)
  {
    if (args.m_bShowColor || (args.m_bShowLabels && args.m_bNeedLabels ))
      RenderMIP(args);
    else
      RenderMIPBlock(args);
  }
  else if (args.m_pRenderingMode->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB)
  {
    if ( args.m_bShowColor || args.m_bShowThresholds ||
         ( args.m_bShowLabels && args.m_bNeedLabels  ) )
      RenderThinSlabBlockWithLabelVoxel(args);
    else
      RenderThinSlabBlock(args);
  }
  else if (args.m_pRenderingMode->GetRenderingAlgorithm() ==  vxRenderingMode::XRAY)
  {
    RenderXRay(args);
  }
  else
  {
    RenderSingleSlice(args);
  }
} // RenderWindow()


/**
 * Call this function to create image of single slice
 *
 * @param args the rendering environment
 * @return returns 1 if anything was rendered
 */
void vxPlanetracer::RenderSingleSlice(vxPlanetracerRenderArgs & args)
{

  // make sure we are trying to render inside the crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    Vector3D<float4> minCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> maxCorner(args.m_cropbox.GetMaxPoint());
    minCorner[args.m_uAxisIndex] /= args.m_volUnits[args.m_uAxisIndex];
    maxCorner[args.m_uAxisIndex] /= args.m_volUnits[args.m_uAxisIndex];
    // do not draw the slice if outside bounding box
    if ( args.m_iPlaneOrthoDistance < minCorner[args.m_uAxisIndex] ||
         args.m_iPlaneOrthoDistance > maxCorner[args.m_uAxisIndex] )
    {
      return;
    }
  }

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);
  uint4 uPixelOffset(4 * (overallImageSize[0]*args.m_renderWindow.GetMinY() + args.m_renderWindow.GetMinX()));

  // declare these outside the loop
  Vector3D<int4> vVoxelPosition(0,0,0);
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4 uRed(0), uGreen(0), uBlue(0), uAlpha(0);
  //Image<PixelRGBA<uint1> > & image(GetImage());

  // setup for visible block acceleration

  // for all rows of blocks
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      vPixelPosition.m_x = vBlockPosition.m_x;
      vPixelPosition.m_y = vBlockPosition.m_y;
      vVoxelPosition[args.m_uAxisIndex]         = args.m_iPlaneOrthoDistance;
      vVoxelPosition[(args.m_uAxisIndex+1) % 3] = vPixelPosition.m_x;
      vVoxelPosition[(args.m_uAxisIndex+2) % 3] = vPixelPosition.m_y;
      args.m_pIterVol->SetPos(vVoxelPosition);

      // see if we need to render this block
      if (args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos()) ||
         args.m_pBlockVisibility->IsBlockVisibleSegmentationRange( args.m_pIterVol->GetPos()))
      {
        // for all rows in block
        for (uint4 y(0); y<args.m_uBlockSize && y+vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); ++y)
        {
          vPixelPosition.m_x = vBlockPosition.m_x;
          vPixelPosition.m_y = vBlockPosition.m_y + y;
          vVoxelPosition[args.m_uAxisIndex]         = args.m_iPlaneOrthoDistance;
          vVoxelPosition[(args.m_uAxisIndex+1) % 3] = vPixelPosition.m_x;
          vVoxelPosition[(args.m_uAxisIndex+2) % 3] = vPixelPosition.m_y;        
          args.m_pIterVol->SetPos(vVoxelPosition);
          args.m_pIterLabel->SetPos(vVoxelPosition);

          // initialize the index to the start of the next scan line
          // (this is needed for multi-threading where we use screen space subdivision)
          uint4 uPixelOffset(4 * (overallImageSize[0]*vPixelPosition.m_y + vPixelPosition.m_x));

          // for all columns in block
          for (uint4 x(0); x<args.m_uBlockSize && x+vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); ++x)
          {
            vPixelPosition.m_x = vBlockPosition.m_x + x;

            const int4 iIntensity( min(m_iMaxVoxelValue, args.m_pIterVol->GetVoxel()) );            
            const uint4 uIndex(iIntensity);
              
            if (args.m_bShowColor == true)
            {
              const float4 * pColorTableEntry(&args.m_pfRGBA[4*uIndex]);
              uRed   = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
              uGreen = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
              uBlue  = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
              uAlpha = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
            }
            else
            {
              uRed = uGreen = uBlue = 255;
              uAlpha = (uint2)(args.m_pfRGBA[4*uIndex+3]) >> args.m_iShiftBitsFor8bitAlpha;
            }
            // is there a tag volume?
            bool bComponentDensityVisible(true);

            if (args.m_bShowLabels && args.m_bNeedLabels )
            {
              // Get the label value of this voxel
              // Mark the CT by the component colors
              uint2 uLabelVoxel(args.m_pIterLabel->GetVoxel());

              // TODO: I'm very slow, a little vector will do
              bComponentDensityVisible = (* args.m_pvComponents)[uLabelVoxel].IsIntensityVisible();
              if (bComponentDensityVisible == false)
              {
                uRed = uGreen = uBlue = 0;
                uAlpha = 0;
              }
              else if ((* args.m_pvComponents)[uLabelVoxel].IsLabelVisible())
              {
                PixelRGBA<uint1> pixelColor((* args.m_pvComponents)[uLabelVoxel].GetColor());
                uRed   = vxRendererUtils::InterpolateFixedPoint(uRed  , pixelColor.R(), pixelColor.A());
                uGreen = vxRendererUtils::InterpolateFixedPoint(uGreen, pixelColor.G(), pixelColor.A());
                uBlue  = vxRendererUtils::InterpolateFixedPoint(uBlue , pixelColor.B(), pixelColor.A());

                //PixelRGBA<uint1> pixelColor((* args.m_pvComponents)[uLabelVoxel].GetColor());
                //uRed    = static_cast<uint1>( uRed   * ( 1 - pixelColor.A() / 255.0F ) + pixelColor.R() * pixelColor.A() / 255.0F );
                //uGreen  = static_cast<uint1>( uGreen * ( 1 - pixelColor.A() / 255.0F ) + pixelColor.G() * pixelColor.A() / 255.0F );
                //uBlue   = static_cast<uint1>( uBlue  * ( 1 - pixelColor.A() / 255.0F ) + pixelColor.B() * pixelColor.A() / 255.0F );
                //uAlpha  += static_cast<uint1>( (255-uAlpha) / 255.0F * pixelColor.A() );
              } // if component LabelVisible

              // increment the label pointer
              switch (args.m_uAxisIndex)
              {
              case vxUtils::SAGITTAL: args.m_pIterLabel->NextXZY(); break;
              case vxUtils::CORONAL:  args.m_pIterLabel->NextYXZ(); break;
              case vxUtils::AXIAL:    args.m_pIterLabel->NextZYX(); break;
              default: break; // should never happen!
              }

            } // if no tag volume
            // is there a segmentation range to be overlayed?
            if (bComponentDensityVisible == true && args.m_bShowThresholds == true &&
                iIntensity >= args.m_uThresholdLow && iIntensity <= args.m_uThresholdHigh)
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
              if ((x-1)%16<4 && (y-1)%16<4 &&(args.m_pBlockVisibility->IsBlockVisible         (vVoxelPosition))) uRed   = 255;
              if ((x-5)%16<4 && (y-5)%16<4 &&(args.m_pBlockVisibility->IsBlockVisibleIntensity(vVoxelPosition))) uGreen = 255;
              if ((x-9)%16<4 && (y-9)%16<4 &&(args.m_pBlockVisibility->IsBlockVisibleLabel    (vVoxelPosition))) uBlue  = 255;
            }

            if( Global_bShowThreadingPattern2D )
            {
              uRed = args.m_debugColor.R();
              uGreen = args.m_debugColor.G();
              uBlue = args.m_debugColor.B();
              uAlpha = args.m_debugColor.A();
            }

            // increment the pointers
            switch (args.m_uAxisIndex)
            {
            case vxUtils::SAGITTAL:
              args.m_pImagePixels[uPixelOffset++] = uRed;
              args.m_pImagePixels[uPixelOffset++] = uGreen;
              args.m_pImagePixels[uPixelOffset++] = uBlue;
              args.m_pImagePixels[uPixelOffset++] = uAlpha;
              args.m_pIterVol->IncY();
              break;
            case vxUtils::CORONAL:
              args.m_pImagePixels[uPixelOffset++] = uRed;
              args.m_pImagePixels[uPixelOffset++] = uGreen;
              args.m_pImagePixels[uPixelOffset++] = uBlue;
              args.m_pImagePixels[uPixelOffset++] = uAlpha;
              args.m_pIterVol->IncZ();
//            image.SetPixelRaw(Point2D<uint2>(vPixelPosition.m_x, vPixelPosition.m_y), PixelRGBA<uint1>(uRed, uGreen, uBlue, uAlpha));
              break;
            case vxUtils::AXIAL:
              args.m_pImagePixels[uPixelOffset++] = uRed;
              args.m_pImagePixels[uPixelOffset++] = uGreen;
              args.m_pImagePixels[uPixelOffset++] = uBlue;
              args.m_pImagePixels[uPixelOffset++] = uAlpha;
              args.m_pIterVol->IncX();
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
        if (args.m_bShowColor && (!args.m_bShowLabels || args.m_pBlockVisibility->IsBlockVisibleLabel( args.m_pIterVol->GetPos())))
        {
          const int4 iIntensity( args.m_pIterVol->GetVoxel());
          const uint4 uIndex(iIntensity);
          float4 * pfColorTableEntry(&args.m_pfRGBA[4*uIndex]);
          uint1 * pColorChannel((uint1*) (&color));
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
          *pColorChannel++ = (uint2)(*pfColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
        }

        // block is not visible
        // for all rows in block
        for (uint4 y(0); y<args.m_uBlockSize; ++y)
        {
          vPixelPosition.m_x = vBlockPosition.m_x;
          vPixelPosition.m_y = vBlockPosition.m_y + y;

          // initialize the index to the start of the next scan line within block
          uint4 uPixelOffset(4 * (overallImageSize[0]*vPixelPosition.m_y + vPixelPosition.m_x));

          // fill block tile with solid color

          int4 * pPixel((int4*)(args.m_pImagePixels + uPixelOffset));
          int4 * pLastPixel(pPixel + args.m_uBlockSize);
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
void vxPlanetracer::RenderMIP(vxPlanetracerRenderArgs & args)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);

  // get current slab thickness
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    const Vector3D<float4> fVoxelUnits( args.m_volUnits );
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ args.m_uAxisIndex ] ) )
      return;
    if ( minMaxSlicePair.second < int4( fMinCorner[ args.m_uAxisIndex ] ) )
      return;
  } // show crop box?

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  PixelRGBA<float4> pixelValue[4];

  // for all rows of blocks
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(args.m_volDim[(args.m_uAxisIndex+1)%3] - vBlockPosition.m_x, args.m_uBlockSize));
      const int4 uThisBlockSizeY(min(args.m_volDim[(args.m_uAxisIndex+2)%3] - vBlockPosition.m_y, args.m_uBlockSize));

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

          MIPComposition_4Rays(vPixelPosition.m_x, vPixelPosition.m_y, minMaxSlicePair, args, pixelValue);

          // write the pixel
          args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].R();
          args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].G();
          args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].B();
          args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[0].A();
 
          if ((x+1)<uThisBlockSizeX)
          {
            args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].R();
            args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].G();
            args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].B();
            args.m_pImagePixels[uPixelOffset++] = (uint1) pixelValue[1].A();
          }

          if ((y+1)<uThisBlockSizeY)
          {
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].R();
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].G();
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].B();
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[2].A();

            if ((x+1)<uThisBlockSizeX)
            {
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].R();
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].G();
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].B();
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1) pixelValue[3].A();
            }
          }
        } // for x in block
      } // for y in block
    } // end for all blocks in scanline
  }  // endfor all v

  // set up new slab of rendering
//  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderMIP()


/**
 * Call this function to create an x-ray rendered image
 * @param rendering environment
 * @return int4
 */
int4 vxPlanetracer::RenderXRay(vxPlanetracerRenderArgs & args)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    const Vector3D<float4> fVoxelUnits( args.m_volUnits );
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ args.m_uAxisIndex ] ) )
      return 1;
    if ( minMaxSlicePair.second < int4( fMinCorner[ args.m_uAxisIndex ] ) )
      return 1;
  } // show crop box?

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  PixelRGBA<float4> pixelValue[4];

  // for all rows of blocks
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(args.m_volDim[(args.m_uAxisIndex+1)%3] - vBlockPosition.m_x, args.m_uBlockSize));
      const int4 uThisBlockSizeY(min(args.m_volDim[(args.m_uAxisIndex+2)%3] - vBlockPosition.m_y, args.m_uBlockSize));

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

          XRayComposite_4Rays(vPixelPosition.m_x, vPixelPosition.m_y, minMaxSlicePair, args, pixelValue);

          // write the pixel
          args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].R();
          args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].G();
          args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[0].B();
          args.m_pImagePixels[uPixelOffset++] = 255;

          if ((x+1)<uThisBlockSizeX)
          {
            args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].R();
            args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].G();
            args.m_pImagePixels[uPixelOffset++] = (uint1)pixelValue[1].B();
            args.m_pImagePixels[uPixelOffset++] = 255;
          }

          if ((y+1)<uThisBlockSizeY)
          {
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].R();
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].G();
            args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[2].B();
            args.m_pImagePixels[uNextLinePixelOffset++] = 255;

            if ((x+1)<uThisBlockSizeX)
            {
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].R();
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].G();
              args.m_pImagePixels[uNextLinePixelOffset++] = (uint1)pixelValue[3].B();
              args.m_pImagePixels[uNextLinePixelOffset++] = 255;
            }
          }
        } // for x in block
      } // for y in block
    } // end for all blocks in scanline
  }  // endfor all v

  // set up new slab of rendering.
//  SetMinMaxSlicePair(minMaxSlicePair);

  return 1;

} // RenderXRay()


/**
 * Call this function to create image of MIP rendering
 * @param args the rendering environment
 */
void vxPlanetracer::RenderMIPBlock( vxPlanetracerRenderArgs & args )
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    const Vector3D<float4> fVoxelUnits( args.m_volUnits );
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];

    // adjust the min max slice pair if the crop box is set
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);

    // ensure that nothing is rendered if outside the crop box
    if ( minMaxSlicePair.first > int4( fMaxCorner[ args.m_uAxisIndex ] ) )
      return;
    if ( minMaxSlicePair.second < int4( fMinCorner[ args.m_uAxisIndex ] ) )
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
  int4 iZPos[args.m_uBlockSize*args.m_uBlockSize];
  int4 iMaxIntensity[args.m_uBlockSize*args.m_uBlockSize];
  int4 iMaxPosition[args.m_uBlockSize*args.m_uBlockSize];

  for (int4 y(0); y<args.m_uBlockSize*args.m_uBlockSize; y++)
  {
    iZPos[y] = minMaxSlicePair.second - minMaxSlicePair.first + 1;
    iMaxIntensity[y] = 0;
    iMaxPosition[y]  = 0;
  }

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch (args.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    iXStep = args.m_uBlockSize;
    iYStep = args.m_uBlockSize * args.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = args.m_uBlockSize * args.m_uBlockSize;
    iYStep = 1;
    iZStep = args.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = args.m_uBlockSize;
    iZStep = args.m_uBlockSize * args.m_uBlockSize;
    break;
  }


  // for all rows of blocks
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all columns of blocks
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(args.m_volDim[(args.m_uAxisIndex+1)%3] - vBlockPosition.m_x, args.m_uBlockSize));
      const int4 uThisBlockSizeY(min(args.m_volDim[(args.m_uAxisIndex+2)%3] - vBlockPosition.m_y, args.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        switch (args.m_uAxisIndex)
        {
        case vxUtils::SAGITTAL: 
          args.m_pIterVol->SetPos(Vector3D<int4>(block*args.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          break;
        case vxUtils::CORONAL:
          args.m_pIterVol->SetPos(Vector3D<int4>(vBlockPosition.m_y, block*args.m_uBlockSize, vBlockPosition.m_x));
          break;
        case vxUtils::AXIAL:
          args.m_pIterVol->SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*args.m_uBlockSize));
          break;
        }

        if (args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos()))
        {
          int4 iImageOffset(0);
          int4 iStartZ((block*args.m_uBlockSize<minMaxSlicePair.first) ? minMaxSlicePair.first : block*args.m_uBlockSize);
          int4 iEndZ(((block+1)*args.m_uBlockSize>minMaxSlicePair.second) ? minMaxSlicePair.second : (block+1)*args.m_uBlockSize);
          if (iEndZ == iStartZ)
          {
            iEndZ = iStartZ + 1;
          }

          for (int4 y(0), iYOffset(0); y<uThisBlockSizeY; y++, iYOffset+=iYStep)
          {
            for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset++, iXOffset+=iXStep)
            {
              for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%args.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
              {
                const int4 iCurrInt( args.m_pIterVol->QuickPeekInsideBlock(iZOffset));
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

      if ( args.m_bShowThresholds )
      {
        for ( int4 y=0; y<uThisBlockSizeY; y++ )
        {
          const uint4 uPixelOffset( 4 * ( overallImageSize.m_x * ( vBlockPosition.m_y + y ) + vBlockPosition.m_x ) );
          uint1 * puImg( args.m_pImagePixels + uPixelOffset );

          for ( int4 x(0); x<uThisBlockSizeX; x++ )
          {
            iMaxPosition[ iImageOffset ] = minMaxSlicePair.second - iMaxPosition[ iImageOffset ];
            const uint4 uIndex( min( m_iMaxVoxelValue, iMaxIntensity[ iImageOffset ] ));
            const float4 fMIPOpacity( (uint2) ( args.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> args.m_iShiftBitsFor8bitAlpha);

            if (uIndex >= args.m_uThresholdLow && uIndex <= args.m_uThresholdHigh)
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
          uint1 * puImg(args.m_pImagePixels + uPixelOffset);

          for (int4 x(0); x<uThisBlockSizeX; x++)
          {
            iMaxPosition[iImageOffset] = minMaxSlicePair.second - iMaxPosition[iImageOffset];
            const uint4 uIndex( min( m_iMaxVoxelValue, iMaxIntensity[iImageOffset]));
            const float4 fMIPOpacity((uint2)(args.m_pfRGBA[(uIndex<<2)+3]) >> args.m_iShiftBitsFor8bitAlpha);

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
//  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderMIPBlock()


/**
 * Volume render a thin slab
 * @param args rendering environment
 */
void vxPlanetracer::RenderThinSlabBlock(vxPlanetracerRenderArgs & args)
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);
  uint4 uPixelOffset(4 * (overallImageSize[0]*args.m_renderWindow.GetMinY() + args.m_renderWindow.GetMinX()));

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));

  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    const Vector3D<float4> fVoxelUnits( args.m_volUnits );
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);
  }
  // first and last blocks.
  std::pair<int4, int4> MinMaxBlock(minMaxSlicePair.first>>4, minMaxSlicePair.second>>4);

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4  uIndex(0);
  float4 fAccuInt[ args.m_uBlockSize*args.m_uBlockSize ];
  float4 fAccuOpa[ args.m_uBlockSize*args.m_uBlockSize ];

  // initialize...
  for (int4 y(0); y<args.m_uBlockSize*args.m_uBlockSize; y++)
  {
    *(fAccuInt+y) = 0.0F;
    *(fAccuOpa+y) = 0.0F;
  }
  float4 fSampleAlpha;

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch (args.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    iXStep = args.m_uBlockSize;
    iYStep = args.m_uBlockSize * args.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = args.m_uBlockSize * args.m_uBlockSize;
    iYStep = 1;
    iZStep = args.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = args.m_uBlockSize;
    iZStep = args.m_uBlockSize * args.m_uBlockSize;
    break;
  }


  // for all scanlines
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all pixels in scanline
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(args.m_volDim[(args.m_uAxisIndex+1)%3] - vBlockPosition.m_x, args.m_uBlockSize));
      const int4 uThisBlockSizeY(min(args.m_volDim[(args.m_uAxisIndex+2)%3] - vBlockPosition.m_y, args.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        Vector3D<int4> pos; 
        pos[ ( args.m_uAxisIndex     )     ] = block * args.m_uBlockSize;
        pos[ ( args.m_uAxisIndex + 1 ) % 3 ] = vBlockPosition.m_x;
        pos[ ( args.m_uAxisIndex + 2 ) % 3 ] = vBlockPosition.m_y;
        args.m_pIterVol->SetPos( pos );

        const bool bBlockVisible( args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos() ) );
        int4 iStartZ( ( block * args.m_uBlockSize < minMaxSlicePair.first ) ? minMaxSlicePair.first : block * args.m_uBlockSize );
        int4 iEndZ( ( ( block + 1 ) * args.m_uBlockSize > minMaxSlicePair.second ) ? minMaxSlicePair.second : ( block + 1 ) * args.m_uBlockSize );
        if ( iEndZ == iStartZ ) iEndZ = iStartZ + 1;

        int4 iImageOffset(0);
        for (int4 y(0), iYOffset(0); (bBlockVisible && y<uThisBlockSizeY); y++, iYOffset+=iYStep)
        {
          for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset++, iXOffset+=iXStep)
          {
            const float4 fOpacityThreshold( 0.99F );
            if ( fAccuOpa[ iImageOffset ] < fOpacityThreshold )
            {
              for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%args.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
              {
                uIndex = args.m_pIterVol->QuickPeekInsideBlock( iZOffset );
                fSampleAlpha = ( uint2( args.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> args.m_iShiftBitsFor8bitAlpha) / 255.0F;
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
        uint1 * puImg(args.m_pImagePixels + uPixelOffset);

        for (int4 x(0); x<uThisBlockSizeX; x++)
        {
#ifndef VX_VC_DLL
          * puImg++ = 255;
          * puImg++ = 255;
          * puImg++ = 255;
          * puImg++ = uint1( fAccuInt[ iImageOffset ] );
#else
          * puImg++ = uint1( fAccuInt[ iImageOffset ] );
          * puImg++ = uint1( fAccuInt[ iImageOffset ] );
          * puImg++ = uint1( fAccuInt[ iImageOffset ] );
          * puImg++ = 255;
#endif
          fAccuInt[ iImageOffset ] = 0.0F;
          fAccuOpa[ iImageOffset ] = 0.0F;
          iImageOffset++;
        }
      }
    } // end-for all volume(u)
  }  // end-for all volume(v)

  // set up new slab of rendering
//  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderThinSlabBlock() : Block-based acceleration.


/**
 * Call this function to create image for ThinSlab rendering (with labeled voxls)
 * @param args render environment
 */
void vxPlanetracer::RenderThinSlabBlockWithLabelVoxel( vxPlanetracerRenderArgs & args )
{

  // initialize the index where to put pixel into the 1D array
  Triple<uint4> overallImageSize(args.m_imageSize);
  uint4 uPixelOffset(4 * (overallImageSize[0]*args.m_renderWindow.GetMinY() + args.m_renderWindow.GetMinX()));

  // get current slab thickness.
  std::pair<int4, int4> minMaxSlicePair(vxUtils::GetSlabPair(args.m_volDim, args.m_volUnits, args.m_iNumSlicesInSlab, args.m_uAxisIndex, args.m_iPlaneOrthoDistance));
  // get boundary slices considering crop box extent
  if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    const Vector3D<float4> fVoxelUnits( args.m_volUnits );
    Vector3D<float4> fMinCorner(args.m_cropbox.GetMinPoint());
    Vector3D<float4> fMaxCorner(args.m_cropbox.GetMaxPoint());
    fMinCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    fMaxCorner[args.m_uAxisIndex] /= fVoxelUnits[args.m_uAxisIndex];
    // adjust the min max slice pair if the crop box is set.
    minMaxSlicePair.first  = max(minMaxSlicePair.first , (int4)fMinCorner[args.m_uAxisIndex]);
    minMaxSlicePair.second = min(minMaxSlicePair.second, (int4)fMaxCorner[args.m_uAxisIndex]);
  }
  // first and last blocks.
  std::pair<int4, int4> MinMaxBlock(minMaxSlicePair.first>>4, minMaxSlicePair.second>>4);

  // declare these outside the loop
  Point<uint2> vPixelPosition(0,0,0);
  Point<uint2> vBlockPosition(0,0,0);
  uint4 uIndex(0);
  uint2 uVoxelLabel(0);
  float4 fSampleAlpha(0), fSampleRed(0), fSampleGreen(0), fSampleBlue(0);
  const bool bShowColor( args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR) );

  // initialize...
  float4 fAccuRGBA[ args.m_uBlockSize * args.m_uBlockSize * 4 ];
  for ( int4 y(0); y<args.m_uBlockSize * args.m_uBlockSize * 4; y++ )
  {
    * ( fAccuRGBA + y ) = 0.0F;
  }

  int4 iXStep(0), iYStep(0), iZStep(0);
  switch ( args.m_uAxisIndex )
  {
  case vxUtils::SAGITTAL:
    iXStep = args.m_uBlockSize;
    iYStep = args.m_uBlockSize * args.m_uBlockSize;
    iZStep = 1;
    break;
  case vxUtils::CORONAL:
    iXStep = args.m_uBlockSize * args.m_uBlockSize;
    iYStep = 1;
    iZStep = args.m_uBlockSize;
    break;
  case vxUtils::AXIAL:
    iXStep = 1;
    iYStep = args.m_uBlockSize;
    iZStep = args.m_uBlockSize * args.m_uBlockSize;
    break;
  }

  // for all scanlines
  for (vBlockPosition.m_y=args.m_renderWindow.GetMinY(); vBlockPosition.m_y<args.m_renderWindow.GetMaxY(); vBlockPosition.m_y += args.m_uBlockSize)
  {
    // for all pixels in scanline
    for (vBlockPosition.m_x=args.m_renderWindow.GetMinX(); vBlockPosition.m_x<args.m_renderWindow.GetMaxX(); vBlockPosition.m_x += args.m_uBlockSize)
    {
      const int4 uThisBlockSizeX(min(args.m_volDim[(args.m_uAxisIndex+1)%3] - vBlockPosition.m_x, args.m_uBlockSize));
      const int4 uThisBlockSizeY(min(args.m_volDim[(args.m_uAxisIndex+2)%3] - vBlockPosition.m_y, args.m_uBlockSize));

      // for each block along view direction...
      for (int4 block(MinMaxBlock.first); block<=(MinMaxBlock.second); block++)
      {
        bool bBlockVisible(true);
        switch (args.m_uAxisIndex)
        {
        case vxUtils::SAGITTAL: 
          args.m_pIterVol->SetPos(Vector3D<int4>(block*args.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          bBlockVisible = args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos());
          if (bBlockVisible) args.m_pIterLabel->SetPos(Vector3D<int4>(block*args.m_uBlockSize, vBlockPosition.m_x, vBlockPosition.m_y));
          break;
        case vxUtils::CORONAL:
          args.m_pIterVol->SetPos(Vector3D<int4>(vBlockPosition.m_y, block*args.m_uBlockSize, vBlockPosition.m_x));
          bBlockVisible = args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos());
          if (bBlockVisible) args.m_pIterLabel->SetPos(Vector3D<int4>(vBlockPosition.m_y, block*args.m_uBlockSize, vBlockPosition.m_x));
          break;
        case vxUtils::AXIAL:
          args.m_pIterVol->SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*args.m_uBlockSize));
          bBlockVisible = args.m_pBlockVisibility->IsBlockVisible( args.m_pIterVol->GetPos());
          if (bBlockVisible) args.m_pIterLabel->SetPos(Vector3D<int4>(vBlockPosition.m_x, vBlockPosition.m_y, block*args.m_uBlockSize));
          break;
        }

        if (bBlockVisible == true)
        {
          int4 iStartZ((block*args.m_uBlockSize<minMaxSlicePair.first) ? minMaxSlicePair.first : block*args.m_uBlockSize);
          int4 iEndZ(((block+1)*args.m_uBlockSize>minMaxSlicePair.second) ? minMaxSlicePair.second : (block+1)*args.m_uBlockSize);
          if (iEndZ == iStartZ) iEndZ = iStartZ+1;
          int4 iImageOffset_A(3);

          for (int4 y(0), iYOffset(0); y<uThisBlockSizeY; y++, iYOffset+=iYStep)
          {
            for (int4 x(0), iXOffset(iYOffset); x<uThisBlockSizeX; x++, iImageOffset_A+=4, iXOffset+=iXStep)
            {
              if (fAccuRGBA[iImageOffset_A] < 0.95F)
              {
                for (int z(iStartZ), iZOffset(iXOffset+(iStartZ%args.m_uBlockSize)*iZStep); z<(iEndZ); z++, iZOffset+=iZStep)
                {
                  uIndex = ( args.m_pIterVol->QuickPeekInsideBlock(iZOffset));
                  float4 * pColorTableEntry(&(args.m_pfRGBA[uIndex<<2]));
                  fSampleAlpha = ((uint2)(pColorTableEntry[3]) >> args.m_iShiftBitsFor8bitAlpha) / 255.0F;
                  if (fSampleAlpha>0.0F)
                  {
                    uVoxelLabel = (args.m_pIterLabel->QuickPeekInsideBlock(iZOffset));
                    // if the voxel is invisible, no contribution, therefore ...
                    const bool bComponentDensityVisible((* args.m_pvComponents)[uVoxelLabel].IsIntensityVisible());
                    if (bComponentDensityVisible == true)
                    {
                      bool bComponentLabelVisible( ( uVoxelLabel > 0 ) && ( ( * args.m_pvComponents )[ uVoxelLabel ].IsLabelVisible() ) );
                      if ( bComponentLabelVisible && args.m_bShowLabels )
                      {
                        // if labeled, use label color...
                        const PixelRGBA<uint1> & labelVoxelColor = ( * args.m_pvComponents )[ uVoxelLabel ].GetColor();
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
                          fSampleRed   = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
                          fSampleGreen = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
                          fSampleBlue  = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
                        }
                        else
                        {
                          // if no color, use shades of gray ...
                          fSampleRed = fSampleGreen = fSampleBlue = fSampleAlpha * 255;
                        }
                      }

                      // possibly blend in some threshold color
                      if ( args.m_bShowThresholds && uIndex >= args.m_uThresholdLow && uIndex <= args.m_uThresholdHigh )
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
        uint1 * puImg( args.m_pImagePixels + uPixelOffset );
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
//  SetMinMaxSlicePair(minMaxSlicePair);

} // RenderThinSlabBlock() : Parallel + Block


/**
 * Clears the borders of the image that are not a mutiple of 16.
 *
 * @param window   size of the displayed rectangle.
 * @param cropRect size of the actual rendered part.
 */
void vxPlanetracer::ClearBorders( vxPlanetracerRenderArgs & args )
{
  Image<PixelRGBA<uint1> > & image( m_pRenderBuffer->GetColorImage() );

  BoundingRect<uint2> window( 0, 0, image.GetDim().m_x, image.GetDim().m_y );

  // get the crop window
  BoundingRect<uint2> cropRect(GetCropBoxWindow( window, args ) );

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
 * MIP rendering: process 4(2x2) pixels per loop
 * @param increment in x
 * @param increment in y
 * @param min max slab pair
 * @param render environment
 * @param pixel color
 */
void vxPlanetracer::MIPComposition_4Rays( const uint2 vx, const uint2 vy, const std::pair< uint2, uint2 > minMaxSlicePair,
                                                       vxPlanetracerRenderArgs & args, PixelRGBA< float4 > pixelColor[] )
{

  int4 z(minMaxSlicePair.second - minMaxSlicePair.first);
  int4 iMaxIntensity[4] = { 0};
  int4 iMaxPosition[4] = { 0};

  switch (args.m_uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    args.m_pIterVol->SetPos(minMaxSlicePair.first, vx, vy);

    for (; z>=0; --z, args.m_pIterVol->IncX())
    {
      const int4 iCurrIntensityY0Z0( args.m_pIterVol->GetVoxel()  ); 
      if (iCurrIntensityY0Z0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityY0Z0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityY1Z0( args.m_pIterVol->Peek0px1py0pz()); 
      if (iCurrIntensityY1Z0 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityY1Z0; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityY0Z1( args.m_pIterVol->Peek0px0py1pz()); 
      if (iCurrIntensityY0Z1 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityY0Z1; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityY1Z1( args.m_pIterVol->Peek0px1py1pz()); 
      if (iCurrIntensityY1Z1 > iMaxIntensity[3])
      {
        iMaxIntensity[3] = iCurrIntensityY1Z1; iMaxPosition[3] = z;
      } 
    }
    break;

  case vxUtils::CORONAL:
    args.m_pIterVol->SetPos(vy, minMaxSlicePair.first, vx);

    for (; z>=0; --z, args.m_pIterVol->IncY())
    {
      const int4 iCurrIntensityX0Z0( args.m_pIterVol->GetVoxel()  ); if (iCurrIntensityX0Z0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityX0Z0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityX0Z1( args.m_pIterVol->Peek0px0py1pz()); if (iCurrIntensityX0Z1 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityX0Z1; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityX1Z0( args.m_pIterVol->Peek1px0py0pz()); if (iCurrIntensityX1Z0 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityX1Z0; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityX1Z1( args.m_pIterVol->Peek1px0py1pz()); if (iCurrIntensityX1Z1 > iMaxIntensity[3])
      {
        iMaxIntensity[3] = iCurrIntensityX1Z1; iMaxPosition[3] = z;
      }
    }
    break;

  case vxUtils::AXIAL:
    args.m_pIterVol->SetPos(vx, vy, minMaxSlicePair.first);

    for (; z>=0; --z, args.m_pIterVol->IncZ())
    {
      const int4 iCurrIntensityX0Y0( args.m_pIterVol->GetVoxel()       ); if (iCurrIntensityX0Y0 > iMaxIntensity[0])
      {
        iMaxIntensity[0] = iCurrIntensityX0Y0; iMaxPosition[0] = z;
      }
      const int4 iCurrIntensityX1Y0( args.m_pIterVol->QuickPeek1px0py0pz()); if (iCurrIntensityX1Y0 > iMaxIntensity[1])
      {
        iMaxIntensity[1] = iCurrIntensityX1Y0; iMaxPosition[1] = z;
      }
      const int4 iCurrIntensityX0Y1( args.m_pIterVol->QuickPeek0px1py0pz()); if (iCurrIntensityX0Y1 > iMaxIntensity[2])
      {
        iMaxIntensity[2] = iCurrIntensityX0Y1; iMaxPosition[2] = z;
      }
      const int4 iCurrIntensityX1Y1( args.m_pIterVol->QuickPeek1px1py0pz()); if (iCurrIntensityX1Y1 > iMaxIntensity[3])
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

    uint4 uIndex( min( m_iMaxVoxelValue, iMaxIntensity[ k ] ));
    const bool bShowColor( args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR) == true );
    if ( bShowColor )
    {
      float4 * pfLutEntry( & args.m_pfRGBA[ ( uIndex << 2 ) ] );
      pixelColor[ k ] = PixelRGBA<float4>( (uint2) ( * ( pfLutEntry     ) ) >> args.m_iShiftBitsFor8bitAlpha,
                                           (uint2) ( * ( pfLutEntry + 1 ) ) >> args.m_iShiftBitsFor8bitAlpha,
                                           (uint2) ( * ( pfLutEntry + 2 ) ) >> args.m_iShiftBitsFor8bitAlpha,
                                           (uint2) ( * ( pfLutEntry + 3 ) ) >> args.m_iShiftBitsFor8bitAlpha );
    }
    else
    {
      float4 fMIPOpacity( (uint2) ( args.m_pfRGBA[ ( uIndex << 2 ) + 3 ] ) >> args.m_iShiftBitsFor8bitAlpha );
      pixelColor[ k ] = PixelRGBA<float4>( 255.0F, 255.0F, 255.0F, fMIPOpacity );
    }

    if ( args.m_bShowLabels )
    {
      // if the voxel with maximum intensity is a label one.
      int4 iXOffset(k%2), iYOffset(k/2);
      switch (args.m_uAxisIndex)
      {
      case vxUtils::SAGITTAL: args.m_pIterLabel->SetPos(iMaxPosition[k], vx+iXOffset, vy+iYOffset); break;
      case vxUtils::CORONAL:  args.m_pIterLabel->SetPos(vy+iYOffset, iMaxPosition[k], vx+iXOffset); break;
      case vxUtils::AXIAL:    args.m_pIterLabel->SetPos(vx+iXOffset, vy+iYOffset, iMaxPosition[k]); break;
      default: break;
      }
      uint2 uVoxelLabel(args.m_pIterLabel->GetVoxel());

      bool bComponentLabelVisible(false); 
      bComponentDensityVisible = (* args.m_pvComponents)[uVoxelLabel].IsIntensityVisible();

      if (uVoxelLabel > 0)
      {
        if (bComponentDensityVisible == true)
        {
          bComponentLabelVisible = (* args.m_pvComponents)[uVoxelLabel].IsLabelVisible();
        }
        else
        {
          // find a voxel that is <a> with maximum intensity AND <b> density visible AND <c> label visible.
          pixelColor[ k ] = MIPAuxiliary( vx + iXOffset, vy + iYOffset, minMaxSlicePair, args );
        }
      } //  end-if(uVoxelLabel>0) 

      if ( bComponentLabelVisible )
      {
        const PixelRGBA<uint1> & LabelVoxelColor = ( * args.m_pvComponents ) [ uVoxelLabel ].GetColor();
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
    if (bComponentDensityVisible && args.m_bShowThresholds)
    {
      if (uIndex >= args.m_uThresholdLow && uIndex <= args.m_uThresholdHigh)
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
 * @param args        the rendering environment
 * @param voxelOffset      the offset of the current voxel from the voxel pos in the vol iter
 */
void vxPlanetracer::SumPixel( PixelRGBA<float4> & destinationPixel, int2 iIntensity,
                                           vxPlanetracerRenderArgs & args,
                                           const Vector3Di & voxelOffset )
{
  // get intensity sample color
  PixelRGBAf sample;
  if ( args.m_bShowColor )
  {
    float4 * pfTfunc( & args.m_pfRGBA[ iIntensity << 2] );
    sample.R() += uint2( pfTfunc[0] ) >> args.m_iShiftBitsFor8bitAlpha;
    sample.G() += uint2( pfTfunc[1] ) >> args.m_iShiftBitsFor8bitAlpha;
    sample.B() += uint2( pfTfunc[2] ) >> args.m_iShiftBitsFor8bitAlpha;
  }
  else
  {
    float4 * pfTfunc( & args.m_pfRGBA[ iIntensity << 2 ] );
    const float4 fColor( uint2( pfTfunc[3] ) >> args.m_iShiftBitsFor8bitAlpha );
    sample.R() += fColor;
    sample.G() += fColor;
    sample.B() += fColor;
  }

  // possibly blend label color
  if ( args.m_bShowLabels && args.m_bNeedLabels )
  {
    args.m_pIterLabel->SetPos( args.m_pIterVol->GetPos() );
    args.m_pIterLabel->SetPosRel( voxelOffset );
    uint2 uLabelVoxel( args.m_pIterLabel->GetVoxel() );
    bool bComponentDensityVisible( ( * args.m_pvComponents )[ uLabelVoxel ].IsIntensityVisible() );
    if ( bComponentDensityVisible == false )
    {
      sample.R() = sample.G() = sample.B() = 0;
    }
    else if ( ( * args.m_pvComponents )[ uLabelVoxel ].IsLabelVisible() )
    {
      PixelRGBA<uint1> pixelColor = ( * args.m_pvComponents )[ uLabelVoxel ].GetColor();
      PixelRGBAf labelColorF( pixelColor.R(), pixelColor.G(), pixelColor.B(), 0.5F );
      sample.R() = sample.R() * (1.0F - labelColorF.A()) + labelColorF.R() * labelColorF.A();
      sample.G() = sample.G() * (1.0F - labelColorF.A()) + labelColorF.G() * labelColorF.A();
      sample.B() = sample.B() * (1.0F - labelColorF.A()) + labelColorF.B() * labelColorF.A();
    } // if component LabelVisible
  }

  // possibly blend in threshold color
  if ( args.m_bShowThresholds && iIntensity >= args.m_uThresholdLow && iIntensity <= args.m_uThresholdHigh )
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
void vxPlanetracer::XRayComposite_4Rays( const uint2 vx, const uint2 vy, const std::pair< uint2, uint2 > minMaxSlicePair,
                                                      vxPlanetracerRenderArgs & args, PixelRGBA< float4 > pixelColor[] )
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

  switch ( args.m_uAxisIndex )
  {
  case vxUtils::SAGITTAL:
    args.m_pIterVol->SetPos(minMaxSlicePair.first, vx, vy);

    for (; z>=0; --z, args.m_pIterVol->IncX())
    {
      SumPixel( pixelColor[0], args.m_pIterVol->GetVoxel     (), args, Vector3Di( 0, 0, 0 ) );
      SumPixel( pixelColor[1], args.m_pIterVol->Peek0px1py0pz(), args, Vector3Di( 0, 1, 0 ) );
      SumPixel( pixelColor[2], args.m_pIterVol->Peek0px0py1pz(), args, Vector3Di( 0, 0, 1 ) );
      SumPixel( pixelColor[3], args.m_pIterVol->Peek0px1py1pz(), args, Vector3Di( 0, 1, 1 ) );
    }
    break;

  case vxUtils::CORONAL:
    args.m_pIterVol->SetPos(vy, minMaxSlicePair.first, vx);

    for (; z>=0; --z, args.m_pIterVol->IncY())
    {
      SumPixel(pixelColor[0], args.m_pIterVol->GetVoxel     (), args, Vector3Di( 0, 0, 0 ) );
      SumPixel(pixelColor[1], args.m_pIterVol->Peek0px0py1pz(), args, Vector3Di( 0, 0, 1 ) );
      SumPixel(pixelColor[2], args.m_pIterVol->Peek1px0py0pz(), args, Vector3Di( 1, 0, 0 ) );
      SumPixel(pixelColor[3], args.m_pIterVol->Peek1px0py1pz(), args, Vector3Di( 1, 0, 1 ) );
    }
    break;

  case vxUtils::AXIAL:
    args.m_pIterVol->SetPos(vx, vy, minMaxSlicePair.first);

    for (; z>=0; --z, args.m_pIterVol->IncZ())
    {
      SumPixel(pixelColor[0], args.m_pIterVol->GetVoxel          (), args, Vector3Di( 0, 0, 0 ) );
      SumPixel(pixelColor[1], args.m_pIterVol->QuickPeek1px0py0pz(), args, Vector3Di( 1, 0, 0 ) );
      SumPixel(pixelColor[2], args.m_pIterVol->QuickPeek0px1py0pz(), args, Vector3Di( 0, 1, 0 ) );
      SumPixel(pixelColor[3], args.m_pIterVol->QuickPeek1px1py0pz(), args, Vector3Di( 1, 1, 0 ) );
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
PixelRGBA<float4> vxPlanetracer::MIPAuxiliary(const uint2 vx, 
                                                           const uint2 vy, 
                                                           const std::pair<uint2, uint2> minMaxSlicePair,
                                                           vxPlanetracerRenderArgs & args )
{

  int4 iMaxIntensity(0);
  int4 iMaxPosition(0);
  uint2 uVoxelLabelAtMaxPos(0);

  switch (args.m_uAxisIndex)
  {
    case vxUtils::SAGITTAL: args.m_pIterVol->SetPos(minMaxSlicePair.first, vx, vy); break;
    case vxUtils::CORONAL: args.m_pIterVol->SetPos(vy, minMaxSlicePair.first, vx); break;
    case vxUtils::AXIAL: args.m_pIterVol->SetPos(vx, vy, minMaxSlicePair.first); break;
    default: break;
  }

  for (int2 z(minMaxSlicePair.first); z<=minMaxSlicePair.second; z++)
  {
    // find the voxel with maximum intensity.
    int4 iCurrIntensity( args.m_pIterVol->GetVoxel());
    if (iCurrIntensity > iMaxIntensity)
    {
      switch (args.m_uAxisIndex)
      {
        case vxUtils::SAGITTAL: args.m_pIterLabel->SetPos(z, vx, vy); break;
        case vxUtils::CORONAL:  args.m_pIterLabel->SetPos(vy, z, vx); break;
        case vxUtils::AXIAL:    args.m_pIterLabel->SetPos(vx, vy, z); break;
        default: break;
      } // switch()

      uint2 uVoxelLabel(args.m_pIterLabel->GetVoxel());

      bool bComponentDensityVisible = (* args.m_pvComponents)[uVoxelLabel].IsIntensityVisible();
      if (bComponentDensityVisible==true)
      {
        iMaxIntensity = iCurrIntensity;
        iMaxPosition = z;
        uVoxelLabelAtMaxPos = uVoxelLabel;
      }
    } // if()

    switch (args.m_uAxisIndex)
    {
      case vxUtils::SAGITTAL: args.m_pIterVol->IncX(); break;
      case vxUtils::CORONAL:  args.m_pIterVol->IncY(); break;
      case vxUtils::AXIAL:    args.m_pIterVol->IncZ(); break;
      default: break;
    } // switch()
  } // end-for(z)

  // original color.
  const uint4 uIndex( iMaxIntensity );
  float4 fMIPOpacity( (uint2) ( args.m_pfRGBA[ 4 * uIndex + 3 ] ) >> args.m_iShiftBitsFor8bitAlpha );
  PixelRGBA<float4> pixelColor( 255.0F, 255.0f, 255.0F, fMIPOpacity );

  if ( args.m_bShowLabels )
  {
    bool bComponentLabelVisible = ( * args.m_pvComponents ) [ uVoxelLabelAtMaxPos ].IsLabelVisible();
    if ( bComponentLabelVisible )
    {
      const PixelRGBA<uint1> & LabelVoxelColor( ( * args.m_pvComponents ) [ uVoxelLabelAtMaxPos ].GetColor() );
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
  bool bComponentDensityVisible((* args.m_pvComponents)[uVoxelLabelAtMaxPos].IsIntensityVisible());

  if (bComponentDensityVisible && args.m_bShowThresholds)
  {
    if (uIndex >= args.m_uThresholdLow && uIndex <= args.m_uThresholdHigh)
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
BoundingRect<uint2> vxPlanetracer::GetCropRectImage( vxPlanetracerRenderArgs & args )
{
  // get the crop box
  uint4 uAxisIndex(args.m_pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord());
  BoundingRect<float4> fCropRect( args.m_croprect );

  // project
  uint2 i1((uAxisIndex + 0) % 3);
  uint2 i2((uAxisIndex + 1) % 3);
  uint2 i3((uAxisIndex + 2) % 3);

  // convert to volume coordinates
  BoundingRect<uint2 > uCropRect;
  const Vector3D<float4> volUnits( args.m_volUnits );
  const Triple<int4> dim( args.m_volDim );
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
BoundingRect<uint2> vxPlanetracer::GetCropBoxWindow(const BoundingRect<uint2> & window, vxPlanetracerRenderArgs & args )
{
  // Commented out per DQ's instructions (This code allows the image to be cropped)
  // 2D images will not crop
  /*if (args.m_pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
  {
    BoundingRect<uint2> uCropRect(GetCropRectImage(args));
    return window.GetIntersection(uCropRect);
  }
  else*/
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
void vxPlanetracer::ExpandWindowToMultipleOfBlockSize(BoundingRect<uint2> & rect, const uint4 uBlockSize)
{
  rect.SetMinX( (  rect.GetMinX()                / uBlockSize) * uBlockSize );
  rect.SetMinY( (  rect.GetMinY()                / uBlockSize) * uBlockSize );
  rect.SetMaxX( ( (rect.GetMaxX()+uBlockSize-1)  / uBlockSize) * uBlockSize );
  rect.SetMaxY( ( (rect.GetMaxY()+uBlockSize-1)  / uBlockSize) * uBlockSize );
} // ExpandWindowToMultipleOfBlockSize()


// $Log: vxPlanetracer.C,v $
// Revision 1.15.2.1  2008/11/21 00:22:37  gdavidson
// Issue #6168: Do not crop 2D images
//
// Revision 1.15  2007/10/09 14:59:21  geconomos
// bound index values for MIP mode
//
// Revision 1.14  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.13  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.12.2.3  2007/04/09 19:08:37  geconomos
// undid previous checkin
//
// Revision 1.12.2.2  2007/04/09 17:26:42  geconomos
// Commented out code that forced render window to multiple of block size
//
// Revision 1.12.2.1  2007/04/03 19:11:04  gdavidson
// Issue #5553: Do not draw components when label is not enabled
//
// Revision 1.12  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.11  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.10  2007/02/22 16:39:19  geconomos
// code cleanup
//
// Revision 1.9  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.8  2006/12/12 16:22:19  geconomos
// Issue# 5229: components not blending
//
// Revision 1.7  2006/11/13 14:15:38  geconomos
// issue# 5082: temp fix for high intensity color lookup.
//
// Revision 1.6  2006/11/04 02:55:54  geconomos
// alpha blending for components
//
// Revision 1.5  2006/09/21 14:58:33  geconomos
// switch datatype for components from vxComponentArray to std::vector<vxComponent>
//
// Revision 1.4  2006/09/14 20:09:04  geconomos
// commenting
//
// Revision 1.3  2006/08/04 19:25:57  geconomos
// implemented ClearBorder for when the crop box is visible
//
// Revision 1.2  2006/08/03 14:17:25  geconomos
// updated for switch to vxRenderingNodePlugin usage
//
// Revision 1.1  2006/07/21 14:59:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxPlanetracer.C,v 1.15.2.1 2008/11/21 00:22:37 gdavidson Exp $
// $Id: vxPlanetracer.C,v 1.15.2.1 2008/11/21 00:22:37 gdavidson Exp $
