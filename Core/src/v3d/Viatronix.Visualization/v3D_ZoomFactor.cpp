// $Id: v3D_ZoomFactor.cpp,v 1.3 2006/10/04 18:29:07 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_ZoomFactor.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a ZoomFactor object with default values.
 */
ZoomFactor::ZoomFactor()
{
  m_pZoomFactor = new vxShareableObject<vxZoomFactor>();
}


/**
 * destructor
 */
ZoomFactor::~ZoomFactor()
{
  this->!ZoomFactor();
} // ~ZoomFactor()


/**
 * Finalizer
 */
ZoomFactor::!ZoomFactor()
{
  delete m_pZoomFactor;
  m_pZoomFactor = NULL;
} // !ZoomFactor()


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void ZoomFactor::Connect( System::IntPtr observer )
{
  m_pZoomFactor->Connect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void ZoomFactor::Disconnect( System::IntPtr  observer )
{
  m_pZoomFactor->Disconnect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void ZoomFactor::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// Revision History:
// $Log: v3D_ZoomFactor.cpp,v $
// Revision 1.3  2006/10/04 18:29:07  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2004/04/26 19:28:08  michael
// Added managed zoom factor class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ZoomFactor.cpp,v 1.3 2006/10/04 18:29:07 gdavidson Exp $
// $Id: v3D_ZoomFactor.cpp,v 1.3 2006/10/04 18:29:07 gdavidson Exp $
