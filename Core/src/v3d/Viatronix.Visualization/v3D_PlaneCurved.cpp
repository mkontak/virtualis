// $Id: v3D_PlaneCurved.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "v3D_PlaneCurved.h"
#include "vxManagedObserver.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes the PlaneCurved object with default values.
 */
PlaneCurved::PlaneCurved()
{
  Initialize();
} // constructor


/**
 * Initializes the object.
 */
void PlaneCurved::Initialize()
{
  m_pPlaneCurved = new vxShareableObject< vxPlaneCurved >();
  m_pObserver = new vxManagedObserver( this );

  m_pPlaneCurved->Connect( m_pObserver );
} // Initialize()


/**
 * destructor
 */
PlaneCurved::~PlaneCurved()
{
  this->!PlaneCurved();
} // ~PlaneCurved()


/**
 * Finalizer
 */
PlaneCurved::!PlaneCurved()
{
  m_pPlaneCurved->Disconnect(m_pObserver);

  delete m_pPlaneCurved; m_pPlaneCurved = NULL;
  delete m_pObserver;   m_pObserver = NULL;
} // !PlaneCurved()


/** 
 * Connects an observer to the object.
 * @param observer vxManagedObserver interested in events.
 */
void PlaneCurved::Connect( System::IntPtr observer )
{
  m_pPlaneCurved->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void PlaneCurved::Disconnect( System::IntPtr  observer )
{
  m_pPlaneCurved->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void PlaneCurved::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// Revision History:
// $Log: v3D_PlaneCurved.cpp,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/24 20:10:27  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.5  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.4  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.3  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.2  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.1  2002/11/18 15:54:15  michael
// added renamed version fitting to vxBase objet's name
//
// Revision 1.2  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.1  2002/10/21 22:19:55  frank
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneCurved.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_PlaneCurved.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
