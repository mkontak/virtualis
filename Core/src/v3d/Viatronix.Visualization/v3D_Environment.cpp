// $Id: v3D_Environment.cpp,v 1.52.2.1 2009/07/29 19:54:06 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Environment.h"
#include "vxManagedObserver.h"
#include "v3D_Preset.h"
#include "v3D_ViewObliqueData.h"
#include "v3D_ModifiedQueue.h"
#include "v3D_ID.h"
#include "v3D_VisualizationPreset.h"
#include "vxWindowHandle.h"
#include "vxSerializer.h"
#include "vxViewerTypeUndefined.h"
#include "vxOverlayStrings.h"
#include "vxUtils.h"
#include "vxElementBool.h"
#include "Timer.h"
#include "vxRayProfile.h"
#include "vxElementPixelRGBA.h"
#include "v3D_Elements.h"
#include "vxUndoAction.h"
#include "v3D_Dataset.h"
#include "vxiRenderingContext.h"
#include "v3D_Cine.h"
#include "v3D_Momentum.h"
#include "v3D_CameraFlySettings.h"
#include "vxCadFindings.h"
#include "v3D_CadFindings.h"
#include "v3D_VisualizationGraph.h"
#include "vxVisualizationGraph.h"


// defines
#define FILE_REVISION "$Revision: 1.52.2.1 $"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a Environment object with default values.
 */
Environment::Environment() : 
  m_mpVolume( nullptr ),
  m_mpLabelVolume( nullptr ),
  m_mpComponents( nullptr ),
  m_mpPlaneOrtho( nullptr ),
  m_mpPlaneCurved( nullptr ),
  m_mpRenderingMode( nullptr ),
  m_mpColormap( nullptr ),
  m_mpCamera( nullptr ),
  m_mpCropBox( nullptr ),
  m_mpContourList( nullptr ),
  m_mpCursor3d( nullptr ),
  m_mpAnnotations( nullptr ),
  m_mpSegmentation( nullptr ),
  m_mpPreset( nullptr ),
  m_mpDataset( nullptr ),
  m_mpMomentum( nullptr ),
  m_mpCameraFlySettings( nullptr ),
  m_mpDataRendererSoftware( nullptr ),
  m_mpCine( nullptr ),
  m_mpViewerType( nullptr ),
  m_mpCadFindings( nullptr ),
  m_mpVisualizationGraph( nullptr )
{

  // unmanaged types 
  m_pEnvironment = new vxShareableObject< vxEnvironment >();
  m_pObserver = new vxManagedObserver( this );
  m_pDefaultViewerType = new vxViewerTypeUndefined( *m_pEnvironment );
  m_elements = new std::map< std::string, gcroot< IElement ^ > >();

  m_pEnvironment->Connect( m_pObserver );

  // initialize managed environment elements
  this->ZoomFactor        = gcnew Viatronix::Visualization::ZoomFactor();    
  this->Volume            = gcnew Viatronix::Visualization::Volume( VolumeType::Intensity );    
  this->LabelVolume       = gcnew Viatronix::Visualization::Volume( VolumeType::Label );
  this->Annotations       = gcnew Viatronix::Visualization::Annotations();
  this->Colormap          = gcnew Viatronix::Visualization::Colormap();
  this->ContourList       = gcnew Viatronix::Visualization::ContourList();
  this->CropBox           = gcnew Viatronix::Visualization::CropBox();
  this->Components        = gcnew Viatronix::Visualization::Components();
  this->Cursor3d          = gcnew Viatronix::Visualization::Cursor3d();
  this->Plane             = gcnew Viatronix::Visualization::PlaneOrtho( Orientations::Axial );
  this->PlaneOblique      = gcnew Viatronix::Visualization::PlaneOblique( Orientations::Axial );
  this->PlaneCurved       = gcnew Viatronix::Visualization::PlaneCurved();
  this->Preset            = gcnew Viatronix::Visualization::Preset();
  this->RenderingMode     = gcnew Viatronix::Visualization::RenderingMode();    
  this->Segmentation      = gcnew Viatronix::Visualization::Segmentation();
  this->Camera            = gcnew Viatronix::Visualization::Camera();
  this->Cine              = gcnew Viatronix::Visualization::Cine();
  this->Momentum          = gcnew Viatronix::Visualization::Momentum();
  this->CameraFlySettings = gcnew Viatronix::Visualization::CameraFlySettings();
  this->CadFindings       = gcnew Viatronix::Visualization::CadFindings();

  // this will need to be refined!!!
  this->DataRendererSoftware = gcnew Viatronix::Visualization::DataRendererSoftware();
  
  // set up the background colors
  m_pBackgroundColor2D = new vxShareableObject< PixelRGBA<uint1> >();
  m_pBackgroundColor2D->Connect( m_pObserver );
  m_pBackgroundColor3D = new vxShareableObject< PixelRGBA<uint1> >();
  m_pBackgroundColor3D->Connect( m_pObserver );

  // default viewer type
  m_pEnvironment->SetViewerType( m_pDefaultViewerType );
  
  // shared font info
  m_pEnvironment->SetFontInfo( ToNativeFontInfoPtr( m_mpFontInfo->GetElementPtr() ) );
  m_mpFontInfo->Connect( System::IntPtr( m_pObserver ));

  // current voxel position
  m_pVoxelPosition = new vxShareableObject< Vector3D< int4 > >();
  m_pVoxelPosition->Connect( m_pObserver );
  m_pEnvironment->SetCurrentVoxelPos( m_pVoxelPosition );

  // Sculpting lasso
  m_pSculptLasso = new vxShareableObject< vxContour >();
  m_pSculptLasso->Connect( m_pObserver );
  m_pEnvironment->SetSculptContour( m_pSculptLasso );

  // overlay strings
  m_pOverlayStrings = new vxShareableObject< vxOverlayStrings >();
  m_pOverlayStrings->Connect( m_pObserver );
  m_pEnvironment->AddElement( vxIDs::OverlayStrings, reinterpret_cast< vxElement * >( m_pOverlayStrings ) );

  // progressive refinement mode
  vxShareableObject< vxElementBool > * pElementBool = new vxShareableObject< vxElementBool >( true );
  pElementBool->Connect( m_pObserver );
  m_pEnvironment->AddElement( vxIDs::ProgressiveRefinement, static_cast< vxElement * >( pElementBool ) );

  // ray profile data
  vxShareableObject< vxRayProfile > * pRayProfile( new vxShareableObject< vxRayProfile >() );
  pRayProfile->Connect( m_pObserver );
  m_pEnvironment->AddElement( vxIDs::RayProfileData, static_cast< vxElement * >( pRayProfile ) );
} // Environment()


/**
 * destructor.
 */
Environment::~Environment()
{
  DisconnectFromElement( m_mpAnnotations );
  DisconnectFromElement( m_mpCamera );
  DisconnectFromElement( m_mpColormap );
  DisconnectFromElement( m_mpComponents );
  DisconnectFromElement( m_mpContourList );
  DisconnectFromElement( m_mpCropBox );
  DisconnectFromElement( m_mpCursor3d );
  DisconnectFromElement( m_mpLabelVolume );
  DisconnectFromElement( m_mpPlaneOrtho );
  DisconnectFromElement( m_mpPlaneOblique );
  DisconnectFromElement( m_mpPlaneCurved );
  DisconnectFromElement( m_mpRenderingMode );
  DisconnectFromElement( m_mpSegmentation );
  DisconnectFromElement( m_mpVolume );
  DisconnectFromElement( m_mpDataRendererSoftware );
  DisconnectFromElement( m_mpFontInfo );
  DisconnectFromElement( m_mpMomentum );
  DisconnectFromElement( m_mpCadFindings );

  this->!Environment();
} // ~Environment()


