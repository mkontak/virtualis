// $Id: vxViewer.C,v 1.11 2007/10/24 20:15:04 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewer.h"
#include "vxController.h"
#include "vxManipulator.h"
#include "Point2D.h"
#include "GLPaintDevice.h"
#include "GLFormat.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "GLFontGeorge.h"
#include "vxViewerTypes.h"


// defines
#define FILE_REVISION "$Revision: 1.11 $"


/**
 * Constructor
 *
 * @param reference to an environment object
 */ 
vxViewer::vxViewer(vxEnvironment & environment) :
  vxRenderer(environment),
  m_lastDoubleClickPosition(0,0),
  m_pViewerType( NULL )
//*******************************************************************
{
  //m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE = new vxViewerTypeUndefined(environment);
  //m_pViewerType = m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE;
} // constructor


/**
 * Destructor
 */ 
vxViewer::~vxViewer()
//******************************************************************* 
{
  //delete m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE;
  //m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE = NULL;
} // destructor


/**
 * Checks if the point is within the drawing area
 *
 * @return true if within viewer, false else
 */
bool vxViewer::HitTest(Point2D<int2> & point)
//******************************************************************* 
{
  return GetWinRect().IsInside(point);
}


/** 
 * Has something changed that makes rendering necessary?
 * @return feedback from renderers
 */
bool vxViewer::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false);

  // see if any renderers require redrawing
  std::list<vxRenderer *>::iterator iter;
  for (iter = GetRendererList().begin(); iter != GetRendererList().end(); ++iter)
  {
    bRedrawNecessary |= (*iter)->IsRedrawNecessary();
  }

  // see if any controllers require processing
  std::list<vxController *>::iterator conIter;
  for (conIter = GetControllerList().begin(); conIter != GetControllerList().end(); ++conIter)
  {
    bRedrawNecessary |= (*conIter)->IsProcessingNecessary();
  }

  return bRedrawNecessary;
} // IsRedrawNecessary()


/** 
 * executes each of the controllers
 */
void vxViewer::ProcessControllers()
{

  std::list< vxController * >::iterator iter;

  for ( iter = m_lpController.begin(); iter != m_lpController.end(); ++iter )
  {
    if( (*iter)->IsProcessingNecessary() )
      (*iter)->Process();
  }

} // PreRender()


/** 
 * Inits the OpenGL drawing area and calls all renderers
 * @return feedback from renderers
 */
int4 vxViewer::PreRender()
//*******************************************************************
{
  std::list<vxRenderer *>::iterator iter;
  for (iter = GetRendererList().begin(); iter != GetRendererList().end(); ++iter)
  {
    (*iter)->PreRender();
  }
  return true;
} // PreRender()


/** 
 * Performs the rendering by calling all registered renderers
 */
void vxViewer::Render()
//*******************************************************************
{
  std::list<vxRenderer *>::iterator iter;
  for (iter = GetRendererList().begin(); iter != GetRendererList().end(); ++iter)
  {
    (*iter)->Render();
  }
  return;
} // Render()


/** 
 * Finishes the rendering by calling all registered renderers
 * @return feedback from renderers
 */
int4 vxViewer::PostRender()
//*******************************************************************
{
  std::list<vxRenderer *>::iterator iter;
  for (iter = GetRendererList().begin(); iter != GetRendererList().end(); ++iter)
  {
    (*iter)->PostRender();
  }
  return true;
} // PostRender()


/** 
 * Reset the renderer
 */
void vxViewer::Reset()
//*******************************************************************
{
  // make sure the pointer does not point to a dangling pointer
  //m_pViewerType = m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE;
} // Reset()



/**
 * Set environment in lists for manipulators and renderers
 * @param reference to an environment
 */
void vxViewer::SetEnvironment(vxEnvironment & environment)
//*******************************************************************
{
  //delete m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE;
  //m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE = new vxViewerTypeUndefined(environment);

  vxRenderer::SetEnvironment(environment);

  std::list<vxController *>::iterator iterController;
  std::list<vxManipulator *>::iterator iterManipulator;
  std::list<vxRenderer *>::iterator iterRenderer;

  for (iterRenderer = m_lpRenderer.begin(); iterRenderer != m_lpRenderer.end(); ++iterRenderer)
  {
    (*iterRenderer)->SetEnvironment(environment);
  }

  for (iterController = m_lpController.begin(); iterController != m_lpController.end(); ++iterController)
  {
    (*iterController)->SetEnvironment(environment);
  }

  for (iterManipulator = m_lpManipulator.begin(); iterManipulator != m_lpManipulator.end(); ++iterManipulator)
  {
    (*iterManipulator)->SetEnvironment(environment);
  }
} // SetEnvironment()


