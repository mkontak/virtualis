// $Id: vxRendererCpu3D.C,v 1.54 2007/02/13 22:32:27 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "vxRendererCpu3D.h"
#include "vxEnvironment.h"
#include "Timer.h"
#include "GLFontGeorge.h"
#include "vxCamera.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxBlockVolumeIterator.h"
#include "SpaceLeapInfoMaker.h"
#include "PixelRGBA.h"
#include "vxRenderingMode.h"
#include "vxUtils.h"
#include "vxRendererUtils.h"
#include "Matrix.h"
#include "vxWindowLevel.h"
#include "vxCropBox.h"
#include "vxRendererUtils.h"
#include "Point2D.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"
#include "vxMathUtils.h"
#include "ValueRange.h"
#include "vxIntensityConverter.h"
#include "vxWindowHandle.h"
#include "ImageUtilities.h"
#include "vxElementBool.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.54 $"
#define ALIGN16 __declspec(align(16)) 


// global const variables
const bool   Global_bShowAdaptiveSamplingPattern  = false;
const bool   Global_bShowSpaceLeapPattern         = false;
const bool   Global_bDebug                        = (Global_bShowAdaptiveSamplingPattern || Global_bShowSpaceLeapPattern);
const bool   Global_bShowTimingsInLog             = false;
const int4   Global_iMaxDifferenceColor  = 8;
const float4 Global_fInterSampleDistance = 0.5;
const float4 Global_fSpecularLight1Power = 8.0f;
const float4 Global_fKA = 0.20f;
const float4 Global_fKD = 0.70f;
const float4 Global_fKS = 0.30f;
const int4   Global_iSldBlockWidth = 16;
const bool   Global_bShowRenderingTime            = false; // bool to write out seconds per frame
const float4 Global_fSliceShadowOpacity = 0.5F;


// static member initializations
uint4 vxRendererCpu3D::m_uNumberOfTimers(0 );



// Enumerations
enum SamplingInfoEnum
{
  /// Space leap info enums
  eRayStart,             //0    // red    
  eMultipleEmptyBlocks,  //1    // green       
  eSingleEmptyBlock,     //2    // blue (dark) 
  eSingleEmptySmallBlock,//3    // orange      
  eInvisible,            //4    // pink        
  eInvisibleBacktrace,   //5    // yellow      
  eZeroOpacity,          //6    // light blue  
  eZeroOpacityBacktrace, //7     
  eColoredSample,       
  eGradientLocation,    
  eDummy1,    
  eDummy2,    
  eRayEnd,    

  /// adaptive sampling info enums
  eFirstRayInTile,          //0
  eAdditionalRay,           //1
  eInterpolatedColor,       //2
  eHeterogeniousRay,        //3
  eAdditionalRayPassTwo,    //4
  eInterpolatedColorPassTwo,//5
  eHeterogeniousRayPassTwo, //6

  eNowDrawImage
}; // enum SamplingInfoEnum


/** 
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererCpu3D::vxRendererCpu3D(vxEnvironment & environment)
:vxRendererSoftware(environment),
m_iIsVisibleMaskIntensity(0),
m_iIsVisibleMaskLabel(0),
m_fQualitySampleDistanceScaleResolutionDependent(1.0F),
m_bPreviousMomentum(false)
{
  if (Global_bDebug)
  {
    m_iSamplingPatternIndex = 0;
    m_evSamplingInfo.resize(2000);
    m_fvSamplingPos.resize(m_evSamplingInfo.size());

    for (int4 k(0); k<m_evSamplingInfo.size(); ++k)
    {
      m_evSamplingInfo[k] = 0;
    }
  }

  // initialize 1D transfer function
  m_pfColormap1d                   = NULL;
  m_pfColormap1dAlphaCorrected     = new float4[4*65536];
  m_pfColormap1d8bit               = NULL;
  m_pfColormap1d8bitPreIntegrated  = new float4[5*256];
  m_pfColormap1d8bitAlphaCorrected = new float4[4*256];
  m_pfColormap2d8bitPreIntegrated  = new float4[5*256*256];
  m_pfColormap2d8bitPreIntegratedOutOf1d = new float4[5*256*256];

  // indicate no need to draw on first run
  m_iImageSizeIndex = -1;
  m_iImageSizeIndexResponsive = 1;

  // setup sizes
  m_viImageSize.clear();
  m_viImageSize.push_back(32);
  m_viImageSize.push_back(64);
  m_viImageSize.push_back(128);
  m_viImageSize.push_back(256);
  m_viImageSize.push_back(512);
  m_viImageSize.push_back(1024);

  m_bNeedToStartTimer = false;
  m_bIntensityVolumeClamped = false;

  m_uCurrentlyUsedThreads = 0;

  // set up a unique timer name
  m_sTimerName = vxEventTimer::GetQualitySpeedTradeoffTimerName() + ToAscii(++m_uNumberOfTimers );
  if (m_uNumberOfTimers >= vxEventTimer::GetMaxNumberQualitySpeedTradoffTimers() ) m_uNumberOfTimers = 0;

} // Default constructor


/**
 * Destructor.
 */
vxRendererCpu3D::~vxRendererCpu3D()
{
  m_bCancelRendering = true;
  for (int i(0); i<GetNumberOfThreads(); i++)
    GetRenderThreadsPtr()[i].WaitForRenderingToComplete();
  m_bCancelRendering = false;

  if (m_pfColormap1dAlphaCorrected != NULL)
  {
    delete [] m_pfColormap1dAlphaCorrected;
    m_pfColormap1dAlphaCorrected = NULL;
  }

  if (m_pfColormap1d8bitAlphaCorrected != NULL)
  {
    delete [] m_pfColormap1d8bitAlphaCorrected;
    m_pfColormap1d8bitAlphaCorrected = NULL;
  }

  if (m_pfColormap1d8bitPreIntegrated != NULL)
  {
    delete [] m_pfColormap1d8bitPreIntegrated;
    m_pfColormap1d8bitPreIntegrated = NULL;
  }

  if (m_pfColormap2d8bitPreIntegrated != NULL)
  {
    delete [] m_pfColormap2d8bitPreIntegrated;
    m_pfColormap2d8bitPreIntegrated = NULL;
  }

  if (m_pfColormap2d8bitPreIntegratedOutOf1d != NULL)
  {
    delete [] m_pfColormap2d8bitPreIntegratedOutOf1d;
    m_pfColormap2d8bitPreIntegratedOutOf1d = NULL;
  }
} // Destructor


//////////////////////////////////////////////////////////////////////
//// Initialization and cleanup

/**
 * Initialize the render environment.
 */
void vxRendererCpu3D::Initialize()
{
  try
  {
    vxRendererSoftware::Initialize();

    // cache rendering mode
    m_renderingMode = *GetEnvironment().GetRenderingMode();

    // initialize label volume
    m_labelRenderVol.Clear(0);

    // initialize image buffer
    GetImage().SetClearColor(*GetEnvironment().GetBackgroundColor());
    SetImageSize(0);

    //LogDbg("create texture (initialization)", "", "");
    CreateTexture();

    // set sample distance
    m_illuminator.SetPhongConstants(Global_fKA, Global_fKD, Global_fKS, Global_fSpecularLight1Power);

    m_bShading = false;
    m_bShadowing = false;
    m_bJittering = false;
    m_bPreIntegration = false;
    m_bSampleEveryPixel = false;
    m_bShowCutsShaded = false;
    m_bShowCutsGrayscale = false;
    m_bIsSliceShadowEnabled = false;
    m_bClipSliceShadow = false;
    m_iSliceShadowOrientation = -2;
    m_bIsShowIntensityRange = false;
    m_bIsProgressiveRefinement = false;
    m_bIsFirstRefinementStepPrevious = m_bIsFirstRefinementStep  = false;
    m_bCancelRendering = false;
    m_bNeedToStartTimer = false;
    m_pSliceShadowData = NULL;

    m_bIsBoxOfInterestActive = GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_BOX_OF_INTEREST) &&
                               GetEnvironment().GetElement(vxIDs::BoxOfInterest) != NULL;
  }
  catch (...)
  {
    throw ex::VException(LogRec("Initialize failed.", "vxRendererCpu3D", "Initialize"));
  }
} // Initialize() 


/**
 * Is the rendering mode change but checking only sensitive flags
 * @return true if sensitive flags changed
 */
bool vxRendererCpu3D::IsRenderingModeChanged()
{
  vxRenderingMode renderingModeNow(*GetEnvironment().GetRenderingMode());
  vxRenderingMode renderingModeBefore(m_renderingMode);

  // set the cached one to be the current one: ASSUMES THIS FUNCTION TO BE CALLED ONCE!!!
  m_renderingModePrevious = m_renderingMode;
  m_renderingMode = renderingModeNow;

  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_LABEL_ON_TOP) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_LABEL_ON_TOP))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_CROP_BOX) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_BOX_OF_INTEREST) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_BOX_OF_INTEREST))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_SHADOWS ) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_SHADOWS))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_SHADING) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_SHADING))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_JITTERING) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_JITTERING))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_PRE_INTEGRATION) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_PRE_INTEGRATION))
    return true;
  if (renderingModeNow.IsEnabled(vxRenderingMode::SHOW_EVERY_PIXEL) != renderingModeBefore.IsEnabled(vxRenderingMode::SHOW_EVERY_PIXEL))
    return true;
  if (renderingModeNow.GetRenderingAlgorithm() != renderingModeBefore.GetRenderingAlgorithm() )
    return true;
  if (renderingModeNow.GetSliceShadowMode() != renderingModeBefore.GetSliceShadowMode() )
    return true;
  if (renderingModeNow.GetRelativeMousePosition() != renderingModeBefore.GetRelativeMousePosition() )
    return true;

  return false;
} // IsRenderingModeChanged()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererCpu3D::IsRedrawNecessary()
{
  bool bRedrawNecessary(false); //vxRendererSoftware::IsRedrawNecessary()); ???

  bool bChangedVolumeIntensity(GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY));
  bool bChangedVolumeLabel(GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL));
  bool bChangedColormap(GetEnvironment().IsModified(vxEnvironment::COLORMAP));
  bool bChangedComponents(GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL));
  bool bChangedCropbox(GetEnvironment().IsModified(vxEnvironment::CROP_BOX) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX));
  bool bChangedSegmentation(GetEnvironment().IsModified(vxEnvironment::INTERACTIVE_SEGMENTATION) == true && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE));
  bool bChangedSliceShadow(GetEnvironment().IsModified(vxEnvironment::SLICE_SHADOWS) == true && GetEnvironment().GetRenderingMode()->GetSliceShadowMode() != vxRenderingMode::SHADOWS_NONE );
  bool bChangedCpu3dData(GetEnvironment().IsModified(vxEnvironment::DATA_RENDERER_SOFTWARE));
  bool bChangedCamera(GetEnvironment().IsModified(vxEnvironment::CAMERA));
  bool bChangedMomentum(GetEnvironment().IsModified(vxEnvironment::MOMENTUM));
  bool bChangedBackgroundColor(GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR));
  bool bChangedBoxOfInterest(GetEnvironment().IsModified(vxIDs::BoxOfInterest) && GetEnvironment().GetElement(vxIDs::BoxOfInterest) != NULL);
  bool bChangedEventTimerForProgressiveRefinement(GetEnvironment().IsModified(vxEnvironment::EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT) && m_bIsProgressiveRefinement == true);
  bool bChangedEventTimerForMomentum(GetEnvironment().IsModified(vxEnvironment::EVENT_TIMER_MOMENTUM) && m_bIsProgressiveRefinement == true);

  bRedrawNecessary |= bChangedVolumeIntensity;
  bRedrawNecessary |= bChangedVolumeLabel;
  bRedrawNecessary |= bChangedColormap;
  bRedrawNecessary |= bChangedComponents;
  bRedrawNecessary |= bChangedCropbox;
  bRedrawNecessary |= bChangedSegmentation;
  bRedrawNecessary |= bChangedSliceShadow;
  bRedrawNecessary |= bChangedCpu3dData;
  bRedrawNecessary |= bChangedCamera;
  bRedrawNecessary |= bChangedMomentum;
  bRedrawNecessary |= bChangedBackgroundColor;
  bRedrawNecessary |= bChangedBoxOfInterest;
  bRedrawNecessary |= bChangedEventTimerForProgressiveRefinement;
  bRedrawNecessary |= bChangedEventTimerForMomentum;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * PreRender routine
 * @return success
 */
int4 vxRendererCpu3D::PreRender()
{
  // set the thread priorities
  for ( int i(0); i<m_uCurrentlyUsedThreads; i++ )
  {
    const bool bActive( GetEnvironment().GetActiveEnvironment() == & GetEnvironment() );
    GetRenderThreadsPtr()[i].SetPriority( bActive ? Thread::NORMAL: Thread::LOWER );
    GetRenderThreadsPtr()[i].SetAffinity( i );
  }
  SetRenderingNecessary(false);

  // call based class
  vxRendererSoftware::PreRender();

  // find out maximum index size (size that is closest to current window size)
  uint2 uWindowSize(Min(GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight()));
  uint4 iMaxLevel(4);

  // find out what is the closest image size
  int4 i=0;
  for ( ; i<=iMaxLevel && m_viImageSize[i] < uWindowSize; i++)
    ;
  if (i != 0 && abs(m_viImageSize[i-1] - uWindowSize) < abs(m_viImageSize[i] - uWindowSize))
    m_iImageSizeIndexMax = i-1;
  else
    m_iImageSizeIndexMax = i;
  
  // detect whether momentum was switched off (should be part of vxManipulator::OnTimer(timername, timerevent)
  bool bMomentumJustSwitchedOff(GetEnvironment().GetMomentum()->GetInitiated() == false && m_bPreviousMomentum == true);
  m_bPreviousMomentum = GetEnvironment().GetMomentum()->GetInitiated();

  try
  {
    // exit early if no volume is loaded
    if (GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().X() <= 2)
    {
      return 1;
    }

    //LogDbg("RendererCpu3d: the following caused this: " + GetEnvironment().GetModifiedElementsString(), "", "");

    Timer timer;

    m_bIsAdjustFrameRate = false;

    // any of the threads active?
    m_bAnyThreadBusy = false;
    for (int i(0); i<m_uCurrentlyUsedThreads; i++)
      m_bAnyThreadBusy |= GetRenderThreadsPtr()[i].IsRendering();

    bool bIsRenderingModeChanged(IsRenderingModeChanged() );

    bool bNeedToRestart( false );

    bool bIsSliceShadowModified( false );

    bool bIsSomeEnvironmentElementModified = (GetEnvironment().IsModified(vxEnvironment::CAMERA)           ||
                                              GetEnvironment().IsModified(vxEnvironment::COLORMAP)         ||
                                              GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR) ||
                                              GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL) ||
                                              bIsRenderingModeChanged                                      ||
                                              bIsSliceShadowModified                                       ||
                                              GetEnvironment().IsModified(vxEnvironment::WIN_RECT)         ||
                                              GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY) ||
                                              GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL)     ||
                                              (GetEnvironment().IsModified(vxEnvironment::CROP_BOX) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))         ||
                                              GetEnvironment().IsModified(vxIDs::BoxOfInterest) && GetEnvironment().GetElement(vxIDs::BoxOfInterest) != NULL ||
                                              (GetEnvironment().IsModified(vxEnvironment::INTERACTIVE_SEGMENTATION) 
                                              && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE)));

    //string sIsModified = (bIsSomeEnvironmentElementModified) ? "M" : "m"; string sIsRefining = (m_bIsProgressiveRefinement) ? "R" : "r"; LogDbg(sIsModified + sIsRefining + GetEnvironment().GetModifiedElementsString(),"vxRendererCpu3D", "PreRender");

    if (m_bIsProgressiveRefinement == true)
    {
      // is it a timed event?
      if (bIsSomeEnvironmentElementModified == false)
      {
        //CreateTexture() // uncomment to show intermediate images for debugging only
        // did we complete the image that we worked on? HACK TO DO, go through all threads in case there are more than just two
        if (m_bAnyThreadBusy == false || m_bIsFirstRefinementStepPrevious == true)
        {
          // yes, then create the new texture image as long as it was not already created
          if (m_bIsFirstRefinementStepPrevious == false)
          {
            m_fLastFrameTime = m_frameRateTimer.ElapsedTime();
            GetEnvironment().GetDataRendererSoftware()->SetFrameTime(m_fLastFrameTime);

            if (Global_bShowRenderingTime)
            {
              LogDbg(m_frameRateTimer.AsString() + " spf for image size: " + ToAscii(m_viImageSize[m_iImageSizeIndex]), "vxRendererCpu3D", "PreRender");
            }

            //LogDbg("create texture (just completed)", "", "");
            CreateTexture();
          }

          // did we complete the image at highest level of refinement?
          if (m_iImageSizeIndex == m_iImageSizeIndexMax)
          {
            //LogDbg("Killing " + m_sTimerName + " (I) cause finest resolution was reached", "", "");
            //GetEnvironment().GetEventTimer()->KillTimer(m_sTimerName );
            //GetEnvironment().GetEventTimer()->Modified(vxModInfoTxt(vxEventTimer, vxUtils::EVENTTIMER_KILLED, m_sTimerName ) );
            m_bIsProgressiveRefinement = m_bIsFirstRefinementStepPrevious = m_bIsProgressiveRefinement = false;
          }
          else
          {
            // the image is not yet at max resolution, lets compute a bigger one in the background
            //LogDbg( "the image is not yet at max resolution, lets compute a bigger one in the background", "", "" );
            m_bIsFirstRefinementStepPrevious = m_bIsFirstRefinementStep;
            m_bIsFirstRefinementStep = false;
            SetRenderingNecessary(true);
            SetImageSize(m_iImageSizeIndex+1);
            //LogDbg("Increase image size (1)", "", "");
          }
        }
        return 1;
      }
      else
      {
        //LogDbg( "some other environment object was changed, let's cancel progressive", "", "" );
        // some other environment object was changed, let's cancel progressive 
        // refinement rendering and get back to responsive frame rate.
        ////////////////////////////////////////////////////////////////////////
        m_bIsProgressiveRefinement = true;
        m_bIsFirstRefinementStep = true;

        m_bCancelRendering = true;
        //Timer waiting;
        for (int i(0); i<m_uCurrentlyUsedThreads; i++)
          GetRenderThreadsPtr()[i].WaitForRenderingToComplete();
        //LogDbg( "waited for rendering to complete: " + ToAscii( waiting.ElapsedTime() ), "", "" );
        m_bCancelRendering = false;
        m_bAnyThreadBusy = false;

        //LogDbg("Get back to responsive frame-time: " + ToAscii(m_iImageSizeIndexResponsive), "", "");
        SetImageSize(m_iImageSizeIndexResponsive);
        SetRenderingNecessary(true);
      }
    }
    else if (GetEnvironment().IsModified(vxEnvironment::EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT) && bIsSomeEnvironmentElementModified == false)
    {
      // got here because there was no change in the last few ms 
      if (m_iImageSizeIndex < m_iImageSizeIndexMax)
      {
        //LogDbg( "the image is not yet at max resolution, lets compute a bigger one in the background", "", "" );
        // the image is not yet at max resolution, lets compute a bigger one in the background
        m_bIsProgressiveRefinement = true;
        m_bIsFirstRefinementStep = true;
        //LogDbg("Increase image size (2)", "", "");
        SetImageSize(m_iImageSizeIndex++);
        SetRenderingNecessary(true);
      }
      else
      {
        //LogDbg( "this is not happening anymore since the timer flag gets removed from", "", "" );
        // this is not happening anymore since the timer flag gets removed from 
        // the vxEnvironment if the event is cause due to the timer being killed
        //LogDbg("Killing " + m_sTimerName + " (II) --> index is " + ToAscii(m_iImageSizeIndex), "", "");
        //GetEnvironment().GetEventTimer()->KillTimer(m_sTimerName );
        //GetEnvironment().GetEventTimer()->Modified(vxModInfoTxt(vxEventTimer, vxUtils::EVENTTIMER_KILLED, m_sTimerName ) );
        return 1;        
      }
    }
    else if (bIsSomeEnvironmentElementModified == true || bMomentumJustSwitchedOff == true)
    {
      if( bMomentumJustSwitchedOff )
        int i = 0;
      //LogDbg( "ok, something is modified, so what about progressive refinement?", "", "" );
      // ok, something is modified, so what about progressive refinement?
      if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::AUTO_FLY_MODE ) == false && GetEnvironment().GetMomentum()->GetInitiated() == false)
      { 
        // obviously it is not auto fly and thus, we need to initialize progressive refinement
        m_bNeedToStartTimer = true;
        m_bIsProgressiveRefinement = true; 
        m_bIsFirstRefinementStep   = true;

        //LogDbg("Launching timer for " + m_sTimerName, "", "");
        //GetEnvironment().GetEventTimer()->SetTimer(m_sTimerName, 100);
        //GetEnvironment().GetEventTimer()->Modified(vxModInfoTxt(vxEventTimer, vxUtils::EVENTTIMER_STARTED, m_sTimerName));

        // must have changed the environment after max res had already been displayed
        //LogDbg("Increase image size (4)", "", "");
        SetImageSize(m_iImageSizeIndexResponsive);
        SetRenderingNecessary(true);
      }
    }

    ////////////////////////////////////////////////////////
    // here now comes all the regular checks for modified
    // stuff that is independent of progressive refinement
    ////////////////////////////////////////////////////////

    // set progress bar parameters
    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_LABEL_ON_TOP))
    {
      m_fFractionRaysBorder      = 0.03F;
      m_fFractionRaysFill        = 0.91F;
      m_fFractionRaysMissed      = 0.03F;
      m_fFractionRaysLabelsOnTop = 0.03F;
    }
    else
    {
      m_fFractionRaysBorder      = 0.03F;
      m_fFractionRaysFill        = 0.94F;
      m_fFractionRaysMissed      = 0.03F;
      m_fFractionRaysLabelsOnTop = 0.0F;
    }

    //// normal updates according to environment change (no longer progressiverefinement)
    bool bNeedToUpdateBoundingBox(false);
    bool bNeedToUpdateMaskForIntensities(false);
    bool bNeedToUpdateMaskForLabels(false);
    bool bNeedToUpdateSpaceLeap(false);
    bool bIsBoxOfInterestChanged(false);
    bool bIsCameraChanged(false);

    if (GetEnvironment().IsModified(vxIDs::BoxOfInterest) || 
        ((m_bIsBoxOfInterestActive != GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_BOX_OF_INTEREST)) && GetEnvironment().GetElement(vxIDs::BoxOfInterest) != NULL))
    {
      m_bIsBoxOfInterestActive = GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_BOX_OF_INTEREST) &&
                                 GetEnvironment().GetElement(vxIDs::BoxOfInterest) != NULL;
      bIsBoxOfInterestChanged = true;
    }

    if (GetEnvironment().IsModified(vxEnvironment::CROP_BOX) ||
        m_bIsCropBoxActive != GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX))
    {
      m_bIsCropBoxActive = GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX);
      bNeedToUpdateBoundingBox = true;
      bNeedToUpdateSpaceLeap = true; 
    }

    if (GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY ) )
    {
      UpdateRenderVolumeIntensity();
      bNeedToUpdateBoundingBox = true;
      bNeedToUpdateMaskForIntensities = true;
      bNeedToUpdateSpaceLeap = true;
      m_bIntensityVolumeClamped = true;
      bNeedToRestart = true;
    }

    if (GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL))
    {
      UpdateRenderVolumeLabel();
      bNeedToUpdateBoundingBox = true;
      bNeedToUpdateMaskForLabels = true;
      bNeedToUpdateSpaceLeap = true;
      bNeedToRestart = true;
    }

    if (GetEnvironment().IsModified(vxEnvironment::COLORMAP) )
    {
      UpdateTFunc2D();
      bNeedToUpdateSpaceLeap |= UpdateMaskForIntensities();
      bNeedToUpdateBoundingBox = true;
    }

    if (GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL))
    {
      // make local copy of the components in the environment
      // TODO: put this code in raytracer settings
      m_vComponents.resize(GetEnvironment().GetComponents()->ArrayByLabel().size());
      for (int4 k(0); k<m_vComponents.size(); ++k)
      {
        if( m_vComponents[k] ) delete m_vComponents[k];
        m_vComponents[k] = new vxComponent(*(GetEnvironment().GetComponents()->ArrayByLabel())[k]);
      }
      bNeedToUpdateBoundingBox = true;
      bNeedToUpdateSpaceLeap |= UpdateMaskForLabels();
    }

    bool bBackgroundColorChanged(false);
    if (GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR))
    {
      bBackgroundColorChanged = true;
    }

    if (GetEnvironment().IsModified(vxEnvironment::CAMERA))
    {
      bIsCameraChanged = true;
    }

    m_bIsShowIntensityRange = GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE);

    // compute the slice shadows enabled and clipping flags
    m_bIsSliceShadowEnabled = false;
    m_bClipSliceShadow = false;

   

    if (m_bIsBoxOfInterestActive) UpdateBoxOfInterest();
    if (bNeedToUpdateBoundingBox)
    {
      // this is the box we want to use for ray/volume intersection testing
      m_fVisibilityBox = vxUtils::GetVisibleBoundingBox(GetEnvironment() );
    }
    if (bNeedToUpdateMaskForIntensities) UpdateMaskForIntensities();
    if (bNeedToUpdateMaskForLabels)      UpdateMaskForLabels();
    if (bNeedToUpdateSpaceLeap)          UpdateVolumeSpaceLeap();

    if (bNeedToUpdateBoundingBox
         || bNeedToUpdateMaskForIntensities || bNeedToUpdateMaskForLabels
         || bNeedToUpdateSpaceLeap          || bIsCameraChanged
         || bIsRenderingModeChanged         || bIsBoxOfInterestChanged
         || bBackgroundColorChanged )
    {
      m_bIsAdjustFrameRate = true;
      m_bSampleEveryPixel = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_EVERY_PIXEL );
      m_bShowCutsShaded = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_SHADED );
      m_bShowCutsGrayscale = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE );
      m_bPreIntegration = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION );
      m_bShadowing = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADOWS );
      m_bShading = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADING );
      m_bJittering = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_JITTERING );
      m_camera = * GetEnvironment().GetCamera();
      SetRenderingNecessary(true);
    }

    if ( bNeedToRestart )
    {
      m_bCancelRendering = true;
      for (int i(0); i<m_uCurrentlyUsedThreads; i++)
        GetRenderThreadsPtr()[i].WaitForRenderingToComplete();
      m_bCancelRendering = false;
      m_bAnyThreadBusy = false;
      SetRenderingNecessary( true );
    }

    return 1;
  }
  catch (...)
  {
    throw ex::VException(LogRec("PreRender failed.", "vxRendererCpu3D", "PreRender"));
  }
} // PreRender()



