// $Id: v3D_Triplet.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
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
#include "vxShareableObject.h"
#include "Triple.h"

// forward declarations
class vxManagedObserver;

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class Triplet : public IObserver, public IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pTriple ); }

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

/// functions
public:

  /// initializer
  Triplet();

  /// initializer from floats
  Triplet( float x, float y, float z );

  /// destructor
  virtual ~Triplet();

  /// destructor
  !Triplet();

  /// gets or sets the x component
  property float x 
  {
    float4 get() { return m_pTriple->m_x; }
    void set( float val ) { m_pTriple->m_x = val; }
  }

  /// gets or sets the y component
  property float y 
  {
    float4 get() { return m_pTriple->m_y; }
    void set( float val ) { m_pTriple->m_y = val; }
  }

  /// gets or sets the z component
  property float z 
  {
    float4 get() { return m_pTriple->m_z; }
    void set( float val ) { m_pTriple->m_z = val; }
  }

// data
private:

  vxShareableObject< Triple< float4 > > * m_pTriple;
}; // Triplet


CLOSE_VISUALIZATION_NS


// $Log: v3D_Triplet.h,v $
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:11:33  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.9  2003/08/20 14:28:55  michael
// added "mailto:"
//
// Revision 1.8  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.7  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.6  2003/05/12 19:06:50  geconomos
// Coding standards.
//
// Revision 1.5  2003/04/01 18:01:38  geconomos
// Coding convention for event naming.
//
// Revision 1.4  2003/03/07 20:34:24  geconomos
// Removed GetTriplePtr()
//
// Revision 1.3  2002/11/05 22:54:30  ingmar
// implemented IObserver interface
//
// Revision 1.2  2002/09/24 15:16:26  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Triplet.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Triplet.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $