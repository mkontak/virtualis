// $Id: ImportDialog.cs,v 1.6.2.2 2008/01/31 21:01:58 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Main dialog for importing data.
	/// </summary>
	public class ImportDialog : System.Windows.Forms.Form
	{

    #region delegate

    /// <summary>
    /// Filter Complete Invoke Delegate
    /// </summary>
    private delegate void FilterCompleteInvokeDelegate(FilterEventArgs args);

    #endregion

    #region fields

    /// <summary>
    /// filter complete delegate
    /// </summary>
    private FilterCompleteInvokeDelegate m_filterCompleteDelegate;

    /// <summary>
    /// main panel
    /// </summary>
    private System.Windows.Forms.Panel m_mainPanel;

    /// <summary>
    /// top panel
    /// </summary>
    private Viatronix.v3D.UI.Panel m_topPanel;

    /// <summary>
    /// left panel
    /// </summary>
    private Viatronix.v3D.UI.Panel m_leftPanel;

    /// <summary>
    /// right panel
    /// </summary>
    private Viatronix.v3D.UI.Panel m_rightPanel;

    /// <summary>
    /// content panel
    /// </summary>
    private System.Windows.Forms.Panel m_contentPanel;

    /// <summary>
    /// import panel
    /// </summary>
    private Viatronix.v3D.UI.Button m_importButton;

    /// <summary>
    /// cancel button
    /// </summary>
    private Viatronix.v3D.UI.Button m_cancelButton;

    /// <summary>
    /// components
    /// </summary>
    private IContainer components;

    /// <summary>
    /// listview
    /// </summary>
    private System.Windows.Forms.ListView m_filterPatients;

    /// <summary>
    /// directory column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_directoryColumn;

    /// <summary>
    /// patient column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_patientColumn;

    /// <summary>
    /// treeview
    /// </summary>
    private System.Windows.Forms.TreeView m_patientStructure;

    /// <summary>
    /// import patient info object
    /// </summary>
    private ImportPatientInfo m_patientInfo = new ImportPatientInfo();

    /// <summary>
    /// import study info object
    /// </summary>
    private ImportStudyInfo m_studyInfo = new ImportStudyInfo();

    /// <summary>
    /// import series info object
    /// </summary>
    private ImportSeriesInfo m_seriesInfo = new ImportSeriesInfo();

    /// <summary>
    /// file menu
    /// </summary>
    private System.Windows.Forms.MenuItem m_fileMenu;

    /// <summary>
    /// add menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_addMenuItem;

    /// <summary>
    /// delete menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_deleteMenuItem;

    /// <summary>
    /// exit menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_exitMenuItem;

    /// <summary>
    /// menu item
    /// </summary>
    private System.Windows.Forms.MenuItem menuItem1;

    /// <summary>
    /// import menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_importMenuItem;

    /// <summary>
    /// delete patient menuitem
    /// </summary>
    private System.Windows.Forms.MenuItem m_deletePatient;

    /// <summary>
    /// delete study menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_deleteStudy;

    /// <summary>
    /// delete series menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_deleteSeries;

    /// <summary>
    /// treeview context menu
    /// </summary>
    private System.Windows.Forms.ContextMenu m_treeviewContextMenu;

    /// <summary>
    /// list view context menu
    /// </summary>
    private System.Windows.Forms.ContextMenu m_listviewContextMenu;

    /// <summary>
    /// delete tree node menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_deleteTreeNode;

    /// <summary>
    /// delete listview menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_deleteListViewItem;

    /// <summary>
    /// main menu
    /// </summary>
    private System.Windows.Forms.MainMenu m_mainMenu;

    /// <summary>
    /// add list view menu item
    /// </summary>
    private System.Windows.Forms.MenuItem m_addListViewItem;

    /// <summary>
    /// filter manager
    /// </summary>
    private FilterManager m_filterManager = null;

    /// <summary>
    /// type column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_typeColumn;

    /// <summary>
    /// state column
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_stateColumn;

    /// <summary>
    /// import manager
    /// </summary>
    private ImportManager m_importManager = null;

    /// <summary>
    /// flag to track import state
    /// </summary>
    private bool m_importSuccess = false;

    /// <summary>
    /// image list
    /// </summary>
    private static ImageList m_imageList = null;

    /// <summary>
    /// parent form
    /// </summary>
    private Form m_parent = null;


   /// <summary>
    /// visibility handler for the console
    /// </summary>
    private EventHandler m_consoleVisibilityChangedHandler = null;

    #endregion

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static ImportDialog()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      m_imageList = new ImageList();
      m_imageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.Core.res.error.png" ))); 
      m_imageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.Core.res.warning.png" ))); 
    } // ImportDialog()


    /// <summary>
    /// Creates a new instance of an ImportDialog.
    /// </summary>
    /// <param name="provider">IDataProvider object</param>
    public ImportDialog( IDataProvider provider, User user ) : this( provider, user, null )
    {
    } // ImportDialog( provider )


    /// <summary>
    /// Creates a new instance of an ImportDialog.
    /// </summary>
    /// <param name="provider">IDataProvider object</param>
    /// <param name="parent">Parent form</param>
		public ImportDialog( IDataProvider provider, User user, Form parent )
		{
			InitializeComponent();

     
      m_parent = parent;

      m_filterCompleteDelegate = new FilterCompleteInvokeDelegate(this.UpdateDialog);

      m_filterPatients.SmallImageList = m_imageList;

      m_filterManager = new FilterManager( this );
      m_importManager = new ImportManager( this, provider, user );

      m_patientInfo.Dock = DockStyle.Fill;
      m_patientInfo.Visible = false;
      m_contentPanel.Controls.Add( m_patientInfo );

      m_studyInfo.Dock = DockStyle.Fill;
      m_studyInfo.Visible = false;
      m_contentPanel.Controls.Add( m_studyInfo );

      m_seriesInfo.Dock = DockStyle.Fill;
      m_seriesInfo.Visible = false;
      m_contentPanel.Controls.Add( m_seriesInfo );

			m_filterManager.FilterCompleted += new FilterEventHandler( this.OnFilterCompletedHandler );
      m_importManager.PatientImported += new FilterEventHandler( this.OnPatientImportHandler );

      m_consoleVisibilityChangedHandler = new EventHandler( this.OnConsoleVisibilityChangedHandler );
		} // ImportDialog( provider )


    #endregion

    #region override methods

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


    /// <summary>
    /// Invalidates the control on Resize.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Assigns a visibility event handler to the console.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnLoad( EventArgs args )
    {
      base.OnLoad( args );

      if( m_parent != null )
        m_parent.VisibleChanged += m_consoleVisibilityChangedHandler; 
    } // OnLoad( args )


    /// <summary>
    /// Cleans up any temporary files when the dialog closes.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnClosed( EventArgs args )
    {
      base.OnClosed( args );

      if( m_parent != null )
        m_parent.VisibleChanged -= m_consoleVisibilityChangedHandler;

      ArrayList filters = new ArrayList();
      foreach( ListViewItem item in m_filterPatients.Items )
      {
        FilterEventArgs filterArgs = (FilterEventArgs) item.Tag;
        try
        {
          if( filterArgs.State != FilterState.Failed && filterArgs.State != FilterState.Error && filterArgs.Filter.RequiresDeletion )
            Directory.Delete( Path.Combine( Utilities.Registry.SystemSettings.TempDirectory, filterArgs.Patient.Location ), true );
        }
        catch( Exception e )
        {  
          Viatronix.v3D.Logging.Logger.Err( "Unable to delete temporary files [PATH=" +  filterArgs.Patient.Location + "] [ERROR=" + e.Message + "]");
        }
      }
    } // OnClosed( args )


    #endregion

    #region methods

    /// <summary>
    /// Saves the information contained in the control.
    /// </summary>
    private void SaveInfo()
    {
      if( m_patientStructure.SelectedNode == null )
        return;

      if( m_patientStructure.SelectedNode.Tag is Patient )
        m_patientInfo.Save( (Patient) m_patientStructure.SelectedNode.Tag );

      else if( m_patientStructure.SelectedNode.Tag is Study )
        m_studyInfo.Save( (Study) m_patientStructure.SelectedNode.Tag );

      else if( m_patientStructure.SelectedNode.Tag is Series )
        m_seriesInfo.Save( (Series) m_patientStructure.SelectedNode.Tag );
    } // SaveInfo()


    /// <summary>
    /// Deletes the selected patient.
    /// </summary>
    private void DeletePatient()
    {
      ListViewItem item = m_filterPatients.SelectedItems[0];
      int index = item.Index;

      item.Remove();
      if( m_filterPatients.Items.Count > index )
        m_filterPatients.Items[index].Selected = true;
      else if( m_filterPatients.Items.Count > 0 )
        m_filterPatients.Items[index-1].Selected = true;
      else
      {
        m_patientStructure.Nodes.Clear();

        m_patientInfo.Visible = false;
        m_studyInfo.Visible = false;
        m_seriesInfo.Visible = false;

        m_importButton.Enabled = false;
        m_importMenuItem.Enabled = false;
      }
    } // DeletePatient()


    #endregion

    #region event handlers

    /// <summary>
    /// Handles the import click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnImportClickHandler( object sender, EventArgs args )
    {
      SaveInfo();

      // create a collection of all filtered patients
      bool hasInvalids = false;
      ArrayList filters = new ArrayList();
      foreach( ListViewItem item in m_filterPatients.Items )
      {
        FilterEventArgs filterArgs = (FilterEventArgs) item.Tag;
        if( filterArgs.State == FilterState.Filtered )
        {
          filterArgs.Index = item.Index;
          filters.Add( filterArgs );
        }
        else
          hasInvalids &= ( filterArgs.State == FilterState.Failed ) || ( filterArgs.State == FilterState.Error );
      }

      // notify that some of the patients cannot be imported since they could not be filtered
      if( hasInvalids && Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "Some patients are invalid an will not be imported. Do you wish to continue?" ) == false )
        return;

      // import the patients and track the failures
      ArrayList failures = m_importManager.Import( filters );
      m_importSuccess |= ( failures.Count < filters.Count );

      m_filterPatients.BeginUpdate();
      foreach( FilterEventArgs filterArgs in failures )
        m_filterPatients.Items[ filterArgs.Index ].SubItems[3].Text = filterArgs.State.ToString();
      m_filterPatients.EndUpdate();

      m_importButton.Enabled = false;

      // allow the user to close the import utility if they wish
      bool isFinished = false;

      if (failures.Count > 0)
        isFinished = Viatronix.v3D.UI.MessageBox.ShowQuestion(this, "Some patients could not be imported into the system. Do you want to close the import utility?");
      else
        isFinished = Viatronix.v3D.UI.MessageBox.ShowQuestion(this, "Import complete, do you want to exit?");

      if( isFinished )
        this.DialogResult = DialogResult.OK;
    } // OnImportClickHandler( sender, args )

      
    /// <summary>
    /// Displays an ImportDialog on the drop event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An DragEventArgs that contains the event data.</param>
    private void OnFileDragDropHandler( object sender, DragEventArgs args )
    {
      m_filterManager.Filter( (string[]) args.Data.GetData( DataFormats.FileDrop, false ));
      if( m_filterPatients.Items.Count > 0 && m_filterPatients.SelectedItems.Count == 0 )
        m_filterPatients.Items[0].Selected = true;
    } // OnDragDropHandler( sender, args )


    /// <summary>
    /// Handles the drag enter event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An DragEventArgs that contains the event data.</param>
    private void OnFileDragEnterHandler( object sender, DragEventArgs args )
    {
      if( args.Data.GetDataPresent( DataFormats.FileDrop ))
        args.Effect = DragDropEffects.Copy;
      else
        args.Effect = DragDropEffects.None;
    } // OnDragEnterHandler( sender, args )


    /// <summary>
    /// Handles the filter completed event generated by the filter operation.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    private void OnFilterCompletedHandler( object sender, FilterEventArgs args )
    {

      if (this.InvokeRequired)
      {
        this.Invoke(m_filterCompleteDelegate, new object[] { args });
      }
      else
      {
        UpdateDialog(args);
      }
    } // OnFilterCompletedHandler( sender, args )


    /// <summary>
    /// Updates the dialog
    /// </summary>
    /// <param name="args">Filetr args</param>
    private void UpdateDialog(FilterEventArgs args)
    {
      ListViewItem item = new ListViewItem(args.Directory);

      if (args.State == FilterState.Filtered)
      {
        item.SubItems.Add(args.Filter.Name);
        item.SubItems.Add(args.Patient.Name.FullName);
        item.SubItems.Add("Filtered");

        m_importButton.Enabled = true;
        m_importMenuItem.Enabled = true;
        m_deleteMenuItem.Enabled = true;
      }
      else
      {
        if (args.State == FilterState.Failed)
          item.ImageIndex = 0;
        else if (args.State == FilterState.Error)
          item.ImageIndex = 1;

        item.UseItemStyleForSubItems = false;
        item.SubItems.Add("---");
        item.SubItems.Add(string.Empty);
        item.SubItems.Add("Failed");
      }

      item.Tag = args;
      item.Selected = (m_filterPatients.Items.Count == 0);
      m_filterPatients.Items.Add(item);

    } // UpdateDilaog(FilterEventArgs args)


    /// <summary>
    /// Handles the index changed event for the listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSelectedFilterChangedHandler( object sender, EventArgs args )
    {
      if( m_filterPatients.SelectedItems.Count == 0 )
      {
        m_patientStructure.ContextMenu = null;
       
        m_patientStructure.Nodes.Clear();
        m_patientInfo.Visible = false;
        m_studyInfo.Visible = false;
        m_seriesInfo.Visible = false;
        return;
      }

      m_patientStructure.ContextMenu = m_treeviewContextMenu;

      m_patientStructure.BeginUpdate();
      m_patientStructure.Nodes.Clear();
      
      // create a tree of core objects
      FilterEventArgs filterArgs = (FilterEventArgs) m_filterPatients.SelectedItems[0].Tag;
      if( filterArgs.State != FilterState.Failed )
      {
        TreeNode patientNode = new TreeNode(( filterArgs.Patient.Name.FullName == string.Empty ) ? "(Patient)" : filterArgs.Patient.Name.FullName  );
        patientNode.Tag = filterArgs.Patient;
        m_patientStructure.Nodes.Add( patientNode );
        m_patientStructure.SelectedNode = patientNode;

        foreach( Study study in filterArgs.Patient.Studies )
        {
          TreeNode studyNode = new TreeNode(( study.Description == string.Empty ) ? "(Study)" : study.Description );
          studyNode.Tag = study;
          patientNode.Nodes.Add( studyNode );

          foreach( Series series in study.Series )
          {
            TreeNode seriesNode = new TreeNode(( series.Description == string.Empty ) ? "(Series)" : series.Description );
            seriesNode.Tag = series;
            studyNode.Nodes.Add( seriesNode );
          }
        }
      }
      else
      {
        m_deleteMenuItem.Enabled = true;
        m_deletePatient.Enabled = true;
        m_deleteStudy.Enabled = false;
        m_deleteSeries.Enabled = false;
      }

      m_patientStructure.ExpandAll();
      m_patientStructure.EndUpdate();
    } // OnSelectedFilterChangedHandler( sender, args )


    /// <summary>
    /// Handles the before selection event for the import tree view.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An TreeViewCancelEventArgs that contains the event data.</param>
    private void OnBeforeSelectionChangedHandler( object sender, TreeViewCancelEventArgs args )
    {
      SaveInfo();

      FilterEventArgs filterArgs = (FilterEventArgs) m_filterPatients.SelectedItems[0].Tag;
      if( args.Node.Tag is Patient )
      {
        m_patientInfo.Visible = true;
        m_studyInfo.Visible = false;
        m_seriesInfo.Visible = false;

        m_deletePatient.Enabled = true;
        m_deleteStudy.Enabled = false;
        m_deleteSeries.Enabled = false;

        m_patientInfo.Reset();
        m_patientInfo.Initialize( (Patient) args.Node.Tag  );
      }
      else if( args.Node.Tag is Study )
      {
        m_patientInfo.Visible = false;
        m_studyInfo.Visible = true;
        m_seriesInfo.Visible = false;

        m_deletePatient.Enabled = true;
        m_deleteStudy.Enabled = true;
        m_deleteSeries.Enabled = false;

        m_studyInfo.Reset();
        m_studyInfo.Initialize( (Study) args.Node.Tag  );
      }
      else
      {
        m_patientInfo.Visible = false;
        m_studyInfo.Visible = false;
        m_seriesInfo.Visible = true;

        m_deletePatient.Enabled = true;
        m_deleteStudy.Enabled = true;
        m_deleteSeries.Enabled = true;

        m_seriesInfo.Reset();
        m_seriesInfo.Initialize( (Series) args.Node.Tag  );
      }

      // don't allow the user to be able to modified filters that are in the error state
      m_patientInfo.Enabled = ( filterArgs.State != FilterState.Error );
      m_studyInfo.Enabled   = ( filterArgs.State != FilterState.Error );
      m_seriesInfo.Enabled  = ( filterArgs.State != FilterState.Error );

      m_deleteStudy.Enabled &= ( filterArgs.State != FilterState.Error );
      m_deleteSeries.Enabled &= ( filterArgs.State != FilterState.Error );
    } // OnBeforeSelectionChangedHandler( sender, args )


    /// <summary>
    /// Handles the delete event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDeleteHandler( object sender, EventArgs args )
    {
      m_filterPatients.BeginUpdate();
      m_patientStructure.BeginUpdate();

      // delete patient
      if( sender == m_deletePatient || sender == m_deleteListViewItem )
      {
        if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "Are you sure you want to delete the patient?" ))
          DeletePatient();
      }
      else if( sender == m_deleteStudy )
      {
        TreeNode parentNode = m_patientStructure.SelectedNode.Parent;

        // delete study
        if( parentNode.Nodes.Count > 1 )
        {
          if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "Do you wish to delete the study?" ) == false )
            return;

          Patient patient = (Patient) parentNode.Parent.Tag;
          patient.Studies.Remove( (Study) m_patientStructure.SelectedNode.Tag );

          m_patientStructure.SelectedNode.Remove();
          m_patientStructure.SelectedNode = parentNode;
        }
        // delete patient
        else
        {
          if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "This operation will also delete the patient. Do you wish to continue?" ))
            DeletePatient();
        }
      }
      else if( sender == m_deleteSeries )
      {
        TreeNode studyNode = m_patientStructure.SelectedNode.Parent;
        // delete series
        if( studyNode.Nodes.Count > 1 )
        {
          if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "Do you wish to delete the series?" ) == false )
            return;

          Study study = (Study) studyNode.Tag;
          study.Series.Remove( (Series) m_patientStructure.SelectedNode.Tag );

          m_patientStructure.SelectedNode.Remove();
          m_patientStructure.SelectedNode = studyNode;
        }
        // delete study
        else if( studyNode.Parent.Nodes.Count > 1 )
        {
          if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "This operation will also delete the study. Do you wish to continue?" ))
          {
            Patient patient = (Patient) studyNode.Parent.Tag;
            patient.Studies.Remove( (Study) studyNode.Tag );

            studyNode.Remove();
            m_patientStructure.SelectedNode = m_patientStructure.Nodes[0];
          }
        }
        // delete patient
        else if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "This operation will also delete the patient. Do you wish to continue?" ))
          DeletePatient();
      }
      // delete from tree contextmenu
      else if( sender == m_deleteTreeNode )
      {
        TreeNode node = m_patientStructure.SelectedNode;
        if( node.Tag is Patient )
          OnDeleteHandler( m_deletePatient, args );
        else if( node.Tag is Study )
          OnDeleteHandler( m_deleteStudy, args );
        else
          OnDeleteHandler( m_deleteSeries, args );
      }

      m_filterPatients.EndUpdate();
      m_patientStructure.EndUpdate();

      // disable menuitem
      m_deleteMenuItem.Enabled = ( m_filterPatients.Items.Count > 0 );
    } // OnDeleteHandler( sender, args )


    /// <summary>
    /// Handles the popup event for the tree's context menu
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnContextMenuPopupHandler( object sender, EventArgs args )
    {  
      // set the active tree node before displaying context menu
      TreeNode node = m_patientStructure.GetNodeAt( m_patientStructure.PointToClient( Control.MousePosition ));
      if( node != null )
        m_patientStructure.SelectedNode = node;

      m_deleteTreeNode.Enabled = ( node != null );
    } // OnContextMenuPopupHandler( sender, args )


    /// <summary>
    /// Handles the popup event for the listview's context menu
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnListViewPopupHandler( object sender, EventArgs args )
    {
      m_deleteListViewItem.Enabled = ( m_filterPatients.Items.Count > 0 );
    } // OnListViewPopupHandler( sender, args )


    /// <summary>
    /// Handles the mouse down event for the listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An MouseEventArgs that contains the event data.</param>
    private void OnListViewMouseDownHandler( object sender, MouseEventArgs args )
    {
      // enable and disable options based on the newly selected listview item
      ListViewItem item = m_filterPatients.GetItemAt( args.X, args.Y );
      m_deleteListViewItem.Enabled = ( item != null );
    } // OnListViewMouseDownHandler( sender, args )


    /// <summary>
    /// Handles the add click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnAddClickHandler( object sender, EventArgs args )
    {
      FolderBrowserDialog dialog = new FolderBrowserDialog();
      if( dialog.ShowDialog( this ) == DialogResult.OK )
      {
        m_filterManager.Filter( dialog.SelectedPath );
        
        if( m_filterPatients.Items.Count > 0 && m_filterPatients.SelectedItems.Count == 0 )
          m_filterPatients.Items[0].Selected = true;
      }
    } // OnAddClickHandler( sender, args )


    /// <summary>
    /// Handles the close and exit click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCloseClickHandler( object sender, EventArgs args )
    {
      this.DialogResult = (( m_importSuccess ) ? DialogResult.OK : DialogResult.Cancel );
      Close();
    } // OnCloseClickHandler( sender, args )


    /// <summary>
    /// Hides the dialog when the main form is not visible.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnConsoleVisibilityChangedHandler( object sender, EventArgs args )
    {
      this.Visible = m_parent.Visible;
    } // OnConsoleVisibilityChangedHandler( sender, args )


    /// <summary>
    /// Handles the patient import event generated during the import operation.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    private void OnPatientImportHandler( object sender, FilterEventArgs args )
    {
      ListViewItem item = m_filterPatients.Items[ args.Index ];

      int stateColumnIndex = 3;
      item.SubItems[ stateColumnIndex ].Text = args.State.ToString();

      if( args.State == FilterState.Failed )
        item.ImageIndex = 0;
      else if( args.State == FilterState.Error )
        item.ImageIndex = 1;

      Invoke( new MethodInvoker( m_filterPatients.Refresh ));
    } // OnPatientImportHandler( sender, args )

 

    /// <summary>
    /// Handles the key down event for the listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnListviewKeyDownHandler( object sender, KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Delete && m_filterPatients.SelectedItems.Count > 0 )
      {
        if( Viatronix.v3D.UI.MessageBox.ShowQuestion( this, "Are you sure you want to delete the patient?" ))
          DeletePatient();
      }
    } // OnListviewKeyDownHandler( sender, args )


    /// <summary>
    /// Handles the key down event for the treeview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnTreeViewKeyDownHandler( object sender, KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Delete && m_patientStructure.SelectedNode != null )
        OnDeleteHandler( m_deleteTreeNode, args );
    } // OnListviewKeyDownHandler( sender, args )


    /// <summary>
    /// Handles the column click event for the filtered studies.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An ColumnClickEventArgs that contains the event data.</param>
    private void OnColumnClickHandler( object sender, ColumnClickEventArgs args )
    {
      if( m_filterPatients.Sorting == SortOrder.Descending || m_filterPatients.Sorting == SortOrder.None )
        m_filterPatients.Sorting = SortOrder.Ascending;
      else
        m_filterPatients.Sorting = SortOrder.Descending;

      m_filterPatients.ListViewItemSorter = new ImportDialogListViewDefaultComparer( m_filterPatients.Sorting, args.Column );
      m_filterPatients.Sort();
    } // OnColumnClickHandler( sender, args )

    #endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      this.m_mainPanel = new System.Windows.Forms.Panel();
      this.m_cancelButton = new Viatronix.v3D.UI.Button();
      this.m_importButton = new Viatronix.v3D.UI.Button();
      this.m_rightPanel = new Viatronix.v3D.UI.Panel();
      this.m_contentPanel = new System.Windows.Forms.Panel();
      this.m_leftPanel = new Viatronix.v3D.UI.Panel();
      this.m_patientStructure = new System.Windows.Forms.TreeView();
      this.m_treeviewContextMenu = new System.Windows.Forms.ContextMenu();
      this.m_deleteTreeNode = new System.Windows.Forms.MenuItem();
      this.m_topPanel = new Viatronix.v3D.UI.Panel();
      this.m_filterPatients = new System.Windows.Forms.ListView();
      this.m_directoryColumn = new System.Windows.Forms.ColumnHeader();
      this.m_typeColumn = new System.Windows.Forms.ColumnHeader();
      this.m_patientColumn = new System.Windows.Forms.ColumnHeader();
      this.m_stateColumn = new System.Windows.Forms.ColumnHeader();
      this.m_listviewContextMenu = new System.Windows.Forms.ContextMenu();
      this.m_addListViewItem = new System.Windows.Forms.MenuItem();
      this.m_deleteListViewItem = new System.Windows.Forms.MenuItem();
      this.m_deleteMenuItem = new System.Windows.Forms.MenuItem();
      this.m_deletePatient = new System.Windows.Forms.MenuItem();
      this.m_deleteStudy = new System.Windows.Forms.MenuItem();
      this.m_deleteSeries = new System.Windows.Forms.MenuItem();
      this.m_mainMenu = new System.Windows.Forms.MainMenu(this.components);
      this.m_fileMenu = new System.Windows.Forms.MenuItem();
      this.m_addMenuItem = new System.Windows.Forms.MenuItem();
      this.m_importMenuItem = new System.Windows.Forms.MenuItem();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.m_exitMenuItem = new System.Windows.Forms.MenuItem();
      this.m_mainPanel.SuspendLayout();
      this.m_rightPanel.SuspendLayout();
      this.m_leftPanel.SuspendLayout();
      this.m_topPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_mainPanel
      // 
      this.m_mainPanel.AllowDrop = true;
      this.m_mainPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                  | System.Windows.Forms.AnchorStyles.Left) 
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_mainPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
      this.m_mainPanel.Controls.Add(this.m_cancelButton);
      this.m_mainPanel.Controls.Add(this.m_importButton);
      this.m_mainPanel.Controls.Add(this.m_rightPanel);
      this.m_mainPanel.Controls.Add(this.m_leftPanel);
      this.m_mainPanel.Controls.Add(this.m_topPanel);
      this.m_mainPanel.Location = new System.Drawing.Point(0, 0);
      this.m_mainPanel.Name = "m_mainPanel";
      this.m_mainPanel.Size = new System.Drawing.Size(720, 560);
      this.m_mainPanel.TabIndex = 0;
      this.m_mainPanel.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_mainPanel.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      // 
      // m_cancelButton
      // 
      this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cancelButton.ForeColor = System.Drawing.Color.Black;
      this.m_cancelButton.Location = new System.Drawing.Point(632, 528);
      this.m_cancelButton.Name = "m_cancelButton";
      this.m_cancelButton.OldStockButton = false;
      this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
      this.m_cancelButton.TabIndex = 4;
      this.m_cancelButton.Text = "Cancel";
      this.m_cancelButton.Click += new System.EventHandler(this.OnCloseClickHandler);
      // 
      // m_importButton
      // 
      this.m_importButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_importButton.Enabled = false;
      this.m_importButton.ForeColor = System.Drawing.Color.Black;
      this.m_importButton.Location = new System.Drawing.Point(544, 528);
      this.m_importButton.Name = "m_importButton";
      this.m_importButton.OldStockButton = false;
      this.m_importButton.Size = new System.Drawing.Size(75, 23);
      this.m_importButton.TabIndex = 3;
      this.m_importButton.Text = "Import";
      this.m_importButton.Click += new System.EventHandler(this.OnImportClickHandler);
      // 
      // m_rightPanel
      // 
      this.m_rightPanel.AllowDrop = true;
      this.m_rightPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_rightPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
      this.m_rightPanel.BorderColor = System.Drawing.Color.White;
      this.m_rightPanel.BottomPadding = 0;
      this.m_rightPanel.Controls.Add(this.m_contentPanel);
      this.m_rightPanel.Curve = 10;
      this.m_rightPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
      this.m_rightPanel.LeftPadding = 0;
      this.m_rightPanel.Location = new System.Drawing.Point(232, 200);
      this.m_rightPanel.Name = "m_rightPanel";
      this.m_rightPanel.RightPadding = 10;
      this.m_rightPanel.Size = new System.Drawing.Size(488, 320);
      this.m_rightPanel.TabIndex = 2;
      this.m_rightPanel.TopPadding = 0;
      this.m_rightPanel.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_rightPanel.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      // 
      // m_contentPanel
      // 
      this.m_contentPanel.AllowDrop = true;
      this.m_contentPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                  | System.Windows.Forms.AnchorStyles.Left) 
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_contentPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
      this.m_contentPanel.Location = new System.Drawing.Point(8, 8);
      this.m_contentPanel.Name = "m_contentPanel";
      this.m_contentPanel.Size = new System.Drawing.Size(458, 304);
      this.m_contentPanel.TabIndex = 0;
      this.m_contentPanel.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_contentPanel.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      // 
      // m_leftPanel
      // 
      this.m_leftPanel.AllowDrop = true;
      this.m_leftPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
      this.m_leftPanel.BorderColor = System.Drawing.Color.White;
      this.m_leftPanel.BottomPadding = 0;
      this.m_leftPanel.Controls.Add(this.m_patientStructure);
      this.m_leftPanel.Curve = 10;
      this.m_leftPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
      this.m_leftPanel.LeftPadding = 10;
      this.m_leftPanel.Location = new System.Drawing.Point(0, 200);
      this.m_leftPanel.Name = "m_leftPanel";
      this.m_leftPanel.RightPadding = 10;
      this.m_leftPanel.Size = new System.Drawing.Size(232, 320);
      this.m_leftPanel.TabIndex = 1;
      this.m_leftPanel.TopPadding = 0;
      this.m_leftPanel.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_leftPanel.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      // 
      // m_patientStructure
      // 
      this.m_patientStructure.AllowDrop = true;
      this.m_patientStructure.ContextMenu = this.m_treeviewContextMenu;
      this.m_patientStructure.HideSelection = false;
      this.m_patientStructure.Location = new System.Drawing.Point(20, 12);
      this.m_patientStructure.Name = "m_patientStructure";
      this.m_patientStructure.Size = new System.Drawing.Size(192, 296);
      this.m_patientStructure.TabIndex = 0;
      this.m_patientStructure.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_patientStructure.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      this.m_patientStructure.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.OnBeforeSelectionChangedHandler);
      this.m_patientStructure.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnTreeViewKeyDownHandler);
      // 
      // m_treeviewContextMenu
      // 
      this.m_treeviewContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                  this.m_deleteTreeNode});
      this.m_treeviewContextMenu.Popup += new System.EventHandler(this.OnContextMenuPopupHandler);
      // 
      // m_deleteTreeNode
      // 
      this.m_deleteTreeNode.Index = 0;
      this.m_deleteTreeNode.Text = "Delete";
      this.m_deleteTreeNode.Click += new System.EventHandler(this.OnDeleteHandler);
      // 
      // m_topPanel
      // 
      this.m_topPanel.AllowDrop = true;
      this.m_topPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
      this.m_topPanel.BorderColor = System.Drawing.Color.White;
      this.m_topPanel.BottomPadding = 10;
      this.m_topPanel.Controls.Add(this.m_filterPatients);
      this.m_topPanel.Curve = 10;
      this.m_topPanel.Dock = System.Windows.Forms.DockStyle.Top;
      this.m_topPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
      this.m_topPanel.LeftPadding = 10;
      this.m_topPanel.Location = new System.Drawing.Point(0, 0);
      this.m_topPanel.Name = "m_topPanel";
      this.m_topPanel.RightPadding = 10;
      this.m_topPanel.Size = new System.Drawing.Size(720, 200);
      this.m_topPanel.TabIndex = 0;
      this.m_topPanel.TopPadding = 10;
      this.m_topPanel.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_topPanel.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      // 
      // m_filterPatients
      // 
      this.m_filterPatients.AllowDrop = true;
      this.m_filterPatients.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                  | System.Windows.Forms.AnchorStyles.Left) 
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_filterPatients.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                  this.m_directoryColumn,
                  this.m_typeColumn,
                  this.m_patientColumn,
                  this.m_stateColumn});
      this.m_filterPatients.ContextMenu = this.m_listviewContextMenu;
      this.m_filterPatients.FullRowSelect = true;
      this.m_filterPatients.GridLines = true;
      this.m_filterPatients.HideSelection = false;
      this.m_filterPatients.Location = new System.Drawing.Point(20, 20);
      this.m_filterPatients.MultiSelect = false;
      this.m_filterPatients.Name = "m_filterPatients";
      this.m_filterPatients.Size = new System.Drawing.Size(680, 160);
      this.m_filterPatients.TabIndex = 0;
      this.m_filterPatients.UseCompatibleStateImageBehavior = false;
      this.m_filterPatients.View = System.Windows.Forms.View.Details;
      this.m_filterPatients.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnFileDragEnterHandler);
      this.m_filterPatients.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnFileDragDropHandler);
      this.m_filterPatients.SelectedIndexChanged += new System.EventHandler(this.OnSelectedFilterChangedHandler);
      this.m_filterPatients.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnListviewKeyDownHandler);
      this.m_filterPatients.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.OnColumnClickHandler);
      this.m_filterPatients.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnListViewMouseDownHandler);
      // 
      // m_directoryColumn
      // 
      this.m_directoryColumn.Text = "Directory";
      this.m_directoryColumn.Width = 296;
      // 
      // m_typeColumn
      // 
      this.m_typeColumn.Text = "Type";
      this.m_typeColumn.Width = 124;
      // 
      // m_patientColumn
      // 
      this.m_patientColumn.Text = "Patient Name";
      this.m_patientColumn.Width = 175;
      // 
      // m_stateColumn
      // 
      this.m_stateColumn.Text = "State";
      this.m_stateColumn.Width = 80;
      // 
      // m_listviewContextMenu
      // 
      this.m_listviewContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                  this.m_addListViewItem,
                  this.m_deleteListViewItem});
      this.m_listviewContextMenu.Popup += new System.EventHandler(this.OnListViewPopupHandler);
      // 
      // m_addListViewItem
      // 
      this.m_addListViewItem.Index = 0;
      this.m_addListViewItem.Text = "Add...";
      this.m_addListViewItem.Click += new System.EventHandler(this.OnAddClickHandler);
      // 
      // m_deleteListViewItem
      // 
      this.m_deleteListViewItem.Index = 1;
      this.m_deleteListViewItem.Text = "Delete";
      this.m_deleteListViewItem.Click += new System.EventHandler(this.OnDeleteHandler);
      // 
      // m_deleteMenuItem
      // 
      this.m_deleteMenuItem.Enabled = false;
      this.m_deleteMenuItem.Index = 1;
      this.m_deleteMenuItem.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                  this.m_deletePatient,
                  this.m_deleteStudy,
                  this.m_deleteSeries});
      this.m_deleteMenuItem.Text = "Delete";
      // 
      // m_deletePatient
      // 
      this.m_deletePatient.Enabled = false;
      this.m_deletePatient.Index = 0;
      this.m_deletePatient.Text = "Patient";
      this.m_deletePatient.Click += new System.EventHandler(this.OnDeleteHandler);
      // 
      // m_deleteStudy
      // 
      this.m_deleteStudy.Enabled = false;
      this.m_deleteStudy.Index = 1;
      this.m_deleteStudy.Text = "Study";
      this.m_deleteStudy.Click += new System.EventHandler(this.OnDeleteHandler);
      // 
      // m_deleteSeries
      // 
      this.m_deleteSeries.Enabled = false;
      this.m_deleteSeries.Index = 2;
      this.m_deleteSeries.Text = "Series";
      this.m_deleteSeries.Click += new System.EventHandler(this.OnDeleteHandler);
      // 
      // m_mainMenu
      // 
      this.m_mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                  this.m_fileMenu});
      // 
      // m_fileMenu
      // 
      this.m_fileMenu.Index = 0;
      this.m_fileMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                  this.m_addMenuItem,
                  this.m_deleteMenuItem,
                  this.m_importMenuItem,
                  this.menuItem1,
                  this.m_exitMenuItem});
      this.m_fileMenu.Text = "File";
      // 
      // m_addMenuItem
      // 
      this.m_addMenuItem.Index = 0;
      this.m_addMenuItem.Text = "Add...";
      this.m_addMenuItem.Click += new System.EventHandler(this.OnAddClickHandler);
      // 
      // m_importMenuItem
      // 
      this.m_importMenuItem.Enabled = false;
      this.m_importMenuItem.Index = 2;
      this.m_importMenuItem.Text = "Import";
      this.m_importMenuItem.Click += new System.EventHandler(this.OnImportClickHandler);
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 3;
      this.menuItem1.Text = "-";
      // 
      // m_exitMenuItem
      // 
      this.m_exitMenuItem.Index = 4;
      this.m_exitMenuItem.Text = "Exit";
      this.m_exitMenuItem.Click += new System.EventHandler(this.OnCloseClickHandler);
      // 
      // ImportDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(720, 558);
      this.Controls.Add(this.m_mainPanel);
      this.Menu = this.m_mainMenu;
      this.Name = "ImportDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Import";
      this.m_mainPanel.ResumeLayout(false);
      this.m_rightPanel.ResumeLayout(false);
      this.m_leftPanel.ResumeLayout(false);
      this.m_topPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

  } // class ImportDialog

  #region DynamicListViewDefaultComparer class

  public class ImportDialogListViewDefaultComparer : IComparer
  {

    #region fields

    /// <summary>
    /// tracks sort order
    /// </summary>
    private SortOrder m_sortOrder = SortOrder.Ascending;

    /// <summary>
    /// index
    /// </summary>
    private int m_index = 0;

    /// <summary>
    /// comparer
    /// </summary>
    private Comparer m_comparer = new Comparer( System.Globalization.CultureInfo.CurrentCulture );

    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="order">sort order enumeration</param>
    /// <param name="index">index</param>
    public ImportDialogListViewDefaultComparer( SortOrder order, int index )
    {
      m_sortOrder = order;
      m_index = index;
    } // ImportDialogListViewDefaultComparer( order )

    #endregion

    #region IComparer Members

    /// <summary>
    /// Compares the two objects
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    public int Compare( object x, object y )
    {
      int sortOrder = ( m_sortOrder == SortOrder.Ascending ) ? 1 : -1;

      ListViewItem itemX = (ListViewItem) x;
      ListViewItem itemY = (ListViewItem) y;

      try
      {
        string objX = itemX.SubItems[ m_index ].Text;
        string objY = itemY.SubItems[ m_index ].Text;

        return sortOrder * m_comparer.Compare( objX, objY );
      }
      catch( ArgumentException )
      {
        return 0;
      }
    } // Compare( x, y )

    #endregion
    
  } // class DynamicListViewDefaultComparer


  #endregion

} // namespace Viatronix.v3D.Console