//////////////////////////////////////////////////////////////////////
//// Making the image


/**
 * Render routine.
 */
void vxRendererCpu3D::Render()
{
  try
  {
    Timer timer;

    // save the OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // do we need to lauunch threads?
    if (GetRenderingNecessary() == true && GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().X() > 2) // need to re-generate the image?
    {
      LaunchThreads();
    }

    // draw image
    DrawTexture();

    // draw progress bar
    if (m_bIsProgressiveRefinement == true) 
    {
      DrawProgressBar();
    }

    // restore the OpenGL state
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
  }
  catch (...)
  {
    throw ex::VException(LogRec("Render failed.", "vxRendererCpu3D", "Render"));
  }
} // Render()


/**
 * PostRender routine.
 * @return 1 in case of success
 */
int4 vxRendererCpu3D::PostRender()
{
/*
  static uint2 uCount(0);
  uCount++;
  if (uCount == 20)
  {
    WriteTFunc1Dand2D();
    uCount = 0;
  }
*/
  try
  {
    if (m_renderingMode.IsFixedImageSizeRendering() == false)
    {
      // number of iterations before we can actually jump one level up in image quality, 
      // once we are able to compute the image faster than the highest frame-rate
      ////////////////////////////////////////////////////////////////////////////////////
      const int4 iLowPassSteps(5);

      if (GetRenderingNecessary() == true && m_bIsAdjustFrameRate == true) // did we re-generate the image?
      {
        SetRenderingNecessary(false);

        // scale thrust by speed slider (currently the cine/momentum slider)
        const float4 fSpeedTimer(m_renderingMode.GetQualityScaleFactor());
        const float4 fSpeedTimerMin(1.0F);
        const float4 fSpeedTimerMax(10.0F);
        const float4 fSpeedSlow(0.0F);
        const float4 fSpeedFast(2.0F);
        const float4 fSliderScale(Interpolate(fSpeedTimerMin, fSpeedTimer, fSpeedTimerMax, fSpeedSlow, fSpeedFast));

        const float4 fSpeedFactorLevel(fSliderScale); //0.0F); // could be 1, 2, 3, 4, etc.
        const float4 fSpeedFactorScale(1.0F/pow(3.0F,fSpeedFactorLevel));
        const float4 fFramesPerSecondMin(10.0F * fSpeedFactorScale);
        const float4 fFramesPerSecondMax(30.0F * fSpeedFactorScale);
        const float4 fFrameTimeMax(1/fFramesPerSecondMin);
        const float4 fFrameTimeMin(1/fFramesPerSecondMax);

        //LogDbg("Slider value is: " + ToAscii(fSpeedTimer), "", "");
        //LogDbg("Scale factor is: " + ToAscii(fSliderScale), "", "");
        //LogDbg("Min frames/sec: " + ToAscii(fFramesPerSecondMin), "", "");
        //LogDbg("Max frames/sec: " + ToAscii(fFramesPerSecondMax), "", "");
        //LogDbg("Frame time range: " + ToAscii(fFrameTimeMin) + " "  + ToAscii(m_fLastFrameTime) + " "  + ToAscii(fFrameTimeMax), "", "");

        // were we too slow in generating this frame?
        if (m_fLastFrameTime > fFrameTimeMax && m_iImageSizeIndex != 0)
        {
          //LogDbg("Interaction mode: decreased responsive image size index: " + ToAscii(m_iImageSizeIndex) + "(" + ToAscii(this) + ")", "", "");
          SetImageSize(m_iImageSizeIndex-1);
          m_iImageSizeIndexResponsive = m_iImageSizeIndex;
          //GetEnvironment().GetRenderingMode()->SetLowPassFilterFastFrames(iLowPassSteps);
        }
        // was it fast enough to switch to higher resolution (slower rendering)?
        else if (m_fLastFrameTime < fFrameTimeMin)
        {
          //// check low pass filter so that we do not continuously bounce between two resolutions
          //const int4 iCount(GetEnvironment().GetRenderingMode()->GetLowPassFilterFastFrames() - 1);
          //GetEnvironment().GetRenderingMode()->SetLowPassFilterFastFrames(iCount);

          //if (iCount < 0 && m_iImageSizeIndex != m_iImageSizeIndexMax)
          //{
          //  GetEnvironment().GetRenderingMode()->SetLowPassFilterFastFrames(0);
          //  m_iImageSizeIndexResponsive = m_iImageSizeIndex+1;
          //  //SetImageSize(m_iImageSizeIndexResponsive); // is already done in pre-render, otherwise it is double
          //  //LogDbg("Interaction mode: increased image size index: " + ToAscii(m_iImageSizeIndex), "", "");
          //}
        }
      }
      else
      {
//        GetEnvironment().GetRenderingMode()->SetLowPassFilterFastFrames(iLowPassSteps);
      }

      if (m_bNeedToStartTimer == true)
      {
        // need to flush since only after manipulation there is a flush but not 
        // after all rendering. The reason is that rendering should not alter any
        // environment data but due to this hack ... :-(The right thing would be
        // to have a manipulator that checks what is going on based on a timed 
        // event and the renderer "tells" the manipulator its state by putting the
        // important data in the vxRendering3Ddata
        vxModifiedQueue::Flush();
        m_bNeedToStartTimer = false;
      }

      m_bNeedToStartTimer = false;

      return 1;
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("PostRender failed.", "vxRendererCpu3D", "PostRender"));
  }

  return 0;
} // PostRender()


/**
 * distribute rendering of the current image across threads.
 */
void vxRendererCpu3D::LaunchThreads()
{
  if (m_renderingMode.IsFixedImageSizeRendering())
  {
    GetEnvironment().GetDataRendererSoftware()->GetDepthImage().Resize(Triplef(m_renderingMode.GetFixedImageSize().first, m_renderingMode.GetFixedImageSize().second+2, 1));
    GetImage().Resize(Triplef(m_renderingMode.GetFixedImageSize().first, m_renderingMode.GetFixedImageSize().second+2, 1));
    m_bIsProgressiveRefinement = false;
  }
  else if (m_bIsProgressiveRefinement == false && GetEnvironment().GetMomentum()->GetInitiated() == false)
  {
    return;
  }

  // clearing the image
  //LogDbg("clearing the image", "", "");
  GetImage().SetClearColor(*GetEnvironment().GetBackgroundColor());
  GetImage().Clear();

  GetEnvironment().GetDataRendererSoftware()->GetDepthImage().Clear();

  // in case momentum is on, do simply use responsive image size
  if (GetEnvironment().GetMomentum()->GetInitiated() == true)
  {
    //LogDbg("Increase image size (6)", "", "");
    SetImageSize(m_iImageSizeIndexResponsive);
  }

  SetCommonRenderArguments();

  const int4 iImgX(GetImage().GetDim().X()); 
  const int4 iImgY(GetImage().GetDim().Y()-2);
  const int4 iImgXHalf(iImgX/2);
  const int4 iImgYHalf(iImgY/2);

  if (m_bIsFirstRefinementStep == true || GetEnvironment().GetMomentum()->GetInitiated() == true )
  {
    //LogDbg("launching threads (blocking: " + ToAscii(iImgX) + ")", "", "");

    if (GetNumberOfThreads() == 1)
    {
      GetRenderThreadsPtr()[0].SetRenderer(this);
      GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(0, 0, iImgX, iImgY));
      GetRenderThreadsPtr()[0].StartRendering();
    }
    else if(GetNumberOfThreads() == 2 )
    {
      GetRenderThreadsPtr()[0].SetRenderer(this);
      GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(0, 0, iImgX, iImgYHalf));
      GetRenderThreadsPtr()[0].StartRendering();

      GetRenderThreadsPtr()[1].SetRenderer(this);
      GetRenderThreadsPtr()[1].SetWindow(BoundingRect<uint2>(0, iImgYHalf, iImgX, iImgY));
      GetRenderThreadsPtr()[1].StartRendering();
      
    }
    else
    {
      GetRenderThreadsPtr()[0].SetRenderer(this);
      GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(      0,         0, iImgXHalf, iImgYHalf));
      GetRenderThreadsPtr()[0].StartRendering();

      GetRenderThreadsPtr()[1].SetRenderer(this);
      GetRenderThreadsPtr()[1].SetWindow(BoundingRect<uint2>(iImgXHalf,         0,     iImgX, iImgYHalf));
      GetRenderThreadsPtr()[1].StartRendering();

      GetRenderThreadsPtr()[2].SetRenderer(this);
      GetRenderThreadsPtr()[2].SetWindow(BoundingRect<uint2>(      0, iImgYHalf, iImgXHalf, iImgY));
      GetRenderThreadsPtr()[2].StartRendering();

      GetRenderThreadsPtr()[3].SetRenderer(this);
      GetRenderThreadsPtr()[3].SetWindow(BoundingRect<uint2>(iImgXHalf, iImgYHalf,     iImgX, iImgY));
      GetRenderThreadsPtr()[3].StartRendering();
    }
    m_frameRateTimer.Reset();

    for( int i(0); i < GetNumberOfThreads(); ++i )
      GetRenderThreadsPtr()[i].WaitForRenderingToComplete();
    
    m_fLastFrameTime = m_frameRateTimer.ElapsedTime();
    GetEnvironment().GetDataRendererSoftware()->SetFrameTime(m_fLastFrameTime);

    if (Global_bShowRenderingTime)
    {
      //LogDbg(m_frameRateTimer.AsString() + " spf for image size: " + ToAscii(m_viImageSize[m_iImageSizeIndex]), "vxRendererCpu3D", "PostRender");
    }

    //LogDbg("create texture (was commented out)", "", "");
    CreateTexture();
    m_bIsFirstRefinementStepPrevious = m_bIsFirstRefinementStep;
    m_bIsFirstRefinementStep = false;

  }
  else if (m_bAnyThreadBusy == false && GetEnvironment().GetMomentum()->GetInitiated() == false)
  {
    int4 iNumOfThreads(GetNumberOfThreads());

    //LogDbg("launching threads (non-blocking: " + ToAscii(iImgX) + ")", "", "");
    switch (iNumOfThreads)
    {
    case 1: 
      { // single square tile
        m_uCurrentlyUsedThreads = 1;
        GetRenderThreadsPtr()[0].SetRenderer(this);
        GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(0, 0, iImgX, iImgY));
        GetRenderThreadsPtr()[0].StartRendering();
        m_frameRateTimer.Reset();
        break;
      }
    case 2: 
    case 3: 
      { // two horizontal stripes
        m_uCurrentlyUsedThreads = 2;
        GetRenderThreadsPtr()[0].SetRenderer(this);
        GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(0,         0, iImgX, iImgYHalf));
        GetRenderThreadsPtr()[0].StartRendering();

        GetRenderThreadsPtr()[1].SetRenderer(this);
        GetRenderThreadsPtr()[1].SetWindow(BoundingRect<uint2>(0, iImgYHalf, iImgX, iImgY));
        GetRenderThreadsPtr()[1].StartRendering();
        m_frameRateTimer.Reset();
        break;
      }
    case 4 :
    default:
      { // 2x2 square tiles
        m_uCurrentlyUsedThreads = 4;
        GetRenderThreadsPtr()[0].SetRenderer(this);
        GetRenderThreadsPtr()[0].SetWindow(BoundingRect<uint2>(      0,         0, iImgXHalf, iImgYHalf));
        GetRenderThreadsPtr()[0].StartRendering();

        GetRenderThreadsPtr()[1].SetRenderer(this);
        GetRenderThreadsPtr()[1].SetWindow(BoundingRect<uint2>(iImgXHalf,         0,     iImgX, iImgYHalf));
        GetRenderThreadsPtr()[1].StartRendering();

        GetRenderThreadsPtr()[2].SetRenderer(this);
        GetRenderThreadsPtr()[2].SetWindow(BoundingRect<uint2>(      0, iImgYHalf, iImgXHalf, iImgY));
        GetRenderThreadsPtr()[2].StartRendering();

        GetRenderThreadsPtr()[3].SetRenderer(this);
        GetRenderThreadsPtr()[3].SetWindow(BoundingRect<uint2>(iImgXHalf, iImgYHalf,     iImgX, iImgY));
        GetRenderThreadsPtr()[3].StartRendering();
        m_frameRateTimer.Reset();
        break;
      }
    }
    // reset the progress of the threads
    for (int i(0); i<m_uCurrentlyUsedThreads; i++)
      GetRenderThreadsProgressPtr()[i] = 0.0F;
  }
} // LaunchThreads()


/**
 * Draws progress bar
 */
void vxRendererCpu3D::DrawProgressBar()
{
  // accumulate status of all currently running threads
  m_fRenderingProgress = 1.0F;
  for (int i(0); i<m_uCurrentlyUsedThreads; i++)
  {
    m_fRenderingProgress *= GetRenderThreadsProgressPtr()[i];
  }

  // green, yellow, orange, red-orange, red
  const uint4 iNumberOfBins(6); // should actuall be "m_viImageSize.size()" but compiler complains
  static int4 viR[iNumberOfBins] = {   0, 255, 255, 255, 255, 250};
  static int4 viG[iNumberOfBins] = { 255, 255, 130,  65,   0,  64};
  static int4 viB[iNumberOfBins] = {   0,   0,   0,   0,   0, 250};
  glDisable(GL_DEPTH_TEST);

  // set the OpenGL matrix to a gluOrtho2D
  //////////////////////////////////////////
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f, float4(GetEnvironment().GetWinRect().GetViewportSize().first), 0.0f, float4(GetEnvironment().GetWinRect().GetViewportSize().second));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  int4 iLevel(m_iImageSizeIndexMax - m_iImageSizeIndex);
  iLevel = Bound(0, iLevel, (int4)iNumberOfBins-1);

  int4 iLowerLeftX(5);  // kind of a hack since it has to be changed when the ViewerBar changes in size ...
  int4 iLowerLeftY(85); // kind of a hack since it has to be changed when the ViewerBar changes in size ...

  int4 iSizeX(3), iSizeY(25), iProgress(m_fRenderingProgress*iSizeY);
  vxRendererUtils::DrawRectangle2i(GL_QUADS, 150, 150, 150, iLowerLeftX-4, iLowerLeftY, iSizeX+8, iSizeY+8);
  vxRendererUtils::DrawRectangle2i(GL_QUADS, 250, 250, 250, iLowerLeftX-3, iLowerLeftY+1, iSizeX+6, iSizeY+6);
  vxRendererUtils::DrawRectangle2i(GL_QUADS, 150, 150, 150, iLowerLeftX-2, iLowerLeftY+2, iSizeX+4, iSizeY+4);
  vxRendererUtils::DrawRectangle2i(GL_QUADS,   0,   0,   0, iLowerLeftX-1, iLowerLeftY+3, iSizeX+2, iSizeY+2);
  vxRendererUtils::DrawRectangle2i(GL_QUADS, viR[iLevel], viG[iLevel], viB[iLevel], iLowerLeftX, iLowerLeftY+4, iSizeX, iProgress);
} // DrawProgressBar()



/**
 * Fills in all render arguments that are common amoung all threads.
 */
void vxRendererCpu3D::SetCommonRenderArguments()
{
  try
  {

    // Get local const of environment elements
    const Vector3Df fVoxelUnits(m_intensityRenderVol.GetHeader().GetVoxelUnits());
    const Vector3Df fVoxelUnitsReciprocal(1.0/fVoxelUnits.X(),1.0/fVoxelUnits.Y(),1.0/fVoxelUnits.Z());
    const Vector3Df fViewReferenceWC(m_camera.GetViewReference().X(),m_camera.GetViewReference().Y(),m_camera.GetViewReference().Z());
    const Vector3Df fViewDirectionWC(m_camera.GetViewDirection().X(),m_camera.GetViewDirection().Y(),m_camera.GetViewDirection().Z());
    const Vector3Df fViewUpWC(m_camera.GetViewUp().X(),m_camera.GetViewUp().Y(),m_camera.GetViewUp().Z());
    const Vector3Df fViewDownWC(-fViewUpWC);
    const Vector3Df fViewEyeWC(m_camera.GetEye().X(),m_camera.GetEye().Y(),m_camera.GetEye().Z());
    const Vector3Df fViewRightWC(fViewDirectionWC.GetNormalized().Cross(fViewUpWC));
    const Vector3Df fBoxOfInterestMin(Vector3D<float4>(m_fBoxOfInterest.GetMinX(), m_fBoxOfInterest.GetMinY(), m_fBoxOfInterest.GetMinZ()));
    const Vector3Df fBoxOfInterestMax(Vector3D<float4>(ceil(m_fBoxOfInterest.GetMaxX()), ceil(m_fBoxOfInterest.GetMaxY()), ceil(m_fBoxOfInterest.GetMaxZ())));

    m_commonRenderArguments.rendererCpu3dData = * GetEnvironment().GetDataRendererSoftware();

    m_commonRenderArguments.fVoxelUnits           = fVoxelUnits;
    m_commonRenderArguments.fVoxelUnitsReciprocal = fVoxelUnitsReciprocal;
    m_commonRenderArguments.fInterSampleDistance  = Global_fInterSampleDistance * m_fQualitySampleDistanceScaleResolutionDependent;

    // Set all the parameterd which are necessary for rendering
    m_commonRenderArguments.fCropBox         = m_fVisibilityBox; 
    m_commonRenderArguments.fBoxOfInterest   = m_fBoxOfInterest; 
    m_commonRenderArguments.fViewDirectionVC = Vector3D<float4>(fViewDirectionWC.X() * fVoxelUnitsReciprocal.X(), fViewDirectionWC.Y() * fVoxelUnitsReciprocal.Y(), fViewDirectionWC.Z() * fVoxelUnitsReciprocal.Z());
    Vector3D<float4> fViewUpVC               = Vector3D<float4>(fViewUpWC.X() * fVoxelUnitsReciprocal.X(), fViewUpWC.Y() * fVoxelUnitsReciprocal.Y(), fViewUpWC.Z() * fVoxelUnitsReciprocal.Z());

    /// initial light direction
    Vector3D<float4> fLight1DirVC            =  fViewUpVC.GetNormalized();
    m_commonRenderArguments.fLight2DirVC     = -m_commonRenderArguments.fViewDirectionVC.GetNormalized();

    /// moveable light adjustment
    float4 fAngleX(M_PI_2 * (0.5 - GetEnvironment().GetRenderingMode()->GetRelativeMousePosition().first));
    float4 fAngleY(M_PI_2 * (0.0 - GetEnvironment().GetRenderingMode()->GetRelativeMousePosition().second));
    const Normal<float4> fAxisX(fViewUpWC);
    const Normal<float4> fAxisY(fViewUpWC.Cross(fViewDirectionWC));
    Matrix<float4> fRotateMatrix;
    fRotateMatrix.Rotate(fAxisY, -fAngleY);
    fRotateMatrix.Rotate(fAxisX,  fAngleX);
    m_commonRenderArguments.fLight2DirVC = fRotateMatrix * m_commonRenderArguments.fLight2DirVC;

    // set vectors dependent on light direction
    m_commonRenderArguments.fHalfwayLight1VC = (-m_commonRenderArguments.fViewDirectionVC.GetNormalized() + fLight1DirVC).GetNormalized();
    m_commonRenderArguments.fHalfwayLight1VC = fRotateMatrix * m_commonRenderArguments.fHalfwayLight1VC;

    /// anisotropic correction gradient scaling factors
    m_commonRenderArguments.fGradientScaleY = m_intensityRenderVol.GetHeader().GetVoxelUnits().X() / m_intensityRenderVol.GetHeader().GetVoxelUnits().Y();
    m_commonRenderArguments.fGradientScaleZ = m_intensityRenderVol.GetHeader().GetVoxelUnits().X() / m_intensityRenderVol.GetHeader().GetVoxelUnits().Z();

    /// image info
    m_commonRenderArguments.pImageBuffer = (uint1 *)(GetImage().GetDataPtr());
    m_commonRenderArguments.pDepthBuffer = (float4*) GetEnvironment().GetDataRendererSoftware()->GetDepthImage().GetDataPtr();
    m_commonRenderArguments.iImageWidth  = GetImage().GetDim().X();
    m_commonRenderArguments.iImageHeight = GetImage().GetDim().Y()-2;

    /// slice shadow setup
    m_commonRenderArguments.bIsMipMode  = GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::MIP;
    m_commonRenderArguments.bIsXRayMode = GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::XRAY;
    m_commonRenderArguments.bIsVRMode   = m_commonRenderArguments.bIsMipMode == false && m_commonRenderArguments.bIsXRayMode == false;

    m_commonRenderArguments.bIsShowIntensityRange = GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE);
    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE))
    {
      m_commonRenderArguments.iShowIntensityRangeLower = vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow(),
        GetEnvironment().GetIntensityVolume()->GetHeader())   ;

      m_commonRenderArguments.iShowIntensityRangeUpper = vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh(),
        GetEnvironment().GetIntensityVolume()->GetHeader())   ;
    }

    m_commonRenderArguments.pvComponents = &m_vComponents; 

    m_commonRenderArguments.bIsLabelsOnTopMode = false; 

    // get the GL state
    glGetIntegerv(GL_VIEWPORT, m_commonRenderArguments.viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, m_commonRenderArguments.mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_commonRenderArguments.projmatrix);
    m_commonRenderArguments.fImageToViewportScaleX = m_commonRenderArguments.viewport[2] / float4(m_commonRenderArguments.iImageWidth-1);
    m_commonRenderArguments.fImageToViewportScaleY = m_commonRenderArguments.viewport[3] / float4(m_commonRenderArguments.iImageHeight-1);

    m_commonRenderArguments.uBackgroundColor = * GetEnvironment().GetBackgroundColor();
    m_commonRenderArguments.fBackgroundColor = PixelRGBAf(m_commonRenderArguments.uBackgroundColor.R(), 
                                                          m_commonRenderArguments.uBackgroundColor.G(), 
                                                          m_commonRenderArguments.uBackgroundColor.B(), 
                                                          m_commonRenderArguments.uBackgroundColor.A());

    // Choose a foreground color (black or white only) that is most dissimilar to the background color.
    // This is only used in x-ray rendering.
    if ((m_commonRenderArguments.fBackgroundColor.R() + 
         m_commonRenderArguments.fBackgroundColor.G() +
         m_commonRenderArguments.fBackgroundColor.B() ) / 3 > 128)
    {
      m_commonRenderArguments.fForegroundColor = PixelRGBAf(0, 0, 0, m_commonRenderArguments.fBackgroundColor.A());
    }
    else
    {
      m_commonRenderArguments.fForegroundColor = PixelRGBAf(255, 255, 255, m_commonRenderArguments.fBackgroundColor.A());
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("SetCommonRenderArguments failed.", "vxRendererCpu3D", "SetCommonRenderArguments"));
  }
} // SetCommonRenderArguments()


/**
 * Renders the actual image, but constraint to the 
 * area defined by the window parameter.
 * @param window defines the windows to render
 * @param uThreadID thread ID
 * @return 1 in case of success
 */
