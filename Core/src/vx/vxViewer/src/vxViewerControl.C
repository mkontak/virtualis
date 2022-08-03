// $Id: vxViewerControl.C,v 1.3 2008/05/02 04:57:46 cqian Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewerControl.h"
#include "vxRendererSoftwareMPROrtho.h"
#include "vxRendererCpu3D.h"
#include "vxViewer.h"
#include "vxWinRect.h"
#include "vxEnvironment.h"
#include "vxManipulator.h"
#include "vxController.h"
#include "vxRenderer.h"
#include "GlFontGeorge.h"
#include "vxModifiedQueue.h"
#include "vxUndoAction.h"
#include "vxUndoActionStack.h"
#include "vxVisualizationPreferences.h"
#include "vxCine.h"
#include "vxRenderingContextDIB.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"
#define NUMBER_OF_VIEWERS 3


// namespaces
using namespace std;


/**
 * Default Constuctor.
 *
 * @param pEnvironment vxEnvironment pointer
 */
vxViewerControl::vxViewerControl( vxShareableObject<vxEnvironment> * pEnvironment ) :
  m_pEnvironment( pEnvironment ),
  m_bDrawBorder( true ),
  m_borderColor( PixelRGBA<uint1>( 0, 0, 0, 255 )),
  m_fBorderWidth( 1.0F )
{  
  // create the "Main View"
  vxViewer * pViewer = new vxViewer( *m_pEnvironment );
  vxWinRect newRect( pViewer->GetWinRect() );
  newRect.SetBorder( 1, 1, 1, 1 );
  pViewer->Resize( newRect );
  
  AddViewer( vxViewerControl::MainViewer, pViewer );
  
  // create the "Sub View"
  pViewer = new vxViewer( *m_pEnvironment );
  vxWinRect winRect(vxWinRect::PROPORTIONAL, vxWinRect::LOWER_RIGHT);
  winRect.SetProportion(std::make_pair(25, 25));
  const uint4 uMaxSize( 130 );
  if ( winRect.GetViewportWidth () > uMaxSize ) winRect.SetViewportSize( make_pair( uMaxSize, winRect.GetViewportHeight() ) );
  if ( winRect.GetViewportHeight() > uMaxSize ) winRect.SetViewportSize( make_pair( winRect.GetViewportWidth(), uMaxSize  ) );
  winRect.SetBorder(1,1,1,1);
  pViewer->Resize(winRect);
  pViewer->SetEnabled( false );
  
  AddViewer( vxViewerControl::SubViewer, pViewer );
 
} // vxViewerControl()


/**
 * Destructor.
 */
vxViewerControl::~vxViewerControl()
{
  Destroy();
} // ~vxViewerControl()


/**
 * Creates the opengl context and associated it with the specified window handle
 *
 * @param hwnd Window handle of this viewer.
 * @return true if successful; false otherwise
 */
bool vxViewerControl::Create( HWND hwnd )
{
  try
  {
    // add the window's handle to the associated environment
    m_windowHandle.SetHandle( hwnd );
    m_pEnvironment->AddElement( vxIDs::WindowHandle, &m_windowHandle );

    // NOTE: Make sure that any initialization code that depends on the having a active
    // renderering context follows this code!
    //m_pEnvironment->GetContext()->MakeCurrent();

    // get the width and height of this window
    RECT clientRect;
    GetClientRect( hwnd, &clientRect );
    uint2 uWidth(clientRect.right - clientRect.left);
    uint2 uHeight(clientRect.bottom - clientRect.top);

    // failsafe for bad client rects
    if ( uWidth == 0 || uHeight == 0 ) uWidth = uHeight = 10;

    // for each viewer set the overall window size and resize
    for ( Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter )
    {
      (*iter).second->m_pViewer->Initialize();
      
      vxWinRect winRect((*iter).second->m_pViewer->GetWinRect());
      winRect.SetWindowSize( make_pair( LOWORD( uWidth ), HIWORD( uHeight ) ) );
      
      (*iter).second->m_pViewer->Resize(winRect);
    }
  }
  catch( ex::VException & )
  {
    LogErr( "Create failed", "vxViewerImpl", "Create" );
    return false;
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception in Create", "vxViewerImpl", "Create" );
    return false;
  }
  return true;
} // OnCreate()


/**
 * Destroys the opengl rendering context.
 */
