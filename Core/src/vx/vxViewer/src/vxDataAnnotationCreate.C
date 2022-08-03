// $Id: vxDataAnnotationCreate.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxDataAnnotationCreate.h"
#include "vxEnvironment.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
vxDataAnnotationCreate::vxDataAnnotationCreate()
:m_eCreationMode(vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP),
m_pAnnotation(NULL)
//*******************************************************************
{
} // constructor


#undef FILE_REVISION


// Revision History:
// $Log: vxDataAnnotationCreate.C,v $
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1  2005/05/23 20:24:31  michael
// Introduced concept of data objects to start with vxData....
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/01/23 03:59:34  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.4  2003/09/22 21:12:34  michael
// changed points to two handles that can be rendered
//
// Revision 1.3  2003/08/20 14:19:28  michael
// added pointer to environment we are manipulating it in
//
// Revision 1.2  2003/08/13 19:24:54  michael
// plane oblique creating, added environment to dynamic environment data
//
// Revision 1.1  2003/08/13 12:46:24  michael
// added initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxDataAnnotationCreate.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxDataAnnotationCreate.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
