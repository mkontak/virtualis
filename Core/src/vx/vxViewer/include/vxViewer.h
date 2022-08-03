// $Id: vxViewer.h,v 1.5 2007/05/03 21:31:42 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

/*
  This is the new Viewer class providing the backbone
  for renderers and algorithms
*/

#ifndef vxViewer_h
#define vxViewer_h


// includes
#include "vxInput.h"
#include "vxRenderer.h"
#include "Image.h"
#include "PixelRGB.h"
#include "Point2D.h"
#include "vxWinRect.h"
#include "vxUtils.h"


// forward declarations
class vxEnvironment;
class vxController;
class vxManipulator;
class GLPaintDevice;
class GLFormat;
class GLContext;
class vxViewerType;


// class definition
class VX_VIEWER_DLL vxViewer : public vxInput, public vxRenderer
{  
// functions
public:

  /// constructor
  vxViewer(vxEnvironment &);
  
  /// destructor
  virtual ~vxViewer();

  /// tests if the coordinates are within the viewer
  bool HitTest(Point2D<int2> &);

  /// retreive an image of the current frame buffer
  Image<PixelRGB<uint1> > GetImage();

  /// dets the ViewerType
  void SetViewerType(vxViewerType * pViewerType ) { m_pViewerType = pViewerType; }

  /// returns the ViewerType
  vxViewerType * GetViewerType() { return m_pViewerType; }
  
  /////////////////////////////////////////////////////////////////////////
  ///                         HANDLE RENDERING
  /////////////////////////////////////////////////////////////////////////

  /// sets the environment
  virtual void SetEnvironment(vxEnvironment & environment);

  /// resize the renderers area
  virtual void Resize(vxWinRect & winRect);

  /// returns the window and view size parameters
  const vxWinRect & GetWinRect() const { return m_winRect; };

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform controller operations
  virtual void ProcessControllers();

  /// perform pre-rendering operations
  virtual int4 PreRender();
  
  /// perform rendering operations
  virtual void Render();
  
  /// perform pre-rendering operations
  virtual int4 PostRender();

  /// reset the renderer
  virtual void Reset();

  /// initialize the renderer
  virtual void Initialize();
  
  /// uninitialize the renderer
  virtual void Uninitialize();

  /////////////////////////////////////////////////////////////////////////
  ///                         HANDLE INPUTS
  /////////////////////////////////////////////////////////////////////////

