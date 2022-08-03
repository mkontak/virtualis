// $Id: TabPage.cs,v 1.1 2005/10/11 14:41:57 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.IO;
using System.Drawing;
using System.Resources;
using System.Reflection;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxItem(false)]
  [DefaultProperty("Title")]
  [DefaultEvent("PropertyChanged")]
  public class TabPage : Panel
  {
    // Enumeration of property change events
    public enum Property
    {
      Title,
      Control,
      ImageIndex,
      ImageList,
      Icon,
      Selected,
    }

    // Declare the property change event signature
    public delegate void PropChangeHandler(TabPage page, Property prop, object oldValue);

    // Public events
    public event PropChangeHandler PropertyChanged;

    // Instance fields
    protected string m_title;
    protected Control m_control;
    protected int m_imageIndex;
    protected ImageList m_imageList;
    protected Icon m_icon;
    protected bool m_selected;
    protected Control m_startFocus;
    protected bool m_shown;

    public TabPage()
    {
      InternalConstruct("Page", null, null, -1, null);
    }

    public TabPage(string title)
    {
      InternalConstruct(title, null, null, -1, null);
    }

    public TabPage(string title, Control control)
    {
      InternalConstruct(title, control, null, -1, null);
    }
			
    public TabPage(string title, Control control, int imageIndex)
    {
      InternalConstruct(title, control, null, imageIndex, null);
    }

    public TabPage(string title, Control control, ImageList imageList, int imageIndex)
    {
      InternalConstruct(title, control, imageList, imageIndex, null);
    }

    public TabPage(string title, Control control, Icon icon)
    {
      InternalConstruct(title, control, null, -1, icon);
    }

    protected void InternalConstruct(string title, 
      Control control, 
      ImageList imageList, 
      int imageIndex,
      Icon icon)
    {
      // Assign parameters to internal fields
      m_title = title;
      m_control = control;
      m_imageIndex = imageIndex;
      m_imageList = imageList;
      m_icon = icon;

      // Appropriate defaults
      m_selected = false;
      m_startFocus = null;
    }

    [DefaultValue("Page")]
    [Localizable(true)]
    public string Title
    {
      get { return m_title; }
		   
      set 
      { 
        if (m_title != value)
        {
          string oldValue = m_title;
          m_title = value; 

          OnPropertyChanged(Property.Title, oldValue);
        }
      }
    }

    [DefaultValue(null)]
    public Control Control
    {
      get { return m_control; }

      set 
      { 
        if (m_control != value)
        {
          Control oldValue = m_control;
          m_control = value; 

          OnPropertyChanged(Property.Control, oldValue);
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
          int oldValue = m_imageIndex;
          m_imageIndex = value; 

          OnPropertyChanged(Property.ImageIndex, oldValue);
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
          ImageList oldValue = m_imageList;
          m_imageList = value; 

          OnPropertyChanged(Property.ImageList, oldValue);
        }
      }
    }

    [DefaultValue(null)]
    public Icon Icon
    {
      get { return m_icon; }
		
      set 
      { 
        if (m_icon != value)
        {
          Icon oldValue = m_icon;
          m_icon = value; 

          OnPropertyChanged(Property.Icon, oldValue);
        }
      }
    }

    [DefaultValue(true)]
    public bool Selected
    {
      get { return m_selected; }

      set
      {
        if (m_selected != value)
        {
          bool oldValue = m_selected;
          m_selected = value;

          OnPropertyChanged(Property.Selected, oldValue);
        }
      }
    }

    [DefaultValue(null)]
    public Control StartFocus
    {
      get { return m_startFocus; }
      set { m_startFocus = value; }
    }

    public virtual void OnPropertyChanged(Property prop, object oldValue)
    {
      // Any attached event handlers?
      if (PropertyChanged != null)
        PropertyChanged(this, prop, oldValue);
    }
        
    internal bool Shown
    {
      get { return m_shown; }
      set { m_shown = value; }
    }
  }
}

#region revision history

// $Log: TabPage.cs,v $
// Revision 1.1  2005/10/11 14:41:57  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabControl/TabPage.cs,v 1.1 2005/10/11 14:41:57 mkontak Exp $
// $Id: TabPage.cs,v 1.1 2005/10/11 14:41:57 mkontak Exp $

#endregion

