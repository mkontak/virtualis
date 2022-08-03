// $Id: v3D_CadFindings.h,v 1.3 2007/04/11 20:56:25 romy Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxCadFindings.h"


// forward unmanaged declarations
class vxManagedObserver;

// namespaces
OPEN_VISUALIZATION_NS


// forward declarations
ref class ElementVector;
ref class Environment;
ref class Volume;


// class definition
public ref class CadFindings : 
  public IElement, 
  public IObserver,
  public System::ICloneable
{
public:

  /// constructor
  CadFindings();

  /// destructor
  ~CadFindings();

  /// finalizer
  !CadFindings();

 
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pCadFindings ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

  // raised when an annotation is selected
  event System::EventHandler ^ SelectionChanged;
  
// IObserver implementation
public:
  
  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );
 
// ICloneable implementation
public:

  /// creates a new object that is a copy of the current instance.
  virtual System::Object ^ Clone();

public:

  /// Gets the number of findings
  property int4 Count { int4 get() { return m_pCadFindings->GetCount(); }}

  /// Get the ID
  System::String ^ GetID( int4 iIndex );

  /// Get the centroid (in MM)
  ElementVector ^ GetCentroidMM( int4 iIndex );

  /// resets the collection
  void Reset() { m_pCadFindings->ClearFindings(); }

  /// loads cad findings from file
  void Load( System::String ^ mpPath );

  ///Stes the selected Index
  void SetSelectedIndex( const int4 index);
  
  
private:

  // shared vxCadFindings  object
  vxShareableObject<  vxCAD::vxCadFindings > * m_pCadFindings;

  // observer
  vxManagedObserver * m_pObserver;

}; // class CadFindings


CLOSE_VISUALIZATION_NS



// $Log: v3D_CadFindings.h,v $
// Revision 1.3  2007/04/11 20:56:25  romy
// made it as a vxShareableObject
//
// Revision 1.2  2007/04/06 18:02:50  romy
// fixed the cad findings load
//
// Revision 1.1  2007/04/06 14:14:10  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CadFindings.h,v 1.3 2007/04/11 20:56:25 romy Exp $
// $Id: v3D_CadFindings.h,v 1.3 2007/04/11 20:56:25 romy Exp $
