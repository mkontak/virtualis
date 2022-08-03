// $Id: WindowDetailCaption.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using System.ComponentModel;
using System.Drawing.Imaging;
using Microsoft.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class WindowDetailCaption : WindowDetail, IMessageFilter
  {
    // Class fields
    protected static ImageList m_images;

    // Instance events
    public event EventHandler Close;
    public event EventHandler Restore;
    public event EventHandler InvertAutoHide;
    public event ContextHandler Context;

    // Instance fields
    protected InertButton m_maxButton;
    protected InertButton m_closeButton;
    protected InertButton m_hideButton;
    protected RedockerContent m_redocker;
    protected IZoneMaximizeWindow m_maxInterface;
    protected bool m_showCloseButton;
    protected bool m_showHideButton;
    protected bool m_ignoreHideButton;
    protected bool m_pinnedImage;

    // Class fields
    protected static ImageAttributes m_activeAttr = new ImageAttributes();
    protected static ImageAttributes m_inactiveAttr = new ImageAttributes();

    public WindowDetailCaption(DockingManager manager, 
      Size fixedSize, 
      EventHandler closeHandler, 
      EventHandler restoreHandler, 
      EventHandler invertAutoHideHandler, 
      ContextHandler contextHandler)
      : base(manager)
    {
      // Setup correct color remapping depending on initial colors
      DefineButtonRemapping();

      // Default state
      m_maxButton = null;
      m_hideButton = null;
      m_maxInterface = null;
      m_redocker = null;
      m_showCloseButton = true;
      m_showHideButton = true;
      m_ignoreHideButton = false;
      m_pinnedImage = false;
            
      // Prevent flicker with double buffering and all painting inside WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);

      // Our size is always fixed at the required length in both directions
      // as one of the sizes will be provided for us because of our docking
      this.Size = fixedSize;

      if (closeHandler != null)
        this.Close += closeHandler;	

      if (restoreHandler != null)
        this.Restore += restoreHandler;	

      if (invertAutoHideHandler != null)
        this.InvertAutoHide += invertAutoHideHandler;
    
      if (contextHandler != null)
        this.Context += contextHandler;	

      // Let derived classes override the button creation
      CreateButtons();

      // Need to hook into message pump so that the ESCAPE key can be 
      // intercepted when in redocking mode
      Application.AddMessageFilter(this);
    }

    public override Zone ParentZone
    {
      set
      {
        base.ParentZone = value;

        RecalculateMaximizeButton();
        RecalculateButtons();
      }
    }

    public virtual void OnClose()
    {
      // Any attached event handlers?
      if (Close != null)
        Close(this, EventArgs.Empty);
    }

    public virtual void OnInvertAutoHide()
    {
      // Any attached event handlers?
      if (InvertAutoHide != null)
        InvertAutoHide(this, EventArgs.Empty);
    }
        
    public virtual void OnRestore()
    {
      // Any attached event handlers?
      if (Restore != null)
        Restore(this, EventArgs.Empty);
    }

    public virtual void OnContext(Point screenPos)
    {
      // Any attached event handlers?
      if (Context != null)
        Context(screenPos);
    }

    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if (m_closeButton != null)
        {
          m_closeButton.Click -= new EventHandler(OnButtonClose);
          m_closeButton.GotFocus -= new EventHandler(OnButtonGotFocus);
        }

        if (m_hideButton != null)
        {
          m_hideButton.Click -= new EventHandler(OnButtonHide);
          m_hideButton.GotFocus -= new EventHandler(OnButtonGotFocus);
        }
                
        if (m_maxButton != null)
        {
          m_maxButton.Click -= new EventHandler(OnButtonMax);
          m_maxButton.GotFocus -= new EventHandler(OnButtonGotFocus);
        }
      }
      base.Dispose( disposing );
    }

    public override void NotifyAutoHideImage(bool autoHidden)
    {
      m_pinnedImage = autoHidden;
      UpdateAutoHideImage();
    }

    public override void NotifyCloseButton(bool show)
    {
      m_showCloseButton = show;
      RecalculateButtons();
    }

    public override void NotifyHideButton(bool show)
    {
      // Ignore the AutoHide feature when in floating form
      m_ignoreHideButton = (m_parentWindow.State == State.Floating);
            
      m_showHideButton = show;
      RecalculateButtons();
    }

    public override void NotifyShowCaptionBar(bool show)
    {
      this.Visible = show;
    }
        
    protected void RecalculateMaximizeButton()
    {
      // Are we inside a Zone?
      if (this.ParentZone != null)
      {
        // Does the Zone support the maximizing of a Window?
        IZoneMaximizeWindow zmw = this.ParentZone as IZoneMaximizeWindow;

        if (zmw != null)
        {
          AddMaximizeInterface(zmw);
          return;
        }
      }

      RemoveMaximizeInterface();
    }

    protected void AddMaximizeInterface(IZoneMaximizeWindow zmw)
    {
      // Has the maximize button already been created?
      if (m_maxInterface == null)
      {
        // Create the InertButton
        m_maxButton = new InertButton(m_images, 0);

        // Hook into button events
        m_maxButton.Click += new EventHandler(OnButtonMax);
        m_maxButton.GotFocus += new EventHandler(OnButtonGotFocus);

        // Define the default remapping
        m_maxButton.ImageAttributes = m_inactiveAttr;

        OnAddMaximizeInterface();

        Controls.Add(m_maxButton);

        // Remember the interface reference
        m_maxInterface = zmw;

        // Hook into the interface change events
        m_maxInterface.RefreshMaximize += new EventHandler(OnRefreshMaximize);

        RecalculateButtons();
      }
    }

    protected void RemoveMaximizeInterface()
    {
      if (m_maxInterface != null)
      {
        // Unhook from the interface change events
        m_maxInterface.RefreshMaximize -= new EventHandler(OnRefreshMaximize);

        // Remove the interface reference
        m_maxInterface = null;

        // Use helper method to circumvent form Close bug
        ControlHelper.Remove(this.Controls, m_maxButton);

        OnRemoveMaximizeInterface();

        // Unhook into button events
        m_maxButton.Click -= new EventHandler(OnButtonMax);
        m_maxButton.GotFocus -= new EventHandler(OnButtonGotFocus);

        // Kill the button which is no longer needed
        m_maxButton.Dispose();
        m_maxButton = null;

        RecalculateButtons();
      }
    }

    protected void OnRefreshMaximize(object sender, EventArgs e)
    {
      UpdateMaximizeImage();
    }
	
    protected void OnButtonMax(object sender, EventArgs e)
    {
      if (this.ParentWindow != null)
      {
        if (m_maxInterface.IsMaximizeAvailable())
        {
          // Are we already maximized?
          if (m_maxInterface.IsWindowMaximized(this.ParentWindow))
            m_maxInterface.RestoreWindow();
          else
            m_maxInterface.MaximizeWindow(this.ParentWindow);
        }
      }			
    }

    protected void OnButtonClose(Object sender, EventArgs e)
    {
      if (m_showCloseButton)
        OnClose();
    }

    protected void OnButtonHide(Object sender, EventArgs e)
    {
      // Plain button can still be pressed when disabled, so double check 
      // that an event should actually be generated
      if (m_showHideButton && !m_ignoreHideButton)
        OnInvertAutoHide();
    }

    protected void OnButtonGotFocus(Object sender, EventArgs e)
    {
      // Inform parent window we have now got the focus
      if (this.ParentWindow != null)
        this.ParentWindow.WindowDetailGotFocus(this);
    }

    protected override void OnDoubleClick(EventArgs e)
    {
      // The double click event will cause the control to be destroyed as 
      // the Contents are restored to their alternative positions, so need to
      // double check the control is not already dead
      if (!IsDisposed)
      {
        // Are we currently in a redocking state?
        if (m_redocker != null)
        {
          // No longer need the object
          m_redocker = null;
        }
      }

      // Fire attached event handlers
      OnRestore();
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      // The double click event will cause the control to be destroyed as 
      // the Contents are restored to their alternative positions, so need to
      // double check the control is not already dead
      if (!IsDisposed)
      {
        // Left mouse down begins a redocking action
        if (e.Button == MouseButtons.Left)
        {
          if (this.ParentWindow.RedockAllowed)
          {
            WindowContent wc = this.ParentWindow as WindowContent;

            // Is our parent a WindowContent instance?				
            if (wc != null)
            {
              // Start redocking activity for the whole WindowContent
              m_redocker = new RedockerContent(this, wc, new Point(e.X, e.Y));
            }
          }
        }

        this.Focus();
      }
            
      base.OnMouseDown(e);
    }

    protected override void OnMouseMove(MouseEventArgs e)
    {
      // The double click event will cause the control to be destroyed as 
      // the Contents are restored to their alternative positions, so need to
      // double check the control is not already dead
      if (!IsDisposed)
      {
        // Redocker object needs mouse movements
        if (m_redocker != null)
          m_redocker.OnMouseMove(e);
      }

      base.OnMouseMove(e);
    }

    protected override void OnMouseUp(MouseEventArgs e)
    {
      // The double click event will cause the control to be destroyed as 
      // the Contents are restored to their alternative positions, so need to
      // double check the control is not already dead
      if (!IsDisposed)
      {
        // Are we currently in a redocking state?
        if (m_redocker != null)
        {
          // Let the redocker finish off
          m_redocker.OnMouseUp(e);

          // No longer need the object
          m_redocker = null;
        }

        // Right mouse button can generate a Context event
        if (e.Button == MouseButtons.Right)
        {
          // Get screen coordinates of the mouse
          Point pt = this.PointToScreen(new Point(e.X, e.Y));
    				
          // Box to transfer as parameter
          OnContext(pt);
        }
      }
            
      base.OnMouseUp(e);
    }

    protected override void OnResize(EventArgs e)
    {
      // Any resize of control should redraw all of it
      Invalidate();
      base.OnResize(e);
    }

    protected virtual void DefineButtonRemapping() {}
    protected virtual void OnAddMaximizeInterface() {}
    protected virtual void OnRemoveMaximizeInterface() {}
    protected virtual void UpdateMaximizeImage() {}
    protected virtual void UpdateAutoHideImage() {}
    protected virtual void RecalculateButtons() {}
        
    protected virtual void CreateButtons() 
    {
      // Attach events to button
      if (m_closeButton != null)
      {
        m_closeButton.Click += new EventHandler(OnButtonClose);
        m_closeButton.GotFocus += new EventHandler(OnButtonGotFocus);
      }

      if (m_hideButton != null)
      {
        m_hideButton.Click += new EventHandler(OnButtonHide);
        m_hideButton.GotFocus += new EventHandler(OnButtonGotFocus);
      }
    }

    public bool PreFilterMessage(ref Message m)
    {
      // Has a key been pressed?
      if (m.Msg == (int)Win32.Msgs.WM_KEYDOWN)
      {
        // Is it the ESCAPE key?
        if ((int)m.WParam == (int)Win32.VirtualKeys.VK_ESCAPE)
        {                   
          // Are we in redocking mode?
          if (m_redocker != null)
          {
            // Cancel the redocking activity
            m_redocker.QuitTrackingMode(null);

            // No longer need the object
            m_redocker = null;
                        
            return true;
          }
        }
      }
            
      return false;
    }
  }

  [ToolboxItem(false)]
  public class WindowDetailCaptionPlain : WindowDetailCaption
  {
    protected enum ImageIndex
    {
      Close					= 0,
      EnabledHorizontalMax	= 1,
      EnabledHorizontalMin	= 2,
      EnabledVerticalMax		= 3,
      EnabledVerticalMin		= 4, 
      AutoHide		        = 5, 
      AutoShow		        = 6 
    }

    // Class constants
    protected const int m_inset = 3;
    protected const int m_offset = 5;
    protected const int m_fixedLength = 14;
    protected const int m_imageWidth = 10;
    protected const int m_imageHeight = 10;
    protected const int m_buttonWidth = 12;
    protected const int m_buttonHeight = 12;
    protected const int m_insetButton = 2;

    // Instance fields
    protected bool m_dockLeft;
    protected int m_buttonOffset;

    static WindowDetailCaptionPlain()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_images = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.WindowDetailCaptionPlain"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesCaptionPlain.bmp",
        new Size(m_imageWidth, m_imageHeight),
        new Point(0,0));
    }

    public WindowDetailCaptionPlain(DockingManager manager, 
      EventHandler closeHandler, 
      EventHandler restoreHandler, 
      EventHandler invertAutoHideHandler, 
      ContextHandler contextHandler)
      : base(manager, 
      new Size(m_fixedLength, m_fixedLength), 
      closeHandler, 
      restoreHandler, 
      invertAutoHideHandler, 
      contextHandler)
    {
      // Default to thinking we are docked on a left edge
      m_dockLeft = true;

      // Modify painting to prevent overwriting the button control
      m_buttonOffset = 1 + (m_buttonWidth + m_insetButton) * 2;
    }
        
    public override void ParentStateChanged(State newState)
    {
      // Should we dock to the left or top of our container?
      switch(newState)
      {
        case State.DockTop:
        case State.DockBottom:
          m_dockLeft = true;
          break;
        case State.Floating:
        case State.DockLeft:
        case State.DockRight:
          m_dockLeft = false;
          break;
      }

      // Ignore the AutoHide feature when in floating form
      m_ignoreHideButton = (m_parentWindow.State == State.Floating);

      RecalculateButtons();
    }

    public override void RemovedFromParent(Window parent)
    {
      if (parent != null)
      {
        if (this.Dock != DockStyle.None)
        {
          Size minSize = parent.MinimalSize;

          if (this.Dock == DockStyle.Left)
          {
            // Remove our width from the minimum size of the parent
            minSize.Width -= m_fixedLength;
          }
          else
          {
            // Remove our height from the minimum size of the parent
            minSize.Height -= m_fixedLength;
          }

          parent.MinimalSize = minSize;
        }
      }
    }

    public override void AddedToParent(Window parent)
    {
      if (parent != null)
      {
        if (this.Dock != DockStyle.None)
        {
          Size minSize = parent.MinimalSize;

          if (this.Dock == DockStyle.Left)
          {
            // Add our width from the minimum size of the parent
            minSize.Width += m_fixedLength;
          }
          else
          {
            // Add our height from the minimum size of the parent
            minSize.Height += m_fixedLength;
          }

          parent.MinimalSize = minSize;
        }
      }
    }
        
    protected override void DefineButtonRemapping()
    {
      // Define use of current system colors
      ColorMap activeMap = new ColorMap();
      ColorMap inactiveMap = new ColorMap();
			
      activeMap.OldColor = Color.Black;
      activeMap.NewColor = m_manager.InactiveTextColor;
      inactiveMap.OldColor = Color.Black;
      inactiveMap.NewColor = Color.FromArgb(128, m_manager.InactiveTextColor);

      // Create remap attributes for use by button
      m_activeAttr.SetRemapTable(new ColorMap[]{activeMap}, ColorAdjustType.Bitmap);
      m_inactiveAttr.SetRemapTable(new ColorMap[]{inactiveMap}, ColorAdjustType.Bitmap);
    }

    protected override void OnAddMaximizeInterface()
    {
      if (m_maxButton != null)
      {
        m_maxButton.Size = new Size(m_buttonWidth, m_buttonHeight);

        // Shows border all the time and not just when mouse is over control
        m_maxButton.PopupStyle = false;

        // Define the fixed remapping
        m_maxButton.ImageAttributes = m_inactiveAttr;
                
        // Reduce the lines drawing
        m_buttonOffset += (m_buttonWidth + m_insetButton);
      }
    }

    protected override void OnRemoveMaximizeInterface()
    {
      // Reduce the lines drawing
      m_buttonOffset -= (m_buttonWidth + m_insetButton);
    }

    protected override void CreateButtons()
    {
      // Define the ImageList and which ImageIndex to show initially
      m_closeButton = new InertButton(m_images, (int)ImageIndex.Close);
      m_hideButton = new InertButton(m_images, (int)ImageIndex.AutoHide);
			
      m_closeButton.Size = new Size(m_buttonWidth, m_buttonHeight);
      m_hideButton.Size = new Size(m_buttonWidth, m_buttonHeight);

      // Shows border all the time and not just when mouse is over control
      m_closeButton.PopupStyle = false;
      m_hideButton.PopupStyle = false;

      // Define the fixed remapping
      m_closeButton.ImageAttributes = m_activeAttr;
      m_hideButton.ImageAttributes = m_activeAttr;

      // Add to the display
      Controls.Add(m_closeButton);
      Controls.Add(m_hideButton);
            
      // Let base class perform common actions
      base.CreateButtons();
    }

    protected override void UpdateAutoHideImage()
    {
      if (m_pinnedImage)
        m_hideButton.ImageIndexEnabled = (int)ImageIndex.AutoShow;
      else
        m_hideButton.ImageIndexEnabled = (int)ImageIndex.AutoHide;
    }
        
    protected override void UpdateMaximizeImage()
    {
      if (m_showCloseButton)
        m_closeButton.ImageAttributes = m_activeAttr;
      else    
        m_closeButton.ImageAttributes = m_inactiveAttr;

      if (m_showHideButton && !m_ignoreHideButton)
        m_hideButton.ImageAttributes = m_activeAttr;
      else
        m_hideButton.ImageAttributes = m_inactiveAttr;

      if (m_maxButton != null)
      {
        if (m_maxInterface.IsMaximizeAvailable())
          m_maxButton.ImageAttributes = m_activeAttr;
        else
          m_maxButton.ImageAttributes = m_inactiveAttr;

        bool maximized = m_maxInterface.IsWindowMaximized(this.ParentWindow);

        if (m_maxInterface.Direction == Direction.Vertical)
        {
          if (maximized)
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledVerticalMin;	
          else
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledVerticalMax;	
        }
        else
        {
          if (maximized)
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledHorizontalMin;	
          else
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledHorizontalMax;	
        }
      }
    }

    protected override void RecalculateButtons()
    {
      if (m_dockLeft)
      {
        if (this.Dock != DockStyle.Left)
        {
          RemovedFromParent(this.ParentWindow);
          this.Dock = DockStyle.Left;
          AddedToParent(this.ParentWindow);
        }

        int iStart = m_inset;

        // Button position is fixed, regardless of our size
        m_closeButton.Location = new Point(m_insetButton, iStart);
        m_closeButton.Anchor = AnchorStyles.Top;
        m_closeButton.Show();
        iStart += m_buttonHeight + m_insetButton;
                
        // Button position is fixed, regardless of our size
        m_hideButton.Location = new Point(m_insetButton, iStart);
        m_hideButton.Anchor = AnchorStyles.Top;
        m_hideButton.Show();
        iStart += m_buttonHeight + m_insetButton;

        if (m_maxButton != null)
        {
          // Button position is fixed, regardless of our size
          m_maxButton.Location = new Point(m_insetButton, iStart);
          m_maxButton.Anchor = AnchorStyles.Top;
        }
      }
      else
      {
        if (this.Dock != DockStyle.Top)
        {
          RemovedFromParent(this.ParentWindow);
          this.Dock = DockStyle.Top;
          AddedToParent(this.ParentWindow);
        }

        Size client = this.ClientSize;
        int iStart = m_inset;

        // Button is positioned to right hand side of bar
        m_closeButton.Location = new Point(client.Width - iStart - m_buttonWidth, m_insetButton);
        m_closeButton.Anchor = AnchorStyles.Right;
        m_closeButton.Show();
        iStart += m_buttonWidth + m_insetButton;
                
        // Next the AutoHide button is positioned
        m_hideButton.Location = new Point(client.Width - iStart - m_buttonWidth, m_insetButton);
        m_hideButton.Anchor = AnchorStyles.Right;
        m_hideButton.Show();
        iStart += m_buttonWidth + m_insetButton;

        if (m_maxButton != null)
        {
          // Button position is fixed, regardless of our size
          m_maxButton.Location = new Point(client.Width - iStart - m_buttonWidth, m_insetButton);
          m_maxButton.Anchor = AnchorStyles.Right;
        }
      }

      UpdateMaximizeImage();
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      Size ourSize = this.ClientSize;
      Point[] light = new Point[4];
      Point[] dark = new Point[4];
				
      // Depends on orientation
      if (m_dockLeft)
      {
        int iBottom = ourSize.Height - m_inset - 1;
        int iRight = m_offset + 2;
        int iTop = m_inset + m_buttonOffset;

        light[3].X = light[2].X = light[0].X = m_offset;
        light[2].Y = light[1].Y = light[0].Y = iTop;
        light[1].X = m_offset + 1;
        light[3].Y = iBottom;
			
        dark[2].X = dark[1].X = dark[0].X = iRight;
        dark[3].Y = dark[2].Y = dark[1].Y = iBottom;
        dark[0].Y = iTop;
        dark[3].X = iRight - 1;
      }
      else
      {
        int iBottom = m_offset + 2;
        int iRight = ourSize.Width - (m_inset + m_buttonOffset);
				
        light[3].X = light[2].X = light[0].X = m_inset;
        light[1].Y = light[2].Y = light[0].Y = m_offset;
        light[1].X = iRight;
        light[3].Y = m_offset + 1;
			
        dark[2].X = dark[1].X = dark[0].X = iRight;
        dark[3].Y = dark[2].Y = dark[1].Y = iBottom;
        dark[0].Y = m_offset;
        dark[3].X = m_inset;
      }

      using (Pen lightPen = new Pen(ControlPaint.LightLight(m_manager.BackColor)),
               darkPen = new Pen(ControlPaint.Dark(m_manager.BackColor)))
      {
        e.Graphics.DrawLine(lightPen, light[0], light[1]);
        e.Graphics.DrawLine(lightPen, light[2], light[3]);
        e.Graphics.DrawLine(darkPen, dark[0], dark[1]);
        e.Graphics.DrawLine(darkPen, dark[2], dark[3]);

        // Shift coordinates to draw section grab bar
        if (m_dockLeft)
        {
          for(int i=0; i<4; i++)
          {
            light[i].X += 4;
            dark[i].X += 4;
          }
        }
        else
        {
          for(int i=0; i<4; i++)
          {
            light[i].Y += 4;
            dark[i].Y += 4;
          }
        }

        e.Graphics.DrawLine(lightPen, light[0], light[1]);
        e.Graphics.DrawLine(lightPen, light[2], light[3]);
        e.Graphics.DrawLine(darkPen, dark[0], dark[1]);
        e.Graphics.DrawLine(darkPen, dark[2], dark[3]);
      }

      base.OnPaint(e);
    }
  }

  public class WindowDetailCaptionIDE : WindowDetailCaption
  {
    protected enum ImageIndex
    {
      Close					= 0,
      EnabledVerticalMax		= 1,
      EnabledVerticalMin		= 2,
      AutoHide		        = 3, 
      AutoShow		        = 4 
    }

    // Class constants
    protected const int m_yInset = 3;
    protected const int m_yInsetExtra = 3;
    protected const int m_imageWidth = 12;
    protected const int m_imageHeight = 11;
    protected const int m_buttonWidth = 14;
    protected const int m_buttonHeight = 13;
    protected const int m_buttonSpacer = 3;

    // Class fields
    protected static int m_fixedLength;
        
    static WindowDetailCaptionIDE()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_images = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.WindowDetailCaptionIDE"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesCaptionIDE.bmp",
        new Size(m_imageWidth, m_imageHeight),
        new Point(0,0));
    }

    public WindowDetailCaptionIDE(DockingManager manager, 
      EventHandler closeHandler, 
      EventHandler restoreHandler, 
      EventHandler invertAutoHideHandler, 
      ContextHandler contextHandler)
      : base(manager, 
      new Size(m_fixedLength, m_fixedLength), 
      closeHandler, 
      restoreHandler, 
      invertAutoHideHandler,
      contextHandler)
    {
      // Use specificed font in the caption 
      UpdateCaptionHeight(manager.CaptionFont);
    }

    public override void PropogateNameValue(PropogateName name, object value)
    {
      base.PropogateNameValue(name, value);
        
      switch(name)
      {
        case PropogateName.CaptionFont:
          UpdateCaptionHeight((Font)value);    
          break;
        case PropogateName.ActiveTextColor:
        case PropogateName.InactiveTextColor:
          DefineButtonRemapping();
          Invalidate();
          break;
      }
    }
        
    public override void WindowGotFocus()
    {
      SetButtonState();
      Invalidate();
    }

    public override void WindowLostFocus()
    {
      SetButtonState();
      Invalidate();
    }
      
    public override void NotifyFullTitleText(string title)
    {
      this.Text = title;
      Invalidate();
    }

    public override void ParentStateChanged(State newState)
    { 
      // Ignore the AutoHide feature when in floating form
      m_ignoreHideButton = (m_parentWindow.State == State.Floating);

      this.Dock = DockStyle.Top;
      RecalculateButtons();
      Invalidate();
    }

    public override void RemovedFromParent(Window parent)
    {
      if (parent != null)
      {
        Size minSize = parent.MinimalSize;

        // Remove our height from the minimum size of the parent
        minSize.Height -= m_fixedLength;
        minSize.Width -= m_fixedLength;

        parent.MinimalSize = minSize;
      }
    }

    protected override void DefineButtonRemapping()
    {
      // Define use of current system colors
      ColorMap activeMap = new ColorMap();
      ColorMap inactiveMap = new ColorMap();
			
      activeMap.OldColor = Color.Black;
      activeMap.NewColor = m_manager.ActiveTextColor;
      inactiveMap.OldColor = Color.Black;
      inactiveMap.NewColor = m_manager.InactiveTextColor;

      // Create remap attributes for use by button
      m_activeAttr.SetRemapTable(new ColorMap[]{activeMap}, ColorAdjustType.Bitmap);
      m_inactiveAttr.SetRemapTable(new ColorMap[]{inactiveMap}, ColorAdjustType.Bitmap);
    }

    public override void AddedToParent(Window parent)
    {
      if (parent != null)
      {
        Size minSize = parent.MinimalSize;

        // Remove our height from the minimum size of the parent
        minSize.Height += m_fixedLength;
        minSize.Width += m_fixedLength;

        parent.MinimalSize = minSize;
      }
    }

    protected override void OnAddMaximizeInterface()
    {
      if (m_maxButton != null)
      {
        // Set the correct size for the button
        m_maxButton.Size = new Size(m_buttonWidth, m_buttonHeight);

        // Give the button a very thin button
        m_maxButton.BorderWidth = 1;

        // Define correct color setup
        m_maxButton.BackColor = this.BackColor;
        m_maxButton.ImageAttributes = m_inactiveAttr;

        // Force the ImageAttribute for the button to be set
        SetButtonState();
      }
    }

    protected override void UpdateAutoHideImage()
    {
      if (m_pinnedImage)
        m_hideButton.ImageIndexEnabled = (int)ImageIndex.AutoShow;
      else
        m_hideButton.ImageIndexEnabled = (int)ImageIndex.AutoHide;
    }

    protected override void UpdateMaximizeImage()
    {
      if ((m_maxButton != null) && (m_maxInterface != null))
      {
        bool enabled = m_maxInterface.IsMaximizeAvailable();

        if (!enabled)
        {
          if (m_maxButton.Visible)
            m_maxButton.Hide();
        }
        else
        {
          bool maximized = m_maxInterface.IsWindowMaximized(this.ParentWindow);

          if (!m_maxButton.Visible)
            m_maxButton.Show();

          if (maximized)
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledVerticalMin;	
          else
            m_maxButton.ImageIndexEnabled = (int)ImageIndex.EnabledVerticalMax;	
        }
      }
    }

    protected void SetButtonState()
    {
      if (this.ParentWindow != null)
      {
        if (this.ParentWindow.ContainsFocus)
        {
          if (m_closeButton.BackColor != m_manager.ActiveColor)
          {
            m_closeButton.BackColor = m_manager.ActiveColor;
            m_closeButton.ImageAttributes = m_activeAttr;
            m_closeButton.Invalidate();
          }

          if (m_hideButton != null)
          {
            if (m_hideButton.BackColor != m_manager.ActiveColor)
            {
              m_hideButton.BackColor = m_manager.ActiveColor;
              m_hideButton.ImageAttributes = m_activeAttr;
              m_hideButton.Invalidate();
            }
          }

          if (m_maxButton != null)
          {
            if (m_maxButton.BackColor != m_manager.ActiveColor)
            {
              m_maxButton.BackColor = m_manager.ActiveColor;
              m_maxButton.ImageAttributes = m_activeAttr;
              m_maxButton.Invalidate();
            }
          }
        }
        else
        {
          if (m_closeButton.BackColor != this.BackColor)
          {
            m_closeButton.BackColor = this.BackColor;
            m_closeButton.ImageAttributes = m_inactiveAttr;
            m_closeButton.Invalidate();
          }

          if (m_hideButton != null)
          {
            if (m_hideButton.BackColor != this.BackColor)
            {
              m_hideButton.BackColor = this.BackColor;
              m_hideButton.ImageAttributes = m_inactiveAttr;
              m_hideButton.Invalidate();
            }
          }

          if (m_maxButton != null)
          {
            if (m_maxButton.BackColor != this.BackColor)
            {
              m_maxButton.BackColor = this.BackColor;
              m_maxButton.ImageAttributes = m_inactiveAttr;
              m_maxButton.Invalidate();
            }
          }
        }
      }
    }

    protected override void RecalculateButtons()
    {
      int buttonX = this.Width - m_buttonWidth - m_buttonSpacer;
      int buttonY = (m_fixedLength - m_yInset * 2 - m_buttonHeight) / 2 + m_yInset;
        
      if (m_showCloseButton)
      {
        // Button position is fixed, regardless of our size
        m_closeButton.Location = new Point(buttonX, buttonY);

        m_closeButton.Size = new Size(m_buttonWidth, m_buttonHeight);
    		
        // Give the button a very thin button
        m_closeButton.BorderWidth = 1;

        // Let the location of the control be updated for us
        m_closeButton.Anchor = AnchorStyles.Right;

        // Just in case currently hidden
        m_closeButton.Show();
                
        // Define start of next button
        buttonX -= m_buttonWidth;
      }
      else
        m_closeButton.Hide();
                        
      if (m_showHideButton && !m_ignoreHideButton)
      {
        // Button position is fixed, regardless of our size
        m_hideButton.Location = new Point(buttonX, buttonY);

        m_hideButton.Size = new Size(m_buttonWidth, m_buttonHeight);
			
        // Give the button a very thin button
        m_hideButton.BorderWidth = 1;

        // Let the location of the control be updated for us
        m_hideButton.Anchor = AnchorStyles.Right;

        // Just in case currently hidden
        m_hideButton.Show();

        // Define start of next button
        buttonX -= m_buttonWidth;
                
        UpdateAutoHideImage();
      }
      else
        m_hideButton.Hide();
            
      if (m_maxButton != null)
      {
        // Button position is fixed, regardless of our size
        m_maxButton.Location = new Point(buttonX, buttonY);

        m_maxButton.Size = new Size(m_buttonWidth, m_buttonHeight);
			
        // Give the button a very thin button
        m_maxButton.BorderWidth = 1;

        // Let the location of the control be updated for us
        m_maxButton.Anchor = AnchorStyles.Right;

        // Define start of next button
        buttonX -= m_buttonWidth;

        UpdateMaximizeImage();
      }
    }

    protected override void CreateButtons()
    {
      // Define the ImageList and which ImageIndex to show initially
      m_closeButton = new InertButton(m_images, (int)ImageIndex.Close);
      m_hideButton = new InertButton(m_images, (int)ImageIndex.AutoHide);
			
      m_closeButton.Size = new Size(m_buttonWidth, m_buttonHeight);
      m_hideButton.Size = new Size(m_buttonWidth, m_buttonHeight);

      // Let the location of the control be updated for us
      m_closeButton.Anchor = AnchorStyles.Right;
      m_hideButton.Anchor = AnchorStyles.Right;

      // Define the button position relative to the size set in the constructor
      m_closeButton.Location = new Point(m_fixedLength - m_buttonWidth - m_buttonSpacer, 
        (m_fixedLength - m_yInset * 2 - m_buttonHeight) / 2 + m_yInset);
            
      m_hideButton.Location = new Point(m_fixedLength - (m_buttonWidth - m_buttonSpacer) * 2, 
        (m_fixedLength - m_yInset * 2 - m_buttonHeight) / 2 + m_yInset);

      // Give the button a very thin button
      m_closeButton.BorderWidth = 1;
      m_hideButton.BorderWidth = 1;

      // Define correct color setup
      m_closeButton.BackColor = this.BackColor;
      m_closeButton.ImageAttributes = m_inactiveAttr;
      m_hideButton.BackColor = this.BackColor;
      m_hideButton.ImageAttributes = m_inactiveAttr;

      // Add to the display
      Controls.Add(m_closeButton);
      Controls.Add(m_hideButton);

      // Let base class perform common actions
      base.CreateButtons();
    }

    protected void UpdateCaptionHeight(Font captionFont)
    {
      // Dynamically calculate the required height of the caption area
      m_fixedLength = (int)captionFont.GetHeight() + (m_yInset + m_yInsetExtra) * 2;
    
      int minHeight = m_buttonHeight + m_yInset * 4 + 1;

      // Maintain a minimum height to allow correct showing of buttons
      if (m_fixedLength < minHeight)
        m_fixedLength = minHeight;

      this.Size = new Size(m_fixedLength, m_fixedLength);
			
      RecalculateButtons();

      Invalidate();
    }

    protected override void OnPaintBackground(PaintEventArgs e)
    {
      // Overriden to prevent background being painted
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      bool focused = false;

      if (this.ParentWindow != null)
        focused = this.ParentWindow.ContainsFocus;

      // Sometimes the min/max button is created and then an attempt is made to 
      // hide the button. But for some reason it does not always manage to hide 
      // the button. So forced to check here everytime to ensure its hidden.
      UpdateMaximizeImage();

      SetButtonState();
            
      Size ourSize = this.ClientSize;

      int xEnd = ourSize.Width;
      int yEnd = ourSize.Height - m_yInset * 2;

      Rectangle rectCaption = new Rectangle(0, m_yInset, xEnd, yEnd - m_yInset + 1);

      using(SolidBrush backBrush = new SolidBrush(this.BackColor),
              activeBrush = new SolidBrush(m_manager.ActiveColor),
              activeTextBrush = new SolidBrush(m_manager.ActiveTextColor),
              inactiveBrush = new SolidBrush(m_manager.InactiveTextColor))
      {
        // Is this control Active?
        if (focused)
        {
          // Fill the entire background area
          e.Graphics.FillRectangle(backBrush, e.ClipRectangle);
    	
          // Use a solid filled background for text
          e.Graphics.FillRectangle(activeBrush, rectCaption);
    			
          // Start drawing text a little from the left
          rectCaption.X += m_buttonSpacer;
          rectCaption.Y += 1;
          rectCaption.Height -= 2;

          // Reduce the width to account for close button
          rectCaption.Width -= m_closeButton.Width + m_buttonSpacer;

          // Reduce width to account for the optional maximize button
          if ((m_maxButton != null) && (m_maxButton.Visible))
            rectCaption.Width -= m_closeButton.Width;
    				
          e.Graphics.DrawString(this.Text, m_manager.CaptionFont, activeTextBrush, rectCaption);
        }
        else
        {
          // Fill the entire background area
          e.Graphics.FillRectangle(backBrush, e.ClipRectangle);
    	            
          // Inactive and so use a rounded rectangle
          using (Pen dark = new Pen(ControlPaint.LightLight(m_manager.InactiveTextColor)))
          {
            e.Graphics.DrawLine(dark, 1, m_yInset, xEnd - 2, m_yInset);
            e.Graphics.DrawLine(dark, 1, yEnd, xEnd - 2, yEnd);
            e.Graphics.DrawLine(dark, 0, m_yInset + 1, 0, yEnd - 1);
            e.Graphics.DrawLine(dark, xEnd - 1, m_yInset + 1, xEnd - 1, yEnd - 1);

            // Start drawing text a little from the left
            rectCaption.X += m_buttonSpacer;
            rectCaption.Y += 1;
            rectCaption.Height -= 2;

            // Reduce the width to account for close button
            rectCaption.Width -= m_closeButton.Width + m_buttonSpacer;

            // Reduce width to account for the optional maximize button
            if ((m_maxButton != null) && (m_maxButton.Visible))
              rectCaption.Width -= m_maxButton.Width;

            e.Graphics.DrawString(this.Text, m_manager.CaptionFont, inactiveBrush, rectCaption);
          }
        }	
      }
            
      // Let delegates fire through base
      base.OnPaint(e);

      // Always get the button to repaint as we have painted over their area
      m_closeButton.Refresh();
    }				
  }
}

#region revision history

// $Log: WindowDetailCaption.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/WindowDetailCaption.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: WindowDetailCaption.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

