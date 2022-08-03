// $Id: GLChildView.h,v 1.13.2.3 2010/03/09 20:14:18 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net

// pragmas
#pragma once

// includes
#include "GLFont.h"
#include "Rect.h"
#include "WmTimer.h"
#include "V1KMessages.h"
#include "GLUpdateBuffer.h"

// forward declarations
namespace ColonViews
{
  namespace Layouts
  {
    class GLWnd;
  }
}

/// log function
inline const double LogB(const float8 & fBase, const float8 & fVal) { return(log(fVal) / log(fBase)); }

namespace ColonViews
{

namespace ChildViews
{

/// A single OpenGL-enabled child view window
class GLChildView  : public CWnd
{
// enums and definitions
public:   

  /// GLChildView Instance identifier
  typedef uint8 GLVID;

  /// Types of registered child views
  enum ViewWndTypeEnum
  {
    eGVT_NONE            = 0,
    eGVT_OVERVIEW        = 1,
    eGVT_REPORTENTRY     = 1 << 1,
    eGVT_CROSS_SECTION   = 1 << 2,
    eGVT_NAVIGATION      = 1 << 3,
    eGVT_CONTROLS        = 1 << 4,
    eGVT_AXIAL           = 1 << 5,
    eGVT_SAGITTAL        = 1 << 6, 
    eGVT_CORONAL         = 1 << 7,
    eGVT_2DSEGMENTS      = 1 << 8,
    eGVT_3DSEGMENTS      = 1 << 9,
    eGVT_UDSEGMENTSMAUI  = 1 << 10,
    eGVT_ALL             = (1 << 11) - 1,
    eGVT_ORTHOGONAL = eGVT_SAGITTAL + eGVT_CORONAL + eGVT_AXIAL,
    eGVT_SLICE = eGVT_CROSS_SECTION + eGVT_ORTHOGONAL + eGVT_2DSEGMENTS,
  };

  /// Info on a child view
  class Info
  {
  public:
    /// Default constructor
    Info() : m_uId(0), m_eType(eGVT_NONE) { }
    /// Copy constructor
    Info(const Info & src) { *this = src; }
    /// Copy operator
    Info & operator=(const Info & src) { m_uId = src.m_uId; m_eType = src.m_eType; return *this; }

  public:
    /// Id of the view
    GLVID m_uId;
    /// Type of the view
    ViewWndTypeEnum m_eType;
  };

// member functions
public:
  
  /// constructor
  GLChildView( const ViewWndTypeEnum eGLViewType = eGVT_NONE  );
  
  /// destructor
  virtual ~GLChildView();

  /// renders the view (includes render and raster)
  virtual void GLRender() { GLRender( true ); }

  /// renders the view (includes render and raster)
  void GLRender( bool bSwapBuffers );

  /// returns true to redraw the view on idle
  virtual bool GLIdle();

  /// sends a window message to the view
  LRESULT SendMessage( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );

  /// Retrieves update image; DO NOT DELETE DATA!
  virtual const void * GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat,
    const GLenum eDatatype = GL_UNSIGNED_BYTE, const bool bOnlyGLRenderScene = false);

  /// called to support custom invalidation
  virtual void InvalidateView() {}

  void InvalidateAsscociatedControl( CWnd & control );

  /// Returns true if this is a Main View or the is no main view window defined
  bool IsMainViewWindow() const;

  /// gets the image buffered flag
  inline bool GetImageBuffered() const { return m_bImageBuffered; }

  /// sets the image buffered flag
  inline void SetImageBuffered( bool bImageBuffered ) { m_bImageBuffered = bImageBuffered; }

  /// set up the flag: inverse image (when taking snapshot).
  virtual void SetInverseImage(const bool bInverseImage ) { m_bInverseImage = bInverseImage; }

  /// get flag of inverse image.
  inline bool GetInverseImage() const { return m_bInverseImage; }

  /// gets the control ID for the view
  inline const uint4 GetControlID() const { return m_uControlID; }
  
