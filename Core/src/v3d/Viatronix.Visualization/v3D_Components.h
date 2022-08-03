// $Id: v3D_Components.h,v 1.14 2007/06/28 19:34:14 jmeade Exp $
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
#include "vxComponentArray.h"
#include "ComponentHandler.h"


// unmanaged forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS 


// managed forward declarations
ref class Volume;


// class definition
public ref class Components sealed :
  public IElement,
  public IObserver
{
// events
public:

  /// raised when a component is added
  event System::EventHandler ^ Added;
  
  /// raised when a component is removed
  event System::EventHandler ^ Removed;
  
  /// raised when a component is modified
  event System::EventHandler ^ Changed;

  /// raised when a component's name is modified
  event System::EventHandler ^ NameChanged;

  /// raised when a component is selected
  event System::EventHandler ^ Selected;

  /// raised when a component is Removing
  event System::EventHandler ^ Removing;

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pvComponents ); }

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

// Serialization
public:

  /// serializes the Components instance to xml
  System::String ^ ToXml();

  /// creates a new Components instance from the specified xml string
  static Components ^ FromXml( System::String ^ mpXml );

  /// creates a new Components instance from the specified xml resource
  static Components ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new Components instance from the specified xml file
  static Components ^ FromFile( System::String ^ mpXmlFile );
  
  /// configure the components from xml
  void ConfigureFromXml( System::Xml::XmlNode ^ mpParent );

// functions
public:  
  
  /// initializer
  Components();

  /// initializer from components
  Components( Components ^ components );
  
  /// initializer from vxComponentArray
  Components( System::IntPtr componentArray );
  
  /// finalizer
  virtual ~Components();

  /// finalizer
  !Components();

  /// initializes the internal data structures
  void Initialize();

  /// Set a restore point
  void SetRestorePoint();

	///the composite Undo interface
	void StartCompositeUndo();

	///End the composite Undo interface
	void EndCompositeUndo();

	///Compact the holes
	void Compact();

  /// gets the currently selected component
  property int SelectedItem { int get(); }

  /// gets the specified component's name
  System::String ^ GetName( long iIndex );
  
  /// sets the specified component's name
  void SetName( long index, System::String ^ mpName );
  
  /// gets the specified component's color
  System::Drawing::Color GetColor( long iIndex );
  
  /// sets the specified component's color
  void SetColor( long iIndex, System::Drawing::Color color );

  /// gets the specified component's label visible flag
  bool GetLabelVisible( long iIndex );
  
  /// sets the specified component's label visible flag
  void SetLabelVisible( long iIndex, bool bVisible );

  /// gets the specified component's density visible flag
  bool GetIntensityVisible( long iIndex );
  
  /// sets the specified component's density visible flag
  void SetItensityVisible( long iIndex, bool bVisible );
  
  /// gets the specified component's locked flag
  bool GetLocked( long iIndex );
  
  /// sets the specified component's locked flag
  void SetLocked( long index, bool bLocked );

  /// gets the specified component's selected flag
  long GetSelected();
  
  /// sets the specified component's selected flag
  void SetSelected( long iIndex );

  /// gets if the specified component is segmented
  bool GetSegmented( long iIndex );

  /// gets the component's label index
  int GetLabelIndex( long iHistoryIndex );

  /// gets the specified component's removable status
  bool GetRemovable( long iIndex );

  /// gets the specified component's growable status
  bool GetGrowable( long iIndex );

  /// gets the specified component's shrinkable status
  bool GetShrinkable( long iIndex );

  /// gets the specified component's unlockable status
  bool GetUnlockable( long iIndex );

  /// gets the specified component's name editable status
  bool GetNameEditable( long iIndex );

  /// gets the lnumber if components
  property long Count { long get() { return m_pComponentHandler->GetComponentCount(); }}

  /// gets the label volume
  property Viatronix::Visualization::Volume ^ LabelVolume 
  { 
    Viatronix::Visualization::Volume ^ get() { return m_mpLabelVolume; }
    void set( Viatronix::Visualization::Volume ^ mpLabelVolume );
  }
  
  /// gets the underlying implementation
  System::IntPtr GetComponentsPtr() { return System::IntPtr( m_pvComponents ) ; }

  /// gets the underlying implementation
  System::IntPtr GetCompHdlrPtr() { return System::IntPtr( m_pComponentHandler ) ; }

  /// delete a comopnent from the list
  void DeleteComponent( long iHistoryIndex );

  /// resets the list to a deafult state
  void Reset();

  /// updates components on user inputs
  void UpdateOnUserInput();

  ///Gets the component object from the index specified
  vxComponent * GetComponent( long iIndex ) 
  { 
    // get the underlying component data
    vxComponentArray * pComponentArray(ToNativeComponentsPtr( GetComponentsPtr() ));
    return pComponentArray->operator []( iIndex );
  } 