int4 vxRendererCpu3D::RenderWindow(const BoundingRect<uint2> & window, 
                                   const uint1 & uThreadID)
{
  try
  {
    //// make a list of arguments that is local to the current thread and partially updated for each ray
    ALIGN16 Args args;
    args = m_commonRenderArguments;
    args.iShiftTo8Bit = GetNumberOfShiftBitsFor8bitAlpha();
    args.pfColormap2d8bitPreIntegrated = m_pfColormap2d8bitPreIntegrated;

    args.uThreadID = uThreadID;
    if (args.iImageWidth == 0 && args.iImageHeight == 0)
    {
      return 0;
    }

    vxBlockVolumeRayIterator<uint2> rayIterBlk(m_intensityRenderVol);
    vxBlockVolumeRayIterator<uint2> labelRayIter(m_labelRenderVol);
    vxBlockVolumeRayIterator<uint2> shadowRayIter(m_intensityRenderVol);
    vxBlockVolumeIterator<uint2>    iterLabelShadow(m_labelRenderVol);
    vxBlockVolumeIterator<int1>     SpaceLeapInfoVolIter(m_spaceLeapInfoVol);
    args.pRayIterLabel      = &labelRayIter;
    args.pRayIterIntensity  = &rayIterBlk;
    args.pVolIterSLD        = &SpaceLeapInfoVolIter;   
    args.pRayIterShadow     = &shadowRayIter;
    args.pIterLabelShadow   = &iterLabelShadow;

    const uint4 byte_per_pixel = 4;
    const uint4 flc_xStepPixel = byte_per_pixel;
    const uint4 flc_yStepPixel = GetImage().GetDim().X() * byte_per_pixel;

    //Reorder reorderY( window.GetMaxY() - window.GetMinY() + 1 );
    std::vector< float4 > fDepths( window.GetMaxX() - window.GetMinX() + 1, 0 );

    //
    // Cast rays scanline by scanline.
    // Cast ray normally for every eighth pixel, but accelerate by casting from nearest
    // (nearest using depth information) of left and right fully cast rays.
    // Sequence along a ray is:
    //   1 - - - - - - - -
    //   1 - - - - - - 2 -
    //   1 3 4 5 6 7 8 2 -
    //
    float4 fDepth = 0;
    float4 fPreviousDepth = 0;
    const int4 iBlockSize = 8;
    bool bPreviousSomething = true;
    for ( int y=0; y<=window.GetMaxY() - window.GetMinY(); y++ )
    {
      for ( int x=0; x<=window.GetMaxX() - window.GetMinX(); x+=iBlockSize )
      {
        // send feeler ray
        args.u = x + window.GetMinX();
        args.v = y + window.GetMinY();
        args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
        args.bUseNeighborDepth = false;
        CastOneRay(args);

        const int4 * const pBackgroundColor = reinterpret_cast <int4 *>( & args.uBackgroundColor.R() );
        int4 * const pPixelColor      = reinterpret_cast <int4 *>( args.pImageBuffer + args.iPixelOffset );

        bool bSomething = ( * pBackgroundColor != * pPixelColor );
        args.pDepthBuffer[ args.iPixelOffset / 4 ] = fDepths[x] = fDepth = args.fDepthToSurface;

        // use the minimum of the left and right x%8==0 columns to decide initial depth
        // (don't do this the first time where x==0)
        for ( int xx = x - iBlockSize + 1; x > 0 && xx < x ; xx++ )
        {
          //Check the previous pixel color and only cast if the previous is not background.
          bool bSomethingBelow = true;
          if ( y > 0 )
          {
            args.u = xx + window.GetMinX();
            args.v = y  + window.GetMinY() - 1;
            args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
            int4 * const pBelowPixelColor = reinterpret_cast <int4 *>( args.pImageBuffer + args.iPixelOffset );
            bSomethingBelow = (* pBackgroundColor != * pBelowPixelColor );
          }

          if  ( bSomething || bPreviousSomething ||  bSomethingBelow  ) 
          {
            args.u = xx + window.GetMinX();
            args.v = y + window.GetMinY();
            args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;

            // miniumum of left and right and previous scanline
            args.fNeighborDepth = Min( Min( fPreviousDepth, fDepth ), fDepths[xx] );
            args.bUseNeighborDepth = true;
            CastOneRay( args );

            const float4 fSuprisingDepthChange = 10;
            bool bDepthSurprise = ( args.fDepthToSurface < fDepths[xx] && fDepths[xx] - args.fDepthToSurface > fSuprisingDepthChange );
            fDepths[xx] = args.fDepthToSurface;

            // if there was a depth suprise (depth discontinuity)
            // Go back through the scanlines in the reverse order to make see if there were any discontinuities
            // in the depth image. As we went up the image last time, we were able to anticipate increasing depth
            // but not decreasing depth (getting much closer).
            if ( bDepthSurprise )
            {
              // re-cast the ray from the very beginning
              args.bUseNeighborDepth = false;
              CastOneRay( args );
              fDepths[xx] = args.fDepthToSurface;

              // loop over pixels in -y direction casting each ray fully and
              // stop when we find that we have matched a previous depth
              bool bExpectedDepth = false;
              for ( int i=1; bExpectedDepth == false && y - i > 0; i++ )
              {
                args.v = y - i + window.GetMinY();
                args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
                args.bUseNeighborDepth = false;
                const float4 fOldDepth = args.pDepthBuffer[ args.iPixelOffset / 4 ];
                CastOneRay( args );
                args.pDepthBuffer[ args.iPixelOffset / 4 ] = args.fDepthToSurface;
                const float4 fReasonableDepthChange = 2;
                bExpectedDepth = ( args.fDepthToSurface > fOldDepth || fOldDepth - args.fDepthToSurface < fReasonableDepthChange );
              }
            }
          }//if  ( bSomething || bPreviousSomething ||  bSomethingBelow  ) 
        }//for ( int xx = x - iBlockSize + 1; x > 0 && xx < x ; xx++ )
        // prepare for next group
        fPreviousDepth = fDepth;
        bPreviousSomething = bSomething;
      }
      if (m_bCancelRendering == true)
      {
        return 0;
      }
      if ( window.GetMinY() != window.GetMaxY() )
      {
        const float4 fProgress = float4( y ) / float4( window.GetMaxY() - window.GetMinY() );
        if ( fProgress >= 0.0F && fProgress <= 1.0F )
        {
          GetRenderThreadsProgressPtr()[args.uThreadID] = fProgress;
        }
      }
    }

    return 0;
  } // try
  catch (ex::VException & error)
  {
    ClampIntensityVolume();
    LogRec(error.GetLogRecord().GetTextMessage(), "vxRendererCpu3D", "RenderWindow" );
    throw;
  }
  catch (... )
  {
    ClampIntensityVolume();
    throw ex::DataCorruptionException(LogRec("unhandled exception", "vxRendererCpu3D", "RenderWindow" ) );
  }
} // RenderWindow()


//////////////////////////////////////////////////////////////////////
// PROTECTED: rendering helper functions 


/**
 * Sets the size of the volume rendered image (the texture content).
 */
void vxRendererCpu3D::SetImageSize(int4 iImageSizeIndex)
{
  try
  {

    // adjust sampling rate according to image resolution
    if (iImageSizeIndex == m_iImageSizeIndexMax)
    {
      const float4 fValue(1.0F);
      if (m_fQualitySampleDistanceScaleResolutionDependent != fValue)
      {
        m_fQualitySampleDistanceScaleResolutionDependent = fValue;
        UpdateTFunc2D();
      }
    }
    else 
    {
      const float4 fValue(1.0F);
      if (m_fQualitySampleDistanceScaleResolutionDependent != fValue)
      {
        m_fQualitySampleDistanceScaleResolutionDependent = fValue;
        UpdateTFunc2D();
      }
    }

    m_iImageSizeIndex = Bound(0, iImageSizeIndex, int4(m_viImageSize.size()-1));
    int4 iNewImageSize(m_viImageSize[m_iImageSizeIndex]);

    if (iImageSizeIndex != m_iImageSizeIndex)
    {
      //LogDbg("Set image size index (SetImageSize()): from " + ToAscii(iImageSizeIndex) + " to " + ToAscii(m_iImageSizeIndex) + "(" + ToAscii(this) + ") and new size is: " + ToAscii(iNewImageSize), "", "");
    }

    if (iNewImageSize != GetImage().GetDim().X())
    {
      GetImage().Resize(Triple<uint4>(iNewImageSize, iNewImageSize+2, 0));
      GetEnvironment().GetDataRendererSoftware()->GetDepthImage().Resize(Triple<uint4>(iNewImageSize, iNewImageSize+2, 0));
      SetRenderingNecessary(true);
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("SetImageSize failed.", "vxRendererCpu3D", "UpdateMaskForIntensities"));
  }
} // SetImageSize()


/**
 * Generates the texture.
 */
void vxRendererCpu3D::CreateTexture()
{
  try
  {
    // call base class to generate the OpenGL texture
    vxRendererSoftware::CreateTexture();

    // compute the texture coordinates for the texture tile
    /////////////////////////////////////////////////////////
    const Vector3D< int4 > size (GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim());
    const Vector3D<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());

    Vector<float4> lowerLeftPoint(0,0,0);
    Vector<float4> upperRightPoint(GetImage().GetDim().X()-1, GetImage().GetDim().Y()-3, 0);
    GetTextureTile().SetOrigin(lowerLeftPoint);
    GetTextureTile().SetSize(upperRightPoint);

    Vector<float4> lowerLeftCoord(0.0, 0.0, 0.0);
    Vector<float4> upperRightCoord(1.0, 1.0, 1.0);
    GetTextureTile().SetTexCoordLowerLeft(lowerLeftCoord);
    GetTextureTile().SetTexCoordUpperRight(upperRightCoord);

    Vector<float4> direction(m_camera.GetViewDirection().X(), m_camera.GetViewDirection().Y(), m_camera.GetViewDirection().Z());
    GetTextureTile().SetNormal(Normal3Df(direction));

/*
    uint1 * pData = reinterpret_cast<uint1 *>(GetImage().GetDataPtr());
    bool bEmpty(true);
    int4 iSize(GetImage().GetDim().X() * GetImage().GetDim().Y());

    for (int i(0); i<iSize; i++)
    {
      if (pData[3] > 0)
      {
        bEmpty = false;
        break;
      }
      pData += 4;
    }

    if (bEmpty == true)
    {
      LogDbg("BLACK IMAGE CREATED!!!", "", "");
    }
*/
  }
  catch (...)
  {
    throw ex::VException(LogRec("CreateTexture failed.", "vxRendererCpu3D", "CreateTexture"));
  }
} // CreateTexture()


//////////////////////////////////////////////////////////////////////
// PROTECTED: rendering helper functions 

/**
 * Draws the image stored in the texture tile.
 */
void vxRendererCpu3D::DrawTexture()
{
  try
  {
    glBindTexture(GL_TEXTURE_2D, GetTextureTile().GetDensityTextureId());
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    bool bUseReal3dDrawing(false);

    if (bUseReal3dDrawing == true)
    {
      uint4 uIndex(m_camera.GetViewDirection().GetMaxAbsComponentCoord());

      // get the GL state
      GLint viewport[4];
      GLdouble mvmatrix[16], projmatrix[16];
      glGetIntegerv(GL_VIEWPORT, viewport);
      glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
      glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

      // setup GL draw textured tile
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthFunc(GL_LEQUAL);
      glBegin(GL_TRIANGLE_FAN);

      // traverse rectange in clockwise order 
      // 1-----2
      // |   / |
      // | /   |
      // 0-----3
      for (int i(0); i<4; i++)
      {
        // find screen coordinates of the corner point
        GLdouble x(viewport[0]); if (i == 2 || i == 3) x += viewport[2]; // add width  if on right
        GLdouble y(viewport[1]); if (i == 1 || i == 2) y += viewport[3]; // add height if on top

        // find corresponding world coordinate
        GLdouble xWorld, yWorld, zWorld;
        if (gluUnProject(x, y, 0.5, mvmatrix, projmatrix, viewport, &xWorld, &yWorld, &zWorld) == GL_FALSE) 
        {
          throw ex::VException(LogRec("gluUnProject failed", "vxRendererCpu3D", "DrawTexture"));
        }

        int4 offsetX(1); 
        int4 offsetY(2); 
        if (uIndex == 2)
        {
          swap(offsetX, offsetY);
        }
        glTexCoord2f(GetTextureTile().GetVertexTexCoord(i)[(uIndex+offsetX)%3], 
                     GetTextureTile().GetVertexTexCoord(i)[(uIndex+offsetY)%3]);
        glVertex3f(xWorld, yWorld, zWorld);
      }
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    else
    {
      // Get a square viewport size
      const uint2 uWidth (GetEnvironment().GetWinRect().GetViewportSize().first);
      const uint2 uHeight(GetEnvironment().GetWinRect().GetViewportSize().second);
      const uint2 uMinHalfSquareSide(min(uWidth/2.0, uHeight/2.0));
      const uint2 uCenterX(uWidth /2.0);
      const uint2 uCenterY(uHeight/2.0);

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      {
        // set the OpenGL matrix to a gluOrtho2D
        //////////////////////////////////////////
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0f, float4(GetEnvironment().GetWinRect().GetViewportSize().first), 
                   0.0f, float4(GetEnvironment().GetWinRect().GetViewportSize().second));
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBegin(GL_QUADS); 
        {
          vxWinRect winRect(GetEnvironment().GetWinRect());
          std::pair<int2, int2> reference(winRect.GetReferencePoint());
          std::pair<int2, int2> size(winRect.GetViewportSize());
          int2 X1(reference.first);
          int2 X2(reference.second);
          int2 Y1(reference.first + size.first);
          int2 Y2(reference.second + size.second);
          glTexCoord2f(0.0, 0.0); glVertex2f(reference.first, reference.second);
          glTexCoord2f(0.0, 1.0); glVertex2f(reference.first, reference.second + size.second);
          glTexCoord2f(1.0, 1.0); glVertex2f(reference.first + size.first, reference.second + size.second);
          glTexCoord2f(1.0, 0.0); glVertex2f(reference.first + size.first, reference.second);
        } 
        glEnd();
      }
      glDisable(GL_TEXTURE_2D);

    }

    if (false)
    {
      vxRendererUtils::GLDrawBox(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())), PixelRGBA<float4>(0,1,0,1));
    }

  }
  catch (...)
  {
    throw ex::VException(LogRec("DrawTexture failed.", "vxRendererCpu3D", "DrawTexture"));
  }
} // DrawTexture()


/**
 * Casts a single ray from the image plane. Test if ray will hit the volume, 
 * if yes it will sample a ray otherwise the corresponding pixel in the final 
 * image is set to black.
 * @param args     the ray data structure
 */
void vxRendererCpu3D::CastOneRay(Args & args)
{
  // find screen coordinates of the current point
  GLdouble x(args.u * args.fImageToViewportScaleX);
  GLdouble y(args.v * args.fImageToViewportScaleY);

  // find corresponding volume index coordinates
  GLdouble xWorld, yWorld, zWorld;
  if (gluUnProject(x, y, 0, args.mvmatrix, args.projmatrix, args.viewport, &xWorld, &yWorld, &zWorld) == GL_FALSE)
  {
    throw ex::VException(LogRec("gluUnProject failed", "vxRendererCpu3D", "DrawTexture"));
  }
  Vector4d<float4> fRayBgnVC(xWorld * args.fVoxelUnitsReciprocal.X(), 
                             yWorld * args.fVoxelUnitsReciprocal.Y(), 
                             zWorld * args.fVoxelUnitsReciprocal.Z(), 0);

  if (gluUnProject(x, y, 1, args.mvmatrix, args.projmatrix, args.viewport, &xWorld, &yWorld, &zWorld) == GL_FALSE) 
  {
    throw ex::VException(LogRec("gluUnProject failed", "vxRendererCpu3D", "DrawTexture"));
  }
  Vector4d<float4> fRayEndVC(xWorld * args.fVoxelUnitsReciprocal.X(), 
                             yWorld * args.fVoxelUnitsReciprocal.Y(), 
                             zWorld * args.fVoxelUnitsReciprocal.Z(), 1);

  fRayEndVC.T() = (fRayEndVC - fRayBgnVC).GetLengthXYZ();

  Vector4d<float4> fRayDirVC((fRayEndVC - fRayBgnVC).GetNormalizedXYZ());

  args.fRayIntersectBoundingBoxBgnVC = fRayBgnVC; 
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eRayStart);
  }

  bool bIntersection(vxUtils::HitBoundingBox4d(args.fCropBox, fRayBgnVC, fRayDirVC, args.fRayIntersectBoundingBoxBgnVC));

  if (bIntersection == true)
  {
    // compute ray exit point
    vxUtils::HitBoundingBox4d(args.fCropBox, fRayEndVC, -fRayDirVC, args.fRayIntersectBoundingBoxEndVC);

    // set the exit value of T to the matching OpenGL depth buffer value
    args.fRayIntersectBoundingBoxEndVC.T() = fRayEndVC.T() - args.fRayIntersectBoundingBoxEndVC.T();

    // move from outside bounding box into valid data area (precision issue)
    args.fRayIntersectBoundingBoxBgnVC += fRayDirVC * 0.01F;

    // set iterators to ray start location
    RaySegment raySegment(args.fRayIntersectBoundingBoxBgnVC, args.fRayIntersectBoundingBoxEndVC); 
    if (raySegment.GetEndPoint().T() <= raySegment.GetStartPoint().T())
    {
      //LogWrn("fixing inverted T on ray setup", "vxRendererCpu3D", "CastOneRay");
      raySegment.GetEndPoint().T() = raySegment.GetStartPoint().T()+0.1F;
    }
    Vector4Df raySampleIncVec(raySegment.GetDirection().GetNormalizedXYZ() * args.fInterSampleDistance);
    args.pRayIterIntensity->SetRaySegment(raySegment);
    args.pRayIterLabel    ->SetRaySegment(raySegment);
    args.pRayIterShadow   ->SetRaySegment(raySegment);
    args.pRayIterIntensity->SetSampleIncrementVector(raySampleIncVec);
    args.pRayIterLabel    ->SetSampleIncrementVector(raySampleIncVec);
    args.pRayIterShadow   ->SetSampleIncrementVector(raySampleIncVec);

    args.fLight2DirVC.X() = fRayDirVC.X();
    args.fLight2DirVC.Y() = fRayDirVC.Y();
    args.fLight2DirVC.Z() = fRayDirVC.Z();

    //args.fDepthToSurface = 1000 * (raySampleIncVec.T());
    args.bContinueRay = true;

    args.iSample    = 0;
    args.iPrevIndex = 0;

    if (m_bJittering == true)
    {
      float4 fJitterOffset(m_jitter.GetValue(args.u, args.v ) );
      args.pRayIterIntensity->SetPos(args.pRayIterIntensity->GetPosFloat() + raySampleIncVec * fJitterOffset );
      args.pRayIterLabel    ->SetPos(args.pRayIterLabel    ->GetPosFloat() + raySampleIncVec * fJitterOffset );
    }


    SampleSingleRay(args);

    // store result
    if (!args.bIsLabelsOnTopMode || args.fPreMultAccum.A() > 0.1)
    {
      uint1 * pImageDataPtrCurrent(args.pImageBuffer + args.iPixelOffset);
      *(pImageDataPtrCurrent++) = args.iDisplayColor.R();
      *(pImageDataPtrCurrent++) = args.iDisplayColor.G();
      *(pImageDataPtrCurrent++) = args.iDisplayColor.B();
      *(pImageDataPtrCurrent  ) = args.iDisplayColor.A();

      // code purely for debugging purposes
      ///////////////////////////////////////
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurface/500));
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurface/2000));
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurface/8000));

      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().X());  
      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().Y());
      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().Z());

      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.X());  
      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.Y());
      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.Z());

      *(args.pDepthBuffer + args.iPixelOffset/4) = args.fDepthToSurface;
    }
  }
  else
  {
    uint1 * pImageDataPtrCurrent(args.pImageBuffer + args.iPixelOffset);
    *(pImageDataPtrCurrent++) = args.uBackgroundColor.R();
    *(pImageDataPtrCurrent++) = args.uBackgroundColor.G();
    *(pImageDataPtrCurrent++) = args.uBackgroundColor.B();
    *(pImageDataPtrCurrent  ) = 0;
    *(args.pDepthBuffer + args.iPixelOffset/4) = (std::numeric_limits<float>::max)();
  }
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eNowDrawImage);
  }
} // CastOneRay()


/**
 * Space Leap
 * @param args the ray data structure
 */
void vxRendererCpu3D::SpaceLeap(Args & args)
{
  //// traverse empty space
  RaySpaceLeapLargeBlocks(args);
  RaySkipDensityInvisible(args);
  RaySkipAlphaInvisible(args);
} // SpaceLeap()


/**
 * Space leaps through all 16^3 blocks that have at least one layer of empty 16^3 
 * blocks around them. After that it jumps top the end of the border 16^3 blocks.
 * @param args ray data structure
 */
inline void vxRendererCpu3D::RaySpaceLeapLargeBlocks(Args & args)
{
  // space leap through empty blocks
  int4 iSpaceLeapInfo;
  args.pVolIterSLD->SetPos(args.pRayIterLabel->GetPosInt() / Global_iSldBlockWidth);
  while (0 < (iSpaceLeapInfo = args.pVolIterSLD->GetVoxel()) && args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() )
  {
    args.bFirstSample = false;
    float4 fSpaceLeapDist;
    if (1 < iSpaceLeapInfo)
    {
      // leap through complete iSldBlockWidth^3 blocks
      fSpaceLeapDist = (iSpaceLeapInfo-1) * Global_iSldBlockWidth;
      if (Global_bShowSpaceLeapPattern) 
        DrawSpaceLeapPatternLine(args, eMultipleEmptyBlocks);
    }
    else
    {
      // 1 == iSpaceLeapDist
      // leap to the end of the current block
      fSpaceLeapDist = args.pRayIterLabel->GetDistanceToEndOfBlock() + 0.1;
      if (Global_bShowSpaceLeapPattern) 
        DrawSpaceLeapPatternLine(args, eSingleEmptyBlock);
    }
    args.pRayIterLabel->SpaceLeap(fSpaceLeapDist); 

    args.pVolIterSLD->SetPos(args.pRayIterLabel->GetPosInt() / Global_iSldBlockWidth);
  }  
} // RaySpaceLeapLargeBlocks()


/**
 * Move in 2 voxel increments through the region that has invisible labels associated.
 * Go backwards in 0.5 voxel increments if a visible voxel is encountered.
 * @param args ray data structure
 */
inline void vxRendererCpu3D::RaySkipDensityInvisible(Args & args)
{
  if (args.bContinueRay == false || args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
  {
    args.bContinueRay = false;
    return;
  }
  int4 iLabel(args.pRayIterLabel->GetVoxel());
  if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && !(*args.pvComponents)[iLabel]->IsIntensityVisible())
  {
    args.bFirstSample = false;
    // current position is outside the visible region, -> move forward till visible or end of block
    for (int4 iCount(Global_iSldBlockWidth); iCount > 0; --iCount)
    {
      args.pRayIterLabel->SpaceLeapTwo();
      if (args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
      {
        // the slice shadow would be missing here if we do not
        // update the position of the intensity ray iterator
        args.pRayIterIntensity->SetPos(args.pRayIterLabel->GetPosFloat() );
        args.bContinueRay = false;
        return;
      }
      iLabel = args.pRayIterLabel->GetVoxel();
      
      if (Global_bShowSpaceLeapPattern)
      {
        DrawSpaceLeapPatternLine(args, eInvisible);
      }
      if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && (*args.pvComponents)[iLabel]->IsIntensityVisible())
      {
        // we went a little to far, lets go back to the last invisible position
        int4 iNumIterations(0);
        do
        {
          ++iNumIterations;
          args.pRayIterLabel->SpaceLeapMinusHalf();
          if (Global_bShowSpaceLeapPattern == true) 
          {
            DrawSpaceLeapPatternLine(args, eInvisibleBacktrace);
          }
          if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxBgnVC.T())
          {
            // solves the dotted "lines" around sculpted areas which happen for a 
            // new ray that enters, space leaps too far into a visible component, 
            // and then backs up several times.  Thus we need to go back into the
            // volume several steps.
            if (iNumIterations == 2)
            {
              // In this case it stepped 2*(-0.5) times until it is outside the volume again
              // so we step forward half that distance or 0.5 voxel units
              args.pRayIterLabel->SpaceLeap(0.5);
            }
            else if (iNumIterations == 4 )
            {
              // In this case it stepped 4*(-0.5) times until it is outside the volume again
              // so we step forward half that distance or 1.0 voxel units
              args.pRayIterLabel->SpaceLeap(1.0);
            }
            else
            {
              // does this ever happen
              args.bContinueRay = false;
            }
            return;
          }
        } while ((*args.pvComponents)[args.pRayIterLabel->GetVoxel()]->IsIntensityVisible());
        iCount = 0;
      } // end visibility test
    } // end off-label skipping
  } // end section advance through off-labels
} // RaySkipDensityInvisible()


/**
 * Move in 1 voxel increments through the region that is invisible according 
 * to the transfer function. Go backwards in 0.5 voxel increments if a visible 
 * sample is encountered
 * @param args ray data structure
 */
