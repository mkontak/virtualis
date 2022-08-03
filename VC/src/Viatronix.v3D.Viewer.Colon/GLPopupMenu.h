// $Id: GLPopupMenu.h,v 1.3.12.1 2007/12/12 23:08:16 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
//#include "GLFont.h"
#include "GLChildView.h"
#include "typedef.h"
#include "Rect.h"

namespace WinUtils
{

  /**
   * A popup menu drawn in the current OpenGL viewport
   */
  class GLPopupMenu
  {
  public:

    /// Default Constructor
    GLPopupMenu();

    /// Constructor
    GLPopupMenu(const ColonViews::ChildViews::GLChildViewPtr & pParent, const Rect<int4> & rect, const uint4 uId);

    /// Initialize the menu
    void Initialize(const ColonViews::ChildViews::GLChildViewPtr & pParent, const Rect<int4> & rect, const uint4 uId);

    /// Set the view position of the menu
    void MoveWindow(const Rect<int4> & rect);

    /// Returns true if point is within menu
    bool PtInMenu(const Point2D<int4>& pt) const;

    /// Recalculate text positions
    void RecalcPositions();

    /// Draw the menu
    void Draw(std::string sText, const GLFont & font, const GDT_TEXTLOC & eTextLoc, const bool & bMenuEnabled = true);

    /// Returns the menu ID
    inline uint4 GetID() const { return m_uId;  }

  private:
    /// For displayed text, if the end of a string does not fit in the display rectangle, it is truncated and ellipses are added.
    void EndEllipsesText(std::string & sText, const GLFont & font);

  public:
    /// Windows menu for popup items
    CMenu m_winMenu;

  private:

    /// Id for the popup menu
    uint4 m_uId;

    /// Client rectangle for menu to be drawn
    Rect<int4> m_clientRect;

    /// Parent child view pointer
    ColonViews::ChildViews::GLChildViewPtr m_pParent;

    /// Location of the menu with respect to the stored draw coordinate
    GDT_TEXTLOC m_eTextLocation;

    /// Point at which menu is drawn
    Point2D<int4> m_textPoint;

    /// Rectangle bordering menu
    Rect<int4> m_glEdgeRect;

    /// Rectangle in which a mouse click displays rectangle
    Rect<int4> m_clickRect;

  }; // class GLPopupMenu

} // namespace WinUtils

// $Log: GLPopupMenu.h,v $
// Revision 1.3.12.1  2007/12/12 23:08:16  jmeade
// Issue 5874: truncate long menu strings with ellipsis.
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.4  2005/04/21 12:56:08  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 1.1.2.3  2005/04/19 13:40:55  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 1.1.2.2  2005/04/14 02:47:08  jmeade
// popup menu for rendering mode.
// more encapsulation for GLPopupMenu class.
//
// Revision 1.1.2.1  2005/04/13 19:21:39  frank
// split out GL popup menu class
//
// Revision 1.1.2.1  2005/04/13 19:17:33  frank
// split out GL popup menu class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLPopupMenu.h,v 1.3.12.1 2007/12/12 23:08:16 jmeade Exp $
// $Id: GLPopupMenu.h,v 1.3.12.1 2007/12/12 23:08:16 jmeade Exp $
