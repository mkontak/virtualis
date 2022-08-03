// $Id: InertButton.cs,v 1.1 2005/10/11 14:41:18 mkontak Exp $
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
using System.Windows.Forms;
using System.ComponentModel;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxBitmap(typeof(InertButton))]
  [DefaultProperty("PopupStyle")]
  public class InertButton : Control
  {
    // Instance fields
    protected int m_borderWidth;
    protected bool m_mouseOver;
    protected bool m_mouseCapture;
    protected bool m_popupStyle;
    protected int m_imageIndexEnabled;
    protected int m_imageIndexDisabled;
    protected ImageList m_imageList;
    protected ImageAttributes m_imageAttr;
    protected MouseButtons m_mouseButton;
		
    public InertButton()
    {
      InternalConstruct(null, -1, -1, null);
    }

    public InertButton(ImageList imageList, int imageIndexEnabled)
    {
      InternalConstruct(imageList, imageIndexEnabled, -1, null);
    }

    public InertButton(ImageList imageList, int imageIndexEnabled, int imageIndexDisabled)
    {
      InternalConstruct(imageList, imageIndexEnabled, imageIndexDisabled, null);
    }

    public InertButton(ImageList imageList, int imageIndexEnabled, int imageIndexDisabled, ImageAttributes imageAttr)
    {
      InternalConstruct(imageList, imageIndexEnabled, imageIndexDisabled, imageAttr);
    }
		
    public void InternalConstruct(ImageList imageList, 
      int imageIndexEnabled, 
      int imageIndexDisabled, 
      ImageAttributes imageAttr)
    {
      // Remember parameters
      m_imageList = imageList;
      m_imageIndexEnabled = imageIndexEnabled;
      m_imageIndexDisabled = imageIndexDisabled;
      m_imageAttr = imageAttr;

      // Set initial state
      m_borderWidth = 2;
      m_mouseOver = false;
      m_mouseCapture = false;
      m_popupStyle = true;
      m_mouseButton = MouseButtons.None;

      // Prevent drawing flicker by blitting from memory in WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);

      // Prevent base class from trying to generate double click events and
      // so testing clicks against the double click time and rectangle. Getting
      // rid of this allows the user to press then button very quickly.
      SetStyle(ControlStyles.StandardDoubleClick, false);

      // Should not be allowed to select this control
      SetStyle(ControlStyles.Selectable, false);
    }

    [Category("Appearance")]
    [DefaultValue(null)]
    public ImageList ImageList
    {
      get { return m_imageList; }

      set
      {
        if (m_imageList != value)
        {
          m_imageList = value;
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(-1)]
    public int ImageIndexEnabled
    {
      get { return m_imageIndexEnabled; }

      set
      {
        if (m_imageIndexEnabled != value)
        {
          m_imageIndexEnabled = value;
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(-1)]
    public int ImageIndexDisabled
    {
      get { return m_imageIndexDisabled; }

      set
      {
        if (m_imageIndexDisabled != value)
        {
          m_imageIndexDisabled = value;
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(null)]
    public ImageAttributes ImageAttributes
    {
      get { return m_imageAttr; }
			
      set
      {
        if (m_imageAttr != value)
        {
          m_imageAttr = value;
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(2)]
    public int BorderWidth
    {
      get { return m_borderWidth; }

      set
      {
        if (m_borderWidth != value)
        {
          m_borderWidth = value;
          Invalidate();
        }
      }
    }

    [Category("Appearance")]
    [DefaultValue(true)]
    public bool PopupStyle
    {
      get { return m_popupStyle; }

      set
      {
        if (m_popupStyle != value)
        {
          m_popupStyle = value;
          Invalidate();
        }
      }
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      if (!m_mouseCapture)
      {
        // Mouse is over the button and being pressed, so enter the button depressed 
        // state and also remember the original button that was pressed. As we only 
        // generate an event when the same button is released.
        m_mouseOver = true;
        m_mouseCapture = true;
        m_mouseButton = e.Button;

        // Redraw to show button state
        Invalidate();
      }

      base.OnMouseDown(e);
    }

    protected override void OnMouseUp(MouseEventArgs e)
    {
      // Are we waiting for this button to go up?
      if (e.Button == m_mouseButton)
      {
        // Set state back to become normal
        m_mouseOver = false;
        m_mouseCapture = false;

        // Redraw to show button state
        Invalidate();
      }
      else
      {
        // We don't want to lose capture of mouse
        Capture = true;
      }

      base.OnMouseUp(e);
    }

    protected override void OnMouseMove(MouseEventArgs e)
    {
      // Is mouse point inside our client rectangle
      bool over = this.ClientRectangle.Contains(new Point(e.X, e.Y));

      // If entering the button area or leaving the button area...
      if (over != m_mouseOver)
      {
        // Update state
        m_mouseOver = over;

        // Redraw to show button state
        Invalidate();
      }

      base.OnMouseMove(e);
    }

    protected override void OnMouseEnter(EventArgs e)
    {
      // Update state to reflect mouse over the button area
      m_mouseOver = true;

      // Redraw to show button state
      Invalidate();

      base.OnMouseEnter(e);
    }

    protected override void OnMouseLeave(EventArgs e)
    {
      // Update state to reflect mouse not over the button area
      m_mouseOver = false;

      // Redraw to show button state
      Invalidate();

      base.OnMouseLeave(e);
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      // Do we have an image list for use?
      if (m_imageList != null)
      {
        // Is the button disabled?
        if (!this.Enabled)
        {
          // Do we have an image for showing when disabled?
          if (m_imageIndexDisabled != -1)
          {
            // Any caller supplied attributes to modify drawing?
            if (null == m_imageAttr)
            {
              // No, so use the simple DrawImage method
              e.Graphics.DrawImage(m_imageList.Images[m_imageIndexDisabled], new Point(1,1));
            }
            else
            {
              // Yes, need to use the more complex DrawImage method instead
              Image image = m_imageList.Images[m_imageIndexDisabled];

              // Three points provided are upper-left, upper-right and 
              // lower-left of the destination parallelogram. 
              Point[] pts = new Point[3];
              pts[0].X = 1;
              pts[0].Y = 1;
              pts[1].X = pts[0].X + image.Width;
              pts[1].Y = pts[0].Y;
              pts[2].X = pts[0].X;
              pts[2].Y = pts[1].Y + image.Height;

              e.Graphics.DrawImage(m_imageList.Images[m_imageIndexDisabled], 
                pts,
                new Rectangle(0, 0, image.Width, image.Height),
                GraphicsUnit.Pixel, m_imageAttr);
            }
          }
          else
          {
            // No disbled image, how about an enabled image we can draw grayed?
            if (m_imageIndexEnabled != -1)
            {
              // Yes, draw the enabled image but with color drained away
              ControlPaint.DrawImageDisabled(e.Graphics, m_imageList.Images[m_imageIndexEnabled], 1, 1, this.BackColor);
            }
            else
            {
              // No images at all. Do nothing.
            }
          }
        }
        else
        {
          // Button is enabled, any caller supplied attributes to modify drawing?
          if (null == m_imageAttr)
          {
            // No, so use the simple DrawImage method
            e.Graphics.DrawImage(m_imageList.Images[m_imageIndexEnabled], 
              (m_mouseOver &&  m_mouseCapture ? new Point(2,2) : 
              new Point(1,1)));
          }
          else
          {
            // Yes, need to use the more complex DrawImage method instead
            Image image = m_imageList.Images[m_imageIndexEnabled];

            // Three points provided are upper-left, upper-right and 
            // lower-left of the destination parallelogram. 
            Point[] pts = new Point[3];
            pts[0].X = (m_mouseOver && m_mouseCapture) ? 2 : 1;
            pts[0].Y = (m_mouseOver && m_mouseCapture) ? 2 : 1;
            pts[1].X = pts[0].X + image.Width;
            pts[1].Y = pts[0].Y;
            pts[2].X = pts[0].X;
            pts[2].Y = pts[1].Y + image.Height;

            e.Graphics.DrawImage(m_imageList.Images[m_imageIndexEnabled], 
              pts,
              new Rectangle(0, 0, image.Width, image.Height),
              GraphicsUnit.Pixel, m_imageAttr);
          }

          ButtonBorderStyle bs;

          // Decide on the type of border to draw around image
          if (m_popupStyle)
          {
            if (m_mouseOver && this.Enabled)
              bs = (m_mouseCapture ? ButtonBorderStyle.Inset : ButtonBorderStyle.Outset);
            else
              bs = ButtonBorderStyle.Solid;
          }
          else
          {
            if (this.Enabled)
              bs = ((m_mouseOver && m_mouseCapture) ? ButtonBorderStyle.Inset : ButtonBorderStyle.Outset);
            else
              bs = ButtonBorderStyle.Solid;
          }

          ControlPaint.DrawBorder(e.Graphics, this.ClientRectangle, 
            this.BackColor, m_borderWidth, bs,
            this.BackColor, m_borderWidth, bs,
            this.BackColor, m_borderWidth, bs,
            this.BackColor, m_borderWidth, bs);
        }
      }

      base.OnPaint(e);
    }
  }
}

#region revision history

// $Log: InertButton.cs,v $
// Revision 1.1  2005/10/11 14:41:18  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/InertButton/InertButton.cs,v 1.1 2005/10/11 14:41:18 mkontak Exp $
// $Id: InertButton.cs,v 1.1 2005/10/11 14:41:18 mkontak Exp $

#endregion

