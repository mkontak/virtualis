// $Id: MenuCommandCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Collections;
using Viatronix.v3D.Dicom.UI.Docking.Menus;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class MenuCommandCollection : CollectionWithEvents
  {
    // Instance fields
    protected string m_extraText;
    protected Font m_extraFont;
    protected Color m_extraTextColor;
    protected Brush m_extraTextBrush;
    protected Color m_extraBackColor;
    protected Brush m_extraBackBrush;
    protected bool m_showInfrequent;

    public MenuCommandCollection()
    {
      // Define defaults for internal state
      m_extraText = "";
      m_extraFont = SystemInformation.MenuFont;
      m_extraTextColor = SystemColors.ActiveCaptionText;
      m_extraTextBrush = null;
      m_extraBackColor = SystemColors.ActiveCaption;
      m_extraBackBrush = null;
      m_showInfrequent = false;
    }

    public MenuCommand Add(MenuCommand value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(MenuCommand[] values)
    {
      // Use existing method to add each array entry
      foreach(MenuCommand page in values)
        Add(page);
    }

    public void Remove(MenuCommand value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, MenuCommand value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(MenuCommand value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public MenuCommand this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as MenuCommand); }
    }

    public MenuCommand this[string text]
    {
      get 
      {
        // Search for a MenuCommand with a matching title
        foreach(MenuCommand mc in base.List)
          if (mc.Text == text)
            return mc;

        return null;
      }
    }

    public int IndexOf(MenuCommand value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }

    public bool VisibleItems()
    {
      foreach(MenuCommand mc in base.List)
      {
        // Is the item visible?
        if (mc.Visible)
        {
          // And its not a separator...
          if (mc.Text != "-")
          {
            // Then should return 'true' except when we are a sub menu item ourself
            // in which case there might not be any visible children which means that
            // this item would not be visible either.
            if ((mc.MenuCommands.Count > 0) && (!mc.MenuCommands.VisibleItems()))
              continue;

            return true;
          }
        }
      }

      return false;
    }

    public string ExtraText
    {
      get { return m_extraText; }
      set { m_extraText = value; }
    }

    public Font ExtraFont
    {
      get { return m_extraFont; }
      set { m_extraFont = value; }
    }

    public Color ExtraTextColor
    {
      get { return m_extraTextColor; }
      set { m_extraTextColor = value; }
    }

    public Brush ExtraTextBrush
    {
      get { return m_extraTextBrush; }
      set { m_extraTextBrush = value; }
    }

    public Color ExtraBackColor
    {
      get { return m_extraBackColor; }
      set { m_extraBackColor = value; }
    }

    public Brush ExtraBackBrush
    {
      get { return m_extraBackBrush; }
      set { m_extraBackBrush = value; }
    }

    public bool ShowInfrequent
    {
      get { return m_showInfrequent; }
      set { m_showInfrequent = value; }
    }
  }
}


#region revision history

// $Log: MenuCommandCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/MenuCommandCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: MenuCommandCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

