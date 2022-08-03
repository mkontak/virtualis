// $Id: v3D_DataPlaneObliqueCreate.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
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
#include "vxDataPlaneObliqueCreate.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class DataPlaneObliqueCreate : public IObserver, public IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr(m_pDataPlaneObliqueCreate); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

/// functions
public:

  /// initializer
  DataPlaneObliqueCreate();

  /// destructor
  virtual ~DataPlaneObliqueCreate();

  /// finalizer
  !DataPlaneObliqueCreate();

// data
private:

  vxShareableObject<vxDataPlaneObliqueCreate> * m_pDataPlaneObliqueCreate;
}; // Triplet


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_DataPlaneObliqueCreate.h,v $
// Revision 1.3  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2005/05/23 20:25:22  michael
// Introduced concept of data objects to start with v3D_Data....
//
// Revision 1.2  2004/10/04 15:10:45  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.1  2003/08/20 14:41:02  michael
// added managed plane oblique create data class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_DataPlaneObliqueCreate.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_DataPlaneObliqueCreate.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $