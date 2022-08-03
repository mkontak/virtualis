// $Id: SliceViewer.C,v 1.33.2.19 2010/04/22 18:34:09 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.net)


/**
 * A class to support viewing of volume slices
 */


// includes
#include "stdafx.h"
#include <GL/gl.h>
#include <stack>
#include "SliceViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"
#include "VCSettings.h"


// defines
#define FILE_REVISION "$Revision: 1.33.2.19 $"

// namespaces
using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;
using namespace vxCAD;

// static member initialization
const uint4 SliceViewer::m_uCameraSliceDiffThreshold(30);
const uint4 SliceViewer::m_uAnnotationSliceDiffThreshold(4);
const float4 SliceViewer::m_uCameraArrowLength(25.0F);
bool SliceViewer::m_bShowUncleansed(false);
bool SliceViewer::m_bFlightAlongCenterline(false);
bool SliceViewer::m_bArrowInSliceCenter(false);
vxSliceRenderer::vxSliceRenderingModeEnum SliceViewer::m_eGlobalRenderMode(vxSliceRenderer::Default);
float4 SliceViewer::m_fGlobalSlabThickness = 10.0f;
bool SliceViewer::m_bRenderCamera(true);


/**
 * Constructor
 */
SliceViewer::SliceViewer() :
m_puSliceImage(NULL),
m_puObliqueImage(NULL),
m_growSliceImage(false),
m_uSliceWidthMax(512),
m_uSliceHeightMax(512),
m_uObliqueImageSize(128),
m_bForceSliceRender(false),
m_bSliceRendererEnabled( true )
{
  Initialize();
} // Constructor()


/**
 * Initialize this SliceViewer.
 */
void SliceViewer::Initialize()
{
  m_eSliceOrientation = SliceOrientations::eAXIAL;
  
  m_uDisplayWidth = m_uDisplayHeight = 0;
  m_uSliceWidth = m_uSliceHeight = m_uSliceSize = 0;
  m_uNumOfSlices = 0;
  m_iSliceNumber = -1;
  m_scale = Triple<float4>(1.0F, 1.0F, 1.0F);
  m_crop = Triple<float4>(0.0F, 0.0F, 0.0F);

  m_bVolumeInitialed = false;
  m_uRenderHeight = m_uRenderWidth = 0;
  m_fWorldWidth = m_fWorldHeight = 0;

  m_eDatasetType = DatasetOrientations::ePRIMARY;

  m_bDisplayOverviewSliceGrasp = false;
  m_bBlockedPixel = false;

  if (m_puObliqueImage)  delete [] m_puObliqueImage;
  m_puObliqueImage = new uint1 [m_uObliqueImageSize * m_uObliqueImageSize];

  if (m_puSliceImage)  delete [] m_puSliceImage;
  m_puSliceImage = new uint1 [m_uSliceWidthMax * m_uSliceHeightMax];
  m_iCurrentSliceNo = -1;

  m_fSizeWorldUnits = 50.0F; // world units (like field of view)
  m_fSkeletonIndex = -1.0F;

  // m_fCrossSecDistance = 20.0F;
  m_fViewDirDistance = 20.0F;

  m_rSelect = Rect<uint4>(9999, 9999, 9999, 9999);
  m_viStartMovePt = Point2D<int4>(9999, 9999);
  m_vfAnchorPt = Point2D<float4>(-1.0f, -1.0f);
  m_rZoom = Rect<float4>(0.0F, 0.0F, 1.0F, 1.0F);

  m_growSliceImage = Image<bool>(Triple<uint2>(1024, 1024, 0), false);  // Registration stuff.

  m_sliceRenderer.SetObliqueImageSize( m_uObliqueImageSize, m_uObliqueImageSize );

} // Initialize().


/**
 * Destructor
 */
SliceViewer::~SliceViewer()
{
  if (m_puSliceImage)    delete [] m_puSliceImage;
  if (m_puObliqueImage)  delete [] m_puObliqueImage;
} // Destructor().


/**
 * Copy constructor
 *
 * @param other   Source
 */
SliceViewer::SliceViewer(const SliceViewer & other) : m_puSliceImage(NULL), m_puObliqueImage(NULL), m_growSliceImage(false),
  m_uSliceWidthMax(512), m_uSliceHeightMax(512), m_uObliqueImageSize(128)
{
  Initialize();
  *this = other;
} // Copy constructor().


/**
 * Assignment operator
 *
 * @param other   Source
 */
SliceViewer & SliceViewer::operator =(const SliceViewer & other)
{
  // todo: write function

  return * this;
} // operator=


/**
 * Sets the slice orientation
 *
 * @param   sliceOrientation  orientation to be set
 */
void SliceViewer::SetOrientation(const SliceOrientations::TypeEnum eSliceOrientation)
{
  m_eSliceOrientation = eSliceOrientation;
  InitVolumeLoaded();
} // SetOrientation().


/**
 * Initialize volume parameters (from .ecv volume)
 */
void SliceViewer::InitVolumeLoaded()
{
  uint1 uint1Dummy = 0;
  Dataset & myDataset = * GetDataset();

  m_sliceRenderer.SetData( &myDataset.m_fields.m_sliceRendererData );
  bool bHQ2Don(true);
  if( rdrGlobal.GetSettings()->GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) <= 8 )
    bHQ2Don = false;
  m_bSliceRendererEnabled =  bHQ2Don && myDataset.m_fields.m_sliceRendererData.IsValid();

  const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);
  const Triple<float4> units(m_bSliceRendererEnabled ? myDataset.m_fields.m_units2D : myDataset.m_vUnits);

  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    m_uVolumeWidth  = myDataset.m_vDim.m_x;
    m_uVolumeHeight = myDataset.m_vDim.m_y;
    m_uSliceWidth   = GetMaxPowerOf2(dim.m_x);
    m_uSliceHeight  = GetMaxPowerOf2(dim.m_y);
    m_fWorldWidth = dim.m_x * units.m_x;
    m_fWorldHeight = dim.m_y * units.m_y;
    m_uNumOfSlices  = dim.m_z;
    m_scale.m_x = float4(dim.m_x)/float4(myDataset.m_vDim.m_x);
    m_scale.m_y = float4(dim.m_y)/float4(myDataset.m_vDim.m_y);
    m_scale.m_z = float4(dim.m_z)/float4(myDataset.m_vDim.m_z);
    m_crop.m_x = 1.0F - (float4)dim.m_x / (float4)m_uSliceWidth;
    m_crop.m_y = 1.0F - (float4)dim.m_y / (float4)m_uSliceHeight;
    m_crop.m_z = 0.0F;
    m_iCurrentSliceNo = -1;
    m_sliceRenderer.SetOrientation( vxSliceRenderer::Axial );
    break;
  case eSAGITTAL:
    m_uVolumeWidth  = myDataset.m_vDim.m_z;
    m_uVolumeHeight = myDataset.m_vDim.m_y;
    m_uSliceWidth  = GetMaxPowerOf2(dim.m_z);
    m_uSliceHeight = GetMaxPowerOf2(dim.m_y);
    if ( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
    {
      m_fWorldWidth = dim.m_y * units.m_y;
      m_fWorldHeight = dim.m_z * units.m_z;
    }
    else
    {
      m_fWorldWidth = dim.m_z * units.m_z;
      m_fWorldHeight = dim.m_y * units.m_y;
    }
    m_uNumOfSlices  = dim.m_x;
    m_scale.m_x = float4(dim.m_x)/float4(myDataset.m_vDim.m_x);
    m_scale.m_y = float4(dim.m_y)/float4(myDataset.m_vDim.m_y);
    m_scale.m_z = float4(dim.m_z)/float4(myDataset.m_vDim.m_z);
    m_crop.m_x = 1.0F - (float4)dim.m_z / (float4)m_uSliceWidth;
    m_crop.m_y = 1.0F - (float4)dim.m_y / (float4)m_uSliceHeight;
    m_crop.m_z = 0.0F;
    m_iCurrentSliceNo = -1;
    m_sliceRenderer.SetOrientation( vxSliceRenderer::Sagittal );
    break;
  case eCORONAL:
    m_uVolumeWidth  = myDataset.m_vDim.m_x;
    m_uVolumeHeight = myDataset.m_vDim.m_z;
    m_uSliceWidth   = GetMaxPowerOf2(dim.m_x);
    m_uSliceHeight  = GetMaxPowerOf2(dim.m_z);
    m_fWorldWidth = dim.m_x * units.m_x;
    m_fWorldHeight = dim.m_z * units.m_z;
    m_uNumOfSlices  = dim.m_y;
    m_scale.m_x = float4(dim.m_x)/float4(myDataset.m_vDim.m_x);
    m_scale.m_y = float4(dim.m_y)/float4(myDataset.m_vDim.m_y);
    m_scale.m_z = float4(dim.m_z)/float4(myDataset.m_vDim.m_z);
    m_crop.m_x = 1.0F - (float4)dim.m_x / (float4)m_uSliceWidth;
    m_crop.m_y = 1.0F - (float4)dim.m_z / (float4)m_uSliceHeight;
    m_iCurrentSliceNo = -1;
    m_sliceRenderer.SetOrientation( vxSliceRenderer::Coronal );
    break;
  case eCROSS_SECTION:
    m_uNumOfSlices  = myDataset.m_joinedSkeleton.GetNumOfElements();
    m_crop.m_x = 0.0F;
    m_crop.m_y = 0.0F;
    m_uSliceWidth   = m_uObliqueImageSize;
    m_uSliceHeight  = m_uObliqueImageSize;
    m_fWorldWidth = 1;
    m_fWorldHeight = 1;
    m_sliceRenderer.SetOrientation( vxSliceRenderer::Oblique );
    m_iCurrentSliceNo = -1;
    break;
  case eVIEWDIR:
    m_crop.m_x = 0.0F;
    m_crop.m_y = 0.0F;
    m_uSliceWidth   = m_uObliqueImageSize;
    m_uSliceHeight  = m_uObliqueImageSize;
    m_fWorldWidth = 1;
    m_fWorldHeight = 1;
    m_sliceRenderer.SetOrientation( vxSliceRenderer::Oblique );
    break;
  default:
    // todo: log error
    break;
  } // end-switch().

  m_uSliceSize = m_uSliceWidth * m_uSliceHeight;
} // InitVolumeLoaded().


/**
 * Sets up the OpenGL rendering context
 */
void SliceViewer::SetupOpenGLContext()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Initialize the projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} // SetupOpenGLContext().


/**
 * Resizes the display window
 *
 * @param   width    width of the display window.
 * @param   height   height of the display window.
 */
void SliceViewer::ResizeDisplay(const uint4 uWidth, const uint4 uHeight)
{
  // save the display size for later
  m_uDisplayWidth = uWidth;
  m_uDisplayHeight = (uHeight == 0) ? 1 : uHeight;
} // ResizeDisplay().


/**
 *  get current orth slice number/distance
 *
 *  @return the slice number/distance 
 */
uint4 SliceViewer::GetCurrentOrthoSlice() const
{
  try
  {
    Dataset & myDataset = * GetDataset();

    // should we use the current viewpoint
    switch (m_eSliceOrientation) 
    {
    case eAXIAL:
      return myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z + 0.5F;
      break;

    case eSAGITTAL:
      return myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x + 0.5F;
      break;
    
    case eCORONAL:
      return myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y + 0.5F;
      break;
    
    case eCROSS_SECTION:
      return myDataset.m_iXsectionScrollBarLocationIndex;
      break;

    case eVIEWDIR:
      return GetCurrentDisplayedSlice();
      break;

    default:
      // todo: log error
      break;
    } // endswitch m_eSliceOrientation

    return 0;
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceViewer", "GetCurrentOrthoSlice" );
  }
  return 0;
} // GetCurrentOrthoSlice()


/**
 *  reset current volume position.
 */
void SliceViewer::ResetCurrentVolumePosition()
{
  Dataset *myDataset = GetDataset();

  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    myDataset->m_vOrthogonalPtWorld.m_z = myDataset->m_submarine.GetPositionWorld().m_z;
    m_iSliceNumber = myDataset->m_vOrthogonalPtWorld.m_z / myDataset->m_fields.m_units2D.m_z + 0.5F;
    break;
  case eSAGITTAL:
    myDataset->m_vOrthogonalPtWorld.m_x = myDataset->m_submarine.GetPositionWorld().m_x;
    m_iSliceNumber = myDataset->m_vOrthogonalPtWorld.m_x / myDataset->m_fields.m_units2D.m_x + 0.5F;
    break;
  case eCORONAL:
    myDataset->m_vOrthogonalPtWorld.m_y = myDataset->m_submarine.GetPositionWorld().m_y;
    m_iSliceNumber = myDataset->m_vOrthogonalPtWorld.m_y / myDataset->m_fields.m_units2D.m_y + 0.5F;
    break;
  case eCROSS_SECTION:
    myDataset->m_iXsectionScrollBarLocationIndex = myDataset->m_joinedSkeleton.ConvertToJoinedIndex( myDataset->m_segmentList.GetCurrentSegment(), 
                                                       myDataset->m_fields.GetClosestSkeletonIndex(myDataset->m_submarine.GetPositionVolume()) );
    m_iSliceNumber = myDataset->m_iXsectionScrollBarLocationIndex;
    break;
  case eVIEWDIR:
    m_iSliceNumber = m_fViewDirDistance = 0;
    break;
  default:
    break;
  } // endswitch m_eSliceOrientation
} // ResetCurrentVolumePosition().


/**
 * Scrolls the slice by a specified amount
 *
 * @param count      scroll amount
 * @param bAutoFly   true when auto-(centerline)flight.
 * @return           only valid during 2D flights, -1: stop auto-fly
 */
int4 SliceViewer::Scroll(const int4 iCount, const bool bMainView)
{
  try
  {
    Dataset & myDataset = * GetDataset();
    const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);

    switch (m_eSliceOrientation) 
    {
    case eAXIAL:
      myDataset.m_vOrthogonalPtWorld.m_z += iCount * myDataset.m_fields.m_units2D.m_z;
      if (myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z > dim.m_z-1)
        myDataset.m_vOrthogonalPtWorld.m_z = (dim.m_z-1) * myDataset.m_fields.m_units2D.m_z;
      if (myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z < 0)
        myDataset.m_vOrthogonalPtWorld.m_z = 0;
      break;

    case eSAGITTAL:
      myDataset.m_vOrthogonalPtWorld.m_x += iCount * myDataset.m_fields.m_units2D.m_x;

      if(myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x > dim.m_x-1)
        myDataset.m_vOrthogonalPtWorld.m_x = (dim.m_x-1) * myDataset.m_fields.m_units2D.m_x;
      if(myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x < 0)
        myDataset.m_vOrthogonalPtWorld.m_x = 0;
      break;

    case eCORONAL:
      myDataset.m_vOrthogonalPtWorld.m_y += iCount * myDataset.m_fields.m_units2D.m_y;

      if(myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y > dim.m_y-1)
      {
        myDataset.m_vOrthogonalPtWorld.m_y = (dim.m_y-1) * myDataset.m_fields.m_units2D.m_y;
      }
      if(myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y < 0)
      {
        myDataset.m_vOrthogonalPtWorld.m_y = 0;
      }
      break;

    case eCROSS_SECTION:
      myDataset.m_iXsectionScrollBarLocationIndex += iCount;

      if( myDataset.m_iXsectionScrollBarLocationIndex < 0 )
        myDataset.m_iXsectionScrollBarLocationIndex = 0;
      else if( myDataset.m_iXsectionScrollBarLocationIndex >= myDataset.m_joinedSkeleton.GetNumOfElements() )
        myDataset.m_iXsectionScrollBarLocationIndex = myDataset.m_joinedSkeleton.GetNumOfElements()-1;
      break;

    case eVIEWDIR:
      if (m_bFlightAlongCenterline && bMainView)
      {
        return Scroll(iCount, true);
      }
      else 
      {
        m_fViewDirDistance += iCount;
        m_fViewDirDistance = Bound(0.0F, m_fViewDirDistance, float4(GetNumberOfSlices()-1));
      }
      break;

    default:
      // todo: log error
      break;
    } // endswitch m_eSliceOrientation

    SetSliceNumber();

    return 0;
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceViewer", "Scroll" );
  }
  return 0;
} // Scroll().


/**
 * Scroll along centerline : call this function for 2D slice flight. 
 *
 * @param bAutoFly   true when auto-(centerline)flight.
 * @return           valid only during in 2D flights, -1: stop auto-fly
 */
