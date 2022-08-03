// $Id: v3D_CycleSettings.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "v3D_CycleSettings.h"
#include "vxCycleSettings.h"
#include "vxManagedObserver.h"


USING_VISUALIZATION_NS


/**
 * default constructor
 */
CycleSettings::CycleSettings() :
  m_pCycleSettings( new vxShareableObject<vxCycleSettings>() )
{
  m_pObserver = new vxManagedObserver( this );
  m_pCycleSettings->Connect( m_pObserver );
} // CycleSettings()


/**
 * destructor
 */
CycleSettings::~CycleSettings()
{
  this->!CycleSettings();
} // ~CycleSettings()


/**
 * finalizer
 */
CycleSettings::!CycleSettings()
{
  if( m_pCycleSettings != NULL )
  {
    m_pCycleSettings->Disconnect( m_pObserver );
    delete m_pCycleSettings;
    m_pCycleSettings = NULL;
  }

  if( m_pObserver != NULL )
  {
    delete m_pObserver;
    m_pObserver = NULL;
  }
} // !CycleSettings()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CycleSettings::Connect( System::IntPtr observer )
{
  m_pCycleSettings->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CycleSettings::Disconnect( System::IntPtr  observer )
{
  m_pCycleSettings->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/** 
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void CycleSettings::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// $Log: v3D_CycleSettings.cpp,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CycleSettings.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_CycleSettings.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $

