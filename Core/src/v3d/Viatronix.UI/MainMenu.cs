// $Id: MainMenu.cs,v 1.2 2004/04/12 17:42:33 frank Exp $
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
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region MainMenuOrientation
  
  /// <summary>
  /// Specifies the orientation of the MainMenu object.
  /// </summary>
  public enum MainMenuOrientation
  {
    /// <summary>
    /// Menu items should be displayed horizontally within the menu.
    /// </summary>
    Horizontal,
    
    /// <summary>
    /// Menu items should be displayed vertically within the menu.
    /// </summary>
    Vertical
  }; // enum MainMenuOrientation

  #endregion
 
  #region MainMenuItem
  
  /// <summary>
  /// Represents a menu item in a MainMenu control.
  /// </summary>
  public sealed class MainMenuItem
  {
    #region events

    /// <summary>
    /// Raised when a menu item is clicked
    /// </summary>
    public event EventHandler Popup;

    #endregion

    #region methods

    /// <summary>
    /// Raises the Popup event.
    /// </summary>
    internal void RaisePopupEvent()
    {
      if( Popup != null )
        Popup( this, System.EventArgs.Empty );
    } // RaisePopupEvent()
    
    #endregion
    
    #region fields

    /// <summary>
    /// Item text.
    /// </summary>
    string m_text = string.Empty;

    /// <summary>
    /// Menu item's PopupItemCollection object.
    /// </summary>
    PopupItemCollection m_items = new PopupItemCollection();

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the item's text.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // text

    
    /// <summary>
    /// Gets the PopupItemCollection object.
    /// </summary>
    public PopupItemCollection Items
    {
      get { return m_items; }
    } // Items

    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes a ManiMenuItem object with default values.
    /// </summary>
    public MainMenuItem()
    {
    } // MainMenuItem()

    
    /// <summary>
    /// Initializes a MainMenuItem object with the specified text.
    /// </summary>
    /// <param name="text">Item text.</param>
    public MainMenuItem( string text )
    {
      m_text = text;
    } // MainMenuItem( text )

    
    #endregion
  
  }; // class MainMenuItem

  #endregion

  #region MainMenu
  
  public class MainMenu : System.Windows.Forms.Control
	{
    #region MainMenuItemCollection

    /// <summary>
    /// Represents a collection of MainMenuItem objects.
    /// </summary>
    public class MainMenuItemCollection : CollectionBase
    {
      #region fields

      /// <summary>
      /// Parent MainMenu object.
      /// </summary>
      private MainMenu m_parent;

      #endregion

      #region initialization

      /// <summary>
      /// Initializes the MainMenuItemCollectionObject with the specified MainMenu parent.
      /// </summary>
      /// <param name="parent">Parent MaainMenu object.</param>
      internal MainMenuItemCollection( MainMenu parent )
      {
        m_parent = parent;
      } // MainMenuItemCollection( parent )

    
      #endregion

      #region methods
 
      /// <summary>
      /// Adds a MenuItem object to the collection with the specified text.
      /// </summary>
      /// <param name="text">Text for menu item.</param>
      /// <returns>Array index of new item</returns>
      public int Add( string text )
      {
        return Add( new MainMenuItem( text ) );
      } // Add( text )
    
    
      /// <summary>
      /// Adds a the specified MainMenuItem object to the collection.
      /// </summary>
      /// <param name="item">MainMenuItem object to add.</param>
      /// <returns>Array index of new item</returns>
      public int Add( MainMenuItem item )
      {
        int index = List.Add( item );
        m_parent.LayoutMenuItems();
        return index;
      } // Add( item )
    
    
      /// <summary>
      /// Adds the specified array of MainMenuItem objects to the collection.
      /// </summary>
      /// <param name="items">Array of MainMenuItem objects to add to the collection.</param>
      public void AddRange( MainMenuItem[] items )
      {
        foreach( MainMenuItem item in items )
          Add( item );
        m_parent.LayoutMenuItems();
      } // AddRange( items )


      /// <summary>
      /// Inserts the MainMenuItem at the specified index.
      /// </summary>
      /// <param name="index">Index to insert item.</param>
      /// <param name="item">MainMenuItem object to insert.</param>
      public void Insert( int index, MainMenuItem item )
      {
        List.Insert( index, item );
        m_parent.LayoutMenuItems();
      } // Insert( index, item )

    
      /// <summary>
      /// Indexer to get or set a MainMenuItem object.
      /// </summary>
      public MainMenuItem this[ int index ] 
      {
        get { return ( MainMenuItem ) List[ index ]; }
        set 
        { 
          List[ index ] = value; 
          m_parent.LayoutMenuItems();
        } // set
      } // this[ index ] 

      /// <summary>
      /// Indexer using the menu item text
      /// </summary>
      public MainMenuItem this[ string text ]
      {
        get
        {
          foreach ( MainMenuItem item in List )
          {
            if ( item.Text == text ) return item;
          }
          return null;
        } // get
        set 
        {
          for ( int i=0; i<List.Count; i++ )
          {
            if ( ( List[i] as MainMenuItem).Text == text ) List[i] = value;
          }
          m_parent.LayoutMenuItems();
        } // set
      } // this[ text ] 
    
      #endregion

    } // class MainMenuItemCollection

  
  #endregion

    #region constants
    
    /// <summary>
    /// Bevel for rounded edges.
    /// </summary>
    static private readonly int m_bevel = 5;

    /// <summary>
    /// Vertical padding to add to first menu item position
    /// </summary>
    static private readonly int m_verticalPadding = 2 * m_bevel;
    
    /// <summary>
    /// Horizontal padding to add to first menu item position
    /// </summary>
    static private readonly int m_horizontalPadding = 2 * m_bevel;

    /// <summary>
    /// Vertical padding to add to menu item height.
    /// </summary>
    static private readonly int m_itemVerticalPadding = 4;
    
    /// <summary>
    /// Horizontal padding to add to menu item width.
    /// </summary>
    static private readonly int m_itemHorizontalPadding = 4;

    /// <summary>
    /// Vertical offset to add to menu item text position
    /// </summary>
    static private readonly int m_itemOffsetY = ( m_verticalPadding / 2 );
    
    /// <summary>
    /// Horizontal offset to add to menu item text position
    /// </summary>
    static private readonly int m_itemOffsetX = ( m_horizontalPadding / 2 );

    /// <summary>
    /// Selected fill color.
    /// </summary>
    static private readonly Color m_highlightColorDark = Color.FromArgb( 0, 0, 255 ) ;
    
    /// <summary>
    /// Selected outline color.
    /// </summary>
    static private readonly Color m_highlightColorLight = Color.FromArgb( 80, 0, 0, 255 );
    
    #endregion

    #region fields
    
    /// <summary>
    /// Menu item collection.
    /// </summary>
    private MainMenuItemCollection m_items;
    
    /// <summary>
    /// Menu orientation.
    /// </summary>
    private MainMenuOrientation m_orientation = MainMenuOrientation.Vertical;
    
    /// <summary>
    /// Points describing the shape of the frame.
    /// </summary>
    private PointF[] m_frameShape = new PointF[ 0 ];
    
    /// <summary>
    /// Points describing the shape of the inside of the menu.
    /// </summary>
    private PointF[] m_innerShape = new PointF[ 0 ];
    
    /// <summary>
    /// Bounding rectangles for menu items.
    /// </summary>
    private Rectangle[] m_itemBounds = new Rectangle[ 0 ];
    
    /// <summary>
    /// Item that is currently being tracked.
    /// </summary>
    private int m_trackItem = -1;
    
    /// <summary>
    /// Frame color.
    /// </summary>
    private Color m_frameColor = Color.FromArgb( 255, 91, 61, 204 );
    
    /// <summary>
    /// Outline color.
    /// </summary>
    private Color m_outlineColor = Color.FromArgb( 255, 51, 51, 102 );

    /// <summary>
    /// Fill color.
    /// </summary>
    private Color m_fillColor = Color.FromArgb( 240, 240, 240 );

    #endregion

    #region properties

    /// <summary>
    /// Gets the MainMenuItemCollection object.
    /// </summary>
    public MainMenuItemCollection MenuItems
    {
      get { return m_items; }
    } // MenuItems


    /// <summary>
    /// Gets or sets the menu's orientation.
    /// </summary>
    public MainMenuOrientation Orientation
    {
      get { return m_orientation; }
      set { m_orientation = value; LayoutMenuItems(); Invalidate(); }
    } // Orientation
    
    
    /// <summary>
    /// Gets or sets the frame color.
    /// </summary>
    public Color Color
    {
      get { return m_frameColor; }
      set { m_frameColor = value; Invalidate(); }
    } // Color
    
    
    /// <summary>
    /// Gets or sets the fill color.
    /// </summary>
    public Color FillColor
    {
      get { return m_fillColor; }
      set { m_fillColor = value; Invalidate(); }
    } // FillColor


    /// <summary>
    /// Horizontal padding to add to menu item width.
    /// </summary>
    static public int ItemHorizontalPadding
    {
      get { return m_itemHorizontalPadding; }
    } // ItemHorizontalPadding

    
    #endregion

    #region initialization
    
    /// <summary>
    /// Initializes a MainMenu object with default values.
    /// </summary>
    public MainMenu()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );

      // enable transparent back colors
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      
      // set the background as transparent
      this.BackColor = Color.Transparent;

      m_items = new MainMenuItemCollection( this );
		} // MainMenu()
	

    #endregion
    
    #region drawing
    
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // use high quailty settings
      Graphics gfx = args.Graphics;
      gfx.SmoothingMode = SmoothingMode.HighQuality;

      // fill the frame with the frame color
      using( SolidBrush brush = new SolidBrush( m_frameColor ) )
        gfx.FillPolygon( brush, m_frameShape );
      
      // fill the inside with the fill color
      using( SolidBrush brush = new SolidBrush( m_fillColor ) )
        gfx.FillPolygon( brush, m_innerShape );

      // outline both the frame and the inside
      using( Pen pen = new Pen( m_outlineColor ) )
      {
        gfx.DrawPolygon( pen, m_frameShape );
        gfx.DrawPolygon( pen, m_innerShape );
      } // using( Pen pen = new Pen( m_outlineColor ) )

      // set the format for the menu item's text
      StringFormat format = new StringFormat();
      format.Alignment = (m_orientation == MainMenuOrientation.Vertical)?  StringAlignment.Near : StringAlignment.Center;
      format.LineAlignment = StringAlignment.Center;

      // draw each menu item
      for( int index = 0; index < m_items.Count; ++index )
      {
        // get the menu item object
        MainMenuItem item = m_items[ index ];
        
        // get the menu items bounds
        Rectangle bounds = m_itemBounds[ index ];

        // is the item is selected?
        if( m_trackItem == index )
        {
          // fill the menu item with the selection color
          using( SolidBrush brush = new SolidBrush( m_highlightColorLight ) )
            gfx.FillRectangle( brush, bounds );
                
          // outline the menu item with the selection outline color
          using( Pen pen = new Pen( m_highlightColorDark ) )
            gfx.DrawRectangle( pen, bounds );
        
        } // if( m_trackItem == index )

        // dra the menu item's text
        using( SolidBrush brush = new SolidBrush( this.ForeColor ) )
          gfx.DrawString( item.Text, this.Font, brush, ( RectangleF ) bounds, format );        
      
      } // for( int index = 0; index < m_items.Count; ++index )
    } // OnPaint( args )

    
    #endregion    
 
    #region mouse overrides
    
    /// <summary>
    /// Called when the mouse is moved within the control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this override.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      base.OnMouseMove(args );
      
      // menu item that the mouse is over
      int trackItem = -1;
      
      // for each menu item
      for( int index = 0; index < m_items.Count; ++index )
      {
        // is the mouse over a menu item?
        if( m_itemBounds[ index ].Contains( args.X, args.Y ) )
        {
          // indicate that this item is to be tracked
          trackItem = index;
          break;
        } // if( m_itemBounds[ index ].Contains( args.X, args.Y ) )
      } // for( int index = 0; index < m_items.Count; ++index )
      
      // has the track item changed?
      if( trackItem != m_trackItem )
      {
        m_trackItem = trackItem;
        Invalidate();     
      } // if( trackItem != m_trackItem )
    
    } // OnMouseMove( args )
    

    /// <summary>
    /// Called when one of the mouse buttons are down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this override.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      // for each menu item
      for( int index = 0; index < m_items.Count; ++index )
      {
        // is the mouse over a menu item?
        if( m_itemBounds[ index ].Contains( args.X, args.Y ) )
        {
          // track this item
          m_trackItem = index;          
          
          // force the control to repaint
          Refresh();
          
          // get the associated MainMenuItem object
          MainMenuItem item = m_items[ index ];

          // raise the popup event
          item.RaisePopupEvent();

          // get bounds for the menu control in screen coordinates
          Rectangle mainMenuBounds = this.RectangleToScreen( this.ClientRectangle );
          
          // get the menu item bounds in screen coordinates
          Rectangle itemBounds = this.RectangleToScreen( m_itemBounds[ index ] );
          
          // create a new PopupMenu object and add the popup items
          PopupMenu popup = new PopupMenu();
          foreach( PopupItem popupItem in item.Items )
            popup.Items.Add( popupItem );
          
          // show the popup menu based on the orientation of the MainMenuObject
          if( m_orientation == MainMenuOrientation.Vertical )
            popup.Show( this, new Point( mainMenuBounds.Right, itemBounds.Top ) ); 
          else
            popup.Show( this, new Point( itemBounds.Left, itemBounds.Bottom ) ); 
          
          // no longer being tracked
          m_trackItem = -1;
          
          // force a redraw
          Invalidate();    
          
          // done
          break;
         } // if( m_itemBounds[ index ].Contains( args.X, args.Y ) )
      } // for( int index = 0; index < m_items.Count; ++index )
      
    } // OnMouseDown( args ) 
    

    /// <summary>
    /// Called when the mouse leaves the control.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnMouseLeave( EventArgs args )
    {
      // no longer begin tracked
      m_trackItem = -1;
      
      // update control
      Refresh();
    } // OnMouseLeave( args )
    

    #endregion

    #region menu item layout

    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      // layout the menu items
      LayoutMenuItems();

      // dirty the control
      Invalidate();
    } // OnResize( args )

 

    /// <summary>
    /// Decides if the menu items should be laid out vertically or horizontally.
    /// </summary>
    internal void LayoutMenuItems()
    {
      if( m_orientation == MainMenuOrientation.Vertical )
        LayoutItemsVertically();
      else
        LayoutItemsHorizontally();
    } // LayoutMenuItems()


    /// <summary>
    /// Lays out the menu items vertically.
    /// </summary>
    private void LayoutItemsVertically()
    {
      Rectangle bounds = new Rectangle( 0, 0, this.Width - 1, this.Height - 1 );
      
      // define the points for the frame
      m_frameShape = Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.BottomLeft, m_bevel );
      
      // define the points for the inside of the control
      bounds.Inflate( -2, -2 );      
      m_innerShape = Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.BottomLeft, m_bevel );

      // allocate an array of Rectangles for the menu item's bounds
      m_itemBounds = new Rectangle[ m_items.Count ];
      
      // height required for a single menu item
      int requiredItemHeight = ( this.Font.Height + m_itemVerticalPadding );
      
      // height required for ALL the menu items
      int requiredItemsHeight = requiredItemHeight * m_items.Count;

      // calculate the spacing in between the menu items
      int itemSpacing = ( this.Height - m_verticalPadding - requiredItemsHeight )  / ( m_items.Count + 1 );

      // figure out the each items bounds
      for( int index = 0; index < m_items.Count; ++index )
      {
        // set the item's bounds
        m_itemBounds[ index ] = new Rectangle
        (
          ( m_horizontalPadding / 2 ),
          m_itemOffsetY + ( requiredItemHeight * index ) + ( itemSpacing * ( index + 1 ) ),
          this.Width - m_horizontalPadding,
          requiredItemHeight
        );
      } // for( int index = 0; index < m_items.Count; ++index )
    } // LayoutItemsVertically()   
 
    
    /// <summary>
    /// Lays out the menu items horizontally.
    /// </summary>
    private void LayoutItemsHorizontally()
    {
      Rectangle bounds = new Rectangle( 0, 0, this.Width - 1, this.Height - 1 );
      
      // define the points for the frame
      m_frameShape = Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.TopRight, m_bevel );
      
      // define the points for the inside of the control
      bounds.Inflate( -3, -3 );
      m_innerShape = Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.TopRight, m_bevel );

      // allocate an array of Rectangles for the menu item's bounds
      m_itemBounds = new Rectangle[ m_items.Count ];
      
      // height required for a single menu item
      int requiredItemHeight = ( this.Font.Height + m_itemVerticalPadding );
      
      // figure out the width of each menu item
      int requiredItemsWidth = 0;
      using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      {
        // for each menu item
        for( int index = 0; index < m_items.Count; ++index )
        {
          // get the menu item
          MainMenuItem item = m_items[ index ];
          
          // get the size of the menu item text in pixels
          SizeF size = gfx.MeasureString( item.Text, this.Font );
          
          // get the required with for the item adding in any extra padding
          int requiredItemWidth = (int) size.Width + m_itemHorizontalPadding;
          
          // for now just set the size of the menu item
          m_itemBounds[ index ]  = new Rectangle( 0, 0, requiredItemWidth, requiredItemHeight );

          // width required for all the menu items
          requiredItemsWidth += requiredItemWidth;          
        
        } // for( int index = 0; index < m_items.Count; ++index )
      }// using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )

      // calculate the spacing in between the menu items
      int spacing = ( this.Width - requiredItemsWidth ) / ( m_items.Count + 1 );
      
      // center the menu item vertically with the menu
      int offsetY = ( this.Height - requiredItemHeight ) / 2;

      // center the menu items horizontally within the control
      int offsetX =  ( ( this.Width - requiredItemsWidth - ( spacing *( m_items.Count + 1 ) ) ) / 2 ) + spacing;

      // set the position of each menu item
      for( int index = 0; index < m_items.Count; ++index )
      {
        m_itemBounds[ index ].Offset( offsetX, offsetY );
        offsetX = m_itemBounds[ index ].Right + spacing;        
      } // for( int index = 0; index < m_items.Count; ++index )
    } // LayoutItemsHorizontally()
    
    
    #endregion    
	} // class MainMenu

#endregion

} // Viatronix.UI

#region revision history

// $Log: MainMenu.cs,v $
// Revision 1.2  2004/04/12 17:42:33  frank
// extended main menu class to include indexer based on menu item text
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.15.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.15  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.14  2003/05/30 16:43:00  geconomos
// Added Insert method to collection class.
//
// Revision 1.13  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.12  2003/05/15 17:47:38  geconomos
// Added Popup event to MainMenuItem.
//
// Revision 1.11  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.10  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.9  2003/04/24 14:27:36  frank
// code review
//
// Revision 1.8  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.7  2003/04/22 14:48:33  geconomos
// More coding standards.
//
// Revision 1.6  2003/04/07 18:07:04  geconomos
// Changed color White to RGB( 250, 250, 250 ).
//
// Revision 1.5  2003/02/19 13:34:31  geconomos
// Changed selection color.
//
// Revision 1.4  2002/12/12 15:39:19  geconomos
// Code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/MainMenu.cs,v 1.2 2004/04/12 17:42:33 frank Exp $
// $Id: MainMenu.cs,v 1.2 2004/04/12 17:42:33 frank Exp $

#endregion