int4 SliceViewer::ScrollToSegmentEnd(const bool bAutoFly)
{ 
  Dataset & myDataset = * GetDataset();

  uint2 uSkeletonIndex = myDataset.m_fields.GetClosestSkeletonIndex( myDataset.m_submarine.GetPositionVolume() );
  uint2 uNumOfElements = myDataset.m_segmentList.GetCurrentSkeleton().GetNumOfElements();
  bool bRegular = myDataset.m_segmentList.GetCurrentSegment().IsRegularDirection();
  bool bFlip;
  if ( myDataset.m_submarine.GetFlightDirection() == Submarine::CECUM_TO_RECTUM )
    bFlip = bRegular;
  else
    bFlip = !bRegular;
  
  if ( ((myDataset.m_submarine.GetFlightDirection() == Submarine::CECUM_TO_RECTUM) && !bRegular)
    || ((myDataset.m_submarine.GetFlightDirection() == Submarine::RECTUM_TO_CECUM) && bRegular) )
  {
    int2 iIncrease = 0;
    Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
    SkeletonNode skelnode = skeleton.GetSkelNode(uSkeletonIndex+iIncrease);
    myDataset.m_submarine.SetViewSkeleton(skelnode, bFlip);
    while ( ! (myDataset.m_fields.GetClosestSkeletonIndex( myDataset.m_submarine.GetPositionVolume() ) > uSkeletonIndex) )
    {
      iIncrease ++;
      Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
      SkeletonNode skelnode = skeleton.GetSkelNode(uSkeletonIndex+iIncrease);
      myDataset.m_submarine.SetViewSkeleton(skelnode, bFlip);
      // if reach the end ...
      if (uSkeletonIndex+iIncrease >= uNumOfElements) 
      {
        SetSliceNumber();
        return -1;
      }
    }
  }
  else
  {
    int2 iIncrease = -1;
    Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
    SkeletonNode skelnode = skeleton.GetSkelNode(uSkeletonIndex+iIncrease);
    myDataset.m_submarine.SetViewSkeleton(skelnode, bFlip);
    while ( ! (myDataset.m_fields.GetClosestSkeletonIndex( GetDataset()->m_submarine.GetPositionVolume() ) < uSkeletonIndex) && uSkeletonIndex>0)
    {
      Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
      SkeletonNode skelnode = skeleton.GetSkelNode(uSkeletonIndex+iIncrease);
      myDataset.m_submarine.SetViewSkeleton(skelnode, bFlip);
      iIncrease --;
      // if reach the start point ...
      if (uSkeletonIndex+iIncrease <= 0)       
      {
        SetSliceNumber();
        return -1;
      }
    }
  }

  return 0;
} // ScrollToSegmentEnd


/**
 * Step flight along centerline
 *
 * @param bAutoFly   true when auto-(centerline)flight.
 * @return           valid only during in 2D flights, -1: stop auto-fly
 */
int4 SliceViewer::StepFlight2D(const bool bStepForward)
{
  Dataset & myDataset = * GetDataset();

  if (myDataset.m_submarine.GetFlightDirection() == Submarine::CECUM_TO_RECTUM)
  {
    if (myDataset.m_segmentList.GetCurrentSegment().IsRegularDirection())
    {
      if (bStepForward)
        return StepBackward(true);
      else
        return StepForward(true);
    }
    else
    {
      if (bStepForward)
        return StepForward(false);
      else
        return StepBackward(false);
    }
  }
  else
  {
    if (myDataset.m_segmentList.GetCurrentSegment().IsRegularDirection())
    {
      if (bStepForward)
        return StepForward(false);
      else
        return StepBackward(false);
    }
    else
    {
      if (bStepForward)
        return StepBackward(true);
      else
        return StepForward(true);
    }
  }

  return 0;
} // StepFlight2D().


/**
 * Step forward : step flight
 *
 * @param bFlip   Whether or not to flip
 * @return        Zero if successful, 
 */
int4 SliceViewer::StepForward(const bool bFlip)
{

  Dataset & myDatasetRef = * GetDataset();
  uint2 uNumOfElements = myDatasetRef.m_segmentList.GetCurrentSkeleton().GetNumOfElements();
  uint2 uSkeletonIndex = (float4)myDatasetRef.m_submarine.GetClosestSkeletonIndex();

  uint2 uIncrease = 0;
  Skeleton & skeleton = myDatasetRef.m_segmentList.GetCurrentSkeleton();
  myDatasetRef.m_submarine.SetViewSkeleton(uSkeletonIndex+uIncrease, bFlip);
  
  while ( !(myDatasetRef.m_submarine.GetClosestSkeletonIndex() > uSkeletonIndex) )
  {
    uIncrease ++;
    Skeleton & skeleton = myDatasetRef.m_segmentList.GetCurrentSkeleton();
    myDatasetRef.m_submarine.SetViewSkeleton(uSkeletonIndex+uIncrease, bFlip);
    // if reach the end ...
    if (uSkeletonIndex+uIncrease >= uNumOfElements) 
    {
      SetSliceNumber();
      return -1;
    }
  }

  return 0;
} // StepForward()


/**
 * Step backward : step flight
 *
 * @param bFlip   Whether or not to flip
 * @return        Zero if successful, 
 */
int4 SliceViewer::StepBackward(const bool bFlip)
{
  Dataset & myDatasetRef = * GetDataset();
  uint2 uSkeletonIndex = myDatasetRef.m_submarine.GetClosestSkeletonIndex();
  uint2 uNumOfElements = myDatasetRef.m_segmentList.GetCurrentSkeleton().GetNumOfElements();

  int2 iIncrease = 0;
  Skeleton & skeleton = myDatasetRef.m_segmentList.GetCurrentSkeleton();
  myDatasetRef.m_submarine.SetViewSkeleton(uSkeletonIndex+iIncrease, bFlip);
  while ( !(myDatasetRef.m_submarine.GetClosestSkeletonIndex() < uSkeletonIndex) && uSkeletonIndex>0)
  {
    Skeleton & skeleton = myDatasetRef.m_segmentList.GetCurrentSkeleton();
    myDatasetRef.m_submarine.SetViewSkeleton(uSkeletonIndex+iIncrease, bFlip);
    iIncrease --;
    // if reach the start point ...
    if (uSkeletonIndex+iIncrease <=0)       
    {
      SetSliceNumber();
      return -1;
    }
  }

  return 0;
} // StepBackward()


/**
 * Scrolls the the specified distance.
 *
 * @param iSliceOrDistance   Slice (axials) or Distance (multi-planar) to which to scroll
 */
void SliceViewer::ScrollTo(const int4 iSliceOrDistance)
{
  Dataset & myDataset = * GetDataset();
  const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);

  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    myDataset.m_vOrthogonalPtWorld.m_z = iSliceOrDistance * myDataset.m_fields.m_units2D.m_z;
    if (myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z > dim.m_z-1)
      myDataset.m_vOrthogonalPtWorld.m_z = (dim.m_z-1) * myDataset.m_fields.m_units2D.m_z;
    if (myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z < 0)
      myDataset.m_vOrthogonalPtWorld.m_z = 0;
    break;

  case eSAGITTAL:
    myDataset.m_vOrthogonalPtWorld.m_x = iSliceOrDistance * myDataset.m_fields.m_units2D.m_x;

    if(myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x > dim.m_x-1)
      myDataset.m_vOrthogonalPtWorld.m_x = (dim.m_x-1) * myDataset.m_fields.m_units2D.m_x;
    if(myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x < 0)
      myDataset.m_vOrthogonalPtWorld.m_x = 0;
    break;

  case eCORONAL:
    myDataset.m_vOrthogonalPtWorld.m_y = iSliceOrDistance * myDataset.m_fields.m_units2D.m_y;

    if(myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y > dim.m_y-1)
      myDataset.m_vOrthogonalPtWorld.m_y = (dim.m_y-1) * myDataset.m_fields.m_units2D.m_y;
    if(myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y < 0)
      myDataset.m_vOrthogonalPtWorld.m_y = 0;
    break;

  case eCROSS_SECTION:
    if (iSliceOrDistance < 0)
      myDataset.m_iXsectionScrollBarLocationIndex = 0;
    else if (iSliceOrDistance >= myDataset.m_joinedSkeleton.GetNumOfElements() )   
      myDataset.m_iXsectionScrollBarLocationIndex = myDataset.m_joinedSkeleton.GetNumOfElements()-1;
    else
      myDataset.m_iXsectionScrollBarLocationIndex = iSliceOrDistance;
    break;

  case eVIEWDIR:
    m_fViewDirDistance = Bound(0.0F, float4(iSliceOrDistance), float4(GetNumberOfSlices()-1));
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation
  SetSliceNumber();

} // ScrollTo()


/**
 * call this function to implement 2D auto-fly
 *
 * @return   -1 if autoflight reached end of a segment (and thus should be turned off), 0 otherwise.
 */
int4 SliceViewer::AutoFlight2D()
{ 
  Dataset & myDatasetRef = * GetDataset();

  float4 fSpeed = myDatasetRef.m_submarine.GetThrottle();
  // correct flight direction.
  if (myDatasetRef.m_submarine.GetFlightDirection() == Submarine::CECUM_TO_RECTUM) fSpeed = -fSpeed;
  if (!(myDatasetRef.m_segmentList.GetCurrentSegment().IsRegularDirection()))      fSpeed = -fSpeed;

  if (m_fSkeletonIndex < 0.0f)
  {
    m_fSkeletonIndex = (float4)myDatasetRef.m_submarine.GetClosestSkeletonIndex();
    // adjustment to avoid jumping.
    if (fSpeed<=0) m_fSkeletonIndex = m_fSkeletonIndex - 1.0f;
  }

  // auto flight...
  m_fSkeletonIndex += fSpeed;
  if ( (m_fSkeletonIndex > float4(myDatasetRef.m_segmentList.GetCurrentSkeleton().GetNumOfElements())) || (m_fSkeletonIndex < 0.0f) )
  {
    m_fSkeletonIndex = myDatasetRef.m_segmentList.GetCurrentSkeleton().GetNumOfElements();
    SetSliceNumber();
    return -1;
  }
  myDatasetRef.m_submarine.SetViewSkeleton(m_fSkeletonIndex, !(myDatasetRef.m_segmentList.GetCurrentSegment().IsRegularDirection()));
  return 0;
} // AutoFlight2D()


/**
 * set viewport, (Square viewport).
 *
 * @param fLeft     left edge of viewport
 * @param fBottom   bottom edge of viewport
 * @param bDummy    not used.
 */
void SliceViewer::SetViewport(const float4 fLeft, const float4 fBottom, bool bDummy)
{
  // set up the viewport, a SQUARE view not, not as that above.
  if (m_uDisplayHeight > m_uDisplayWidth)
  {
    float4 fShiftY = (m_uDisplayHeight-m_uDisplayWidth) / 2.0F;
    glViewport(fLeft, fBottom+fShiftY, m_uDisplayWidth, m_uDisplayWidth);
  }
  else
  {
    float4 fShiftX = (m_uDisplayWidth-m_uDisplayHeight) / 2.0F;
    glViewport(fLeft+fShiftX, fBottom, m_uDisplayHeight, m_uDisplayHeight);
  }
} // SetViewport()


/**
 * get viewport rectangle (tall narrow or wide short viewport: no good!!!)
 *
 * @param fLeft     Left edge of viewport
 * @param fBottom   Bottom edge of viewport
 * @return          viewport rectangle
 */
Rect<int4> SliceViewer::GetViewportRect(const float4 fLeft, const float4 fBottom)
{
  /// no, no, this viewport setting is definitely no-good -> lots of problems may cause.
  Rect<int4> rtnRect;

  // setup the viewport so the aspect ratio is correct
  float4 fSliceAspectRatio = float4(m_fWorldHeight) / float4(m_fWorldWidth);
  float4 fDisplayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (fDisplayAspectRatio < fSliceAspectRatio) 
  {
    // tall and narrow slice
    int4 iViewportHeight = m_uDisplayHeight;
    int4 iViewportWidth = iViewportHeight / fSliceAspectRatio;
    rtnRect = Rect<int4>(Point2D<int4>((m_uDisplayWidth-iViewportWidth)/2 + fLeft, fBottom), Point2D<int4>(iViewportWidth, iViewportHeight));
  }
  else
  {
    // wide and short slice
    int4 iViewportWidth = m_uDisplayWidth;
    int4 iViewportHeight = iViewportWidth * fSliceAspectRatio; // (iViewportWidth / (1/fSliceAspectRatio)
    rtnRect = Rect<int4>(Point2D<int4>(fLeft, ((m_uDisplayHeight-iViewportHeight)/2 + fBottom)), Point2D<int4>(iViewportWidth, iViewportHeight));
  } // endif(displayAspectRatio<sliceAspectRatio)

  return rtnRect;
} // SetViewport()


/**
 * set viewport (tall narrow or wide short viewport: no good!!!)
 *
 * @param fLeft     Left edge of viewport
 * @param fBottom   Bottom edge of viewport
 */
void SliceViewer::SetViewport(const float4 fLeft, const float4 fBottom)
{
  Rect<int4> viewRect = GetViewportRect(fLeft, fBottom);
  glViewport(viewRect.m_left, viewRect.m_top, viewRect.Width(), viewRect.Height());
}

/**
 *  set slice number.
 */
void SliceViewer::SetSliceNumber()
{
  Dataset & myDataset = * GetDataset();

  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    m_iSliceNumber = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z + 0.5F;
    if( m_iSliceNumber < 0 )
      m_iSliceNumber = 0;
    else if( m_iSliceNumber > myDataset.m_vDim.m_z-1 )
      m_iSliceNumber = myDataset.m_vDim.m_z-1;
    break;
  case eSAGITTAL:
    m_iSliceNumber = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x + 0.5F;
    if( m_iSliceNumber < 0 )
      m_iSliceNumber = 0;
    else if( m_iSliceNumber > myDataset.m_vDim.m_x-1 )
      m_iSliceNumber = myDataset.m_vDim.m_x-1;
    break;
  case eCORONAL:
    m_iSliceNumber = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y + 0.5F;
    if( m_iSliceNumber < 0 )
      m_iSliceNumber = 0;
    else if( m_iSliceNumber > myDataset.m_vDim.m_y-1 )
      m_iSliceNumber = myDataset.m_vDim.m_y-1;
    break;
  case eCROSS_SECTION:
    m_iSliceNumber = myDataset.m_iXsectionScrollBarLocationIndex;
    if( m_iSliceNumber < 0 )
    {
      m_iSliceNumber = 0;
      myDataset.m_iXsectionScrollBarLocationIndex = 0;
    }
    else if( m_iSliceNumber >= myDataset.m_joinedSkeleton.GetNumOfElements() )
    {
      m_iSliceNumber = myDataset.m_joinedSkeleton.GetNumOfElements()-1;
      myDataset.m_iXsectionScrollBarLocationIndex = myDataset.m_joinedSkeleton.GetNumOfElements()-1;
    }
    break;
  case eVIEWDIR:
    m_iSliceNumber = m_fViewDirDistance;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation
} // SetSliceNumber()


/**
 * Returns the dimensions based on the current viewer orientation
 *
 * @return   dimensions of current orientation
 */
Point<uint4> SliceViewer::GetOrientationDim() const
{
  const Dataset& myDatasetRef = *GetDataset();
  const Triple<float4> dim(m_bSliceRendererEnabled ? myDatasetRef.m_fields.m_sliceDimensions : myDatasetRef.m_vDim);
  switch (m_eSliceOrientation)
  {
  case SliceOrientations::eAXIAL:
    // 2DGL_x = 3DVol_x, 2DGL_y = 3DVol_y, 2DGL_z = 3DVol_z
    return Point<uint4>(myDatasetRef.m_vDim.m_x,myDatasetRef.m_vDim.m_y,dim.m_z);
  case SliceOrientations::eSAGITTAL:
    // 2DGL_x = 3DVol_z, 2DGL_y = 3DVol_y, 2DGL_z = 3DVol_x
    return Point<uint4>(myDatasetRef.m_vDim.m_z,myDatasetRef.m_vDim.m_y,dim.m_x);
  case SliceOrientations::eCORONAL:
    // 2DGL_x = 3DVol_x, 2DGL_y = 3DVol_z, 2DGL_z = 3DVol_y
    return Point<uint4>(myDatasetRef.m_vDim.m_x,myDatasetRef.m_vDim.m_z,dim.m_y);
  case SliceOrientations::eCROSS_SECTION:
  case SliceOrientations::eVIEWDIR:
    return Point<uint4>(128, 128, 0);
  default:
    return Point<uint4>(0, 0, 0);
  } // endswitch(m_sliceViewer.Orientation).
} // GetOrientationDim


/**
 * Gets a pointer to the internal buffer of the current render slice.
 *
 * @param sliceStats   Out:  Volume slice statistics
 */
