// $Id: v3D_Annotations.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
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
#include "vxShareableObject.h"
#include "vxAnnotations.h"


// forward unmanaged declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// forward managed declarations
ref class Environment;
ref class Volume;


// class definition
public ref class Annotations : 
  public IElement, 
  public IObserver,
  public System::ICloneable
{

// events
public:

  // raised when an annotation is added
  event System::EventHandler ^ Added;
  
  // raised when an annotation operation has completed 
  event System::EventHandler ^ Completed;
  
  // raised when an annotation is removed
  event System::EventHandler ^ Removed;
  
  // raised when an annotation is modified
  event System::EventHandler ^ Changed;
  
  // raised when an annotation is selected
  event System::EventHandler ^ SelectionChanged;
  
 
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pAnnotations ); }

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

  /// serializes the annotations object to xml
  System::String ^ ToXml();

  /// creates a new Annotations instance from the specified xml string
  static Annotations ^ FromXml( System::String ^ mpXml );

  /// creates a new Annotations instance from the specified xml resource
  static Annotations ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new Annotations instance from the specified xml file
  static Annotations ^ FromFile( System::String ^ mpXmlFile );


// functions
public:

  /// initializer
  Annotations();

  /// destructor
  virtual ~Annotations();

  /// finalizer
  !Annotations();

  /// initializer from vxAnnotations
  Annotations( vxAnnotations * pAnnotations );

  /// adds an annotation 
  void Add( IAnnotationFactory ^ mpFactory );

  /// removes the annotation at the specified index
  void Remove( long iIndex ); 

	/// removes the annotations 
  void RemoveAll();  

  /// gets the text
  System::String ^ GetText( long iIndex );

  /// gets the label
  System::String ^ GetLabel( long iIndex );
  
  /// sets the label
  void SetLabel( long index, System::String ^ mpText );

  /// gets the color
  System::Drawing::Color GetColor( long iIndex );
  
  /// sets the color
  void SetColor( long iIndex, System::Drawing::Color color );

  /// gets the associated image
  System::Drawing::Bitmap ^ GetImage( long iIndex );

  /// gets the associated component index 
  int GetComponentIndex( int iIndex, Viatronix::Visualization::Volume ^ mpLabelVolume );

  /// updates all component based annotations
  void UpdateComponentAnnotations( Viatronix::Visualization::Environment ^ mpEnvironment );

	/// Gets the label Modifiable flag
	bool IsLabelEditable( long iIndex );


// properties
public:

  /// gets the number of annotations in the collection
  property long Count { long get(); }


  /// gets the index of the selected annotation or -1
  property long Selected
  { 
    long get();
    void set( long iIndex );
  }

// data
private:
  
  // shared handles set object
  vxShareableObject< vxAnnotations > * m_pAnnotations;
  
  // observer
  vxManagedObserver * m_pObserver;

}; // Annotations


CLOSE_VISUALIZATION_NS


// $Log: v3D_Annotations.h,v $
// Revision 1.6  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.4  2006/09/25 20:48:35  romy
// added label editable flag for annotations.
//
// Revision 1.3  2006/09/21 15:13:20  romy
// added annotation removeAll
//
// Revision 1.2  2004/10/04 15:07:06  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.30  2004/03/02 01:10:47  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.29.2.1  2004/02/24 20:15:11  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.29  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.28  2003/08/20 14:31:09  michael
// added "mailto:"
//
// Revision 1.27  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.26  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.25  2003/05/12 19:14:36  frank
// code review
//
// Revision 1.24  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.23  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.22  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.21  2003/03/20 13:42:32  geconomos
// Foward declaration of Environment.
//
// Revision 1.20  2003/03/18 16:47:59  geconomos
// Added UpdateComponentAnnotations() method.
//
// Revision 1.19  2003/03/18 16:16:31  geconomos
// Issue # 2916: Deleting associated annotation when deleting  a component.
//
// Revision 1.18  2003/03/04 16:51:43  geconomos
// Removed unused methods.
//
// Revision 1.17  2003/03/03 16:51:30  geconomos
// Removed unused methods from Annotations.
//
// Revision 1.16  2003/02/26 19:28:06  geconomos
// Added annotation images.
//
// Revision 1.15  2003/02/19 18:26:18  michael
// added serialization of annotations to the managed code
//
// Revision 1.14  2003/01/13 14:45:57  geconomos
// Switched implementation to new vxAnnotations class.
//
// Revision 1.13  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.12  2002/11/11 19:19:06  geconomos
// Reworked serialization.
//
// Revision 1.11  2002/11/02 03:12:41  frank
// spelling
//
// Revision 1.10  2002/10/11 21:34:23  geconomos
// Fixed some spelling.
//
// Revision 1.9  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.8  2002/09/30 14:37:58  dmitry
// All events changed to IntegerArg.
//
// Revision 1.7  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// Revision 1.6  2002/09/24 15:37:26  geconomos
// Exception handling and formatting.
//
// Revision 1.5  2002/09/20 15:34:05  dmitry
// Selected property added.
//
// Revision 1.4  2002/08/08 21:05:30  dmitry
// EventCompleted added.
//
// Revision 1.3  2002/08/07 19:42:38  dmitry
// Event handler changed, code debugged, and serialization implemented.
//
// Revision 1.2  2002/07/16 23:23:26  geconomos
// no message
//
// Revision 1.1  2002/07/10 23:13:48  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Annotations.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Annotations.h,v 1.6 2006/10/04 18:29:04 gdavidson Exp $