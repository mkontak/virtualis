// $Id: vxViewerControl.h,v 1.1 2007/10/24 20:33:32 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxWindowHandle.h"
#include "vxEnvironment.h"
#include "vxObserver.h"


// forward declarations
class vxViewer;
class vxRenderer;
class vxManipulator;
class vxController;
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxViewerControl  : vxObserver
{
// typedef and struct  
public:  
  
  /// renderer collection implementation
  typedef std::multimap< std::string, vxRenderer * > Renderers;
  
  /// manipulator collection implementation
  typedef std::map< std::string, vxManipulator * > Manipulators;

  /// controller collection implementation
  typedef std::map< std::string, vxController* > Controllers;

  // mimic collection of vxViewer
  struct ViewerStruct
  {
    /// viewer
    vxViewer * m_pViewer;

    ///// renderers collection
    Renderers m_renderers;
    
    /// manipulators collection
    Manipulators m_manipulators;

    /// controllers collection
    Controllers m_controllers;

  };  // struct ViewerStruct
  
  // viewer collection implementation
  typedef std::map< int, ViewerStruct * > Viewers;

// enums
public:

  /// viewer type
  enum ViewerTypeEnum
  {
    SubViewer  = 0x02,
    MainViewer = 0x04,
  };

// functions
public:

  /// constructor from vxEnvironment
  vxViewerControl( vxShareableObject< vxEnvironment > * pEnvironment );

  /// desturctor
  virtual ~vxViewerControl();

  /// returns the environment
  vxEnvironment & GetEnvironment() const { return *m_pEnvironment; }

  /// returns the window handle
  vxWindowHandle & GetWindowHandle() { return m_windowHandle; }

  /// retrieves the specified viewer
  vxViewer * GetViewer( ViewerTypeEnum eViewerType ) { return m_viewers[ eViewerType ]->m_pViewer; }

  /// retrieves the specified viewer struct
  vxViewerControl::ViewerStruct * GetViewerStruct( ViewerTypeEnum eViewerType );

  /// retrieves the viewers
  Viewers & GetViewerStructs() { return m_viewers; }

  /// adds the provided viewer to the top z order position
  uint4 AddViewer( ViewerTypeEnum eViewerType, vxViewer * pViewer );

  /// adds a controller to the specified viewer
  void AddController( ViewerTypeEnum eViewerType, vxController * pController, const std::string & sType );

  /// removes the controller from the specified viewer
  void RemoveController( ViewerTypeEnum eViewerType, const std::string & sType );

  /// checks if the controller is contained within the specified viewer
  bool HasController( ViewerTypeEnum eViewerType, const std::string & sType );

  /// Lists all the controllers within the viewer to the debug log
  void ListControllers( ViewerTypeEnum eViewerType );

  /// removes all controllers
  void ClearControllers();

  /// adds a manipulator to the specified viewer
  void AddManipulator( ViewerTypeEnum eViewerType, vxManipulator * pManipulator, const std::string & sType );

  /// removes the manipulator from the specified viewer
  void RemoveManipulator( ViewerTypeEnum eViewerType, const std::string & sType );
  
  /// checks if the manipulator is contained within the specified viewer
  bool HasManipulator( ViewerTypeEnum eViewerType, const std::string & sType );

  /// Lists all the manipulators within the viewer to the debug log
  void ListManipulators( ViewerTypeEnum eViewerType );

  /// removes all manipulators
  void ClearManipulators();

  /// adds a renderer to the specified viewer
  void AddRenderer( ViewerTypeEnum eViewerType, vxRenderer *  pRenderer, const std::string & sType);

  /// removes all renderers of this type from the specified viewer
  void RemoveRenderer(ViewerTypeEnum eViewerType, const std::string & sType );

  /// checks if the renderer is contained within the specified viewer
  bool HasRenderer( ViewerTypeEnum eViewerType, const std::string & sType );

  /// removes all renderers
  void ClearRenderers();
  
  /// returns the enabled state of a specified viewer
  bool GetEnabled( ViewerTypeEnum eViewerType );

  /// sets the enabled state of a specified viewer
  void SetEnabled( ViewerTypeEnum eViewerType, bool bEnabled );

  /// sets the viewer type for the specified viewer
  void SetViewerType( ViewerTypeEnum eViewerType, vxViewerType * pViewerType );

  /// sets the size of a subviewer viewer
  void SetSubViewerSize( int4 x, int4 y );
  
  /// resets the viewer to a default state
  void Reset();

  /// returns data to the screen image
  void GetImage( int4 x, int4 y, int4 iWidth, int4 iHeight, uint1 * pData );

  /// draws an image to the screen
  void DrawImage( int4 x, int4 y, int4 iWidth, int4 iHeight, uint1 * pData );
  
  /// gets the color of the viewer's border
  PixelRGBA< uint1 > GetBorderColor() { return m_borderColor; }
  
  /// sets the color of the viewer's border
  void SetBorderColor( PixelRGBA<uint1> borderColor ) { m_borderColor = borderColor; }
  
  /// gets the border width in pixels
  float4 GetBorderWidth() const { return m_fBorderWidth; }

  /// sets the border width in pixels
  void SetBorderWidth( float4 fBorderWidth ){ m_fBorderWidth = fBorderWidth; }

  bool HasBorder() const { return m_bDrawBorder; }

  void SetHasBorder( bool bHasBorder ) { m_bDrawBorder = bHasBorder; }

  /// creates the opengl context and associated it with the specified window handle
  bool Create( HWND hwnd );

  ///  destroy the opengl context
  void Destroy();

  void OnModified( vxModifiedInfoList & info );

private:

  /// clears the OpenGL buffers
  void ClearGlBuffer(vxViewer * pViewer);

  /// draws a border on the inside of the window
  void DrawBorder();

// data
private:
   
  /// viewers collection
  Viewers m_viewers;

  /// vxEnvironment pointer
  vxShareableObject<vxEnvironment> * m_pEnvironment;

  /// indicates if the a border should be drawn in the window
  bool m_bDrawBorder;  
  
  /// viewer's native window handle
  vxWindowHandle m_windowHandle;

  /// color of the viewer's border
  PixelRGBA< uint1 > m_borderColor;
  
  // width of the border in pixels
  float4 m_fBorderWidth;

}; // vxViewerControl


