// $Id: ListBox.cs,v 1.4 2004/10/18 14:20:42 geconomos Exp $
//
// Copyright (c) 2003, Viatronix Inc., All Rights Reserved.
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
using System.Drawing.Text;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region ListBoxItem

  /// <summary>
  /// Represents a collection of ListBoxItem objects.
  /// </summary>
  public sealed class ListBoxItem
  {
    #region fields

    /// <summary>
    /// Item text.
    /// </summary>
    private string m_text;

    /// <summary>
    /// Item Font.
    /// </summary>
    private Font m_font;

    /// <summary>
    /// Item Fore Color.
    /// </summary>
    private Color m_foreColor;

    /// <summary>
    /// Item Back Color.
    /// </summary>
    private Color m_backColor;

    /// <summary>
    /// Item's bounds within parent list box.
    /// </summary>
    private RectangleF m_bounds;

    /// <summary>
    /// Associated data.
    /// </summary>
    private object m_data = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the text.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text

    
    /// <summary>
    /// Gets or sets the font.
    /// </summary>    
    public Font Font
    {
      get { return m_font; }
      set { m_font = value; }
    } // Font


    /// <summary>
    /// Gets or sets the fore color.
    /// </summary>
    public Color ForeColor
    {
      get { return m_foreColor; }
      set { m_foreColor = value; }
    } // ForeColor

    
    /// <summary>
    /// Gets or sets the back color.
    /// </summary>
    public Color BackColor
    {
      get { return m_backColor; }
      set { m_backColor = value; }
    } // BackColor

    
    /// <summary>
    /// Gets or sets the bounds.
    /// </summary>
    public RectangleF Bounds
    {
      get { return m_bounds; }
      set { m_bounds = value; }
    } // Bounds

   
    /// <summary>
    /// Gets or sets the associated data.
    /// </summary>
    public object Data
    {
      get { return m_data; }
      set { m_data = value; }
    } // Data


    #endregion
  
  } // class ListBoxItem
  
  
  #endregion

  #region ListBox
	
  /// <summary>
  /// Represents a Windows list box control.
  /// </summary>
  public class ListBox : Viatronix.UI.ScrollableControl
	{
    #region ListBoxCollection
    
    /// <summary>
    /// Collection class for ListBox object.
    /// </summary>
    public class ListBoxCollection : CollectionBase
    {
      #region fields

      /// <summary>
      /// Parent list box object.
      /// </summary>
      Viatronix.UI.ListBox m_listbox = null;

      #endregion
      
      #region initialization

      /// <summary>
      /// Initializes the ListBoxCollection object with the specified parent ListBox object.
      /// </summary>
      /// <param name="listbox">Parent list box object.</param>
      internal ListBoxCollection( Viatronix.UI.ListBox listbox )
      {
        m_listbox = listbox;
      } // ListBoxCollection( ListBox listbox )
      
      
      #endregion
      
      #region methods

      /// <summary>
      /// Adds an item to the collection with the specified text.
      /// </summary>
      /// <param name="text">Item text.</param>
      /// <returns>Item's array index.</returns>
      public int Add( string text )
      {
        // create new list box item
        ListBoxItem item = new ListBoxItem();
        
        // fill properties with parent's as default
        item.Text       = text;
        item.Font       = m_listbox.Font;
        item.ForeColor  = m_listbox.ForeColor;
        item.BackColor  = m_listbox.FillColor;

        return Add( item );
      } // Add( text )     
      

      /// <summary>
      /// Adds an item to the collection with the specified text and data.
      /// </summary>
      /// <param name="text">Item text.</param>
      /// <param name="data">Item data object.</param>
      /// <returns>Items array index.</returns>
      public int Add( string text, object data )
      {
        // create new list box item
        ListBoxItem item = new ListBoxItem();
        
        // fill properties with parent's as default
        item.Text       = text;
        item.Font       = m_listbox.Font;
        item.ForeColor  = m_listbox.ForeColor;
        item.BackColor  = m_listbox.FillColor;
        item.Data       = data;

        return Add( item );
      } // Add( text, data )     
      
      
      /// <summary>
      /// Adds an item to the collection with the specified text and font.
      /// </summary>
      /// <param name="text">Item text.</param>
      /// <param name="data">Font used to render text.</param>
      /// <returns>Items array index.</returns>
      public int Add( string text, Font font )
      {
        // create new list box item
        ListBoxItem item = new ListBoxItem();
        
        // fill properties with parent's as default
        item.Text       = text;
        item.Font       = font;
        item.ForeColor  = m_listbox.ForeColor;
        item.BackColor  = m_listbox.FillColor;

        return Add( item );
      } // Add( text, font )         
      
      
      /// <summary>
      /// Adds an item to the collection with the specified text and font.
      /// </summary>
      /// <param name="text">Item text.</param>
      /// <param name="data">Font used to render text.</param>
      /// <param name="foreColor">Item fore color.</param>
      /// <param name="backColor">Item back color.</param>
      /// <returns>Items array index.</returns>
      public int Add( string text, Font font, Color foreColor, Color backColor )
      {
        // create new list box item
        ListBoxItem item = new ListBoxItem();
        
        // fill properties with parent's as default
        item.Text       = text;
        item.Font       = font;
        item.ForeColor  = foreColor;
        item.BackColor  = backColor;

        return Add( item );
      } // Add( text, font, foreColor, backColor )    
      
      
      /// <summary>
      /// Adds an item to the collection with the specified text and font.
      /// </summary>
      /// <param name="text">Item text.</param>
      /// <param name="data">Font used to render text.</param>
      /// <param name="foreColor">Item fore color.</param>
      /// <param name="backColor">Item back color.</param>
      /// <param name="data">Item data.</param>
      /// <returns>Items array index.</returns>
      public int Add( string text, Font font, Color foreColor, Color backColor, object data )
      {
        // create new list box item
        ListBoxItem item = new ListBoxItem();
        
        // fill properties with parent's as default
        item.Text       = text;
        item.Font       = font;
        item.ForeColor  = foreColor;
        item.BackColor  = backColor;
        item.Data       = data;

        return Add( item );
      } // Add( text, font, foreColor, backColor, data )         
      
      
      /// <summary>
      /// Adds the specified ListBoxItem object to the collection.
      /// </summary>
      /// <param name="item">ListBoxItem oject to add to the collection.</param>
      /// <returns>The resulting index into the list</returns>
      public int Add( ListBoxItem item )
      {
        // add the item to the internal list
        int index = List.Add( item );
       
        // layout the thumbnails
        m_listbox.LayoutItems();

        // return the new index
        return index;
      } // Add( item )

      
      /// <summary>
      /// Inserts the ListBoxItem object into the collection at the specified index.
      /// </summary>
      /// <param name="index">Index position to insert ListBox object.</param>
      /// <param name="item">ListBoxItem object to insert.</param>
      public void Insert( int index, ListBoxItem item )
      {
        // insert the item to the internal list
        List.Insert( index, item );
        
        // layout the thumbnails
        m_listbox.LayoutItems();
      } // Insert( index, item )
      
      
      /// <summary>
      /// Removes the ListBoxItem object at the specified index position.
      /// </summary>
      /// <param name="index">Index position to remove ListboxItem object.</param>
      public new void RemoveAt( int index )
      {
        // remove from the internal list
        List.RemoveAt( index );

        // update the selected item
        if( m_listbox.SelectedIndex >= m_listbox.Items.Count )
          --m_listbox.SelectedIndex;

        // layout the thumbnails
        m_listbox.LayoutItems();
      } // RemoveAt( index )

      
      /// <summary>
      /// Removed the specified ListBoxItem object from the collection.
      /// </summary>
      /// <param name="item">ListBoxItem object to remove.</param>
      public void Remove( ListBoxItem item )
      {
        RemoveAt( List.IndexOf( item ) );
      } // Remove( item )

      
      /// <summary>
      /// Returns the array index of the specified ListboxItem object.
      /// </summary>
      /// <param name="item">ListboxItem object to return index for.</param>
      /// <returns>Array index of ListBoxItem object.</returns>
      public int IndexOf( ListBoxItem item )
      {
        return List.IndexOf( item ); 
      } // IndexOf( item )      
            

      /// <summary>
      /// Indexer to get or set an ListBoxItem object.
      /// </summary>
      public ListBoxItem this[ int index ] 
      {
        get { return ( ListBoxItem ) List[ index ]; }
        set { List[ index ] = value; }
      } // this[ index ] 
      

      #endregion
    
    } // class ListBoxCollection
    
    #endregion

    #region events

    /// <summary>
    /// Raised when the selection in the list box changes.
    /// </summary>
    public event EventHandler SelectionChanged;

    #endregion

    #region fields

    /// <summary>
    /// ListBoxItem collection class.
    /// </summary>
    private ListBoxCollection m_items = null;
    
    /// <summary>
    /// Currently selected ListBoxItem object.
    /// </summary>
    private int m_selectedIndex = -1;

    /// <summary>
    /// Vertical spacing( in pixels ) between items.
    /// </summary>
    private int m_spacing = 2;

    /// <summary>
    /// Indicates if single-selection is supported.
    /// </summary>
    private bool m_allowSelection = true;

    #endregion

    #region properties

    /// <summary>
    /// Gets the ListBoxItemCollection object.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public ListBoxCollection Items
    {
      get { return m_items; }
    } // Items
    
    
    /// <summary>
    /// Gets or sets the selected ListBoxItem object by index.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public int SelectedIndex
    {
      get { return m_selectedIndex; }
      set { m_selectedIndex = value; Invalidate(); }
    } // SelectedIndex


    /// <summary>
    /// Gets or sets the selected ListBoxItem object.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public ListBoxItem SelectedItem
    {
      get 
      { 
        ListBoxItem item = null;

        if( m_selectedIndex != - 1 )
          item = m_items[ m_selectedIndex ];
        
        return item;
      }
      
      set 
      { 
        this.SelectedIndex = m_items.IndexOf( value ); 
      }
    } // SelectedItem

    
    /// <summary>
    /// Gets or sets the vertical spacing between items ( in pixels ).
    /// </summary>
    public int Spacing
    {
      get { return m_spacing; }
      set { m_spacing = value; }
    } // Spacing


    /// <summary>
    /// Gets or sets if single-selection should be supported.
    /// </summary>
    public bool AllowSelection
    {
      get { return m_allowSelection; }
      set { m_allowSelection = value; Invalidate(); }
    } // AllowSelection
    
    
    #endregion

    #region methods

    /// <summary>
    /// Ensures the ListboxItem object at the specified index is entirely visible.
    /// </summary>
    /// <param name="index">Array index of LitBoxItem object that should be entirely visible.</param>
    public void EnsureVisible( int index )
    {
      // get the thumbnail item
      ListBoxItem item = m_items[ index ];

      Rectangle bounds = new Rectangle( ( int ) Math.Round( item.Bounds.X ), ( int ) Math.Round( item.Bounds.Y ), ( int ) Math.Round( item.Bounds.Width ), ( int ) Math.Round( item.Bounds.Height ) );

      // only do this if the thumbnail is not entriely visible
      if( !this.ViewRectangle.Contains( bounds ) )
      {
        // figure out the vertical scroll amount needed to make the item visible
        int dy = 0;
        if( item.Bounds.Top > this.ViewRectangle.Top )         
          dy = bounds.Bottom - this.ViewRectangle.Bottom + m_spacing;
        else
          dy = bounds.Top - this.ViewRectangle.Top - m_spacing;          

        // scroll the view rectangle 
        ScrollControl( 0, dy );

      } // this.ViewRectangle.Contains( item.Bounds )
    } // EnsureVisible( index )


    #endregion

    #region initialization

    /// <summary>
    /// Initializes a ListBox object with default values.
    /// </summary>
    public ListBox()
		{
      m_items = new ListBoxCollection( this );
		} // ListBox()


    #endregion        
    
    #region layout
    
    /// <summary>
    /// Calculates the client positions for all the ListBoxItem objects.
    /// </summary>
    internal void LayoutItems()
    {
      // initial y position
      float y = 5;
      
      // layout the items
      for( int index = 0; index < m_items.Count; ++index )
      { 
        // grab the list box item of interest
        ListBoxItem item = m_items[ index ];

        // measure the text string
        SizeF textExtents;
        using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
          textExtents = gfx.MeasureString( item.Text, item.Font );

        // format the items bounding box
        item.Bounds = new RectangleF( 5, y, textExtents.Width, textExtents.Height );

        // increment the y position
        y += m_spacing + textExtents.Height;       
      }

      SetVirtualSize( this.Width - this.VScrollWidth, ( int )( y + 0.5F ) );

      Invalidate();
    } // void LayoutItems()

    
    #endregion

    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // let the base class do it's thing
      base.OnPaint( args );

      // cache the graphics object
      Graphics gfx = args.Graphics;

      // high quality text output
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      // for each thumbnail
      for( int index = 0; index < m_items.Count; ++ index )
      {
        ListBoxItem item = m_items[ index ];
        
        // get the thumbnail's bounds and image
        RectangleF bounds = item.Bounds;
        
        // is the thumbnail visible?
        if( bounds.IntersectsWith( this.ViewRectangle ) )
        {
          // translate thumbnail bounds
          bounds.Offset( ( float ) -this.ViewRectangle.Left, ( float ) -this.ViewRectangle.Top );

          // draw the item text
          using( SolidBrush brush = new SolidBrush( item.ForeColor ) )
            gfx.DrawString( item.Text, item.Font, brush, bounds );

          // are we drawing selected items
          if( m_allowSelection )
          {
            Rectangle borderBounds = new Rectangle( ( int ) bounds.X, ( int ) bounds.Y, ( int ) bounds.Width, ( int )bounds.Height );
            borderBounds.Inflate( 1, 1 );
  
            // is the item selected?
            if( index == m_selectedIndex )
            {
              using( SolidBrush brush = new SolidBrush( Color.FromArgb( 60, Theme.SelectedBrush.Color ) ) )
                gfx.FillRectangle( brush, borderBounds );
              gfx.DrawRectangle( Theme.SelectedPen, borderBounds.X, borderBounds.Y, borderBounds.Width, borderBounds.Height );
            } // if( index == m_selectedIndex )
          } // if( m_allowSelection )
        } // if( bounds.IntersectsWith( this.ViewRectangle ) )
      } // for( int index = 0; index < m_items.Count; ++ index )
    } // OnPaint( args )

    
    #endregion

    #region mouse

    /// <summary>
    /// Called when the mouse button is down.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      // grab the mouse position
      Point pos = new Point( args.X, args.Y );

      // translate the position to view coordinates
      pos.Offset( this.ViewRectangle.X, this.ViewRectangle.Y );

      // for each thumbnail
      for( int index = 0; index < m_items.Count; ++index )
      {
        ListBoxItem item = m_items[ index ];        
        
        // did the mouse click a thumbnail?
        if( item.Bounds.Contains( pos ) )
        {
          // select and ensure visible this item if it isn't already
          if( index != this.SelectedIndex)
          {
            EnsureVisible( index );
            this.SelectedIndex = index;
            
            // raise the SelectionChanged event
            if( SelectionChanged != null && m_allowSelection )
              SelectionChanged( this, EventArgs.Empty );
          } // if( index != this.SelectedIndex)

          // outta here
          break;
        } // if( item.Bounds.Contains( pos ) )
      } // for( int index = 0; index < m_items.Count; ++index )
    } // OnMouseDown( args )

    
    #endregion
  
  } // ListBox

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: ListBox.cs,v $
// Revision 1.4  2004/10/18 14:20:42  geconomos
// fixed the rendering of listbox text
//
// Revision 1.3  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.2  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.8  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.7.2.1  2003/08/17 23:49:37  geconomos
// Updated to handle the mouse wheel event.
//
// Revision 1.7  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.6  2003/04/28 18:28:25  frank
// code review
//
// Revision 1.5  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.4  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.3  2003/04/22 14:48:33  geconomos
// More coding standards.
//
// Revision 1.2  2003/03/31 19:18:09  geconomos
// Made "selected item" properties not serializable in the designer.
//
// Revision 1.1  2003/03/31 19:12:36  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ListBox.cs,v 1.4 2004/10/18 14:20:42 geconomos Exp $
// $Id: ListBox.cs,v 1.4 2004/10/18 14:20:42 geconomos Exp $

#endregion
