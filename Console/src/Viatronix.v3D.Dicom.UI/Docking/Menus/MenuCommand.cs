// $Id: MenuCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
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
using System.Drawing.Imaging;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Menus
{
  // Declare event signature
  public delegate void CommandHandler(MenuCommand item);

  // Should animation be shown?
  public enum Animate
  {
    No,
    Yes,
    System
  }
    
  // How should animation be displayed?
  public enum Animation
  {
    System                  = 0x00100000,
    Blend                   = 0x00080000,
    SlideCenter             = 0x00040010,
    SlideHorVerPositive     = 0x00040005,
    SlideHorVerNegative     = 0x0004000A,
    SlideHorPosVerNegative  = 0x00040009,
    SlideHorNegVerPositive  = 0x00040006
  }

  [ToolboxItem(false)]
  [DefaultProperty("Text")]
  [DefaultEvent("Click")]
  public class MenuCommand : Component
  {
    // Enumeration of property change events
    public enum Property
    {
      Text,
      Enabled,
      ImageIndex,
      ImageList,
      Image,
      Shortcut,
      Checked,
      RadioCheck,
      Break,
      Infrequent,
      Visible,
      Description
    }

    // Declare the property change event signature
    public delegate void PropChangeHandler(MenuCommand item, Property prop);

    // Instance fields
    protected bool m_visible;
    protected bool m_break;
    protected string m_text;
    protected string m_description;
    protected bool m_enabled;
    protected bool m_checked;
    protected int m_imageIndex;
    protected bool m_infrequent;
    protected object m_tag;
    protected bool m_radioCheck;
    protected Shortcut m_shortcut;
    protected ImageList m_imageList;
    protected Image m_image;
    protected MenuCommandCollection m_menuItems;

    // Exposed events
    public event EventHandler Click;
    public event EventHandler Update;
    public event CommandHandler PopupStart;
    public event CommandHandler PopupEnd;
    public event PropChangeHandler PropertyChanged;

    public MenuCommand()
    {
      InternalConstruct("MenuItem", null, -1, Shortcut.None, null);
    }

    public MenuCommand(string text)
    {
      InternalConstruct(text, null, -1, Shortcut.None, null);
    }

    public MenuCommand(string text, EventHandler clickHandler)
    {
      InternalConstruct(text, null, -1, Shortcut.None, clickHandler);
    }

    public MenuCommand(string text, Shortcut shortcut)
    {
      InternalConstruct(text, null, -1, shortcut, null);
    }

    public MenuCommand(string text, Shortcut shortcut, EventHandler clickHandler)
    {
      InternalConstruct(text, null, -1, shortcut, clickHandler);
    }

    public MenuCommand(string text, ImageList imageList, int imageIndex)
    {
      InternalConstruct(text, imageList, imageIndex, Shortcut.None, null);
    }

    public MenuCommand(string text, ImageList imageList, int imageIndex, Shortcut shortcut)
    {
      InternalConstruct(text, imageList, imageIndex, shortcut, null);
    }

    public MenuCommand(string text, ImageList imageList, int imageIndex, EventHandler clickHandler)
    {
      InternalConstruct(text, imageList, imageIndex, Shortcut.None, clickHandler);
    }

    public MenuCommand(string text, 
      ImageList imageList, 
      int imageIndex, 
      Shortcut shortcut, 
      EventHandler clickHandler)
    {
      InternalConstruct(text, imageList, imageIndex, shortcut, clickHandler);
    }

    protected void InternalConstruct(string text, 
      ImageList imageList, 
      int imageIndex, 
      Shortcut shortcut, 
      EventHandler clickHandler)
    {
      // Save parameters
      m_text = text;
      m_imageList = imageList;
      m_imageIndex = imageIndex;
      m_shortcut = shortcut;
      m_description = text;

      if (clickHandler != null)
        Click += clickHandler;
		
      // Define defaults for others
      m_enabled = true;
      m_checked = false;
      m_radioCheck = false;
      m_break = false;
      m_tag = null;
      m_visible = true;
      m_infrequent = false;
      m_image = null;

      // Create the collection of embedded menu commands
      m_menuItems = new MenuCommandCollection();
    }

    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public MenuCommandCollection MenuCommands
    {
      get { return m_menuItems; }
    }

    [DefaultValue("MenuItem")]
    [Localizable(true)]
    public string Text
    {
      get { return m_text; }
			
      set 
      { 
        if (m_text != value)
        {
          m_text = value;
          OnPropertyChanged(Property.Text);
        } 
      }
    }

    [DefaultValue(true)]
    public bool Enabled
    {
      get { return m_enabled; }

      set 
      {
        if (m_enabled != value)
        {
          m_enabled = value;
          OnPropertyChanged(Property.Enabled);
        }
      }
    }

    [DefaultValue(-1)]
    public int ImageIndex
    {
      get { return m_imageIndex; }

      set 
      { 
        if (m_imageIndex != value)
        {
          m_imageIndex = value;
          OnPropertyChanged(Property.ImageIndex);
        } 
      }
    }

    [DefaultValue(null)]
    public ImageList ImageList
    {
      get { return m_imageList; }

      set 
      { 
        if (m_imageList != value)
        {
          m_imageList = value;
          OnPropertyChanged(Property.ImageList);
        }
      }
    }

    [DefaultValue(null)]
    public Image Image
    {
      get { return m_image; }
            
      set
      {
        if (m_image != value)
        {
          m_image = value;
          OnPropertyChanged(Property.Image);
        }
      }
    }

    [DefaultValue(typeof(Shortcut), "None")]
    public Shortcut Shortcut
    {
      get { return m_shortcut; }

      set 
      { 
        if (m_shortcut != value)
        {
          m_shortcut = value;
          OnPropertyChanged(Property.Shortcut);
        }
      }
    }

    [DefaultValue(false)]
    public bool Checked
    {
      get { return m_checked; }

      set 
      { 
        if (m_checked != value)
        {
          m_checked = value;
          OnPropertyChanged(Property.Checked);
        }
      }
    }

    [DefaultValue(false)]
    public bool RadioCheck
    {
      get { return m_radioCheck; }

      set 
      { 
        if (m_radioCheck != value)
        {
          m_radioCheck = value;
          OnPropertyChanged(Property.RadioCheck);
        }
      }
    }

    [DefaultValue(false)]
    public bool Break
    {
      get { return m_break; }
			
      set 
      { 
        if (m_break != value)
        {
          m_break = value;
          OnPropertyChanged(Property.Break);
        }
      }
    }

    [DefaultValue(false)]
    public bool Infrequent
    {
      get { return m_infrequent; }
			
      set 
      {	
        if (m_infrequent != value)
        {
          m_infrequent = value;
          OnPropertyChanged(Property.Infrequent);
        }
      }
    }

    [DefaultValue(true)]
    public bool Visible
    {
      get { return m_visible; }

      set 
      { 
        if (m_visible != value)
        {
          m_visible = value;
          OnPropertyChanged(Property.Visible);
        }
      }
    }

    [Browsable(false)]
    public bool IsParent
    {
      get { return (m_menuItems.Count > 0); }
    }

    [DefaultValue("")]
    [Localizable(true)]
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    }

    [DefaultValue(null)]
    public object Tag
    {
      get { return m_tag; }
      set { m_tag = value; }
    }

    public virtual void OnPropertyChanged(Property prop)
    {
      // Any attached event handlers?
      if (PropertyChanged != null)
        PropertyChanged(this, prop);
    }

    public void PerformClick()
    {
      // Update command with correct state
      OnUpdate(EventArgs.Empty);
            
      // Notify event handlers of click event
      OnClick(EventArgs.Empty);
    }
  
    public virtual void OnClick(EventArgs e)
    {
      // Any attached event handlers?
      if (Click != null)
        Click(this, e);
    }

    public virtual void OnUpdate(EventArgs e)
    {
      // Any attached event handlers?
      if (Update != null)
        Update(this, e);
    }

    public virtual void OnPopupStart()
    {
      // Any attached event handlers?
      if (PopupStart != null)
        PopupStart(this);
    }
            
    public virtual void OnPopupEnd()
    {
      // Any attached event handlers?
      if (PopupEnd != null)
        PopupEnd(this);
    }
  }
}

#region revision history

// $Log: MenuCommand.cs,v $
// Revision 1.1  2005/10/11 14:40:23  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Menus/MenuCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
// $Id: MenuCommand.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $

#endregion

