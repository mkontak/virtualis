// $Id: Rollout.cs,v 1.27.8.1 2009/02/25 14:24:49 kchalupa Exp $
//
// Copyright (c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


using System;
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Data;
using System.Reflection;
using System.Windows.Forms;
using System.Windows.Forms.Design;


namespace Viatronix.UI 
{  

  #region Rollout class

  /// <summary>
  /// Provides a tab-like control with collapsable panes.
  /// </summary>
  [ Designer( typeof( RolloutDesigner ) ) ]
  public class Rollout : System.Windows.Forms.ContainerControl, System.ComponentModel.ISupportInitialize
  {

    #region constants

    /// <summary>
    /// Spacing between panes in pixels.
    /// </summary>
    static internal readonly int m_spaceBetweenPanes = 4;    
    
    #endregion

    #region fields

    /// <summary>
    /// collection of panes
    /// </summary>
    private RolloutPaneCollection m_panes = new RolloutPaneCollection();

    /// <summary>
    /// Manages the expanded panes
    /// </summary>
    private ExpandedPaneManager m_expandedPanes = null;

    /// <summary>
    /// indicates if the rollout is being initialized
    /// </summary>
    private bool m_initializing = false;

    #endregion

    #region construction

    /// <summary>
    /// Initializes a Rollout object with default values.
    /// </summary>
    public Rollout()
    {
      m_expandedPanes = new ExpandedPaneManager( this );

      // enable double buffering
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
    } // Rollout()

    #endregion

    #region properties
    
    /// <summary>
    /// Gets the panes collection
    /// </summary>
    public RolloutPaneCollection Panes
    {
      get { return m_panes; }
    }


    /// <summary>
    /// Gets the expanded panes collection
    /// </summary>
    public ExpandedPaneManager ExpandedPanes
    {
      get { return m_expandedPanes; }
    } // ExpandedPanes

    
    #endregion 

    #region events

    /// <summary>
    /// raised when the pane is about to be expanded
    /// </summary>
    public event RolloutEventHandler PaneExpanding;

    /// <summary>
    /// raised after the panes has been expanded
    /// </summary>
    public event RolloutEventHandler PaneExpanded;


    /// <summary>
    /// raised when the pane is about to be collapsed
    /// </summary>
    public event RolloutEventHandler PaneCollapsing;

    /// <summary>
    /// raised after the panes has been collapsed
    /// </summary>
    public event RolloutEventHandler PaneCollapsed;

    #endregion

    #region methods
   
    /// <summary>
    /// Adds a pane to the rollout control using the specified object.
    /// </summary>
    /// <param name="child">Child object that becomes a child object of the new pane.</param>
    /// <returns>RolloutPane instance.</returns>
    public RolloutPane AddPane( Control child )
    {
      RolloutPane pane = CreatePane( child.Text );

      pane.SetChild( child );
      
      //      ArrangePanes();

      return pane;
    } // AddPane( child )

    
    /// <summary>
    /// Adds a pane with the specified text.
    /// </summary>
    /// <param name="text">Pane's text</param>
    /// <returns>RolloutPane instance.</returns>
    public RolloutPane AddPane( string text )
    {
      RolloutPane pane = CreatePane( text );
   
      //      ArrangePanes();

      return pane;
    } // AddPane( string text )


    /// <summary>
    /// Expands the specified pane.
    /// </summary>
    /// <param name="pane">The RolloutPane that should be expanded.</param>
    public void ExpandPane( RolloutPane pane )
    {
      if( this.Visible )
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );

      OnPaneExpanding( pane );

      if( this.Visible )
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );

      this.ExpandedPanes.Add( pane );
      ArrangePanes();

      OnPaneExpanded( pane );
    } // ExpandPane( RolloutPane pane )


    /// <summary>
    /// Collapses the specified pane.
    /// </summary>
    /// <param name="pane">The RolloutPane that should be collapsed.</param>
    public void CollapsePane( RolloutPane pane )
    {
      if( this.Visible )
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );

      OnPaneCollapsing( pane );

      if( this.Visible )
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );

      this.ExpandedPanes.Remove( pane );
      ArrangePanes();

      OnPaneCollapsed( pane );
    } // CollapsePane( pane )


    /// <summary>
    /// Arranges the panes contained in the Rollouts collection.
    /// </summary>
    /// <remarks>Uses UpdatePanes for primary implementation but places the call within a disabled redraw block.</remarks>
    public void ArrangePanes()
    {
      if( m_initializing )
        return;

      try
      {
        if( this.Visible )
          Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );

        UpdatePanes();

      }
      finally
      {  
        if( this.Visible )
          Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
      }
    } // ArrangePanes()


    /// <summary>
    /// Updates the panes contained in the Rollouts collection.
    /// </summary>
    public void UpdatePanes()
    {
      if( m_initializing )
        return;

      try
      {    
        SuspendLayout();
      
        int ypos = m_spaceBetweenPanes;
        int width = this.Width - 2;
        foreach( RolloutPane pane in m_panes )
        {
          if( !pane.Visible )
            continue;

          pane.Location = new Point( 1, ypos );

          int height = ( this.ExpandedPanes.Contains( pane )) ? pane.ExpandedHeight : pane.CollapsedHeight;
          pane.Size = new Size( width, height );

          ypos += height + m_spaceBetweenPanes;
        }       
      }
      finally
      {
        ResumeLayout( true );
      }
    } // UpdatePanes()


    /// <summary>
    /// Creates a pane with the specified text.
    /// </summary>
    /// <param name="text">Text for pane's caption</param>
    /// <returns>RolloutPane instance</returns>
    private RolloutPane CreatePane( string text )
    {
      // create the new pane 
      RolloutPane pane = new RolloutPane();      
      pane.Text = text;
      
      // add the pane to the rollout
      this.Controls.Add( pane );

      pane.SetExpanded( false );

      return pane;
    }


    /// <summary>
    /// Sets the number of expaned panes.
    /// </summary>
    /// <param name="count">The number of expanded panes.</param>
    protected void SetPaneCount( int count )
    {
      this.ExpandedPanes.PaneCount = count;
    } // SetPaneCount( count )

    #endregion

    #region override methods
   
   
    /// <summary>
    /// Called when the control needs repainting.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {      
      Graphics gfx = args.Graphics;

      // are we in design mode?
      if( this.DesignMode )
      {
        // draw a dashed frame aroundthe entire control
        using( Pen pen = new Pen( Color.Black ) )
        {
          pen.DashStyle = DashStyle.Dash;
          gfx.DrawRectangle( pen, 0, 0, this.Width - 1, this.Height - 1 );
        } // using( Pen pen = new Pen( Color.Black ) )
      } // if( this.DesignMode )
    
    } // OnPaint( args ) )

    
    /// <summary>
    /// Called when a pane is about to be expanded
    /// </summary>
    /// <param name="pane">Pane that is to be expanded</param>
    protected virtual void OnPaneExpanding( RolloutPane pane )
    {
      if( PaneExpanding != null )
        PaneExpanding( this, new RolloutEventArgs( pane ) );
    } // OnPaneExpanding( RolloutPane pane )
   
    
    /// <summary>
    /// Called after a pane is expanded
    /// </summary>
    /// <param name="pane">Pane that is to be expanded</param>
    protected virtual void OnPaneExpanded( RolloutPane pane )
    {
      if( PaneExpanded != null )
        PaneExpanded( this, new RolloutEventArgs( pane ) );
    } // OnPaneExpanded( RolloutPane pane )


    /// <summary>
    /// Called when a pane is about to be collapsed
    /// </summary>
    /// <param name="pane">Pane that is to be collapsed</param>
    protected virtual void OnPaneCollapsing( RolloutPane pane )
    {
      if( PaneCollapsing != null )
        PaneCollapsing( this, new RolloutEventArgs( pane ) );
    } // OnPaneCollapsing( RolloutPane pane )
   
    
    /// <summary>
    /// Called after a pane is collapsed
    /// </summary>
    /// <param name="pane">Pane that is to be collapsed</param>
    protected virtual void OnPaneCollapsed( RolloutPane pane )
    {
      if( PaneCollapsed != null )
        PaneCollapsed( this, new RolloutEventArgs( pane ) );
    } // OnPaneCollapsed( RolloutPane pane )

    
    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data releated to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      if( !m_initializing )
      {
        this.ExpandedPanes.Refresh();
        this.UpdatePanes();

        if( this.Visible )
          Invalidate();
      }
    } // OnResize( args )


    /// <summary>
    /// Called when a pane is added to the rollout.
    /// </summary>
    /// <param name="args"></param>
    protected override void OnControlAdded( ControlEventArgs args )
    {
      base.OnControlAdded( args );
      m_panes.Add( ( RolloutPane ) args.Control );
    } // OnControlAdded( ControlEventArgs args )

   
    /// <summary>
    /// Called when a pane is removed from the rollout.
    /// </summary>
    /// <param name="args"></param>
    protected override void OnControlRemoved( ControlEventArgs args )
    {     
      base.OnControlRemoved( args );
      
      RolloutPane pane = (RolloutPane) args.Control;
      m_expandedPanes.Remove( pane );
      m_panes.Remove( pane );
      
      if( !m_initializing )
      {
        ArrangePanes();
        Refresh();
      }
    } // OnControlRemoved( args )


    /// <summary>
    /// Called when the visibility flag change for the rollout
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event</param>
    protected override void OnVisibleChanged( EventArgs args )
    {
      uint redrawOn = ( this.Visible ) ? (uint)1 : 0;
      Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, redrawOn, 0 );

      foreach( Control control in this.Controls )
      {
        if( control.Visible )
          Win32.User32.SendMessage( control.Handle,(int) Win32.Msgs.WM_SETREDRAW, redrawOn, 0 );
      }

      base.OnVisibleChanged( args );
    } // OnVisibleChanged( EventArgs args )


    #endregion
 
    #region ISupportInitialize members

    /// <summary>
    /// Signals the object that initialization is starting.
    /// </summary>
    public void BeginInit()
    {
      m_initializing = true;
    } // BeginInit()

    
    /// <summary>
    /// Signals the object that initialization is complete.
    /// </summary>
    public void EndInit()
    {
      m_initializing = false;
      ArrangePanes();
      Refresh();
    } // EndInit()
    
    
    #endregion
  
  } // class Rollout
	
  #endregion

  #region RolloutEventArgs class

  /// <summary>
  /// Event arguments for all rollout events
  /// </summary>
  public class RolloutEventArgs : EventArgs
  {
    #region fields

    /// <summary>
    /// the rollout pane associated with this event
    /// </summary>
    private RolloutPane m_pane;

    #endregion

    #region properties

    /// <summary>
    /// Gets the rollout pane associated with the event
    /// </summary>
    public RolloutPane Pane
    {
      get { return m_pane; }
      set { m_pane = value; }
    }

    #endregion

    #region initialization
    
    /// <summary>
    /// Initializes a RolloutEventArgs instance with default values
    /// </summary>
    public RolloutEventArgs()
    {
    } // RolloutEventArgs()

    
    /// <summary>
    /// Initializes a RolloutEventArgs instance with the specified rollout pane.
    /// </summary>
    public RolloutEventArgs( RolloutPane pane )
    {
      m_pane = pane;
    } // RolloutEventArgs( RolloutPane pane )

    
    #endregion
  }

  
  #endregion

  #region RolloutEventHandler

  /// <summary>
  /// delegate for all rollout events
  /// </summary>
  public delegate void RolloutEventHandler( object sender, RolloutEventArgs args );

  #endregion

  #region RolloutPane
  
  /// <summary>
  /// Provides the child pane that the Rollout control manages.
  /// </summary>
  [ ToolboxItem( false ), Designer( typeof( RolloutPaneDesiger ) ) ]
  public class RolloutPane : System.Windows.Forms.Control
  {

    #region constants

    /// <summary>
    /// collapsed left image
    /// </summary>
    static private readonly Bitmap m_collapsedLeftImage = null;
    
    /// <summary>
    /// collapsed middle image
    /// </summary>
    static private readonly Bitmap m_collapsedMiddleImage = null;
    
    /// <summary>
    /// collapsed right image
    /// </summary>
    static private readonly Bitmap m_collapsedRightImage = null;

    /// <summary>
    /// expanded left image
    /// </summary>
    static private readonly Bitmap m_expandedLeftImage = null;
    
    /// <summary>
    /// expanded middle image
    /// </summary>
    static private readonly Bitmap m_expandedMiddleImage = null;
    
    /// <summary>
    /// expanded right image
    /// </summary>
    static private readonly Bitmap m_expandedRightImage = null;

    /// <summary>
    /// Height in pixels of titlebar.
    /// </summary>
    static private readonly int m_titlebarHeight = 26;
    
    #endregion
    
    #region fields

    /// <summary>
    /// Pane id.
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Height of pane when minimized in pixels.
    /// </summary>
    private int m_collapsedHeight = m_titlebarHeight;
    
    /// <summary>
    /// Height of pane when maximized in pixels.
    /// </summary>
    private int m_expandedHeight = 100;
    
    /// <summary>
    /// Indicates if the pane is current;y expanded.
    /// </summary>
    private bool m_expanded = false;

    /// <summary>
    /// The back color for the expanded portion of the pane.
    /// </summary>
    private Color m_paneBackColor = Theme.PaneBackColor;

    #endregion

    #region construction

    /// <summary>
    /// Static initialization. Loads the panes skins.
    /// </summary>
    static RolloutPane()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
      
      m_collapsedLeftImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-collapsed-left.png") );
      m_collapsedMiddleImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-collapsed-middle.png") );
      m_collapsedRightImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-collapsed-right.png") );
      
      m_expandedLeftImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-expanded-left.png") );
      m_expandedMiddleImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-expanded-middle.png") );
      m_expandedRightImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.pane-expanded-right.png") );
 
    } // RolloutPane
   
    
    /// <summary>
    /// Initializes a RolloutPane instance with default values.
    /// </summary>
    public RolloutPane()
    {
      // enable double buffering
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      BackColor = Color.Transparent;

      this.TabStop = false;
    } // RolloutPane()


    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the id.
    /// </summary>
    public string Id
    {
      get { return m_id; }
      set { m_id = value; }
    }


    /// <summary>
    /// Gets or sets the expanded property
    /// </summary>
    public bool Expanded
    {
      get { return m_expanded; }
      set { m_expanded = value; }
    } // Expanded

    
    /// <summary>
    /// Gets the height of the pane in pixels when minimized.
    /// </summary>
    [ Browsable( false ) ]
    public int CollapsedHeight
    {
      get { return m_collapsedHeight; }
    } // CollapsedHeight


    /// <summary>
    /// Gets the height of the pane in pixels when minimized.
    /// </summary>
    [ Browsable( false ) ]
    public static int TitleBarHeight
    {
      get { return m_titlebarHeight; }
    } // TitleBarHeight


    /// <summary>
    /// Gets the first child in pane
    /// </summary>
    public System.Windows.Forms.Control FirstChild
    {
      get 
      {
        if( this.Controls.Count > 0 )
          return this.Controls[ 0 ];
        else
          return null;
      }
    }
   
    /// <summary>
    /// Gets or sets the height of the pane when expanded.
    /// </summary>
    public int ExpandedHeight
    {
      get { return m_expandedHeight; }
      set { m_expandedHeight = value; }
    } // ExpandedHeight


    /// <summary>
    /// Gets or sets the back color for the pane.
    /// </summary>
    public Color PaneBackColor
    {
      get { return m_paneBackColor; }
      set { m_paneBackColor = value; }
    } // PaneBackColor


    /// <summary>
    /// Returns the height of the pane.
    /// </summary>
    public int ChildHeight
    {
      get { return this.ExpandedHeight - this.CollapsedHeight; }
    } // ChildHeight


    /// <summary>
    /// Returns the parent Rollout control.
    /// </summary>
    public Rollout Rollout
    {
      get { return this.Parent as Rollout; }
    } // Rollout
    
    #endregion

    #region methods

    /// <summary>
    /// Sets the child for the pane.
    /// </summary>
    /// <param name="child">Control instance to set as chlid.</param>
    public void SetChild( Control child )
    {
      // add the child to the pane and position it
      this.Controls.Add( child );
      
      int x = ( int )( ( this.Parent.Width - child.Width ) / 2.0F );

      int padding = 5;

      child.Location = new System.Drawing.Point( x, m_titlebarHeight + padding );
      
      child.BackColor = this.PaneBackColor;

      // set the maximized height to take the pane bevel into account
      m_expandedHeight = m_titlebarHeight + child.Height + ( 2 * padding );
    } // SetChild( Control child )


    /// <summary>
    /// Expands the pane.
    /// </summary>
    public void Expand()
    {
      SetExpanded( true );

      this.TabStop = true;

      if( this.Rollout.Visible )
        this.Rollout.Refresh();
    } // Expand()


    /// <summary>
    /// Collapses the pane.
    /// </summary>
    public void Collapse()
    {
      SetExpanded( false );

      this.TabStop = false;

      if( this.Rollout.Visible )
        this.Rollout.Refresh();
    } // Collapse()


    /// <summary>
    /// Expands/Collapses the pane.
    /// </summary>
    /// <param name="expand">true to expand the pane; false to collapse</param>
    public void SetExpanded( bool expand )
    {
      // toggle the expanded flag
      m_expanded = expand;

      // ask the parent to expand the pane
      Rollout rollout = ( Rollout ) this.Parent;
      if( m_expanded )
        rollout.ExpandPane( this );
      else
        rollout.CollapsePane( this );

    } // SetExpanded( expand )


    /// <summary>
    /// Sets the enabled state of all child controls.  Useful when the child controls should be locked without locking the Pane.
    /// </summary>
    /// <remarks>It is the caller's responsibility to preserve the enabled state of the controls before calling this function.</remarks>
    /// <param name="enabled">True for enabled, false for disabled.</param>
    public void SetChildControlsEnabled(bool enabled)
    {
      foreach(Control control in Controls)
      {
        control.Enabled = enabled;
      }
    } // SetChildControlsEnabled(bool enabled)

    #endregion

    #region event handlers

    /// <summary>
    /// Called when the mouse is down on the pane.
    /// </summary>
    /// <param name="args">MouseEventArgs containg data related to this event.</param>
    protected override void OnMouseDown(MouseEventArgs args)
    {
      // did the mouse click the titlebar?
      if( args.Button == MouseButtons.Left && args.Y < m_titlebarHeight )
      {
        SetExpanded(!m_expanded);
        this.Rollout.Refresh();
      }
    } // OnMouseDown( args )


    /// <summary>
    /// Called when the pane needs to paint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint(PaintEventArgs args)
    {
      base.OnPaint(args);

      Graphics gfx = args.Graphics;

      Image paneLeft = m_expanded ? m_expandedLeftImage : m_collapsedLeftImage;
      Image paneMiddle = m_expanded ? m_expandedMiddleImage : m_collapsedMiddleImage;
      Image paneRight = m_expanded ? m_expandedRightImage : m_collapsedRightImage;

      gfx.DrawImageUnscaled(paneLeft, 0, 0, paneLeft.Width, paneLeft.Height);

      for( int x = paneLeft.Width; x < this.Width - paneRight.Width; x += paneMiddle.Width )
      {
        gfx.DrawImageUnscaled(paneMiddle, x, 0, paneMiddle.Width, paneMiddle.Height);
      }

      gfx.DrawImageUnscaled(paneRight, this.Width - paneRight.Width, 0, paneRight.Width, paneRight.Height);

      RectangleF layout = new RectangleF(2, 0, this.Width, m_titlebarHeight);
      layout.Inflate(-2.0F, -2.0F);

      StringFormat format = new StringFormat();
      format.LineAlignment = StringAlignment.Center;

      gfx.DrawString(this.Text, Theme.PaneFont, m_expanded ? Theme.PaneExpandedBrush : Theme.PaneCollapsedBrush, layout, format);

      int xPos = 0;
      int yPos = m_titlebarHeight - 1;
      int width = this.Width - 2;
      int height = this.Height - m_titlebarHeight;

      if( this.Expanded )
      {
        using( SolidBrush brush = new SolidBrush(this.PaneBackColor) )
          gfx.FillRectangle(brush, xPos, yPos, width, height);

        gfx.DrawLine(Pens.White, new Point(xPos, yPos), new Point(xPos, yPos + height));
        gfx.DrawLine(Pens.White, new Point(xPos, yPos + height), new Point(xPos + width, yPos + height));
        gfx.DrawLine(Pens.White, new Point(xPos + width, yPos + height), new Point(xPos + width, yPos));
      }
    } // OnPaint( PaintEventArgs args )


    /// <summary>
    /// Called when the text property changes.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnTextChanged(EventArgs args)
    {
      base.OnTextChanged(args);
      Invalidate();
    } // OnTextChanged( args )


    /// <summary>
    /// Called when the pane is resized.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnResize(EventArgs args)
    {
      base.OnResize(args);

      if( this.Rollout.Visible )
        Invalidate();
    } // OnResize( args )

    #endregion

  } // class RolloutPane
	
  #endregion

  #region RolloutPaneCollection class

  /// <summary>
  /// Provides a collction of RolloutPanes
  /// </summary>
  public class RolloutPaneCollection : CollectionBase
  {

    #region methods

    /// <summary>
    /// Adds the specified RolloutPane to the collection.
    /// </summary>
    /// <param name="pane">RolloutPane instance to add.</param>
    /// <returns>Index of newly inserted instance</returns>
    public int Add( RolloutPane pane )
    {
      return List.Add( pane );
    } // Add( RolloutPane pane )


    /// <summary>
    /// Returns the index of the specified pane.
    /// </summary>
    /// <param name="pane">RolloutPane to get index for</param>
    /// <returns>Index of specified pane.</returns>
    public int IndexOf( RolloutPane pane )
    {
      return List.IndexOf( pane );
    } // IndexOf( RolloutPane pane )
    
    
    /// <summary>
    /// Removes the specified RolloutPane from the collection.
    /// </summary>
    /// <param name="pane">RollutPane instance to remove.</param>
    
    public void Remove( RolloutPane pane )
    {
      List.Remove( pane );
    } // Remove( RolloutPane pane )


    /// <summary>
    /// Indicates if the specified rollout pane is contained within the collection
    /// </summary>
    /// <param name="id">Id of pane</param>
    /// <returns>True if found; false otherwise</returns>
    public bool Contains( string id )
    {
      return Find( id ) != null;
    } // Contains( string id )
    
    
    /// <summary>
    /// Gets or sets the RolloutPane at the specified index.
    /// </summary>
    public RolloutPane this[ int index ]
    {
      get { return ( RolloutPane ) List[ index ]; }
      set { List[ index ] = value; }
    } // this[ int index ]

    
    /// <summary>
    /// Gets or sets the RolloutPane at the specified index.
    /// </summary>
    public RolloutPane this[ string id ]
    {
      get { return Find( id ); }
      set 
      { 
        RolloutPane pane = Find( id );
        if( pane == null )
        {
          throw new ArgumentException( "Invalid id specified." );
        }

        int index = IndexOf( pane );
        this[ index ] = value;
      }
    } // this[ int index ]

    
    /// <summary>
    /// Validates the type is of RolloutPane.
    /// </summary>
    /// <param name="value">TYpe to check.</param>
    protected override void OnValidate( object value )
    {
      base.OnValidate( value );

      if( !( value is RolloutPane ) )
      {
        throw new ArgumentException( "Invalid type." );
      }
    } // this[ int index ]


    /// <summary>
    /// Gets the pane instance with the specified id.
    /// </summary>
    /// <param name="id">Id of pane to search for.</param>
    /// <returns>Pane with matching id, null if not found</returns>
    private RolloutPane Find( string id )
    {
      foreach( RolloutPane pane in this )
      {
        if( pane.Id.Equals( id ) )
          return pane;
      }
      return null;
    } // Find( id )

    
    #endregion

  } // RolloutPaneCollection

  #endregion

  #region ExpandedPaneManager class

  /// <summary>
  /// Provides a collction of RolloutPanes
  /// </summary>
  public class ExpandedPaneManager : IEnumerable
  {

    #region fields

    /// <summary>
    /// Collection of RolloutPanes.
    /// </summary>
    private ArrayList m_panes = new ArrayList();

    /// <summary>
    /// The height of all the panes.
    /// </summary>
    private int m_height = 0;

    /// <summary>
    /// The parent rollout control.
    /// </summary>
    private Rollout m_rollout = null; 

    /// <summary>
    /// The number of panes in the control.
    /// </summary>
    private int m_count = -1;

    #endregion

    #region construction
  
    /// <summary>
    /// Creates a new instance of an ExpandedPaneManager. 
    /// </summary>
    /// <param name="rollout">The parent Rollout control.</param>
    public ExpandedPaneManager( Rollout rollout )
    {
      m_rollout = rollout;
    } // ExpandedPaneManager( rollout )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the RolloutPane at the specified index.
    /// </summary>
    public RolloutPane this[ int index ]
    {
      get { return m_panes[ index ] as RolloutPane; }
    } // Indexer

    
    /// <summary>
    /// Returns the height of all the panes.
    /// </summary>
    public int Height 
    {
      get { return m_height; }
    } // Height


    /// <summary>
    /// Returns the amount of height already used.
    /// </summary>
    public int UsedHeight
    {
      get { return ( this.PaneCount * RolloutPane.TitleBarHeight ) + ( this.PaneCount * Rollout.m_spaceBetweenPanes ) + this.Height; }
    } // UsedHeight


    /// <summary>
    /// Returns the amount of height available.
    /// </summary>
    public int AvailableHeight
    {
      get { return m_rollout.Height - this.UsedHeight; }
    } // AvailableHeight


    /// <summary>
    /// Returns the number of panes.
    /// </summary>
    internal int PaneCount 
    {
      get 
      { 
        UpdatePaneCount();
        return m_count; 
      }
      set { m_count = value; }
    } // PaneCount

    #endregion

    #region methods

    /// <summary>
    /// Adds the specified pane to the collection.
    /// </summary>
    /// <param name="pane">The pane being added.</param>
    public void Add( RolloutPane pane )
    {
      if( m_panes.Contains( pane ))
      {
        m_panes.Remove( pane );
        m_panes.Insert( 0, pane );
        return;
      }

      m_panes.Insert( 0, pane );
      m_height += pane.ChildHeight;
      pane.Expanded = true;

      FitPanes();
    } // Add( pane )


    /// <summary>
    /// Removes the specified pane from the collection.
    /// </summary>
    /// <param name="pane">The pane being removed.</param>
    public void Remove( RolloutPane pane )
    {
      if( m_panes.Contains( pane ))
      {
        m_panes.Remove( pane );

        pane.Expanded = false;

        m_height -= pane.ChildHeight;
      }
    } // Remove( pane )


    /// <summary>
    /// Refershes the contents of the collection.
    /// </summary>
    public void Refresh()
    {
      FitPanes();
    } // Refresh()


    /// <summary>
    /// Returns the index of the specified pane.
    /// </summary>
    /// <param name="pane">RolloutPane to get index for</param>
    /// <returns>Index of specified pane.</returns>
    public int IndexOf( RolloutPane pane )
    {
      return m_panes.IndexOf( pane );
    } // IndexOf( RolloutPane pane )


    /// <summary>
    /// Checks if the specified pane is contained in the colleciton.
    /// </summary>
    /// <param name="pane">The pane being checked.</param>
    /// <returns>true if the colelction contains the pane; otherwise false</returns>
    public bool Contains( RolloutPane pane )
    {
      return m_panes.Contains( pane );
    } // Contains( pane )
    

    /// <summary>
    /// Clears the contents of the collection.
    /// </summary>
    public void Clear()
    {
      m_panes.Clear();
      m_height = 0;
    } // Clear

    
    /// <summary>
    /// Returns an IEnumerator over the panes
    /// </summary>
    /// <return>an IEnumerator</return>
    public IEnumerator GetEnumerator()
    {
      return m_panes.GetEnumerator();
    } // GetEnumerator()


    /// <summary>
    /// Updates the pane count based on the nuber of visible panes.
    /// </summary>
    private void UpdatePaneCount()
    {
      if( m_count == -1 )
      {
        m_count = 0;
        foreach( RolloutPane pane in m_rollout.Panes )
        {
          if( pane.Visible )
            m_count++;
        }
      }
    } // UpdatePaneCount()


    /// <summary>
    /// Closes any panes that will not fit.
    /// </summary>
    private void FitPanes()
    {
      while( this.AvailableHeight < 0 )
      {
        if( m_panes.Count <= 1 )
          break;

        Remove( m_panes[ m_panes.Count - 1 ] as RolloutPane );
      }
    } // FitPanes()

    #endregion

  } // RolloutPaneCollection

  #endregion
    
  #region RolloutDesigner class
  
  /// <summary>
  /// Form designer for the Rollout control.
  /// </summary>
  internal class RolloutDesigner : ParentControlDesigner
  {

    #region fields
    
    /// <summary>
    /// Provides the basic framework for building a custom designer.
    /// </summary>
    private IDesignerHost m_designerHost = null;
    
    /// <summary>
    /// Provides an interface for a designer to select components.
    /// </summary>
    private ISelectionService m_selectionService = null;
    
    /// <summary>
    /// Represents a collection of DesignerVerb objects.
    /// </summary>
    private DesignerVerbCollection m_verbs = null;

    #endregion

    #region properties
    
    /// <summary>
    /// Gets the IDesignerHost interface.
    /// </summary>
    public IDesignerHost DesignerHost 
    {
      get 
      {
        if( m_designerHost == null )
        {
          m_designerHost = (IDesignerHost)GetService( typeof( IDesignerHost ) );
        }
        return m_designerHost;
      } // get
    } // DesignerHost     

    
    /// <summary>
    /// Gets the ISelectionService interface.
    /// </summary>
    public ISelectionService SelectionService 
    {
      get 
      {
        if( m_selectionService == null )
          m_selectionService = (ISelectionService)GetService( typeof( ISelectionService ) );
        return m_selectionService;
      } // get
    } // SelectionService

    
    /// <summary>
    /// Gets the DesignerVerbCollection object.
    /// </summary>
    public override DesignerVerbCollection Verbs 
    {
      get 
      {
        if( m_verbs == null ) 
        {
          m_verbs = new DesignerVerbCollection();
          m_verbs.Add( new DesignerVerb( "Add Pane", new EventHandler( OnVerbAddPaneHandler ) ));
        } // if( m_verbs == null ) 

        return m_verbs;
      } // get
    } // Verbs

    
    #endregion

    #region overrides

    /// <summary>
    /// Gets  a value indicating whether a grid should be drawn on the 
    /// control for this designer.
    /// </summary>
    protected override bool DrawGrid 
    {
      get { return false; }
    } //  DrawGrid

    
    /// <summary>
    /// Indicates whether the specified control can be a child of the 
    /// control managed by this designer.
    /// </summary>
    /// <param name="control">The Control to test. </param>
    /// <returns>true if the specified control can be a child of the control managed by this designer; otherwise, false.</returns>
    public override bool CanParent( Control control ) 
    {
      return control is RolloutPane;
    } // CanParent( control ) 
    
    
    #endregion

    #region verb handlers
    
    /// <summary>
    /// Event handler for the "Add Pane" verb.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containg data specific to this event.</param>
    private void OnVerbAddPaneHandler( object sender, EventArgs args ) 
    {
      Rollout rollout = ( Rollout ) this.Control;
      
      // create a new panee object
      RolloutPane pane = ( RolloutPane ) DesignerHost.CreateComponent( typeof( RolloutPane ) );
      
      // fill in the pane's properties
      pane.Text = pane.Name;
      
      // add the pane to the parrent Rollout control
      this.Control.Controls.Add( pane );

      rollout.ExpandPane( pane );
      rollout.Refresh();

      // select the pane in the designer
      ArrayList arrayList = new System.Collections.ArrayList();
      arrayList.Add( pane );      
      SelectionService.SetSelectedComponents( arrayList );

    } // OnVerbAddPaneHandler( sender, args ) 

 
    #endregion
  
  } // class RolloutDesigner
  
  #endregion

  #region RolloutPaneDesigner class

  /// <summary>
  /// Form designer for the RolloutPane control.
  /// </summary>
  internal class RolloutPaneDesiger : ParentControlDesigner
  {

    #region fields
    
    /// <summary>
    /// Provides the basic framework for building a custom designer.
    /// </summary>
    private IDesignerHost m_designerHost = null;
    
    /// <summary>
    /// Provides an interface for a designer to select components.
    /// </summary>
    private ISelectionService m_selectionService = null;
    
    /// <summary>
    /// Represents a collection of DesignerVerb objects.
    /// </summary>
    private DesignerVerbCollection m_verbs = null;
    
    #endregion

    #region properties

    /// <summary>
    /// Gets the IDesignerHost interface.
    /// </summary>
    public IDesignerHost DesignerHost 
    {
      get 
      {
        if( m_designerHost == null )
          m_designerHost = (IDesignerHost)GetService( typeof( IDesignerHost ) );
        return m_designerHost;
      } // get
    } // DesignerHost     

    
    /// <summary>
    /// Gets the ISelectionService interface.
    /// </summary>
    public ISelectionService SelectionService 
    {
      get 
      {
        if( m_selectionService == null )
          m_selectionService = (ISelectionService)GetService( typeof( ISelectionService ) );
        return m_selectionService;
      } // get
    } // SelectionService

    
    /// <summary>
    /// Gets the DesignerVerbCollection object.
    /// </summary>
    public override DesignerVerbCollection Verbs 
    {
      get 
      {
        if( m_verbs == null ) 
        {
          m_verbs = new DesignerVerbCollection();
          m_verbs.Add( new DesignerVerb( "Toggle",  new EventHandler( OnVerbTogglePaneHandler ) ));
          m_verbs.Add( new DesignerVerb( "Move Up",  new EventHandler( OnVerbMoveUpHandler ) ));
          m_verbs.Add( new DesignerVerb( "Move Down",  new EventHandler( OnVerbMoveDownHandler ) ));
        } // if( m_verbs == null ) 
        return m_verbs;
      } // get
    } // Verbs
   
    #endregion

    #region verb handlers

    /// <summary>
    /// Event handler for the "Toggle" verb.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnVerbTogglePaneHandler( object sender, EventArgs args ) 
    {
      // get pane being designed
      RolloutPane pane  = ( RolloutPane ) this.Control;

      // store the current value of the expanded flag
      bool oldValue = pane.Expanded;
      
      // toggle the pane
      pane.SetExpanded( !pane.Expanded );
      pane.Rollout.Refresh();

      // deselect everything in the designer
      this.SelectionService.SetSelectedComponents( null );      
      
      // build array list of components for the selection service
      ArrayList components = new ArrayList();
      components.Add( pane );      
      
      // select the pane
      this.SelectionService.SetSelectedComponents( components );

      // notify the host designer that the "Expanded" property has changed
      this.RaiseComponentChanged( TypeDescriptor.GetProperties( this.Control )["Expanded"], oldValue, !oldValue );
    
    } // OnVerbTogglePaneHandler( sender,  args ) 
   
    
    /// <summary>
    /// Event handler for the "Move Up" verb.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containg data related to this event.</param>
    private void OnVerbMoveUpHandler( object sender, EventArgs args ) 
    {
      // get this control and it's parent
      Rollout rollout = ( Rollout ) this.Control.Parent;
      RolloutPane pane  = ( RolloutPane ) this.Control;
      
      // get the index of the pane being designed
      int tabIndex = rollout.Controls.IndexOf( pane );
      
      // make sure it is not the first tab already
      if( tabIndex > 0 )
      {        
        // get the pane before this one
        Control previousPane = rollout.Controls[ tabIndex - 1 ];
        
        // swap current pane with the one before it
        rollout.Controls.SetChildIndex( pane, tabIndex - 1 );
        rollout.Controls.SetChildIndex( previousPane, tabIndex );
        
        // rearrange the tabs
        rollout.ArrangePanes();
        rollout.Refresh();

        // build array list of components for the selection service
        ArrayList components = new System.Collections.ArrayList();
        components.Add( rollout.Controls[ tabIndex - 1 ] );      
        
        // select the pane
        this.SelectionService.SetSelectedComponents( components );

        // notify the host designer that the "Controls" property has changed
        this.RaiseComponentChanged( TypeDescriptor.GetProperties( rollout )["Controls"], rollout.Controls, rollout.Controls );
        
      } // if( tabIndex > 0 )
    } // OnVerbMoveUpHandler( sender, args ) 

    
    /// <summary>
    /// Event handler for the "Move Down" verb.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containg data related to this event.</param>
    private void OnVerbMoveDownHandler( object sender, EventArgs args ) 
    {
      // get this control and it's parent
      Rollout rollout = ( Rollout ) this.Control.Parent;
      RolloutPane pane  = ( RolloutPane ) this.Control;
      
      // get the index of the pane being designed
      int tabIndex = rollout.Controls.IndexOf( pane );
      
      // is the designed tab already at the bottom?
      if( tabIndex < (rollout.Controls.Count - 1) )
      {
        // get the pane after this one
        Control nextPane = rollout.Controls[ tabIndex + 1 ];
      
        // swap current pane with the one after it
        rollout.Controls.SetChildIndex( nextPane, tabIndex  );
        rollout.Controls.SetChildIndex( pane, tabIndex + 1 );
        
        // build array list of components for the selection service
        ArrayList components = new System.Collections.ArrayList();
        components.Add( rollout.Controls[ tabIndex + 1 ] );      
        
        // select the pane
        this.SelectionService.SetSelectedComponents( components );

        // notify the host designer that the "Controls" property has changed
        this.RaiseComponentChanged( TypeDescriptor.GetProperties( rollout )["Controls"], rollout.Controls, rollout.Controls );
      } // if( tabIndex < (rollout.Controls.Count - 1) )   
    } // OnVerbMoveDownHandler( sender, args ) 
   
    
    #endregion

    #region overrides
    
    /// <summary>
    /// Allows a designer to change or remove items from the set of 
    /// properties that it exposes through a TypeDescriptor.
    /// </summary>
    /// <param name="properties">The properties for the class of the component.</param>
    protected override void PostFilterProperties( IDictionary properties ) 
    {
      base.PostFilterProperties( properties );

      #region remove unwanted properties
      
      properties.Remove( "AccessibiltyObject" );
      properties.Remove( "AccessibleDefaultActionDescription" );
      properties.Remove( "AccessibleDescription" );
      properties.Remove( "AccessibleName" );
      properties.Remove( "AccessibleRole" );
      properties.Remove( "AllowDrop" );
      properties.Remove( "Anchor" );
      properties.Remove( "BackgroundImage" );
      properties.Remove( "BindingContext" );
      properties.Remove( "Bottom" );
      properties.Remove( "Bound" );
      properties.Remove( "CanFocus" );
      properties.Remove( "CanSelect" );
      properties.Remove( "Capture" );
      properties.Remove( "CausesValidation" );
      properties.Remove( "ClientRectangle" );
      properties.Remove( "ClientSize" );
      properties.Remove( "CompanyName" );
      properties.Remove( "ContainsFocus" );
      properties.Remove( "ContextMenu" );
      properties.Remove( "Created" );
      properties.Remove( "Cursor" );
      properties.Remove( "DataBindings" );
      properties.Remove( "RightToLeft" );
      properties.Remove( "Enabled" );
      properties.Remove( "DrawGrid" );
      properties.Remove( "GridSize" );
      properties.Remove( "Advanced" );  
      properties.Remove( "Name" );  
      properties.Remove( "ImeMode" );  
      properties.Remove( "TabIndex" );  
      properties.Remove( "TabStop" );  
      properties.Remove( "Visible" );  
      properties.Remove( "Locked" );  
      properties.Remove( "Tag" );  
      properties.Remove( "Modifiers" );  
      properties.Remove( "SnapToGrid" );  
      properties.Remove( "(Name)" );  
      properties.Remove( "Location" );  
      properties.Remove( "Size" );  
      properties.Remove( "AutoScroll" );  
      properties.Remove( "AutoScrollMargin" );  
      properties.Remove( "AutoScrollMinSize" );  
      properties.Remove( "Dock" );  
      properties.Remove( "DockPadding" );  
      properties.Remove( "Layout" );  
      properties.Remove( "Design" );    
      properties.Remove( "BackColor" );    
      properties.Remove( "ForeColor" );    
      properties.Remove( "Font" );    

      #endregion
    
    } // PostFilterProperties( properties ) 

    
    /// <summary>
    /// Initializes the designer with the specified component.
    /// </summary>
    /// <param name="component">The IComponent to associate the designer with.</param>
    public override void Initialize( IComponent component ) 
    {
      base.Initialize( component );
      
      // we are interested in events for when controls are added to the pane
      IComponentChangeService changeService = ( IComponentChangeService )GetService( typeof( IComponentChangeService ) );
      changeService.ComponentAdded += new ComponentEventHandler( OnComponentAddedhandler );
    
    } // Initialize( component ) 

    
    /// <summary>
    /// Handler for the ComponentAdded event.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">ComponentEventArgs containg data releated to this event.</param>
    private void OnComponentAddedhandler( object sender, ComponentEventArgs args )
    {
      // get the pane currently being designed
      RolloutPane pane = ( RolloutPane ) this.Control;

      // get the control that was added
      Control addedControl = ( Control ) args.Component;

      // make sure it is NOT a RolloutPane( not sure why I have to do this, bug??? )
      if( addedControl is RolloutPane == false )
      {
        // add the control to the pane
        pane.Controls.Add( addedControl );
        this.RaiseComponentChanged( TypeDescriptor.GetProperties( pane )["Controls"], pane.Controls, pane.Controls );
      } // if( addedControl is RolloutPane == false )
    } // OnComponentAddedhandler( sender, args )
    
    #endregion

  } // class RolloutPaneDesiger
  

  #endregion   

} // namespace Viatronix.UI

#region revision history

// $Log: Rollout.cs,v $
// Revision 1.27.8.1  2009/02/25 14:24:49  kchalupa
// Coding standards.
//
// Revision 1.27  2007/02/26 18:00:13  gdavidson
// Set the TabStop to false when the pane is not expanded
//
// Revision 1.26  2005/03/29 01:04:08  gdavidson
// Added PaneCollapsing and PaneCollapsed events.
//
// Revision 1.25  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.24  2005/01/26 19:15:14  geconomos
// moved ui functionailty to v3d core
//
// Revision 1.23.2.1  2005/02/28 18:54:40  gdavidson
// Code review
//
// Revision 1.23  2004/12/10 16:27:05  geconomos
// Added a method to check if a specific pane is contained within
//
// Revision 1.22  2004/11/24 19:40:54  gdavidson
// Fixed refresh issue when child control is not visible.
//
// Revision 1.21  2004/11/23 13:42:45  gdavidson
// Fixed refresh issues.
//
// Revision 1.20  2004/11/22 20:15:42  gdavidson
// Fixed rollout resize.
//
// Revision 1.19  2004/11/19 17:54:57  gdavidson
// Added Expand and Collapse methods to a Rollout pane.
//
// Revision 1.18  2004/11/19 15:53:18  gdavidson
// Optimized arranging of the panes.
//
// Revision 1.17  2004/11/18 21:28:05  gdavidson
// Added functionality to only calculate the size of the visible panes.
//
// Revision 1.16  2004/11/18 18:38:34  gdavidson
// Modified implementation.
//
// Revision 1.15  2004/11/17 19:06:29  gdavidson
// Added transparent background.
//
// Revision 1.14  2004/11/17 13:25:06  geconomos
// transparency
//
// Revision 1.13  2004/11/16 13:46:57  geconomos
// changed outline color of pane control
//
// Revision 1.12  2004/11/16 13:39:43  gdavidson
// Stop recursive expansion of children panes.
//
// Revision 1.11  2004/11/12 19:04:17  gdavidson
// Added a FitPanes method.
//
// Revision 1.10  2004/11/02 15:56:16  geconomos
// added id property to rollout pane
//
// Revision 1.9  2004/11/02 14:58:49  geconomos
// Fixed error when computing available height
//
// Revision 1.8  2004/10/22 19:58:11  geconomos
// code cleanup
//
// Revision 1.7  2004/10/20 13:09:42  geconomos
// increased the spacing in between panes
//
// Revision 1.6  2004/10/18 14:49:56  geconomos
// modified the rollout pane to use the back color from the theme manager
//
// Revision 1.5  2004/10/18 02:17:10  geconomos
// changed pane drawing to use the theme manager
//
// Revision 1.4  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.3  2004/07/14 19:00:07  geconomos
// Removed setting of pane text property in SetChild method
//
// Revision 1.2  2004/07/09 18:34:03  geconomos
// Added an event to notify  clients when a rollout pane is about to expand.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.20.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.20  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.19  2003/07/07 15:12:37  geconomos
// Added method to expand tab by index.
//
// Revision 1.18  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.17  2003/04/28 20:13:48  frank
// code review
//
// Revision 1.16  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.15  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.14  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.13  2003/04/10 13:29:07  geconomos
// Set BackColor property to right color.
//
// Revision 1.12  2003/03/03 14:12:14  geconomos
// Verified that parent exists before expanding pane.
//
// Revision 1.11  2002/12/12 15:39:19  geconomos
// Code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Rollout.cs,v 1.27.8.1 2009/02/25 14:24:49 kchalupa Exp $
// $Id: Rollout.cs,v 1.27.8.1 2009/02/25 14:24:49 kchalupa Exp $

#endregion
