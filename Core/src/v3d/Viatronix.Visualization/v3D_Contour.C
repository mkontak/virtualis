// $Id: v3D_Contour.C,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_Contour.h"
#include "v3D_Observer.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a Contour object with default values
 */
Contour::Contour()
{
  m_mpObserver = new Observer( this );
  m_pContour = new vxShareableObject< vxContour >();
  m_pContour->Connect( m_mpObserver );
} // Contour()


/**
 * Finalizer
 */
Contour::~Contour()
{
  delete m_pContour;   m_pContour = NULL;
  delete m_mpObserver; m_mpObserver = NULL;
} // ~Contour()


/** 
* Connects an observer to the object
* @param observer Observer object interested in events
*/
void Contour::Connect( System::IntPtr observer )
{
  m_pContour->Connect( reinterpret_cast< Observer * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer to the object
 * @param observer Observer object interested in events
 */
void Contour::Disconnect( System::IntPtr  observer )
{
  m_pContour->Disconnect( reinterpret_cast< Observer * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events
 * @param modifiedList Modified info list
 */
void Contour::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );

  for ( const vxModifiedInfo * pInfo(pInfoList->Begin()); pInfo != NULL; pInfo = pInfoList->Next() )
  {  
    if ( pInfo->GetFlag() & vxContour::EVENT_COMPLETED )
    {
      if ( Completed != NULL ) 
        Completed( this, System::EventArgs::Empty );
    }
  }
} // OnModified()


// $Log: v3D_Contour.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/24 20:05:33  michael
// coding standard: assign NULL after deletion
//
// Revision 1.7  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.6  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.5  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.4  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.3  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.2  2002/11/12 21:30:21  geconomos
// Made vxContour  a first class citizen of vxEnvironment ( frank really did it ).
//
// Revision 1.1  2002/11/06 22:19:41  frank
// Added Contour renderer, manipulator, and asociated classes.
//
// Revision 1.6  2002/09/30 14:39:16  dmitry
// EventCompleted added
//
// Revision 1.5  2002/09/26 14:51:52  frank
// Moved InitializeData into C++.
//
// Revision 1.4  2002/09/24 18:59:01  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_Contour.C,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id: v3D_Contour.C,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
