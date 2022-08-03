// $Id: v3D_WindowLevel.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
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
#include "v3D_WindowLevel.h"
#include "vxManagedObserver.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a WindwLevel object with default values.
 */
WindowLevel::WindowLevel()
{
  // managed type
  m_mpTitle = System::String::Empty;

  // unmanaged types 
  m_pObserver = new vxManagedObserver( this );
  m_pWindowLevel = new vxShareableObject< vxWindowLevel >();
  m_pWindowLevel->Connect( m_pObserver );
} // WindowLevel()


/**
 * Destructor.
 */
WindowLevel::~WindowLevel()
{
  this->!WindowLevel();
} // ~WindowLevel()


/**
 * Finalizer.
 */
WindowLevel::!WindowLevel()
{
  if ( m_pWindowLevel != NULL ) 
  {
    m_pWindowLevel->Disconnect( m_pObserver );
  }

  delete m_pWindowLevel; m_pWindowLevel = NULL;
  delete m_pObserver;   m_pObserver = NULL;
} // !WindowLevel()


/**
 * Gets the window value.
 *
 * @return window value
 */
float WindowLevel::Window::get()
{
  try
  {
    return m_pWindowLevel->GetWindow();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Window()


/**
 * Sets the window value.
 *
 * @param fWindow new window value
 */
void WindowLevel::Window::set( float fWindow )
{
  try
  {
    m_pWindowLevel->SetWindow( fWindow );
    m_pWindowLevel->Modified( vxModInfo( vxWindowLevel, true ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Window()


/**
 * Gets the level value.
 *
 * @return level value
 */
float WindowLevel::Level::get()
{
  try
  {
    return m_pWindowLevel->GetLevel();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Level()


/**
 * Sets the level value.
 *
 * @param fLevel new level value 
 */
void WindowLevel::Level::set( float fLevel )
{
  try
  {
    m_pWindowLevel->SetLevel( fLevel );
    m_pWindowLevel->Modified( vxModInfo( vxWindowLevel, true ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Level()


/**
 * Processes modified queue events.
 * 
 * @param modifiedList Modified info list.
 */
void WindowLevel::OnModified( System::IntPtr ptrInfo )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( ptrInfo.ToPointer() );
  
  for ( const vxModifiedInfo * pInfo(pInfoList->Begin()); pInfo != NULL; pInfo = pInfoList->Next() )
  {  
    const type_info & typeInfo = pInfo->GetTypeInfo();
  }
} // OnModified()


// Revision history:
// $Log: v3D_WindowLevel.cpp,v $
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
// Revision 1.12  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.11.2.1  2004/02/24 20:12:53  michael
// took out code that was commented out
//
// Revision 1.11  2003/12/16 13:27:53  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.10  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.9  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.8  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.7  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.6  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.5  2003/05/13 13:28:48  michael
// code review
//
// Revision 1.4  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.3  2002/09/24 14:34:37  geconomos
// Exception handling and formatting.
//
// Revision 1.2  2002/08/22 23:58:48  dmitry
// Serialization implemented.
//
// Revision 1.1  2002/07/30 20:23:26  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_WindowLevel.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id:
