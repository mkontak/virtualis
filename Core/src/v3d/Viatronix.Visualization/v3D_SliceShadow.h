// $Id: v3D_SliceShadow.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "v3D_Enums.h"
#include "vxShareableObject.h"
#include "vxSliceShadow.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// forward declarations
ref class PlaneOrtho;
ref class PlaneOblique;
ref class RenderingMode;


// class definition
public ref class  SliceShadow : public IElement, public IObserver
{

// events
public:

  /// raised when the SliceShadow object is modified
  event System::EventHandler ^ Changed;

// functions
public:
  
  /// initializer
  SliceShadow();
  
  /// destructor
  virtual ~SliceShadow();

  /// finalizer
  !SliceShadow();

  /// adds an ortho plane
  void AddPlane( Viatronix::v3D::Visualization::PlaneOrtho ^ mpPlaneOrtho );

  /// removes an ortho plane
  void RemovePlane( Viatronix::v3D::Visualization::PlaneOrtho ^ mpPlaneOrtho );

  /// adds an oblique plane
  void AddPlane( Viatronix::v3D::Visualization::PlaneOblique ^ mpPlaneOblique );

  /// removes an oblique plane
  void RemovePlane( Viatronix::v3D::Visualization::PlaneOblique ^ mpPlaneOblique );

  /// adds an rendering mode
  void AddRenderingMode( Viatronix::v3D::Visualization::RenderingMode ^ mpRenderingMode );

  /// removes an rendering mode
  void RemoveRenderingMode( Viatronix::v3D::Visualization::RenderingMode ^ mpRenderingMode );
    
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pSliceShadow ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr mpObserver );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr mpObserver );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// data
private:

  /// vxPlaneOrtho<float4> pointer
  vxShareableObject< vxSliceShadow > * m_pSliceShadow;
  
  /// observer
  vxManagedObserver * m_pObserver;

}; // SliceShadow 


inline vxShareableObject< vxSliceShadow > * ToNativePtrSliceShadow( System::IntPtr ptr )
{
  return _native_ptr_t< vxSliceShadow >( ptr );
}


CLOSE_VISUALIZATION_NS


// $Log: v3D_SliceShadow.h,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/03/27 19:41:49  gdavidson
// new build
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_SliceShadow.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_SliceShadow.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
