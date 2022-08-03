// $Id: v3D_EnvironmentList.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_EnvironmentList.h"
#include "vxManagedObserver.h"
#include "v3D_Environment.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a Contour list object with default values
 */
EnvironmentList::EnvironmentList() : 
  m_pEnvironmentList( new vxShareableObject< vxEnvironmentList >() )
{
  m_pObserver = new vxManagedObserver( this );
  m_pEnvironmentList->Connect( m_pObserver );
} // EnvironmentList()


/**
 * destructor
 */
EnvironmentList::~EnvironmentList()
{
  this->!EnvironmentList();
} // ~EnvironmentList()


/**
 * Finalizer
 */
EnvironmentList::!EnvironmentList()
{
  delete m_pEnvironmentList; 
  m_pEnvironmentList = NULL;

  delete m_pObserver; 
  m_pObserver = NULL;
} // !EnvironmentList()


/**
 * Adds an environment to the list
 *
 * @param   mpEnvironment     Environment object being added
 */
void EnvironmentList::Add( Environment ^ mpEnvironment )
{
  m_pEnvironmentList->GetEnvironmentList().push_back( reinterpret_cast< vxEnvironment * >( mpEnvironment->GetEnvironmentPtr().ToPointer() ));
} // Add( mpEnvironment )


/**
 * Removes an environment from the list
 *
 * @param   mpEnvironment     Environment object being removed
 */
void EnvironmentList::Remove( Environment ^ mpEnvironment )
{
  m_pEnvironmentList->GetEnvironmentList().remove( reinterpret_cast< vxEnvironment * >( mpEnvironment->GetEnvironmentPtr().ToPointer() ));
} // Remove( mpEnvironment )


/**
 * Clears the environments from the list
 */
void EnvironmentList::Clear()
{
  m_pEnvironmentList->GetEnvironmentList().clear();
} // Clears()


/** 
* Connects an observer to the object
* @param observer vxManagedObserver object interested in events
*/
void EnvironmentList::Connect( System::IntPtr observer )
{
  m_pEnvironmentList->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer to the object
 * @param observer vxManagedObserver object interested in events
 */
void EnvironmentList::Disconnect( System::IntPtr  observer )
{
  m_pEnvironmentList->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events
 * @param modifiedList Modified info list
 */
void EnvironmentList::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// $Log: v3D_EnvironmentList.cpp,v $
// Revision 1.3  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/09/27 17:09:54  geconomos
// changed file extension to .cpp
//
// Revision 1.1  2006/03/27 19:41:49  gdavidson
// new build
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_EnvironmentList.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_EnvironmentList.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
