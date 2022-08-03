// $Id: FrameConsole.cs,v 1.2.2.1 2007/09/10 19:32:55 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Threading;
using System.ComponentModel;
using System.Collections.Generic;
using System.Configuration;
using System.Windows.Forms;
using System.Xml;

using Viatronix.Logging;
using Viatronix.UI;

namespace Viatronix.Console
{
	/// <summary>
	/// Main application form. Contains the toolbars, menus and views.
	/// </summary>
	public class FrameConsole: Viatronix.UI.Frame
	{

    #region fields

    /// <summary>
    /// reference to a user label control
    /// </summary>
    private UserLabel m_userLabel = new UserLabel();


    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    public FrameConsole() : base("V3D Console")
    {

      this.Font = Viatronix.UI.Theme.FormFont;

      DisplayUser();


      //LaunchManager.ViewerLaunched += new EventHandler(OnViewerLaunchedHandler);
      //LaunchManager.ViewerClosed   += new EventHandler(OnViewerClosedHandler);

      this.WorkflowTabs.SelectedTabChanging += new TabStrip.SelectedTabChangingHandler( this.OnWorkflowTabChangingHandler );


    } // FrameConsole()

    #endregion

    #region methods


    /// <summary>
    /// Displays the users name in the upper left hand corener.
    /// </summary>
    private void DisplayUser()
    {
      m_userLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      m_userLabel.Location = new System.Drawing.Point( this.Width - ( m_userLabel.Width + 20 ), this.Menus.Height );
      this.Controls.Add( m_userLabel );

      m_userLabel.BringToFront();
    } // DisplayUser()

    #endregion

    #region override methods

    /// <summary>
    /// Load the frame.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnHandleCreated( EventArgs args )
    {
      base.OnHandleCreated( args );

      Configure( (XmlNode) ConfigurationManager.GetSection( "frame" ));
    } // OnHandleCreated( args )


    /// <summary>
    /// Stores the expanded panes before the view is changed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnViewChanging( CancelEventArgs args )
    {
      base.OnViewChanging( args );

      // store expanded panes
      List<string> expandedPanes = new List<string>();
      foreach(RolloutPane pane in this.Panes.Panes )
      {
        if( pane.Enabled && pane.Expanded ) 
          expandedPanes.Add( pane.Id );
      }

      this.ViewState[ "ExpandedPanes" ] = expandedPanes;
    } // OnViewChanging( args )


    /// <summary>
    /// Displays the panes that were open previously.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnViewChanged(CancelEventArgs args)
    {
      base.OnViewChanged( args );

      // open expanded panes
      object state = null;
      if( this.ViewState.TryGetValue( "ExpandedPanes", out state )) 
      {
        List< string > expandedPanes = (List< string >) state;
        foreach( RolloutPane pane in this.Panes.Panes )
        {
          if( !pane.Enabled )
            continue;
          else if( expandedPanes.Contains( pane.Id ) )
            pane.Expand();
          else
            pane.Collapse();
        }
      }
    } // OnViewChanged( args )


    /// <summary>
    /// Disposes of the lock manager when the application closes.
    /// </summary>
    /// <param name="e">CancelEventArgs containing data related to this event.</param>
    protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
    {

      try
      {
        if (this.CurrentUser != null)
          this.SecurityProvider.Logout();

      }
      catch (Exception)
      {
        // Ignore
      }
 
      //Global.LockManager.Dispose();
      base.OnClosing (e);
    } // OnClosing( e )

   
    /// <summary>
    /// Called when the form is closed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnClosed( EventArgs args )
    {
      try
      {

        base.OnClosed( args );

      }
      catch ( Exception ex )
      {
        Viatronix.UI.MessageBox.ShowError ( "Error occurred during close" + ex.Message );
        Viatronix.Logging.Log.Error ( "Error occurred during close" + ex.Message, "FrameConsole", "OnClosed");
      }

    } // OnClosed( args )
   
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the WorkFlow Tab Change Event of the Frame.
    /// </summary>
    /// <param name="sender">obejct</param>
    /// <param name="args">TabStripEventArgs</param>
    public void OnWorkflowTabChangingHandler ( object sender, TabStripEventArgs args )
    {
      if( args.NewlySelectedTabIndex == -1 )
        return;

      // show the viewer when the viewer tab is selected
      if( this.WorkflowTabs.Items[ args.NewlySelectedTabIndex ].ID == LaunchManager.ViewerTab )
      {
        IViewerLauncher launcher = (IViewerLauncher) this.WorkflowTabs.Items[ LaunchManager.ViewerTab ].Data;
        args.Cancel = true;
      }
    } // OnWorkflowTabChangingHandler ( sender, args )


    /// <summary>
    /// Handles the launch manager's ViewerLaunched event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    //private void OnViewerLaunchedHandler(object sender, EventArgs args)
    //{

    //  Log.Debug("Viewer Launch was detected.", "FrameConsole", "OnViewerLaunchedHandler");

    //  // Dispose of timer to make sure it doesn't come up while viewer is launched.
    //  m_renewTimer.Change(Timeout.Infinite, Timeout.Infinite);
    //  m_renewTimer.Dispose();
    //  m_renewTimer = null;

    //} // OnViewerLaunchedHandler( sender, args )


    /// <summary>
    /// Handles the launch manager's ViewerClosed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    //private void OnViewerClosedHandler(object sender, EventArgs args)
    //{
    //  Log.Debug("Viewer Close was detected.", "FrameConsole", "OnViewerClosedHandler");

    //  // Create a new timer.
    //  m_renewTimer = new System.Threading.Timer(new TimerCallback(OnRenewTimerTickHandler), null, 0, m_renewalInterval);
    //} // OnViewerClosedHandler( sender, args )

    #endregion

  } // class FrameConsole
} // namespace Viatronix.Console

