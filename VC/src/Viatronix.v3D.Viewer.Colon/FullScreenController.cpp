// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FullScreenController.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "FullScreenController.h"
#include "MainAppFrame.h"
#include <algorithm>
#include "SliceViewer.h"
#include "DatasetUtils.h"
#include "ViatronViewWnd.h"


USING_COLON_NS


/** 
 * singleton
 */
FullScreenController & FullScreenController::GetInstance() 
{
  static FullScreenController instance; 
  return instance; 
} // GetInstance() 



/**
 * constructor
 */
FullScreenController::FullScreenController()
{
  MainAppFrame::GetMainFrame()->Connect( this );
} // FullScreenController()


/**
 * destructor
 */
FullScreenController::~FullScreenController()
{
  if( MainAppFrame::GetMainFrame() != nullptr )
    MainAppFrame::GetMainFrame()->Disconnect( this );
} // ~FullScreenController()


/**
 * updates the SliceChildViewState for the specified layout
 *
 * @param       view            Active layout
 */
void FullScreenController::UpdateFullScreenViewerState( ColonViews::Layouts::GLWnd & view )
{
  StateVector::iterator iter = std::find_if( m_viewStates.begin(), m_viewStates.end(), []( ColonViews::ChildViews::SliceChildView::SliceChildViewState & state ) { return state.m_bFullScreen; });
  if( iter != m_viewStates.end() )
  {
    for( int4 i = 0; i < view.NumGLViews(); ++i )
    {
      GLChildViewPtr pChild = &view.GetChildViewByPos( i );
      SliceChildView * pSliceChild( dynamic_cast< SliceChildView * >( pChild ));
      if( pSliceChild )
      {
        ColonViews::ChildViews::SliceChildView::SliceChildViewState state = pSliceChild->GetSliceChildViewState();
        iter->m_windowLevel = state.m_windowLevel;
        iter->m_eOrientation = state.m_eOrientation;
        break;
      }
    }
  }
} // UpdateState( view )


/**
 * stores the SliceChildViewState for the specified layout
 *
 * @param       view            Active layout
 */
void FullScreenController::PersistLayout( ColonViews::Layouts::GLWnd & view )
{
  for( int4 i = 0; i < view.NumGLViews(); ++i )
  {
    GLChildViewPtr pChild = &view.GetChildViewByPos( i );
    SliceChildView * pSliceChild( dynamic_cast< SliceChildView * >( pChild ));
    if( pSliceChild )
    {
      // retrieve the viewer state and determine if the viewer is allowed to adjust its orientation
      ColonViews::ChildViews::SliceChildView::SliceChildViewState state = pSliceChild->GetSliceChildViewState();
      state.m_bSetOrientation = ( view.OnShowPopupMenu( *pSliceChild, IDM_SLICE_ORIENTATION ) == IDM_SLICE_ORIENTATION );
      m_viewStates.push_back( state );

    }
  }
} // PersistLayout( view )


/**
 * sets up the specified layout with the stored SliceChildViewState
 *
 * @param       view            Active layout
 */
void FullScreenController::SetupLayout( ColonViews::Layouts::GLWnd & view )
{
  if( m_viewStates.empty() )
    throw ex::RuntimeException( LogRec( "IllegalOperation FullScreenController::SetupLayout must be matched to a previous call to FullScreenController::Persistlayout.", "FullScreenController", "SetupLayout" ));

  // if datasets are linked and the layout supports multiple datasets then set every viewer's window level equal to the full screen viewer
  if( view.IsDisplayingMultipleDatasets() && Utilities::Datasets::IsLocked() )
  {
    ColonViews::ChildViews::SliceChildView::SliceChildViewState fullscreenState;
    if( GetFullScreenState( fullscreenState ))
    {
      std::for_each( m_viewStates.begin(), m_viewStates.end(), [&]( ColonViews::ChildViews::SliceChildView::SliceChildViewState & state ) 
      { 
        state.m_windowLevel = fullscreenState.m_windowLevel; 
        state.m_eOrientation = fullscreenState.m_eOrientation;
      });
    }
  }

  int4 iStateIndex = 0;
  for( int4 i = 0; i < view.NumGLViews(); ++i )
  {
    GLChildViewPtr pChild = &view.GetChildViewByPos( i );
    SliceChildView * pSliceChild( dynamic_cast< SliceChildView * >( pChild ));
    if( pSliceChild )
      pSliceChild->SetSliceChildViewState( m_viewStates[ iStateIndex++ ]);
  }

  m_viewStates.clear();

  // send a notification
  Utilities::Datasets::Lock( Utilities::Datasets::IsLocked() );

} // SetupLayout( view )


/**
 * returns the SliceChildViewState for the full screen viewer
 *
 * @param       state            slice child view state
 * return       true if viewer is found; otherwise false
 */
bool FullScreenController::GetFullScreenState( ColonViews::ChildViews::SliceChildView::SliceChildViewState & state )
{
  StateVector::iterator iter = std::find_if( m_viewStates.begin(), m_viewStates.end(), []( ColonViews::ChildViews::SliceChildView::SliceChildViewState & state ) { return state.m_bFullScreen; });
  if( iter != m_viewStates.end() )
  {
    state = *iter;
    return true;
  }

  return false;
} // GetFullScreenState( state )


/**
 * Notifies any observers that the report state has been changed
 */
void FullScreenController::Notify( const FullScreenArgs & args )
{
  if( m_bDelayNotification && !args.IsFullScreen() )
    m_pArgs = std::make_shared< FullScreenArgs >( args );
  else
    ObservableObject::Notify( args );
} // Notify( args )


/**
 * Observers the state of the MainAppFrame
 */
void FullScreenController::OnNotified( const MainAppFrameStates & args )
{
  switch( args )
  {
    case MainAppFrameStates_SubViewChanging:
      m_bDelayNotification = true;
      break;

    case MainAppFrameStates_SubViewChanged:
      m_bDelayNotification = false;

      if( m_pArgs != nullptr )
      {
        Notify( *m_pArgs.get() );
        m_pArgs = nullptr;
      }
      break;
  }    
} // OnNotified( args )


/**
 * constructor
 */
FullScreenArgs::FullScreenArgs( bool bFullscreen ) :
  m_bIsFullscreen( bFullscreen )
{
} // FullScreenArgs(