// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ApplicationController.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "ApplicationController.h"
#include "ApplicationSettings.h"
#include "MainAppFrame.h"
#include "ViatronViewWnd.h"
#include "vcUtils.h"


USING_COLON_NS


/** 
 * singleton
 */
ApplicationController & ApplicationController::GetInstance() 
{
  static ApplicationController instance; 
  return instance; 
} // GetInstance() 


/**
 * constructor
 */
ApplicationController::ApplicationController() : 
  m_bDelayNotification( false ),
  m_bSendDatasetNotification( false ),
  m_bSendReportNotification( false )
{
  MainAppFrame::GetMainFrame()->Connect( this );
} // ApplicationController()


/**
 * destructor
 */
ApplicationController::~ApplicationController()
{
  if( MainAppFrame::GetMainFrame() != nullptr )
    MainAppFrame::GetMainFrame()->Disconnect( this );
} // ~ApplicationController()


/**
 * Allows observers to register for notification
 *
 * @param         pObserver                 IObserver< DatasetsLockedArgs >
 */
void ApplicationController::Connect( IObserver< DatasetsLockedArgs > * pObserver )
{
  GetInstance().m_datasetObserver.Connect( pObserver );
} // Connect


/**
 * Allows observers to unregister from notification
 *
 * @param         pObserver                 IObserver< DatasetsLockedArgs >
 */
void ApplicationController::Disconnect( IObserver< DatasetsLockedArgs > * pObserver )
{
  GetInstance().m_datasetObserver.Disconnect( pObserver );
} // Disconnect


/**
 * Notifies any observers that the dataset locking state has been changed
 *
 *  @param lockArg
 */
void ApplicationController::Notify( const DatasetsLockedArgs & )
{
  ApplicationController & controller = GetInstance();
  if( controller.m_bDelayNotification )
    controller.m_bSendDatasetNotification = true;
  else
  {
    const bool bInitializing = controller.m_bSendDatasetNotification;
    controller.m_bSendDatasetNotification = false;

    ColonViews::Layouts::ColonViatronixWndPtr pWnd( dynamic_cast< ColonViews::Layouts::ColonViatronixWndPtr >( &MainAppFrame::GetMainFrame()->GetCurrentLayout() ));
    if( pWnd && pWnd->IsDatasetsLockingEnabled() )
      ApplicationSettings::UserSettings().SetBool( APP_SETTING_LINK_DATASETS, Utilities::Datasets::IsLocked() );
 
    controller.m_datasetObserver.Notify( DatasetsLockedArgs(( ( pWnd ) ? pWnd->IsDatasetsLockingEnabled() : false ), bInitializing ));
  }
} // Notify( const DatasetsLockedArgs & )


/**
 * Allows observers to register for notification
 *
 * @param         pObserver                 IObserver< ReportArgs >
 */
void ApplicationController::Connect( IObserver< ReportArgs > * pObserver )
{
  GetInstance().m_reportObserver.Connect( pObserver );
} // Connect(


/**
 * Allows observers to unregister from notification
 *
 * @param         pObserver                 IObserver< ReportArgs >
 */
void ApplicationController::Disconnect( IObserver< ReportArgs > * pObserver )
{
  GetInstance().m_reportObserver.Disconnect( pObserver );
} // Disconnect


/**
 * Notifies any observers that the report state has been changed
 *
 *  @param reportArg
 */
void ApplicationController::Notify( const ReportArgs & )
{
  ApplicationController & controller = GetInstance();
  if( controller.m_bDelayNotification )
    controller.m_bSendReportNotification = true;
  else
  {
    const bool bInitializing = controller.m_bSendReportNotification;
    controller.m_bSendReportNotification = false;
    controller.m_reportObserver.Notify( ReportArgs() );
  }
} // Notify( const ReportArgs & )


/**
 * Observers the state of the MainAppFrame
 *
 *  @param args
 */
void ApplicationController::OnNotified( const MainAppFrameStates & args )
{
  switch( args )
  {
    case MainAppFrameStates_SubViewChanging:
      m_bDelayNotification = true;
      break;

    case MainAppFrameStates_SubViewChanged:
      m_bDelayNotification = false;

      if( m_bSendDatasetNotification )
        Notify( DatasetsLockedArgs() );

      if( m_bSendReportNotification )
        Notify( ReportArgs() );
      break;
  }    
} // OnNotified( args )


/**
 * constructor
 *
 *  @param bEnabled
 *  @param bInitializing
 */
DatasetsLockedArgs::DatasetsLockedArgs( bool bEnabled, bool bInitializing ) :
  m_bEnabled( bEnabled ),
  m_bIsInitializing( bInitializing )
{
} // DatasetsLockedArgs


/**
 * Is the datasets locked
 */
bool DatasetsLockedArgs::IsLocked() const
{
  return Utilities::Datasets::IsLocked();
} // IsLocked()


/** 
 * constructor
 */
ReportArgs::ReportArgs()
{
} // ReportArgs()


/**
 * Is the report completed
 */
bool ReportArgs::IsCompleted() const
{
  return Utilities::Reporting::IsComplete();
} // IsCompleted()