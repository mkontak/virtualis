// $Id: StatusIndicator.h $
//
// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StatusIndicator.h  
//
// Owner(s): Dongqing Chen  dongqing@viatronix.net

#pragma once

#include "IToolTipProvider.h"
#include "ObservableObject.h"
#include "ApplicationController.h"

OPEN_WINUTILS_NS

// StatusIndicator: status indicator to be displayed at the bottom status bar of the application

class StatusIndicator : public CWnd, public IToolTipProvider, public IObserver< Colon::DatasetsLockedArgs >, public IObserver< Colon::ReportArgs >
{
  DECLARE_DYNAMIC(StatusIndicator)

public:
	/// default constructor
  StatusIndicator();

	/// destructor
  virtual ~StatusIndicator();

	/// get the tool tip
  std::string GetToolTip();

  /// observer of datasets locked changes
  void OnNotified( const Colon::DatasetsLockedArgs & args );

  /// observer of report state changes
  void OnNotified( const Colon::ReportArgs & args );

	/// set the link icon visibility
	void SetLinkIconVisibility( const bool & bShowLinked );


protected:
	/// declarition for message map
  DECLARE_MESSAGE_MAP()

  /// WM_CREATE
  int OnCreate( LPCREATESTRUCT lpCreateStruct );

  /// WM_DESTROY
  void OnDestroy();

  /// WM_PAINT handler
  void OnPaint();

  /// WM_ERASEBKGND handler
  BOOL OnEraseBkgnd( CDC * pdc );

  /// WM_SIZE handler
  void OnSize( UINT uType, int cx, int cy );

  /// Left button down handler.
  void OnLButtonDown(UINT uFlags, CPoint point);


private:

  /// redraws the control into the draw buffer
  void RedrawControl();

private:

  /// linked dataset state indicator
  Gdiplus::Bitmap m_linkedImage;

  /// report completed state indicator
  Gdiplus::Bitmap m_reportCompletedImage;

  /// bool flag to track the links visibility
  bool m_bShowLinked;

};


CLOSE_WINUTILS_NS