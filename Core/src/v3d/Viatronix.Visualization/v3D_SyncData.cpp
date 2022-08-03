// $Id: v3D_SyncData.cpp,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Cheng Qian ( cqian@viatronix.net )


// includes
#include "Stdafx.h"
#include "vxSyncData.h"
#include "v3D_Interfaces.h"
#include "vxManagedObserver.h"
#include "v3D_Syncdata.h"
#include "v3D_Viewer.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Constructor
 *
 * @param elementValue the initial value
 */
SyncData::SyncData()
{
  CommonInit();
} // SyncData()


/**
 * destructor
 */
SyncData::~SyncData()
{
  this->!SyncData();
} // ~SyncData()


/**
 * Finalize
 */
SyncData::!SyncData()
{
  m_pSyncData->Disconnect( m_pObserver );

  delete m_pSyncData;      m_pSyncData = NULL;
  delete m_pObserver; m_pObserver = NULL;
} // !SyncData()


/**
 * Returns a pointer to the extended element
 *
 * @return pointer to element
 */
System::IntPtr SyncData::GetElementPtr()
{
  return System::IntPtr( m_pSyncData );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the overver
 */
void SyncData::Connect( System::IntPtr observer )
{
  m_pSyncData->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void SyncData::Disconnect( System::IntPtr observer )
{
  m_pSyncData->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param ptrInfo the modified list
 */
void SyncData::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void SyncData::CommonInit()
{
  m_pSyncData = new vxShareableObject< vxSyncData >();
  m_pSyncData->Disable();
  m_pObserver = new vxManagedObserver( this );
  m_pSyncData->Connect( m_pObserver );
} // CommonInit()



/**
 * add a viewer 
 *
 * @param mpViewer  pointer to the viewer
 */
void SyncData::AddEnvironment( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  m_pSyncData->AddEnvironment( ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() ) ); 
} // AddViewer( mpEnvironment )


/**
 * add a viewer 
 *
 * @param mpViewer  pointer to the viewer
 */
void SyncData::RemoveEnvironment ( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  m_pSyncData->RemoveEnvironment( ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() ) ); 
}// RemoveViewer( mpEnvironment )


/**
 * Returns if the synchronization is enabled.
 *
 * @returns If the synchronization is enabled.
 */
bool SyncData::Enabled::get()
{
  return m_pSyncData->IsEnabled();
} // get_Enabled()


/**
 * Sets whether or not the synchronization is enabled.
 *
 * @param bEnabled  The state to change to.
 */
void SyncData::Enabled::set( bool bEnabled )
{
  m_pSyncData->SetEnabled( bEnabled );
} // set_Enabled( bEnabled )


#pragma region revision history

// $Log: v3D_SyncData.cpp,v $
// Revision 1.1.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/02/19 14:43:22  kchalupa
// Coding Standards
//
// Revision 1.1  2008/05/02 05:01:50  cqian
// check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SyncData.cpp,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_SyncData.cpp,v 1.1.2.2 2009/07/29 19:54:07 kchalupa Exp $

#pragma endregion
