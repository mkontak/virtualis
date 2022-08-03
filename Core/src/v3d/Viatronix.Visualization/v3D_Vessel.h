// $Id: v3D_Vessel.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright  2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wenli Cai (wenli@viatronix.com)


// Pragmas
#pragma once


// Includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxVessel.h"


// Forwards
class vxManagedObserver;


// Namespaces
OPEN_VISUALIZATION_NS


/**
 * Interface between vxVessel and v3D Explorer. 
 */
public ref class Vessel : public IElement
{

// Serialization
public:

  /// serializes the vessel instance to xml
  System::String ^ ToXml();

  /// creates a new vessel instance from the specified xml string
  static Vessel ^ FromXml( System::String ^ mpXml );

// IElement implementation
public:

  /// gets a pointer to the IElement
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pVessel ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// implements the IObserver interface
  void OnModified( System::IntPtr ptrInfo );

public:
  
  /// default constructor
  Vessel();

  /// destructor
  virtual ~Vessel();

  /// finalizer
  !Vessel();

  /// import from a simple text file ( <numPoints> \n x y z \n x y z \n ... )
  static Vessel ^ FromTextFile( System::String ^ mpFilename );

// properties
public:

  /// get the vessel label
  property long Label { long get(); }

// data members
private:

  /// the vessel
  vxShareableObject< vxVessel > * m_pVessel;

}; // class Vessel

CLOSE_VISUALIZATION_NS

// $Log: v3D_Vessel.h,v $
// Revision 1.7  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/07/18 13:51:02  frank
// added importing from a text file
//
// Revision 1.4  2005/06/07 19:01:20  vxconfig
// removed the get/set name property of vessels
//
// Revision 1.3  2004/10/04 15:11:33  frank
// improved the extended elements modified interface
//
// Revision 1.2  2004/09/23 15:32:25  frank
// implemented to/from xml
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.3  2003/08/22 19:22:02  frank
// Added IObserver implementation
//
// Revision 1.2  2003/08/13 11:43:03  frank
// Modernization
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.2  2003/05/13 12:31:51  frank
// updated the namespace declarations for the official project name
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/04/22 15:24:27  frank
// Code review
//
// Revision 1.4  2003/01/14 16:17:54  dongqing
// Code review
//
// Revision 1.3  2002/11/21 02:43:09  frank
// Added a property to get the label
//
// Revision 1.2  2002/11/04 20:07:22  frank
// Added get and set Name properties
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.5  2002/10/25 01:51:48  frank
// Moved working vessel into the VesselArray
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Vessel.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Vessel.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