inline void vxRendererCpu3D::RaySkipAlphaInvisible(Args & args)
{
  // check right away to make sure we are not already outside the volume or previously interruped the ray
  if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
      args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
  {
    args.bContinueRay = false;
  }
  if (args.bContinueRay == false)
  {
    return;
  }

  // transfer the labelRayIter position to the intensity ray iter position
  args.pRayIterIntensity->SetPosFromRayIter(*args.pRayIterLabel);

  // move to next somewhat opaque voxel
  int2 iIntensitySample(args.pRayIterIntensity->GetSampleInt());

  // clamp for out-of-range values
  if (iIntensitySample < 0) iIntensitySample = 0;

  // use 2D pre-integration table: NEEDS TO BE REFINED TO TAKE WHATEVER IS APPROPRIATE!!!! 1D/2D
  const int4 iIndexSample((uint2(iIntensitySample)) >> args.iShiftTo8Bit);
  const int4 iIndex(5 * (256 * args.iPrevIndex + iIndexSample) + 3);

  if (args.pfColormap2d8bitPreIntegrated[iIndex] == 0)
  {
    args.bFirstSample = false;
    args.iPrevIndex = iIndexSample;

    if (Global_bShowSpaceLeapPattern)
    {
      DrawSpaceLeapPatternLine(args, eZeroOpacity, true);
    }

    for (int4 iCount(Global_iSldBlockWidth); iCount>0; --iCount)
    {
      args.pRayIterIntensity->SpaceLeap(1); //Two();
      if (Global_bShowSpaceLeapPattern == true)
      {
        DrawSpaceLeapPatternLine(args, eZeroOpacity, true);
      }
  
      // we jumped forward - make sure we did not jump outside
      if (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
      {
        // if we jumped forward, go back and start from there
        args.pRayIterIntensity->SpaceLeap(-1); //Int(-2);
        return;
      }
  
      if (0 != args.pfColormap2d8bitPreIntegrated[3 + 5 * (256 * args.iPrevIndex + ( args.pRayIterIntensity->GetSampleInt() >> args.iShiftTo8Bit))])
      {
        // sample visible
        do
        {
          args.pRayIterIntensity->SpaceLeapMinusHalf();
          if (Global_bShowSpaceLeapPattern) 
          {
            DrawSpaceLeapPatternLine(args, eZeroOpacityBacktrace, true);
          }
        } while (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxBgnVC.T() && 0 != args.pfColormap2d8bitPreIntegrated[3 + 5 * (256 * args.iPrevIndex + (args.pRayIterIntensity->GetSampleInt()>>args.iShiftTo8Bit))]);

        // WHY IS THIS ONE DONE???
        //args.pRayIterIntensity->SpaceLeap(0.5 - 0.5*args.fInterSampleDistance);
        //if (Global_bShowSpaceLeapPattern) 
        //{
        //  DrawSpaceLeapPatternLine(args, eZeroOpacityBacktrace, true);
        //}
        if (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
        {
          args.bContinueRay = false;
          return;
        }
        iCount = 0;
      }
      else if (iCount == 1)
      { 
        // sample not visible
        args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
        int4 iLabel(args.pRayIterLabel->GetVoxel());

        if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && (*args.pvComponents)[iLabel]->IsIntensityVisible())
        {
          iCount = 16;
        }
      }
    }
  }
} // RaySkipAlphaInvisible()


/**
 * Sample the intensity and label at the current ray position
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayGetSampleAndLabel(Args & args)
{
  // Get sample intensity
  args.iSample = args.pRayIterIntensity->GetSampleInt();

  // check for out-of-bounds
  if (args.bIsVRMode == true || args.bIsXRayMode == true)
  {
    if ( args.bFirstSample )
    {
      // similar to sample-based classification
      float4 * pfRGBA(m_pfColormap1d + 4 * args.iSample);
      const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
      args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
      args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
      args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
      // don't shift but divide because it throws away important bits!!!
      args.fPreMultSample.A() = ((*pfRGBA) * fFactor) / 256.0F;
      //////////////////////////////////////////////////////////////////////////////////////
      // The storage of values in the colormap needs to be completely re-implemented to
      // either use floating-point values (0-1) or better, integer values that can be
      // more efficiently computed on standard processors than floating-point computations.
      // We use "___/256.0F" above instead of float(int(___/255.0F)) for speed purposes.
      // The side effect is that you don't reach full opacity quite as quickly as you
      // should. But, the effect should be impossible to notice.
      //////////////////////////////////////////////////////////////////////////////////////
      args.fPreMultSample.ScaleRGB(args.fPreMultSample.A());

      // accentuate the opacity of the first sample
      args.fPreMultSample.A() = pow( args.fPreMultSample.A(), 0.1F );

      if ( m_bShowCutsGrayscale )
      {
        args.fPreMultSample.R() = args.fPreMultSample.G() = args.fPreMultSample.B() =
          ( args.fPreMultSample.R() + args.fPreMultSample.G() + args.fPreMultSample.B() ) * 0.3333F;
      }
    }
    else // not first sample
    {

      // VR mode
      if (m_bPreIntegration == true)
      {
        {
          const int4 iIndex((uint2(args.iSample)) >> args.iShiftTo8Bit);
          float4 * pTFunc2D(&args.pfColormap2d8bitPreIntegrated[5*(256*args.iPrevIndex + iIndex)]);
          args.iPrevIndex = iIndex;
          args.fPreMultSample.R() = *pTFunc2D++;
          args.fPreMultSample.G() = *pTFunc2D++;
          args.fPreMultSample.B() = *pTFunc2D++;
          if (args.bIsVRMode == true)
            args.fPreMultSample.A() = *pTFunc2D++;
          else
            args.fPreMultSample.A() = *pTFunc2D++ * 0.1F;
          args.fGradientWeight = *pTFunc2D;
        }
  /*
        {
          const int4 iIndex((uint2(args.iSample)) >> args.iShiftTo8Bit);
          float4 * pTFunc2D(&m_pfColormap2d8bitPreIntegratedOutOf1d[5*(256*args.iPrevIndex + iIndex)]);
          args.iPrevIndex = iIndex;
          args.fPreMultSample.R() = *pTFunc2D++;
          args.fPreMultSample.G() = *pTFunc2D++;
          args.fPreMultSample.B() = *pTFunc2D++;
          if (args.bIsVRMode == true)
            args.fPreMultSample.A() = *pTFunc2D++;
          else
            args.fPreMultSample.A() = *pTFunc2D++ * 0.1F;
          args.fGradientWeight = *pTFunc2D;
        }
  */
      }
      else // sample based classification!!!
      {
        float4 * pfRGBA(m_pfColormap1dAlphaCorrected + 4 * args.iSample);
        const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
        args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
        args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
        args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
        // don't shift but divide because it throws away important bits!!!
        args.fPreMultSample.A() = ((*pfRGBA) * fFactor) / 256.0F;
        //////////////////////////////////////////////////////////////////////////////////////
        // The storage of values in the colormap needs to be completely re-implemented to
        // either use floating-point values (0-1) or better, integer values that can be
        // more efficiently computed on standard processors than floating-point computations.
        // We use "___/256.0F" above instead of float(int(___/255.0F)) for speed purposes.
        // The side effect is that you don't reach full opacity quite as quickly as you
        // should. But, the effect should be impossible to notice.
        //////////////////////////////////////////////////////////////////////////////////////
        args.fPreMultSample.ScaleRGB(args.fPreMultSample.A());
      }

    }

  }
  else //if (args.bIsMipMode == true)
  {
    // MIP mode
    float4 * pfRGBA(m_pfColormap1d + 4 * args.iSample);
    const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
    args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
    args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
    args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
    // don't shift but divide because it throws away important bits!!!
    float4 fTempAlpha = ((*pfRGBA) * fFactor) / 255.0F;
    args.fPreMultSample.A() = ( fTempAlpha <= 1.0F ) ? fTempAlpha : 1.0F;
    //////////////////////////////////////////////////////////////////////////////////////
    // The storage of values in the colormap needs to be completely re-implemented to
    // either use floating-point values (0-1) or better, integer values that can be
    // more efficiently computed on standard processors than floating-point computations.
    // We are using a floating-point comparison here instead of a truncation for a
    // hopeful speed increase.
    //////////////////////////////////////////////////////////////////////////////////////
    args.fPreMultSample.ScaleRGB( args.fPreMultSample.A() );
  }

  // get corresponding label
  args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
  args.iLabel = args.pRayIterLabel->GetSampleNearest();
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eColoredSample, false);
  }

} // RayGetSampleAndLabel()


/**
 * blend the label color over the sample
 *
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayBlendLabelColor(Args & args)
{
  // blend label color onto sampleColor
  if (args.iLabel > 0 && (*args.pvComponents)[args.iLabel]->IsLabelVisible())
  {
    PixelRGBA<float4> color = static_cast<PixelRGBAf>((*args.pvComponents)[args.iLabel]->GetColor());

    const float4 fBlendFraction(color.A() / 255.0F );

    args.fPreMultSample.BlendRGB(color, fBlendFraction);
  }
} // RayBlendLabelColor()


/**
 * blend the intensity range over the sample if blending is enabled
 *
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayBlendIntensityRange(Args & args)
{
  if (args.bIsShowIntensityRange)
  {
    if ((args.iShowIntensityRangeLower <= args.iSample) && (args.iSample <= args.iShowIntensityRangeUpper))
    {
      PixelRGBAf color(0, 255, 0, 0);
      const float4 fBlendFraction(0.5F );
      color.ScaleRGB(args.fPreMultSample.A());
      args.fPreMultSample.BlendRGB(color, fBlendFraction);
    }
  }
}


/**
 * Blend the slice shadows over the sample if shadow blending is enabled
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayBlendSliceShadows( Args & args )
{

  if ( m_bIsSliceShadowEnabled &&
       args.pRayIterIntensity->GetPosFloat().T() > args.fShadowPlaneDepth )
  {
    PixelRGBAf color(0, 0, 0, Global_fSliceShadowOpacity );

    // 0-2 slice shadow orientation means orthogonal, -1 means oblique
    if ( m_iSliceShadowOrientation > -1 )
    {
      // orthogonal slice shadow
      color[ ( m_iSliceShadowOrientation + 1 ) % 3 ] = 128;
    }
    else
    {
      // oblique slice shadow
      color[ 1 ] = 128;
      color[ 2 ] = 128;
    }

    if ( m_bClipSliceShadow == false ) args.fPreMultSample.Composite( color );

    // lets not add more slice shadow color after this
    args.fShadowPlaneDepth = (std::numeric_limits<float>::max)();
  }

} // RayBlendSliceShadows()


/**
 * Blend the slice shadows at the end of sampling the ray
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayBlendSliceShadowsAtEndOfRay( Args & args )
{

  if ( m_bIsSliceShadowEnabled &&
       ( args.pRayIterIntensity->GetPosFloat().T() > args.fShadowPlaneDepth ||
         args.pRayIterLabel    ->GetPosFloat().T() > args.fShadowPlaneDepth ) )
  {

    args.fPreMultSample.R() = args.fPreMultSample.G() = args.fPreMultSample.B() = 0; 
    args.fPreMultSample.A() = Global_fSliceShadowOpacity;

    if ( m_bClipSliceShadow == false )
    {

      // 0-2 slice shadow orientation means orthogonal, -1 means oblique
      if ( m_iSliceShadowOrientation > -1 )
      {
        // orthogonal slice shadow
        args.fPreMultSample[ ( m_iSliceShadowOrientation + 1 ) % 3 ] = 128;
      }
      else
      {
        // oblique slice shadow
        args.fPreMultSample[ 1 ] = 128;
        args.fPreMultSample[ 2 ] = 128;
      }

      args.fPreMultAccum.Composite( args.fPreMultSample );

    } // if not clipping

  } // if should blend

} // RayBlendSliceShadowsAtEndOfRay()


/**
 * Color the ray based on the gradient direction (debug use only)
 * @param args          the ray data structure
 * @param vGradVector3D the gradient vector
 */
inline void vxRendererCpu3D::RayColorFromGradientDir(Args & args, 
                                                     const Vector3D<float4> vGradVector3D)
{
  //// replace colors with gradient direction for debugging
  if (args.u%64 < 32)
  {
    args.fPreMultSample.R() = args.fPreMultSample.A()*255*fabs(vGradVector3D.Z());
    args.fPreMultSample.G() = args.fPreMultSample.A()*255*fabs(vGradVector3D.X());
    args.fPreMultSample.B() = args.fPreMultSample.A()*255*fabs(vGradVector3D.Y());
  }
}


/**
 * ???
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayNextMipSample(Args & args)
{
  // only do something (replace) if the current sample has higher opacity than any of the previous.
  if (args.fPreMultAccum.A() < args.fPreMultSample.A())
  {
    args.fPreMultAccum = args.fPreMultSample;
  }
}


/**
 * Adjust (lower) the sample color intensity doe to shading.
 * Considers one diffuse and one specular light source.
 * Both operate on the data gradient as well as the inverted data gradient to
 * allow surface appearance for low to high density transitions as well as high to low
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayShadeSample(Args & args)
{
  if ( m_bShading && ! args.bFirstSample )
  {
    // get gradient from precomputed optimal position between previous and curent sample
    if (m_bPreIntegration == true)
      args.pRayIterIntensity->SpaceLeap(-args.fGradientWeight * args.fInterSampleDistance);

    Vector3D<float4> vGradVector3D((args.fPreMultSample.A() > 0.005) ? -args.pRayIterIntensity->GetSampleGradientSobel() : -args.pRayIterIntensity->GetSampleGradientCentralDifference());
    if (Global_bShowSpaceLeapPattern) DrawSpaceLeapPatternLine(args, eGradientLocation, true);

    if (m_bPreIntegration == true)
      args.pRayIterIntensity->SpaceLeap(args.fGradientWeight * args.fInterSampleDistance);

    // anisotropic gradient direction correction
    vGradVector3D.Y() *= args.fGradientScaleY;
    vGradVector3D.Z() *= args.fGradientScaleZ;

    // illumination computation
    if (vGradVector3D.GetLengthSquared() > 10)
    {
      vGradVector3D.Normalize();
      float4 fIlluminationIntensity(m_illuminator.ShadePhongDiffuse (vGradVector3D, args.fLight2DirVC));
      float4 fSpecular(args.fPreMultSample.A() * 255 * m_illuminator.ShadePhongSpecular(vGradVector3D, args.fHalfwayLight1VC));

      // replace above with below for x-ray shading effect
      //fIlluminationIntensity = 1.0F - fIlluminationIntensity * fIlluminationIntensity; // invert the direct shading
      //float4 fSpecular( 0.5F * args.fPreMultSample.A() * 255 * m_illuminator.ShadePhongSpecular( vGradVector3D, args.fHalfwayLight1VC ) );

      if (m_bShadowing == true)
      {

        //
        // sample from the current sample position toward the light source and get an opacity in the range (0,1)
        //
        Vector3Df samplePosition( args.pRayIterIntensity->GetPosFloat().m_x, args.pRayIterIntensity->GetPosFloat().m_y, args.pRayIterIntensity->GetPosFloat().m_z );
        //const float4 fJitterAmount = 0.1F;
        //Normal3Df jitteredLightDirection( args.fHalfwayLight1VC +
        //  Vector3Df( Drand48( -fJitterAmount, fJitterAmount ),
        //             Drand48( -fJitterAmount, fJitterAmount ),
        //             Drand48( -fJitterAmount, fJitterAmount ) ) );
        Normal3Df jitteredLightDirection( args.fHalfwayLight1VC );
        Point3Df startPosition( samplePosition + jitteredLightDirection * 1.0F ); // start a little beyond current sample to avoid self-shadowing
        Point3Df endPosition( samplePosition + jitteredLightDirection * 1000.0F ); // well outside the volume - revisit

        // compute ray exit point
        Vector4Df endPos4d;
        vxUtils::HitBoundingBox4d(args.fCropBox, Vector4Df(endPosition, 0 ), -Vector4Df(jitteredLightDirection.X(), jitteredLightDirection.Y(), jitteredLightDirection.Z(), 1 ), endPos4d);
        RaySegment raySegment(startPosition, Point3Df(endPos4d.m_x, endPos4d.m_y, endPos4d.m_z ) ); 
        if (raySegment.GetEndPoint().T() <= raySegment.GetStartPoint().T())
        {
          raySegment.GetEndPoint().T() = raySegment.GetStartPoint().T()+0.1F;
        }

        // setup the shadow ray
        Vector4Df raySampleIncVec(raySegment.GetDirection().GetNormalizedXYZ() * args.fInterSampleDistance);
        args.pRayIterShadow->SetRaySegment(raySegment );
        args.pRayIterShadow->SetSampleIncrementVector(raySampleIncVec );

        // sample the entire shadow ray
        float4 fOpacity = 0.0F;
        while (fOpacity < 0.95F && args.pRayIterShadow->GetPosFloat().T() < raySegment.GetEndPoint().T() )
        {

          // sample and classify
          int4 iSample = args.pRayIterShadow->GetSampleInt();
          float4 * pfRGBA(m_pfColormap1dAlphaCorrected + 4 * iSample + 3); // get to Alpha
          const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
          // don't shift but divide because it throws away important bits!!!
          float4 fAlpha = ((*pfRGBA) * fFactor) / 256.0F;

          // check that it's not invisible
          if (fAlpha > 0 )
          {
            Vector3Di nearestPos(args.pRayIterShadow->GetPosFloat().m_x + 0.5F,
                                  args.pRayIterShadow->GetPosFloat().m_y + 0.5F,
                                  args.pRayIterShadow->GetPosFloat().m_z + 0.5F );
            args.pIterLabelShadow->SetPos(nearestPos );
            int4 iLabel(args.pIterLabelShadow->GetVoxel() );
            if ((* args.pvComponents ) [ iLabel ]->IsIntensityVisible() == false ) fAlpha = 0;
          }
          // accumlate ray opacity
          fOpacity += (1.0F - fOpacity ) * fAlpha;
          args.pRayIterShadow->SpaceLeapOne();
        }

        // clamp
        if (fOpacity < 0 ) fOpacity = 0;
        if (fOpacity > 1 ) fOpacity = 1;

        // increase the ambient light so the shadows are not too dark
        const float4 fAmbientIntensity(0.65F );
        fOpacity *= (1.0F - fAmbientIntensity );

        // darken the sample
        fIlluminationIntensity *= (1.0F - fOpacity );
        fSpecular *= (1.0F - fOpacity );
      }

      // sample color intensity adjustment
      args.fPreMultSample.ScaleRGB(fIlluminationIntensity);
      args.fPreMultSample.AddRGB(fSpecular);
    }
  }

  if (m_bIsBoxOfInterestActive == true)
  {
    Vector4d<float4> fPos(args.pRayIterIntensity->GetPosFloat());

    if(args.fBoxOfInterest.IsInside(Triple<float4>(fPos.X(), fPos.Y(), fPos.Z())))
    {
      PixelRGBA<float4> blend(0, 0, 255, 255);
      args.fPreMultSample.BlendRGB(blend, .40F);
      //args.fPreMultSample.ScaleRGB(0.75F);
    }
  }
} // RayShadeSample()


/**
 * Composite the current sample over the accumulated ray
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RayCompositeSample(Args & args)
{
  args.fPreMultAccum.Composite(args.fPreMultSample);
} // RayCompositeSample()


/**
 * Take the accumulated ray colors and convert them into displayable uint1 pixels
 * @param args     the ray data structure
 */
inline void vxRendererCpu3D::RaySetFinalColors(Args & args)
{
  // if x-ray convert to RGBA
  if (args.bIsXRayMode)
  {
    args.fPreMultAccum.A() = 1 - exp(- 3.0F * args.fPreMultAccum.A() );
    const float4 fAlpha( args.fPreMultAccum.A() );
    args.fPreMultAccum.R() = fAlpha * args.fForegroundColor.R() + (1 - fAlpha ) * args.fBackgroundColor.R();
    args.fPreMultAccum.G() = fAlpha * args.fForegroundColor.G() + (1 - fAlpha ) * args.fBackgroundColor.G();
    args.fPreMultAccum.B() = fAlpha * args.fForegroundColor.B() + (1 - fAlpha ) * args.fBackgroundColor.B();
    args.fPreMultAccum.A() = fAlpha * args.fForegroundColor.R();
  }
  else
  {
    // blend ray over background color
    args.fPreMultAccum.Composite( args.fBackgroundColor );
  }

  // convert alpha from 0-1 to 0-255
  args.fPreMultAccum.A() *= 255;

  // ensure [0,255] range for color
  if (args.fPreMultAccum.R() > 255) args.fPreMultAccum.R() = 255;
  if (args.fPreMultAccum.G() > 255) args.fPreMultAccum.G() = 255;
  if (args.fPreMultAccum.B() > 255) args.fPreMultAccum.B() = 255;
  if (args.fPreMultAccum.A() > 255) args.fPreMultAccum.A() = 255;

  //// transfer final float4 colors to int1 display colors
  args.iDisplayColor.R() = RoundFloatToInt_FISTP(args.fPreMultAccum.R());
  args.iDisplayColor.G() = RoundFloatToInt_FISTP(args.fPreMultAccum.G());
  args.iDisplayColor.B() = RoundFloatToInt_FISTP(args.fPreMultAccum.B());
  args.iDisplayColor.A() = RoundFloatToInt_FISTP(args.fPreMultAccum.A());
} // RaySetFinalColors()


/**
 * Composite sampling points along a single ray.
 * @param args the ray data structure
 */
void vxRendererCpu3D::SampleSingleRay(Args & args)
{
  // initialize the colors that are about to be accumulated along the ray
  args.fPreMultAccum.R() = args.fPreMultAccum.G() = args.fPreMultAccum.B() = args.fPreMultAccum.A() = 0;
  args.fDepthToSurface = (std::numeric_limits<float>::max)();
  args.iPrevIndex = 0;
  args.bFirstSample = ! m_bShowCutsShaded                   && ! m_commonRenderArguments.bIsMipMode &&
                      ! m_commonRenderArguments.bIsXRayMode && m_bPreIntegration;

  // potentially jump to neighbor depth in order to speed up the space leaping portion of ray casting
  if ( args.bUseNeighborDepth )
  {
    const float4 fStepBack = 2.0F;
    const float4 fDistanceToLeap( args.fNeighborDepth - fStepBack - args.pRayIterIntensity->GetPosFloat().T() );
    if ( fDistanceToLeap > 1 )
    {
      const float4 fDistancePerLeap( args.pRayIterIntensity->GetSldDeltaVector().T() );
      const float4 fLeaps( fDistanceToLeap / fDistancePerLeap );
      const Vector4d<float4> originalPosition = args.pRayIterIntensity->GetPosFloat();
      args.pRayIterIntensity->SpaceLeap( fLeaps );
      args.pRayIterLabel->SetPosFromRayIter( * args.pRayIterIntensity );

      // forget acceleration if we leaped directly into the data or out of the volume
      RayGetSampleAndLabel(args);
      if ( args.fPreMultSample.A() > 0 )
      {
        // send a message back that the neighbor depth was bad since we encountered a suprise patch of opaque data
        args.bUseNeighborDepth = false;
        args.pRayIterIntensity->SetPos( originalPosition );
        args.pRayIterLabel->SetPosFromRayIter( * args.pRayIterIntensity );
      }
    }
  }

  //// Composite the sampling points along a ray.
  while (args.bContinueRay == true)
  {
    SpaceLeap(args);

    //// sample at least 4 times, expecting to get a density > 0
    for (int4 iCount(4); args.bContinueRay == true && iCount>0; --iCount)
    {
      if (args.pRayIterIntensity->GetPosFloat().T() <= args.fRayIntersectBoundingBoxEndVC.T()) 
      {
        RayGetSampleAndLabel(args);

        // composite only if alpha is > 0
          if (args.fPreMultSample.A() > 0 && (*args.pvComponents)[args.iLabel]->IsIntensityVisible()
               && ((!args.bIsLabelsOnTopMode) || (args.bIsLabelsOnTopMode && args.iLabel!=0 )) )
        {
          // set "z"-buffer depth
          if (args.fPreMultAccum.A() == 0)
          {
            args.fDepthToSurface = args.pRayIterIntensity->GetPosFloat().T();
          }

          RayBlendLabelColor(args);
          RayBlendIntensityRange(args);
          RayBlendSliceShadows(args);

          if (args.bIsVRMode )
          {
            RayShadeSample(args);
            RayCompositeSample(args);
          }
          else if (args.bIsMipMode )
          {
            RayNextMipSample(args);
          }
          else if (args.bIsXRayMode )
          {
            RayCompositeSample(args);
          }

          if (args.fPreMultAccum.A() > 0.95 && ! args.bIsMipMode )
          {
            args.bContinueRay = false;
          }
          if (Global_bShowSpaceLeapPattern )
          {
            DrawSpaceLeapPatternLine(args, eColoredSample);
          }

          args.bFirstSample = false;
          ++iCount;  // now we are in the important section of the data, lets keep sampling
        } // composite only if alpha is > 0
        else
        { // alpha == 0
          if (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T()) 
          {
            args.bContinueRay = false;
          }
          if (Global_bShowSpaceLeapPattern && args.fPreMultSample.A() == 0) 
          {
            DrawSpaceLeapPatternLine(args, eZeroOpacity);
          }
        } // start empty space traversal again, if iCount has reached value 4
      }
      else
      {
        // we actually went outside and the iterator is outside volume 
        args.bContinueRay = false;
        break;
      }

      ++(*args.pRayIterIntensity);
    } // end sampling
  } // while not early ray termination

  RayBlendSliceShadowsAtEndOfRay(args);
  RaySetFinalColors(args);
  
  if (Global_bShowSpaceLeapPattern) DrawSpaceLeapPatternLine(args, eRayEnd);

} // SampleSingleRay()


/**
 * Decide if the pixels are close enough in color to interpolate rather than raycast
 * @param pPixelA             pixel A
 * @param pPixelB1            pixel B1
 * @param pPixelB2            pixel B2
 * @param pPixelB3            pixel B3
 * @param bPixelsAreDifferent if the corner pixels are similar
 * 
 * @return true if ok to interpolate
 */
inline bool vxRendererCpu3D::IsOkToInterpolate(const uint1 * const pPixelA,
                                               const uint1 * const pPixelB1,
                                               const uint1 * const pPixelB2,
                                               const uint1 * const pPixelB3,
                                               bool & bPixelsAreDifferent )
{
  const int4 iMaxDiff(Global_iMaxDifferenceColor );

  // in case any of the three corner pixels is different, it is not ok to interpolate
  if ((abs(* (pPixelA   ) - * (pPixelB1   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB1+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB1+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB1+3 ) ) >= iMaxDiff ) || // A

       (abs(* (pPixelA   ) - * (pPixelB2   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB2+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB2+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB2+3 ) ) >= iMaxDiff ) || // A

       (abs(* (pPixelA   ) - * (pPixelB3   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB3+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB3+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB3+3 ) ) >= iMaxDiff ) )  // A
  {
    bPixelsAreDifferent = true;
    return false;
  }

  // if we get here we know that the corner pixels are similar
  bPixelsAreDifferent = false;

  // we may still want NOT to interpolate if there are heterogeneous pixels nearby
  #ifdef _WIN64
    int8 iPixelPosDiff = _abs64((int8)pPixelB1 - (int8)pPixelA);
  #else
    int4 iPixelPosDiff = abs(pPixelB1 - pPixelA);
  #endif
  if ( (* (pPixelB1 + 3 ) & iPixelPosDiff ) ||
       (* (pPixelB2 + 3 ) & iPixelPosDiff ) ||
       (* (pPixelB3 + 3 ) & iPixelPosDiff ) )
  {
    return false;
  }

  // ok all bad cases did not happen, lets interpolate
  return true;
} // IsOkToInterpolate()


/**
 * Test if Pixels are similar enough to just interpolate intead of sending a ray.
 * 
 * @param flc_pPixelA first pixel
 * @param flc_pPixelB second pixel
 * @param depthA      depth value of first pixel
 * @param depthB      depth pixel of second pixel
 * 
 * @return true if pixels are similar
 */
bool vxRendererCpu3D::IsPixelsAreSimilar(const uint1 * const pPixelA, 
                                         const uint1 * const pPixelB1, 
                                         const uint1 * const pPixelB2, 
                                         const uint1 * const pPixelB3)
{
  const int4 iMaxDiff(Global_iMaxDifferenceColor );

  return ((abs(* (pPixelA   ) - * (pPixelB1   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB1+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB1+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB1+3 ) ) < iMaxDiff ) && // A

           (abs(* (pPixelA   ) - * (pPixelB2   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB2+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB2+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB2+3 ) ) < iMaxDiff ) && // A

           (abs(* (pPixelA   ) - * (pPixelB3   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB3+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB3+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB3+3 ) ) < iMaxDiff ) ); // A

} // IsPixelsAreSimilar()


/**
 * Test if Pixels are similar enough to just interpolate intead of sending a ray
 * @param pPixelA pixel in question
 * @return true if pixel is transparent
 */
bool vxRendererCpu3D::IsPixelTransparent(const unsigned char * pPixelA)
{
  return (*(pPixelA+3)==0); // previously it tested RGB to each be zero
} // IsPixelTransparent()


/**
 * Interpolates between two pixels. Assumes interpolation weight 0.5
 * @param pPixelA first input pixel
 * @param pPixelB second input pixel
 * @param pPixelC result pixel
 */
void vxRendererCpu3D::PixelInterpolate(const uint1 * const pPixelA, 
                                       const uint1 * const pPixelB, 
                                             uint1 *       pPixelC)
{
  *(pPixelC )  = ((int4)*(pPixelA  ) + (int4)*(pPixelB  )) / 2; // R
  *(pPixelC+1) = ((int4)*(pPixelA+1) + (int4)*(pPixelB+1)) / 2; // G
  *(pPixelC+2) = ((int4)*(pPixelA+2) + (int4)*(pPixelB+2)) / 2; // B
  *(pPixelC+3) = ((int4)*(pPixelA+3) + (int4)*(pPixelB+3)) / 2; // A
} // PixelInterpolate()


