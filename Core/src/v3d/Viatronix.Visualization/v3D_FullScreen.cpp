// $Id: v3D_FullScreen.cpp,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_FullScreen.h"
#include "vxManagedObserver.h"
#include "v3D_Environment.h"
#include "v3D_Viewer.h"
#include "v3D_VisualizationGraph.h"


// defines
#define FILE_REVISION "$Revision: 1.1.2.2 $"


// namespaces
USING_VISUALIZATION_NS
USING_GRAPH_NS


/**
 * Initializes a FullScreen object with default values.
 */
FullScreen::FullScreen() :
  m_pFullScreen( new vxShareableObject< vxFullScreen >() )
{
  CommonInit();
} // FullScreen()


/**
 * Initializes a FullScreen object with default values.
 *
 * @param mpOther  The object to initialize from
 */
FullScreen::FullScreen( FullScreen ^ mpOther ) :
  m_pFullScreen( new vxShareableObject< vxFullScreen >() )
{
  CommonInit();
} // FullScreen( mpOther )


/**
 * Destructor
 */
FullScreen::~FullScreen()
{
  this->!FullScreen();
} // ~FullScreen()


/**
 * Finalizer
 */
FullScreen::!FullScreen()
{
  if( m_pFullScreen != NULL )
  {
    m_pFullScreen->Disconnect( m_pObserver );
  }
  
  delete m_pFullScreen;
  m_pFullScreen = NULL;

  delete m_pObserver;
  m_pObserver = NULL;
} // !FullScreen()


/**
 * Common initialization routine.
 */
void FullScreen::CommonInit()
{
  // unmanaged types 
  m_pObserver = new vxManagedObserver( this );
  m_pFullScreen->Connect( m_pObserver );
} // CommonInit()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void FullScreen::Connect( System::IntPtr observer )
{
  m_pFullScreen->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void FullScreen::Disconnect( System::IntPtr  observer )
{
  m_pFullScreen->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()



/** 
 * Receives modified information on connected objects. 
 *
 * @param modifiedList Modified info list.
 */
void FullScreen::OnModified( System::IntPtr modifiedList )
{
  LogDbg( "FullScreen Modified", "FullScreen", "OnModified" );
  Changed( this, System::EventArgs::Empty );
} // OnModified()


/**
 * Returns the window value.
 *
 * @return Window value.
 */
Viewer ^ FullScreen::Viewer::get()
{
  for each( Viatronix::Visualization::Viewer ^ mpViewer in VisualizationGraph::Instance->Viewers )
  {
    if( ToNativeEnvironmentPtr( mpViewer->Environment->GetEnvironmentPtr() ) == m_pFullScreen->GetEnvironment() )
    {
      return mpViewer;
    }
  }

  return nullptr;
} // Viewer


// $Log: v3D_FullScreen.cpp,v $
// Revision 1.1.2.2  2009/07/29 16:12:58  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2008/11/24 17:11:03  gdavidson
// Issue #6171: Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_FullScreen.cpp,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $
// $Id: v3D_FullScreen.cpp,v 1.1.2.2 2009/07/29 16:12:58 kchalupa Exp $
