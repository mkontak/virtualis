// $Id: vxFontInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

#include "stdafx.h"
#include "vxFontInfo.h"

/**
 * Default consturctor.
 */
vxFontInfo::vxFontInfo() : 
 m_uSize( 16 ), 
 m_sFontFamily( "Arial" )
{
} // vxFontInfo()


/**
 * Constructor from size and family name.
 *
 * @param   uSize         Font size in pixels
 * @param   sFontFamily   Font family name (i.e. Arial, Courier )
 */
vxFontInfo::vxFontInfo( uint4 uSize, const std::string & sFontFamily ) : 
 m_uSize( uSize ), 
 m_sFontFamily( sFontFamily )
{
} // vxFontInfo( uSize, sFamilyName )


/**`
 * Virtual destructor.
 */
vxFontInfo::~vxFontInfo()
{
} // ~vxFontInfo()


// $Log: vxFontInfo.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/10/02 10:22:44  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxFontInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxFontInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $