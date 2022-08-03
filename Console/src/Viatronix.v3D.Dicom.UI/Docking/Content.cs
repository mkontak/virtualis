// $Id: Content.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Xml;
using System.Drawing;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class Content
  {
    // Enumeration of property change events
    public enum Property
    {
      Control,
      Title,
      FullTitle,
      ImageList,
      ImageIndex,
      CaptionBar,
      CloseButton,
      HideButton,
      DisplaySize,
      AutoHideSize,
      FloatingSize,
      DisplayLocation
    }

    // Declare the property change event signature
    public delegate void PropChangeHandler(Content obj, Property prop);

    // Class constant
    protected static int m_defaultDisplaySize = 150;
    protected static int m_defaultAutoHideSize = 150;
    protected static int m_defaultFloatingSize = 150;
    protected static int m_defaultLocation = 150;
    protected static int m_counter = 0;

    // Instance fields
    protected Control m_control;
    protected string m_title;
    protected string m_fullTitle;
    protected ImageList m_imageList;
    protected int m_imageIndex;
    protected Size m_displaySize;
    protected Size m_autoHideSize;
    protected Size m_floatingSize;
    protected Point m_displayLocation;
    protected int m_order;
    protected DockingManager m_manager;
    protected bool m_docked;
    protected bool m_autoHidden;
    protected bool m_visible;
    protected bool m_captionBar;
    protected bool m_closeButton;
    protected bool m_hideButton;
    protected AutoHidePanel m_autoHidePanel;
    protected WindowContent m_parentWindowContent;
    protected Restore m_defaultRestore;
    protected Restore m_autoHideRestore;
    protected Restore m_dockingRestore;
    protected Restore m_floatingRestore;

    // Instance events
    public event PropChangeHandler PropertyChanging;
    public event PropChangeHandler PropertyChanged;

    public Content(XmlTextReader xmlIn, int formatVersion)
    {
      // Define the initial object state
      m_control = null;
      m_title = "";
      m_fullTitle = "";
      m_imageList = null;
      m_imageIndex = -1;
      m_manager = null;
      m_parentWindowContent = null;
      m_displaySize = new Size(m_defaultDisplaySize, m_defaultDisplaySize);
      m_autoHideSize = new Size(m_defaultAutoHideSize, m_defaultAutoHideSize);
      m_floatingSize = new Size(m_defaultFloatingSize, m_defaultFloatingSize);
      m_displayLocation = new Point(m_defaultLocation, m_defaultLocation);
      m_order = m_counter++;
      m_visible = false;
      m_defaultRestore = null;
      m_autoHideRestore = null;
      m_floatingRestore = null;
      m_dockingRestore = null;
      m_autoHidePanel = null;
      m_docked = true;
      m_captionBar = true;
      m_closeButton = true;
      m_hideButton = true;
      m_autoHidden = false;

      // Overwrite default with values read in
      LoadFromXml(xmlIn, formatVersion);
    }

    public Content(DockingManager manager)
    {
      InternalConstruct(manager, null, "", null, -1);
    }

    public Content(DockingManager manager, Control control)
    {
      InternalConstruct(manager, control, "", null, -1);
    }

    public Content(DockingManager manager, Control control, string title)
    {
      InternalConstruct(manager, control, title, null, -1);
    }

    public Content(DockingManager manager, Control control, string title, ImageList imageList, int imageIndex)
    {
      InternalConstruct(manager, control, title, imageList, imageIndex);
    }

    protected void InternalConstruct(DockingManager manager, 
      Control control, 
      string title, 
      ImageList imageList, 
      int imageIndex)
    {
      // Must provide a valid manager instance
      if (manager == null)
        throw new ArgumentNullException("DockingManager");

      // Define the initial object state
      m_control = control;
      m_title = title;
      m_imageList = imageList;
      m_imageIndex = imageIndex;
      m_manager = manager;
      m_parentWindowContent = null;
      m_order = m_counter++;
      m_visible = false;
      m_displaySize = new Size(m_defaultDisplaySize, m_defaultDisplaySize);
      m_autoHideSize = new Size(m_defaultAutoHideSize, m_defaultAutoHideSize);
      m_floatingSize = new Size(m_defaultFloatingSize, m_defaultFloatingSize);
      m_displayLocation = new Point(m_defaultLocation, m_defaultLocation);
      m_defaultRestore = new RestoreContentState(State.DockLeft, this);
      m_floatingRestore = new RestoreContentState(State.Floating, this);
      m_autoHideRestore = new RestoreAutoHideState(State.DockLeft, this);
      m_dockingRestore = m_defaultRestore;
      m_autoHidePanel = null;
      m_docked = true;
      m_captionBar = true;
      m_closeButton = true;
      m_hideButton = true;
      m_autoHidden = false;
      m_fullTitle = title;
    }

    public DockingManager DockingManager
    {
      get { return m_manager; }
      set { m_manager = value; }
    }

    public Control Control
    {
      get { return m_control; }
			
      set 
      {
        if (m_control != value)
        {
          OnPropertyChanging(Property.Control);
          m_control = value;
          OnPropertyChanged(Property.Control);
        }
      }
    }

    public string Title
    {
      get { return m_title; }

      set 
      {
        if (m_title != value)
        {
          OnPropertyChanging(Property.Title);
          m_title = value;
          OnPropertyChanged(Property.Title);
        }
      }
    }
        
    public string FullTitle
    {
      get { return m_fullTitle; }
            
      set
      {
        if (m_fullTitle != value)
        {
          OnPropertyChanging(Property.FullTitle);
          m_fullTitle = value;
          OnPropertyChanged(Property.FullTitle);
        }
      }
    }

    public ImageList ImageList
    {
      get { return m_imageList; }

      set 
      {
        if(m_imageList != value) 
        {
          OnPropertyChanging(Property.ImageList);
          m_imageList = value; 
          OnPropertyChanged(Property.ImageList);
        }
      }
    }

    public int ImageIndex
    {
      get { return m_imageIndex; }

      set 
      {
        if (m_imageIndex != value)
        {
          OnPropertyChanging(Property.ImageIndex);
          m_imageIndex = value;
          OnPropertyChanged(Property.ImageIndex);
        }
      }
    }

    public bool CaptionBar
    {
      get { return m_captionBar; }
            
      set
      {
        if (m_captionBar != value)
        {
          OnPropertyChanging(Property.CaptionBar);
          m_captionBar = value;
          OnPropertyChanged(Property.CaptionBar);
        }
      }
    }

    public bool CloseButton
    {
      get { return m_closeButton; }
            
      set
      {
        if (m_closeButton != value)
        {
          OnPropertyChanging(Property.CloseButton);
          m_closeButton = value;
          OnPropertyChanged(Property.CloseButton);
        }
      }
    }

    public bool HideButton
    {
      get { return m_hideButton; }
            
      set
      {
        if (m_hideButton != value)
        {
          OnPropertyChanging(Property.HideButton);
          m_hideButton = value;
          OnPropertyChanged(Property.HideButton);
        }
      }
    }

    public Size DisplaySize
    {
      get { return m_displaySize; }

      set 
      {
        if (m_displaySize != value)
        {
          OnPropertyChanging(Property.DisplaySize);
          m_displaySize = value;
          OnPropertyChanged(Property.DisplaySize);
        }
      }
    }
        
    public Size AutoHideSize
    {
      get { return m_autoHideSize; }
            
      set
      {
        if (m_autoHideSize != value)
        {
          OnPropertyChanging(Property.AutoHideSize);
          m_autoHideSize = value;
          OnPropertyChanged(Property.AutoHideSize);
        }
      }
    }

    public Size FloatingSize
    {
      get { return m_floatingSize; }

      set 
      {
        if (m_floatingSize != value)
        {
          OnPropertyChanging(Property.FloatingSize);
          m_floatingSize = value;
          OnPropertyChanged(Property.FloatingSize);
        }
      }
    }

    public Point DisplayLocation
    {
      get { return m_displayLocation; }

      set 
      {
        if (m_displayLocation != value)
        {
          OnPropertyChanging(Property.DisplayLocation);
          m_displayLocation = value;
          OnPropertyChanged(Property.DisplayLocation);
        }
      }
    }
        
    public int Order
    {
      get { return m_order; }
    }

    public bool Visible
    {
      get { return m_visible; }
    }

    public Restore DefaultRestore
    {
      get { return m_defaultRestore; }
      set { m_defaultRestore = value; }
    }

    public Restore AutoHideRestore
    {
      get { return m_autoHideRestore; }
      set { m_autoHideRestore = value; }
    }
        
    public Restore DockingRestore
    {
      get { return m_dockingRestore; }
      set { m_dockingRestore = value; }
    }

    public Restore FloatingRestore
    {
      get { return m_floatingRestore; }
      set { m_floatingRestore = value; }
    }

    public bool Docked
    {
      get { return m_docked; }
      set { m_docked = value; }
    }

    public WindowContent ParentWindowContent
    {
      get { return m_parentWindowContent; }
            
      set 
      { 
        if (m_parentWindowContent != value)
        {
          m_parentWindowContent = value; 

          // Recalculate the visibility value
          UpdateVisibility();
        }
      }
    }

    public AutoHidePanel AutoHidePanel
    {
      get { return m_autoHidePanel; }
            
      set 
      {
        if (m_autoHidePanel != value)
        {
          m_autoHidePanel = value; 
                
          // Recalculate the visibility value
          UpdateVisibility();
        }
      }
    }

    internal bool AutoHidden
    {
      get { return m_autoHidden; }

      set 
      { 
        if (m_autoHidden != value)
        {
          m_autoHidden = value; 

          // Recalculate the visibility value
          UpdateVisibility();
        }                
      }
    }

    public void UpdateVisibility()
    {
      m_visible = ((m_parentWindowContent != null) || (m_autoHidden && (m_autoHidePanel != null)));
    }

    public virtual void OnPropertyChanging(Property prop)
    {
      // Any attached event handlers?
      if (PropertyChanging != null)
        PropertyChanging(this, prop);
    }

    public virtual void OnPropertyChanged(Property prop)
    {
      // Any attached event handlers?
      if (PropertyChanged != null)
        PropertyChanged(this, prop);
    }

    public void BringToFront()
    {
      if (!m_visible)
      {
        // Use docking manager to ensure we are Visible
        m_manager.ShowContent(this);
      }
            
      if (m_autoHidden)
      {
        // Request docking manager bring to window into view
        m_manager.BringAutoHideIntoView(this);
      }
      else
      {
        // Ask the parent WindowContent to ensure we are the active Content
        m_parentWindowContent.BringContentToFront(this);
      }
    }

    public Restore RecordRestore()
    {
      if (m_parentWindowContent != null)
      {
        if (m_autoHidden)
          return RecordAutoHideRestore();
        else
        {			    
          Form parentForm = m_parentWindowContent.ParentZone.FindForm();

          // Cannot record restore information if not in a Form
          if (parentForm != null)
          {
            // Decide which restore actually needs recording
            if (parentForm is FloatingForm)
              return RecordFloatingRestore();
            else
              return RecordDockingRestore();
          }	
        }
      }

      return null;
    }

    public Restore RecordAutoHideRestore()
    {
      // Remove any existing restore object
      m_autoHideRestore = null;
                
      // We should be inside a parent window
      if (m_parentWindowContent != null)
      {
        // And in the auto hidden state
        if (m_autoHidden)
        {
          // Get access to the AutoHostPanel that contains use
          AutoHidePanel ahp = m_parentWindowContent.DockingManager.AutoHidePanelForContent(this);
                    
          // Request the ahp create a relevant restore object for us
          m_autoHideRestore = ahp.RestoreObjectForContent(this);
        }
      }
        
      return m_autoHideRestore;
    }

    public Restore RecordDockingRestore()
    {
      // Remove any existing Restore object
      m_dockingRestore = null;

      // Do we have a parent window we are inside?
      if (m_parentWindowContent != null)
      {
        // Ask the parent to provide a Restore object for us
        m_dockingRestore = m_parentWindowContent.RecordRestore(this);
      }

      // If we cannot get a valid Restore object from the parent then we have no choice 
      // but to use the default restore which is less accurate but better than nothing
      if (m_dockingRestore == null)
        m_dockingRestore = m_defaultRestore;

      return m_dockingRestore;
    }
		
    public Restore RecordFloatingRestore()
    {
      // Remove any existing Restore object
      m_floatingRestore = null;

      // Do we have a parent window we are inside?
      if (m_parentWindowContent != null)
      {
        // Ask the parent to provide a Restore object for us
        m_floatingRestore = m_parentWindowContent.RecordRestore(this);
      }

      // If we cannot get a valid Restore object from the parent then we have no choice 
      // but to use the default restore which is less accurate but better than nothing
      if (m_floatingRestore == null)
        m_floatingRestore = m_defaultRestore;

      return m_floatingRestore;
    }

    internal void ContentBecomesFloating()
    {
      m_docked = false;

      if (m_parentWindowContent != null)
      {
        switch(m_parentWindowContent.State)
        {
          case State.DockLeft:
          case State.DockRight:
          case State.DockTop:
          case State.DockBottom:
            // Record the current position before content is moved
            RecordDockingRestore();
            break;
          case State.Floating:
          default:
            // Do nothing, already floating
            break;
        }
      }
    }

    internal void ContentLeavesFloating()
    {
      m_docked = true;

      if (m_parentWindowContent != null)
      {
        switch(m_parentWindowContent.State)
        {
          case State.DockLeft:
          case State.DockRight:
          case State.DockTop:
          case State.DockBottom:
            // Do nothing, already floating
            break;
          case State.Floating:
          default:
            // Record the current position before content is moved
            RecordFloatingRestore();
            break;
        }
      }
    }

    internal void ReconnectRestore()
    {
      m_defaultRestore.Reconnect(m_manager);
      m_autoHideRestore.Reconnect(m_manager);
      m_dockingRestore.Reconnect(m_manager);
      m_floatingRestore.Reconnect(m_manager);
    }

    internal void SaveToXml(XmlTextWriter xmlOut)
    {
      // Output standard values appropriate for all Content 
      xmlOut.WriteStartElement("Content");
      xmlOut.WriteAttributeString("Name", m_title);
      xmlOut.WriteAttributeString("Visible", m_visible.ToString());
      xmlOut.WriteAttributeString("Docked", m_docked.ToString());
      xmlOut.WriteAttributeString("AutoHidden", m_autoHidden.ToString());
      xmlOut.WriteAttributeString("CaptionBar", m_captionBar.ToString());
      xmlOut.WriteAttributeString("CloseButton", m_closeButton.ToString());
      xmlOut.WriteAttributeString("DisplaySize", ConversionHelper.SizeToString(m_displaySize));
      xmlOut.WriteAttributeString("DisplayLocation", ConversionHelper.PointToString(m_displayLocation));
      xmlOut.WriteAttributeString("AutoHideSize", ConversionHelper.SizeToString(m_autoHideSize));
      xmlOut.WriteAttributeString("FloatingSize", ConversionHelper.SizeToString(m_floatingSize));
      xmlOut.WriteAttributeString("FullTitle", m_fullTitle);

      // Save the Default Restore object to Xml
      xmlOut.WriteStartElement("DefaultRestore");
      m_defaultRestore.SaveToXml(xmlOut);
      xmlOut.WriteEndElement();

      // Save the AutoHideRestore object to Xml
      xmlOut.WriteStartElement("AutoHideRestore");
      m_autoHideRestore.SaveToXml(xmlOut);
      xmlOut.WriteEndElement();
            
      // Save the DockingRestore object to Xml
      xmlOut.WriteStartElement("DockingRestore");
      m_dockingRestore.SaveToXml(xmlOut);
      xmlOut.WriteEndElement();

      // Save the floating Restore object to Xml
      xmlOut.WriteStartElement("FloatingRestore");
      m_floatingRestore.SaveToXml(xmlOut);
      xmlOut.WriteEndElement();

      xmlOut.WriteEndElement();
    }

    internal void LoadFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      // Read in the attribute values
      string attrTitle = xmlIn.GetAttribute(0);
      string attrVisible = xmlIn.GetAttribute(1);
      string attrDocked = xmlIn.GetAttribute(2);
      string attrAutoHide = xmlIn.GetAttribute(3);
      string attrCaptionBar = xmlIn.GetAttribute(4);
      string attrCloseButton = xmlIn.GetAttribute(5);
      string attrDisplaySize = xmlIn.GetAttribute(6);
      string attrDisplayLocation = xmlIn.GetAttribute(7);
      string attrAutoHideSize = xmlIn.GetAttribute(8);
      string attrFloatingSize = xmlIn.GetAttribute(9);
      string attrFullTitle = attrTitle;
            
      // 'FullTitle' property added in version 5 format and above
      if (formatVersion >= 5)
        attrFullTitle = xmlIn.GetAttribute(10);

      // Convert to correct types
      m_title = attrTitle;
      m_visible = System.Convert.ToBoolean(attrVisible);
      m_docked = System.Convert.ToBoolean(attrDocked);
      m_autoHidden = System.Convert.ToBoolean(attrAutoHide);
      m_captionBar = System.Convert.ToBoolean(attrCaptionBar);
      m_closeButton = System.Convert.ToBoolean(attrCloseButton);
      m_displaySize = ConversionHelper.StringToSize(attrDisplaySize);
      m_displayLocation = ConversionHelper.StringToPoint(attrDisplayLocation);
      m_autoHideSize = ConversionHelper.StringToSize(attrAutoHideSize);
      m_floatingSize = ConversionHelper.StringToSize(attrFloatingSize);
      m_fullTitle = attrFullTitle;

      // Load the Restore objects
      m_defaultRestore = Restore.CreateFromXml(xmlIn, true, formatVersion);
      m_autoHideRestore  = Restore.CreateFromXml(xmlIn, true, formatVersion);
      m_dockingRestore  = Restore.CreateFromXml(xmlIn, true, formatVersion);
      m_floatingRestore = Restore.CreateFromXml(xmlIn, true, formatVersion);

      // Move past the end element
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");
		
      // Check it has the expected name
      if (xmlIn.NodeType != XmlNodeType.EndElement)
        throw new ArgumentException("EndElement expected but not found");
    }
  } 
}

#region revision history

// $Log: Content.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Content.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Content.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