void SliceViewer::GetRenderSlice(SliceStats & sliceStats, const bool bAutoFly, const bool bPixelMap)
{
  uint4 uScaledVolumeSlice;
  Dataset & myDataset = * GetDataset();

  // Lazy load the Diff volume if required
  if(m_bShowUncleansed && !myDataset.m_fields.IsDiffVolLoaded()) myDataset.m_fields.LazyLoadDiffVolume();

  sliceStats = SliceStats();

  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    sliceStats.m_uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_z / myDataset.m_fields.m_units2D.m_z + 0.5F;
    uScaledVolumeSlice = (uint4) sliceStats.m_uFullVolumeSlice*m_scale.m_z + 0.5F;
    if (m_bForceSliceRender || (uScaledVolumeSlice != m_iCurrentSliceNo) || (SliceOptions(*this) != m_currSliceOptions))
    {
      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        m_sliceRenderer.RenderSlice( sliceStats.m_uFullVolumeSlice, m_bShowUncleansed, bPixelMap );
      }
      else
      {
        CreateSliceImageFromVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eAXIAL, m_bShowUncleansed);
        if(rdrGlobal.m_bShowHomogenity) CreateSliceImageFromTestVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eAXIAL, m_bShowUncleansed);
      }
      m_iCurrentSliceNo = uScaledVolumeSlice;
      m_bForceSliceRender = false;
    }
    sliceStats.m_uSliceDiff = abs( ( myDataset.m_submarine.GetPositionWorld().m_z - myDataset.m_vOrthogonalPtWorld.m_z ) / myDataset.m_fields.m_units2D.m_z );
    sliceStats.m_sliceDims = Point2D<uint4>(m_uSliceWidth, m_uSliceHeight);
    break;

  case eSAGITTAL:
    sliceStats.m_uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x + 0.5F;
    uScaledVolumeSlice = (uint4) (myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x * m_scale.m_x + 0.5F);
    m_iSliceNumber = myDataset.m_vOrthogonalPtWorld.m_x / myDataset.m_fields.m_units2D.m_x + 0.5F;
    sliceStats.m_uSliceDiff = abs( myDataset.m_submarine.GetPositionWorld().m_x - myDataset.m_vOrthogonalPtWorld.m_x ) / myDataset.m_fields.m_units2D.m_x;

    if (m_bForceSliceRender || (uScaledVolumeSlice != m_iCurrentSliceNo) || (SliceOptions(*this) != m_currSliceOptions))
    {
      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        m_sliceRenderer.RenderSlice( uScaledVolumeSlice, m_bShowUncleansed, bPixelMap );
      }
      else
      {
        CreateSliceImageFromVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eSAGITTAL, m_bShowUncleansed);
        if(rdrGlobal.m_bShowHomogenity) CreateSliceImageFromTestVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eSAGITTAL, m_bShowUncleansed);
      }
      m_iCurrentSliceNo = uScaledVolumeSlice;
      m_bForceSliceRender = false;
    }
    sliceStats.m_sliceDims = Point2D<uint4>(m_uSliceWidth, m_uSliceHeight);
    break;

  case eCORONAL:
    sliceStats.m_uFullVolumeSlice = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y + 0.5F;
    uScaledVolumeSlice = (uint4) (myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y * m_scale.m_y + 0.5F);
    m_iSliceNumber = myDataset.m_vOrthogonalPtWorld.m_y / myDataset.m_fields.m_units2D.m_y + 0.5F;
    sliceStats.m_uSliceDiff = abs( myDataset.m_submarine.GetPositionWorld().m_y - myDataset.m_vOrthogonalPtWorld.m_y ) / myDataset.m_fields.m_units2D.m_y;

    if (m_bForceSliceRender || (uScaledVolumeSlice != m_iCurrentSliceNo) || (SliceOptions(*this) != m_currSliceOptions))
    {
      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        m_sliceRenderer.RenderSlice( uScaledVolumeSlice, m_bShowUncleansed, bPixelMap );
      }
      else
      {
        CreateSliceImageFromVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eCORONAL, m_bShowUncleansed);
        if(rdrGlobal.m_bShowHomogenity) CreateSliceImageFromTestVol(m_uSliceWidth, m_uSliceHeight, uScaledVolumeSlice, eCORONAL, m_bShowUncleansed);
      }
      m_iCurrentSliceNo = uScaledVolumeSlice;
      m_bForceSliceRender = false;
    }

    sliceStats.m_sliceDims = Point2D<uint4>(m_uSliceWidth, m_uSliceHeight);
    break;

  case eCROSS_SECTION: 
  { 
    int4 iObliqueIndexNear = static_cast< float4 >( myDataset.m_fields.GetClosestSkeletonIndex( myDataset.m_submarine.GetPositionVolume() ) );
    if (iObliqueIndexNear < 0) 
      iObliqueIndexNear = 0;

    // get the nearest skeleton node
    SkeletonNode const & node2D = myDataset.m_joinedSkeleton.GetSkelNode( myDataset.m_iXsectionScrollBarLocationIndex );
    SkeletonNode const & node3D = myDataset.m_segmentList.GetCurrentSkeleton().GetSkelNode( iObliqueIndexNear, true );

    // Check if the user currently interacts with 2D x-sectional or 3D endo view
    if( (node2D.m_vWorldPos.m_x-node3D.m_vWorldPos.m_x)*(node2D.m_vWorldPos.m_x-node3D.m_vWorldPos.m_x) + 
        (node2D.m_vWorldPos.m_y-node3D.m_vWorldPos.m_y)*(node2D.m_vWorldPos.m_y-node3D.m_vWorldPos.m_y) + 
        (node2D.m_vWorldPos.m_z-node3D.m_vWorldPos.m_z)*(node2D.m_vWorldPos.m_z-node3D.m_vWorldPos.m_z) > 28.0F )
    {
      Point<float4>  positionWorld = node2D.m_vWorldPos;
      Normal<float4> nOrientation  = node2D.m_vWorldTan;
      Normal<float4> nUp           = node2D.m_vWorldUp;

      if( Submarine::CECUM_TO_RECTUM == myDataset.m_submarine.GetFlightDirection() )
      {
        nOrientation = -nOrientation;
      } // This will make sure the fly back in correct orientation for X-sectional

      if (bAutoFly)
      {
        Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
        myDataset.m_uCrossSecIndex = myDataset.m_fields.GetClosestSkeletonIndex( myDataset.m_submarine.GetPositionVolume() ) + myDataset.GetCrossSecDistance();
        SkeletonNode const & skeletonNode1 = skeleton.GetSkelNode( myDataset.m_uCrossSecIndex, true);
        nOrientation = skeletonNode1.m_vWorldTan;
        positionWorld = skeletonNode1.m_vWorldPos;
        nUp = skeletonNode1.m_vWorldUp;
      }

      // Compute 4 corners in world coordinates
      Point<float4> bottomLeft, bottomRight, topLeft, topRight;
      Vector<float4> vRight = nOrientation.Cross(nUp);
      Vector<float4> vUp(nUp);
      
      const float4 fZoomFactor( 100.0F / GetZoomPercentage() );

      m_bottomLeft  = positionWorld + (vUp *  m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
      m_bottomRight = positionWorld + (vUp *  m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
      m_topLeft     = positionWorld + (vUp * -m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
      m_topRight    = positionWorld + (vUp * -m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
      
      // save the slice definition for later volume rendering of the shadow
      myDataset.m_vObliqueLocation    = positionWorld;
      myDataset.m_vObliqueOrientation = nOrientation;
      myDataset.m_vObliqueUp          = nUp;

      /// get the extended node determination of if the node is in a real lumen segment
      SkeletonNodeVCExt jointNode = myDataset.m_joinedSkeleton.GetExtSkelNode( myDataset.m_iXsectionScrollBarLocationIndex );
      
      if( jointNode.m_iSegmentIndex >= 0 )
      { // This is the true skeleton node in a real lumen segment rather than in the connecting line
        Vector3D<int4> viPos = Vector3D<int4>(node2D.m_vVolumePos.X(), node2D.m_vVolumePos.Y(), node2D.m_vVolumePos.Z());

        vxBlockVolumeIterator<uint1> iti( myDataset.m_fields.m_ins );
        iti.SetPos(viPos);
        int4 iIdentifier = iti.GetVoxel();

        if( iIdentifier == 0 )
          myDataset.m_vObliqueDistanceFromRectumMM = -20.0F;  // This should not happen theoretically. But, it does happen when user manual fly at the end of a segment.
        else
        {
          Segment & currentSeg = myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) );

          /// The following code try to copy the code implementation in the Dataset.C.
          /// The reason for this is the skeleton node could not generate correct distance from rectum. We don't have time to find the bug at this time. Dongqing 02/12/2008
          uint2 uNumOfNode = myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) ).GetCenterline(0).GetNumOfElements();
          uint2 uCsi = myDataset.m_fields.GetClosestSkeletonIndex( node2D.m_vVolumePos );

          // make sure the uCsi should not be out of bound
          if (uCsi <= 0)
            uCsi = 0;
          else if( uCsi >= uNumOfNode )
            uCsi = uNumOfNode - 1;

          //// save the distance from rectum for later display on the image
          float4 f = myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) ).GetOffsetDistanceFromRectumMM();
          if( myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) ).IsRegularDirection() )
            f += myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) ).GetCenterline(0).GetSkelNode(uCsi).m_worldDfs;
          else
            f += myDataset.m_segmentList.GetUnsortedSegment( myDataset.m_segmentList.GetUnsortedSegmentIndexFromIdentifier(iIdentifier) ).GetCenterline(0).GetSkelNode(uCsi).m_worldDfe;

          myDataset.m_vObliqueDistanceFromRectumMM = f;
        } // else
      } // if the current skeleton node is not in the connection line
      else
        myDataset.m_vObliqueDistanceFromRectumMM = -10.0F;

      // save the 4 corners for later rendering of the overview plane
      myDataset.m_vObliqueBottomLeft  = m_bottomLeft;
      myDataset.m_vObliqueBottomRight = m_bottomRight;
      myDataset.m_vObliqueTopLeft     = m_topLeft;
      myDataset.m_vObliqueTopRight    = m_topRight;

      myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();

      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        m_sliceRenderer.RenderSlice( m_bottomLeft, m_bottomRight, m_topLeft, m_topRight, m_bShowUncleansed, bPixelMap );
      }
      else
      {
        myDataset.m_fields.m_volumeRender.CreateObliqueSlice(m_bottomLeft, m_bottomRight, m_topLeft, m_topRight, 
                                    m_puSliceImage, m_uObliqueImageSize, m_uObliqueImageSize, 0, m_bShowUncleansed);
      }     
      
      sliceStats.m_sliceDims = Point2D<uint4>(m_uObliqueImageSize, m_uObliqueImageSize); 
      sliceStats.m_uFullVolumeSlice = myDataset.m_iXsectionScrollBarLocationIndex;
      m_iSliceNumber = myDataset.m_iXsectionScrollBarLocationIndex;
      m_bForceSliceRender = false; 

    } // if in scrolling mode

    else
    { /// in 3D endo view interaction
      Point<float4> positionWorld;
      Normal<float4> nOrientation;
      Normal<float4> nUp;
      Skeleton & skeleton = myDataset.m_segmentList.GetCurrentSkeleton();
      
      // choose the skeleton direction that is closer to the current view direction
      if ( node3D.m_vWorldTan.Dot( myDataset.m_submarine.GetOrientation() ) > 0 )
      {
        myDataset.m_uCrossSecIndex = iObliqueIndexNear + myDataset.GetCrossSecDistance();
        SkeletonNode const & skeletonNodeFar = skeleton.GetSkelNode( myDataset.m_uCrossSecIndex, true );
        positionWorld = skeletonNodeFar.m_vWorldPos;
        nOrientation = skeletonNodeFar.m_vWorldTan;
      }
      else
      {
        int4 iCrossSecIndex = iObliqueIndexNear - int4(myDataset.GetCrossSecDistance());
        myDataset.m_uCrossSecIndex = ( iCrossSecIndex > 0 ) ? iCrossSecIndex : 0;
        SkeletonNode const & skeletonNodeFar = skeleton.GetSkelNode( myDataset.m_uCrossSecIndex, true );
        positionWorld = skeletonNodeFar.m_vWorldPos;
        nOrientation = - skeletonNodeFar.m_vWorldTan;
      }
      nUp = myDataset.m_submarine.GetUpDirection();

      // Calculate the approximate line of intersection between the vertical view plane
      // and the perpendicular centerline plane
      Normal<float4> nRight = Normal<float4>(myDataset.m_submarine.GetOrientation().Cross(myDataset.m_submarine.GetUpDirection()));
      for (int4 i=0; i<10; i++) 
      {
        nUp = Normal<float4>(GetProjectionParallelToPlane(Vector<float4>(nUp), nRight));
        nUp = Normal<float4>(GetProjectionParallelToPlane(Vector<float4>(nUp), nOrientation));
      }

      // Compute 4 corners in world coordinates
      Point<float4> bottomLeft, bottomRight, topLeft, topRight;
      Vector<float4> vRight = nOrientation.Cross(nUp);
      Vector<float4> vUp(nUp);
      
      const float4 fZoomFactor( 100.0F / GetZoomPercentage() );

      m_bottomLeft  = positionWorld + (vUp *  m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
      m_bottomRight = positionWorld + (vUp *  m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
      m_topLeft     = positionWorld + (vUp * -m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
      m_topRight    = positionWorld + (vUp * -m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
      
      // save the slice definition for later volume rendering of the shadow
      myDataset.m_vObliqueLocation    = positionWorld;
      myDataset.m_vObliqueOrientation = nOrientation;
      myDataset.m_vObliqueUp          = nUp;

      // save the distance from rectum for later display on the image
      SkeletonNode const & currentSkeletonNode = skeleton.GetSkelNode( myDataset.m_uCrossSecIndex, true );
      myDataset.m_vObliqueDistanceFromRectumMM =
        myDataset.m_segmentList.GetCurrentSegment().GetOffsetDistanceFromRectumMM() +
        ( myDataset.m_segmentList.GetCurrentSegment().IsRegularDirection() ?
          currentSkeletonNode.m_worldDfs :
          currentSkeletonNode.m_worldDfe );

      // save the 4 corners for later rendering of the overview plane
      myDataset.m_vObliqueBottomLeft  = m_bottomLeft;
      myDataset.m_vObliqueBottomRight = m_bottomRight;
      myDataset.m_vObliqueTopLeft     = m_topLeft;
      myDataset.m_vObliqueTopRight    = m_topRight;
      myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();

      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        m_sliceRenderer.RenderSlice( m_bottomLeft, m_bottomRight, m_topLeft, m_topRight, m_bShowUncleansed, bPixelMap );
      }
      else
      {
        myDataset.m_fields.m_volumeRender.CreateObliqueSlice(m_bottomLeft, m_bottomRight, m_topLeft, m_topRight, 
                                                              m_puSliceImage, m_uObliqueImageSize, m_uObliqueImageSize, 0, m_bShowUncleansed);
      }     
      
      sliceStats.m_sliceDims = Point2D<uint4>(m_uObliqueImageSize, m_uObliqueImageSize);
      sliceStats.m_uFullVolumeSlice = myDataset.m_iXsectionScrollBarLocationIndex;
      m_iSliceNumber = myDataset.m_iXsectionScrollBarLocationIndex;
      m_bForceSliceRender = false;
    } // else not in scrolling mode

    break;
  } // endcase CROSS_SECTION
  case eVIEWDIR:
  {
    Normal<float4> nOrientation = myDataset.m_submarine.GetOrientation();
    Point<float4> positionWorld = myDataset.m_submarine.GetPositionWorld() + Vector<float4>(nOrientation)*(m_fViewDirDistance/2.0F);
    Normal<float4> nUp = myDataset.m_submarine.GetUpDirection();
    
    // Compute 4 corners in world coordinates
    Point<float4> bottomLeft, bottomRight, topLeft, topRight;
    Vector<float4> vRight = nOrientation.Cross(nUp);
    Vector<float4> vUp(nUp);
    
    const float4 fZoomFactor( 100.0F / GetZoomPercentage() );

    m_bottomLeft  = positionWorld + (vUp *  m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
    m_bottomRight = positionWorld + (vUp *  m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
    m_topLeft     = positionWorld + (vUp * -m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits) * fZoomFactor;
    m_topRight    = positionWorld + (vUp * -m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits) * fZoomFactor;
    
    // resample the slice
    
    if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
    {
      m_sliceRenderer.RenderSlice( m_bottomLeft, m_bottomRight,   m_topLeft,   m_topRight,
                                   m_bShowUncleansed,  bPixelMap );
    }
    else
    {
      myDataset.m_fields.m_volumeRender.CreateObliqueSlice(m_bottomLeft, m_bottomRight, m_topLeft, m_topRight, 
                                                          m_puSliceImage, m_uObliqueImageSize, m_uObliqueImageSize, 0, m_bShowUncleansed);
    }                                                     
    
    // save the slice definition for later volume rendering of the shadow
    myDataset.m_vObliqueLocation = positionWorld;
    myDataset.m_vObliqueOrientation = nOrientation;
    myDataset.m_vObliqueUp = nUp;
    
    // save the 4 corners for later rendering of the overview plane
    myDataset.m_vObliqueBottomLeft  = m_bottomLeft;
    myDataset.m_vObliqueBottomRight = m_bottomRight;
    myDataset.m_vObliqueTopLeft     = m_topLeft;
    myDataset.m_vObliqueTopRight    = m_topRight;
    myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();

    sliceStats.m_sliceDims = Point2D<uint4>(m_uObliqueImageSize, m_uObliqueImageSize);
    m_iSliceNumber = m_fViewDirDistance;
    m_bForceSliceRender = false;
    break;
  } // endcase VIEWDIR
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  m_currSliceOptions.UpdateOptions(*this);

  // Issue 3578: restrict datasets to maximum m_uSliceWidthMax,m_uSliceHeightMax number of slices
  sliceStats.m_sliceDims.m_x = min(m_uSliceWidthMax, sliceStats.m_sliceDims.m_x);
  sliceStats.m_sliceDims.m_y = min(m_uSliceHeightMax, sliceStats.m_sliceDims.m_y);

} // GetRenderSlice()


/**
 * Render the slice
 * We need to specify the viewport for the more general (not aligned at (0,0) case)
 *
 * @param bUseCachedImaged   whether to use the cached image
 * @param bIsMainWindow      if this is the main window
 * @param rect               the viewport rectangle
 * @param fLeft              the left side of the viewport
 * @param fBottom            the bottom side of the viewport
 * @param bPixelMap          if using the pixel map to adjust window/level
 * @param bAutoFly           if autoflying
 * @param bInverseImage      if the image should be inverted
 */
void SliceViewer::RenderSlice(const bool bUseCachedImaged, const bool bIsMainWindow, const Rect<int4> &rect, const float4 fLeft, 
                              const float4 fBottom, const bool bPixelMap, const bool bAutoFly, const bool bInverseImage)
{
  try
  {
    Dataset & myDataset = * GetDataset();

    if ( myDataset.m_fields.IsEcvVolLoaded() == false ) return;

    if (IsVolumeInitialed() == false)
    {
      // call this function at once after the volume is loaded.
      InitVolumeLoaded();
      SetVolumeInitialed(true);
    }

    SetViewport(fLeft, fBottom, true);

    // adjust 'viewport' to make sure the arrow is visible.
    if (bAutoFly) AdjustImageAreaForArrowPos(m_bArrowInSliceCenter);

    SliceStats sliceStats;
    if( !bUseCachedImaged )
    {
      try
      {
        GetRenderSlice(sliceStats, bAutoFly, bPixelMap);
      }
      catch (...)
      {
        LogErr("An exception caught while rendering slice.", "SliceViewer", "RenderSlice");
        return;
      }
    }
    else
    {
      sliceStats = SliceStats();
    }

    // draw the image
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
      gluOrtho2D(0, 1, 1, 0);

      // not to do zoom adjust if not required.
      if ( ! m_bArrowInSliceCenter )
        ZoomAdjust();

      Point<float4> corners[4];

      // are we using the newer 12-bit renderer?
      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        InitTextureCoords12Bit( corners );
      }
      else
      {
        InitTextureCoords8Bit( corners );
      }

      if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled && 
        ( ( m_eSliceOrientation == eAXIAL )     ||  ( m_eSliceOrientation == eCORONAL ) || 
          ( m_eSliceOrientation == eSAGITTAL )  ||  ( m_eSliceOrientation == eCROSS_SECTION ) || 
          ( m_eSliceOrientation == eVIEWDIR ) ) )
      {
        // reverse-video image if required.
        if (bInverseImage) 
          ReverseVideoImage(eRVI_RGBA, m_sliceRenderer.GetImageWidth(),  m_sliceRenderer.GetImageHeight(), (void*) m_sliceRenderer.GetImageData());

        glPixelStorei(GL_UNPACK_ROW_LENGTH, m_sliceRenderer.GetImageWidth());
        glBindTexture( GL_TEXTURE_2D, 0 );
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,  m_sliceRenderer.GetImageWidth(),  m_sliceRenderer.GetImageHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) m_sliceRenderer.GetImageData() );

        // restore original image data.
        if (bInverseImage) 
          ReverseVideoImage(eRVI_RGBA, m_sliceRenderer.GetImageWidth(),  m_sliceRenderer.GetImageHeight(), (void*) m_sliceRenderer.GetImageData());
      }
      else    
      {
        corners[1].m_x -= m_crop.m_x;
        corners[2].m_x -= m_crop.m_x;
        corners[2].m_y -= m_crop.m_y;
        corners[3].m_y -= m_crop.m_y;

        // reverse-video image if required.
        if (bInverseImage) 
          ReverseVideoImage(eRVI_Gray, sliceStats.m_sliceDims.m_x, sliceStats.m_sliceDims.m_y, m_puSliceImage);

        glPixelTransferi(GL_MAP_COLOR, int4(bPixelMap));
        glPixelStorei(GL_UNPACK_ROW_LENGTH, sliceStats.m_sliceDims.m_x);
        glBindTexture( GL_TEXTURE_2D, 0 );
        glTexImage2D (GL_TEXTURE_2D, 0, 3, sliceStats.m_sliceDims.m_x, sliceStats.m_sliceDims.m_y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*) m_puSliceImage);
        glPixelTransferi(GL_MAP_COLOR, 0);
      
        // restore original image data.
        if (bInverseImage) 
          ReverseVideoImage(eRVI_Gray, sliceStats.m_sliceDims.m_x, sliceStats.m_sliceDims.m_y, m_puSliceImage);
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
      }

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glEnable(GL_TEXTURE_2D);
      glTranslatef(0,0,-1);

      // adjust the m_rZoom in case there is a difference of m_uSliceWidth and m_uSliceHeight.
      Point2D<float4> shiftPt, aspectRatioPt;
      GetAspectRatioStats(shiftPt, aspectRatioPt);

      // enable blending for the new renderer
      if ( m_bSliceRendererEnabled )
      {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      }

      if( ( m_eSliceOrientation == eAXIAL ) || ( m_eSliceOrientation == eCORONAL ) || ( m_eSliceOrientation == eSAGITTAL ) ) 
      {
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x);   glVertex2f(m_rZoom.m_left  +m_rZoom.Width()*shiftPt.m_x,  m_rZoom.m_top    +m_rZoom.Height()*shiftPt.m_y);
          glTexCoord2fv(&corners[1].m_x);   glVertex2f(m_rZoom.m_right -m_rZoom.Width()*shiftPt.m_x,  m_rZoom.m_top    +m_rZoom.Height()*shiftPt.m_y);
          glTexCoord2fv(&corners[2].m_x);   glVertex2f(m_rZoom.m_right -m_rZoom.Width()*shiftPt.m_x,  m_rZoom.m_bottom -m_rZoom.Height()*shiftPt.m_y);
          glTexCoord2fv(&corners[3].m_x);   glVertex2f(m_rZoom.m_left  +m_rZoom.Width()*shiftPt.m_x,  m_rZoom.m_bottom -m_rZoom.Height()*shiftPt.m_y);
        glEnd();
      }
      else
      {
        Rect<float4> rZoom(0.0F, 0.0F, 1.0F, 1.0F);
        
        glBegin(GL_QUADS);
          glTexCoord2fv(&corners[0].m_x);   glVertex2f(rZoom.m_left ,  rZoom.m_top   );
          glTexCoord2fv(&corners[1].m_x);   glVertex2f(rZoom.m_right,  rZoom.m_top   );
          glTexCoord2fv(&corners[2].m_x);   glVertex2f(rZoom.m_right,  rZoom.m_bottom);
          glTexCoord2fv(&corners[3].m_x);   glVertex2f(rZoom.m_left ,  rZoom.m_bottom);
        glEnd();
      }

      // disable blending after the new renderer
      if ( m_bSliceRendererEnabled )
        glPopAttrib();

      // display 2D thumbnail, presumably only when panning (and not on select)
      if ( m_bDisplayOverviewSliceGrasp ) // || (m_rSelect.m_left < 9999) )
      {
        glPushAttrib(GL_ENABLE_BIT);
          glEnable(GL_SCISSOR_TEST);
          glScissor(fLeft, fBottom, m_uDisplayWidth*0.25f, m_uDisplayHeight*0.25f);
          glClearColor( 0, 0, 0, 0 );
          ::glClear(GL_COLOR_BUFFER_BIT);
        glPopAttrib();

        // enable blending for the new renderer
        if ( m_bSliceRendererEnabled )
        {
          glEnable( GL_BLEND );
          glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
        
        // if zoom factor <= 1.0; centering slice in the zoom selection area.
        if (((m_rZoom.Height()) <= 1.0F) || ((m_rZoom.Width()) <= 1.0F))
        {
          // render the 2D overview slice.
          float4 x0 = (0.25F - (m_rZoom.Width()*aspectRatioPt.m_x)/4.0F)/2.0F;
          float4 x1 = x0 + (m_rZoom.Width()*aspectRatioPt.m_x)/4.0F;
          float4 y0 = (0.75F + (0.25F-(m_rZoom.Height()*aspectRatioPt.m_y)/4.0F)/2.0);
          float4 y1 = y0 + (m_rZoom.Height()*aspectRatioPt.m_y)/4.0F;

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
          float4 x0 = (0.25F - aspectRatioPt.m_x/4.0F)/2.0F;
          float4 x1 = x0     + (aspectRatioPt.m_x)/4.0F;
          float4 y0 = (0.75F + (0.25F-aspectRatioPt.m_y/4.0F)/2.0);
          float4 y1 = y0     + (aspectRatioPt.m_y)/4.0F;

          // render the 2D overview slice.
          glBegin(GL_QUADS);
            glTexCoord2fv(&corners[0].m_x); glVertex2f(x0, y0);
            glTexCoord2fv(&corners[1].m_x); glVertex2f(x1, y0);
            glTexCoord2fv(&corners[2].m_x); glVertex2f(x1, y1);
            glTexCoord2fv(&corners[3].m_x); glVertex2f(x0, y1);
          glEnd();

          glDisable(GL_TEXTURE_2D);

          Draw2DOverviewSliceFrame();
        } // (zoomfactor <1.0F)

      }
      else 
      {
        glDisable(GL_TEXTURE_2D);
      } // end-if (m_bDisplayOverviewSlice)

      glDisable( GL_BLEND );

      // frame to select an area for zooming.
      if ( m_rSelect.m_left < 9999 ) DrawSelectionFrame();

    } glPopMatrix();  
    glClear(GL_DEPTH_BUFFER_BIT);

    if ( m_eSliceOrientation == eAXIAL ||
        m_eSliceOrientation == eSAGITTAL ||
        m_eSliceOrientation == eCORONAL )
    {
      m_iSliceNumber = sliceStats.m_uFullVolumeSlice;
    }

    if (m_bRenderCamera && (sliceStats.m_uSliceDiff < m_uCameraSliceDiffThreshold))
      RenderCamera(rect);
    if (m_bRenderCamera &&
      ( m_eSliceOrientation == SliceOrientations::eAXIAL ||
      m_eSliceOrientation == SliceOrientations::eSAGITTAL ||
      m_eSliceOrientation == SliceOrientations::eCORONAL ))
    {
      Render2DAnnotations(rect);
    }

    if ( ( rdrGlobal.m_bDisplayCadFindings || rdrGlobal.m_bAlwaysShowCadArrows ) )
    {
      RenderCadFindings( rect );
    }

    glFlush();
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceViewer", "RenderSlice" );
  }
} // RenderSlice()