void vxViewerControl::Destroy()
{
  try
  {
    Reset();

    // set the context active
    m_pEnvironment->GetContext()->MakeCurrent();

    // free all vxViewers
    for( Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter )
    {
      (*iter).second->m_pViewer->Uninitialize();
      delete (*iter).second->m_pViewer;
      (*iter).second->m_pViewer = NULL;      
    }
    m_viewers.clear();
  } // try
  catch ( ex::VException & )
  {
    LogErr( "exception", "vxViewerImpl", "Destroy" );
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "vxViewerImpl", "Destroy" );
  }

} // Destroy()


/**
 * Resets the viewer to a default state.
 *
 * @return S_OK on success.
 */
void vxViewerControl::Reset()
{

  try
  {
    m_pEnvironment->GetContext()->MakeCurrent();

    // remove all objects
    ClearRenderers();
    ClearManipulators();
    ClearControllers();
    
    // reset all viewers
    for (Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter)
      (*iter).second->m_pViewer->Reset();

    // hide the sub viewer
    SetEnabled( vxViewerControl::SubViewer, false );

    // for sure no dangling pointer remains in the environment
    //m_pEnvironment->SetActiveEnvironment( NULL );
    // We replace the following line codes with the line under it.
    // This may cause some memory leak. However, it hopes to solve the 
    // dangling pointer crush later on viewType. 
    // This modification was made when we debug the crash at Vascular
    // session load of RSNA 2003 version. (Dongqing 12-15-2003)
    //m_pEnvironment->SetViewerType( NULL );
    //m_pEnvironment->SetViewerType( new vxViewerTypeUndefined( * m_pEnvironment ) );

    m_pEnvironment->UpdateFont();

  } // try
  catch (ex::VException &)
  {
    LogErr( "exception caught", "vxViewerImpl", "Reset" );
  }
  catch ( ... )
  {
    LogErr( "unhandled exception in destructor", "vxViewerImpl", "Reset" );
  }

} // Reset()


/**
 * Adds the specified viewer to the top z order position.
 *
 * @param eViewerType Viewer's type.
 * @param pViewer A pointer to the new viewer.
 * @return The number of viewers in the collection.
 */
uint4 vxViewerControl::AddViewer( ViewerTypeEnum eViewerType, vxViewer * pViewer )
{
  if (pViewer != NULL)
  {
    ViewerStruct * viewer = new ViewerStruct;
    viewer->m_pViewer = pViewer;
    
    m_viewers.insert(std::make_pair( (int)eViewerType, viewer ));
  }
  return m_viewers.size();
} // AddViewer()


/**
 * Gets the specified viewer.
 *
 * @param eViewerType Viewer type of viewer to retrieve.
 */
vxViewerControl::ViewerStruct * vxViewerControl::GetViewerStruct( ViewerTypeEnum eViewerType )
{
  Viewers::iterator iter(m_viewers.find( (int)eViewerType ));

  return (*iter).second;
} // GetViewer()


/**
 * Sets the viewer type for the specified viewer.
 *
 * @param eViewerType viewer to set
 * @param pViewerType new viewer 
 */
void vxViewerControl::SetViewerType( ViewerTypeEnum eViewerType, vxViewerType * pViewerType )
{
  vxViewer * pViewer( GetViewer( eViewerType ));
  
  pViewer->SetViewerType( pViewerType );

  if( eViewerType == vxViewerControl::MainViewer )
  {
    pViewer->GetEnvironment().SetViewerType( pViewerType );
    pViewer->GetEnvironment().SetWinRect( pViewer->GetWinRect() );
  }
} // SetViewerType()


/** 
 * Adds a renderer to the specified viewer.
 *
 * @param eViewerType Type of viewer to add renderer to.
 * @param pRenderer Renderer to add.
 * @param sType Textual representation of renderer type.
 */
void vxViewerControl::AddRenderer( ViewerTypeEnum eViewerType, vxRenderer * pRenderer, const std::string & sType )
{
  GetViewerStruct( eViewerType )->m_renderers.insert( std::make_pair( sType, pRenderer ) );
  GetViewerStruct( eViewerType )->m_pViewer->AddRenderer( pRenderer );

  m_pEnvironment->GetContext()->MakeCurrent();
  pRenderer->Initialize();  //doing nothing
} // AddRenderer()


