// $Id: vxRendererSoftwareMPRCurvedProjected.C,v 1.2 2006/09/25 13:23:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererSoftwareMPRCurvedProjected.h"
#include "Matrix.h"
#include "vxEnvironment.h"
#include "vxPlaneCurved.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxRibbon.h"
#include "vxColormap.h"
#include "vxManipulatorVesselCurved.h"
#include "vxIDs.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"
#include "vxVesselPreferences.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


// namespaces
using namespace std;


/**
* Default constructor
* @param environment
*/
vxRendererSoftwareMPRCurvedProjected::vxRendererSoftwareMPRCurvedProjected(vxEnvironment & environment)
:vxRendererSoftwareMPRCurved(environment)
{
  //LogRec("Constructor", "vxRendererSoftwareMPRCurvedProjected", "Constructor");
  //const uint4 uHeight(512);
  //const uint4 uWidth(256);
  //GetImage().Resize(Triple<uint4>(uWidth, uHeight, 0));
} // Constructor


/**
* Destructor
*/
vxRendererSoftwareMPRCurvedProjected::~vxRendererSoftwareMPRCurvedProjected()
{
} // Destructor


/**
* Called before rendering to decide if the compute-intensive rendering needs to be executed.
* It ends up calling SetRenderingNecessary() with its decision.
*
* @return true if successful
*/
int4 vxRendererSoftwareMPRCurvedProjected::PreRender()
{
  // call base class
  vxRendererSoftwareMPRCurved::PreRender();

  // always have 512 scanlines tall, but the same or less wide
  const uint4 uHeight(512);
  uint4 uWidth(256);
  if ( GetEnvironment().GetWinRect().GetViewportWidth() > 511 )
    uWidth = 512;
  if ( GetImage().GetDim().m_x != uWidth || GetImage().GetDim().m_y != uHeight )
  {
    const Triple<uint4> imageDim( uWidth, uHeight, 0 );
    GetImage().Resize( imageDim );

    // zoom the image so that we see the entire vessel and data width
    vxManipulatorVesselCurved::ZoomEntireVolume( GetEnvironment(), imageDim );
  }

  // make sure the color table is available
  if (GetEnvironment().GetColormap() == NULL)
  {
    LogWrn( "GetEnvironment().GetColormap() is NULL", "vxRendererSoftwareMPRCurvedProjected", "PreRender" );
    return 0;
  }

  // start setup
  ////////////////
  SetRenderingNecessary(false);

  // has the volume changed?
  if (GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY) == true)
    SetRenderingNecessary(true);

  // has the transfer function changed?
  if (GetEnvironment().IsModified(vxEnvironment::COLORMAP) == true)
  {
    SetRenderingNecessary(true);
  }

  // have any of the labels changed?
  if (GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL) == true)
    SetRenderingNecessary(true);

  // has the camera changed?
  if (GetEnvironment().IsModified(vxEnvironment::CAMERA) == true)
    SetRenderingNecessary(true);

  // has the plane mpr changed?
  if (GetEnvironment().IsModified(vxEnvironment::PLANE_MPR) == true)
    SetRenderingNecessary(true);

    // have the preferences changed?
  if ( GetEnvironment().IsModified( vxIDs::VesselPreferences ))
    SetRenderingNecessary(true);

  // has the ribbon changed?
  if ( GetEnvironment().IsModified( vxIDs::Ribbon ) )
    SetRenderingNecessary(true);

  // update the scanlines
  if (GetRenderingNecessary())
  {
    SetupScanlines();
    GetImage().Clear();
  }

  return 1;
} // PreRender()


/**
* Setup the scanline ends.
*/
void vxRendererSoftwareMPRCurvedProjected::SetupScanlines()
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurvedProjected", "SetupScanlines" );
    return;
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurvedProjected", "SetupScanlines" );
    return;
  }

  // compute the ribbon
  pRibbon->ComputeScanlinesCurvedProjected(*pVessel, GetEnvironment(), GetImage().GetDim());

  SetRenderingNecessary(true);

} // SetupScanlines()


