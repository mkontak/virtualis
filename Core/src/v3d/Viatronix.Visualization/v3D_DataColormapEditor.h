// $Id: v3D_DataColormapEditor.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_Enums.h"
#include "vxDataColormapEditor.h"


// namespaces
OPEN_VISUALIZATION_NS 


// class definition
public ref class DataColormapEditor : 
  public IElement,
  public System::ICloneable
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pDataColormapEditor ); }

  /// connects the specified observer
  virtual void Connect( System::IntPtr observer );

  /// disconnects the specified observer
  virtual void Disconnect( System::IntPtr observer );
  
  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// ICloneable implementation
public:

  /// creates a new object that is a copy of the current instance.
  virtual System::Object ^ Clone();

// functions
public:
  
  /// initializer
  DataColormapEditor();
  
  /// destructor
  virtual ~DataColormapEditor();

  /// finalizer
  !DataColormapEditor();

// data
private:

  // colormap editor data
  vxShareableObject< vxDataColormapEditor > * m_pDataColormapEditor;

}; // DataColormapEditor


CLOSE_VISUALIZATION_NS


// $Log: v3D_DataColormapEditor.h,v $
// Revision 1.2  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2005/05/23 20:25:21  michael
// Introduced concept of data objects to start with v3D_Data....
//
// Revision 1.2  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.6  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.5  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.4  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.3  2003/05/06 15:32:10  geconomos
// More coding standards.
//
// Revision 1.2  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.1  2002/10/04 22:59:19  michael
// initial version (after renaming)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_DataColormapEditor.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_DataColormapEditor.h,v 1.2 2006/10/04 18:29:04 gdavidson Exp $