// $Id: v3D_Viewer.cpp,v 1.54.2.2 2009/05/01 21:45:52 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Viewer.h"
#include "v3D_Events.h"
#include "v3D_Interfaces.h"
#include "v3D_Viewerbar.h"
#include "v3D_ViewerbarButtonCollection.h"
#include "v3D_ViewerTypes.h"
#include "v3D_ModifiedQueue.h"
#include "v3D_VisualizationPreset.h"
#include "v3D_Dataset.h"
#include "vxViewer.h"
#include "Timer.h"
#include "vxNodeRendering.h"
#include "v3d_undoactionStack.h"
#include "v3D_RenderingNodePlugin.h"
#include "v3D_Momentum.h"
#include "v3D_Cine.h"
#include "v3D_NodeRendering.h"
#include "v3D_VisualizationGraph.h"
#include "v3D_CycleSettings.h"
#include "vxInputAdapter.h"
#include "vxIRenderingContext.h"
#include "vxRenderingContextPBuffer.h"
#include "vxNodeDrawing.h"
#include "vxNodeIDs.h"
#include "vxVisualizationPreferences.h"
#include <mmsystem.h>

// namespaces
USING_VISUALIZATION_NS


// defines
#define FILE_REVISION "$Revision: 1.54.2.2 $"


/**
 * Initializes a Viewer object with default values.
 */
Viewer::Viewer() : 
  m_bRightButtonDown( false ),
  m_bAllowSubView( true ),
  m_mpViewerType( nullptr ),
  m_mpSubViewerType( nullptr ),
  m_mpRenderingManagerPlugin( nullptr ),
  m_pInputAdapter( new vxInputAdapter() )
{
  // managed types
  m_mpEnvironment = gcnew Viatronix::Visualization::Environment();
  m_mpViewerbar = gcnew Viatronix::Visualization::Viewerbar( this );
  m_mpPopupMenu = gcnew Viatronix::UI::PopupMenu();
  m_mpSupportedAnimations = gcnew array< System::Type ^ >( 0 );
  
  // unmanaged types
  m_pViewer = new vxViewerControl( ToNativeEnvironmentPtr( m_mpEnvironment->GetEnvironmentPtr() ) );
  
  // attach input adapter to vxViewers
  m_pInputAdapter->AddViewer( m_pViewer->GetViewer( vxViewerControl::MainViewer ));
  m_pInputAdapter->AddViewer( m_pViewer->GetViewer( vxViewerControl::SubViewer ));

  // enable drag and drop
  this->AllowDrop = true;
} // Viewer( )


/**
 * destructor
 */
Viewer::~Viewer()
{
  //delete m_mpEnvironment;

  this->!Viewer();
} // ~Viewer()


/**
 * Finalizer
 */
Viewer::!Viewer()
{
  if( m_pViewer != NULL )
  {
    delete m_pViewer;
    m_pViewer = NULL;
  }
} // ~Viewer()


/**
 * Initializes the viewer from the specified dataset.
 *
 * @param   mpDataset   Dataset to initialize from.
 */
void Viewer::Initialize( Viatronix::Visualization::Dataset ^ mpDataset )
{
  // copy elements from dataset into viewer's environment
  this->Environment->AssociatedDataset = mpDataset;
  this->Environment->Annotations       = mpDataset->Annotations;
  this->Environment->Components        = mpDataset->Components;
  this->Environment->Volume            = mpDataset->IntensityVolume;
  this->Environment->LabelVolume       = mpDataset->LabelVolume;
  this->Environment->Segmentation      = mpDataset->Segmentation;
  this->Environment->CropBox           = mpDataset->CropBox;
  this->Environment->Cursor3d          = mpDataset->Cursor3d;      
  this->Environment->Preset            = mpDataset->Preset;
  this->Environment->CadFindings       = mpDataset->CadFindings;
  
  // TODO: double check this with frank
  if( this->Is3D )
  {
    this->Environment->Camera->CopyFrom( mpDataset->VisualizationPresetCamera3d, this->Environment->Volume );
    this->Environment->Colormap = mpDataset->VisualizationPreset3d->Colormap;
    this->Environment->RenderingMode = safe_cast< RenderingMode ^ >(mpDataset->VisualizationPreset3d->RenderingMode->Clone() );
  }
  else
  {
    this->Environment->Camera = gcnew Camera();
    this->Environment->Colormap = mpDataset->VisualizationPreset2d->Colormap;
    this->Environment->RenderingMode = safe_cast< RenderingMode ^ >(mpDataset->VisualizationPreset2d->RenderingMode->Clone() );
  } 

  // copy over the extended elements
  System::Collections::IDictionaryEnumerator ^ mpEnumerator = mpDataset->ExtendedElements->GetEnumerator();
  while( mpEnumerator->MoveNext() )
  {
    if( dynamic_cast< Viatronix::Visualization::IElement ^ >( mpEnumerator->Value ) )
      this->Environment->AddElement( safe_cast< System::String ^ >( mpEnumerator->Key ), safe_cast< Viatronix::Visualization::IElement ^ >( mpEnumerator->Value ));
  }
  
} // Initialize( mpDataset )


/**
 * Resets the viewer to a default state.
 */
