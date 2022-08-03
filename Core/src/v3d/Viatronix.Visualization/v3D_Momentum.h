// $Id: v3D_Momentum.h,v 1.2 2007/01/17 14:08:18 geconomos Exp $
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
#include "vxMomentum.h"
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


public ref class Momentum: public IElement, public IObserver
{
// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pMomentum ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:

  /// receives modified information on connected objects
  virtual void OnModified( System::IntPtr ptrInfo );

public:

  /// default constructor
  Momentum();

  /// destructor
  virtual ~Momentum();

  /// finalizer
  !Momentum();

  /// gets or sets enabled
  property bool Enabled
  {
    bool get() { return m_pMomentum->GetEnabled(); }
    void set( bool bEnabled );
  } // Enabled

  /// gets or sets the frames per second
  property int4 FramesPerSecond
  {
    int4 get() { return m_pMomentum->GetFramesPerSecond(); }
    void set( int4 iFramesPerSecond ) { m_pMomentum->SetFramesPerSecond( iFramesPerSecond ); }
  } // FramePerSecond


  /// resets the momentum object 
  void Reset() { m_pMomentum->Reset(); }

private:

  /// vxMomentum
  vxShareableObject<vxMomentum> * m_pMomentum;
  
  // observer
  vxManagedObserver * m_pObserver;

}; // class Momentum


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_Momentum.h,v $
// Revision 1.2  2007/01/17 14:08:18  geconomos
// added reset to momentum
//
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Momentum.h,v 1.2 2007/01/17 14:08:18 geconomos Exp $
// $Id: v3D_Momentum.h,v 1.2 2007/01/17 14:08:18 geconomos Exp $

