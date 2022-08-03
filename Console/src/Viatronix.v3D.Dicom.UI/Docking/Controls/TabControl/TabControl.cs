// $Id: TabControl.cs,v 1.3 2006/10/30 16:30:08 gdavidson Exp $
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
using System.Text;
using System.Drawing;
using System.Resources;
using System.Reflection;
using System.Collections;
using System.Drawing.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Imaging;
using Microsoft.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxBitmap(typeof(TabControl))]
  [DefaultProperty("Appearance")]
  [DefaultEvent("SelectionChanged")]
  [Designer(typeof(Viatronix.v3D.Dicom.UI.Docking.Controls.TabControlDesigner))]
  public class TabControl : UserControl
  {
    // Enumeration of appearance styles
    public enum VisualAppearance
    {
      MultiDocument = 0,
      MultiForm = 1,
      MultiBox = 2
    }

    // Enumeration of modes that control display of the tabs area
    public enum HideTabsModes
    {
      ShowAlways,
      HideAlways,
      HideUsingLogic,
      HideWithoutMouse
    }

    // Indexes into the menu images strip
    protected enum ImageStrip
    {
      LeftEnabled = 0,
      LeftDisabled = 1,
      RightEnabled = 2,
      RightDisabled = 3,
      Close = 4,
      Error = 5
    }

    // Enumeration of Indexes into positioning constants array
    protected enum PositionIndex
    {
      BorderTop			= 0,
      BorderLeft			= 1,
      BorderBottom		= 2, 
      BorderRight			= 3,
      ImageGapTop			= 4,
      ImageGapLeft		= 5,
      ImageGapBottom		= 6,
      ImageGapRight		= 7,
      TextOffset			= 8,
      TextGapLeft			= 9,
      TabsBottomGap		= 10,
      ButtonOffset		= 11,
    }

    // Helper class for handling multiline calculations
    protected class MultiRect
    {
      protected Rectangle m_rect;
      protected int m_index;

      public MultiRect(Rectangle rect, int index)
      {
        m_rect = rect;
        m_index = index;
      }

      public int Index
      {
        get { return m_index; }
      }            
            
      public Rectangle Rect
      {
        get { return m_rect; }
        set { m_rect = value; }
      }
            
      public int X
      {
        get { return m_rect.X; }
        set { m_rect.X = value; }
      }

      public int Y
      {
        get { return m_rect.Y; }
        set { m_rect.Y = value; }
      }

      public int Width
      {
        get { return m_rect.Width; }
        set { m_rect.Width = value; }
      }

      public int Height
      {
        get { return m_rect.Height; }
        set { m_rect.Height = value; }
      }
    }
        
    protected class HostPanel : Panel
    {
      public HostPanel()
      {
        // Prevent flicker with double buffering and all painting inside WM_PAINT
        SetStyle(ControlStyles.DoubleBuffer, true);
        SetStyle(ControlStyles.AllPaintingInWmPaint, true);
        SetStyle(ControlStyles.UserPaint, true);
      }
            
      protected override void OnResize(EventArgs e)
      {
        // Update size of each child to match ourself
        foreach(Control c in this.Controls)
          c.Size = this.Size;
            
        base.OnResize(e);
      }
    }

    // Class constants for sizing/positioning each style
    protected static int[,] m_position = { 
                                                {3, 1, 1, 1, 1, 2, 1, 1, 2, 1, 3, 2},	// IDE
                                                {6, 2, 2, 3, 3, 1, 1, 0, 1, 1, 2, 0}	// Plain
                                         };

    // Class constants
    protected static int m_plainBorder = 3;
    protected static int m_plainBorderDouble = 6;
    protected static int m_tabsAreaStartInset = 5;
    protected static int m_tabsAreaEndInset = 5;
    protected static float m_alphaIDE = 1.5F;
    protected static int m_buttonGap = 3;
    protected static int m_buttonWidth = 14;
    protected static int m_buttonHeight = 14;
    protected static int m_imageButtonWidth = 12;
    protected static int m_imageButtonHeight = 12;
    protected static int m_multiBoxAdjust = 2;
    protected readonly Rectangle m_nullPosition = new Rectangle(-999, -999, 0, 0);

    // Class state
    protected static ImageList m_internalImages;

    // Instance fields - size/positioning
    protected int m_textHeight;
    protected int m_imageWidth;
    protected int m_imageHeight;
    protected int m_imageGapTopExtra;
    protected int m_imageGapBottomExtra;
    protected Rectangle m_pageRect;
    protected Rectangle m_pageAreaRect;
    protected Rectangle m_tabsAreaRect;

    // Instance fields - state
    protected int m_ctrlTopOffset;			// How far from top edge embedded controls should offset
    protected int m_ctrlLeftOffset;			// How far from left edge embedded controls should offset
    protected int m_ctrlRightOffset;			// How far from right edgeembedded controls should offset
    protected int m_ctrlBottomOffset;	    // How far from bottom edge embedded controls should offset
    protected int m_styleIndex;				// Index into position array
    protected int m_pageSelected;			// index of currently selected page (-1 is none)
    protected int m_startPage;				// index of first page to draw, used when scrolling pages
    protected int m_hotTrackPage;			// which page is currently displayed as being tracked
    protected int m_topYPos;                 // Y position of first line in multiline mode
    protected int m_bottomYPos;              // Y position of last line in multiline mode
    protected int m_leaveTimeout;            // How long from leaving to timeout occuring
    protected bool m_dragFromControl;        // Must drag away from whole control before drag events generated
    protected bool m_mouseOver;              // Mouse currently over the control (or child pages)
    protected bool m_multiline;              // should tabs that cannot fit on a line create new lines
    protected bool m_multilineFullWidth;     // when in multiline mode, all lines are extended to end 
    protected bool m_shrinkPagesToFit;		// pages are shrunk so they all fit in control width
    protected bool m_changed;				// Flag for use when updating contents of collection
    protected bool m_positionAtTop;			// display tabs at top or bottom of the control
    protected bool m_showClose;				// should the close button be displayed
    protected bool m_showArrows;				// should then scroll arrow be displayed
    protected bool m_insetPlain;				// Show the inset border for controls
    protected bool m_insetBorderPagesOnly;   // Remove the border entirely for Plain mode
    protected bool m_selectedTextOnly;	    // Only draw text for selected tab
    protected bool m_rightScroll;			// Should the right scroll button be enabled
    protected bool m_leftScroll;				// Should the left scroll button be enabled
    protected bool m_dimUnselected;			// should unselected pages be drawn slightly dimmed
    protected bool m_boldSelected;			// should selected page use a bold font
    protected bool m_hotTrack;				// should mouve moving over text hot track it
    protected bool m_hoverSelect;			// select a page when he mouse hovers over it
    protected bool m_recalculate;			// flag to indicate recalculation is needed before painting
    protected bool m_leftMouseDown;			// Is the left mouse button down
    protected bool m_leftMouseDownDrag;		// Has a drag operation begun
    protected bool m_ignoreDownDrag;         // When pressed the left button cannot generate two drags
    protected bool m_defaultColor;			// Is the background color the default one?
    protected bool m_defaultFont;			// Is the Font the default one?
    protected bool m_recordFocus;			// Record the control with focus when leaving a page
    protected bool m_idePixelArea;           // Place a one pixel border at top/bottom of tabs area
    protected bool m_idePixelBorder;         // Place a one pixel border around control
    protected PopupMenu m_contextMenu;       // Context menu to show on right mouse up
    protected Point m_leftMouseDownPos;		// Initial mouse down position for left mouse button
    protected Color m_hotTextColor;			// color for use when drawing text as hot
    protected Color m_textColor;				// color for use when text not hot
    protected Color m_textInactiveColor;	    // color for use when text not hot and not the active tab
    protected Color m_backIDE;				// background drawing color when in IDE appearance
    protected Color m_buttonActiveColor;		// color for drawing buttons images when active
    protected Color m_buttonInactiveColor;	// color for drawing buttons images when inactive
    protected Color m_backLight;				// light variation of the back color
    protected Color m_backLightLight;		// lightlight variation of the back color
    protected Color m_backDark;				// dark variation of the back color
    protected Color m_backDarkDark;			// darkdark variation of the back color
    protected VisualStyle m_style;			// which style of use 
    protected HideTabsModes m_hideTabsMode;  // Decide when to hide/show tabs area
    protected System.Windows.Forms.Timer m_overTimer;             // Time when mouse has left control
    protected HostPanel m_hostPanel;         // Hosts the page instance control/form
    protected VisualAppearance m_appearance;	// which appearance style
    protected ImageList m_imageList;			// collection of images for use is tabs
    protected ArrayList m_tabRects;			// display rectangles for associated page
    protected TabPageCollection m_tabPages;	// collection of pages

    // Instance fields - buttons
    protected InertButton m_closeButton;
    protected InertButton m_leftArrow;
    protected InertButton m_rightArrow;

    public delegate void DoubleClickTabHandler(TabControl sender, TabPage page);

    // Exposed events
    public event EventHandler ClosePressed;
    public event EventHandler SelectionChanging;
    public event EventHandler SelectionChanged;
    public event EventHandler PageGotFocus;
    public event EventHandler PageLostFocus;
    public event CancelEventHandler PopupMenuDisplay;
    public event MouseEventHandler PageDragStart;
    public event MouseEventHandler PageDragMove;
    public event MouseEventHandler PageDragEnd;
    public event MouseEventHandler PageDragQuit;
    public event DoubleClickTabHandler DoubleClickTab;

    static TabControl()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_internalImages = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesTabControl.bmp",
        new Size(m_imageButtonWidth, m_imageButtonHeight),
        new Point(0,0));
    }

    public TabControl()
    {
      // Prevent flicker with double buffering and all painting inside WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.UserPaint, true);

      // Create collections
      m_tabRects = new ArrayList();
      m_tabPages = new TabPageCollection();

      // Hookup to collection events
      m_tabPages.Clearing += new CollectionClear(OnClearingPages);
      m_tabPages.Cleared += new CollectionClear(OnClearedPages);
      m_tabPages.Inserting += new CollectionChange(OnInsertingPage);
      m_tabPages.Inserted += new CollectionChange(OnInsertedPage);
      m_tabPages.Removing += new CollectionChange(OnRemovingPage);
      m_tabPages.Removed += new CollectionChange(OnRemovedPage);

      // Define the default state of the control
      m_startPage = -1;						
      m_pageSelected = -1;						
      m_hotTrackPage = -1;					
      m_imageList = null;						
      m_insetPlain = true;
      m_multiline = false;
      m_multilineFullWidth = false;
      m_dragFromControl = true;
      m_mouseOver = false;
      m_leftScroll = false;
      m_defaultFont = true;
      m_defaultColor = true;
      m_rightScroll = false;
      m_hoverSelect = false;
      m_leftMouseDown = false;
      m_ignoreDownDrag = true;
      m_selectedTextOnly = false;
      m_leftMouseDownDrag = false;
      m_insetBorderPagesOnly = false;
      m_hideTabsMode = HideTabsModes.ShowAlways;
      m_recordFocus = true;
      m_styleIndex = 1;
      m_leaveTimeout = 200;
      m_ctrlTopOffset = 0;
      m_ctrlLeftOffset = 0;
      m_ctrlRightOffset = 0;
      m_ctrlBottomOffset = 0;
      m_style = VisualStyle.IDE;
      m_buttonActiveColor = Color.FromArgb(128, this.ForeColor);
      m_buttonInactiveColor = m_buttonActiveColor;
      m_textColor = TabControl.DefaultForeColor;	
      m_textInactiveColor = Color.FromArgb(128, m_textColor);
      m_hotTextColor = SystemColors.ActiveCaption;

      // Create the panel that hosts each page control. This is done to prevent the problem where a 
      // hosted Control/Form has 'AutoScaleBaseSize' defined. In which case our attempt to size it the
      // first time is ignored and the control sizes itself to big and would overlap the tabs area.
      m_hostPanel = new HostPanel();
      m_hostPanel.Location = new Point(-1,-1);
      m_hostPanel.Size = new Size(0,0);
      m_hostPanel.MouseEnter += new EventHandler(OnPageMouseEnter);
      m_hostPanel.MouseLeave += new EventHandler(OnPageMouseLeave);

      // Create hover buttons
      m_closeButton = new InertButton(m_internalImages, (int)ImageStrip.Close);
      m_leftArrow = new InertButton(m_internalImages, (int)ImageStrip.LeftEnabled, (int)ImageStrip.LeftDisabled);
      m_rightArrow = new InertButton(m_internalImages, (int)ImageStrip.RightEnabled, (int)ImageStrip.RightDisabled);

      // We want our buttons to have very thin borders
      m_closeButton.BorderWidth = m_leftArrow.BorderWidth = m_rightArrow.BorderWidth = 1;

      // Hookup to the button events
      m_closeButton.Click += new EventHandler(OnCloseButton);
      m_leftArrow.Click += new EventHandler(OnLeftArrow);
      m_rightArrow.Click += new EventHandler(OnRightArrow);

      // Set their fixed sizes
      m_leftArrow.Size = m_rightArrow.Size = m_closeButton.Size = new Size(m_buttonWidth, m_buttonHeight);

      // Add child controls
      Controls.AddRange(new Control[]{m_closeButton, m_leftArrow, m_rightArrow, m_hostPanel});

      // Grab some contant values
      m_imageWidth = 16;
      m_imageHeight = 16;

      // Default to having a MultiForm usage
      SetAppearance(VisualAppearance.MultiForm);
            
      // Need a timer so that when the mouse leaves, a fractionaly delay occurs before
      // noticing and hiding the tabs area when the appropriate style is set
      m_overTimer = new System.Windows.Forms.Timer();
      m_overTimer.Interval = m_leaveTimeout;
      m_overTimer.Tick += new EventHandler(OnMouseTick);

      // Need notification when the MenuFont is changed
      Microsoft.Win32.SystemEvents.UserPreferenceChanged += 
        new UserPreferenceChangedEventHandler(OnPreferenceChanged);

      // Define the default Font, BackColor and Button images
      DefineFont(SystemInformation.MenuFont);
      DefineBackColor(SystemColors.Control);
      DefineButtonImages();

      Recalculate();
    }

    protected override void Dispose( bool disposing )
    {
      if(disposing)
      {
        // Remove notifications
        Microsoft.Win32.SystemEvents.UserPreferenceChanged -= 
          new UserPreferenceChangedEventHandler(OnPreferenceChanged);
      }
      base.Dispose(disposing);
    }

    [Category("Appearance")]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public virtual TabPageCollection TabPages
    {
      get { return m_tabPages; }
    }

    [Category("Appearance")]
    public override Font Font
    {
      get { return base.Font; }

      set
      {
        if (value != null)
        {
          if (value != base.Font)
          {
            m_defaultFont = (value == SystemInformation.MenuFont);

            DefineFont(value);

            m_recalculate = true;
            Invalidate();
          }
        }
      }
    }

    private bool ShouldSerializeFont()
    {
      return !m_defaultFont;
    }
        
    [Category("Appearance")]
    public override Color ForeColor
    {
      get { return m_textColor; }
			
      set 
      {
        if (m_textColor != value)
        {
          m_textColor = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeForeColor()
    {
      return m_textColor != TabControl.DefaultForeColor;
    }

    [Category("Appearance")]
    public override Color BackColor
    {
      get { return base.BackColor; }
			
      set 
      {
        if (this.BackColor != value)
        {
          m_defaultColor = (value == SystemColors.Control);

          DefineBackColor(value);
		
          m_recalculate = true;
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeBackColor()
    {
      return this.BackColor != SystemColors.Control;
    }

    [Category("Appearance")]
    public virtual Color ButtonActiveColor
    {
      get { return m_buttonActiveColor; }

      set
      {
        if (m_buttonActiveColor != value)
        {
          m_buttonActiveColor = value;
          DefineButtonImages();
        }
      }
    }

    private bool ShouldSerializeButtonActiveColor()
    {
      return m_buttonActiveColor != Color.FromArgb(128, this.ForeColor);
    }

    public void ResetButtonActiveColor()
    {
      ButtonActiveColor = Color.FromArgb(128, this.ForeColor);
    }

    [Category("Appearance")]
    public virtual Color ButtonInactiveColor
    {
      get { return m_buttonInactiveColor; }

      set
      {
        if (m_buttonInactiveColor != value)
        {
          m_buttonInactiveColor = value;
          DefineButtonImages();
        }
      }
    }

    private bool ShouldSerializeButtonInactiveColor()
    {
      return m_buttonInactiveColor != Color.FromArgb(128, this.ForeColor);
    }

    public void ResetButtonInactiveColor()
    {
      ButtonInactiveColor = Color.FromArgb(128, this.ForeColor); 
    }
        
    [Category("Appearance")]
    [DefaultValue(typeof(VisualAppearance), "MultiForm")]
    public virtual VisualAppearance Appearance
    {
      get { return m_appearance; }
			
      set
      {
        if (m_appearance != value)
        {
          SetAppearance(value);

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetAppearance()
    {
      Appearance = VisualAppearance.MultiForm;
    }

    [Category("Appearance")]
    [DefaultValue(typeof(VisualStyle), "IDE")]
    public virtual VisualStyle Style
    {
      get { return m_style; }
			
      set
      {
        if (m_style != value)
        {
          m_style = value;

          // Define the correct style indexer
          SetStyleIndex();

          Recalculate();
          Invalidate();
        }
      }
    }
        
    public void ResetStyle()
    {
      Style = VisualStyle.IDE;
    }
        
    [Category("Behavour")]
    public virtual PopupMenu ContextPopupMenu
    {
      get { return m_contextMenu; }
      set { m_contextMenu = value; }
    }
        
    protected bool ShouldSerializeContextPopupMenu()
    {
      return m_contextMenu != null;
    }
        
    public void ResetContextPopupMenu()
    {
      ContextPopupMenu = null;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool HotTrack
    {
      get { return m_hotTrack; }
			
      set 
      {
        if (m_hotTrack != value)
        {
          m_hotTrack = value;

          if (!m_hotTrack)
            m_hotTrackPage = -1;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    public void ResetHotTrack()
    {
      HotTrack = false;
    }

    [Category("Appearance")]
    public virtual Color HotTextColor
    {
      get { return m_hotTextColor; }
			
      set 
      {
        if (m_hotTextColor != value)
        {
          m_hotTextColor = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeHotTextColor()
    {
      return m_hotTextColor != SystemColors.ActiveCaption;
    }
        
    public void ResetHotTextColor()
    {
      HotTextColor = SystemColors.ActiveCaption;
    }

    [Category("Appearance")]
    public virtual Color TextColor
    {
      get { return m_textColor; }
			
      set 
      {
        if (m_textColor != value)
        {
          m_textColor = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeTextColor()
    {
      return m_textColor != TabControl.DefaultForeColor;
    }

    public void ResetTextColor()
    {   
      TextColor = TabControl.DefaultForeColor;
    }

    [Category("Appearance")]
    public virtual Color TextInactiveColor
    {
      get { return m_textInactiveColor; }
			
      set 
      {
        if (m_textInactiveColor != value)
        {
          m_textInactiveColor = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    private bool ShouldSerializeTextInactiveColor()
    {
      return m_textInactiveColor != Color.FromArgb(128, TabControl.DefaultForeColor);
    }

    public void TextTextInactiveColor()
    {
      TextInactiveColor = Color.FromArgb(128, TabControl.DefaultForeColor);
    }

    [Browsable(false)]
    public virtual Rectangle TabsAreaRect
    {
      get { return m_tabsAreaRect; }
    }

    [Category("Appearance")]
    public virtual ImageList ImageList
    {
      get { return m_imageList; }

      set 
      {
        if (m_imageList != value)
        {
          m_imageList = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }
		
    private bool ShouldSerializeImageList()
    {
      return m_imageList != null;
    }
        
    public void ResetImageList()
    {
      ImageList = null;
    }

    [Category("Appearance")]
    public virtual bool PositionTop
    {
      get { return m_positionAtTop; }
			
      set
      {
        if (m_positionAtTop != value)
        {
          m_positionAtTop = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializePositionTop()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_positionAtTop != false;
        case VisualAppearance.MultiDocument:
        default:
          return m_positionAtTop != true;
      }
    }

    public void ResetPositionTop()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          PositionTop = false;
          break;
        case VisualAppearance.MultiDocument:
        default:
          PositionTop = true;
          break;
      }
    }

    [Category("Appearance")]
    public virtual bool ShowClose
    {
      get { return m_showClose; }
			
      set
      {
        if (m_showClose != value)
        {
          m_showClose = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }
        
    protected bool ShouldSerializeShowClose()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_showClose != false;
        case VisualAppearance.MultiDocument:
        default:
          return m_showClose != true;
      }
    }

    public void ResetShowClose()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          ShowClose = false;
          break;
        case VisualAppearance.MultiDocument:
        default:
          ShowClose = true;
          break;
      }
    }

    [Category("Appearance")]
    public virtual bool ShowArrows
    {
      get { return m_showArrows; }
			
      set
      {
        if (m_showArrows != value)
        {
          m_showArrows = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializeShowArrows()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_showArrows != false;
        case VisualAppearance.MultiDocument:
        default:
          return m_showArrows != true;
      }
    }
        
    public void ResetShowArrows()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          ShowArrows = false;
          break;
        case VisualAppearance.MultiDocument:
        default:
          ShowArrows = true;
          break;
      }
    }

    [Category("Appearance")]
    public virtual bool ShrinkPagesToFit
    {
      get { return m_shrinkPagesToFit; }
			
      set 
      {
        if (m_shrinkPagesToFit != value)
        {
          m_shrinkPagesToFit = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializeShrinkPagesToFit()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_shrinkPagesToFit != true;
        case VisualAppearance.MultiDocument:
        default:
          return m_shrinkPagesToFit != false;
      }
    }
        
    public void ResetShrinkPagesToFit()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          ShrinkPagesToFit = true;
          break;
        case VisualAppearance.MultiDocument:
        default:
          ShrinkPagesToFit = false;
          break;
      }
    }

    [Category("Appearance")]
    public virtual bool BoldSelectedPage
    {
      get { return m_boldSelected; }
			
      set 
      {
        if (m_boldSelected != value)
        {
          m_boldSelected = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializeBoldSelectedPage()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_boldSelected != false;
        case VisualAppearance.MultiDocument:
        default:
          return m_boldSelected != true;
      }
    }
        
    public void ResetBoldSelectedPage()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          BoldSelectedPage = false;
          break;
        case VisualAppearance.MultiDocument:
        default:
          BoldSelectedPage = true;
          break;
      }
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool MultilineFullWidth
    {
      get { return m_multilineFullWidth; }
            
      set
      {
        if (m_multilineFullWidth != value)
        {
          m_multilineFullWidth = value;
                    
          m_recalculate = true;
          Invalidate();
        }
      }
    }
        
    public void ResetMultilineFullWidth()
    {
      MultilineFullWidth = false;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool Multiline
    {
      get { return m_multiline; }
			
      set 
      {
        if (m_multiline != value)
        {
          m_multiline = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }
        
    public void ResetMultiline()
    {
      Multiline = false;
    }

    [Category("Appearance")]
    [DefaultValue(0)]
    public virtual int ControlLeftOffset
    {
      get { return m_ctrlLeftOffset; }
			
      set 
      {
        if (m_ctrlLeftOffset != value)
        {
          m_ctrlLeftOffset = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetControlLeftOffset()
    {
      ControlLeftOffset = 0;
    }

    [Category("Appearance")]
    [DefaultValue(0)]
    public virtual int ControlTopOffset
    {
      get { return m_ctrlTopOffset; }
			
      set 
      {
        if (m_ctrlTopOffset != value)
        {
          m_ctrlTopOffset = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetControlTopOffset()
    {
      ControlTopOffset = 0;
    }

    [Category("Appearance")]
    [DefaultValue(0)]
    public virtual int ControlRightOffset
    {
      get { return m_ctrlRightOffset; }
			
      set 
      {
        if (m_ctrlRightOffset != value)
        {
          m_ctrlRightOffset = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetControlRightOffset()
    {
      ControlRightOffset = 0;
    }

    [Category("Appearance")]
    [DefaultValue(0)]
    public virtual int ControlBottomOffset
    {
      get { return m_ctrlBottomOffset; }
			
      set 
      {
        if (m_ctrlBottomOffset != value)
        {
          m_ctrlBottomOffset = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetControlBottomOffset()
    {
      ControlBottomOffset = 0;
    }

    [Category("Appearance")]
    [DefaultValue(true)]
    public virtual bool InsetPlain
    {
      get { return m_insetPlain; }
			
      set 
      {
        if (m_insetPlain != value)
        {
          m_insetPlain = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetInsetPlain()
    {
      InsetPlain = true;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool InsetBorderPagesOnly
    {
      get { return m_insetBorderPagesOnly; }
			
      set 
      {
        if (m_insetBorderPagesOnly != value)
        {
          m_insetBorderPagesOnly = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetInsetBorderPagesOnly()
    {
      InsetBorderPagesOnly = true;
    }

    [Category("Appearance")]
    public virtual bool IDEPixelBorder
    {
      get { return m_idePixelBorder; }
			
      set 
      {
        if (m_idePixelBorder != value)
        {
          m_idePixelBorder = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializeIDEPixelBorder()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          return m_idePixelBorder != false;
        case VisualAppearance.MultiDocument:
        default:
          return m_idePixelBorder != true;
      }
    }

    public void ResetIDEPixelBorder()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
        case VisualAppearance.MultiForm:
          IDEPixelBorder = false;
          break;
        case VisualAppearance.MultiDocument:
        default:
          IDEPixelBorder = true;
          break;
      }
    }

    [Category("Appearance")]
    [DefaultValue(true)]
    public virtual bool IDEPixelArea
    {
      get { return m_idePixelArea; }
			
      set 
      {
        if (m_idePixelArea != value)
        {
          m_idePixelArea = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    public void ResetIDEPixelArea()
    {
      IDEPixelArea = true;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool SelectedTextOnly
    {
      get { return m_selectedTextOnly; }
			
      set 
      {
        if (m_selectedTextOnly != value)
        {
          m_selectedTextOnly = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    public void ResetSelectedTextOnly()
    {
      SelectedTextOnly = false;
    }

    [Category("Behavour")]
    [DefaultValue(200)]
    public int MouseLeaveTimeout
    {
      get { return m_leaveTimeout; }
            
      set 
      { 
        if (m_leaveTimeout != value)
        {
          m_leaveTimeout = value;
          m_overTimer.Interval = value;
        }
      }
    }

    public void ResetMouseLeaveTimeout()
    {
      m_leaveTimeout = 200;
    }

    [Category("Behavour")]
    [DefaultValue(true)]
    public bool DragFromControl
    {
      get { return m_dragFromControl; }
      set { m_dragFromControl = value ; }
    }
        
    public void ResetDragFromControl()
    {
      DragFromControl = true;
    }
        
    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual HideTabsModes HideTabsMode
    {
      get { return m_hideTabsMode; }
			
      set 
      {
        if (m_hideTabsMode != value)
        {
          m_hideTabsMode = value;

          Recalculate();
          Invalidate();
        }
      }
    }

    protected bool ShouldSerializeHideTabsMode()
    {
      return HideTabsMode != HideTabsModes.ShowAlways;
    }

    public void ResetHideTabsMode()
    {
      HideTabsMode = HideTabsModes.ShowAlways;
    }

    [Category("Appearance")]
    [DefaultValue(false)]
    public virtual bool HoverSelect
    {
      get { return m_hoverSelect; }
			
      set 
      {
        if (m_hoverSelect != value)
        {
          m_hoverSelect = value;

          m_recalculate = true;
          Invalidate();
        }
      }
    }

    public void ResetHoverSelect()
    {
      HoverSelect = false;
    }

    [Category("Behavour")]
    [DefaultValue(true)]
    public virtual bool RecordFocus
    {
      get { return m_recordFocus; }
			
      set 
      {
        if (m_recordFocus != value)
          m_recordFocus = value;
      }
    }

    public void ResetRecordFocus()
    {
      RecordFocus = true;
    }

    [Browsable(false)]
    [DefaultValue(-1)]
    public virtual int SelectedIndex
    {
      get { return m_pageSelected; }

      set
      {
        if ((value >= 0) && (value < m_tabPages.Count))
        {
          if (m_pageSelected != value)
          {
            // Raise selection changing event
            OnSelectionChanging(EventArgs.Empty);

            // Any page currently selected?
            if (m_pageSelected != -1)
              DeselectPage(m_tabPages[m_pageSelected]);

            m_pageSelected = value;

            if (m_pageSelected != -1)
            {
              SelectPage(m_tabPages[m_pageSelected]);

              // If newly selected page is scrolled off the left hand side
              if (m_pageSelected < m_startPage)
                m_startPage = m_pageSelected;  // then bring it into view
            }

            // Change in selection causes tab pages sizes to change
            if (m_boldSelected)
            {
              Recalculate();
              Invalidate();
            }

            // Raise selection change event
            OnSelectionChanged(EventArgs.Empty);

            Invalidate();
          }
        }
      }
    }

    [Browsable(false)]
    [DefaultValue(null)]
    public virtual TabPage SelectedTab
    {
      get 
      {
        // If nothing is selected we return null
        if (m_pageSelected == -1)
          return null;
        else
          return m_tabPages[m_pageSelected];
      }

      set
      {
        // Cannot change selection to be none of the tabs
        if (value != null)
        {
          // Get the requested page from the collection
          int index = m_tabPages.IndexOf(value);

          // If a valid known page then using existing property to perform switch
          if (index != -1)
            this.SelectedIndex = index;
        }
      }
    }

    public virtual void MakePageVisible(TabPage page)
    {
      MakePageVisible(m_tabPages.IndexOf(page));
    }

    public virtual void MakePageVisible(int index)
    {
      // Only relevant if we do not shrink all pages to fit and not in multiline
      if (!m_shrinkPagesToFit && !m_multiline)
      {
        // Range check the request page
        if ((index >= 0) && (index < m_tabPages.Count))
        {
          // Is requested page before those shown?
          if (index < m_startPage)
          {
            // Define it as the new start page
            m_startPage = index;

            m_recalculate = true;
            Invalidate();
          }
          else
          {
            // Find the last visible position
            int xMax = GetMaximumDrawPos();

            Rectangle rect = (Rectangle)m_tabRects[index];

            // Is the page drawn off over the maximum position?
            if (rect.Right >= xMax)
            {
              // Need to find the new start page to bring this one into view
              int newStart = index;

              // Space left over for other tabs to be drawn inside
              int spaceLeft = xMax - rect.Width - m_tabsAreaRect.Left - m_tabsAreaStartInset;

              do
              {
                // Is there a previous tab to check?
                if (newStart == 0)
                  break;

                Rectangle rectStart = (Rectangle)m_tabRects[newStart - 1];
		
                // Is there enough space to draw it?
                if (rectStart.Width > spaceLeft)
                  break;

                // Move to new tab and reduce available space left
                newStart--;
                spaceLeft -= rectStart.Width;

              } while(true);

              // Define the new starting page
              m_startPage = newStart;

              m_recalculate = true;
              Invalidate();
            }
          }
        }
      }
    }

    protected override bool ProcessMnemonic(char key)
    {
      int total = m_tabPages.Count;
      int index = this.SelectedIndex + 1;
            
      for(int count=0; count<total; count++, index++)
      {
        // Range check the index
        if (index >= total)
          index = 0;

        TabPage page = m_tabPages[index];
        
        // Find position of first mnemonic character
        int position = page.Title.IndexOf('&');

        // Did we find a mnemonic indicator?
        if (IsMnemonic(key, page.Title))
        {
          // Select this page
          this.SelectedTab = page;
                
          return true;
        }
      }
                        
      return false;
    }

    protected override void OnResize(EventArgs e)
    {
      Recalculate();
      Invalidate();
            
      base.OnResize(e);
    }

    protected override void OnSizeChanged(EventArgs e)
    {
      Recalculate();
      Invalidate();

      base.OnSizeChanged(e);
    }

    public virtual void OnPopupMenuDisplay(CancelEventArgs e)
    {
      // Has anyone registered for the event?
      if (PopupMenuDisplay != null)
        PopupMenuDisplay(this, e);
    }

    public virtual void OnSelectionChanging(EventArgs e)
    {
      // Has anyone registered for the event?
      if (SelectionChanging != null)
        SelectionChanging(this, e);
    }

    public virtual void OnSelectionChanged(EventArgs e)
    {
      // Has anyone registered for the event?
      if (SelectionChanged != null)
        SelectionChanged(this, e);
    }

    public virtual void OnClosePressed(EventArgs e)
    {
      // Has anyone registered for the event?
      if (ClosePressed != null)
        ClosePressed(this, e);
    }

    public virtual void OnPageGotFocus(EventArgs e)
    {
      // Has anyone registered for the event?
      if (PageGotFocus != null)
        PageGotFocus(this, e);
    }
		
    public virtual void OnPageLostFocus(EventArgs e)
    {
      // Has anyone registered for the event?
      if (PageLostFocus != null)
        PageLostFocus(this, e);
    }

    public virtual void OnPageDragStart(MouseEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageDragStart != null)
        PageDragStart(this, e);
    }

    public virtual void OnPageDragMove(MouseEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageDragMove != null)
        PageDragMove(this, e);
    }

    public virtual void OnPageDragEnd(MouseEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageDragEnd != null)
        PageDragEnd(this, e);
    }

    public virtual void OnPageDragQuit(MouseEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageDragQuit != null)
        PageDragQuit(this, e);
    }
        
    public virtual void OnDoubleClickTab(TabPage page)
    {
      // Has anyone registered for the event?
      if (DoubleClickTab != null)
        DoubleClickTab(this, page);
    }

    protected virtual void OnCloseButton(object sender, EventArgs e)
    {
      OnClosePressed(EventArgs.Empty);
    }

    protected virtual void OnLeftArrow(object sender, EventArgs e)
    {
      // Set starting page back one
      m_startPage--;

      m_recalculate = true;
      Invalidate();
    }
	
    protected virtual void OnRightArrow(object sender, EventArgs e)
    {
      // Set starting page forward one
      m_startPage++;

      m_recalculate = true;
      Invalidate();
    }

    protected virtual void DefineFont(Font newFont)
    {
      // Use base class for storage of value
      base.Font = newFont;
	
      // Update internal height value using Font
      m_textHeight = newFont.Height;

      // Is the font height bigger than the image height?
      if (m_imageHeight >= m_textHeight)
      {
        // No, do not need extra spacing around the image to fit in text
        m_imageGapTopExtra = 0;
        m_imageGapBottomExtra = 0;
      }
      else
      {
        // Yes, need to make the image area bigger so that its height calculation
        // matchs that height of the text
        int extraHeight = m_textHeight - m_imageHeight;

        // Split the extra height between the top and bottom of image
        m_imageGapTopExtra = extraHeight / 2;
        m_imageGapBottomExtra = extraHeight - m_imageGapTopExtra;
      }
    }

    protected virtual void DefineBackColor(Color newColor)
    {
      base.BackColor = newColor;

      // Calculate the modified colors from this base
      m_backLight = ControlPaint.Light(newColor);
      m_backLightLight = ControlPaint.LightLight(newColor);
      m_backDark = ControlPaint.Dark(newColor);
      m_backDarkDark = ControlPaint.DarkDark(newColor);

      m_backIDE = ColorHelper.TabBackgroundFromBaseColor(newColor);
    }
		
    protected virtual void DefineButtonImages()
    {
      ImageAttributes ia = new ImageAttributes();

      ColorMap activeMap = new ColorMap();
      ColorMap inactiveMap = new ColorMap();

      // Define the color transformations needed
      activeMap.OldColor = Color.Black;
      activeMap.NewColor = m_buttonActiveColor;
      inactiveMap.OldColor = Color.White;
      inactiveMap.NewColor = m_buttonInactiveColor;

      // Create remap attributes for use by button
      ia.SetRemapTable(new ColorMap[]{activeMap, inactiveMap}, ColorAdjustType.Bitmap);

      // Pass attributes to the buttons
      m_leftArrow.ImageAttributes = ia;
      m_rightArrow.ImageAttributes = ia;
      m_closeButton.ImageAttributes = ia;
    }

    protected virtual void SetAppearance(VisualAppearance appearance)
    {
      switch(appearance)
      {
        case VisualAppearance.MultiForm:
        case VisualAppearance.MultiBox:
          m_shrinkPagesToFit = true;					// shrink tabs to fit width
          m_positionAtTop = false;						// draw tabs at bottom of control
          m_showClose = false;							// do not show the close button
          m_showArrows = false;						// do not show the scroll arrow buttons
          m_boldSelected = false;						// do not show selected pages in bold
          m_idePixelArea = true;                       // show a one pixel border at top or bottom
          IDEPixelBorder = false;                     // do not show a one pixel border round control
          break;
        case VisualAppearance.MultiDocument:
          m_shrinkPagesToFit = false;					// shrink tabs to fit width
          m_positionAtTop = true;						// draw tabs at bottom of control
          m_showClose = true;							// do not show the close button
          m_showArrows = true;						    // do not show the scroll arrow buttons
          m_boldSelected = true;						// do not show selected pages in bold
          m_idePixelArea = true;                       // show a one pixel border at top or bottom
          IDEPixelBorder = false;                     // do not show a one pixel border round control
          break;
      }

      // These properties are the same whichever style is selected
      m_hotTrack = false;							// do not hot track paes
      m_dimUnselected = true;						// draw dimmed non selected pages

      // Define then starting page for drawing
      if (m_tabPages.Count > 0)
        m_startPage = 0;
      else
        m_startPage = -1;

      m_appearance = appearance;

      // Define the correct style indexer
      SetStyleIndex();
    }

    protected virtual void SetStyleIndex()
    {
      switch(m_appearance)
      {
        case VisualAppearance.MultiBox:
          // Always pretend we are plain style
          m_styleIndex = 1;
          break;
        case VisualAppearance.MultiForm:
        case VisualAppearance.MultiDocument:
          m_styleIndex = (int)m_style;
          break;
      }
    }

    protected virtual bool HideTabsCalculation()
    {
      bool hideTabs = false;
        
      switch(m_hideTabsMode)
      {
        case HideTabsModes.ShowAlways:
          hideTabs = false;
          break;
        case HideTabsModes.HideAlways:
          hideTabs = true;
          break;
        case HideTabsModes.HideUsingLogic:
          hideTabs = (m_tabPages.Count <= 1);                            
          break;
        case HideTabsModes.HideWithoutMouse:
          hideTabs = !m_mouseOver;
          break;
      }
            
      return hideTabs;
    }
		
    protected virtual void Recalculate()
    {
      // Reset the need for a recalculation
      m_recalculate = false;

      // The height of a tab button is...
      int tabButtonHeight = m_position[m_styleIndex, (int)PositionIndex.ImageGapTop] + 
        m_imageGapTopExtra +
        m_imageHeight + 
        m_imageGapBottomExtra + 
        m_position[m_styleIndex, (int)PositionIndex.ImageGapBottom] +
        m_position[m_styleIndex, (int)PositionIndex.BorderBottom]; 

      // The height of the tabs area is...
      int tabsAreaHeight = m_position[m_styleIndex, (int)PositionIndex.BorderTop] + 
        tabButtonHeight + m_position[m_styleIndex, (int)PositionIndex.TabsBottomGap];

      bool hideTabsArea = HideTabsCalculation();

      // Should the tabs area be hidden?
      if (hideTabsArea)
      {
        // ... then do not show the tabs or button controls
        m_pageAreaRect = new Rectangle(0, 0, this.Width, this.Height);
        m_tabsAreaRect = new Rectangle(0, 0, 0, 0);
      }
      else
      {
        if (m_positionAtTop)
        {
          // Create rectangle that represents the entire tabs area
          m_pageAreaRect = new Rectangle(0, tabsAreaHeight, this.Width, this.Height - tabsAreaHeight);

          // Create rectangle that represents the entire area for pages
          m_tabsAreaRect = new Rectangle(0, 0, this.Width, tabsAreaHeight);
        }
        else
        {
          // Create rectangle that represents the entire tabs area
          m_tabsAreaRect = new Rectangle(0, this.Height - tabsAreaHeight, this.Width, tabsAreaHeight);

          // Create rectangle that represents the entire area for pages
          m_pageAreaRect = new Rectangle(0, 0, this.Width, this.Height - tabsAreaHeight);
        }
      }

      int xEndPos = 0;

      if (!hideTabsArea && m_tabPages.Count > 0)
      {
        // The minimum size of a button includes its left and right borders for width,
        // and then fixed height which is based on the size of the image and font
        Rectangle tabPosition;
				
        if (m_positionAtTop)
          tabPosition = new Rectangle(0,		
            m_tabsAreaRect.Bottom - tabButtonHeight - 
            m_position[m_styleIndex, (int)PositionIndex.BorderTop],
            m_position[m_styleIndex, (int)PositionIndex.BorderLeft] + 
            m_position[m_styleIndex, (int)PositionIndex.BorderRight],
            tabButtonHeight);
        else
          tabPosition = new Rectangle(0,		
            m_tabsAreaRect.Top + 
            m_position[m_styleIndex, (int)PositionIndex.BorderTop],
            m_position[m_styleIndex, (int)PositionIndex.BorderLeft] + 
            m_position[m_styleIndex, (int)PositionIndex.BorderRight],
            tabButtonHeight);

        // Find starting and ending positons for drawing tabs
        int xStartPos = m_tabsAreaRect.Left + m_tabsAreaStartInset;
        xEndPos = GetMaximumDrawPos();

        // Available width for tabs is size between start and end positions
        int xWidth = xEndPos - xStartPos;

        if (m_multiline)
          RecalculateMultilineTabs(xStartPos, xEndPos, tabPosition, tabButtonHeight);
        else
          RecalculateSinglelineTabs(xWidth, xStartPos, tabPosition);
      }

      // Position of Controls defaults to the entire page area
      m_pageRect = m_pageAreaRect;

      // Adjust child controls positions depending on style
      if ((m_style == VisualStyle.Plain) && (m_appearance != VisualAppearance.MultiBox))
      {
        m_pageRect = m_pageAreaRect;

        // Shrink by having a border on left,top and right borders
        m_pageRect.X += m_plainBorderDouble;
        m_pageRect.Width -= (m_plainBorderDouble * 2) - 1;

        if (!m_positionAtTop)
          m_pageRect.Y += m_plainBorderDouble;

        m_pageRect.Height -= m_plainBorderDouble - 1;
				
        // If hiding the tabs then need to adjust the controls positioning
        if (hideTabsArea)
        {
          m_pageRect.Height -= m_plainBorderDouble;

          if (m_positionAtTop)
            m_pageRect.Y += m_plainBorderDouble;
        }
      }

      // Calcualte positioning of the child controls/forms
      int leftOffset = m_ctrlLeftOffset;
      int rightOffset = m_ctrlRightOffset;
      int topOffset = m_ctrlTopOffset;
      int bottomOffset = m_ctrlBottomOffset;

      if (m_idePixelBorder && (m_style == VisualStyle.IDE))
      {
        leftOffset += 2;
        rightOffset += 2;

        if (m_positionAtTop || hideTabsArea)
          bottomOffset += 2;
                    
        if (!m_positionAtTop || hideTabsArea)
          topOffset += 2;
      }
        
      Point pageLoc = new Point(m_pageRect.Left + leftOffset,
        m_pageRect.Top + topOffset);

      Size pageSize = new Size(m_pageRect.Width - leftOffset - rightOffset,
        m_pageRect.Height - topOffset - bottomOffset);

      // If in Plain style and requested to only show top or bottom border
      if ((m_style == VisualStyle.Plain) && m_insetBorderPagesOnly)
      {
        // Then need to increase width to occupy where borders would have been 
        pageLoc.X -= m_plainBorderDouble;
        pageSize.Width += m_plainBorderDouble * 2;

        if (hideTabsArea || m_positionAtTop)
        {
          // Draw into the bottom border area
          pageSize.Height += m_plainBorderDouble;
        }

        if (hideTabsArea || !m_positionAtTop)
        {
          // Draw into the top border area
          pageLoc.Y -= m_plainBorderDouble;
          pageSize.Height += m_plainBorderDouble;
        }
      }

      // Position the host panel appropriately
      m_hostPanel.Size = pageSize;
      m_hostPanel.Location = pageLoc;
            
      // If we have any tabs at all
      if (m_tabPages.Count > 0)
      {
        Rectangle rect = (Rectangle)m_tabRects[m_tabPages.Count - 1];
				
        // Determine is the right scrolling button should be enabled
        m_rightScroll = (rect.Right > xEndPos);
      }
      else
      {
        // No pages means there can be no right scrolling
        m_rightScroll = false;
      }

      // Determine if left scrolling is possible
      m_leftScroll = (m_startPage > 0);

      // Handle then display and positioning of buttons
      RecalculateButtons();
    }

    protected virtual void RecalculateMultilineTabs(int xStartPos, int xEndPos, 
      Rectangle tabPosition, int tabButtonHeight)
    {
      using (Graphics g = this.CreateGraphics())
      {
        // MultiBox style needs a pixel extra drawing room on right
        if (m_appearance == VisualAppearance.MultiBox)
          xEndPos-=2;
                        
        // How many tabs on this line
        int lineCount = 0;
                            
        // Remember which line is the first displayed
        m_topYPos = tabPosition.Y;
                            
        // Next tab starting position
        int xPos = xStartPos;
        int yPos = tabPosition.Y;
                            
        // How many full lines were there
        int fullLines = 0;
                            
        // Line increment value
        int lineIncrement = tabButtonHeight + 1;

        // Track which line has the selection on it                                
        int selectedLine = 0;

        // Vertical adjustment
        int yAdjust = 0;
                        
        // Create array for holding lines of tabs
        ArrayList lineList = new ArrayList();
                            
        // Add the initial line
        lineList.Add(new ArrayList());
                        
        // Process each tag page in turn
        for(int i=0; i<m_tabPages.Count; i++)
        {
          // Get the tab instance for this position
          TabPage page = m_tabPages[i];
                            
          // Find out the tabs total width
          int tabWidth = GetTabPageSpace(g, page);
                                
          // If not the first on the line, then check if newline should be started
          if (lineCount > 0)
          {
            // Does this tab extend pass end of the lines
            if ((xPos + tabWidth) > xEndPos)
            {
              // Next tab position is down a line and back to the start
              xPos = xStartPos;
              yPos += lineIncrement;
                                        
              // Remember which line is the last displayed
              m_bottomYPos = tabPosition.Y;

              // Increase height of the tabs area
              m_tabsAreaRect.Height += lineIncrement;
                                        
              // Decrease height of the control area
              m_pageAreaRect.Height -= lineIncrement;
                                        
              // Moving areas depends on drawing at top or bottom
              if (m_positionAtTop)
                m_pageAreaRect.Y += lineIncrement;
              else
              {
                yAdjust -= lineIncrement;
                m_tabsAreaRect.Y -= lineIncrement;
              }
                                        
              // Start a new line 
              lineList.Add(new ArrayList());
                                        
              // Make sure the entries are aligned to fill entire line
              fullLines++;
            }
          }

          // Limit the width of a tab to the whole line
          if (tabWidth > (xEndPos - xStartPos))
            tabWidth = xEndPos - xStartPos;
                                
          // Construct rectangle for representing this tab
          Rectangle tabRect = new Rectangle(xPos, yPos, tabWidth, tabButtonHeight);
                                
          // Add this tab to the current line array
          ArrayList thisLine = lineList[lineList.Count - 1] as ArrayList;
                                
          // Create entry to represent the sizing of the given page index
          MultiRect tabEntry = new MultiRect(tabRect, i);
                                
          thisLine.Add(tabEntry);
                                
          // Track which line has the selection on it
          if (i == m_pageSelected)
            selectedLine = fullLines;
                                
          // Move position of next tab along
          xPos += tabWidth + 1;
                                
          // Increment number of tabs on this line
          lineCount++;
        }

        int line = 0;

        // Do we need all lines to extend full width
        if (m_multilineFullWidth)
          fullLines++;
                            
        // Make each full line stretch the whole line width
        foreach(ArrayList lineArray in lineList)
        {
          // Only right fill the full lines
          if (line < fullLines)
          {
            // Number of items on this line
            int numLines = lineArray.Count;
                                
            // Find ending position of last entry
            MultiRect itemEntry = (MultiRect)lineArray[numLines - 1];

            // Is there spare room between last entry and end of line?                            
            if (itemEntry.Rect.Right < (xEndPos - 1))
            {
              // Work out how much extra to give each one
              int extra = (int)((xEndPos - itemEntry.Rect.Right - 1) / numLines);
                                        
              // Keep track of how much items need moving across
              int totalMove = 0;
                                        
              // Add into each entry
              for(int i=0; i<numLines; i++)
              {
                // Get the entry class instance
                MultiRect expandEntry = (MultiRect)lineArray[i];
                                        
                // Move across requried amount
                expandEntry.X += totalMove;
                                            
                // Add extra width
                expandEntry.Width += (int)extra;

                // All items after this needing moving
                totalMove += extra;
              }
                                        
              // Extend the last position, in case rounding errors means its short
              itemEntry.Width += (xEndPos - itemEntry.Rect.Right - 1);
            }
          }
                                
          line++;
        }

        if (m_positionAtTop)
        {
          // If the selected line is not the bottom line
          if (selectedLine != (lineList.Count - 1))
          {
            ArrayList lastLine = (ArrayList)(lineList[lineList.Count - 1]);
                                    
            // Find y offset of last line
            int lastOffset = ((MultiRect)lastLine[0]).Rect.Y;
                                
            // Move all lines below it up one
            for(int lineIndex=selectedLine+1; lineIndex<lineList.Count; lineIndex++)
            {
              ArrayList al = (ArrayList)lineList[lineIndex];
                                    
              for(int item=0; item<al.Count; item++)
              {
                MultiRect itemEntry = (MultiRect)al[item];
                itemEntry.Y -= lineIncrement;
              }
            }
                                    
            // Move selected line to the bottom
            ArrayList sl = (ArrayList)lineList[selectedLine];
                                    
            for(int item=0; item<sl.Count; item++)
            {
              MultiRect itemEntry = (MultiRect)sl[item];
              itemEntry.Y = lastOffset;
            }
          }
        }
        else
        {
          // If the selected line is not the top line
          if (selectedLine != 0)
          {
            ArrayList topLine = (ArrayList)(lineList[0]);
                                    
            // Find y offset of top line
            int topOffset = ((MultiRect)topLine[0]).Rect.Y;
                                
            // Move all lines above it down one
            for(int lineIndex=0; lineIndex<selectedLine; lineIndex++)
            {
              ArrayList al = (ArrayList)lineList[lineIndex];
                                    
              for(int item=0; item<al.Count; item++)
              {
                MultiRect itemEntry = (MultiRect)al[item];
                itemEntry.Y += lineIncrement;
              }
            }
                                    
            // Move selected line to the top
            ArrayList sl = (ArrayList)lineList[selectedLine];
                                    
            for(int item=0; item<sl.Count; item++)
            {
              MultiRect itemEntry = (MultiRect)sl[item];
              itemEntry.Y = topOffset;
            }
          }
        }

        // Now assignt each lines rectangle to the corresponding structure
        foreach(ArrayList al in lineList)
        {
          foreach(MultiRect multiEntry in al)
          {
            Rectangle newRect = multiEntry.Rect;
                                    
            // Make the vertical adjustment
            newRect.Y += yAdjust;
                                    
            m_tabRects[multiEntry.Index] = newRect;
          }
        }
      }
    }

    protected virtual void RecalculateSinglelineTabs(int xWidth, int xStartPos, Rectangle tabPosition)
    {
      using (Graphics g = this.CreateGraphics())
      {
        // Remember which lines are then first and last displayed
        m_topYPos = tabPosition.Y;
        m_bottomYPos = m_topYPos;
            
        // Set the minimum size for each tab page
        for(int i=0; i<m_tabPages.Count; i++)
        {
          // Is this page before those displayed?
          if (i < m_startPage)
            m_tabRects[i] = (object)m_nullPosition;  // Yes, position off screen
          else
            m_tabRects[i]= (object)tabPosition;	 // No, create minimum size
        }

        // Subtract the minimum tab sizes already allocated
        xWidth -= m_tabPages.Count * (tabPosition.Width + 1);

        // Is there any more space left to allocate
        if (xWidth > 0)
        {
          ArrayList listNew = new ArrayList();
          ArrayList listOld = new ArrayList();

          // Add all pages to those that need space allocating
          for(int i=m_startPage; i<m_tabPages.Count; i++)
            listNew.Add(m_tabPages[i]);
            		
          // Each tab can have an allowance
          int xAllowance;
            		
          do 
          {
            // The list generated in the last iteration becomes 
            // the to be processed in this iteration
            listOld = listNew;
            	
            // List of pages that still need more space allocating
            listNew = new ArrayList();

            if (m_shrinkPagesToFit)
            {
              // Each page is allowed a maximum allowance of space
              // during this iteration. 
              xAllowance = xWidth / m_tabPages.Count;
            }
            else
            {
              // Allow each page as much space as it wants
              xAllowance = 999;
            }

            // Assign space to each page that is requesting space
            foreach(TabPage page in listOld)
            {
              int index = m_tabPages.IndexOf(page);

              Rectangle rectPos = (Rectangle)m_tabRects[index];

              // Find out how much extra space this page is requesting
              int xSpace = GetTabPageSpace(g, page) - rectPos.Width;

              // Does it want more space than its currently allowed to have?
              if (xSpace > xAllowance)
              {
                // Restrict allowed space
                xSpace = xAllowance;

                // Add page to ensure it gets processed next time around
                listNew.Add(page);
              }

              // Give space to tab
              rectPos.Width += xSpace;

              m_tabRects[index] = (object)rectPos;

              // Reduce extra left for remaining tabs
              xWidth -= xSpace;
            }
          } while ((listNew.Count > 0) && (xAllowance > 0) && (xWidth > 0));
        }

        // Assign the final positions to each tab now we known their sizes
        for(int i=m_startPage; i<m_tabPages.Count; i++)
        {
          Rectangle rectPos = (Rectangle)m_tabRects[i];

          // Define position of tab page
          rectPos.X = xStartPos;

          m_tabRects[i] = (object)rectPos;

          // Next button must be the width of this one across
          xStartPos += rectPos.Width + 1;
        }
      }
    }

    protected virtual void RecalculateButtons()
    {
      int buttonTopGap = 0;
            
      if (m_multiline)
      {
        // The height of a tab row is
        int tabButtonHeight = m_position[m_styleIndex, (int)PositionIndex.ImageGapTop] + 
          m_imageGapTopExtra +
          m_imageHeight + 
          m_imageGapBottomExtra + 
          m_position[m_styleIndex, (int)PositionIndex.ImageGapBottom] +
          m_position[m_styleIndex, (int)PositionIndex.BorderBottom]; 
        
        // The height of the tabs area is...
        int tabsAreaHeight = m_position[m_styleIndex, (int)PositionIndex.BorderTop] + 
          tabButtonHeight + m_position[m_styleIndex, (int)PositionIndex.TabsBottomGap];
                
        // Find offset to place button halfway down the tabs area rectangle
        buttonTopGap = m_position[m_styleIndex, (int)PositionIndex.ButtonOffset]	+ 
          (tabsAreaHeight - m_buttonHeight) / 2;

        // Invert gap position when at bottom
        if (!m_positionAtTop)
          buttonTopGap = m_tabsAreaRect.Height - buttonTopGap - m_buttonHeight;
      }
      else
      {
        // Find offset to place button halfway down the tabs area rectangle
        buttonTopGap = m_position[m_styleIndex, (int)PositionIndex.ButtonOffset]	+ 
          (m_tabsAreaRect.Height - m_buttonHeight) / 2;
      }
        
      // Position to place next button
      int xStart = m_tabsAreaRect.Right - m_buttonWidth - m_buttonGap;

      // Close button should be shown?
      if (m_showClose)
      {
        // Define the location
        m_closeButton.Location = new Point(xStart, m_tabsAreaRect.Top + buttonTopGap);

        if (xStart < 1)
          m_closeButton.Hide();
        else
          m_closeButton.Show();

        xStart -= m_buttonWidth;
      }
      else
        m_closeButton.Hide();

      // Arrows should be shown?
      if (m_showArrows)
      {
        // Position the right arrow first as its more the right hand side
        m_rightArrow.Location = new Point(xStart, m_tabsAreaRect.Top + buttonTopGap);

        if (xStart < 1)
          m_rightArrow.Hide();
        else
          m_rightArrow.Show();
                    
        xStart -= m_buttonWidth;

        m_leftArrow.Location = new Point(xStart, m_tabsAreaRect.Top + buttonTopGap);

        if (xStart < 1)
          m_leftArrow.Hide();
        else
          m_leftArrow.Show();
                    
        xStart -= m_buttonWidth;

        // Define then enabled state of buttons
        m_leftArrow.Enabled = m_leftScroll;
        m_rightArrow.Enabled = m_rightScroll;
      }
      else
      {
        m_leftArrow.Hide();
        m_rightArrow.Hide();
      }

      if ((m_appearance == VisualAppearance.MultiBox) || (m_style == VisualStyle.Plain))
        m_closeButton.BackColor = m_leftArrow.BackColor = m_rightArrow.BackColor = this.BackColor;
      else
        m_closeButton.BackColor = m_leftArrow.BackColor = m_rightArrow.BackColor = m_backIDE;
    }

    protected virtual int GetMaximumDrawPos()
    {
      int xEndPos = m_tabsAreaRect.Right - m_tabsAreaEndInset;

      // Showing the close button reduces available space
      if (m_showClose)
        xEndPos -= m_buttonWidth + m_buttonGap;

      // If showing arrows then reduce space for both
      if (m_showArrows)
        xEndPos -= m_buttonWidth * 2;
				
      return xEndPos;
    }

    protected virtual int GetTabPageSpace(Graphics g, TabPage page)
    {
      // Find the fixed elements of required space
      int width = m_position[m_styleIndex, (int)PositionIndex.BorderLeft] + 
        m_position[m_styleIndex, (int)PositionIndex.BorderRight];

      // Any icon or image provided?
      if ((page.Icon != null) || (((m_imageList != null) || (page.ImageList != null)) && (page.ImageIndex != -1)))
      {
        width += m_position[m_styleIndex, (int)PositionIndex.ImageGapLeft] +
          m_imageWidth + 
          m_position[m_styleIndex, (int)PositionIndex.ImageGapRight];
      }

      // Any text provided?
      if (page.Title.Length > 0)
      {
        if (!m_selectedTextOnly || (m_selectedTextOnly && (m_pageSelected == m_tabPages.IndexOf(page))))
        {
          Font drawFont = base.Font;

          if (m_boldSelected && page.Selected)
            drawFont = new Font(drawFont, FontStyle.Bold);

          // Find width of the requested text
          SizeF dimension = g.MeasureString(page.Title, drawFont);			

          // Convert to integral
          width += m_position[m_styleIndex, (int)PositionIndex.TextGapLeft] +
            (int)dimension.Width + 1; 
        }						 
      }

      return width;
    }

    protected override void OnPaintBackground(PaintEventArgs e)
    {
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      // Does the state need recalculating before paint can occur?
      if (m_recalculate)
        Recalculate();

      using (SolidBrush pageAreaBrush = new SolidBrush(this.BackColor))
      {
        // Fill backgrounds of the page and tabs areas
        e.Graphics.FillRectangle(pageAreaBrush, m_pageAreaRect);

        if ((m_style == VisualStyle.Plain) || (m_appearance == VisualAppearance.MultiBox))
        {
          e.Graphics.FillRectangle(pageAreaBrush, m_tabsAreaRect);
        }
        else
        {
          using(SolidBrush tabsAreaBrush = new SolidBrush(m_backIDE))
            e.Graphics.FillRectangle(tabsAreaBrush, m_tabsAreaRect);
        }
      }

      // MultiBox appearance does not have any borders
      if (m_appearance != VisualAppearance.MultiBox)
      {
        bool hiddenPages = HideTabsCalculation();

        // Draw the borders
        switch(m_style)
        {
          case VisualStyle.Plain:
            // Height for drawing the border is size of the page area extended 
            // down to draw the bottom border inside the tabs area
            int pageHeight = m_pageAreaRect.Height + m_plainBorderDouble;
	
            int xDraw = m_pageAreaRect.Top;

            // Should the tabs area be hidden?
            if (hiddenPages)
            {
              // Then need to readjust pageHeight
              pageHeight -= m_plainBorderDouble;
            }
            else
            {
              // If drawing at top then overdraw upwards and not down
              if (m_positionAtTop)
                xDraw -= m_plainBorderDouble;
            }

            if (m_insetBorderPagesOnly)
            {
              if (!hiddenPages)
              {
                // Draw the outer border around the page area			
                DrawHelper.DrawPlainRaisedBorderTopOrBottom(e.Graphics, new Rectangle(0, xDraw, this.Width, pageHeight),
                  m_backLightLight, base.BackColor, m_backDark, m_backDarkDark, m_positionAtTop);
              }
            }
            else
            {
              // Draw the outer border around the page area			
              DrawHelper.DrawPlainRaisedBorder(e.Graphics, new Rectangle(m_pageAreaRect.Left, xDraw, m_pageAreaRect.Width, pageHeight),
                m_backLightLight, base.BackColor, m_backDark, m_backDarkDark);
            }

            // Do we have any tabs?
            if ((m_tabPages.Count > 0) && m_insetPlain)
            {
              // Draw the inner border around the page area
              Rectangle inner = new Rectangle(m_pageAreaRect.Left + m_plainBorder,
                xDraw + m_plainBorder,
                m_pageAreaRect.Width - m_plainBorderDouble,
                pageHeight - m_plainBorderDouble);

              if (m_insetBorderPagesOnly)
              {
                if (!hiddenPages)
                {
                  DrawHelper.DrawPlainSunkenBorderTopOrBottom(e.Graphics, new Rectangle(0, inner.Top, this.Width, inner.Height),
                    m_backLightLight, base.BackColor, m_backDark, m_backDarkDark, m_positionAtTop);
                }                                                                       
              }
              else
              {
                DrawHelper.DrawPlainSunkenBorder(e.Graphics, new Rectangle(inner.Left, inner.Top, inner.Width, inner.Height),
                  m_backLightLight, base.BackColor, m_backDark, m_backDarkDark);
              }

            }						
            break;

          case VisualStyle.IDE:
            // Draw the top and bottom borders to the tabs area
            using (Pen darkdark = new Pen(m_backDarkDark),
                     dark = new Pen(m_backDark),
                     lightlight = new Pen(m_backLightLight),
                     backColor = new Pen(base.BackColor))
            {
              int borderGap = m_position[m_styleIndex, (int)PositionIndex.BorderTop];

              if (m_positionAtTop)
              {
                // Fill the border between the tabs and the embedded controls
                using(SolidBrush backBrush = new SolidBrush(base.BackColor))
                  e.Graphics.FillRectangle(backBrush, 0, m_tabsAreaRect.Bottom - borderGap, m_tabsAreaRect.Width, borderGap);

                int indent = 0;

                // Is a single pixel border required around whole area?                            
                if (m_idePixelBorder)
                {
                  using(Pen llFore = new Pen(ControlPaint.LightLight(this.ForeColor)))
                    e.Graphics.DrawRectangle(dark, 0, 0, this.Width - 1, this.Height - 1);
                                        
                  indent++;
                }
                else
                {
                  if (m_idePixelArea)
                  {
                    // Draw top border
                    e.Graphics.DrawLine(dark, 0, m_tabsAreaRect.Top, m_tabsAreaRect.Width, m_tabsAreaRect.Top);
                  }
                }
                                
                // Draw bottom border
                if (!hiddenPages)
                  e.Graphics.DrawLine(lightlight, indent, 
                    m_tabsAreaRect.Bottom - borderGap, 
                    m_tabsAreaRect.Width - (indent * 2), 
                    m_tabsAreaRect.Bottom - borderGap);
              }
              else
              {
                // Fill the border between the tabs and the embedded controls
                using(SolidBrush backBrush = new SolidBrush(base.BackColor))
                  e.Graphics.FillRectangle(backBrush, 0, m_tabsAreaRect.Top, m_tabsAreaRect.Width, borderGap);

                int indent = 0;

                // Is a single pixel border required around whole area?                            
                if (m_idePixelBorder)
                {
                  using(Pen llFore = new Pen(ControlPaint.LightLight(this.ForeColor)))
                    e.Graphics.DrawRectangle(dark, 0, 0, this.Width - 1, this.Height - 1);
                                        
                  indent++;
                }
                else
                {
                  if (m_idePixelArea)
                  {
                    // Draw bottom border
                    e.Graphics.DrawLine(backColor, 0, m_tabsAreaRect.Bottom - 1, m_tabsAreaRect.Width, m_tabsAreaRect.Bottom - 1);
                  }
                }
                                
                // Draw top border
                if (!hiddenPages)
                  e.Graphics.DrawLine(darkdark, indent, 
                    m_tabsAreaRect.Top + 2, 
                    m_tabsAreaRect.Width - (indent * 2), 
                    m_tabsAreaRect.Top + 2);
              }
            }
            break;
        }
      }

      // Clip the drawing to prevent drawing in unwanted areas
      ClipDrawingTabs(e.Graphics);

      // Paint each tab page
      foreach(TabPage page in m_tabPages)
        DrawTab(page, e.Graphics, false);
    }

    protected virtual Rectangle ClippingRectangle()
    {
      // Calculate how much to reduce width by for clipping rectangle
      int xReduce = m_tabsAreaRect.Width - GetMaximumDrawPos();

      // Create clipping rect
      return new Rectangle(m_tabsAreaRect.Left, 
        m_tabsAreaRect.Top,
        m_tabsAreaRect.Width - xReduce,
        m_tabsAreaRect.Height);
    }

    protected virtual void ClipDrawingTabs(Graphics g)
    {
      Rectangle clipRect = ClippingRectangle();

      // Restrict drawing to this clipping rectangle
      g.Clip = new Region(clipRect);
    }

    protected virtual void DrawTab(TabPage page, Graphics g, bool highlightText)
    {
      Rectangle rectTab = (Rectangle)m_tabRects[m_tabPages.IndexOf(page)];

      DrawTabBorder(ref rectTab, page, g);

      int xDraw = rectTab.Left + m_position[m_styleIndex, (int)PositionIndex.BorderLeft];
      int xMax = rectTab.Right - m_position[m_styleIndex, (int)PositionIndex.BorderRight];

      DrawTabImage(rectTab, page, g, xMax, ref xDraw);            
      DrawTabText(rectTab, page, g, highlightText, xMax, xDraw);
    }

    protected virtual void DrawTabImage(Rectangle rectTab, 
      TabPage page, 
      Graphics g, 
      int xMax, 
      ref int xDraw)
    {
      // Default to using the Icon from the page
      Icon drawIcon = page.Icon;
      Image drawImage = null;
			
      // If there is no valid Icon and the page is requested an image list index...
      if ((drawIcon == null) && (page.ImageIndex != -1))
      {
        try
        {
          // Default to using an image from the TabPage
          ImageList imageList = page.ImageList;

          // If page does not have an ImageList...
          if (imageList == null)
            imageList = m_imageList;   // ...then use the TabControl one

          // Do we have an ImageList to select from?
          if (imageList != null)
          {
            // Grab the requested image
            drawImage = imageList.Images[page.ImageIndex];
          }
        }
        catch(Exception)
        {
          // User supplied ImageList/ImageIndex are invalid, use an error image instead
          drawImage = m_internalImages.Images[(int)ImageStrip.Error];
        }
      }

      // Draw any image required
      if ((drawImage != null) || (drawIcon != null))
      {
        // Enough room to draw any of the image?
        if ((xDraw + m_position[m_styleIndex, (int)PositionIndex.ImageGapLeft]) <= xMax)
        {
          // Move past the left image gap
          xDraw += m_position[m_styleIndex, (int)PositionIndex.ImageGapLeft];

          // Find down position for drawing the image
          int yDraw = rectTab.Top + 
            m_position[m_styleIndex, (int)PositionIndex.ImageGapTop] + 
            m_imageGapTopExtra;

          // If there is enough room for all of the image?
          if ((xDraw + m_imageWidth) <= xMax)
          {
            if (drawImage != null)
              g.DrawImage(drawImage, new Rectangle(xDraw, yDraw, m_imageWidth, m_imageHeight));
            else
              g.DrawIcon(drawIcon, new Rectangle(xDraw, yDraw, m_imageWidth, m_imageHeight));

            // Move past the image and the image gap to the right
            xDraw += m_imageWidth + m_position[m_styleIndex, (int)PositionIndex.ImageGapRight];
          }
          else
          {
            // Calculate how much room there is
            int xSpace = xMax - xDraw;

            // Any room at all?
            if (xSpace > 0)
            {
              if (drawImage != null)
              {
                // Draw only part of the image
                g.DrawImage(drawImage, 
                  new Point[]{new Point(xDraw, yDraw), 
                               new Point(xDraw + xSpace, yDraw), 
                               new Point(xDraw, yDraw + m_imageHeight)},
                  new Rectangle(0, 0, xSpace, 
                  m_imageHeight),
                  GraphicsUnit.Pixel);
              }
                            
              // All space has been used up, nothing left for text
              xDraw = xMax;
            }
          }
        }
      }
    }

    protected virtual void DrawTabText(Rectangle rectTab, 
      TabPage page, 
      Graphics g, 
      bool highlightText,
      int xMax, 
      int xDraw)
    {
      if (!m_selectedTextOnly || (m_selectedTextOnly && page.Selected))
      {
        // Any space for drawing text?
        if (xDraw < xMax)
        {
          Color drawColor;
          SolidBrush drawBrush;
          Font drawFont = base.Font;

          // Decide which base color to use
          if (highlightText)
            drawColor = m_hotTextColor;
          else
          {
            // Do we modify base color depending on selection?
            if (m_dimUnselected && !page.Selected)
            {
              // Reduce the intensity of the color
              drawColor = m_textInactiveColor;
            }
            else
              drawColor = m_textColor;
          }


          // Should selected items be drawn in bold?
          if (m_boldSelected && page.Selected)
            drawFont = new Font(drawFont, FontStyle.Bold);

          System.Console.WriteLine("DrawText {0}", drawColor.ToString());

          // Now the color is determined, create solid brush
          drawBrush = new SolidBrush(drawColor);

          // Ensure only a single line is draw from then left hand side of the
          // rectangle and if to large for line to shows ellipsis for us
          StringFormat drawFormat = new StringFormat();
          drawFormat.FormatFlags = StringFormatFlags.NoClip | StringFormatFlags.NoWrap;
          drawFormat.Trimming = StringTrimming.EllipsisCharacter;
          drawFormat.Alignment = StringAlignment.Center;
          drawFormat.HotkeyPrefix = HotkeyPrefix.Show;

          // Find the vertical drawing limits for text
          int yStart = rectTab.Top + m_position[m_styleIndex, (int)PositionIndex.ImageGapTop];

          int yEnd = rectTab.Bottom - 
            m_position[m_styleIndex, (int)PositionIndex.ImageGapBottom] - 
            m_position[m_styleIndex, (int)PositionIndex.BorderBottom];

          // Use text offset to adjust position of text
          yStart += m_position[m_styleIndex, (int)PositionIndex.TextOffset];

          // Across the text left gap
          xDraw += m_position[m_styleIndex, (int)PositionIndex.TextGapLeft];

          // Need at least 1 pixel width before trying to draw
          if (xDraw < xMax)
          {
            // Find drawing rectangle
            Rectangle drawRect = new Rectangle(xDraw, yStart, xMax - xDraw, yEnd - yStart);

            // Finally....draw the string!
            g.DrawString(page.Title, drawFont, drawBrush, drawRect, drawFormat);
          }
                        	
          // Cleanup resources!
          drawBrush.Dispose();
        }
      }
    }

    protected virtual void DrawTabBorder(ref Rectangle rectTab, TabPage page, Graphics g)
    {
      if (m_appearance == VisualAppearance.MultiBox)
      {
        // Adjust the drawing upwards two pixels to 'look pretty'
        rectTab.Y -= m_multiBoxAdjust;

        // Draw the same regardless of style
        DrawMultiBoxBorder(page, g, rectTab);
      }
      else
      {
        // Drawing the border is style specific
        switch(m_style)
        {
          case VisualStyle.Plain:
            DrawPlainTabBorder(page, g, rectTab);
            break;				
          case VisualStyle.IDE:
            DrawIDETabBorder(page, g, rectTab);
            break;				
        }
      }
    }
		
    protected virtual void DrawMultiBoxBorder(TabPage page, Graphics g, Rectangle rectPage)
    {
      if (page.Selected)
      {
        using (SolidBrush lightlight = new SolidBrush(m_backLightLight))
          g.FillRectangle(lightlight, rectPage);

        using (Pen darkdark = new Pen(m_backDarkDark))
          g.DrawRectangle(darkdark, rectPage);
      }
      else
      {
        using (SolidBrush backBrush = new SolidBrush(this.BackColor))
          g.FillRectangle(backBrush, rectPage.X + 1, rectPage.Y, rectPage.Width - 1, rectPage.Height);
            
        // Find the index into TabPage collection for this page
        int index = m_tabPages.IndexOf(page);

        // Decide if the separator should be drawn
        bool drawSeparator = (index == m_tabPages.Count - 1) ||
          (index < (m_tabPages.Count - 1)) && 
          (m_tabPages[index+1].Selected != true);
                                         
        // MultiLine mode is slighty more complex
        if (m_multiline && !drawSeparator)
        {
          // By default always draw separator
          drawSeparator = true;
                    
          // If we are not the last item
          if (index < (m_tabPages.Count - 1))
          {
            // If the next item is selected
            if (m_tabPages[index+1].Selected == true)
            {
              Rectangle thisRect = (Rectangle)m_tabRects[index];
              Rectangle nextRect = (Rectangle)m_tabRects[index+1];

              // If we are on the same drawing line then do not draw separator
              if (thisRect.Y == nextRect.Y)
                drawSeparator = false;
            }
          }
        }

        // Draw tab separator unless the next page after us is selected
        if (drawSeparator)
        {
          using(Pen lightlight = new Pen(m_backLightLight),
                  dark = new Pen(m_backDark))
          {
            g.DrawLine(dark, rectPage.Right, rectPage.Top + 2, rectPage.Right, 
              rectPage.Bottom - m_position[m_styleIndex, (int)PositionIndex.TabsBottomGap] - 1);
            g.DrawLine(lightlight, rectPage.Right+1, rectPage.Top + 2, rectPage.Right+1, 
              rectPage.Bottom - m_position[m_styleIndex, (int)PositionIndex.TabsBottomGap] - 1);
          }
        }
      }
    }

    protected virtual void DrawPlainTabBorder(TabPage page, Graphics g, Rectangle rectPage)
    {
      using(Pen light = new Pen(m_backLightLight),
              dark = new Pen(m_backDark),
              darkdark = new Pen(m_backDarkDark))
      {
        int yLeftOffset = 0;
        int yRightOffset = 0;

        using (SolidBrush backBrush = new SolidBrush(base.BackColor))
        {
          if (page.Selected)
          {
            // Calculate the rectangle that covers half the top border area
            int yBorder;
						
            if (m_positionAtTop)
              yBorder = rectPage.Top + (m_position[m_styleIndex, (int)PositionIndex.BorderTop] / 2);
            else
              yBorder = rectPage.Top - (m_position[m_styleIndex, (int)PositionIndex.BorderTop] / 2);

            // Construct rectangle that covers the outer part of the border
            Rectangle rectBorder = new Rectangle(rectPage.Left, yBorder, rectPage.Width - 1, rectPage.Height);

            // Blank out area 
            g.FillRectangle(backBrush, rectBorder);

            // Make the left and right border lines extend higher up
            yLeftOffset = -2;
            yRightOffset = -1;
          }
        }

        if (m_positionAtTop)
        {
          // Draw the left border
          g.DrawLine(light, rectPage.Left, rectPage.Bottom, rectPage.Left, rectPage.Top + 2); 
          g.DrawLine(light, rectPage.Left + 1 , rectPage.Top + 1, rectPage.Left + 1, rectPage.Top + 2); 

          // Draw the top border
          g.DrawLine(light, rectPage.Left + 2, rectPage.Top + 1, rectPage.Right - 2, rectPage.Top + 1); 

          // Draw the right border
          g.DrawLine(darkdark, rectPage.Right, rectPage.Bottom - yRightOffset, rectPage.Right, rectPage.Top + 2); 
          g.DrawLine(dark, rectPage.Right - 1, rectPage.Bottom - yRightOffset, rectPage.Right - 1, rectPage.Top + 2); 
          g.DrawLine(dark, rectPage.Right - 2, rectPage.Top + 1, rectPage.Right - 2, rectPage.Top + 2); 
          g.DrawLine(darkdark, rectPage.Right - 2, rectPage.Top, rectPage.Right, rectPage.Top + 2);
        }
        else
        {
          // Draw the left border
          g.DrawLine(light, rectPage.Left, rectPage.Top + yLeftOffset, rectPage.Left, rectPage.Bottom - 2); 
          g.DrawLine(dark, rectPage.Left + 1 , rectPage.Bottom - 1, rectPage.Left + 1, rectPage.Bottom - 2); 

          // Draw the bottom border
          g.DrawLine(dark, rectPage.Left + 2, rectPage.Bottom - 1, rectPage.Right - 2, rectPage.Bottom - 1); 
          g.DrawLine(darkdark, rectPage.Left + 2, rectPage.Bottom, rectPage.Right - 2, rectPage.Bottom); 

          // Draw the right border
          g.DrawLine(darkdark, rectPage.Right, rectPage.Top, rectPage.Right, rectPage.Bottom - 2); 
          g.DrawLine(dark, rectPage.Right - 1, rectPage.Top + yRightOffset, rectPage.Right - 1, rectPage.Bottom - 2); 
          g.DrawLine(dark, rectPage.Right - 2, rectPage.Bottom - 1, rectPage.Right - 2, rectPage.Bottom - 2); 
          g.DrawLine(darkdark, rectPage.Right - 2, rectPage.Bottom, rectPage.Right, rectPage.Bottom - 2);
        }
      }
    }

    protected virtual void DrawIDETabBorder(TabPage page, Graphics g, Rectangle rectPage)
    {
      using(Pen lightlight = new Pen(m_backLightLight),
              backColor = new Pen(base.BackColor),
              dark = new Pen(m_backDark),
              darkdark = new Pen(m_backDarkDark))
      {
        if (page.Selected)
        {
          // Draw background in selected color
          using (SolidBrush pageAreaBrush = new SolidBrush(this.BackColor))
            g.FillRectangle(pageAreaBrush, rectPage);

          if (m_positionAtTop)
          {
            // Overdraw the bottom border
            g.DrawLine(backColor, rectPage.Left, rectPage.Bottom, rectPage.Right - 1, rectPage.Bottom);

            // Draw the right border
            g.DrawLine(darkdark, rectPage.Right, rectPage.Top, rectPage.Right, rectPage.Bottom); 
          }
          else
          {
            // Draw the left border
            g.DrawLine(lightlight, rectPage.Left, rectPage.Top - 1 , rectPage.Left, rectPage.Bottom); 

            // Draw the bottom border
            g.DrawLine(darkdark, rectPage.Left + 1, rectPage.Bottom, rectPage.Right, rectPage.Bottom); 

            // Draw the right border
            g.DrawLine(darkdark, rectPage.Right, rectPage.Top - 1 , rectPage.Right, rectPage.Bottom); 

            // Overdraw the top border
            g.DrawLine(backColor, rectPage.Left + 1, rectPage.Top - 1, rectPage.Right - 1, rectPage.Top - 1);
          }
        }
        else
        {
          // Draw background in unselected color
          using(SolidBrush tabsAreaBrush = new SolidBrush(m_backIDE))
            g.FillRectangle(tabsAreaBrush, rectPage);

          // Find the index into TabPage collection for this page
          int index = m_tabPages.IndexOf(page);

          // Decide if the separator should be drawn
          bool drawSeparator = (index == m_tabPages.Count - 1) ||
            (index < (m_tabPages.Count - 1)) && 
            (m_tabPages[index+1].Selected != true);
                                         
          // MultiLine mode is slighty more complex
          if (m_multiline && !drawSeparator)
          {
            // By default always draw separator
            drawSeparator = true;
                    
            // If we are not the last item
            if (index < (m_tabPages.Count - 1))
            {
              // If the next item is selected
              if (m_tabPages[index+1].Selected == true)
              {
                Rectangle thisRect = (Rectangle)m_tabRects[index];
                Rectangle nextRect = (Rectangle)m_tabRects[index+1];

                // If we are on the same drawing line then do not draw separator
                if (thisRect.Y == nextRect.Y)
                  drawSeparator = false;
              }
            }
          }

          // Draw tab separator unless the next page after us is selected
          if (drawSeparator)
          {
            // Reduce the intensity of the color
            using(Pen linePen = new Pen(m_textInactiveColor))
              g.DrawLine(linePen, rectPage.Right, rectPage.Top + 2, rectPage.Right, 
                rectPage.Bottom - m_position[m_styleIndex, (int)PositionIndex.TabsBottomGap] - 1);
          }
        }
      }
    }

    protected virtual void OnClearingPages()
    {
      // Is a page currently selected?
      if (m_pageSelected != -1)
      {
        // Deselect the page
        DeselectPage(m_tabPages[m_pageSelected]);

        // Remember that nothing is selected
        m_pageSelected = -1;
        m_startPage = -1;
      }

      // Remove all the user controls 
      foreach(TabPage page in m_tabPages)
        RemoveTabPage(page);

      // Remove all rectangles associated with TabPage's
      m_tabRects.Clear();
    }

    protected virtual void OnClearedPages()
    {
      // Must recalculate after the pages have been removed and
      // not before as that would calculate based on pages still
      // being present in the list
      Recalculate();
            
      // Raise selection changing event
      OnSelectionChanging(EventArgs.Empty);

      // Must notify a change in selection
      OnSelectionChanged(EventArgs.Empty);
            
      Invalidate();
    }

    protected virtual void OnInsertingPage(int index, object value)
    {
      // If a page currently selected?
      if (m_pageSelected != -1)
      {
        // Is the selected page going to be after this new one in the list
        if (m_pageSelected >= index)
          m_pageSelected++;  // then need to update selection index to reflect this
      }
    }

    protected virtual void OnInsertedPage(int index, object value)
    {
      bool selectPage = false;

      TabPage page = value as TabPage;

      // Hookup to receive TabPage property changes
      page.PropertyChanged += new TabPage.PropChangeHandler(OnPagePropertyChanged);

      // Add the appropriate Control/Form/TabPage to the control
      AddTabPage(page);
			
      // Do we want to select this page?
      if ((m_pageSelected == -1) || (page.Selected))
      {
        // Raise selection changing event
        OnSelectionChanging(EventArgs.Empty);

        // Any page currently selected
        if (m_pageSelected != -1)
          DeselectPage(m_tabPages[m_pageSelected]);

        // This becomes the newly selected page
        m_pageSelected = m_tabPages.IndexOf(page);

        // If no page is currently defined as the start page
        if (m_startPage == -1)
          m_startPage = 0;	 // then must be added then first page

        // Request the page be selected
        selectPage = true;
      }

      // Add new rectangle to match new number of pages, this must be done before
      // the 'SelectPage' or 'OnSelectionChanged' to ensure the number of m_tabRects 
      // entries matches the number of m_tabPages entries.
      m_tabRects.Add((object)new Rectangle());

      // Cause the new page to be the selected one
      if (selectPage)
      {
        // Must recalculate to ensure the new m_tabRects entry above it correctly
        // filled in before the new page is selected, as a change in page selection
        // may cause the m_tabRects values ot be interrogated.
        Recalculate();

        SelectPage(page);

        // Raise selection change event
        OnSelectionChanged(EventArgs.Empty);
      }

      Recalculate();
      Invalidate();
    }

    protected virtual void OnRemovingPage(int index, object value)
    {
      TabPage page = value as TabPage;

      page.PropertyChanged -= new TabPage.PropChangeHandler(OnPagePropertyChanged);

      // Remove the appropriate Control/Form/TabPage to the control
      RemoveTabPage(page);

      // Notice a change in selected page
      m_changed = false;

      // Is this the currently selected page
      if (m_pageSelected == index)
      {
        // Raise selection changing event
        OnSelectionChanging(EventArgs.Empty);

        m_changed = true;
        DeselectPage(page);
      }
    }

    protected virtual void OnRemovedPage(int index, object value)
    {
      // Is first displayed page then one being removed?
      if (m_startPage >= index)
      {
        // Decrement to use start displaying previous page
        m_startPage--;

        // Have we tried to select off the left hand side?
        if (m_startPage == -1)
        {
          // Are there still some pages left?
          if (m_tabPages.Count > 0)
            m_startPage = 0;
        }
      }

      // Is the selected page equal to or after this new one in the list
      if (m_pageSelected >= index)
      {
        // Decrement index to reflect this change
        m_pageSelected--;

        // Have we tried to select off the left hand side?
        if (m_pageSelected == -1)
        {
          // Are there still some pages left?
          if (m_tabPages.Count > 0)
            m_pageSelected = 0;
        }

        // Is the new selection valid?
        if (m_pageSelected != -1)
          SelectPage(m_tabPages[m_pageSelected]);  // Select it
      }

      // Change in selection causes event generation
      if (m_changed)
      {
        // Reset changed flag
        m_changed = false;

        // Raise selection change event
        OnSelectionChanged(EventArgs.Empty);
      }

      // Remove a rectangle to match number of pages
      m_tabRects.RemoveAt(0);

      Recalculate();
      Invalidate();
    }
			
    protected virtual void AddTabPage(TabPage page)
    {
      // Has not been shown for the first time yet
      page.Shown = false;
        
      // Add user supplied control 
      if (page.Control != null)
      {
        Form controlIsForm = page.Control as Form;

        page.Control.Hide();

        // Adding a Form takes extra effort
        if (controlIsForm == null)
        {
          // Monitor focus changes on the Control
          page.Control.GotFocus += new EventHandler(OnPageEnter);
          page.Control.LostFocus += new EventHandler(OnPageLeave);
          page.Control.MouseEnter += new EventHandler(OnPageMouseEnter);
          page.Control.MouseLeave += new EventHandler(OnPageMouseLeave);

          // Must fill the entire hosting panel it is on
          page.Control.Dock = DockStyle.None;

          // Set correct size
          page.Control.Location = new Point(0,0);
          page.Control.Size = m_hostPanel.Size;

          m_hostPanel.Controls.Add(page.Control);
        }
        else
        {
          // Monitor activation changes on the TabPage
          controlIsForm.Activated += new EventHandler(OnPageEnter);
          controlIsForm.Deactivate += new EventHandler(OnPageLeave);
          controlIsForm.MouseEnter += new EventHandler(OnPageMouseEnter);
          controlIsForm.MouseLeave += new EventHandler(OnPageMouseLeave);
	
          // Have to ensure the Form is not a top level form
          controlIsForm.TopLevel = false;

          // We are the new parent of this form
          controlIsForm.Parent = m_hostPanel;

          // To prevent user resizing the form manually and prevent
          // the caption bar appearing, we use the 'None' border style.
          controlIsForm.FormBorderStyle = FormBorderStyle.None;

          // Must fill the entire hosting panel it is on
          controlIsForm.Dock = DockStyle.None;

          // Set correct size
          controlIsForm.Location = new Point(0,0);
          controlIsForm.Size = m_hostPanel.Size;
        }

        // Need to monitor when the Form/Panel is clicked
        if ((page.Control is Form) || (page.Control is Panel))
          page.Control.MouseDown += new MouseEventHandler(OnPageMouseDown);

        RecursiveMonitor(page.Control, true);
      }
      else
      {
        page.Hide();

        // Monitor focus changes on the TabPage
        page.GotFocus += new EventHandler(OnPageEnter);
        page.LostFocus += new EventHandler(OnPageLeave);
        page.MouseEnter += new EventHandler(OnPageMouseEnter);
        page.MouseLeave += new EventHandler(OnPageMouseLeave);

        // Must fill the entire hosting panel it is on
        page.Dock = DockStyle.None;

        // Need to monitor when the Panel is clicked
        page.MouseDown += new MouseEventHandler(OnPageMouseDown);

        RecursiveMonitor(page, true);

        // Set correct size
        page.Location = new Point(0,0);
        page.Size = m_hostPanel.Size;

        // Add the TabPage itself instead
        m_hostPanel.Controls.Add(page);
      }
    }

    protected virtual void RemoveTabPage(TabPage page)
    {
      // Remove user supplied control 
      if (page.Control != null)
      {
        RecursiveMonitor(page.Control, false);

        Form controlIsForm = page.Control as Form;

        // Need to unhook hooked up event
        if ((page.Control is Form) || (page.Control is Panel))
          page.Control.MouseDown -= new MouseEventHandler(OnPageMouseDown);

        if (controlIsForm == null)
        {
          // Unhook event monitoring
          page.Control.GotFocus -= new EventHandler(OnPageEnter);
          page.Control.LostFocus -= new EventHandler(OnPageLeave);
          page.Control.MouseEnter -= new EventHandler(OnPageMouseEnter);
          page.Control.MouseLeave -= new EventHandler(OnPageMouseLeave);
		
          // Use helper method to circumvent form Close bug
          ControlHelper.Remove(m_hostPanel.Controls, page.Control);
        }
        else
        {
          // Unhook activation monitoring
          controlIsForm.Activated -= new EventHandler(OnPageEnter);
          controlIsForm.Deactivate -= new EventHandler(OnPageLeave);
          controlIsForm.MouseEnter -= new EventHandler(OnPageMouseEnter);
          controlIsForm.MouseLeave -= new EventHandler(OnPageMouseLeave);

          // Remove Form but prevent the Form close bug
          ControlHelper.RemoveForm(m_hostPanel, controlIsForm);
        }
      }
      else
      {
        RecursiveMonitor(page, false);

        // Need to unhook hooked up event
        page.MouseDown -= new MouseEventHandler(OnPageMouseDown);

        // Unhook event monitoring
        page.GotFocus -= new EventHandler(OnPageEnter);
        page.LostFocus -= new EventHandler(OnPageLeave);
        page.MouseEnter -= new EventHandler(OnPageMouseEnter);
        page.MouseLeave -= new EventHandler(OnPageMouseLeave);

        // Use helper method to circumvent form Close bug
        ControlHelper.Remove(m_hostPanel.Controls, page);
      }
    }

    protected virtual void OnPageMouseDown(object sender, MouseEventArgs e)
    {
      Control c = sender as Control;

      // If the mouse has been clicked and it does not have 
      // focus then it should receive the focus immediately.
      if (!c.ContainsFocus)
        c.Focus();
    }

    protected virtual void RecursiveMonitor(Control top, bool monitor)
    {
      foreach(Control c in top.Controls)
      {
        if (monitor)
        {
          // Monitor focus changes on the Control
          c.GotFocus += new EventHandler(OnPageEnter);
          c.LostFocus += new EventHandler(OnPageLeave);
          c.MouseEnter += new EventHandler(OnPageMouseEnter);
          c.MouseLeave += new EventHandler(OnPageMouseLeave);
        }
        else
        {
          // Unmonitor focus changes on the Control
          c.GotFocus -= new EventHandler(OnPageEnter);
          c.LostFocus -= new EventHandler(OnPageLeave);
          c.MouseEnter -= new EventHandler(OnPageMouseEnter);
          c.MouseLeave -= new EventHandler(OnPageMouseLeave);
        }
				
        RecursiveMonitor(c, monitor);
      }
    }

    protected virtual void OnPageEnter(object sender, EventArgs e)
    {
      OnPageGotFocus(e);
    }

    protected virtual void OnPageLeave(object sender, EventArgs e)
    {
      OnPageLostFocus(e);
    }
    
    protected virtual void OnPageMouseEnter(object sender, EventArgs e)
    {
      m_mouseOver = true;
      m_overTimer.Stop();
            
      if (m_hideTabsMode == HideTabsModes.HideWithoutMouse)
      {
        Recalculate();
        Invalidate();
      }
    }
	
    protected virtual void OnPageMouseLeave(object sender, EventArgs e)
    {
      m_overTimer.Start();
    }

    protected virtual void OnMouseTick(object sender, EventArgs e)
    {
      m_mouseOver = false;
      m_overTimer.Stop();
            
      if (m_hideTabsMode == HideTabsModes.HideWithoutMouse)
      {
        Recalculate();
        Invalidate();
      }
    }

    protected virtual void OnPagePropertyChanged(TabPage page, TabPage.Property prop, object oldValue)
    {
      switch(prop)
      {
        case TabPage.Property.Control:
          Control pageControl = oldValue as Control;

          // Is there a Control to be removed?
          if (pageControl != null)
          {
            // Use helper method to circumvent form Close bug
            ControlHelper.Remove(this.Controls, pageControl);
          }
          else
          {
            // Use helper method to circumvent form Close bug
            ControlHelper.Remove(this.Controls, page); // remove the whole TabPage instead
          }

          // Add the appropriate Control/Form/TabPage to the control
          AddTabPage(page);

          // Is a page currently selected?
          if (m_pageSelected != -1)
          {
            // Is the change in Control for this page?
            if (page == m_tabPages[m_pageSelected])
              SelectPage(page);   // make Control visible
          }

          Recalculate();
          Invalidate();
          break;
        case TabPage.Property.Title:
        case TabPage.Property.ImageIndex:
        case TabPage.Property.ImageList:
        case TabPage.Property.Icon:
          m_recalculate = true;
          Invalidate();
          break;
        case TabPage.Property.Selected:
          // Becoming selected?
          if (page.Selected)
          {
            // Move selection to the new page and update page properties
            MovePageSelection(page);
            MakePageVisible(page);
          }
          break;
      }
    }

    protected virtual Control FindFocus(Control root)
    {
      // Does the root control has focus?
      if (root.Focused)
        return root;

      // Check for focus at each child control
      foreach(Control c in root.Controls)
      {
        Control child = FindFocus(c);

        if (child != null)
          return child;
      }

      return null;
    }

    protected virtual void DeselectPage(TabPage page)
    {
      page.Selected = false;

      // Hide any associated control
      if (page.Control != null)
      {
        // Should we remember which control had focus when leaving?
        if (m_recordFocus)
        {
          // Record current focus location on Control
          if (page.Control.ContainsFocus)
            page.StartFocus = FindFocus(page.Control);
          else
            page.StartFocus = null;
        }

        page.Control.Hide();
      }
      else
      {
        // Should we remember which control had focus when leaving?
        if (m_recordFocus)
        {
          // Record current focus location on Control
          if (page.ContainsFocus)
            page.StartFocus = FindFocus(page);
          else
            page.StartFocus = null;
        }

        page.Hide();
      }
    }

    protected virtual void SelectPage(TabPage page)
    {
      page.Selected = true;

      // Bring the control for this page to the front
      if (page.Control != null)
        HandleShowingTabPage(page, page.Control);
      else
        HandleShowingTabPage(page, page);
    }

    protected virtual void HandleShowingTabPage(TabPage page, Control c)
    {
      // First time this page has been displayed?
      if (!page.Shown)
      {
        // Special testing needed for Forms
        Form f = c as Form;
                
        // AutoScaling can cause the Control/Form to be
        if ((f != null) && (f.AutoScaleMode != AutoScaleMode.None))
        {
          // Workaround the problem where a form has a defined 'AutoScaleBaseSize' value. The 
          // first time it is shown it calculates the size of each contained control and scales 
          // as needed. But if the contained control is Dock=DockStyle.Fill it scales up/down so 
          // its not actually filling the space! Get around by hiding and showing to force correct 
          // calculation.
          c.Show();
          c.Hide();
        }
                    
        // Only need extra logic first time around
        page.Shown = true;
      }

      // Finally, show it!
      c.Show();

      // Restore focus to last know control to have it
      if (page.StartFocus != null)
        page.StartFocus.Focus();
      else
        c.Focus();
    }
		
    protected virtual void MovePageSelection(TabPage page)
    {
      int pageIndex = m_tabPages.IndexOf(page);

      if (pageIndex != m_pageSelected)
      {
        // Raise selection changing event
        OnSelectionChanging(EventArgs.Empty);

        // Any page currently selected?
        if (m_pageSelected != -1)
          DeselectPage(m_tabPages[m_pageSelected]);

        m_pageSelected = pageIndex;

        if (m_pageSelected != -1)
          SelectPage(m_tabPages[m_pageSelected]);

        // Change in selection causes tab pages sizes to change
        if (m_boldSelected || m_selectedTextOnly || !m_shrinkPagesToFit || m_multiline)
        {
          Recalculate();
          Invalidate();
        }

        // Raise selection change event
        OnSelectionChanged(EventArgs.Empty);

        Invalidate();
      }
    }

    // Used by the TabControlDesigner
    internal bool WantDoubleClick(IntPtr hWnd, Point mousePos)
    {
      return ControlWantDoubleClick(hWnd, mousePos, m_leftArrow) ||
        ControlWantDoubleClick(hWnd, mousePos, m_rightArrow) ||
        ControlWantDoubleClick(hWnd, mousePos, m_closeButton);
    }

    // Used by the TabControlDesigner
    internal void ExternalMouseTest(IntPtr hWnd, Point mousePos)
    {
      if (!(ControlMouseTest(hWnd, mousePos, m_leftArrow) ||
        ControlMouseTest(hWnd, mousePos, m_rightArrow) ||
        ControlMouseTest(hWnd, mousePos, m_closeButton)))
        InternalMouseDown(mousePos);
    }

    protected virtual bool ControlWantDoubleClick(IntPtr hWnd, Point mousePos, Control check)
    {
      // Cannot have double click if control not visible
      if (check.Visible)
      {
        // Is double click for this control?
        if (check.Enabled && (hWnd == check.Handle))
        {
          if (check == m_leftArrow)
            OnLeftArrow(null, EventArgs.Empty);
	
          if (check == m_rightArrow)
            OnRightArrow(null, EventArgs.Empty);
					
          return true;
        }
        else
        {
          // Create rectangle for control position
          Rectangle checkRect = new Rectangle(check.Location.X,
            check.Location.Y,
            check.Width,
            check.Height);

          // Was double click over a disabled button?
          if (checkRect.Contains(mousePos))
            return true;
        }
      }

      return false;
    }

    protected virtual bool ControlMouseTest(IntPtr hWnd, Point mousePos, Control check)
    {
      // Is the mouse down for the left arrow window and is it valid to click?
      if ((hWnd == check.Handle) && check.Visible && check.Enabled)
      {
        // Check if the mouse click is over the left arrow
        if (check.ClientRectangle.Contains(mousePos))
        {
          if (check == m_leftArrow)
            OnLeftArrow(null, EventArgs.Empty);
	
          if (check == m_rightArrow)
            OnRightArrow(null, EventArgs.Empty);

          return true;
        }
      }

      return false;
    }

    protected override void OnDoubleClick(EventArgs e)
    {
      Point pos = TabControl.MousePosition;

      int count = m_tabRects.Count;

      for(int index=0; index<count; index++)
      {
        // Get tab drawing rectangle
        Rectangle local = (Rectangle)m_tabRects[index];
				
        // If drawing on the control
        if (local != m_nullPosition)
        {
          // Convert from Control to screen coordinates
          Rectangle screen = this.RectangleToScreen(local);

          if (screen.Contains(pos))
          {
            // Generate appropriate event
            OnDoubleClickTab(m_tabPages[index]);
            break;
          }
        }
      }

      base.OnDoubleClick(e);
    }

    protected override void OnMouseUp(MouseEventArgs e)
    {
      if (m_leftMouseDownDrag)
      {
        // Generate event for interested parties
        if (e.Button == MouseButtons.Left)
          OnPageDragEnd(e);
        else
          OnPageDragQuit(e);
                
        m_leftMouseDownDrag = false;
        m_ignoreDownDrag = true;
      }

      if (e.Button == MouseButtons.Left)
      {
        // Exit any page dragging attempt
        m_leftMouseDown = false;
      }
      else
      {
        // Is it the button that causes context menu to show?
        if (e.Button == MouseButtons.Right)
        {
          Point mousePos = new Point(e.X, e.Y);
                
          // Is the mouse in the tab area
          if (m_tabsAreaRect.Contains(mousePos))
          {
            CancelEventArgs ce = new CancelEventArgs();
                        
            // Generate event giving handlers cancel to update/cancel menu
            OnPopupMenuDisplay(ce);
                        
            // Still want the popup?
            if (!ce.Cancel)
            {
              // Is there any attached menu to show
              if (m_contextMenu != null)
                m_contextMenu.TrackPopup(this.PointToScreen(new Point(e.X, e.Y)));
            }
          }
        }
      }

      base.OnMouseUp(e);
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      // Only select a button or page when using left mouse button
      InternalMouseDown(new Point(e.X, e.Y));

      base.OnMouseDown(e);
    }
        
    protected virtual void InternalMouseDown(Point mousePos)
    {
      // Clicked on a tab page?
      for(int i=0; i<m_tabPages.Count; i++)
      {
        Rectangle rect = (Rectangle)m_tabRects[i];

        if (rect.Contains(mousePos))
        {
          // Are the scroll buttons being shown?
          if (m_leftArrow.Visible)
          {
            // Ignore mouse down over then buttons area
            if (mousePos.X >= m_leftArrow.Left)
              break;
          }
          else
          {
            // No, is the close button visible?
            if (m_closeButton.Visible)
            {
              // Ignore mouse down over then close button area
              if (mousePos.X >= m_closeButton.Left)
                break;
            }
          }

          // Remember where the left mouse was initially pressed
          if (Control.MouseButtons == MouseButtons.Left)
          {
            m_leftMouseDown = true;
            m_ignoreDownDrag = false;
            m_leftMouseDownDrag = false;
            m_leftMouseDownPos = mousePos;
          }
                    
          MovePageSelection(m_tabPages[i]);
          MakePageVisible(m_tabPages[i]);
          break;
        }
      }
    }		

    protected override void OnMouseMove(MouseEventArgs e)
    {
      if (m_leftMouseDown)
      {
        if (!m_leftMouseDownDrag)
        {
          Point thisPosition = new Point(e.X, e.Y);
        
          bool startDrag = false;

          if (m_dragFromControl)
            startDrag = !this.ClientRectangle.Contains(thisPosition);
          else
          {
            // Create starting mouse down position
            Rectangle dragRect = new Rectangle(m_leftMouseDownPos, new Size(0,0));
                        
            // Expand by size of the double click area
            dragRect.Inflate(SystemInformation.DoubleClickSize);
                        
            // Drag when mouse moves outside the double click area
            startDrag = !dragRect.Contains(thisPosition);
          }

          if (startDrag && !m_ignoreDownDrag)
          {
            // Generate event for interested parties
            OnPageDragStart(e);
		
            // Enter dragging mode
            m_leftMouseDownDrag = true;	
          }
        }
        else
        {
          // Generate event for interested parties
          OnPageDragMove(e);
        }
      }
      else
      {
        if (m_hotTrack || m_hoverSelect)
        {
          int mousePage = -1;
          bool pageChanged = false;

          // Create a point representing current mouse position
          Point mousePos = new Point(e.X, e.Y);

          // Find the page this mouse point is inside
          for(int pos=0; pos<m_tabPages.Count; pos++)
          {
            Rectangle rect = (Rectangle)m_tabRects[pos];

            if (rect.Contains(mousePos))
            {
              mousePage = pos;
              break;
            }
          }

          // Should moving over a tab cause selection changes?
          if (m_hoverSelect && !m_multiline && (mousePage != -1))
          {
            // Has the selected page changed?
            if (mousePage != m_pageSelected)
            {
              // Move selection to new page
              MovePageSelection(m_tabPages[mousePage]);

              pageChanged = true;
            }
          }

          if (m_hotTrack && !pageChanged && (mousePage != m_hotTrackPage))
          {
            Graphics g = this.CreateGraphics();

            // Clip the drawing to prevent drawing in unwanted areas
            ClipDrawingTabs(g);

            // Remove highlight of old page
            if (m_hotTrackPage != -1)
              DrawTab(m_tabPages[m_hotTrackPage], g, false);

            m_hotTrackPage = mousePage;

            // Add highlight to new page
            if (m_hotTrackPage != -1)
              DrawTab(m_tabPages[m_hotTrackPage], g, true);

            // Must correctly release resource
            g.Dispose();
          }
        }
      }
            
      base.OnMouseMove(e);
    }

    protected override void OnMouseEnter(EventArgs e)
    {
      m_mouseOver = true;
      m_overTimer.Stop();
            
      base.OnMouseEnter(e);
    }

    protected override void OnMouseLeave(EventArgs e)
    {
      if (m_hotTrack)
      {
        int newTrackPage = -1;

        if (newTrackPage != m_hotTrackPage)
        {
          Graphics g = this.CreateGraphics();

          // Clip the drawing to prevent drawing in unwanted areas
          ClipDrawingTabs(g);

          // Remove highlight of old page
          if (m_hotTrackPage != -1)
            DrawTab(m_tabPages[m_hotTrackPage], g, false);

          m_hotTrackPage = newTrackPage;

          // Must correctly release resource
          g.Dispose();
        }
      }

      m_overTimer.Start();

      base.OnMouseLeave(e);
    }		

    protected virtual void OnPreferenceChanged(object sender, UserPreferenceChangedEventArgs e)
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
      // If still using the Default color when we were created
      if (m_defaultColor)
      {
        DefineBackColor(TabControl.DefaultBackColor);

        Recalculate();
        Invalidate();
      }

      base.OnSystemColorsChanged(e);
    }
  }
}


#region revision history

// $Log: TabControl.cs,v $
// Revision 1.3  2006/10/30 16:30:08  gdavidson
// Added the namespace when accessing the System.Console
//
// Revision 1.2  2006/06/05 20:07:01  mkontak
// Depricated code
//
// Revision 1.1  2005/10/11 14:41:57  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabControl/TabControl.cs,v 1.3 2006/10/30 16:30:08 gdavidson Exp $
// $Id: TabControl.cs,v 1.3 2006/10/30 16:30:08 gdavidson Exp $

#endregion