void Viewer::Reset()
{
  try
  {
    // TODO: revisit this
    Viatronix::Visualization::Graph::VisualizationGraph::Instance->Abort();

    m_mpViewerbar->Buttons->Clear();
    m_mpPopupMenu->Items->Clear();
    m_mpSupportedAnimations = gcnew array< System::Type ^ >( 0 );
    
    m_pViewer->Reset();

    m_mpEnvironment->Momentum->Reset();
    m_mpEnvironment->Cine->CineMode = CineModes::Idle;
    m_mpEnvironment->ResetEvents();
    m_mpEnvironment->Offset = 0;
    m_mpEnvironment->MaximumOffset = 0;
    m_mpEnvironment->ScrollFactor = 1;
    m_mpEnvironment->RenderingMode->Mode = RenderingAlgorithm::Default;

    m_bAllowSubView = true;

    this->ViewerType = gcnew ViewerTypeUndefined( m_mpEnvironment );
    this->SubViewerType = gcnew ViewerTypeUndefined( m_mpEnvironment );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Reset()


/**
 * set this viewer's environment as the active one
 */
void Viewer::SetActive()
{
  // set this viewer's environment as the active one
  vxEnvironment::SetActiveEnvironment( ToNativeEnvironmentPtr( m_mpEnvironment->GetEnvironmentPtr() ) );

} // SetActive()


/**
 * Called when the underlying window handle is created.
 *
 * @param   mpArgs    EventArgs containing data related to this event.
 */
void Viewer::OnHandleCreated( System::EventArgs ^ mpArgs )
{
  __super::OnHandleCreated( mpArgs );
  
  // create underlying unmanaged implementation
  m_pViewer->Create( reinterpret_cast< HWND >( this->Handle.ToInt32() ) );
  
  // intiailize the default viewer types
  this->ViewerType = gcnew ViewerTypeUndefined( m_mpEnvironment );
  this->SubViewerType = gcnew ViewerTypeUndefined( m_mpEnvironment );

} // OnHandleCreated( System::EventArgs ^ mpArgs )


/**
 * Returns true if the given key is used for input
 *
 * @param keyData the input key
 *
 * @return true if the given key is used for input
 */
bool Viewer::IsInputKey( System::Windows::Forms::Keys keyData )
{
  return keyData == System::Windows::Forms::Keys::Up   ||
    keyData == System::Windows::Forms::Keys::Down ||
    keyData == System::Windows::Forms::Keys::Left ||
    keyData == System::Windows::Forms::Keys::Right;
} // IsInputKey()
  

/**
 * Control's wndproc procedure.
 *
 * @param   mpMsg     windows message
 */
void Viewer::WndProc( System::Windows::Forms::Message % msg )
{    
  // TODO: fix this mess!!!!

  try
  {
    if( msg.Msg == WM_PAINT )
    {
      Redraw();
      ValidateRect( reinterpret_cast< HWND >( this->Handle.ToPointer() ), NULL );
    }
    else if( msg.Msg == vxNodeRendering::VX_REDRAW )
    {
      Redraw();
      return;
    }
    else if( msg.Msg == WM_RBUTTONDOWN )
    {
      // raise the click event
      this->InvokeOnClick( this, System::EventArgs::Empty );
      
      m_bRightButtonDown = true;
      m_lastRButtonDownMsg = msg;
      m_lastMousePosition = Control::MousePosition;
      return;
    }
    else if( msg.Msg == WM_RBUTTONUP && m_bRightButtonDown )
    {
      m_bRightButtonDown = false;
			this->SetActive();
      ContextMenu( this, System::EventArgs::Empty );
      return;
    }
    else if( msg.Msg == WM_MOUSELEAVE )
    {
      SetFocus( NULL );
    }
    else if( msg.Msg == WM_MOUSEMOVE )
    { 
      // is the right mouse button down?
      if( m_bRightButtonDown )
      {
        // compute the changed in mouse position since the button down message
        int iDelta = System::Math::Max( System::Math::Abs( m_lastMousePosition.X - Control::MousePosition.X ), System::Math::Abs( m_lastMousePosition.Y - Control::MousePosition.Y ) );

        // is the delata allowed?
        const int4 iAllowedMouseDelta = 6;
        if( iDelta > iAllowedMouseDelta )
        {
          // send the initial WM_RBBUTTONDOWN message to the intended recipient
          #ifdef _WIN64
            bool bReturn =
              m_pInputAdapter->AdaptMessage( reinterpret_cast<HWND>(m_lastRButtonDownMsg.HWnd.ToPointer()), m_lastRButtonDownMsg.Msg, m_lastRButtonDownMsg.WParam.ToInt64(), m_lastRButtonDownMsg.LParam.ToInt64() );
          #else
            bool bReturn =
              m_pInputAdapter->AdaptMessage( reinterpret_cast<HWND>(m_lastRButtonDownMsg.HWnd.ToPointer()), m_lastRButtonDownMsg.Msg, m_lastRButtonDownMsg.WParam.ToInt32(), m_lastRButtonDownMsg.LParam.ToInt32() );
          #endif
          m_bRightButtonDown = false;
        } // if( delta > m_allowedDelta )
      } // if( m_rightButtonDown )

      // is this first time we are entering this viewer?
      const bool bShiftKeyPressed( ( GetAsyncKeyState(VK_SHIFT) & 0x8000 ) != 0 );
      if( GetFocus() != (HWND) msg.HWnd.ToPointer() && bShiftKeyPressed == false )
      {
          m_mpEnvironment->SetPrimary();
          // force the focus to this viewer
          SetFocus( (HWND) msg.HWnd.ToPointer() ); 
          SetActive();
      }
    } // else if( msg.Msg == WM_MOUSEMOVE )

    // TODO: Get this out of here
    else if ( msg.Msg == WM_KEYDOWN )
    {
      this->InvokeOnClick( this, System::EventArgs::Empty );

      int key(msg.WParam.ToInt32());

      switch ( key )
      {
      case vxInput::KEY_P:
        {
          // if allowed and not control key
          const bool bCtrlKeyPressed( ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) != 0 );
          if ( m_bAllowSubView == true && bCtrlKeyPressed == false )
          {
            SetEnabled( ViewerTypes::Sub, ! GetEnabled( ViewerTypes::Sub ) );
            Redraw();
          }
        }
        break;
      case vxInput::KEY_Z:
        {
          const bool bCtrlKeyPressed( ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) != 0 );
          if ( bCtrlKeyPressed )
          {
						Viatronix::UndoActions::UndoActionStack::UndoLastAction();
            //m_pViewer->UndoAction();
          }
        }
        break;
      case vxInput::KEY_F12:
        {
          const bool bCtrlKeyPressed( ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) != 0 );
          if ( bCtrlKeyPressed )
          {
            vxVisualizationPreferences::SetShowStatistics( !vxVisualizationPreferences::GetShowStatistics() );
            Redraw();
          }
        }
        break;
      
      /// The following is for writing out the corrunt colormap in harddrive d:/temp/colormap.xml
      /// It is for development purpose only
      //case vxInput::KEY_END:
      //  {
      //    m_mpEnvironment->Colormap->ToXml();
      //  }
      //  break;

      }
    }
  
    // let the unmanged implementation handle the message
    #ifdef _WIN64
      bool bReturn =
        m_pInputAdapter->AdaptMessage( reinterpret_cast< HWND >( msg.HWnd.ToPointer() ), msg.Msg, msg.WParam.ToInt64(), msg.LParam.ToInt64() );
    #else
      bool bReturn =
        m_pInputAdapter->AdaptMessage( reinterpret_cast< HWND >( msg.HWnd.ToPointer() ), msg.Msg, msg.WParam.ToInt32(), msg.LParam.ToInt32() );
    #endif

    // special case for WM_SETCURSOR
    if( bReturn && msg.Msg == WM_SETCURSOR )
      return;

    // check the input feedback queue to see if any manipulator has finished working
    vxEnvironment * pEnvironment = static_cast< vxShareableObject< vxEnvironment > * >( m_mpEnvironment->GetEnvironmentPtr().ToPointer() );

    if( pEnvironment->GetInputFeedback().GetList().empty() == false )
    {
      std::list< vxFeedback > list = pEnvironment->GetInputFeedback().GetList();
      for( std::list< vxFeedback >::iterator iter = list.begin(); iter != list.end(); ++iter )
        Viatronix::Visualization::Events::RaiseToolCompleted( gcnew System::String( iter->GetId().name() ));

      pEnvironment->GetInputFeedback().Clear();
    }
    
    // let the base class do it's thing
    __super::WndProc( msg );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // WndProc()


/**
 * Adds a manipulator to the specified viewer
 *
 * @param   eViewerType     viewer to add manipulator to
 * @param   mpFactoryType   System::Type of Manipulator factory
 */
void Viewer::AddManipulator( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    IManipulatorFactory ^ mpFactory = dynamic_cast<IManipulatorFactory ^>( System::Activator::CreateInstance( mpFactoryType ) );
    if ( mpFactory == nullptr )
    {
      throw gcnew v3DException( System::String::Concat( "unable to create factory: ", mpFactoryType->ToString() ) );
    }
    
    vxManipulator * pManipulator = 
      reinterpret_cast< vxManipulator * >( mpFactory->CreateManipulator(  m_mpEnvironment->GetEnvironmentPtr() ).ToPointer() );

    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->AddManipulator( vxViewerControl::MainViewer, pManipulator, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->AddManipulator( vxViewerControl::SubViewer, pManipulator, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AddManipulator()


/**
 * Removes a manipulator from the specified viewer.
 *
 * @param   eViewerType     viewer to remove manipulator from
 * @param   mpFactoryType   System::Type of Manipulator factory
 */
void Viewer::RemoveManipulator( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->RemoveManipulator( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->RemoveManipulator( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveManipulator()


/**
 * Checks if a manipulator is contained within the specified viewer.
 *
 * @param   eViewerType     viewer to check
 * @param   mpFactoryType   System::Type of Manipulator factory
 * @return true if contanis manipulator, false otherwise
 */
bool Viewer::HasManipulator( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    bool bHasManipulator(false);

    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      bHasManipulator = m_pViewer->HasManipulator( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if( !bHasManipulator && (( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub ))
      bHasManipulator = m_pViewer->HasManipulator( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );

    return bHasManipulator;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // HasManipulator()


/**
 * Removes all manipulators from all viewers.
 */
void Viewer::RemoveAllManipulators()
{
  try
  {
    m_pViewer->ClearManipulators();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveAllManipulators()



/**
 * Adds a controller to the specified viewer
 *
 * @param   eViewerType     viewer to add controller to
 * @param   mpFactoryType   System::Type of controller factory
 */
void Viewer::AddController( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    IControllerFactory ^ mpFactory = dynamic_cast<IControllerFactory ^>( System::Activator::CreateInstance( mpFactoryType ) );
    
    vxController * pController = 
      reinterpret_cast< vxController * >( mpFactory->CreateController(  m_mpEnvironment->GetEnvironmentPtr() ).ToPointer() );

    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->AddController( vxViewerControl::MainViewer, pController, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->AddController( vxViewerControl::SubViewer, pController, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AddController()


/**
 * Removes a controller from the specified viewer.
 *
 * @param   eViewerType     viewer to remove controller from
 * @param   mpFactoryType   System::Type of Controller factory
 */
void Viewer::RemoveController( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->RemoveController( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->RemoveController( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveController()


/**
 * Checks if a controller is contained within the specified viewer.
 *
 * @param   eViewerType     viewer to check
 * @param   mpFactoryType   System::Type of controller factory
 * @return true if contanis controller, false otherwise
 */
bool Viewer::HasController( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    bool bHasController(false);

    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      bHasController = m_pViewer->HasController( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if( !bHasController && (( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub ))
      bHasController = m_pViewer->HasController( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );

    return bHasController;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // HasController()


/**
 * Removes all controllers from all viewers.
 */
void Viewer::RemoveAllControllers()
{
  try
  {
    m_pViewer->ClearControllers();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveAllControllers()



/**
 * Adds a renderer to the specified viewer.
 *
 * @param   eViewerType     viewer to add renderer to
 * @param   mpFactoryType   System::Type of Renderer factory
 */
void Viewer::AddRenderer( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    // create an instance of the renderer factory
    IRendererFactory ^ mpFactory = dynamic_cast<IRendererFactory ^ >( System::Activator::CreateInstance( mpFactoryType ) );
    
    // request that the factory create the actual vxRenderer
    vxRenderer * pRenderer = 
      reinterpret_cast< vxRenderer * >( mpFactory->CreateRenderer(  m_mpEnvironment->GetEnvironmentPtr()  ).ToPointer() ); 
    
    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->AddRenderer( vxViewerControl::MainViewer, pRenderer, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->AddRenderer( vxViewerControl::SubViewer, pRenderer, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AddRenderer()



/**
 * Removes a renderer from the specified viewer.
 *
 * @param   eViewerType     viewer to remove renderer from
 * @param   mpFactoryType   System::Type of Renderer factory
 */
void Viewer::RemoveRenderer( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      m_pViewer->RemoveRenderer( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if(( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub )
      m_pViewer->RemoveRenderer( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveRenderer()


/**
 * Checks if a renderer is contained within the specified viewer.
 *
 * @param   eViewerType       viewer to remove renderer from
 * @param   mpFactoryType     System::Type of Renderer factory
 * @return  true if contains renderer, false otherwise
 */
bool Viewer::HasRenderer( ViewerTypes eViewerType, System::Type ^ mpFactoryType )
{
  try
  {
    bool bHasRenderer(false);

    if(( eViewerType & ViewerTypes::Main ) == ViewerTypes::Main )
      bHasRenderer = m_pViewer->HasRenderer( vxViewerControl::MainViewer, ss( mpFactoryType->ToString() ) );
    
    if( !bHasRenderer && (( eViewerType & ViewerTypes::Sub ) == ViewerTypes::Sub ))
      bHasRenderer = m_pViewer->HasRenderer( vxViewerControl::SubViewer, ss( mpFactoryType->ToString() ) );

    return bHasRenderer;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // HasRenderer()


/**
 * Removes all renderers from all viewers.
 */
void Viewer::RemoveAllRenderers()
{
  try
  {
    m_pViewer->ClearRenderers();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveAllRenderers()


/**
 * Sets the main viewer's viewer type
 *
 * @param   mpViewerType  viewer type
 */
void Viewer::ViewerType::set( IViewerType ^ mpViewerType )
{
  try
  {
    m_pViewer->SetViewerType( vxViewerControl::MainViewer, reinterpret_cast< vxViewerType*>( mpViewerType->GetViewerTypePtr().ToPointer() ) );
    m_mpEnvironment->ViewerType = mpViewerType;

    // don't dispose the viewer type if it is being used by the sub-viewer
    if( m_mpViewerType != nullptr && m_mpViewerType != m_mpSubViewerType && m_mpViewerType != mpViewerType )
      delete m_mpViewerType;

    // reset the current rendering manager plugin
    if( m_mpRenderingManagerPlugin != nullptr )
      delete m_mpRenderingManagerPlugin;
    m_mpRenderingManagerPlugin = nullptr;

    m_mpViewerType = mpViewerType;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_ViewerType()


/**
 * Sets the sub viewer's viewer type
 *
 * @param   mpViewerType    viewer type
 */
void Viewer::SubViewerType::set( IViewerType ^ mpViewerType )
{
  try
  {  
    m_pViewer->SetViewerType( vxViewerControl::SubViewer, reinterpret_cast< vxViewerType*>( mpViewerType->GetViewerTypePtr().ToPointer() ) );

    // don't dispose the viewer type if it is being used by the main-viewer
    if( m_mpSubViewerType != nullptr && m_mpSubViewerType != m_mpViewerType )
      delete m_mpSubViewerType;
    m_mpSubViewerType =  mpViewerType;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_SubViewerType()


/**
 * Sets the sub viewer's viewer type
 *
 * @param   mpViewerType    viewer type
 */
Viatronix::Visualization::Graph::RenderingNodePlugin ^ Viewer::RenderingNodePlugin::get()
{
  try
  {  
    if( m_mpRenderingManagerPlugin == nullptr )
    {
      vxViewerType & viewerType = * reinterpret_cast< vxViewerType * >( m_mpViewerType->GetViewerTypePtr().ToPointer() );
      m_mpRenderingManagerPlugin = gcnew Viatronix::Visualization::Graph::RenderingNodePlugin( System::IntPtr( & viewerType.GetRenderingNodePlugin() ));
    }

    return m_mpRenderingManagerPlugin;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_SubViewerType()


/** 
 * Returns the enabled state of a specified viewer.
 *
 * @param   eViewerType     viewer to check
 */
bool Viewer::GetEnabled( ViewerTypes eViewerType )
{
  try
  {
    return  m_pViewer->GetEnabled( static_cast< vxViewerControl::ViewerTypeEnum >( eViewerType ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetEnabled()


/** 
 * Sets the enabled state of a specified viewer.
 *
 * @param   eViewerType   viewer to set
 * @param   bEnabled      enable/disable viewer
 */
void Viewer::SetEnabled( ViewerTypes eViewerType, bool bEnabled )
{
  try
  {
    m_pViewer->SetEnabled( static_cast< vxViewerControl::ViewerTypeEnum >( eViewerType ), bEnabled );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetEnabled()


/**
 * sets the size of a subviewer viewer
 *
 * @param       x            x proportion
 * @param       y            x proportion
 */
void Viewer::SetSubViewerSize( int4 x, int4 y )
{
  m_pViewer->SetSubViewerSize( x, y );
} // SetSubViewerSize( x, y )


/** 
 * Returns the screen shot for the entire viewer.
 *
 * @param   bPlaySound    Indicates if the snapshot sound should be played.
 * @return  snapshot of viewer
 */
System::Drawing::Bitmap ^ Viewer::GetScreenShot( bool bPlaySound )
{
  try
  {
    if( bPlaySound )
      PlaySound( "CAMERACLICK" , GetModuleHandle(), SND_ASYNC | SND_RESOURCE );

    // redraw without the viewerbar
    bool bViewerbarWasVisible = m_mpViewerbar->Visible;
    if ( bViewerbarWasVisible )
    {
      m_mpViewerbar->Visible = false;
      Refresh();
    }

    // width and height of image shaving off a 1 pixel border
    int4 iWidth(this->Width - 2);    
    int4 iHeight(this->Height - 2);    
    const int4 iScanlineSize = ( (iWidth * 24 + 31) & (~31) ) / 8;
    const int4 iBufferSize( iScanlineSize * iHeight );
    
    // allocate a temporary buffer to hold the image
    uint1 * pBufferRGB = new uint1[ iBufferSize ];

    // get the image from the unmanaged viewer implementation
    m_pViewer->GetImage( 1, 1, iWidth, iHeight, pBufferRGB );

    // create a managed bitmap to receive screen shot
    System::Drawing::Bitmap ^ mpBitmap = gcnew System::Drawing::Bitmap( iWidth, iHeight, System::Drawing::Imaging::PixelFormat::Format24bppRgb );
    
    // lock the down managed bitmap and grab a pointer to it's first scan line
    System::Drawing::Rectangle  bounds = System::Drawing::Rectangle( 0, 0, iWidth, iHeight );          
    System::Drawing::Imaging::BitmapData ^ mpData = mpBitmap->LockBits( bounds, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb );

    // copy the image into the managed bitmap
    memcpy( mpData->Scan0.ToPointer(), pBufferRGB, iBufferSize );
    
    // release the pixels
    mpBitmap->UnlockBits( mpData );
    
    // flip bitmap( top-down / bottom-up thing )
    mpBitmap->RotateFlip( System::Drawing::RotateFlipType::RotateNoneFlipY );

    if( bPlaySound )
    {
      // invert the image (we invert all the bits, but VC swaps red and blue) and redraw
      // this better handles all possible image colors
      for ( int4 i=0; i<iBufferSize; i++ )
        pBufferRGB[i] = ~pBufferRGB[i];
      m_pViewer->DrawImage( 0, 0, iWidth, iHeight, pBufferRGB );

      // wait a bit
      Sleep( 100 );

      // un-invert the image and redraw
      for ( int4 i=0; i<iBufferSize; i++ )
        pBufferRGB[i] = ~pBufferRGB[i];
      m_pViewer->DrawImage( 0, 0, iWidth, iHeight, pBufferRGB );
    }

    // restore the viewerbar and image
    if ( bViewerbarWasVisible )
    {
      m_mpViewerbar->Visible = true;
      Refresh();
    }

    // release the temporary buffer
    delete[] pBufferRGB;
    
    return mpBitmap;
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Image()


/**
 * Retuns the color of the border.
 *
 * @return border color
 */
System::Drawing::Color Viewer::BorderColor::get()
{
  try
  {
    PixelRGBA< uint1 > color(m_pViewer->GetBorderColor());
    return System::Drawing::Color::FromArgb
    ( 
      color.A(), 
      color.R(), 
      color.G(), 
      color.B() 
    );  
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_BorderColor()


/**
 * Sets the color of the border.
 *
 * @param color new border color
 */
void Viewer::BorderColor::set( System::Drawing::Color color )
{
  try
  {
    PixelRGBA< uint1 > newColor(color.R, color.G, color.B, color.A );

    m_pViewer->SetBorderColor( newColor );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_BorderColor()


/**
 * Called when the viewer bar is clicked.
 *
 * @param     mpSender  Sender of this event
 * @param     mpArgs    EventArgs containg data related to this event
 */
void Viewer::OnViewerbarClicked( System::Object ^ mpSender, System::EventArgs ^ mpArgs )
{
  this->InvokeOnClick( this, System::EventArgs::Empty );
} // OnViewerbarClicked( System::Object ^ mpSender, System::EventArgs ^ mpArgs )


/**
 * Called when the control is resized.
 *
 * @param   mpArgs    EventArgs containg data related to this event.
 */
void Viewer::OnResize( System::EventArgs ^ mpArgs )
{
  __super::OnResize( mpArgs );
  m_mpViewerbar->RecalculateLayout();
} // OnResize( mpArgs )


/**
 * Called when a drag and drop operation is in progress and the mouse is over the view
 *
 * @param mpDragArgs drag and drop arguments
 */
void Viewer::OnDragOver( System::Windows::Forms::DragEventArgs ^ mpDragArgs )
{
  // retieve the data associated with this drop operation
  IDropSource ^ mpDropSource = safe_cast< IDropSource ^>( mpDragArgs->Data->GetData( "Viatronix.Viusalization.IDropSource", true ) );

  vxViewerType * pViewerType(ToNativeEnvironmentPtr( m_mpEnvironment->GetEnvironmentPtr() )->GetViewerType());

  // does this viewer support the active drop type?
  if( pViewerType->CanDragDrop( static_cast< vxViewerType::DragDropEnum>(mpDropSource->DropType ) ) )
  {
    // if the control key is down then we want to copy
    if ( mpDragArgs->KeyState & 0x8 && mpDropSource->IsSupportShare() )
      mpDragArgs->Effect = System::Windows::Forms::DragDropEffects::Copy;
    else if ( mpDropSource->IsSupportCopy() )
      mpDragArgs->Effect = System::Windows::Forms::DragDropEffects::Move;
    else 
      mpDragArgs->Effect = System::Windows::Forms::DragDropEffects::Link;
  }
  else
    mpDragArgs->Effect = System::Windows::Forms::DragDropEffects::None;
} // OnDragOver()


/**
 * Called when a drop operation is detected
 *
 * @param mpDragArgs drag and drop arguments.
 */
void Viewer::OnDragDrop( System::Windows::Forms::DragEventArgs ^ mpDragArgs )
{
  // retrieve the data associated with this drop operation
  IDropSource ^ mpDropSource = safe_cast< IDropSource ^>( mpDragArgs->Data->GetData( "Viatronix.Viusalization.IDropSource", true ) );

  // perform the drop operation
  mpDropSource->Drop( this, mpDragArgs->Effect == System::Windows::Forms::DragDropEffects::Copy  );

  // update the viewbar
  m_mpViewerbar->Update();
  m_mpViewerbar->Refresh();

  __super::OnDragDrop( mpDragArgs );
} // OnDragDrop()


/**
 * Redraws the viewer
 */
void Viewer::Redraw()
{
  vxNodeDrawing * pNode = dynamic_cast< vxNodeDrawing * >( m_pViewer->GetEnvironment().GetVisualizationGraph()->GetNode( vxNodeIDs::NodeDrawing ));
  if( pNode != NULL )
    pNode->Redraw( * m_pViewer );
} // Redraw()


// $Log: v3D_Viewer.cpp,v $
// Revision 1.54.2.2  2009/05/01 21:45:52  gdavidson
// Raised DragDrop event and added F12 option for viewer statistics
//
// Revision 1.54.2.1  2008/11/24 17:14:03  gdavidson
// Fixed redraw issue with subviewer display
//
// Revision 1.54  2007/10/31 14:37:10  gdavidson
// Issue #5835: Let the input adapter handle WM_MOUSELEAVE message
//
// Revision 1.53  2007/10/29 20:34:26  gdavidson
// Added a Redraw method
//
// Revision 1.52  2007/10/24 20:08:39  gdavidson
// Moved vxViewerImpl to vxViewer and renamed vxViewerControl
//
// Revision 1.51  2007/07/24 20:45:14  gdavidson
// Issue #5747: Set the rendering mode to default when the viewer resets
//
// Revision 1.50  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.49  2007/05/17 20:05:05  gdavidson
// Added SetSubViewerSize
//
// Revision 1.48  2007/05/03 21:35:39  gdavidson
// Added moved visualizationGraph to Viatronix.Visualization.Graph namespace
//
// Revision 1.47  2007/04/18 17:01:01  gdavidson
// Add IsBeating method
//
// Revision 1.45  2007/03/23 15:37:54  romy
// OnDragOver modified to handle PresetGallery Move options
//
// Revision 1.44.2.1  2007/03/22 21:37:17  gdavidson
// Issue #5505: Stop the visualizationGraph timer when resetting the viewer
//
// Revision 1.44  2007/02/27 20:41:46  dongqing
// add debug code for writing out the current colormap
// for the viewer to the hard drive d:/temp/colormap.xml
//
// Revision 1.43  2007/02/05 20:30:55  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.42  2007/02/05 19:29:12  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.41  2007/02/05 15:57:34  romy
// added code to handle the Shift Key operation
//
// Revision 1.40  2007/01/24 14:40:50  gdavidson
// Set rendering manager plugin to null when viewertype changes
//
// Revision 1.39  2007/01/18 21:48:49  gdavidson
// Added clearing of Environment's modified bits
//
// Revision 1.38  2007/01/17 16:42:54  geconomos
// remove unused debug output
//
// Revision 1.37  2007/01/17 14:09:01  geconomos
// reset method enhanced
//
// Revision 1.36  2007/01/16 17:10:32  gdavidson
// Added RenderingNodePlugin, Cine and Momentum
//
// Revision 1.35  2007/01/12 21:35:59  romy
// Reset method modifies to Stop the HQ Rendering
//
// Revision 1.34  2006/12/11 20:12:44  geconomos
// Issue# 5227: crash when switching orientation; viewer type problem
//
// Revision 1.33  2006/10/24 12:35:09  geconomos
// removed old slice shadows mechanism
//
// Revision 1.32  2006/10/12 14:58:12  geconomos
// disposing existing viewer types before assigning new ones
//
// Revision 1.31  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.30  2006/09/28 21:08:45  romy
// moved undo actions to its on class
//
// Revision 1.29  2006/09/27 17:06:28  geconomos
// updated for class name changes to InutAdapter and ViewerImpl
//
// Revision 1.28  2006/09/19 20:17:55  gdavidson
// Synchronizes dataset's extended elements with the viewer's extended elements
//
// Revision 1.27  2006/09/19 19:00:05  romy
// added SetActive method in MouseUp event
//
// Revision 1.26  2006/09/19 18:11:21  gdavidson
// Issue #4989, #4990, #4992, #4997: Ignored alpha channel
//
// Revision 1.25  2006/08/28 21:45:36  gdavidson
// Changed ToolCompleted delegate
//
// Revision 1.24  2006/08/23 18:02:10  geconomos
// added support for VX_REDRAW flag
//
// Revision 1.23  2006/08/03 14:24:11  geconomos
// temporary support for old and new rendering mechanisms
//
// Revision 1.22  2006/07/14 13:32:43  geconomos
// more code cleanup
//
// Revision 1.21  2006/07/12 19:33:56  geconomos
// code clean up
//
// Revision 1.20  2006/04/25 20:22:34  romy
// StackModifiedFuncPtr added
//
// Revision 1.19  2006/04/25 15:16:12  romy
// Undo action exposed
//
// Revision 1.18  2006/03/24 14:43:34  gdavidson
// Removed focus when the mouse leaves the viewer
//
// Revision 1.17  2006/02/09 14:10:06  geconomos
// corrected calls to wndproc for 64-bit builds
//
// Revision 1.16  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.15  2005/12/08 16:43:34  frank
// added undo support
//
// Revision 1.14  2005/11/22 02:10:07  frank
// fixed problem with overwriting cameras when changing camera orientation
//
// Revision 1.13  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.12.2.3  2005/07/08 15:51:56  frank
// fixed viewerbar and inversion problem during movie creation
//
// Revision 1.12.2.2  2005/07/06 15:04:47  frank
// removed the viewerbar (and the black-out rectangle) from snapshots
// inverted the image during snapshot
//
// Revision 1.12.2.1  2005/06/23 18:32:00  frank
// selected viewer upon key click
//
// Revision 1.12  2005/06/07 19:00:36  vxconfig
// cleared the space under the viewerbar in snapshots so that it does not show OpenGL junk
//
// Revision 1.11  2001/01/03 19:16:47  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.10  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.9.2.2  2005/02/17 21:01:31  geconomos
// updated for change in visualization presets
//
// Revision 1.9.2.1  2005/02/09 15:25:04  geconomos
// Changed ShowViewerPopup event name to "ContextMenu"
//
// Revision 1.9  2001/01/01 16:03:40  frank
// Enabled arrow key usage
//
// Revision 1.8  2004/08/17 14:53:49  geconomos
// Reimplemented viewer creatation and destruction to utilize OnHandleCreated and OnHandleDestroyed control methods.
//
// Revision 1.7  2004/07/09 18:40:01  geconomos
// Wired up to the viewerbar's "Click" event in order to set the viewer as selected when the viewerbar is initially clicked.
//
// Revision 1.6  2004/06/15 19:47:39  geconomos
// Moved context menu support from MainForm.cs to this class.
//
// Revision 1.5  2004/05/25 14:27:51  frank
// improved error reporting
//
// Revision 1.4  2004/05/20 12:47:54  frank
// added controllers
//
// Revision 1.3  2004/04/07 20:32:54  geconomos
// Removed NotifyParent method.
//
// Revision 1.2  2004/03/30 15:11:15  geconomos
// Added property for border width.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.94  2004/02/09 15:35:50  frank
// Added initialization interface support
//
// Revision 1.93  2004/01/10 11:01:58  geconomos
// Added resize methods to reposition viewerbar when viewer is resized or moved.
//
// Revision 1.92  2004/01/07 20:01:35  frank
// typo
//
// Revision 1.91  2004/01/07 18:55:50  frank
// Forced a redraw when you hide the pan/zoom window
//
// Revision 1.90  2004/01/07 18:32:34  frank
// Used standard key codes rather than hardcoded values
//
// Revision 1.89  2003/12/17 01:02:22  geconomos
// Added call to Environment->ResetEvents in Reset.
//
// Revision 1.88  2003/11/19 13:15:07  frank
// Removed explicit rendering mode reset in viewer reset
//
// Revision 1.87  2003/11/11 12:34:59  geconomos
// Added call to rendering mode reset in vioewer's reset.
//
// Revision 1.86  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.85  2003/11/05 13:39:05  frank
// Added the ability to handle the right mouse using manipulators instead of the popup menu
//
// Revision 1.84  2003/10/15 01:27:31  geconomos
// Added ExcludedRenderingModes property.
//
// Revision 1.83  2003/10/03 12:14:18  michael
// took out log messages that were used for debugging
//
// Revision 1.82  2003/10/01 00:45:31  geconomos
// Changed implementation of GetSnapshot() method.
//
// Revision 1.81  2003/09/29 17:23:28  michael
// added a mechanism to kill all ongoing timers
//
// Revision 1.80  2003/09/16 16:03:21  frank
// Exposed function to set viewer type active
//
// Revision 1.79  2003/09/05 14:59:03  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.78  2003/08/29 18:46:52  frank
// Prevented sub view upon ctrl-P
//
// Revision 1.77  2003/08/22 03:45:18  geconomos
// Added supported annotations propery.
//
// Revision 1.76  2003/08/20 14:28:55  michael
// added "mailto:"
//
// Revision 1.75.2.1  2003/08/17 21:50:50  geconomos
// Changed return type of TakeScreenShot() fron Snapshot to System::Dragwing::Bitmap.
//
// Revision 1.75  2003/08/01 21:36:12  michael
// changed order in Dispose() so that viewers (and renderers) are destroyed before
// the data they depend on is destroyed. Previously, this caused an exception
// whenever there was still some thread of the 3D renderer running.
//
// Revision 1.74  2003/07/24 13:04:19  michael
// added redraw border instead of causing a full Redraw on the viewer and its
// clients (renderers)
//
// Revision 1.73  2003/07/17 17:34:06  geconomos
// GetScreenShot() now returns a Snaphot object.
//
// Revision 1.72  2003/07/03 16:56:20  geconomos
// Changed property Snapshot to GetScreenShot
//
// Revision 1.71  2003/05/27 19:10:18  geconomos
// Made viewer active when clicking the viewerbar.
//
// Revision 1.70  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.69  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.68  2003/05/13 13:52:11  geconomos
// More coding standards.
//
// Revision 1.67  2003/05/13 13:50:04  geconomos
// Coding standards.
//
// Revision 1.66  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.65  2003/04/25 13:46:26  geconomos
// Added popupMenu property.
//
// Revision 1.64  2003/04/23 20:10:59  geconomos
// Renamed event ModeCompleted to ToolCompleted
//
// Revision 1.63  2003/04/16 18:50:22  geconomos
// Issue #3110:  Buttons not refreshing after drop event.
//
// Revision 1.62  2003/03/30 21:00:29  michael
// added viewertype to environment
//
// Revision 1.61  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.60  2003/03/20 21:25:34  frank
// Removed extra toggling of rendering mode
//
// Revision 1.59  2003/03/20 13:44:55  geconomos
// Added ViewerType and SubViewerType properties.
//
// Revision 1.58  2003/03/17 20:45:26  geconomos
// Added a property to allow/disallow the sub view window.
//
// Revision 1.57  2003/03/17 19:48:01  geconomos
// Issue #2914. Implemented DropType property from vxViewer.
//
// Revision 1.56  2003/03/11 18:44:39  geconomos
// Set the ActiveViewerType in vxEnvironment when entering viewer.
//
// Revision 1.55  2003/03/06 16:02:08  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.54  2003/03/04 15:35:53  geconomos
// Added RemoveAllRenderers and RemoveAllManipulators methods.
//
// Revision 1.53  2003/02/19 15:23:14  geconomos
// More coding conventions.
//
// Revision 1.52  2003/01/09 19:04:35  geconomos
// Coding conventions.
//
// Revision 1.51  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.50  2002/12/17 14:42:37  michael
// took out redundant include of vxWinRect
//
// Revision 1.49  2002/11/27 23:36:27  michael
// ScrollFactor needs to be reset to '1', not '0' which broke page-up/down for plane manipulators
//
// Revision 1.48  2002/11/27 21:36:08  geconomos
// Setting of plane offset stuff in Reset( should be moved elsewhere )
//
// Revision 1.47  2002/11/26 18:11:29  geconomos
// enabled snapshot sound ( I hate it  )
//
// Revision 1.46  2002/11/26 14:46:00  geconomos
// Now checking vxInputFeedback list for manipulation completion.
//
// Revision 1.45  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.44  2002/11/25 13:55:58  geconomos
// Mouse cursor handling.
//
// Revision 1.43  2002/11/20 15:25:49  geconomos
// Removed the call to m_mpEnvironment->RemoveAllElements in Reset()
//
// Revision 1.42  2002/11/20 13:48:54  geconomos
// Refined snapshot image to remove border around viewer.
//
// Revision 1.41  2002/11/18 20:37:33  geconomos
// New viewerbar implementation
//
// Revision 1.40  2002/11/15 21:46:26  geconomos
// Updated to use new ViewType enum value from vxUtils.
//
// Revision 1.39  2002/11/14 13:41:42  geconomos
// fxied typos in HasManipulator.
//
// Revision 1.38  2002/11/13 15:20:02  geconomos
// Set viewer active on middle and right click.
//
// Revision 1.37  2002/11/11 21:23:24  geconomos
// Allowed for ORing of ViewerTypes while add/removing manipulators and renderers.
//
// Revision 1.36  2002/11/07 13:02:19  geconomos
// Added border color property to viewer.
//
// Revision 1.35  2002/11/06 22:02:36  geconomos
// Added HasRenderer and HasManipulator methods.
//
// Revision 1.34  2002/11/05 20:52:17  geconomos
// Implementation of new viewerbar
//
// Revision 1.33  2002/11/04 14:30:36  geconomos
// new toolbar class.
//
// Revision 1.32  2002/10/31 12:51:55  geconomos
// Updated for change to Environment::GetEnvironmentPtr().
//
// Revision 1.31  2002/10/21 15:43:33  geconomos
// Removed some keyboarding handling routines to application classes.
//
// Revision 1.30  2002/10/07 18:55:04  geconomos
// isuue# 2706: Application losing focus after selecting plaque.
//
// Revision 1.29  2002/10/04 18:13:44  geconomos
// Moved unmanged toolbar class into the managed wrapper.
//
// Revision 1.28  2002/09/26 19:04:43  dmitry
// RemoveAllElements called in Reset.
//
// Revision 1.27  2002/09/26 18:08:16  dmitry
// P hot key moved here.
//
// Revision 1.26  2002/09/25 17:00:04  geconomos
// Imporved message handling.
//
// Revision 1.25  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// Revision 1.24  2002/09/24 15:16:26  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Viewer.cpp,v 1.54.2.2 2009/05/01 21:45:52 gdavidson Exp $
// $Id: v3D_Viewer.cpp,v 1.54.2.2 2009/05/01 21:45:52 gdavidson Exp $
