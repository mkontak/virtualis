// $Id: Stdafx.h,v 1.7 2007/05/03 21:37:39 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _SECURE_SCL 0
//#define _HAS_ITERATOR_DEBUGGING  1

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

// includes
#include "Logger.h"
#include "vxViewerLibrary.h"
#include "ExplorerPlugin2.h"

// defines
#define OPEN_VISUALIZATION_NS namespace Viatronix {  namespace Visualization {
#define CLOSE_VISUALIZATION_NS } } 
#define USING_VISUALIZATION_NS using namespace Viatronix::Visualization;

#define OPEN_GRAPH_NS namespace Viatronix {  namespace Visualization { namespace Graph {
#define CLOSE_GRAPH_NS } } } 
#define USING_GRAPH_NS using namespace Viatronix::Visualization::Graph;

#define OPEN_ANIMATION_NS namespace Viatronix {  namespace Animations { 
#define CLOSE_ANIMATION_NS } }  
#define USING_ANIMATION_NS using namespace Viatronix::Animations;

#define OPEN_UNDOACTIONS_NS namespace Viatronix {  namespace UndoActions { 
#define CLOSE_UNDOACTIONS_NS } }  
#define USING_UNDOACTIONS_NS using namespace Viatronix::UndoActions;


#include "v3D_Exception.h"


// $Log: Stdafx.h,v $
// Revision 1.7  2007/05/03 21:37:39  gdavidson
// Added Viatronix.Visualization.Graph namespace
//
// Revision 1.6  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/28 21:05:26  romy
// UndoActions namespace added
//
// Revision 1.4  2006/05/31 16:19:13  geconomos
// porting to visual studio 2005
//
// Revision 1.3  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.2  2004/03/21 23:10:00  geconomos
// Upgrade to Visual Studio .NET 2003
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/08/22 03:40:05  geconomos
// Added namespace macross for animations.
//
// Revision 1.19  2003/08/20 14:33:16  michael
// added "mailto:"
//
// Revision 1.18  2003/05/13 19:08:31  michael
// code review
//
// Revision 1.17  2003/05/12 18:14:26  frank
// code review
//
// Revision 1.16  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/Stdafx.h,v 1.7 2007/05/03 21:37:39 gdavidson Exp $
// $Id: Stdafx.h,v 1.7 2007/05/03 21:37:39 gdavidson Exp $