/** 
 * Removes the renderer from the specified viewer.
 *
 * @param eViewerType  Type of viewer to remove renderer from.
 * @param sType Textual representation of renderer type.
 */
void vxViewerControl::RemoveRenderer( ViewerTypeEnum eViewerType, const std::string & sType )
{
  m_pEnvironment->GetContext()->MakeCurrent();

  ViewerStruct * pViewerStruct = GetViewerStruct( eViewerType );

  // get the other viewer type( i.e. if viewerTppe is Main that the other type is sub and vice versa )
  ViewerTypeEnum eOtherViewerType = ( eViewerType == vxViewerControl::MainViewer ) ? vxViewerControl::SubViewer : vxViewerControl::MainViewer;

  // loop through all renderers of this type
  Renderers::iterator rendererIterator;
  while( ( rendererIterator = pViewerStruct->m_renderers.find( sType ) ) != pViewerStruct->m_renderers.end() )
  {
    vxRenderer * pRenderer(rendererIterator->second);

    // remove the renderer from the viewer
    pViewerStruct->m_pViewer->RemoveRenderer( pRenderer );
    pViewerStruct->m_renderers.erase( rendererIterator );

    // check if the other view contains this renderer
    bool bCanDelete(true);
    Renderers::iterator otherRendererIterator(GetViewerStruct( eOtherViewerType )->m_renderers.find( sType ));
    while ( otherRendererIterator != GetViewerStruct( eOtherViewerType )->m_renderers.end() )
    {
      vxRenderer * pOtherRenderer(otherRendererIterator->second);

      // are they the same renderer
      if ( pRenderer == pOtherRenderer )
      {
        bCanDelete = false;
        break;
      }      
      otherRendererIterator++;
    }

    // if renderer is not found in the other view we can delete it
    if ( bCanDelete == true)
    {
      pRenderer->Uninitialize();
      delete pRenderer;
    }
  } // while( ( rendererIterator = pViewerStruct->m_renderers.find( sType ) ) != pViewerStruct->m_renderers.end() )
} // RemoveRenderer()


/** 
 * Checks if the renderer is contained within the specified viewer.
 *
 * @param eViewerType Type of viewer to check for renderer.
 * @param sType Textual representation of renderer type.
 */
bool vxViewerControl::HasRenderer( ViewerTypeEnum eViewerType, const std::string & sType )
{ 
  Renderers::iterator iter(GetViewerStruct( eViewerType )->m_renderers.find( sType ));

  return iter != GetViewerStruct( eViewerType )->m_renderers.end();
} // HasRenderer()


/**
 * Removes all renderers from all viewers.
 */
void vxViewerControl::ClearRenderers()
{
  try
  {
    // collection of renderers to be deleted
    std::map< vxRenderer *, bool > deleteRenderers;

    // for each vxViewer clear out the list of renderers
    for ( Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter )
    {
      (*iter).second->m_pViewer->GetRendererList().clear();

      // free all renderers and empty the renderers list
      for ( Renderers::iterator rIter((*iter).second->m_renderers.begin()); rIter != (*iter).second->m_renderers.end(); ++rIter )
      {
        deleteRenderers.insert( std::make_pair( (*rIter).second, true ) );
      }
      (*iter).second->m_renderers.clear();
    }  

    std::map< vxRenderer *, bool >::iterator iterDelete = deleteRenderers.begin();
    for ( ; iterDelete != deleteRenderers.end(); ++iterDelete )
    {
      (*iterDelete).first->Uninitialize();
      delete (*iterDelete).first;
    }
  } // try
  catch (ex::VException &)
  {
    LogErr("ClearRenderers failed.", "vxViewerImpl", "ClearRenderers");
  }

} // ClearRenderers()


/** 
 * Adds a controller to the specified viewer.
 *
 * @param eViewerType Type of viewer to add renderer to.
 * @param pController Controller to add.
 * @param sType Textual representation of controller type.
 */
void vxViewerControl::AddController( ViewerTypeEnum eViewerType, vxController * pController, const std::string & sType )
{
  GetViewerStruct( eViewerType )->m_controllers.insert( std::make_pair( sType, pController ) );
  GetViewerStruct( eViewerType )->m_pViewer->AddController( pController );  
} // AddController()