// $Log: vxViewerControl.h,v $
// Revision 1.1  2007/10/24 20:33:32  gdavidson
// Moved from Visualization
//
// Revision 1.5  2007/05/17 20:05:06  gdavidson
// Added SetSubViewerSize
//
// Revision 1.4  2006/11/17 20:17:13  geconomos
// temporary code for fusion demo
//
// Revision 1.3  2006/10/10 15:37:59  geconomos
// + removed UpdateViewer
// + cancel rendering when resetting the viewer
// + Issue 5024: image from prior viewer type incorrectly displaying
//
// Revision 1.2  2006/09/28 21:06:25  romy
// moved undo actions to its on class
//
// Revision 1.1  2006/09/27 17:06:56  geconomos
// using vx naming convention for unmanaged classes
//
// Revision 1.11  2006/09/19 18:28:33  gdavidson
// Issue #4989, #4990, #4992, #4997: Ignored alpha channel when getting image
//
// Revision 1.10  2006/07/12 19:34:03  geconomos
// code clean up
//
// Revision 1.9  2006/04/25 20:22:34  romy
// StackModifiedFuncPtr added
//
// Revision 1.8  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.7  2005/12/08 16:43:13  frank
// added undo support
//
// Revision 1.6  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.6.1  2005/07/06 15:04:47  frank
// removed the viewerbar (and the black-out rectangle) from snapshots
// inverted the image during snapshot
//
// Revision 1.5  2004/08/17 14:52:59  geconomos
// a) Renamed OnCreate and OnDestroy to Create and Destroy respectively.
//
// b) Moved class to Create and Destroy to managed parent.
//
// Revision 1.4  2004/05/20 12:47:54  frank
// added controllers
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
// Revision 1.31  2003/12/18 17:02:22  frank
// doxygen likes three slashes for data members
//
// Revision 1.30  2003/12/17 15:41:19  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.29  2003/12/16 19:14:53  michael
// modified the timing statistics display and turned it off by default
//
// Revision 1.28  2003/12/16 13:39:11  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.27  2003/11/17 17:57:09  frank
// Added function to list all the manipulators
//
// Revision 1.26  2003/10/01 00:44:36  geconomos
// Changed GetImage() function.
//
// Revision 1.25  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.24  2003/07/24 13:04:19  michael
// added redraw border instead of causing a full Redraw on the viewer and its
// clients (renderers)
//
// Revision 1.23  2003/05/16 13:14:16  frank
// formatting
//
// Revision 1.22  2003/05/13 19:08:32  michael
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerControl.h,v 1.1 2007/10/24 20:33:32 gdavidson Exp $
// $Id: vxViewerControl.h,v 1.1 2007/10/24 20:33:32 gdavidson Exp $
