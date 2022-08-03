// $Id: ViewerSelector.cs,v 1.7 2007/03/06 19:15:00 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using Viatronix.UI;
using Viatronix.v3D.Core;


namespace Viatronix.Console
{

	/// <summary>
	/// Selection control for choosing the appropriate launch operation.
	/// </summary>
	public class ViewerSelector : System.Windows.Forms.UserControl
	{

    #region fields


    /// <summary>
    /// collection of viewer controllers.
    /// </summary>
    private System.Windows.Forms.ListView m_viewControllers;

    /// <summary>
    /// Viewer text column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_viewerColumn;

    /// <summary>
    /// Viewer description column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_descriptionColumn;

    /// <summary>
    /// Reference to the seelcted controller.
    /// </summary>
    private IViewerLauncher m_selectedLauncher = null;

    /// <summary>
    /// Reference to the existing session group.
    /// </summary>
    private System.Windows.Forms.ListViewGroup m_existingSessions = null;

    /// <summary>
    /// Reference to the new session group.
    /// </summary>
    private System.Windows.Forms.ListViewGroup m_newSessions = null;

    /// <summary>
    /// Collection of controller ids in the control.
    /// </summary>
    private List<string> m_loadedLauncherIDs = new List<string>();

    /// <summary>
    /// Viewer selection event.
    /// </summary>
    public event SetterDelegate< IViewerLauncher> ViewerSelected = null;

    #endregion

    #region construction

    /// <summary>
    /// Default constructor
    /// </summary>
		public ViewerSelector()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      m_viewControllers.ShowGroups = true;

      m_existingSessions = new System.Windows.Forms.ListViewGroup("Existing Sessions");
      m_viewControllers.Groups.Add( m_existingSessions );

      m_newSessions = new System.Windows.Forms.ListViewGroup("New Sessions");
      m_viewControllers.Groups.Add( m_newSessions );
		} // ViewerSelector()

    #endregion

    #region properties

    /// <summary>
    /// Returns the selected viewer controller.
    /// </summary>
    public IViewerLauncher SelectedLauncher
    {
      get { return m_selectedLauncher; }
    } // SelectedController


    /// <summary>
    /// Gets the viewer column
    /// </summary>
    public System.Windows.Forms.ColumnHeader ViewerColumn
    {
      get { return m_viewerColumn; }
    } // ViewerColumn


    /// <summary>
    /// Gets the description column
    /// </summary>
    public System.Windows.Forms.ColumnHeader DescriptionColumn
    {
      get { return m_descriptionColumn; }
    } // DescriptionColumn

    #endregion

    #region methods

    /// <summary>
    /// Adds a viewer that will load an existing session.
    /// </summary>
    /// <param name="controller">IViewerLauncher</param>
    public void AddExistingSession( IViewerLauncher launcher )
    {
      System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(launcher.Text);
      item.SubItems.Add( launcher.Description );
      item.Tag = launcher;
      item.ToolTipText = launcher.Text + "       " + launcher.Description;

      AddImage( launcher, item );


      m_viewControllers.Items.Add(item);
      m_existingSessions.Items.Add( item );
    } // AddExistingSession( controller )


    /// <summary>
    /// Adds a viewer that will create a new session.
    /// </summary>
    /// <param name="controller">IViewerLauncher</param>
    public void AddNewSession( IViewerLauncher launcher )
    {
      System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(launcher.Text);
      item.SubItems.Add( launcher.Description );
      item.Tag = launcher;
      item.ToolTipText = launcher.Text + "       " + launcher.Description;

      AddImage( launcher, item );

      m_viewControllers.Items.Add(item);
      m_newSessions.Items.Add( item );
    } // AddNewSession( controller )


    /// <summary>
    /// Clears the view controllers.
    /// </summary>
    public void Clear()
    {
      m_viewControllers.Items.Clear();
    } // Clear()


    /// <summary>
    /// Adds the IViewerController to the collection.
    /// </summary>
    /// <param name="controller">IViewerController</param>
    public void AddViewerController( IViewerLauncher launcher )
    {
      if( launcher.IsSession )
        AddExistingSession( launcher );
      else
        AddNewSession( launcher );
    } // AddViewer( controller )


    /// <summary>
    /// Prevents the control from drawing until the EndUpdate method is called.
    /// </summary>
    public void BeginUpdate()
    {
      m_viewControllers.BeginUpdate();
    } // BeginUpdate()


    /// <summary>
    /// Resumes drawing of the list view control after drawing is suspended by the BeginUpdate method.
    /// </summary>
    public void EndUpdate()
    {
      m_viewControllers.EndUpdate();
    } // EndUpdate()


    /// <summary>
    /// Raises the ViewerSelected event.
    /// </summary>
    /// <param name="controller">IViewerController</param>
    protected void OnViewerSelected( IViewerLauncher launcher )
    {
      if( this.ViewerSelected != null )
        this.ViewerSelected( launcher );
    } // OnViewerSelected( controller )


