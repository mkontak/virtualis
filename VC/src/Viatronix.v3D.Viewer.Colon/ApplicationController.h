// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ApplicationController.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ObservableObject.h"
#include "ReaderGlobal.h"
#include "MainAppFrameStates.h"
#include "FullScreenController.h"

// forward declarations
class MainAppFrame;


OPEN_COLON_NS

class DatasetsLockedArgs;
class ReportArgs;


/**
 * Implements an application controller that inherits the Observer functionality to watch the Main Application
 * Frames states
 */
class ApplicationController : public IObserver< MainAppFrameStates >
{
public:

  /// allows observers to register for notification
  static void Connect( IObserver< DatasetsLockedArgs > * pObserver );

  /// allows observers to unregister from notification
  static void Disconnect( IObserver< DatasetsLockedArgs > * pObserver );

  /// notify all observers of dataset locking
  static void Notify( const DatasetsLockedArgs & );

public:

  /// allows observers to register for notification
  static void Connect( IObserver< ReportArgs > * pObserver );

  /// allows observers to unregister from notification
  static void Disconnect( IObserver< ReportArgs > * pObserver );

  /// notify all observers of dataset locking
  static void Notify( const ReportArgs & );

private:

  /// singleton
  static ApplicationController & GetInstance();

private:

  /// constructor
  ApplicationController();

  /// destructor
  virtual ~ApplicationController();

private:

  /// notification handler
  void OnNotified( const MainAppFrameStates & args );

private:

  /// delay notification
  bool m_bDelayNotification;

  /// should a notification be sent
  bool m_bSendDatasetNotification;

  /// dataset observer
  ObservableObject< DatasetsLockedArgs > m_datasetObserver; 

  /// should a notification be sent
  bool m_bSendReportNotification;

  /// report observer
  ObservableObject< ReportArgs > m_reportObserver;

};


class DatasetsLockedArgs
{
public:

  /// constructor
  DatasetsLockedArgs( bool bEnabled = true, bool bInitializing = false );

  /// returns the lock status 
  bool IsLocked() const;

  /// returns if dataset locking is enabled
  inline bool IsEnabled() const { return m_bEnabled; }

  /// tracks the state of the main app frame
  inline bool IsMainAppFrameChanging() const { return m_bIsInitializing; }


private:

  /// is dataset locking enabled
  bool m_bEnabled;

  /// is the main app frame changing 
  bool m_bIsInitializing;

};


class ReportArgs
{
public:

  /// constructor
  ReportArgs();

  /// returns the completed status
  bool IsCompleted() const;

};


CLOSE_COLON_NS
