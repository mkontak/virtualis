// $Id: v3D_CropBox.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_Volume.h"
#include "v3D_Enums.h"
#include "vxCropBox.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS 


// class definition
public ref class CropBox : public IElement, public IObserver
{
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pCropBox ); }

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

// functions
public:
  
  /// initializer
  CropBox();
  
  /// destructor
  virtual ~CropBox();

  /// finalize
  !CropBox();

  /// initializes the object
  void Initialize( Volume ^ mpVolume );

  /// initializes the object to the full extent of the volume
  void InitializeFull( Volume ^ mpVolume );

  /// gets the shared crop box object
  System::IntPtr GetCropBoxPtr() { return System::IntPtr( m_pCropBox ); }

// data
private:

  // crop box
  vxShareableObject< vxCropBox > * m_pCropBox;

  // observer 
  vxManagedObserver * m_pObserver;
}; // CropBox


CLOSE_VISUALIZATION_NS


// $Log: v3D_CropBox.h,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:07:41  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13.2.1  2004/02/24 20:06:20  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.13  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.12  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.11  2003/05/13 14:05:21  geconomos
// Fixed up after code walkthru.
//
// Revision 1.10  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.9  2003/05/06 15:32:10  geconomos
// More coding standards.
//
// Revision 1.8  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.7  2003/03/28 13:07:10  frank
// Added a function to initialize the crop box to the full extent of the volume
//
// Revision 1.6  2002/11/02 03:12:12  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.5  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.4  2002/09/24 16:08:22  geconomos
// Exception handling and formatting.
//
// Revision 1.3  2002/08/09 16:20:12  dmitry
// Added Connect and Disconnect methods to IElement.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CropBox.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_CropBox.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $