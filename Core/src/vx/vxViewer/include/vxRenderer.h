// $Id: vxRenderer.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

/**
 *  define the render base class
 */

#ifndef vxRenderer_h
#define vxRenderer_h


// includes
#include "vxWinRect.h"
#include "vxUtils.h"
#include "BoundingRect.h"


// forward declarations
template <class T> class Rect;
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxRenderer
{
// functions
public:
  
  /// constructor
  vxRenderer(vxEnvironment & environment);

  /// destructor
  virtual ~vxRenderer() {};

  /// sets the environment
  virtual void SetEnvironment(vxEnvironment & environment);

  /// returns the environment
  vxEnvironment & GetEnvironment() const;

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary() { return true; }

  /// perform pre-rendering operations
  virtual int4 PreRender() { return true;}
  
  /// perform rendering operations
  virtual void Render() {};
  
  /// perform post-rendering operations
  virtual int4 PostRender() { return true;}

  /// this is ONLY for image space parallel algorithms
  virtual int4 RenderWindow(const BoundingRect<uint2> &, const uint1 & uThreadID) { return 0; }

  /// reset the renderer
  virtual void Reset() {}

  /// initialize the renderer
  virtual void Initialize() {}
  
  /// uninitialize the renderer
  virtual void Uninitialize() {}

  /// enable or disable
  void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

  /// retrive enabled information
  bool IsEnabled() { return m_bEnabled; }

private:

  /// default constructor
  vxRenderer();

  /// copy constructor
  vxRenderer(const vxRenderer & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRenderer & operator=(const vxRenderer & other); // should only be public if really implemented!

// data
private:

  /// environment
  vxEnvironment * m_pEnvironment;

  /// flag for being enabled
  bool m_bEnabled;
}; // vxRenderer


#endif // vxRenderer_h


// Revision History:
// $Log: vxRenderer.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.30  2003/12/16 13:55:19  michael
// added initial interface function to test for necessary redraws before redrawing
// an entire viewer.
//
// Revision 1.29  2003/07/24 17:53:52  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.28  2003/05/22 14:56:57  dongqing
// code review
//
// Revision 1.27  2003/05/19 14:37:24  frank
// Removed unused return parameter
//
// Revision 1.26  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.25  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.24  2003/05/14 16:10:20  michael
// code review
//
// Revision 1.23  2003/04/29 21:18:05  michael
// adjusted author name
//
// Revision 1.22  2003/03/21 15:40:40  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.21  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.20  2003/01/28 15:04:31  frank
// Const access functions should be const!
//
// Revision 1.19  2003/01/21 22:13:53  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.18  2002/08/29 17:40:56  michael
// made default constructor private so that one can only use it with environment, as intended
//
// Revision 1.17  2002/08/09 15:37:32  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.16  2002/07/29 14:57:49  michael
// Removed viewer type from vxViewer and moved it to vxRenderer
//
// Revision 1.15  2002/07/26 23:16:15  michael
// added a view type to the renderers
//
// Revision 1.14  2002/07/19 16:26:35  dmitry
// Removed the virtual keyword from size.
//
// Revision 1.13  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.12  2002/04/02 21:06:21  dmitry
// Added SetEnabled and IsEnabled methods.
//
// Revision 1.11  2002/03/29 22:31:44  jaddonisio
// Made destructor virtual.
//
// Revision 1.10  2002/03/28 23:05:22  ingmar
// fixed comment
//
// Revision 1.9  2002/03/27 20:18:44  michael
// removed unused and redundant method
//
// Revision 1.8  2002/03/19 17:28:56  dmitry
// added winRect to the Resize of the vxRenderer (and vxViewer)
//
// Revision 1.7  2002/03/15 01:26:37  michael
// fixed include guard name
//
// Revision 1.6  2002/03/15 00:01:44  michael
// added destructor and mechanism to determine in PreRender() whether rendering is necessary
//
// Revision 1.5  2002/03/13 20:55:50  dmitry
// Changed positions of PostRender and RenderWindow methods.
//
// Revision 1.4  2002/03/13 18:58:31  michael
// Added RenderWindow() for parallel rendering (image space subdivision)
//
// Revision 1.3  2002/03/12 15:38:36  jaddonisio
// Added vxRenderer to 79
//
// Revision 1.1  2002/03/11 20:40:44  michael
// added initial vxRenderer classes
//
// Revision 1.4  2002/03/07 22:41:06  dmitry
// Modified to fit new architecture.
//
// Revision 1.3  2002/02/28 21:04:21  michael
// some more code for the new backbone (vxViewer)
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:57  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.4  2002/01/08 21:42:06  dmitry
// Mearged Renderer3D into Renderer.
//
// Revision 1.3  2001/12/11 18:55:58  dmitry
// cleaned up
//
// Revision 1.2  2001/12/11 17:47:43  dmitry
// fixed reloading
//
// Revision 1.1  2001/12/11 17:19:02  dmitry
// inital version
//
// Revision 1.4  2001/12/10 17:42:18  dmitry
// no message
//
// Revision 1.3  2001/12/04 22:37:45  dmitry
// Rewritten, Initial Versions
//
// Revision 1.2  2001/11/30 20:36:03  dmitry
// trackball added
//