  /// sets a control ID for the view
  inline void SetControlID(const uint4 uControlID) { m_uControlID = uControlID; }

  /// returns the child view type
  inline ViewWndTypeEnum GetViewWndType() const { return m_eGLChildViewType; }

  /// sets the child view type
  void SetViewWndType( const ViewWndTypeEnum eType );

  /// gets the viewport rectangle (client area)
  inline const Rect< int4 > & GetViewport() const { return m_viewport; }

  /// gets the view rectangle (window rect)
  const Rect< int4 > GetViewRect() const;

  /// gets the class instance identifier
  inline GLVID GetClassInstanceID() const { return m_uClassInstanceID; }

  /// gets proper type info
  virtual const type_info& GetClassTypeInfo() { return typeid(*this); }

  /// gets the resource ID of the popup menu for this view
  virtual uint4 GetPopupMenuID( UINT uFlags, CPoint point ) const { return 0; }
  
  /// allows view to alter popup menu before display (e.g. add/remove/disable items)
  virtual void AdjustPopupMenu( CMenu & menu, const uint4 uMenuID ) const {}

  /// set string resource IDs for mouse use key
  virtual void GetMouseKeyStringIDs( uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID ) const {}

  /// called when the current dataset has changed
  virtual void OnDatasetChanged() {}

  /// set whether snapshots for a report are being taken
  static void SetReportRendering(const bool bOn) { m_bReportRendering = bOn; }

  /// get whether snapshots for a report are being taken
  static bool IsReportRendering() { return m_bReportRendering; }

protected:

  /// called before the current layout changes
  virtual void OnLayoutChanging() {}

  /// called after the current layout changes
  virtual void OnLayoutChanged() {}

  /// creates the rendering context
  virtual bool GLCreate();

  /// frees the rendering context
  virtual void GLDestroy();

  /// sets up the opengl rendering context
  virtual void GLSetupRC();
  
  /// renders the 3D scene
  virtual void GLRenderScene(void * pData = NULL);
  
  /// 2D Scene rendering
  virtual void GLRasterScene();

  /// called to resize the view
  virtual void GLResize( GLsizei iWidth, GLsizei iHeight );
  
  /// resize on the 2D rendering context
  virtual void GLRasterResize( GLsizei iWidth, GLsizei iHeight );

  /// Save a buffer of the current view
  void GLSaveUpdateBuffer( const bool bInverseImage = false );

  /// Render a previously-saved buffer
  bool GLRenderUpdateBuffer( const bool bAdjustViewport = false );

  /// renders the "Viatronix" logo
  void GLRenderLogo( bool bTranslucent = false );

  /// invalidates the specified views
  void InvalidateViews( WPARAM wParam, LPARAM lParam );

  /// gets the opengl rendering context
  virtual HGLRC GetRenderingContext() const { return m_hrc; }

  /// sets the opengl rendering context
  inline void SetRenderingContext( HGLRC hrc ) { m_hrc = hrc; }

  /// get the "render only the background" flag
  inline bool GetRenderOnlyBackground() const { return m_bRenderOnlyBackground; }

  /// get the "render only the background" flag
  inline void SetRenderOnlyBackground( bool bRenderOnlyBackground ) { m_bRenderOnlyBackground = bRenderOnlyBackground; }

  /// gets the associated HDC for this child view
  virtual HDC GetHDC() const { return m_pdc->GetSafeHdc(); }

  /// gets the normal font
  inline GLFont & GetNormalFont() { return m_normalGLFont; }

  /// gets the large font
  inline GLFont & GetLargeFont() { return m_largeGLFont; }

  /// gets the parent GLWnd
  inline ColonViews::Layouts::GLWnd * GetParentGLWnd() { return m_pGLWnd; }

  /// Whether or not this child view has mouse capture
  bool IsCaptured();

  /// sets the viewport
  void SetViewPort( int4 iWidth, int4 iHeight );


/************************ MFC message handlers **************************/
  