// fields
private:
  
  // shareable vector of pointers to component objects
  vxShareableObject< vxComponentArray > * m_pvComponents;
  
  // component handler with components
  ComponentHandler * m_pComponentHandler;

  // attached label volume object
  Viatronix::Visualization::Volume ^ m_mpLabelVolume;

  // observer
  vxManagedObserver * m_pObserver;

}; // class Components


CLOSE_VISUALIZATION_NS


// $Log: v3D_Components.h,v $
// Revision 1.14  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.13.2.1  2007/04/30 21:13:32  romy
// exposes the compact method
//
// Revision 1.13  2007/02/07 22:42:58  romy
// added Removing Event - This helps Plaques to reset the label volume before it gets removed.
//
// Revision 1.12  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.11  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.10  2006/09/26 14:47:05  romy
// added Compiste Undo action interfaces for the managed calls
//
// Revision 1.9  2006/06/05 15:05:36  frank
// bad mix of managed and unmanaged code
//
// Revision 1.8  2006/04/18 18:49:15  romy
// naming convention followed for 'm_pObserver'
//
// Revision 1.7  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.6  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/02/15 17:08:11  frank
// added unlockable attribute
//
// Revision 1.5  2004/12/13 15:34:05  frank
// added name editable property
//
// Revision 1.4  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.3  2004/07/12 18:06:13  frank
// added support for the growable, shrinkable, and removable component properties
//
// Revision 1.2  2004/05/11 14:41:46  frank
// added ability to configure the components from xml
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.30  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.29  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.28  2003/08/07 11:34:28  frank
// fixed typo
//
// Revision 1.27  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.26  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.25  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.24  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.23  2003/04/10 18:30:28  geconomos
// Proper handling of selected index.
//
// Revision 1.22  2003/04/09 15:06:25  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.21  2003/03/31 13:42:22  geconomos
// Updated components class event names to coding conventions..
//
// Revision 1.20  2003/03/18 16:16:31  geconomos
// Issue # 2916: Deleting associated annotation when deleting  a component.
//
// Revision 1.19  2003/03/10 15:00:44  geconomos
// Implemented EVENT_NAME_MODIFIED flag.
//
// Revision 1.18  2002/11/26 13:41:20  michael
// moved initialize of ComponentHandler our of constructor (from vxComponentArray)
// Should only be called once label volume is set!!!
//
// Revision 1.17  2002/11/14 14:14:20  geconomos
// Added constructor from vxComponentArray.
//
// Revision 1.16  2002/11/14 14:09:44  geconomos
// Added copy constructor.
//
// Revision 1.15  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.14  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.13  2002/10/31 12:50:15  geconomos
// Chaged GetColormapPtr() to return a System::IntPtr for interoperability.
//
// Revision 1.12  2002/10/18 17:20:57  geconomos
// Added read-only property for Segmented.
//
// Revision 1.11  2002/10/11 21:33:49  geconomos
// Implemented IObserver interface.
//
// Revision 1.10  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.9  2002/10/04 13:31:00  geconomos
// Added component handler.
//
// Revision 1.8  2002/09/25 22:37:46  wenli
// Add BoxOfInterest
//
// Revision 1.7  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Components.h,v 1.14 2007/06/28 19:34:14 jmeade Exp $
// $Id: v3D_Components.h,v 1.14 2007/06/28 19:34:14 jmeade Exp $