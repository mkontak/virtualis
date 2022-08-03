// $Id: GLWnd.h,v 1.6.2.1 2007/10/03 02:28:57 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GLWnd.h
// Description: Interface for an OpenGL-enabled CWnd
// Owner: Jeffrey Meade  jmeade@viatronix.com


#pragma once

#include "GLChildView.h"
#include "WinGDIUtils.h"
#include "typedef.h"
#include "resource.h"
#include "GLFont.h"

namespace ColonViews
{

namespace Layouts
{

class GLWnd : public CWnd
{
/// member functions
public:
  
  /// constructor from background resource id
  GLWnd(const uint4 uViewBkgdResID = 0);
  
  /// virtual destructor
  virtual ~GLWnd();

  /// send a message to one or more views
  void BroadcastMessage( int4 iWndTypes,  const UINT uMsg, const WPARAM wParam,
                          const LPARAM lParam,  const bool bWait );

  /// does this window display multiple datasets
  virtual bool IsDisplayingMultipleDatasets() const { return false; }

  /// gets the GLChildView identified by uDlgControlID (analogous to CWnd::GetDlgItem())
  ChildViews::GLChildViewRef GetChildViewDlgItem( const uint4 uDlgControlID );

  /// gets information on a child view by ID
  ChildViews::GLChildView::Info GetChildViewInfo( const ChildViews::GLChildView::GLVID & uChildID ) const;
  
  /// gets a child view by ID
  ChildViews::GLChildViewRef GetChildView( const ChildViews::GLChildView::GLVID & uId ) const;
  
  /// gets a child view by its index in the child view vector
  inline ChildViews::GLChildViewRef GetChildViewByPos( const int4 iPos ) const;
  
  /// gets the null-event view (background view, for events that don't occur in any real view)
  inline ChildViews::GLChildViewRef GetNullEventView() const;
  
  /// gets the first child view of the specified type
  ChildViews::GLChildViewRef GetChildViewByType( const ChildViews::GLChildView::ViewWndTypeEnum eType ) const;
  
  /// gets the current child view
  ChildViews::GLChildViewRef GetCurrentView() const;

    /// gets the resource id for the background image
  const uint4 GetBkgdResId() const { return m_uViewBkgdResID; }
  
  /// gets the child view vector index of a view by its ID
  int4 GetChildViewPos( const ChildViews::GLChildView::GLVID & id ) const;

  /// gets the childview at the given point
  enum CVPFlagsEnum { eCVP_All = 0, eCVP_OnlyVisible = 1 };

  ChildViews::GLChildViewRef GetChildViewFromPoint( const Point2D<int4>& clientPt, const CVPFlagsEnum& eFindFlags = eCVP_OnlyVisible ) const;

  virtual uint4 OnShowPopupMenu(const ChildViews::GLChildView& childView, const uint4 uPopupMenuID) const;

  virtual afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

  virtual afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

  void InvalidateViews( WPARAM wParam, LPARAM lParam );

  /// Renders all child views; override for specific instances
  virtual void GLRenderViews();

  /// Render the child at the specified view vector index
  void GLRenderView(const int4 iView);
  
  /// Renders the child view
  void GLRenderView(ChildViews::GLChildViewRef childView);

  /// Idle event loop handler
  virtual void GLIdle();

  /// Returns number of valid child views
  inline int4 NumGLViews() const { return (m_vpViews.size() - 1); }

  /// initially updates the child views
  virtual void BeginInitialUpdate();

  /// called when the view has been initialized
  virtual void OnInitialized();

  /// called before the current layout changes
  virtual void OnLayoutChanging();

  /// called after the current layout changes
  virtual void OnLayoutChanged();

protected:

  /// set child views
  void SetChildViews( const std::list< ChildViews::GLChildViewPtr > & childViewPtrList );

  /// override to call constructors for views
  virtual void CreateChildViews( HWND hWndThis ) = 0;
  
  /// Override to calculate the view rectangle (not viewports!) for each view
  virtual void CalculateViewRects(const int4 iWidth, const int4 iHeight) = 0;
  
  /// Override to perform any necessary rendering for any stored child controls
  virtual void RenderControls() { }
  
  /// !If necessary! Override to redirect a certain broadcasted messages, if necessary
  virtual void RedirectBroadcastMessage(int4 & iWndTypes, const UINT uMsg) const { }