  DECLARE_MESSAGE_MAP()
  
  virtual int  OnCreate( LPCREATESTRUCT lpCreateStruct );
  virtual BOOL OnEraseBkgnd( CDC * pDC );  
  virtual void OnDestroy();
  virtual void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags){ }
  virtual void OnKeyUp(UINT uChar, UINT uRepCnt, UINT uFlags){ }
  virtual void OnLButtonDblClk( UINT uFlags, CPoint point );
  virtual void OnLButtonDown(UINT uFlags, CPoint point){}
  virtual void OnLButtonUp(UINT uFlags, CPoint point){}
  virtual void OnMButtonDblClk(UINT uFlags, CPoint point){}
  virtual void OnMButtonDown(UINT uFlags, CPoint point){}
  virtual void OnMButtonUp(UINT uFlags, CPoint point){}
  virtual void OnMouseMove(UINT uFlags, CPoint point){}
  virtual BOOL OnMouseWheel(UINT uFlags, short zDelta, CPoint pt){ return FALSE; }
  virtual void OnPaint();
  virtual void OnRButtonDblClk(UINT uFlags, CPoint point){}
  virtual void OnRButtonDown(UINT uFlags, CPoint point);
  virtual void OnRButtonUp(UINT uFlags, CPoint point){}
  virtual BOOL OnSetCursor( CWnd* pWnd, UINT uHitTest, UINT uMessage );
  virtual void OnSize( UINT uType, int4 iCX, int4 iCY );
  virtual void OnTimer(const uint4 uIDEvent) {};
  virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam );
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) { return FALSE; }
  virtual int4 OnMessage(uint2 uMessage, int4 lParam) {return 0; }
  virtual void OnMouseHover( UINT uFlags, CPoint point ) {}
  virtual void OnMouseLeave() {}
  virtual void OnSizing( const UINT uUnused, Rect<int4>& viewRect ) {};
  virtual void OnWindowPosChanged( WINDOWPOS * pWndPos ) { CWnd::OnWindowPosChanged( pWndPos ); }
  virtual void OnWindowPosChanging( WINDOWPOS * pWndPos ) { CWnd::OnWindowPosChanging( pWndPos ); }


private:

  /// returns a prepared (items enabled/disabled/removed per context) popup menu for display.
  bool PreparePopupMenu(UINT uFlags, CPoint point, CMenu & popupMenu);

  /// displays a popup menu for the current child view
  int4 ShowPopupMenu(UINT uFlags, CPoint point);

  /// performs special processing on an image.
  enum ImageProcessEnum { eIP_RedBlueSwap };

  /// performs the specified image processing on the image data
  static void ImageProcess(const ImageProcessEnum eType, uint1 * pImage, const uint4 uImgWidth, const uint4 uimgHeight);

// member variables
protected:

  /// the number of pixels of padding at the edges
  static const int4 m_iTextPadding = 3;

  /// the number of pixels extra padding at the bottom of the image
  static const int4 m_iBottomExtraTextPadding = 2;

private:

  /// windows device context
  CClientDC * m_pdc;
  
  /// gl rendering context
  HGLRC m_hrc;

  /// normal gl font 
  GLFont m_normalGLFont;

  /// large size gl font
  GLFont m_largeGLFont;

  /// texture id for the update buffer
  GLuint m_uTexture;
  
  /// width of update buffer
  int8 m_iUpdateWidth;
  
  /// height of update buffer
  int8 m_iUpdateHeight;

  /// temporary update buffer
  OpenGLUtils::GLUpdateBuffer m_tempUpdateBuffer;

  /// view update/snapshot buffer
  unsigned char * m_pUpdateBuffer;
  
  /// whether the image in the update buffer is up-to-date
  bool m_bImageBuffered;

  /// flag: inverse image in this view.
  bool m_bInverseImage;

  /// control ID for the view, used if necessary like Windows dialog control IDs
  uint4 m_uControlID;

  /// indicates if only the background should be rendered
  bool m_bRenderOnlyBackground;

  /// viewport, same as client area
  Rect< int4 > m_viewport;

  /// view rect, same as window rect
  Rect< int4 > m_viewrect;

  /// Identifies the type of the class instance
  ViewWndTypeEnum m_eGLChildViewType;
  
  /// Identifies the instance of the class
  const GLVID m_uClassInstanceID;

  /// Global counter for unique IDs for each instance
  static GLVID uNextClassInstanceID;

  /// typed pointer to parent GLWND
  ColonViews::Layouts::GLWnd * m_pGLWnd;

  /// whether we are taking snapshots for the report
  static bool m_bReportRendering;

