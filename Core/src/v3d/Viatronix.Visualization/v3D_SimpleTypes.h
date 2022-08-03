// $Id: v3D_SimpleTypes.h,v 1.9 2006/11/02 21:46:04 romy Exp $
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
#include "StdAfx.h"
#include "vxManagedObserver.h"
#include "vxOverlayStrings.h"
#include "vxSeedpoints.h"

//#include "vxGraph.h"


// namespaces
OPEN_VISUALIZATION_NS


// List of simple elements
DEFINE_SIMPLE_ELEMENT( OverlayStrings )
//DEFINE_SIMPLE_ELEMENT( Graph )


//DEFINE_SIMPLE_ELEMENT( Seedpoints )

public ref class Seedpoints :
  public IElement,
  public IObserver
{
public:
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pObject ); }
  virtual void Connect( System::IntPtr observer )
  {
    m_pObject->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
  }
  virtual void Disconnect( System::IntPtr observer )
  {
    m_pObject->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
  }
  virtual event System::EventHandler ^ Modified;
public:
  virtual void OnModified( System::IntPtr ptrInfo ) {}
public:
  Seedpoints( )
  {
    m_pObject = new vxShareableObject< vxSeedpoints >();
    m_pObserver = new vxManagedObserver( this );
    m_pObject->Connect( m_pObserver );
  }
  ~Seedpoints()
  {
    m_pObject->Disconnect( m_pObserver );
    delete m_pObserver;
  }
  !Seedpoints()
  {
    m_pObject->Disconnect( m_pObserver );
    delete m_pObserver;
  }
private:
  vxShareableObject< vxSeedpoints > * m_pObject;
  vxManagedObserver * m_pObserver;
};
  
inline vxShareableObject< vxSeedpoints > * ToNativePtrSeedpoints( System::IntPtr ptr )
{
  return _native_ptr_t< vxSeedpoints >( ptr );
}



// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_SimpleTypes.h,v $
// Revision 1.9  2006/11/02 21:46:04  romy
// Made vxGraph an abstract class. So no need of a simple type
//
// Revision 1.8  2006/11/02 04:10:18  romy
// added Graph
//
// Revision 1.7  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2006/09/01 13:27:20  frank
// moved seedpoints to viewer/visualization
//
// Revision 1.4  2006/08/07 19:44:43  gdavidson
// removed DataRendererSoftware
//
// Revision 1.3  2006/03/27 19:47:11  gdavidson
// Removed SliceShadow
//
// Revision 1.2  2004/10/29 12:16:16  frank
// wrapped data renderer software in a managed class
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.4  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.3  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.2  2003/05/12 18:14:43  frank
// Added slice shadows object
//
// Revision 1.1  2003/04/10 02:48:48  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SimpleTypes.h,v 1.9 2006/11/02 21:46:04 romy Exp $
// $Id: v3D_SimpleTypes.h,v 1.9 2006/11/02 21:46:04 romy Exp $
