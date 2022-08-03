// $I: vxDataColormapEditor.C,v 1.2 2003/03/30 20:58:32 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxDataColormapEditor.h"
#include "Image.h"
#include "ResourceImage.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision:"


// static initializations
Image<PixelRGBA<uint1> > * vxDataColormapEditor::m_pImage = NULL;


/**
 * Default constructor
 */
vxDataColormapEditor::vxDataColormapEditor():
m_iSelectedColorPointIndex(-1),
m_iSelectedAlphaPointIndex(-1),
m_colormapLocation(Point2D<float4>(0.0F, 0.0F))
//*******************************************************************
{
  if (m_pImage == NULL)
  {
    m_pImage = ResourceImage::FromResource("COLORMAP");
  }
}


/**
 * Destructor
 */
vxDataColormapEditor::~vxDataColormapEditor()
//*******************************************************************
{
  //m_pImage = NULL; // don't delete it, we just got it from the resource(s)
}


#undef FILE_REVISION


// Revision History:
// $Log: vxDataColormapEditor.C,v $
// Revision 1.1  2005/05/23 20:24:31  michael
// Introduced concept of data objects to start with vxData....
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.4.4.1  2004/02/24 19:52:16  michael
// maded m_pImage a static variable so that we are not initalizing it multiple times
//
// Revision 1.4  2003/06/04 19:02:35  jmeade
// code review.
//
// Revision 1.3  2003/04/19 01:49:07  michael
// some more code review
//
// Revision 1.2  2003/03/30 20:58:32  michael
// formatting and cosmetics
//
// Revision 1.1  2002/10/04 18:10:49  michael
// renaming vxColormapEditorSharedData --> vxDataColormapEditor
//
// Revision 1.2  2002/09/26 19:23:25  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.1  2002/09/19 15:12:56  michael
// Added vxColormap and vxDataColormapEditor
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxDataColormapEditor.C,v 1.1 2005/05/23 20:24:31 michael Exp $
// $Id: vxDataColormapEditor.C,v 1.1 2005/05/23 20:24:31 michael Exp $