/**
 * Helper function to pre-integrate the RGBA between two pixels
 * @param iIndexEnd          end index
 * @param fAlphaCorrExponent unknown
 * @param fMax               maxium alpha value
 * @param fColorScaleTo8bit  scale factor to get color to [0...255]
 * @param pfColormap         incoming colormap (RGBA)
 * @param pfResult           outgoing results (RGBAW)
 */
inline void FileScope_PreIntegrate1D(const int4 iIndexEnd, 
                                     const float4 fAlphaCorrExponent, 
                                     const float4 fMax,
                                     const float4 fColorScaleTo8bit,
                                     const float4 * pfColormap, 
                                     float4 * pfResult)
{
  float4 fARacc(0);
  float4 fAGacc(0);
  float4 fABacc(0);
  float4 fTacc(1);
  float4 fA(0);
  float4 fAccWeight1(0);
  float4 fAccWeight2(0);

  for (int4 k(0); ((fTacc >= 0.05) && (k<iIndexEnd)); k++)
  {
    float4 * pfColormapEntry(const_cast<float *>(pfColormap + k*4) );
    fA = *(pfColormapEntry + 3)/fMax;

    if (fA > 0.0)
    {
      const float4 fAlphaCorr(1 - pow(1-fA, fAlphaCorrExponent));
      const float4 fTA(fTacc * fAlphaCorr);
      fARacc += fTA * fColorScaleTo8bit * *(pfColormapEntry++);
      fAGacc += fTA * fColorScaleTo8bit * *(pfColormapEntry++);
      fABacc += fTA * fColorScaleTo8bit * *(pfColormapEntry);
      fTacc *= (1 - fAlphaCorr);

      // pre-integrate the gradient location weight for each interval
      float4 fWeight(float4(k)/float4(iIndexEnd));
      fAccWeight1 += (1.0f - fWeight) * fAlphaCorr; 
      fAccWeight2 +=         fWeight  * fAlphaCorr; 
    }
  }
  *(pfResult++) = fARacc; 
  *(pfResult++) = fAGacc; 
  *(pfResult++) = fABacc;     
  *(pfResult++) = 1 - fTacc;

  float4 fAccWeightSum(fAccWeight1 + fAccWeight2);

  if (fAccWeightSum > 0)
  {
    fAccWeight1 /= fAccWeightSum;
    fAccWeight2 /= fAccWeightSum;
  }
  *(pfResult) = 0.5F; //2 * fAccWeight1;
} // FileScope_PreIntegrate1D()


/**
 * Helper function to pre-integrate the RGBA between two pixels
 *
 * @param iIndexStart        the start index
 * @param iIndexEnd          the end index
 * @param iIndexInc          unknown
 * @param fAlphaCorrExponent unknown
 * @param p8BitTFunc         the 8-bit 1D transfer function
 * @param pfTFunc2D          the 8-bit 2D transfer function
 */
inline void FileScope_PreIntegrate2D8bit(const int4 iIndexStart, 
                                         const int4 iIndexEnd, 
                                         const int4 iIndexInc, 
                                         const float4 fAlphaCorrExponent, 
                                         const float4 * pColormap, 
                                         float4 * pfTarget)
{
  float4 fARacc(0);
  float4 fAGacc(0);
  float4 fABacc(0);
  float4 fTacc(1);
  float4 fA(0);
  float4 fAccWeight1(0);
  float4 fAccWeight2(0);

  for (int4 k(iIndexStart); ((fTacc >= 0.05) && ((iIndexInc > 0) ? (k<iIndexEnd) : (k>iIndexEnd))); k += iIndexInc)
  {
    // pre-integrate color and alpha for each interval
    float4 * pfTFunc1D(const_cast<float *>(pColormap + k*4) );
    fA = *(pfTFunc1D + 3);

    if (fA > 0.0)
    {
      const float4 fAlphaCorr(1 - pow(1-fA, fAlphaCorrExponent));
      const float4 fTA(fTacc * fAlphaCorr);
      fARacc += fTA * *(pfTFunc1D++);
      fAGacc += fTA * *(pfTFunc1D++);
      fABacc += fTA * *(pfTFunc1D);
      fTacc *= (1 - fAlphaCorr);

      // pre-integrate the gradient location weight for each interval
      float4 fWeight(float4(iIndexStart - k)/float4(iIndexStart - iIndexEnd));
      fAccWeight1 += (1.0f - fWeight) * fAlphaCorr; 
      fAccWeight2 +=         fWeight  * fAlphaCorr; 
    }
  }
  *(pfTarget++) = fARacc; 
  *(pfTarget++) = fAGacc; 
  *(pfTarget++) = fABacc;     
  *(pfTarget++) = 1 - fTacc;

  float4 fAccWeightSum(fAccWeight1 + fAccWeight2);

  if (fAccWeightSum > 0)
  {
    fAccWeight1 /= fAccWeightSum;
    fAccWeight2 /= fAccWeightSum;
  }
  *(pfTarget) = 2 * fAccWeight1;

  return;
 } // FileScope_PreIntegrate2D8bit()


/**
 * Updates the preintegrated table. (complete overwrite)
 * Assumes that the 8BitTFunc contains alpha pre-multiplied colors (is this true?)
 */
void vxRendererCpu3D::UpdateTFunc2D()
{
  //Timer timer;

  const float4 fSampleDistance(Global_fInterSampleDistance*m_fQualitySampleDistanceScaleResolutionDependent );

  // create a copy of the original 1D lookup table with corrected alpha values
  //////////////////////////////////////////////////////////////////////////////
  m_pfColormap1d = GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader());
  float4 * pfSrc(m_pfColormap1d);
  float4 * pfDest(m_pfColormap1dAlphaCorrected);
  int4 iMaxIndex(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader(), false));

  for (int4 i(0); i<=iMaxIndex; i++)
  {
    *pfDest++ = *pfSrc++; // red
    *pfDest++ = *pfSrc++; // green
    *pfDest++ = *pfSrc++; // blue
    *pfDest++ = iMaxIndex * (1 - pow(1-(*pfSrc++/(float4)iMaxIndex), Global_fInterSampleDistance/fSampleDistance)); // alpha
  }

  // create a 8bit lookup table as well as an alpha corrected one
  /////////////////////////////////////////////////////////////////
  m_pfColormap1d8bit = GetEnvironment().GetColormap()->GetTablePointerRGBA(255);
  pfSrc  = m_pfColormap1d8bit;
  pfDest = m_pfColormap1d8bitAlphaCorrected;

  for (int4 i(0); i<=255; i++)
  {
    *pfDest++ = *pfSrc++; // red
    *pfDest++ = *pfSrc++; // green
    *pfDest++ = *pfSrc++; // blue
    *pfDest++ = 1 - pow(1-(*pfSrc++/255.0F), Global_fInterSampleDistance/fSampleDistance); // alpha
  }

/*
  // compute 1D preintegrated table (8 bit)
  ///////////////////////////////////////////
  FileScope_PreIntegrate2D8bit(0, 1, +1, 1.0F, m_pfColormap1d8bitAlphaCorrected, m_pfColormap1d8bitPreIntegrated);
  for (int4 i(1); i<=256; i++)
  {
    FileScope_PreIntegrate2D8bit(0, i+1, +1, 1.0F/(float4)(i), m_pfColormap1d8bitAlphaCorrected, m_pfColormap1d8bitPreIntegrated + i*5);
    //LogDbg("setting " + ToAscii(i) + " " + ToAscii(m_pfColormap1d8bitPreIntegrated[i*5]) + " " + ToAscii(m_pfColormap1d8bitPreIntegrated[i*5+1]) + " " + ToAscii(m_pfColormapPreIntegrated1D[i*5+2]) + " " + ToAscii(m_pfColormap1d8bitPreIntegrated[i*5+3]) , "", "");
  }
  for (int4 j(255); j>=0; j--)
  {
    for (int4 i(0); i<=255; i++)
    {
      uint2 uR, uG, uB, uA, uW;

      if (i == j)
      {
        uR = m_pfColormap1d8bitPreIntegrated[5*i];
        uG = m_pfColormap1d8bitPreIntegrated[5*i + 1];
        uB = m_pfColormap1d8bitPreIntegrated[5*i + 2];
        uA = m_pfColormap1d8bitPreIntegrated[5*i + 3];
        uW = m_pfColormap1d8bitPreIntegrated[5*i + 4];
      }
      else if (j>i)
      {
        uR =        m_pfColormap1d8bitPreIntegrated[5*j]     - m_pfColormap1d8bitPreIntegrated[5*i];
        uG =        m_pfColormap1d8bitPreIntegrated[5*j + 1] - m_pfColormap1d8bitPreIntegrated[5*i + 1];
        uB =        m_pfColormap1d8bitPreIntegrated[5*j + 2] - m_pfColormap1d8bitPreIntegrated[5*i + 2];
        uA =        m_pfColormap1d8bitPreIntegrated[5*j + 3] - m_pfColormap1d8bitPreIntegrated[5*i + 3];
        uW = 0.5 * (m_pfColormap1d8bitPreIntegrated[5*j + 4] + m_pfColormap1d8bitPreIntegrated[5*i + 4]);
      }
      else
      {
        uR =        m_pfColormap1d8bitPreIntegrated[5*i]     - m_pfColormap1d8bitPreIntegrated[5*j];
        uG =        m_pfColormap1d8bitPreIntegrated[5*i + 1] - m_pfColormap1d8bitPreIntegrated[5*j + 1];
        uB =        m_pfColormap1d8bitPreIntegrated[5*i + 2] - m_pfColormap1d8bitPreIntegrated[5*j + 2];
        uA =        m_pfColormap1d8bitPreIntegrated[5*i + 3] - m_pfColormap1d8bitPreIntegrated[5*j + 3];
        uW = 0.5 * (m_pfColormap1d8bitPreIntegrated[5*i + 4] + m_pfColormap1d8bitPreIntegrated[5*j + 4]);
      }

      m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)]   = uR;
      m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+1] = uG;
      m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+2] = uB;
      m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+3] = uA;
      m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+4] = uW;
    }
  }
*/

  // compute 2D preintegrated table (8 bit)
  ///////////////////////////////////////////
  // (i < j <= k)
  for (int4 j(255); j>=0; --j)
  {
    for (int4 i(j-1); i>=0; --i)  FileScope_PreIntegrate2D8bit(j-1,i-1, -1, 1.0F/(j-i), m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + i)*5);
                                  FileScope_PreIntegrate2D8bit(j,  j+1, +1, 1.0F/(1),   m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + j)*5);
    for (int4 k(j+1); k<256; ++k) FileScope_PreIntegrate2D8bit(j+1,k+1, +1, 1.0F/(k-j), m_pfColormap1d8bitAlphaCorrected, m_pfColormap2d8bitPreIntegrated + (256*j + k)*5);
  }

  //LogDbg("Pre-integration took (sample distance = " + ToAscii(fSampleDistance) + "): " + ToAscii(timer.ElapsedTime()), "vxRendererCpu3D", "UpdateTFunc2D");
} // UpdateTFunc2D()


/**
 * Writes out the preintegrated table to a file
 * RGB Integrated  | A Integrated 
 * RGB Integrated  | A Integrated 
 * RGB Integrated  | A Integrated 
 * ----------------|--------------
 * RGB plain TFunc | A plain TFunc
 */
void vxRendererCpu3D::WriteTFunc1Dand2D()
{
  Image<PixelRGB<uint1> > image(Triple<uint4>(1, 1, 0), PixelRGB<uint1>(0, 100, 0));

  // plain TFunc out of the environment, full resolution
  ////////////////////////////////////////////////////////
  int4 iMax(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()));
  float4 fScaleTo8bit(255.0F/(float4)iMax);
  image.Resize(Triple<uint4>(iMax+20, 50, 0));

  for (int4 j(0); j<=iMax; j++)
  {
    uint2 uR(fScaleTo8bit * m_pfColormap1d[4*j+0]);
    uint2 uG(fScaleTo8bit * m_pfColormap1d[4*j+1]);
    uint2 uB(fScaleTo8bit * m_pfColormap1d[4*j+2]);
    uint2 uA(fScaleTo8bit * m_pfColormap1d[4*j+3]);
    if (uR == 0   && uG == 0   && uB == 0)   uB = 255; // make black appear blue to stand out
    if (uR == 255 && uG == 255 && uB == 255) uB = 0;   // make white appear yellow to stand out

    for (int4 k(0); k<10; ++k)
    {
      image.SetPixel(Point2D<uint2>(10+j, 10+k), PixelRGB<uint1>(uR, uG, uB));
      if (uA ==   0)      image.SetPixel(Point2D<uint2>(10+j, 30+k), PixelRGB<uint1>(0,   0, 255));
      else if (uA == 255) image.SetPixel(Point2D<uint2>(10+j, 30+k), PixelRGB<uint1>(255, 255,  0));
      else                image.SetPixel(Point2D<uint2>(10+j, 30+k), PixelRGB<uint1>(uA,  uA,  uA));
    }
  }
  image.Write("D:/Colormap1dFull.ppm");

  // write out a 1D table, 8bit resolution
  //////////////////////////////////////////
  iMax = 255;
  image.Resize(Triple<uint4>(iMax+20, 90, 0));

  // write 1D table
  for (int4 j(0); j<10; j++)
  {
    float4 * pfSrc1(m_pfColormap1d8bitAlphaCorrected);
    float4 * pfSrc2(m_pfColormap1d8bitPreIntegrated);

    for (int4 i(0); i<=iMax; i++)
    {
      image.SetPixel(Point2D<uint2>(10+i, 10+j), PixelRGB<uint1>(pfSrc1[0], pfSrc1[1], pfSrc1[2]));
      image.SetPixel(Point2D<uint2>(10+i, 30+j), PixelRGB<uint1>(255.0F * pfSrc1[3], 255.0F * pfSrc1[3], 255.0F * pfSrc1[3]));
      pfSrc1 += 4;

      image.SetPixel(Point2D<uint2>(10+i, 50+j), PixelRGB<uint1>(pfSrc2[0], pfSrc2[1], pfSrc2[2]));
      image.SetPixel(Point2D<uint2>(10+i, 70+j), PixelRGB<uint1>(255.0F * pfSrc2[3], 255.0F * pfSrc2[3], 255.0F * pfSrc2[3]));
      pfSrc2 += 5;
    }
  }
  image.Write("D:/Colormap1D8bitAndPreIntegrated8bit.ppm");

  // other tables ...
  image.Resize(Triple<uint4>(3*256+50, 30+256, 0));

  // plain 8bit TFunc out of the environment
  for (int4 j(0); j<256; j++)
  {
    uint2 uR(m_pfColormap1d8bit[4*j+0]);
    uint2 uG(m_pfColormap1d8bit[4*j+1]);
    uint2 uB(m_pfColormap1d8bit[4*j+2]);
    uint2 uA(m_pfColormap1d8bit[4*j+3]);
    if (uR == 0   && uG == 0   && uB == 0)   uB = 255; // make black appear blue to stand out
    if (uR == 255 && uG == 255 && uB == 255) uB = 0;   // make white appear yellow to stand out

    for (int4 k(0); k<10; ++k)
    {
      image.SetPixel(Point2D<uint2>(10+j, 10+k), PixelRGB<uint1>(uR, uG, uB));
      if (uA ==   0)      image.SetPixel(Point2D<uint2>(20+256+j, 10+k), PixelRGB<uint1>(0,   0, 255));
      else if (uA == 255) image.SetPixel(Point2D<uint2>(20+256+j, 10+k), PixelRGB<uint1>(255, 255,  0));
      else                image.SetPixel(Point2D<uint2>(20+256+j, 10+k), PixelRGB<uint1>(uA,  uA,  uA));
    }
  }

  // write out pre-integrated 2D colormap
  for (int4 j(255); j>=0; j--)
  {
    for (int4 i(0); i<256; ++i)
    {
      // write out 2D table
      uint2 uR(m_pfColormap2d8bitPreIntegrated[5*(j*256+i)+0] + 0.5F);
      uint2 uG(m_pfColormap2d8bitPreIntegrated[5*(j*256+i)+1] + 0.5F);
      uint2 uB(m_pfColormap2d8bitPreIntegrated[5*(j*256+i)+2] + 0.5F);
      uint2 uA(m_pfColormap2d8bitPreIntegrated[5*(j*256+i)+3] * 255);
      uint2 uW(m_pfColormap2d8bitPreIntegrated[5*(j*256+i)+4] * 255);

      if (uR == 0   && uG == 0   && uB == 0)   uB = 255; // make black appear blue to stand out
      if (uR == 255 && uG == 255 && uB == 255) uB = 0;   // make white appear yellow to stand out
      image.SetPixel(Point2D<uint2>(10+i, 30+j), PixelRGB<uint1>(uR, uG, uB));
      if (uA == 0)        image.SetPixel(Point2D<uint2>(20+256+i, 30+j), PixelRGB<uint1>(0, 0, 255));
      else if (uA == 255) image.SetPixel(Point2D<uint2>(20+256+i, 30+j), PixelRGB<uint1>(255, 255, 0));
      else                image.SetPixel(Point2D<uint2>(20+256+i, 30+j), PixelRGB<uint1>(uA, uA, uA));
      if (uW == 0)        image.SetPixel(Point2D<uint2>(30+2*256+i, 30+j), PixelRGB<uint1>(0, 0, 255));
      else if (uW == 255) image.SetPixel(Point2D<uint2>(30+2*256+i, 30+j), PixelRGB<uint1>(255, 255, 0));
      else                image.SetPixel(Point2D<uint2>(30+2*256+i, 30+j), PixelRGB<uint1>(uW, uW, uW));
    }
  }

  const float4 fSampleDistance(Global_fInterSampleDistance * m_fQualitySampleDistanceScaleResolutionDependent );
  image.Write("D:/ColormapPreIntegrated2D " + ToAscii(fSampleDistance) + ".ppm");

  // write out a 2D preintegrated table that substracts the results
  ///////////////////////////////////////////////////////////////////
  image.Resize(Triple<uint4>(40+3*256, 20+256,0));
  float4 fScale((iMax+1)/256.0F);
  const int4 iScanline(0);

  for (int4 j(255); j>=0; j--)
  {
    for (int4 i(0); i<256; ++i)
    {
      // write out 2D table
      uint2 uR(m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+0] + 0.5F);
      uint2 uG(m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+1] + 0.5F);
      uint2 uB(m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+2] + 0.5F);
      uint2 uA(m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+3] * 255);
      uint2 uW(m_pfColormap2d8bitPreIntegratedOutOf1d[5*(j*256+i)+4] * 255);

      if (uR == 0   && uG == 0   && uB == 0)   uB = 255; // make black appear blue to stand out
      if (uR == 255 && uG == 255 && uB == 255) uB = 0;   // make white appear yellow to stand out
      image.SetPixel(Point2D<uint2>(10+i, 10+j), PixelRGB<uint1>(uR, uG, uB));
      if (uA == 0)        image.SetPixel(Point2D<uint2>(20+256+i, 10+j), PixelRGB<uint1>(0, 0, 255));
      else if (uA == 255) image.SetPixel(Point2D<uint2>(20+256+i, 10+j), PixelRGB<uint1>(255, 255, 0));
      else                image.SetPixel(Point2D<uint2>(20+256+i, 10+j), PixelRGB<uint1>(uA, uA, uA));
      if (uW == 0)        image.SetPixel(Point2D<uint2>(30+2*256+i, 10+j), PixelRGB<uint1>(0, 0, 255));
      else if (uW == 255) image.SetPixel(Point2D<uint2>(30+2*256+i, 10+j), PixelRGB<uint1>(255, 255, 0));
      else                image.SetPixel(Point2D<uint2>(30+2*256+i, 10+j), PixelRGB<uint1>(uW, uW, uW));
    }
  }

  image.Write("D:/ColormapPreIntegrated2DbasedOn1D.ppm");

  return;
} // WriteTFunc1Dand2D()


/**
 * Calculates the box of interest
 */
void vxRendererCpu3D::UpdateBoxOfInterest()
{
  try
  {
    vxShareableObject< vxCropBox > *pInterestBox = reinterpret_cast< vxShareableObject< vxCropBox > *> (GetEnvironment().GetElement(vxIDs::BoxOfInterest));
    if (NULL == pInterestBox)
    {
      return;
    }

    const Box<float4> & m_fBoxOfInterestinterest(pInterestBox->GetBox());

    // make sure we did not expand past the volume boundaries
    const Vector3D<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    Point<float4> minPointVol, maxPointVol;
    minPointVol.m_x = m_fBoxOfInterestinterest.GetMinX()/units.X();
    minPointVol.m_y = m_fBoxOfInterestinterest.GetMinY()/units.Y();
    minPointVol.m_z = m_fBoxOfInterestinterest.GetMinZ()/units.Z();
    maxPointVol.m_x = m_fBoxOfInterestinterest.GetMaxX()/units.X();
    maxPointVol.m_y = m_fBoxOfInterestinterest.GetMaxY()/units.Y();
    maxPointVol.m_z = m_fBoxOfInterestinterest.GetMaxZ()/units.Z();

    m_fBoxOfInterest.SetMinPoint(minPointVol);
    m_fBoxOfInterest.SetMaxPoint(maxPointVol);
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateBoxOfInterest failed.", "vxRendererCpu3D", "UpdateBoxOfInterest"));
  }
} // UpdateBoxOfInterest()


///////////////////////////////////////////////////////////////////////
// update state after environment change


/**
 * Update the mask for intensities
 * @return whether mask has changed
 */
bool vxRendererCpu3D::UpdateMaskForIntensities()
{
  try
  {
    int4 iMaskIntensity(vxUtils::GenerateBitCodeForTransferFunction(GetEnvironment()));

    // update the class member mask
    if (m_iIsVisibleMaskIntensity != iMaskIntensity)
    {
      m_iIsVisibleMaskIntensity = iMaskIntensity;
      return true;
    }
    else
    {
      return false;
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateMaskForIntensities failed.", "vxRendererCpu3D", "UpdateMaskForIntensities"));
  }
} // UpdateMaskForIntensities()


/**
 * Update the volume histogram.
 */
bool vxRendererCpu3D::UpdateMaskForLabels()
{
  try
  {
    int4 iMaskLabel(vxUtils::GenerateBitCodeComponentsVisible(* GetEnvironment().GetComponents() ) );

    // update the class member mask
    if (m_iIsVisibleMaskLabel != iMaskLabel)
    {
      m_iIsVisibleMaskLabel = iMaskLabel;
      return true;
    }
    else
    {
      return false;
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateMaskForLabels failed.", "vxRendererCpu3D", "UpdateMaskForLabels"));
  }
} // UpdateMaskForLabels()


/**
 * Update render environment according to new transfer function.
 */
void vxRendererCpu3D::UpdateRenderVolumeIntensity()
{
  try
  {
    Timer timer;

    m_intensityRenderVol.SetHeader(GetEnvironment().GetIntensityVolume()->GetHeader());
    m_intensityRenderVol.Clear();

    vxBlockVolumeIterator<uint2> iterIntensityVolOrig(*GetEnvironment().GetIntensityVolume());
    vxBlockVolumeIterator<uint2> iterIntensityVolRender(m_intensityRenderVol);

    for (; iterIntensityVolOrig.IsNotAtEnd();  iterIntensityVolOrig.NextBlockZYX(), iterIntensityVolRender.NextBlockZYX())
    {
      iterIntensityVolRender.SetCurrentBlock(iterIntensityVolOrig.GetCurrentBlock());
    }

    if (Global_bShowTimingsInLog == true)
    {
//      LogDbg(std::string("UpdateRenderVolumeIntensity took ")+ToAscii(timer.ElapsedTime()), "vxRendererCpu3D", "UpdateRenderVolumeIntensity");
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateRenderVolumeIntensity failed.", "vxRendererCpu3D", "UpdateRenderVolumeIntensity"));
  }
} // UpdateRenderVolumeIntensity()


/**
 * Updates the render label volume according to current label volume.
 */
void vxRendererCpu3D::UpdateRenderVolumeLabel()
{
  try
  {
    Timer timer;
    m_labelRenderVol.SetHeader(GetEnvironment().GetLabelVolume()->GetHeader());
    m_labelRenderVol.Clear();
    vxBlockVolumeIterator<uint2> iterLabelOrig(*GetEnvironment().GetLabelVolume());
    vxBlockVolumeIterator<uint2> iterLabelRender(m_labelRenderVol);
    for (; iterLabelOrig.IsNotAtEnd();  iterLabelOrig.NextBlockZYX(), iterLabelRender.NextBlockZYX())
    {
      iterLabelRender.SetCurrentBlock(iterLabelOrig.GetCurrentBlock());
    }
    if (Global_bShowTimingsInLog)
    {
      LogDbg(std::string("UpdateRenderVolumeLabel took ")+ToAscii(timer.ElapsedTime()), "vxRendererCpu3D", "UpdateRenderVolumeLabel");
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateRenderVolumeLabel failed.", "vxRendererCpu3D", "UpdateRenderVolumeLabel"));
  }
} // UpdateRenderVolumeLabel()



/**
 * Update the volume for space leaping.
 */
void vxRendererCpu3D::UpdateVolumeSpaceLeap()
{
  try
  {
    Timer timer;
    const int4 iSldBlockWidth(Global_iSldBlockWidth);

    /// make new space leap volume
    Triplei newDim(Triplei((Vector3Di(m_intensityRenderVol.GetHeader().GetVolDim()) - Vector3Di(1,1,1)) / iSldBlockWidth + Vector3Di(1,1,1)));
    m_spaceLeapInfoVol.GetHeader().SetVolDim(newDim);
    m_spaceLeapInfoVol.Clear();
    SpaceLeapInfoMaker<int2> slim;
    slim.ComputeInt1SpaceLeapRadius(m_intensityRenderVol, m_labelRenderVol, m_spaceLeapInfoVol, m_iIsVisibleMaskIntensity, m_iIsVisibleMaskLabel); 

    if (Global_bShowTimingsInLog == true)
    {
//      LogDbg(std::string("UpdateVolumeSpaceLeap took ")+string(ToAscii(timer.ElapsedTime())), "vxRendererCpu3D", "UpdateVolumeSpaceLeap");
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateVolumeSpaceLeap failed.", "vxRendererCpu3D", "UpdateVolumeSpaceLeap"));
  }
} // UpdateVolumeHistogram()



///////////////////////////////////////////////////////////////////////
// debug image creation


/**
 * Draws the adaptive sampling pattern.
 * 
 * @param args          render settings
 * @param u             x position
 * @param v             y position
 * @param eSamplingInfo indicates ray - start/interpolated/additional
 */
void vxRendererCpu3D::DrawAdaptiveSamplingPatternPixel(Args & args, 
                                                       int4 u, 
                                                       int4 v, 
                                                       uint1 eSamplingInfo)
{
  if (u==0 || v==0 || u==args.iImageWidth || v==args.iImageHeight) return;
  const uint4 byte_per_pixel(4);
  const uint4 uStepPixelX(byte_per_pixel);
  const uint4 uStepPixelY(GetImage().GetDim().X() * uStepPixelX);
  uint1 * pPixel(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY);
  int4 iIndex(eSamplingInfo - eFirstRayInTile);
  PixelRGBA<uint1> pixel(PixelRGBA<uint1>::GetColorPalettePixel(iIndex));
  *(pPixel++) = pixel.R(); *(pPixel++) = pixel.G(); *(pPixel++) = pixel.B(); *(pPixel++) = pixel.A();

  if (iIndex <= eHeterogeniousRay - eFirstRayInTile)
  {
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v+args.iImageHeight) * uStepPixelY;
    *(pPixel++) = pixel.R(); *(pPixel++) = pixel.G(); *(pPixel++) = pixel.B(); *(pPixel++) = pixel.A();
  }
  if (u<4 && v<4)
  {
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 0) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 1) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 2) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 3) = 255;
  }
  // make non-colored pixels darker
  pPixel = args.pImageBuffer + (u) * uStepPixelX + (v) * uStepPixelY;
  if (*pPixel == 0 && *(pPixel+1) == 0 && *(pPixel+2) == 0 && *(pPixel+3) == 0)
  {
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v+args.iImageHeight) * uStepPixelY;
    *(pPixel) = *pPixel++ / 2; // R
    *(pPixel) = *pPixel++ / 2; // G
    *(pPixel) = *pPixel++ / 2; // B
    *(pPixel) = *pPixel++ / 2; // A
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v) * uStepPixelY;
    *(pPixel) = *pPixel++ / 2; // R
    *(pPixel) = *pPixel++ / 2; // G
    *(pPixel) = *pPixel++ / 2; // B
    *(pPixel) = *pPixel++ / 2; // A
  }
} // DrawAdaptiveSamplingPatternPixel()