/**
 * Initializes the texture coordinates array for the 12-bit renderer
 *
 * @param   pCorners    Array of texture coordinates to be initializes.
 */
void SliceViewer::InitTextureCoords12Bit( Point<float4> pCorners[4] )
{
  Dataset & myDataset = * GetDataset();
  
  const Triple< uint4 > & dim = myDataset.m_fields.m_sliceDimensions;

  switch ( m_eSliceOrientation )
  {
    case eAXIAL:
    {
      const float4 fCropX( float4( dim.m_x - 1 ) / float4( m_sliceRenderer.GetImageWidth() - 1 ) );
      const float4 fCropY( float4( dim.m_y - 1 ) / float4( m_sliceRenderer.GetImageHeight() - 1 ) );
      pCorners[0] = Point<float4>(    0  ,    0  , 0 );
      pCorners[1] = Point<float4>( fCropX,    0  , 0 );
      pCorners[2] = Point<float4>( fCropX, fCropY, 0 );
      pCorners[3] = Point<float4>(    0  , fCropY, 0 );
      break;
    } 
    case eCORONAL:
    {
      const float4 fCropX( float4( dim.m_z - 1 ) / float4( m_sliceRenderer.GetImageWidth() - 1 ) );
      const float4 fCropY( float4( dim.m_x - 1 ) / float4( m_sliceRenderer.GetImageHeight() - 1 ) );
      pCorners[0] = Point<float4>( fCropX,    0  , 0 );
      pCorners[1] = Point<float4>( fCropX, fCropY, 0 );
      pCorners[2] = Point<float4>(    0  , fCropY, 0 );
      pCorners[3] = Point<float4>(    0  ,    0  , 0 );
      break;
    }
    case eSAGITTAL:
    {
      const float4 fCropX( float4( dim.m_y - 1 ) / float4( m_sliceRenderer.GetImageWidth() - 1 ) );
      const float4 fCropY( float4( dim.m_z - 1 ) / float4( m_sliceRenderer.GetImageHeight() - 1 ) );
      pCorners[0] = Point<float4>(    0  , fCropY, 0 );
      if ( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        pCorners[3] = Point<float4>(    0  ,    0  , 0 );
        pCorners[2] = Point<float4>( fCropX,    0  , 0 );
        pCorners[1] = Point<float4>( fCropX, fCropY, 0 );
      }
      else
      {
        pCorners[1] = Point<float4>(    0  ,    0  , 0 );
        pCorners[2] = Point<float4>( fCropX,    0  , 0 );
        pCorners[3] = Point<float4>( fCropX, fCropY, 0 );
      }
      break;
    }
    case eCROSS_SECTION:
    {
      pCorners[0] = Point<float4>(0,1,0);
      pCorners[1] = Point<float4>(1,1,0);
      pCorners[2] = Point<float4>(1,0,0);
      pCorners[3] = Point<float4>(0,0,0);
      break;
    }
    case eVIEWDIR:
    {
      pCorners[0] = Point<float4>(0,1,0);
      pCorners[1] = Point<float4>(1,1,0);
      pCorners[2] = Point<float4>(1,0,0);
      pCorners[3] = Point<float4>(0,0,0);
      break;
    }
  } // switch (orientation)
} // InitTextureCoords12Bit( Point<float4> * pCorners )


/**
 * Initializes the texture coordinates array for the 8-bit renderer
 *
 * @param   pCorners    Array of texture coordinates to be initializes.
 */
void SliceViewer::InitTextureCoords8Bit( Point<float4> pCorners[4] )
{
  pCorners[0] = Point<float4>(0, 0, 0);
  pCorners[1] = Point<float4>(1, 0, 0);
  pCorners[2] = Point<float4>(1, 1, 0);
  pCorners[3] = Point<float4>(0, 1, 0);

  // we no longer need to flip the axial, prone view vertically and horizontally
} // InitTextureCoords8Bit( Point<float4> * pCorners )


/**
 * test if the arrow is visible after zooming.
 * if not, adjust the visible area.
 *
 * @param bAutoFly   Whether or not autofly is engaged.
 */
void SliceViewer::AdjustImageAreaForArrowPos(const bool bAutoFly)
{
  Dataset & myDataset = * GetDataset();

  Point<float4> pos(myDataset.m_submarine.GetPositionWorld().GetDividedAnIso(myDataset.m_fields.m_units2D));
  const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);

  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    pos = Point<float4>(pos.m_x / dim.m_x, (dim.m_y - 1.0f - pos.m_y) / dim.m_y, pos.m_z / dim.m_z);
    break;
  case eSAGITTAL:
    pos = Point<float4>(pos.m_y / dim.m_y, pos.m_z / dim.m_z, pos.m_x / dim.m_x);
    break;
  case eCORONAL:
    pos = Point<float4>(pos.m_x / dim.m_x, pos.m_z / dim.m_z, pos.m_y / dim.m_y);
    break;
  case eCROSS_SECTION:
  case eVIEWDIR:
    return;
  default:
      // todo: log error
    break;
  } // endswitch m_eSliceOrientation

  pos.m_x = pos.m_x * m_rZoom.Width() +  m_rZoom.m_left;
  // because of the difference of originals in "Screen" coordinates and 
  // "OpenGL" coordinates, the y coordinate should be reversed. 
  pos.m_y = 1.0f - pos.m_y;
  pos.m_y = pos.m_y * m_rZoom.Height() + m_rZoom.m_top;
  
  // adjust visible area.
  if ( bAutoFly || (pos.m_x > 1.0f) || (pos.m_x < 0.0f) || (pos.m_y > 1.0f) || (pos.m_y < 0.0f) )
  {
    float4 dx = 0.5f - pos.m_x;
    float4 dy = 0.5f - pos.m_y;
    
    m_rZoom.Offset(dx,dy);
  }
  
} // AdjustImageAreaForArrowPos()


/**
 * Returns the distance of the given point from the currently displayed slice.
 *
 * @param ptVolumeCoord   Point to test
 * @return                Distance from current slice
 */
uint4 SliceViewer::DistFromCurrentSlice(const Point<float4>& ptVolumeCoord) const
{
  switch (GetOrientation()) 
  {
  case SliceOrientations::eAXIAL:
    return abs(static_cast< long >( uint4(ptVolumeCoord.m_z+0.5F) - m_iSliceNumber) );
    break;
  case SliceOrientations::eSAGITTAL:
    return abs(static_cast<long>(uint4(ptVolumeCoord.m_x+0.5F) - m_iSliceNumber));
    break;
  case SliceOrientations::eCORONAL:
    return abs(static_cast<long>(uint4(ptVolumeCoord.m_y+0.5F) - m_iSliceNumber));
    break;
    
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation
  return 0;
} // DistFromCurrentSlice


/**
 * Returns manhattan distance between the two given points in the plane of the current slice orientation
 *
 * @param pt1   first point to compare
 * @param pt2   second point to compare
 * @return      manhattan distance between points in current slice orientation's plane
 */
float4 SliceViewer::ManhattanDistInPlane(const Point<float4>& pt1, const Point<float4>& pt2) const
{
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
    return abs(pt1.m_x - pt2.m_x) + abs(pt1.m_y - pt2.m_y);
    break;
  case SliceOrientations::eSAGITTAL:
    return abs(pt1.m_z - pt2.m_z) + abs(pt1.m_y - pt2.m_y);
    break;
  case SliceOrientations::eCORONAL:
    return abs(pt1.m_x - pt2.m_x) + abs(pt1.m_z - pt2.m_z);
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation
  return 0.0;
} // MaxDistInPlane



/**
 * Calculate the position of camera in m_rZoom area.
 *
 * @param pos   3D position
 * @return      Position as percentage of given rectangle area
 */
Point<float4> SliceViewer::Calc2DGLPosFromVolumeCoord(const Point<float4> &pos)
{
  Dataset & myDataset = * GetDataset();

  const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);
  const Triple<float4> units(m_bSliceRendererEnabled ? myDataset.m_fields.m_units2D : myDataset.m_vUnits);
  Point<float4> outPos;

  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    // 2DGL_x = 3DVol_x, 2DGL_y = 3DVol_y, 2DGL_z = 3DVol_z
    outPos = Point<float4>(pos.m_x / dim.m_x, (dim.m_y - 1 - pos.m_y) / dim.m_y, pos.m_z / dim.m_z);
    break;

  case eSAGITTAL:
    // 2DGL_x = 3DVol_z, 2DGL_y = 3DVol_y, 2DGL_z = 3DVol_x
    if( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      outPos = Point<float4>((/*dim.m_y - 1 -*/ pos.m_y) / dim.m_y, (/*dim.m_z - 1 -*/ pos.m_z) / dim.m_z, (dim.m_x - 1 - pos.m_x) / dim.m_x);
    else
      outPos = Point<float4>((dim.m_z - 1 - pos.m_z) / dim.m_z, (dim.m_y - 1 - pos.m_y) / dim.m_y, (dim.m_x - 1 - pos.m_x) / dim.m_x);

    break;

  case eCORONAL:
    // 2DGL_x = 3DVol_x, 2DGL_y = 3DVol_z, 2DGL_z = 3DVol_y
    outPos = Point<float4>(pos.m_x / dim.m_x, pos.m_z / dim.m_z, pos.m_y / dim.m_y);
    break;

  case eCROSS_SECTION:
  case eVIEWDIR:
  default:
    // todo: log error
    break;
  } // endswitch m_sliceOrientation

  Point2D<float4> shiftPt, aspectRatioPt;
  GetAspectRatioStats(shiftPt, aspectRatioPt);

  outPos.m_x = Interpolate( 0, outPos.m_x, 1, m_rZoom.m_left  + m_rZoom.Width() * shiftPt.m_x, m_rZoom.m_right - m_rZoom.Width() * shiftPt.m_x );

  // because of the difference of originals in "Screen" coordinates and 
  // "OpenGL" coordinates, the y coordinate should be reversed. 
  outPos.m_y = 1 - outPos.m_y;
  outPos.m_y = Interpolate( 0, outPos.m_y, 1, m_rZoom.m_top    + m_rZoom.Height() * shiftPt.m_y, m_rZoom.m_bottom - m_rZoom.Height() * shiftPt.m_y );
  outPos.m_y = 1 - outPos.m_y;

  return outPos;
} // Calc2DGLPosFromVolumeCoord()


/**
 * Render camera: the arrow the position/tip haven't got yet. 
 *
 * @param rect       the viewport rectangle
 */
void SliceViewer::RenderCamera( const Rect<int4> & rect )
{
  Dataset & myDataset = * GetDataset();
  const float4 arrowLength = m_uCameraArrowLength / rect.Width();

  // Set up and Draw the Camera
  glMatrixMode(GL_MODELVIEW); 
  glPushMatrix(); 
  {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); 
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, -1, 1);
      const Triple<float4> units( m_bSliceRendererEnabled ? myDataset.m_fields.m_units2D : myDataset.m_vUnits );

      Point<float4> tip = Calc2DGLPosFromVolumeCoord(
        myDataset.m_submarine.GetPositionWorld().GetDividedAnIso( units ) );
      Point<float4> backWC = myDataset.m_submarine.GetPositionWorld()+Vector<float4>(myDataset.m_submarine.GetOrientation());
      Point<float4> back = Calc2DGLPosFromVolumeCoord( backWC.GetDividedAnIso( units ) );

      const Normal<float4> nDirection(back - tip);
      RenderUtil::RenderArrow( tip, nDirection, arrowLength, false, 3, PixelRGBA<float4>(0.3F, 0.0F, 0.5F, 1.0F), true );
    } // end Push-Pop Projection Matrix()
    glMatrixMode(GL_PROJECTION); 
    glPopMatrix();
  } // end Push-Pop ModelView Matrix.
  glMatrixMode(GL_MODELVIEW); 
  glPopMatrix();

} // RenderCamera()


/**
 * Render the 2D annotations
 *
 * @param rect the viewport rectangle
 */
void SliceViewer::Render2DAnnotations(const Rect<int4> & rect)
{
  Dataset & myDataset = * GetDataset();
  const float4 arrowLength = m_uCameraArrowLength / rect.Width();

  glMatrixMode(GL_MODELVIEW); 
  glPushMatrix(); 
  {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); 
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, -1, 1);
            
      BookmarkList& atnList = myDataset.m_session.m_2DAnnotationList;
      for (BookmarkList::const_iterator bmIt = atnList.begin(); bmIt != atnList.end(); bmIt++)
      {
        Point<float4> posVolCoords = bmIt->m_position.GetDividedAnIso(myDataset.m_fields.m_units2D);
        if (/*(SliceOrientations::TypeEnum(bmIt->m_ePriority) == m_eSliceOrientation) && */DistFromCurrentSlice(posVolCoords) < m_uAnnotationSliceDiffThreshold)
        {
          Point<float4> tip = Calc2DGLPosFromVolumeCoord(posVolCoords);
          Point<float4> backWC = bmIt->m_position+Vector<float4>(bmIt->m_orientation);
          Point<float4> back = Calc2DGLPosFromVolumeCoord(backWC.GetDividedAnIso(myDataset.m_fields.m_units2D));
          
          const Normal<float4> nDirection(back - tip);
          RenderUtil::RenderArrow(tip, nDirection, arrowLength, false, -1, PixelRGBA<float4>(1.0F, 0.0F, 0.0F, 0.75F), true);
        } // endif right orientation and close to correct slice
        
      } // endfor all annotations
      
    } // end Push-Pop Projection Matrix()
    glMatrixMode(GL_PROJECTION); 
    glPopMatrix();
  } // end Push-Pop ModelView Matrix.
  glMatrixMode(GL_MODELVIEW); 
  glPopMatrix();
} // Render2DAnnotations()


/**
 * Render the CAD findings in a window, scaling the arrow to the given window size
 *
 * @param viewRect the rectangle describing the view window
 */
void SliceViewer::RenderCadFindings( const Rect<int4> & viewRect )
{

  Dataset & myDataset = * GetDataset();
  const float4 fArrowLength( m_uCameraArrowLength / viewRect.Width() );

  vxShareableObject< vxComponentArray > * pComponentArray( myDataset.m_fields.m_sliceRendererData.GetComponents() );
  if( pComponentArray->GetCount() <= 1 )
  { /// This will avoid crashing at the Verification tab Orthogonal layout. When user switch to that layout, the 
    /// components data may not be loaded yet.
    return;
  } // if

  // use yellow circles for undecided and green for shown (i.e. true positives)
  const float4 fYellow[3] = { 1.0F, 1.0F, 0 };
  const float4 fGreen[3]  = { 0, 1.0F, 0.7F };
  ColonSession::FindingInfo::DecisionEnum eDecis;
  ColonSession::FindingInfo::DecisionEnum eCurrVis(ColonSession::FindingInfo::eDecisionUndecided);
  glColor3fv( fYellow );

  // early return if nothing to display
  if ( myDataset.m_cadFindings.GetCount() == 0 )
    return;

  glMatrixMode(GL_MODELVIEW); 
  glPushMatrix(); 
  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);

  if( GetOrientation() == SliceOrientations::eAXIAL ||
       GetOrientation() == SliceOrientations::eSAGITTAL ||
       GetOrientation() == SliceOrientations::eCORONAL )
  {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); 
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, -1, 1);

      for ( int4 i(0); i < myDataset.m_cadFindings.GetCount(); ++i )
      {
        const vxCadFinding & cadFinding = myDataset.m_cadFindings.GetFinding(i);
        if ( RenderUtil::IsFindingVisible(cadFinding, myDataset.m_session.m_mapCadFindingInfo, myDataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings )
          && ( eDecis=RenderUtil::GetFindingDecision(cadFinding, myDataset.m_session.m_mapCadFindingInfo) ) != ColonSession::FindingInfo::eDecisionDenied )
        {
          Point3D<float4> centerlinePos( myDataset.GetNearestSkeletonPosition( cadFinding ) );
          const Point3Df cadCentroid( cadFinding.GetCentroidMM().GetVector() );
          Normal<float4> orientation( cadCentroid - centerlinePos );

          Point<float4> posVolCoords = cadCentroid.GetDividedAnIso( myDataset.m_fields.m_units2D );
          if ( DistFromCurrentSlice( posVolCoords ) < m_uAnnotationSliceDiffThreshold )
          {
            if ( eDecis != eCurrVis )
            {
              glColor3fv( eDecis == ColonSession::FindingInfo::eDecisionUndecided ? fYellow : fGreen );
              eCurrVis = eDecis;
            }

            Point<float4> tip = Calc2DGLPosFromVolumeCoord( posVolCoords );
            Point2D<float4> pos2DTip( tip.m_x, tip.m_y );
            const float4 fRadiusPixels = 12;
            // scale with the image zoom
            RenderUtil::GLDrawCircle( pos2DTip, fRadiusPixels / viewRect.Width() * m_rZoom.Width(), false );
          } // if right orientation and close to correct slice

        } // if finding is visible
        
      } // for all findings
      
    } // push projection matrix
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

  } // if it is orthogonal view

  else if( GetOrientation() == SliceOrientations::eCROSS_SECTION )
  {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); 
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, -1, 1);
      glColor3f( 1.0F, 1.0F, 0.0F );

      register float4 f;
      register Vector3D<float4> fPosRelative;
      const Vector3D<float4>    vfOrientation( myDataset.m_vObliqueOrientation.X(), myDataset.m_vObliqueOrientation.Y(), 
                                               myDataset.m_vObliqueOrientation.Z() );

      const Vector3D<float4> vfOrigin(myDataset.m_vObliqueTopLeft.m_x, myDataset.m_vObliqueTopLeft.m_y, myDataset.m_vObliqueTopLeft.m_z);
      Vector3D<float4> xNormal(myDataset.m_vObliqueTopRight.m_x - vfOrigin.m_x,
                                      myDataset.m_vObliqueTopRight.m_y - vfOrigin.m_y,
                                      myDataset.m_vObliqueTopRight.m_z - vfOrigin.m_z);
      const float4 fXlength = xNormal.GetLength();
      xNormal = xNormal.GetNormalized();

      Vector3D<float4> yNormal(myDataset.m_vObliqueBottomLeft.m_x-vfOrigin.m_x,
                                      myDataset.m_vObliqueBottomLeft.m_y-vfOrigin.m_y,
                                      myDataset.m_vObliqueBottomLeft.m_z-vfOrigin.m_z);
      const float4 fYlength = yNormal.GetLength();
      yNormal = yNormal.GetNormalized();

      for ( int4 i(0); i < myDataset.m_cadFindings.GetCount(); ++i )
      { 
        const vxCadFinding & cadFinding = myDataset.m_cadFindings.GetFinding(i);
        if ( RenderUtil::IsFindingVisible(cadFinding, myDataset.m_session.m_mapCadFindingInfo, myDataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings )
          && ( eDecis=RenderUtil::GetFindingDecision(cadFinding, myDataset.m_session.m_mapCadFindingInfo) ) != ColonSession::FindingInfo::eDecisionDenied )
        {
          fPosRelative.m_x = cadFinding.GetCentroidMM().GetVector().m_x - myDataset.m_vObliqueLocation.m_x;
          fPosRelative.m_y = cadFinding.GetCentroidMM().GetVector().m_y - myDataset.m_vObliqueLocation.m_y;
          fPosRelative.m_z = cadFinding.GetCentroidMM().GetVector().m_z - myDataset.m_vObliqueLocation.m_z;
          f = fPosRelative.GetLength();

          if( abs(fPosRelative.Dot(vfOrientation)/f) < 0.3F && f < 48.0F )
          { // if the centroid is in the oblique plane and not far (<36 mm) from the oblique view center
            Point2D<float4> pos2DTip;

            pos2DTip.m_x = (cadFinding.GetCentroidMM().GetVector()-vfOrigin).Dot(xNormal);
            pos2DTip.m_y = (cadFinding.GetCentroidMM().GetVector()-vfOrigin).Dot(yNormal);

            if( pos2DTip.m_x > 0.0F && pos2DTip.m_y > 0.0F && pos2DTip.m_x < fXlength && pos2DTip.m_y < fYlength )
            {
              if ( eDecis != eCurrVis )
              {
                glColor3fv( eDecis == ColonSession::FindingInfo::eDecisionUndecided ? fYellow : fGreen );
                eCurrVis = eDecis;
              }

              pos2DTip.m_x = pos2DTip.m_x/fXlength;
              pos2DTip.m_y = pos2DTip.m_y/fYlength;

              const float4 fRadiusPixels = 12;
              // scale with the image zoom
              RenderUtil::GLDrawCircle( pos2DTip, fRadiusPixels / viewRect.Width() * m_rZoom.Width(), false );
            }

          } // if right orientation and close to correct slice

        } // if finding is visible
        
      } // for all findings
      
    } // push projection matrix
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

  } // else if cross-section view

  else
  {
    glPopMatrix();
    glPopAttrib();
    return;
  }

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glPopAttrib();

} // RenderCadFindings()


/**
 * Get a picked location in world coordinates.
 *
 * @param fPosX the x position in normalized image coordinates [0,1]
 * @param fPosY the y position in normalized image coordinates [0,1]
 *
 * @return the world coordinates
 */
Point<float4> SliceViewer::GetPickedLocationWorldCoord(const float4 fPosX, const float4 fPosY) const
{
  Point<float4> vPosVolumeCoord = GetPickedLocationVolumeCoord(fPosX, fPosY);
  const Triple<float4> units(m_bSliceRendererEnabled ? GetDataset()->m_fields.m_units2D : GetDataset()->m_vUnits);
  return Point<float4>(
    vPosVolumeCoord.m_x * units.m_x,
    vPosVolumeCoord.m_y * units.m_y,
    vPosVolumeCoord.m_z * units.m_z);
} // GetPickedLocationWorldCoord()


/**
 * Gets the aspect ratio and shift adjustment for each edge for non-square image volumes
 *
 * @param shiftPt         Shift in x or y to adjust
 * @param aspectRatioPt   Aspect ratio adjustment in width or height
 */
void SliceViewer::GetAspectRatioStats(Point2D<float4>& shiftPt, Point2D<float4>& ratioPt) const
{
  ratioPt.m_x = 1.0f; ratioPt.m_y = 1.0f;
  shiftPt.m_x = 0.0f; shiftPt.m_y = 0.0f;

  if (m_fWorldWidth < m_fWorldHeight)
  {
    shiftPt.m_x =  ((m_fWorldHeight-m_fWorldWidth) / 2.0F) / m_fWorldHeight;
    ratioPt.m_x = m_fWorldWidth / m_fWorldHeight;
  }
  else if (m_fWorldWidth > m_fWorldHeight)
  {  
    shiftPt.m_y =  ((m_fWorldWidth-m_fWorldHeight) / 2.0F) / m_fWorldWidth;
    ratioPt.m_y = m_fWorldHeight / m_fWorldWidth;
  }
}


/**
 * Get a picked location in volume coordinates.
 *
 * @param fPosX the x position in normalized image coordinates [0,1]
 * @param fPosY the y position in normalized image coordinates [0,1]
 *
 * @return the volume coordinates
 */
