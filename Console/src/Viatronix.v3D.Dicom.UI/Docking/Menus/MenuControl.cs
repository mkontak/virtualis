// $Id: MenuControl.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
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
using System.Reflection;
using System.Collections;
using System.Drawing.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using Microsoft.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Menus
{
  [ToolboxBitmap(typeof(MenuControl))]
  [DefaultProperty("MenuCommands")]
  [DefaultEvent("PopupSelected")]
  [Designer(typeof(Viatronix.v3D.Dicom.UI.Docking.Menus.MenuControlDesigner))]
  public class MenuControl : ContainerControl, IMessageFilter
  {
    internal class MdiClientSubclass : NativeWindow
    {
      protected override void WndProc(ref Message m)
      {
        switch(m.Msg)
        {
          case (int)Win32.Msgs.WM_MDISETMENU:
          case (int)Win32.Msgs.WM_MDIREFRESHMENU:
            return;
        }

        base.WndProc(ref m);
      }			
    }

    // Class constants
    protected const int m_lengthGap = 3;
    protected const int m_boxExpandUpper = 1;
    protected const int m_boxExpandSides = 2;
    protected const int m_shadowGap = 4;
    protected const int m_shadowYOffset = 4;
    protected const int m_separatorWidth = 15;
    protected const int m_subMenuBorderAdjust = 2;
    protected const int m_minIndex = 0;
    protected const int m_restoreIndex = 1;
    protected const int m_closeIndex = 2;
    protected const int m_chevronIndex = 3;
    protected const int m_buttonLength = 16;
    protected const int m_chevronLength = 12;
    protected const int m_pendantLength = 48;
    protected const int m_pendantOffset = 3;

    // Class constant is marked as 'readonly' to allow non constant initialization
    protected readonly int WM_OPERATEMENU = (int)Win32.Msgs.WM_USER + 1;

    // Class fields
    protected static ImageList m_menuImages = null;
    protected static bool m_supportsLayered = false;

    // Instance fields
    protected int m_rowWidth;
    protected int m_rowHeight;
    protected int m_trackItem;
    protected int m_breadthGap;
    protected int m_animateTime;
    protected IntPtr m_oldFocus;
    protected Pen m_controlLPen;
    protected bool m_animateFirst;
    protected bool m_selected;
    protected bool m_multiLine;
    protected bool m_mouseOver;
    protected bool m_manualFocus;
    protected bool m_drawUpwards;
    protected bool m_defaultFont;
    protected bool m_defaultBackColor;
    protected bool m_defaultTextColor;
    protected bool m_defaultHighlightBackColor;
    protected bool m_defaultHighlightTextColor;
    protected bool m_defaultSelectedBackColor;
    protected bool m_defaultSelectedTextColor;
    protected bool m_defaultPlainSelectedTextColor;
    protected bool m_plainAsBlock;
    protected bool m_dismissTransfer;
    protected bool m_ignoreMouseMove;
    protected bool m_expandAllTogether;
    protected bool m_rememberExpansion;
    protected bool m_deselectReset;
    protected bool m_highlightInfrequent;
    protected bool m_exitLoop;
    protected Color m_textColor;
    protected Color m_highlightBackColor;
    protected Color m_useHighlightBackColor;
    protected Color m_highlightTextColor;
    protected Color m_highlightBackDark;
    protected Color m_highlightBackLight;
    protected Color m_selectedBackColor;
    protected Color m_selectedTextColor;
    protected Color m_plainSelectedTextColor;
    protected Form m_activeChild;
    protected Form m_mdiContainer;
    protected VisualStyle m_style;
    protected Direction m_direction;
    protected PopupMenu m_popupMenu;
    protected ArrayList m_drawCommands;
    protected SolidBrush m_controlLBrush;
    protected SolidBrush m_backBrush;
    protected Animate m_animate;
    protected Animation m_animateStyle;
    internal MdiClientSubclass m_clientSubclass;
    protected MenuCommand m_chevronStartCommand;
    protected MenuCommandCollection m_menuCommands;

    // Instance fields - pendant buttons
    protected InertButton m_minButton;
    protected InertButton m_restoreButton;
    protected InertButton m_closeButton;

    // Instance fields - events
    public event CommandHandler Selected;
    public event CommandHandler Deselected;

    static MenuControl()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_menuImages = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Menus.MenuControl"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesMenuControl.bmp",
        new Size(m_buttonLength, m_buttonLength),
        new Point(0,0));

      // We need to know if the OS supports layered windows
      m_supportsLayered = (OSFeature.Feature.GetVersionPresent(OSFeature.LayeredWindows) != null);
    }

    public MenuControl()
    {
      // Set default values
      m_trackItem = -1;
      m_oldFocus = IntPtr.Zero;
      m_minButton = null;
      m_popupMenu = null;
      m_activeChild = null;
      m_closeButton = null;
      m_controlLPen = null;
      m_mdiContainer = null;
      m_restoreButton = null;
      m_controlLBrush = null;
      m_chevronStartCommand = null;
      m_animateFirst = true;
      m_exitLoop = false;
      m_selected = false;
      m_multiLine = false;
      m_mouseOver = false;
      m_defaultFont = true;
      m_manualFocus = false;
      m_drawUpwards = false;
      m_plainAsBlock = false;
      m_clientSubclass = null;
      m_ignoreMouseMove = false;
      m_deselectReset = true;
      m_expandAllTogether = true;
      m_rememberExpansion = true;
      m_highlightInfrequent = true;
      m_dismissTransfer = false;
      m_style = VisualStyle.IDE;
      m_direction = Direction.Horizontal;
      m_menuCommands = new MenuCommandCollection();
      this.Dock = DockStyle.Top;
      this.Cursor = System.Windows.Forms.Cursors.Arrow;
			
      // Animation details
      m_animateTime = 100;
      m_animate = Animate.System;
      m_animateStyle = Animation.System;

      // Prevent flicker with double buffering and all painting inside WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);

      // Should not be allowed to select this control
      SetStyle(ControlStyles.Selectable, false);

      // Hookup to collection events
      m_menuCommands.Cleared += new CollectionClear(OnCollectionCleared);
      m_menuCommands.Inserted += new CollectionChange(OnCollectionInserted);
      m_menuCommands.Removed += new CollectionChange(OnCollectionRemoved);

      // Need notification when the MenuFont is changed
      Microsoft.Win32.SystemEvents.UserPreferenceChanged += 
        new UserPreferenceChangedEventHandler(OnPreferenceChanged);

      DefineColors();
            
      // Set the starting Font
      DefineFont(SystemInformation.MenuFont);

      // Do not allow tab key to select this control
      this.TabStop = false;

      // Default to one line of items
      this.Height = m_rowHeight;

      // Add ourself to the application filtering list
      Application.AddMessageFilter(this);
    }

    protected override void Dispose(bool disposing)
    {
      if( disposing )
      {
        // Remove notifications
        Microsoft.Win32.SystemEvents.UserPreferenceChanged -= 
          new UserPreferenceChangedEventHandler(OnPreferenceChanged);
      }
      base.Dispose( disposing );
    }

    [Category("Behaviour")]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public MenuCommandCollection MenuCommands
    {
      get { return m_menuCommands; }
    } 

    [Category("Appearance")]
    public VisualStyle Style
    {
      get { return m_style; }
			
      set
      {
        if (m_style != value)
        {
          m_style = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public override Font Font
    {
      get { return base.Font; }
			
      set
      {
        if (value != base.Font)
        {
          m_defaultFont = (value == SystemInformation.MenuFont);

          DefineFont(value);

          Recalculate();
          Invalidate();
        }
      }
    }

    public override Color BackColor
    {
      get { return base.BackColor; }

      set
      {
        if (value != base.BackColor)
        {
          m_defaultBackColor = (value == SystemColors.Control);
          base.BackColor = value;
          m_backBrush = new SolidBrush(base.BackColor);
                    
          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeBackColor()
    {
      return this.BackColor != SystemColors.Control;
    }

    [Category("Appearance")]
    public Color TextColor
    {
      get { return m_textColor; }

      set
      {
        if (value != m_textColor)
        {
          m_textColor = value;
          m_defaultTextColor = (value == SystemColors.MenuText);

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeTextColor()
    {
      return m_textColor != SystemColors.MenuText;
    }

    [Category("Appearance")]
    public Color HighlightBackColor
    {
      get { return m_highlightBackColor; }

      set
      {
        if (value != m_highlightBackColor)
        {
          m_defaultHighlightBackColor = (value == SystemColors.Highlight);
          DefineHighlightBackColors(value);                    

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeHighlightBackColor()
    {
      return m_highlightBackColor != SystemColors.Highlight;
    }

    [Category("Appearance")]
    public Color HighlightTextColor
    {
      get { return m_highlightTextColor; }

      set
      {
        if (value != m_highlightTextColor)
        {
          m_highlightTextColor = value;
          m_defaultHighlightTextColor = (value == SystemColors.MenuText);

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeHighlightTextColor()
    {
      return m_highlightTextColor != SystemColors.HighlightText;
    }

    [Category("Appearance")]
    public Color SelectedBackColor
    {
      get { return m_selectedBackColor; }

      set
      {
        if (value != m_selectedBackColor)
        {
          DefineSelectedBackColors(value);
          m_defaultSelectedBackColor = (value == SystemColors.Control);

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeSelectedBackColor()
    {
      return m_selectedBackColor != SystemColors.Control;
    }

    [Category("Appearance")]
    public Color SelectedTextColor
    {
      get { return m_selectedTextColor; }

      set
      {
        if (value != m_selectedTextColor)
        {
          m_selectedTextColor = value;
          m_defaultSelectedTextColor = (value == SystemColors.MenuText);

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeSelectedTextColor()
    {
      return m_selectedTextColor != SystemColors.MenuText;
    }

    [Category("Appearance")]
    public Color PlainSelectedTextColor
    {
      get { return m_plainSelectedTextColor; }

      set
      {
        if (value != m_plainSelectedTextColor)
        {
          m_plainSelectedTextColor = value;
          m_defaultPlainSelectedTextColor = (value == SystemColors.ActiveCaptionText);

          Recalculate();
          Invalidate();
        }
      }
    }

    private bool ShouldSerializePlainSelectedTextColor()
    {
      return m_plainSelectedTextColor != SystemColors.ActiveCaptionText;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public bool PlainAsBlock
    {
      get { return m_plainAsBlock; }

      set
      {
        if (m_plainAsBlock != value)
        {
          m_plainAsBlock = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public bool MultiLine
    {
      get { return m_multiLine; }

      set
      {
        if (m_multiLine != value)
        {
          m_multiLine = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    public Direction Direction
    {
      get { return m_direction; }

      set
      {
        if (m_direction != value)
        {
          m_direction = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    [Category("Behaviour")]
    [DefaultValue(true)]
    public bool RememberExpansion
    {
      get { return m_rememberExpansion; }
      set { m_rememberExpansion = value; }
    }

    [Category("Behaviour")]
    [DefaultValue(true)]
    public bool ExpandAllTogether
    {
      get { return m_expandAllTogether; }
      set { m_expandAllTogether = value; }
    }
        
    [Category("Behaviour")]
    [DefaultValue(true)]
    public bool DeselectReset
    {
      get { return m_deselectReset; }
      set { m_deselectReset = value; }
    }
        
    [Category("Behaviour")]
    [DefaultValue(true)]
    public bool HighlightInfrequent
    {
      get { return m_highlightInfrequent; }
      set { m_highlightInfrequent = value; }
    }

    public override DockStyle Dock
    {
      get { return base.Dock; }

      set
      {
        base.Dock = value;

        switch(value)
        {
          case DockStyle.None:
            m_direction = Direction.Horizontal;
            break;
          case DockStyle.Top:
          case DockStyle.Bottom:
            this.Height = 0;
            m_direction = Direction.Horizontal;
            break;
          case DockStyle.Left:
          case DockStyle.Right:
            this.Width = 0;
            m_direction = Direction.Vertical;
            break;
        }

        Recalculate();
        Invalidate();
      }
    }

    [Category("Animate")]
    [DefaultValue(typeof(Animate), "System")]
    public Animate Animate
    {
      get { return m_animate; }
      set { m_animate = value; }
    }

    [Category("AnimateTime")]
    public int AnimateTime
    {
      get { return m_animateTime; }
      set { m_animateTime = value; }
    }

    [Category("AnimateStyle")]
    public Animation AnimateStyle
    {
      get { return m_animateStyle; }
      set { m_animateStyle = value; }
    }

    [Category("Behaviour")]
    [DefaultValue(null)]
    public Form MdiContainer
    {
      get { return m_mdiContainer; }
			
      set
      {
        if (m_mdiContainer != value)
        {
          if (m_mdiContainer != null)
          {
            // Unsubclass from MdiClient and then remove object reference
            m_clientSubclass.ReleaseHandle();
            m_clientSubclass = null;

            // Remove registered events
            m_mdiContainer.MdiChildActivate -= new EventHandler(OnMdiChildActivate);

            RemovePendantButtons();
          }

          m_mdiContainer = value;

          if (m_mdiContainer != null)
          {
            CreatePendantButtons();

            foreach(Control c in m_mdiContainer.Controls)
            {
              MdiClient client = c as MdiClient;

              if (client != null)
              {
                // We need to subclass the MdiClient to prevent any attempt
                // to change the menu for the container Form. This prevents
                // the system from automatically adding the pendant.
                m_clientSubclass = new MdiClientSubclass();
                m_clientSubclass.AssignHandle(client.Handle);
              }
            }

            // Need to be informed of the active child window
            m_mdiContainer.MdiChildActivate += new EventHandler(OnMdiChildActivate);
          }
        }
      }
    }

    protected void DefineColors()
    {
      // Define starting text colors
      m_defaultTextColor = true;
      m_defaultHighlightTextColor = true;
      m_defaultSelectedTextColor = true;
      m_defaultPlainSelectedTextColor = true;
      m_textColor = SystemColors.MenuText;
      m_highlightTextColor = SystemColors.MenuText;
      m_selectedTextColor = SystemColors.MenuText;
      m_plainSelectedTextColor = SystemColors.ActiveCaptionText;

      // Define starting back colors
      m_defaultBackColor = true;
      m_defaultHighlightBackColor = true;
      m_defaultSelectedBackColor = true;
      base.BackColor = SystemColors.Control;
      m_backBrush = new SolidBrush(base.BackColor);
      m_highlightBackColor = SystemColors.Highlight;            
      DefineHighlightBackColors(SystemColors.Highlight);
      DefineSelectedBackColors(SystemColors.Control);
    }
        
    public void ResetColors()
    {
      this.BackColor = SystemColors.Control;
      this.TextColor = SystemColors.MenuText;
      this.HighlightBackColor = SystemColors.Highlight;            
      this.HighlightTextColor = SystemColors.MenuText;
      this.SelectedBackColor = SystemColors.Control;
      this.SelectedTextColor = SystemColors.MenuText;
    }

    protected void DefineFont(Font newFont)
    {
      base.Font = newFont;

      m_breadthGap = (this.Font.Height / 3) + 1;

      // Calculate the initial height/width of the control
      m_rowWidth = m_rowHeight = this.Font.Height + m_breadthGap * 2 + 1;
    }

    protected void DefineSelectedBackColors(Color baseColor)
    {
      m_selectedBackColor = baseColor;
      m_controlLPen = new Pen(Color.FromArgb(200, baseColor));
      m_controlLBrush = new SolidBrush(Color.FromArgb(200, baseColor));
    }

    protected void DefineHighlightBackColors(Color baseColor)
    {
      m_highlightBackColor = baseColor;
        
      if (m_defaultHighlightBackColor)
      {
        m_highlightBackDark = SystemColors.Highlight;
        m_highlightBackLight = Color.FromArgb(70, m_highlightBackDark);
      }
      else
      {
        m_highlightBackDark = ControlPaint.Dark(baseColor);
        m_highlightBackLight = baseColor;
      }
    }

    public virtual void OnSelected(MenuCommand mc)
    {
      // Any attached event handlers?
      if (Selected != null)
        Selected(mc);
    }

    public virtual void OnDeselected(MenuCommand mc)
    {
      // Any attached event handlers?
      if (Deselected != null)
        Deselected(mc);
    }

    protected void OnCollectionCleared()
    {
      // Reset state ready for a recalculation
      Deselect();
      RemoveItemTracking();

      Recalculate();
      Invalidate();
    }

    protected void OnCollectionInserted(int index, object value)
    {
      MenuCommand mc = value as MenuCommand;

      // We need notification whenever the properties of this command change
      mc.PropertyChanged += new MenuCommand.PropChangeHandler(OnCommandChanged);
				
      // Reset state ready for a recalculation
      Deselect();
      RemoveItemTracking();

      Recalculate();
      Invalidate();
    }

    protected void OnCollectionRemoved(int index, object value)
    {
      // Reset state ready for a recalculation
      Deselect();
      RemoveItemTracking();

      Recalculate();
      Invalidate();
    }

    protected void OnCommandChanged(MenuCommand item, MenuCommand.Property prop)
    {
      Recalculate();
      Invalidate();
    }

    protected void OnMdiChildActivate(object sender, EventArgs e)
    {
      // Unhook from event
      if (m_activeChild != null)
        m_activeChild.SizeChanged -= new EventHandler(OnMdiChildSizeChanged);

      // Remember the currently active child form
      m_activeChild = m_mdiContainer.ActiveMdiChild;

      // Need to know when window becomes maximized
      if (m_activeChild != null)
        m_activeChild.SizeChanged += new EventHandler(OnMdiChildSizeChanged);

      // Might be a change in pendant requirements
      Recalculate();
      Invalidate();
    }

    protected void OnMdiChildSizeChanged(object sender, EventArgs e)
    {
      // Has window changed to become maximized?
      if (m_activeChild.WindowState == FormWindowState.Maximized)
      {
        // Reflect change in menu
        Recalculate();
        Invalidate();
      }
    }

    protected void OnMdiMin(object sender, EventArgs e)
    {
      if (m_activeChild != null)
      {
        m_activeChild.WindowState = FormWindowState.Minimized;

        // Reflect change in menu
        Recalculate();
        Invalidate();
      }
    }

    protected void OnMdiRestore(object sender, EventArgs e)
    {
      if (m_activeChild != null)
      {
        m_activeChild.WindowState = FormWindowState.Normal;

        // Reflect change in menu
        Recalculate();
        Invalidate();
      }
    }

    protected void OnMdiClose(object sender, EventArgs e)
    {
      if (m_activeChild != null)
      {
        m_activeChild.Close();

        // Reflect change in menu
        Recalculate();
        Invalidate();
      }
    }

    protected void CreatePendantButtons()
    {
      // Create the objects
      m_minButton = new InertButton(m_menuImages, m_minIndex);
      m_restoreButton = new InertButton(m_menuImages, m_restoreIndex);
      m_closeButton = new InertButton(m_menuImages, m_closeIndex);

      // Define the constant sizes
      m_minButton.Size = new Size(m_buttonLength, m_buttonLength);
      m_restoreButton.Size = new Size(m_buttonLength, m_buttonLength);
      m_closeButton.Size = new Size(m_buttonLength, m_buttonLength);

      // Default their position so they are not visible
      m_minButton.Location = new Point(-m_buttonLength, -m_buttonLength);
      m_restoreButton.Location = new Point(-m_buttonLength, -m_buttonLength);
      m_closeButton.Location = new Point(-m_buttonLength, -m_buttonLength);

      // Hookup event handlers
      m_minButton.Click += new EventHandler(OnMdiMin);
      m_restoreButton.Click += new EventHandler(OnMdiRestore);
      m_closeButton.Click += new EventHandler(OnMdiClose);

      // Add to display
      this.Controls.AddRange(new Control[]{m_minButton, m_restoreButton, m_closeButton});
    }

    protected void RemovePendantButtons()
    {
      // Unhook event handlers
      m_minButton.Click -= new EventHandler(OnMdiMin);
      m_restoreButton.Click -= new EventHandler(OnMdiRestore);
      m_closeButton.Click -= new EventHandler(OnMdiClose);

      // Remove from display

      // Use helper method to circumvent form Close bug
      ControlHelper.Remove(this.Controls, m_minButton);
      ControlHelper.Remove(this.Controls, m_restoreButton);
      ControlHelper.Remove(this.Controls, m_closeButton);

      // Release resources
      m_minButton.Dispose();
      m_restoreButton.Dispose();
      m_closeButton.Dispose();

      // Remove references
      m_minButton = null;
      m_restoreButton = null;
      m_closeButton = null;
    }
        
    protected override void OnEnabledChanged(EventArgs e)
    {
      base.OnEnabledChanged(e);

      // Have we become disabled?
      if (!this.Enabled)
      {
        // Is an item selected?
        if (m_selected)
        {
          // Is a popupMenu showing?
          if (m_popupMenu != null)
          {
            // Dismiss the submenu
            m_popupMenu.Dismiss();

            // No reference needed
            m_popupMenu = null;
          }

          // Reset state
          Deselect();
          m_drawUpwards = false;

          SimulateReturnFocus();
        }
      }

      // Do not draw any item as being tracked
      RemoveItemTracking();

      // Change in state changes the way items are drawn
      Invalidate();
    }

    internal void OnWM_MOUSEDOWN(Win32.POINT screenPos)
    {
      // Convert the mouse position to screen coordinates
      User32.ScreenToClient(this.Handle, ref screenPos);

      OnProcessMouseDown(screenPos.x, screenPos.y);
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      OnProcessMouseDown(e.X, e.Y);

      base.OnMouseDown(e);
    }
		
    protected void OnProcessMouseDown(int xPos, int yPos)
    {
      Point pos = new Point(xPos, yPos);

      for(int i=0; i<m_drawCommands.Count; i++)
      {
        DrawCommand dc = m_drawCommands[i] as DrawCommand;

        // Find the DrawCommand this is over
        if (dc.SelectRect.Contains(pos) && dc.Enabled)
        {
          // Is an item already selected?
          if (m_selected)
          {
            // Is it this item that is already selected?
            if (m_trackItem == i)
            {
              // Is a popupMenu showing
              if (m_popupMenu != null)
              {
                // Dismiss the submenu
                m_popupMenu.Dismiss();

                // No reference needed
                m_popupMenu = null;
              }
            }
          }
          else
          {
            // Select the tracked item
            m_selected = true;
            m_drawUpwards = false;
								
            // Is there a change in tracking?
            if (m_trackItem != i)
            {
              // Modify the display of the two items 
              m_trackItem = SwitchTrackingItem(m_trackItem, i);
            }
            else
            {
              // Update display to show as selected
              DrawCommand(m_trackItem, true);
            }

            // Is there a submenu to show?
            if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
              User32.PostMessage(this.Handle, WM_OPERATEMENU, 1, 0);
          }

          break;
        }
      }
    }

    protected override void OnMouseUp(MouseEventArgs e)
    {
      // Is an item currently being tracked?
      if (m_trackItem != -1)
      {
        // Is it also selected?
        if (m_selected == true)
        {
          // Is it also showing a submenu
          if (m_popupMenu == null)
          {
            // Deselect the item
            Deselect();
            m_drawUpwards = false;

            DrawCommand(m_trackItem, true);

            SimulateReturnFocus();
          }
        }
      }

      base.OnMouseUp(e);
    }

    internal void OnWM_MOUSEMOVE(Win32.POINT screenPos)
    {
      // Convert the mouse position to screen coordinates
      User32.ScreenToClient(this.Handle, ref screenPos);

      OnProcessMouseMove(screenPos.x, screenPos.y);
    }

    protected override void OnMouseMove(MouseEventArgs e)
    {
      // Sometimes we need to ignore this message
      if (m_ignoreMouseMove)
        m_ignoreMouseMove = false;
      else
        OnProcessMouseMove(e.X, e.Y);

      base.OnMouseMove(e);
    }

    protected void OnProcessMouseMove(int xPos, int yPos)
    {
      // Sometimes we need to ignore this message
      if (m_ignoreMouseMove)
        m_ignoreMouseMove = false;
      else
      {
        // Is the first time we have noticed a mouse movement over our window
        if (!m_mouseOver)
        {
          // Create the structure needed for User32 call
          Win32.TRACKMOUSEEVENTS tme = new Win32.TRACKMOUSEEVENTS();

          // Fill in the structure
          tme.cbSize = 16;									
          tme.dwFlags = (uint)Win32.TrackerEventFlags.TME_LEAVE;
          tme.hWnd = this.Handle;								
          tme.dwHoverTime = 0;								

          // Request that a message gets sent when mouse leaves this window
          User32.TrackMouseEvent(ref tme);

          // Yes, we know the mouse is over window
          m_mouseOver = true;
        }

        Form parentForm = this.FindForm();

        // Only hot track if this Form is active
        if ((parentForm != null) && parentForm.ContainsFocus)
        {
          Point pos = new Point(xPos, yPos);

          int i = 0;

          for(i=0; i<m_drawCommands.Count; i++)
          {
            DrawCommand dc = m_drawCommands[i] as DrawCommand;

            // Find the DrawCommand this is over
            if (dc.SelectRect.Contains(pos) && dc.Enabled)
            {
              // Is there a change in selected item?
              if (m_trackItem != i)
              {
                // We are currently selecting an item
                if (m_selected)
                {
                  if (m_popupMenu != null)
                  {
                    // Note that we are dismissing the submenu but not removing
                    // the selection of items, this flag is tested by the routine
                    // that will return because the submenu has been finished
                    m_dismissTransfer = true;

                    // Dismiss the submenu
                    m_popupMenu.Dismiss();
		
                    // Default to downward drawing
                    m_drawUpwards = false;
                  }

                  // Modify the display of the two items 
                  m_trackItem = SwitchTrackingItem(m_trackItem, i);

                  // Does the newly selected item have a submenu?
                  if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))	
                    User32.PostMessage(this.Handle, WM_OPERATEMENU, 1, 0);
                }
                else
                {
                  // Modify the display of the two items 
                  m_trackItem = SwitchTrackingItem(m_trackItem, i);
                }
              }

              break;
            }
          }

          // If not in selected mode
          if (!m_selected)
          {
            // None of the commands match?
            if (i == m_drawCommands.Count)
            {
              // If we have the focus then do not change the tracked item
              if (!m_manualFocus)
              {
                // Modify the display of the two items 
                m_trackItem = SwitchTrackingItem(m_trackItem, -1);
              }
            }
          }
        }
      }
    }

    protected override void OnMouseLeave(EventArgs e)
    {
      m_mouseOver = false;

      // If we manually grabbed focus then do not switch
      // selection when the mouse leaves the control area
      if (!m_manualFocus)
      {
        if (m_trackItem != -1)
        {
          // If an item is selected then do not change tracking item when the 
          // mouse leaves the control area, as a popup menu might be showing and 
          // so keep the tracking and selection indication visible
          if (m_selected == false)
            m_trackItem = SwitchTrackingItem(m_trackItem, -1);
        }
      }

      base.OnMouseLeave(e);
    }

    protected override void OnResize(EventArgs e)
    {
      Recalculate();

      // Any resize of control should redraw all of it otherwise when you 
      // stretch to the right it will not paint correctly as we have a one
      // pixel gap between text and min button which is not honoured otherwise
      this.Invalidate();

      base.OnResize(e);
    }

    internal void DrawSelectionUpwards()
    {
      // Double check the state is correct for this method to be called
      if ((m_trackItem != -1) && (m_selected))
      {
        // This flag is tested in the DrawCommand method
        m_drawUpwards = true;

        // Force immediate redraw of the item
        DrawCommand(m_trackItem, true);
      }
    }

    protected void Deselect()
    {
      // The next submenu should be animated
      m_animateFirst = true;

      // Remove selection state
      m_selected = false;
            
      // Should expansion items be reset on deselection?
      if (m_deselectReset)
      {
        // Set everything to expanded
        SetAllCommandsExpansion(m_menuCommands, false);
      }
    }

    protected void Recalculate()
    {
      int length;

      if (m_direction == Direction.Horizontal)
        length = this.Width;
      else 
        length = this.Height;

      // Is there space for any commands?
      if (length > 0)
      {
        // Count the number of rows needed
        int rows = 0;

        // Number of items on this row
        int columns = 0;

        // Create a collection of drawing objects
        m_drawCommands = new ArrayList();

        // Minimum length is a gap on either side of the text
        int cellMinLength = m_lengthGap * 2;

        // Each cell is as broad as the whole control
        int cellBreadth = this.Height;
				
        // Accumulate starting position of each cell
        int lengthStart = 0;

        // Allow enough space to draw a chevron
        length -= (cellMinLength + m_chevronLength);

        bool showPendant = ((rows == 0) && (m_activeChild != null));

        // If we are showing on a single line but the active child is not 
        // currently maximized then we can show a menu item in pendant space
        if (showPendant && !m_multiLine && (m_activeChild.WindowState != FormWindowState.Maximized))
          showPendant = false;

        // Pendant positioning information
        int xPos = 0; 
        int yPos = 0;
        int xInc = 0;
        int yInc = 0;

        // First line needs extra space for pendant
        if (showPendant)
        {
          length -= (m_pendantLength + m_pendantOffset + m_shadowGap);

          bool popupStyle = (m_style == VisualStyle.IDE);
          int borderWidth = (m_style == VisualStyle.IDE) ? 1 : 2;

          // Define the correct visual style
          m_minButton.PopupStyle = popupStyle;
          m_restoreButton.PopupStyle = popupStyle;
          m_closeButton.PopupStyle = popupStyle;

          // Define correct border width
          m_minButton.BorderWidth = borderWidth;
          m_restoreButton.BorderWidth = borderWidth;
          m_closeButton.BorderWidth = borderWidth;

          if (m_direction == Direction.Horizontal)
          {
            xPos = this.Width - m_pendantOffset - m_buttonLength;
            yPos = m_pendantOffset;
            xInc = -m_buttonLength;
          }
          else
          {
            xPos = m_pendantOffset;
            yPos = this.Height - m_pendantOffset - m_buttonLength;
            yInc = -m_buttonLength;
          }
        }

        // Assume chevron is not needed by default
        m_chevronStartCommand = null;

        using(Graphics g = this.CreateGraphics())
        {
          // Count the item we are processing
          int index = 0;

          foreach(MenuCommand command in m_menuCommands)
          {
            // Give the command a chance to update its state
            command.OnUpdate(EventArgs.Empty);

            // Ignore items that are marked as hidden
            if (!command.Visible)
              continue;

            int cellLength = 0;

            // Is this a separator?
            if (command.Text == "-")
              cellLength = m_separatorWidth;
            else
            {
              // Calculate the text width of the cell
              SizeF dimension = g.MeasureString(command.Text, this.Font);

              // Always add 1 to ensure that rounding is up and not down
              cellLength = cellMinLength + (int)dimension.Width + 1;
            }

            Rectangle cellRect;

            // Create a new position rectangle
            if (m_direction == Direction.Horizontal)
              cellRect = new Rectangle(lengthStart, m_rowHeight * rows, cellLength, m_rowHeight);
            else
              cellRect = new Rectangle(m_rowWidth * rows, lengthStart, m_rowWidth, cellLength);

            lengthStart += cellLength;
            columns++;

            // If this item is overlapping the control edge and it is not the first
            // item on the line then we should wrap around to the next row.
            if ((lengthStart > length) && (columns > 1))
            {
              if (m_multiLine)
              {
                // Move to next row
                rows++;

                // Reset number of items on this column
                columns = 1;

                // Reset starting position of next item
                lengthStart = cellLength;

                // Reset position of this item
                if (m_direction == Direction.Horizontal)
                {
                  cellRect.X = 0;
                  cellRect.Y += m_rowHeight;
                }
                else
                {
                  cellRect.X += m_rowWidth;
                  cellRect.Y = 0;
                }

                // Only the first line needs extra space for pendant
                if (showPendant && (rows == 1))
                  length += (m_pendantLength + m_pendantOffset);
              }
              else
              {
                // Is a tracked item being make invisible
                if (index <= m_trackItem)
                {
                  // Need to remove tracking of this item
                  RemoveItemTracking();
                }

                // Remember which item is first for the chevron submenu
                m_chevronStartCommand = command;

                if (m_direction == Direction.Horizontal)
                {
                  cellRect.Y = 0;
                  cellRect.Width = cellMinLength + m_chevronLength;
                  cellRect.X = this.Width - cellRect.Width;
                  cellRect.Height = m_rowHeight;
                  xPos -= cellRect.Width;
                }
                else
                {
                  cellRect.X = 0;
                  cellRect.Height = cellMinLength + m_chevronLength;
                  cellRect.Y = this.Height - (cellMinLength + m_chevronLength);
                  cellRect.Width = m_rowWidth;
                  yPos -= cellRect.Height;
                }

                // Create a draw command for this chevron
                m_drawCommands.Add(new DrawCommand(cellRect));

                // Exit, do not add the current item or any afterwards
                break;
              }
            }

            Rectangle selectRect = cellRect;

            // Selection rectangle differs from drawing rectangle with IDE, because pressing the
            // mouse down causes the menu to appear and because the popup menu appears drawn slightly
            // over the drawing area the mouse up might select the first item in the popup. 
            if (m_style == VisualStyle.IDE)
            {
              // Modify depending on orientation
              if (m_direction == Direction.Horizontal)
                selectRect.Height -= (m_lengthGap + 2);
              else
                selectRect.Width -= m_breadthGap;
            }

            // Create a drawing object
            m_drawCommands.Add(new DrawCommand(command, cellRect, selectRect));

            index++;
          }
        }

        // Position the pendant buttons
        if (showPendant)
        {
          if (m_activeChild.WindowState == FormWindowState.Maximized)
          {
            // Window maximzied, must show the buttons
            if (!m_minButton.Visible)
            {
              m_minButton.Show();
              m_restoreButton.Show();
              m_closeButton.Show();
            }
	
            // Only enabled minimize box if child is allowed to be minimized
            m_minButton.Enabled = m_activeChild.MinimizeBox;

            m_closeButton.Location = new Point(xPos, yPos);

            xPos += xInc; yPos += yInc;
            m_restoreButton.Location = new Point(xPos, yPos);

            xPos += xInc; yPos += yInc;
            m_minButton.Location = new Point(xPos, yPos);
          }
          else
          {
            // No window is maximized, so hide the buttons
            if (m_minButton.Visible)
            {
              m_minButton.Hide();
              m_restoreButton.Hide();
              m_closeButton.Hide();
            }
          }
        }
        else
        {
          // No window is maximized, so hide the buttons
          if ((m_minButton != null) && m_minButton.Visible)
          {
            m_minButton.Hide();
            m_restoreButton.Hide();
            m_closeButton.Hide();
          }
        }

        if (m_direction == Direction.Horizontal)
        {
          int controlHeight = (rows + 1) * m_rowHeight;

          // Ensure the control is the correct height
          if (this.Height != controlHeight)
            this.Height = controlHeight;
        }
        else
        {
          int controlWidth = (rows + 1) * m_rowWidth;

          // Ensure the control is the correct width
          if (this.Width != controlWidth)
            this.Width = controlWidth;
        }				
      }
    }

    protected void DrawCommand(int drawItem, bool tracked)
    {
      // Create a graphics object for drawing
      using(Graphics g = this.CreateGraphics())
        DrawSingleCommand(g, m_drawCommands[drawItem] as DrawCommand, tracked);
    }

    internal void DrawSingleCommand(Graphics g, DrawCommand dc, bool tracked)
    {
      Rectangle drawRect = dc.DrawRect;
      MenuCommand mc = dc.MenuCommand;

      // Copy the rectangle used for drawing cell
      Rectangle shadowRect = drawRect;

      // Expand to right and bottom to cover the area used to draw shadows
      shadowRect.Width += m_shadowGap;
      shadowRect.Height += m_shadowGap;

      // Draw background color over cell and shadow area to the right
      g.FillRectangle(m_backBrush, shadowRect);

      if (!dc.Separator)
      {
        Rectangle textRect;

        // Text rectangle size depends on type of draw command we are drawing
        if (dc.Chevron)
        {
          // Create chevron drawing rectangle
          textRect = new Rectangle(drawRect.Left + m_lengthGap, drawRect.Top + m_boxExpandUpper,
            drawRect.Width - m_lengthGap * 2, drawRect.Height - (m_boxExpandUpper * 2));
        }
        else
        {
          // Create text drawing rectangle
          textRect = new Rectangle(drawRect.Left + m_lengthGap, drawRect.Top + m_lengthGap,
            drawRect.Width - m_lengthGap * 2, drawRect.Height - m_lengthGap * 2);
        }

        if (dc.Enabled)
        {
          // Draw selection 
          if (tracked)
          {
            Rectangle boxRect;

            // Create the rectangle for box around the text
            if (m_direction == Direction.Horizontal)
            {
              boxRect = new Rectangle(textRect.Left - m_boxExpandSides,
                textRect.Top - m_boxExpandUpper,
                textRect.Width + m_boxExpandSides * 2,
                textRect.Height + m_boxExpandUpper);
            }
            else
            {					
              if (!dc.Chevron)
              {
                boxRect = new Rectangle(textRect.Left,
                  textRect.Top - m_boxExpandSides,
                  textRect.Width - m_boxExpandSides,
                  textRect.Height + m_boxExpandSides * 2);
              }
              else
                boxRect = textRect;
            }

            switch(m_style)
            {
              case VisualStyle.IDE:
                if (m_selected)
                {
                  // Fill the entire inside
                  g.FillRectangle(Brushes.White, boxRect);
                  g.FillRectangle(m_controlLBrush, boxRect);
								
                  Color extraColor = Color.FromArgb(64, 0, 0, 0);
                  Color darkColor = Color.FromArgb(48, 0, 0, 0);
                  Color lightColor = Color.FromArgb(0, 0, 0, 0);
                
                  int rightLeft = boxRect.Right + 1;
                  int rightBottom = boxRect.Bottom;

                  if (m_drawUpwards && (m_direction == Direction.Horizontal))                                    
                  {
                    // Draw the box around the selection area
                    using(Pen dark = new Pen(ControlPaint.Dark(m_selectedBackColor)))
                      g.DrawRectangle(dark, boxRect);

                    if (dc.SubMenu)
                    {
                      // Right shadow
                      int rightTop = boxRect.Top;
                      int leftLeft = boxRect.Left + m_shadowGap;

                      // Bottom shadow
                      int top = boxRect.Bottom + 1;
                      int left = boxRect.Left + m_shadowGap;
                      int width = boxRect.Width + 1;
                      int height = m_shadowGap;

                      Brush rightShadow;
                      Brush bottomLeftShadow;
                      Brush bottomShadow;
                      Brush bottomRightShadow;

                      // Decide if we need to use an alpha brush
                      if (m_supportsLayered)
                      {
                        // Create brushes
                        rightShadow = new LinearGradientBrush(new Point(rightLeft, 9999),
                          new Point(rightLeft + m_shadowGap, 9999),
                          darkColor, lightColor);

                        bottomLeftShadow = new LinearGradientBrush(new Point(left + m_shadowGap, top - m_shadowGap),
                          new Point(left, top + height),
                          extraColor, lightColor);

                        bottomRightShadow = new LinearGradientBrush(new Point(left + width - m_shadowGap - 2, top - m_shadowGap - 2),
                          new Point(left + width, top + height),
                          extraColor, lightColor);

                        bottomShadow = new LinearGradientBrush(new Point(9999, top),
                          new Point(9999, top + height),
                          darkColor, lightColor);
                      }
                      else
                      {
                        rightShadow = new SolidBrush(SystemColors.ControlDark);
                        bottomLeftShadow = rightShadow;
                        bottomShadow = rightShadow;
                        bottomRightShadow = rightShadow;
                      }

                      // Draw each part of the shadow area
                      g.FillRectangle(rightShadow, new Rectangle(rightLeft, rightTop, m_shadowGap,  rightBottom - rightTop + 1));
                      g.FillRectangle(bottomLeftShadow, left, top, m_shadowGap, height);
                      g.FillRectangle(bottomRightShadow, left + width - m_shadowGap, top, m_shadowGap, height);
                      g.FillRectangle(bottomShadow, left + m_shadowGap, top, width - m_shadowGap * 2, height);

                      // Dispose of brush objects		
                      if (m_supportsLayered)
                      {
                        rightShadow.Dispose();
                        bottomLeftShadow.Dispose();
                        bottomShadow.Dispose();
                        bottomRightShadow.Dispose();
                      }
                      else
                        rightShadow.Dispose();
                    }
                  }
                  else
                  {
                    // Draw the box around the selection area
                    using(Pen dark = new Pen(ControlPaint.Dark(m_selectedBackColor)))
                      g.DrawRectangle(dark, boxRect);

                    if (dc.SubMenu)
                    {
                      if (m_direction == Direction.Horizontal)
                      {
                        // Remove the bottom line of the selection area
                        g.DrawLine(Pens.White, boxRect.Left, boxRect.Bottom, boxRect.Right, boxRect.Bottom);
                        g.DrawLine(m_controlLPen, boxRect.Left, boxRect.Bottom, boxRect.Right, boxRect.Bottom);

                        int rightTop = boxRect.Top + m_shadowYOffset;

                        Brush shadowBrush;

                        // Decide if we need to use an alpha brush
                        if (m_supportsLayered && (m_style == VisualStyle.IDE))
                        {
                          using(LinearGradientBrush topBrush = new LinearGradientBrush(new Point(rightLeft - m_shadowGap, rightTop + m_shadowGap), 
                                  new Point(rightLeft + m_shadowGap, rightTop),
                                  extraColor, lightColor))
                          {
                            g.FillRectangle(topBrush, new Rectangle(rightLeft, rightTop, m_shadowGap, m_shadowGap));
                        
                            rightTop += m_shadowGap;
                          }

                          shadowBrush = new LinearGradientBrush(new Point(rightLeft, 9999), 
                            new Point(rightLeft + m_shadowGap, 9999),
                            darkColor, lightColor);
                        }
                        else
                          shadowBrush = new SolidBrush(SystemColors.ControlDark);

                        g.FillRectangle(shadowBrush, new Rectangle(rightLeft, rightTop, m_shadowGap, rightBottom - rightTop));

                        shadowBrush.Dispose();
                      }
                      else
                      {
                        // Remove the right line of the selection area
                        g.DrawLine(Pens.White, boxRect.Right, boxRect.Top, boxRect.Right, boxRect.Bottom);
                        g.DrawLine(m_controlLPen, boxRect.Right, boxRect.Top, boxRect.Right, boxRect.Bottom);

                        int leftLeft = boxRect.Left + m_shadowYOffset;

                        Brush shadowBrush;

                        // Decide if we need to use an alpha brush
                        if (m_supportsLayered && (m_style == VisualStyle.IDE))
                        {
                          using(LinearGradientBrush topBrush = new LinearGradientBrush(new Point(leftLeft + m_shadowGap, rightBottom + 1 - m_shadowGap), 
                                  new Point(leftLeft, rightBottom + 1 + m_shadowGap),
                                  extraColor, lightColor))
                          {
                            g.FillRectangle(topBrush, new Rectangle(leftLeft, rightBottom + 1, m_shadowGap, m_shadowGap));
                        
                            leftLeft += m_shadowGap;
                          }

                          shadowBrush = new LinearGradientBrush(new Point(9999, rightBottom + 1), 
                            new Point(9999, rightBottom + 1 + m_shadowGap),
                            darkColor, lightColor);
                        }
                        else
                          shadowBrush = new SolidBrush(SystemColors.ControlDark);

                        g.FillRectangle(shadowBrush, new Rectangle(leftLeft, rightBottom + 1, rightBottom - leftLeft - m_shadowGap, m_shadowGap));

                        shadowBrush.Dispose();
                      }
                    }
                  }
                }
                else
                {
                  using (Pen selectPen = new Pen(m_highlightBackDark))
                  {
                    // Draw the selection area in white so can alpha draw over the top
                    g.FillRectangle(Brushes.White, boxRect);

                    using (SolidBrush selectBrush = new SolidBrush(m_highlightBackLight))
                    {
                      // Draw the selection area
                      g.FillRectangle(selectBrush, boxRect);

                      // Draw a border around the selection area
                      g.DrawRectangle(selectPen, boxRect);
                    }
                  }
                }
                break;
              case VisualStyle.Plain:
                if (m_plainAsBlock)
                {
                  using (SolidBrush selectBrush = new SolidBrush(m_highlightBackDark))
                    g.FillRectangle(selectBrush, drawRect);
                }
                else
                {
                  if (m_selected)
                  {
                    using(Pen lighlight = new Pen(ControlPaint.LightLight(this.BackColor)),
                            dark = new Pen(ControlPaint.DarkDark(this.BackColor)))
                    {                                            
                      g.DrawLine(dark, boxRect.Left, boxRect.Bottom, boxRect.Left, boxRect.Top);
                      g.DrawLine(dark, boxRect.Left, boxRect.Top, boxRect.Right, boxRect.Top);
                      g.DrawLine(lighlight, boxRect.Right, boxRect.Top, boxRect.Right, boxRect.Bottom);
                      g.DrawLine(lighlight, boxRect.Right, boxRect.Bottom, boxRect.Left, boxRect.Bottom);
                    }
                  }
                  else
                  {
                    using(Pen lighlight = new Pen(ControlPaint.LightLight(this.BackColor)),
                            dark = new Pen(ControlPaint.DarkDark(this.BackColor)))
                    {
                      g.DrawLine(lighlight, boxRect.Left, boxRect.Bottom, boxRect.Left, boxRect.Top);
                      g.DrawLine(lighlight, boxRect.Left, boxRect.Top, boxRect.Right, boxRect.Top);
                      g.DrawLine(dark, boxRect.Right, boxRect.Top, boxRect.Right, boxRect.Bottom);
                      g.DrawLine(dark, boxRect.Right, boxRect.Bottom, boxRect.Left, boxRect.Bottom);
                    }
                  }
                }
                break;
            }
          }
        }

        if (dc.Chevron)
        {
          // Draw the chevron image in the centre of the text area
          int yPos = drawRect.Top;
          int xPos = drawRect.X + ((drawRect.Width - m_chevronLength) / 2);

          // When selected...
          if (m_selected)
          {
            // ...offset down and to the right
            xPos += 1;
            yPos += 1;
          }

          g.DrawImage(m_menuImages.Images[m_chevronIndex], xPos, yPos);
        }
        else
        {	
          // Left align the text drawing on a single line centered vertically
          // and process the & character to be shown as an underscore on next character
          StringFormat format = new StringFormat();
          format.FormatFlags = StringFormatFlags.NoClip | StringFormatFlags.NoWrap;
          format.Alignment = StringAlignment.Center;
          format.LineAlignment = StringAlignment.Center;
          format.HotkeyPrefix = HotkeyPrefix.Show;

          if (m_direction == Direction.Vertical)
            format.FormatFlags |= StringFormatFlags.DirectionVertical;

          if (dc.Enabled && this.Enabled)
          {
            if (tracked && (m_style == VisualStyle.Plain) && m_plainAsBlock)
            {
              // Is the item selected as well as tracked?
              if (m_selected)
              {
                // Offset to show it is selected
                textRect.X += 2;
                textRect.Y += 2;
              }

              using (SolidBrush textBrush = new SolidBrush(m_plainSelectedTextColor))
                g.DrawString(mc.Text, this.Font, textBrush, textRect, format);
            }
            else
            {
              if (m_selected && tracked)
              {
                using (SolidBrush textBrush = new SolidBrush(m_selectedTextColor))
                  g.DrawString(mc.Text, this.Font, textBrush, textRect, format);
              }
              else
              {
                if (tracked)
                {
                  using (SolidBrush textBrush = new SolidBrush(m_highlightTextColor))
                    g.DrawString(mc.Text, this.Font, textBrush, textRect, format);
                }
                else
                {
                  using (SolidBrush textBrush = new SolidBrush(m_textColor))
                    g.DrawString(mc.Text, this.Font, textBrush, textRect, format);
                }
              }
            }
          }
          else 
          {
            // Helper values used when drawing grayed text in plain style
            Rectangle rectDownRight = textRect;
            rectDownRight.Offset(1,1);

            // Draw the text offset down and right
            g.DrawString(mc.Text, this.Font, Brushes.White, rectDownRight, format);

            // Draw then text offset up and left
            using (SolidBrush grayBrush = new SolidBrush(SystemColors.GrayText))
              g.DrawString(mc.Text, this.Font, grayBrush, textRect, format);
          }
        }
      }
    }

    protected void DrawAllCommands(Graphics g)
    {
      for(int i=0; i<m_drawCommands.Count; i++)
      {
        // Grab some commonly used values				
        DrawCommand dc = m_drawCommands[i] as DrawCommand;

        // Draw this command only
        DrawSingleCommand(g, dc, (i == m_trackItem));
      }
    }

    protected int SwitchTrackingItem(int oldItem, int newItem)
    {
      // Create a graphics object for drawinh
      using(Graphics g = this.CreateGraphics())
      {
        // Deselect the old draw command
        if (oldItem != -1)
        {
          DrawCommand dc = m_drawCommands[oldItem] as DrawCommand;

          // Draw old item not selected
          DrawSingleCommand(g, m_drawCommands[oldItem] as DrawCommand, false);

          // Generate an unselect event
          if (dc.MenuCommand != null)
            OnDeselected(dc.MenuCommand);
        }

        m_trackItem = newItem;

        // Select the new draw command
        if (m_trackItem != -1)
        {
          DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

          // Draw new item selected
          DrawSingleCommand(g, m_drawCommands[m_trackItem] as DrawCommand, true);
					
          // Generate an select event
          if (dc.MenuCommand != null)
            OnSelected(dc.MenuCommand);
        }

        // Force redraw of all items to prevent strange bug where some items
        // never get redrawn correctly and so leave blank spaces when using the
        // mouse/keyboard to shift between popup menus
        DrawAllCommands(g);
      }

      return m_trackItem;
    }

    protected void RemoveItemTracking()
    {
      if (m_trackItem != -1)
      {
        DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

        // Generate an unselect event
        if (dc.MenuCommand != null)
          OnDeselected(dc.MenuCommand);

        // Remove tracking value
        m_trackItem = -1;
      }		
    }

    internal void OperateSubMenu(DrawCommand dc, bool selectFirst, bool trackRemove)
    {
      if (this.IsDisposed)
        return;
                
      Rectangle drawRect = dc.DrawRect;

      // Find screen positions for popup menu
      Point screenPos;
			
      if (m_style == VisualStyle.IDE)
      {
        if (m_direction == Direction.Horizontal)
          screenPos = PointToScreen(new Point(dc.DrawRect.Left + 1, drawRect.Bottom - m_lengthGap - 2));
        else
          screenPos = PointToScreen(new Point(dc.DrawRect.Right - m_breadthGap, drawRect.Top + m_boxExpandSides - 1));
      }
      else
      {
        if (m_direction == Direction.Horizontal)
          screenPos = PointToScreen(new Point(dc.DrawRect.Left + 1, drawRect.Bottom));
        else
          screenPos = PointToScreen(new Point(dc.DrawRect.Right, drawRect.Top));
      }

      Point aboveScreenPos;
			
      if (m_style == VisualStyle.IDE)
      {
        if (m_direction == Direction.Horizontal)
          aboveScreenPos = PointToScreen(new Point(dc.DrawRect.Left + 1, drawRect.Top + m_breadthGap + m_lengthGap - 1));
        else
          aboveScreenPos = PointToScreen(new Point(dc.DrawRect.Right - m_breadthGap, drawRect.Bottom + m_lengthGap + 1));
      }
      else
      {
        if (m_direction == Direction.Horizontal)
          aboveScreenPos = PointToScreen(new Point(dc.DrawRect.Left + 1, drawRect.Top));
        else
          aboveScreenPos = PointToScreen(new Point(dc.DrawRect.Right, drawRect.Bottom));
      }

      int borderGap;

      // Calculate the missing gap in the PopupMenu border
      if (m_direction == Direction.Horizontal)
        borderGap = dc.DrawRect.Width - m_subMenuBorderAdjust;
      else
        borderGap = dc.DrawRect.Height - m_subMenuBorderAdjust;		
	
      m_popupMenu = new PopupMenu();

      // Define the correct visual style based on ours
      m_popupMenu.Style = this.Style;

      // Key direction when keys cause dismissal
      int returnDir = 0;

      // Command selected by the PopupMenu
      MenuCommand returnCommand = null;

      // Should the PopupMenu tell the collection to remember expansion state
      m_popupMenu.RememberExpansion = m_rememberExpansion;

      // Propogate our highlight setting
      m_popupMenu.HighlightInfrequent = m_highlightInfrequent;

      // Might need to define custom colors
      if (!m_defaultSelectedBackColor)
        m_popupMenu.BackColor = m_selectedBackColor;
            
      if (!m_defaultSelectedTextColor)
        m_popupMenu.TextColor = m_selectedTextColor;

      if (!m_defaultHighlightTextColor)
        m_popupMenu.HighlightTextColor = m_highlightTextColor;

      if (!m_defaultHighlightBackColor)
        m_popupMenu.HighlightColor = m_highlightBackColor;

      if (!m_defaultFont)
        m_popupMenu.Font = base.Font;

      // Pass on the animation values
      m_popupMenu.Animate = m_animate;
      m_popupMenu.AnimateStyle = m_animateStyle;
      m_popupMenu.AnimateTime = m_animateTime;
    
      if (dc.Chevron)
      {
        MenuCommandCollection mcc = new MenuCommandCollection();

        bool addCommands = false;

        // Generate a collection of menu commands for those not visible
        foreach(MenuCommand command in m_menuCommands)
        {
          if (!addCommands && (command == m_chevronStartCommand))
            addCommands = true;

          if (addCommands)
            mcc.Add(command);
        }

        // Track the popup using provided menu item collection
        returnCommand = m_popupMenu.TrackPopup(screenPos, 
          aboveScreenPos,
          m_direction,
          mcc, 
          borderGap,
          selectFirst, 
          this,
          m_animateFirst,
          ref returnDir);
      }
      else
      {
        // Generate event so that caller has chance to modify MenuCommand contents
        dc.MenuCommand.OnPopupStart();
                
        // Honour the collections request for showing infrequent items
        m_popupMenu.ShowInfrequent = dc.MenuCommand.MenuCommands.ShowInfrequent;

        // Track the popup using provided menu item collection
        returnCommand = m_popupMenu.TrackPopup(screenPos, 
          aboveScreenPos,
          m_direction,
          dc.MenuCommand.MenuCommands, 
          borderGap,
          selectFirst,
          this,
          m_animateFirst,
          ref returnDir);
      }
            
      // No more animation till simulation ends
      m_animateFirst = false;

      // If we are supposed to expand all items at the same time
      if (m_expandAllTogether)
      {   
        // Is anything we have shown now in the expanded state
        if (AnythingExpanded(m_menuCommands))
        {
          // Set everything to expanded
          SetAllCommandsExpansion(m_menuCommands, true);
        }
      }
            
      // Was arrow key not used to dismiss the submenu?
      if (returnDir == 0)
      {
        // The submenu may have eaten the mouse leave event
        m_mouseOver = false;

        // Only if the submenu was dismissed at the request of the submenu
        // should the selection mode be cancelled, otherwise keep selection mode
        if (!m_dismissTransfer)
        {
          // This item is no longer selected
          Deselect();
          m_drawUpwards = false;

          if (!this.IsDisposed)
          {
            // Should we stop tracking this item
            if (trackRemove)
            {
              // Unselect the current item
              m_trackItem = SwitchTrackingItem(m_trackItem, -1);
            }
            else
            {
              if (m_trackItem != -1)
              {
                // Repaint the item
                DrawCommand(m_trackItem, true);
              }
            }
          }
        }
        else
        {
          // Do not change m_selected status
          m_dismissTransfer = false;
        }
      }

      if (!dc.Chevron)
      {
        // Generate event so that caller has chance to modify MenuCommand contents
        dc.MenuCommand.OnPopupEnd();
      }

      // Spin the message loop so the messages dealing with destroying
      // the PopupMenu window are processed and cause it to disappear from
      // view before events are generated
      Application.DoEvents();

      // Remove unwanted object
      m_popupMenu = null;

      // Was arrow key used to dismiss the submenu?
      if (returnDir != 0)
      {
        if (returnDir < 0)
        {
          // Shift selection left one
          ProcessMoveLeft(true);
        }
        else
        {
          // Shift selection right one
          ProcessMoveRight(true);
        }

        // A WM_MOUSEMOVE is generated when we open up the new submenu for 
        // display, ignore this as it causes the selection to move
        m_ignoreMouseMove = true;
      }
      else
      {
        // Was a MenuCommand returned?
        if (returnCommand != null)
        {
          // Remove 

          // Are we simulating having the focus?
          if (m_manualFocus)
          {
            // Always return focus to original when a selection is made
            SimulateReturnFocus();
          }

          // Pulse the selected event for the command
          returnCommand.OnClick(EventArgs.Empty);
        }
      }
    }

    protected bool AnythingExpanded(MenuCommandCollection mcc)
    {
      foreach(MenuCommand mc in mcc)
      {
        if (mc.MenuCommands.ShowInfrequent)
          return true;
                    
        if (AnythingExpanded(mc.MenuCommands))
          return true;
      }
            
      return false;
    }
        
    protected void SetAllCommandsExpansion(MenuCommandCollection mcc, bool show)
    {
      foreach(MenuCommand mc in mcc)
      {
        // Set the correct value for this collection
        mc.MenuCommands.ShowInfrequent = show;
                    
        // Recurse into all lower level collections
        SetAllCommandsExpansion(mc.MenuCommands, show);
      }
    }

    protected void SimulateGrabFocus()
    {	
      if (!m_manualFocus)
      {
        m_manualFocus = true;
        m_animateFirst = true;

        Form parentForm = this.FindForm();

        // Want notification when user selects a different Form
        parentForm.Deactivate += new EventHandler(OnParentDeactivate);

        // Must hide caret so user thinks focus has changed
        bool hideCaret = User32.HideCaret(IntPtr.Zero);

        // Create an object for storing windows message information
        Win32.MSG msg = new Win32.MSG();

        m_exitLoop = false;

        // Process messages until exit condition recognised
        while(!m_exitLoop)
        {
          // Suspend thread until a windows message has arrived
          if (User32.WaitMessage())
          {
            // Take a peek at the message details without removing from queue
            while(!m_exitLoop && User32.PeekMessage(ref msg, 0, 0, 0, (int)Win32.PeekMessageFlags.PM_NOREMOVE))
            {
              //							Console.WriteLine("Loop {0} {1}", this.Handle, ((Win32.Msgs)msg.message).ToString());
    
              if (User32.GetMessage(ref msg, 0, 0, 0))
              {
                // Should this method be dispatched?
                if (!ProcessInterceptedMessage(ref msg))
                {
                  User32.TranslateMessage(ref msg);
                  User32.DispatchMessage(ref msg);
                }
              }
            }
          }
        }

        // Remove notification when user selects a different Form
        parentForm.Deactivate -= new EventHandler(OnParentDeactivate);

        // If caret was hidden then show it again now
        if (hideCaret)
          User32.ShowCaret(IntPtr.Zero);

        // We lost the focus
        m_manualFocus = false;
      }
    }

    protected void SimulateReturnFocus()
    {
      if (m_manualFocus)
        m_exitLoop = true;

      // Remove any item being tracked
      if (m_trackItem != -1)
      {
        // Unselect the current item
        m_trackItem = SwitchTrackingItem(m_trackItem, -1);
      }
    }

    protected void OnParentDeactivate(object sender, EventArgs e)
    {
      SimulateReturnFocus();
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      DrawAllCommands(e.Graphics);
      base.OnPaint(e);
    }

    protected void ProcessMoveLeft(bool select)
    {
      if (m_popupMenu == null)
      {
        int newItem = m_trackItem;
        int startItem = newItem;

        for(int i=0; i<m_drawCommands.Count; i++)
        {
          // Move to previous item
          newItem--;

          // Have we looped all the way around all choices
          if (newItem == startItem)
            return;

          // Check limits
          if (newItem < 0)
            newItem = m_drawCommands.Count - 1;

          DrawCommand dc = m_drawCommands[newItem] as DrawCommand;

          // Can we select this item?
          if (!dc.Separator && (dc.Chevron || dc.MenuCommand.Enabled))
          {
            // If a change has occurred
            if (newItem != m_trackItem)
            {
              // Modify the display of the two items 
              m_trackItem = SwitchTrackingItem(m_trackItem, newItem);
							
              if (m_selected)
              {
                if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
                  User32.PostMessage(this.Handle, WM_OPERATEMENU, 0, 1);
              }

              break;
            }
          }
        }
      }
    }

    protected void ProcessMoveRight(bool select)
    {
      if (m_popupMenu == null)
      {
        int newItem = m_trackItem;
        int startItem = newItem;

        for(int i=0; i<m_drawCommands.Count; i++)
        {
          // Move to previous item
          newItem++;

          // Check limits
          if (newItem >= m_drawCommands.Count)
            newItem = 0;

          DrawCommand dc = m_drawCommands[newItem] as DrawCommand;

          // Can we select this item?
          if (!dc.Separator && (dc.Chevron || dc.MenuCommand.Enabled))
          {
            // If a change has occurred
            if (newItem != m_trackItem)
            {
              // Modify the display of the two items 
              m_trackItem = SwitchTrackingItem(m_trackItem, newItem);

              if (m_selected)
              {
                if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
                  User32.PostMessage(this.Handle, WM_OPERATEMENU, 0, 1);
              }

              break;
            }
          }
        }
      }
    }

    protected void ProcessEnter()
    {
      if (m_popupMenu == null)
      {
        // Are we tracking an item?
        if (m_trackItem != -1)
        {
          // The item must not already be selected
          if (!m_selected)
          {
            DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

            // Is there a submenu to show?
            if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count >= 0))
            {
              // Select the tracked item
              m_selected = true;
              m_drawUpwards = false;
										
              // Update display to show as selected
              DrawCommand(m_trackItem, true);

              // Show the submenu

              if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
                User32.PostMessage(this.Handle, WM_OPERATEMENU, 0, 1);
            }
            else
            {
              // No, pulse the Click event for the command
              dc.MenuCommand.OnClick(EventArgs.Empty);

              int item = m_trackItem;

              // Not tracking anymore
              RemoveItemTracking();

              // Update display to show as not selected
              DrawCommand(item, false);

              // Finished, so return focus to origin
              SimulateReturnFocus();
            }
          }
          else
          {
            // Must be showing a submenu less item as selected
            DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

            // Pulse the event
            dc.MenuCommand.OnClick(EventArgs.Empty);

            int item = m_trackItem;

            RemoveItemTracking();

            // Not selected anymore
            Deselect();
                        
            // Update display to show as not selected
            DrawCommand(item, false);

            // Finished, so return focus to origin
            SimulateReturnFocus();
          }
        }
      }
    }

    protected void ProcessMoveDown()
    {
      if (m_popupMenu == null)
      {
        // Are we tracking an item?
        if (m_trackItem != -1)
        {
          // The item must not already be selected
          if (!m_selected)
          {
            DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

            // Is there a submenu to show?
            if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count >= 0))
            {
              // Select the tracked item
              m_selected = true;
              m_drawUpwards = false;
										
              // Update display to show as selected
              DrawCommand(m_trackItem, true);

              // Show the submenu
              if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
                User32.PostMessage(this.Handle, WM_OPERATEMENU, 0, 1);
            }
          }
        }
      }
    }

    protected bool ProcessMnemonicKey(char key)
    {
      // No current selection
      if (!m_selected)
      {
        // Search for an item that matches
        for(int i=0; i<m_drawCommands.Count; i++)
        {
          DrawCommand dc = m_drawCommands[i] as DrawCommand;

          // Only interested in enabled items
          if ((dc.MenuCommand != null) && dc.MenuCommand.Enabled && dc.MenuCommand.Visible)
          {
            // Does the character match?
            if (key == dc.Mnemonic)
            {
              // Select the tracked item
              m_selected = true;
              m_drawUpwards = false;
										
              // Is there a change in tracking?
              if (m_trackItem != i)
              {
                // Modify the display of the two items 
                m_trackItem = SwitchTrackingItem(m_trackItem, i);
              }
              else
              {
                // Update display to show as selected
                DrawCommand(m_trackItem, true);
              }

              // Is there a submenu to show?
              if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count >= 0))
              {
                if (dc.Chevron || (dc.MenuCommand.MenuCommands.Count > 0))
                  User32.PostMessage(this.Handle, WM_OPERATEMENU, 0, 1);

                return true;
              }
              else
              {			
                // No, pulse the Click event for the command
                dc.MenuCommand.OnClick(EventArgs.Empty);
	
                int item = m_trackItem;

                RemoveItemTracking();

                // No longer seleted
                Deselect();
                                
                // Update display to show as not selected
                DrawCommand(item, false);

                // Finished, so return focus to origin
                SimulateReturnFocus();

                return false;
              }
            }
          }
        }
      }

      return false;
    }

    protected void OnPreferenceChanged(object sender, UserPreferenceChangedEventArgs e)
    {
      // Are we using the default menu or a user defined value?
      if (m_defaultFont)
      {
        DefineFont(SystemInformation.MenuFont);

        Recalculate();
        Invalidate();
      }
    }

    protected override void OnSystemColorsChanged(EventArgs e)
    {
      if (m_defaultBackColor)
        this.BackColor = SystemColors.Control;
			
      if (m_defaultHighlightBackColor)
        this.HighlightBackColor = SystemColors.Highlight;

      if (m_defaultSelectedBackColor)
        this.SelectedBackColor = SystemColors.Control;
            
      if (m_defaultTextColor)
        m_textColor = SystemColors.MenuText;

      if (m_defaultHighlightTextColor)
        m_highlightTextColor = SystemColors.MenuText;

      if (m_defaultSelectedTextColor)
        m_selectedTextColor = SystemColors.MenuText;
            
      Recalculate();
      Invalidate();
    
      base.OnSystemColorsChanged(e);
    }
        
    public bool PreFilterMessage(ref Message msg)
    {
      Form parentForm = this.FindForm();

      // Only interested if the Form we are on is activate (i.e. contains focus)
      if ((parentForm != null) && (parentForm == Form.ActiveForm) && parentForm.ContainsFocus)
      {		
        switch(msg.Msg)
        {
          case (int)Win32.Msgs.WM_KEYDOWN:
            // Ignore keyboard input if the control is disabled
            if (this.Enabled)
            {
              // Find up/down state of shift and control keys
              ushort shiftKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_SHIFT);
              ushort controlKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_CONTROL);

              // Basic code we are looking for is the key pressed...
              int code = (int)msg.WParam;

              // ...plus the modifier for SHIFT...
              if (((int)shiftKey & 0x00008000) != 0)
                code += 0x00010000;

              // ...plus the modifier for CONTROL
              if (((int)controlKey & 0x00008000) != 0)
                code += 0x00020000;

              // Construct shortcut from keystate and keychar
              Shortcut sc = (Shortcut)(code);

              // Search for a matching command
              return GenerateShortcut(sc, m_menuCommands);
            }
            break;
          case (int)Win32.Msgs.WM_SYSKEYUP:
            // Ignore keyboard input if the control is disabled
            if (this.Enabled)
            {
              if ((int)msg.WParam == (int)Win32.VirtualKeys.VK_MENU)
              {
                // Are there any menu commands?
                if (m_drawCommands.Count > 0)
                {
                  // If no item is currently tracked then...
                  if (m_trackItem == -1)
                  {
                    // ...start tracking the first valid command
                    for(int i=0; i<m_drawCommands.Count; i++)
                    {
                      DrawCommand dc = m_drawCommands[i] as DrawCommand;
											
                      if (!dc.Separator && (dc.Chevron || dc.MenuCommand.Enabled))
                      {
                        m_trackItem = SwitchTrackingItem(-1, i);
                        break;
                      }
                    }
                  }
											
                  // Grab the focus for key events						
                  SimulateGrabFocus();							
                }

                return true;
              }
            }
            break;
          case (int)Win32.Msgs.WM_SYSKEYDOWN:
            // Ignore keyboard input if the control is disabled
            if (this.Enabled)
            {
              if ((int)msg.WParam != (int)Win32.VirtualKeys.VK_MENU)
              {
                // Construct shortcut from ALT + keychar
                Shortcut sc = (Shortcut)(0x00040000 + (int)msg.WParam);
		
                if (GenerateShortcut(sc, m_menuCommands))
                  return true;
								
                // Last resort is treat as a potential mnemonic
                if (ProcessMnemonicKey((char)msg.WParam))
                {
                  if (!m_manualFocus)
                    SimulateGrabFocus();

                  return true;
                }
              }
            }
            break;
          default:
            break;
        }
      }

      return false;
    }

    protected bool ProcessInterceptedMessage(ref Win32.MSG msg)
    {
      bool eat = false;
        
      switch(msg.message)
      {
        case (int)Win32.Msgs.WM_LBUTTONDOWN:
        case (int)Win32.Msgs.WM_MBUTTONDOWN:
        case (int)Win32.Msgs.WM_RBUTTONDOWN:
        case (int)Win32.Msgs.WM_XBUTTONDOWN:
        case (int)Win32.Msgs.WM_NCLBUTTONDOWN:
        case (int)Win32.Msgs.WM_NCMBUTTONDOWN:
        case (int)Win32.Msgs.WM_NCRBUTTONDOWN:
          // Mouse clicks cause the end of simulated focus unless they are
          // inside the client area of the menu control itself
          Point pt = new Point( (int)((uint)msg.lParam & 0x0000FFFFU), 
            (int)(((uint)msg.lParam & 0xFFFF0000U) >> 16));
				
          if (!this.ClientRectangle.Contains(pt))	
            SimulateReturnFocus();
          break;
        case (int)Win32.Msgs.WM_KEYDOWN:
          // Find up/down state of shift and control keys
          ushort shiftKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_SHIFT);
          ushort controlKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_CONTROL);

          // Basic code we are looking for is the key pressed...
          int basecode = (int)msg.wParam;
          int code = basecode;

          // ...plus the modifier for SHIFT...
          if (((int)shiftKey & 0x00008000) != 0)
            code += 0x00010000;

          // ...plus the modifier for CONTROL
          if (((int)controlKey & 0x00008000) != 0)
            code += 0x00020000;

          if (code == (int)Win32.VirtualKeys.VK_ESCAPE)
          {
            // Is an item being tracked
            if (m_trackItem != -1)
            {
              // Is it also showing a submenu
              if (m_popupMenu == null)
              {
                // Unselect the current item
                m_trackItem = SwitchTrackingItem(m_trackItem, -1);

              }
            }

            SimulateReturnFocus();

            // Prevent intended destination getting message
            eat = true;
          }
          else if (code == (int)Win32.VirtualKeys.VK_LEFT)
          {
            if (m_direction == Direction.Horizontal)
              ProcessMoveLeft(false);

            if (m_selected)
              m_ignoreMouseMove = true;

            // Prevent intended destination getting message
            eat = true;
          }
          else if (code == (int)Win32.VirtualKeys.VK_RIGHT)
          {
            if (m_direction == Direction.Horizontal)
              ProcessMoveRight(false);
            else
              ProcessMoveDown();

            if (m_selected)
              m_ignoreMouseMove = true;

            // Prevent intended destination getting message
            eat = true;
          }
          else if (code == (int)Win32.VirtualKeys.VK_RETURN)
          {
            ProcessEnter();

            // Prevent intended destination getting message
            eat = true;
          }
          else if (code == (int)Win32.VirtualKeys.VK_DOWN)
          {
            if (m_direction == Direction.Horizontal)
              ProcessMoveDown();
            else
              ProcessMoveRight(false);

            // Prevent intended destination getting message
            eat = true;
          }
          else if (code == (int)Win32.VirtualKeys.VK_UP)
          {
            ProcessMoveLeft(false);

            // Prevent intended destination getting message
            eat = true;
          }
          else
          {
            // Construct shortcut from keystate and keychar
            Shortcut sc = (Shortcut)(code);

            // Search for a matching command
            if (!GenerateShortcut(sc, m_menuCommands))
            {
              // Last resort is treat as a potential mnemonic
              ProcessMnemonicKey((char)msg.wParam);

              if (m_selected)
                m_ignoreMouseMove = true;
            }
            else
            {
              SimulateReturnFocus();
            }

            // Always eat keyboard message in simulated focus
            eat = true;
          }
          break;
        case (int)Win32.Msgs.WM_KEYUP:
          eat = true;
          break;
        case (int)Win32.Msgs.WM_SYSKEYUP:
          // Ignore keyboard input if the control is disabled
          if ((int)msg.wParam == (int)Win32.VirtualKeys.VK_MENU)
          {
            if (m_trackItem != -1)
            {
              // Is it also showing a submenu
              if (m_popupMenu == null)
              {
                // Unselect the current item
                m_trackItem = SwitchTrackingItem(m_trackItem, -1);

              }
            }

            SimulateReturnFocus();

            // Always eat keyboard message in simulated focus
            eat = true;
          }
          break;
        case (int)Win32.Msgs.WM_SYSKEYDOWN:
          if ((int)msg.wParam != (int)Win32.VirtualKeys.VK_MENU)
          {
            // Construct shortcut from ALT + keychar
            Shortcut sc = (Shortcut)(0x00040000 + (int)msg.wParam);

            // Search for a matching command
            if (!GenerateShortcut(sc, m_menuCommands))
            {
              // Last resort is treat as a potential mnemonic
              ProcessMnemonicKey((char)msg.wParam);

              if (m_selected)
                m_ignoreMouseMove = true;
            }
            else
            {
              SimulateReturnFocus();
            }

            // Always eat keyboard message in simulated focus
            eat = true;
          }
          break;
        default:
          break;
      }

      return eat;
    }

    protected bool GenerateShortcut(Shortcut sc, MenuCommandCollection mcc)
    {
      foreach(MenuCommand mc in mcc)
      {
        // Does the command match?
        if (mc.Enabled && (mc.Shortcut == sc))
        {
          // Generate event for command
          mc.OnClick(EventArgs.Empty);

          return true;
        }
        else
        {
          // Any child items to test?
          if (mc.MenuCommands.Count > 0)
          {
            // Recursive descent of all collections
            if (GenerateShortcut(sc, mc.MenuCommands))
              return true;
          }
        }
      }

      return false;
    }

    protected void OnWM_OPERATEMENU(ref Message m)
    {
      // Is there a valid item being tracted?
      if (m_trackItem != -1)
      {
        DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

        OperateSubMenu(dc, (m.LParam != IntPtr.Zero), (m.WParam != IntPtr.Zero));
      }
    }

    protected void OnWM_GETDLGCODE(ref Message m)
    {
      // We want to the Form to provide all keyboard input to us
      m.Result = (IntPtr)Win32.DialogCodes.DLGC_WANTALLKEYS;
    }

    protected override void WndProc(ref Message m)
    {
      // WM_OPERATEMENU is not a constant and so cannot be in a switch
      if (m.Msg == WM_OPERATEMENU)
        OnWM_OPERATEMENU(ref m);
      else
      {
        switch(m.Msg)
        {
          case (int)Win32.Msgs.WM_GETDLGCODE:
            OnWM_GETDLGCODE(ref m);
            return;
        }
      }

      base.WndProc(ref m);
    }
  }
}

#region revision history

// $Log: MenuControl.cs,v $
// Revision 1.1  2005/10/11 14:40:23  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Menus/MenuControl.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
// $Id: MenuControl.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $

#endregion

