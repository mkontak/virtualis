// $Id: v3D_DataRendererSoftware3d.cpp,v 1.2 2004/10/05 14:39:50 frank Exp $
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
#include "v3D_DataRendererSoftware3d.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a RendererCpu3dData object with default values.
 */
DataRendererSoftware3d::DataRendererSoftware3d()
{
  m_pDataRendererSoftware3d = new vxShareableObject<vxDataRendererSoftware3d>();
}


/**
 * Finalizer
 */
DataRendererSoftware3d::~DataRendererSoftware3d()
{
  delete m_pDataRendererSoftware3d;
  m_pDataRendererSoftware3d = NULL;
} // ~DataRendererSoftware3d()


/** 
 * Connects an observer from the object.
 * @param observer Observer interested in events.
 */
void DataRendererSoftware3d::Connect( System::IntPtr observer )
{
  m_pDataRendererSoftware3d->Connect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer Observer interested in events.
 */
void DataRendererSoftware3d::Disconnect( System::IntPtr  observer )
{
  m_pDataRendererSoftware3d->Disconnect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void DataRendererSoftware3d::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// $Log: v3D_DataRendererSoftware3d.cpp,v $
// Revision 1.2  2004/10/05 14:39:50  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.1  2004/08/30 17:16:44  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.2  2004/05/12 19:27:08  frank
// improved quality for off-line rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.2  2004/02/24 20:11:39  michael
// took out code that was commented out
//
// Revision 1.3.2.1  2004/02/17 16:16:42  geconomos
// Added DisplayShading property.
//
// Revision 1.3  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/10/08 19:32:50  frank
// Changed the property to a property syntax
//
// Revision 1.1  2003/09/29 19:26:04  michael
// added managed wrapper RendererCpu3dData
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_DataRendererSoftware3d.cpp,v 1.2 2004/10/05 14:39:50 frank Exp $
// $Id: v3D_DataRendererSoftware3d.cpp,v 1.2 2004/10/05 14:39:50 frank Exp $
