// $Id: COMViewer.h,v 1.1 2007/02/16 14:43:41 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#pragma once

// include declarations
#include "resource.h"
#include <atlctl.h>
#include "Timer.h"

// forward declarations
class vxRenderingContextGL;
class GLPaintDevice;
class GLFontGeorge;


// class declaration
[
	coclass,
	control,
	threading(apartment),
	aggregatable(never),
	vi_progid("V3D.Viewer"),
	progid("V3D.Viewer.1"),
	version(1.0),
	uuid("20FF161A-6C59-4DF5-A692-5241FA3D2009"),
	helpstring("Viewer Class"),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE COMViewer :
	public IPersistStreamInitImpl<COMViewer>,
  public IPersistPropertyBagImpl<COMViewer>,
	public IOleControlImpl<COMViewer>,
	public IOleObjectImpl<COMViewer>,
	public IOleInPlaceActiveObjectImpl<COMViewer>,
	public IViewObjectExImpl<COMViewer>,
	public IOleInPlaceObjectWindowlessImpl<COMViewer>,
	public IPersistStorageImpl<COMViewer>,
	public ISpecifyPropertyPagesImpl<COMViewer>,
	public IQuickActivateImpl<COMViewer>,
	public IDataObjectImpl<COMViewer>,
	public CComControl<COMViewer>
{
// atl macros
public:

// ole status
DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | OLEMISC_CANTLINKINSIDE | OLEMISC_INSIDEOUT | OLEMISC_ACTIVATEWHENVISIBLE | OLEMISC_SETCLIENTSITEFIRST )

// view status
DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// final construct
DECLARE_PROTECT_FINAL_CONSTRUCT()

// property map
BEGIN_PROP_MAP(COMViewer)
	PROP_DATA_ENTRY( "_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY( "_cy", m_sizeExtent.cy, VT_UI4)
  PROP_DATA_ENTRY( "dataset", m_bsInitialDirectory, VT_BSTR )
END_PROP_MAP()

/// message map
BEGIN_MSG_MAP(COMViewer)
  MESSAGE_HANDLER(WM_CREATE,OnCreate)
  MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
  MESSAGE_HANDLER(WM_SIZE,OnSize);
  MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd);
  MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
  MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
  MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
  MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLButtonDblClk)
  MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
	CHAIN_MSG_MAP(CComControl<COMViewer>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()


// member functions
public:

  /// constructor
  COMViewer();

  /// handles drawing for the control
  HRESULT OnDraw( ATL_DRAWINFO & di );

private:

  /// WM_CREATE handler
  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_DESTROY handler
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_SIZE handler
  LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
  
  /// WM_ERASEBKGND handler
  LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_KEYDOWN handler
  LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_LBUTTONDOWN handler
  LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_LBUTTONUP handler
  LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_LBUTTONDBLCLK handler
  LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// WM_MOUSEMOVE handler
  LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

  /// loads the dataset
  void  Load( const std::string & sDirectory );

  /// starts the animation timer
  void StartTimer();

  /// stops the animation timer
  void StopTimer();
  
  /// study load callback procedure
  static int4 StudyLoadNotify( void * pData, const ReaderLib::StudyLoad::StepsEnum ePrevStep,
                               const ReaderLib::StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                               const ReaderLib::DatasetOrientations::TypeEnum eNewOrientation );

  /// animation timer procedure
  static void __stdcall TimerProc( UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2 );

// member variables
private:

  // dataset
  ReaderLib::Dataset * m_pDataset;

  // rendering context
  vxRenderingContextGL * m_pContext;

  // device associated with rendering context
  GLPaintDevice * m_pDevice;

  // rendering font
  GLFontGeorge * m_pFont;

  // animation timer id
  UINT m_uTimerId;

  // animation timer resolution
  UINT m_uTimerResolution;

  // animation timer period
  UINT m_uTimerPeriod;

  // current perent of load process
  float m_fPercent;

  /// auto fly enabled flag
  bool m_bAutoFlyEnabled;

  /// indicates if nag message ar enabled
  bool m_bNagMessageEnabled;

  /// initial dataset directory
  CComBSTR m_bsInitialDirectory;

  /// indicates if the timer is running
  bool m_bTimerRunning;

}; // class COMViewer

// $Log: COMViewer.h,v $
// Revision 1.1  2007/02/16 14:43:41  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/activex/COMViewer.h,v 1.1 2007/02/16 14:43:41 geconomos Exp $
// $Id: COMViewer.h,v 1.1 2007/02/16 14:43:41 geconomos Exp $
