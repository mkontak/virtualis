// $Id: v3D_FontInfo.cpp,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_FontInfo.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"

// namespaces
USING_VISUALIZATION_NS

/**
* Initializes a FontInfo object with default values.
*/
FontInfo::FontInfo()
{
  m_pFontInfo = new vxShareableObject< vxFontInfo >();
} // FontInfo


/**
 * destructor.
 */
FontInfo::~FontInfo()
{
  this->!FontInfo();
} // ~FontInfo()


/**
 * Finalizer.
 */
FontInfo::!FontInfo()
{
  if (m_pFontInfo != NULL)
  {
    delete m_pFontInfo;
    m_pFontInfo = NULL;
  }
} // !FontInfo()


/**
 * Sets the font size.
 *
 * @param   iSize    Font size.
 */
void FontInfo::Size::set( int iSize ) 
{ 
  m_pFontInfo->SetSize( iSize ); 
  m_pFontInfo->Modified( vxModInfo( vxFontInfo, vxUtils::FONT_SIZE_CHANGED ) );
} // set_Size()


/**
 * Sets the font family name.
 *
 * @param   mpFontFamily    Font family name.
 */
void FontInfo::FontFamily::set( System::String ^ mpFontFamily ) 
{ 
  m_pFontInfo->SetFontFamily( ss( mpFontFamily ) ); 
  m_pFontInfo->Modified( vxModInfo( vxFontInfo, vxUtils::FONT_FAMILY_NAME_CHANGED ) );
} // set_FontFamily()


/** 
 * Connects an observer to the object.
 */
void FontInfo::Connect( System::IntPtr observer )
{
  m_pFontInfo->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 */
void FontInfo::Disconnect( System::IntPtr  observer )
{
  m_pFontInfo->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


// Revision History:
// $Log: v3D_FontInfo.cpp,v $
// Revision 1.4  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.1.2.1  2004/02/24 20:08:26  michael
// coding standard: assign NULL after deletion
//
// Revision 1.1  2003/10/02 10:26:11  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_FontInfo.cpp,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_FontInfo.cpp,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
