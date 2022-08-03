// $Id: PopupMenu.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
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
using System.Drawing.Text;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Menus
{
  [ToolboxItem(false)]
  [DefaultProperty("MenuCommands")]
  public class PopupMenu : NativeWindow
  {
    // Enumeration of Indexes into positioning constants array
    protected enum PI
    {
      BorderTop		= 0,
      BorderLeft		= 1,
      BorderBottom	= 2, 
      BorderRight		= 3,
      ImageGapTop		= 4,
      ImageGapLeft	= 5,
      ImageGapBottom	= 6,
      ImageGapRight	= 7,
      TextGapLeft		= 8,
      TextGapRight	= 9,
      SubMenuGapLeft	= 10,
      SubMenuWidth	= 11,
      SubMenuGapRight	= 12,
      SeparatorHeight	= 13,
      SeparatorWidth	= 14,
      ShortcutGap		= 15,
      ShadowWidth		= 16,
      ShadowHeight	= 17,
      ExtraWidthGap	= 18,
      ExtraHeightGap	= 19,
      ExtraRightGap	= 20,
      ExtraReduce		= 21
    }

    // Class constants for sizing/positioning each style
    protected static readonly int[,] m_position = { 
                                                        {2, 1, 0, 1, 2, 3, 3, 5, 4, 4, 2, 6, 5, 5, 1, 10, 4, 4, 2, 2, 0, 0},	// IDE
                                                        {1, 0, 1, 2, 2, 1, 3, 4, 3, 3, 2, 8, 5, 5, 5, 10, 0, 0, 2, 2, 2, 5}		// Plain
                                                  };
    // Other class constants
    protected static int m_selectionDelay = 400;
    protected static int m_expansionDelay = 1100;
    protected static int m_imageWidth = 16;
    protected static int m_imageHeight = 16;
    protected static int m_shadowLength = 4;
    protected static int m_shadowHalf = 2;
    protected static int m_blendSteps = 6;
    protected static Bitmap m_shadowCache = null;
    protected static int m_shadowCacheWidth = 0;
    protected static int m_shadowCacheHeight = 0;
		
    // Class fields
    protected static ImageList m_menuImages = null;
    protected static bool m_supportsLayered = false;
		
    // Indexes into the menu images strip
    protected enum ImageIndex
    {
      Check			= 0,
      Radio			= 1,
      SubMenu			= 2,
      CheckSelected	= 3,
      RadioSelected	= 4,
      SubMenuSelected	= 5,
      Expansion		= 6,
      ImageError		= 7
    }

    // Operation of DrawShadowHorizontal
    protected enum Shadow
    {
      Left,
      Right,
      All
    }

    // Class constants that are marked as 'readonly' are allowed computed initialization
    protected readonly int WM_DISMISS = (int)Win32.Msgs.WM_USER + 1;
    protected readonly int WM_OPERATE_SUBMENU = (int)Win32.Msgs.WM_USER + 2;

    // Instance fields
    protected System.Windows.Forms.Timer m_timer;
    protected Font m_textFont;
    protected int m_popupItem;
    protected int m_trackItem;
    protected int m_borderGap;
    protected int m_returnDir;
    protected int m_extraSize;
    protected int m_excludeOffset;
    protected int m_animateTime;
    protected bool m_animateFirst;
    protected bool m_animateIn;
    protected bool m_layered;
    protected bool m_exitLoop;
    protected bool m_mouseOver;
    protected bool m_popupDown;
    protected bool m_popupRight;
    protected bool m_excludeTop;
    protected bool m_showInfrequent;
    protected bool m_rememberExpansion;
    protected bool m_highlightInfrequent;
    protected Color m_backColor;
    protected Color m_textColor;
    protected Color m_highlightTextColor;
    protected Color m_highlightColor;
    protected Color m_highlightColorDark;
    protected Color m_highlightColorLight;
    protected Color m_highlightColorLightLight;
    protected Color m_controlLL;
    protected Color m_controlLLight;
    protected Size m_currentSize;
    protected VisualStyle m_style;
    protected Point m_screenPos;
    protected Point m_lastMousePos;
    protected Point m_currentPoint;
    protected Point m_leftScreenPos;
    protected Point m_aboveScreenPos;
    protected Direction m_direction;
    protected PopupMenu m_parentMenu;
    protected PopupMenu m_childMenu;
    protected SolidBrush m_controlLBrush;
    protected SolidBrush m_controlEBrush;
    protected SolidBrush m_controlLLBrush;
    protected Animate m_animate;
    protected Animate m_animateTrack;
    protected Animation m_animateStyle;
    protected ArrayList m_drawCommands;
    protected MenuControl m_parentControl;
    protected MenuCommand m_returnCommand;
    protected MenuCommandCollection m_menuCommands;

    // Instance fields - events
    public event CommandHandler Selected;
    public event CommandHandler Deselected;

    static PopupMenu()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_menuImages = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Menus.PopupMenu"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesPopupMenu.bmp",
        new Size(16,16),
        new Point(0,0));

      // We need to know if the OS supports layered windows
      m_supportsLayered = (OSFeature.Feature.GetVersionPresent(OSFeature.LayeredWindows) != null);
    }

    public PopupMenu()
    {
      // Create collection objects
      m_drawCommands = new ArrayList();
      m_menuCommands = new MenuCommandCollection(); 

      // Default the properties
      m_returnDir = 0;
      m_extraSize = 0;
      m_popupItem = -1;
      m_trackItem = -1;
      m_childMenu = null;
      m_exitLoop = false;
      m_popupDown = true;
      m_mouseOver = false;
      m_excludeTop = true;
      m_popupRight = true;
      m_parentMenu = null;
      m_excludeOffset = 0;
      m_parentControl = null;
      m_returnCommand = null;
      m_controlLBrush = null;
      m_controlEBrush = null;
      m_controlLLBrush = null;
      m_highlightInfrequent = false;
      m_showInfrequent = false;
      m_style = VisualStyle.IDE;
      m_rememberExpansion = true;
      m_lastMousePos = new Point(-1,-1);
      m_direction = Direction.Horizontal;
      m_textFont = SystemInformation.MenuFont;

      // Animation details
      m_animateTime = 100;
      m_animate = Animate.System;
      m_animateStyle = Animation.System;
      m_animateFirst = true;
      m_animateIn = true;

      // Create and initialise the timer object (but do not start it running!)
      m_timer = new System.Windows.Forms.Timer();
      m_timer.Interval = m_selectionDelay;
      m_timer.Tick += new EventHandler(OnTimerExpire);

      // Define default colors
      m_textColor = SystemColors.MenuText;
      m_highlightTextColor = SystemColors.HighlightText;
      DefineHighlightColors(SystemColors.Highlight);
      DefineColors(SystemColors.Control);
    }

    [Category("Appearance")]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public MenuCommandCollection MenuCommands
    {
      get { return m_menuCommands; }
    } 

    [Category("Appearance")]
    [DefaultValue(typeof(VisualStyle), "IDE")]
    public VisualStyle Style
    {
      get { return m_style; }
      set { m_style = value; }
    }

    [Category("Appearance")]
    public Font Font
    {
      get { return m_textFont; }
      set { m_textFont = value; }
    }

    [Category("Behaviour")]
    [DefaultValue(false)]
    public bool ShowInfrequent
    {
      get { return m_showInfrequent; }
      set { m_showInfrequent = value; }
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
    public bool HighlightInfrequent
    {
      get { return m_highlightInfrequent; }
      set { m_highlightInfrequent = value; }
    }

    [Category("Behaviour")]
    public Color BackColor
    {
      get { return m_backColor; }
      set { DefineColors(value); }
    }
       
    [Category("Behaviour")]
    public Color TextColor
    {
      get { return m_textColor; }
      set { m_textColor = value; }
    }

    [Category("Behaviour")]
    public Color HighlightTextColor
    {
      get { return m_highlightTextColor; }
      set { m_highlightTextColor = value; }
    }

    [Category("Behaviour")]
    public Color HighlightColor
    {
      get { return m_highlightColor; }
      set { DefineHighlightColors(value); }
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

    public MenuCommand TrackPopup(Point screenPos)
    {
      return TrackPopup(screenPos, false);
    }

    public MenuCommand TrackPopup(Point screenPos, bool selectFirst)
    {
      // No point in showing PopupMenu if there are no entries
      if (m_menuCommands.VisibleItems())
      {
        // Default the drawing direction
        m_direction = Direction.Horizontal;

        // Remember screen positions
        m_screenPos = screenPos;
        m_aboveScreenPos = screenPos;
        m_leftScreenPos = screenPos;

        return InternalTrackPopup(selectFirst);
      }

      return null;
    }

    internal MenuCommand TrackPopup(Point screenPos, 
      Point aboveScreenPos, 
      Direction direction,
      MenuCommandCollection menuCollection, 
      int borderGap, 
      bool selectFirst, 
      MenuControl parentControl,
      bool animateIn,
      ref int returnDir)
    {
      // Remember which direction the MenuControl is drawing in
      m_direction = direction;

      // Remember the MenuControl that initiated us
      m_parentControl = parentControl;

      // Remember the gap in drawing the top border
      m_borderGap = borderGap;

      // Is this the first time a menu at this level has been animated
      m_animateIn = animateIn;

      // Remember any currect menu item collection
      MenuCommandCollection oldCollection = m_menuCommands;

      // Use the passed in collection of menu commands
      m_menuCommands = menuCollection;

      // Remember screen positions
      m_screenPos = screenPos;
      m_aboveScreenPos = aboveScreenPos;
      m_leftScreenPos = screenPos;

      MenuCommand ret = InternalTrackPopup(selectFirst);

      // Restore to original collection
      m_menuCommands = oldCollection;

      // Remove reference no longer required
      m_parentControl = null;

      // Return the direction key that caused dismissal
      returnDir = m_returnDir;

      return ret;
    }

    protected MenuCommand InternalTrackPopup(Point screenPosTR, 
      Point screenPosTL, 
      MenuCommandCollection menuCollection, 
      PopupMenu parentMenu, 
      bool selectFirst, 
      MenuControl parentControl, 
      bool popupRight,
      bool popupDown, 
      bool animateIn,
      ref int returnDir)
    {
      // Default the drawing direction
      m_direction = Direction.Horizontal;

      // Remember the MenuControl that initiated us
      m_parentControl = parentControl;

      // We have a parent popup menu that should be consulted about operation
      m_parentMenu = parentMenu;
            
      // Is this the first time a menu at this level has been animated
      m_animateIn = animateIn;

      // Remember any currect menu item collection
      MenuCommandCollection oldCollection = m_menuCommands;

      // Use the passed in collection of menu commands
      m_menuCommands = menuCollection;

      // Remember screen positions
      m_screenPos = screenPosTR;
      m_aboveScreenPos = screenPosTR;
      m_leftScreenPos = screenPosTL;

      // Remember display directions
      m_popupRight = popupRight;
      m_popupDown = popupDown;

      MenuCommand ret = InternalTrackPopup(selectFirst);

      // Restore to original collection
      m_menuCommands = oldCollection;

      // Remove references no longer required
      m_parentControl = null;
      m_parentMenu = null;

      // Return the direction key that caused dismissal
      returnDir = m_returnDir;

      return ret;
    }

    protected MenuCommand InternalTrackPopup(bool selectFirst)
    {
      // MenuCommand to return as method result
      m_returnCommand = null;

      // No item is being tracked
      m_trackItem = -1;

      // Flag to indicate when to exit the message loop
      m_exitLoop = false;
			
      // Assume the mouse does not start over our window
      m_mouseOver = false;

      // Direction of key press if this caused dismissal
      m_returnDir = 0;

      // Flag to indicate if the message should be dispatched
      bool leaveMsg = false;

      // First time a submenu is shown we pass in our value
      m_animateFirst = true;

      // Create and show the popup window (without taking the focus)
      CreateAndShowWindow();
		
      // Create an object for storing windows message information
      Win32.MSG msg = new Win32.MSG();

      // Pretend user pressed key down to get the first valid item selected
      if (selectFirst)
        ProcessKeyDown();

      // Always use the arrow cursor
      User32.SetCursor(User32.LoadCursor(IntPtr.Zero, (uint)Win32.Cursors.IDC_ARROW));

      // Must hide caret so user thinks focus has changed
      bool hideCaret = User32.HideCaret(IntPtr.Zero);

      // Process messages until exit condition recognised
      while(!m_exitLoop)
      {
        // Suspend thread until a windows message has arrived
        if (User32.WaitMessage())
        {
          // Take a peek at the message details without removing from queue
          while(!m_exitLoop && User32.PeekMessage(ref msg, 0, 0, 0, (int)Win32.PeekMessageFlags.PM_NOREMOVE))
          {
            //						Console.WriteLine("Track {0} {1}", this.Handle, ((Win32.Msgs)msg.message).ToString());

            bool eatMessage = false;

            int localWidth = m_currentSize.Width - m_position[(int)m_style, (int)PI.ShadowWidth];
            int localHeight = m_currentSize.Height - m_position[(int)m_style, (int)PI.ShadowHeight];

            // Mouse was pressed in a window of this application
            if ((msg.message == (int)Win32.Msgs.WM_LBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_MBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_RBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_XBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_NCLBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_NCMBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_NCRBUTTONUP) ||
              (msg.message == (int)Win32.Msgs.WM_NCXBUTTONUP))
            {
              Win32.POINT screenPos = MousePositionToScreen(msg);

              // Is the POINT inside the Popup window rectangle
              if ((screenPos.x >= m_currentPoint.X) && (screenPos.x <= (m_currentPoint.X + localWidth)) &&
                (screenPos.y >= m_currentPoint.Y) && (screenPos.y <= (m_currentPoint.Y + localHeight)))
              {
                OnWM_YBUTTONUP(screenPos.x, screenPos.y);

                // Eat the message to prevent the intended destination getting it
                eatMessage = true;								
              }
              else
              {
                PopupMenu target = ParentPopupMenuWantsMouseMessage(screenPos, ref msg);
                            
                // Let the parent chain of PopupMenu's decide if they want it
                if (target != null)
                {
                  target.OnWM_YBUTTONUP(screenPos.x, screenPos.y);
                                
                  // Eat the message to prevent the intended destination getting it
                  eatMessage = true;								
                }
              }
            }
                                          
            // Mouse was pressed in a window of this application
            if ((msg.message == (int)Win32.Msgs.WM_LBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_MBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_RBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_XBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_NCLBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_NCMBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_NCRBUTTONDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_NCXBUTTONDOWN))
            {
              Win32.POINT screenPos = MousePositionToScreen(msg);

              // Is the POINT inside the Popup window rectangle
              if ((screenPos.x >= m_currentPoint.X) && (screenPos.x <= (m_currentPoint.X + localWidth)) &&
                (screenPos.y >= m_currentPoint.Y) && (screenPos.y <= (m_currentPoint.Y + localHeight)))
              {
                // Eat the message to prevent the intended destination getting it
                eatMessage = true;								
              }
              else
              {
                // Let the parent chain of PopupMenu's decide if they want it
                if (ParentPopupMenuWantsMouseMessage(screenPos, ref msg) == null)
                {
                  if (ParentControlWantsMouseMessage(screenPos, ref msg))
                  {
                    // Let the MenuControl do its business
                    m_parentControl.OnWM_MOUSEDOWN(screenPos);

                    // Eat the message to prevent the intended destination getting it
                    eatMessage = true;								
                  }
                  else
                  {
                    // No, then we need to exit the popup menu tracking
                    m_exitLoop = true;

                    // DO NOT process the message, leave it on the queue
                    // and let the real destination window handle it.
                    leaveMsg = true;

                    // Is a parent control specified?
                    if (m_parentControl != null)
                    {
                      // Is the mouse event destination the parent control?
                      if (msg.hwnd == m_parentControl.Handle)
                      {
                        // Then we want to consume the message so it does not get processed 
                        // by the parent control. Otherwise, pressing down will cause this 
                        // popup to disappear but the message will then get processed by 
                        // the parent and cause a popup to reappear again. When we actually
                        // want the popup to disappear and nothing more.
                        leaveMsg = false;
                      }
                    }
                  }
                }
                else
                {                                   
                  // Eat the message to prevent the intended destination getting it
                  eatMessage = true;								
                }
              }
            }

            // Mouse move occurred
            if (msg.message == (int)Win32.Msgs.WM_MOUSEMOVE) 
            {
              Win32.POINT screenPos = MousePositionToScreen(msg);

              // Is the POINT inside the Popup window rectangle
              if ((screenPos.x >= m_currentPoint.X) && (screenPos.x <= (m_currentPoint.X + localWidth)) &&
                (screenPos.y >= m_currentPoint.Y) && (screenPos.y <= (m_currentPoint.Y + localHeight)))
              {
                OnWM_MOUSEMOVE(screenPos.x, screenPos.y);
              }
              else
              {
                // Do we still think the mouse is over our window?
                if (m_mouseOver)
                {
                  // Process mouse leaving situation
                  OnWM_MOUSELEAVE();
                }
							
                // Let the parent chain of PopupMenu's decide if they want it
                PopupMenu target = ParentPopupMenuWantsMouseMessage(screenPos, ref msg);

                if (target != null)
                {
                  // Let parent target process the message
                  target.OnWM_MOUSEMOVE(screenPos.x, screenPos.y);	
                }
                else
                {
                  if (ParentControlWantsMouseMessage(screenPos, ref msg))
                  {
                    // Let the MenuControl do its business
                    m_parentControl.OnWM_MOUSEMOVE(screenPos);
                  }
                }
              }

              // Eat the message to prevent the intended destination getting it
              eatMessage = true;								
            }

            if (msg.message == (int)Win32.Msgs.WM_SETCURSOR) 
            {
              OnWM_SETCURSOR();

              // Eat the message to prevent the intended destination getting it
              eatMessage = true;								
            }

            // Was the alt key pressed?
            if (msg.message == (int)Win32.Msgs.WM_SYSKEYDOWN)
            {
              // Alt key pressed on its own
              if((int)msg.wParam == (int)Win32.VirtualKeys.VK_MENU)	// ALT key
              {
                // Then we should dimiss ourself
                m_exitLoop = true;
              }
              else
              {
                // Pretend it is a normal keypress for processing
                msg.message = (int)Win32.Msgs.WM_KEYDOWN;
              }
            }

            // Was a non-alt key pressed?
            if (msg.message == (int)Win32.Msgs.WM_KEYDOWN)
            {
              switch((int)msg.wParam)
              {
                case (int)Win32.VirtualKeys.VK_UP:
                  ProcessKeyUp();
                  break;
                case (int)Win32.VirtualKeys.VK_DOWN:
                  ProcessKeyDown();
                  break;
                case (int)Win32.VirtualKeys.VK_LEFT:
                  ProcessKeyLeft();
                  break;
                case (int)Win32.VirtualKeys.VK_RIGHT:
                  if(ProcessKeyRight())
                  {
                    // Do not attempt to pull a message off the queue as the
                    // ProcessKeyRight has eaten the message for us
                    leaveMsg = true;
                  }
                  break;
                case (int)Win32.VirtualKeys.VK_RETURN:
                  // Is an item currently selected
                  if (m_trackItem != -1)
                  {
                    DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

                    // Does this item have a submenu?
                    if (dc.SubMenu)
                    {
                      // Handle the submenu
                      OperateSubMenu(m_trackItem, false);

                      // Do not attempt to pull a message off the queue as it has already
                      // been eaten by us in the above code
                      leaveMsg = true;
                    }
                    else
                    {
                      // Is this item the expansion command?
                      if (dc.Expansion)
                      {
                        RegenerateExpansion();
                      }
                      else
                      {
                        // Define the selection to return to caller
                        m_returnCommand = dc.MenuCommand;

                        // Finish processing messages
                        m_exitLoop = true;	
                      }
                    }
                  }
                  break;
                case (int)Win32.VirtualKeys.VK_ESCAPE:
                  // User wants to exit the menu, so set the flag to exit the message loop but 
                  // let the message get processed. This way the key press is thrown away.
                  m_exitLoop = true;
                  break;
                default:
                  // Any other key is treated as a possible mnemonic
                  int selectItem = ProcessMnemonicKey((char)msg.wParam);

                  if (selectItem != -1)
                  {
                    DrawCommand dc = m_drawCommands[selectItem] as DrawCommand;

                    // Define the selection to return to caller
                    m_returnCommand = dc.MenuCommand;

                    // Finish processing messages
                    m_exitLoop = true;	

                    // Do not attempt to pull a message off the queue as it has already
                    // been eaten by us in the above code
                    leaveMsg = true;
                  }
                  break;
              }
            }

            // We consume all keyboard input
            if ((msg.message == (int)Win32.Msgs.WM_KEYDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_KEYUP) ||
              (msg.message == (int)Win32.Msgs.WM_SYSKEYDOWN) ||
              (msg.message == (int)Win32.Msgs.WM_SYSKEYUP))					
            {
              // Eat the message to prevent the intended destination getting it
              eatMessage = true;								
            }						

            // Should the message be eaten to prevent intended destination getting it?
            if (eatMessage)
            {
              Win32.MSG eat = new Win32.MSG();
              User32.GetMessage(ref eat, 0, 0, 0);
            }
            else
            {	
              // Should the message we pulled from the queue?
              if (!leaveMsg)
              {
                if (User32.GetMessage(ref msg, 0, 0, 0))
                {
                  User32.TranslateMessage(ref msg);
                  User32.DispatchMessage(ref msg);
                }
              }
              else
                leaveMsg = false;
            }
          }
        }
      }	

      // If caret was hidden then show it again now
      if (hideCaret)
        User32.ShowCaret(IntPtr.Zero);

      // Remove tracking of any item, this ensure 'Deselected' event is generated if required
      SwitchSelection(m_trackItem, -1, false, false);

      // Hide the window from view before killing it, as sometimes there is a
      // short delay between killing it and it disappearing because of the time
      // it takes for the destroy messages to get processed
      HideMenuWindow();

      // Commit suicide
      DestroyHandle();

      // Was a command actually selected in a top level PopupMenu AND we
      // are not here at the request of a MenuControl
      if ((m_parentMenu == null) && (m_returnCommand != null) && (m_parentControl == null))
      {
        // Pulse the selected event for the command
        m_returnCommand.OnClick(EventArgs.Empty);
      }

      return m_returnCommand;
    }

    public void Dismiss()
    {
      if (this.Handle != IntPtr.Zero)
      {
        // Prevent the timer from expiring
        m_timer.Stop();

        // Kill any child menu
        if (m_childMenu != null)
          m_childMenu.Dismiss();

        // Finish processing messages
        m_exitLoop = true;	

        // Hide ourself
        HideMenuWindow();

        // Cause our own message loop to exit
        User32.PostMessage(this.Handle, WM_DISMISS, 0, 0); 
      }
    }
		
    protected void HideMenuWindow()
    {
      User32.ShowWindow(this.Handle, (short)Win32.ShowWindowStyles.SW_HIDE);
    }

    protected void ManualAnimateBlend(bool show)
    {
      // Set the image to be completely transparent so the following command
      // to show the window does not actual show anything.
      UpdateLayeredWindow(0);

      // Show the window without activating it (i.e. do not take focus)
      User32.ShowWindow(this.Handle, (short)Win32.ShowWindowStyles.SW_SHOWNOACTIVATE);

      int stepDelay = (int)(m_animateTime / m_blendSteps);
            
      for(int i=0; i<m_blendSteps; i++)
      {
        // Calculate increasing values of opaqueness
        byte alpha = (byte)(63 + (192 / m_blendSteps * (i + 1)));
		        
        DateTime beforeTime = DateTime.Now;
		    
        // Update the image for display
        UpdateLayeredWindow(alpha);

        DateTime afterTime = DateTime.Now;

        // Need to subtract elapsed time from required step delay                
        TimeSpan elapsed = afterTime.Subtract(beforeTime);
    
        // Short delay before showing next frame, but test if delay is actually needed
        // because sometimes the time to update layered window is longer than delay
        if ((m_animateTime > 0) && (elapsed.Milliseconds < stepDelay))
          System.Threading.Thread.Sleep(stepDelay - elapsed.Milliseconds);
      }
    }

    protected void CreateAndShowWindow()
    {
      // Decide if we need layered windows
      m_layered = (m_supportsLayered && (m_style == VisualStyle.IDE));

      // Process the menu commands to determine where each one needs to be
      // drawn and return the size of the window needed to display it.
      Size winSize = GenerateDrawPositions();

      Point screenPos = CorrectPositionForScreen(winSize);

      // Special case, if there are no menu options to show then show nothing by
      // making the window 0,0 in size.
      if (m_menuCommands.Count == 0)
        winSize = new Size(0,0);

      CreateParams cp = new CreateParams();

      // Any old title will do as it will not be shown
      cp.Caption = "NativePopupMenu";
			
      // Define the screen position/size
      cp.X = screenPos.X;
      cp.Y = screenPos.Y;
      cp.Height = winSize.Height;
      cp.Width = winSize.Width;

      // As a top-level window it has no parent
      cp.Parent = IntPtr.Zero;
			
      // Appear as a top-level window
      cp.Style = unchecked((int)(uint)Win32.WindowStyles.WS_POPUP);
			
      // Set styles so that it does not have a caption bar and is above all other 
      // windows in the ZOrder, i.e. TOPMOST
      cp.ExStyle = (int)Win32.WindowExStyles.WS_EX_TOPMOST + 
        (int)Win32.WindowExStyles.WS_EX_TOOLWINDOW;

      // OS specific style
      if (m_layered)
      {
        // If not on NT then we are going to use alpha blending on the shadow border
        // and so we need to specify the layered window style so the OS can handle it
        cp.ExStyle += (int)Win32.WindowExStyles.WS_EX_LAYERED;
      }

      // Is this the plain style of appearance?
      if (m_style == VisualStyle.Plain)
      {
        // We want the tradiditonal 3D border
        cp.Style += unchecked((int)(uint)Win32.WindowStyles.WS_DLGFRAME);
      }

      // Create the actual window
      this.CreateHandle(cp);

      // Update the window clipping region
      if (!m_layered)
        SetWindowRegion(winSize);			

      // Remember the correct screen drawing details
      m_currentSize = winSize;
      m_currentPoint = screenPos;

      bool animated = false;

      if (m_layered)
      {
        // Update the image for display
        UpdateLayeredWindow();

        bool animate = false;
				
        switch(m_animate)
        {
          case Animate.No:
            animate = false;
            break;
          case Animate.Yes:
            animate = true;
            break;
          case Animate.System:
            int bRetValue = 0;

            // Does the system want animation to occur?
            User32.SystemParametersInfo((uint)Win32.SPIActions.SPI_GETMENUANIMATION, 0, ref bRetValue, 0);

            animate = (bRetValue != 0);
            break;
        }

        // Should the menu be shown with animation?
        if (animate && m_animateIn)
        {
          uint animateFlags = (uint)m_animateStyle;

          if (m_animateStyle == Animation.System)
          {
            int bRetValue = 0;

            // Does the system want fading or sliding?
            User32.SystemParametersInfo((uint)Win32.SPIActions.SPI_GETMENUFADE, 0, ref bRetValue, 0);

            // Use appropriate flags to match request
            if (bRetValue != 0)
              animateFlags = (uint)Animation.Blend;
            else
              animateFlags = (uint)Animation.SlideHorVerPositive;
          }

          // Animate the appearance of the window
          if ((animateFlags & (uint)Win32.AnimateFlags.AW_BLEND) != 0)
          {
            // Cannot use Win32.AnimateWindow to blend a layered window
            ManualAnimateBlend(true);
          }
          else
          {
            // Animate the appearance of the window
            User32.AnimateWindow(this.Handle, (uint)m_animateTime, animateFlags); 
          }

          animated = true;
        }
      }

      // Did any animation take place?
      if (!animated)
      {
        // Show the window without activating it (i.e. do not take focus)
        User32.ShowWindow(this.Handle, (short)Win32.ShowWindowStyles.SW_SHOWNOACTIVATE);
      }
    }

    protected void UpdateLayeredWindow()
    {
      UpdateLayeredWindow(m_currentPoint, m_currentSize, 255);
    }

    protected void UpdateLayeredWindow(byte alpha)
    {
      UpdateLayeredWindow(m_currentPoint, m_currentSize, alpha);
    }

    protected void UpdateLayeredWindow(Point point, Size size, byte alpha)
    {
      // Create bitmap for drawing onto
      Bitmap memoryBitmap = new Bitmap(size.Width, size.Height, PixelFormat.Format32bppArgb);

      using(Graphics g = Graphics.FromImage(memoryBitmap))
      {
        Rectangle area = new Rectangle(0, 0, size.Width, size.Height);
		
        // Draw the background area
        DrawBackground(g, area);

        // Draw the actual menu items
        DrawAllCommands(g);

        // Get hold of the screen DC
        IntPtr hDC = User32.GetDC(IntPtr.Zero);
				
        // Create a memory based DC compatible with the screen DC
        IntPtr memoryDC = Gdi32.CreateCompatibleDC(hDC);
				
        // Get access to the bitmap handle contained in the Bitmap object
        IntPtr hBitmap = memoryBitmap.GetHbitmap(Color.FromArgb(0));

        // Select this bitmap for updating the window presentation
        IntPtr oldBitmap = Gdi32.SelectObject(memoryDC, hBitmap);

        // New window size
        Win32.SIZE ulwsize;
        ulwsize.cx = size.Width;
        ulwsize.cy = size.Height;

        // New window position
        Win32.POINT topPos;
        topPos.x = point.X;
        topPos.y = point.Y;

        // Offset into memory bitmap is always zero
        Win32.POINT pointSource;
        pointSource.x = 0;
        pointSource.y = 0;

        // We want to make the entire bitmap opaque 
        Win32.BLENDFUNCTION blend = new Win32.BLENDFUNCTION();
        blend.BlendOp             = (byte)Win32.AlphaFlags.AC_SRC_OVER;
        blend.BlendFlags          = 0;
        blend.SourceConstantAlpha = alpha;
        blend.AlphaFormat         = (byte)Win32.AlphaFlags.AC_SRC_ALPHA;

        // Tell operating system to use our bitmap for painting
        User32.UpdateLayeredWindow(Handle, hDC, ref topPos, ref ulwsize, 
          memoryDC, ref pointSource, 0, ref blend, 
          (int)Win32.UpdateLayeredWindowsFlags.ULW_ALPHA);

        // Put back the old bitmap handle
        Gdi32.SelectObject(memoryDC, oldBitmap);

        // Cleanup resources
        User32.ReleaseDC(IntPtr.Zero, hDC);
        Gdi32.DeleteObject(hBitmap);
        Gdi32.DeleteDC(memoryDC);
      }
    }

    protected void SetWindowRegion(Size winSize)
    {
      // Style specific handling
      if (m_style == VisualStyle.IDE)
      {
        int shadowHeight = m_position[(int)m_style, (int)PI.ShadowHeight];
        int shadowWidth = m_position[(int)m_style, (int)PI.ShadowWidth];

        // Create a new region object
        Region drawRegion = new Region();

        // Can draw anywhere
        drawRegion.MakeInfinite();

        // Remove the area above the right hand shadow
        drawRegion.Xor(new Rectangle(winSize.Width - shadowWidth, 0, shadowWidth, shadowHeight));

        // When drawing upwards from a vertical menu we need to allow a connection between the 
        // MenuControl selection box and the PopupMenu shadow
        if (!((m_direction == Direction.Vertical) && !m_excludeTop))
        {
          // Remove the area left of the bottom shadow
          drawRegion.Xor(new Rectangle(0, winSize.Height - shadowHeight, shadowWidth, shadowHeight));
        }

        // Define a region to prevent drawing over exposed corners of shadows
        using(Graphics g = Graphics.FromHwnd(this.Handle))
          User32.SetWindowRgn(this.Handle, drawRegion.GetHrgn(g), false);
      }

    }

    protected Point CorrectPositionForScreen(Size winSize)
    {
      Rectangle screenRect = Screen.GetWorkingArea (m_screenPos);
      Point screenPos = m_screenPos;

      int screenWidth = screenRect.Width;
      int screenLeft  = screenRect.Left;
      int screenRight = screenRect.Right;
      int screenHeight = screenRect.Height;
      int screenBottom = screenRect.Bottom;
      int screenTop    = screenRect.Top;

      // Default to excluding menu border from top
      m_excludeTop = true;
      m_excludeOffset = 0;

      // Shadow area does not count as part of width
      winSize.Width -= m_position[(int)m_style, (int)PI.ShadowWidth];

      // Calculate the downward position first
      if (m_popupDown)
      {
        // Ensure the end of the menu is not off the bottom of the screen
        if ((screenPos.Y + winSize.Height) > screenBottom)
        {
          // If the parent control exists then try and position upwards instead
          if ((m_parentControl != null) && (m_parentMenu == null))
          {
            // Is there space above the required position?
            if ((m_aboveScreenPos.Y - winSize.Height) > screenTop)
            {
              // Great...do that instead
              screenPos.Y = m_aboveScreenPos.Y - winSize.Height;

              // Reverse direction of drawing this and submenus
              m_popupDown = false;

              // Remember to exclude border from bottom of menu and not the top
              m_excludeTop = false;

              // Inform parent it needs to redraw the selection upwards
              m_parentControl.DrawSelectionUpwards();
            }	
          }

          // Did the above logic still fail?
          if ((screenPos.Y + winSize.Height) > screenBottom)
          {
            // If not a top level PopupMenu then..
            if (m_parentMenu != null)
            {
              // Reverse direction of drawing this and submenus
              m_popupDown = false;

              // Is there space above the required position?
              if ((m_aboveScreenPos.Y - winSize.Height) > screenTop)
                screenPos.Y = m_aboveScreenPos.Y - winSize.Height;
              else
                screenPos.Y = screenTop;
            }
            else
              screenPos.Y = screenBottom - winSize.Height - 1;
          }
        }
      }
      else
      {
        // Ensure the end of the menu is not off the top of the screen
        if ((screenPos.Y - winSize.Height) < screenTop)
        {
          // Reverse direction
          m_popupDown = true;

          // Is there space below the required position?
          if ((screenPos.Y + winSize.Height) > screenBottom)
            screenPos.Y = screenBottom - winSize.Height - 1;
        }
        else
          screenPos.Y -= winSize.Height;
      }

      // Calculate the across position next
      if (m_popupRight)
      {
        // Ensure that right edge of menu is not off right edge of screen
        if ((screenPos.X + winSize.Width) > screenRight)
        {
          // If not a top level PopupMenu then...
          if (m_parentMenu != null)
          {
            // Reverse direction
            m_popupRight = false;

            // Adjust across position						
            screenPos.X = m_leftScreenPos.X - winSize.Width;				
		
            if (screenPos.X < screenLeft)
              screenPos.X = screenLeft;
          }
          else
          {	
            // Find new position of X coordinate
            int newX = screenRight - winSize.Width - 1;

            // Modify the adjust needed when drawing top/bottom border
            m_excludeOffset = screenPos.X - newX;

            // Use new position for popping up menu
            screenPos.X = newX;
          }
        }
      }
      else
      {
        // Start by using the left screen pos instead
        screenPos.X = m_leftScreenPos.X;

        // Ensure the left edge of the menu is not off the left of the screen
        if ((screenPos.X - winSize.Width) < screenLeft)
        {
          // Reverse direction
          m_popupRight = true;

          // Is there space below the required position?
          if ((m_screenPos.X + winSize.Width) > screenRight)
            screenPos.X = screenRight - winSize.Width - 1;
          else
            screenPos.X = m_screenPos.X;
        }
        else
          screenPos.X -= winSize.Width;
      }

      return screenPos;
    }

    protected void RegenerateExpansion()
    {
      // Remove all existing draw commands
      m_drawCommands.Clear();

      // Move into the expanded mode
      m_showInfrequent = true;

      // Show we remember the expansion to the collection?
      if (m_rememberExpansion)
        m_menuCommands.ShowInfrequent = true;
            
      // Generate new ones
      Size newSize = GenerateDrawPositions();

      // Find the new screen location for the window
      Point newPos = CorrectPositionForScreen(newSize);

      // Remember the correct screen drawing details
      m_currentPoint = newPos;
      m_currentSize = newSize;

      // Update the window clipping region
      if (!m_layered)
      {
        SetWindowRegion(newSize);

        // Alter size and location of window
        User32.MoveWindow(this.Handle, newPos.X, newPos.Y, newSize.Width, newSize.Height, true);			

        Win32.RECT clientRect = new Win32.RECT();

        clientRect.left = 0;
        clientRect.top = 0;
        clientRect.right = newSize.Width;
        clientRect.bottom = newSize.Height;

        // Get the client area redrawn after MoveWindow has been processed
        User32.InvalidateRect(this.Handle, ref clientRect, true);
      }
      else
      {
        // Update the image for display
        UpdateLayeredWindow();

        // Lets repaint everything
        RefreshAllCommands();
      }
    }

    protected Size GenerateDrawPositions()
    {
      // Create a collection of drawing objects
      m_drawCommands = new ArrayList();

      // Calculate the minimum cell width and height
      int cellMinHeight = m_position[(int)m_style, (int)PI.ImageGapTop] +
        m_imageHeight + 
        m_position[(int)m_style, (int)PI.ImageGapBottom];
			
      int cellMinWidth = m_position[(int)m_style, (int)PI.ImageGapLeft] +
        m_imageWidth + 
        m_position[(int)m_style, (int)PI.ImageGapRight] + 
        m_position[(int)m_style, (int)PI.TextGapLeft] +
        m_position[(int)m_style, (int)PI.TextGapRight] +
        m_position[(int)m_style, (int)PI.SubMenuGapLeft] +
        m_position[(int)m_style, (int)PI.SubMenuWidth] +
        m_position[(int)m_style, (int)PI.SubMenuGapRight];

      // Find cell height needed to draw text
      int textHeight = m_textFont.Height;

      // If height needs to be more to handle image then use image height
      if (textHeight < cellMinHeight)
        textHeight = cellMinHeight;

      // Make sure no column in the menu is taller than the screen
      int screenHeight = SystemInformation.WorkingArea.Height;
	
      // Define the starting positions for calculating cells
      int xStart = m_position[(int)m_style, (int)PI.BorderLeft];
      int yStart =m_position[(int)m_style, (int)PI.BorderTop];
      int yPosition = yStart;

      // Largest cell for column defaults to minimum cell width
      int xColumnMaxWidth = cellMinWidth;

      int xPreviousColumnWidths = 0;
      int xMaximumColumnHeight = 0;

      // Track the row/col of each cell
      int row = 0;
      int col = 0;

      // Are there any infrequent items
      bool infrequent = false;
      bool previousInfrequent = false;

      // Get hold of the DC for the desktop
      IntPtr hDC = User32.GetDC(IntPtr.Zero);

      // Contains the collection of items in the current column
      ArrayList columnItems = new ArrayList();

      using(Graphics g = Graphics.FromHdc(hDC))
      {
        // Handle any extra text drawing
        if (m_menuCommands.ExtraText.Length > 0)
        {
          // Calculate the column width needed to show this text
          SizeF dimension = g.MeasureString(m_menuCommands.ExtraText, m_menuCommands.ExtraFont);

          // Always add 1 to ensure that rounding is up and not down
          int extraHeight = (int)dimension.Height + 1;

          // Find the total required as the text requirement plus style specific spacers
          m_extraSize = extraHeight + 
            m_position[(int)m_style, (int)PI.ExtraRightGap] +
            m_position[(int)m_style, (int)PI.ExtraWidthGap] * 2;

          // Push first column of items across from the extra text
          xStart += m_extraSize;
				
          // Add this extra width to the total width of the window
          xPreviousColumnWidths = m_extraSize;
        }

        foreach(MenuCommand command in m_menuCommands)
        {
          // Give the command a chance to update its state
          command.OnUpdate(EventArgs.Empty);

          // Ignore items that are marked as hidden
          if (!command.Visible)
            continue;

          // If this command has menu items (and so it a submenu item) then check
          // if any of the submenu items are visible. If none are visible then there
          // is no point in showing this submenu item
          if ((command.MenuCommands.Count > 0) && (!command.MenuCommands.VisibleItems()))
            continue;

          // Ignore infrequent items unless flag set to show them
          if (command.Infrequent && !m_showInfrequent)
          {
            infrequent = true;
            continue;
          }

          int cellWidth = 0;
          int cellHeight = 0;

          // Shift across to the next column?
          if (command.Break)
          {		
            // Move row/col tracking to the next column				
            row = 0;
            col++;

            // Apply cell width to the current column entries
            ApplySizeToColumnList(columnItems, xColumnMaxWidth);

            // Move cell position across to start of separator position
            xStart += xColumnMaxWidth;

            // Get width of the separator area
            int xSeparator = m_position[(int)m_style, (int)PI.SeparatorWidth];
						
            DrawCommand dcSep = new DrawCommand(new Rectangle(xStart, 0, xSeparator, 0), false);

            // Add to list of items for drawing
            m_drawCommands.Add(dcSep);

            // Move over the separator
            xStart += xSeparator;						

            // Reset cell position to top of column
            yPosition = yStart;

            // Accumulate total width of previous columns
            xPreviousColumnWidths += xColumnMaxWidth + xSeparator;
						
            // Largest cell for column defaults to minimum cell width
            xColumnMaxWidth  = cellMinWidth;
          }

          // Is this a horizontal separator?
          if (command.Text == "-")
          {
            cellWidth = cellMinWidth;
            cellHeight = m_position[(int)m_style, (int)PI.SeparatorHeight];
          }
          else
          {
            // Use precalculated height
            cellHeight = textHeight;

            // Calculate the text width portion of the cell
            SizeF dimension = g.MeasureString(command.Text, m_textFont);
	
            // Always add 1 to ensure that rounding is up and not down
            cellWidth = cellMinWidth + (int)dimension.Width + 1;

            // Does the menu command have a shortcut defined?
            if (command.Shortcut != Shortcut.None)
            {
              // Find the width of the shortcut text
              dimension = g.MeasureString(GetShortcutText(command.Shortcut), m_textFont);

              // Add to the width of the cell
              cellWidth += m_position[(int)m_style, (int)PI.ShortcutGap] + (int)dimension.Width + 1;
            }
          }

          // If the new cell expands past the end of the screen...
          if ((yPosition + cellHeight) >= screenHeight)
          {
            // .. then need to insert a column break

            // Move row/col tracking to the next column				
            row = 0;
            col++;

            // Apply cell width to the current column entries
            ApplySizeToColumnList(columnItems, xColumnMaxWidth);

            // Move cell position across to start of separator position
            xStart += xColumnMaxWidth;

            // Get width of the separator area
            int xSeparator = m_position[(int)m_style, (int)PI.SeparatorWidth];
						
            DrawCommand dcSep = new DrawCommand(new Rectangle(xStart, yStart, xSeparator, 0), false);

            // Add to list of items for drawing
            m_drawCommands.Add(dcSep);

            // Move over the separator
            xStart += xSeparator;						

            // Reset cell position to top of column
            yPosition = yStart;

            // Accumulate total width of previous columns
            xPreviousColumnWidths += xColumnMaxWidth + xSeparator;
						
            // Largest cell for column defaults to minimum cell width
            xColumnMaxWidth  = cellMinWidth;

            // Show this be drawn in the infrequent colors
            dcSep.Infrequent = previousInfrequent;
          }
	
          // Create a new position rectangle (the width will be reset later once the 
          // width of the column has been determined but the other values are correct)
          Rectangle cellRect = new Rectangle(xStart, yPosition, cellWidth, cellHeight);
	
          // Create a drawing object
          DrawCommand dc = new DrawCommand(command, cellRect, row, col);

          // Is the infrequent state different from previous commands?
          if (previousInfrequent != command.Infrequent)
          {
            // If was not infrequent but now is
            if (command.Infrequent)
            {
              // Then draw this infrequent item with a top border
              dc.TopBorder = true;
            }
            else
            {
              if (m_drawCommands.Count > 0)
              {
                // Find the previous command (excluding separators) and make it as needed a border
                for(int index = m_drawCommands.Count - 1; index>=0; index--)
                {
                  if (!( m_drawCommands[index] as DrawCommand).Separator)
                  {
                    (m_drawCommands[index] as DrawCommand).BottomBorder = true;
                    break;
                  }
                }
              }
            }
          }
                    
          // Remember the state of previous command only if not a separator
          if (!dc.Separator)
            previousInfrequent = command.Infrequent;
                    
          // Add to list of items for drawing
          m_drawCommands.Add(dc);

          // Add to list of items in this column
          columnItems.Add(dc);					

          // Remember the biggest cell width in this column
          if (cellWidth > xColumnMaxWidth)
            xColumnMaxWidth = cellWidth;

          // Move down to start of next cell in column
          yPosition += cellHeight;

          // Remember the tallest column in the menu
          if (yPosition > xMaximumColumnHeight)
            xMaximumColumnHeight = yPosition;

          row++;
        }

        // Check if we need to add an infrequent expansion item
        if (infrequent)
        {
          // Create a minimum size cell
          Rectangle cellRect = new Rectangle(xStart, yPosition, cellMinWidth, cellMinHeight);

          // Create a draw command to represent the drawing of the expansion item
          DrawCommand dc = new DrawCommand(cellRect, true);

          // Must be last item
          m_drawCommands.Add(dc);

          // Add to list of items in this column
          columnItems.Add(dc);					

          yPosition += cellMinHeight;

          // Remember the tallest column in the menu
          if (yPosition > xMaximumColumnHeight)
            xMaximumColumnHeight = yPosition;
        }

        // Apply cell width to the current column entries
        ApplySizeToColumnList(columnItems, xColumnMaxWidth);
      }

      // Must remember to release the HDC resource!
      User32.ReleaseDC(IntPtr.Zero, hDC);

      // Find width/height of window
      int windowWidth = m_position[(int)m_style, (int)PI.BorderLeft] +
        xPreviousColumnWidths + 
        xColumnMaxWidth + 
        m_position[(int)m_style, (int)PI.BorderRight];

      int windowHeight = m_position[(int)m_style, (int)PI.BorderTop] +
        xMaximumColumnHeight + 
        m_position[(int)m_style, (int)PI.BorderBottom];

      // Define the height of the vertical separators
      ApplyVerticalSeparators(xMaximumColumnHeight);

      // Style specific modification of window size
      int xAdd = m_position[(int)m_style, (int)PI.ShadowHeight];
      int yAdd = m_position[(int)m_style, (int)PI.ShadowWidth];

      if (m_style == VisualStyle.Plain)
      {
        xAdd += SystemInformation.Border3DSize.Width * 2;
        yAdd += SystemInformation.Border3DSize.Height * 2;
      }

      return new Size(windowWidth + xAdd, windowHeight + yAdd);
    }

    protected void DefineHighlightColors(Color baseColor)
    {
      m_highlightColor = baseColor;
        
      if (m_highlightColor == SystemColors.Highlight)
      {
        m_highlightColorDark = SystemColors.Highlight;
        m_highlightColorLight = Color.FromArgb(70, m_highlightColorDark);
        m_highlightColorLightLight = Color.FromArgb(20, m_highlightColorDark);
      }
      else
      {
        m_highlightColorDark = ControlPaint.Dark(baseColor);
        m_highlightColorLight = baseColor;
        m_highlightColorLightLight = Color.FromArgb(70, baseColor);
      }
    }
            
    protected void DefineColors(Color backColor)
    {
      m_backColor = backColor;
      m_controlLL = ControlPaint.LightLight(m_backColor);
      m_controlLBrush = new SolidBrush(Color.FromArgb(200, m_backColor));
      m_controlEBrush = new SolidBrush(Color.FromArgb(150, m_backColor));
      m_controlLLBrush = new SolidBrush(m_controlLL);
    }

    protected void ApplyVerticalSeparators(int sepHeight)
    {
      // Each vertical separator needs to be the same height, this has already 
      // been calculated and passed in from the tallest column in the menu
      foreach(DrawCommand dc in m_drawCommands)
      {
        if (dc.VerticalSeparator)
        {
          // Grab the current drawing rectangle
          Rectangle cellRect = dc.DrawRect;

          // Modify the height to that requested
          dc.DrawRect = new Rectangle(cellRect.Left, cellRect.Top, cellRect.Width, sepHeight);
        }
      }
    }

    protected void ApplySizeToColumnList(ArrayList columnList, int cellWidth)
    {
      // Each cell in the same column needs to be the same width, this has already
      // been calculated and passed in as the widest cell in the column
      foreach(DrawCommand dc in columnList)
      {
        // Grab the current drawing rectangle
        Rectangle cellRect = dc.DrawRect;

        // Modify the width to that requested
        dc.DrawRect = new Rectangle(cellRect.Left, cellRect.Top, cellWidth, cellRect.Height);
      }

      // Clear collection out ready for reuse
      columnList.Clear();
    }

    protected void RefreshAllCommands()
    {
      Win32.RECT rectRaw = new Win32.RECT();

      // Grab the screen rectangle of the window
      User32.GetWindowRect(this.Handle, ref rectRaw);

      // Convert from screen to client sizing
      Rectangle rectWin = new Rectangle(0, 0, 
        rectRaw.right - rectRaw.left, 
        rectRaw.bottom - rectRaw.top);

      using(Graphics g = Graphics.FromHwnd(this.Handle))
      {
        // Draw the background area
        DrawBackground(g, rectWin);

        // Draw the actual menu items
        DrawAllCommands(g);
      }
    }

    protected void DrawBackground(Graphics g, Rectangle rectWin)
    { 
      Rectangle main = new Rectangle(0, 0, 
        rectWin.Width - 1 - m_position[(int)m_style, (int)PI.ShadowWidth], 
        rectWin.Height - 1 - m_position[(int)m_style, (int)PI.ShadowHeight]);

      // Style specific drawing
      switch(m_style)
      {
        case VisualStyle.IDE:
          // Calculate some common values
          int imageColWidth = m_position[(int)m_style, (int)PI.ImageGapLeft] + 
            m_imageWidth + 
            m_position[(int)m_style, (int)PI.ImageGapRight];

          int xStart = m_position[(int)m_style, (int)PI.BorderLeft];
          int yStart = m_position[(int)m_style, (int)PI.BorderTop];
          int yHeight = main.Height - yStart - m_position[(int)m_style, (int)PI.BorderBottom] - 1;

          // Paint the main area background
          g.FillRectangle(m_controlLLBrush, main);

          // Draw single line border around the main area
          using(Pen mainBorder = new Pen(ControlPaint.Dark(m_backColor)))
          {
            g.DrawRectangle(mainBorder, main);

            // Should the border be drawn with part of the border missing?
            if (m_borderGap > 0)
            {
              // Remove the appropriate section of the border
              if (m_direction == Direction.Horizontal)
              {
                if (m_excludeTop)
                {
                  g.FillRectangle(Brushes.White, main.Left + 1 + m_excludeOffset, main.Top, m_borderGap - 1, 1);
                  g.FillRectangle(m_controlLBrush, main.Left + 1 + m_excludeOffset, main.Top, m_borderGap - 1, 1);
                }
                else
                {
                  g.FillRectangle(Brushes.White, main.Left + 1 + m_excludeOffset, main.Bottom, m_borderGap - 1, 1);
                  g.FillRectangle(m_controlLBrush, main.Left + 1 + m_excludeOffset, main.Bottom, m_borderGap - 1, 1);
                }
              }
              else
              {
                if (m_excludeTop)
                {
                  g.FillRectangle(Brushes.White, main.Left, main.Top + 1 + m_excludeOffset, 1, m_borderGap - 1);
                  g.FillRectangle(m_controlLBrush, main.Left, main.Top + 1 + m_excludeOffset, 1, m_borderGap - 1);
                }
                else
                {
                  if (m_popupDown)
                  {
                    g.FillRectangle(Brushes.White, main.Left, main.Bottom - 1 - m_excludeOffset, 1, m_borderGap - 1);
                    g.FillRectangle(m_controlLBrush, main.Left, main.Bottom - 1 - m_excludeOffset, 1, m_borderGap - 1);
                  }
                  else
                  {
                    g.FillRectangle(Brushes.White, main.Left, main.Bottom - m_borderGap + 1, 1, m_borderGap - 1);
                    g.FillRectangle(m_controlLBrush, main.Left, main.Bottom - m_borderGap + 1, 1, m_borderGap - 1);
                  }
                }
              }
            }
          }

          // Draw the first image column
          Rectangle imageRect = new Rectangle(xStart, yStart, imageColWidth, yHeight);

          g.FillRectangle(Brushes.White, imageRect);
          g.FillRectangle(m_controlLBrush, imageRect);

          // Draw image column after each vertical separator
          foreach(DrawCommand dc in m_drawCommands)
          {
            if (dc.Separator && dc.VerticalSeparator)
            {
              // Recalculate starting position (but height remains the same)
              imageRect.X = dc.DrawRect.Right;

              g.FillRectangle(Brushes.White, imageRect);
              g.FillRectangle(m_controlLBrush, imageRect);
            }
          }

          // Draw shadow around borders
          int rightLeft = main.Right + 1;
          int rightTop = main.Top + m_position[(int)m_style, (int)PI.ShadowHeight];
          int rightBottom = main.Bottom + 1;
          int leftLeft = main.Left + m_position[(int)m_style, (int)PI.ShadowWidth];
          int xExcludeStart = main.Left + m_excludeOffset;
          int xExcludeEnd = main.Left + m_excludeOffset + m_borderGap;

          if ((m_borderGap > 0) && (!m_excludeTop) && (m_direction == Direction.Horizontal))
          {
            int rightright = rectWin.Width;

            if (xExcludeStart >= leftLeft)
              DrawShadowHorizontal(g, leftLeft, rightBottom, xExcludeStart - leftLeft, m_position[(int)m_style, (int)PI.ShadowHeight], Shadow.Left);

            if (xExcludeEnd <= rightright)
              DrawShadowHorizontal(g, xExcludeEnd, rightBottom, rightright - xExcludeEnd, m_position[(int)m_style, (int)PI.ShadowHeight], Shadow.Right);
          }
          else
          {
            if ((m_direction == Direction.Vertical) && (!m_excludeTop))
              leftLeft = 0;
								
            DrawShadowHorizontal(g, leftLeft, rightBottom, rightLeft, m_position[(int)m_style, (int)PI.ShadowHeight], Shadow.All);
          }

          DrawShadowVertical(g, rightLeft, rightTop, m_position[(int)m_style, (int)PI.ShadowWidth], rightBottom - rightTop - 1);
          break;
        case VisualStyle.Plain:
          // Paint the main area background
          using(SolidBrush mainBrush = new SolidBrush(m_backColor))
            g.FillRectangle(mainBrush, rectWin);
          break;
      }

      // Is there an extra title text to be drawn?
      if (m_menuCommands.ExtraText.Length > 0)
        DrawColumn(g, main);
    }

    protected void DrawShadowVertical(Graphics g, int left, int top, int width, int height)
    {
      if (m_layered)
      {
        Color extraColor = Color.FromArgb(64, 0, 0, 0);
        Color darkColor = Color.FromArgb(48, 0, 0, 0);
        Color lightColor = Color.FromArgb(0, 0, 0, 0);
            
        // Enough room for top and bottom shades?
        if (height >= m_shadowLength)
        {
          using(LinearGradientBrush topBrush = new LinearGradientBrush(new Point(left - m_shadowLength, top + m_shadowLength), 
                  new Point(left + m_shadowLength, top),
                  extraColor, lightColor))
          {
            // Draw top shade
            g.FillRectangle(topBrush, left, top, m_shadowLength, m_shadowLength);
                        
            top += m_shadowLength;
            height -= m_shadowLength;
          }
        }
               
        using(LinearGradientBrush middleBrush = new LinearGradientBrush(new Point(left, 0), 
                new Point(left + width, 0),
                darkColor, lightColor))
        {                                                                               
          // Draw middle shade
          g.FillRectangle(middleBrush, left, top, width, height + 1);
        }
      }
      else
      {
        using(SolidBrush shadowBrush = new SolidBrush(ControlPaint.Dark(m_backColor)))
          g.FillRectangle(shadowBrush, left, top, width, height + 1);
      }
    }

    protected void DrawShadowHorizontal(Graphics g, int left, int top, int width, int height, Shadow op)
    {
      if (m_layered)
      {
        Color extraColor = Color.FromArgb(64, 0, 0, 0);
        Color darkColor = Color.FromArgb(48, 0, 0, 0);
        Color lightColor = Color.FromArgb(0, 0, 0, 0);

        // Do we need to draw the left shadow?
        if (op != Shadow.Right)
        {
          if (width >= m_shadowLength)
          {
            // Draw the remaining middle
            using(LinearGradientBrush leftBrush = new LinearGradientBrush(new Point(left + m_shadowLength, top - m_shadowLength), 
                    new Point(left, top + height),
                    extraColor, lightColor))
            {                                                                               
              // Draw middle shade
              g.FillRectangle(leftBrush, left, top, m_shadowLength, height);
                            
              left += m_shadowLength;
              width -= m_shadowLength;
            }
          }
        }
                
        // Do we need to draw the right shadow?
        if (op != Shadow.Left)
        {
          if (width >= m_shadowLength)
          {
            try
            {
              g.DrawImageUnscaled(GetShadowCache(m_shadowLength, height), left + width - m_shadowLength, top);
            }
            catch
            {
              //	just to be on the safe side...
            }
            width -= m_shadowLength;
          }
        }

        // Draw the remaining middle
        using(LinearGradientBrush middleBrush = new LinearGradientBrush(new Point(9999, top), 
                new Point(9999, top + height),
                darkColor, lightColor))
        {                                                                               
          // Draw middle shade
          g.FillRectangle(middleBrush, left, top, width, height);
        }
      }
      else
      {
        using(SolidBrush shadowBrush = new SolidBrush(ControlPaint.Dark(m_backColor)))
          g.FillRectangle(shadowBrush, left, top, width, height);
      }
    }
        
    protected void DrawColumn(Graphics g, Rectangle main)
    {
      // Create the rectangle that encloses the drawing
      Rectangle rectText = new Rectangle(main.Left, 
        main.Top, 
        m_extraSize - m_position[(int)m_style, (int)PI.ExtraRightGap], 
        main.Height);

      Brush backBrush = null;
      bool disposeBack = true;

      if (m_menuCommands.ExtraBackBrush != null)
      {
        backBrush = m_menuCommands.ExtraBackBrush;
        disposeBack = false;
        rectText.Width++;
      }
      else
        backBrush = new SolidBrush(m_menuCommands.ExtraBackColor);

      // Fill background using brush
      g.FillRectangle(backBrush, rectText);

      // Do we need to dispose of the brush?
      if (disposeBack)
        backBrush.Dispose();

      // Adjust rectangle for drawing the text into
      rectText.X += m_position[(int)m_style, (int)PI.ExtraWidthGap];
      rectText.Y += m_position[(int)m_style, (int)PI.ExtraHeightGap];
      rectText.Width -= m_position[(int)m_style, (int)PI.ExtraWidthGap] * 2;
      rectText.Height -= m_position[(int)m_style, (int)PI.ExtraHeightGap] * 2;

      // For Plain style we need to take into account the border sizes
      if (m_style == VisualStyle.Plain)
        rectText.Height -= SystemInformation.Border3DSize.Height * 2;

      // Draw the text into this rectangle
      StringFormat format = new StringFormat();
      format.FormatFlags = StringFormatFlags.DirectionVertical | 
        StringFormatFlags.NoClip | 
        StringFormatFlags.NoWrap;
      format.Alignment = StringAlignment.Near;
      format.LineAlignment = StringAlignment.Center;

      Brush textBrush = null;
      bool disposeText = true;

      if (m_menuCommands.ExtraTextBrush != null)
      {
        textBrush = m_menuCommands.ExtraTextBrush;
        disposeText = false;
      }
      else
        textBrush = new SolidBrush(m_menuCommands.ExtraTextColor);

      // Draw string from bottom of area towards the top using the given Font/Brush
      DrawHelper.DrawReverseString(g, m_menuCommands.ExtraText, m_menuCommands.ExtraFont, rectText, textBrush, format);

      // Do we need to dispose of the brush?
      if (disposeText)
        textBrush.Dispose();
    }

    internal void DrawSingleCommand(Graphics g, DrawCommand dc, bool hotCommand)
    {
      Rectangle drawRect = dc.DrawRect;
      MenuCommand mc = dc.MenuCommand;
	
      // Remember some often used values
      int textGapLeft = m_position[(int)m_style, (int)PI.TextGapLeft];
      int imageGapLeft = m_position[(int)m_style, (int)PI.ImageGapLeft];
      int imageGapRight = m_position[(int)m_style, (int)PI.ImageGapRight];
      int imageLeft = drawRect.Left + imageGapLeft;

      // Calculate some common values
      int imageColWidth = imageGapLeft + m_imageWidth + imageGapRight;

      int subMenuWidth = m_position[(int)m_style, (int)PI.SubMenuGapLeft] +
        m_position[(int)m_style, (int)PI.SubMenuWidth] +
        m_position[(int)m_style, (int)PI.SubMenuGapRight];

      int subMenuX = drawRect.Right - 
        m_position[(int)m_style, (int)PI.SubMenuGapRight] -
        m_position[(int)m_style, (int)PI.SubMenuWidth];

      // Text drawing rectangle needs to know the right most position for drawing
      // to stop. This is the width of the window minus the relevant values
      int shortCutX = subMenuX - 
        m_position[(int)m_style, (int)PI.SubMenuGapLeft] -
        m_position[(int)m_style, (int)PI.TextGapRight];

      // Is this item an expansion command?
      if (dc.Expansion)
      {
        Rectangle box = drawRect;

        // In IDE style the box is next to the image column
        if (m_style == VisualStyle.IDE)
        {
          // Reduce the box to take into account the column
          box.X += imageColWidth;
          box.Width -= imageColWidth;
        }

        // Find centre for drawing the image
        int xPos = box.Left + ((box.Width - m_imageHeight) / 2);
        int yPos = box.Top + ((box.Height - m_imageHeight) / 2);
				
        // Should the item look selected
        if (hotCommand)
        {
          switch(m_style)
          {
            case VisualStyle.IDE:
              Rectangle selectArea = new Rectangle(drawRect.Left + 1, drawRect.Top,
                drawRect.Width - 3, drawRect.Height - 1);

              using (Pen selectPen = new Pen(m_highlightColorDark))
              {
                // Draw the selection area white, because we are going to use an alpha brush
                using (SolidBrush whiteBrush = new SolidBrush(Color.White))
                  g.FillRectangle(whiteBrush, selectArea);

                using (SolidBrush selectBrush = new SolidBrush(m_highlightColorLight))
                {
                  // Draw the selection area
                  g.FillRectangle(selectBrush, selectArea);

                  // Draw a border around the selection area
                  g.DrawRectangle(selectPen, selectArea);
                }
              }
              break;
            case VisualStyle.Plain:
              // Shrink the box to provide a small border
              box.Inflate(-2, -2);

              using (Pen lightPen = new Pen(ControlPaint.LightLight(m_backColor)),
                       darkPen = new Pen(ControlPaint.DarkDark(m_backColor)))
              {
                g.DrawLine(lightPen, box.Right, box.Top, box.Left, box.Top);
                g.DrawLine(lightPen, box.Left, box.Top, box.Left, box.Bottom);
                g.DrawLine(darkPen, box.Left, box.Bottom, box.Right, box.Bottom);
                g.DrawLine(darkPen, box.Right, box.Bottom, box.Right, box.Top);
              }
              break;
          }
        }
        else
        {
          switch(m_style)
          {
            case VisualStyle.IDE:
              // Fill the entire drawing area with white
              g.FillRectangle(m_controlLLBrush, new Rectangle(drawRect.Left + 1, drawRect.Top,
                drawRect.Width - 1, drawRect.Height));

              // Draw the image column background
              g.FillRectangle(Brushes.White, new Rectangle(drawRect.Left, drawRect.Top,
                imageColWidth, drawRect.Height));
        
              g.FillRectangle(m_controlLBrush, new Rectangle(drawRect.Left, drawRect.Top,
                imageColWidth, drawRect.Height));
              break;
            case VisualStyle.Plain:
              using(SolidBrush drawBrush = new SolidBrush(m_backColor))
                g.FillRectangle(drawBrush, new Rectangle(drawRect.Left, drawRect.Top,
                  drawRect.Width, drawRect.Height));
              break;
          }

        }

        // Always draw the expansion bitmap
        g.DrawImage(m_menuImages.Images[(int)ImageIndex.Expansion], xPos, yPos);
      }
      else
      {
        // Is this item a separator?
        if (dc.Separator)
        {
          if (dc.VerticalSeparator)
          {
            switch(m_style)
            {
              case VisualStyle.IDE:
                // Draw the separator as a single line
                using (Pen separatorPen = new Pen(ControlPaint.Dark(m_backColor)))
                  g.DrawLine(separatorPen, drawRect.Left, drawRect.Top, drawRect.Left, drawRect.Bottom);
                break;
              case VisualStyle.Plain:
                ButtonBorderStyle bsInset = ButtonBorderStyle.Inset;
                ButtonBorderStyle bsNone = ButtonBorderStyle.Inset;
							
                Rectangle sepRect = new Rectangle(drawRect.Left + 1, drawRect.Top, 2, drawRect.Height);
							
                // Draw the separator as two lines using Inset style
                ControlPaint.DrawBorder(g, sepRect, 
                  m_backColor, 1, bsInset, m_backColor, 0, bsNone,
                  m_backColor, 1, bsInset, m_backColor, 0, bsNone);
                break;
            }
          }
          else
          {
            switch(m_style)
            {
              case VisualStyle.IDE:
                // Draw the image column background
                Rectangle imageCol = new Rectangle(drawRect.Left, drawRect.Top, imageColWidth, drawRect.Height);

                g.FillRectangle(Brushes.White, imageCol);
                g.FillRectangle(m_controlLBrush, imageCol);

                // Draw a separator
                using (Pen separatorPen = new Pen(Color.FromArgb(75, m_textColor)))
                {
                  // Draw the separator as a single line
                  g.DrawLine(separatorPen, 
                    drawRect.Left + imageColWidth + textGapLeft, drawRect.Top + 2, 
                    drawRect.Right, 
                    drawRect.Top + 2);
                }
                break;
              case VisualStyle.Plain:
                if (dc.Infrequent && m_highlightInfrequent)
                {
                  // Change background to be a lighter shade
                  using(Brush drawBrush = new SolidBrush(ControlPaint.Light(m_backColor)))
                    g.FillRectangle(drawBrush, drawRect);
                }
                                    
                ButtonBorderStyle bsInset = ButtonBorderStyle.Inset;
                ButtonBorderStyle bsNone = ButtonBorderStyle.Inset;
							
                Rectangle sepRect = new Rectangle(drawRect.Left + 2, drawRect.Top + 2, drawRect.Width - 4, 2);
							
                // Draw the separator as two lines using Inset style
                ControlPaint.DrawBorder(g, sepRect, 
                  m_backColor, 0, bsNone, m_backColor, 1, bsInset,
                  m_backColor, 0, bsNone, m_backColor, 1, bsInset);
                break;
            }
          }
        }
        else
        {
          int leftPos = drawRect.Left + imageColWidth + textGapLeft;

          // Should the command be drawn selected?
          if (hotCommand)
          {
            switch(m_style)
            {
              case VisualStyle.IDE:
                Rectangle selectArea = new Rectangle(drawRect.Left + 1, drawRect.Top, drawRect.Width - 3, drawRect.Height - 1);

                using (Pen selectPen = new Pen(m_highlightColorDark))
                {
                  // Draw the selection area white, because we are going to use an alpha brush
                  using (SolidBrush whiteBrush = new SolidBrush(Color.White))
                    g.FillRectangle(whiteBrush, selectArea);

                  using (SolidBrush selectBrush = new SolidBrush(m_highlightColorLight))
                  {
                    // Draw the selection area
                    g.FillRectangle(selectBrush, selectArea);

                    // Draw a border around the selection area
                    g.DrawRectangle(selectPen, selectArea);
                  }
                }
                break;
              case VisualStyle.Plain:
                using (SolidBrush selectBrush = new SolidBrush(m_highlightColorDark))
                  g.FillRectangle(selectBrush, drawRect);
                break;
            }
          }
          else
          {
            switch(m_style)
            {
              case VisualStyle.IDE:
                // Fill the entire drawing area with ControlLightLight
                g.FillRectangle(m_controlLLBrush, new Rectangle(drawRect.Left + 1, drawRect.Top, drawRect.Width - 1, drawRect.Height));

                if (dc.Infrequent && m_highlightInfrequent)
                {
                  // Draw the text area in a darker shade
                  g.FillRectangle(Brushes.White, new Rectangle(leftPos, drawRect.Top, drawRect.Right - leftPos - textGapLeft, drawRect.Height));
                  g.FillRectangle(m_controlEBrush, new Rectangle(leftPos, drawRect.Top, drawRect.Right - leftPos - textGapLeft, drawRect.Height));
                }

                Rectangle imageCol = new Rectangle(drawRect.Left, drawRect.Top,imageColWidth, drawRect.Height);

                // Draw the image column background
                g.FillRectangle(Brushes.White, imageCol);
                g.FillRectangle(m_controlLBrush, imageCol);
                break;
              case VisualStyle.Plain:
                if (dc.Infrequent && m_highlightInfrequent)
                {
                  using(Brush drawBrush = new SolidBrush(ControlPaint.Light(m_backColor)))
                    g.FillRectangle(drawBrush, new Rectangle(drawRect.Left, drawRect.Top, drawRect.Width, drawRect.Height));
                }
                else                                
                {
                  using(Brush drawBrush = new SolidBrush(m_backColor))
                    g.FillRectangle(drawBrush, new Rectangle(drawRect.Left, drawRect.Top, drawRect.Width, drawRect.Height));
                }
                                
                if (dc.TopBorder && m_highlightInfrequent)
                  using(Pen drawPen = new Pen(ControlPaint.Dark(m_backColor)))
                    g.DrawLine(drawPen, drawRect.Left, drawRect.Top, drawRect.Right, drawRect.Top);
                                
                if (dc.BottomBorder && m_highlightInfrequent)
                  using(Pen drawPen = new Pen(ControlPaint.LightLight(m_backColor)))
                    g.DrawLine(drawPen, drawRect.Left, drawRect.Bottom - 1, drawRect.Right, drawRect.Bottom - 1);
                break;
            }
          }

          // Calculate text drawing rectangle
          Rectangle strRect = new Rectangle(leftPos, drawRect.Top, shortCutX - leftPos, drawRect.Height);

          // Left align the text drawing on a single line centered vertically
          // and process the & character to be shown as an underscore on next character
          StringFormat format = new StringFormat();
          format.FormatFlags = StringFormatFlags.NoClip | StringFormatFlags.NoWrap;
          format.Alignment = StringAlignment.Near;
          format.LineAlignment = StringAlignment.Center;
          format.HotkeyPrefix = HotkeyPrefix.Show;

          SolidBrush textBrush;

          // Create brush depending on enabled state
          if (mc.Enabled)
          {
            if (!hotCommand || (m_style == VisualStyle.IDE))
              textBrush = new SolidBrush(m_textColor);
            else
              textBrush = new SolidBrush(m_highlightTextColor);
          }
          else 
            textBrush = new SolidBrush(SystemColors.GrayText);

          // Helper values used when drawing grayed text in plain style
          Rectangle rectDownRight = strRect;
          rectDownRight.Offset(1,1);

          if (mc.Enabled || (m_style == VisualStyle.IDE))
            g.DrawString(mc.Text, m_textFont, textBrush, strRect, format);
          else
          {
            if (m_style == VisualStyle.Plain)
            {
              // Draw grayed text by drawing white string offset down and right
              using (SolidBrush whiteBrush = new SolidBrush(Color.White))
                g.DrawString(mc.Text, m_textFont, whiteBrush, rectDownRight, format);
            }

            // And then draw in correct color offset up and left
            g.DrawString(mc.Text, m_textFont, textBrush, strRect, format);
          }

          if (mc.Shortcut != Shortcut.None)
          {
            // Right align the shortcut drawing
            format.Alignment = StringAlignment.Far;

            if (mc.Enabled || (m_style == VisualStyle.IDE))
            {
              // Draw the shortcut text 
              g.DrawString(GetShortcutText(mc.Shortcut), m_textFont, textBrush, strRect, format);
            }
            else
            {
              if (m_style == VisualStyle.Plain)
              {
                // Draw grayed text by drawing white string offset down and right
                using (SolidBrush whiteBrush = new SolidBrush(Color.White))
                  g.DrawString(GetShortcutText(mc.Shortcut), m_textFont, whiteBrush, rectDownRight, format);
              }

              // And then draw in corret color offset up and left
              g.DrawString(GetShortcutText(mc.Shortcut), m_textFont, textBrush, strRect, format);
            }
          }

          // The image offset from top of cell is half the space left after
          // subtracting the height of the image from the cell height
          int imageTop = drawRect.Top + (drawRect.Height - m_imageHeight) / 2;

          Image image = null;

          // Should a check mark be drawn?
          if (mc.Checked)
          {
            switch(m_style)
            {
              case VisualStyle.IDE:
                Pen boxPen;
                Brush boxBrush;

                if (mc.Enabled)
                {
                  boxPen = new Pen(m_highlightColorDark);
                  boxBrush = new SolidBrush(m_highlightColorLightLight);
                }
                else
                {
                  boxPen = new Pen(SystemColors.GrayText);
                  boxBrush = new SolidBrush(Color.FromArgb(20, SystemColors.GrayText));
                }

                Rectangle boxRect = new Rectangle(imageLeft - 1, imageTop - 1, m_imageHeight + 2, m_imageWidth + 2);

                // Fill the checkbox area very slightly
                g.FillRectangle(boxBrush, boxRect);

                // Draw the box around the checkmark area
                g.DrawRectangle(boxPen, boxRect);

                boxPen.Dispose();
                boxBrush.Dispose();
                break;
              case VisualStyle.Plain:
                break;
            }

            // Grab either tick or radio button image
            if (mc.RadioCheck)
            {
              if (hotCommand && (m_style == VisualStyle.Plain))
                image = m_menuImages.Images[(int)ImageIndex.RadioSelected];
              else
                image = m_menuImages.Images[(int)ImageIndex.Radio];
            }
            else
            {
              if (hotCommand && (m_style == VisualStyle.Plain))
                image = m_menuImages.Images[(int)ImageIndex.CheckSelected];
              else
                image = m_menuImages.Images[(int)ImageIndex.Check];
            }
          }
          else
          {
            try
            {
              // Always use the Image property in preference to the ImageList
              if (mc.Image != null)
                image = mc.Image;
              else
              {
                if ((mc.ImageList != null) && (mc.ImageIndex >= 0))
                  image = mc.ImageList.Images[mc.ImageIndex];
              }
            }
            catch(Exception)
            {
              // User supplied ImageList/ImageIndex are invalid, use an error image instead
              image = m_menuImages.Images[(int)ImageIndex.ImageError];
            }
          }

          // Is there an image to be drawn?
          if (image != null)
          {
            if (mc.Enabled)
            {
              if ((hotCommand) && (!mc.Checked) && (m_style == VisualStyle.IDE))
              {
                // Draw a gray icon offset down and right
                Bitmap shadowImage = new Bitmap((Bitmap) image);
                Color shadowColor = Color.FromArgb(154, 156, 146);
                Color transparent = Color.FromArgb(0, 0, 0, 0);

                for(int pixelX = 0; pixelX < image.Width; pixelX++)
                {
                  for(int pixelY = 0; pixelY < image.Height; pixelY++)
                  {
                    if (shadowImage.GetPixel(pixelX, pixelY) != transparent)
                      shadowImage.SetPixel(pixelX, pixelY, shadowColor);
                  }
                }
		
                g.DrawImage(shadowImage, imageLeft + 1, imageTop + 1);

                // Draw an enabled icon offset up and left
                g.DrawImage(image, imageLeft - 1, imageTop - 1);
              }
              else
              {
                // Draw an faded enabled icon
                // A new bitmap so we don't change the actual image
                Bitmap fadedImage = new Bitmap(image);
                Color transparent = Color.FromArgb(0, 0, 0, 0);

                for(int pixelX = 0; pixelX < image.Width; pixelX++)
                {
                  for(int pixelY = 0; pixelY < image.Height; pixelY++)
                  {
                    Color pixelColor = fadedImage.GetPixel(pixelX, pixelY);
                    if (pixelColor != transparent) 
                    {
                      Color newPixelColor = Color.FromArgb((pixelColor.R + 76) - (((pixelColor.R + 32) / 64) * 19), 			
                        (pixelColor.G + 76) - (((pixelColor.G + 32) / 64) * 19), 
                        (pixelColor.B + 76) - (((pixelColor.B + 32) / 64) * 19));
									
                      fadedImage.SetPixel(pixelX, pixelY, newPixelColor);
                    }
                  }
                }

                g.DrawImage(fadedImage, imageLeft, imageTop);

                // Draw an enabled icon
                //g.DrawImage(image, imageLeft, imageTop);
              }
            }
            else
            {
              // Draw a image disabled
              ControlPaint.DrawImageDisabled(g, image, imageLeft, imageTop, SystemColors.HighlightText);
            }
          }    

          // Does the menu have a submenu defined?
          if (dc.SubMenu)
          {
            // Is the item enabled?
            if (mc.Enabled)
            {
              int subMenuIndex = (int)ImageIndex.SubMenu;

              if (hotCommand && (m_style == VisualStyle.Plain))
                subMenuIndex = (int)ImageIndex.SubMenuSelected;

              // Draw the submenu arrow 
              g.DrawImage(m_menuImages.Images[subMenuIndex], subMenuX, imageTop);
            }
            else
            {
              // Draw a image disabled
              ControlPaint.DrawImageDisabled(g, m_menuImages.Images[(int)ImageIndex.SubMenu], 
                subMenuX, imageTop, m_highlightTextColor);
            }
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

    protected string GetShortcutText(Shortcut shortcut)
    {
      // Get the key code
      char keycode = (char)((int)shortcut & 0x0000FFFF);

      // The type converter does not work for numeric values as it returns
      // Alt+D0 instad of Alt+0. So check for numeric keys and construct the
      // return string ourself.
      if ((keycode >= '0') && (keycode <= '9'))
      {
        string display = "";

        // Get the modifier
        int modifier = (int)((int)shortcut & 0xFFFF0000);
		
        if ((modifier & 0x00010000) != 0)
          display += "Shift+";

        if ((modifier & 0x00020000) != 0)
          display += "Ctrl+";
				
        if ((modifier & 0x00040000) != 0)
          display += "Alt+";

        display += keycode;

        return display;
      }
      else
      {
        return TypeDescriptor.GetConverter(typeof(Keys)).ConvertToString((Keys)shortcut);
      }
    }

    protected bool ProcessKeyUp()
    {
      int newItem = m_trackItem;
      int startItem = newItem;

      for(int i=0; i<m_drawCommands.Count; i++)
      {
        // Move to previous item
        newItem--;

        // Have we looped all the way around all the choices
        if (newItem == startItem)
          return false;

        // Check limits
        if (newItem < 0)
          newItem = m_drawCommands.Count - 1;

        DrawCommand dc = m_drawCommands[newItem] as DrawCommand;

        // Can we select this item?
        if (!dc.Separator && dc.Enabled)
        {
          // If a change has occurred
          if (newItem != m_trackItem)
          {
            // Modify the display of the two items 
            SwitchSelection(m_trackItem, newItem, false, false);

            return true;
          }
        }
      }

      return false;
    }

    protected bool ProcessKeyDown()
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
        if (!dc.Separator && dc.Enabled)
        {
          // If a change has occurred
          if (newItem != m_trackItem)
          {
            // Modify the display of the two items 
            SwitchSelection(m_trackItem, newItem, false, false);

            return true;
          }
        }
      }

      return false;
    }

    protected void ProcessKeyLeft()
    {
      // Are we the first submenu of a parent control?
      bool autoLeft = (m_parentMenu != null) || (m_parentControl != null);
      bool checkKeys = false;

      if (m_trackItem != -1)
      {
        // Get the col this item is in
        DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;
	
        // Grab the current column/row values
        int col = dc.Col;
        int row = dc.Row;

        // If not in the first column then move left one
        if (col > 0)
        {
          int newItem = -1;
          int newRow = -1;
          int findCol = col - 1;
          DrawCommand newDc = null;

          for(int i=0; i<m_drawCommands.Count; i++)
          {
            DrawCommand listDc = m_drawCommands[i] as DrawCommand;
					
            // Interested in cells in the required column
            if (listDc.Col == findCol)
            {
              // Is this Row nearer to the one required than those found so far?							
              if ((listDc.Row <= row) && (listDc.Row > newRow) && 
                !listDc.Separator && listDc.Enabled)
              {
                // Remember this item
                newRow = listDc.Row;
                newDc = listDc;
                newItem = i;
              }
            }
          }

          if (newDc != null)
          {
            // Track the new item
            // Modify the display of the two items 
            SwitchSelection(m_trackItem, newItem, false, false);
          }
          else
            checkKeys = true;
        }
        else
          checkKeys = true;
      }
      else
      {
        if (m_parentMenu != null)
        {
          if (!ProcessKeyUp())
            checkKeys = true;
        }
        else
          checkKeys = true;
      }

      // If we have a parent control and nothing to move right into
      if (autoLeft && checkKeys)
      {
        m_returnCommand = null;

        // Finish processing messages
        m_timer.Stop();
        m_exitLoop = true;	

        // Only a top level PopupMenu should cause the MenuControl to select the 
        // next left menu command. A submenu should just become dismissed.
        if (m_parentMenu == null)
          m_returnDir = -1;
      }
    }

    protected bool ProcessKeyRight()
    {
      // Are we the first submenu of a parent control?
      bool autoRight = (m_parentControl != null);
      bool checkKeys = false;
      bool ret = false;

      // Is an item currently selected?
      if (m_trackItem != -1)
      {
        DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

        // Does this item have a submenu?
        if (dc.SubMenu)
        {
          // Handle the submenu
          OperateSubMenu(m_trackItem, true);

          ret = true;
        }
        else
        {
          // Grab the current column/row values
          int col = dc.Col;
          int row = dc.Row;

          // If not in the first column then move left one
          int newItem = -1;
          int newRow = -1;
          int findCol = col + 1;
          DrawCommand newDc = null;

          for(int i=0; i<m_drawCommands.Count; i++)
          {
            DrawCommand listDc = m_drawCommands[i] as DrawCommand;
					
            // Interesting in cells in the required column
            if (listDc.Col == findCol)
            {
              // Is this Row nearer to the one required than those found so far?							
              if ((listDc.Row <= row) && (listDc.Row > newRow) && 
                !listDc.Separator && listDc.Enabled)
              {
                // Remember this item
                newRow = listDc.Row;
                newDc = listDc;
                newItem = i;
              }
            }
          }

          if (newDc != null)
          {
            // Track the new item
            // Modify the display of the two items 
            SwitchSelection(m_trackItem, newItem, false, false);
          }
          else
            checkKeys = true;
        }
      }
      else
      {
        if (m_parentMenu != null)
        {
          if (!ProcessKeyDown())
            checkKeys = true;
        }
        else
          checkKeys = true;
      }

      // If we have a parent control and nothing to move right into
      if (autoRight && checkKeys)
      {
        m_returnCommand = null;

        // Finish processing messages
        m_timer.Stop();
        m_exitLoop = true;	

        m_returnDir = 1;
      }

      return ret;
    }

    protected int ProcessMnemonicKey(char key)
    {
      // Check against each draw command mnemonic
      for(int i=0; i<m_drawCommands.Count; i++)
      {
        DrawCommand dc = m_drawCommands[i] as DrawCommand;

        if (dc.Enabled)
        {
          // Does the character match?
          if (key == dc.Mnemonic)
          {
            // Does this have any submenu?
            if (dc.SubMenu)
            {
              // Is there a change in selected item?
              if (m_trackItem != i)
              {
                // Modify the display of the two items 
                SwitchSelection(m_trackItem, i, true, false);
              }

              return -1;
            }
            else
            {
              // No submenu so just selected the item which
              // will cause the PopupMenu to exit
              return i;
            }
          }
        }
      }

      // No match found
      return -1;
    }

    protected Win32.POINT MousePositionToScreen(Win32.MSG msg)
    {
      Win32.POINT screenPos;
      screenPos.x = (short)((uint)msg.lParam & 0x0000FFFFU);
      screenPos.y = (short)(((uint)msg.lParam & 0xFFFF0000U) >> 16);

      // Convert the mouse position to screen coordinates
      User32.ClientToScreen(msg.hwnd, ref screenPos);

      return screenPos;
    }

    protected bool ParentControlWantsMouseMessage(Win32.POINT screenPos, ref Win32.MSG msg)
    {
      // Special case the MOUSEMOVE so if we are part of a MenuControl
      // then we should let the MenuControl process that message
      if ((msg.message == (int)Win32.Msgs.WM_MOUSEMOVE) && (m_parentControl != null))
      {
        Win32.RECT rectRaw = new Win32.RECT();

        // Grab the screen rectangle of the parent control
        User32.GetWindowRect(m_parentControl.Handle, ref rectRaw);
				
        if ((screenPos.x >= rectRaw.left) && (screenPos.x <= rectRaw.right) &&
          (screenPos.y >= rectRaw.top) && (screenPos.y <= rectRaw.bottom))
          return true;
      }

      return false;
    }
				
    internal PopupMenu ParentPopupMenuWantsMouseMessage(Win32.POINT screenPos, ref Win32.MSG msg)
    {
      if (m_parentMenu != null)
        return m_parentMenu.WantMouseMessage(screenPos);

      return null;
    }

    protected PopupMenu WantMouseMessage(Win32.POINT screenPos)
    {
      Win32.RECT rectRaw = new Win32.RECT();

      // Grab the screen rectangle of the window
      User32.GetWindowRect(this.Handle, ref rectRaw);

      if ((screenPos.x >= rectRaw.left) && (screenPos.x <= rectRaw.right) &&
        (screenPos.y >= rectRaw.top) && (screenPos.y <= rectRaw.bottom))
        return this;

      if (m_parentMenu != null)
        return m_parentMenu.WantMouseMessage(screenPos);

      return null;
    }

    protected void SwitchSelection(int oldItem, int newItem, bool mouseChange, bool reverting)
    {
      bool updateWindow = false;

      // Create a graphics object for drawing with
      using(Graphics g = Graphics.FromHwnd(this.Handle))
      {
        // Deselect the old draw command
        if (oldItem != -1)
        {
          DrawCommand dc = m_drawCommands[oldItem] as DrawCommand;

          // Draw old item not selected
          if (m_layered)
            updateWindow = true;
          else
            DrawSingleCommand(g, m_drawCommands[oldItem] as DrawCommand, false);

          // Generate an unselect event
          if (dc.MenuCommand != null)
            OnDeselected(dc.MenuCommand);
        }
				
        if (newItem != -1)
        {
          // Stop the timer as a new selection has occurred
          m_timer.Stop();

          // Do we have a child menu?
          if (!reverting && (m_childMenu != null))
          {
            // Start timer to test if it should be dismissed
            m_timer.Interval = m_selectionDelay;
            m_timer.Start();
          }

          DrawCommand dc = m_drawCommands[newItem] as DrawCommand;

          // Select the new draw command
          if (!dc.Separator && dc.Enabled)
          {
            // Draw the newly selected item
            if (m_layered)
              updateWindow = true;
            else
              DrawSingleCommand(g, dc, true);

            // Only is mouse movement caused the selection change...
            if (!reverting && mouseChange)
            {
              //...should we start a timer to test for sub menu displaying
              if (dc.Expansion)
                m_timer.Interval = m_expansionDelay;
              else
                m_timer.Interval = m_selectionDelay;

              m_timer.Start();
            }

            // Generate an unselect event
            if (dc.MenuCommand != null)
              OnSelected(dc.MenuCommand);
          }
          else
          {
            // Cannot become selected
            newItem = -1;
          }
        }
	
        // Remember the new selection
        m_trackItem = newItem;
				
        if (m_layered && updateWindow)
        {
          // Update the image for display
          UpdateLayeredWindow();
        }
      }
    }

    protected void OnTimerExpire(object sender, EventArgs e)
    {
      // Prevent it expiring again
      m_timer.Stop();

      bool showPopup = true;

      // Is a popup menu already being displayed?
      if (m_childMenu != null)
      {
        // If the submenu popup is for a different item?
        if (m_popupItem != m_trackItem)
        {
          // Then need to kill the submenu
          User32.PostMessage(m_childMenu.Handle, WM_DISMISS, 0, 0); 
        }
        else
          showPopup = false;
      }

      // Should we show the popup for this item
      if (showPopup)
      {
        // Check an item really is selected
        if (m_trackItem != -1)
        {
          DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

          // Does this item have a submenu?
          if (dc.SubMenu)
            OperateSubMenu(m_trackItem, false);
          else
          {
            if (dc.Expansion)
              RegenerateExpansion();
          }
        }
      }
    }

    protected void OperateSubMenu(int popupItem, bool selectFirst)
    {
      User32.PostMessage(this.Handle, WM_OPERATE_SUBMENU, (uint)popupItem, (uint)(selectFirst ? 1 : 0));
    }

    protected void OnWM_OPERATE_SUBMENU(ref Message m)
    {
      int popupItem = (int)m.WParam;
      bool selectFirst = (m.LParam != IntPtr.Zero);

      m_popupItem = popupItem;
      m_childMenu = new PopupMenu();

      DrawCommand dc = m_drawCommands[popupItem] as DrawCommand;

      // Find screen coordinate of Top right of item cell
      Win32.POINT screenPosTR;
      screenPosTR.x = dc.DrawRect.Right;
      screenPosTR.y = dc.DrawRect.Top;
      User32.ClientToScreen(this.Handle, ref screenPosTR);
			
      // Find screen coordinate of top left of item cell
      Win32.POINT screenPosTL;
      screenPosTL.x = dc.DrawRect.Left;
      screenPosTL.y = dc.DrawRect.Top;
      User32.ClientToScreen(this.Handle, ref screenPosTL);

      // Ensure the child has the same properties as ourself
      m_childMenu.Style = this.Style;
      m_childMenu.Font = this.Font;
      m_childMenu.BackColor = this.BackColor;
      m_childMenu.TextColor = this.TextColor;
      m_childMenu.HighlightTextColor = this.HighlightTextColor;
      m_childMenu.HighlightColor = this.HighlightColor;
      m_childMenu.Animate = this.Animate;
      m_childMenu.AnimateStyle = this.AnimateStyle;
      m_childMenu.AnimateTime = this.AnimateTime;
			
      // Record keyboard direction
      int returnDir = 0;

      // Propogate the remembering of expansion state
      m_childMenu.RememberExpansion = m_rememberExpansion;

      // Honour the collections request for showing infrequent items
      m_childMenu.m_showInfrequent = dc.MenuCommand.MenuCommands.ShowInfrequent;

      // Propogate the highlight property
      m_childMenu.HighlightInfrequent = m_highlightInfrequent;
            
      // Generate event so that caller has chance to modify MenuCommand contents
      dc.MenuCommand.OnPopupStart();
            
      m_returnCommand = m_childMenu.InternalTrackPopup(new Point(screenPosTR.x, screenPosTR.y),
        new Point(screenPosTL.x, screenPosTL.y), 
        dc.MenuCommand.MenuCommands, 
        this, 
        selectFirst,
        m_parentControl,
        m_popupRight,
        m_popupDown,
        m_animateFirst,
        ref returnDir);

      // Generate event so that caller has chance to modify MenuCommand contents
      dc.MenuCommand.OnPopupEnd();
            
      m_popupItem = -1;;
      m_childMenu = null;

      // Subsequent times a submenu is shown we do not want it to animate
      m_animateFirst = false;

      if ((m_returnCommand != null) || (returnDir != 0))
      {
        // Finish processing messages
        m_timer.Stop();
        m_exitLoop = true;	
        m_returnDir = returnDir;
      }
    }
        
    public virtual void OnSelected(MenuCommand mc)
    {
      // If initiated by a MenuControl item then let the control handle this
      if (m_parentControl != null)
        m_parentControl.OnSelected(mc);
      else
      {
        // If we have an event defined then fire it
        if (Selected != null)
          Selected(mc);
        else
        {
          // Maybe our parent has an event defined instead
          if (m_parentMenu != null)
            m_parentMenu.OnSelected(mc);
        }
      }
    }

    public virtual void OnDeselected(MenuCommand mc)
    {
      // If initiated by a MenuControl item then let the control handle this
      if (m_parentControl != null)
        m_parentControl.OnDeselected(mc);
      else
      {
        // If we have an event defined then fire it
        if (Deselected != null)
          Deselected(mc);
        else
        {
          // Maybe our parent has an event defined instead
          if (m_parentMenu != null)
            m_parentMenu.OnDeselected(mc);
        }
      }
    }

    protected void OnWM_PAINT(ref Message m)
    {
      Win32.PAINTSTRUCT ps = new Win32.PAINTSTRUCT();

      // Have to call BeginPaint whenever processing a WM_PAINT message
      IntPtr hDC = User32.BeginPaint(m.HWnd, ref ps);

      Win32.RECT rectRaw = new Win32.RECT();

      // Grab the screen rectangle of the window
      User32.GetWindowRect(this.Handle, ref rectRaw);

      // Convert to a client size rectangle
      Rectangle rectWin = new Rectangle(0, 0, 
        rectRaw.right - rectRaw.left, 
        rectRaw.bottom - rectRaw.top);

      // Create a graphics object for drawing
      using(Graphics g = Graphics.FromHdc(hDC))
      {
        // Create bitmap for drawing onto
        Bitmap memoryBitmap = new Bitmap(rectWin.Width, rectWin.Height);

        using(Graphics h = Graphics.FromImage(memoryBitmap))
        {
          // Draw the background area
          DrawBackground(h, rectWin);

          // Draw the actual menu items
          DrawAllCommands(h);
        }

        // Blit bitmap onto the screen
        g.DrawImageUnscaled(memoryBitmap, 0, 0);
      }

      // Don't forget to end the paint operation!
      User32.EndPaint(m.HWnd, ref ps);
    }

    protected void OnWM_ACTIVATEAPP(ref Message m)
    {
      // Another application has been activated, so we need to kill ourself	
      m_timer.Stop();
      m_exitLoop = true;
    }

    protected void SubMenuMovement()
    {
      // Cancel timer to prevent auto closing of an open submenu
      m_timer.Stop();

      // Has the selected item changed since child menu shown?
      if (m_popupItem != m_trackItem)
      {
        // Need to put it back again
        SwitchSelection(m_trackItem, m_popupItem, false, true);
      }

      // Are we a submenu?
      if (m_parentMenu != null)
      {
        // Inform parent that we have movement and so do not
        // use a timer to close us up
        m_parentMenu.SubMenuMovement();
      }
    }

    protected void OnWM_MOUSEMOVE(int xPos, int yPos)
    {
      // Convert from screen to client coordinates
      xPos -= m_currentPoint.X;
      yPos -= m_currentPoint.Y;

      // Are we a submenu?
      if (m_parentMenu != null)
      {
        // Inform parent that we have movement and so do not
        // use a timer to close us up
        m_parentMenu.SubMenuMovement();
      }

      // Yes, we know the mouse is over window
      m_mouseOver = true;

      Point pos = new Point(xPos, yPos);

      // Has mouse position really changed since last time?
      if (m_lastMousePos != pos)
      {
        for(int i=0; i<m_drawCommands.Count; i++)
        {
          DrawCommand dc = m_drawCommands[i] as DrawCommand;

          if (dc.DrawRect.Contains(pos))
          {
            // Is there a change in selected item?
            if (m_trackItem != i)
            {
              // Modify the display of the two items 
              SwitchSelection(m_trackItem, i, true, false);
            }
          }
        }

        // Remember for next time around
        m_lastMousePos = pos;
      }
    }

    protected void OnWM_MOUSELEAVE()
    {
      // Deselect the old draw command if not showing a child menu
      if ((m_trackItem != -1) && (m_childMenu == null))
      {
        // Modify the display of the two items 
        SwitchSelection(m_trackItem, -1, false, false);
      }

      // Reset flag so that next mouse move start monitor for mouse leave message
      m_mouseOver = false;

      // No point having a last mouse position
      m_lastMousePos = new Point(-1,-1);
    }

    protected void OnWM_YBUTTONUP(int xPos, int yPos)
    {
      // Convert from screen to client coordinates
      xPos -= m_currentPoint.X;
      yPos -= m_currentPoint.Y;

      Point pos = new Point(xPos, yPos);

      for(int i=0; i<m_drawCommands.Count; i++)
      {
        DrawCommand dc = m_drawCommands[i] as DrawCommand;

        if (dc.DrawRect.Contains(pos))
        {
          // Is there a change in selected item?
          if (m_trackItem != i)
          {
            // Modify the display of the two items 
            SwitchSelection(m_trackItem, i, false, false);
          }
        }
      }

      // Is an item selected?			
      if (m_trackItem != -1)
      {
        DrawCommand dc = m_drawCommands[m_trackItem] as DrawCommand;

        // Does this item have a submenu?
        if (dc.SubMenu)
        {
          // If we are not already showing this submenu...
          if (m_popupItem != m_trackItem)
          {
            // Is a submenu for a different item showing?
            if (m_childMenu != null)
            {
              // Inform the child menu it is no longer needed
              User32.PostMessage(m_childMenu.Handle, WM_DISMISS, 0, 0); 
            }

            // Handle the submenu
            OperateSubMenu(m_trackItem, false);
          }
        }
        else
        {
          if (dc.Expansion)
            RegenerateExpansion();
          else
          {
            // Kill any child menus open
            if (m_childMenu != null)
            {
              // Inform the child menu it is no longer needed
              User32.PostMessage(m_childMenu.Handle, WM_DISMISS, 0, 0); 
            }

            // Define the selection to return to caller
            m_returnCommand = dc.MenuCommand;

            // Finish processing messages
            m_timer.Stop();
            m_exitLoop = true;	
          }
        }
      }
    }

    protected void OnWM_MOUSEACTIVATE(ref Message m)
    {
      // Do not allow the mouse down to activate the window, but eat 
      // the message as we still want the mouse down for processing
      m.Result = (IntPtr)Win32.MouseActivateFlags.MA_NOACTIVATE;
    }

    protected void OnWM_SETCURSOR()
    {
      // Always use the arrow cursor
      User32.SetCursor(User32.LoadCursor(IntPtr.Zero, (uint)Win32.Cursors.IDC_ARROW));
    }
		
    protected void OnWM_DISMISS()
    {	
      // Pass on to any child menu of ours
      if (m_childMenu != null)
      {
        // Inform the child menu it is no longer needed
        User32.PostMessage(m_childMenu.Handle, WM_DISMISS, 0, 0); 
      }
			
      // Define the selection to return to caller
      m_returnCommand = null;

      // Finish processing messages
      m_timer.Stop();
      m_exitLoop = true;	

      // Hide ourself
      HideMenuWindow();

      // Kill ourself
      DestroyHandle();
    }

    protected bool OnWM_NCHITTEST(ref Message m)
    {
      // Get mouse coordinates
      Win32.POINT screenPos;
      screenPos.x = (short)((uint)m.LParam & 0x0000FFFFU);
      screenPos.y = (short)(((uint)m.LParam & 0xFFFF0000U) >> 16);

      // Only the IDE style has shadows
      if (m_style == VisualStyle.IDE)
      {	
        Win32.POINT popupPos;
        popupPos.x = m_currentSize.Width - m_position[(int)m_style, (int)PI.ShadowWidth];
        popupPos.y = m_currentSize.Height - m_position[(int)m_style, (int)PI.ShadowHeight];

        // Convert the mouse position to screen coordinates
        User32.ClientToScreen(this.Handle, ref popupPos);

        // Is the mouse in the shadow areas?
        if ((screenPos.x > popupPos.x) ||
          (screenPos.y > popupPos.y))
        {
          // Allow actions to occur to window beneath us
          m.Result = (IntPtr)Win32.HitTest.HTTRANSPARENT;

          return true;
        }
      }

      return false;
    }

    protected override void WndProc(ref Message m)
    {
      //			Console.WriteLine("WndProc(PopupMenu) {0} {1}", this.Handle, ((Win32.Msgs)m.Msg).ToString());

      // WM_DISMISS is not a constant and so cannot be in a switch
      if (m.Msg == WM_DISMISS)
        OnWM_DISMISS();
      else if (m.Msg == WM_OPERATE_SUBMENU)
        OnWM_OPERATE_SUBMENU(ref m);
      else
      {
        // Want to notice when the window is maximized
        switch(m.Msg)
        {
          case (int)Win32.Msgs.WM_ACTIVATEAPP:
            OnWM_ACTIVATEAPP(ref m);
            break;
          case (int)Win32.Msgs.WM_MOUSEACTIVATE:
            OnWM_MOUSEACTIVATE(ref m);
            break;
          case (int)Win32.Msgs.WM_PAINT:
            OnWM_PAINT(ref m);
            break;
          case (int)Win32.Msgs.WM_SETCURSOR:
            OnWM_SETCURSOR();
            break;
          case (int)Win32.Msgs.WM_NCHITTEST:
            if (!OnWM_NCHITTEST(ref m))
              base.WndProc(ref m);
            break;
          default:
            base.WndProc(ref m);
            break;
        }
      }
    }

    protected static Bitmap GetShadowCache(int width, int height)
    {
      // Do we already have a cached bitmap of the correct size?
      if ((m_shadowCacheWidth == width) && (m_shadowCacheHeight == height) && (m_shadowCache != null))
        return m_shadowCache;

      // Dispose of any previously cached bitmap	
      if (m_shadowCache != null)
        m_shadowCache.Dispose();
	
      // Create our new bitmap with 32bpp so we have an alpha channel
      Bitmap image = new Bitmap(width, height, PixelFormat.Format32bppArgb);
            
      // We want direct access to the bits so we can change values
      BitmapData data = image.LockBits(new System.Drawing.Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
				
      unsafe
      {
        // Direct pointer to first line
        uint* pixptr = (uint*)(data.Scan0);
					
        // For each row
        for (int y = 0; y < height; y++)
        {
          int offset = data.Stride * y / 4;
                    
          // Fade each line as we go down
          int alphay = 64 * (height - y) / (height + 1);
						
          // For each column pixel
          for (int x = 0; x < width; x++)
          {
            // Fade each pixel as we go across
            int alphax = alphay * (width - x) / (width + 1);
            pixptr[offset+x] = (uint)alphax << 24;
          }
        }
      }
	
      image.UnlockBits(data);

      // Cache values for next time around	
      m_shadowCache = image;
      m_shadowCacheWidth = width;
      m_shadowCacheHeight = height;
	
      return m_shadowCache;
    }
  }
}



#region revision history

// $Log: PopupMenu.cs,v $
// Revision 1.1  2005/10/11 14:40:23  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Menus/PopupMenu.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
// $Id: PopupMenu.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $

#endregion

