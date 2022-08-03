// $Id: v3D_DataRendererSoftware3d.h,v 1.4 2004/10/18 14:50:43 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxDataRendererSoftware3d.h"


// forward declarations
class Observer;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public __gc class DataRendererSoftware3d : public IObserver, public IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pDataRendererSoftware3d ); }

  /// connects an observer to the object
  void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  __event System::EventHandler * Modified;

// IObserver implementation
public:
  
  /// processes modified queue events
  void OnModified( System::IntPtr modifiedList );

/// functions
public:

  /// initializer
  DataRendererSoftware3d();

  /// finalizer
  ~DataRendererSoftware3d();

// data
private:

  vxShareableObject<vxDataRendererSoftware3d> * m_pDataRendererSoftware3d;

}; // DataRendererSoftware3d


CLOSE_VISUALIZATION_NS


// $Log: v3D_DataRendererSoftware3d.h,v $
// Revision 1.4  2004/10/18 14:50:43  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.3  2004/10/05 14:41:44  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/10/04 15:08:04  frank
// improved the extended elements modified interface
//
// Revision 1.1  2004/08/30 17:16:44  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.3  2004/05/13 19:18:26  frank
// formatting
//
// Revision 1.2  2004/05/12 19:27:08  frank
// improved quality for off-line rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.4.2.1  2004/02/17 16:16:41  geconomos
// Added DisplayShading property.
//
// Revision 1.4  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.3  2003/11/16 23:39:02  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.2  2003/10/08 19:32:50  frank
// Changed the property to a property syntax
//
// Revision 1.1  2003/09/29 19:26:04  michael
// added managed wrapper RendererCpu3dData
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_DataRendererSoftware3d.h,v 1.4 2004/10/18 14:50:43 frank Exp $
// $Id: v3D_DataRendererSoftware3d.h,v 1.4 2004/10/18 14:50:43 frank Exp $