/*
* Render a section of the window
* this is ONLY for image space parallel algorithms
*
* @param window the part of the image to render
*
* @return true if computed
*/
int4 vxRendererSoftwareMPRCurvedProjected::RenderWindow(const BoundingRect<uint2> & window, const uint1 & uThreadID)
{

  if (GetRenderingNecessary() == false)
    return 1;

  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());
  if (pIntensityVolume == NULL)
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }
  
  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>(GetEnvironment().GetElement( vxIDs::Ribbon ));
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );
  Matrix<float4> mVolumeToWorld;
  mVolumeToWorld.Scale( Vector<float4>( units.X(), units.Y(), units.Z() ) );

  const uint4 uSliceWidth(GetImage().GetDim().m_x);
  const uint4 uSliceHeight(GetImage().GetDim().m_y);
  
  // whether to show the label in MPR
  bool bLabelsVisible( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );
    
  // get the label volume -- this is the detail label volume 
  vxBlockVolume<uint2> * pLabelVolume = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetLabelVolume());
  if (pLabelVolume == NULL)
  {
    LogDbg( "Label volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the reference point of Label Volume
  const Point<float4> referPntLabelVolume(  vxVesselUtilities::GetVolReferencePoint( pLabelVolume->GetHeader() ) );  
  const Vector3D<float4> labelUnits( pLabelVolume->GetHeader().GetVoxelUnits() );
  

  vxCamera & camera(pRibbon->GetRenderCamera());
  Triplef size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Box<float4> volBoundingBox(Box<float4>(0, 0, 0, size[0], size[1], size[2]));
  //Box<float4> volBoundingBox(camera.GetBoundingBox());
  Point<float4> viewCenter(camera.GetViewReference());
  Point<float4> eyePoint(camera.GetEye());
  //LogDbg( " center = " + ToAscii( viewCenter ), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
  //LogDbg( " eyePoint = " + ToAscii( eyePoint ), "vxRendererSoftwareMPRCurved", "RenderWindow"); 

  Matrix<float4> matTran;
  matTran.Translate( Vector3Df(viewCenter*(-1.0)) );  

  Normal<float4> viewXdir(camera.GetViewRight());
  Normal<float4> viewYdir(camera.GetViewUp());
  Normal<float4> viewZdir(camera.GetViewDirection());
  Matrix<float4> viewRotMat( viewXdir.X(),  viewXdir.Y(),   viewXdir.Z(),   0.0f, 
                             viewYdir.X(),  viewYdir.Y(),   viewYdir.Z(),   0.0f,
                             viewZdir.X(),  viewZdir.Y(),   viewZdir.Z(),   0.0f,
                             0.0f,          0.0f,           0.0f,           1.0f );
  /*
  LogDbg( "xdir = " + viewXdir.AsString(), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
  LogDbg( "ydir = " + viewYdir.AsString(), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
  LogDbg( "zdir = " + viewZdir.AsString(), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
  */

  Matrix<float4> matZMove;
  matZMove.Translate( Vector3Df(0.0, 0.0, 2*volBoundingBox.GetOuterRadius()) );
  //LogDbg( " Zmove = " + ToAscii( volBoundingBox.GetMaxZ() ), "vxRendererSoftwareMPRCurved", "RenderWindow"); 

  Matrix<float4> scalingMat;

  float4 scalingFactor =  uSliceWidth / pRibbon->GetRibbonWidthHalf();
  scalingMat.Scale( Vector<float4>( scalingFactor, scalingFactor, 1.0) ); // we donot need to zoomin Z.

  Matrix<float4>  viewMat = scalingMat*mWorldToVolume*matZMove*viewRotMat*matTran;

  //for parral only projection  - the new (x, y) is the projection position centered on image center  
  //LogDbg( " view center projection poisiton = " + (viewMat*viewCenter).AsString(), "vxRendererSoftwareMPRCurved", "RenderWindow"); 

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // see if the user wishes to display the graphs
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  float * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));
  
  if (pVessel->size() > 1)
  {
    // some items used frequently inside the loop
    Point<uint2> vPixelPosition(0,0,0);
    uint1 * pImage((uint1*)(GetImage().GetDataPtr()));
    const std::vector< vxComponent * > & vpComponentInfo = GetEnvironment().GetComponents()->ArrayByLabel();

    Point<float4> refImagePoint( 0.5f*uSliceWidth, 0.5f*uSliceHeight, 0.0f );
    //LogDbg( " refImagePoint in Render = " + refImagePoint.AsString(), "vxRendererSoftwareMPRCurved", "RenderWindow"); 

    Image<float4> zBuffer( Triple<uint4>(uSliceWidth, uSliceHeight, 0), 1.0e5 );
    //float4 *pZBuffer = reinterpret_cast<float4*>( zBuffer.GetDataPtrEx() );

    // loop over scanlines
    float4 maxZDepth(-1.0e5);
    float4 minZDepth(1.0e5);

    // one scanline is a rectangle
    float4 fPrevMiddlePixel(-1);
    int4 iNumOfScanLines(pRibbon->GetScanlineData().size()-1);

    for (int4 scanLine(window.GetMinY()); scanLine<window.GetMaxY(); scanLine++)
    { 
      Point<float4> leftPointWorld(pRibbon->GetScanlineData()[scanLine].m_leftPoint);
      Point<float4> rightPointWorld(pRibbon->GetScanlineData()[scanLine].m_rightPoint);
      Point<float4> middlePointWorld(pRibbon->GetScanlineData()[scanLine].m_centerlinePoint);
      Point<float4> leftPointVol(mWorldToVolume*leftPointWorld);
      Point<float4> rightPointVol(mWorldToVolume*rightPointWorld);
      Point<float4> middlePointVol(mWorldToVolume*middlePointWorld);
      Point<float4> leftPointImage(viewMat * leftPointWorld + Vector3Df(refImagePoint));
      Point<float4> rightPointImage(viewMat * rightPointWorld + Vector3Df(refImagePoint));
      Point<float4> middlePointImage(viewMat * middlePointWorld + Vector3Df(refImagePoint));

      const Vector<float4> majorDirectionVector( rightPointVol - leftPointVol );
      if ( majorDirectionVector.GetLength() < 1.0f )
        continue;

      // next lines points.
      Point<float4> leftPointWorldNext(pRibbon->GetScanlineData()[scanLine+1].m_leftPoint);
      Point<float4> rightPointWorldNext(pRibbon->GetScanlineData()[scanLine+1].m_rightPoint);
      Point<float4> middlePointWorldNext(pRibbon->GetScanlineData()[scanLine+1].m_centerlinePoint);
      Point<float4> leftPointVolNext(mWorldToVolume*leftPointWorldNext);
      Point<float4> rightPointVolNext(mWorldToVolume*rightPointWorldNext);
      Point<float4> middlePointVolNext(mWorldToVolume*middlePointWorldNext);
      Point<float4> leftPointImageNext(viewMat * leftPointWorldNext + Vector3Df(refImagePoint));
      Point<float4> rightPointImageNext(viewMat * rightPointWorldNext + Vector3Df(refImagePoint));
      Point<float4> middlePointImageNext(viewMat * middlePointWorldNext + Vector3Df(refImagePoint));

      const Vector<float4> majorDirectionVectorNext( rightPointVolNext - leftPointVolNext );
      if ( majorDirectionVectorNext.GetLength() < 1.0f )
        continue;


      // it is parallel line. skip the line outside the range
      int4 imageLine, imageLineNext, dl;
      if ( leftPointImageNext.m_y - leftPointImage.m_y > 0 ) 
      {
        imageLine = (int4)(leftPointImage.m_y) + 1;
        imageLineNext = (int4)(leftPointImageNext.m_y );
        dl = 1;
      }
      else
      {
        imageLine = (int4)(leftPointImage.m_y );
        imageLineNext = (int4)(leftPointImageNext.m_y) + 1; 
        dl = -1;
      }
      // not cover an imageline on screen
      if ( (  imageLine - leftPointImageNext.m_y ) * ( imageLine - leftPointImage.m_y ) > 0.0f )
      {
        //LogDbg("y and next y is : " + ToAscii(leftPointImageNext.m_y) + " , " +ToAscii(leftPointImage.m_y), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
        //LogDbg(" imageLine is : " + ToAscii(imageLine), "vxRendererSoftwareMPRCurved", "RenderWindow"); 
        continue;
      }

      vxBlockVolumeRayIterator<uint2>  iterRayIntensity( *pIntensityVolume);
      vxBlockVolumeRayIterator<uint2> iterRayLabel    ( *pLabelVolume    );
      
      // loop over all scan lines
      for ( int4 imgline=imageLine; imgline!=imageLineNext+dl; imgline += dl ) 
      {
        if ( imgline < 0 || imgline > uSliceHeight-1)
          continue;

        // point on image
        float4 imglineY((float4)( imgline ));

        const float4 w0(( leftPointImageNext.m_y - imglineY ) / ( leftPointImageNext.m_y - leftPointImage.m_y ));
        const float4 w1(1.0f - w0);

        const Point<float4> intpLeftPointImage(leftPointImage*w0 + Vector3Df(leftPointImageNext*w1));
        const Point<float4> intpRightPointImage(rightPointImage*w0 + Vector3Df(rightPointImageNext*w1));
        const Point<float4> intpMiddlePointImage(middlePointImage*w0 + Vector3Df(middlePointImageNext*w1));
        const Point<float4> intpLeftPointVol(leftPointVol*w0 + Vector3Df(leftPointVolNext*w1));
        const Point<float4> intpRightPointVol(rightPointVol*w0 + Vector3Df(rightPointVolNext*w1));

        Point<float4> intpLeftPointLabelVol, intpRightPointLebelVol;
        intpLeftPointLabelVol.m_x = ( intpLeftPointVol.m_x * units.X() - referPntLabelVolume.m_x ) / labelUnits.X();
        intpLeftPointLabelVol.m_y = ( intpLeftPointVol.m_y * units.Y() - referPntLabelVolume.m_y ) / labelUnits.Y();
        intpLeftPointLabelVol.m_z = ( intpLeftPointVol.m_z * units.Z() - referPntLabelVolume.m_z ) / labelUnits.Z();
            
        intpRightPointLebelVol.m_x = ( intpRightPointVol.m_x * units.X() - referPntLabelVolume.m_x ) / labelUnits.X();
        intpRightPointLebelVol.m_y = ( intpRightPointVol.m_y * units.Y() - referPntLabelVolume.m_y ) / labelUnits.Y();
        intpRightPointLebelVol.m_z = ( intpRightPointVol.m_z * units.Z() - referPntLabelVolume.m_z ) / labelUnits.Z();


        int4 iLeftPixel((int4)(intpLeftPointImage.m_x + 0.5f));
        int4 iRightPixel((int4)(intpRightPointImage.m_x + 0.5f));  
        int4 iMiddlePixel((int4)(intpMiddlePointImage.m_x + 0.5f));
        
        const Vector<float4> lineDirectionVector( intpRightPointVol - intpLeftPointVol );
        const Normal<float4> lineDirectionNormal( lineDirectionVector );
        const Vector3D<float4>lineDirectionVector3D( lineDirectionNormal.X(), lineDirectionNormal.Y(), lineDirectionNormal.Z() );
        const float4 fSampleDistance( lineDirectionVector.GetLength() / (iRightPixel-iLeftPixel+1) );
                
        const Vector<float4> lineDirectionVectorLabelVol( intpRightPointLebelVol - intpLeftPointLabelVol );
        const Normal<float4> lineDirectionNormalLabelVol(lineDirectionVectorLabelVol);
        const Vector3D<float4> lineDirectionVector3DLabelVol(lineDirectionNormalLabelVol.X(), lineDirectionNormalLabelVol.Y(), lineDirectionNormalLabelVol.Z());
        const float4 fSampleDistanceLabelVol(lineDirectionVectorLabelVol.GetLength() / (iRightPixel-iLeftPixel+1));


        Vector4d<float4> lineStartPointVol( intpLeftPointVol.m_x, intpLeftPointVol.m_y, intpLeftPointVol.m_z, 0 );
        Vector4d<float4> lineStartPointLabelVol( intpLeftPointLabelVol.m_x, intpLeftPointLabelVol.m_y, intpLeftPointLabelVol.m_z, 0 );
        if ( iLeftPixel < 0 )
        {
          lineStartPointVol = lineStartPointVol + Vector4d<float4>(lineDirectionVector3D*fSampleDistance*fabs(static_cast<float4>(iLeftPixel)), 0);
          lineStartPointLabelVol = lineStartPointLabelVol + Vector4d<float4>(lineDirectionVector3DLabelVol*fSampleDistanceLabelVol*fabs(static_cast<float4>(iLeftPixel)), 0);
          iLeftPixel = 0;
          //LogDbg(" change start point ", "vxRendererSoftwareMPRCurved", "RenderWindow");
        }
        iRightPixel = iRightPixel > uSliceWidth-1 ? uSliceWidth-1 : iRightPixel;

        RaySegment raySegmentIntensity( lineStartPointVol, Vector4Df(intpRightPointVol     ,1));
        RaySegment raySegmentLabel    ( lineStartPointLabelVol, Vector4Df(intpRightPointLebelVol,1));

        iterRayIntensity.SetRaySegment( raySegmentIntensity );
        iterRayLabel    .SetRaySegment( raySegmentLabel     );

        Vector4Df sampleIncVecIntensity( raySegmentIntensity.GetDirection().GetNormalizedXYZ() * fSampleDistance         );
        Vector4Df sampleIncVecLabel    ( raySegmentLabel    .GetDirection().GetNormalizedXYZ() * fSampleDistanceLabelVol );

        iterRayIntensity.SetSampleIncrementVector( sampleIncVecIntensity );
        iterRayLabel    .SetSampleIncrementVector( sampleIncVecLabel );

        float4 zDepth(0.5f*(intpLeftPointImage.m_z + intpRightPointImage.m_z )); //it is the same as right point
        maxZDepth = maxZDepth > zDepth ? maxZDepth : zDepth;
        minZDepth = minZDepth < zDepth ? minZDepth : zDepth;
        uint1 * pImageLine(pImage + 4*( uSliceWidth*imgline + iLeftPixel));

        uint2 uLastLabel(0), uCurrLabel(0), uNextLabel(0);
        uCurrLabel = iterRayLabel.GetVoxel();

        // loop over pixels in scanline
        for (int4 iPixelx(iLeftPixel); iPixelx<iRightPixel; ++iPixelx, ++iterRayIntensity )
        {
          Vector3D<int4> currLabelPos( iterRayLabel.GetPosInt() );
          float4 fLabel = iterRayLabel.GetSample();
          bool bLabelFound =  fLabel > 1.0e-3 ? TRUE : FALSE;

          ++iterRayLabel ;
          uNextLabel = iterRayLabel.GetVoxel();

          Point2D<uint2> pos( iPixelx, imgline );
          if ( pos.m_x < uSliceWidth && pos.m_y < uSliceHeight && zBuffer.GetPixel( pos ) > zDepth )
          {
            const uint4 uIntensity( iterRayIntensity.GetSample() );
            const uint1 uColor((uint2)(pfRGBA[ 4 * uIntensity + 3 ]) >> GetNumberOfShiftBitsFor8bitAlpha() );
            uint1 rColor(uColor);
            uint1 gColor(uColor);
            uint1 bColor(uColor);

            //bool bLabelFound = uLastLabel || uCurrLabel || uNextLabel; 
            if ( bLabelsVisible && bLabelFound )
            {
              bool bLabelShown = vpComponentInfo[ uLastLabel ]->IsLabelVisible() ||
                                 vpComponentInfo[ uCurrLabel ]->IsLabelVisible() ||
                                 vpComponentInfo[ uNextLabel ]->IsLabelVisible();

              if ( bLabelShown )
              {
                int2 fMaxLabelCount(8);
                int2 labelCount(0);

                vxBlockVolumeIterator<uint2> labelnVolNBIter( (*pLabelVolume), iterRayLabel.GetPosInt() );

                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.IncX();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.IncY();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.DecX();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.DecY();

                labelnVolNBIter.IncZ();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.IncX();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.IncY();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); labelnVolNBIter.DecX();
                labelCount += ( labelnVolNBIter.GetVoxel() ? 1 : 0 ); //labelnVolNBIter.DecY();
              
                uint2 uLabel = ( uCurrLabel ? uCurrLabel : ( uLastLabel ? uLastLabel : ( uNextLabel ? uNextLabel : 0 ) ) );
                if ( uLabel == 0 ) // should solved later .....
                  uLabel = (int2)(fLabel)+1;

                PixelRGBA<uint1> labelColor( vpComponentInfo[ uLabel ]->GetColor() );
                
                float4 fWeightLabel( (labelColor.A()/255.0f ) * (labelCount / fMaxLabelCount) );
                fWeightLabel = Bound( 0.0f, fWeightLabel, 1.0f );
                float4 fWeightIntensity( 1.0F - fWeightLabel );

                rColor = rColor*fWeightIntensity + labelColor.R() * fWeightLabel;
                gColor = gColor*fWeightIntensity + labelColor.G() * fWeightLabel;
                bColor = bColor*fWeightIntensity + labelColor.B() * fWeightLabel;
              }
            }

            /*
            const uint2 uLabel( iterRayLabel.GetVoxel() );

            if ( uLabel && bLabelsVisible && vpComponentInfo[ uLabel ]->IsLabelVisible() )
            {
              PixelRGBA<uint1> pixelColor( vpComponentInfo[ uLabel ]->GetColor() );
              *pImageLine++ = FileScope_InterpolateFixedPoint( uColor, pixelColor.GetRed  (), pixelColor.GetAlpha() );
              *pImageLine++ = FileScope_InterpolateFixedPoint( uColor, pixelColor.GetGreen(), pixelColor.GetAlpha() );
              *pImageLine++ = FileScope_InterpolateFixedPoint( uColor, pixelColor.GetBlue (), pixelColor.GetAlpha() );
            }
            else
            {
              *pImageLine++ = uColor; // red
              *pImageLine++ = uColor; // green
              *pImageLine++ = uColor; // blue
            }
            */


            *pImageLine++ = rColor; // red
            *pImageLine++ = gColor; // green
            *pImageLine++ = bColor; // blue
            *pImageLine++ = 255;    // alpha

            zBuffer.SetPixel( pos, zDepth );
          }
          else
          {
            pImageLine += 4;
          }

          uLastLabel = uCurrLabel;
          uCurrLabel = uNextLabel;

        } // loop over voxels on one imglines


        if ( iMiddlePixel < iLeftPixel || iMiddlePixel > iRightPixel-1 )
        {
          fPrevMiddlePixel = -1;
          continue;
        }

        if( pPreferences->GetDisplayCenterline() )
        {
          if ( fPrevMiddlePixel >= 0 )
            RenderCenterlineSpan( imgline, zDepth, zBuffer, make_pair( fPrevMiddlePixel, intpMiddlePointImage.m_x ) );
          else
            RenderCenterlineSpan( imgline, zDepth, zBuffer, make_pair( intpMiddlePointImage.m_x, intpMiddlePointImage.m_x ) );
        }

        // save previous pixel
        fPrevMiddlePixel = intpMiddlePointImage.m_x;

      } // loop over all image lines

    } // loop over scanlines

    //LogDbg("the ZDepth( " + ToAscii(minZDepth) + " , " + ToAscii(maxZDepth) + " ) ", "vxRendererSoftwareMPRCurved", "RenderWindow");

  } // if any nodes in skeleton
  else
  {
    GetImage().Clear(); 
  }

  return 1;
} // RenderWindow()


