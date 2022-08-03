// $Id: DrawCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Drawing;

namespace Viatronix.v3D.Dicom.UI.Docking.Menus
{
  internal class DrawCommand
  {
    // Instance fields
    protected int m_row;
    protected int m_col;
    protected char m_mnemonic;
    protected bool m_enabled;
    protected bool m_subMenu;
    protected bool m_expansion;
    protected bool m_separator;
    protected bool m_vertSeparator;
    protected bool m_chevron;
    protected bool m_topBorder;
    protected bool m_bottomBorder;
    protected bool m_infrequent;
    protected Rectangle m_drawRect;
    protected Rectangle m_selectRect;
    protected MenuCommand m_command;

    public DrawCommand(Rectangle drawRect)
    {
      m_row = -1;
      m_col = -1;
      m_mnemonic = '0';
      m_enabled = true;
      m_subMenu = false;
      m_expansion = false;
      m_separator = false;
      m_vertSeparator = false;
      m_topBorder = false;
      m_bottomBorder = false;
      m_infrequent = false;
      m_chevron = true;
      m_drawRect = drawRect;
      m_selectRect = drawRect;
      m_command = null;
    }

    public DrawCommand(Rectangle drawRect, bool expansion)
    {
      m_row = -1;
      m_col = -1;
      m_mnemonic = '0';
      m_enabled = true;
      m_subMenu = false;
      m_expansion = expansion;
      m_separator = !expansion;
      m_vertSeparator = !expansion;
      m_topBorder = false;
      m_bottomBorder = false;
      m_infrequent = false;
      m_chevron = false;
      m_drawRect = drawRect;
      m_selectRect = drawRect;
      m_command = null;
    }

    public DrawCommand(MenuCommand command, Rectangle drawRect)
    {
      InternalConstruct(command, drawRect, drawRect, -1, -1);
    }

    public DrawCommand(MenuCommand command, Rectangle drawRect, Rectangle selectRect)
    {
      InternalConstruct(command, drawRect, selectRect, -1, -1);
    }

    public DrawCommand(MenuCommand command, Rectangle drawRect, int row, int col)
    {
      InternalConstruct(command, drawRect, drawRect, row, col);
    }

    public void InternalConstruct(MenuCommand command, Rectangle drawRect, Rectangle selectRect, int row, int col)
    {
      m_row = row;
      m_col = col;
      m_enabled = command.Enabled;
      m_expansion = false;
      m_vertSeparator = false;
      m_drawRect = drawRect;
      m_selectRect = selectRect;
      m_command = command;
      m_topBorder = false;
      m_bottomBorder = false;
      m_infrequent = command.Infrequent;

      m_chevron = false;

      // Is this MenuCommand a separator?
      m_separator = (m_command.Text == "-");

      // Does this MenuCommand contain a submenu?
      m_subMenu = (m_command.MenuCommands.Count > 0);

      // Find position of first mnemonic character
      int position = -1;
            
      if (command.Text != null)
        position = command.Text.IndexOf('&');

      // Did we find a mnemonic indicator?
      if (position != -1)
      {
        // Must be a character after the indicator
        if (position < (command.Text.Length - 1))
        {
          // Remember the character
          m_mnemonic = char.ToUpper(command.Text[position + 1]);
        }
      }
    }

    public Rectangle DrawRect
    {
      get { return m_drawRect; }
      set { m_drawRect = value; }
    }

    public Rectangle SelectRect
    {
      get { return m_selectRect; }
      set { m_selectRect = value; }
    }

    public MenuCommand MenuCommand
    {
      get { return m_command; }
    }

    public bool Separator
    {
      get { return m_separator; }
    }

    public bool VerticalSeparator
    {
      get { return m_vertSeparator; }
    }

    public bool Expansion
    {
      get { return m_expansion; }
    }

    public bool SubMenu
    {
      get { return m_subMenu; }
    }

    public char Mnemonic
    {
      get { return m_mnemonic; }
    }

    public bool Enabled
    {
      get { return m_enabled; }
    }

    public int Row
    {
      get { return m_row; }
    }

    public int Col
    {
      get { return m_col; }
    }

    public bool Chevron
    {
      get { return m_chevron; }
    }

    public bool TopBorder
    {
      get { return m_topBorder; }
      set { m_topBorder = value; }
    }

    public bool BottomBorder
    {
      get { return m_bottomBorder; }
      set { m_bottomBorder = value; }
    }

    public bool Infrequent
    {
      get { return m_infrequent; }
      set { m_infrequent = value; }
    }
  }
}

#region revision history

// $Log: DrawCommand.cs,v $
// Revision 1.1  2005/10/11 14:40:23  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Menus/DrawCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
// $Id: DrawCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $

#endregion