/** 
 * Removes the controller from the specified viewer.
 *
 * @param eViewerType  Type of viewer to remove controller from.
 * @param sType Textual representation of controller type.
 */
void vxViewerControl::RemoveController( ViewerTypeEnum eViewerType, const std::string & sType )
{
  ViewerStruct * pStruct = GetViewerStruct( eViewerType );
  
  // get the other viewer type( i.e. if viewerTppe is Main that the other type is sub and vice versa )
  ViewerTypeEnum eOtherViewerType = ( eViewerType == vxViewerControl::MainViewer ) ? vxViewerControl::SubViewer : vxViewerControl::MainViewer;
 
  Controllers::iterator controllers(pStruct->m_controllers.find( sType ));
  if ( controllers != pStruct->m_controllers.end() )
  {
    pStruct->m_pViewer->RemoveController( controllers->second );

    vxController * pController(controllers->second);
    pStruct->m_controllers.erase( controllers );

    // check if the other view contains this renderer
    bool bCanDelete(true);
    Controllers::iterator otherController(GetViewerStruct( eOtherViewerType )->m_controllers.find( sType ));
    if ( otherController != GetViewerStruct( eOtherViewerType )->m_controllers.end() )
    {
      vxController * pOtherController(otherController->second);

      // are they the same renderer
      if ( pController == pOtherController )
      {
        bCanDelete = false;  
      }

      otherController++;
    }

    // if controller is not found in the other view we can delete it
    if ( bCanDelete == true)
    {
      delete pController;
    }
  }
} // RemoveController()


/** 
 * Checks if the controller is contained within the specified viewer.
 *
 * @param eViewerType Type of viewer to check for controller.
 * @param sType Textual representation of controller type.
 */
bool vxViewerControl::HasController( ViewerTypeEnum eViewerType, const std::string & sType )
{
  Controllers::iterator iter(GetViewerStruct( eViewerType )->m_controllers.find( sType ));

  return iter != GetViewerStruct( eViewerType )->m_controllers.end();
} // HasController()


/** 
 * Lists all the controllers within the viewer to the debug log
 *
 * @param eViewerType Type of viewer to check for controller.
 */
void vxViewerControl::ListControllers( ViewerTypeEnum eViewerType )
{
  Controllers::iterator iter( GetViewerStruct( eViewerType )->m_controllers.begin() );

  while ( iter != GetViewerStruct( eViewerType )->m_controllers.end() )
  {
    std::string sType = iter->first;
    LogDbg( "Controller type is: " + sType, "", "" );
    ++iter;
  }

} // ListController()


/**
 * Removes all controllers from all viewers.
 */
void vxViewerControl::ClearControllers()
{
  try
  {
    // for each vxViewer clear out the list of manipulators
    for ( Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter )
    {
      (*iter).second->m_pViewer->GetControllerList().clear();

      // free all controllers and empty the controllers list
      for ( Controllers::iterator mIter((*iter).second->m_controllers.begin()); mIter != (*iter).second->m_controllers.end(); ++mIter )
      {
        delete (*mIter).second;
      }
      (*iter).second->m_controllers.clear();
    }
  }
  catch (ex::VException &)
  {
    LogErr( "ClearControllers failed", "v3D_Viewer", "ClearControllers" );
  }
} // ClearControllers()


/** 
 * Adds a manipulator to the specified viewer.
 *
 * @param eViewerType Type of viewer to add renderer to.
 * @param pManipulator Manipulator to add.
 * @param sType Textual representation of manipulator type.
 */
void vxViewerControl::AddManipulator( ViewerTypeEnum eViewerType, vxManipulator * pManipulator, const std::string & sType )
{
  GetViewerStruct( eViewerType )->m_manipulators.insert( std::make_pair( sType, pManipulator ) );
  GetViewerStruct( eViewerType )->m_pViewer->AddManipulator( pManipulator );  
} // AddManipulator()


/** 
 * Removes the manipulator from the specified viewer.
 *
 * @param eViewerType  Type of viewer to remove manipulator from.
 * @param sType Textual representation of manipulator type.
 */
