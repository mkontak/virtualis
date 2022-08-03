// $Id: GLPopupMenu.C,v 1.3.12.3 2010/02/25 17:50:47 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


// includes
#include "StdAfx.h"
#include "GLPopupMenu.h"

using namespace ColonViews::ChildViews;
using namespace WinUtils;


/**
 * Default Constructor
 */
GLPopupMenu::GLPopupMenu()
{
  m_pParent = GLChildViewPtr(NULL);
  m_clientRect = Rect<int4>();
  m_uId = 0;
  m_eTextLocation = GDT_TEXTLOCNONE;
} // GLPopupMenu()


/**
 * Constructor
 * @param pParent   menu's owner
 * @param rect      display rectangle
 * @param uId       menu's ID
 */
GLPopupMenu::GLPopupMenu( const GLChildViewPtr & pParent, const Rect<int4> & rect, const uint4 uId )
{
  Initialize(pParent, rect, uId);
} // GLPopupMenu()


/**
 * Initialize the menu
 * @param pParent   menu's owner
 * @param rect      display rectangle
 * @param uId       menu's ID
 */
void GLPopupMenu::Initialize(const GLChildViewPtr & pParent, const Rect<int4> & rect, const uint4 uId)
{
  m_pParent = pParent;
  m_clientRect = rect;
  m_uId = uId;
  m_eTextLocation = GDT_TEXTLOCNONE;
} // Initialize()


/**
 * Set the view position of the menu
 * @param rect      display rectangle
 */
void GLPopupMenu::MoveWindow(const Rect<int4> & rect)
{
  m_clientRect = rect;
  m_eTextLocation = GDT_TEXTLOCNONE; // set text position to be recalculated on next draw
} // MoveWindow()


/**
 * Returns true if point is within menu
 *
 * @param pt   Test point
 * @return     true if point within the menu rect, false otherwise
 */
bool GLPopupMenu::PtInMenu(const Point2D<int4>& pt) const
{
  return m_clickRect.PtInRect(pt);
} // PtInMenu()


/**
 * Recalculate text positions
 */
void GLPopupMenu::RecalcPositions()
{
  if (m_pParent == GLChildViewPtr(NULL))
  {
    return;
  }

  m_textPoint.Set(m_clientRect.m_left, m_pParent->GetViewport().Height() - m_clientRect.m_top);

  m_clickRect = m_clientRect;
  if (m_eTextLocation & GDT_LEFT)
  {
    m_clickRect.Offset(-m_clickRect.Width(), 0);
  }
  if (m_eTextLocation & GDT_ABOVE)
  {
    m_clickRect.Offset(0, -m_clickRect.Height());
  }

  // reflected rectangle for OpenGL drawing
  const int4 iHeight(m_pParent->GetViewport().Height());
  const int4 iTextPadding( 3 ); // needs to match one in SliceChildView!
  m_glEdgeRect.Set( m_clickRect.m_left - iTextPadding, iHeight - ( m_clickRect.m_bottom + iTextPadding ),
                    m_clickRect.m_right,  iHeight - m_clickRect.m_top );
} // RecalcPositions()


/**
 * Draw the menu
 *
 * @param sText           Menu text
 * @param font            menu font
 * @param eTextLoc        text location
 * @param bMenuEnabled    whether menu is to be drawn enabled
 */
void GLPopupMenu::Draw(std::string sText, const GLFont & font, const GDT_TEXTLOC & eTextLoc, const bool & bMenuEnabled)
{
  if (m_pParent == GLChildViewPtr(NULL))
  {
    return;
  }

  CPoint pt;  GetCursorPos(&pt);
  m_pParent->ScreenToClient(&pt);

  if (m_eTextLocation != eTextLoc)
  {
    m_eTextLocation = eTextLoc;
    RecalcPositions();
  }

  bool bIn = bMenuEnabled && m_clickRect.PtInRect(pt.x, pt.y);
  bool bDown = bIn && ((GetAsyncKeyState(VK_LBUTTON)&0x8000) || (GetAsyncKeyState(VK_RBUTTON)&0x8000));

  EndEllipsesText(sText, font);
  if (bIn)
  {
    Rect<int4> edgeRect(m_glEdgeRect);
    int4 iPixelWidth( font.GetPixelWidth( sText ) );
    if ( iPixelWidth > edgeRect.Width())
    {
      edgeRect.m_right = edgeRect.m_left + (iPixelWidth * 1.1);
    }

    // draw frame
    edgeRect.Draw3dEdge(bDown ? RectType::SUNKEN : RectType::RAISED);

    // draw text
    if (!bDown)
    {
      font.DrawText(m_textPoint.m_x, m_textPoint.m_y, sText.c_str(), eTextLoc);
    }
    else
    {
      font.DrawText(m_textPoint.m_x + 1, m_textPoint.m_y - 1, sText.c_str(), eTextLoc);
    }
  }
  else
  {
    font.DrawText(m_textPoint.m_x, m_textPoint.m_y, sText.c_str(), eTextLoc);
  }
} // Draw()


/**
 * For displayed text, if the end of a string does not fit in the display rectangle, it is truncated and ellipses are added.
 * @param font    text font
 * @param sText   text to test
 * @return        truncated text, if too long, or original text otherwise
 */
void GLPopupMenu::EndEllipsesText(std::string & sText, const GLFont & font)
{
  try
  {
    int4 iPixelWidth( font.GetPixelWidth( sText ) );
    if ( iPixelWidth > m_glEdgeRect.Width())
    {
      // TODO: Truncate string, with ellipses, instead of elongating menubox
      CDC *pDC = ((CWnd*)(m_pParent))->GetDC();
      CRect cr(m_glEdgeRect.m_left,m_glEdgeRect.m_top,m_glEdgeRect.m_right,m_glEdgeRect.m_bottom);

      char *pSText = new char[sText.length()+1];

      if ( !pDC || !pSText )
        return;

      strcpy(pSText, sText.c_str());
      pDC->DrawTextEx(pSText, sText.length(), &cr, DT_CALCRECT | DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_MODIFYSTRING, NULL);

      sText = pSText;
    }
  }
  catch (...)
  {
  }
} // EndEllipsesText()


// $Log: GLPopupMenu.C,v $
// Revision 1.3.12.3  2010/02/25 17:50:47  dongqing
// added try/catch to catch any possible exception in the endellipseText method
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3.12.2  2010/02/16 10:02:06  dongqing
// extra null checks for safety
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
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
// Revision 1.1.2.5  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.3.2.1  2005/05/25 15:50:22  geconomos
// initial backup
// Revision 1.1.2.3  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLPopupMenu.C,v 1.3.12.3 2010/02/25 17:50:47 dongqing Exp $
// $Id: GLPopupMenu.C,v 1.3.12.3 2010/02/25 17:50:47 dongqing Exp $