/**
 * Returns an image
 * @return image
 */
Image<PixelRGB<uint1> > vxViewer::GetImage()
//*******************************************************************
{
  return Image<PixelRGB<uint1> >(PixelRGB<uint1>(0,0,0));
} 


/**
 * Initialize
 */
void vxViewer::Initialize()
//*******************************************************************
{
  //GetEnvironment().GetGLEnvironment().Init();

  std::list<vxRenderer *>::iterator iter;

  for (iter = m_lpRenderer.begin(); iter != m_lpRenderer.end(); ++iter)
  {
    (*iter)->Initialize();
  }
} // Initialize()


/**
 * Unitialize
 */
void vxViewer::Uninitialize()
//*******************************************************************
{
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

  std::list<vxRenderer *>::iterator iterRenderer;

  for (iterRenderer = m_lpRenderer.begin(); iterRenderer != m_lpRenderer.end(); ++iterRenderer)
  {
    (*iterRenderer)->Uninitialize();
  }
} // Uninitialize()


/**
 * Resize the viewer
 * @param window rectangle object
 */
void vxViewer::Resize(vxWinRect & winRect)
//*******************************************************************
{
  m_winRect = winRect;
  m_lastDoubleClickPosition = Point2D<int4>(winRect.GetReferencePoint().first, 
                                            winRect.GetReferencePoint().second);
} // Resize()


/**
 * Get position relative to viewer boundaries (main, sub)
 * @param window rectangle object
 */
Point2D<uint2> vxViewer::GetPositionRelativeToViewer(const Point2D<int2> & position)
//*******************************************************************
{
  const vxWinRect & winRect(GetEnvironment().GetWinRect());
  const Point2D<uint2> point((position.X() - winRect.GetReferencePoint().first),
                             (position.Y() - (winRect.GetWindowSize().second - (winRect.GetReferencePoint().second + winRect.GetViewportSize().second))));
  return point;
}


/**
 * Setup the camera for a view
 */
void vxViewer::SetupView()
//*******************************************************************
{
  try
  {
    // set the window rectangle in the environment
    const vxWinRect & winRect(GetWinRect());
    GetEnvironment().SetWinRect(winRect);
    GetEnvironment().SetViewerType(m_pViewerType);
    //LogWrn("=============  " + ToAscii(m_pViewerType), "", "");

    // viewer type to setup view
    GetEnvironment().GetViewerType()->SetupView();

    glDrawBuffer(GL_BACK);
  }
  catch( ... )
  {
    throw ex::InvalidDataException( LogRec("Unhandle exception on", "vxViewer", "SetupView") );
  }
} // SetupView()


/**
 * Action to be taken on mouse wheel changes
 * @param screen position
 * @param delta of mouse wheel
 * @param state of keyboard
 * @return success
 */
bool vxViewer::OnMouseWheel(const Point2D<int2> & position, 
                            const int2 iDelta, 
                            const uint4 uKeyboardState)
