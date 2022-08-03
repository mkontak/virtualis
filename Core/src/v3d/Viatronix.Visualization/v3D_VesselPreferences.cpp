// $Id: v3D_VesselPreferences.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_VesselPreferences.h"
#include "vxManagedObserver.h"
#include "vxIDs.h"


// namespaces
USING_VISUALIZATION_NS


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 */
VesselPreferences::VesselPreferences()
{
  m_pObserver = new vxManagedObserver(this);
  m_mpVesselPreferences = new vxShareableObject< vxVesselPreferences >();
  m_mpVesselPreferences->Connect( m_pObserver );
} // VesselPreferences()


/**
 * Destructor
 */
VesselPreferences::~VesselPreferences()
{
  this->!VesselPreferences();
} // ~VesselPreferences()


/**
 * Destructor
 */
VesselPreferences::!VesselPreferences()
{
  delete m_mpVesselPreferences;
  m_mpVesselPreferences = nullptr;
} // !VesselPreferences()


/** 
 * Connects an observer to the object.
 */
void VesselPreferences::Connect( System::IntPtr observer )
{
  m_mpVesselPreferences->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 */
void VesselPreferences::Disconnect( System::IntPtr observer )
{
  m_mpVesselPreferences->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Public method to reset vxVesselPreferences.
 */
void VesselPreferences::Reset()
{
  m_mpVesselPreferences->Reset();
} // Reset()


/**
 * Implements the IObserver interface.
 *
 * @param modifiedInfoList the modfied information
 */
void VesselPreferences::OnModified( System::IntPtr modifiedInfoList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedInfoList.ToPointer() );

  for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
  {  
    if( pInfo->GetID() == vxIDs::VesselPreferences )
    {
      const uint4 uFlags = pInfo->GetFlag();
      if ( uFlags & vxVesselPreferences::EVENT_MODIFIED )
        Changed( this, nullptr );
    }
  }
} // OnModified()


/**
 * set the display of vessel outline
 *
 * @param bDisplayOutline whether or not to display the outline
 */
void VesselPreferences::DisplayOutline::set( bool bDisplayOutline )
{
  m_mpVesselPreferences->SetDisplayOutline( bDisplayOutline );
  m_mpVesselPreferences->Modified( vxModInfoID( vxElement, vxIDs::VesselPreferences, vxVesselPreferences::EVENT_MODIFIED ) );
} // set_DisplayOutline()


/**
 * get the display of vessel outline
 *
 * @return whether or not to display the outline
 */
bool VesselPreferences::DisplayOutline::get()
{
  return m_mpVesselPreferences->GetDisplayOutline();
} // get_DisplayOutline()


/**
 * set the display of vessel centerline
 *
 * @param bDisplayCenterline whether or not to display the centerline
 */
void VesselPreferences::DisplayCenterline::set( bool bDisplayCenterline )
{
  m_mpVesselPreferences->SetDisplayCenterline( bDisplayCenterline );
  m_mpVesselPreferences->Modified( vxModInfoID( vxElement, vxIDs::VesselPreferences, vxVesselPreferences::EVENT_MODIFIED ) );
} // set_DisplayCenterline()


/**
 * get the display of vessel centerline
 *
 * @return whether or not to display the centerline
 */
bool VesselPreferences::DisplayCenterline::get()
{
  return m_mpVesselPreferences->GetDisplayCenterline();
} // get_DisplayCenterline()


/**
 * set the display of vessel graphs
 *
 * @param bDisplayGraphs whether or not to display the vessel graphs
 */
void VesselPreferences::DisplayGraphs::set( bool bDisplayGraphs )
{
  m_mpVesselPreferences->SetDisplayGraphs( bDisplayGraphs );
  m_mpVesselPreferences->Modified( vxModInfoID( vxElement, vxIDs::VesselPreferences, vxVesselPreferences::EVENT_MODIFIED ) );
} // set_DisplayGraphs()


/**
 * get the display of vessel graphs
 *
 * @return whether or not to display the vessel graphs
 */
bool VesselPreferences::DisplayGraphs::get()
{
  return m_mpVesselPreferences->GetDisplayGraphs();
} // get_DisplayGraphs()


/**
 * set the rotating curved MPR mode
 *
 * @param bRotatingCurvedMPR the new rotating curved MPR mode
 */
void VesselPreferences::RotatingCurvedMPR::set( bool bRotatingCurvedMPR )
{
  vxVesselPreferences::SetRotatingCurvedMPR( bRotatingCurvedMPR );
} // set_RotatingCurvedMPR()


/**
 * get the rotating curved MPR mode
 *
 * @return the rotating curved MPR mode
 */
bool VesselPreferences::RotatingCurvedMPR::get()
{
  return vxVesselPreferences::GetRotatingCurvedMPR();
} // get_RotatingCurvedMPR()


// undefines
#undef FILE_REVISION


// $Log: v3D_VesselPreferences.cpp,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/09/27 17:09:54  geconomos
// changed file extension to .cpp
//
// Revision 1.1  2006/08/17 14:07:06  gdavidson
// Initial Revision
//
// Revision 1.2  2005/06/08 14:57:03  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/03/02 01:10:34  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.6.2.2  2004/02/25 14:56:48  frank
// formatting
//
// Revision 1.6.2.1  2004/02/24 18:34:12  frank
// set deleted pointers to null
//
// Revision 1.6  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.5  2003/10/29 20:54:01  frank
// Added rotating curved MPR preference and disablement
//
// Revision 1.4  2003/08/08 20:12:36  wenli
// add the calciumcleansed in the preference
//
// Revision 1.3  2003/05/14 19:29:12  frank
// Added calcification threshold
//
// Revision 1.2  2003/05/13 12:31:35  frank
// updated the namespace declarations for the official project name
//
// Revision 1.1  2003/05/08 19:59:24  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.7  2003/05/02 13:02:47  geconomos
// Code walkthrough.
//
// Revision 1.6  2003/05/01 16:28:20  frank
// code review
//
// Revision 1.5  2003/04/22 15:24:26  frank
// Code review
//
// Revision 1.4  2003/01/28 23:20:33  frank
// Added a preference for displaying the graph.
//
// Revision 1.3  2002/11/12 16:59:18  frank
// Disabled serialization
//
// Revision 1.2  2002/10/31 21:34:04  frank
// added properties
//
// Revision 1.1  2002/10/31 21:29:52  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VesselPreferences.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_VesselPreferences.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
