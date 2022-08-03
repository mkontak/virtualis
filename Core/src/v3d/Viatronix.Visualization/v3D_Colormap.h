// $Id: v3D_Colormap.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $
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
#include "v3D_WindowLevel.h"
#include "vxColormap.h"
#include "vxShareableObject.h"
#include "ColorList.h"


// forward declarations
class vxManagedObserver;
class vxColorTableCPU;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class Colormap : 
  public IElement, 
  public IObserver,
  public System::ICloneable
{
// events
public:

  // fired when an annotation is modified
  event System::EventHandler ^ Changed;

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pColormap ); }

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
  
// ICloneable implementation
public:

  /// creates a new object that is a copy of the current instance.
  virtual System::Object ^ Clone();

// Serialization
public:

  /// serializes the colormap object to xml
  System::String ^ ToXml();

  /// creates a new Colormap instance from the specified xml string
  static Colormap ^ FromXml( System::String ^ mpXml );

  /// creates a new Colormap instance from the specified xml resource
  static Colormap ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new Colormap instance from the specified xml file
  static Colormap ^ FromFile( System::String ^ mpXmlFile );

// functions
public:
  
  /// initializer
  Colormap();

  /// copy consturctor
  Colormap( Colormap ^ mpOther );

  /// destructor
  virtual ~Colormap();

  /// finalizer
  !Colormap();

  /// copies the specified colormap settings into this colormap
  void CopyFrom( Colormap ^ mpColormap );

  /// gets the shared colormap object
  System::IntPtr GetColormapPtr() { return System::IntPtr( m_pColormap ); }

  /// gets the associated name
  property System::String ^ Name
  {
    System::String ^ get() { return gcnew System::String( m_pColormap->GetName().c_str() ); }
    void set( System::String ^ mpName );
  }

  /// gets the window value
  property float Window
  {
    float get();
    void set( float fWindow );
  }

  /// gets the range setting (full/active)
  property bool IsActiveRange
  { 
    bool get() { return m_bIsActiveRange; }
  }

  /// sets the range setting (full/active)
  property bool EnableActiveRange { void set( bool bIsActiveRange ); }

  /// gets the level value
  property float Level
  {
    float get();
    void set( float fWindow );
  }
  
  /// assigned color property (READ ONLY)
  property System::Drawing::Color AssignedColor
  {
    System::Drawing::Color get() { return m_color; }
  }

  /// gets the underlying implemantation
  vxColormap & GetColormap() { return *m_pColormap; };

private:

  // common initializtion routine
  void CommonInit();

// data
private:

  // shared colormap object
  vxShareableObject< vxColormap > * m_pColormap;
  
  // is it active range (else full)
  bool m_bIsActiveRange;

  // observer
  vxManagedObserver * m_pObserver;

  // associated name
  System::String ^ m_mpName;

  // assigned color
  System::Drawing::Color m_color;

  // color list
  static ColorList * m_mpColors = new ColorList();

}; // RenderingMode


CLOSE_VISUALIZATION_NS


// $Log: v3D_Colormap.h,v $
// Revision 1.6  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.4  2006/09/25 20:13:14  geconomos
// added copy constructor
//
// Revision 1.3  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/17 21:00:21  geconomos
// cleaned up ICloneable
//
// Revision 1.2  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.24  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.23  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.22  2003/06/09 12:29:53  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.21  2003/05/27 14:20:01  geconomos
// Made GetName and SetName a propery of the class.
//
// Revision 1.20  2003/05/23 18:30:15  michael
// name is now pulled from/set in unmanaged object.
//
// Revision 1.19  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.18  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.17  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.16  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.15  2003/04/16 12:48:55  michael
// added calls to set the active/full range in the vxColormap objects
//
// Revision 1.14  2003/04/15 17:41:04  michael
// added active range
//
// Revision 1.13  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.12  2003/03/30 21:05:08  michael
// formatting and cosmetics
//
// Revision 1.11  2003/03/21 16:52:08  michael
// formatting
//
// Revision 1.10  2002/11/27 16:31:20  geconomos
// Added CopyFrom method
//
// Revision 1.9  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.8  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.7  2002/11/06 23:53:10  wenli
// Returned System::IntPtr instead of native pointer.
//
// Revision 1.6  2002/10/22 21:11:40  geconomos
// Added copy constructor from v3D Colormap.
//
// Revision 1.5  2002/10/17 15:28:16  geconomos
// Added constructor from native type and created CommonInit() method
//
// Revision 1.4  2002/10/08 19:02:30  michael
// added event handler to get the colormap GUI updated
//
// Revision 1.3  2002/10/01 23:04:23  frank
// Adding FromResource function.  Needs more work.
//
// Revision 1.2  2002/09/25 20:37:02  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.1  2002/09/20 22:16:03  michael
// Added colormap wrapper class
//
// Revision 1.5  2002/09/19 14:26:00  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.4  2002/09/17 14:37:12  geconomos
// Removed property AlphaFromWindow
//
// Revision 1.3  2002/08/22 23:58:48  dmitry
// Serialization implemented.
//
// Revision 1.2  2002/08/02 13:53:31  dmitry
// Color List added.
//
// Revision 1.1  2002/07/31 14:10:29  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Colormap.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Colormap.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $