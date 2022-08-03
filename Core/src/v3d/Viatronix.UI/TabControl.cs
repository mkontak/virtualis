// $Id: TabControl.cs,v 1.7 2005/03/08 19:11:06 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.ComponentModel;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;

namespace Viatronix.UI
{

  #region TabControl
  
  /// <summary>
  /// Manages a related set of tab pages.
  /// </summary>
  public class TabControl : System.Windows.Forms.Control
  {

    #region TabPageCollection
    
    /// <summary>
    /// Represents a collection of TabPage objects.
    /// </summary>
    public class TabPageCollection : CollectionBase
    {

      #region methods

      /// <summary>
      /// Adds the specified TabPage object to the collection.
      /// </summary>
      /// <param name="image">TabPage object to add.</param>
      /// <returns>Array index of TabPage object.</returns>
      public int Add( TabPage tabpage )
      {
        return List.Add( tabpage );
      } // Add( tabpage )
      
    
      /// <summary>
      /// Inserts a TabPage object into the collection at the specified index.
      /// </summary>
      /// <param name="index">Index to insert TabPage object.</param>
      /// <param name="tabpage">TabPage object to insert.</param>
      public void Insert( int index, TabPage tabpage )
      {
        // insert the tabpage to the internal list
        List.Insert( index, tabpage );
      } // Insert( index, tabpage )
      
      
      /// <summary>
      /// Returns the index of the specified tab page object.
      /// </summary>
      /// <param name="pane">TabPage to get index for.</param>
      /// <returns>Index of specified pane.</returns>
      public int IndexOf( TabPage tabpage )
      {
        return List.IndexOf( tabpage );
      } // IndexOf( TabPage tabpage )
      

      /// <summary>
      /// Removes the specified tab page.
      /// </summary>
      /// <param name="tabpage">TabPage to remove.</param>
      public void Remove( TabPage tabpage )
      {
        List.Remove( tabpage );
      } // Remove( TabPage tabpage )
      

      /// <summary>
      /// Get or sets the specified TabPage.
      /// </summary>
      public TabPage this[ int index ] 
      {
        get { return ( TabPage ) List[ index ]; }
        set { List[ index ] = value; }
      } // this[ int index ] 
     
      #endregion

    } // class TabPageCollection

    #endregion
    
    #region events

    /// <summary>
    /// Raised when the selected tab index has changed
    /// </summary>
    public event TabEventHandler TabChanging;

    /// <summary>
    /// Raised when the selected tab index has changed
    /// </summary>
    public event TabEventHandler TabChanged;

    #endregion

    #region fields

    /// <summary>
    /// padding ( in pixels ) around tab's text on all sides 
    /// </summary>
    private const int TAB_PADDING = 5;

    /// <summary>
    /// selected tab index; -1 otherwise
    /// </summary>
    private int m_selectedIndex = -1;

    /// <summary>
    /// color of tab
    /// </summary>    
    private Color m_defaultColor = Color.FromArgb( 128, 180, 143 );

    /// <summary>
    /// color of selected tab
    /// </summary>
    private Color m_selectedColor = Color.FromArgb( 63, 71, 120 );

    /// <summary>
    /// array of Tab instance 
    /// </summary>
    private TabPageCollection m_tabs = new TabPageCollection();

    /// <summary>
    /// specifiex the location of the tabs ( top, botttom, left, right )
    /// </summary>
    private TabOrientation m_orientation = TabOrientation.Top;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the select tab.
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public TabPage SelectedTab
    {
      get { return m_selectedIndex == -1? null : ( ( TabPage ) m_tabs[ m_selectedIndex ] ); }
      set 
      { 
        foreach( TabPage tab in m_tabs )
        {
          if( tab == value )
          {
            this.SelectedIndex = m_tabs.IndexOf( tab );
            break;
          }
        }        
      }
    } // SelectedTab

    
    /// <summary>
    /// Gets or sets the selected tab index
    /// </summary>
    public int SelectedIndex
    {
      get { return m_selectedIndex; }
      set { SetSelectedIndex( value ); }
    } // SelectedIndex

    
    /// <summary>
    /// Gets or sets the orientation of the tabs.
    /// </summary>
    public TabOrientation Orientation 
    {
      get { return m_orientation; }
      set { m_orientation = value; LayoutTabs(); }
    } // Orientation 


