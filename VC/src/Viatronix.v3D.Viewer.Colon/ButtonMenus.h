//////////////////////////////////////////////////////////////////////
// $Id: ButtonMenus.h,v 1.4.2.1 2010/03/20 21:30:11 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Definitions of menus displayed in CMenuButton instances
// Author: Jeff Meade  jmeade@viatronix.net
//

#if !defined(AFX_BUTTONMENUS_H__94AAAE50_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_)
#define AFX_BUTTONMENUS_H__94AAAE50_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////////////
//
// Menu item IDs for each menu (id == 0 means add a separator at that menu position)
//

namespace OverviewMenus
{
  const int4 iOverviewSlices = 6;
  const UINT viOverviewSliceIds[iOverviewSlices]
    = { ID_OVERVIEW_OPTIONS_CENTERLINE,
        ID_OVERVIEW_SLICEINDICATORS_AXIAL, ID_OVERVIEW_SLICEINDICATORS_SAGITTAL,
        ID_OVERVIEW_SLICEINDICATORS_CORONAL, ID_OVERVIEW_SLICEINDICATORS_CROSSSECTIONAL, ID_OVERVIEW_SLICEINDICATORS_DISSECTION };
};

namespace EndoscopicMenus
{
  const int4 iShadowIds = 5;
  const UINT viIdShadows[iShadowIds] =
  { ID_ENDOSCOPICVIEW_OPTIONS_CENTERLINE,
    ID_ENDOSCOPICVIEW_SLICEINDICATORS_AXIAL, ID_ENDOSCOPICVIEW_SLICEINDICATORS_SAGITTAL,
    ID_ENDOSCOPICVIEW_SLICEINDICATORS_CORONAL, ID_ENDOSCOPICVIEW_SLICEINDICATORS_CROSSSECTIONAL };

  const int4 iCurrModeIds = 5;
  const UINT viIdCurrModes[iCurrModeIds] =
  { ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE,
    ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY,
    ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING,
    ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE,
    ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,
  };

  const int4 iCurrModePlusIds = 6;
  const UINT viIdCurrPlusModes[iCurrModePlusIds] =
  { ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE,
    ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY,
    ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING,
    ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE,
    ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,
    ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION
  };
};

#endif // !defined(AFX_BUTTONMENUS_H__94AAAE50_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_)


// Revision History:
// $Log: ButtonMenus.h,v $
// Revision 1.4.2.1  2010/03/20 21:30:11  dongqing
// added menu item for dissection plane display in overview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2006/10/03 17:16:21  jmeade
// variable names.
//
// Revision 1.3  2006/10/03 01:25:26  jmeade
// segment region button on controls.
//
// Revision 1.2  2006/09/13 15:52:57  jmeade
// enable segment polyp mode.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1.10.1.4.1  2005/03/14 23:01:42  jmeade
// Issue 4022: Removed beta tool.
//
// Revision 3.1.10.1  2004/09/16 22:13:03  jmeade
// support for polyp segmentation.
//
// Revision 3.1  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.0.4.1  2002/05/23 20:51:27  jmeade
// Issue 2270:  Support for 2D annotations mode.
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:00   ingmar
// Initial revision.
// Revision 1.9  2001/07/05 15:12:36  jmeade
// reordered mode menu options
//
// Revision 1.8  2001/05/08 19:17:25  jmeade
// Don't need a Bookmark menu option for Bug 377/342
//
// Revision 1.7  2001/05/07 21:43:43  binli
// Added control to toggle display bookmarks in Overview
// (TrackRecord ID: 000375 & 000342 )
//
// Revision 1.6  2001/04/13 02:31:49  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ButtonMenus.h,v 1.4.2.1 2010/03/20 21:30:11 dongqing Exp $
// $Id: ButtonMenus.h,v 1.4.2.1 2010/03/20 21:30:11 dongqing Exp $
