// $Id: LaunchViewerPane.cs,v 1.7 2006/10/30 16:24:12 gdavidson Exp $
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

using Viatronix.v3D.Core;
using Viatronix.v3D.UI;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Summary description for LaunchViewerPane.
	/// </summary>
	public class LaunchViewerPane : System.Windows.Forms.UserControl, ILaunchable
	{
    
    #region fields

    /// <summary>
    /// Displays the IViewerControllers.
    /// </summary>
    private Viatronix.v3D.Console.ViewerSelector m_selector;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Reference to a LaunchManager.
    /// </summary>
    private LaunchManager m_launchManager = null;

    /// <summary>
    /// RolloutPane expanded handler
    /// </summary>
    private RolloutEventHandler m_expandedHandler = null;

    #endregion

    #region construction

    /// <summary>
    /// Default constructor.
    /// </summary>
		public LaunchViewerPane()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      m_expandedHandler = new Viatronix.v3D.UI.RolloutEventHandler( this.OnPaneExpandedHandler );
      Global.Frame.Panes.PaneExpanded += m_expandedHandler;

      Global.Frame.ViewChanged += new EventHandler( this.OnViewChangedHandler );
      Global.Frame.ViewChanging += new EventHandler( this.OnViewChangedHandler );
      ((ViewBrowser) ( Global.Frame.ActiveView )).SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
      ((ViewBrowser) ( Global.Frame.ActiveView )).StudiesSelectionChanged += new StudyCollectionDelegate( this.OnStudiesChangedHandler );
      ((ViewBrowser) ( Global.Frame.ActiveView )).Searcher.ResetSearch += new EventHandler( this.OnResetSearchHandler );

      m_selector.ViewerSelected += new SetterDelegate< IViewerLauncher>( this.OnViewerSelectedHandler );
		} // LaunchViewerPane()

    #endregion

    #region methods

    /// <summary>
    /// Populates the ViewerSelector with IViewerControllers.
    /// </summary>
    /// <param name="collection">SeriesCollection</param>
    private void PopulateSelector( SeriesCollection collection )
    {
      m_selector.BeginUpdate();

      foreach( IViewerLauncher launcher in this.Launcher.GetPossibleViewers( collection ))
        m_selector.AddViewerController( launcher );

      m_selector.EndUpdate();
    } // PopulateSelector( collection )


    private void UpdateLaunchSeries( SeriesCollection seriesCollection )
    {
       if( m_expandedHandler != null )
      {
        Global.Frame.Panes.PaneExpanded -= m_expandedHandler;
        m_expandedHandler = null;
      }

      m_selector.Clear();

      PopulateSelector( seriesCollection );
    }


		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
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
		} // Dispose( disposing )

    #endregion

    #region event handlers

    /// <summary>
    /// Attaches to the actie view's SeriesSelectionChanged event, if one exists.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangedHandler( object sender, EventArgs args )
    {
      if( Global.Frame.ActiveView is ViewBrowser )
      {
        ((ViewBrowser) ( Global.Frame.ActiveView )).SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
        ((ViewBrowser) ( Global.Frame.ActiveView )).StudiesSelectionChanged += new StudyCollectionDelegate( this.OnStudiesChangedHandler );
        ((ViewBrowser) ( Global.Frame.ActiveView )).Searcher.ResetSearch += new EventHandler( this.OnResetSearchHandler );
      }

      m_selector.Clear();
    } // OnViewChangedHandler( sender, args )


    /// <summary>
    /// Attaches to the actie view's SeriesSelectionChanged event, if one exists.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangingHandler( object sender, EventArgs args )
    {
      if( Global.Frame.ActiveView is ViewBrowser )
      {
        ((ViewBrowser) ( Global.Frame.ActiveView )).SeriesSelectionChanged -= new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
        ((ViewBrowser) ( Global.Frame.ActiveView )).StudiesSelectionChanged -= new StudyCollectionDelegate( this.OnStudiesChangedHandler );
        ((ViewBrowser) ( Global.Frame.ActiveView )).Searcher.ResetSearch -= new EventHandler( this.OnResetSearchHandler );
      }
    } // OnViewChangedHandler( sender, args )


    /// <summary>
    /// Updates the preview image based on the newly selected series.
    /// </summary>
    /// <param name="series">The currently selected series in the ViewBrowser.</param>
    private void OnSeriesChangedHandler( SeriesCollection collection )
    {
      if( this.InvokeRequired )
      {
        SeriesCollectionDelegate method = new SeriesCollectionDelegate( this.UpdateLaunchSeries );
        Invoke( method, new object[] { collection });
      }
      else
        UpdateLaunchSeries( collection );
    } // OnSeriesChangedHandler( collection )


    /// <summary>
    /// Updates the preview image based on the newly selected series.
    /// </summary>
    /// <param name="series">The currently selected series in the ViewBrowser.</param>
    private void OnStudiesChangedHandler( StudyCollection collection )
    {
      SeriesCollection seriesCollection = new SeriesCollection();
      if( collection.Count > 0 )
        seriesCollection = collection[0].Series;

      if( this.InvokeRequired )
      {
        SeriesCollectionDelegate method = new SeriesCollectionDelegate( this.UpdateLaunchSeries );
        Invoke( method, new object[] { seriesCollection });
      }
      else
        UpdateLaunchSeries( seriesCollection );
    } // OnStudiesChangedHandler( collection )


    /// <summary>
    /// Handles the viewer selected event.
    /// </summary>
    /// <param name="launcher">IViewerLauncher</param>
    private void OnViewerSelectedHandler( IViewerLauncher launcher )
    {
      this.Launcher.Launch( launcher );
    } // OnViewerSelectedHandler( controller )


    /// <summary>
    /// Handles the pane expanded event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An RolloutEventArgs that contains the event data.</param>
    private void OnPaneExpandedHandler( object sender, RolloutEventArgs args )
    {
      Global.Frame.Panes.PaneExpanded -= m_expandedHandler;
      m_expandedHandler = null;

      ViewBrowser browser = (ViewBrowser) ( Global.Frame.ActiveView );

      if( browser.Studies.Count > 0 )
        PopulateSelector( browser.Studies[0].Series );
      else
        PopulateSelector( browser.Series );
    } // OnPaneExpandedHandler( sender, args )


    /// <summary>
    /// Handles the ISearchable reset handler
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnResetSearchHandler( object sender, EventArgs args )
    {
      if( this.InvokeRequired )
        Invoke( new MethodInvoker( m_selector.Clear ));
      else
        m_selector.Clear();
    } // OnResetSearchHandler( sender, args )

    #endregion

    #region ILaunchable Members

    /// <summary>
    /// Gets or set the LaunchManager.
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
      this.m_selector = new Viatronix.v3D.Console.ViewerSelector();
      this.SuspendLayout();
      // 
      // m_selector
      // 
      this.m_selector.Location = new System.Drawing.Point(4, 0);
      this.m_selector.Name = "m_selector";
      this.m_selector.Size = new System.Drawing.Size(292, 240);
      this.m_selector.TabIndex = 0;
      // 
      // LaunchViewerPane
      // 
      this.Controls.Add(this.m_selector);
      this.Name = "LaunchViewerPane";
      this.Size = new System.Drawing.Size(300, 240);
      this.ResumeLayout(false);

    }
		#endregion

  } // class LaunchViewerPane
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: LaunchViewerPane.cs,v $
// Revision 1.7  2006/10/30 16:24:12  gdavidson
// Changed IViewerController to IViewerLauncher
//
// Revision 1.6  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.5  2006/05/10 19:54:32  gdavidson
// Issue #4790: Clear the LaunchPane when searching
//
// Revision 1.4  2006/04/24 15:48:01  gdavidson
// Implemented immediate launch on a double clicked study
//
// Revision 1.3  2006/02/01 14:09:10  gdavidson
// Cleared out the launch selector when the view changes
//
// Revision 1.2  2006/01/09 19:51:40  gdavidson
// Added vode to optimize the update for the ViewerSelector user control.
//
// Revision 1.1  2005/12/14 20:02:34  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console/panes/LaunchViewerPane.cs,v 1.7 2006/10/30 16:24:12 gdavidson Exp $
// $Id: LaunchViewerPane.cs,v 1.7 2006/10/30 16:24:12 gdavidson Exp $

#endregion