/**
 * Helper function to overlay one pixel over another
 *
 * @param front the front pixel
 * @param back  the back pixel
 *
 * @return      the result
 */
PixelRGBA<uint1> Overlay( const PixelRGBA<uint1> & front, const PixelRGBA<uint1> & back )
{
  float4 fFrontOpacity( front.A() / 255.0F );
  float4 fBackOpacity( back.A() / 255.0F );
  float4 fRed  ( Bound( 0.0F, back.R() + front.R() * fFrontOpacity, 255.0F ) );
  float4 fGreen( Bound( 0.0F, back.G() + front.G() * fFrontOpacity, 255.0F ) );
  float4 fBlue ( Bound( 0.0F, back.B() + front.B() * fFrontOpacity, 255.0F ) );
  float4 fAlpha( fFrontOpacity + ( 1.0F - fFrontOpacity ) * fBackOpacity );
  return PixelRGBA<uint1>( fRed, fGreen, fBlue, fAlpha * 255.0F );
} // Overlay()


/**
 * Render the centerline for one scanline.
 *
 * @param iScanline
 * @param fDepthZ
 * @param zBuffer
 * @param ySpan
 */
void vxRendererSoftwareMPRCurvedProjected::RenderCenterlineSpan( const int4 iScanline, const float4 fDepthZ, Image<float4> & zBuffer, const std::pair< float4, float4 > & ySpan )
{
  PixelRGBA<uint1> middleColor( 0, 255, 0, 255 );
  const float4 fLeftPixel( min( ySpan.first, ySpan.second ) );
  const float4 fRightPixel( max( ySpan.first, ySpan.second ) );
  const int4 iLeftPixel( ceil( fLeftPixel ) );
  const int4 iRightPixel( floor( fRightPixel ) );

  // draw the line in between left and right
  for ( int4 x( iLeftPixel ); x <= iRightPixel; x++ )
  {
    Point2D<uint2> midPos( x, iScanline);
    if( midPos.m_x < GetImage().GetDim().m_x && midPos.m_y < GetImage().GetDim().m_y )
    {
      if ( fabs( fDepthZ - zBuffer.GetPixel( midPos ) ) < 1.0e-3 )
      {
        PixelRGBA<uint1> midColor = middleColor;
        GetImage().SetPixel( midPos, midColor );
        zBuffer.SetPixel( midPos, fDepthZ-0.05F );
      }
    }
  }

  // antialias the left side
  float4 fDistance( floor( fLeftPixel ) - fLeftPixel );
  Point2D<uint2> leftPos( floor( fLeftPixel ), iScanline);
  if( leftPos.m_x < GetImage().GetDim().m_x && leftPos.m_y < GetImage().GetDim().m_y )
  {
    PixelRGBA<uint1> backColor = GetImage().GetPixel( leftPos );
    PixelRGBA<uint1> frontColor( middleColor );
    frontColor.A() = fDistance * 64.0F;
    PixelRGBA<uint1> result( Overlay( frontColor, backColor ) );    
    //frontColor.Composite( backColor );
    GetImage().SetPixel( leftPos, result );
  }

  // antialias the right side
  fDistance = fRightPixel - ceil( fRightPixel );
  Point2D<uint2> rightPos( ceil( fRightPixel ), iScanline);
  if( rightPos.m_x < GetImage().GetDim().m_x && rightPos.m_y < GetImage().GetDim().m_y )
  {
    PixelRGBA<uint1> backColor( GetImage().GetPixel( rightPos ) );
    PixelRGBA<uint1> frontColor( middleColor );
    frontColor.A() = fDistance * 255.0F;
    PixelRGBA<uint1> result( Overlay( frontColor, backColor ) );    
    //frontColor.Composite( backColor );
    GetImage().SetPixel( rightPos, result );
  }

} // RenderCenterlineSpan()


