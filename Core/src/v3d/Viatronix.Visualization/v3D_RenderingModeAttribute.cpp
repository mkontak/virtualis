// $Id: v3D_RenderingModeAttribute.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

#include "stdafx.h"
#include "v3D_RenderingModeAttribute.h"


USING_VISUALIZATION_NS


/**
 * Initializes a RenderingModeAttribute with the specified display text.
 *
 * @param   mpDisplayText   Display text.
 * @param   eMode           Rendering mode bit.
 */
RenderingModeAttribute::RenderingModeAttribute( System::String ^ mpDisplayText, Viatronix::Visualization::RenderingOptions eOptions )
{
  m_mpDisplayText = mpDisplayText;
  m_eOptions = eOptions;
} // RenderingModeAttribute( mpDisplayText )



// $Log: v3D_RenderingModeAttribute.cpp,v $
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.1  2003/10/15 01:26:52  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingModeAttribute.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_RenderingModeAttribute.cpp,v 1.2 2006/10/04 18:29:05 gdavidson Exp $