Point<float4> SliceViewer::GetPickedLocationVolumeCoord(const float4 fPosX, const float4 fPosY) const
{
  Point<float4> vPosVolumeCoord;
  Dataset & myDataset = * GetDataset();
  const Triple<float4> dim(m_bSliceRendererEnabled ? myDataset.m_fields.m_sliceDimensions : myDataset.m_vDim);
  const Triple<float4> units(m_bSliceRendererEnabled ? myDataset.m_fields.m_units2D : myDataset.m_vUnits);

  // Calculate aspect ratio and shift
  Point2D<float4> shiftPt, aspectRatioPt;
  GetAspectRatioStats(shiftPt, aspectRatioPt);

  // adjust the position after zooming in/out.
  float4 x, y;
  x = Interpolate( m_rZoom.m_left + shiftPt.m_x*m_rZoom.Width(), fPosX, m_rZoom.m_right - shiftPt.m_x*m_rZoom.Width(), 0.0F, 1.0F );
  y = Interpolate( m_rZoom.m_top + shiftPt.m_y*m_rZoom.Height(), fPosY, m_rZoom.m_bottom - shiftPt.m_y*m_rZoom.Height(), 0.0F, 1.0F );

  switch (m_eSliceOrientation) 
  {
    case eAXIAL:
      vPosVolumeCoord.m_x = x * dim.m_x;
      vPosVolumeCoord.m_y = y * dim.m_y;
      vPosVolumeCoord.m_z = m_iSliceNumber;
      break;
    case eSAGITTAL:
      // flip vertically
      vPosVolumeCoord.m_x = m_iSliceNumber;
      if ( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
      {
        vPosVolumeCoord.m_y = x * dim.m_y;
        vPosVolumeCoord.m_z = (1.0-y) * dim.m_z;
      }
      else
      {
        vPosVolumeCoord.m_y = y * dim.m_y;
        vPosVolumeCoord.m_z = (1.0-x) * dim.m_z;
      }
      break;
    case eCORONAL:
      vPosVolumeCoord.m_x = x * dim.m_x;
      vPosVolumeCoord.m_y = m_iSliceNumber;
      vPosVolumeCoord.m_z = (1.0-y) * dim.m_z;
      break;
    case eCROSS_SECTION:
    case eVIEWDIR:
    {
      vPosVolumeCoord = Point<float4>(
        Vector<float4>(m_bottomLeft *(1.0F-fPosY)*(1.0F-fPosX)) + 
        Vector<float4>(m_bottomRight*(1.0F-fPosY)*(     fPosX)) + 
        Vector<float4>(m_topLeft    *(     fPosY)*(1.0F-fPosX)) + 
        Vector<float4>(m_topRight   *(     fPosY)*(     fPosX))   );
      vPosVolumeCoord.m_x /= units.m_x;
      vPosVolumeCoord.m_y /= units.m_y;
      vPosVolumeCoord.m_z /= units.m_z;
    }
    break;
    default:
      // todo: log error
      break;
  }
  return vPosVolumeCoord;
} // GetPickedLocationVolumeCoord


/**
 * Move the selected location to the center of the screen
 *
 * @param fPosX the x position in normalized image coordinates [0,1]
 * @param fPosY the y position in normalized image coordinates [0,1]
 */
void SliceViewer::MovePickedLocationToVoxelCenter(float4 & fPosX, float4 & fPosY) const
{
  Dataset & myDataset = * GetDataset();

  float4 fXPercent, fYPercent;
  fXPercent = (fPosX - m_rZoom.m_left) / (m_rZoom.Width());
  fYPercent = (fPosY - m_rZoom.m_top ) / (m_rZoom.Height());

  float4 fXPos, fYPos;
  uint4 uXpos, uYpos;
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
    fXPos = fXPercent * myDataset.m_vDim.m_x;
    fYPos = fYPercent * myDataset.m_vDim.m_y;
    uXpos = fXPos;
    uYpos = fYPos;
    fXPercent = float4(uXpos)/float4(myDataset.m_vDim.m_x);
    fYPercent = float4(uYpos)/float4(myDataset.m_vDim.m_y);
    break;
  case eSAGITTAL:
    fXPos = fXPercent * myDataset.m_vDim.m_z;
    fYPos = fYPercent * myDataset.m_vDim.m_y;
    uXpos = fXPos;
    uYpos = fYPos;
    fXPercent = float4(uXpos)/float4(myDataset.m_vDim.m_z);
    fYPercent = float4(uYpos)/float4(myDataset.m_vDim.m_y);
    break;
  case eCORONAL:
    fXPos = fXPercent * myDataset.m_vDim.m_x;
    fYPos = fYPercent * myDataset.m_vDim.m_z;
    uXpos = fXPos;
    uYpos = fYPos;
    fXPercent = float4(uXpos)/float4(myDataset.m_vDim.m_x);
    fYPercent = float4(uYpos)/float4(myDataset.m_vDim.m_z);
    break;
  case eCROSS_SECTION:
  case eVIEWDIR:
    return;
    break;
  default:
    // todo: log error
    break;
  }
  fPosX = (fXPercent*m_rZoom.Width() )+m_rZoom.m_left;
  fPosY = (fYPercent*m_rZoom.Height())+m_rZoom.m_top;
  return;
} // MovePickedLocationToVoxelCenter()


/**
 * Is the volume loaded?
 *
 * @return is the volume loaded
 */
bool SliceViewer::IsVolumeLoaded() const
{
  switch (m_eSliceOrientation) 
  {
  case eAXIAL:
  case eSAGITTAL:
  case eCORONAL:
    return (m_uNumOfSlices!=0);
    break;
  case eCROSS_SECTION:
  case eVIEWDIR:
    return GetDataset()->IsLoadingComplete();
    break;
  default:
    // todo: log error
    break;
  }
  return false;
} // IsVolumeLoaded()


/**
 * Draw a 2D frame according to values of m_rZoom.
 */
void SliceViewer::Draw2DOverviewSliceFrame()
{
  // display a frame that around the slice.
  glColor3f(0.0f, 0.2f, 1.0f);
  glLineWidth(2.0);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glBegin(GL_QUADS);
    glVertex2f(0.00f, 0.75f);
    glVertex2f(0.25f, 0.75f);
    glVertex2f(0.25f, 1.00f);
    glVertex2f(0.00f, 1.00f);
  glEnd();

  Point2D<float4> shiftPt, aspectRatioPt;
  GetAspectRatioStats(shiftPt, aspectRatioPt);

  float4 deltaX = m_rZoom.Width();
  float4 deltaY = m_rZoom.Height();
  glLineWidth(1.0);
  // display frame that represents the selected area.
  if ( m_bDisplayOverviewSliceGrasp || (m_rSelect.m_left < 9999) )
  {
    glColor3f(1.0f, 1.0f, 0.0f);
    float4 x0 = - m_rZoom.m_left * 0.25f / deltaX;
    float4 y0 = - m_rZoom.m_top * 0.25f / deltaY;
    float4 x1 = (1.0f - m_rZoom.m_left) * 0.25f / deltaX;
    float4 y1 = (1.0f - m_rZoom.m_top) * 0.25f / deltaY;
    glBegin(GL_QUADS);
      glVertex2f(x0, y0+0.75f);
      glVertex2f(x1, y0+0.75f);
      glVertex2f(x1, y1+0.75f);
      glVertex2f(x0, y1+0.75f);
    glEnd();
  }

  // display frame around entire overview
  if ( (m_rSelect.m_left < 9999) ) 
  {
    glColor3f(1.0f, 0.0f, 1.0f);
    float4 ratioX = 1.0f / (float4)m_uDisplayWidth;
    float4 ratioY = 1.0f / (float4)m_uDisplayHeight;

    float4 x0 = ((float4) m_rSelect.m_left  * ratioX - m_rZoom.m_left) / deltaX * 0.25;
    float4 x1 = ((float4) m_rSelect.m_right * ratioX - m_rZoom.m_left) / deltaX * 0.25;
    float4 y0 = ((float4) m_rSelect.m_top   * ratioY - m_rZoom.m_top)  / deltaY * 0.25;
    float4 y1 = ((float4) m_rSelect.m_bottom* ratioY - m_rZoom.m_top)  / deltaY * 0.25;

    glBegin(GL_QUADS);
      glVertex2f(x0, y0+0.75f);
      glVertex2f(x1, y0+0.75f);
      glVertex2f(x1, y1+0.75f);
      glVertex2f(x0, y1+0.75f);
    glEnd();
  }

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
} // Draw2DOverviewSliceFrame().


/**
 * Draw a 2D frame, described by vRect.
 *
 * @param vRect   reference to a Rect.
 */
void SliceViewer::Draw2DOverviewSliceFrame(Rect<float4> & vRect)
{
  // display a frame that around the slice.
  glColor3f(0.0f, 0.2f, 1.0f);
  glLineWidth(2.0);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glBegin(GL_QUADS);
    glVertex2f(0.00f, 0.75f);
    glVertex2f(0.25f, 0.75f);
    glVertex2f(0.25f, 1.00f);
    glVertex2f(0.00f, 1.00f);
  glEnd();

  glLineWidth(1.0);
  glColor3f(1.0f, 1.0f, 0.0f);
  glBegin(GL_QUADS);
    glVertex2f(vRect.m_left,  vRect.m_bottom);
    glVertex2f(vRect.m_right, vRect.m_bottom);
    glVertex2f(vRect.m_right, vRect.m_top);
    glVertex2f(vRect.m_left,  vRect.m_top);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
} // Draw2DOverviewSliceFrame()


/**
 * draw a frame that represents an area selected.
 */
void SliceViewer::DrawSelectionFrame()
{
  glColor3f(1.0f, 1.0f, 0.0f);
  float4 ratioX = 1.0f / (float4)m_uDisplayWidth;
  float4 ratioY = 1.0f / (float4)m_uDisplayHeight;
      
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glBegin(GL_QUADS);
    glVertex2f((float4) m_rSelect.m_left * ratioX,  (float4) m_rSelect.m_top * ratioY);
    glVertex2f((float4) m_rSelect.m_left * ratioX,  (float4) m_rSelect.m_bottom * ratioY);
    glVertex2f((float4) m_rSelect.m_right * ratioX, (float4) m_rSelect.m_bottom * ratioY);
    glVertex2f((float4) m_rSelect.m_right * ratioX, (float4) m_rSelect.m_top * ratioY);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
} // DrawSelectionFrame()


/**
 * Returns the current dataset
 *
 * @return the current dataset
 */
Dataset * SliceViewer::GetDataset() const
{
  switch (m_eDatasetType) 
  {
  case DatasetOrientations::eSUPINE:
    return & rdrGlobal.m_supine;
    break;
  case DatasetOrientations::ePRONE:
    return & rdrGlobal.m_prone;
    break;
  case DatasetOrientations::ePRIMARY:
    return rdrGlobal.m_pCurrDataset;
    break;
  case DatasetOrientations::eSECONDARY:
    return rdrGlobal.m_pSecondaryDataset;
    break;
  default:
    LogRec("bad dataset case","SliceViewer", "GetDataset");
    return & rdrGlobal.m_nullDataset;
    break;
  }
  ex::IllegalArgumentException(LogRec("illegal dataset type value in switch statement","SliceViewer", "GetDataset"));

  return & rdrGlobal.m_nullDataset;
} // GetDataset()


/**
 * Gets the type of the current dataset
 *
 * @return the type of the current dataset
 */
DatasetOrientations::TypeEnum SliceViewer::GetDatasetType() const
{
  return GetDataset()->GetOrientation();
} // GetDatasetType()


/**
 * Get the current slice number/distance
 * 
 * @return slice number/distance.
 */
uint4 SliceViewer::GetCurrentDisplayedSlice() const
{
  // should we use the current viewpoint
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
  case SliceOrientations::eSAGITTAL:
  case SliceOrientations::eCORONAL:
    return m_iSliceNumber;
    break;
  case SliceOrientations::eCROSS_SECTION:
    return m_iSliceNumber; 
    break;
  case SliceOrientations::eVIEWDIR:
    return m_fViewDirDistance;
    break;
  default:
    // todo: log error
    break;
  } // endswitch m_eSliceOrientation
  return 0;
} // GetCurrentDisplayedSlice().


/** 
 * Construct slice image from volume
 *
 * @param uSliceWidth         Slice Width
 * @param uSliceHeight        Slice Height
 * @param uSliceNo            Slice No.
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 * @param bShowUncleansed     Display cleansed or uncleansed image data
 */
void SliceViewer::CreateSliceImageFromVol(const uint4 uSliceWidth, const uint4 uSliceHeight, const uint4 uSliceNo,
                                             const SliceOrientations::TypeEnum eSliceOrientation,
                                             const bool bShowUncleansed)
{

  Dataset & myDataset = * GetDataset();
  uint1 * uSliceImagePtr = m_puSliceImage;
  
  switch (eSliceOrientation)
  {
  case eAXIAL:
  {
    uint4 uVolSliceWidth  = min(m_uSliceWidthMax, myDataset.m_fields.m_volumeRender.GetDim().m_x);
    uint4 uVolSliceHeight = min(m_uSliceHeightMax, myDataset.m_fields.m_volumeRender.GetDim().m_y);
    for (uint4 j=0; j<uVolSliceHeight; j++)
    {
      uSliceImagePtr = m_puSliceImage + j * uSliceWidth;
      for (uint4 i=0; i<uVolSliceWidth; i++)
      {
        if (bShowUncleansed)
          *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetUncleansedValue(Point<uint2>(i, j, uSliceNo));
        else
          *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetVoxelRaw(Point<uint2>(i, j, uSliceNo));
        
        uSliceImagePtr ++;
      }
    }
  }
  break;

  case eSAGITTAL:
  {
    uint4 uVolSliceWidth = min(m_uSliceWidthMax, myDataset.m_fields.m_volumeRender.GetDim().m_z);
    uint4 uVolSliceHeight = min(m_uSliceHeightMax, myDataset.m_fields.m_volumeRender.GetDim().m_y);

    if ( myDataset.m_fields.m_sliceRendererData.IsValid() && m_bSliceRendererEnabled )
    {
      for (uint4 i=0; i<uVolSliceWidth; i++)
      {
        uSliceImagePtr = m_puSliceImage + i * uSliceHeight;
        for (uint4 j=0; j<uVolSliceHeight; j++)
        {
          if (bShowUncleansed)
            *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetUncleansedValue(Point<uint2>(uSliceNo, j, i));
          else
            *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetVoxelRaw(Point<uint2>(uSliceNo, j, i));

          uSliceImagePtr ++;
        }
      }
    }
    else
    {
      for (uint4 j=0; j<uVolSliceHeight; j++)
      {
        uSliceImagePtr = m_puSliceImage + j * uSliceWidth;
        for (uint4 i=uVolSliceWidth-1; i>0; i--)
        {
          if (bShowUncleansed)
            *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetUncleansedValue(Point<uint2>(uSliceNo, j, i));
          else
            *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetVoxelRaw(Point<uint2>(uSliceNo, j, i));
        
          uSliceImagePtr ++;
        }
      }
    }
  }
  break;

  case eCORONAL:
  {
    uint4 uVolSliceWidth = min(m_uSliceWidthMax, myDataset.m_fields.m_volumeRender.GetDim().m_x);
    uint4 uVolSliceHeight = min(m_uSliceHeightMax, myDataset.m_fields.m_volumeRender.GetDim().m_z);
    for (uint4 j=0; j<uVolSliceHeight; j++)
    {
      uSliceImagePtr = m_puSliceImage + (uVolSliceHeight-1-j) * uSliceWidth;
      for (uint4 i=0; i<uVolSliceWidth; i++)
      {
        if (bShowUncleansed)
          *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetUncleansedValue(Point<uint2>(i, uSliceNo, j));
        else
          *uSliceImagePtr = myDataset.m_fields.m_volumeRender.GetVoxelRaw(Point<uint2>(i, uSliceNo, j));
        
        uSliceImagePtr ++;
      }
    }
  }
  break;

  default:
    break;
  } // end-switch(eSliceOrientation)
} // CreateSliceImageFromVol()


/** 
 * Construct slice image from volume
 *
 * @param uSliceWidth         Slice Width
 * @param uSliceHeight        Slice Height
 * @param uSliceNo            Slice No.
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 * @param bShowUncleansed     Display cleansed or uncleansed image data
 */
void SliceViewer::CreateSliceImageFromTestVol(const uint4 uSliceWidth, const uint4 uSliceHeight, const uint4 uSliceNo,
                                              const SliceOrientations::TypeEnum eSliceOrientation,
                                              const bool bShowUncleansed)
{

  Dataset & myDataset = * GetDataset();
  uint1 * uSliceImagePtr = m_uRGBSegImage;

  unsigned char * pTestVol = (unsigned char *) myDataset.m_fields.m_testVolumeUC.GetDataPtr();
  const uint4 yStep = myDataset.m_vDim.m_x;
  const uint4 zStep = myDataset.m_vDim.m_x* myDataset.m_vDim.m_y;
  
  switch (eSliceOrientation)
  {
  case eAXIAL:
  {
    uint4 uVolSliceWidth  = myDataset.m_fields.m_volumeRender.GetDim().m_x;
    uint4 uVolSliceHeight = myDataset.m_fields.m_volumeRender.GetDim().m_y;
    for (uint4 j=0; j<uVolSliceHeight; j++)
    {
      uSliceImagePtr = m_uRGBSegImage + j * uSliceWidth;
      for (uint4 i=0; i<uVolSliceWidth; i++)
      {
        *uSliceImagePtr++ = pTestVol[uSliceNo*zStep+j*yStep+i];
      }
    }
  }
  break;

  case eSAGITTAL:
  {
    uint4 uVolSliceWidth = myDataset.m_fields.m_volumeRender.GetDim().m_z;
    uint4 uVolSliceHeight = myDataset.m_fields.m_volumeRender.GetDim().m_y;
    for (uint4 j=0; j<uVolSliceHeight; j++)
    {
      uSliceImagePtr = m_uRGBSegImage + j * uSliceWidth;
      for (uint4 i=uVolSliceWidth-1; i>0; i--)
      {
        *uSliceImagePtr++ = pTestVol[i*zStep+j*yStep+uSliceNo];
      }
    }
  }
  break;

  case eCORONAL:
  {
    uint4 uVolSliceWidth = myDataset.m_fields.m_volumeRender.GetDim().m_x;
    uint4 uVolSliceHeight = myDataset.m_fields.m_volumeRender.GetDim().m_z;
    for (uint4 j=0; j<uVolSliceHeight; j++)
    {
      uSliceImagePtr = m_uRGBSegImage + (uVolSliceHeight-1-j) * uSliceWidth;
      for (uint4 i=0; i<uVolSliceWidth; i++)
      {
        *uSliceImagePtr++ = pTestVol[j*zStep+uSliceNo*yStep+i];
      }
    }
  }
  break;

  default:
    break;
  } // end-switch(eSliceOrientation)
} // CreateSliceImageFromTestVol()


