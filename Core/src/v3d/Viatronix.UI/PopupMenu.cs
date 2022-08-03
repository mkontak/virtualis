// $Id: PopupMenu.cs,v 1.5 2006/11/16 20:45:26 gdavidson Exp $
//
// Copyright (c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Collections;
using System.Configuration;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region PopupItem
  
  /// <summary>
  /// Represents an individual item that is displayed within a PopupMenu.
  /// </summary>
  public class PopupItem : PopupMenu
  {
    #region events

    /// <summary>
    /// Raised when a menu item is clicked
    /// </summary>
    public event EventHandler Click;

    #endregion

    #region fields

    /// <summary>
    /// Tag for item
    /// </summary>
    private string m_tag = string.Empty;

    /// <summary>
    /// Item text.
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// Item accelerator text.
    /// </summary>
    private string m_accelerator = string.Empty;

    /// <summary>
    /// Item image to displayed within menu column.
    /// </summary>
    private Image m_image = null;

    /// <summary>
    /// Indicates whether the menu item is enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// Indicates whether the menu item is grayed
    /// </summary>
    private bool m_grayed = false;

    /// <summary>
    /// Indicates whether the menu item is visible. What's an invisible menu item?
    ///  I have no idea, but visible means something to C#, so it's here for compatibility.
    /// Presently, no UI cues or functionality are affected by this flag
    /// </summary>
    private bool m_visible = true;
    
    /// <summary>
    /// Indicates if menu item is checked.
    /// </summary>
    private bool m_checked = false;

    /// <summary>
    /// Index within the menu.
    /// </summary>
    private int m_index = -1;

    /// <summary>
    /// Associated data.
    /// </summary>
    private object m_data = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the tag.
    /// </summary>
    public string Tag
    {
      get{ return m_tag; }
      set{ m_tag = value; }
    } // Tag

    /// <summary>
    /// Gets or sets the text.
    /// </summary>
    public string Text
    {
      get{ return m_text; }
      set{ m_text = value; }
    } // Text

    /// <summary>
    /// Gets or sets the accelerator text.
    /// </summary>
    public string Accelerator
    {
      get{ return m_accelerator; }
      set{ m_accelerator = value; }
    } // Accelerator

    /// <summary>
    /// Gets or sets the image.
    /// </summary>
    public Image Image
    {
      get { return m_image; }
      set { m_image = value; }
    } // Image


    /// <summary>
    /// Gets or sets the grayed state.
    /// </summary>
    public bool Grayed
    {
      get { return m_grayed; }
      set { m_grayed = value; }
    }


    /// <summary>
    /// Gets or sets the enabled state.
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    }


    /// <summary>
    /// Gets or sets the checked state.
    /// </summary>
    public bool Checked
    {
      get { return m_checked; }
      set { m_checked = value; }
    } // Checked


    /// <summary>
    /// Indicates whether the menu item is visible. What's an invisible menu item?
    ///  I have no idea, but visible means something to C#, so it's here for compatibility.
    /// Presently, no UI cues or functionality are affected by this flag
    /// </summary>
    public bool Visible
    {
      get { return m_visible; }
      set { m_visible = value; }
    }


    /// <summary>
    /// Gets or Sets the index.
    /// </summary>
    public int Index
    {
      get { return m_index; }
      set { m_index = value; }
    } // Index
    
    
    /// <summary>
    /// Gets or sets the associated data.
    /// </summary>
    public object Data
    {
      get { return m_data; }
      set { m_data = value; }
    } // Data


    /// <summary>
    /// Gets if this item is a separator.
    /// </summary>
    public bool IsSeparator
    {
      get { return m_text == "-"; }
    } // IsSeparator
    
    
    /// <summary>
    /// Gets if this item contains a sub menu.
    /// </summary>
    public bool IsSubMenu
    {
      get { return this.Items.Count > 0; }
    } // IsSubMenu
    
    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes an empty PopupItem.
    /// </summary>
    public PopupItem()
    {
      m_image = null;
      m_text = string.Empty;
      m_accelerator = string.Empty;
      m_checked = false;
      m_enabled = true;
      m_grayed = false;
    } // PopupItem()

    
    /// <summary>
    /// Initializes a PopupItem with the specified text.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    public PopupItem( string text )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = null;
      m_checked = false;
      m_data = null;
      m_enabled = true;
      m_grayed = false;
    } // PopupItem( text )
    
    
    /// <summary>
    /// Initializes a PopupItem with the specified text and image.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="image">The image for the menu item<./param>
    public PopupItem( string text, Bitmap image )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = image;
      m_checked = false;
      m_data = null;
      m_enabled = true;
      m_grayed = false;
    } // PopupItem( text, image )    


    /// <summary>
    /// Initializes a PopupItem with the specified text, image and data.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="image">The image for the menu item<./param>
    /// <param name="data">The associated data for the menu item<./param>
    public PopupItem( string text, Bitmap image, object data )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = image;
      m_checked = false;
      m_data = data;
      m_enabled = true;
      m_grayed = false;
    } // PopupItem( text, image, data )    

    
    /// <summary>
    /// Initializes a PopupItem with the specified text and click handler.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="handler">The click handler for the menu item.</param>
    public PopupItem( string text, EventHandler handler )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = null;
      m_checked = false;
      m_data = null;
      m_enabled = true;
      m_grayed = false;

      this.Click += handler;
    } // PopupItem( text, handler )   
    
    
    /// <summary>
    /// Initializes a PopupItem with the specified text, accelerator and click handler.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="accelerator">The text for the accelerator.</param>
    /// <param name="handler">The click handler for the menu item.</param>
    public PopupItem( string text, string accelerator, EventHandler handler )
    {
      m_text = text;
      m_accelerator = accelerator;
      m_image = null;
      m_checked = false;
      m_data = null;
      m_enabled = true;
      m_grayed = false;

      this.Click += handler;
    } // PopupItem( text, accelerator, handler )   
    
    
    /// <summary>
    /// Initializes a PopupItem with the specified text, image and click handler.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="image">The image for the menu item<./param>
    /// <param name="handler">The click handler for the menu item.</param>
    public PopupItem( string text, Bitmap image, EventHandler handler )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = image;
      m_checked = false;
      m_data = null;
      m_enabled = true;
      m_grayed = false;

      this.Click += handler;
    } // PopupItem( text, image, handler )
    
    
    /// <summary>
    /// Initializes a PopupItem with the specified text, image, click handler and data.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="image">The image for the menu item<./param>
    /// <param name="handler">The click handler for the menu item.</param>
    /// <param name="data">The associated data for the menu item<./param>
    public PopupItem( string text, Bitmap image, EventHandler handler, object data )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = image;
      m_checked = false;
      m_data = data;
      m_enabled = true;
      m_grayed = false;

      this.Click += handler;
    } // PopupItem( text, image, handler, data )

    
    /// <summary>
    /// Initializes a PopupItem with the specified text, image, click handler and data.
    /// </summary>
    /// <param name="text">The text for the menu item.</param>
    /// <param name="handler">The click handler for the menu item.</param>
    /// <param name="data">The associated data for the menu item<./param>
    public PopupItem( string text, EventHandler handler, object data )
    {
      m_text = text;
      m_accelerator = string.Empty;
      m_image = null;
      m_checked = false;
      m_data = data;
      m_enabled = true;
      m_grayed = false;

      this.Click += handler;
    } // PopupItem( text, handler, data )
    
    
    #endregion

    #region methods

    /// <summary>
    /// Raises the item's click event
    /// </summary>
    internal void RaiseClickEvent()
    {
      if( this.Click != null )
        Click( this, System.EventArgs.Empty );
    } // RaiseClickEvent()

    
    #endregion
  
  } // class PopupItem

  
  #endregion

  #region PopupItemCollection

  /// <summary>
  /// Represents a collection of PopupItem objects.
  /// </summary>
  public class PopupItemCollection : CollectionBase
  {
      #region methods
    
    /// <summary>
    /// Adds the specified popupItem to the collection.
    /// </summary>
    /// <param name="item">PopupItem to add.</param>
    /// <returns>Array index of added object</returns>
    public int Add( PopupItem item )
    {
      return List.Add( item );
    } // Add( item )
    
      
    /// <summary>
    /// Adds a PopupItem to the collection with the specified text.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <returns>Array index of added object</returns>
    public int Add( string text )
    {
      return Add( new PopupItem( text ) );
    } // Add( text )

      
    /// <summary>
    /// Adds a PopupItem to the collection with the specified text and image.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="image">Item image.</param>    
    /// <returns>Array index of added object</returns>
    public int Add( string text, Bitmap image )
    {
      return Add( new PopupItem( text, image ) );
    } //  Add( text, image )
    

    /// <summary>
    /// Adds a PopupItem to the collection with the specified text, image and data.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="image">Item image.</param>    
    /// <param name="data">Item data.</param>
    /// <returns>Array index of added object</returns>
    public int Add( string text, Bitmap image, object data )
    {
      return Add( new PopupItem( text, image, data ) );
    } // Add( text, image, data )
    

    /// <summary>
    /// Adds a popupItem to the collection with the specified text and click handler.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="clickHandler">Item click handler.</param>
    /// <returns>Array index of added object</returns>
    public int Add( string text, EventHandler clickHandler )
    {
      return Add( new PopupItem( text, clickHandler ) );
    } // Add( text, clickHandler )
    

    /// <summary>
    /// Adds a popupItem to the collection with the specified text, image and click handler.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="image">Item image.</param>
    /// <param name="clickHandler">Item click handler.</param>
    /// <returns>Array index of added object</returns>
    public int Add( string text, Bitmap image, EventHandler clickHandler )
    {
      return Add( new PopupItem( text, image, clickHandler ) );
    } // Add( text, image, clickHandler )
    
      
    /// <summary>
    /// Adds a popupItem to the collection with the specified text, image, click handler and data.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="image">Item image.</param>
    /// <param name="clickHandler">Item click handler.</param>
    /// <param name="data">Item data.</param>
    /// <returns>Array index of added object</returns>
    public int Add( string text, Bitmap image, EventHandler clickHandler, object data )
    {
      return Add( new PopupItem( text, image, clickHandler, data ) );
    } // Add( text, image, clickHandler, data )
    
    
    /// <summary>
    /// Adds the specified range of PopupItem objects to the collection.
    /// </summary>
    /// <param name="items">PopupItem array to add to collection.</param>
    public void AddRange( PopupItem[] items )
    {
      foreach( PopupItem item in items )
        Add( item );
    } // AddRange( items )


    /// <summary>
    /// Inserts the specified popupItem to the collection.
    /// </summary>
    /// <param name="index">index</param>
    /// <param name="item">PopupItem to add.</param>
    public void Insert( int index, PopupItem item )
    {
      List.Insert( index, item );
    } // Add( item )
    
      
    /// <summary>
    /// Indexer to get or set a PopupItem object.
    /// </summary>
    public PopupItem this[ int index ] 
    {
      get { return List[ index ] as PopupItem; }
      set { List[ index ] = value; }
    } // this[ int index ] 


    public int IndexOf( object value )
    {
      return List.IndexOf( value );
    }
      
      #endregion
    
  } // class PopupItemCollection
  
    #endregion

  #region PopupDirection
  
  /// <summary>
  /// Specifies the direction of the PopupMenu.
  /// </summary>
  public enum PopupDirection
  {
    /// <summary>
    /// Menu will be displayed above the specified location.
    /// </summary>
    Up,
    
    /// <summary>
    /// Menu will de displayed below the specified location.
    /// </summary>
    Down,
  }; // enum PopupDirection

  #endregion

  #region PopupMenu
  
  /// <summary>
  /// Represents a shortcut menu.
  /// </summary>
  public class PopupMenu : PopupWindow
  {
    #region events

    /// <summary>
    /// Raised when the popup menu item is invoked
    /// </summary>
    public event EventHandler Popup;

    #endregion

    #region constants

    /// <summary>
    /// sensitivity time in millesconds before mouse events are processed when the menu is displayed
    /// </summary>
    private const int m_defaultMouseSensitivity = 500;
    
    /// <summary>
    /// Bevel for rounded edges.
    /// </summary>
    private const int m_bevel = 4;
    
    
    /// <summary>
    /// Separator height in pixels.
    /// </summary>
    private const int m_separatorHeight = 2;
    
    /// <summary>
    /// Minimum allowed column width in pixels.
    /// </summary>
    private const int m_minumumColumnWidth = 16;
    
    /// <summary>
    /// Extra padding to add to the overall menu size calculation.
    /// </summary>
    private static readonly int m_popupPadding = 2*m_bevel;
     
    /// <summary>
    /// Extra padding to add to the overall item height calculation.
    /// </summary>
    private static readonly int m_itemHeightPadding = 4;
    
    /// <summary>
    /// Extra padding to add to the overall item width calculation.
    /// </summary>
    private static readonly int m_itemWidthPadding = 4;
    
    /// <summary>
    /// Extra padding to add to text width calculation.
    /// </summary>
    private static readonly int m_textPadding = 6;
    
    /// <summary>
    /// Extra padding to add to column width calculation.
    /// </summary>
    private static readonly int m_columnPadding = 6;

    /// <summary>
    /// Timer interval in milliseconds.
    /// </summary>
    private static readonly int m_timerInterval = 800;

    /// <summary>
    ///  Default checkbox size.
    /// </summary>
    private static readonly Size m_checkSize = new Size( 10, 10 );
    
    /// <summary>
    /// Default arrow size.
    /// </summary>
    private static readonly Size m_arrowSize = new Size( 5, 10 );
    
    /// <summary>
    /// Extra arrow padding to add to text width calculation.
    /// </summary>
    private static readonly int m_arrowPadding = 10;

    /// <summary>
    /// Default selection color.
    /// </summary>
    private static readonly Color m_selectedColor = Color.FromArgb( 0, 0, 255 ) ;
    
    /// <summary>
    /// Default selection fill color.
    /// </summary>
    private static readonly Color m_selectedFillColor = Color.FromArgb(80, 0, 0, 255);

    #endregion
        
    #region fields

    /// <summary>
    /// Collection of popup items.
    /// </summary>
    private PopupItemCollection m_items = new PopupItemCollection();

    /// <summary>
    /// Currently tracked item.
    /// </summary>
    private int m_trackItem = -1;
    
    /// <summary>
    /// Index of menu item that was selected.
    /// </summary>
    private int m_selectedIndex = -1;

    /// <summary>
    /// x location to renderer text to.
    /// </summary>
    private int m_textOffset = -1;

    /// <summary>
    /// Width in pixels of the column.
    /// </summary>
    private int m_columnWidth = -1;

    /// <summary>
    /// Width for text rendering.
    /// </summary>
    private int m_textWidth = 0;

    /// <summary>
    /// Timer use for delaying display of sub menus.
    /// </summary>
    private System.Windows.Forms.Timer m_timer = new System.Windows.Forms.Timer();
    
    private Color m_color = Color.FromArgb( 128, 143, 180 );
    
    private Color m_columnColor = Color.FromArgb( 188, 195, 214 );
    
    private PointF[] m_innerShape = new PointF[ 0 ];    
    
    private PointF[] m_backgroundShape = new PointF[ 0 ];
    
    private PointF[] m_columnShape = new PointF[ 0 ];   
    
    private Rectangle[] m_bounds = new Rectangle[ 0 ];

    /// <summary>
    /// indicates if mouse events should be processed
    /// </summary>
    private bool m_processMouseInput = false;

    /// <summary>
    /// amount of time (milliseconds) before the mouse is enabled when the menu is first displayed
    /// </summary>
    private int m_mouseSensitivity = m_defaultMouseSensitivity;

    private Hashtable m_map = new Hashtable();

    private EventHandler m_handler = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets the Popup Items collections.
    /// </summary>
    public PopupItemCollection Items
    {
      get { return m_items; }
    }


    /// <summary>
    /// Gets the Popup Items collections (Duplicate is to keep the same syntax as standard C# menu)
    /// </summary>
    public PopupItemCollection MenuItems
    {
      get { return m_items; }
    }


    /// <summary>
    /// Gets the selected menu index.
    /// </summary>
    public int SelectedIndex
    {
      get { return m_selectedIndex; }
    }

    
    /// <summary>
    /// Gets the selected menu item.
    /// </summary>
    public PopupItem SelectedItem
    {
      get
      {
        PopupItem selected = null;

        if ( m_selectedIndex >= 0 )
          selected = m_items[ m_selectedIndex ];
        return selected;
      }
    }
    
    #endregion

    #region methods
    
    /// <summary>
    /// Shows the PopupMenu at the specified location.
    /// </summary>
    /// <param name="parent">Parent control that PopupMenu is associated.</param>
    /// <param name="position">Screen coordinates to display the PopupMenu.</param>
    /// <param name="direction">Direction releative to position that menu is displayed.</param>
    /// <returns>Selected menu item index, -1 if no selected.</returns>
    public override int Show( Control parent, Point position, PopupDirection direction )
    {
      if( Utilities.WinXpOrGreater )
      {
        ShowWindowsMenu( parent, position, direction );
        return m_selectedIndex;
      }

      if( this.Popup != null )
        Popup( this, System.EventArgs.Empty );

      // update the item's indicies
      for( int index = 0; index < m_items.Count; ++index )
        m_items[ index ].Index = index;
      
      // indicate that there is anything selected yet
      m_selectedIndex = -1;

      // LayoutMenuItems retuns the required size for this menu
      Size size = LayoutMenuItems( parent );
      
      // associate a window for this menu
      Create( parent, position, size, direction );
  
      try
      {
        string val = ConfigurationManager.AppSettings[ "MouseSensitivity" ];
        if( val != null )
          m_mouseSensitivity =  int.Parse( val );
      }
      catch( ArgumentOutOfRangeException )
      {
        m_mouseSensitivity = m_defaultMouseSensitivity;
      }
      
      // we will ignore mouse messages until the timer function gets called
      m_processMouseInput = true;

      System.Threading.TimerCallback callback = new System.Threading.TimerCallback( this.OnTimerCallback );
      System.Threading.Timer timer = new System.Threading.Timer( callback, null, m_mouseSensitivity, System.Threading.Timeout.Infinite );
  
      // enter modal loop
      BeginModal();
      
      // close and destroy the window associated with this menu
      if( m_selectedIndex == -1 )
        Dismiss();
      else
      {
        DismissAll();

        PopupItem item = this.Items[ m_selectedIndex ];

        item.RaiseClickEvent();
      }

      // return the selected item's index
      return m_selectedIndex;
    } // Show( parent, position, direction )
    

    private void OnTimerCallback( object state )
    {
      m_processMouseInput = true;
    }
    
 
    #endregion

    #region window's menus

    private void ShowWindowsMenu( Control parent, Point position, PopupDirection direction )
    {
      m_handler = new EventHandler( OnMenuItemClickedHandler );

      m_map.Clear();
      
      System.Windows.Forms.ContextMenu menu = new System.Windows.Forms.ContextMenu();

      foreach( PopupItem popupItem in m_items )
        AddMenuItems( menu.MenuItems, popupItem );

      Point pos = parent.PointToClient( position );
      
      menu.Show( parent, pos );
    }

    
    private void AddMenuItems( System.Windows.Forms.Menu.MenuItemCollection items, PopupItem popupItem )
    {
      MenuItem item = new MenuItem( popupItem.Text );
      
      item.Click += m_handler;
      item.Checked = popupItem.Checked;
      
      items.Add( item );
      
      m_map.Add( item, popupItem );
      
      foreach( PopupItem childItem in popupItem.Items )
        AddMenuItems( item.MenuItems, childItem );
    }

   
    private void OnMenuItemClickedHandler( object sender, EventArgs args )
    {
      PopupItem selected = ( PopupItem ) m_map[ sender ];

      selected.RaiseClickEvent();
      
      m_selectedIndex = -1;
      
      foreach( PopupItem pi in this.Items )
      {
        if( selected == pi )
        {
          m_selectedIndex = m_items.IndexOf( pi );
          break;
        }
      }
    }


    #endregion
  
    #region initialization
    
    /// <summary>
    /// Initializes a PopupMenu with default settings.
    /// </summary>
    public PopupMenu()
    {
      CommonInit();
    } // PopupMenu()
    
    
    /// <summary>
    /// Initializes a PopupMenu with the specified items.
    /// </summary>
    /// <param name="items">Default menu items.</param>
    public PopupMenu( PopupItem[] items )
    {
      // add the items to the 
      m_items.AddRange( items );

      CommonInit();
    } // PopupMenu( items )


    /// <summary>
    /// Common initialization routine.
    /// </summary>
    private void CommonInit()
    {
      m_timer.Interval = m_timerInterval;
      m_timer.Tick += new EventHandler( this.OnTimerHandler );
    } // CommonInit()
    
    
    #endregion

    #region submenus

    /// <summary>
    /// Event handler for timer click event.
    /// </summary>
    /// <param name="sender">Sender of the event.</param>
    /// <param name="args"> EventArgs containing data related to this event.</param>
    private void OnTimerHandler( object sender, EventArgs args )
    {
      // stop the timer
      m_timer.Stop();

      // show sub menu if enabled
      if ( (m_trackItem >= 0) && this.Items[ m_trackItem ].Enabled )
      {
        ShowSubMenu();
      }
    } // OnTimerHandler
    
    
    /// <summary>
    /// Displays associated with currently tracked menu item.
    /// </summary>
    private void ShowSubMenu()
    {
      // close any child bopen popups
      if( this.ChildPopup != null )
      {
        this.ChildPopup.Dismiss();
        this.ChildPopup = null;
      } // if( this.ChildPopup != null )

      // get the menu item associated with this event
      PopupItem item = this.Items[ m_trackItem ];

      // create a the child menu
      PopupMenu subMenu = new PopupMenu();
      
      // child popup
      this.ChildPopup = subMenu;

      // set the parent
      subMenu.ParentPopup = this;

      // add the items to the child menu
      foreach( PopupItem subitem in item.Items )
        subMenu.Items.Add( subitem );

      // display the sub menu  
      int x = this.Location.X + this.Size.Width + 1;
      int y = this.Location.Y + m_bounds[ m_trackItem ].Y;

      // force a redraw
      Refresh();

      // show the popup menu
      subMenu.Show( this.Parent, new Point( x, y ) );  
    } // ShowSubMenu()


    #endregion

    #region drawing
    
    /// <summary>
    /// Draws the entire popup menu.
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // set to highest quailty rendering settings
      Graphics gfx = args.Graphics;
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      // fill in the background shape
      using( SolidBrush brush = new SolidBrush( m_color ) )
        gfx.FillPolygon( brush, m_backgroundShape );

      // draw an outline around the background shape
      using( Pen pen = new Pen( Color.Black ) )
        gfx.DrawPolygon( pen, m_backgroundShape );

      // fill the inner m_beveled rectangle
      using( SolidBrush brush = new SolidBrush( Color.FromArgb( 240, 240, 240 ) ) )
        gfx.FillPolygon( brush, m_innerShape );
      
      // fill the column shape
      using( SolidBrush brush = new SolidBrush( m_columnColor ) )
        gfx.FillPolygon( brush, m_columnShape );

      // draw the inner m_beveled rectangle
      using( Pen pen = new Pen( Color.FromArgb( 51, 51, 102 ) ) )
        gfx.DrawPolygon( pen, m_innerShape );

      // draw the menu items 
      for( int index = 0; index < m_items.Count; ++index )
        DrawItem( gfx, m_items[ index ], m_bounds[ index ] );
    } // Draw( gfx )
    

    /// <summary>
    /// Draws a single PopupItem object
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="item">PopupItem object to draw</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    private void DrawItem( Graphics gfx, PopupItem item, Rectangle itemBounds )
    {          
      // is the item selected?
      bool bDrawGray = !item.Enabled || item.Grayed;
      if( m_trackItem == item.Index )
      {
        using( SolidBrush brush = new SolidBrush( m_selectedFillColor ) )
          gfx.FillRectangle( brush, itemBounds );
            
        using( Pen pen = new Pen( bDrawGray ? Color.Gray : m_selectedColor ) )
          gfx.DrawRectangle( pen, itemBounds );
      } // if( m_trackItem == item.Index )
          
      // is the item checked?
      if( item.Checked )
        DrawCheckMark( gfx, itemBounds, bDrawGray );
      else
        DrawItemImage( gfx, item, itemBounds, bDrawGray );

      // is this item a separator?
      if( item.IsSeparator )
        DrawSeparator( gfx, itemBounds );
      else
        DrawItemText( gfx, item, itemBounds );
      
      // does the item have any children?
      if( item.IsSubMenu )
        DrawArrow( gfx, itemBounds, bDrawGray );
    } // DrawItem( gfx, item, itemBounds )
    
    
    /// <summary>
    /// Draws the PopupItem object's image in the column area
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="item">PopupItem object to draw</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    /// <param name="bGrayed">Whether or not the item image should be grayed</param>
    private void DrawItemImage( Graphics gfx, PopupItem item, Rectangle itemBounds, bool bGrayed )
    {
      // do we need to draw an image?
      if( item.Image != null )
      {
        // calculate image position so that it is centered within the column
        int x = itemBounds.Left +  ( m_columnPadding / 2 );
        int y = itemBounds.Top  + ( m_itemHeightPadding / 2 );

        // TODO: Draw menu item image semi-transparent if bGrayed is true

        // offset the image if it is selected
        if( m_trackItem == item.Index )
          gfx.DrawImage( item.Image, x-1, y-1, item.Image.Width, item.Image.Height );
        else
          gfx.DrawImage( item.Image, x, y, item.Image.Width, item.Image.Height );
            
      } // if( item.Image != null )
    } // DrawItemImage( gfx, item, itemBounds )
        
    
    /// <summary>
    /// Draws the PopupItem object's text
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="item">PopupItem object to draw</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    private void DrawItemText( Graphics gfx, PopupItem item, Rectangle itemBounds )
    {
      // set the format for the text
      StringFormat format = new StringFormat();
      format.Alignment = StringAlignment.Near;
      format.LineAlignment = StringAlignment.Center;
      
      // create layout rectangle for text
      Rectangle layout = new Rectangle( m_textOffset, itemBounds.Top, m_textWidth, itemBounds.Height );

      // TODO: Implement menu accelerators; until till then, remove single ampersands

      // Replace single ampersands (this will skip the 2nd of double-ampersands, which is expected)
      // Note: This cannot be replaced by a call to string.Replace(), which would remove '&&' as well
      string itemText = item.Text;
      for (int index = 0; (index < itemText.Length-1) && (index = itemText.IndexOf('&', index)) >= 0; index++)
      {
        itemText = itemText.Remove(index,1);
      }

      // draw the text
      using( SolidBrush brush = new SolidBrush( (!item.Enabled || item.Grayed) ? Color.Gray : Color.Black ) )
        gfx.DrawString( itemText, this.Parent.Font, brush, layout, format );

      // draw the accelerator
      format.Alignment = StringAlignment.Far;
      using( SolidBrush brush = new SolidBrush( (!item.Enabled || item.Grayed) ? Color.Gray : Color.Black ) )
        gfx.DrawString( item.Accelerator, this.Parent.Font, brush, layout, format );

    } // DrawItemText( gfx, item, itemBounds )

    
    /// <summary>
    /// Draws a checkmark
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    /// <param name="bGrayed">Whether or not the corresponding item is grayed</param>
    private void DrawCheckMark( Graphics gfx, Rectangle itemBounds, bool bGrayed )
    {
      // create bounding rectangle for checkbox
      Rectangle bounds = new Rectangle( itemBounds.Left, itemBounds.Top, m_checkSize.Width, m_checkSize.Height );
      
      // center the checkbox within the column
      bounds.Offset( ( m_columnWidth - m_checkSize.Width ) / 2, ( itemBounds.Height - m_checkSize.Height ) / 2 );
            
      // fill in the bounds
      using( SolidBrush brush = new SolidBrush( m_selectedFillColor ) )
        gfx.FillRectangle( brush, bounds );
                
      // outline the bounds
      using( Pen pen = new Pen( bGrayed ? Color.Gray : m_selectedColor ) )
        gfx.DrawRectangle( pen, bounds );

      // draw the checkmark
      bounds.Inflate( -( bounds.Width / 3 ), -( bounds.Height / 3 ) );
      
      // calculate checkmark points
      PointF[ ] points = new PointF[ 3 ];      
      points[ 0 ].X = bounds.Left + ( bounds.Width  / 4 );
      points[ 0 ].Y = bounds.Top  + ( bounds.Height / 4 );
      points[ 1 ].X = bounds.Left + ( bounds.Width  / 3);
      points[ 1 ].Y = bounds.Bottom;
      points[ 2 ].X = bounds.Right;
      points[ 2 ].Y = bounds.Top;
    
      // connect the points
      using( Pen pen = new Pen( bGrayed ? Color.Gray : Color.Black ) )
      {
        gfx.DrawLine( pen, points[ 0 ], points[ 1 ] );
        gfx.DrawLine( pen, points[ 1 ], points[ 2 ] );
      }
    } // DrawCheckMark( gfx, bounds )

        
    /// <summary>
    /// Draws a separator
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    private void DrawSeparator( Graphics gfx, Rectangle itemBounds )
    {
      // draw the item text
      Rectangle bounds = new Rectangle( m_textOffset, itemBounds.Top, m_textWidth, itemBounds.Height );

      int y = bounds.Y  + ( ( bounds.Height - m_separatorHeight ) / 2 );        

      using( SolidBrush brush = new SolidBrush( m_columnColor ) )
        gfx.FillRectangle( brush, bounds );
    }
    

    /// <summary>
    /// Draws a arrow
    /// </summary>
    /// <param name="gfx">The Graphics object to draw on</param>
    /// <param name="itemBounds">PopupItem's bounding area</param>
    /// <param name="bGrayed">Whether or not the corresponding item is grayed</param>
    private void DrawArrow( Graphics gfx, Rectangle itemBounds, bool bGrayed )
    {
      Rectangle bounds = new Rectangle();
      bounds.Location  = new Point( itemBounds.Right - m_arrowSize.Width - 2, itemBounds.Top + ( (itemBounds.Height - m_arrowSize.Height ) / 2 ) );
      bounds.Size = m_arrowSize ;

      using( SolidBrush brush = new SolidBrush( bGrayed ? Color.Gray : Color.Black ) )
      {
        PointF[ ] arrow = new PointF[ 3 ];
        arrow[ 0 ] = new Point( bounds.Left, bounds.Top );
        arrow[ 1 ] = new Point( bounds.Left, bounds.Bottom );
        arrow[ 2 ] = new Point( bounds.Right, bounds.Top + ( ( bounds.Height / 2 ) ) );
        gfx.FillPolygon( brush, arrow );
      }

    }
    
    
    #endregion

    #region layout

    /// <summary>
    /// Arranges all the menu items
    /// </summary>
    /// <returns>Required size of menu in pixels</returns>
    private Size LayoutMenuItems( Control parent )
    {
      // create array to store item positions
      m_bounds = new Rectangle[ m_items.Count ];

      // default column width
      m_columnWidth = m_minumumColumnWidth;
      int textWidth = 0;
      int acceleratorWidth = 0;
      int itemHeight = 0;

      bool containsSubMenus = false;
      
      // find the maximum width and height of the menu images
      foreach( PopupItem item in m_items )
      {
        // does the item contain an image?
        if( item.Image != null )
        {
          m_columnWidth = Math.Max( m_columnWidth, item.Image.Width + m_columnPadding );
          itemHeight = Math.Max( itemHeight, item.Image.Height  );
        }

        // does this item contain a sub menu
        containsSubMenus |= item.Items.Count > 0;

        // measure the item's text
        using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
        {
          SizeF textSize = gfx.MeasureString( item.Text, parent.Font );
          SizeF acceleratorSize = gfx.MeasureString( item.Accelerator, parent.Font );
          
          textWidth = Math.Max( textWidth, ( int ) textSize.Width + m_textPadding );
          acceleratorWidth = Math.Max( acceleratorWidth, ( int ) acceleratorSize.Width );
          itemHeight = Math.Max( itemHeight,  ( int ) textSize.Height ); 
        }
      }
      m_textWidth = textWidth + acceleratorWidth;

      // do we need to add in some extra space for the any sub menu arrows?
      if( containsSubMenus )
        m_textWidth += m_arrowPadding;

      // store the required size of the popup item
      Size popupItemSize = new Size
      ( 
        m_columnWidth + m_textWidth + m_itemWidthPadding,
        itemHeight + m_itemHeightPadding
      );

      // store the client position of the popup items
      int x = m_bevel;
      int y = m_bevel + (m_itemHeightPadding / 2);
      for( int index = 0; index < m_bounds.Length; ++index )
      {
        PopupItem item = m_items[ index ] as PopupItem;
        m_bounds[ index ] = new Rectangle
          (
          x, y,
          popupItemSize.Width - 1,
          item.Text == "-" ? m_separatorHeight : popupItemSize.Height
          );

        y += m_bounds[ index ].Height;
      }

      // store the text offset for the popup items
      m_textOffset = m_bevel + m_columnWidth + ( int )( ( float ) m_textPadding / 2 );
      
      // create the required size for the popup menu
      int requiredItemsHeight = y;
      Size popupSize = new Size( popupItemSize.Width + m_popupPadding, y + m_popupPadding );      

      // create the background hsape
      Rectangle bounds = new Rectangle( 0, 0, popupSize.Width - 1, popupSize.Height -1 );      
      m_backgroundShape = Shapes.Rectangle( bounds );      
      
      // create inner shape( white circle )
      bounds.Inflate( -m_bevel, -m_bevel );   
      m_innerShape = Shapes.BeveledEdge( bounds, Edges.All, m_bevel );
   
      // create the column shape
      bounds = new Rectangle( bounds.X, bounds.Y, m_columnWidth, bounds.Height );      
      m_columnShape = Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.BottomLeft, m_bevel );
      
      // return the required size of the popup menu
      return popupSize;
    } // LayoutMenuItems()
    
    
    #endregion

    #region mouse handlers

    /// <summary>
    /// Handles mouse down.
    /// </summary>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      // mouse sensitivity
      if( !m_processMouseInput )
        return;
    } // OnMouseDown()
    
    
    /// <summary>
    /// Handles mouse up.
    /// </summary>
    protected override void OnMouseUp( MouseEventArgs args )
    {

      // perform hit test
      int index = HitTest( new Point( args.X, args.Y ) );       
      if( index != -1 )
      {
        // see if an item was clicked
        PopupItem item = this.Items[ index ];

        if ( !item.Enabled )
        {
          // Do nothing if item is disabled
        }
        else if ( item.IsSubMenu )
        {
          // if it's a submenu fire the timer right away
          m_timer.Stop();
          m_timer.Interval = 10;
          m_timer.Start();
        }
        else if( m_processMouseInput )// A menu item was selected
        {
          // exit from the modal loop
          EndModal();

          // newly selected index
          m_selectedIndex = index;
          
        } // if( item.Items.Count == 0 )
      } // if( index != -1 )
    } // OnMouseUp()
    
       
    /// <summary>
    /// Handles mouse move.
    /// </summary>
    protected override void OnMouseMove( MouseEventArgs args )
    {
       // perform hit test
      int trackItem = HitTest( new Point( args.X, args.Y ) );       
      
      // has the track item changed?
      if( trackItem != m_trackItem )
      {
        // are we currently showing a sub menu?
        if( this.ChildPopup != null )
        {
          // if we aren't currently tracking a valid item
          // we don't want to dismiss the sub menu
          if( trackItem == -1 )
            return; // bail out

        } // if( m_childMenu != null )
        
        // store the new track
        m_trackItem = trackItem;

        // stop the timer
        m_timer.Stop();
        
        // get rid of any child menus
        if( this.ChildPopup != null )
        {
          this.ChildPopup.Dismiss();
          this.ChildPopup = null;
        } // if( m_childMenu != null )
  
        // are we tracking anything?
        if( m_trackItem != -1 )
        {
          // grab the associated item
          PopupItem item = ( PopupItem ) m_items[ m_trackItem ];
        
          // do we need to display a sub menu?
          if( item.IsSubMenu )
          {
            // fire the timer right away
            m_timer.Interval = m_timerInterval;
            m_timer.Start();
          } // if( item.IsSubMenu )
            //ShowSubMenu(); 
        } // if( m_trackItem != -1 )

        // force a redraw
        Refresh();
      } // if( trackItem != m_trackItem )
    } // OnMouseMove()  
    

    /// <summary>
    /// Checks if the mouse is over a menu item.
    /// </summary>
    /// <returns>Index of item, -1 if none.</returns>
    private int HitTest( Point position )
    {
      // see if an item was clicked
      for( int index = 0; index < m_items.Count; ++index )
      {
        // get the item's bounds
        Rectangle itemBounds = m_bounds[ index ];
        
        // expand the bounds for the entire popup menu width
        Rectangle bounds = new Rectangle( 0, itemBounds.Top, this.Size.Width, itemBounds.Height );
        
        // is the mouse contained within the item's bounds?
        if( bounds.Contains( position ) )
        {  
          PopupItem item = ( PopupItem ) m_items[ index ];
          
          // check if the item is a separator?
          if( item.IsSeparator )
            continue;
          
          return index;
        }
      } // for each item
      return -1;
    } // HitTest()

    
    #endregion   
    
  } // class PopupMenu

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: PopupMenu.cs,v $
// Revision 1.5  2006/11/16 20:45:26  gdavidson
// Added insert method
//
// Revision 1.4  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.3  2004/07/12 15:04:08  geconomos
// default color for menu item accelerator text to black
//
// Revision 1.2  2004/06/15 19:46:18  geconomos
// Fixed menu item text color to black.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.31.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.31  2003/11/26 02:27:13  frank
// Temporarily disabled the mouse timer feature.
//
// Revision 1.30  2003/11/25 12:18:34  geconomos
// Added timer to enable mouse processing when fired.
//
// Revision 1.29  2003/11/03 20:53:20  frank
// Added accelerators
//
// Revision 1.28  2003/09/25 19:23:21  jmeade
// Issue 3500:  Don't show a submenu if the parent is disabled.
//
// Revision 1.27  2003/09/24 19:58:40  mkontak
// Added TAG to popup item
//
// Revision 1.26  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.25  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.24  2003/05/15 17:47:26  geconomos
// Added Popup event to MainMenuItem.
//
// Revision 1.23  2003/05/14 21:19:21  jmeade
// Issue 3222:  Remove single ampersands.
//
// Revision 1.22  2003/05/09 00:32:20  jmeade
// Popup (menu first shown) event; Visible property.
//
// Revision 1.21  2003/05/08 20:00:13  jmeade
// Menu item disabling and graying.
//
// Revision 1.20  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.19  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.18  2003/04/24 14:31:00  frank
// code review
//
// Revision 1.17  2003/04/23 15:47:43  geconomos
// Added more commenting.
//
// Revision 1.16  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.15  2003/04/15 19:59:42  geconomos
// Fixed ugly bug with firing click event beore modal lopp finsihes.
//
// Revision 1.14  2003/04/14 15:53:16  geconomos
// Fixed bug when dismissing a menu after picking a menu item.
//
// Revision 1.13  2003/04/14 13:09:15  geconomos
// Broke popup implementation into PopupWindow class.
//
// Revision 1.12  2003/04/11 16:22:54  geconomos
// Implemented sub menus.
//
// Revision 1.11  2003/04/07 23:16:17  geconomos
// Issue #3048: Popup menu off screen.
//
// Revision 1.10  2003/04/07 18:07:04  geconomos
// Changed color White to RGB( 250, 250, 250 ).
//
// Revision 1.9  2003/03/10 16:13:29  geconomos
// Fixed size of checked menu item.
//
// Revision 1.8  2003/02/19 13:34:21  geconomos
// Changed selection color.
//
// Revision 1.7  2002/11/26 17:35:27  geconomos
// Added Data property to PopupItem. Also added some overrides on the ADD
// method of the corresponding collection class.
//
// Revision 1.6  2002/11/25 13:59:31  geconomos
// Initialized default colors to new values.
//
// Revision 1.5  2002/11/20 22:06:12  geconomos
// Removed check for ALT to dismiss popup menu.
//
// Revision 1.4  2002/11/19 20:36:56  geconomos
// Temporaily disable drawing shadow image for selected popup item.
//
// Revision 1.3  2002/11/18 21:14:06  geconomos
// corrected starting position of first popup item.
//
// Revision 1.2  2002/11/18 20:12:32  geconomos
// Headers and footers.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/PopupMenu.cs,v 1.5 2006/11/16 20:45:26 gdavidson Exp $
// $Id: PopupMenu.cs,v 1.5 2006/11/16 20:45:26 gdavidson Exp $

#endregion