    /// <summary>
    /// Raises the Load event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnLoad( EventArgs args )
    {
      base.OnLoad( args );

      Win32.LVS_EX styles = (Win32.LVS_EX) Win32.User32.SendMessage( m_viewControllers.Handle, (int) Win32.LVM.LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0 );
      styles |= Win32.LVS_EX.LVS_EX_ONECLICKACTIVATE | Win32.LVS_EX.LVS_EX_UNDERLINEHOT;

      Win32.User32.SendMessage( m_viewControllers.Handle, (int)Win32.LVM.LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (int)styles );
    } // OnLoad( args )


    /// <summary>
    /// Adds the appropriate viewer image to the listview item.
    /// </summary>
    /// <param name="controller">IViewerLauncher</param>
    /// <param name="item">ListViewItem2</param>
    private void AddImage(IViewerLauncher launcher, System.Windows.Forms.ListViewItem item)
    {
      if( launcher.Logo != null )
      {
        int index = m_loadedLauncherIDs.IndexOf( launcher.ID );
        if( index == -1 )
        {
          if( m_viewControllers.SmallImageList == null )
            m_viewControllers.SmallImageList = new ImageList();

          item.ImageIndex = m_viewControllers.SmallImageList.Images.Count;
          m_viewControllers.SmallImageList.Images.Add( launcher.Logo );
          m_loadedLauncherIDs.Add( launcher.ID );
        }
        else
          item.ImageIndex = index;
      }
    } // AddImage( controller, item )

    #endregion

    #region event handlers


    /// <summary>
    /// Handles the selected index changed handler.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnItemActivatedHandler( object sender, EventArgs args )
    {
      if( m_viewControllers.SelectedItems.Count > 0 )
      {
        m_selectedLauncher = (IViewerLauncher) m_viewControllers.SelectedItems[ 0 ].Tag;
        m_viewControllers.SelectedItems.Clear();
        m_viewControllers.Refresh();
        OnViewerSelected( m_selectedLauncher );
      }
    } // OnItemActivatedHandler( sender, args )

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_viewControllers = new System.Windows.Forms.ListView();
      this.m_viewerColumn = new System.Windows.Forms.ColumnHeader();
      this.m_descriptionColumn = new System.Windows.Forms.ColumnHeader();
      this.SuspendLayout();
      // 
      // m_viewControllers
      // 
      this.m_viewControllers.Activation = System.Windows.Forms.ItemActivation.OneClick;
      this.m_viewControllers.Columns.AddRange( new System.Windows.Forms.ColumnHeader[] {
            this.m_viewerColumn,
            this.m_descriptionColumn} );
      this.m_viewControllers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_viewControllers.ShowGroups = true;
      this.m_viewControllers.FullRowSelect = true;
      this.m_viewControllers.GridLines = true;
      this.m_viewControllers.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.m_viewControllers.Location = new System.Drawing.Point( 0, 0 );
      this.m_viewControllers.MultiSelect = false;
      this.m_viewControllers.Name = "m_viewControllers";
      this.m_viewControllers.ShowItemToolTips = true;
      this.m_viewControllers.Size = new System.Drawing.Size( 440, 152 );
      this.m_viewControllers.TabIndex = 2;
      this.m_viewControllers.UseCompatibleStateImageBehavior = false;
      this.m_viewControllers.View = System.Windows.Forms.View.Details;
      this.m_viewControllers.ItemActivate += new System.EventHandler( this.OnItemActivatedHandler );
      // 
      // m_viewerColumn
      // 
      this.m_viewerColumn.Text = "Viewer";
      this.m_viewerColumn.Width = 90;
      // 
      // m_descriptionColumn
      // 
      this.m_descriptionColumn.Text = "Description";
      this.m_descriptionColumn.Width = 337;
      // 
      // ViewerSelector
      // 
      this.Controls.Add( this.m_viewControllers );
      this.Name = "ViewerSelector";
      this.Size = new System.Drawing.Size( 440, 152 );
      this.ResumeLayout( false );

    }
		#endregion

	} // class ViewerSelector
} // namespace Viatronix.Console

#region revision history

// $Log: ViewerSelector.cs,v $
// Revision 1.7  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.6  2007/03/05 16:16:44  gdavidson
// Removed uninitialized members
//
// Revision 1.5  2007/02/23 19:57:50  gdavidson
// Refreshed the control after clearing the selection
//
// Revision 1.4  2007/02/23 17:11:50  gdavidson
// Cleared out selection after raising the event
//
// Revision 1.3  2007/02/23 17:01:01  gdavidson
// Added tooltips to the viewer items
//
// Revision 1.2  2007/02/23 16:43:25  gdavidson
// Exposed the colums as properties
//
// Revision 1.1  2006/10/30 15:43:19  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.4  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.3  2006/01/09 19:47:34  gdavidson
// Changed ListView to GroupListView
//
// Revision 1.2  2005/12/15 18:00:00  gdavidson
// Added underline hot item to the listview control.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ui/ViewerSelector.cs,v 1.7 2007/03/06 19:15:00 gdavidson Exp $
// $Id: ViewerSelector.cs,v 1.7 2007/03/06 19:15:00 gdavidson Exp $

#endregion
