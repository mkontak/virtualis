// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FullScreenController.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ObservableObject.h"
#include "ReaderGlobal.h"
#include "MainAppFrameStates.h"
#include "SliceChildView.h"
#include "GLWnd.h"


class MainAppFrame;


OPEN_COLON_NS

class FullScreenArgs;


/**
 * Full screen controller
 */
class FullScreenController : public ObservableObject< FullScreenArgs >, public IObserver< MainAppFrameStates >
{
public:

  /// singleton
  static FullScreenController & GetInstance();

private:

  /// constructor
  FullScreenController();

  /// destructor
  virtual ~FullScreenController();

public:

  /// notifies any observer's of a state change
  void Notify( const FullScreenArgs & args );

  /// updates the SliceChildViewState for the full screen viewer
  void UpdateFullScreenViewerState( ColonViews::Layouts::GLWnd & view );

  /// stores the SliceChildViewState for the specified layout
  void PersistLayout( ColonViews::Layouts::GLWnd & view );

  /// sets up the specified layout with the stored SliceChildViewState
  void SetupLayout( ColonViews::Layouts::GLWnd & view );

  /// returns the SliceChildViewState for the full screen viewer
  bool GetFullScreenState( ColonViews::ChildViews::SliceChildView::SliceChildViewState & state );

private:

  /// notification handler
  void OnNotified( const MainAppFrameStates & args );

private:

  /// delay notification
  bool m_bDelayNotification;

  /// fullsreen args
  std::shared_ptr< FullScreenArgs > m_pArgs;

  /// view states
  typedef std::vector< ColonViews::ChildViews::SliceChildView::SliceChildViewState > StateVector;
  StateVector m_viewStates;

};


class FullScreenArgs
{
public:

  /// constructor
  FullScreenArgs( bool bFullscreen );

  /// is fullscreen
  bool IsFullScreen() const { return m_bIsFullscreen; }

private:

  /// is fullscreen
  bool m_bIsFullscreen;

};

CLOSE_COLON_NS