/**
 * Draws space leap pattern line.
 * 
 * @param args                       render settings
 * @param eSamplingInfo              indicates ray - start/interpolated/additional
 * @param bNeedToUpdateLabelPosition unknown
 */
void vxRendererCpu3D::DrawSpaceLeapPatternLine(Args & args, 
                                               uint1 eSamplingInfo, 
                                               bool bNeedToUpdateLabelPos)
{
  // early exit if we are not in the right scanline 
  if (args.v != args.iImageHeight/2)
  {
    return;
  }

  if (bNeedToUpdateLabelPos == true)
  {
    args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
  }

  const int4 iSldBlockWidth = Global_iSldBlockWidth;
  // depending on the type of sampling info, either fill in the sampling info arrays or display the image
  m_evSamplingInfo[m_iSamplingPatternIndex]  = eSamplingInfo;
  switch (eSamplingInfo)
  {
  case eRayStart:
    {
      m_fvSamplingPos[m_iSamplingPatternIndex++] = args.fRayIntersectBoundingBoxBgnVC;
      break;
    }
  case eMultipleEmptyBlocks:
  case eSingleEmptyBlock:
  case eSingleEmptySmallBlock:
  case eInvisible:
  case eInvisibleBacktrace:
  case eZeroOpacity:
  case eZeroOpacityBacktrace:
  case eColoredSample:
  case eGradientLocation:
  case eRayEnd:
    {
      m_fvSamplingPos[m_iSamplingPatternIndex++] = args.pRayIterLabel->GetPosFloat();
      break;
    }
  case eNowDrawImage:
    {
      // update extreme points of the ray
      float4 fThisFrameDistBBox(9999999.9f);
      float4 fThisFrameDistRayEnd(0.2f);
      float4 fLastFrameDistBBox(0.1f);
      float4 fLastFrameDistRayEnd(0.2f);

      if (m_iSamplingPatternIndex>1)
      {
        Vector4d<float4> fRayStartPos(m_fvSamplingPos[0]);
        Vector4d<float4> fBBoxHitPos(m_fvSamplingPos[1]);
        Vector4d<float4> fRayEndPos(m_fvSamplingPos[m_iSamplingPatternIndex-1]);
        float4 fDistBBox((fBBoxHitPos.T() - fRayStartPos.T()));
        float4 fDistRayEnd((fRayEndPos.T()  - fRayStartPos.T()));
        if (fDistBBox   > 0.1f && fDistBBox   < fThisFrameDistBBox) fThisFrameDistBBox = fDistBBox;
        if (fDistRayEnd > 0.1f && fDistRayEnd > fThisFrameDistRayEnd) fThisFrameDistRayEnd = fDistRayEnd;
      }

      const uint4 byte_per_pixel(4);
      const uint4 uStepPixelX(byte_per_pixel);
      const uint4 uStepPixelY(GetImage().GetDim().X() * uStepPixelX);
      const float4 fScale(((args.iImageHeight-1) / (fLastFrameDistRayEnd-fLastFrameDistBBox)));
      const float4 fOffset(0 + fScale * fLastFrameDistBBox); 
      Vector4d<float4> fPosInc(args.fViewDirectionVC.GetNormalized() / fScale, 1/fScale);
      Vector4d<float4> fPos(m_fvSamplingPos[0] + fPosInc * fOffset);
      {
        //// sampling pattern
        uint1 * pImageDataPtr(args.pImageBuffer + (args.iImageHeight+1)*uStepPixelY + (args.iPixelOffset % uStepPixelY) + args.iImageWidth * 4);

        // clear bg to black
        for (int4 yPos(0); yPos < args.iImageHeight-1; ++yPos)
        {
          uint1 * pPixel(pImageDataPtr +  yPos * uStepPixelY);
          *pPixel++ = 0;
          *pPixel++ = 0;
          *pPixel   = 0;
        }

        // fill in fg with colors for the sampling events
        for (int4 iIndex(1); iIndex < m_iSamplingPatternIndex; ++iIndex)
        {
          PixelRGBA<uint1> pixel(PixelRGBA<uint1>::GetColorPalettePixel(m_evSamplingInfo[iIndex] - 1));
          float4 fPosY((-fOffset + fScale * (m_fvSamplingPos[iIndex] - m_fvSamplingPos[0]).GetLength()));
          //fPosY /= (args.iImageHeight-1.0f);
          //fPosY = pow(fPosY, 3);
          //fPosY *= (args.iImageHeight-1.0f);
          int4 yPos(Bound(0.0f, fPosY, args.iImageHeight-1.0f));
          uint1 * pPixel(pImageDataPtr +  yPos * uStepPixelY);
          *pPixel++ = pixel.R();
          *pPixel++ = pixel.G();
          *pPixel++ = pixel.B();
          *pPixel   = pixel.A();
        }
      }
      {
        //// space leap info pattern
        // set gray color according to SLD info
        uint1 * pImageDataPtr(args.pImageBuffer + (args.iImageHeight+1)*uStepPixelY + (args.iPixelOffset % uStepPixelY));
        vxBlockVolumeIterator< int1> sldIter(m_spaceLeapInfoVol);
        vxBlockVolumeIterator<uint2> labelIter(m_labelRenderVol);
        vxBlockVolumeIterator<uint2> intensityIter(m_intensityRenderVol);

        { // draw color map
          for (int4 yPos(args.iImageHeight-11); yPos < args.iImageHeight-1; ++yPos)
          {
            uint1 * pPixel(pImageDataPtr + yPos * uStepPixelY);
            int4 xPos(args.iPixelOffset % uStepPixelY);
            PixelRGBA<uint1> pixel = PixelRGBA<uint1>::GetColorPalettePixel(xPos * 5 / args.iImageWidth);
            *pPixel++ = pixel.R();
            *pPixel++ = pixel.G();
            *pPixel++ = pixel.B();
            *pPixel   = pixel.A();
          }
        }
        // draw space leap info
        for (int4 yPos(0); yPos < args.iImageHeight-11; ++yPos, fPos = fPos + fPosInc)
        {
          sldIter.SetPos(fPos.X()/iSldBlockWidth, fPos.Y()/iSldBlockWidth, fPos.Z()/iSldBlockWidth);
          int4 color(Min(255, int4(sldIter.GetVoxel()) * 55));
          uint1 * pPixel(pImageDataPtr + yPos * uStepPixelY);

          PixelRGBA<uint1> pixel;
          labelIter.SetPos(fPos.X()+0.5, fPos.Y()+0.5, fPos.Z()+0.5);
          if (fPos.X() < 0 || fPos.X() > m_labelRenderVol.GetHeader().GetVolDim().X()-1 ||
               fPos.Y() < 0 || fPos.Y() > m_labelRenderVol.GetHeader().GetVolDim().Y()-1 ||
               fPos.Z() < 0 || fPos.Z() > m_labelRenderVol.GetHeader().GetVolDim().Z()-1)
          { // outside valid blocks area
            pixel = PixelRGBA<uint1>::GetColorPalettePixel(11);
          }
          else
          { // normal inside, but not visible area
            pixel = PixelRGBA<uint1>::GetColorPalettePixel(sldIter.GetVoxel());
            // visibile aera
            if ((*args.pvComponents)[labelIter.GetVoxel()]->IsIntensityVisible())
            {
              intensityIter.SetPos(labelIter);
              if (args.pfColormap2d8bitPreIntegrated[3+5*(257*(intensityIter.GetVoxel()>>args.iShiftTo8Bit))] > 0)
              {
                pixel = PixelRGBA<uint1>::GetColorPalettePixel(12);
              }
            }
          }
          *pPixel++ = pixel.R();
          *pPixel++ = pixel.G();
          *pPixel++ = pixel.B();
          *pPixel   = pixel.A();
        }
      }
      if (args.u == args.iImageWidth-1)
      {
        if (fThisFrameDistBBox > 9999999) fThisFrameDistBBox = 0.2f;
        fLastFrameDistBBox   = fThisFrameDistBBox  ; 
        fLastFrameDistRayEnd = fThisFrameDistRayEnd; 
        fThisFrameDistBBox   = 9999999.9f;
        fThisFrameDistRayEnd = 0.2f;
      }
      // reintialize for the next ray.
      for (int4 k(0); k<m_evSamplingInfo.size(); ++k)
      {
        m_evSamplingInfo[k] = 0;
      }

      // show line in volume rendered image
      *(args.pImageBuffer + args.u * uStepPixelX + args.v * uStepPixelY   ) = 200;

      m_iSamplingPatternIndex = 0;
      break;
    } // end case eNowDrawImage
  } // end switch eSamplingInfo
} // DrawSpaceLeapPatternLine()


/**
 * Clamp the intensity volume to the valid range
 */
void vxRendererCpu3D::ClampIntensityVolume()
{
  // do it only once per volume
  if (m_bIntensityVolumeClamped == false )
  {
    m_bIntensityVolumeClamped = true;

    // clamp to valid range
    Timer timer;
    ValueRange<int2> range;
    vxBlockVolumeIterator<uint2> iterVol(* GetEnvironment().GetIntensityVolume() );
    for (; iterVol.IsNotAtEnd(); iterVol.NextBlockZYX() )
    {
      for (; iterVol.IsNotAtEndOfBlock(); iterVol.NextZYXinsideBlock() )
      {
        // clamp
        int2 iValue(iterVol.GetVoxel() );
        if (iValue < 0    )
        {
          range.AddValue(iValue );
          iterVol.SetVoxel(0 );
        }
        if (iValue & 0xF000 )
        {
          range.AddValue(iValue );
          iterVol.SetVoxel(4095 );
        }
      }
    }
    LogDbg("Range of data before clamping: " + ToAscii(int4(range.GetMinimum() ) ) + " to " + ToAscii(int4(range.GetMaximum() ) ), "vxRendererCpu3D", "ClampIntensityVolume" );
    LogDbg("Clamping to valid range took: " + timer.AsString(), "vxRendererCpu3D", "ClampIntensityVolume" );
  }
} // ClampIntensityVolume();


// undefines
#undef FILE_REVISION


