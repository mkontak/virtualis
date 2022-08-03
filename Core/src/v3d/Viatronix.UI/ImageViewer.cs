// $Id: ImageViewer.cs,v 1.5 2006/09/21 14:26:28 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
	/// <summary>
	/// This control displays a collection of images in a scrollable control.
	/// </summary>
  public class ImageViewer : Viatronix.UI.ScrollableControl, ISupportInitialize
  {

    #region events

    /// <summary>
    /// Occurs when an image is selected.
    /// </summary>
    public event EventHandler SelectionChanged = null;

    #endregion

    #region fields

    /// <summary>
    /// The size of the images.
    /// </summary>
    private Size m_imageSize = Size.Empty;

    /// <summary>
    /// The spacing between the images.
    /// </summary>
    private int m_spacing = 10;

    /// <summary>
    /// The collection of images diplayed by the control.
    /// </summary>
    private ImageCollection m_images = null;

    /// <summary>
    /// The bounds of all the images.
    /// </summary>
    private ArrayList m_imageBounds = new ArrayList();

    /// <summary>
    /// Determines if the size of the images should be automatically adjusted.
    /// </summary>
    private bool m_autoSize = false;

    /// <summary>
    /// Determines if an outline should be drawn around the images.
    /// </summary>
    private bool m_drawOutline = true;

    /// <summary>
    /// The color of the outline.
    /// </summary>
    private Color m_outlineColor = Color.White;

    /// <summary>
    /// Collection of selected indices.
    /// </summary>
    private ArrayList m_selectedIndices = new ArrayList();

    /// <summary>
    /// Collection of selected images.
    /// </summary>
    private ImageCollection m_selectedImages = new ImageCollection();

    /// <summary>
    /// Determines if multiple selections can be performed.
    /// </summary>
    private bool m_multiSelect = false;

    /// <summary>
    /// Determines if selections are allowed.
    /// </summary>
    private bool m_allowSelection = true;

    /// <summary>
    /// Determines if the images proportions should be maintained.
    /// </summary>
    private bool m_maintainProportions = true;

    /// <summary>
    /// Used to delay the intialization of the control.
    /// </summary>
    private bool m_delayIntialize = false;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of an ImageViewer.
    /// </summary>
    public ImageViewer()
    {
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

			m_images = new ImageCollection( this );
    } // ImageViewer()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the display size for the images.
    /// </summary>
    public Size ImageSize
    {
      get 
      { 
        if( this.AutoSize )
        {
          int size = this.Height - ( 2 * this.Spacing );
          m_imageSize = new Size( size, size );
        }

        return m_imageSize; 
      }
      set { m_imageSize = value; }
    } // ImageSize


    /// <summary>
    /// Gets or sets the spacing beween theimages.
    /// </summary>
    public int Spacing
    {
      get { return m_spacing; }
      set { m_spacing = value; }
    } // Spacing


    /// <summary>
    /// Gets or sets the collection of images displayed by this control.
    /// </summary>
    public ImageCollection Images
    {
      get { return m_images; }
      set 
      {
        // remove reference in old collection
        m_images.ImageViewer = null;

        m_images = value;

        m_selectedIndices.Clear();
        m_selectedImages.Clear();

        m_images.ImageViewer = this;
      }
    } // Images


    /// <summary>
    /// Gets or sets if auto resizing the images should b performed.
    /// </summary>
    public new bool AutoSize
    {
      get { return m_autoSize; }
      set { m_autoSize = value; }
    } // AutoSize


    /// <summary>
    /// Gets or sets if an outline should be drawn around the images.
    /// </summary>
    public bool DrawOutline
    {
      get { return m_drawOutline; }
      set { m_drawOutline = value; }
    } // DrawOutline


    /// <summary>
    /// Gets or set the outline color.
    /// </summary>
    public Color OutlineColor
    {
      get { return m_outlineColor; }
      set { m_outlineColor = value; }
    } // OutlineColor


    /// <summary>
    /// Returns the selected indices.
    /// </summary>
    public int[] SelectedIndices
    {
      get { return (int[]) m_selectedIndices.ToArray( typeof( int )); }
      set
      {
        m_selectedIndices.Clear();
        m_selectedIndices.AddRange( value );

        if( m_selectedIndices.Count > 0 )
          EnsureVisible( (Rectangle) m_imageBounds[ (int) m_selectedIndices[0] ]);

        Invalidate();
      }
    } // SelectedIndices


    /// <summary>
    /// Return the selected images.
    /// </summary>
    public ImageCollection SelectedImages
    {
      get { return m_selectedImages; }
    } // SelectedImages

    
    /// <summary>
    /// Gets or sets if multiple selctions is valid.
    /// </summary>
    public bool MultiSelect
    {
      get { return m_multiSelect; }
      set { m_multiSelect = value; }
    } // MultiSelect


    /// <summary>
    /// Gets or sets if selections are allowed.
    /// </summary>
    public bool AllowSelection
    {
      get { return m_allowSelection; }
      set { m_allowSelection = value; }
    } // AllowSelection


    /// <summary>
    /// Gets or set if the images proportions should be maintained.
    /// </summary>
    public bool MaintainProportions
    {
      get { return m_maintainProportions; }
      set { m_maintainProportions = value; }
    } // MaintainProportions

    
    /// <summary>
    /// Return the bounds for all the images.
    /// </summary>
    internal ArrayList ImageBounds
    {
      get { return m_imageBounds; }
    } // ImageBounds

    #endregion

    #region override methods

    /// <summary>
    /// Raises the Resize event. 
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Layout();

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Raises the Paint event. 
    /// </summary>
    /// <param name="args">An PaintEventArgs that contains the event data.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;
      gfx.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
      gfx.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
      for( int i = 0; i < this.Images.Count; ++i )
      {
        Image image = this.Images[ i ];

        Rectangle bounds = (Rectangle) this.ImageBounds[ i ];
        if( bounds.IntersectsWith( this.ViewRectangle ))
        {
          // translate image bounds
          bounds.Offset( -this.ViewRectangle.Left, -this.ViewRectangle.Top );
          if( m_maintainProportions )
            Utilities.DrawImage( gfx, image, bounds );
          else
            gfx.DrawImage( image, bounds );

          // inflate the bounds 1 pixel for the outline
          bounds.Inflate( 1, 1 );
          if( m_allowSelection && m_selectedIndices.Contains( i ))
          {
            using( Pen pen = new Pen( Theme.SelectedPen.Color, 2.0F ) )
              gfx.DrawRectangle( pen, bounds );
          } 
          else if( m_drawOutline ) 
          {
            using( Pen pen = new Pen( this.OutlineColor ) )
              gfx.DrawRectangle( pen, bounds );
          }
        }
      }
    } // OnPaint( args )


    /// <summary>
    /// Raises the MouseDown event. 
    /// </summary>
    /// <param name="args">An MouseEventArgs that contains the event data.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      if( !m_allowSelection )
        return;

      // grab the mouse position
      Point pos = new Point( args.X, args.Y );

      // translate the position to view coordinates
      pos.Offset( this.ViewRectangle.X, this.ViewRectangle.Y );

      for( int i = 0; i < this.ImageBounds.Count; ++i )
      {
        Rectangle bounds = (Rectangle) this.ImageBounds[ i ];
        if( bounds.Contains( pos ))
        {
          // multiple selection
          if( m_multiSelect && Control.ModifierKeys == Keys.Control ) 
          {
            if( m_selectedIndices.Contains( i ))
              m_selectedIndices.Remove( i );
            else
            {
              m_selectedIndices.Add( i );
              m_selectedImages.Add( this.Images[ i ]);  
            }
          }
          else
          {
            m_selectedIndices.Clear();
            m_selectedImages.Clear();
            m_selectedIndices.Add( i );
            m_selectedImages.Add( this.Images[ i ]);  
          }

          // ensure the selected image is visible
          EnsureVisible( bounds );

          // raise the SelectionChanged event
          if( this.SelectionChanged != null )
            this.SelectionChanged( this, EventArgs.Empty );

          Refresh();

          break;
        }
      }
    } // OnMouseDown( args )


    /// <summary>
    /// Raises the DoubleClick event. 
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnDoubleClick( EventArgs args )
    {
      // grab the mouse position
      Point pos = PointToClient( Control.MousePosition );

      // translate the position to view coordinates
      pos.Offset( this.ViewRectangle.X, this.ViewRectangle.Y );

      for( int i = 0; i < this.ImageBounds.Count; ++i )
      {
        Rectangle bounds = (Rectangle) this.ImageBounds[i];
        if( bounds.Contains( pos ))
        {
          base.OnDoubleClick( args );
          break;
        }
      } 
    } //OnDoubleClick( args )


    #endregion

    #region methods

    /// <summary>
    /// Resets the contents of the ImageViewer.
    /// </summary>
    public void Reset()
    {
      this.Images.Clear();
      m_selectedImages.Clear();
      m_selectedIndices.Clear();
      ResetScrollBar();
    } // Reset()


    /// <summary>
    /// Determines the bounds for each image in the collection.
    /// </summary>
    internal new void Layout()
    {
      // don't calculate if delayIntialize is active
      if( m_delayIntialize )
        return;

      if( this.Images.Count == 0 )
        return;

      this.ImageBounds.Clear();

      int x = this.Spacing;
      int y = this.Spacing;
      for( int i = 0; i < this.Images.Count; ++i )
      {
        int availableWidth = this.Width - this.VScrollWidth - x;
        if( availableWidth < ( this.ImageSize.Width + this.Spacing ))
        {
          x = this.Spacing;
          y += this.ImageSize.Height + this.Spacing;
        }

        Image image = this.Images[ i ];
        Rectangle bounds = new Rectangle( x, y, this.ImageSize.Width, this.ImageSize.Height );
        this.ImageBounds.Add( bounds );

        x += this.ImageSize.Width + this.Spacing;
      }

      SetVirtualSize( this.Width, y + this.ImageSize.Height + this.Spacing );

      Rectangle virtualArea = new Rectangle( 0, 0, this.VirtualSize.Width, this.VirtualSize.Height );
      if( this.ViewRectangle.Bottom > virtualArea.Bottom )
        ScrollControl( 0, this.ScrollBar.Maximum );
    } // Layout()


    /// <summary>
    /// Ensures that the thumbnail is entirely visble at the specified index.
    /// </summary>
    /// <param name="index">Index of ThumbnailItem object.</param>
    private void EnsureVisible( Rectangle bounds )
    {
      // only do this if the thumbnail is not entirely visible
      if( !this.ViewRectangle.Contains( bounds ) )
      {
        // figure out the vertical scroll amount needed to make the item visible
        int dy = 0;
        if( bounds.Top > this.ViewRectangle.Top )              
          dy = bounds.Bottom - this.ViewRectangle.Bottom + m_spacing; 
        else
          dy = bounds.Top - this.ViewRectangle.Top - m_spacing;          

        // scroll the view rectangle 
        ScrollControl( 0, dy );
      } 
    }  // EnsureVisible( bounds )

    #endregion

    #region ISupportInitialize Members

    /// <summary>
    /// Signals the object that initialization is starting.
    /// </summary>
    public void BeginInit()
    {
      m_delayIntialize = true;
    } // BeginInit()


    /// <summary>
    /// Signals the object that initialization is complete.
    /// </summary>
    public void EndInit()
    {
      m_delayIntialize = false;
      Layout();
    } // EndInit()

    #endregion

  } // class ImageViewer

  #region ImageCollection class

  /// <summary>
  /// A collection of Images.
  /// </summary>
  public class ImageCollection : CollectionBase
  {

    #region fields

    /// <summary>
    /// Reference to the parent ImageViewer.
    /// </summary>
    private ImageViewer m_imageViewer = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of an ImageCollection with the specified parent.
    /// </summary>
    /// <param name="imageViewer">The parent.</param>
    internal ImageCollection( ImageViewer imageViewer )
    {
      m_imageViewer = imageViewer;
    } // ImageCollection( imageViewer )


    /// <summary>
    /// Creates a new instance of an ImageCollection.
    /// </summary>
    public ImageCollection()
    {
    } // ImageCollection()

    #endregion

    #region properties

    /// <summary>
    /// Returns the Image at the specified index.
    /// </summary>
    public Image this[ int index ]
    {
      get { return (Image) this.List[ index ]; }
    } // Indexer


    /// <summary>
    /// Gets or sets the parent ImageViewer.
    /// </summary>
    internal ImageViewer ImageViewer
    {
      get { return m_imageViewer; }
      set { m_imageViewer = value; }
    } // ImageViewer

    #endregion

    #region methods

    /// <summary>
    /// Adds the provided image to the collection.
    /// </summary>
    /// <param name="image">An image.</param>
    public void Add( Image image )
    {
      this.List.Add( image );

      if( m_imageViewer != null )
      {
        m_imageViewer.Layout();
        m_imageViewer.Invalidate();
      }
    } // Add( image )


    /// <summary>
    /// Adds the contents of the provided ImageCollection to this collection.
    /// </summary>
    /// <param name="collection">An ImageCollection.</param>
    public void Add( ImageCollection collection )
    {
      if( m_imageViewer != null )
        m_imageViewer.BeginInit();

      foreach( Image image in collection )
        this.List.Add( image );

      if( m_imageViewer != null )
        m_imageViewer.EndInit();
    } // Add( collection )


    /// <summary>
    /// Removes the specified image from the collection.
    /// </summary>
    /// <param name="image">An Image.</param>
    public void Remove( Image image )
    {
      this.List.Remove( image );

      if( m_imageViewer != null )
      {
        m_imageViewer.Layout();
        m_imageViewer.Invalidate();
      }
    } // Remove( image )

    #endregion

  } // class ImageCollection

  #endregion

} // namespace Viatronix.UI


#region revision history

// $Log: ImageViewer.cs,v $
// Revision 1.5  2006/09/21 14:26:28  gdavidson
// Issue #5009: Check mouse position before raising DoubleClick event
//
// Revision 1.4  2006/01/03 16:27:55  gdavidson
// Improved the image quality.
//
// Revision 1.3  2005/10/26 19:56:53  gdavidson
// Added a Reset method.
//
// Revision 1.2  2005/06/24 14:55:08  gdavidson
// Added a set property for the SelectedIndices.
//
// Revision 1.1  2005/06/22 15:05:27  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ImageViewer.cs,v 1.5 2006/09/21 14:26:28 gdavidson Exp $
// $Id: ImageViewer.cs,v 1.5 2006/09/21 14:26:28 gdavidson Exp $

#endregion