/**
 * Creates a reverse-video image
 *
 * @param eDataFormat     image format
 * @param uImgWidth       the width & height of the image.
 * @param uImgHeight      the width & height of the image.
 * @param pImg            pointer to image buffer
 */
void SliceViewer::ReverseVideoImage(const ReverseVideoImageEnum eDataFormat, const uint4 uImgWidth, const uint4 uImgHeight, void* pImg)
{
  switch(eDataFormat)
  {
    case eRVI_Gray: // Gray (Byte)
    {
      uint1 *ptr = (uint1 *)pImg;
      for (uint4 uSize=0; uSize<uImgWidth * uImgHeight; uSize++)
      {
        *ptr++ = (uint1)(255 - *ptr);
      }
      break;
    }

    case eRVI_RGB: // RGB: inverse R<->B.
    {
      uint1 *ptr = (uint1 *)pImg;
      for (uint4 uSize=0; uSize<uImgWidth * uImgHeight; uSize++)
      {
        Swap(*ptr, *(ptr+2));
        ptr += 3;
      }
      break;
    }

    case eRVI_RGBA: // RGBA: inverse R<->B.
    {
      uint1 *ptr = (uint1 *)pImg;
      for (uint4 uSize=0; uSize<uImgWidth * uImgHeight; uSize++)
      {
        *ptr++ = (uint1)(255 - *ptr);
        *ptr++ = (uint1)(255 - *ptr);
        *ptr++ = (uint1)(255 - *ptr);
        ptr++;
      }
      break;
    }

    default:
      break;
  } // end-switch()

} // ReverseVideoImage()


/** 
 * get min power of 2 larger or equal to nValue.
 * 
 * @param uValue   input value.
 * @return         the min power of 2 larger or equal to input.
 */
uint4 SliceViewer::GetMaxPowerOf2(const uint4 uValue)
{
  uint4 uPower2 = 0x0100; // default = 512;
  if (uValue == uPower2)
    return uPower2;

  if (uValue < uPower2)
  {
    do 
    {
      uPower2 = uPower2 >> 1;
    } while (uValue < uPower2);

    return (uPower2 << 1);
  }
  else
  {
    do 
    {
      uPower2 = uPower2 << 1;
    } while (uValue > uPower2);

    return uPower2;
  }

  return 0;
} // GetMaxPowerOf2()