// $Log: vxRendererCpu3D.C,v $
// Revision 1.54  2007/02/13 22:32:27  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.53  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.52  2007/01/16 17:03:14  gdavidson
// Removed event timer
//
// Revision 1.51  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.50  2006/10/06 18:52:40  geconomos
// renabled fixed size rendering
//
// Revision 1.49  2006/10/03 20:07:50  geconomos
// modified to use new SliceShadowMode enumeration
//
// Revision 1.48  2006/07/18 16:19:29  geconomos
// removed unused "SampleDistanceScale" instance field
//
// Revision 1.47  2006/05/31 16:18:22  geconomos
// porting to visual studio 2005
//
// Revision 1.46  2006/05/02 18:41:07  romy
// Considering the previous pixel color for casting.
//
// Revision 1.45  2006/04/25 18:57:10  geconomos
// Issue# 4730:  Added condition to check that the label position is within bounding box while space leaping along ray
//
// Revision 1.44  2006/04/12 15:37:58  romy
// Avoided calling CastOneRay in empty object areas.
//
// Revision 1.43  2006/04/10 19:45:51  frank
// rendering acceleration 30%+
// leaped to min distance of nearest left and right eighth column in scanline as well to above and below pixels
//
// Revision 1.42  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.41  2006/03/02 15:47:40  frank
// a property on the crop box changed to a constant
//
// Revision 1.40  2006/02/17 15:14:50  frank
// brightened up shadowed rendering
//
// Revision 1.39  2006/02/09 14:06:37  geconomos
// 64-bit conditionals
//
// Revision 1.38  2006/02/01 21:23:12  frank
// working on soft shadows
//
// Revision 1.37  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.36  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.35  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.34  2005/11/25 16:53:19  frank
// fixed swapping datasets during rendering
//
// Revision 1.33  2005/11/22 15:29:45  frank
// formatting
//
// Revision 1.32  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.31  2005/10/25 17:33:02  geconomos
// added for up to four threads
//
// Revision 1.30.2.12  2005/08/24 15:14:08  frank
// Issue #4388: X-Ray mode looks bad on the edges of the volume
//
// Revision 1.30.2.11  2005/08/12 17:20:51  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.30.2.10  2005/08/05 17:34:36  frank
// fixed progressive refinement problem
//
// Revision 1.30.2.9  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.30.2.8  2005/08/04 18:32:38  frank
// Issue #4330: Moved cursor tracking from the 3D renderer to a controller
//
// Revision 1.30.2.7  2005/08/01 18:50:49  frank
// fixed MIP projection
//
// Revision 1.30.2.6  2005/07/29 13:06:09  frank
// Issue #4319: Fixed clip plane with unshaded voxels
//
// Revision 1.30.2.5  2005/07/19 15:33:17  frank
// fixed some rendering bugs
//
// Revision 1.30.2.4  2005/07/13 19:21:35  frank
// fixed endoluminal rendering
//
// Revision 1.30.2.3  2005/07/13 12:06:57  frank
// improved responsiveness of rendering
//
// Revision 1.30.2.2  2005/07/08 15:01:23  frank
// added rendering option for unshaded and uncolored edge, crop, slice cuts
//
// Revision 1.30.2.1  2005/06/24 13:00:46  frank
// Issue #4223: properly blended in the background color
//
// Revision 1.30  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.29  2005/05/20 12:13:18  frank
// cleaning up slice shadows in 3D
//
// Revision 1.28  2005/05/18 15:06:49  frank
// cleaning up slice shadows
//
// Revision 1.27  2001/01/03 19:30:22  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.26  2004/11/17 19:12:45  michael
// fixed issue with not starting progressive refinement AFTER loading session.
//
// Revision 1.25  2004/11/17 16:40:53  michael
// fixed black image but refinement doesn't always trigger after loading session!!!
//
// Revision 1.24  2004/11/04 01:34:26  michael
// Added check for fixed size rendering to force the immediate image generation
// using the specified size. Attention, ONLY POWERS OF TWO WILL WORK!!!
//
// Revision 1.23  2004/10/21 16:39:37  michael
// Added correct reporting of fps in vxDataRenderer
//
// Revision 1.22  2004/10/18 14:48:13  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.21  2004/10/05 14:46:35  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.20  2004/09/21 15:51:02  frank
// fixed rendering at thread boundaries
//
// Revision 1.19  2004/09/09 14:55:24  michael
// Added frame-time to vxDataRenderer
//
// Revision 1.18  2004/09/01 12:30:24  frank
// decreased the granularity of component array modified bits
//
// Revision 1.17  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.16  2004/08/05 13:56:15  michael
// Took out du and dv in rendering arguments. Was never used.
//
// Revision 1.15  2004/05/20 12:50:13  frank
// removed UpdateBoundingBox and moved functionality to a controller
//
// Revision 1.14  2004/05/18 15:36:21  frank
// fixed uninitialized pointer offset
//
// Revision 1.13  2004/05/18 15:01:42  vxconfig
// renamed lookup tables, cleaned up the writing of lookup tables, and added
// initialization for sample and previosu sample is args.
//
// Revision 1.12  2004/05/14 21:16:37  vxconfig
// brought back 2nd half of the preintegration table to see front sides, too.
//
// Revision 1.11  2004/05/14 20:58:09  vxconfig
// Major changes:
// - Fixed long time broken computation of pre-integration table!!!
// - Adjusted debug output of pre-integrated table to show 3 tables
// - Alpha based Space leap forward is now stepping only 1 instead of 2 for better quality
// - Skipping checks the 2D preintegrated alpha and not 1D for better quality
// - Took out one redundant space leap back step (is it really redundant???)
//
// Revision 1.10  2004/05/14 13:24:33  frank
// fixed bad pixels when background color is not black
//
// Revision 1.9  2004/05/12 19:26:51  frank
// improved quality for off-line rendering
//
// Revision 1.8  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.7  2004/04/06 16:26:30  frank
// unified environment access naming
//
// Revision 1.6  2004/03/31 17:46:06  frank
// unused include files
//
// Revision 1.5  2004/03/30 18:19:10  frank
// fixed old cut-paste error
//
// Revision 1.4  2004/03/26 21:16:04  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.3  2004/03/26 19:47:29  frank
// added alpha channel to 3D rendering so overlaying now works properly for PET fusion layouts.
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.504  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.503  2004/02/25 14:14:33  michael
// coding standard: assign NULL after deletion
//
// Revision 1.502  2004/02/10 13:32:46  michael
// improved light source interaction
//
// Revision 1.501  2004/02/07 03:43:30  michael
// fixed light source/direction issue in 3d rendering by making the mouse position
// relative to the viewport size instead of absolut. Now the change in vxWinRect
// has correctly no impact on shading.
//
// Revision 1.500  2004/02/05 14:09:38  michael
// Fixed issue #3665 by adding vxWinRect Sensitivity to 3D renderer
//
// Revision 1.499  2004/01/14 16:54:39  frank
// fixed unnecessary redarws when changing the slice shadow that has no effect on the rendered image
//
// Revision 1.498  2003/12/19 00:03:17  michael
// added being sensitive to change of components INDEPENDENT of whether
// components are displayed in MPRs!!! (copy and paster error)
//
// Revision 1.497  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.496  2003/11/28 23:34:21  michael
// in fly mode, do not attempt to increase frame-rate, results in jumpy behaviour
// with many hickups. Needs to be reworked after RSNA.
//
// Revision 1.495  2003/11/21 16:22:42  frank
// Fixed slice shadows corruption
// Removed unnecessary logic for uint2 label being less than zero
//
// Revision 1.494  2003/11/21 04:49:38  frank
// Initialized light source better
//
// Revision 1.493  2003/11/18 20:00:49  michael
// fixed endoluminal views ... (my self-made bug)
//
// Revision 1.492  2003/11/18 16:23:43  michael
// Fixed the changing view direction when switching cropbox on and off. It was
// only setting a new reference point and then calling ViewAll() but one also needs
// to set the new eye so that the view direction is preserved.
//
// Revision 1.491  2003/11/17 00:45:54  michael
// Added ability to control speed/quality by slider ...
//
// Revision 1.490  2003/11/16 23:41:14  michael
// Changed frame-rate adjustment to allow for quality increase if possible
//
// Revision 1.489  2003/11/11 19:41:42  michael
// made crop box changes trigger rendering only if the crop box display is enabled, too.
//
// Revision 1.488  2003/11/11 19:34:55  michael
// made slice shadow changes trigger rendering only if thei slice shadow display
// is enabled, too.
//
// Revision 1.487  2003/11/11 16:00:33  michael
// sampling the label volume with Nearest Neighbour and not taking the floor of
// all three axises (lower left corner). Solves the issue of not seeing any label in
// 3D that is present on axial slice number 1 (which is the largest coordinate)
//
// Revision 1.486  2003/11/07 18:12:21  frank
// Removed the debug string explaining why rendering was triggered.  This slows down rendering too much in the normal case.
//
// Revision 1.485  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.484  2003/11/05 01:18:47  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.483  2003/11/04 14:37:34  frank
// Issue #3556: Fixed slice shadow degeneration when components are disabled.  Also, fixed problem of black dots near the edges of invisible components.
//
// Revision 1.482  2003/11/03 21:43:46  frank
// Issue #3557: Fixed exception when switching back from report view when oblique slice shadow was previously active.
//
// Revision 1.481  2003/10/09 13:37:19  mkontak
// Changed to better handle threads on a signle CPU system.
//
// Revision 1.480  2003/10/03 12:14:03  michael
// took out log messages that were used for debugging and corrected the size
// of the alpha corrected lookup table to fit for the max (16 bit)
//
// Revision 1.479  2003/09/29 17:19:37  michael
// accounting for the new event flags in vxEnvironment as well as starting
// progressive refinement when it is switched off.
//
// Revision 1.478  2003/09/22 16:01:17  michael
// modified the refinement to be triggered on IsModified() of the EventTimer and
// not the RendererCpu3dData. RendererCpu3dData is now used to set parameters
// for the 3D rendering such as jittering, shading, preintegration, and sampling
// rate.
//
// Revision 1.477  2003/09/16 15:14:06  frank
// Fixed slice shadow cut for oblique planes
//
// Revision 1.476  2003/09/15 17:56:51  frank
// fixed color of slice shadows
//
// Revision 1.475  2003/09/12 20:56:52  frank
// Removed dead code
//
// Revision 1.474  2003/09/12 14:24:41  frank
// Implemented the oblique slice shadow
//
// Revision 1.473  2003/09/08 17:44:24  frank
// Added slice shadow clipping
//
// Revision 1.472  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.471  2003/09/02 14:52:48  frank
// Changed the way slice shadows are determined - made it more flexible
//
// Revision 1.470  2003/08/27 14:37:32  michael
// made 3D renderer more sophisticated with respect to which rendering mode
// flags it is sensitive to
//
// Revision 1.469  2003/08/20 15:48:18  michael
// using M_PI instead of typed in values for it
//
// Revision 1.468  2003/08/11 13:29:45  frank
// Issue #3385: Fixed display of label on top when the color is not visible.  Also fixed the noise in the label image.
//
// Revision 1.467  2003/08/07 11:31:09  frank
// Issue #3385: Removed empty band of labels in the middle of image
//
// Revision 1.466  2003/08/04 21:56:50  michael
// took out debug log message slowing down execution ...
//
// Revision 1.465  2003/08/04 21:53:58  michael
// Fixed a skrewy thing with the update when changing the cropbox. It is a big
// hack but since it was implemented hacky in the first step, we will need quite
// some time to implement this in a clean fashion ...
//
// Revision 1.464  2003/08/01 21:34:58  michael
// renamed TFunc to Colormap since this is what we call it (non academic)
//
// Revision 1.463  2003/08/01 12:25:15  michael
// removed logs, set responsive size to lowest, and added frame time log flag (global)
//
// Revision 1.462  2003/07/29 18:34:29  michael
// added mode to be able to prevent progressive refinement and set image size fixed.
//
// Revision 1.461  2003/07/29 11:05:08  michael
// replaced redundant code by one centralized piece that draws the histogram.
// However, it is still actively drawn but should use a texture that is only updated
// if the BaseHistogram has changed ...
//
// Revision 1.460  2003/07/24 17:56:22  michael
// Changed multi-threaded rendering by creating the first image in blocking
// fashion so that there is an immediate image and the subsequent refinement
// images are created in the background. Also added that the thread priority
// is set to low if it is not the active viewer and to normal if it is the active viewer.
// This way, the rendering of multiple 3D images is intuitively prioritized.
//
// Revision 1.459  2003/07/15 17:09:14  frank
// Set view direction when 3D cursor changed
//
// Revision 1.458  2003/07/15 12:12:01  frank
// Issue #3303: Improved quality of x-ray rendering, allowed inverting colors
//
// Revision 1.457  2003/07/14 15:38:18  michael
// added one test in PreRender() to be sensitive to changes of the interactive
// segmentation object only if the display of intensity range is enabled
//
// Revision 1.456  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.455  2003/07/07 12:40:05  frank
// Improved x-ray rendering
//
// Revision 1.454  2003/06/20 17:09:57  frank
// Fixed MIP rendering for increased quality
// unified calls to 8-bit-shift function
// cleaned out dead code
// took global, unchanging values out of args and into member variables
//
// Revision 1.453  2003/06/19 20:21:42  michael
// removed space
//
// Revision 1.452  2003/06/18 17:48:50  geconomos
// Changed the display of the box of interest.
//
// Revision 1.451  2003/06/18 15:19:57  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.450  2003/06/12 19:43:26  frank
// Checked for NULL pointer in active viewer type
//
// Revision 1.449  2003/06/09 18:13:53  michael
// fixed last hard coded bit-shift in raycaster (hopefully)
//
// Revision 1.448  2003/06/09 16:17:54  michael
// took out redundant code
//
// Revision 1.447  2003/06/09 15:54:19  michael
// replaced intensity mask based on 12 bit data with what is in the environment
//
// Revision 1.446  2003/06/09 14:57:21  michael
// initialization without assignment ...
//
// Revision 1.445  2003/06/09 12:30:22  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.444  2003/06/06 15:22:27  geconomos
// Disabled Michael's progressive refinement code.
//
// Revision 1.443  2003/06/04 18:22:57  michael
// added namespace declaration
//
// Revision 1.442  2003/06/03 19:36:34  frank
// code review
//
// Revision 1.441  2003/05/29 17:04:29  michael
// fixed initial stalling by having the first image being rendered in the
// background, too (progressive refinement).
//
// Revision 1.440  2003/05/22 17:27:10  michael
// made base rendering speed faster but with lower quality. Beta site (SUNY)
// was complaining that it is too slow and slower than Vital.
//
// Revision 1.439  2003/05/21 12:12:30  frank
// Revised the slice shadow to more specific about the current slice shadow
//
// Revision 1.438  2003/05/20 20:20:42  michael
// removed more temporary debug code
//
// Revision 1.437  2003/05/20 20:20:15  michael
// removed debug code
//
// Revision 1.436  2003/05/20 19:54:46  michael
// fixed issue #3253
//
// Revision 1.435  2003/05/20 16:44:40  michael
// fixed issue #3252
//
// Revision 1.434  2003/05/20 02:00:43  michael
// fixed issue #3250
//
// Revision 1.433  2003/05/19 15:07:10  michael
// fixed issue #3203
//
// Revision 1.432  2003/05/19 14:38:32  frank
// Removed unused return parameter
//
// Revision 1.431  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.430  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.429  2003/05/14 22:11:41  michael
// fixed bug in progress parameter initialization
//
// Revision 1.428  2003/05/13 14:25:30  frank
// code review in progress
//
// Revision 1.427  2003/05/12 18:19:33  frank
// Added slice shadows object
//
// Revision 1.426  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.425  2003/04/29 21:19:04  michael
// renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData
//
// Revision 1.424  2003/04/24 14:06:27  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.423  2003/04/23 18:56:17  frank
// Issue #3052: Showed 2D slice shadows in 3D image
//
// Revision 1.422  2003/04/18 15:16:42  frank
// Getting rid of HUconverter
//
// Revision 1.421  2003/04/10 13:51:43  frank
// Added comments
//
// Revision 1.420  2003/04/07 19:05:02  michael
// fixed background color in 3D ... even though it still requires another adaptive
// refinement process instead of just replacing the background color. This is left
// to be done later.
//
// Revision 1.419  2003/04/07 18:32:58  michael
// instead of testing for black pixel, we are now testing for transparent pixel
//
// Revision 1.418  2003/04/07 18:25:15  michael
// fixed background color for MPR views
//
// Revision 1.417  2003/03/25 17:32:27  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.416  2003/03/20 21:27:00  frank
// Brightened up the x-ray image
//
// Revision 1.415  2003/03/19 13:06:46  frank
// Issue #2924: Brightened up the labels.  They were unneccesarily darkened.
// Removed one line in the RayBlendLabelColor function that was doubling down the intensity of the label.
//
// Revision 1.414  2003/03/18 17:18:19  michael
// added a comment to maybe use GetSampleNearest() instead of GetVoxel()
//
// Revision 1.413  2003/03/11 18:45:15  geconomos
// Checked ActiveViewerType before enabling the rendering of slice shadows.
//
// Revision 1.412  2003/03/07 20:33:17  geconomos
// Added a check if box of interest should be shown before
// updating the 3D image.
//
// Revision 1.411  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.410  2003/02/26 18:35:34  frank
// Cleaned out old 0xFFFF label volume blocks outside the valid volume.
// Used the ray parameter T instead to decide when to stop ray.
//
// Revision 1.409  2003/02/21 23:28:55  frank
// Patched up some bugs in space leaping past the end of the volume and going into 0xFFFF (invalid blocks).
// We just check the label value for 0xFFFF before using it to index into any component arrays.  Still the
// question of why we need 0xFFFF outside the label volume valid blocks.  Also it seems to slow down a
// little bit when you make a label invisible, where it should only get faster.
//
// Revision 1.408  2003/02/18 17:59:38  frank
// Made box-of-interest less dramatic
//
// Revision 1.407  2003/02/06 20:06:39  frank
// Moved up progress meter
//
// Revision 1.406  2003/01/31 21:04:00  frank
// Added back box of interest.
//
// Revision 1.405  2003/01/28 19:31:28  geconomos
// Implementation of BackgroundColor.
//
// Revision 1.404  2003/01/27 23:21:26  ingmar
// reenabled dual thread rendering
//
// Revision 1.403  2003/01/27 21:30:32  ingmar
// added capability to have non-black background (taken from environment)
// also now change the RayEndT to RayBgnT+0.1 if RayEndT <= RayBgnT,   this fixes lots of crashes
//
// Revision 1.402  2003/01/23 21:21:57  ingmar
// reduced slice shadow intensity outside visible data to 50%
//
// Revision 1.401  2003/01/23 17:14:22  ingmar
// brought slice shadows back in front of background
//
// Revision 1.400  2003/01/22 22:05:16  ingmar
// the RayIterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.399  2003/01/02 15:29:30  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.398  2002/12/24 15:33:57  frank
// tuned the x-ray attenuation constant
//
// Revision 1.397  2002/12/23 21:53:28  frank
// fixed typo
//
// Revision 1.396  2002/12/23 21:47:22  frank
// Added x-ray projection
//
// Revision 1.395  2002/12/20 19:02:12  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.394  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.393  2002/12/19 21:26:00  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.392  2002/12/19 20:45:08  ingmar
// PixelRGBA member data is now private -> need to use access functions
//
// Revision 1.391  2002/12/18 23:26:47  frank
// Fixed translucent rays over background problem.
//
// Revision 1.390  2002/12/12 17:05:29  ingmar
// removed dead code from old 4^3 block histogram
//
// Revision 1.389  2002/12/10 20:18:47  ingmar
// removed obsolete crop box test (the rays now start & stop at the crop box boundary)
//
// Revision 1.388  2002/12/10 19:58:24  ingmar
// removed  obsolete m_iCropBoxDataBlocks16 (now we always have the complete volume in memory)
//
// Revision 1.387  2002/12/10 19:09:06  ingmar
// slice shadows are now allways visible (even on first and last slice)
//
// Revision 1.386  2002/12/09 23:36:06  ingmar
// switched to using 8bit color table
// improved WriteTFunc1Dand2D()
//
// Revision 1.385  2002/11/29 20:10:13  ingmar
// fixed initiation of refinement mode after hitting preset button
//
// Revision 1.384  2002/11/27 22:29:05  ingmar
// recovered going back to last responsive framerate when restarting interaction
//
// Revision 1.383  2002/11/27 21:57:23  ingmar
// one more pass at getting progressive refinement to start only when really wanted
//
// Revision 1.382  2002/11/27 19:16:10  ingmar
// 3D view now no longer follows 3D cursor if the camera is in parallel projection mode
//
// Revision 1.381  2002/11/27 16:30:34  ingmar
// added 0.1 to ensure that float comparrison allways keeps the slice shadow visible, even on the first/last slice
//
// Revision 1.380  2002/11/26 18:11:20  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
// this now also allows for slice shadows as planes
//
// Revision 1.379  2002/11/25 21:06:30  geconomos
// now not drawing image on first initialization (but starting progressive refinement
//
// Revision 1.378  2002/11/25 18:32:38  ingmar
// cleaned up progressive refinement timer handling,
// now there are no more timer events after the highest resolution image is finished
//
// Revision 1.377  2002/11/21 21:25:30  ingmar
// removed hack to draw text as part of the Cpu3D renderer
//
// Revision 1.376  2002/11/21 21:20:47  ingmar
// made __declspec(align(16))  the default, as VC++ 7.0 now supports it
//
// Revision 1.375  2002/11/21 19:09:20  ingmar
// only change the camera box if the chage is more than 25% of the current size
//
// Revision 1.374  2002/11/21 18:06:49  ingmar
// cleaned up when to start the QUALITYSPEED timer
// this also fixed the problem that progressive refinement used to stop after hitting esc when sculpting
//
// Revision 1.373  2002/11/21 17:41:06  ingmar
// progressive refinement is now disabled when momentum is on
// also added some more comments
//
// Revision 1.372  2002/11/21 16:34:30  ingmar
// removed debug print messages
// and reset progressive refunement delay to 10 ms
//
// Revision 1.371  2002/11/20 21:38:19  geconomos
// Made rendering threads persitent.
//
// Revision 1.370  2002/11/20 17:45:45  michael
// moved progress bar above ViewerBar display
//
// Revision 1.369  2002/11/19 20:22:33  ingmar
// set the camera box to the visibility box (but only if we are not flying)
//
// Revision 1.368  2002/11/19 19:09:10  ingmar
// smoother autoflight
//
// Revision 1.367  2002/11/18 19:27:14  ingmar
// back tr previous 2D texture draw to remove image wable
//
// Revision 1.366  2002/11/18 18:53:05  ingmar
// removed label volume binary histogram generation code, as it is now in vxUtils and as there is now a binary Histogram in the environment
//
// Revision 1.365  2002/11/18 18:45:55  ingmar
// reduced number of calls to Update RenderVolume copies by now copying the complete volume
// so after a bounding box change no reset of the rendervolumes is needed
//
// Revision 1.364  2002/11/18 18:44:36  ingmar
// reduced number of calls to Update RenderVolume copies by now copying the complete volume
// so after a bounding box change no reset of the rendervolumes is needed
//
// Revision 1.363  2002/11/18 18:13:04  ingmar
// cleaned up PreRender()
//
// Revision 1.362  2002/11/18 16:50:23  ingmar
// now setting the camera bounding box according to what is visible by the renderer
//
// Revision 1.361  2002/11/15 18:10:14  frank
// Removed unused parameter (iOpacity)
//
// Revision 1.360  2002/11/14 19:12:18  ingmar
// synchronized ray setup with current OpenGL state
//
// Revision 1.359  2002/11/13 00:12:11  michael
// account for changes in vxComponent and vxComponentArray
//
// Revision 1.358  2002/11/12 21:49:25  ingmar
// removed redundand UpdateTexture() function (CreateTexture() does it all)
//
// Revision 1.357  2002/11/12 19:15:48  ingmar
// decreased time to wait for progressive refinement to start
// also checked if the environment change does not affect the renderer resulting in just redrawing the current texture
//
// Revision 1.356  2002/11/11 16:26:42  ingmar
// added ray direction normalization to fix over-bright images when the ray direction is used as diffuse light direction
//
// Revision 1.355  2002/11/10 23:06:58  michael
// removed includes of texture server and update calls on textures
//
// Revision 1.354  2002/11/08 19:00:23  ingmar
// moved the visible data bounding box into the environment to be accessable by the manip CamFly
//
// Revision 1.353  2002/11/08 17:50:19  ingmar
// use of point light source for better endoluminal images
//
// Revision 1.352  2002/11/08 17:19:12  ingmar
// added computation of depth image
//
// Revision 1.351  2002/11/07 22:46:52  michael
// added float values for the parts that contribute to the progression. They are
// initialized differently in PreRender(), e.g. for TagsOnTop
//
// Revision 1.350  2002/11/07 20:07:38  ingmar
// back to single page SampleSingleRay() that is much easier to read
//
// Revision 1.349  2002/11/07 18:29:52  ingmar
// progress bar now starts above the button bar again
//
// Revision 1.348  2002/11/07 17:24:42  geconomos
// Added check of TRACK_CURSOR_3D rendering mode in PreRender()
//
// Revision 1.347  2002/11/07 00:09:03  ingmar
// code beautify
//
// Revision 1.346  2002/11/06 23:28:46  michael
// fixing the initial lagging of the rendering
//
// Revision 1.345  2002/11/05 22:51:09  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.344  2002/11/05 20:46:25  geconomos
// Commented out setting of eye position in checking of CURSOR3D.
// This caused us 6 hours of debugging(thank you Ingmar!)
//
// Revision 1.343  2002/11/05 00:39:17  ingmar
// added m_iImageSizeIndexResponsive to keep track of las responsive image size to be used when progressive rendering is being canceled
//
// Revision 1.342  2002/11/04 23:42:35  ingmar
// added RenderImageInTiles() to seperate image generation from texture update
//
// Revision 1.341  2002/11/04 15:20:40  ingmar
// made ivImgSize a member variable
//
// Revision 1.340  2002/11/02 03:09:30  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.339  2002/10/25 19:25:40  ingmar
// added checking for Cursor3d
//
// Revision 1.338  2002/10/24 23:06:39  ingmar
// now setting the camera to the Cursor3d location -> one can now set the 3d view from the 2d viewers
//
// Revision 1.337  2002/10/23 23:13:09  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.336  2002/10/23 21:26:44  frank
// Patched over problem with accessing bad data during rendering. (was crashing CS during rendering)
//
// Revision 1.335  2002/10/23 16:01:28  ingmar
// removed RaySkipSmallBlocks() because computing how far to advance costs more time than just jumping 2 voxels at a time and going back if one went to far -> 10% speedup for segmented component rendering
//
// Revision 1.334  2002/10/21 23:14:22  ingmar
// now handles parallel and perspective cameras
//
// Revision 1.333  2002/10/18 17:52:14  ingmar
// local copy of vxComponents array to be used during rendering.
// This avoids problems when other threads change components during rendering
//
// Revision 1.332  2002/10/04 23:34:41  ingmar
// added Global_bShowTimingsInLog  to remove timings from log
//
// Revision 1.331  2002/10/03 19:14:53  ingmar
// removed one redundant call to SpaceLeap(0.5)
//
// Revision 1.330  2002/10/03 18:58:52  ingmar
// adjusted skipping empty alpha to jump two voxels each time -> now segmenting air is only insignificantly faster
//
// Revision 1.329  2002/09/27 22:55:29  ingmar
// more stable crop boc boundary appearance
//
// Revision 1.328  2002/09/26 19:20:26  geconomos
// Removed debug messages.
//
// Revision 1.327  2002/09/25 23:12:35  ingmar
// changes from intel  tuning visit
//
// Revision 1.326  2002/09/25 22:09:21  wenli
// Add BoxOfInterest
//
// Revision 1.325  2002/09/25 20:36:42  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.324  2002/09/25 15:27:29  wenli
// CropBox Update
//
// Revision 1.323  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.322  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.321  2002/09/18 22:41:44  frank
// Updated rendering if crop box changed.
//
// Revision 1.320  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.319  2002/09/10 15:51:45  dmitry
// Nevermind, the last change was not needed.
//
// Revision 1.318  2002/09/10 15:17:17  dmitry
// Write on an image only works with the new compilers because of template specialization.
//
// Revision 1.317  2002/09/05 22:37:47  jaddonisio
// New vxComponent.
//
// Revision 1.316  2002/09/03 20:52:51  ingmar
// bounding box no longher extends past the volume boundary
//
// Revision 1.315  2002/08/26 20:56:44  ingmar
// better crop box computation when the environment crop box is not set
//
// Revision 1.314  2002/08/26 16:49:10  ingmar
// switched intensity mask and label mask computation to use the vxUtils GEnerateBitMask functions
//
// Revision 1.313  2002/08/26 14:39:33  ingmar
// space leap info display now shows where label and intensity are visible
// the benchmark  loop now has only 3 steps for slow rendering modes
// fixed cropBox to include all of the maximum block
//
// Revision 1.312  2002/08/23 14:04:27  uday
// Error update.
//
// Revision 1.311  2002/08/22 22:38:52  uday
// Moved calculation of bounding box to vxutils.
//
// Revision 1.310  2002/08/22 21:23:01  ingmar
// reenabled zero alpha skipping and anisotropic graqdient correction
//
// Revision 1.309  2002/08/22 20:52:06  ingmar
// faster tags on top rendering
//
// Revision 1.308  2002/08/22 15:21:22  ingmar
// added bool member to remember crop box state, now turning the crop box on or off is reflected properly by the rendering
//
// Revision 1.307  2002/08/21 22:31:38  ingmar
// medium sixe progress bar (sorry Micha)
//
// Revision 1.306  2002/08/21 22:05:33  ingmar
// now differentiating between visibleVoxelCropBox and enclosing multipleOf16BlockCropBox
//
// Revision 1.305  2002/08/21 18:33:50  michael
// extended utility method for drawing a rectangle
//
// Revision 1.304  2002/08/19 13:45:56  ingmar
// smaller progress bar
//
// Revision 1.303  2002/08/15 22:27:48  frank
// Added LABEL_TOP_TOP mode.
//
// Revision 1.302  2002/08/14 22:26:21  uday
// Added CropBox.
//
// Revision 1.301  2002/08/14 16:35:15  ingmar
// enabled turning off display of intenmsity range
//
// Revision 1.300  2002/08/14 16:07:41  wenli
// initial version - Vascular
//
// Revision 1.299  2002/08/09 15:49:17  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.298  2002/08/08 18:12:57  ingmar
// finetuned when to switch between resolutions
//
// Revision 1.297  2002/08/08 13:06:16  geconomos
// changed all occurences of const object const * to conto object * const
//
// Revision 1.296  2002/08/07 22:26:46  ingmar
// now only looking for intensity range bounds in environment when the show range mode is on
// -> apps like CS will no longer segfault if they do not activate(have) this mode
//
// Revision 1.295  2002/08/07 20:19:51  ingmar
// added extra delay before going down to low resolutions such as 128^2 and 64^2
//
// Revision 1.294  2002/08/07 20:06:16  ingmar
// fixed mouse scroll 2D initiating 3D slice shadow redraw
//
// Revision 1.293  2002/08/07 17:19:56  ingmar
// removed redundant redraw events
//
// Revision 1.292  2002/08/07 15:39:31  ingmar
// ensured that the image size is only increased if we are still below display resolution
//
// Revision 1.291  2002/08/07 15:29:55  michael
// Removed debug information from log file (current rendering level)
//
// Revision 1.290  2002/08/07 15:11:54  ingmar
// fixed RunBenchMark() to deal with new timer events
//
// Revision 1.289  2002/08/07 14:37:54  michael
// Added progress bar wth different colors for each level
//
// Revision 1.288  2002/08/07 12:33:27  ingmar
// turned tags on top off by default
//
// Revision 1.287  2002/08/06 23:50:00  ingmar
// added TagsOnTop render mode.
// Currently set on/off per RenderMode::ShowPatientInfo,
// as we do not yet have TagsOnTop in the RenderMode settings
//
// Revision 1.286  2002/08/06 23:04:24  michael
// added m_fRenderProgressCompleteFraction
// update it after each scan line of adaptive sampling tiles
// display it in upper right corner as part of DrawText()
//
// Revision 1.285  2002/08/06 21:36:35  michael
// No more busy mouse cursor while rendering and fixed some incremental redraw
// that wasn't done when enabling e.g. on thin slabs.
//
// Revision 1.284  2002/08/06 15:40:40  ingmar
// now killing background render thread in the destructor before releasing the transfer function memory
//
// Revision 1.283  2002/08/06 15:04:13  ingmar
// use of new vxEventTimer
// now only initiating resolution changes inside the renderer
// now also only increasing resolution up to the resolution of the current viewport
//
// Revision 1.282  2002/08/06 14:49:15  ingmar
// use of new vxEventTimer
//
// Revision 1.281  2002/08/06 14:06:44  ingmar
// use of new vxEventTimer
//
// Revision 1.280  2002/08/06 13:31:22  ingmar
// code beautify
// reenabled two pictures for showing ray sampling patern
//
// Revision 1.279  2002/08/06 13:24:25  ingmar
// initial version of MIP rendering
// fixed slice shadow setup
//
// Revision 1.278  2002/08/05 21:14:19  ingmar
// completed preemptive termination of progrsesive rendering in case that something changed in the environment
//
// Revision 1.277  2002/08/05 17:53:57  ingmar
// reduced progressive refinement rendering to one thread
//
// Revision 1.276  2002/08/05 15:05:13  ingmar
// added preemptive termination of progresive rendering in case that something changed in the environment
//
// Revision 1.275  2002/08/02 21:13:41  ingmar
// bck to linear interpolation upon display of the textures
//
// Revision 1.274  2002/08/02 20:27:47  ingmar
// final (?) adaptive sampling solution: now anything lrger than 1 pixel diameter will not be missed.
// Vessels of only 1 pixel width may be missed sometimes. This reduced the framerate by 3% (giuaranteeing to see everything would cost 25%()
//
// Revision 1.273  2002/08/02 14:55:05  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.272  2002/08/01 22:18:48  ingmar
// improved vessel tracking as part of adaptive sampling
//
// Revision 1.271  2002/07/31 18:24:31  ingmar
// added computation of one scan line+column at the border ov the image to reduce vessel popping further (1% performance loss)
//
// Revision 1.270  2002/07/31 18:05:54  ingmar
// added computation of one scan line+column at the border ov the image to reduce vessel popping further (1% performance loss)
//
// Revision 1.269  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.268  2002/07/30 20:22:06  ingmar
// made allowed color difference slightly larger
//
// Revision 1.267  2002/07/30 18:52:44  ingmar
// completed bi-directional dilate in image space of need to send rays -> 2% speed loss, but fewer CTA arteries popping
//
// Revision 1.266  2002/07/30 15:37:37  uday
// Modified diagonal length.
//
// Revision 1.265  2002/07/29 21:28:37  ingmar
// added directional dilate in image space of need to send rays -> 2% speed loss, but fewer CTA arteries popping
//
// Revision 1.264  2002/07/29 17:27:20  ingmar
// added inverted gradient shading
//
// Revision 1.263  2002/07/29 16:57:18  ingmar
// completed code cleanup, all major sections of CastSingleRay now have their own inline function
//
// Revision 1.262  2002/07/29 16:34:19  uday
// Update DrawTexture() for non square viewports.
//
// Revision 1.261  2002/07/29 15:50:39  ingmar
// moved accumulation colors into args array
//
// Revision 1.260  2002/07/29 15:31:22  ingmar
// code cleanup, new inline functions for the major parts of empty space skipping in SampleSingleRay()
//
// Revision 1.259  2002/07/29 14:09:57  ingmar
// removed obsolete bTryToSpaceLeap
//
// Revision 1.258  2002/07/29 14:00:18  ingmar
// added test for reasonable volume size before attempting to render an image
//
// Revision 1.257  2002/07/26 22:19:59  ingmar
// removed modifying TFunc upon initialization
//
// Revision 1.256  2002/07/26 21:00:36  ingmar
// removed jittering (no longer needed, as preintegration is doing a better job)
//
// Revision 1.255  2002/07/26 20:59:02  ingmar
// set first sample of preintegration sequence to 0 -> no difference in image quality but 2% faster
//
// Revision 1.254  2002/07/26 20:41:42  ingmar
// removed update and use of 1D transfer function (is less quality and less speed than 2D preintegrated TFunc)
//
// Revision 1.253  2002/07/26 20:19:15  ingmar
// removed threshold based interpolation weight computation
//
// Revision 1.252  2002/07/26 20:07:35  ingmar
// resolved remaining preintegrated TFunc issues.
// There are now two working ways to compute weights to relocate the gradient backwards inside the integration/sample intervat to compute it right where the region surface is
// The sampling debufg view shows clearly how this works
// Further, the empty opacity skipping is being extended after every 16 tries if the conditions still hold
// also, the sampling inside the crutial zone (opacity > 0 and label on) is beig extendend until the zone is finished of the ray is opaque
//
// Revision 1.251  2002/07/26 16:44:00  ingmar
// added moreinfo to the space leap paters image
// fimplified the use of the call to draw the image
// added nonlinear stretch of the section close to the end of the ray
// temporarely back to lnearest neighbor interpolation
//
// Revision 1.250  2002/07/25 22:25:09  ingmar
// pulled Benchmark code out of Render() into seperate function RunBenchmark()
// cleaned up frame rate counter and display
//
// Revision 1.249  2002/07/25 21:54:54  ingmar
// no longer writing 2DTFunc to disc
//
// Revision 1.248  2002/07/25 21:34:49  ingmar
// made preintegrated TFunc the default
// added preintegrated gradient interpolation threshold to the 2D TFunc
// Removed GetEnvironment().GEtComponentPtr() from SampleSingleRay() -> now uses pComponent in Args
//
// Revision 1.247  2002/07/25 15:45:39  ingmar
// added wait cursor during rendering and started preintegrated gradient weight computation
//
// Revision 1.246  2002/07/24 23:33:28  ingmar
// set the preintegration interval from [i,j] to (i,j], to avoid duplicate consideration of the first/last element of the interval
//
// Revision 1.245  2002/07/24 20:36:01  ingmar
// removed call to ftoa() and gained 2.5% speed
//
// Revision 1.244  2002/07/24 20:32:09  ingmar
// code beautify
//
// Revision 1.243  2002/07/24 19:16:42  ingmar
// no longer writing TFunc image after each frame -> back to full speed
//
// Revision 1.242  2002/07/24 15:49:50  ingmar
// proper generateion and use of pre-integrated TFunc
//
// Revision 1.241  2002/07/23 21:04:41  ingmar
// pulled the alpha pre-multiplication into the TFubc update -> 5% speedup
//
// Revision 1.240  2002/07/23 20:40:50  ingmar
// clarified and fixed proper use of alpha-pre-multiplied colors -> better image but lost 25% speed
//
// Revision 1.239  2002/07/23 20:16:07  ingmar
// coding standard compliant variable naming
// and proper blending of slice and indensity range shadows
//
// Revision 1.238  2002/07/23 14:06:11  ingmar
// made sure the complete integral is computed (one step was missing)
//
// Revision 1.237  2002/07/23 00:05:18  ingmar
// cleaned up preintegrated table computation
//
// Revision 1.236  2002/07/22 21:31:19  ingmar
// variable naming
// use of m_pfColormap1D to generate m_pfColormapPreIntegrated2D
// TFunc image file now consideres changed alpha of TFunc1D
//
// Revision 1.235  2002/07/22 21:13:56  ingmar
// added alpha correction for 1D tFunc
//
// Revision 1.234  2002/07/22 20:46:04  ingmar
// now with conversion from alpha-premultiplied colors back to plain RGBA at end of ray sampling
//
// Revision 1.233  2002/07/22 20:24:19  ingmar
// variable naming
//
// Revision 1.232  2002/07/22 20:05:09  ingmar
// variable naming
// removal of obsolete args.iDepth
//
// Revision 1.231  2002/07/22 18:40:27  ingmar
// improved comments and changed order of computation
//
// Revision 1.230  2002/07/22 18:30:25  ingmar
// better display of the plain and integrated TFunc images on disk
//
// Revision 1.229  2002/07/22 18:27:51  ingmar
// better display of the plain and integrated TFunc images on disk
//
// Revision 1.228  2002/07/22 18:23:11  ingmar
// better display of the plain and integrated TFunc images on disk
//
// Revision 1.227  2002/07/18 21:55:28  ingmar
// fixed gradient magnitude multiplyer and added new GRadient color mode (both disabled for now)
//
// Revision 1.226  2002/07/18 16:42:38  ingmar
// moved 4^3 binary histogram to the environment
//
// Revision 1.225  2002/07/17 22:50:42  ingmar
// more compact version, same speed
//
// Revision 1.224  2002/07/17 21:37:20  ingmar
// made intensity histogram generation 2x faster (work in progress)
//
// Revision 1.223  2002/07/17 20:28:05  ingmar
// made intensity histogram generation 4x faster (work in progress)
//
// Revision 1.222  2002/07/17 17:30:52  ingmar
// added jittering, but disabled it for now
//
// Revision 1.221  2002/07/16 22:41:44  ingmar
// made preintegrated TF the default as it is 7.5% faster -> now 4.3fps for 512^3
//
// Revision 1.220  2002/07/16 22:28:24  ingmar
// no longer writing out preintegrated table to disk
//
// Revision 1.219  2002/07/16 22:22:10  ingmar
// com[pleted preintegrated classification, it needs to have its own entry in the args structure
//
// Revision 1.218  2002/07/16 21:18:50  ingmar
// made 0.5 sampling density the default and adjusted the preintegrated TF accordingly
//
// Revision 1.217  2002/07/16 20:48:09  ingmar
// now with toggle between preintegrated and normal TF and with color from gradients
//
// Revision 1.216  2002/07/16 16:30:51  ingmar
// reenabled preintegraded transfer function, now with 0.5 intersample distance as the default that does not modify the alpha values
//
// Revision 1.215  2002/07/16 15:37:14  ingmar
// removed iso-value sampling as WL with W=0 is just as fast
//
// Revision 1.214  2002/07/15 23:40:27  ingmar
// cleanec up parameters for slice shadow mode
//
// Revision 1.213  2002/07/15 23:19:56  ingmar
// In slice shadow mode set rendering necessarrry only if significant paramwters have changed
//
// Revision 1.212  2002/07/15 22:57:14  ingmar
// added slice shadows
//
// Revision 1.211  2002/07/15 20:44:30  ingmar
// added show intensity range (frame rate 3.8)
//
// Revision 1.210  2002/07/12 23:45:27  ingmar
// now always sending a pack of up to four samples before reattempting to spaceleap -> 4.1fps
//
// Revision 1.209  2002/07/12 23:18:15  ingmar
// avoided calling ftoa() for samples -> 3.8fps
//
// Revision 1.208  2002/07/12 23:01:53  ingmar
// fixed image quality, current framerate 512^3 256^2 => 3.6 (now 0.5 sampling rate instead of 1.0 and get sample instead of get voxel)
//
// Revision 1.207  2002/07/12 21:21:00  ingmar
// fixed space leaoing, current framerate 512^3 256^2 => 5.3
//
// Revision 1.205  2002/07/11 23:58:00  ingmar
// rev-enabled frane rate benchmark
//
// Revision 1.204  2002/07/11 23:36:53  ingmar
// rev-enabled frane rate benchmark
//
// Revision 1.203  2002/07/11 23:17:38  ingmar
// changed adjustment of image size arccording to framrate
//
// Revision 1.202  2002/07/11 22:56:28  ingmar
// cosmetics
//
// Revision 1.201  2002/07/11 21:33:03  ingmar
// worked on progressive refinement
//
// Revision 1.200  2002/07/11 20:56:54  ingmar
// worked on progressive refinement
//
// Revision 1.199  2002/07/11 16:35:09  ingmar
// added timer and events for QualitySpeedTradeoff to maintain 5 to 15 fps
//
// Revision 1.198  2002/07/10 19:03:10  ingmar
// fixed Camera / OpenGL setup to draw the image where OpenGL expects it. This now produces the correct effect even after scaling. Thanks Uday
//
// Revision 1.197  2002/07/10 18:46:27  ingmar
// added m_bIsViewChangeing to se if we need to adjust the image size
// set the base sampling rate to 0.5 to avoid black dot artifacts
//
// Revision 1.196  2002/07/10 17:39:19  ingmar
// fixed space leap pattern drawing
// added colormap on debug image
//
// Revision 1.195  2002/07/10 14:01:21  ingmar
// finetuned sampling parameters
//
// Revision 1.194  2002/07/09 22:27:45  ingmar
// consolidated RayIterLable advances
// added non-visible voxel skipping
//
// Revision 1.193  2002/07/09 20:57:49  ingmar
// added 4^3 intensity histogram and adjusted rendering to
// first spaceleap in 16^3 SLD and than to skipp over 4^3 blocks not "seen" by the transfer function
//
// Revision 1.192  2002/07/09 17:11:10  ingmar
// changed Sld block width to 16
// several cosmetic improvements
//
// Revision 1.191  2002/07/08 22:58:31  ingmar
// improved UpdateLabelBinHistoVol
//
// Revision 1.190  2002/07/08 20:51:34  ingmar
// improved speed of label histogram generation by checking of homogenous flag
//
// Revision 1.189  2002/07/08 20:35:08  ingmar
// seperated the histogram information into two volumes, one for intensity and one for label, each now with 32 bits
//
// Revision 1.188  2002/07/08 15:34:28  ingmar
// added UpdateTextFields() and added comments
//
// Revision 1.187  2002/07/07 23:32:10  ingmar
// finished moveable light. It now follows the mouse
//
// Revision 1.186  2002/07/05 21:44:16  ingmar
// the moveable light now rorates (but only properly as long as the object is not rotated)
//
// Revision 1.185  2002/07/05 20:42:41  ingmar
// removed depth buffer from adaptive sampling code (this was the only place where it was used, so no depth image was created before)
//
// Revision 1.184  2002/07/05 20:06:42  ingmar
// more compact representation of the offsets
// middle point now interpolated diagonally (made 25% speed improvement)
// similarity test and interpolation now from same set of pixels
// removed unused depth from similarity test
//
// Revision 1.183  2002/07/05 15:58:26  ingmar
// fixed typo in adaptive sampling IsSimilar() calls => now bloodvesseld no longer pop
//
// Revision 1.182  2002/07/03 15:08:36  soeren
// removed on wait in the 4 threads version
//
// Revision 1.181  2002/07/01 23:24:45  soeren
// made more use of bounding box
//
// Revision 1.180  2002/07/01 16:14:38  soeren
// removed old bounding box update
//
// Revision 1.179  2002/07/01 15:53:14  soeren
// UpdateBoundingBox uses component visible information now.
//
// Revision 1.178  2002/06/28 16:31:40  soeren
// activated boundding box in UpdateRenderLabelVolume - good speed up with the right transferfunction
//
// Revision 1.177  2002/06/28 16:08:51  soeren
// beautified log in UpdateBoundingBox
//
// Revision 1.176  2002/06/27 20:59:31  soeren
// switch to new Bounding Box (it looks like it is a little bit slower ???)
//
// Revision 1.175  2002/06/27 20:03:48  soeren
// added new very fast updateBoundingBox implementation, needs more testing hence it is disabled.
//
// Revision 1.174  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.173  2002/06/27 16:39:19  soeren
// renamed calculate BoundingBox, removed waiting on Texture generation and added more timings
//
// Revision 1.172  2002/06/27 14:19:02  soeren
// finished uday change of HitBoundingBox
//
// Revision 1.171  2002/06/26 22:09:14  uday
// Using HitBoundingBox() in vxUtils.
//
// Revision 1.170  2002/06/25 21:58:45  soeren
// renamed Average Framerate: to Benchmark Framerate:
//
// Revision 1.169  2002/06/25 21:54:50  soeren
// removed old setting of argument list
//
// Revision 1.168  2002/06/25 21:53:47  soeren
// Added SetCommonRenderArguments() function, rendering is now slightly faster
//
// Revision 1.167  2002/06/25 19:33:02  soeren
// added initial version of moveable light
//
// Revision 1.166  2002/06/24 17:12:12  soeren
// cleaned up OpenGLfunctions
//
// Revision 1.165  2002/06/24 17:07:48  soeren
// removed addtitional display of Window Level
//
// Revision 1.164  2002/06/24 16:29:04  soeren
// removed unnecessary code
//
// Revision 1.163  2002/06/21 23:30:08  ingmar
// added Gradient Magnitude Modulation, but disabled it for now
//
// Revision 1.162  2002/06/21 21:44:11  ingmar
// re-enable multi-threaded mode, moved Illuminator setup to Initialize()
//
// Revision 1.161  2002/06/21 21:26:38  ingmar
// cleaned up ShadePhong Diffuse() / Specular()
//
// Revision 1.160  2002/06/21 20:52:46  ingmar
// fixed label color blending
//
// Revision 1.159  2002/06/21 17:19:34  ingmar
// back to direct computation of the lights
//
// Revision 1.158  2002/06/21 17:09:08  ingmar
// seperated ShadePhongTwoLights() from ShadePhongReflectanceMap()
//
// Revision 1.157  2002/06/20 21:06:20  ingmar
// 20 is optimal size for specular map
//
// Revision 1.156  2002/06/20 20:27:28  ingmar
// activated specular again
//
// Revision 1.155  2002/06/20 20:10:18  ingmar
// cleaned up all functions called by PreRender()
//
// Revision 1.154  2002/06/20 20:05:14  soeren
// disbaled temporaly specular highlight
//
// Revision 1.153  2002/06/20 18:39:53  soeren
// extended Illuminator to have n lights instead of two
//
// Revision 1.152  2002/06/20 16:41:38  soeren
// activated specular map
//
// Revision 1.151  2002/06/20 15:40:07  soeren
// added intial version of specular refelction map
//
// Revision 1.150  2002/06/20 15:23:30  ingmar
// moved private functions to the bottom of the file
//
// Revision 1.149  2002/06/20 14:52:53  soeren
// switch back to transferfunction rendering
//
// Revision 1.148  2002/06/19 23:35:44  ingmar
// the optimal size for the diffuse lookup table is 12x12 when interpolation is used
//
// Revision 1.147  2002/06/19 23:09:16  ingmar
// no longer writing out the debug image
//
// Revision 1.146  2002/06/19 21:54:47  ingmar
// PreRender now consideres the change of the label volume
// the reflectance map now gets written to disk (every frame)
//
// Revision 1.145  2002/06/19 21:36:02  soeren
// added more comments and removed unnecessary functions
//
// Revision 1.144  2002/06/18 21:38:24  soeren
// first step in fixing frame rate
//
// Revision 1.143  2002/06/18 21:31:55  soeren
// removed obsolete ShadeTwoLights function
//
// Revision 1.142  2002/06/18 21:29:17  soeren
// added additional illuminator
//
// Revision 1.141  2002/06/18 20:35:03  ingmar
// back to transfer function use
//
// Revision 1.140  2002/06/18 20:30:48  ingmar
// no longer setting component 0xFFFF to invisible, explicit test for this value instead
//
// Revision 1.139  2002/06/18 15:45:43  ingmar
// added anisotropic gradient correction
//
// Revision 1.138  2002/06/17 17:08:42  soeren
// fixed UpdateLabelRenderVolume
//
// Revision 1.137  2002/06/17 15:34:12  soeren
// added UpdateLabelRenderVolume
//
// Revision 1.136  2002/06/14 22:21:53  ingmar
// now using Sobel Filter for gradients
//
// Revision 1.135  2002/06/14 21:56:06  soeren
// added global Framerate timer
//
// Revision 1.134  2002/06/14 13:40:30  ingmar
// moved the image plane closser to the volume to allow debug images of large volumes
// also replaced #defines with const typename to allow typechecking by the compiler
//
// Revision 1.133  2002/06/13 22:17:48  ingmar
// fixed UH display location and repeat (not yet value)
//
// Revision 1.132  2002/06/13 22:01:31  ingmar
// new SetImageSize() function
// it does consider vxRenderMOde::VIEW_CHANGING for progressive refinement
//
// Revision 1.131  2002/06/13 21:04:05  ingmar
// now using rayIter->GetDistanceToEndOfABlock()
// also ray sampling patters code fixes
//
// Revision 1.130  2002/06/13 00:10:34  ingmar
// made intersampleDistyance adjustable (in sync with image res)
//
// Revision 1.129  2002/06/12 23:33:04  ingmar
// now the third quality slider controls the image size
//
// Revision 1.128  2002/06/12 20:35:42  ingmar
// now can move sampling pattern line
//
// Revision 1.127  2002/06/12 20:03:10  ingmar
// cleaned up debug image display
//
// Revision 1.126  2002/06/12 15:08:17  ingmar
// towards seperate DrawAdaptiveSamplingPatternPixel()
//
// Revision 1.125  2002/06/12 14:46:50  soeren
// may optimized updated of transferfunction
//
// Revision 1.124  2002/06/12 14:27:59  soeren
// removed unnecessary code
//
// Revision 1.123  2002/06/11 23:08:10  ingmar
// improved DrawSpaceLeapPatternLine()
//
// Revision 1.122  2002/06/11 18:16:46  soeren
// fixed multithreading related bug
//
// Revision 1.121  2002/06/11 18:08:15  ingmar
// Draw the text fields now shows Image and volume dimension
//
// Revision 1.120  2002/06/11 17:31:04  ingmar
// reenabled invisible intensity space leaping
//
// Revision 1.119  2002/06/11 16:32:55  ingmar
// reenabled conservative space leaping
//
// Revision 1.118  2002/06/11 16:32:25  soeren
// added try catch around Render and Post Render function
//
// Revision 1.117  2002/06/11 15:09:34  ingmar
// added DrawSpaceLeapVolume()
//
// Revision 1.116  2002/06/11 14:18:17  soeren
// fixed preintegrated table
//
// Revision 1.115  2002/06/10 22:10:08  ingmar
// improved light settings
//
// Revision 1.114  2002/06/10 21:11:15  soeren
// clean up
//
// Revision 1.113  2002/06/10 20:42:43  soeren
// Cleaned up code
//
// Revision 1.112  2002/06/10 18:16:10  ingmar
// improved space leaping (no more black dots)
//
// Revision 1.111  2002/06/07 23:16:55  ingmar
// added zooming and enabled dual CPUs
//
// Revision 1.110  2002/06/06 22:39:20  ingmar
// towards fewer function parameters
//
// Revision 1.109  2002/06/06 18:21:53  ingmar
// 0.4 extra fps from removing function call arguments
//
// Revision 1.108  2002/06/06 18:06:09  ingmar
// changed the colors used in DrawSpaceLeapPatternLine() to an array
//
// Revision 1.107  2002/06/06 17:58:06  soeren
// renamed boom to a real error message
//
// Revision 1.106  2002/06/06 17:51:20  ingmar
// added throw infront of exceptions
//
// Revision 1.105  2002/06/06 17:31:08  ingmar
// added call to EnsureTextureGenerationIsCompleted()
//
// Revision 1.104  2002/06/06 17:22:33  soeren
// added some try catches
//
// Revision 1.103  2002/06/06 14:39:24  ingmar
// improved invisible density traversal
//
// Revision 1.102  2002/06/06 00:50:56  soeren
// optimzed label computation
//
// Revision 1.101  2002/06/05 23:48:56  soeren
// changed to iso rendering
//
// Revision 1.100  2002/06/05 23:15:03  ingmar
// towards leaping to the end of the last empty block
//
// Revision 1.99  2002/06/05 22:46:56  soeren
// added SetVolumeBoundaryOfLabelVolumeToMaxLabel()
// reduced ray termination condition to only early ray termination, no more Volume boundary checks needed
//
// Revision 1.98  2002/06/04 22:51:29  ingmar
// made line visible at which the space leap info is drawn
//
// Revision 1.97  2002/06/04 19:36:24  soeren
// added try and catch arounf updates
//
// Revision 1.96  2002/06/04 19:33:05  soeren
// fixed yellow stuff
//
// Revision 1.95  2002/06/04 15:51:40  soeren
// improved GetSampleGradientCentralDifference
//
// Revision 1.94  2002/06/04 15:01:56  soeren
// renamed GetSampleGradient to GetSampleCentraldifferencegradient
//
// Revision 1.93  2002/06/04 13:03:44  soeren
// Improved update of final Texture image through glSubimage2D
//
// Revision 1.92  2002/06/03 19:05:33  soeren
// removed old unnused code
//
// Revision 1.91  2002/06/03 16:46:47  soeren
// fixed 12 bit transferfuntion access
//
// Revision 1.90  2002/06/03 15:56:09  soeren
// beautified
//
// Revision 1.89  2002/06/03 15:38:49  soeren
// Beautified
//
// Revision 1.88  2002/06/02 21:36:33  soeren
// disabled debug info and enabled mutlithreading
//
// Revision 1.87  2002/06/02 21:26:04  soeren
// changed scaling in drawspaceleap pattern
//
// Revision 1.86  2002/06/02 21:25:35  soeren
// changed scaling in drawspaceleap pattern
//
// Revision 1.85  2002/06/02 21:04:49  soeren
// changed temporarly to 8Bit Lut, now image is much nicer:-)
//
// Revision 1.84  2002/06/02 19:38:23  soeren
// just beautified and switch back to isosurface rendering
//
// Revision 1.83  2002/06/02 19:10:01  soeren
// added some uncommented function
//
// Revision 1.82  2002/06/02 18:59:54  soeren
// fixed new gradientTrilinear function
//
// Revision 1.81  2002/06/02 17:07:35  soeren
// initial version of GetGradientTrilinear()
//
// Revision 1.80  2002/06/02 14:14:14  soeren
// disable space leaping
//
// Revision 1.79  2002/06/02 14:12:14  soeren
// scaled spaceleappatternline - disabled adaptive sampling
//
// Revision 1.78  2002/05/31 00:48:40  soeren
// clean up the code and worked in ShowSpaceLeopPattern
//
// Revision 1.77  2002/05/30 19:15:28  soeren
// moved spaceleapIterator to RenderWindow
//
// Revision 1.76  2002/05/30 17:02:05  soeren
// added space leaping though turned off components
//
// Revision 1.75  2002/05/30 15:00:31  soeren
// changed parameters of SampleSingleRay
//
// Revision 1.74  2002/05/30 13:11:39  soeren
// changed to sampling pattern pointer to real array
//
// Revision 1.73  2002/05/30 01:18:26  ingmar
// fixed scaling of debug info
//
// Revision 1.72  2002/05/30 01:12:07  ingmar
// beautified colors
//
// Revision 1.71  2002/05/30 01:08:42  ingmar
// beautified colors
//
// Revision 1.70  2002/05/30 00:57:24  ingmar
// changed 255 to 1.0 in SampleSingleRay
//
// Revision 1.69  2002/05/29 22:39:47  ingmar
// finished drawSpaceleapPatern
//
// Revision 1.68  2002/05/29 19:49:13  soeren
// added more space leap debug information and enabled broken space leaping
//
// Revision 1.67  2002/05/29 19:05:20  uday
// Display space leap inforamtion in a slice.
//
// Revision 1.66  2002/05/28 21:53:58  soeren
// added new spaceleap method
//
// Revision 1.65  2002/05/28 14:25:32  ingmar
// made abel Volume iterator a dlc_pointer and flipped specular light to above the dataset
//
// Revision 1.64  2002/05/24 20:07:57  ingmar
// added support for component coloring
//
// Revision 1.63  2002/05/13 17:45:52  ingmar
// removed not needed include of "ImgProcUtils.h"
//
// Revision 1.62  2002/05/10 15:09:50  soeren
// added vol dims to timings
//
// Revision 1.61  2002/05/10 15:03:10  soeren
// added timings
//
// Revision 1.60  2002/05/10 12:44:11  soeren
// disabled space leaping
//
// Revision 1.59  2002/05/08 15:49:49  soeren
// just more readable
//
// Revision 1.58  2002/05/08 15:42:11  soeren
// fixed transferfunction
//
// Revision 1.57  2002/05/08 15:03:51  soeren
// removed & 0x0FFF
//
// Revision 1.56  2002/05/08 14:09:07  soeren
// fixed calling updatetf 4 times
//
// Revision 1.55  2002/05/07 23:49:43  soeren
// moved setting of render Volume vol dims
//
// Revision 1.54  2002/05/07 23:41:56  soeren
// added timer to UpdateEnvAccordingToNewTFunc
//
// Revision 1.53  2002/05/07 22:57:32  soeren
// added timer to calculate boundingbox
//
// Revision 1.52  2002/05/07 22:52:08  soeren
// fixed spaceleap bug
//
// Revision 1.51  2002/05/07 21:58:53  soeren
// removed old linVolume code
//
// Revision 1.50  2002/05/07 21:25:14  dmitry
// Magically changing WindowLevel.
//
// Revision 1.49  2002/05/07 21:17:36  soeren
// fixed tranferfunction
//
// Revision 1.48  2002/05/07 18:35:59  frank
// Removed unneccessary includes.
//
// Revision 1.47  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block Volume. God help us!
//
// Revision 1.46  2002/05/01 22:36:17  michael
// replaced VolumeSET and VolumeLABELSET by Volume_{INTENSITY,LABEL}
//
// Revision 1.45  2002/05/01 21:13:36  jenny
// no m_OriginalBlkVol, Volumeset
//
// Revision 1.44  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.43  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.42  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 1.41  2002/04/21 21:59:08  michael
// Re-fixed window level AGAIN since checked in code does not work
//
// Revision 1.40  2002/04/18 20:46:16  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 1.39  2002/04/16 21:41:02  geconomos
// Fixed soeren's and ingmar's compile bug.
//
// Revision 1.38  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.37  2002/04/10 19:00:25  ingmar
// worked on new header Volume class
//
// Revision 1.36  2002/04/10 18:08:15  ingmar
// worked on new header Volume class
//
// Revision 1.35  2002/04/09 00:36:19  ingmar
// slightly faster
//
// Revision 1.34  2002/04/08 23:56:10  ingmar
// working space leaping with distance in top 4 bits
//
// Revision 1.33  2002/04/08 21:49:27  ingmar
// new spaceleap computation
//
// Revision 1.32  2002/04/08 17:43:38  michael
// removed intermediate timer class
//
// Revision 1.31  2002/04/05 22:03:05  ingmar
// set new default W/L for ankle dataset
//
// Revision 1.30  2002/04/05 19:12:37  ingmar
// fixed space leap info computation
//
// Revision 1.29  2002/04/05 15:34:56  ingmar
// added new Peek function
//
// Revision 1.28  2002/04/05 15:15:12  ingmar
// activated new space leap function (still broken)
//
// Revision 1.27  2002/04/04 21:27:02  ingmar
// now using the new Timer
//
// Revision 1.26  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.25  2002/04/04 14:05:20  ingmar
// changed all point to vector3D and switch to new BlockVolume
//
// Revision 1.24  2002/04/03 18:01:23  ingmar
// made Vec3Dfloat4 templetized and renamed to Vector3D
//
// Revision 1.23  2002/04/03 15:38:11  ingmar
// now using Vec3Dfloat4 m_iCropBoxDataBlocks16Min/Max
//
// Revision 1.22  2002/04/03 01:19:49  ingmar
// worked in getting the render Volume right
//
// Revision 1.21  2002/04/02 22:45:37  ingmar
// several changes to make RecBlkVolume rendetring work (now same speed as linear)
// and added m_iIsoValue instead of GEtEnv... -> +10% speed
//
// Revision 1.20  2002/04/02 20:08:16  ingmar
// added RecBlkVol option
// fixed adaptive sampling on outside border
// moved adaptive sampling test image from right to top
//
// Revision 1.19  2002/04/01 22:30:14  ingmar
// added Isovalue adaptive space leap info computation
//
// Revision 1.18  2002/04/01 22:04:34  michael
// only one location does the resetting of the camera parameters (later on pop)
//
// Revision 1.17  2002/04/01 20:16:08  ingmar
// now using BoundingBox test instead of Volume dim test
//
// Revision 1.16  2002/04/01 19:22:18  ingmar
// nicer display of frame rate
//
// Revision 1.15  2002/04/01 19:09:01  ingmar
// fixed multy threading image generation
//
// Revision 1.14  2002/04/01 16:09:36  ingmar
// searching for multy thread race condition
//
// Revision 1.13  2002/04/01 15:37:26  ingmar
// added UpdateBoundingBox()
// fixed multi thread image subdivision
// added reproducable 360 degree spin to measure frame rate
//
// Revision 1.12  2002/03/29 23:48:25  ingmar
// first step to enableing multithreading in CPU raycaster
//
// Revision 1.11  2002/03/29 01:28:36  ingmar
// added RenderQualitySpeedTradeoff to environment to set the RenderQualitySpeedTradeoff
//
// Revision 1.10  2002/03/28 23:13:00  ingmar
// code cleanup and more comments
//
// Revision 1.9  2002/03/28 22:44:05  ingmar
// set W/L display to HU
//
// Revision 1.8  2002/03/28 21:00:10  ingmar
// added DrawText()
//
// Revision 1.7  2002/03/28 19:09:14  ingmar
// flipped texture
//
// Revision 1.6  2002/03/28 18:55:23  ingmar
// code cleanup and defaults for single large image
//
// Revision 1.5  2002/03/27 22:43:21  ingmar
// more cleanup and slickedit formating
//
// Revision 1.4  2002/03/27 22:24:15  ingmar
// zoomed image to full viewport and removed unneeded functions
//
// Revision 1.3  2002/03/27 21:37:06  ingmar
// added Initialize() function to resize the image
// chaged DrawTexture() to get some sort of image, need to fix transformation matrix
//
// Revision 1.2  2002/03/27 19:57:14  ingmar
// transfered lots of code from AAA CPU renderer, no image yet
//
// Revision 1.1  2002/03/27 16:24:45  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererCpu3D.C,v 1.54 2007/02/13 22:32:27 romy Exp $
// $Id: vxRendererCpu3D.C,v 1.54 2007/02/13 22:32:27 romy Exp $