  /// Returns the display rectangle of the child view at the given view vector index
  Rect<int4> GetViewRect(const int4 iViewPos) const;
  
  /// Returns the display rectangle of the specified child view
  Rect<int4> GetViewRect(const ChildViews::GLChildView& view) const;
  
  /// Sets the view rectangle of the child view at the given view vector index
  void SetViewRect(const int4 iViewPos, const Rect<int> & viewRect);
  
  /// Sets the view rectangle of the given child view
  void SetViewRect(ChildViews::GLChildViewRef childView, const Rect<int> & viewRect);
  
  /// Swaps the view rectangles of two child views
  void SwapViewRects(ChildViews::GLChildViewRef view1, ChildViews::GLChildViewRef view2);
  
  /// A little hack to send the OnSizing() msg to a childview
  void SendOnSizingMsg(ChildViews::GLChildViewRef childView, Rect<int4>& rRect) { childView.OnSizing(0, rRect); }

  /// Returns the captured view if any (or NULL if no captured view)
  const ChildViews::GLChildViewPtr GetCaptureViewPtr();

  /// indicates if a WM_MOUSEMOVE needs to be re-routed
  bool NeedToRerouteMouseMove( HWND hWnd );

  /// return the brush to be used for the view's background
  virtual CBrush * GetViewBackgroundBrush();

  // messages
protected:

  DECLARE_MESSAGE_MAP()

  virtual afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  
  virtual afx_msg void OnPaint();

  /// WM_ERASEBKGND handler, called when background needs erasing
  virtual afx_msg BOOL OnEraseBkgnd( CDC* pDC );

  virtual afx_msg void OnSize( UINT uType, int iCX, int iCY );
  
  virtual afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT uCtlColor );
  
  virtual afx_msg void OnTimer( UINT_PTR nIDEvent );

  virtual afx_msg void OnMouseMove( UINT nFlags, CPoint point );

  virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

  virtual BOOL PreTranslateMessage( MSG * pMsg );
  
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );  

  virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam );

  virtual void OnDestroy();
  
  /// Flag indicated GLChildView view type for refresh
  virtual afx_msg LRESULT OnAppGLChildViewTypeInvalidate( WPARAM wpGLChildViewType, LPARAM );  

 
  /// Application-message:  Gets the ID of child view in vector index specified by wParam
  virtual afx_msg LRESULT OnAppGetChildViewID(WPARAM wParam, LPARAM lParam);
  
  /// Application-message:  For idle-loop message processing.
  afx_msg LRESULT OnAppGLIdle(WPARAM, LPARAM);
  
  /// Application-message:  Message for individual child view
  virtual afx_msg LRESULT OnAppChildViewMessage(WPARAM wParam, LPARAM lParam);
  
  /// Application-message:  Change in a child view timer
  afx_msg LRESULT OnAppChildViewTimerStatusChange(WPARAM iTimerID, LPARAM uChildControlID);

public:

  /// null child view
  static ChildViews::GLChildView m_nullChildView;

protected:

  // Vector of miscellaneous view windows/controls/etc.
  typedef std::list<CWnd *> CWndPtrList;
  CWndPtrList m_controlPtrList;

  /// Idle routine that will be called after each WM_PAINT event
  typedef void (* IdleRoutine)(void *);
  
  /// Maps an idle routine to its user-defined data
  typedef std::map<IdleRoutine, void*> IdleRoutineToDataMap;
  
  /// Current set of idle routines that will be called after each WM_PAINT event
  IdleRoutineToDataMap m_mapIdleRoutines;

  /// derived classes can override to remove areas from the parent region
  virtual void OnUpdateBackgroundRegion( CRgn & parentRgn ) {}