// friend declarations
private:
  
  friend class Layouts::GLWnd;

}; // GLChildView


/// Child view pointer
typedef GLChildView * GLChildViewPtr;

/// Child view pointer
typedef GLChildView & GLChildViewRef;

} // namespace ChildViews

} // namespace ColonViews

// $Log: GLChildView.h,v $
// Revision 1.13.2.3  2010/03/09 20:14:18  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.2  2009/01/12 22:21:36  jmeade
// set pick buffer to be the exact size of the window, eliminating buffer overuns for large displays.
//
// Revision 1.13.2.1  2007/10/03 20:01:38  jmeade
// typos.
//
// Revision 1.13  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.12  2007/02/02 21:10:31  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.11  2007/01/03 16:51:40  jmeade
// future ud segments layout.
//
// Revision 1.10  2006/02/07 23:22:42  jmeade
// code review: comments.
//
// Revision 1.9  2006/01/31 18:20:51  romy
// code review-rerranged include files
//
// Revision 1.8  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.7  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.6.2.3  2006/05/09 13:53:59  geconomos
// corrected GetViewrect to return a copy of a rect<int4> instead of passing a temporary reference back
//
// Revision 1.6.2.2  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.6.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.6  2005/10/28 17:48:54  geconomos
// added OnLayoutChanging and OnLayoutChanged
//
// Revision 1.5  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/08/16 21:46:06  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.3  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.18.2.6.4.10.2.10  2005/07/15 18:25:44  geconomos
// Added methods to invalidate parent window where external controls are placed.
//
// Revision 3.18.2.6.4.10.2.9  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 3.18.2.6.4.10.2.8  2005/07/14 14:39:42  geconomos
// Issue #4275: ROI not updating while switching views
//
// Revision 3.18.2.6.4.10.2.7  2005/07/14 14:16:31  geconomos
// Refactored logo renering code
//
// Revision 3.18.2.6.4.10.2.6  2005/07/12 19:16:54  geconomos
// Added accesor for internal GLWnd parent pointer
//
// Revision 3.18.2.6.4.10.2.5  2005/07/06 18:00:41  geconomos
// Renamed Invalidate to invalidateView ( clashing with CWnd::Invalidate() )
//
// Revision 3.18.2.6.4.10.2.4  2005/06/30 16:37:28  geconomos
// Propagated argument to Invalidate
//
// Revision 3.18.2.6.4.10.2.3  2005/06/24 13:01:52  geconomos
// made SetInverseImage virtual
//
// Revision 3.18.2.6.4.10.2.2  2005/06/21 15:52:51  geconomos
// restored revision history
//
// Revision 3.18.2.6.4.10.2.1  2005/06/21 15:14:50  geconomos
// Added method to set the viewport size; changed GetHDC() to be virtual
//
// Revision 3.18.2.6.4.10  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.18.2.6.4.9  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.18.2.6.4.7.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.18.2.6.4.7.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 3.18.2.5.4.7  2005/04/21 12:55:17  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 3.18.2.5.4.6  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
//
// Revision 3.18.2.5.4.5  2005/04/19 13:40:55  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.18.2.5.4.4  2005/04/13 18:54:56  frank
// split out GL update buffer class
//
// Revision 3.18.2.5.4.3  2005/04/08 13:05:58  frank
// capitalization
//
// Revision 3.18.2.5.4.2  2005/04/05 19:25:42  frank
// Issue #4065: draws a blank image if no data loaded
//
// Revision 3.18.2.5.4.1  2005/03/21 19:53:59  frank
// Issue #4038: Sped up font drawing
//
// Revision 3.18.2.5  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.18.2.4  2003/07/18 17:54:12  jmeade
// Comments.
//
// Revision 3.18.2.3  2003/07/18 16:13:41  jmeade
// Child view timers, code standards, comments.
//
// Revision 3.18.2.2  2003/04/15 21:33:28  jmeade
// Issue 3001: Allow childview to alter display rectangle on sizing.
//
// Revision 3.18.2.1  2003/02/26 17:41:17  jmeade
// More concise method for triggering child view updates.
//
// Revision 3.18  2003/01/03 23:08:15  jmeade
// Reordered includes to appease gdi+.
//
// Revision 3.17  2002/11/19 20:17:39  jmeade
// Fixed memory leak: must delete 2D rendering context in child view.
//
// Revision 3.16  2002/11/04 20:37:37  kevin
// Mad annotations 3D instead of 2D
//
// Revision 3.15  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 3.14  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.13  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.11  2002/07/29 17:06:34  jmeade
// Must ensure valid/non-zero values in calls to glReadPixels, or else....CRASH!!!!;
//  Coding standards.
//
// Revision 3.10  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.9  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.8  2002/04/12 02:12:22  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.7  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.6.2.6  2002/08/07 23:03:44  jmeade
// ImageProcess function cleanup.
//
// Revision 3.6.2.5  2002/07/01 19:22:55  jmeade
// Expose GLUpdateBuffer class, add allocation routine.
//
// Revision 3.6.2.4  2002/06/17 18:31:47  jmeade
// Code standards.
//
// Revision 3.6.2.3  2002/06/12 02:20:09  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.6.2.2  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.6.2.1  2002/02/05 19:57:50  jmeade
// Comments; Code standards.
//
// Revision 3.6  2001/12/22 00:08:32  jmeade
// Generalized update buffer retrieval function; Code standards.
//
// Revision 3.5  2001/11/20 22:03:09  binli
// solved problem of unstable slice image: reset viewport when rendering buffered slice image
// (SliceChildView: try to center the slice, so viewport is different from viewwnd.)
//
// Revision 3.4  2001/11/17 03:32:31  jmeade
// Windows created from forms; IsVisible() and controlID characteristics of
//  child views; threw out the old carpet
//
// Revision 3.3  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2.2.1  2001/11/04 04:31:20  jmeade
// Coding standards.
//
// Revision 3.2  2001/10/19 02:27:15  jmeade
// Get current mouse capture convenience functions; Coding standards
//
// Revision 3.1  2001/10/17 21:56:44  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 12 2001 23:07:46   jmeade
// Coding standards
// 
//    Rev 2.0   Sep 16 2001 23:41:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:10   ingmar
// Initial revision.
// Revision 1.56  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.55  2001/05/12 00:00:59  jmeade
// Coding conventions
//
// Revision 1.54  2001/04/13 02:33:21  jmeade
// headers and footers
//
// Revision 1.53  2001/04/05 17:56:29  jmeade
// Coding standards!!!
//
// Revision 1.52  2001/03/08 15:52:10  geconomos
// Worked on grabbing Non-Distended regions when creating report.
//
// Revision 1.51  2001/02/28 23:30:30  liwei
// Overview rerendered when buttons on VerifyView clicked
//
// Revision 1.50  2001/02/28 17:53:55  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.49  2001/02/09 20:41:22  jeff
// Crash fix: memory for m_pDialogBar is not created by GLChildView
//
// Revision 1.48  2001/02/06 22:27:23  jeff
// moved the default dialog bar position closer to the view
//
// Revision 1.47  2001/02/05 23:32:35  liwei
// Non-live views in EndoAxialView are also grayed out
//
// Revision 1.46  2001/02/05 22:50:00  liwei
// Flag showing whether non-active views grayed out moved to ViatronViewWnd
//
// Revision 1.45  2001/02/05 22:19:31  liwei
// Non-active views grayed out during flying
//
// Revision 1.44  2001/02/02 16:28:02  liwei
// A try-catch pair added to wrap the rendering of TTP, hopefully solve the disapperaing problem
//
// Revision 1.43  2001/01/31 18:19:40  jeff
// CreateDlgBar(): doesn't need to take in a pointer; removed unnecessary friends
//
// Revision 1.42  2001/01/30 22:57:53  geconomos
// Added stacke based calls
//
// Revision 1.41  2001/01/29 22:14:11  liwei
// Texture object is exploited for update buffer
//
// Revision 1.40  2001/01/26 19:08:48  geconomos
// Changed GetupdateBuffer() to read from back buffer
//
// Revision 1.39  2001/01/24 20:42:03  liwei
// no message
//
// Revision 1.38  2001/01/15 19:46:08  jeff
// GetViewPort() method
//
// Revision 1.37  2001/01/15 16:37:55  jeff
// Bug: top and left were transposed, for offsetting points
//
// Revision 1.36  2001/01/13 00:33:19  jeff
// Using new Rect<> and Tuple<> classes; more CPoint and CRect translation convenience functions
//
// Revision 1.35  2001/01/11 21:57:53  geconomos
// Added GetUpdateBuffer()
//
// Revision 1.34  2001/01/02 21:02:38  jeff
// removed long-unused child view messages;
//  set view window type protected method, checks validity of type b/f setting
//
// Revision 1.33  2000/12/28 20:52:03  jeff
// SendMessage method, OnMessage handler, for general messages
//
// Revision 1.32  2000/12/28 19:53:54  jcohen
// Storage of database location and record number of current study
//
// Revision 1.31  2000/12/26 23:56:44  jeff
// ViewToChild/ChildToView convenience methods (akin to ScreenToClient())
//
// Revision 1.30  2000/12/26 21:06:37  jeff
// ViewToChild() and ChildToView() convenience methods
//
// Revision 1.29  2000/12/22 16:38:50  jeff
// MoveWindow() for dialog bar, strangely, doesn't preserve size set by parameters,
//  so use SetWindowPos() with SWP_NOSIZE
//
// Revision 1.28  2000/12/20 19:48:05  jeff
// reduced space between dialog bar and view window
//
// Revision 1.27  2000/12/12 23:51:44  jeff
// AdjustPopupMenu() now indicates which popup menu was specified
//
// Revision 1.26  2000/11/23 00:52:22  jeff
// Create one OpenGL font to use throughout application (instead of each
//  view creating their own every frame)
//
// Revision 1.25  2000/11/22 21:03:10  jeff
// GLChildView base class MUST return FALSE from OnCommand() to signify
//  a message (and i.e. all messages if not overridden) is not handled
//
// Revision 1.24  2000/11/20 00:56:21  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
// AdjustPopupMenu() - takes menu reference instead of pointer
// Push/pop attrib instead of disabling
//
// Revision 1.23  2000/11/07 00:19:55  jeff
// Mouse hover/leave window event handlers
//
// Revision 1.22  2000/11/01 22:44:59  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.21  2000/10/27 01:16:50  kevin
// Added Texture Mag filter
//
// Revision 1.20  2000/10/25 17:55:54  jeff
// OnCommand overridable, for menu commands, etc.
// views can specify and adjust popup menus (ShowPopupMenu() must be called by parent view window)
//
// Revision 1.19  2000/10/13 04:55:35  jeff
// Increasing the border size between a view window and its dialog bar
//
// Revision 1.18  2000/09/30 18:30:21  jeff
// Fix the problem of the moving dialog bars
//
// Revision 1.17  2000/09/30 04:41:14  jeff
// Frank and Ingmar with your mathematical accuracy :)  Now I have to use
// ceil with your LogB function...accuracy shmacuracy!!!  :-)
//
// Revision 1.16  2000/09/30 02:51:47  jeff
// Get dialog bar window rect and set view rect functions; hide dialog bar while moving
//
// Revision 1.15  2000/09/28 15:24:46  ingmar
// LogN -> LogB
//
// Revision 1.14  2000/09/27 14:43:59  jeff
// Added LogN function to global utils library
//
// Revision 1.13  2000/09/22 23:16:52  jeff
// button menus
//
// Revision 1.12  2000/09/21 19:30:45  kevin
// Added height for updatebuffers
//
// Revision 1.11  2000/09/19 16:17:40  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
//
// Revision 1.10  2000/09/18 19:33:37  jeff
// New cross-hair cursor
//
// Revision 1.9  2000/09/18 15:06:19  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.8  2000/09/15 22:46:24  jeff
// no message
//
// Revision 1.7  2000/09/15 19:21:04  jeff
// Updating views only when necessary
//
// Revision 1.6  2000/09/15 16:09:28  jeff
// All rendering/drawing now done in OpenGL (no more GDI draw calls)
//
// Revision 1.5  2000/09/15 01:58:24  jeff
// Moving to text draw in OpenGL
//
// Revision 1.4  2000/09/14 16:14:51  jeff
// Borders around all view windows
//
// Revision 1.3  2000/09/13 16:31:17  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.2  2000/09/12 20:47:03  jeff
// Resizing and repositioning overview and (coming soon) visible-surface views
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 16    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 15    9/06/00 12:44p Jeff
// Debug version clears view window
// 
// 14    9/01/00 7:59p Jeff
// Paramter to Set and Release Capture messages is now a child view
// instance ID (and no longer a variable address)
// 
// 13    8/28/00 7:27p Jeff
// Release capture message now passes GLChildViewPtr (as Set capture
// message does)
// 
// 12    8/28/00 1:17p Antonio
// updated header and footer
//
//*****************  Version 11  *****************
//User: Jeff            Date:  8/28/00  Time: 11:51a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Change of plan: references to popup menus shouldn't be here, the
//parent class should handle this
//
//*****************  Version 10  *****************
//User: Jeff            Date:  8/25/00  Time:  6:50p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Handling popup menus
//
//*****************  Version 9  *****************
//User: Jeff            Date:  8/25/00  Time:  5:28p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed ShowViewPopupMenu function
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/24/00  Time: 10:11a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  GLViewWnd's mouse-captured view is a member of GLViewWnd, and no
//longer a single global GLChildView member
//Changed WM_GL_..._INVALIDATE to WM_APP_GL_..._INVALIDATE
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/24/00  Time: 12:47a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Also return true for IsMainViewWindow() if the
//WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID message returns unanswered.
//
//*****************  Version 6  *****************
//User: Jeff            Date:  8/23/00  Time:  3:38p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Correct implementation of IsMainViewWindow()
//
//*****************  Version 5  *****************
//User: Jeff            Date:  8/22/00  Time: 11:46p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Invalidate using GL_CHILDVIEW_INVALIDATE message and overridden
//Invalidate() function calls in GLChildView
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/14/00  Time:  1:27p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//*****************  Version 3  *****************
//User: Jeff            Date:  8/12/00  Time:  8:01p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  First errorless compile after changes to class architecture
//
//*****************  Version 2  *****************
//User: Jeff            Date:  8/12/00  Time:  5:59p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Renaming GLView ==> GLChildView to reflect new class architecture
//
//*****************  Version 1  *****************
//User: Jeff            Date:  8/11/00  Time:  3:44p
//Created GLView.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLChildView.h,v 1.13.2.3 2010/03/09 20:14:18 dongqing Exp $
// $Id: GLChildView.h,v 1.13.2.3 2010/03/09 20:14:18 dongqing Exp $
