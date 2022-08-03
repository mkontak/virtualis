// $Id: vxManipulatorUtils.C,v 1.3 2006/03/13 13:51:46 frank Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// include declarations
#include "stdafx.h"
#include "vxManipulatorUtils.h"
#include "vxShareableObject.h"
#include "vxViewerTypeMPROblique.h"
#include "vxInput.h"


/**
 * Warn user if pressing shift key on an oblique view (has no effect)
 * @param environment    the environment
 * @param uKeyboardState state of the keyboard
 */
void vxManipulatorUtils::WarnShiftOblique( const vxEnvironment & environment, const uint4 uKeyboardState )
{

  const bool bDerivedFromOblique( dynamic_cast< vxViewerTypeMPROblique * >( environment.GetViewerType() ) != NULL );
  const bool bPressingShiftKey( uKeyboardState & vxInput::STATE_SHIFT );

  if ( bDerivedFromOblique && bPressingShiftKey )
  {
    vxShareableObject<vxOverlayStrings> * pOverlayStrings = dynamic_cast<vxShareableObject<vxOverlayStrings> *>(environment.GetElement(vxIDs::OverlayStrings));
    pOverlayStrings->AddElement(vxOverlayStrings::vxOverlayElement("It is not possible to restrict this operation to a double oblique plane.", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>(1, 1, 0)));
    pOverlayStrings->Modified(vxModInfoID(vxElement, vxIDs::OverlayStrings, 0));
  }

} // WarnShiftOblique()


// $Log: vxManipulatorUtils.C,v $
// Revision 1.3  2006/03/13 13:51:46  frank
// formatting
//
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1  2001/01/03 19:05:27  michael
// adding class with helper function for some manipulators to display message
// in case user wants to use "SHIFT" in oblique views which is not supported
// by all viewer types.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorUtils.C,v 1.3 2006/03/13 13:51:46 frank Exp $
// $Id: vxManipulatorUtils.C,v 1.3 2006/03/13 13:51:46 frank Exp $
