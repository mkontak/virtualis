// $Id: ControllerViews.cs,v 1.9.2.1 2007/04/11 15:46:39 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Reflection;
using Viatronix.UI;

namespace Viatronix.UI
{
  /// <summary>
  /// Responsible for maintaining the frame's top-level views
  /// </summary>
  public class ControllerViews : System.Windows.Forms.UserControl
	{

    #region fields

    /// <summary>
    /// selected view
    /// </summary>
    private System.Windows.Forms.Control m_view = null;

    /// <summary>
    /// The currently loaded view's workflow id
    /// </summary>
    private string m_currentViewWorkflowID = string.Empty;

    /// <summary>
    /// selected view template
    /// </summary>
    private Viatronix.UI.TemplateView m_viewTemplate = null;

    /// <summary>
    /// pane controller
    /// </summary>
    private Viatronix.UI.ControllerPanes m_panes;
    
    /// <summary>
    /// workflow tabs
    /// </summary>
    private Viatronix.UI.TabStrip m_workflowTabs;
    
    /// <summary>
    /// view tabs
    /// </summary>
    private Viatronix.UI.TabStrip m_viewTabs;
    
    /// <summary>
    /// progress bar
    /// </summary>
    private Viatronix.UI.ProgressBar2 m_progressbar;

    /// <summary>
    /// company logo bitmap
    /// </summary>
    static private Bitmap m_viatronixLogo;

    /// <summary>
    /// View state for all views
    /// </summary>
    private Dictionary< string, Dictionary< string, Dictionary< string, object >>> m_workflowState = new Dictionary< string, Dictionary< string, Dictionary< string, object >>>();

    #endregion

    #region properties

    /// <summary>
    /// Returns the active view state
    /// </summary>
    internal Dictionary< string, object > ViewState
    {
      get { return m_workflowState[ m_currentViewWorkflowID ][ m_viewTemplate.Id ]; }
    } // ViewState


    /// <summary>
    /// Gets the pane controller
    /// </summary>
    internal ControllerPanes Panes
    {
      get { return m_panes; }
    } // Panes


    /// <summary>
    /// gets the workflow tabs
    /// </summary>
    internal Viatronix.UI.TabStrip WorkflowTabs
    {
      get { return m_workflowTabs; }
    } // WorkflowTabs

    
    /// <summary>
    /// gets the view tabs
    /// </summary>
    internal Viatronix.UI.TabStrip ViewTabs
    {
      get { return m_viewTabs; }
    } // ViewTabs

    
    /// <summary>
    /// gets the active view instance
    /// </summary>
    internal System.Windows.Forms.Control ActiveView
    {
      get { return m_view; }
    } // ActiveView

    /// <summary>
    /// gets the active view template instance
    /// </summary>
    internal Viatronix.UI.TemplateView ActiveViewTemplate
    {
      get { return m_viewTemplate; }
    } // ActiveViewTemplate


    /// <summary>
    /// gets the progress bar 
    /// </summary>
    internal Viatronix.UI.ProgressBar2 ProgressBar
    {
      get { return m_progressbar; }
    } // ProgressBar
    
    
    #endregion

    #region initialization
    
    /// <summary>
    /// type initializer
    /// </summary>
    static ControllerViews()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
      
