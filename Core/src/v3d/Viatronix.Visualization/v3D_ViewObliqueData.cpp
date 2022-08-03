// $Id: v3D_ViewObliqueData.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.ncom)


// Includes
#include "StdAfx.h"
#include "v3D_ViewObliqueData.h"
#include "vxManagedObserver.h"


// Namespaces
USING_VISUALIZATION_NS


/**
 * Default constructor
 */
ViewObliqueData::ViewObliqueData()
{
  m_pViewObliqueData = new vxShareableObject< vxViewObliqueData >();

  m_pObserver = new vxManagedObserver( this );
  m_pViewObliqueData->Connect( m_pObserver );
} // ViewObliqueData()


/**
 * Finalizer
 */
ViewObliqueData::~ViewObliqueData()
{
  this->!ViewObliqueData();
} // finalizer


/**
 * Finalizer
 */
ViewObliqueData::!ViewObliqueData()
{
  m_pViewObliqueData->Disconnect( m_pObserver );

  delete m_pViewObliqueData; m_pViewObliqueData = NULL;
  delete m_pObserver;        m_pObserver = NULL;
} // finalizer


/** 
 * Connects an observer to the object.
 *
 * @param observer the observer
 */
void ViewObliqueData::Connect( System::IntPtr observer )
{
  m_pViewObliqueData->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer the observer
 */
void ViewObliqueData::Disconnect( System::IntPtr observer )
{
  m_pViewObliqueData->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Process the modified info list
 *
 * @param modifiedList the modified info list
 */
void ViewObliqueData::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// $Log: v3D_ViewObliqueData.cpp,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/24 20:13:24  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.3  2003/12/16 13:27:52  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/05/13 17:37:40  frank
// code review
//
// Revision 1.1  2002/11/13 22:08:11  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.1  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewObliqueData.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_ViewObliqueData.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