// undefines
#undef FILE_REVISION


// $Log: vxRendererSoftwareMPRCurvedProjected.C,v $
// Revision 1.2  2006/09/25 13:23:35  geconomos
// commented out unneeded  log debugging
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:02:09  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/09/02 16:24:53  frank
// decreased the granularity of component array modified bits
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.5  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.4  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.3  2004/04/06 12:06:49  frank
// skeleton not used in curved MPR
//
// Revision 1.2  2004/03/08 19:04:21  geconomos
// Updated to compile with vc++ 7.1 compiler
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.11.2.1  2004/02/25 15:00:17  frank
// formatting
//
// Revision 1.11  2004/01/15 17:44:53  wenli
// Add GetVolReferencePoint()
//
// Revision 1.10  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.9  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.8  2003/06/16 13:57:54  michael
// replaced 12 bit hard coded color conversion to use data dependent shifting
//
// Revision 1.7  2003/06/12 21:21:29  dongqing
// fix MR case of MPR curved image panel
//
// Revision 1.6  2003/06/09 12:29:52  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.5  2003/06/04 19:05:48  michael
// added namespace declaration
//
// Revision 1.4  2003/05/22 14:56:32  dongqing
// code review
//
// Revision 1.3  2003/05/21 20:44:48  wenli
// code review
//
// Revision 1.2  2003/05/16 16:46:31  frank
// reflected interface change to Image class
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.8  2003/03/21 19:26:49  frank
// Fixed handle selection and flipped vessel so that up is up
//
// Revision 1.7  2003/03/12 22:38:51  geconomos
// Commented out code to initialize the logger.
//
// Revision 1.6  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.5  2003/02/11 20:07:25  frank
// Moved function to CurvedProjected
//
// Revision 1.4  2003/02/11 18:45:55  wenli
// fix the centerlines and iterator problems in CurvedMPRProject
//
// Revision 1.3.2.1  2003/02/12 23:10:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/01/24 20:17:12  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.2  2003/01/24 18:26:14  frank
// Implemented curved
//
// Revision 1.1  2003/01/21 20:24:48  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPRCurvedProjected.C,v 1.2 2006/09/25 13:23:35 geconomos Exp $
// $Id: vxRendererSoftwareMPRCurvedProjected.C,v 1.2 2006/09/25 13:23:35 geconomos Exp $=