void vxViewerControl::RemoveManipulator( ViewerTypeEnum eViewerType, const std::string & sType )
{
  ViewerStruct * pStruct = GetViewerStruct( eViewerType );
  
  // get the other viewer type( i.e. if viewerTppe is Main that the other type is sub and vice versa )
  ViewerTypeEnum eOtherViewerType = ( eViewerType == vxViewerControl::MainViewer ) ? vxViewerControl::SubViewer : vxViewerControl::MainViewer;
 
  Manipulators::iterator manipulators(pStruct->m_manipulators.find( sType ));
  if ( manipulators != pStruct->m_manipulators.end() )
  {
    pStruct->m_pViewer->RemoveManipulator( manipulators->second );

    vxManipulator * pManipulator(manipulators->second);
    pStruct->m_manipulators.erase( manipulators );

    // check if the other view contains this renderer
    bool bCanDelete(true);
    Manipulators::iterator otherManipulator(GetViewerStruct( eOtherViewerType )->m_manipulators.find( sType ));
    if ( otherManipulator != GetViewerStruct( eOtherViewerType )->m_manipulators.end() )
    {
      vxManipulator * pOtherManipulator(otherManipulator->second);

      // are they the same renderer
      if ( pManipulator == pOtherManipulator )
      {
        bCanDelete = false;  
      }

      otherManipulator++;
    }

    // if manipulator is not found in the other view we can delete it
    if ( bCanDelete == true)
    {
      delete pManipulator;
    }
  }
} // RemoveManipulator()


/** 
 * Checks if the manipulator is contained within the specified viewer.
 *
 * @param eViewerType Type of viewer to check for manipulator.
 * @param sType Textual representation of manipulator type.
 */
bool vxViewerControl::HasManipulator( ViewerTypeEnum eViewerType, const std::string & sType )
{
  Manipulators::iterator iter(GetViewerStruct( eViewerType )->m_manipulators.find( sType ));

  return iter != GetViewerStruct( eViewerType )->m_manipulators.end();
} // HasManipulator()


/** 
 * Lists all the manipulators within the viewer to the debug log
 *
 * @param eViewerType Type of viewer to check for manipulator.
 */
void vxViewerControl::ListManipulators( ViewerTypeEnum eViewerType )
{
  Manipulators::iterator iter( GetViewerStruct( eViewerType )->m_manipulators.begin() );

  while ( iter != GetViewerStruct( eViewerType )->m_manipulators.end() )
  {
    std::string sType = iter->first;
    LogDbg( "Manipulator type is: " + sType, "", "" );
    ++iter;
  }

} // ListManipulator()


/**
 * Removes all manipulators from all viewers.
 */
void vxViewerControl::ClearManipulators()
{
  try
  {
    // for each vxViewer clear out the list of manipulators
    for ( Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter )
    {
      (*iter).second->m_pViewer->GetManipulatorList().clear();

      // free all manipulators and empty the manipulators list
      for ( Manipulators::iterator mIter((*iter).second->m_manipulators.begin()); mIter != (*iter).second->m_manipulators.end(); ++mIter )
      {
        delete (*mIter).second;
      }
      (*iter).second->m_manipulators.clear();
    }
  }
  catch (ex::VException &)
  {
    LogErr("ClearManipulators failed.", "v3D_Viewer", "ClearManipulators");
  }
} // ClearManipulators


/** 
 * Returns the enabled state of a specified viewer.
 *
 * @param eViewerType sub or main view
 */
bool vxViewerControl::GetEnabled( ViewerTypeEnum eViewerType )
{
  return GetViewerStruct( eViewerType )->m_pViewer->IsEnabled();
} // GetEnabled()


/** 
 * Sets the enabled state of a specified viewer.
 *
 * @param eViewerType sub or main view
 * @param bEnabled show/hides 
 */
void vxViewerControl::SetEnabled( ViewerTypeEnum eViewerType, bool bEnabled )
{ 
  GetViewerStruct( eViewerType )->m_pViewer->SetEnabled( bEnabled );
} // SetEnabled()


/**
 * sets the size of a subviewer viewer
 *
 * @param       x            x proportion
 * @param       y            x proportion
 */
void vxViewerControl::SetSubViewerSize( int4 x, int4 y )
{
  vxViewer * pViewer = GetViewerStruct( SubViewer )->m_pViewer;
  vxWinRect winRect = pViewer->GetWinRect();
  winRect.SetProportion( std::pair<uint2, uint2>( x, y ));
  pViewer->Resize( winRect );
} // SetSubViewerSize( x, y )