#region revision history

// $Log: ImportDialog.cs,v $
// Revision 1.6.2.2  2008/01/31 21:01:58  mkontak
// Added GROUP setting for study
//
// Revision 1.6.2.1  2007/03/19 23:45:28  mkontak
// Fix crash wjen cancelling filter and import
//
// Revision 1.6  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.5  2007/02/22 15:18:35  gdavidson
// Issue #5409: Disable delete when no items are in the list
//
// Revision 1.4  2007/02/16 00:16:40  gdavidson
// Issue #5409: Disable delete option
//
// Revision 1.3  2007/02/08 19:30:32  gdavidson
// Issue #5380: Disabled delete option when no patient is available in the listview control
//
// Revision 1.2  2006/10/30 16:17:42  gdavidson
// Modified image paths
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.30  2006/08/14 19:03:55  gdavidson
// Issue #4914: Reset the informatnion pane when the selction changes
//
// Revision 1.29  2006/07/13 14:30:49  mkontak
// Added invoke delegates fro cross thread operation
//
// Revision 1.28  2006/04/27 20:46:16  romy
// eventhandler fix
//
// Revision 1.27  2006/04/24 16:08:35  gdavidson
// Issue #4733: Made columns sortable and changed subtype to application
//
// Revision 1.26  2006/02/21 17:14:49  gdavidson
// Added ellipses to menu items that open dialogs
//
// Revision 1.25  2006/02/16 17:38:43  gdavidson
// Modified how the dialog interacts with the console.
//
// Revision 1.24  2006/02/10 21:31:57  gdavidson
// Added icons for failed an error states.
//
// Revision 1.23  2006/02/07 18:07:06  gdavidson
// Modified the import dialog
//
// Revision 1.22  2005/12/19 21:16:24  gdavidson
// Set the ImportDialog to not show in the taskbar.
//
// Revision 1.21  2005/12/14 19:56:59  gdavidson
// Commented code and implemented key board scrolling of the imported items.
//
// Revision 1.20  2005/11/24 00:07:46  gdavidson
// Added error message if a path cannot be filtered
//
// Revision 1.19  2005/11/23 20:07:31  gdavidson
// Implemented a delete patient menu item.
//
// Revision 1.18  2005/11/18 19:15:38  gdavidson
// Added exception hadling for the filtering process
//
// Revision 1.17  2005/11/18 00:43:00  gdavidson
// Fixed exception which terminated imoprt.
//
// Revision 1.16  2005/11/18 00:17:54  gdavidson
// Modified the exception hadling used during import.
//
// Revision 1.15  2005/11/17 23:13:20  gdavidson
// Added a call to save the modified node information.
//
// Revision 1.14  2005/11/17 16:28:17  gdavidson
// Fixed the import treeview to display multiple studies under a single patient.
//
// Revision 1.13  2005/11/16 22:46:09  gdavidson
// Added exception handling for the import.
//
// Revision 1.12  2005/11/16 18:12:12  mkontak
// Changed Importing ... to just Importing
//
// Revision 1.11  2005/11/16 13:54:05  gdavidson
// Fixed the images displayed in the file tree.
//
// Revision 1.10  2005/11/15 19:38:16  mkontak
// Reports back status of the importing using the Importing event
//
// Revision 1.9  2005/11/08 21:38:01  gdavidson
// Modified the IFileStructureFilter interface
//
// Revision 1.8  2005/11/07 16:36:30  gdavidson
// Modified the implementation of the import operation.
//
// Revision 1.7  2005/10/26 19:48:01  gdavidson
// Added drag drop event for adding files.
//
// Revision 1.6  2005/10/11 13:51:43  romy
// added Transfer In
//
// Revision 1.5  2005/06/24 14:56:15  gdavidson
// Added a delete menu item to the import tree.
//
// Revision 1.4  2005/05/26 17:44:22  gdavidson
// Changed the look of the dialog.
//
// Revision 1.3  2005/04/19 19:55:27  gdavidson
// Modified the IFileStructureFilter interface and commented code.
//
// Revision 1.2  2005/03/29 01:08:18  gdavidson
// Added functionality to create directories.
//
// Revision 1.1  2001/01/02 12:47:09  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/ImportDialog.cs,v 1.6.2.2 2008/01/31 21:01:58 mkontak Exp $
// $Id: ImportDialog.cs,v 1.6.2.2 2008/01/31 21:01:58 mkontak Exp $

#endregion