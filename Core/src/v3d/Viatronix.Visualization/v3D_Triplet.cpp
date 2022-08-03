// $Id: v3D_Triplet.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)
//
// Complete history at bottom of file.

// includes
#include "StdAfx.h"
#include "v3D_Triplet.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a Triplet object with default values.
 */
Triplet::Triplet()
{
  m_pTriple = new vxShareableObject< Triple< float4 > >();
}


/**
 * Initializer from components
 * @param x component
 * @param y component
 * @param z component
 */
Triplet::Triplet( float x, float y, float z )
{  
  m_pTriple = new vxShareableObject< Triple< float4 > >( Triple< float4>( x, y, z) );
} // Triplet( x, y, z )


/**
 * Destructor
 */
Triplet::~Triplet()
{
  this->!Triplet();
} // ~Triplet()


/**
 * Finalizer
 */
Triplet::!Triplet()
{
  delete m_pTriple;
  m_pTriple = NULL;
} // !Triplet()


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void Triplet::Connect( System::IntPtr observer )
{
  m_pTriple->Connect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void Triplet::Disconnect( System::IntPtr  observer )
{
  m_pTriple->Disconnect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void Triplet::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// $Log: v3D_Triplet.cpp,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.8.2.1  2004/02/24 20:10:54  michael
// took out code that was commented out
//
// Revision 1.8  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.7  2003/08/20 14:28:55  michael
// added "mailto:"
//
// Revision 1.6  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.5  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.4  2003/05/12 19:06:50  geconomos
// Coding standards.
//
// Revision 1.3  2002/11/05 22:54:30  ingmar
// implemented IObserver interface
//
// Revision 1.2  2002/09/24 15:16:26  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Triplet.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Triplet.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $