// $Id: TabStrip.cs,v 1.12 2007/05/29 19:50:53 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Data;
using System.Reflection;
using System.Windows.Forms;

namespace Viatronix.UI
{


  #region TabStrip

	/// <summary>
	/// a strip of tabs
	/// </summary>
  public class TabStrip : System.Windows.Forms.Control, ISupportInitialize
  {
    #region delegates
    /// <summary>
    /// Selected Tab Changed Delegate
    /// </summary>
    public delegate void SelectedTabChangedHandler  ( object obj, TabStripEventArgs args );

    /// <summary>
    /// Selected Tab Changing Delegate
    /// </summary>
    public delegate void SelectedTabChangingHandler ( object obj, TabStripEventArgs args );

    #endregion
    
    #region TabStripItemCollection

    /// <summary>
    /// a collection of tab strip items
    /// </summary>
    public class TabStripItemCollection : CollectionBase
    {

      #region fields

      /// <summary>
      /// the tab strip
      /// </summary>
      TabStrip m_tabstrip;
    
      #endregion

      #region initialization

      /// <summary>
      /// Initializes an instance of TabStripItemCollection
      /// </summary>
      /// <param name="tabstrip"></param>
      public TabStripItemCollection( TabStrip tabstrip )
      {
        m_tabstrip = tabstrip;
      } // TabStripItemCollection( TabStrip tabstrip )
      
      #endregion
    
      #region methods

      /// <summary>
      /// add an item to a tab strip
      /// </summary>
      /// <param name="item">the item to add</param>
      /// <returns>The position into which the new element was inserted.</returns>
      public int Add( TabStripItem item )
      {
        return List.Add( item );
      } // Add( TabStripItem item )

      
      /// <summary>
      /// removes the first occurrence of a specific object from the collection
      /// </summary>
      /// <param name="item">the item to remove</param>
      public void Remove( TabStripItem item )
      {
        List.Remove( item );
      } // Remove( TabStripItem item )


      /// <summary>
      /// inserts an item into the collection at the specified index
      /// </summary>
      /// <param name="index">the index of insertion</param>
      /// <param name="item">the item to be inserted</param>
      public void Insert( int index, TabStripItem item )
      {
        this.List.Insert( index, item );
      } // Insert( index, item )

      
      /// <summary>
      /// indexer for the tab strip item collection
      /// </summary>
      public TabStripItem this[ int index ]
      {
        get { return (TabStripItem)List[ index ]; }
        set { List[ index ] = value; }
      } // this[ int index ]


      /// <summary>
      /// indexer for the tab strip item collection
      /// </summary>
      public TabStripItem this[ string id ]
      {
        get 
        { 
          foreach( TabStripItem item in this.List )
          {
            if( item.ID == id )
              return item;
          }

          return null;                                      
        }
      } // Indexer


      /// <summary>
      /// Searches for the specified object and returns the index of the
      /// first occurrence within the current one-dimensional instance.
      /// </summary>
      /// <param name="item">the item to search for</param>
      /// <returns>the index if found, otherwise -1</returns>
      public int IndexOf( TabStripItem item )
      {
        return List.IndexOf( item );
      } // IndexOf( TabStripItem item )


      /// <summary>
      /// Determines if the collection contains the specified item.
      /// </summary>
      /// <param name="item">The item being check for</param>
      /// <returns>true if the collection contains the item; otherwise false</returns>
      public bool Contains( TabStripItem item )
      {
        return this.List.Contains( item );
      } // Contains( item )

      
      /// <summary>
      /// Performs additional custom processes after inserting a new element into the CollectionBase instance.
      /// </summary>
      /// <param name="index">The zero-based index at which to insert value.</param>
      /// <param name="value">The new value of the element at index.</param>
      protected override void OnInsertComplete( int index, object value )
      {
        base.OnInsertComplete( index, value );

        TabStripItem item = ( TabStripItem ) value;
        item.EnabledChanged += m_tabstrip.m_enabledChangedHandler;
      } // OnInsertComplete( int index, object value )

      #endregion

    } // class TabStripItemCollection

    #endregion

    #region fields

    /// <summary>
    /// orientation of the tab strip
    /// </summary>
    private TabStripOrientation m_orientation = TabStripOrientation.Bottom;

    /// <summary>
    /// the items in the tab strip
    /// </summary>
    private TabStripItemCollection m_items;

    /// <summary>
    /// the components
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// the selected index, -1 indicates no selection
    /// </summary>
    private TabStripItem m_selectedItem = null;

    /// <summary>
    /// cached event handler for changed event
    /// </summary>
    private EventHandler m_enabledChangedHandler;

    #endregion

    #region properties

    /// <summary>
    /// gets and sets the orientation of the tab strip
    /// </summary>
    public TabStripOrientation Orientation
    {
      get { return m_orientation; }
      set { m_orientation = value; Update(); }
    } // Orientation

    
    /// <summary>
    /// gets and sets the selected index
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public int SelectedIndex
    {
      get { return this.Items.IndexOf( m_selectedItem ); }
      set
      {
        if( value < 0 || value > m_items.Count )
          throw new ArgumentException();
        
        this.SelectedItem = this.Items[ value ];
      }
    } // SelectedIndex
    
   
    /// <summary>
    /// gets and sets the selected item
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Hidden ) ]
    public TabStripItem SelectedItem
    {
      get { return m_selectedItem; }
      set 
      {  
        if( !this.Items.Contains( value ))
          throw new ArgumentException( "The specified TabStripItem is not contained within the TabStrip's collection." );

        TabStripEventArgs tabStripArgs     = new TabStripEventArgs();
        tabStripArgs.LastSelectedTabIndex  = this.SelectedIndex;
        tabStripArgs.NewlySelectedTabIndex = this.Items.IndexOf(value);

        if( SelectedTabChanging != null )
          SelectedTabChanging( this, tabStripArgs );

        // changing of tabs cancelled
        if( tabStripArgs.Cancel )
          return;

        m_selectedItem = value;

        if( SelectedTabChanged != null )
          SelectedTabChanged( this, tabStripArgs );

        // reset the old tab
        if( tabStripArgs.Cancel )
          m_selectedItem = this.Items[ tabStripArgs.LastSelectedTabIndex ];

        Invalidate();
      }
    } // SelectedItem
   
    
    /// <summary>
    /// gets and sets the items in the tab strip
    /// </summary>
    public TabStripItemCollection Items
    {
      get { return m_items; }
      set { m_items = value; }
    } // Items
    
    #endregion

    #region events

    /// <summary>
    /// event when the selected tab is changing
    /// </summary>
    public event SelectedTabChangingHandler SelectedTabChanging;

    /// <summary>
    /// event when the selected tab is changed
    /// </summary>
    public event SelectedTabChangedHandler SelectedTabChanged;

   
    #endregion

    #region initialization

    /// <summary>
    /// initializes a TabStriip instance
    /// </summary>
    public TabStrip()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      m_items = new TabStripItemCollection( this );

      m_enabledChangedHandler = new EventHandler( this.OnEnabledChangedHandler );

			InitializeComponent();
		} // TabStrip()

		
    /// <summary>
    /// Disposes the current instance
    /// </summary>
    /// <param name="disposing">True to dispose of managed as well</param>
    protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		} // Dispose( bool disposing )

    #endregion

    #region methods

    /// <summary>
    /// Called when the TabControl is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      Invalidate();
    } // OnResize( EventArgs args )


    /// <summary>
    /// Event handler for the Paint event.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;
      if( this.DesignMode )
      {
        using( Pen pen = new Pen( Color.Black ) )
        {
          pen.DashStyle = DashStyle.Dash;
          gfx.DrawRectangle( pen, 0, 0, this.Width - 1, this.Height - 1 );
        } 
      }
      else
      {
        for( int index = m_items.Count - 1; index >= 0; --index )
          DrawTabItem( gfx,  index );
      }
    } // OnPaint( PaintEventArgs args )


    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      Point pos = new Point( args.X, args.Y );
      if( m_orientation == TabStripOrientation.Left )
      {
        Point tmp = new Point( pos.X, pos.Y );
        pos.X = tmp.Y;
        pos.Y = tmp.X;
      }

      foreach( TabStripItem item in m_items )
      {
        if( item.Enabled && item.Bounds.Contains( pos ) )
        {
          if( item != m_selectedItem )
          {
            this.SelectedItem = item;
            break;
          }
        }
      }
    } // OnMouseDown( MouseEventArgs args )


    /// <summary>
    /// Draw a tab strip item
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="index">the index of the item to draw</param>
    private void DrawTabItem( Graphics gfx, int index )
    {

      TextRenderingHint hint = gfx.TextRenderingHint;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      Matrix matrix = gfx.Transform;
      try
      {
        if( m_orientation == TabStripOrientation.Left )
        {
          gfx.TranslateTransform( this.Width, 0 );
          gfx.RotateTransform( 90 );
        }

        TabStripItem item = ( TabStripItem ) m_items[ index ];

        Bitmap tabLeftImage;
        Bitmap tabMiddleImage;
        Bitmap tabRightImage;
            
        if( item == m_selectedItem )
        {
          if( m_orientation == TabStripOrientation.Left )
          {
            tabLeftImage   = index == 0? TabStripImages.TabSideActiveEnd : TabStripImages.TabSideActiveLeft;
            tabMiddleImage = TabStripImages.TabSideActiveMiddle;
            tabRightImage  = TabStripImages.TabSideActiveRight;
          }
          else
          {
            tabLeftImage   = index == 0? TabStripImages.TabActiveEnd : TabStripImages.TabActiveLeft;
            tabMiddleImage = TabStripImages.TabActiveMiddle;
            tabRightImage  = TabStripImages.TabActiveRight;
          }
        }
        else
        {
          if( m_orientation == TabStripOrientation.Left )
          {
            tabLeftImage   = index == 0? TabStripImages.TabSideInactiveEnd : TabStripImages.TabSideInactiveLeft;
            tabMiddleImage = TabStripImages.TabSideInactiveMiddle;
            tabRightImage  = TabStripImages.TabSideInactiveRight;
          }
          else
          {
            tabLeftImage   = index == 0? TabStripImages.TabInactiveEnd : TabStripImages.TabInactiveLeft;
            tabMiddleImage = TabStripImages.TabInactiveMiddle;
            tabRightImage  = TabStripImages.TabInactiveRight;
          }
        }
        
        if( item.Enabled )
          gfx.DrawImage( tabLeftImage, item.Bounds.X, item.Bounds.Y, tabLeftImage.Width, tabLeftImage.Height );
        else
          Utilities.DrawImageTransparent( gfx, tabLeftImage, item.Bounds.X, item.Bounds.Y, 0.5F );

        int i = item.Bounds.X + tabLeftImage.Width;
        for( ; i < item.Bounds.Right - tabRightImage.Width; i += tabMiddleImage.Width )
        {
          if( item.Enabled )
            gfx.DrawImage( tabMiddleImage, i, item.Bounds.Y, tabMiddleImage.Width, tabMiddleImage.Height );
          else
            Utilities.DrawImageTransparent( gfx, tabMiddleImage, i, item.Bounds.Y, 0.5F );
        }


        if( item.Enabled )
          gfx.DrawImage( tabRightImage, i, item.Bounds.Y, tabRightImage.Width, tabRightImage.Height );
        else
          Utilities.DrawImageTransparent( gfx, tabRightImage, i, item.Bounds.Y, 0.5F );

        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;
        
        RectangleF layout = item.Bounds;
        layout.Height = tabMiddleImage.Height - 4;
         
        
        if( item.Enabled )
        {
          if( item == m_selectedItem )
            gfx.DrawString( item.Text, Theme.TabSelectedFont,  Theme.TabSelectedBrush, layout, format );
          else
            gfx.DrawString( item.Text, Theme.TabFont,  Theme.TabBrush, layout, format );
        }
        else
        {
          if( item == m_selectedItem )
          {
            using( SolidBrush brush = new SolidBrush( Color.FromArgb( 130, Theme.TabSelectedBrush.Color ) ) )
              gfx.DrawString( item.Text, Theme.TabSelectedFont, brush, layout, format );
          }
          else
          {
            using( SolidBrush brush = new SolidBrush( Color.FromArgb( 100, Theme.TabBrush.Color ) ) )
              gfx.DrawString( item.Text, Theme.TabFont, brush, layout, format );
          }
        }
      } // try
      finally
      {
        gfx.TextRenderingHint = hint;
        gfx.Transform = matrix;
      } // finally

    } // DrawTabItem( Graphics gfx, int index )


    /// <summary>
    /// Update the tabs
    /// </summary>
    private void UpdateTabs()
    {
      using( Graphics gfx = Graphics.FromHwnd( IntPtr.Zero ) )
      {
        int tabRightPadding = TabStripImages.TabInactiveRight.Width;
        
        int x = 0;
        int y = 0;
        int height = TabStripImages.TabInactiveLeft.Height;
        for( int index = 0; index < m_items.Count; ++index )
        {
          int tabLeftPadding = index == 0? TabStripImages.TabInactiveEnd.Width : TabStripImages.TabInactiveLeft.Width;

          TabStripItem item = ( TabStripItem ) m_items[ index ];
          
          SizeF size = gfx.MeasureString( item.Text, Theme.TabFont );
          
          int nextMultipleOf = TabStripImages.TabInactiveMiddle.Width;

          int result  = Utilities.GetNextMultiple( (int)size.Width, nextMultipleOf );

          int width = result + tabLeftPadding + tabRightPadding;

          width = Math.Max( Utilities.GetNextMultiple( 35, nextMultipleOf ), width );

          item.Bounds = new Rectangle( x, y,  width, height );

          x += width - 13;
        } // for( int index = 0; index < m_items.Count; ++index )
      } // using( Graphics gfx = Graphics.FromHwnd( IntPtr.Zero ) )
    } // UpdateTabs()


    /// <summary>
    /// Handler for the enabled changed event
    /// </summary>
    /// <param name="sender">the sender of this event</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnEnabledChangedHandler( object sender, EventArgs args )
    {
      Invalidate();
    } // OnEnabledChangedHandler( object sender, EventArgs args )
    
    #endregion

    #region ISupportInitialize

    /// <summary>
    /// begin initialization
    /// </summary>
    public void BeginInit()
    {
    } // BeginInit()

    
    /// <summary>
    /// end initialization
    /// </summary>
    public void EndInit()
    {
      UpdateTabs();
      Invalidate();
    } // EndInit()

    #endregion

		#region component designer generated code
		
    	/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			components = new System.ComponentModel.Container();
		} // InitializeComponent()
		
    #endregion

  } // class TabStrip
  
  #endregion

  #region TabStripOrientation

  /// <summary>
  /// the possible orientations of the tab strip
  /// </summary>
  public enum TabStripOrientation
  {
    Left,
    Bottom
  } // enum TabStripOrientation

  #endregion

  #region TabStripItem

  /// <summary>
  /// a tab strip item
  /// </summary>
  public class TabStripItem
  {

    #region fields

    /// <summary>
    /// the text of this item
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// the id of the this item
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// the data of this item
    /// </summary>
    private object m_data = null;

    /// <summary>
    /// if this item is enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// the bounds of this item
    /// </summary>
    private Rectangle m_bounds = new Rectangle();

    #endregion

    #region properties

    /// <summary>
    /// gets and sets the id of this item
    /// </summary>
    public string ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID


    /// <summary>
    /// gets and sets the text of this item
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text

    
    /// <summary>
    /// gets and sets the data of this item
    /// </summary>
    public object Data
    {
      get { return m_data; }
      set { m_data = value; }
    } // Data


    /// <summary>
    /// gets and sets the bounds of this item
    /// </summary>
    public Rectangle Bounds
    {
      get { return m_bounds; }
      set { m_bounds = value; }
    } // Bounds

    
    /// <summary>
    /// gets and sets the enabled proprty of this item
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set 
      { 
        m_enabled = value; 
        if( EnabledChanged != null )
        {
          EnabledChanged( this, EventArgs.Empty );
        }
      }
    } // Enabled
    
    #endregion

    #region events

    /// <summary>
    /// the enabled changed event
    /// </summary>
    public event EventHandler EnabledChanged;

    #endregion

    #region initialization

    /// <summary>
    /// initializes a TabStripItem instance
    /// </summary>
    public TabStripItem()
    {
    } // TabStripItem()


    /// <summary>
    /// initializes a TabStripItem instance with the specified id
    /// </summary>
    /// <param name="id">the id</param>
    public TabStripItem( string id )
    {
      m_id = id;
    } // TabStripItem( id )

    
    /// <summary>
    /// initializes a TabStripItem instance with the specified id and text
    /// </summary>
    /// <param name="id">the id</param>
    /// <param name="text"></param>
    public TabStripItem( string id, string text )
    {
      m_id = id;
      m_text = text;
    } // TabStripItem( id, text )

    
    /// <summary>
    /// initializes a TabStripItem instance with the specified text and data
    /// </summary>
    /// <param name="id">the id</param>
    /// <param name="text">the text</param>
    /// <param name="data">the data</param>
    public TabStripItem( string id, string text, object data )
    {
      m_id = id;
      m_text = text;
      m_data = data;
    } // TabStripItem( id, text, data )
    
    #endregion

  } // class TabStripItem

  #endregion

  #region TabStripImages

  /// <summary>
  /// the images in the tab strip
  /// </summary>
  internal class TabStripImages
  {

    #region fields

    ///
    /// active tab
    ///

    /// <summary>
    /// tab active end
    /// </summary>
    private static System.Drawing.Bitmap m_tabActiveEnd;

    /// <summary>
    /// tab active left
    /// </summary>
    private static System.Drawing.Bitmap m_tabActiveLeft;

    /// <summary>
    /// tab active middle
    /// </summary>
    private static System.Drawing.Bitmap m_tabActiveMiddle;

    /// <summary>
    /// tab active right
    /// </summary>
    private static System.Drawing.Bitmap m_tabActiveRight;

    /// <summary>
    /// tab side active end
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideActiveEnd;

    /// <summary>
    /// tab side active left
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideActiveLeft;

    /// <summary>
    /// tab side active middle
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideActiveMiddle;

    /// <summary>
    /// tab side active right
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideActiveRight;

    ///
    /// inactive tab
    ///

    /// <summary>
    /// tab inactive end
    /// </summary>
    private static System.Drawing.Bitmap m_tabInactiveEnd;

    /// <summary>
    /// tab inactive left
    /// </summary>
    private static System.Drawing.Bitmap m_tabInactiveLeft;
    
    /// <summary>
    /// tab inactive middle
    /// </summary>
    private static System.Drawing.Bitmap m_tabInactiveMiddle;
    
    /// <summary>
    /// tab inactive right
    /// </summary>
    private static System.Drawing.Bitmap m_tabInactiveRight;

    /// <summary>
    /// tab side inactive end
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideInactiveEnd;

    /// <summary>
    /// tab side inactive left
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideInactiveLeft;

    /// <summary>
    /// tab side inactive middle
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideInactiveMiddle;

    /// <summary>
    /// tab side inactive right
    /// </summary>
    private static System.Drawing.Bitmap m_tabSideInactiveRight;

    #endregion

    #region properties

    /// <summary>
    /// gets and sets the tab inactive end
    /// </summary>
    static public Bitmap TabInactiveEnd
    {
      get { return m_tabInactiveEnd; }
      set { m_tabInactiveEnd = value; }
    } // TabInactiveEnd


    /// <summary>
    /// gets and sets the tab inactive left
    /// </summary>
    static public Bitmap TabInactiveLeft
    {
      get { return m_tabInactiveLeft; }
      set { m_tabInactiveLeft = value; }
    } // TabInactiveLeft


    /// <summary>
    /// gets and sets the tab inactive middle
    /// </summary>
    static public Bitmap TabInactiveMiddle
    {
      get { return m_tabInactiveMiddle; }
      set { m_tabInactiveMiddle = value; }
    } // TabInactiveMiddle


    /// <summary>
    /// gets and sets the tab inactive right
    /// </summary>
    static public Bitmap TabInactiveRight
    {
      get { return m_tabInactiveRight; }
      set { m_tabInactiveRight = value; }
    } // TabInactiveRight


    /// <summary>
    /// gets and sets the tab active end
    /// </summary>
    static public Bitmap TabActiveEnd
    {
      get { return m_tabActiveEnd; }
      set { m_tabActiveEnd = value; }
    } // TabActiveEnd


    /// <summary>
    /// gets and sets the tab active left
    /// </summary>
    static public Bitmap TabActiveLeft
    {
      get { return m_tabActiveLeft; }
      set { m_tabActiveLeft = value; }
    } // TabActiveLeft


    /// <summary>
    /// gets and sets the tab active middle
    /// </summary>
    static public Bitmap TabActiveMiddle
    {
      get { return m_tabActiveMiddle; }
      set { m_tabActiveMiddle = value; }
    } // TabActiveMiddle


    /// <summary>
    /// gets and sets the tab active right
    /// </summary>
    static public Bitmap TabActiveRight
    {
      get { return m_tabActiveRight; }
      set { m_tabActiveRight = value; }
    } // TabActiveRight

 
    /// <summary>
    /// gets and sets the tab side inactive end
    /// </summary>
    static public Bitmap TabSideInactiveEnd
    {
      get { return m_tabSideInactiveEnd; }
      set { m_tabSideInactiveEnd = value; }
    } // TabSideInactiveEnd


    /// <summary>
    /// gets and sets the tab side inactive left
    /// </summary>
    static public Bitmap TabSideInactiveLeft
    {
      get { return m_tabSideInactiveLeft; }
      set { m_tabSideInactiveLeft = value; }
    } // TabSideInactiveLeft
    

    /// <summary>
    /// gets and sets the tab side inactive middle
    /// </summary>
    static public Bitmap TabSideInactiveMiddle
    {
      get { return m_tabSideInactiveMiddle; }
      set { m_tabSideInactiveMiddle = value; }
    } // TabSideInactiveMiddle

    
    /// <summary>
    /// gets and sets the tab side inactive right
    /// </summary>
    static public Bitmap TabSideInactiveRight
    {
      get { return m_tabSideInactiveRight; }
      set { m_tabSideInactiveRight = value; }
    } // TabSideInactiveRight

    
    /// <summary>
    /// gets and sets the tab side active end
    /// </summary>
    static public Bitmap TabSideActiveEnd
    {
      get { return m_tabSideActiveEnd; }
      set { m_tabSideActiveEnd = value; }
    } // TabSideActiveEnd

    
    /// <summary>
    /// gets and sets the tab side active left
    /// </summary>
    static public Bitmap TabSideActiveLeft
    {
      get { return m_tabSideActiveLeft; }
      set { m_tabSideActiveLeft = value; }
    } // TabSideActiveLeft
    
    
    /// <summary>
    /// gets and sets the tab side active middle
    /// </summary>
    static public Bitmap TabSideActiveMiddle
    {
      get { return m_tabSideActiveMiddle; }
      set { m_tabSideActiveMiddle = value; }
    } // TabSideActiveMiddle

    
    /// <summary>
    /// gets and sets the tab side active right
    /// </summary>
    static public Bitmap TabSideActiveRight
    {
      get { return m_tabSideActiveRight; }
      set { m_tabSideActiveRight = value; }
    } // TabSideActiveRight
    
    #endregion

    #region initialization

    /// <summary>
    /// static initializer
    /// </summary>
    static TabStripImages()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      m_tabInactiveEnd = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-inactive-end.png") );
      m_tabInactiveLeft = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-inactive-left.png") );
      m_tabInactiveMiddle = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-inactive-middle.png") );
      m_tabInactiveRight = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-inactive-right.png") );
      m_tabActiveEnd = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-active-end.png") );
      m_tabActiveLeft = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-active-left.png") );
      m_tabActiveMiddle = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-active-middle.png") );
      m_tabActiveRight = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.tab-active-right.png") );
      
      m_tabSideInactiveEnd = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-inactive-end.png") );
      m_tabSideInactiveLeft = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-inactive-left.png") );
      m_tabSideInactiveMiddle = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-inactive-middle.png") );
      m_tabSideInactiveRight = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-inactive-right.png") );
      m_tabSideActiveEnd = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-active-end.png") );
      m_tabSideActiveLeft = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-active-left.png") );
      m_tabSideActiveMiddle = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-active-middle.png") );
      m_tabSideActiveRight = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.side-tab-active-right.png") );
    } // TabStripImages()

    #endregion

  } // class TabStripImages

  #endregion

  #region TabStripEventArgs class
  
  /// <summary>
  /// Tab Strip Event args Holds the indexes of old and new tab indexes during a tab change 
  /// </summary>
  public class TabStripEventArgs : System.ComponentModel.CancelEventArgs
  {

    #region fields

    /// <summary>
    /// Current tab index
    /// </summary>
    private int m_lastSelectedTabIndex = -1;

    /// <summary>
    /// Newly selected tab index
    /// </summary>
    private int m_newlySelectedTabIndex = -1;

    #endregion

    #region construction

    //Constructor    
    public TabStripEventArgs ()
    {
    }//TabStripEventArgs

    #endregion

    #region properties

    /// <summary>
    ///  Gets or Sets the current tab index
    /// </summary>
    public int LastSelectedTabIndex
    {
      get { return m_lastSelectedTabIndex; }
      set { m_lastSelectedTabIndex = value; }
    }//CurrentTabIndex

    /// <summary>
    /// Gets or Sets the newly selected tab index
    /// </summary>
    public int NewlySelectedTabIndex
    {
      get { return m_newlySelectedTabIndex; }
      set { m_newlySelectedTabIndex = value; }
    }//NextSelectedTabIndex

    #endregion

  }//class TabStripEventArgs : System.EventArgs


  #endregion

} // namespace Viatronix.Viewer

#region revision history

// $Log: TabStrip.cs,v $
// Revision 1.12  2007/05/29 19:50:53  gdavidson
// Modified the SelectedItem property
//
// Revision 1.11  2006/10/30 16:29:13  gdavidson
// Chnaged TabStripEventArgs to extend CancelEventArgs
//
// Revision 1.10  2005/12/14 20:08:36  gdavidson
// Added a Cancel property to the TabStripEventArgs
//
// Revision 1.9  2005/12/12 21:07:55  romy
// TabStrip Event arg modified
//
// Revision 1.8  2005/06/22 15:09:22  gdavidson
// Assign the id in the constructor.
//
// Revision 1.7  2005/05/05 17:39:08  gdavidson
// Added SelectedTabChanging event.
//
// Revision 1.6  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/03/02 13:49:50  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TabStrip.cs,v 1.12 2007/05/29 19:50:53 gdavidson Exp $
// $Id: TabStrip.cs,v 1.12 2007/05/29 19:50:53 gdavidson Exp $

#endregion
