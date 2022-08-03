// $Id: v3D_DataColormapEditor.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3d_DataColormapEditor.h"
#include "vxManagedObserver.h"


// namespaces
USING_VISUALIZATION_NS


/** 
 * Constructor
 */
DataColormapEditor::DataColormapEditor()
{
  m_pDataColormapEditor = new vxShareableObject< vxDataColormapEditor >();
} // DataColormapEditor()


/** 
 * Finalize
 */
DataColormapEditor::~DataColormapEditor()
{
  this->!DataColormapEditor();
} // ~DataColormapEditor()


/** 
 * Finalize
 */
DataColormapEditor::!DataColormapEditor()
{
  delete m_pDataColormapEditor;
  m_pDataColormapEditor = NULL;
} // !DataColormapEditor()


/** 
 * Connects an observer to the object
 * @param observer vxManagedObserver object interested in events.
 */
void DataColormapEditor::Connect( System::IntPtr observer )
{
  m_pDataColormapEditor->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ DataColormapEditor::Clone()
{
  // create a new instance of the annotations class
  DataColormapEditor ^ mpDataColormapEditor = gcnew DataColormapEditor();

  // get the underlying implementation pointer  
  vxDataColormapEditor * pData = dynamic_cast< vxDataColormapEditor *>( mpDataColormapEditor->m_pDataColormapEditor );

  // make a copy of the underying implementations
  *pData = *dynamic_cast< vxDataColormapEditor *>( m_pDataColormapEditor );  

  return mpDataColormapEditor;
} // Clone()


/** 
 * Disconnects an observer from the object
 * @param observer vxManagedObserver object interested in events.
 */
void DataColormapEditor::Disconnect( System::IntPtr  observer )
{
  m_pDataColormapEditor->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()



// Revision History:
// $Log: v3D_DataColormapEditor.cpp,v $
// Revision 1.3  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2005/05/23 20:25:21  michael
// Introduced concept of data objects to start with v3D_Data....
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/24 20:04:45  michael
// coding standard: assign NULL after deletion
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_DataColormapEditor.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_DataColormapEditor.cpp,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
