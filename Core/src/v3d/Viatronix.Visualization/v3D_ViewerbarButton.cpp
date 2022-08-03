// $Id: v3D_ViewerbarButton.cpp,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_ViewerbarButton.h"
#include "v3D_Viewer.h"
#include "v3D_Viewerbar.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Default constructor.
 */
ViewerbarButton::ViewerbarButton() : 
  m_bIsDropSource( false ),
  m_bHasContextMenu( false ),
  m_bVisible( true ),
  m_mpID( System::String::Empty )
{
} // ViewerbarButton()


/**
 * sets the button image
 *
 * @param mpImage the new image
 */
void ViewerbarButton::Image::set( System::Drawing::Bitmap ^ mpImage )
{
  m_mpImage = mpImage;
  if ( m_mpViewer != nullptr && m_mpViewer->ToolBar != nullptr )
  {
    m_mpViewer->ToolBar->Invalidate();
  }
} // set_Image( mpImage )


// $Log: v3D_ViewerbarButton.cpp,v $
// Revision 1.5.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.5.2.1  2009/03/23 15:13:44  gdavidson
// Added the ablility to changed the visibility of a viewerbar button
//
// Revision 1.5  2007/07/17 21:06:19  gdavidson
// Added ID property
//
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/06/16 18:39:21  frank
// invalidated the button when the image is set
//
// Revision 1.2  2004/10/13 16:04:49  geconomos
// Added new properties.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerbarButton.cpp,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_ViewerbarButton.cpp,v 1.5.2.2 2009/07/29 19:54:07 kchalupa Exp $