      // load the company logo
      m_viatronixLogo = 
        new Bitmap( assembly.GetManifestResourceStream( "Viatronix.UI.res.logo-viatronix.png" ) );
    } // static ControllerViews()

		
    /// <summary>
    /// Initializes a ControllerViews instance with default values
    /// </summary>
    public ControllerViews()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );      
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      InitializeComponent();

      // wire up to the pane controller's visible flag
      m_panes.VisibleChanged += new EventHandler( OnPanesVisibilityChangedHandler );

      // wire up to the selected tab event for the workflow tabstrip
      m_workflowTabs.SelectedTabChanged  += new TabStrip.SelectedTabChangedHandler( OnWorkflowTabChangedHandler );
      
      // wire up to the selected tab event for the views tabstrip
      m_viewTabs.SelectedTabChanged += new TabStrip.SelectedTabChangedHandler( OnViewTabChangedHandler );

      // we only show the progressbar when the value is greater than zero
      m_progressbar.Visible = false;
		} // ControllerViews()

    
    #endregion

    #region events

    /// <summary>
    /// This event is raised before the active view is changed.
    /// </summary>
    internal event CancelEventHandler ViewChanging;

    /// <summary>
    /// This event is raised after the active view has changed.
    /// </summary>
    internal event CancelEventHandler ViewChanged;

    #endregion

    #region methods

    /// <summary>
    /// Removes execessive mouse message from the windows queue
    /// </summary>
    public void ConsumeTabEvents()
    {
      // remove excess mouse messages
      Win32.MSG msg = new Win32.MSG();
      uint mouseFirst = (uint) Win32.WindowMessages.WM_LBUTTONDOWN;
      uint mouseLast = (uint) Win32.WindowMessages.WM_LBUTTONDOWN;
      uint removeMessage = (uint) Win32.PeekMessageFlags.PM_REMOVE;

      bool consumeEvents = true;
      while( consumeEvents )
        consumeEvents = Win32.User32.PeekMessage( ref msg, m_workflowTabs.Handle.ToInt32(), mouseFirst, mouseLast, removeMessage );

      consumeEvents = true;
      while( consumeEvents )
        consumeEvents = Win32.User32.PeekMessage( ref msg, m_viewTabs.Handle.ToInt32(), mouseFirst, mouseLast, removeMessage );
    } // ConsumeTabEvents()

 
    /// <summary>
    /// Called when the controller needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      // redraw the background
      Viatronix.UI.Theme.DrawBackground( args.Graphics, this.ClientRectangle );
    } // OnPaint( PaintEventArgs args )

    
    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      
      Invalidate();
    } // OnResize( EventArgs args )
    
    
    /// <summary>
    /// Maximizes the current view to the entire client area.
    /// </summary>
    private void MaximizeView()
    {
      try
      {
        Win32.User32.SendMessage( m_view.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );
        m_view.SuspendLayout();        

        Point location = m_panes.Location;

        Size size = new Size();
        size.Width = this.Width - location.X - 15;
        size.Height = m_panes.Height;

        m_view.SetBounds( location.X, location.Y, size.Width, size.Height );
      }
      finally
      {
        m_view.ResumeLayout( true );
        Win32.User32.SendMessage( m_view.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
      }
    } // MaximizeView()

    
    /// <summary>
    /// Restores the current view it's initial size and location.
    /// </summary>
    private void RestoreView()
    {
      try
      {
        Win32.User32.SendMessage( m_view.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );
        m_view.SuspendLayout();

        Rectangle bounds = new Rectangle();
        bounds.X = m_panes.Right + 10;
        bounds.Y = m_panes.Top;
        bounds.Width = this.Width - bounds.X - 15;
        bounds.Height = m_panes.Height;
        
        m_view.SetBounds( bounds.X, bounds.Y, bounds.Width, bounds.Height );
      }
      finally
      {
        m_view.ResumeLayout( true );
        Win32.User32.SendMessage( m_view.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
      }
    } // RestoreView()


    /// <summary>
    /// Called when the visibility of the panes changes.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnPanesVisibilityChangedHandler( object sender, EventArgs args )
    {
      // do we have a view?
      if( m_view == null )
        return;

      // are the panes visible?
      if( m_panes.Visible )
      {
        m_panes.ArrangePanes();
        RestoreView();
      }
      else
      {
        MaximizeView();  
      }    
      this.Refresh();
    } // OnPanesVisibilityChangedHandler( object sender, EventArgs args )

    
    /// <summary>
    /// Called when the selecetd workflow tab changes.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containging data related to this event</param>
    private void OnWorkflowTabChangedHandler( object sender, TabStripEventArgs args )
    {
      Cursor cursor =  this.Parent.Cursor;
      try
      {
        // begin initialization for the view tabs
        m_viewTabs.BeginInit();

        // clear out the existing view tabs
        m_viewTabs.Items.Clear();

        WorkflowStage workflowStage = ( WorkflowStage ) m_workflowTabs.SelectedItem.Data;

        // lazily create state for each workflow stage
        if( m_workflowState.ContainsKey( workflowStage.Id ) == false )
          m_workflowState[ workflowStage.Id ] = new Dictionary< string, Dictionary< string, object >>();
      
        // add all the view tabs specified by the workflow tab
        foreach( TemplateView template in workflowStage.ViewTemplates )
        {
          m_viewTabs.Items.Add( new TabStripItem( template.Id, template.Text, template ));
        }

        // we are done with the view tab initializtion
        m_viewTabs.EndInit();

        // set the selected view tab
        m_viewTabs.SelectedIndex = workflowStage.ViewTemplates.IndexOf( workflowStage.DefaultViewTemplate );

      } // try
      finally
      {
        this.Parent.Cursor = cursor;
      }
    } // OnWorkflowTabChangedHandler( object sender, EventArgs args )


    /// <summary>
    /// Called when the selected view tab changes.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containging data related to this event</param>
    private void OnViewTabChangedHandler( object sender, TabStripEventArgs args )
    {
      // the TemplateView instance is associated with the data of the view tab
      SetView( ( TemplateView ) m_viewTabs.SelectedItem.Data );
    } // OnViewTabChangedHandler( object sender, EventArgs args )

    
    /// <summary>
    /// Sets the current view for the controller.
    /// </summary>
    /// <param name="template">Template describing view</param>
    private void SetView( TemplateView template )
    {
      try
      {
        // turn off WM_PAINT handling for this control
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );
        
        // suspend all child layouts
        SuspendLayout();        
        
        // do we have an existing view showing?
        if( m_view != null )
        {
          if( ViewChanging != null )
            ViewChanging( this, new CancelEventArgs() );

          // persist any state
          if( m_view is IPersistable )
            ((IPersistable) m_view ).SaveViewState( this.ViewState );

          // dispose and remove view
          this.Controls.Remove( m_view );
          m_view.Dispose();
          m_view = null;  
        }

        // create an instance of the view described in the template
        m_view = ( Control ) System.Activator.CreateInstance( Type.GetType( template.Typename ));

        m_viewTemplate = template;
        m_currentViewWorkflowID = m_workflowTabs.SelectedItem.ID;

        // lazily create state for the view's
        Dictionary< string, object > viewState = null;
        if( m_workflowState[ m_workflowTabs.SelectedItem.ID ].TryGetValue( template.Id, out viewState ) == false )
        {
          viewState = new Dictionary< string, object >();
          m_workflowState[ m_workflowTabs.SelectedItem.ID ][ template.Id ] = viewState;
        }

        // load any persisted state
        if( m_view is IPersistable )
          ((IPersistable) m_view).LoadViewState( viewState );

        // set the achors on all sides
        m_view.Anchor = AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top | AnchorStyles.Bottom;

        // add the view to the controller
        this.Controls.Add( m_view );

        // if there aren't any panes specified for this view we will hide the pane controller
        m_panes.Visible = template.DisplayPanes && template.Panes.Count > 0;

        // restore/maximize based on existing pane's visibility flag
        if( m_panes.Visible )
          RestoreView();
        else
          MaximizeView();

        // pass the additional xml initialization data to the view
        ( ( IView ) m_view ).Initialize( template.InitializationData );

        if( ViewChanged != null )
          ViewChanged(this, new CancelEventArgs());
      } // try
      finally
      {
        this.Focus();
        ResumeLayout( true );
        Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
        Refresh();
      } // finally
    } // void SetView( TemplateView template )
   
    
    #endregion

    #region event handler

    /// <summary>
    /// Handles the text changed event for the progressbar
    /// </summary>
    /// <param name="sender">sender of this event</param>
    /// <param name="args">EventArgs releated to this event</param>
    private void OnProgressTextChangedHandler( object sender, EventArgs args )
    {
      m_progressbar.Refresh();
    } // OnProgressTextChangedHandler( object sender, EventArgs args )


    /// <summary>
    /// Handles the changed event for the progressbar
    /// </summary>
    /// <param name="sender">sender of this event</param>
    /// <param name="args">EventArgs releated to this event</param>
    private void OnProgressChangedHandler(object sender, EventArgs args)
    {
      try
      {
        if ( m_progressbar.Value >= m_progressbar.Maximum )
          m_progressbar.Visible = false;
        else if ( m_progressbar.Value > m_progressbar.Minimum )
          m_progressbar.Visible = true;        
      }
      finally
      {
        m_progressbar.Refresh();
      }
    } // OnProgressChangedHandler( object sender, EventArgs args )


    #endregion
	
    #region component designer generated code
		
    /// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_workflowTabs = new Viatronix.UI.TabStrip();
      this.m_viewTabs = new Viatronix.UI.TabStrip();
      this.m_progressbar = new Viatronix.UI.ProgressBar2();
      this.m_panes = new Viatronix.UI.ControllerPanes();
      ( (System.ComponentModel.ISupportInitialize) ( this.m_workflowTabs ) ).BeginInit();
      ( (System.ComponentModel.ISupportInitialize) ( this.m_viewTabs ) ).BeginInit();
      ( (System.ComponentModel.ISupportInitialize) ( this.m_panes ) ).BeginInit();
      this.SuspendLayout();
      // 
      // m_workflowTabs
      // 
      this.m_workflowTabs.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.m_workflowTabs.BackColor = System.Drawing.Color.Transparent;
      this.m_workflowTabs.Location = new System.Drawing.Point( 50, 300 );
      this.m_workflowTabs.Name = "m_workflowTabs";
      this.m_workflowTabs.Orientation = Viatronix.UI.TabStripOrientation.Bottom;
      this.m_workflowTabs.Size = new System.Drawing.Size( 653, 31 );
      this.m_workflowTabs.TabIndex = 9;
      this.m_workflowTabs.TabStop = false;
      // 
      // m_viewTabs
      // 
      this.m_viewTabs.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                  | System.Windows.Forms.AnchorStyles.Left ) ) );
      this.m_viewTabs.BackColor = System.Drawing.Color.Transparent;
      this.m_viewTabs.Location = new System.Drawing.Point( 3, 20 );
      this.m_viewTabs.Name = "m_viewTabs";
      this.m_viewTabs.Orientation = Viatronix.UI.TabStripOrientation.Left;
      this.m_viewTabs.Size = new System.Drawing.Size( 30, 286 );
      this.m_viewTabs.TabIndex = 10;
      this.m_viewTabs.TabStop = false;
      // 
      // m_progressbar
      // 
      this.m_progressbar.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.m_progressbar.BackColor = System.Drawing.Color.Transparent;
      this.m_progressbar.DrawText = true;
      this.m_progressbar.Location = new System.Drawing.Point( 487, 302 );
      this.m_progressbar.Maximum = 100;
      this.m_progressbar.Minimum = 0;
      this.m_progressbar.Name = "m_progressbar";
      this.m_progressbar.Pulse = false;
      this.m_progressbar.Size = new System.Drawing.Size( 159, 25 );
      this.m_progressbar.Step = 1;
      this.m_progressbar.TabIndex = 11;
      this.m_progressbar.TabStop = false;
      this.m_progressbar.Value = 0;
      this.m_progressbar.ProgressChanged += new System.EventHandler( this.OnProgressChangedHandler );
      this.m_progressbar.TextChanged += new System.EventHandler( this.OnProgressTextChangedHandler );
      // 
      // m_panes
      // 
      this.m_panes.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                  | System.Windows.Forms.AnchorStyles.Left ) ) );
      this.m_panes.BackColor = System.Drawing.Color.Transparent;
      this.m_panes.ForeColor = System.Drawing.Color.White;
      this.m_panes.Location = new System.Drawing.Point( 40, 13 );
      this.m_panes.Name = "m_panes";
      this.m_panes.Size = new System.Drawing.Size( 310, 283 );
      this.m_panes.TabIndex = 8;
      // 
      // ControllerViews
      // 
      this.Controls.Add( this.m_progressbar );
      this.Controls.Add( this.m_viewTabs );
      this.Controls.Add( this.m_workflowTabs );
      this.Controls.Add( this.m_panes );
      this.Name = "ControllerViews";
      this.Size = new System.Drawing.Size( 653, 333 );
      ( (System.ComponentModel.ISupportInitialize) ( this.m_workflowTabs ) ).EndInit();
      ( (System.ComponentModel.ISupportInitialize) ( this.m_viewTabs ) ).EndInit();
      ( (System.ComponentModel.ISupportInitialize) ( this.m_panes ) ).EndInit();
      this.ResumeLayout( false );

    }
    
  
    #endregion

  } // ControllerViews
} // namespace Viatronix.Viewer


#region revision history

// $Log: ControllerViews.cs,v $
// Revision 1.9.2.1  2007/04/11 15:46:39  gdavidson
// Issue #5575: Only consume left button down events
//
// Revision 1.9  2007/03/01 17:09:18  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/ControllerViews.cs,v 1.9.2.1 2007/04/11 15:46:39 gdavidson Exp $
// $Id: ControllerViews.cs,v 1.9.2.1 2007/04/11 15:46:39 gdavidson Exp $

#endregion