// $Id: StdAfx.h,v 1.10 2006/06/08 13:26:29 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: unknown


// pragmas
#pragma once


// defines
#define _CRT_SECURE_NO_DEPRECATE



// Description: include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently 


// TODO: Jeff I added this, let me know
#define _ATL_APARTMENT_THREADED 
#define _WIN32_WINNT 0x0501
#pragma warning(disable:4786)



//
// Windows includes
//
#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#include <afxdisp.h>
#include <winuser.h>
#include <afxcmn.h>	
#include <afxpriv.h>

// OpenGL Includes
#include <GL/gl.h>
#include <GL/glu.h>

// common includes
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <bitset>
#include <WinUser.h>

#include "Logger.h"
#include "vxBaseLibrary.h"
#include "vxPdfLibrary.h"
#include "vxViewerLibrary.h"
#include "VolApi.h"



// namespace definitions
#define OPEN_WINUTILS_NS namespace WinUtils {
#define CLOSE_WINUTILS_NS }

#define OPEN_DIALOGS_NS namespace ColonViews { namespace Dialogs {
#define CLOSE_DIALOGS_NS } }

#define OPEN_LAYOUTS_NS namespace ColonViews { namespace Layouts {
#define CLOSE_LAYOUTS_NS } }

#define OPEN_MOVIES_NS namespace Movies {
#define CLOSE_MOVIES_NS }

#define OPEN_COLON_NS namespace Colon {
#define CLOSE_COLON_NS }
#define USING_COLON_NS using namespace Colon;


#define OPEN_UTILITIES_NS namespace Utilities {
#define CLOSE_UTILITIES_NS }
#define USING_UTILITIES_NS using namespace Utilities;


// $Log: StdAfx.h,v $
// Revision 1.10  2006/06/08 13:26:29  dongqing
// less checking
//
// Revision 1.9  2006/01/31 14:27:19  frank
// code review
//
// Revision 1.8  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.7  2005/11/07 20:40:38  geconomos
// added atl headers
//
// Revision 1.6.2.1  2005/11/10 15:47:30  geconomos
// issue# 4529: only vxservice user can log in
//
// Revision 1.6  2005/10/28 13:29:58  geconomos
// added Movies namespace
//
// Revision 1.5  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/10/05 15:25:51  geconomos
// added another namespace define
//
// Revision 1.3  2005/10/03 13:01:01  geconomos
// new gui build
//
// Revision 1.2  2005/08/30 17:38:10  geconomos
// reorganized a bit
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0.20.1.4.1  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.0.20.1.6.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.0.20.1  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:42:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:00   ingmar
// Initial revision.
// Revision 1.5  2001/04/13 02:34:36  jmeade
// headers and footers
//
// Revision 1.4  2001/01/11 22:44:07  geconomos
// Added #pragma to diable warning 4786
//
// Revision 1.3  2000/11/10 20:39:17  jcohen
// Initial merge of Database support
//
// Revision 1.2  2000/11/06 20:29:50  jeff
// Disabling warning 4786
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// Revision 1.1.1.1.6.1  2000/11/02 20:11:13  jcohen
// Support for active x
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 2     8/28/00 2:38p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/11/00  Time:  3:44p
// Created StdAfx.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/StdAfx.h,v 1.10 2006/06/08 13:26:29 dongqing Exp $
// $Id: StdAfx.h,v 1.10 2006/06/08 13:26:29 dongqing Exp $
