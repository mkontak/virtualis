// $Id: BinPane.cs,v 1.19 2006/10/30 16:24:43 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Runtime.Remoting.Messaging;
using Viatronix.v3D.UI;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{

	/// <summary>
	/// Provides the ability to track the currently selected series, and launch multiple series.
	/// </summary>
	public class BinPane : System.Windows.Forms.UserControl, ILaunchable
	{

    #region fields

    /// <summary>
    /// temporary imagelist of product logos.
    /// </summary>
    private static readonly ImageList ImageList = null;

    /// <summary>
    /// Template for the mouseover tooltip.
    /// </summary>
    private const string TooltipLayout = "Series Information\n" +
                                         "  Name:      {0}\n" +
                                         "  Study:     {1}\n" +
                                         "  Scan Date: {2}\n" + 
                                         "  Modality:  {3}\n" + 
                                         "  Type:      {4}\n" + 
                                         "  Subtype:   {5}";

    /// <summary>
    /// Displays the contents of the bin.
    /// </summary>
    private System.Windows.Forms.ListView m_listview;

    /// <summary>
    /// Provides the ability to hold multiple items in the bin.
    /// </summary>
    private Viatronix.v3D.UI.Button m_hold;

    /// <summary>
    /// Provides the ability to remove an item from the bin.
    /// </summary>
    private Viatronix.v3D.UI.Button m_remove;

    /// <summary>
    /// Contains the components that makeup the control.
    /// </summary>
    private System.ComponentModel.IContainer components;

    /// <summary>
    /// A collection of series contained in the bin.
    /// </summary>
    private SeriesCollection m_binCollection = new SeriesCollection();

    /// <summary>
    /// A collection of temporary series taht have not been held
    /// </summary>
    private ArrayList m_tempItems = new ArrayList();

    /// <summary>
    /// Provides the ability to launch the contents of the bin.
    /// </summary>
    private Viatronix.v3D.UI.Button m_launch;

    private System.Windows.Forms.ColumnHeader m_nameColumn;
    private System.Windows.Forms.ColumnHeader m_studyColumn;
    private System.Windows.Forms.ColumnHeader m_scanDateColumn;
    private System.Windows.Forms.ColumnHeader m_modalityColumn;
    private System.Windows.Forms.ColumnHeader m_typeColumn;
    private System.Windows.Forms.ColumnHeader m_subtypeColumn;
    private Viatronix.v3D.UI.Button m_transfer; 

    /// <summary>
    /// Reference to a launch manager.
    /// </summary>
    private LaunchManager m_launchManager = null;

    #endregion

    #region construction

    /// <summary>
    /// Loads the temporary logo images into the image list.
    /// </summary>
    static BinPane()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
    
      ImageList = new ImageList();
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-ang.png" )));
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-cs.png" )));
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-ln.png" )));
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-vas.png" )));
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-vc.png" )));     
      ImageList.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.v3D.Console.res.logo-xp.png" )));     
      ImageList.ImageSize = new Size( 32, 32 );
    } // BinPane()


    /// <summary>
    /// Creates a new instance of a BinPane control.
    /// </summary>
		public BinPane()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.BackColor = Theme.PaneBackColor;
      
			InitializeComponent();

      m_listview.SmallImageList = ImageList;
      m_listview.LargeImageList = ImageList;

      Global.Frame.ViewChanged += new EventHandler( this.OnViewChangedHandler );
      if( Global.Frame.ActiveView is ViewBrowser )
      {
        ViewBrowser browser = (ViewBrowser) Global.Frame.ActiveView;
        browser.SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
//        OnSeriesChangedHandler( browser.SelectedSeries );        
      }

      m_launchManager = new LaunchManager( Global.Frame, (IDataProvider) Global.Frame.ViewState[ CoreState.DataProvider ]);
		} // BinPane()

    #endregion

    #region properties

    /// <summary>
    /// Returns a reference to the ViewBrowser.
    /// </summary>
    private ViewBrowser ViewBrowser
    {
      get { return (ViewBrowser) Global.Frame.ActiveView; }
    } // ViewBrowser

    #endregion

    #region methods

    /// <summary>
    /// Returns the tooltip text for the specified series.
    /// </summary>
    /// <param name="series">The series being displayed.</param>
    /// <returns>The tooltip text for the specified series.</returns>
    private string CreateTooltipText( Series series )
    {
      return string.Format( TooltipLayout, series.Study.Patient.Name.ToString(), series.Study.Description, series.Study.Date.ToShortDateString(), series.Modality, series.Type, series.SubType );
    } // CreateTooltipText( series )


    /// <summary>
    /// Returns the image index for the specified series.
    /// </summary>
    /// <param name="series">The series being displayed.</param>
    /// <returns>The image index for the specified series.</returns>
    private int GetImageIndex( Series series )
    {
      Image image = this.ViewBrowser.DataProvider.GetPreviewImage( series );
      if( image != null )
      {
        m_listview.SmallImageList.Images.Add( image );
        m_listview.LargeImageList.Images.Add( image );
        return m_listview.LargeImageList.Images.Count - 1;
      }

      switch( series.SubType.ToLower() )
      {
        case "ang": return 0;
        case "cs":  return 1;
        case "ln":  return 2;
        case "vas": return 3;
        case "vc":  return 4;
        case "xp":  return 5;
        default:    return 5;
      }
    } // GetImageIndex( series )

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
        ((ViewBrowser) Global.Frame.ActiveView ).SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
    } // OnViewChangedHandler( sender, args )


    /// <summary>
    /// Adds the specified series to the bin.
    /// </summary>
    /// <param name="series">The currently selected series in the ViewBrowser.</param>
    private void OnSeriesChangedHandler( SeriesCollection collection )
    {
      if( collection == null )
        return;

      foreach( ListViewItem item in m_tempItems )
        item.Remove();

      foreach( Series series in collection )
      {
        // if the specified series is already in the bin, select it
        if( m_binCollection.Contains( series ))
        {
          int index = m_binCollection.IndexOf( series );
          ListViewItem selectedItem = m_listview.Items[ m_binCollection.IndexOf( series )];
          selectedItem.Selected = true;
          continue;
        }

        // create a listview item and select it
        ListViewItem item = new ListViewItem( series.Description );

        item.ImageIndex = GetImageIndex( series );
        item.SubItems.Add( series.Study.Patient.Name.ToString() );
        item.SubItems.Add( series.Study.Date.ToShortDateString() );
        item.SubItems.Add( series.Modality );
        item.SubItems.Add( series.Type );
        item.SubItems.Add( series.SubType );
        item.Tag = series;

        item.Selected = true;
      
        m_tempItems.Add( item );
        m_listview.Items.Add( item );
      
        m_listview.EnsureVisible( item.Index );
      }
    } // OnSeriesChangedHandler( series )


    /// <summary>
    /// Launches the contents of the bin.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLaunchClickHandler( object sender, EventArgs args )
    {
      m_launchManager.Launch( m_binCollection );
    } // OnLaunchClickHandler( sender, args )


    /// <summary>
    /// Allows multiple series to be added to the bin.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnHoldClickHandler( object sender, EventArgs args )
    {
      m_hold.Enabled = false;  
      m_remove.Enabled = true;       

      foreach( ListViewItem item in m_listview.SelectedItems )
      {
        if( m_tempItems.Contains( item ))
        {
          Series series = (Series) item.Tag;
          if( m_binCollection.Contains( series ) == false )
            m_binCollection.Add( series );

          m_tempItems.Remove( item );
        }        
      }
    } // OnHoldClickHandler( sender, args )


    /// <summary>
    /// Removes the selected item from the bin.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnRemoveClickHandler( object sender, EventArgs args )
    {
      foreach( ListViewItem item in m_listview.SelectedItems )
      {
        Series series = (Series) item.Tag;
        m_binCollection.Remove( series );

        m_tempItems.Remove( item );

        item.Remove();
      }
      
      m_remove.Enabled = false;
      m_hold.Enabled = false;
    } // OnRemoveClickHandler( sender, args )


    /// <summary>
    /// Handles the selection changed event for the listview containing the bin items.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSelectionChangedHandler( object sender, EventArgs args )
    {
      if( m_listview.SelectedItems.Count == 0 )
        return;

      m_remove.Enabled = true;

      foreach( ListViewItem item in m_listview.SelectedItems )
      {
        if( m_tempItems.Contains( item ))
        {
          m_hold.Enabled = true;
          return;
        }
      }

      m_hold.Enabled = false;
    } // OnSelectionChangedHandler( sender, args )


    /// <summary>
    /// Handles the Transfer Click Event Handler.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnTransferClickHandler(object sender, System.EventArgs args)
    {
//      if ( m_collection == null ) return;
//      
//      //Get the Transfer details from the user.
//      TransferDialog transferDialog = new TransferDialog();
//
//      transferDialog.Add ( ((ViewBrowser) Global.Frame.ActiveView ).TransferProviders );
//      
//      if( transferDialog.ShowDialog() != DialogResult.OK )
//        return;
//
//      this.Cursor = Cursors.WaitCursor;
//
//      TransferProviderDetails transferProviderDetails = transferDialog.TransferProviderDetails;
//      transferDialog.Dispose();
//
//      SeriesCollection collection = new SeriesCollection();
//      IDataProvider provider = ((ViewBrowser) Global.Frame.ActiveView ).DataProvider;
//      for( int i = 0; i < m_collection.Count; ++i )
//      {
//        Series series = m_collection[i];
//        provider.RetrieveData( series, true );
//        collection.Add( series );
//      }
//            
//      this.Cursor = Cursors.Default;
//
//      //Transfer Selected study.
//      TransferManager transferManager = new TransferManager( transferProviderDetails );
//      transferManager.Transfer( collection );
    }//OnTransferClickHandler(object sender, System.EventArgs e)


    #endregion

    #region ILaunchable Members

    /// <summary>
    /// Gets or sets a reference to a launch manager.
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
      this.components = new System.ComponentModel.Container();
      this.m_listview = new System.Windows.Forms.ListView();
      this.m_studyColumn = new System.Windows.Forms.ColumnHeader();
      this.m_nameColumn = new System.Windows.Forms.ColumnHeader();
      this.m_scanDateColumn = new System.Windows.Forms.ColumnHeader();
      this.m_modalityColumn = new System.Windows.Forms.ColumnHeader();
      this.m_typeColumn = new System.Windows.Forms.ColumnHeader();
      this.m_subtypeColumn = new System.Windows.Forms.ColumnHeader();
      this.m_hold = new Viatronix.v3D.UI.Button();
      this.m_remove = new Viatronix.v3D.UI.Button();
      this.m_launch = new Viatronix.v3D.UI.Button();
      this.m_transfer = new Viatronix.v3D.UI.Button();
      this.SuspendLayout();
      // 
      // m_listview
      // 
      this.m_listview.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_listview.BackColor = System.Drawing.SystemColors.Window;
      this.m_listview.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                 this.m_studyColumn,
                                                                                 this.m_nameColumn,
                                                                                 this.m_scanDateColumn,
                                                                                 this.m_modalityColumn,
                                                                                 this.m_typeColumn,
                                                                                 this.m_subtypeColumn});
      this.m_listview.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_listview.FullRowSelect = true;
      this.m_listview.GridLines = true;
      this.m_listview.HideSelection = false;
      this.m_listview.Location = new System.Drawing.Point(4, 4);
      this.m_listview.MultiSelect = false;
      this.m_listview.Name = "m_listview";
      this.m_listview.Size = new System.Drawing.Size(290, 212);
      this.m_listview.TabIndex = 0;
      this.m_listview.View = System.Windows.Forms.View.Details;
      this.m_listview.SelectedIndexChanged += new System.EventHandler(this.OnSelectionChangedHandler);
      // 
      // m_studyColumn
      // 
      this.m_studyColumn.Text = "Description";
      this.m_studyColumn.Width = 200;
      // 
      // m_nameColumn
      // 
      this.m_nameColumn.Text = "Name";
      this.m_nameColumn.Width = 133;
      // 
      // m_scanDateColumn
      // 
      this.m_scanDateColumn.Text = "Scan Date";
      this.m_scanDateColumn.Width = 94;
      // 
      // m_modalityColumn
      // 
      this.m_modalityColumn.Text = "Modality";
      // 
      // m_typeColumn
      // 
      this.m_typeColumn.Text = "Type";
      // 
      // m_subtypeColumn
      // 
      this.m_subtypeColumn.Text = "Subtype";
      // 
      // m_hold
      // 
      this.m_hold.CustomImages = new System.Drawing.Bitmap[] {
                                                               null,
                                                               null,
                                                               null};
      this.m_hold.ForeColor = System.Drawing.Color.Black;
      this.m_hold.Location = new System.Drawing.Point(4, 224);
      this.m_hold.Name = "m_hold";
      this.m_hold.OldStockButton = false;
      this.m_hold.Size = new System.Drawing.Size(60, 23);
      this.m_hold.TabIndex = 1;
      this.m_hold.Text = "Hold";
      this.m_hold.Click += new System.EventHandler(this.OnHoldClickHandler);
      // 
      // m_remove
      // 
      this.m_remove.CustomImages = new System.Drawing.Bitmap[] {
                                                                 null,
                                                                 null,
                                                                 null};
      this.m_remove.Enabled = false;
      this.m_remove.ForeColor = System.Drawing.Color.Black;
      this.m_remove.Location = new System.Drawing.Point(72, 224);
      this.m_remove.Name = "m_remove";
      this.m_remove.OldStockButton = false;
      this.m_remove.Size = new System.Drawing.Size(64, 23);
      this.m_remove.TabIndex = 2;
      this.m_remove.Text = "Remove";
      this.m_remove.Click += new System.EventHandler(this.OnRemoveClickHandler);
      // 
      // m_launch
      // 
      this.m_launch.CustomImages = new System.Drawing.Bitmap[] {
                                                                 null,
                                                                 null,
                                                                 null};
      this.m_launch.ForeColor = System.Drawing.Color.Black;
      this.m_launch.Location = new System.Drawing.Point(221, 224);
      this.m_launch.Name = "m_launch";
      this.m_launch.OldStockButton = false;
      this.m_launch.TabIndex = 3;
      this.m_launch.Text = "Launch";
      this.m_launch.Click += new System.EventHandler(this.OnLaunchClickHandler);
      // 
      // m_transfer
      // 
      this.m_transfer.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_transfer.ForeColor = System.Drawing.Color.Black;
      this.m_transfer.Location = new System.Drawing.Point(152, 224);
      this.m_transfer.Name = "m_transfer";
      this.m_transfer.OldStockButton = false;
      this.m_transfer.Size = new System.Drawing.Size(56, 24);
      this.m_transfer.TabIndex = 4;
      this.m_transfer.Text = "Transfer";
      this.m_transfer.Click += new System.EventHandler(this.OnTransferClickHandler);
      // 
      // BinPane
      // 
      this.Controls.Add(this.m_transfer);
      this.Controls.Add(this.m_launch);
      this.Controls.Add(this.m_remove);
      this.Controls.Add(this.m_hold);
      this.Controls.Add(this.m_listview);
      this.Name = "BinPane";
      this.Size = new System.Drawing.Size(300, 250);
      this.ResumeLayout(false);

    }
		#endregion 

  } // class BinPane
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: BinPane.cs,v $
// Revision 1.19  2006/10/30 16:24:43  gdavidson
// Modified LaunchManager's constructor
//
// Revision 1.18  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.17  2005/12/14 19:53:39  gdavidson
// Modified the bin to work with multiple series selection.
//
// Revision 1.16  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.15  2005/11/16 14:05:46  gdavidson
// Allowed multiselect for series.
//
// Revision 1.14  2005/10/27 19:10:59  romy
// commecnted transfer
//
// Revision 1.13  2005/10/25 15:44:28  romy
// Added TransferItem Collection
//
// Revision 1.12  2005/10/18 17:49:47  gdavidson
// Implemented the ILaunchable interface.
//
// Revision 1.11  2005/10/11 13:45:34  romy
// transfer change
//
// Revision 1.10  2005/10/05 12:33:20  mkontak
// Changed DataProvider provider to IDataProvider provider
//
// Revision 1.9  2005/10/03 15:08:24  romy
// modified Transfer Providers
//
// Revision 1.8  2005/09/26 14:04:52  gdavidson
// Added a check for null in the event handler.
//
// Revision 1.7  2005/09/20 12:20:04  romy
// Demo check in
//
// Revision 1.6  2005/08/31 14:02:03  gdavidson
// Added call to the DataProvider's RetrieveData method.
//
// Revision 1.5  2005/08/02 12:39:29  gdavidson
// Added launch methods for viewers and reports.
//
// Revision 1.4  2005/06/24 14:50:48  gdavidson
// Changed the listview to display in Details mode.
//
// Revision 1.3  2005/06/16 13:01:50  gdavidson
// Retrieves the appropriate launcher from a section in the configuration file.
//
// Revision 1.2  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.1  2005/05/26 17:46:12  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console/panes/BinPane.cs,v 1.19 2006/10/30 16:24:43 gdavidson Exp $
// $Id: BinPane.cs,v 1.19 2006/10/30 16:24:43 gdavidson Exp $

#endregion