/** 
 * Returns data to the screen image.
 * @param x       the x position
 * @param y       the y position
 * @param iWidth  the image width
 * @param iHeight the image height
 * @param pData   the image data in BGRA/uint1 format
 */
void vxViewerControl::GetImage( int4 x, int4 y, int4 iWidth, int4 iHeight, uint1 * pData )
{
  m_pEnvironment->GetContext()->MakeCurrent();

  glFlush();

  glReadBuffer( GL_FRONT );  
  glPushAttrib( GL_PIXEL_MODE_BIT );  
  glPixelStorei( GL_PACK_ALIGNMENT, 4 );

  glReadPixels( x, y, iWidth, iHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData );
  glPopAttrib();
} // GetImage()


/** 
 * draws an image to the screen.
 * @param x       the x position
 * @param y       the y position
 * @param iWidth  the image width
 * @param iHeight the image height
 * @param pData   the image data in BGRA/uint1 format
 */
void vxViewerControl::DrawImage( int4 x, int4 y, int4 iWidth, int4 iHeight, uint1 * pData )
{

  // get win rect
  vxViewer * pViewer( GetViewerStruct( vxViewerControl::MainViewer )->m_pViewer );

  m_pEnvironment->GetContext()->MakeCurrent();

  // set up identity matrix
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0f, pViewer->GetWinRect().GetViewportWidth() - 1, 0.0f, pViewer->GetWinRect().GetViewportHeight() - 1 );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  // set up the viewport
  RECT clientRect;
  GetClientRect( m_windowHandle.GetHandle(), & clientRect );
  uint2 uWidth( clientRect.right - clientRect.left );
  uint2 uHeight( clientRect.bottom - clientRect.top );
  glViewport( 0, 0, uWidth, uHeight);

  // draw
  glRasterPos2i( x, y );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
  glDrawPixels( iWidth, iHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData );

  // restore matrices
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glPopAttrib();

  // show back buffer
  m_pEnvironment->GetContext()->SwapBuffers();

} // GetImage()




/**
 * Process modified queue events.
 * @param info Modified information.
 */
void vxViewerControl::OnModified( vxModifiedInfoList & info )
{
  try
  {
    if ( info.Size() > 0 && m_pEnvironment->IsModified( vxEnvironment::ALL ))
    {
      if ( m_pEnvironment->IsModified( vxEnvironment::FONT ) )
      {
        m_pEnvironment->GetContext()->MakeCurrent();
        m_pEnvironment->UpdateFont();
      }
      /*Redraw(); */ 
      m_pEnvironment->SetModified( vxEnvironment::ALL, false );
    }
  }
  catch (ex::VException &)
  {
    LogErr("Modified failed.", "vxViewerImpl", "Modified");
  }
} // OnModified()