  /// event handler can be reimplemented in a subclass to receive wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer  
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer     
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer   
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer 
  virtual bool OnDoubleClick(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive keyboard down events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive keyboard up events for the viewer
  virtual bool OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse enter events for the viewer
  virtual void OnMouseEnter();

  /// event handler can be reimplemented in a subclass to receive mouse leave events for the viewer
  virtual void OnMouseLeave();

  /// event handler can be reimplemented in a subclass to receive SetCursor events for the viewer
  virtual bool OnSetCursor();


  /////////////////////////////////////////////////////////////////////////
  ///                           LIST HANDLING
  /////////////////////////////////////////////////////////////////////////

  /// returns the list of controllers
  std::list<vxController *> & GetControllerList() { return m_lpController; }

  /// add a controller to the list of controllers
  void AddController(vxController * pController);

  /// remove a controller from the list of controllers
  void RemoveController(vxController * pController);

  /// returns the list of manipulators
  std::list<vxManipulator *> & GetManipulatorList() { return m_lpManipulator; }

  /// add a manipulator to the list of manipulators
  void AddManipulator(vxManipulator * pManipulator);

  /// remove a manipulator from the list of manipulators
  void RemoveManipulator(vxManipulator * pManipulator);
  
  /// returns the list of Renderers
  std::list<vxRenderer *> & GetRendererList() { return m_lpRenderer; }

  /// add a Renderer to the list of Renderers
  void AddRenderer(vxRenderer * pRenderer);

  /// remove a Renderer from the list of Renderers
  void RemoveRenderer(vxRenderer * pRenderer);

  /// setup camera for a view
  void SetupView();

private:	
  
  /// disabled copy constructor
  vxViewer(const vxViewer &); 

  /// disabled assignment operator
  const vxViewer & operator=(const vxViewer &);

  /// helper function to get point relative to viewer boundaries (main, sub)
  Point2D<uint2> GetPositionRelativeToViewer(const Point2D<int2> & position);

/// member functions
private:

  /// list of controllers
  std::list<vxController *> m_lpController; 

  /// list of manipulators
  std::list<vxManipulator *> m_lpManipulator;

  /// list of renderers
  std::list<vxRenderer *> m_lpRenderer;

  /// windown rectangle
  vxWinRect m_winRect;

  /// last screen position of double click
  Point2D<uint4> m_lastDoubleClickPosition;

  /// viewer's ViewerType
  vxViewerType * m_pViewerType;

  /// viewer's ViewerType
  vxViewerType * m_pHACK_TO_PREVENT_DANGLING_POINTER_CRASH_DUMMY_VIEWERTYPE;

}; // vxViewer


#endif


// Revision history:
// $Log: vxViewer.h,v $
// Revision 1.5  2007/05/03 21:31:42  gdavidson
// Removed IsBeating
//
// Revision 1.4  2007/04/18 17:05:41  gdavidson
// Add IsBeating method
//
// Revision 1.3  2007/01/16 16:51:07  gdavidson
// Removed OnTimer
//
// Revision 1.2  2004/05/20 13:40:59  frank
// added process controllers
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2003/12/18 12:55:48  michael
// preventing the memory leak
//
// Revision 1.28  2003/12/16 13:56:15  michael
// added initial interface function to test for necessary redraws before redrawing
// the viewer image again. It is going through all renderers to see if any of those
// requires rerendering.
//
// Revision 1.27  2003/09/10 20:59:24  dongqing
// set environment.viewerType = NULL on purpose to avoid them point
// to dangling pointer
//
// Revision 1.26  2003/09/10 20:17:34  dongqing
// set m_pViewerType = NULL for Reset()
//
// Revision 1.25  2003/06/03 12:19:07  michael
// misc
//
// Revision 1.24  2003/05/19 14:43:47  frank
// Removed unused return parameter
//
// Revision 1.23  2003/05/16 11:45:27  michael
// code review
//
// Revision 1.22  2003/04/29 19:41:01  frank
// Added key up message handling
//
// Revision 1.21  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.20  2003/04/16 14:40:36  michael
// performed initial code review
//
// Revision 1.19  2003/04/15 21:39:48  michael
// performed more code review
//
// Revision 1.18  2003/04/15 13:29:22  michael
// performed initial code review
//
// Revision 1.17  2003/03/20 13:46:14  geconomos
// Added ViewerType and SubViewerType properties.
//
// Revision 1.16  2003/02/28 21:43:01  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.15  2002/11/25 13:45:49  geconomos
// Added OnSetCursor override.
//
// Revision 1.14  2002/08/20 22:46:46  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.13  2002/08/05 16:20:46  dmitry
// Event name added to OnTimer.
//
// Revision 1.12  2002/07/29 14:57:49  michael
// Removed viewer type from vxViewer and moved it to vxRenderer
//
// Revision 1.11  2002/07/22 23:17:23  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.10  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.9  2002/06/27 18:05:26  uday
// ViewTypeEnum moved to vxUtils.
//
// Revision 1.8.2.1  2002/07/01 18:52:49  uday
// Moved viewType to vxUtils.
//
// Revision 1.8  2002/05/20 22:10:25  uday
// Made SetUpView() public. (Dmitry)
//
// Revision 1.7  2002/05/17 14:57:13  dmitry
// viewType Enum, Moved SetUpView() from V3d_viewer
//
// Revision 1.6  2002/05/06 21:45:45  dmitry
// OnTimer Added
//
// Revision 1.5  2002/03/26 18:42:18  dmitry
// Moved creation of GLContext to v3D_Viewer.
//
// Revision 1.4  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.3  2002/03/19 17:28:56  dmitry
// added winRect to the Resize of the vxRenderer (and vxViewer)
//
// Revision 1.2  2002/03/18 15:15:36  dmitry
// vxWinRect added, old sizing members removed, and moved gl initialization and buffer control to v3D_Viewer.
//
// Revision 1.1  2002/03/12 15:36:34  jaddonisio
// Initial version.
//
// Revision 1.3  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 21:04:21  michael
// some more code for the new backbone (vxViewer)
//
// Revision 1.1  2002/02/22 15:28:57  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewer.h,v 1.5 2007/05/03 21:31:42 gdavidson Exp $
// $Id: vxViewer.h,v 1.5 2007/05/03 21:31:42 gdavidson Exp $