// $Log: SliceViewer.C,v $
// Revision 1.33.2.19  2010/04/22 18:34:09  dongqing
// fix the 2D auto fly Arrow in Center of Slice for Sagittle view
//
// Revision 1.33.2.18  2010/02/03 22:44:17  dongqing
// add Components data ready check to avoid crash at the Orthogonal layout
// in the Verification tab
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.33.2.17  2009/11/10 19:20:48  dongqing
// Issue Cad09-02: giving overview cad marks colours to indicate visibility setting.
//
// Revision 1.33.2.16  2009/01/21 22:03:41  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.33.2.15  2009/01/12 23:14:51  jmeade
// comments
//
// Revision 1.33.2.14  2008/11/05 05:56:10  jmeade
// Issue : use spine-down orientation of sagittal view for older studies.
//
// Revision 1.33.2.13  2008/10/20 21:17:31  dongqing
// fix the bug after change of display sagittal orientation to standard
//
// Revision 1.33.2.12  2008/10/11 00:14:36  jmeade
// swinging the sagittal slice into upright position.
//
// Revision 1.33.2.11  2008/10/06 19:59:10  dongqing
// add bool flag to indicate if user interacts with 2D x-sectional or 3D endo view
// This will tell the SliceViewer selecting proper oblique 2D view renderer setting.
// This allows the 2D x-sectional view aligned to 3D endo orientation while user
// interacts 3D endo view.
//
// Revision 1.33.2.10  2008/05/30 21:24:09  jmeade
// Issue 5967: incorrect calculation of displayed slice number for sagittal and coronal.
//
// Revision 1.33.2.9  2008/03/25 02:26:19  jmeade
// Issue 6012: Copy code for change from Branch: CON_3-0_RC407_Patch080212
//
// Revision 1.33.2.8  2008/02/14 06:59:16  jmeade
// substandard enum classifier.
//
// Revision 1.33.2.7  2007/10/22 19:43:32  dongqing
// add fly direction check for generating the X-sectional image orientation
//
// Revision 1.33.2.6  2007/09/13 20:46:02  jmeade
// comment.
//
// Revision 1.33.2.5  2007/05/04 21:19:22  jmeade
// Issue 5642: ensure using proper value for number of view-direction slices.
//
// Revision 1.33.2.4  2007/03/22 20:41:51  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.33.2.3  2007/03/16 16:58:49  dongqing
// finally finish the enhancement of the cross-sectional view going
// thru entire colon
//
// Revision 1.33.2.2  2007/03/15 21:58:30  dongqing
// 80% done on X-section view enhancements. CAD mark in
//
// Revision 1.33.2.1  2007/03/15 21:36:08  jmeade
// Issue 5491: removed debug code.
//
// Revision 1.33  2007/03/14 21:52:44  dongqing
// make the cross-section oblique the same viewer mechanism as that of
// orthogonal views
//
// Revision 1.32  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.31  2007/03/05 23:17:53  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.30  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.29  2006/11/21 23:45:25  jmeade
// Issue 5180: don't draw cad marks in x-sec/viewdir orientations.
//
// Revision 1.28  2006/10/04 20:55:35  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.27  2006/09/22 22:39:14  jmeade
// Issue 4924: Don't display annotations in oblique views.
//
// Revision 1.26  2006/09/19 18:50:23  frank
// Issue #4996: Fixed the selection point computation on coronal and sagittal
//
// Revision 1.25  2006/09/14 17:44:57  frank
// Issue #4608: Scaled the CAD mark with the image zoom
//
// Revision 1.24  2006/09/13 15:31:49  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.23  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.22  2006/02/28 17:06:56  jmeade
// code review.
//
// Revision 1.21  2006/02/13 12:57:59  frank
// code review
//
// Revision 1.20  2006/02/09 16:22:53  jmeade
// code review: comments.
//
// Revision 1.19  2006/02/09 15:46:59  jmeade
// code review: comments.
//
// Revision 1.18  2006/02/08 17:49:04  jmeade
// code review: comments.
//
// Revision 1.17  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.16  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.15  2005/11/04 20:04:34  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.14.2.8  2006/03/29 20:53:36  frank
// read the segment number from the cad findings file
//
// Revision 1.14.2.7  2006/03/28 20:46:15  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.14.2.6  2006/03/24 23:45:36  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.14.2.5  2006/03/22 20:56:21  frank
// moved the finding visibility and other information into a class
//
// Revision 1.14.2.4  2006/03/03 01:55:50  jmeade
// Issue 4620: Now that annotations are full-3D, they can be displayed in any orientation.
//
// Revision 1.14.2.3  2006/01/05 21:41:20  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.14.2.2  2005/12/12 14:39:26  frank
// Issue #4561: Changed 2D arrows to circles
//
// Revision 1.14.2.1  2005/11/04 19:25:43  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.14  2005/11/04 01:14:07  jmeade
// updated
//
// Revision 1.13  2005/11/03 13:41:35  frank
// Issue #4508: Fixed HU display on oblique
//
// Revision 1.12  2005/10/31 13:19:49  frank
// removed CAD arrows from oblique views
//
// Revision 1.11  2005/10/28 19:00:03  geconomos
// issue#  4508 reworked zomming of oblique slices to show additional data when zooming out
//
// Revision 1.10  2005/10/13 18:11:31  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.9  2005/10/07 20:33:56  vxconfig
// changed the arrow color a little
//
// Revision 1.8  2005/10/05 17:39:08  vxconfig
// hid hidden cad marks
//
// Revision 1.7  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.6  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.5  2005/09/27 01:20:43  jmeade
// softer colour for findings arrows.
//
// Revision 1.4  2005/09/26 14:12:36  vxconfig
// hid invisible cad findings
//
// Revision 1.3  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.2  2005/09/23 03:19:56  vxconfig
// working on display of cad findings
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
// Revision 3.47.2.13.2.31.2.5  2005/07/15 17:22:00  geconomos
// Fixed rendering error for zoom window.
//
// Revision 3.47.2.13.2.31.2.4  2005/07/15 13:07:15  frank
// blended overview image for pan/zoom
//
// Revision 3.47.2.13.2.31.2.3  2005/07/14 20:18:16  frank
// Issue #4273: No longer require a separate compilation of vxViewer DLL
//
// Revision 3.47.2.13.2.31.2.2  2005/06/30 14:53:09  geconomos
// Issue #4229: Changed slice number on oblique views to show cm from anal verge rather than 0->100 or skeleton node indices
//
// Revision 3.47.2.13.2.31.2.1  2005/06/23 12:17:32  geconomos
// Issue# 4203: added parameter to RenderSlice indicating to use the cached image or not
//
// Revision 3.47.2.13.2.31  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.47.2.13.2.30  2005/06/03 20:24:16  geconomos
// exposed oblique image size
//
// Revision 3.47.2.13.2.29  2005/04/23 01:13:22  frank
// Issue #4145: Fixed camera display and picking on sagittal and coronal.
//
// Revision 3.47.2.13.2.28  2005/04/22 16:48:58  frank
// cleaned up field class interface
//
// Revision 3.47.2.13.2.27  2005/04/21 13:51:30  frank
// Issue #4052: fixed page up/down rounding
//
// Revision 3.47.2.13.2.26  2005/04/21 00:04:02  jmeade
// Issue 4124: fix unsigned int getting assigned negative values.
//
// Revision 3.47.2.13.2.25  2005/04/14 17:04:47  frank
// Issue #4121: Enabled displaying with full resolution
//
// Revision 3.47.2.13.2.24  2005/04/13 20:45:45  frank
// Issue #4052: Fixed rounding problem with page up/down
//
// Revision 3.47.2.13.2.23  2005/04/13 18:33:21  frank
// spelling
//
// Revision 3.47.2.13.2.22  2005/04/13 15:29:27  frank
// Issue #4112: Improved exception handling
//
// Revision 3.47.2.13.2.21  2005/04/08 20:57:46  frank
// Issue #4052: Fixed rounding of slice numbers
//
// Revision 3.47.2.13.2.20  2005/04/05 19:21:38  frank
// Issue #4105: Stopped flying when overview is double clicked
//
// Revision 3.47.2.13.2.19  2005/04/05 14:51:15  frank
// returned early if slice rendering failed
//
// Revision 3.47.2.13.2.18  2005/04/04 20:33:08  frank
// Issue #4101: Fixed HU display on oblique views
//
// Revision 3.47.2.13.2.17  2005/04/04 18:35:05  frank
// Fixed HU display on sagittal and coronal prone images
//
// Revision 3.47.2.13.2.16  2005/04/01 23:34:10  jmeade
// Setting arrow-in-slice-center back to false (as in previous versions).
//
// Revision 3.47.2.13.2.15  2005/04/01 01:19:20  frank
// Issue #4079: fixing cross sectional image slicing
//
// Revision 3.47.2.13.2.14  2005/03/31 15:59:33  frank
// Issue #4079: Fixed slice number for oblique views
//
// Revision 3.47.2.13.2.13  2005/03/30 20:51:36  jmeade
// reset force-redraw flag after drawing obliques.
//
// Revision 3.47.2.13.2.12  2005/03/30 17:01:05  frank
// removed extra semicolon
//
// Revision 3.47.2.13.2.11  2005/03/29 17:12:14  frank
// Issue #4062: Fixed jumping to annotations in 2D - refresh problem
//
// Revision 3.47.2.13.2.10  2005/03/29 16:25:46  frank
// defaulted autofly to true
//
// Revision 3.47.2.13.2.9  2005/03/26 19:46:05  frank
// revamped main application idle/processing loop - checkin for Jeff
//
// Revision 3.47.2.13.2.8  2005/03/25 18:26:17  frank
// Issue #4034: Fixed shifting image during flight and auto-centering
//
// Revision 3.47.2.13.2.7  2005/03/24 19:31:26  frank
// Issue #4028: Reset to view position on both supine and prone at the same time if linked.
//
// Revision 3.47.2.13.2.6  2005/03/23 22:17:24  frank
// Issue #3983: Revamped the slice linking for all ortho directions
//
// Revision 3.47.2.13.2.5  2005/03/23 21:24:38  jmeade
// Issue 4034: Arrow in slice center not contigent on whether view is main window.
//
// Revision 3.47.2.13.2.4  2001/01/02 11:24:19  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 3.47.2.13.2.3  2005/03/15 20:09:18  frank
// Issue #4024: Fixed camera display and pan/zoom window display for non-square aspect ratios
//
// Revision 3.47.2.13.2.2  2005/03/15 17:11:08  frank
// Issue #4024: Fixed HU display for non-square aspect ratios
//
// Revision 3.47.2.13.2.1  2005/03/14 23:22:59  jmeade
// Merged last change from 1.4 branch.
//
// Revision 3.47.2.11.2.14  2005/03/14 21:43:30  frank
// Issue #4024: Fixed slice display aspect ratio when slices > 512
//
// Revision 3.47.2.11.2.13  2005/02/04 23:55:52  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.47.2.11.2.12  2004/11/11 19:59:45  jmeade
// fixed 2dpos --> volume pos calculation for non-square images.
//
// Revision 3.47.2.11.2.11  2004/11/11 19:39:07  jmeade
// fixed volume pos -->2dpos calculation for non-square images.
//
// Revision 3.47.2.11.2.10  2004/10/26 16:18:05  jmeade
// Zooming when displaying slice to account for aspect ratio.
//
// Revision 3.47.2.11.2.9  2004/10/19 23:52:29  jmeade
// prevent annoying debug message about uninitialized variable (until we decide to throw an exception on invalid case).
//
// Revision 3.47.2.11.2.8  2004/10/19 17:24:11  jmeade
// compile error from merge.
//
// Revision 3.47.2.11.2.7  2004/10/19 16:43:11  jmeade
// fixed oblique view direction selection (merge of 3.47.2.12).
//
// Revision 3.47.2.11.2.6  2004/10/07 21:09:19  jmeade
// Clear area behind 2D overview before draw (for undrawn areas in non-square images).
//
// Revision 3.47.2.11.2.5  2004/10/07 20:45:16  jmeade
// Display of 2D overview for non-sqare images.
//
// Revision 3.47.2.11.2.4  2004/10/07 00:29:08  jmeade
// Adjust 2D-point-picking coordinates for non-sqare images; One method for aspect ratio/shift calculation.
//
// Revision 3.47.2.11.2.3  2004/10/06 18:36:02  jmeade
// Adjust window position calculations (e.g. for camera arrow) for non-squared slice images.
//
// Revision 3.47.2.11.2.2  2004/10/06 18:03:07  jmeade
// Apply non-squared change for m_bHomogenity flag.
//
// Revision 3.47.2.11.2.1  2004/10/06 02:16:47  jmeade
// Adjust draw rectangle for non-squared slice images.
//
// Revision 3.47.2.11  2004/09/29 19:52:09  jmeade
// Preference to disable 2d auto flight.
//
// Revision 3.47.2.10  2004/09/22 21:56:48  jmeade
// Reverse-video imaging for new slice renderers.
//
// Revision 3.47.2.9  2004/09/10 18:42:04  jmeade
// Non-square display regions; Global slab thickness.
//
// Revision 3.47.2.8  2004/08/23 21:45:28  jmeade
// updated.
//
// Revision 3.47.2.7  2004/08/12 00:06:26  jmeade
// VCSettings determines whether to use high-quality data.
//
// Revision 3.47.2.6  2004/08/12 00:01:23  jmeade
// VCSettings determines whether to use high-quality data.
//
// Revision 3.47.2.5  2004/06/18 16:36:56  geconomos
// Fixed initialization of m_bSliceRendererEnabled flag.
//
// Revision 3.47.2.4  2004/04/23 15:33:56  geconomos
// Initial work for integrating in newer 12-bit slice renderers.
//
// Revision 3.47.2.3  2004/04/07 19:04:13  jmeade
// Issue 3578: Restrict array access for datasets larger than 512 slices.
//
// Revision 3.47.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.47.2.1  2003/03/26 23:16:04  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.47  2003/02/05 18:30:47  kevin
// Removed Ingmar's checkin which included conflicts
//
// Revision 3.46  2003/01/22 22:27:54  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.45  2003/01/21 19:42:31  jmeade
// Duplicate: Revision : 3.24.2.17: Issue 2825:Don't check and/or undo zoom level on every single draw!Sheesh!
//
// Revision 3.44  2003/01/03 17:35:13  kevin
// 2 Characters. It is amazing how important 2 character can be,
// The question is what elese did this screw up and Is this same bug
// elsewhere in the code.
//
// Revision 3.43  2001/01/02 19:15:30  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.42  2001/01/02 19:09:13  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.41  2002/12/20 19:26:10  ingmar
// the image constructor now needs a clear pixel
//
// Revision 3.40  2002/11/27 17:36:52  jmeade
// Sure, let's all just go removing functions all willy-nilly now.
//
// Revision 3.39  2002/11/24 17:49:16  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
// AND
// removed BinLi's delete and re-new of slice image memory. Now it is allocated once at the beginning.
//
// Revision 3.38  2002/11/15 16:48:16  kevin
// On 2D X-sect autoflight use force the use of the smoothed skelton upvectors
//
// Revision 3.37  2002/11/11 14:28:47  kevin
// Dim the border for segmetnal view
//
// Revision 3.36  2002/11/04 20:38:19  kevin
// Mad annotations 3D instead of 2D and added drawing of annotations in endoscopic view
//
// Revision 3.35  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 3.34  2002/09/13 00:50:04  jmeade
// ColonSession class for appropriate data.
//
// Revision 3.33  2002/09/11 19:18:07  kevin
// Oops, fixing world-to-volume coordinates on segmetnal view borke the cros section and viewdir. This fixes it.
//
// Revision 3.32  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.31  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.30  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.29  2002/05/20 17:29:55  binli
// bug fix: memory access error  when S&P studies have different level of number slices.
//
// Revision 3.28  2002/05/17 20:12:25  binli
// Issue 2104: distorted slice image
//
// Revision 3.27  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.26  2002/04/12 02:15:09  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.25  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.24.2.15  2002/08/13 15:28:41  jmeade
// Issue 2587:  Slice viewer updates slice based on changes to relevant modes.
//
// Revision 3.24.2.14  2002/08/01 14:48:15  jmeade
// Issue 2587:  Made a number of 2D slice options global.
//
// Revision 3.24.2.13  2002/06/02 01:02:28  jmeade
// Issue 2270: Expose camera arrow length value.
//
// Revision 3.24.2.12  2002/05/31 02:52:34  jmeade
// Issue 2270: Localize in one function the determination of current visible z-value.
//
// Revision 3.24.2.11  2002/05/30 16:53:08  binli
// issue 2325: different memory sizes for S&P texture images.
//
// Revision 3.24.2.10  2002/05/29 18:18:53  jmeade
// Issue 2270:  Camera loc display: Expose value of threshold for difference b/w current world pt and current slice;
// Enable alpha blending of annotation arrow (though RenderArrow fn doesn't yet support this).
//
// Revision 3.24.2.9  2002/05/25 05:38:52  jmeade
// Issue 2270:  Store slice orientation, x,y click pos in m_priority field.
//
// Revision 3.24.2.8  2002/05/23 16:44:40  jmeade
// Issue 2270:  Draw list of 2D annotations; Cleaning up mangled functions.
//
// Revision 3.24.2.7  2002/05/23 16:07:16  jmeade
// Issue 2303:  Only adjust view window to include position indicator when autoflying.
//
// Revision 3.24.2.6  2002/05/18 00:18:04  jmeade
// Issue 2248: Implemented scrolling to absolute slice in axial orientations.
//
// Revision 3.24.2.5  2002/05/17 18:58:56  binli
// issue 2014: appendix: thumnail
//
// Revision 3.24.2.4  2002/05/17 18:21:20  binli
// issue 2014: display slice with different w and h.
//
// Revision 3.24.2.3  2002/03/27 15:57:41  binli
// append to Revision 3.24.2.2: center arrow when zooming.
//
// Revision 3.24.2.2  2002/03/26 22:12:18  binli
// bug fix: when width and height have different power.
//
// Revision 3.24.2.1  2002/02/26 16:54:31  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.24  2002/01/24 22:34:16  binli
// issue 1578: condition to call ZoomAdjust()
//
// Revision 3.23  2002/01/22 17:10:29  binli
// issue 1547: non-mainview, disable 'flight along centerline'
//
// Revision 3.22  2002/01/17 21:31:22  binli
// issue 1521: proper setting of bool variable 'm_bArrowInSliceCenter'
//
// Revision 3.21  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.20  2001/12/27 22:12:19  jmeade
// Proper initialization of image buffers
//
// Revision 3.19  2001/12/21 23:59:28  jmeade
// Use a structure for rendered slice statistics.
//
// Revision 3.18  2001/12/21 03:53:09  jmeade
// Split off functionality that renders slice data into internal buffer (for use elsewhere than in RenderSlice).
//
// Revision 3.17  2001/11/21 17:42:13  binli
// call more accurate fucntion GetClosestSkeletonIndex() when 2D flying.
//
// Revision 3.16  2001/11/20 18:35:21  binli
// 2D step flight ('A'/'Z')
//
// Revision 3.15  2001/11/19 19:21:44  binli
// fixed problem to control the speed.
//
// Revision 3.14  2001/11/19 16:17:29  binli
// avoid jumping when start flying.
//
// Revision 3.13  2001/11/16 22:30:40  binli
// new speed control to 2D flight
//
// Revision 3.12  2001/11/15 18:59:17  binli
// flight direction: regular and non-regular
//
// Revision 3.11  2001/11/12 17:30:01  binli
// 2D (slice) flight: default choice: adjust viewport for more visible.
//
// Revision 3.10  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.9  2001/11/09 22:42:24  binli
// slice flight.
//
// Revision 3.8  2001/11/08 20:43:19  binli
// 2D flight
//
// Revision 3.7  2001/11/07 16:47:08  binli
// 2D render of uncleansed crosssec/viewdir slices
//
// Revision 3.6  2001/11/06 21:50:11  binli
// 2D flight along centerline
//
// Revision 3.5  2001/11/06 15:26:32  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.4  2001/11/02 16:13:55  binli
// 2D render from block vol (.diff)
//
// Revision 3.3  2001/11/01 17:11:12  binli
// 2D render from one vol. (.ecv]
//
// Revision 3.2  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1  2001/10/22 18:09:06  binli
// issue 1214: test arrow position when study is PRONE.
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 26 2001 17:04:20   jmeade
// Removed last remnant of registration project debugging code (which created, opened, and never closed debugRegistration.txt in the study directory)
// 
//    Rev 2.0   Sep 16 2001 23:40:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:14   ingmar
// Initial revision.
// Revision 1.138  2001/08/31 15:36:59  binli
// task 168 (tracckID 000620): slice shadow
//
// Revision 1.137  2001/08/30 18:20:04  binli
// task 175 (track ID 000558): set value to -1000 if out of range.
//
// Revision 1.136  2001/08/29 20:15:12  binli
// bug fix: id 1002: scroll Cross-Sectional slice.
//
// Revision 1.135  2001/08/21 20:23:46  binli
// bug 612: fixed
//
// Revision 1.134  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.133  2001/08/09 21:45:07  binli
// bug 000656: make sure no out of boundary
//
// Revision 1.132  2001/07/25 22:52:17  binli
// ID 89: new exception handling.
//
// Revision 1.131  2001/07/24 21:50:19  binli
// ID 89: exception handling in v1k GUI
//
// Revision 1.130  2001/07/24 16:45:29  ana
// Registration input file - path changed.
//
// Revision 1.129  2001/07/24 16:40:01  ana
// Added generation of registration input file.
//
// Revision 1.128  2001/07/20 16:41:21  binli
// defect 626: fixed
// (this is another example that mis-used study pointer: use current study attached to each view, not rdrGlobal.m_pStudy)
//
// Revision 1.127  2001/07/05 14:38:33  binli
// used Rect in Draw2DOverviewSlice()
//
// Revision 1.126  2001/07/03 15:20:38  binli
// better 2D slice zooming.
//
// Revision 1.125  2001/06/27 15:10:08  jmeade
// coding conventions, const where appropriate, tabs to spaces
//
// Revision 1.124  2001/06/26 16:20:55  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.123  2001/06/14 20:46:46  binli
// bug: direction of prone slice in EndoOrth/StandardView.
//
// Revision 1.122  2001/06/06 22:11:34  binli
// bug 000470: fixed
//
// Revision 1.121  2001/05/31 22:21:15  binli
// some works for code conventions and bug fixing (000489, 484)
//
// Revision 1.120  2001/05/25 20:28:58  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.119  2001/05/24 23:05:24  jmeade
// Slice difference computation must take into account the orientation (supine or prone)
//
// Revision 1.118  2001/05/22 14:53:20  binli
// dual-slice control: lock/unlock.
//
// Revision 1.117  2001/05/19 00:41:05  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.116  2001/05/17 20:42:19  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.115  2001/05/16 19:00:20  binli
// bug fixed: 000344
//
// Revision 1.114  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.113  2001/05/03 18:52:29  jmeade
// Flip displaying of Sagittal (left to right flip) and Coronal (front to back flip)
//
// Revision 1.112  2001/05/02 20:12:10  ingmar
// BookmarkList.
// Revision 1.111  2001/04/30 12:32:13  frank
// Reflected changes in Study class.
//
// Revision 1.110  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved objectC
//
// encapsulation before the Code Walkthrough
//
// Revision 1.109.2.1  2001/05/01 23:44:57  jmeade
// tabs to spaces, m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.109  2001/04/19 12:02:31  frank
// Fixed memory leak.
//
// Revision 1.108  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.107  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.106  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.105  2001/04/11 11:58:51  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.104  2001/04/11 11:22:59  frank
// Removed ulong, uint, uint2, uint1 datatypes.
//
// Revision 1.103  2001/04/06 13:53:23  frank
// Reflected changes in SegmentList class.
//
// Revision 1.102  2001/04/05 21:11:04  binli
// code standard: bracks/short/const...
//
// Revision 1.101  2001/04/05 16:27:06  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.100  2001/04/04 14:16:43  binli
// some Point -> Point2D
//
// Revision 1.99  2001/03/30 22:13:06  binli
// int-> int4
// float->float4
//
// Revision 1.98  2001/03/27 21:33:20  binli
// continued code walkthrough.
//
// Revision 1.97  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.96  2001/03/20 18:50:31  binli
// code walkthrough to reflect new code standard:
// 1> moved some inline functions to .inl
// 2> changed some variables name;
// 3> changed some variables properties from public to protected or private.
// 4> new bracket rules.
//
// Revision 1.95  2001/03/16 23:16:26  binli
// renamed some variables according to new code standard.
//
// Revision 1.94  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.93  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.92  2001/03/13 15:49:07  binli
// code walkthrough
//
// Revision 1.91  2001/03/07 20:51:26  jmeade
// removed change for axial 512x512 handling (was not an error);
// removed check for supine/prone in LeftText() and RightText() (always the same)
//
// Revision 1.90  2001/03/07 16:36:34  binli
// coordinate display of original slice and .ins/.csi/.pdi ... slices.
//
// Revision 1.89  2001/03/01 01:08:13  jmeade
// GetNumberOfSlices() reflects maximum 100 distance for xsec/view dir
//
// Revision 1.88  2001/03/01 00:44:03  jmeade
// ResetCurrentVolumePosition() resets xsec or view dir distance for those orientations;
// GetCurrentOrthoSlice() calls GetCurrentDisplayedSlice() for xsec and view dir
//
// Revision 1.87  2001/03/01 00:39:53  jmeade
// imposed a distance limit of 100 voxels (e.g. down centerline) for xsec and view dir distance
//
// Revision 1.86  2001/02/28 22:18:30  kevin
// Fixed the orientation of the crossectional view when looking the opposite direction
//
// Revision 1.85  2001/02/28 21:03:44  jmeade
// ScrollTo() method (for xsec/view); tabs to spaces; const & and Rect<int4> where there should be
//
// Revision 1.84  2001/02/27 23:43:16  jmeade
// hid m_eOrientation for safekeeping (more hiding to come)
//
// Revision 1.83  2001/02/12 17:14:52  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.82  2001/02/05 13:30:36  frank
// Made a single global null study.
//
// Revision 1.81  2001/02/02 17:21:53  liwei
// not to dispaly arrow in Verify View,
//
// Revision 1.80  2001/02/01 19:53:58  frank
// Fixed 2D slice image orientations.
//
// Revision 1.79  2001/01/30 02:06:59  jeff
// ResetCurrentVolumePosition()
//
// Revision 1.78  2001/01/29 16:24:42  kevin
// Added scaling of arrow and bookmark size
// to the view rect
//
// Revision 1.77  2001/01/26 20:56:07  frank
// Flipped drawing orientation of prone, coronal view.
//
// Revision 1.76  2001/01/26 19:15:39  frank
// Changed drawing orientation of prone, axial slice images.
//
// Revision 1.75  2001/01/25 19:36:08  frank
// Removed pointless caching.
//
// Revision 1.74  2001/01/25 16:37:54  frank
// Changed zoomed picking.
//
// Revision 1.73  2001/01/25 16:04:46  frank
// Removed m_pStudy
//
// Revision 1.72  2001/01/24 18:19:03  binli
// added control to switch display 'blocked' or 'smoothed' slice images.
//
// Revision 1.71  2001/01/24 17:23:37  binli
// corrected measurement
//
// Revision 1.70  2001/01/23 19:29:14  binli
// improved the visibility of arrow.
//
// Revision 1.69  2001/01/23 17:43:23  jeff
// enums should be capitalized
//
// Revision 1.68  2001/01/19 14:07:42  frank
// Added pixel color map features to slice rendering.
//
// Revision 1.67  2001/01/12 21:38:37  binli
// changed 'm_startMovePt' from uint to int4.
//
// Revision 1.66  2001/01/09 15:20:47  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.65  2001/01/08 21:47:22  binli
// put all 'CSI', 'INS', 'Paint', ' Pdr', under the zooming control.
//
// Revision 1.64  2001/01/08 21:01:17  binli
// did 'blocked'(nearest), not linear interpolation within the  texel after zooming.
//
// Revision 1.63  2001/01/08 18:42:59  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.62  2001/01/05 18:34:22  binli
// zooming control: zooming around an anchor point.
//
// Revision 1.61  2001/01/05 16:42:52  frank
// Fixed MPR after overview jump position.
//
// Revision 1.60  2001/01/05 00:52:57  jeff
// select and zoom rect now uses Rect class for better encapsulation, corresponding
// functionality completely unchanged!!; show thumbnail only when panning
//
// Revision 1.59  2001/01/04 19:11:39  binli
// code adjust and clean.
//
// Revision 1.58  2001/01/04 17:51:54  binli
// zooming control (10): display 2D overview slice image when select an area for zooming.
//
// Revision 1.57  2001/01/04 16:13:48  binli
// zooming control (8): display 2D overview slice imageon the right-left corner.
// zooming control (9): grasp the 2D slice and move.
//
// Revision 1.56  2001/01/03 19:45:38  jeff
// GetZoomPercentage()
//
// Revision 1.55  2001/01/03 19:36:19  jeff
// GetZoomPercentage()
//
// Revision 1.54  2001/01/03 19:10:01  frank
// Cleaned up some utility functions.
//
// Revision 1.53  2001/01/03 16:31:57  binli
// zooming (3) again: correctly pick the current value (after HU modification).
//
// Revision 1.52  2001/01/03 15:04:07  frank
// Used Math library classes to eliminate variables.
//
// Revision 1.51  2000/12/28 19:53:19  kevin
// Fixed segfault on no segments selected
//
// Revision 1.50  2000/12/28 00:01:13  jeff
// somehow this compiled before; honest!!! (TRANSVERSE ==> AXIAL)
//
// Revision 1.49  2000/12/27 23:03:35  jeff
// TRANSVERSE ==> AXIAL, CROSSECT ==> CROSS_SECTION
//
// Revision 1.48  2000/12/27 21:09:52  kevin
// Set arrow scale size according to IsMainViewWindow
//
// Revision 1.47  2000/12/27 20:22:08  binli
// zooming (6):make sure the arrow is visible  when double click in Overview.
//
// Revision 1.46  2000/12/27 17:52:21  kevin
// CHanges to remove Mprviewer and fix the oblique plane rendring in the overview
//
// Revision 1.45  2000/12/26 22:47:51  binli
// zooming: (3) get the value at the point after zooming;
// zooming: (4) rendering arrow
//
// Revision 1.44  2000/12/22 20:39:08  binli
// zooming: preliminary result(2): select area + keyboard control.
//
// Revision 1.43  2000/12/22 16:06:54  binli
// slice zooming: step 1: preliminary result.
//
// Revision 1.42  2000/12/21 16:33:50  kevin
// SMall steps towards mreging slicesview and mpr view
//
// Revision 1.41  2000/12/20 14:34:38  kevin
// Initial chnges to make mpr viewer and slice viewer one 2d slice viewer class
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SliceViewer.C,v 1.33.2.19 2010/04/22 18:34:09 dongqing Exp $
// $Id: SliceViewer.C,v 1.33.2.19 2010/04/22 18:34:09 dongqing Exp $