    /// <summary>
    /// Gets or sets the BackColor for a selected tab
    /// </summary>
    public Color SelectedBackColor
    {
      get { return m_selectedColor; }
      set { m_selectedColor = value; }
    } // SelectedBackColor
    

    /// <summary>
    /// gets the collection of tab pages
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public TabPageCollection TabPages
    {
      get { return m_tabs; }
    } // TabPages
    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes a tabControl instance with default values
    /// </summary>
    public TabControl()
    {
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      
      this.ForeColor = Color.White;
    } // TabControl()

    #endregion

    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      if( m_orientation == TabOrientation.Top )
        DrawTabAreaTop( gfx );
      
      else if( m_orientation == TabOrientation.Bottom )
        DrawTabAreaBottom( gfx );
      
      else if( m_orientation == TabOrientation.Left )
        DrawTabAreaLeft( gfx );
 
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
      gfx.SmoothingMode = SmoothingMode.HighQuality;

      for( int index = m_tabs.Count - 1; index >= 0; --index )
      {
        TabPage tab = ( TabPage ) m_tabs[ index ];
        
        if( m_orientation == TabOrientation.Top )
          DrawTabTop( gfx,  tab );
        
        else if( m_orientation == TabOrientation.Bottom )
          DrawTabBottom( gfx, tab );
        
        else if( m_orientation == TabOrientation.Left )
          DrawTabLeft( gfx, tab );
      }

    } // OnPaint( args )