/**
 * Finalizer.
 */
Environment::!Environment()
{
  m_pBackgroundColor2D->Disconnect( m_pObserver );
  m_pBackgroundColor3D->Disconnect( m_pObserver );
  m_pVoxelPosition->Disconnect( m_pObserver );
  m_pSculptLasso->Disconnect( m_pObserver );
  m_pOverlayStrings->Disconnect( m_pObserver );    
  
  m_pEnvironment->Disconnect( m_pObserver );
  
  delete m_pVoxelPosition;     m_pVoxelPosition = NULL;
  delete m_pSculptLasso;       m_pSculptLasso = NULL;
  delete m_pOverlayStrings;    m_pOverlayStrings = NULL;
  delete m_pObserver;         m_pObserver = NULL;
  try
  {
    delete m_pDefaultViewerType; m_pDefaultViewerType = NULL;
  }
  catch ( ... )
  {
    LogDbg( "Problem removing the default viewer type.", "Environment", "Dispose" );
  }
  delete m_pEnvironment;       m_pEnvironment = NULL;
  delete m_pBackgroundColor2D; m_pBackgroundColor2D = NULL;
  delete m_pBackgroundColor3D; m_pBackgroundColor3D = NULL;
} // !Environment()


/**
 * Begin a sequence of undo actions (must eventually be followed by an end)
 */
void Environment::UndoSequenceBegin()
{
  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeBegin() );
} // UndoSequenceBegin()


/**
 * End a sequence of undo actions (must have been preceeded by a begin)
 */
void Environment::UndoSequenceEnd()
{
  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeEnd() );
} // UndoSequenceEnd()


/**
 *  Adds an element to the environment.
 *
 * @param mpName Element's name.
 * @param mpElement Element object to add.
 */