// $Log: vxViewerControl.C,v $
// Revision 1.3  2008/05/02 04:57:46  cqian
// add comments
//
// Revision 1.2  2008/01/22 21:08:17  gdavidson
// Fixed bug in RemoveManipulator, RemoveRenderer and RemoveController methods
//
// Revision 1.1  2007/10/24 20:33:38  gdavidson
// Moved from Visualization
//
// Revision 1.16  2007/05/17 20:05:05  gdavidson
// Added SetSubViewerSize
//
// Revision 1.15  2007/05/03 21:29:43  gdavidson
// Changed vxRenderingNode -> vxNodeRendering
//
// Revision 1.14  2007/02/12 22:45:32  gdavidson
// Issue #5354: Modified the reset of the viewertype
//
// Revision 1.13  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.12  2007/01/19 15:28:49  geconomos
// reorganized visualization preferences
//
// Revision 1.11  2007/01/17 14:14:07  geconomos
// moved cine and momentum reset to managed layer
//
// Revision 1.10  2007/01/16 17:06:48  gdavidson
// Added vxCine
//
// Revision 1.9  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.8  2006/12/04 20:55:41  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.7  2006/11/17 20:17:06  geconomos
// temporary code for fusion demo
//
// Revision 1.6  2006/10/12 14:24:30  geconomos
// formatting
//
// Revision 1.5  2006/10/10 15:37:59  geconomos
// + removed UpdateViewer
// + cancel rendering when resetting the viewer
// + Issue 5024: image from prior viewer type incorrectly displaying
//
// Revision 1.4  2006/09/28 21:07:15  romy
// moved undo actions to its on class
//
// Revision 1.3  2006/09/28 15:52:17  geconomos
// Issue 5027: sub viewer causing main view to disappear
//
// Revision 1.2  2006/09/27 19:59:48  romy
// Undo action memory management added
//
// Revision 1.1  2006/09/27 17:06:56  geconomos
// using vx naming convention for unmanaged classes
//
// Revision 1.29  2006/09/20 20:57:37  frank
// spelling
//
// Revision 1.28  2006/09/19 18:28:33  gdavidson
// Issue #4989, #4990, #4992, #4997: Ignored alpha channel when getting image
//
// Revision 1.27  2006/09/14 20:26:08  geconomos
// added modified bits to viewer diagnostics
//
// Revision 1.26  2006/08/23 17:51:39  geconomos
// reading "DrawTimingStatistics" from visualization preferences
//
// Revision 1.25  2006/08/03 14:24:11  geconomos
// temporary support for old and new rendering mechanisms
//
// Revision 1.24  2006/07/26 15:09:53  gdavidson
// Proper handling of display of pan zoom window
//
// Revision 1.23  2006/07/14 13:32:43  geconomos
// more code cleanup
//
// Revision 1.22  2006/07/12 19:34:03  geconomos
// code clean up
//
// Revision 1.21  2006/06/07 15:52:28  geconomos
// corrected compilation errors in debug mode
//
// Revision 1.20  2006/04/25 20:22:34  romy
// StackModifiedFuncPtr added
//
// Revision 1.19  2006/03/02 15:50:19  frank
// added composite undo actions
//
// Revision 1.18  2006/02/17 15:19:26  frank
// working on undoing all segmentation actions
//
// Revision 1.17  2006/02/09 14:10:06  geconomos
// corrected calls to wndproc for 64-bit builds
//
// Revision 1.16  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.15  2005/12/08 16:43:13  frank
// added undo support
//
// Revision 1.14  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.13.2.3  2005/08/15 15:29:55  geconomos
// Added call to momentum::reset
//
// Revision 1.13.2.2  2005/08/04 18:52:02  frank
// Issue #4339: Fixed drawing of inverted snapshot image when pan/zoom window is shown
//
// Revision 1.13.2.1  2005/07/06 15:04:47  frank
// removed the viewerbar (and the black-out rectangle) from snapshots
// inverted the image during snapshot
//
// Revision 1.13  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.12  2001/01/03 19:16:47  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.11  2005/01/07 17:19:46  frank
// uninitialized renderers before deleting gl context
//
// Revision 1.10  2005/01/06 16:32:07  frank
// added controller list to debug output
//
// Revision 1.9  2004/11/02 14:55:41  geconomos
// Changed snapshot to read from back buffer.
//
// Revision 1.8  2004/10/18 02:50:37  geconomos
// reworked selected viewer's border color
//
// Revision 1.7  2004/08/17 14:52:59  geconomos
// a) Renamed OnCreate and OnDestroy to Create and Destroy respectively.
//
// b) Moved class to Create and Destroy to managed parent.
//
// Revision 1.6  2004/05/20 12:47:54  frank
// added controllers
//
// Revision 1.5  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.4  2004/04/06 13:33:16  frank
// fixed problem with dangling context pointer
//
// Revision 1.3  2004/04/02 14:02:40  frank
// working on unifying the various rendering contexts
//
// Revision 1.2  2004/03/30 15:11:15  geconomos
// Added property for border width.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.76  2003/12/18 17:04:39  frank
// disabled debug
//
// Revision 1.75  2003/12/18 17:02:55  frank
// listed the renderers that affirmed the redraw
//
// Revision 1.74  2003/12/17 15:41:19  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.73  2003/12/16 20:01:26  dongqing
// zig around the bug that crash the Vascular loading from
// console if there is a session saved. This did not fix the
// dangling pointer associated to the vxViewerType. It just
// let the XP moving on smoothly.
//
// Revision 1.72  2003/12/16 19:14:52  michael
// modified the timing statistics display and turned it off by default
//
// Revision 1.71  2003/12/16 19:09:39  frank
// removed debug messages on images
//
// Revision 1.70  2003/12/16 14:19:45  michael
// took out logging messages
//
// Revision 1.68  2003/11/17 17:57:09  frank
// Added function to list all the manipulators
//
// Revision 1.67  2003/10/09 01:12:35  geconomos
// Added a call to UpdateFont when resetting the viewer.
//
// Revision 1.66  2003/10/02 10:27:27  geconomos
// Added code to update the current rendering font in Modified
//
// Revision 1.65  2003/10/01 00:44:36  geconomos
// Changed GetImage() function.
//
// Revision 1.64  2003/09/29 17:22:51  michael
// cosmetics
//
// Revision 1.63  2003/09/13 04:54:46  frank
// Better handled exceptions during window creation
//
// Revision 1.62  2003/09/10 20:59:24  dongqing
// set environment.viewerType = NULL on purpose to avoid them point
// to dangling pointer
//
// Revision 1.61  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.60  2003/08/06 01:56:33  geconomos
// ssue 3412: pan/zoom causes wrong viewer type (pan/zoom) to be stored in environment
//
// Revision 1.59  2003/07/24 13:04:19  michael
// added redraw border instead of causing a full Redraw on the viewer and its
// clients (renderers)
//
// Revision 1.58  2003/06/09 12:29:53  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.57  2003/06/04 18:30:10  michael
// added namespace declaration
//
// Revision 1.56  2003/06/03 14:55:59  geconomos
// Set openGL context when resetting a viewer.
//
// Revision 1.55  2003/05/19 14:58:12  frank
// Removed unused return parameter
//
// Revision 1.54  2003/05/14 13:27:24  geconomos
// Updated for changeto GLPaintDevice.
//
// Revision 1.53  2003/05/13 19:33:04  michael
// code review
//
// Revision 1.52  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.51  2003/05/13 13:52:11  geconomos
// More coding standards.
//
// Revision 1.50  2003/05/13 13:50:04  geconomos
// Coding standards.
//
// Revision 1.49  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.48  2003/04/24 14:07:05  michael
// code review
//
// Revision 1.47  2003/04/23 18:19:58  geconomos
// Proper initialization of camera.
//
// Revision 1.46  2003/04/07 18:02:46  michael
// fixed background color for MPR views
//
// Revision 1.45  2003/03/26 01:38:08  michael
// formatting
//
// Revision 1.44  2003/03/21 15:41:15  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.43  2003/03/20 13:49:05  geconomos
// Added ViewerType and SubViewerType properties.
//
// Revision 1.42  2003/02/28 20:33:53  michael
// completed one more step to move to vxViewerType
//
// Revision 1.41  2003/01/28 15:05:19  frank
// Const access functions should be const!
//
// Revision 1.40  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.39  2002/11/26 15:29:20  geconomos
// Fixed bug while removing renderers and manipulators.
//
// Revision 1.38  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.37  2002/11/25 13:55:04  geconomos
// Mouse cursor fixes.
//
// Revision 1.36  2002/11/20 13:49:02  geconomos
// Refined snapshot image to remove border around viewer.
//
// Revision 1.35  2002/11/15 21:46:26  geconomos
// Updated to use new ViewType enum value from vxUtils.
//
// Revision 1.34  2002/11/15 21:06:34  michael
// changed view types
//
// Revision 1.33  2002/11/13 16:10:03  geconomos
// Added redraw of viewer on WM_MOUSELEAVE
//
// Revision 1.32  2002/11/11 21:23:25  geconomos
// Allowed for ORing of ViewerTypes while add/removing manipulators and renderers.
//
// Revision 1.31  2002/11/10 23:11:45  michael
// removed context creation from texture server to local method
//
// Revision 1.30  2002/11/07 18:41:09  ingmar
// it is no longer necessarry to invalidate the complete environment if a renderer is added
//
// Revision 1.29  2002/11/07 13:02:19  geconomos
// Added border color property to viewer.
//
// Revision 1.28  2002/11/06 22:02:36  geconomos
// Added HasRenderer and HasManipulator methods.
//
// Revision 1.27  2002/11/05 20:49:18  geconomos
// Moved initialization of camera into the v3D environment.
//
// Revision 1.26  2002/10/14 16:02:48  michael
// accounting for const methods in vxColormap/vxCamera
//
// Revision 1.25  2002/10/14 15:34:36  frank
// Reflected changes to Camera class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerControl.C,v 1.3 2008/05/02 04:57:46 cqian Exp $
// $Id: vxViewerControl.C,v 1.3 2008/05/02 04:57:46 cqian Exp $