    /// <summary>
    /// Draws the top of the tab.
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="tab">the tab page to draw</param>
    private void DrawTabTop( Graphics gfx, TabPage tab )
    {
      int index = m_tabs.IndexOf( tab );
      
      Rectangle bounds = tab.ButtonBounds;

      if( m_selectedIndex == index )
      {
        using( SolidBrush brush = new SolidBrush( this.BackColor ) )
        {
          Rectangle tmp = bounds;
          tmp.Inflate( 1, 1 );
          gfx.FillRectangle( brush, tmp );
        }
      }

      /**
       *       (x1,y1)         (x4,y1)
       *           ______________
       *          /              \
       * (x2,y2) |                | (x3,y2)
       *         |                |
       * (x2,y3) |----------------| (x3,y3)
       */

      int x1 = bounds.Left + 3;
      int x2 = bounds.Left;
      int x3 = bounds.Right;
      int x4 = bounds.Right - 3;

      int y1 = bounds.Top;
      int y2 = bounds.Top + 3;
      int y3 = bounds.Bottom;

      using( Pen pen = new Pen( Color.Black ) )
      {
        gfx.DrawLine( pen, x1, y1, x2, y2 );
        
        using( Pen white = new Pen( Color.White ) )
          gfx.DrawLine( white, x2, y2, x2, y3 );
        
        gfx.DrawLine( pen, x3, y3, x3, y2 );
        gfx.DrawLine( pen, x3, y2, x4, y1 );
        gfx.DrawLine( pen, x4, y1, x1, y1 );
      }

      // if this tab is selected draw the selection fill on the tab
      if( index == m_selectedIndex )
      {
        int leftIndent = tab.Column == 0 ? 2 : 2;
        int topIndent = 2;
        int rightIndent = 3;
        int bottomIndex = tab.Row != 0 ? 1 : 3;

        Point[] points = new Point[ 6 ];
        points[ 0 ] = new Point( x1+leftIndent,  y1+topIndent );
        points[ 1 ] = new Point( x2+leftIndent,  y2+topIndent );        
        points[ 2 ] = new Point( x2+leftIndent,  y3+bottomIndex );
        points[ 3 ] = new Point( x3-rightIndent, y3+bottomIndex );
        points[ 4 ] = new Point( x3-rightIndent, y2+topIndent );
        points[ 5 ] = new Point( x4-rightIndent, y1+topIndent );

        using( SolidBrush brush = new SolidBrush( m_selectedColor ) )
          gfx.FillPolygon( brush, points );
      } // if( index == m_selectedIndex )

      // output the tab's text
      using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )
      {
        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;
        gfx.DrawString( tab.Text, tab.Font, brush, bounds, format );
      } // using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )  
    } // DrawTab( gfx, tab )

    
    /// <summary>
    /// Draws the top of the tab area
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    private void DrawTabAreaTop( Graphics gfx )
    {
      int y = m_tabs.Count == 0 ? 0 :  ( int )((( TabPage )m_tabs[ 0 ]).ButtonBounds.Bottom );
      
      /*
       * Draw black border around TabPage area
       * 
       *  (x1,y1)     (x2,y1)
       *    +           +
       *    |           |
       *    |           |
       *    |           |
       *    |           |
       *    +-----------+
       *  (x1,y2)     (x2,y2)
       */
      int x1 = 0;
      int y1 = y;
      int x2 = this.Width - 1;
      int y2 = this.Height - 1;
      using( Pen pen = new Pen( Color.White ) )
      {
        gfx.DrawLine( pen, x1, y1, x1, y2 );
        gfx.DrawLine( pen, x1, y2, x2, y2 );
      }

      using( Pen pen = new Pen( Color.Black ) )
      {
        gfx.DrawLine( pen, x2, y2, x2, y1 );
        gfx.DrawLine( pen, x2, y1, x1, y1 );
      }
    
    } // DrawTabArea( Graphics gfx )


    /// <summary>
    /// Draws the left part of the tab
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="tab">the tab page to draw</param>
    private void DrawTabLeft( Graphics gfx, TabPage tab )
    {
      int index = m_tabs.IndexOf( tab );
      
      Rectangle bounds = tab.ButtonBounds;

      if( m_selectedIndex == index )
      {
        using( SolidBrush brush = new SolidBrush( this.BackColor ) )
        {
          Rectangle tmp = bounds;
          tmp.Inflate( 1, 1 );
          gfx.FillRectangle( brush, tmp );
        }
      }


      /**
       *     (x1,y1)  (x3,y1)
       *          /------
       * (x2,y2) |      |
       *         |      |
       *         |      |
       *         |      |
       *         |      |
       * (x2,y3) |      |
       *          \------
       *      (x1,y4)  (x3,y4)
       * 
       */


      int x1 = bounds.Left + 3;
      int x2 = bounds.Left;
      int x3 = bounds.Right;

      int y1 = bounds.Top;
      int y2 = bounds.Top + 3;
      int y3 = bounds.Bottom - 3;
      int y4 = bounds.Bottom;

      using( Pen pen = new Pen( Color.Black ) )
      {
        
        gfx.DrawLine( pen, x1, y1, x2, y2);
        gfx.DrawLine( pen, x2, y2, x2, y3 );
        gfx.DrawLine( pen, x2, y3, x1, y4 );
        gfx.DrawLine( pen, x1, y4, x3, y4 );

        using( Pen white = new Pen( Color.White ) )
          gfx.DrawLine( white, x1, y1, x3, y1 ); 
      }

      // if this tab is selected draw the selection fill on the tab
      if( index == m_selectedIndex )
      {
        int leftIndent = 2;
        int topIndent = 2;
        int rightIndent = 2;

        Point[] points = new Point[ 6 ];
        points[ 0 ] = new Point( x1 + leftIndent,  y1 + topIndent + 1 );
        points[ 1 ] = new Point( x2 + leftIndent,  y2 + topIndent + 1 );        
        points[ 2 ] = new Point( x2 + leftIndent,  y3 - topIndent );        
        points[ 3 ] = new Point( x1 + rightIndent, y4 - topIndent );        
        points[ 4 ] = new Point( x3 + rightIndent, y4 - topIndent );        
        points[ 5 ] = new Point( x3 + rightIndent, y1 + topIndent + 1);

        using( SolidBrush brush = new SolidBrush( m_selectedColor ) )
          gfx.FillPolygon( brush, points );
      } // if( index == m_selectedIndex )

      // output the tab's text
      Matrix matrix = gfx.Transform;
      using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )
      {
        StringFormat format = new StringFormat();
        format.FormatFlags = StringFormatFlags.NoClip;
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;
        
        PointF middle = new PointF( ( float)bounds.Left +  ( bounds.Width / 2.0F ),(float) bounds.Top +  ( bounds.Height / 2.0F ));
        
        gfx.TranslateTransform( middle.X, middle.Y );
        gfx.RotateTransform( -90 );
        
        gfx.DrawString( tab.Text, tab.Font, brush, 0, 0, format );
      } // using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )  
      gfx.Transform = matrix;

    } // DrawTab( gfx, tab )


    /// <summary>
    /// Draws the left of the tab area
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    private void DrawTabAreaLeft( Graphics gfx )
    {
      
      /*
       * Draw black border around TabPage area
       * 
       *  (x1,y1)     (x2,y1)
       *    +-----------+
       *    |           |
       *    |           |
       *    |           |
       *    |           |
       *    +-----------+
       *  (x1,y2)     (x2,y2)
       */
      int x1 = m_tabs.Count == 0 ? this.Width - 1 :  ( int )((( TabPage )m_tabs[ 0 ]).ButtonBounds.Right -1 );
      int y1 = 0;
      int x2 = this.Width - 1;
      int y2 = this.Height - 1;
      using( Pen pen = new Pen( Color.White ) )
      {
        gfx.DrawLine( pen, x1, y1, x2, y1 );
        gfx.DrawLine( pen, x1, y1, x1, y2 );
      }

      using( Pen pen = new Pen( Color.Black ) )
      {
        gfx.DrawLine( pen, x1, y2, x2, y2 );
        gfx.DrawLine( pen, x2, y1, x2, y2 );
      }

    } // DrawTabArea( Graphics gfx )
    

    /// <summary>
    /// Draws the bottom of the tab page
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="tab">the tab page to draw</param>
    private void DrawTabBottom( Graphics gfx, TabPage tab )
    {
      int index = m_tabs.IndexOf( tab );
      
      Rectangle bounds = tab.ButtonBounds;

      if( m_selectedIndex == index )
      {
        using( SolidBrush brush = new SolidBrush( this.BackColor ) )
        {
          Rectangle tmp = bounds;
          tmp.Inflate( 1, 1 );
          gfx.FillRectangle( brush, tmp );
        }
      }

      /**
       * (x1,y1) |----------------| (x4,y1)
       *         |                |
       * (x1,y2) |                | (x4,y2)
       *          \              /
       *           --------------
       *       (x2,y3)         (x3,y3)
       */

      int x1 = bounds.Left;
      int x2 = bounds.Left  + 3;
      int x3 = bounds.Right - 3;
      int x4 = bounds.Right;

      int y1 = bounds.Top;
      int y2 = bounds.Bottom - 3 ;
      int y3 = bounds.Bottom;

      using( Pen pen = new Pen( Color.Black ) )
      {
        using( Pen white = new Pen( Color.White ) )
          gfx.DrawLine( white, x1, y1, x1, y2 );        
        
        gfx.DrawLine( pen, x1, y2, x2, y3 );
        gfx.DrawLine( pen, x2, y3, x3, y3 );
        gfx.DrawLine( pen, x3, y3, x4, y2 );
        gfx.DrawLine( pen, x4, y2, x4, y1 );
      }

      // if this tab is selected draw the selection fill on the tab
      if( index == m_selectedIndex )
      {
        int leftIndent = 2;
        int topIndent = 3;
        int rightIndent = 3;

        Point[] points = new Point[ 6 ];
        points[ 0 ] = new Point( x1+leftIndent,  y1 - topIndent - 1 );
        points[ 1 ] = new Point( x1+leftIndent,  y2 - topIndent );        
        points[ 2 ] = new Point( x2+leftIndent,  y3 - topIndent );        
        points[ 3 ] = new Point( x3-rightIndent, y3 - topIndent );        
        points[ 4 ] = new Point( x4-rightIndent, y2 - topIndent );        
        points[ 5 ] = new Point( x4-rightIndent, y1 - topIndent - 1 );

        using( SolidBrush brush = new SolidBrush( m_selectedColor ) )
          gfx.FillPolygon( brush, points );
      } // if( index == m_selectedIndex )

      // output the tab's text
      using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )
      {
        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;
        gfx.DrawString( tab.Text, tab.Font, brush, bounds, format );
      } // using( SolidBrush brush = new SolidBrush( tab.ForeColor ) )  

    } // DrawTabBottom( gfx, tab )


    /// <summary>
    /// Draws the bottom of the tab area
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    private void DrawTabAreaBottom( Graphics gfx )
    {
      
      /*
       * Draw black border around TabPage area
       * 
       *  (x1,y1)     (x2,y1)
       *    +-----------+
       *    |           |
       *    |           |
       *    |           |
       *    |           |
       *    +-----------+
       *  (x1,y2)     (x2,y2)
       */
      int x1 = 0;
      int y1 = 0;
      int x2 = this.Width - 1;
      int y2 = m_tabs.Count == 0 ? this.Height - 1 :  ((( TabPage )m_tabs[ 0 ]).ButtonBounds.Top );
      
      using( Pen pen = new Pen( Color.White ) )
      {
        gfx.DrawLine( pen, x1, y1, x2, y1 );
        gfx.DrawLine( pen, x1, y1, x1, y2 );
      }

      using( Pen pen = new Pen( Color.Black ) )
      {
        gfx.DrawLine( pen, x1, y2, x2, y2 );
        gfx.DrawLine( pen, x2, y1, x2, y2 );
      }
    
    } // DrawTabAreaBottom( Graphics gfx )

    #endregion

    #region layout

    /// <summary>
    /// Gets the bounds of the top of the tab page
    /// </summary>
    /// <returns>the bounds of the top of the tab page</returns>
    private Rectangle GetTabPageBoundsTop()
    {

      int x = 0;
      int y = m_tabs.Count == 0 ? 0 :  ( int )((( TabPage )m_tabs[ 0 ]).ButtonBounds.Bottom );
      int width = this.Width - x;
      int height = this.Height - y;
      
      Rectangle bounds =  new Rectangle( x, y, width, height );
      bounds.Inflate( -3, -3 );
      return bounds;

    } // GetTabPageBoundsTop()


    /// <summary>
    /// Gets the bounds of the left of the tab page
    /// </summary>
    /// <returns>the bounds of the left of the tab page</returns>
    private Rectangle GetTabPageBoundsLeft()
    {
      int x1 = m_tabs.Count == 0 ? this.Width - 1 :  ( int )((( TabPage )m_tabs[ 0 ]).ButtonBounds.Right -1 );
      int y1 = 0;
      int x2 = this.Width - 1;
      int y2 = this.Height - 1;

      Rectangle bounds = new Rectangle( x1,y1, x2-x1, y2-y1 );
      bounds.Inflate( -3, -3 );
      return bounds;    
    } // GetTabPageBoundsLeft()


    /// <summary>
    /// Gets the bounds of the bottom of the tab page
    /// </summary>
    /// <returns>the bounds of the bottom of the tab page</returns>
    private Rectangle GetTabPageBoundsBottom()
    {
      int x1 = 0;
      int y1 = 0;
      int x2 = this.Width - 1;
      int y2 = m_tabs.Count == 0 ? this.Height - 1 :  ( int )((( TabPage )m_tabs[ 0 ]).ButtonBounds.Top );
 
      Rectangle bounds = new Rectangle( x1,y1, x2-x1, y2-y1 );
      bounds.Inflate( -3, -3 );
      return bounds;    
    } // GetTabPageBoundsBottom()


    /// <summary>
    /// Lays out the tabs
    /// </summary>
    private void LayoutTabs()
    {
      if( m_orientation == TabOrientation.Top || m_orientation == TabOrientation.Bottom )
        LayoutTabsHorizontal();
      else
        LayoutTabsVertical();

      this.Invalidate();
    } // LayoutTabs()
  

    /// <summary>
    /// Lays out the tabs horizontally
    /// </summary>
    private void LayoutTabsHorizontal()
    {
      using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      {
        int x = 0;
        int rowHeight = int.MinValue;
        int row = 0;
        int column = 0;

        foreach( TabPage tab in m_tabs )
        {
          // get the size of the tabpage's caption
          SizeF size = gfx.MeasureString( tab.Text, tab.Font );
          
          // calculate the required with and height for the tabpage
          int width = (int)( size.Width + ( 2 * TAB_PADDING ) + 0.5F );
          int height = (int)( size.Height + ( 2 * TAB_PADDING ) + 0.5F );

          // store the max height
          rowHeight = Math.Max( rowHeight, height );

          // is the tabpage's width past the control's right border
          if( ( x + width ) > this.Width )
          {
            // shift all the existing tabs up/down
            for( int index = 0; index < m_tabs.IndexOf( tab ); ++index )
            {
              TabPage tmp = ( TabPage )m_tabs[ index ];

              Rectangle bounds = tmp.ButtonBounds;              
              
              if( m_orientation == TabOrientation.Top )
                bounds.Offset( 0, rowHeight + 1 );
              else
                bounds.Offset( 0, -rowHeight );
              
              tmp.ButtonBounds = bounds;
            } // for( int index = 0; index < m_tabs.Count; ++index )
          
            x = 0;
            column = 0;
            row += 1;
            rowHeight = int.MinValue;
          } // if( ( x + width ) > this.Width )
          
          // set the tab's bounds
          if( m_orientation == TabOrientation.Top )
            tab.ButtonBounds =  new Rectangle( x, 0, width, height );
          else
            tab.ButtonBounds =  new Rectangle( x, this.Height - height - 1, width, height );

          tab.Row = row;
          tab.Column = column;
          
          x += width + 1;
          column++;
        } // foreach( TabPage tabpage in this.Controls )
      } // using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      // reposition the contained child controls
      this.SuspendLayout();
      
      Rectangle tabPageBounds;
      if( m_orientation == TabOrientation.Top )
        tabPageBounds = GetTabPageBoundsTop();
      else
        tabPageBounds = GetTabPageBoundsBottom();
      
      foreach( Control control in this.Controls )
        control.SetBounds( tabPageBounds.X, tabPageBounds.Y, tabPageBounds.Width, tabPageBounds.Height );
      
      this.ResumeLayout( true );

    } // LayoutTabsHorizontal()


    /// <summary>
    /// Lays out the tabs vertically
    /// </summary>
    private void LayoutTabsVertical()
    {
      using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      {
        int y = 0;
        int maxWidth = int.MinValue;
        int row = 0;
        int column = 0;

        foreach( TabPage tab in m_tabs )
        {
          // get the size of the tabpage's caption
          SizeF size = gfx.MeasureString( tab.Text, tab.Font );
          
          // calculate the required with and height for the tabpage
          int width = (int)( size.Height + ( 2 * TAB_PADDING ) + 0.5F );
          int height = (int)( size.Width + ( 2 * TAB_PADDING ) + 0.5F );

          // store the max height
          maxWidth = Math.Max( maxWidth, width );

          // is the tabpage's width past the control's right border
          if( ( y + height ) > this.Height )
          {
            // shift all the existing over
            for( int index = 0; index < m_tabs.IndexOf( tab ); ++index )
            {
              TabPage tmp = ( TabPage )m_tabs[ index ];

              Rectangle bounds = tmp.ButtonBounds;              
              
              if( m_orientation == TabOrientation.Left )
                bounds.Offset( maxWidth + 1, 0 );
              
              tmp.ButtonBounds = bounds;
            } // for( int index = 0; index < m_tabs.Count; ++index )
          
            y = 0;
            column = 0;
            row += 1;
            maxWidth = int.MinValue;
          } // if( ( x + width ) > this.Width )
          
          // add the TabPage to the collection          
          if( m_orientation == TabOrientation.Left )
            tab.ButtonBounds =  new Rectangle( 0, y, width, height );

          tab.Row = row;
          tab.Column = column;
          
          y += height + 1;
          column++;
        } // foreach( TabPage tabpage in this.Controls )
      } // using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      // reposition the contained child controls
      this.SuspendLayout();
      Rectangle tabPageBounds = GetTabPageBoundsLeft();
      
      foreach( Control control in this.Controls )
        control.SetBounds( tabPageBounds.X, tabPageBounds.Y, tabPageBounds.Width, tabPageBounds.Height );
      this.ResumeLayout( true );

    } // LayoutTabsVertical()

    #endregion

    #region methods

    /// <summary>
    /// Gets the size of the tabs
    /// </summary>
    /// <returns>the size of the tabs</returns>
    public Size GetTabsSize()
    {
      RectangleF bounds = new Rectangle();
      foreach( TabPage tab in m_tabs )
        bounds = RectangleF.Union( bounds, tab.ButtonBounds );
      return new Size( (int)(bounds.Width + 0.5F), (int)(bounds.Height + 0.5F) );
    } // GetTabsSize()


    /// <summary>
    /// Raises the BackColorChanged event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnBackColorChanged( EventArgs args )
    {
      base.OnBackColorChanged( args );
      
      foreach( Control child in this.Controls )
        child.BackColor = this.BackColor;
    } // OnBackColorChanged( EventArgs args )


    /// <summary>
    /// Raises the ControlAdded event.
    /// </summary>
    /// <param name="args">A ControlEventArgs that contains the event data.</param>
    protected override void OnControlAdded( ControlEventArgs args )
    {
      base.OnControlAdded( args );
      
      if( this.Controls.Count > 1 )
        args.Control.Visible = false;
      else
        m_selectedIndex = 0;
      
      if( args.Control.BackColor == SystemColors.Control )
        args.Control.BackColor = this.m_selectedColor;  

      m_tabs.Add( ( TabPage )args.Control );

      LayoutTabs();     
    } // OnControlAdded( ControlEventArgs args )


    /// <summary>
    /// Raises the ControlRemoved event.
    /// </summary>
    /// <param name="args">A ControlEventArgs that contains the event data.</param>
    protected override void OnControlRemoved( ControlEventArgs args )
    {
      base.OnControlRemoved( args );
      foreach( TabPage tab in m_tabs )
      {
        if( tab == args.Control )
        {
          m_tabs.Remove( tab );
          break;
        }
      }
    } // OnControlRemoved( ControlEventArgs args )

   
    /// <summary>
    /// Raises the MouseDown event.
    /// </summary>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );
      
      foreach( TabPage tab in m_tabs )
      {
        if( tab.ButtonBounds.Contains( new Point(  args.X, args.Y ) ) )
        {
          int newTabIndex =  m_tabs.IndexOf( tab );          
          
          if( newTabIndex != m_selectedIndex )
          {
            TabPage previousTab = null;
            if( m_selectedIndex != -1 )
              previousTab = m_tabs[ m_selectedIndex ];

            TabPage newTab = m_tabs[ newTabIndex ];

            TabEventArgs tabArgs = new TabEventArgs( previousTab, newTab );
            
            if( TabChanging != null )
              TabChanging( this, tabArgs );
          
            SetSelectedIndex( newTabIndex );

            if( TabChanged != null )
              TabChanged( this, tabArgs );
 
            Invalidate();
          }
          break;

        } // if( bounds.Contains( new PointF( ( float ) args.X, ( float )args.Y ) ) )
      } // foreach( RectangleF bounds in m_tabs )
    } // OnMouseDown( MouseEventArgs args )


    /// <summary>
    /// Called when the TabControl is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      LayoutTabs();
    } // OnResize( EventArgs args )

    
    /// <summary>
    /// Sets the index of the selected tab.
    /// </summary>
    /// <param name="index">Index of tab to select</param>
    private void SetSelectedIndex( int index )
    {
      // is the requested tab already selected?
      if( index == m_selectedIndex )
        return;

      // suspend all layout changes until we are done
      this.SuspendLayout();
      
      ((TabPage)m_tabs[ index ]).Visible = true;

      Application.DoEvents();

      if( m_selectedIndex != -1 )
        ((TabPage)m_tabs[ m_selectedIndex ]).Visible = false;

      this.ResumeLayout( true );

      m_selectedIndex = index; 

    } // SetSelectedIndex( int index )

    #endregion  
  
  } // class TabControl

  #endregion

  #region TabEventArgs
  
  /// <summary>
  /// the arguments for tab changing events
  /// </summary>
  public class TabEventArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// the previous tab
    /// </summary>
    TabPage m_previousTab = null;

    /// <summary>
    /// the new tab
    /// </summary>
    TabPage m_newTab = null;

    #endregion

    #region properties

    /// <summary>
    /// gets or sets the previous tab
    /// </summary>
    public TabPage PreviousTab
    {
      get { return m_previousTab; }
      set { m_previousTab = value; }
    } // PreviousTab

    /// <summary>
    /// gets or sets the new tab
    /// </summary>
    public TabPage NewTab
    {
      get { return m_newTab; }
      set { m_newTab = value; }
    } // NewTab

    #endregion

    #region initialization

    /// <summary>
    /// Initializes a TabEventArgs instance with default values.
    /// </summary>
    public TabEventArgs()
    {
    } // TabEventArgs()


    /// <summary>
    /// Initializes a TabEventArgs instance with the specified previous and new tabs.
    /// </summary>
    /// <param name="previousTab">the previous tab</param>
    /// <param name="newTab">the new tab</param>
    public TabEventArgs( TabPage previousTab, TabPage newTab )
    {
      m_previousTab = previousTab;
      m_newTab = newTab;
    } // TabEventArgs( TabPage previousTab, TabPage newTab )

    #endregion

  } // class TabEventArgs
  
  #endregion

  #region TabEventHandler

  /// <summary>
  /// defines a delegate for tab event handling
  /// </summary>
  public delegate void TabEventHandler( object sender, TabEventArgs args );

  #endregion

  #region TabOrientation
  
  /// <summary>
  /// Specifies the location of the tabs within a TabControl
  /// </summary>
  public enum TabOrientation
  {
    Top,
    Bottom,
    Left
  } // enum TabOrientation

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: TabControl.cs,v $
// Revision 1.7  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.6.1  2005/03/02 14:54:48  frank
// code review
//
// Revision 1.6  2004/07/09 18:34:24  geconomos
// Cleaned up drawing code and fixed tab ordering bug.
//
// Revision 1.5  2004/05/20 13:01:19  geconomos
// Updated drawing of tab area to use the BackColor when rendering a tab control without any child tabs.
//
// Revision 1.4  2004/05/17 13:56:56  geconomos
// Started coding conventions.
//
// Revision 1.3  2004/05/13 13:32:31  geconomos
// Initial code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TabControl.cs,v 1.7 2005/03/08 19:11:06 frank Exp $
// $Id: TabControl.cs,v 1.7 2005/03/08 19:11:06 frank Exp $

#endregion