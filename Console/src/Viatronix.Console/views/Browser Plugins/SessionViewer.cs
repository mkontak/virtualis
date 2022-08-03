// $Id: SessionViewer.cs,v 1.7.2.1 2008/09/08 18:24:23 kchalupa Exp $
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
using System.Diagnostics;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Data;
using System.Windows.Forms;
using System.Xml;

using Viatronix.Console.Entities;
using Viatronix.UI;
using Viatronix.Reporting;
using Viatronix.Reporting.Formatters;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
	/// <summary>
	/// Displays the snapshots, movies and reports contained in a session series.
	/// </summary>
	public abstract class SessionViewer : System.Windows.Forms.UserControl, ILaunchable
  {

    #region enum SelectedTab

    /// <summary>
    /// Enumeration of tabs
    /// </summary>
    protected enum SelectedTab
    {
      Snapshots,
      Movies,
      Report,
    } // SelectedTab

    #endregion

    #region fields

    /// <summary>
    /// Reference to the Series object.
    /// </summary>
    private Series m_series = null;

    /// <summary>
    /// Additional information describing the ISeriesDetails object.
    /// </summary>
    private string m_additionalInfo = string.Empty;

    /// <summary>
    /// Reference to a launch manager
    /// </summary>
    private LaunchManager m_launchManager = null;

    /// <summary>
    /// The tabstrip control.
    /// </summary>
    private Viatronix.UI.TabStrip m_tabstrip;

    /// <summary>
    /// The image viewer used to display the snapshots, movies or reports.
    /// </summary>
    private Viatronix.UI.ImageViewer m_imageViewer;

    /// <summary>
    /// Provides a border around the image viewer.
    /// </summary>
    private Viatronix.UI.Panel m_backPanel;

    /// <summary>
    /// snapshots tab
    /// </summary>
    private TabStripItem m_snapshotsTab = null;

    /// <summary>
    /// movies tab
    /// </summary>
    private TabStripItem m_moviesTab = null;

    /// <summary>
    /// report tab
    /// </summary>
    private TabStripItem m_reportTab = null;

    /// <summary>
    /// delays the calls to derived class methods at creation time.
    /// </summary>
    private Timer m_timer = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SessionDetails control.
    /// </summary>
		public SessionViewer()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

			InitializeComponent();

      m_tabstrip.BeginInit();

      m_snapshotsTab = new TabStripItem( "snapshots", "Snapshots", SelectedTab.Snapshots );
      m_snapshotsTab.Enabled = false;
      m_tabstrip.Items.Add( m_snapshotsTab );

      m_moviesTab = new TabStripItem( "movies", "Movies", SelectedTab.Movies );
      m_moviesTab.Enabled = false;
      m_tabstrip.Items.Add( m_moviesTab );

      m_reportTab = new TabStripItem( "report", "Report", SelectedTab.Report );
      m_reportTab.Enabled = false;
      m_tabstrip.Items.Add( m_reportTab );

      m_tabstrip.EndInit();
		} // SessionDetails

    #endregion

    #region properties


    /// <summary>
    /// Gets or sets the current Series.
    /// </summary>
    public Series Series
    {
      get { return m_series; }
      set { m_series = value; }
    } // Series


    /// <summary>
    /// Gets or sets any additional data.
    /// </summary>
    public string AdditionalInfo
    {
      get { return m_additionalInfo; }
      set { m_additionalInfo = value; }
    } // AdditionalInfo

  
    /// <summary>
    /// Return a reference to the ImageViewer control.
    /// </summary>
    protected ImageViewer ImageViewer
    {
      get { return m_imageViewer; }
    } // ImageViewer


    /// <summary>
    /// Returns a referenece to the TabStrip control.
    /// </summary>
    protected TabStrip TabStrip
    {
      get { return m_tabstrip; }
    } // TabStrip


    /// <summary>
    /// Returns the default selected tab.
    /// </summary>
    protected abstract SelectedTab DefaultTab { get; }

    #endregion

    #region methods

    /// <summary>
    /// Raises the Load event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnLoad( EventArgs args )
    {
      base.OnLoad( args );

      m_timer = new Timer();
      m_timer.Interval = 50;
      m_timer.Tick += new EventHandler( OnInitializeTickHandler );
      m_timer.Start();      
    } // OnLoad( args )


    /// <summary>
    /// Calls the derived classes Initialize method.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnInitializeTickHandler( object sender, EventArgs args )
    {
      m_timer.Stop();
      m_timer.Dispose();

      Initialize();

      m_timer = new Timer();
      m_timer.Interval = 50;
      m_timer.Tick += new EventHandler( OnDisplayTabTickHandler );
      m_timer.Start();
    } // OnInitializeTickHandler( sender, args )

    
    /// <summary>
    /// Sets the derived classes default tab.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDisplayTabTickHandler( object sender, EventArgs args )
    {
      m_timer.Stop();
      m_timer.Dispose();
      m_timer = null;

      this.TabStrip.SelectedIndex = (int) this.DefaultTab;
    } // OnDisplayTabTickHandler( sender, args )


    #endregion

    #region abstract methods

    /// <summary>
    /// Initializes the tabs.
    /// </summary>
    protected abstract void Initialize();


    /// <summary>
    /// Shows the content for the specified tab.
    /// </summary>
    /// <param name="tab">Selected tab</param>
    protected abstract void DisplayTab( SelectedTab tab );


    /// <summary>
    /// Opens the specified image in a standalone viewer.
    /// </summary>
    /// <param name="path">The path to the image.</param>
    /// <param name="image">The image object.</param>
    protected abstract void OnSnapshotOpen( Snapshot snapshot );


    /// <summary>
    /// Opens the movie at the specified location in a standalone viewer.
    /// </summary>
    /// <param name="path">The path to the movie.</param>
    protected abstract void OnMovieOpen( Movie movie );


    /// <summary>
    /// Opens the specified report page in a standalone viewer.
    /// </summary>
    /// <param name="index">The index of the selected page.</param>
    protected abstract void OnReportOpen( int index );

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the selected tab changed event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSelectedTabChangedHandler( object sender, TabStripEventArgs args )
    {
      this.ImageViewer.Images.Clear();
      DisplayTab( (SelectedTab) m_tabstrip.SelectedIndex );
    } // OnSelectedTabChangedHandler( sender, args )


    /// <summary>
    /// Handles the double click event for the image viewer.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDoubleClickHandler( object sender, EventArgs args )
    {
      if( m_tabstrip.SelectedItem == m_snapshotsTab )
        OnSnapshotOpen( ((SnapshotCollection) m_imageViewer.Tag )[ m_imageViewer.SelectedIndices[0] ]);

      else if( m_tabstrip.SelectedItem == m_moviesTab )
        OnMovieOpen( ((MovieCollection) m_imageViewer.Tag )[ m_imageViewer.SelectedIndices[0] ]);

      else if (m_tabstrip.SelectedItem == m_reportTab)
      {
        if (Global.CurrentUser.IsAllowed("OpenReport"))
        {
          OnReportOpen(m_imageViewer.SelectedIndices[0]);
        }
      }
    } // OnDoubleClickHandler( sender, args )

    #endregion

    #region ILaunchable Members

    /// <summary>
    /// Gets or sets the launch manager.
    /// </summary>
    public LaunchManager Launcher
    {
      get { return m_launchManager; }
      set { m_launchManager = value; }
    } // Launcher

    #endregion

    #region Component Designer generated code
    /// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_tabstrip = new Viatronix.UI.TabStrip();
      this.m_backPanel = new Viatronix.UI.Panel();
      this.m_imageViewer = new Viatronix.UI.ImageViewer();
      ((System.ComponentModel.ISupportInitialize)(this.m_tabstrip)).BeginInit();
      this.m_backPanel.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.m_imageViewer)).BeginInit();
      this.SuspendLayout();
      // 
      // m_tabstrip
      // 
      this.m_tabstrip.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_tabstrip.Location = new System.Drawing.Point(0, 104);
      this.m_tabstrip.Name = "m_tabstrip";
      this.m_tabstrip.Orientation = Viatronix.UI.TabStripOrientation.Bottom;
      this.m_tabstrip.Size = new System.Drawing.Size(744, 24);
      this.m_tabstrip.TabIndex = 4;
      this.m_tabstrip.SelectedTabChanged += new TabStrip.SelectedTabChangedHandler(this.OnSelectedTabChangedHandler);
      // 
      // m_backPanel
      // 
      this.m_backPanel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(148)), ((System.Byte)(166)), ((System.Byte)(198)));
      this.m_backPanel.BorderColor = System.Drawing.Color.White;
      this.m_backPanel.BottomPadding = 0;
      this.m_backPanel.Controls.Add(this.m_imageViewer);
      this.m_backPanel.Curve = 0;
      this.m_backPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_backPanel.Edges = Viatronix.UI.Edges.All;
      this.m_backPanel.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(123)), ((System.Byte)(134)), ((System.Byte)(165)));
      this.m_backPanel.LeftPadding = 0;
      this.m_backPanel.Location = new System.Drawing.Point(0, 0);
      this.m_backPanel.Name = "m_backPanel";
      this.m_backPanel.RightPadding = 0;
      this.m_backPanel.Size = new System.Drawing.Size(744, 104);
      this.m_backPanel.TabIndex = 5;
      this.m_backPanel.TopPadding = 0;
      // 
      // m_imageViewer
      // 
      this.m_imageViewer.AllowSelection = true;
      this.m_imageViewer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_imageViewer.AutoSize = true;
      this.m_imageViewer.Bevel = 0;
      this.m_imageViewer.DrawOutline = true;
      this.m_imageViewer.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(124)), ((System.Byte)(129)), ((System.Byte)(148)));
      this.m_imageViewer.ImageSize = new System.Drawing.Size(82, 82);
      this.m_imageViewer.Location = new System.Drawing.Point(1, 1);
      this.m_imageViewer.MaintainProportions = true;
      this.m_imageViewer.MultiSelect = false;
      this.m_imageViewer.Name = "m_imageViewer";
      this.m_imageViewer.OutlineColor = System.Drawing.Color.White;
      this.m_imageViewer.Size = new System.Drawing.Size(742, 102);
      this.m_imageViewer.Spacing = 10;
      this.m_imageViewer.TabIndex = 6;
      this.m_imageViewer.ViewRectangle = new System.Drawing.Rectangle(0, 0, 725, 102);
      this.m_imageViewer.DoubleClick += new System.EventHandler(this.OnDoubleClickHandler);
      // 
      // SessionDetails
      // 
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(123)), ((System.Byte)(134)), ((System.Byte)(165)));
      this.Controls.Add(this.m_backPanel);
      this.Controls.Add(this.m_tabstrip);
      this.Name = "SessionDetails";
      this.Size = new System.Drawing.Size(744, 128);
      ((System.ComponentModel.ISupportInitialize)(this.m_tabstrip)).EndInit();
      this.m_backPanel.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.m_imageViewer)).EndInit();
      this.ResumeLayout(false);

    }
		#endregion

  } // class SessionViewer
} // namespace Viatronix.Console

#region revision history

// $Log: SessionViewer.cs,v $
// Revision 1.7.2.1  2008/09/08 18:24:23  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
// Revision 1.7  2007/02/26 19:10:00  gdavidson
// Commented Code
//
// Revision 1.6  2006/07/18 14:52:35  mkontak
// Fix rights and transferring into the system
//
// Revision 1.5  2006/07/11 18:12:50  gdavidson
// Issue #4832: Reworked SessionViewer
//
// Revision 1.4  2006/07/06 18:33:50  gdavidson
// Issue #4832: Update control on the main thread
//
// Revision 1.3  2006/06/13 13:12:38  gdavidson
// Fixed cross thread execution
//
// Revision 1.2  2006/01/16 14:25:22  gdavidson
// Added comments
//
// Revision 1.1  2006/01/03 16:16:21  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/Browser Plugins/SessionViewer.cs,v 1.7.2.1 2008/09/08 18:24:23 kchalupa Exp $
// $Id: SessionViewer.cs,v 1.7.2.1 2008/09/08 18:24:23 kchalupa Exp $

#endregion