//*******************************************************************
{
  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  const Point2D<uint2> point(GetPositionRelativeToViewer(position));
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnMouseWheel(point, iDelta, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  return true;
} // OnMouseWheel()


/**
 * Action to be taken on mouse move changes
 * @param screen position
 * @param state of keyboard
 * @return success
 */
bool vxViewer::OnMouseMove(const Point2D<int2> & position, 
                           const uint4 uKeyboardState)
//*******************************************************************
{
  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  const Point2D<uint2> point(GetPositionRelativeToViewer(position));
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnMouseMove(point, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  return true;
} // OnMouseMove()


/**
 * Action to be taken on mouse button down
 * @param screen position
 * @param mouse button id
 * @param state of keyboard
 * @return success
 */
bool vxViewer::OnButtonDown(const Point2D<int2> & position, 
                            const ButtonEnum eButton, 
                            const uint4 uKeyboardState)
//*******************************************************************
{
  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  const Point2D<uint2> point(GetPositionRelativeToViewer(position));
  std::list<vxManipulator *>::iterator iter;
  
  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnButtonDown(point, eButton, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  return true;
} // OnButtonDown()


/**
 * Action to be taken on mouse button up
 * @param screen position
 * @param mouse button id
 * @param state of keyboard
 * @return success
 */
bool vxViewer::OnButtonUp(const Point2D<int2> & position, 
                          const ButtonEnum eButton, 
                          const uint4 uKeyboardState)
//*******************************************************************
{
  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  const Point2D<uint2> point(GetPositionRelativeToViewer(position));
  std::list<vxManipulator *>::iterator iter;
  
  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnButtonUp(point, eButton, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  return true;
} // OnButtonUp()


/**
 * Action to be taken on mouse double click
 * @param screen position
 * @param mouse button id
 * @param state of keyboard
 * @return success
 */
bool vxViewer::OnDoubleClick(const Point2D<int2> & position,
                             const ButtonEnum eButton, 
                             const uint4 uKeyboardState)
//*******************************************************************
{
  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  const Point2D<uint2> point(GetPositionRelativeToViewer(position));
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnDoubleClick(point, eButton, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  m_lastDoubleClickPosition = position;

  return true;
} // OnDoubleClick()


/**
 * Action to be taken on key pressed
 * @param key id
 * @param keyboard state
 * @return success
 */
bool vxViewer::OnKeyPressed( const uint1 iKey, const uint4 uKeyboardState )
//*******************************************************************
{

  SetupView();

  // call all manipulators
  bool bIfHandled(false);
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    bIfHandled = (*iter)->OnKeyPressed(iKey, uKeyboardState);
    if (bIfHandled == true)
      break;
  }

  return true;
} // OnKeyPressed()


/**
 * Action to be taken on key released
 * @param key id
 * @param keyboard state
 * @return success
 */
bool vxViewer::OnKeyReleased(const uint1 iKey, const uint4 uKeyboardState)
//*******************************************************************
{

  SetupView();

  // call all manipulators
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    if ( (*iter)->OnKeyReleased( iKey, uKeyboardState ) == true )
      break;
  }

  return true;

} // OnKeyPressed()


/**
 * Action to be taken on mouse cursor entering the viewer
 */
void vxViewer::OnMouseEnter()
//*******************************************************************
{
  SetupView();

  std::list<vxManipulator *>::iterator iter;

  // call all manipulators
  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    (*iter)->OnMouseEnter();
  }
} // OnMouseEnter()


/**
 * Action to be taken on mouse cursor leaving the viewer
 */
void vxViewer::OnMouseLeave()
//*******************************************************************
{
  SetupView();

  std::list<vxManipulator *>::iterator iter;

  // call all manipulators
  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    (*iter)->OnMouseLeave();
  }
} // OnMouseLeave()


/**
 * Called when the mouse cursor is to be set
 * @return true if cursor was set, false otherwise
 */
bool vxViewer::OnSetCursor()
//*******************************************************************
{
  SetupView();

  bool bHandled(false);
  std::list<vxManipulator *>::iterator iter;

  for (iter = m_lpManipulator.begin(); iter != m_lpManipulator.end(); ++iter)
  {
    if ((*iter)->OnSetCursor())
    {
      bHandled = true;
      break;
    }
  }
  if (bHandled == false)
  {
    vxMouseCursor::SetCursor("CURSOR_DEFAULT");
  }

  return bHandled;
} // OnSetCursor()


/**
 * Add a controller
 * @param controller the controller to add
 */
void vxViewer::AddController( vxController * pController )
{
  m_lpController.push_front( pController );
} // AddController()


/**
 * Remove controller
 * @param controller the controller to remove
 */
void vxViewer::RemoveController( vxController * pController )
{
  m_lpController.remove( pController );
} // RemoveController()


/**
 * Add a manipulator
 * @param manipulator
 */
void vxViewer::AddManipulator(vxManipulator * pManipulator)
//*******************************************************************
{
  m_lpManipulator.push_front(pManipulator);
}


/**
 * Remove manipulator
 * @param manipulator
 */
void vxViewer::RemoveManipulator(vxManipulator * pManipulator)
//*******************************************************************
{
  m_lpManipulator.remove(pManipulator);
}


/**
 * Add renderer
 * @param renderer
 */
void vxViewer::AddRenderer(vxRenderer * pRenderer)
//*******************************************************************
{
  m_lpRenderer.push_back(pRenderer);
}


/**
 * Remove renderer
 * @param renderer
 */
void vxViewer::RemoveRenderer(vxRenderer * pRenderer)
//*******************************************************************
{
  m_lpRenderer.remove(pRenderer);
}


#undef FILE_REVISION


// Revision History:
// $Log: vxViewer.C,v $
// Revision 1.11  2007/10/24 20:15:04  gdavidson
// Removed init for GLEnvironment
//
// Revision 1.10  2007/05/03 21:31:42  gdavidson
// Removed IsBeating
//
// Revision 1.9  2007/04/18 17:05:41  gdavidson
// Add IsBeating method
//
// Revision 1.8  2007/02/12 22:44:39  gdavidson
// Issue #5354: Removed m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE
//
// Revision 1.7  2007/01/16 16:29:21  gdavidson
// Removed Timer
//
// Revision 1.6  2006/11/22 21:47:27  gdavidson
// Check a local controllers IsProcessingNecessary method before processing
//
// Revision 1.5  2005/05/16 17:52:08  frank
// fixed mouse cursor when creating an annotation and the pan/zoom is enabled
//
// Revision 1.4  2004/09/27 14:16:03  frank
// added pre-processing check to controller interface
//
// Revision 1.3  2004/05/20 13:40:59  frank
// added process controllers
//
// Revision 1.2  2004/04/02 14:03:43  frank
// working on unifying the various rendering contexts
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.68  2003/12/18 12:55:48  michael
// preventing the memory leak
//
// Revision 1.67  2003/12/17 15:31:33  dongqing
// In the SetupView, a try-catch was added for potiential dangling pointer.
// In the constructor and Reset, the viewerType was initialized as a
// dummy viewerType instead of NULL. This casues memory leak, but
// seems fix the dangling pointer cresh. Dongqing
//
// Revision 1.66  2003/12/16 13:56:15  michael
// added initial interface function to test for necessary redraws before redrawing
// the viewer image again. It is going through all renderers to see if any of those
// requires rerendering.
//
// Revision 1.65  2003/09/10 20:59:24  dongqing
// set environment.viewerType = NULL on purpose to avoid them point
// to dangling pointer
//
// Revision 1.64  2003/06/03 12:19:06  michael
// misc
//
// Revision 1.63  2003/05/19 14:43:47  frank
// Removed unused return parameter
//
// Revision 1.62  2003/05/16 11:45:27  michael
// code review
//
// Revision 1.61  2003/04/29 19:41:01  frank
// Added key up message handling
//
// Revision 1.60  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.59  2003/04/16 14:40:36  michael
// performed initial code review
//
// Revision 1.58  2003/04/15 21:39:48  michael
// performed more code review
//
// Revision 1.57  2003/04/15 13:29:22  michael
// performed initial code review
//
// Revision 1.56  2003/03/26 03:31:48  michael
// redundant if-else since both are the same code
//
// Revision 1.55  2003/03/21 15:40:40  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.54  2003/03/20 13:46:14  geconomos
// Added ViewerType and SubViewerType properties.
//
// Revision 1.53  2003/03/19 16:06:26  michael
// moved view setup for panzoom to vxVierTypeMPROrthoPanZoom
//
// Revision 1.52  2003/03/01 22:19:43  michael
// fixed OpenGl issues with main and sub view. Before calling any type of
// manipulation, the OpenGL context is initialized properly
//
// Revision 1.51  2003/02/28 22:19:04  michael
// changed logic so that CTA will also be called by default (SetupView())
//
// Revision 1.50  2003/02/28 22:09:35  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.49  2003/02/28 21:43:01  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.48  2003/02/28 20:33:53  michael
// completed one more step to move to vxViewerType
//
// Revision 1.47  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.46  2003/01/28 15:04:00  frank
// Const access functions should be const!
//
// Revision 1.45  2003/01/22 22:06:29  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.44  2002/12/20 19:02:13  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.43  2002/12/17 14:32:17  michael
// initialize the winRect in the Environment on PreRender()
//
// Revision 1.42  2002/12/16 20:08:54  michael
// using IsInside() method of vxWinRect() instead of duplicating code
//
// Revision 1.41  2002/11/25 15:42:36  michael
// near/far hack for RSNA
//
// Revision 1.40  2002/11/25 13:45:49  geconomos
// Added OnSetCursor override.
//
// Revision 1.39  2002/11/20 15:48:39  michael
// set viewer type initially
//
// Revision 1.38  2002/11/19 21:27:55  michael
// added view setup for oblique view
//
// Revision 1.37  2002/11/15 21:06:09  michael
// changed view types
//
// Revision 1.36  2002/11/14 18:37:37  ingmar
// cleaned up 3D view setup
//
// Revision 1.35  2002/11/13 01:52:12  michael
// added FILE_REVISION
//
// Revision 1.34  2002/11/12 16:44:35  michael
// fixed pan/zoom with different aspect ratios
//
// Revision 1.33  2002/11/12 00:14:16  michael
// cosmetics & removed redundant code
//
// Revision 1.32  2002/10/21 23:15:51  ingmar
// cleaner 3D view setup
//
// Revision 1.31  2002/09/19 14:26:03  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.30  2002/08/20 22:46:46  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.29  2002/08/14 16:06:48  wenli
// initial version - Vascular
//
// Revision 1.28  2002/08/05 16:20:46  dmitry
// Event name added to OnTimer.
//
// Revision 1.27  2002/07/29 14:57:49  michael
// Removed viewer type from vxViewer and moved it to vxRenderer
//
// Revision 1.26  2002/07/26 23:16:15  michael
// added a view type to the renderers
//
// Revision 1.25  2002/07/22 23:17:23  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.24  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.23  2002/07/01 15:22:43  michael
// added MakeCurrent to ensure manipulators read from right buffer
//
// Revision 1.22  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.21  2002/06/27 18:05:26  uday
// ViewTypeEnum moved to vxUtils.
//
// Revision 1.20.2.3  2002/07/01 20:07:41  uday
// Add viewtype to the manipulator.
//
// Revision 1.20.2.2  2002/07/01 18:52:49  uday
// Moved viewType to vxUtils.
//
// Revision 1.20.2.1  2002/07/01 16:44:55  michael
// make current on mouse enter so that everything is within valid buffer
//
// Revision 1.20  2002/06/20 17:16:16  uday
// SetUpView() update.
//
// Revision 1.19  2002/06/05 15:17:43  michael
// formatting
//
// Revision 1.18  2002/05/17 14:57:13  dmitry
// viewType Enum, Moved SetUpView() from V3d_viewer
//
// Revision 1.17  2002/05/06 21:45:45  dmitry
// OnTimer Added
//
// Revision 1.16  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.15  2002/05/02 13:25:28  michael
// took out include of unused Trackball
//
// Revision 1.14  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.13  2002/04/22 21:49:48  uday
// Camera set up removed from Prerender().
//
// Revision 1.12  2002/03/28 23:16:26  jaddonisio
// Commented out enabling of GL_DEPTH_TEST, GL_TEXTURE_2D, GL_LIGHTING.
//
// Revision 1.11  2002/03/27 15:07:37  michael
// adjusted Get.SetSize() to Get/SetViewportSize() and commented out some picking code
//
// Revision 1.10  2002/03/26 18:42:18  dmitry
// Moved creation of GLContext to v3D_Viewer.
//
// Revision 1.9  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.8  2002/03/22 19:24:17  manju
// Added WinRect and resize method. --- Dmitry
//
// Revision 1.7  2002/03/22 00:52:38  geconomos
// calling initalize()/uninitialze() on all renderers in the vxViewer
//
// Revision 1.6  2002/03/19 17:28:56  dmitry
// added winRect to the Resize of the vxRenderer (and vxViewer)
//
// Revision 1.5  2002/03/18 15:15:36  dmitry
// vxWinRect added, old sizing members removed, and moved gl initialization and buffer control to v3D_Viewer.
//
// Revision 1.4  2002/03/16 18:07:12  michael
// initialize all renderers when vxViewer is initialized
//
// Revision 1.3  2002/03/14 14:23:26  geconomos
// cosmetic
//
// Revision 1.2  2002/03/13 17:58:56  dmitry
// Iteration through all renderers uncommented in PreRender.
//
// Revision 1.1  2002/03/12 15:36:34  jaddonisio
// Initial version.
//
// Revision 1.4  2002/03/07 22:41:29  dmitry
// Modified to fit new architecture.
//
// Revision 1.3  2002/03/01 20:45:24  michael
// minor changes
//
// Revision 1.2  2002/02/28 21:04:21  michael
// some more code for the new backbone (vxViewer)
//
// Revision 1.1  2002/02/22 15:28:57  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewer.C,v 1.11 2007/10/24 20:15:04 gdavidson Exp $
// $Id: vxViewer.C,v 1.11 2007/10/24 20:15:04 gdavidson Exp $