void Environment::AddElement( System::String ^ mpName, IElement ^ mpElement )
{
  try
  {
    std::string sName = ss( mpName );

    RemoveElement( mpName );

    m_elements->insert( std::make_pair( sName, gcroot< IElement ^ >( mpElement ) ) );
    
    ConnectToElement( mpElement );
    
    mpElement->Modified += gcnew System::EventHandler( this, & Environment::OnElementModifiedHandler );

    m_pEnvironment->AddElement( sName, reinterpret_cast< vxElement * >( mpElement->GetElementPtr().ToPointer() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AddElement( mpName, mpElement )


/**
 * Removes the specified element from environment.
 *
 * @param mpName Name of element to remove.
 */
void Environment::RemoveElement( System::String ^ mpName )
{
  try
  {
    std::string sName = ss( mpName );
    
    // find the element in the map
    Elements::iterator iter = m_elements->find( sName );
    if ( iter != m_elements->end() )
    {
      m_pEnvironment->RemoveElement( sName );
      
      DisconnectFromElement( iter->second );
      
      iter->second->Modified -= gcnew System::EventHandler( this, & Environment::OnElementModifiedHandler );

      m_elements->erase( sName );
    }
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveElement( mpName )


/** 
 * Removes all extended elements from the collection.
 */
void Environment::RemoveAllElements()
{
  try
  {
    // Remove all elements from the
    for ( Elements::iterator iter = m_elements->begin(); iter != m_elements->end(); ++iter ) 
    {
      m_pEnvironment->RemoveElement( iter->first );
      DisconnectFromElement( iter->second );
    }
    m_elements->clear();

    // Add dynamic elements
    AddElement( ElementID::ViewObliqueData, gcnew Viatronix::Visualization::ViewObliqueData() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveAllElements()


/** 
 * Returns the specified element.
 *
 * @param mpName Name of element to return.
 * @return Element object; NULL if not found.
 */
IElement ^ Environment::GetElement( System::String ^ mpName )
{
  try
  {
    // find the extended element in the map
    Elements::iterator iter = m_elements->find( ss( mpName ) );
    if ( iter != m_elements->end() )
    {
      return iter->second;
    }
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  return nullptr;
} // GetElement( mpName )


/** 
 * Returns the specified element.
 *
 * @param mpName Name of element to return.
 * @return Element object; NULL if not found.
 */
generic <typename T>
where T : IElement
T Environment::GetElement( System::String ^ mpName )
{
  return safe_cast< T >( GetElement( mpName ));
} // GetElement( mpName )


/** 
 * Sets the primary viewer type for the environment.
 *
 * @param mpViewerType Primary VIewerType object.
 */
void Environment::ViewerType::set( Viatronix::Visualization::IViewerType ^ mpViewerType )
{
  if ( m_mpViewerType != nullptr )
  {
    if ( m_mpViewerType->Is2D )
    {
      m_pBackgroundColor2D->Disconnect( m_pObserver );
    }
    else
    {
      m_pBackgroundColor3D->Disconnect( m_pObserver );
    }
  }

  m_mpViewerType = mpViewerType; 
    
  if ( m_mpViewerType->Is2D )
  {
    m_pBackgroundColor2D->Connect( m_pObserver );
    m_pEnvironment->SetBackgroundColor( m_pBackgroundColor2D );
  }
  else if ( m_mpViewerType->Is3D )
  {
    m_pBackgroundColor3D->Connect( m_pObserver );
    m_pEnvironment->SetBackgroundColor( m_pBackgroundColor3D );
  }
  else
  {
    m_pEnvironment->SetBackgroundColor( m_pBackgroundColorDefault );
  }

  m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
} // set_ViewerType( mpViewerType )


/** 
 * Sets the intensity volume.
 *
 * @param mpVolume Intensity volume object.
 */
void Environment::Volume::set( Viatronix::Visualization::Volume ^ mpVolume )
{
  try
  {
    DisconnectFromElement( m_mpVolume );    
    m_mpVolume = mpVolume;
    ConnectToElement( m_mpVolume );    
    
    m_pEnvironment->SetIntensityVolume( ToNativeVolumeUint2Ptr( m_mpVolume->GetElementPtr() ) );
    m_pEnvironment->SetHistogram( reinterpret_cast< BaseHistogram * >( m_mpVolume->GetHistogramPtr().ToPointer() ) );
    
    m_pEnvironment->SetModified( vxEnvironment::VOLUME_INTENSITY, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));

    vxEnvironment * pEnvironment(static_cast<vxEnvironment *>(m_pEnvironment));
    Triple<float4> size(pEnvironment->GetIntensityVolume()->GetHeader().GetWorldDim());
    Box<float4> roi(0,0,0, size[0], size[1], size[2]);
    m_pEnvironment->GetZoomFactor()->SetCenter(roi.GetMidPoint());
    m_pEnvironment->GetZoomFactor()->SetSize(roi.GetOuterRadius());
    m_pEnvironment->SetModified(vxEnvironment::ZOOM_FACTOR, true);
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ZOOMFACTOR_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Volume( mpVolume )


/** 
 * Sets the label volume.
 *
 * @param mpVolume label volume object.
 */
void Environment::LabelVolume::set( Viatronix::Visualization::Volume ^ mpVolume )
{
  try
  {
    DisconnectFromElement( m_mpLabelVolume );   
    m_mpLabelVolume = mpVolume;    
    ConnectToElement( m_mpLabelVolume );
    
    m_pEnvironment->SetLabelVolume( ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ) );
    
    m_pEnvironment->SetModified( vxEnvironment::VOLUME_LABEL, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_LabelVolume( mpVolume )


/** 
 * Sets the components
 *
 * @param mpComponents Components object.
 */
void Environment::Components::set( Viatronix::Visualization::Components ^ mpComponents )
{
  try
  {
    DisconnectFromElement( m_mpComponents );       
    m_mpComponents = mpComponents;
    ConnectToElement( m_mpComponents );   

    m_pEnvironment->SetComponents( ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) );
    
    m_pEnvironment->SetModified( vxEnvironment::COMPONENT_NAME, true );
    m_pEnvironment->SetModified( vxEnvironment::COMPONENT_SELECTION, true );
    m_pEnvironment->SetModified( vxEnvironment::COMPONENT_VISUAL, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Components( mpComponents )


/** 
 * Sets the orthogonal plane.
 *
 * @param mpPlaneOrtho Orthogonal plane object.
 */
void Environment::Plane::set( Viatronix::Visualization::PlaneOrtho ^ mpPlaneOrtho )
{
  try
  {
    DisconnectFromElement( m_mpPlaneOrtho );

    m_mpPlaneOrtho = mpPlaneOrtho;
    ConnectToElement( m_mpPlaneOrtho );    

    m_pEnvironment->SetPlaneOrtho( ToNativePlaneOrthoPtr( m_mpPlaneOrtho->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::PLANE_MPR, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::PLANE_INITIALIZED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Plane( mpPlaneOrtho )


/** 
 * Sets the oblique plane.
 *
 * @param mpPlaneOblique Oblique plane object.
 */
void Environment::PlaneOblique::set( Viatronix::Visualization::PlaneOblique ^ mpPlaneOblique )
{
  try
  {
    DisconnectFromElement( m_mpPlaneOblique );

    m_mpPlaneOblique = mpPlaneOblique;
    ConnectToElement( m_mpPlaneOblique );  

    m_pEnvironment->SetPlaneOblique( ToNativePlaneObliquePtr( m_mpPlaneOblique->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::PLANE_MPR, true );    
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::PLANE_INITIALIZED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_PlaneOblique( mpPlaneOblique )


/** 
 * Sets the curved plane.
 *
 * @param mpPlaneCurved Curved plane object.
 */
void Environment::PlaneCurved::set( Viatronix::Visualization::PlaneCurved ^ mpPlaneCurved )
{
  try
  {
    DisconnectFromElement( m_mpPlaneCurved );
    m_mpPlaneCurved = mpPlaneCurved;
    ConnectToElement( m_mpPlaneCurved );

    m_pEnvironment->SetPlaneCurved( ToNativePlaneCurvedPtr( m_mpPlaneCurved->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::PLANE_MPR, true );    
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::PLANE_INITIALIZED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_PlaneCurved( mpPlaneCurved )


/** 
 * Sets the annotations.
 *
 * @param mpAnnotations Annotations object.
 */
void Environment::Annotations::set( Viatronix::Visualization::Annotations ^ mpAnnotations )
{
  try
  {
    DisconnectFromElement( m_mpAnnotations );
    m_mpAnnotations = mpAnnotations;
    ConnectToElement( m_mpAnnotations );

    m_pEnvironment->SetAnnotations( ToNativeAnnotationsPtr( m_mpAnnotations->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::HANDLESET, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Annotations( mpAnnotations )


/** 
 * Sets the segmentation.
 *
 * @param mpSegmentation Segmentation object.
 */
void Environment::Segmentation::set( Viatronix::Visualization::Segmentation ^ mpSegmentation )
{
  try
  {
    DisconnectFromElement( m_mpSegmentation );
    m_mpSegmentation = mpSegmentation;
    ConnectToElement( m_mpSegmentation );

    m_pEnvironment->SetInteractiveSegmentation( ToNativeInteractiveSegmentationPtr( m_mpSegmentation->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::INTERACTIVE_SEGMENTATION, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Segmentation( mpSegmentation )


/** 
 * Sets the colormap.
 *
 * @param mpColormap Colormap object.
 */
void Environment::Colormap::set( Viatronix::Visualization::Colormap ^ mpColormap )
{
  try
  {
    DisconnectFromElement( m_mpColormap );
    m_mpColormap = mpColormap;
    ConnectToElement( m_mpColormap );

    m_pEnvironment->SetColormap( ToNativeColormapPtr( m_mpColormap->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::COLORMAP, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Colormap( mpColormap )


/** 
 * Sets the camera.
 *
 * @param mpCamera Camera object.
 */
void Environment::Camera::set( Viatronix::Visualization::Camera ^ mpCamera )
{
  try
  {    
    DisconnectFromElement(m_mpCamera);
    m_mpCamera = mpCamera;
    ConnectToElement(m_mpCamera);

    m_pEnvironment->SetCamera(ToNativeCameraPtr(m_mpCamera->GetElementPtr()));
    m_pEnvironment->SetModified(vxEnvironment::CAMERA, true);
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Camera( mpCamera )


/**
 * Sets the crop box.
 *
 * @param mpCropBox Crop box object.
 */
void Environment::CropBox::set( Viatronix::Visualization::CropBox ^ mpCropBox )
{
  try
  {
    DisconnectFromElement( m_mpCropBox );
    m_mpCropBox = mpCropBox;
    ConnectToElement( m_mpCropBox );

    m_pEnvironment->SetCropBox( ToNativeCropBoxPtr( m_mpCropBox->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::CROP_BOX, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_CropBox( mpCropBox )


/**
 * Sets the contour list
 *
 * @param mpContourList contour list object
 */
void Environment::ContourList::set( Viatronix::Visualization::ContourList ^ mpContourList )
{
  try
  {
    DisconnectFromElement( m_mpContourList );
    m_mpContourList = mpContourList;
    ConnectToElement( m_mpContourList );

    m_pEnvironment->SetContourList( ToNativeContourListPtr( m_mpContourList->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::CONTOURLIST, true );
    m_pEnvironment->Modified( vxModInfo( vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_ContourList( mpContourList )


/**
 * Sets the 3D cursor.
 *
 * @param mpCursor3d 3D cursor object.
 */
void Environment::Cursor3d::set( Viatronix::Visualization::Cursor3d ^ mpCursor3d )
{
  try
  {
    DisconnectFromElement(m_mpCursor3d);
    m_mpCursor3d = mpCursor3d;
    ConnectToElement(m_mpCursor3d);

    m_pEnvironment->SetCursor3d(ToNativeCursor3dPtr( m_mpCursor3d->GetElementPtr()));

    m_pEnvironment->SetModified(vxEnvironment::CURSOR3D, true);
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Cursor3d( mpCursor3d )


/** 
 * Sets the rendering mode.
 *
 * @param mpRenderingMode Rendering mode object.
 */
void Environment::RenderingMode::set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode  )
{
  try
  {
    DisconnectFromElement( m_mpRenderingMode );

    m_mpRenderingMode = mpRenderingMode;
    ConnectToElement( m_mpRenderingMode );

    m_pEnvironment->SetRenderingMode( ToNativeRenderingModePtr( m_mpRenderingMode->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::RENDERING_MODE, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_RenderingMode( mpRenderingMode )


/** 
 * Sets the colormap and camera presets for the environment.
 *
 * @param preset v3D preset object.
 */
void Environment::Preset::set( Viatronix::Visualization::Preset ^ mpPreset )
{
  DisconnectFromElement( m_mpPreset );
  m_mpPreset = mpPreset;
  ConnectToElement( m_mpPreset );

  m_pEnvironment->SetPreset( ToNativePresetPtr( m_mpPreset->GetElementPtr() ) );

  m_pEnvironment->SetModified( vxEnvironment::PRESET, true );
  m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
} // set_Preset( mpPreset )


/**
 * Sets the zoom factor
 *
 * @param mpZoomFactor zoom factor object
 */
void Environment::ZoomFactor::set( Viatronix::Visualization::ZoomFactor ^ mpZoomFactor )
{
  try
  {
    DisconnectFromElement( m_mpZoomFactor );
    m_mpZoomFactor = mpZoomFactor;
    ConnectToElement( m_mpZoomFactor );

    m_pEnvironment->SetZoomFactor( ToNativePtrZoomFactor( m_mpZoomFactor->GetElementPtr() ) );

    m_pEnvironment->SetModified( vxEnvironment::ZOOM_FACTOR, true );
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_ZoomFactor( mpZoomFactor )

/**
 * Sets the Cine.
 *
 * @param mpTimer Cine object.
 */
void Environment::Cine::set( Viatronix::Visualization::Cine ^ mpCine )
{
  m_mpCine = mpCine;
  m_pEnvironment->SetCine( reinterpret_cast< vxCine * >( mpCine->GetElementPtr().ToPointer() ));
} // set_EventTimer( mpCine )


/** 
 * Sets the DataRendererSoftware3d
 * @param mpDataRendererSoftware DataRendererSoftware3d object
 */
void Environment::DataRendererSoftware::set( Viatronix::Visualization::DataRendererSoftware ^ mpDataRendererSoftware )
{
  try
  {
    DisconnectFromElement(m_mpDataRendererSoftware);
    m_mpDataRendererSoftware = mpDataRendererSoftware;
    ConnectToElement(m_mpDataRendererSoftware);

    m_pEnvironment->SetDataRendererSoftware(ToNativeDataRendererSoftwarePtr(m_mpDataRendererSoftware->GetElementPtr()));

    m_pEnvironment->SetModified(vxEnvironment::DATA_RENDERER_SOFTWARE, true);
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_DataRendererSoftware3d( mpDataRenderingSoftware )


/**
 * Sets current momentum
 *
 * @param mpMomentum  The current momentum
 */
void Environment::Momentum::set( Viatronix::Visualization::Momentum ^ mpMomentum )
{
  try
  {
    DisconnectFromElement( mpMomentum );
    m_mpMomentum = mpMomentum;
    ConnectToElement( mpMomentum );

    m_pEnvironment->SetMomentum( ToNativeMomentumPtr( mpMomentum->GetElementPtr() ));

    m_pEnvironment->SetModified( vxEnvironment::MOMENTUM, true );
    m_pEnvironment->Modified( vxModInfo( vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED ));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Momentum( mpMomentum )


/**
 * Sets the CameraFlySettings.
 *
 * @param mpTimer Cine object.
 */
void Environment::CameraFlySettings::set( Viatronix::Visualization::CameraFlySettings ^ mpCameraFlySettings )
{
  m_mpCameraFlySettings = mpCameraFlySettings;
  m_pEnvironment->SetCameraFlySettings( reinterpret_cast< vxCameraFlySettings * >( mpCameraFlySettings->GetElementPtr().ToPointer() ));
} // set_CameraFlySettings( mpCameraFlySettings )


/**
 * Sets the associated dataset
 *
 * @param     mpDataset           Dataset object.
 */
void Environment::AssociatedDataset::set( Viatronix::Visualization::Dataset ^ mpDataset )
{
  m_mpDataset = mpDataset;
  m_pEnvironment->SetDataset( reinterpret_cast< vxDataset * >( mpDataset->GetElementPtr().ToPointer() ));
} // set_AssociatedDataset( mpDataset )


/**
* Returns the background color for the environment.
*
* @returnBackground color for environment.
*/
System::Drawing::Color Environment::BackgroundColor::get()
{
  PixelRGBA< uint1> * pColor = m_pEnvironment->GetBackgroundColor();

  return System::Drawing::Color::FromArgb( pColor->A(), pColor->R(), pColor->G(), pColor->B() );
} // get_BackgroundColor()


/**
* Sets the background color for the environment.
*
* @param color Background color for environment.
*/
void Environment::BackgroundColor::set( System::Drawing::Color color )
{
  if ( m_mpViewerType->Is2D )
  {
    m_pBackgroundColor2D->SetRGBA( color.R, color.G, color.B, 255 );
    m_pBackgroundColor2D->Modified( vxModInfo( PixelRGBA< uint1 >, 0 ) );
  }
  else
  {
    m_pBackgroundColor3D->SetRGBA( color.R, color.G, color.B, 255 );
    m_pBackgroundColor3D->Modified( vxModInfo( PixelRGBA< uint1 >, 0 ) );
  }  
} // set_BackgroundColor( color )


/**
 * Sets the cad findings.
 *
 * @param   mpFindings      CadFindings object.
 */
void Environment::CadFindings::set( Viatronix::Visualization::CadFindings ^ mpFindings )
{
    DisconnectFromElement( m_mpCadFindings );
    m_mpCadFindings = mpFindings;
    ConnectToElement( mpFindings );
    
    m_pEnvironment->SetCadFindings( ToNativeCadFindingsPtr( m_mpCadFindings->GetElementPtr() ) );

    //m_pEnvironment->SetModified( vxEnvironment::CAD_FINDINGS, true ); Do we need to do this?
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));

} // set_CadFindings( mpFindings )


/**
 * Sets the visualization graph
 *
 * @param   mpFindings      CadFindings object.
 */
void Environment::VisualizationGraph::set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpVisualizationGraph )
{
  m_mpVisualizationGraph = mpVisualizationGraph;
  m_pEnvironment->SetVisualizationGraph( reinterpret_cast< vxVisualizationGraph * >( m_mpVisualizationGraph->GetElementPtr().ToPointer() ));
} // set_VisualizationGraph( mpVisualizationGraph )


/**
 * Sets the selected environment.
 *
 * @param     mpEnvironment      Selected environment.
 */
void Environment::SelectedEnvironment::set( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  m_mpSelectedEnvironment = mpEnvironment;

  vxEnvironment * pEnvironment = ( m_mpSelectedEnvironment == nullptr ) ? NULL : 
                                   reinterpret_cast< vxEnvironment * >( m_mpSelectedEnvironment->GetEnvironmentPtr().ToPointer() );
  vxEnvironment::SetSelectedEnvironment( pEnvironment );
} // set_SelectedEnvironment( mpEnvironment )


/**
 * Sets the active environment.
 *
 * @param     mpEnvironment      Active environment.
 */
void Environment::SetActiveEnvironment( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  vxEnvironment::SetActiveEnvironment( reinterpret_cast< vxEnvironment * >( mpEnvironment->GetEnvironmentPtr().ToPointer() ));
} // SetActiveEnvironment( mpEnvironment )


/**
* Sets the camera to a standard position.
*
* @param eView Standard camera position.
* @param bView True if whole object should be viewed.
*/
void Environment::SetStandardCameraView( Viatronix::Visualization::CameraViews eView, bool bViewAll )
{
  Box<float4> roi( 0, 0, 0, m_mpVolume->WorldDimensions->x, m_mpVolume->WorldDimensions->y, m_mpVolume->WorldDimensions->z );

  vxUtils::SetStandardCameraView( * ToNativeCameraPtr( m_mpCamera->GetElementPtr()),
                                  static_cast< vxUtils::StandardCameraViewEnum >( eView ),
                                  roi,
                                  bViewAll );

  ToNativeCameraPtr( m_mpCamera->GetElementPtr() )->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
} // SetStandardCameraView( eView, bViewAll )


/**
* Adds an observer to the specified element.
*
* @param mpElement Element to add to this observer.
*/
void Environment::ConnectToElement( Viatronix::Visualization::IElement ^ mpElement )
{
  if ( mpElement != nullptr )
  {
    mpElement->Connect( System::IntPtr( m_pObserver ) );
  }
} // ConnectToElement( mpElement )


/**
 * called when the modified event is raised
 *
 * @param mpSender the sender
 * @param mpArgs   the arguments
 */
void Environment::OnElementModifiedHandler( System::Object ^ mpSender, System::EventArgs ^ mpArgs )
{
  gcroot< IElement ^ > pElement = safe_cast< IElement ^ >( mpSender );
  
  Elements::iterator iter = m_elements->begin();
  for( ; iter != m_elements->end(); ++iter )
  {
    if( (*iter).second->Equals( pElement ))
    {
      System::String ^ mpID = gcnew System::String( iter->first.c_str() );
      m_pEnvironment->Modified( vxModInfoID( vxElement, ss( mpID ), 0 ) );
      break;
    }
  }
} // OnElementModifiedHandler( mpSender, mpArgs )


/**
 * Removes an observer to the specified element.
 *
 * @param mpElement Element to remove observer from.
 */
void Environment::DisconnectFromElement( Viatronix::Visualization::IElement ^ mpElement )
{
  if ( mpElement != nullptr )
  {
    mpElement->Disconnect( System::IntPtr( m_pObserver ) );
  }
} // DisconnectFromElement( mpElement )


/**
 * Sets this object as the primary observer.
 */
void Environment::SetPrimary() 
{ 
  vxShareableObjectHelper::ConnectPrimary( m_pObserver ); 
} // SetPrimary()


///**
// * Sets this object as the primary observer.
// * /
//void Environment::SetModified(const vxEnvironmentEnum eElement, const bool bModified) 
//{ 
//  m_pEnvironment->SetModified(eElement, bModified); 
//} // SetModified()


/** 
 * Processes modified queue events. 
 *
 * @param modifiedList Modified queue event list.
 */
void Environment::OnModified( System::IntPtr modifiedList )
{
  // return immediately if not interested in events
  if ( m_pEnvironment->GetIgnoreModifiedEvents() )
  {
    return;
  }
    
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );

  bool bSomeFlagSet( false );
  for ( const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next() )
  {
    bool bFlagSet( true );
    const type_info & elementType = pInfo->GetTypeInfo();

    if ( IsModified( typeid( vxColormap ), *pInfo, m_mpColormap ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::COLORMAP, true );
    }
    else if ( IsModified( typeid( vxAnnotations ), *pInfo, m_mpAnnotations ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::HANDLESET, true );
    }
    else if ( IsModified( typeid( vxCAD::vxCadFindings ), *pInfo, m_mpCadFindings ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::CAD_FINDINGS, true );
    }
    else if ( IsModified( typeid( InteractiveSegmentation ), *pInfo, m_mpSegmentation ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::INTERACTIVE_SEGMENTATION, true );
    }
    else if ( IsModified( typeid( vxBlockVolume<uint2> ), *pInfo, m_mpVolume ) ) 
    {
      if ( pInfo->GetFlag() == vxUtils::VOLUME_INTENSITY_BASEHISTOGRAM_MODIFIED )
      {
        m_pEnvironment->SetModified( vxEnvironment::VOLUME_INTENSITY_HISTOGRAM, true );
      }
      else
      {
        m_pEnvironment->SetModified( vxEnvironment::VOLUME_INTENSITY, true );
      }
    }
    else if ( IsModified( typeid( vxBlockVolume<uint2> ), *pInfo, m_mpLabelVolume ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::VOLUME_LABEL, true );
    }
    else if ( IsModified( typeid( vxCamera ), *pInfo, m_mpCamera ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::CAMERA, true );
    }
    else if ( IsModified( typeid( vxCropBox ), *pInfo, m_mpCropBox ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::CROP_BOX, true );
    }
    else if ( IsModified( typeid( vxContourList ), *pInfo, m_mpContourList ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::CONTOURLIST, true );
    }
    else if ( IsModified( typeid( vxRenderingMode ), *pInfo, m_mpRenderingMode ) ) 
    {
      if( pInfo->GetFlag() == vxRenderingMode::SHOW_CROP_BOX )
      {
        m_pEnvironment->SetModified( vxEnvironment::CROP_BOX, true );
      }

      m_pEnvironment->SetModified( vxEnvironment::RENDERING_MODE, true );
    }

    else if ( IsModified( typeid( vxFontInfo ), *pInfo, m_mpFontInfo ) ) 
    {
      m_pEnvironment->GetContext()->MakeCurrent();
      m_pEnvironment->UpdateFont();
      m_pEnvironment->SetModified( vxEnvironment::FONT, true );
    }
    else if ( IsModified( typeid( vxPlaneOrtho< float4 >   ), *pInfo, m_mpPlaneOrtho )   ||
              IsModified( typeid( vxPlaneOblique< float4 > ), *pInfo, m_mpPlaneOblique ) ||
              IsModified( typeid( vxPlaneCurved            ), *pInfo, m_mpPlaneCurved ) )
    {
      m_pEnvironment->SetModified( vxEnvironment::PLANE_MPR, true );
    }
    else if ( IsModified( typeid( vxComponentArray ), *pInfo, m_mpComponents ) )
    {
      if ( pInfo->GetFlag() == vxUtils::COMPONENTARRAY_NAME_MODIFIED )
      {
        m_pEnvironment->SetModified( vxEnvironment::COMPONENT_NAME, true );
      }
      else if ( pInfo->GetFlag() == vxUtils::COMPONENTARRAY_SELECTED )
      {
        m_pEnvironment->SetModified( vxEnvironment::COMPONENT_SELECTION, true );
      }
      else
      {
        m_pEnvironment->SetModified( vxEnvironment::COMPONENT_VISUAL, true );
      }
    }
    else if ( IsModified( typeid( vxCursor3d ), *pInfo, m_mpCursor3d ) ) 
    {
      m_pEnvironment->SetModified( vxEnvironment::CURSOR3D, true );
    }
    else if ( elementType == typeid( vxEventTimer ) )
    {
      const std::string sPrefix( pInfo->GetText().substr( 0, vxEventTimer::GetQualitySpeedTradeoffTimerName().size() ) );
      if (sPrefix == vxEventTimer::GetQualitySpeedTradeoffTimerName())
      {
        // this is a semi-HACK and we need a better timer mechanism for manipulation as well as rendering timers
        if (pInfo->GetFlag() == vxUtils::EVENTTIMER_KILLED)
        {
          m_pEnvironment->SetModified( vxEnvironment::EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT, false );
        }
        else
        {
          m_pEnvironment->SetModified( vxEnvironment::EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT, true );
        }
      }
      else if (pInfo->GetText() == "FLY")
      {
        // this is a semi-HACK and we need a better timer mechanism for manipulation as well as rendering timers
        if (pInfo->GetFlag() == vxUtils::EVENTTIMER_KILLED)
        {
          m_pEnvironment->SetModified( vxEnvironment::EVENT_TIMER_3D_FLY, false );
          LogDbg("Killing EventTimer flag in vxEnvironment (EVENT_TIMER_3D_FLY)", "", "");
        }
        else
        {
          m_pEnvironment->SetModified( vxEnvironment::EVENT_TIMER_3D_FLY, true );
        }
      }
      else if (pInfo->GetText() == "MOMENTUM")
      {
        // for this timer, we want one more redraw so that we can trigger progressive refinement
        {
          m_pEnvironment->SetModified( vxEnvironment::EVENT_TIMER_MOMENTUM, true );
          LogDbg("EventTimer flag in vxEnvironment set (EVENT_TIMER_MOMENTUM)", "", "");
        }
      }
    }
    else if ( elementType == typeid( vxDataRenderer ) )
    {
      m_pEnvironment->SetModified( vxEnvironment::DATA_RENDERER_SOFTWARE, true );
    }
    else if ( elementType == typeid( vxZoomFactor ) )
    {
      m_pEnvironment->SetModified( vxEnvironment::ZOOM_FACTOR, true );
    }
    else if ( elementType == typeid( Vector3D<int4> ) )
    {
      m_pEnvironment->SetModified( vxEnvironment::CURRENT_VOXEL, true );
    }
    else if ( elementType == typeid( PixelRGBA< uint1 > ) )
    {
      m_pEnvironment->SetModified( vxEnvironment::BACKGROUND_COLOR, true );
    }
    else if (elementType == typeid(vxElement))
    {
      m_pEnvironment->SetModified( dynamic_cast<const vxModifiedInfo *>( pInfo )->GetID(), true ); 
    }
    else if ( elementType == typeid( vxContour ))
    {
      m_pEnvironment->SetModified( vxEnvironment::SCULPT_CONTOUR, true );
    }
    else if ( elementType == typeid( vxMomentum ))
    {
      m_pEnvironment->SetModified( vxEnvironment::MOMENTUM, true );
    }
    else
    {
      bFlagSet = false;
    }

    // while this works for now, it is just a temporary workaround for the limitations
    // of the current ModifiedQueue. The problem is it flushes and reflushes and case
    // something was trying to call flush in the meantime. What we need is a ROYAL FLUSH
    // that flushes it all until there is nothing left and then notfies a global handler
    // that then notifies all registered clients to rerender. Registered clients would be
    // all V3D Viewers that got a Modified() call during the flush.
    bSomeFlagSet |= bFlagSet;

  } // for ( const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next() )

  // pass the modified list along to the viewer type
  m_pEnvironment->GetViewerType()->Modified( *pInfoList );

  // let observers connect to this environment respond to modified events
  if( bSomeFlagSet )
  {
    m_pEnvironment->Modified(vxModInfo(vxEnvironment, vxUtils::ENVIRONMENT_MODIFIED));
  }
  
  //m_pEnvironment->Modified( *pInfoList );  

  // flush again
  ModifiedQueue::Flush();

  // if any element has been modified in the environment, raise a modified event
  if ( m_bEnableEvents && m_pEnvironment->IsModified( vxEnvironment::ALL ) )
  {
    //Timer timer;
    //timer.Reset();
    Changed( this, System::EventArgs::Empty );
    //LogDbg("Changed() took: " + ToAscii(timer.ElapsedTime()), "", "");
  }
} // OnModified( modifiedList )


/**
 * Gets if the specified element has been modified.
 *
 * @param   type    type of element required
 * @param   info    modified info object
 * @param   element element to check
 * @return  true if modified false otherwise
 */
bool Environment::IsModified( const type_info & type, const vxModifiedInfo & info, Viatronix::Visualization::IElement ^ mpElement )
{
  if( type != info.GetTypeInfo() )
  {
    return false;
  }

  if( info.GetSender() != reinterpret_cast< const void * >( mpElement->GetElementPtr().ToPointer() ) )
  {
    return false;
  }

  return true;
} // IsModified( type, info, element );


/**
 * Removes all the handlers from the events
 */
void Environment::ResetEvents()
{
  if( m_mpChangedEvent == nullptr )
  {
    return;
  }

  array< System::Delegate ^ > ^ mpHandlers = m_mpChangedEvent->GetInvocationList();
  for( int index( 0 ); index < mpHandlers->Length; ++index )
  {
    this->Changed -= safe_cast< System::EventHandler ^ >( mpHandlers[ index ] );
  }
} // ResetEvents()


/**
 * Hack for swapping in a new volume and label volume without accumulating modified events - good for 4D data
 *
 * @param mpIntensityVolume the intensity volume
 * @param mpLabelVolume     the label volume
 */
void Environment::SwapInDataset( Viatronix::Visualization::Dataset ^ mpDataset )
{
  try
  {
    // swap in intensity volume
    DisconnectFromElement( m_mpVolume );    
    m_mpVolume = mpDataset->IntensityVolume;
    ConnectToElement( m_mpVolume );    
    m_pEnvironment->SetIntensityVolume( ToNativeVolumeUint2Ptr( m_mpVolume->GetElementPtr() ) );
    m_pEnvironment->SetHistogram( reinterpret_cast< BaseHistogram * >( m_mpVolume->GetHistogramPtr().ToPointer() ) );

    // swap in label volume
    DisconnectFromElement( m_mpLabelVolume );   
    m_mpLabelVolume = mpDataset->LabelVolume;
    ConnectToElement( m_mpLabelVolume );
    m_pEnvironment->SetLabelVolume( ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ) );

    // swap in components
    DisconnectFromElement( m_mpComponents );       
    m_mpComponents = mpDataset->Components;
    ConnectToElement( m_mpComponents );   
    m_pEnvironment->SetComponents( ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SwapInDataset( mpDataset )


// undefines
#undef FILE_REVISION


// $Log: v3D_Environment.cpp,v $
// Revision 1.52.2.1  2009/07/29 19:54:06  kchalupa
// Code Walkthrough
//
// Revision 1.52  2008/03/20 16:55:43  cqian
// add titlestring
//
// Revision 1.51  2008/01/22 21:07:10  gdavidson
// Fixed bug in SetActiveEnvironment
//
// Revision 1.50  2008/01/18 21:29:27  geconomos
// Issue# 5869: Images updating in background while making a movie
//
// Revision 1.49  2007/10/03 20:09:15  gdavidson
// Added SelectedEnvironment property
//
// Revision 1.48  2007/09/19 14:35:54  gdavidson
// Modified the AssociatedDataset property to update the vxEnvironment
//
// Revision 1.47  2007/07/30 15:22:00  gdavidson
// Issue #5766: Initialize oblique plane by default
//
// Revision 1.46  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.45  2007/05/03 21:28:44  gdavidson
// Added VisualizationGraph
//
// Revision 1.44  2007/04/11 20:58:01  romy
// added CadFindings
//
// Revision 1.43  2007/04/06 14:11:35  gdavidson
// Added CadFindings
//
// Revision 1.42.2.1  2007/04/09 14:13:51  geconomos
// Issue #5572: Toggling of cropbox visibility not correct in 3D and oblique
//
// Revision 1.42  2007/01/31 15:05:31  romy
// Initialized m_mpViewerType in the constructor
//
// Revision 1.41  2007/01/16 17:10:54  gdavidson
// Added CameraFlySettings, Cine and Momentum
//
// Revision 1.40  2006/12/11 17:22:03  romy
// Modified Font event is added
//
// Revision 1.39  2006/10/24 12:34:14  geconomos
// removed old slice shadows mechanism
//
// Revision 1.38  2006/10/05 17:28:17  geconomos
// removed slice shadows
//
// Revision 1.37  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.36  2006/10/03 20:15:18  geconomos
// changed colors for slice shadow planes
//
// Revision 1.35  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.34  2006/09/19 18:07:34  gdavidson
// Removed creation of vxEnvironmentList from constructor
//
// Revision 1.33  2006/09/07 13:21:37  geconomos
// code cleanup
//
// Revision 1.32  2006/08/31 14:01:53  geconomos
// Modified SwapInDataset to take Dataset instance as argument
//
// Revision 1.31  2006/04/25 18:47:13  geconomos
// + renamed m_pObserver -> m_pObserver
// + removed unmanged vxSliceShadow instance
// + proper event handling of managed shadow object
//
// Revision 1.30  2006/04/03 18:46:46  geconomos
// BaseHistogram is no longer a shareable object
//
// Revision 1.29  2006/03/29 20:11:50  geconomos
// removed vxBlockVolumeBinaryHistogram related code
//
// Revision 1.28  2006/03/27 19:46:18  gdavidson
// Added EnvironmentList and the configuration of slice shadow colors.
//
// Revision 1.27  2006/03/02 15:50:37  frank
// added composite undo actions
//
// Revision 1.26  2006/02/17 15:18:00  frank
// rendering mode is no longer necessary for the segmentation object
//
// Revision 1.25  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.24  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.23  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.22.2.3  2005/08/12 17:20:58  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.22.2.2  2005/08/05 13:10:20  frank
// added ray profile data
//
// Revision 1.22.2.1  2005/07/29 16:25:16  frank
// removed dead code
//
// Revision 1.22  2005/05/24 14:40:02  frank
// fixed camera link/unlink movement problem by adjusting proportionally only when the preset is selected, not when the camera is set in the environment
//
// Revision 1.21  2005/05/24 11:33:22  frank
// added background color and rendering mode during preset changes
//
// Revision 1.20  2004/10/29 20:18:46  frank
// formatting
//
// Revision 1.19  2004/10/29 12:15:13  frank
// moved plane sychronization into a controller
//
// Revision 1.18  2004/10/20 16:36:50  geconomos
// fixed issue with extended element map
//
// Revision 1.17  2004/10/04 15:13:27  frank
// improved the extended elements modified interface
//
// Revision 1.16  2004/09/01 12:31:11  frank
// decreased the granularity of component array modified bits
//
// Revision 1.15  2004/08/31 00:03:49  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.14  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.13  2004/05/24 15:19:01  frank
// fixed background color opacity
//
// Revision 1.12  2004/05/13 18:40:26  frank
// un-shared the background color
//
// Revision 1.11  2004/04/27 14:08:15  michael
// removed dangling pointers to unmanaged zoom factor
//
// Revision 1.10  2004/04/26 21:27:32  michael
// removed commented out code
//
// Revision 1.9  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.8  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.7  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.6  2004/04/06 12:09:52  frank
// no use for the skeleton in the environment
//
// Revision 1.5  2004/03/30 18:17:09  frank
// split out the progressive refinement flag from the renderer CPU 3d data so that it can be applied to all renderers in an environment at once
//
// Revision 1.4  2004/03/26 21:16:03  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.3  2004/03/12 19:54:49  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:07:51  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.138  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.137  2004/02/20 23:33:43  michael
// added new zoom factor class
//
// Revision 1.136.2.1  2004/02/24 20:14:36  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.136  2004/01/06 13:11:15  michael
// fixed typo in comments ...
//
// Revision 1.135  2003/12/17 01:01:28  geconomos
// Added ResetEvents method to clear out all handlers in multicast delegate "Changed"
//
// Revision 1.134  2003/12/16 13:40:25  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.133  2003/11/11 02:51:27  michael
// took out redundant log messages
//
// Revision 1.132  2003/11/11 01:52:51  michael
// added check so that if the distance of the new cursor 3D is less than a certain
// epsilon, then we are not resetting the plane. Without that, the plane oblique is
// moving whiel dragging the cursor, due to arithmetic precision issues.
//
// Revision 1.131  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.130  2003/10/29 20:55:01  frank
// Removed double rendering in curved MPR
//
// Revision 1.129  2003/10/03 12:13:12  michael
// took out log messages that were used for debugging
//
// Revision 1.128  2003/10/02 10:26:54  geconomos
// Added a static instance of FontInfo class to environment and added to observer list.
//
// Revision 1.127  2003/10/01 21:20:14  michael
// fixed typo in comment (copy and paste error)
//
// Revision 1.126  2003/09/29 19:27:13  michael
// unmanaged vxShareableObject<vxRendererCpu3dData> ==> managed RendererCpu3dData
//
// Revision 1.124  2003/09/22 15:57:44  michael
// added event timer
//
// Revision 1.123  2003/08/22 03:44:04  geconomos
// Added temporary hack for animations( will remove in the next couple of days )
//
// Revision 1.122  2003/08/08 20:37:52  michael
// took out redundant code
//
// Revision 1.121  2003/07/30 18:40:55  michael
// took out debug message
//
// Revision 1.119  2003/07/30 15:39:12  geconomos
// Fixed check for base histogram modifications.
//
// Revision 1.118  2003/07/30 13:17:11  geconomos
// Added special check for base histogram modifications in modified queue processing.
//
// Revision 1.117  2003/06/23 14:22:29  michael
// fixed update problem on sculpt lasso (3D image was not showing it after
// progressive refinement was completed)
//
// Revision 1.116  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.115  2003/06/09 12:29:53  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.114  2003/06/03 15:18:23  geconomos
// Added OverlayStrings as a default element in the initializer of v3D_Environment.
//
// Revision 1.113  2003/06/03 14:38:07  geconomos
// Added modified event when setting the colormap for the environment.
//
// Revision 1.112  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.111  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.110  2003/05/12 18:17:07  frank
// Added slice shadows object
//
// Revision 1.109  2003/05/06 17:01:11  geconomos
// Coding conventions.
//
// Revision 1.108  2003/05/06 12:20:38  geconomos
// Issue # 3077: Added separate background color for undefined viewer types.
//
// Revision 1.107  2003/05/01 13:19:58  michael
// replaced modified parameter from Triple<> to vxCursor3d
//
// Revision 1.106  2003/04/29 21:19:03  michael
// renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData
//
// Revision 1.105  2003/04/24 14:03:45  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.104  2003/04/14 16:46:29  frank
// Removed unused code
//
// Revision 1.103  2003/04/10 12:03:31  frank
// Added OverlayStrings extended element to environment
//
// Revision 1.102  2003/04/09 15:57:01  geconomos
// Issue# 3077: Maintaining background colors for 2D and 3D.
//
// Revision 1.101  2003/04/07 18:02:46  michael
// fixed background color for MPR views
//
// Revision 1.100  2003/04/07 15:52:36  michael
// transition from BackColor to BackgroundColor
//
// Revision 1.99  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.98  2003/04/01 16:02:08  geconomos
// Updated to public interface of managed volume class.
//
// Revision 1.97  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.96  2003/03/30 21:00:30  michael
// added viewertype to environment
//
// Revision 1.95  2003/03/25 21:42:37  frank
// Access to InteractiveSegmentation object is now through a System::IntPtr
//
// Revision 1.94  2003/03/23 18:40:26  frank
// Misplaced /* denied execution of critical code.
//
// Revision 1.93  2003/03/20 13:43:23  geconomos
// Removed property ViewerType.
//
// Revision 1.92  2003/03/17 20:21:02  michael
// fixed update of annotation drawing
//
// Revision 1.91  2003/03/14 14:02:08  geconomos
// Updated for changed in Volume::BinaryHistogram return type.
//
// Revision 1.90  2003/03/12 16:13:32  geconomos
// Removed inclusion of v3D_Logger.h
//
// Revision 1.89  2003/03/10 15:00:49  geconomos
// Implemented EVENT_NAME_MODIFIED flag.
//
// Revision 1.88  2003/03/07 20:36:00  geconomos
// Changed to use managed method GetElementPtr for class
// Triplet.
//
// Revision 1.87  2003/03/04 15:36:16  geconomos
// Added call to current ViewerType to process modified queue.
//
// Revision 1.86  2003/03/03 17:18:29  geconomos
// Initial ViewerType implementation
//
// Revision 1.85  2003/02/19 15:23:14  geconomos
// More coding conventions.
//
// Revision 1.84  2003/02/14 19:23:06  frank
// Fixed interface to access rendering mode encapsulated object
//
// Revision 1.83  2003/01/28 19:31:36  geconomos
// Implementation of BackgroundColor.
//
// Revision 1.82  2003/01/22 22:08:16  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.81  2003/01/13 23:21:17  michael
// added redraw filter for vxAnnotations ... will sooner or later replace HandlesSet
//
// Revision 1.80  2003/01/13 14:46:30  geconomos
// Updated for new annotations class.
//
// Revision 1.79  2003/01/06 21:15:03  ingmar
// removed unused static member variable
//
// Revision 1.78  2002/12/18 15:46:59  geconomos
// Added IViewerType property.
//
// Revision 1.77  2002/11/28 02:48:16  frank
// component selected events
//
// Revision 1.76  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.75  2002/11/22 17:25:51  michael
// removed redundant code and hooked up oblique plane to 3D cursor modification
//
// Revision 1.74  2002/11/19 21:44:07  michael
// added camera initialization code for oblique view ... still to be centralized ...
//
// Revision 1.73  2002/11/18 20:39:58  geconomos
// Added IgnoreModifiedEvents property.
//
// Revision 1.72  2002/11/18 20:23:41  michael
// added plane oblique
//
// Revision 1.71  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.70  2002/11/15 18:04:43  geconomos
// Added AssociatedPreset field and property
//
// Revision 1.69  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.68  2002/11/13 22:02:41  geconomos
// add null check for vxWindowHandle before setting the camera
//
// Revision 1.67  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.66  2002/11/13 14:59:30  geconomos
// Removed EVENT_FLOAT_DISTANCE check in modified.
//
// Revision 1.65  2002/11/12 21:31:07  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.64  2002/11/12 17:05:52  michael
// fixed pan/zoom with different aspect ratios
//
// Revision 1.63  2002/11/12 00:13:02  michael
// fixed setting the camera (pane/zoom problem and sagittal image was missing)
//
// Revision 1.62  2002/11/08 23:58:23  frank
// Added a contour element to the environment.
//
// Revision 1.61  2002/11/08 14:49:19  geconomos
// Added momentum property.
//
// Revision 1.60  2002/11/07 18:21:21  ingmar
// the CURSOR3D event is now set regardless of the plane being actually modified 
// (hence the 3D view will update if the cursor3d is moved in an axial slice
//
// Revision 1.59  2002/11/07 00:09:26  ingmar
// code beautify
//
// Revision 1.58  2002/11/06 23:53:32  wenli
// Colormap now returns a System::IntPtr
//
// Revision 1.57  2002/11/06 23:09:18  michael
// took out the Get/SetFloatDistance() bool which has nothing to do with this class!!!
//
// Revision 1.56  2002/11/06 22:15:30  geconomos
// Implemented GetPlaneOrthoPtr()  as System::IntPtr
//
// Revision 1.55  2002/11/05 22:44:50  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.54  2002/11/05 20:52:56  geconomos
// Clean up of modified method.
//
// Revision 1.53  2002/11/04 18:52:14  frank
// Enabling crop box
//
// Revision 1.52  2002/11/04 18:00:45  geconomos
// removed
//
// Revision 1.51  2002/11/04 14:30:35  geconomos
// new toolbar class.
//
// Revision 1.50  2002/11/02 03:11:37  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.49  2002/10/31 12:51:08  geconomos
// Chaged GetEnvironmentPtr() to return a System::IntPtr for interoperability.
//
// Revision 1.48  2002/10/24 19:53:24  michael
// added presets and initial camera and colormap which are activated right away
//
// Revision 1.47  2002/10/21 22:26:06  frank
// Added PlaneCurvilinear
//
// Revision 1.46  2002/10/21 15:43:13  geconomos
// Moved loading of deafult preset from environment to explorer executable.
//
// Revision 1.45  2002/10/19 20:53:06  michael
// cosmetics
//
// Revision 1.44  2002/10/18 18:53:06  michael
// brought back in reading of preset from string (initialization)
//
// Revision 1.43  2002/10/18 17:23:13  geconomos
// Creation of default preset.
//
// Revision 1.42  2002/10/17 15:29:09  geconomos
// Added Preset property.
//
// Revision 1.41  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.40  2002/10/04 13:31:35  geconomos
// Added segmentation class.
//
// Revision 1.39  2002/10/03 17:55:31  michael
// Extended possible presets (Colormap and Camera)
//
// Revision 1.38  2002/10/02 03:23:28  michael
// Moved helper functions for camera initialization to 70_vxUtils
//
// Revision 1.37  2002/10/02 02:37:28  michael
// Moved helper functions for camera initialization to 70_vxUtils
//
// Revision 1.36  2002/09/27 19:25:34  dmitry
// Removed unnecessary calls to update colortable.
//
// Revision 1.35  2002/09/26 19:04:27  dmitry
// RemoveAllElements implemented.
//
// Revision 1.34  2002/09/26 18:12:27  dmitry
// ColormapPresets property added.
//
// Revision 1.33  2002/09/25 22:12:44  michael
// Fixed intial update of discrete lookup tables for 3D view ... needs better fix
//
// Revision 1.32  2002/09/25 20:37:02  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.31  2002/09/24 18:17:54  geconomos
// Exception handling and formatting.
//
// Revision 1.30  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.29  2002/09/23 15:06:27  michael
// removed last instance of transfer function
//
// Revision 1.28  2002/09/20 16:41:08  geconomos
// Changed implemetation of class vxID
//
// Revision 1.27  2002/09/19 15:47:02  dmitry
// gcroot used for elements
//
// Revision 1.26  2002/09/19 14:26:00  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.25  2002/09/09 18:51:00  geconomos
// Implemented disposable pattern.
//
// Revision 1.24  2002/09/05 22:39:08  jaddonisio
// New vxComponent.
//
// Revision 1.23  2002/08/29 20:16:47  dmitry
// m_pEnvironment->RemoveElement placed on correct line.
//
// Revision 1.22  2002/08/26 21:05:24  dmitry
// Distance set to the top slice when orientation changes.
//
// Revision 1.21  2002/08/16 15:07:46  dmitry
// Moved RQST.
//
// Revision 1.20  2002/08/13 14:06:34  dmitry
// update indices on remove element
//
// Revision 1.19  2002/08/09 16:20:41  dmitry
// IElement management implemented and GetElement method added.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Environment.cpp,v 1.52.2.1 2009/07/29 19:54:06 kchalupa Exp $
// $Id: v3D_Environment.cpp,v 1.52.2.1 2009/07/29 19:54:06 kchalupa Exp $