private:

    /// user specific data passed to UpdateBackgroundRegionCallback function
  struct EnumChildWindowsData
  {
    /// parent window
    CWnd * m_pParentWnd;

    /// parent region
    CRgn * m_pParentRgn;
  };

  /// callback function for EnumChildWindows ( used when calculating HRGN for background updates )
  static BOOL CALLBACK UpdateBackgroundRegionCallback( HWND hWnd, LPARAM lParam );

  /// callback function for EnumChildWindows, used to send LayoutChanging/Changed events to dialogs
  static BOOL CALLBACK Callback_SendLayoutChangeEvent( HWND hWnd, LPARAM lParam );

  /// vector of child views; will contain an extra view for events which don't correspond to any real view.
  std::vector<ChildViews::GLChildViewPtr> m_vpViews;

  /// resource id for background image
  uint4 m_uViewBkgdResID;

  /// last known mouse position
  CPoint m_lastMousePosition;

  /// the one view that has mouse input capture (if any).
  ChildViews::GLChildViewPtr m_pCaptureView;
  
  /// index of view in which mouse cursor lies.
  int4 m_iMouseInViewWnd;
  
  /// width and height of window, set in OnSize().
  CSize m_viewWndSz;
  
  /// maps timer IDs to their corresponding child view control ID
  std::map<uint4, uint4> m_mapTimerToChildCtrlID;

  /// cache of the last mouse move event mouse position
  CPoint m_lastMouseMovePosition;

  /// initial update timer
  static const int4 m_iInitialUpdateTimerId;

  /// initial update timer frequency
  static const int4 m_iInitialUpdateTimerFrequency;

// friend classes
public:

  friend class ViatronixWnd;
  friend class ColonViatronixWnd;
  friend class ChildViews::GLChildView;
  friend class GLWnd;

}; // class GLWnd

typedef GLWnd *GLWndPtr;
typedef GLWnd &GLWndRef;


/**
 * Gets a child view reference by view vector position (i.e. the i-th view)
 *
 * @param const int4 iPos   Position of the child view
 * @return GLChildViewRef   Reference to the corresponding child view window
 */
inline ChildViews::GLChildViewRef GLWnd::GetChildViewByPos(const int4 iPos) const
{
#ifndef FINAL_RELEASE
  if (iPos < 0 || iPos > NumGLViews())
  {
    return m_nullChildView;
  }
#endif

  return *m_vpViews[iPos];
}


/**
 * Gets the null-event view (background view, for events that don't occur in any real view)
 * @return GLChildViewRef   Reference to the null event child view window
 */
inline ChildViews::GLChildViewRef GLWnd::GetNullEventView() const
{
  return *m_vpViews[NumGLViews()];
}

} // namespace Layouts

} // namespace ColonViews

// $Log: GLWnd.h,v $
// Revision 1.6.2.1  2007/10/03 02:28:57  jmeade
// allow childviews to set their own window background colour.
//
// Revision 1.6  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5  2007/01/09 19:12:31  jmeade
// layout changed/changing events.
//
// Revision 1.4  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.2.4  2006/05/16 12:42:33  geconomos
// proper destruction of child views in WM_DESTROY handler
//
// Revision 1.2.2.3  2006/02/28 17:25:15  geconomos
// reworked WM_MOUSEMOVE handling
//
// Revision 1.2.2.2  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.2.2.1  2006/01/04 13:05:32  frank
// Moved default background brush to the theme class
// Pre-blended logo for supine/prone view to fix over-accumulation of white
//
// Revision 1.2  2005/10/28 18:02:17  geconomos
// code cleanup
//
// Revision 1.1  2005/10/28 17:47:34  geconomos
// renamed files from GLViewWnd => GLWnd
//
// Revision 1.8  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.7  2005/10/03 13:22:54  geconomos
// removed references to brushes
//
// Revision 1.6  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.5  2005/08/17 18:58:44  geconomos
// Added support to render the background using a default solid brush instead of an image. ( vcsettings.xml - DrawBackgroundImage )
//
// Revision 1.4  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
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
// Revision 3.19.2.12.4.7.2.4  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 3.19.2.12.4.7.2.3  2005/07/14 14:39:42  geconomos
// Issue #4275: ROI not updating while switching views
//
// Revision 3.19.2.12.4.7.2.2  2005/07/11 19:51:51  geconomos
// Optimazation of RegistrationView
//
// Revision 3.19.2.12.4.7.2.1  2005/06/17 13:16:09  geconomos
// using new BackgroundBrush class
//
// Revision 3.19.2.12.4.7  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.19.2.12.4.6  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.19.2.12.4.4.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.19.2.12.4.4.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLWnd.h,v 1.6.2.1 2007/10/03 02:28:57 jmeade Exp $
// $Id: GLWnd.h,v 1.6.2.1 2007/10/03 02:28:57 jmeade Exp $
