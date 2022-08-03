// $Id: ThumbnailViewer.cs,v 1.6 2005/01/17 20:52:21 gdavidson Exp $
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
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region ThumbnailItem

  /// <summary>
  /// Represents a data item in a Thumbnail viewer control.
  /// </summary>
  public class ThumbnailItem
  {
    #region fields

    /// <summary>
    /// Original image.
    /// </summary>
    Image m_image;
    
    /// <summary>
    /// Thumbnail image.
    /// </summary>
    Image m_thumbnail;

    /// <summary>
    /// Thumbnail's bounds within ThumbnailViewer control.
    /// </summary>
    Rectangle m_bounds;

    #endregion

    #region properties

    /// <summary>
    /// Gets or set the image.
    /// </summary>
    public Image Image
    {
      get { return m_image; }
      set { m_image = value; }
    } // Image
    
    
    /// <summary>
    /// Gets or sets the thumbnail version of the image.
    /// </summary>
    public Image Thumbnail
    {
      get { return m_thumbnail; }
      set { m_thumbnail = value; }
    } // Thumbnail

    
    /// <summary>
    /// Gets or sets the thumbnail's bounds within the ThumbnailViewer control.
    /// </summary>
    public Rectangle Bounds
    {
      get { return m_bounds; }
      set { m_bounds = value; }
    } // Bounds
    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes a Thumbnail object with the specified image and thumbnail image.
    /// </summary>
    /// <param name="image">Image object.</param>
    /// <param name="thumbnail">Thumbnail version of image.</param>
    public ThumbnailItem( Image image, Image thumbnail )
    {
      m_image = image;
      m_thumbnail = thumbnail;
      m_bounds = new Rectangle();
    } // ThumbnailItem( image, thumbnail )
    
    #endregion

  } // class ThumbnailItem
  
  #endregion
	
  #region ThumbnailViewer

  /// <summary>
  /// Represents a control that displays thumbnail images in a rectangular grid.
  /// </summary>
  public class ThumbnailViewer : Viatronix.UI.ScrollableControl
	{
    #region ThumbnailCollection
    
    /// <summary>
    /// Represents a collection of ThumbnailItem objects.
    /// </summary>
    public class ThumbnailCollection : CollectionBase
    {
      #region fields

      /// <summary>
      /// Minimum size for an image.
      /// </summary>
      private const int m_minimumSize = 1;

      /// <summary>
      /// Parent thumbnail viewer. 
      /// </summary>
      ThumbnailViewer m_viewer = null;

      #endregion
      
      #region initialization

      /// <summary>
      /// Initializes a ThumbnailCollection object with the specified ThumbnailViewer object.
      /// </summary>
      /// <param name="viewer">Parent ThumbnailViewer object.</param>
      internal ThumbnailCollection( ThumbnailViewer viewer )
      {
        m_viewer = viewer;
      } // ThumbnailCollection( viewer )
      
      #endregion
      
      #region methods

      /// <summary>
      /// Adds a ThumbnailItem object to the collection with the specified image.
      /// </summary>
      /// <param name="image">Image object. A thumbnail image will be created for the image.</param>
      /// <returns>Array index of ThumbnailItem object.</returns>
      public int Add( Image image )
      {
        return Add( new ThumbnailItem( image, CreateThumbnailImage( image ) ) );
      } // Add( image )
      
      
      /// <summary>
      /// Adds the specified ThumbnailItem object to the collection.
      /// </summary>
      /// <param name="image">ThumbnailItem object to add.</param>
      /// <returns>Array index of ThumbnailItem object.</returns>
      public int Add( ThumbnailItem item )
      {
        // add the item to the internal list
        int index = List.Add( item );
       
        // layout the thumbnails
        m_viewer.LayoutThumbnails();

        // return the new index
        return index;
      } // Add( item )
      
      /// <summary>
      /// Sets the ThumbnailItem object with the specified image.
      /// </summary>
      /// <param name="index">Array index of ThumbnailItem object to set.</param>
      /// <param name="image">Image object. A thumbnail will be created for the image.</param>
      public void Set( int index, Image image )
      {
        // update the thumbnail image
        this[ index ].Thumbnail = CreateThumbnailImage( image );

        // invalidate view
        m_viewer.Invalidate();
      } // Set( index, image )

      
      /// <summary>
      /// Inserts the specified image into the collection at the specified index.
      /// </summary>
      /// <param name="index">Index to insert ThumbnailItem object.</param>
      /// <param name="image">Image object. A thumbnail will be created for the image.</param>
      public void Insert( int index, Image image )
      {
        Insert( index, new ThumbnailItem( image, CreateThumbnailImage( image ) ) );
      } // Insert( index, image )

      
      /// <summary>
      /// Inserts a ThumbnailItem object into the collection at the specified index.
      /// </summary>
      /// <param name="index">Index to insert ThumbnailItem object.</param>
      /// <param name="item">ThumbnailItem object to insert.</param>
      public void Insert( int index, ThumbnailItem item )
      {
        // insert the item to the internal list
        List.Insert( index, item );
        
        // layout the thumbnails
        m_viewer.LayoutThumbnails();
      } // Insert( index, item )
      
      
      /// <summary>
      /// Removes the ThumbnailItem object at the specified index.
      /// </summary>
      /// <param name="index">Index of ThumbnailItem object to remove.</param>
      public new void RemoveAt( int index )
      {
        // remove from the internal list
        List.RemoveAt( index );

        // update the selected item
        if( m_viewer.SelectedItem >= m_viewer.Thumbnails.Count )
          --m_viewer.SelectedItem;

        // layout the thumbnails
        m_viewer.LayoutThumbnails();
      } // RemoveAt( index )

      
      /// <summary>
      /// Removes the specified ThumbnailItem object from the collection.
      /// </summary>
      /// <param name="item">ThumbnailItem object to be removed.</param>
      public void Remove( ThumbnailItem item )
      {
        RemoveAt( List.IndexOf( item ) );
      } // Remove( item )
            

      /// <summary>
      /// Get or sets the specified ThumbnailItem.
      /// </summary>
      public ThumbnailItem this[ int index ] 
      {
        get { return ( ThumbnailItem ) List[ index ]; }
        set { List[ index ] = value; }
      } // this[ int index ] 
      

      /// <summary>
      /// Creates a thumbnail image from the specified source image. The aspect ratio of the image is preserved.
      /// </summary>
      /// <param name="source">Image object.</param>
      /// <returns></returns>
      private Image CreateThumbnailImage( Image source )
      {
        // size of source image
        SizeF size = new SizeF( source.Width  / source.HorizontalResolution, source.Height / source.VerticalResolution );

        // scale and keep aspect ratio of source image
        float scale = Math.Min( m_viewer.ThumbnailSize / size.Width, m_viewer.ThumbnailSize / size.Height );
        size.Width *= scale;
        size.Height *= scale;

        // create the thumbnail image
        int imageWidth = Math.Max( m_minimumSize, ( int )( size.Width + 0.5F ));
        int imageHeight = Math.Max( m_minimumSize, ( int )( size.Height + 0.5F ));
        Bitmap destination = new Bitmap( imageWidth, imageHeight, PixelFormat.Format24bppRgb );

        // create a graphics object from the new thumbnail image
        using( Graphics gfx = Graphics.FromImage( destination ) )
        {
          // highest quality interpolation method
          gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;

          // create the source and destination bounds
          Rectangle sourceBounds = new Rectangle( 0, 0, source.Width, source.Height );
          Rectangle destinationBounds = new Rectangle( 0, 0, destination.Width, destination.Height );

          // scale the image into the destination
          gfx.DrawImage( source, destinationBounds, sourceBounds, GraphicsUnit.Pixel );
        }

         // return thumbnail image
        return destination;
      } // CreateThumbnailImage( source )
       

      /// <summary>
      /// Clears out the collection
      /// </summary>
      public new void Clear()
      {
        List.Clear();

        // layout the thumbnails
        m_viewer.SelectedItem = -1;
        m_viewer.LayoutThumbnails();
      } // Clear()
      
     
      #endregion
    } // class ThumbnailCollection

    #endregion

    #region events

    /// <summary>
    /// Raised when a drag and drop operation starts.
    /// </summary>
    public event ItemDragEventHandler ItemDrag;

    /// <summary>
    /// Raised when the selected thumbnail changes.
    /// </summary>
    public event EventHandler SelectionChanged;

    #endregion
    
    #region fields

    /// <summary>
    /// Thumbnail collection.
    /// </summary>
    private ThumbnailCollection m_thumbnails = null;

    /// <summary>
    /// Number of thumbnails to display horizontally.
    /// </summary>
    private int m_numberOfColumns = 1;

    /// <summary>
    /// Spacing between thumbnails horizontally and vertically.
    /// </summary>
    private int m_spacing = 5;

    /// <summary>
    /// Thumbnail size.
    /// </summary>
    private int m_size = 0;

    /// <summary>
    /// Current selection.
    /// </summary>
    private int m_selectedItem = -1;

    /// <summary>
    /// Last mouse position.
    /// </summary>
    private Point m_dragPoint = new Point();

    /// <summary>
    /// Indicates if an outline should be drawn around the thumbnail.
    /// </summary>
    private bool m_drawOutline = false;

    /// <summary>
    /// Image border color.
    /// </summary>
    private Color m_outlineColor = Color.White;

    /// <summary>
    /// Indicates if thumbnails can be selected.
    /// </summary>
    private bool m_allowSelection = true;

    /// <summary>
    /// Determines the selection mode
    /// </summary>
    private bool m_multiSelection = true;

    private ArrayList m_selectedIndices=null;

    #endregion

    #region properties

    /// <summary>
    /// Gets the ThumbnailCollection object.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public ThumbnailCollection Thumbnails
    {
      get { return m_thumbnails; }
    } // Thumbnails

    
    /// <summary>
    /// Gets or sets the number of thumbnails to display horizontally.
    /// </summary>
    public int NumberOfColumns
    {
      get { return m_numberOfColumns; }
      set { m_numberOfColumns = value; }
    } // NumberOfColumns

    
    /// <summary>
    /// Gets or sets the spacing between thumbnails.
    /// </summary>
    public int Spacing 
    {
      get { return m_spacing; }
      set { m_spacing = value; }
    } // Spacing

   
    /// <summary>
    /// Gets or sets the index of the selected thumbnail.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public int SelectedItem
    {
      get { return m_selectedItem; }
      set 
      { 
        m_selectedItem = value; 
        m_selectedIndices.Clear(); 
        m_selectedIndices.Add( value );
        Invalidate(); 
      }
    } // SelectedItem


    /// <summary>
    /// Gets or sets if an outline is to be drawn around the thumbnail.
    /// </summary>
    public bool DrawOutline
    {
      get { return m_drawOutline; }
      set { m_drawOutline = value; Invalidate(); }
    } // DrawOutline

    
    /// <summary>
    /// Gets or sets the outline color.
    /// </summary>
    public Color OutlineColor
    {
      get { return m_outlineColor; }
      set { m_outlineColor = value; Invalidate(); }
    } // OutlineColor

    
    /// <summary>
    /// Gets or sets if the thumbnails are selectable.
    /// </summary>
    public bool AllowSelection
    {
      get { return m_allowSelection; }
      set { m_allowSelection = value; Invalidate(); }
    } // AllowSelection

    
    /// <summary>
    /// Gets the current thumbnail size in pixels.
    /// </summary>
    internal int ThumbnailSize
    {
      get { return m_size; }
    } // ThumbnailSize

    
    #endregion

    #region methods

    /// <summary>
    /// Ensures that the thumbnail is entirely visble at the specified index.
    /// </summary>
    /// <param name="index">Index of ThumbnailItem object.</param>
    public void EnsureVisible( int index )
    {
      // get the thumbnail item
      ThumbnailItem item = m_thumbnails[ index ];

      // only do this if the thumbnail is not entirely visible
      if( !this.ViewRectangle.Contains( item.Bounds ) )
      {
        // figure out the vertical scroll amount needed to make the item visible
        int dy = 0;
        if( item.Bounds.Top > this.ViewRectangle.Top )         
        {          
          dy = item.Bounds.Bottom - this.ViewRectangle.Bottom + m_spacing;
        }        
        else
        {
          dy = item.Bounds.Top - this.ViewRectangle.Top - m_spacing;          
        }

        // scroll the view rectangle 
        ScrollControl( 0, dy );

      } // if( !this.ViewRectangle.Contains( item.Bounds ) )
    }  // EnsureVisible( index )


    #endregion
		
    #region initialization
    
    /// <summary>
    /// Initializes a ThumbnailViewer object with default values.
    /// </summary>
    public ThumbnailViewer()
		{
      m_thumbnails = new ThumbnailCollection( this );
      m_selectedIndices = new ArrayList();
      //By default Keep the Multiple Selection option 
		} // ThumbnailViewer()

    /// <summary>
    /// AllowMultipleSelection, Sets or gets the MultipleSelection Flag
    /// </summary>
    public bool AllowMultipleSelection
    {
      get {return m_multiSelection;}
      set {m_multiSelection=value;}
    }//public bool AllowMultipleSelection
    

    //Get/Set the selectd Items from the Thumbnail list
    public ArrayList SelectedIndices
    {
      get {return m_selectedIndices;}
      set {m_selectedIndices = value;}
    }//public ArrayList SelectedItems

    #endregion

    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containging data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
        base.OnPaint( args );
        
        // cache the graphics object
        Graphics gfx = args.Graphics;

        // for each thumbnail
        for( int index = 0; index < m_thumbnails.Count; ++ index )
        {
          // get the thumbnail's bounds and image
          Rectangle bounds = m_thumbnails[ index ].Bounds;
          Image image = m_thumbnails[ index ].Thumbnail;

          // is the thumbnail visible?
          if( bounds.IntersectsWith( this.ViewRectangle ) )
          {
            // translate thumbnail bounds
            bounds.Offset( -this.ViewRectangle.Left, -this.ViewRectangle.Top );

            // center the thumbnail image
            int x = bounds.Left + ( ( bounds.Width - image.Width ) / 2 );
            int y = bounds.Top + ( ( bounds.Height - image.Height ) / 2 );

            // draw thumbnail
            gfx.DrawImage( image, x, y );

            // format the boder rctangle
            Rectangle borderBounds = m_drawOutline? new Rectangle( x-1, y-1, image.Width+2, image.Height+2 ) : bounds;
            
            // is the item(s) selected?
            if(m_selectedIndices.Contains(index) && m_allowSelection )
            {
              using( Pen pen = new Pen( Theme.SelectedPen.Color, 2.0F ) )
                gfx.DrawRectangle( pen, borderBounds );
            } // if( index == m_selectedItem && m_allowSelection )
            else
            {
              // draw outline rectangle
              using( Pen pen = new Pen( m_outlineColor ) )
                gfx.DrawRectangle( pen, borderBounds );
            } // else if( m_drawOutline )
          } // if( bounds.IntersectsWith( this.ViewRectangle ) )
        } // for( int index = 0; index < m_thumbnails.Count; ++ index )
      } // OnPaint( args )

    #endregion
    
    #region layout

    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      RecalculateThumbnailSize();
    } // OnResize( args )
    
    
    /// <summary>
    /// Recalculates the size of a thumbnail image in pixels.
    /// </summary>
    private void RecalculateThumbnailSize()
    {
      // figure out the size of a thumbnail
      m_size = ( this.Width - this.VScrollWidth - ( ( m_numberOfColumns + 1 ) * m_spacing ) ) / m_numberOfColumns;
    } // RecalculateThumbnailSize()
    
    /// <summary>
    /// Sets the client bounds for each thumbnail.
    /// </summary>
    internal void LayoutThumbnails()
    {      
      RecalculateThumbnailSize();
 
      // layout the thumbnails
      for( int index = 0; index < m_thumbnails.Count; ++index )
      { 
        // figure out the row and column for this thumbnail item
        int column = index % m_numberOfColumns;
        int row = index / m_numberOfColumns;

        // create the bounds for the thumbnail
        int x = ( column * m_size ) + ( ( column + 1 ) * m_spacing );
        int y = ( row    * m_size ) + ( ( row    + 1 ) * m_spacing );

        // set the bounds for the thumbnail
        m_thumbnails[ index ].Bounds = new Rectangle( x, y, m_size, m_size );        
      } // for( int index = 0; index < m_thumbnails.Count; ++index )

      // update the view rectangle
      int numberOfRows = ( int )Math.Ceiling( ( float ) m_thumbnails.Count / m_numberOfColumns );
      int requiredWidth = this.Width - this.VScrollWidth;
      int requiredHeight = m_thumbnails.Count > 0? m_thumbnails[ m_thumbnails.Count - 1 ].Bounds.Bottom + m_spacing : 0;
      SetVirtualSize( requiredWidth, requiredHeight );

      Invalidate();
    } // LayoutThumbnails()

    #endregion

    #region mouse

    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      try
      {
        // grab the mouse position
        Point pos = new Point( args.X, args.Y );

        // translate the position to view coordinates
        pos.Offset( this.ViewRectangle.X, this.ViewRectangle.Y );

        // for each thumbnail
        for( int index = 0; index < m_thumbnails.Count; ++index )
        {
          ThumbnailItem item = m_thumbnails[ index ];        
    
          // did the mouse click a thumbnail?
          if(item.Bounds.Contains( pos ) )
          {
            // select and enusre visible this thumbnail if it isn't already
            if(m_multiSelection == true || index != this.SelectedItem )
            {
              EnsureVisible( index );
              this.SelectedItem = index;

              //add the index in to the selectedindices array(for multiple selection) if multiselection is ON and Ctrl Key is down
              if ( Control.ModifierKeys == Keys.Control)
              {
                if(this.SelectedItem != -1)
                {
                  if(!m_selectedIndices.Contains(SelectedItem))
                    this.m_selectedIndices.Add(SelectedItem);
                  else
                    this.m_selectedIndices.Remove(SelectedItem);
                }//if(this.SelectedItem != -1)
              }
              else
              {
                //Clear the m_selectedIndices List if the Ctrl key is released
                this.m_selectedIndices.Clear();
                this.m_selectedIndices.Add(SelectedItem);
              }//if ( Control.ModifierKeys == Keys.Control)

              // raise the SelectionChanged event
              if( m_allowSelection && SelectionChanged != null )
                SelectionChanged( this, EventArgs.Empty );
            } // if( index != this.SelectedItem )
            
            // start checking for drag event on left button down
            if( args.Button == MouseButtons.Left )
              m_dragPoint = new Point( args.X, args.Y );
            
            // outta here
            break;
          } // if( item.Bounds.Contains( pos ) )
        } // for( int index = 0; index < m_thumbnails.Count; ++index )
      } // try
      catch( Exception e )
      {
        Viatronix.UI.MessageBox.ShowError( e );
      } // catch( Exception e )
    }// OnMouseDown( args )

    
    /// <summary>
    /// Called when the mouse moved within the control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      base.OnMouseMove( args );

      try
      {
        // threshold distance from drag point to initiate drag and drop
        const int threshold = 8;
        
        // is the left mouse buton is down and a thumbnail is selected?
        if( args.Button == MouseButtons.Left && m_selectedItem != -1 )
        {
          // calculate the difference from the original drag point
          int dx = Math.Abs( args.X - m_dragPoint.X );
          int dy = Math.Abs( args.Y - m_dragPoint.Y );

          // has the threshold been reached?
          if( dx > threshold || dy > threshold )
          {
            // raise the event
            if( ItemDrag != null )
            {
              ItemDragEventArgs dragArgs = new ItemDragEventArgs( MouseButtons.Left, m_thumbnails[ m_selectedItem ] );
              ItemDrag( this, dragArgs );
            }
          } // if( dx > threshold || dy > threshold )
        } // if( args.Button == MouseButtons.Left && m_selectedItem != -1 )
      } // try
      catch( Exception e )
      {
        Viatronix.UI.MessageBox.ShowError( e );
      } // catch( Exception e )
    } // OnMouseMove( args )
    
    #endregion

 	} // class ThumbnailViewer

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: ThumbnailViewer.cs,v $
// Revision 1.6  2005/01/17 20:52:21  gdavidson
// Set a minimum bitmap size.
//
// Revision 1.5  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.4  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.3  2004/06/09 19:09:01  geconomos
// Fixed error with selected index property.
//
// Revision 1.2  2004/05/12 13:01:10  romy
// Modified OnMouseDown for Multiple Selection.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.12  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.11.2.1  2003/08/17 23:49:37  geconomos
// Updated to handle the mouse wheel event.
//
// Revision 1.11  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.10  2003/05/15 15:08:36  geconomos
// Issue #3240: Application crashing when selecting a thumbnail.
//
// Revision 1.9  2003/05/08 15:26:53  geconomos
// Issue # 3198: Thumbnails not properly updating.
//
// Revision 1.8  2003/05/05 15:04:47  frank
// code review
//
// Revision 1.7  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.6  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.5  2003/03/31 19:18:09  geconomos
// Made "selected item" properties not serializable in the designer.
//
// Revision 1.4  2003/03/31 19:12:54  geconomos
// Default the thumbnail viewer not to allow selection.
//
// Revision 1.3  2003/03/25 20:35:04  geconomos
// Implemented border type.
//
// Revision 1.2  2003/03/25 20:16:57  geconomos
// Fixed bug with not properly setting the selected item after removing a thumbnail.
//
// Revision 1.1  2003/03/24 16:03:27  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ThumbnailViewer.cs,v 1.6 2005/01/17 20:52:21 gdavidson Exp $
// $Id: ThumbnailViewer.cs,v 1.6 2005/01/17 20:52:21 gdavidson Exp $

#endregion
