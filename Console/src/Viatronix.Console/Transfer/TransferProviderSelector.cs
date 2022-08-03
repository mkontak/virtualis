// $Id: TransferProviderSelector.cs,v 1.23.2.19 2011/05/18 01:56:59 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.IO;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.v3D.Core;
using Viatronix.UI;
using System.Data;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.Console
{
  /// <summary>
  /// TransferDialog let the user Pick Items for Transfer from a Series Collection.
  /// It Builds a collection of Series and its Selected Transfer Items
  /// </summary>
  public class TransferDialog : Viatronix.UI.Dialog
  {
    #region fields

    private IDataProvider m_dataProvider;

    /// <summary>
    /// Selected series if there is one
    /// </summary>
    private Series m_selectedSeries = null;

    /// <summary>
    /// Transfer Details object
    /// </summary>
    private TransferProvider m_selectedTransferProvider = null;

    /// <summary>
    /// List for Transfer Providers
    /// </summary>
    private System.Collections.ArrayList m_transferProvidersList = new ArrayList();


    /// <summary>
    /// Series Collection 
    /// </summary>
    private List<Series> m_seriesCollection;

    /// <summary>
    /// Transfer Button
    /// </summary>
    private Viatronix.UI.Button m_transfer;

    /// <summary>
    /// Cancel Button
    /// </summary>
    private Viatronix.UI.Button m_cancel;

    /// <summary>
    /// Selected Transfer Data hash Table.
    /// </summary>
    private System.Collections.Hashtable m_selectedTransferData = new Hashtable();

    /// <summary>
    /// Anonymization Override check box.
    /// </summary>
    private Viatronix.UI.CheckBox m_overrideCheckBox;

    /// <summary>
    /// Anonymize check box
    /// </summary>
    private Viatronix.UI.CheckBox m_anonimizeCheckbox;

    /// <summary>
    /// To label
    /// </summary>
    private System.Windows.Forms.Label m_transferToLabel;

    /// <summary>
    /// To label
    /// </summary>
    private System.Windows.Forms.Label m_pacsServerLabel;

    /// <summary>
    /// Transfer providers combo box.
    /// </summary>
    private Viatronix.UI.ComboBox m_transferProviders;

    /// <summary>
    /// study Tree View
    /// </summary>
    private System.Windows.Forms.TreeView m_studyTree;

    /// <summary>
    /// Transfer Items check list box
    /// </summary>
    private System.Windows.Forms.CheckedListBox m_transferItemsListBox;

    /// <summary>
    /// group box
    /// </summary>
    private System.Windows.Forms.GroupBox m_seriesGroupBox;

    /// <summary>
    /// Dicom servers combo box
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxDicomServers;

    /// <summary>
    /// Label for DICOM Server
    /// </summary>
    private System.Windows.Forms.Label m_labelDicomServer;

    /// <summary>
    /// Dest Group Box
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBoxDest;
    private System.Windows.Forms.Button m_pathNavigatorButton;
    private TextBox m_pathSpec;
    private Label m_transferFSLabel;
    private ToolTip toolTip1;
    private IContainer components;

    /// <summary>
    /// PACS Selection control
    /// </summary>
    private Viatronix.v3D.Dicom.UI.PacsSelectionControl m_pacsSelectionControl;

    #endregion

    #region properties

    /// <summary>
    /// Gtes the TransferDetails
    /// </summary>
    public TransferProvider SelectedTransferProvider
    {
      get { return m_selectedTransferProvider; }
      set { m_selectedTransferProvider = value; }
    }//TransferProviderDetails


    /// <summary>
    /// Gets the Selected Transfer Data
    /// </summary>
    public System.Collections.Hashtable SelectedTransferData
    {
      get { return m_selectedTransferData; }
    }//SelectedTransferData

    #endregion

    #region construction

    /// <summary>
    /// Default constructor
    /// </summary>
    protected TransferDialog()
    {
      InitializeComponent();
    }//TransferDialog()


    /// <summary>
    /// Transfer Dialog args constructor
    /// </summary>
    /// <param name="transferProvidersList">ArrayList</param>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="supportedTransferItems">TransferItemsCollection</param>
    public TransferDialog(IDataProvider dataProvider, ArrayList transferProvidersList, List<Series> seriesCollection, Series selectedSeries)
      : this()
    {
      m_dataProvider = dataProvider;
      m_selectedSeries = selectedSeries;

      // TODOL=: New (Is the colne of the collection really needed? )
      //m_seriesCollection = ( List<Series> )seriesCollection.Clone();
      m_seriesCollection = seriesCollection;
      m_transferProvidersList = transferProvidersList;

      bool hasDicomSeries = false;

      foreach( Series series in m_seriesCollection )
      {
        if( series.Type == SeriesTypes.Dicom) 
        {
          hasDicomSeries = true;
          break;
        }
      }

      if( !hasDicomSeries )
      {
        this.m_anonimizeCheckbox.Enabled = false;
      }

      AddTransferProviders();

    }//TransferDialog( ArrayList transferProvidersList, List<Series> seriesCollection, TransferItemsCollection supportedTransferItems )


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if( disposing )
      {
        if( components != null )
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }//Dispose( bool disposing )

    #endregion

    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.m_transfer = new Viatronix.UI.Button();
      this.m_cancel = new Viatronix.UI.Button();
      this.m_overrideCheckBox = new Viatronix.UI.CheckBox();
      this.m_anonimizeCheckbox = new Viatronix.UI.CheckBox();
      this.m_transferToLabel = new System.Windows.Forms.Label();
      this.m_pacsServerLabel = new System.Windows.Forms.Label();
      this.m_transferProviders = new Viatronix.UI.ComboBox();
      this.m_studyTree = new System.Windows.Forms.TreeView();
      this.m_transferItemsListBox = new System.Windows.Forms.CheckedListBox();
      this.m_seriesGroupBox = new System.Windows.Forms.GroupBox();
      this.m_groupBoxDest = new System.Windows.Forms.GroupBox();
      this.m_transferFSLabel = new System.Windows.Forms.Label();
      this.m_pathNavigatorButton = new System.Windows.Forms.Button();
      this.m_pathSpec = new System.Windows.Forms.TextBox();

      // TODO REST Fix
      this.m_pacsSelectionControl = new Viatronix.v3D.Dicom.UI.PacsSelectionControl(Global.DataProvider);
      this.m_labelDicomServer = new System.Windows.Forms.Label();
      this.m_comboBoxDicomServers = new System.Windows.Forms.ComboBox();
      this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
      this.m_seriesGroupBox.SuspendLayout();
      this.m_groupBoxDest.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_pacsSelectionControl
      // 
      this.m_pacsSelectionControl.BackColor = System.Drawing.Color.Transparent;
      this.m_pacsSelectionControl.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( System.Byte )( 0 ) ));
      this.m_pacsSelectionControl.ForeColor = System.Drawing.Color.White;
      this.m_pacsSelectionControl.Location = new System.Drawing.Point(99, 49);
      this.m_pacsSelectionControl.Name = "m_pacsSelectionControl";
      this.m_pacsSelectionControl.ServiceType = DicomServiceTypes.Store;
      this.m_pacsSelectionControl.Size = new System.Drawing.Size(149, 22);
      this.m_pacsSelectionControl.TabIndex = 12;
      this.toolTip1.SetToolTip(this.m_pacsSelectionControl, "Select the PACS server to transfer to.");

      // 
      // m_transfer
      // 
      this.m_transfer.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_transfer.ForeColor = System.Drawing.Color.Black;
      this.m_transfer.Location = new System.Drawing.Point(324, 326);
      this.m_transfer.Name = "m_transfer";
      this.m_transfer.OldStockButton = false;
      this.m_transfer.Size = new System.Drawing.Size(67, 23);
      this.m_transfer.TabIndex = 5;
      this.m_transfer.Text = "Transfer";
      this.toolTip1.SetToolTip(this.m_transfer, "Click to begin transfer.");
      this.m_transfer.Click += new System.EventHandler(this.OnTransferClickHandler);
      // 
      // m_cancel
      // 
      this.m_cancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancel.ForeColor = System.Drawing.Color.Black;
      this.m_cancel.Location = new System.Drawing.Point(404, 326);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.Size = new System.Drawing.Size(67, 23);
      this.m_cancel.TabIndex = 6;
      this.m_cancel.Text = "Cancel";
      this.toolTip1.SetToolTip(this.m_cancel, "Click to cancel transfer.");
      this.m_cancel.Click += new System.EventHandler(this.OnCancelHandler);
      // 
      // m_overrideCheckBox
      // 
      this.m_overrideCheckBox.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_overrideCheckBox.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_overrideCheckBox.Font = new System.Drawing.Font("Arial", 8F);
      this.m_overrideCheckBox.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_overrideCheckBox.Location = new System.Drawing.Point(352, 20);
      this.m_overrideCheckBox.Name = "m_overrideCheckBox";
      this.m_overrideCheckBox.Size = new System.Drawing.Size(70, 22);
      this.m_overrideCheckBox.TabIndex = 6;
      this.m_overrideCheckBox.Text = "Override";
      this.toolTip1.SetToolTip(this.m_overrideCheckBox, "Select to override the predefined anonymization");
      this.m_overrideCheckBox.UseVisualStyleBackColor = false;
      // 
      // m_anonimizeCheckbox
      // 
      this.m_anonimizeCheckbox.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_anonimizeCheckbox.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_anonimizeCheckbox.Font = new System.Drawing.Font("Arial", 8F);
      this.m_anonimizeCheckbox.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_anonimizeCheckbox.Location = new System.Drawing.Point(263, 20);
      this.m_anonimizeCheckbox.Name = "m_anonimizeCheckbox";
      this.m_anonimizeCheckbox.Size = new System.Drawing.Size(83, 22);
      this.m_anonimizeCheckbox.TabIndex = 5;
      this.m_anonimizeCheckbox.Tag = "Anonymizes the selected series on transfer.";
      this.m_anonimizeCheckbox.Text = "Anonymize";
      this.toolTip1.SetToolTip(this.m_anonimizeCheckbox, "Select to anonymize transferred data");
      this.m_anonimizeCheckbox.UseVisualStyleBackColor = false;
      this.m_anonimizeCheckbox.Click += new System.EventHandler(this.OnAnonymizeClickHandler);
      // 
      // m_transferToLabel
      // 
      this.m_transferToLabel.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_transferToLabel.Font = new System.Drawing.Font("Arial", 8F);
      this.m_transferToLabel.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_transferToLabel.Location = new System.Drawing.Point(10, 20);
      this.m_transferToLabel.Name = "m_transferToLabel";
      this.m_transferToLabel.Size = new System.Drawing.Size(43, 22);
      this.m_transferToLabel.TabIndex = 4;
      this.m_transferToLabel.Text = "To:";
      this.m_transferToLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_pacsServerLabel
      // 
      this.m_pacsServerLabel.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_pacsServerLabel.Font = new System.Drawing.Font("Arial", 8F);
      this.m_pacsServerLabel.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_pacsServerLabel.Location = new System.Drawing.Point(10, 49);
      this.m_pacsServerLabel.Name = "m_pacsServerLabel";
      this.m_pacsServerLabel.Size = new System.Drawing.Size(75, 22);
      this.m_pacsServerLabel.TabIndex = 4;
      this.m_pacsServerLabel.Text = "PACS Server:";
      this.m_pacsServerLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_transferProviders
      // 
      this.m_transferProviders.DisplayMember = "String";
      this.m_transferProviders.Font = new System.Drawing.Font("Arial", 8F);
      this.m_transferProviders.ItemHeight = 14;
      this.m_transferProviders.Location = new System.Drawing.Point(99, 23);
      this.m_transferProviders.Name = "m_transferProviders";
      this.m_transferProviders.Size = new System.Drawing.Size(149, 22);
      this.m_transferProviders.TabIndex = 3;
      this.m_transferProviders.ValueMember = "Object";
      this.m_transferProviders.SelectedIndexChanged += new System.EventHandler(this.OnSelectedIndexChangedHandler);
      // 
      // m_studyTree
      // 
      this.m_studyTree.Font = new System.Drawing.Font("Arial", 8F);
      this.m_studyTree.FullRowSelect = true;
      this.m_studyTree.HideSelection = false;
      this.m_studyTree.Location = new System.Drawing.Point(6, 14);
      this.m_studyTree.Name = "m_studyTree";
      this.m_studyTree.Size = new System.Drawing.Size(222, 199);
      this.m_studyTree.TabIndex = 1;
      this.toolTip1.SetToolTip(this.m_studyTree, "Select the series.");
      this.m_studyTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.OnAfterSelectHandler);
      this.m_studyTree.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.OnBeforeSelectHandler);
      // 
      // m_transferItemsListBox
      // 
      this.m_transferItemsListBox.CheckOnClick = true;
      this.m_transferItemsListBox.Font = new System.Drawing.Font("Arial", 8F);
      this.m_transferItemsListBox.Location = new System.Drawing.Point(235, 14);
      this.m_transferItemsListBox.Name = "m_transferItemsListBox";
      this.m_transferItemsListBox.Size = new System.Drawing.Size(221, 199);
      this.m_transferItemsListBox.TabIndex = 4;
      this.toolTip1.SetToolTip(this.m_transferItemsListBox, "Select items to be enabled or disabled for transfer.");
      // 
      // m_seriesGroupBox
      // 
      this.m_seriesGroupBox.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_seriesGroupBox.Controls.Add(this.m_studyTree);
      this.m_seriesGroupBox.Controls.Add(this.m_transferItemsListBox);
      this.m_seriesGroupBox.Font = new System.Drawing.Font("Arial", 8F);
      this.m_seriesGroupBox.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_seriesGroupBox.Location = new System.Drawing.Point(7, 95);
      this.m_seriesGroupBox.Name = "m_seriesGroupBox";
      this.m_seriesGroupBox.Size = new System.Drawing.Size(463, 222);
      this.m_seriesGroupBox.TabIndex = 9;
      this.m_seriesGroupBox.TabStop = false;
      this.m_seriesGroupBox.Text = "Select Items";
      // 
      // m_groupBoxDest
      // 
      this.m_groupBoxDest.BackColor = System.Drawing.Color.FromArgb(( ( int )( ( ( byte )( 123 ) ) ) ), ( ( int )( ( ( byte )( 134 ) ) ) ), ( ( int )( ( ( byte )( 165 ) ) ) ));
      this.m_groupBoxDest.Controls.Add(this.m_transferFSLabel);
      this.m_groupBoxDest.Controls.Add(this.m_pathNavigatorButton);
      this.m_groupBoxDest.Controls.Add(this.m_pathSpec);
      this.m_groupBoxDest.Controls.Add(this.m_labelDicomServer);
      this.m_groupBoxDest.Controls.Add(this.m_comboBoxDicomServers);
      this.m_groupBoxDest.Controls.Add(this.m_overrideCheckBox);
      this.m_groupBoxDest.Controls.Add(this.m_anonimizeCheckbox);
      this.m_groupBoxDest.Controls.Add(this.m_transferToLabel);
      this.m_groupBoxDest.Controls.Add(this.m_pacsServerLabel);
      this.m_groupBoxDest.Controls.Add(this.m_transferProviders);
      this.m_groupBoxDest.Controls.Add(this.m_pacsSelectionControl);
      this.m_groupBoxDest.Font = new System.Drawing.Font("Arial", 8F);
      this.m_groupBoxDest.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_groupBoxDest.Location = new System.Drawing.Point(7, 7);
      this.m_groupBoxDest.Name = "m_groupBoxDest";
      this.m_groupBoxDest.Size = new System.Drawing.Size(463, 80);
      this.m_groupBoxDest.TabIndex = 7;
      this.m_groupBoxDest.TabStop = false;
      // 
      // m_transferFSLabel
      // 
      this.m_transferFSLabel.AutoSize = true;
      this.m_transferFSLabel.Location = new System.Drawing.Point(10, 53);
      this.m_transferFSLabel.Name = "m_transferFSLabel";
      this.m_transferFSLabel.Size = new System.Drawing.Size(67, 14);
      this.m_transferFSLabel.TabIndex = 10;
      this.m_transferFSLabel.Text = "Transfer To:";
      this.m_transferFSLabel.Visible = false;
      // 
      // m_pathNavigatorButton
      // 
      this.m_pathNavigatorButton.Location = new System.Drawing.Point(428, 49);
      this.m_pathNavigatorButton.Name = "m_pathNavigatorButton";
      this.m_pathNavigatorButton.Size = new System.Drawing.Size(28, 23);
      this.m_pathNavigatorButton.TabIndex = 12;
      this.m_pathNavigatorButton.Text = "...";
      this.m_pathNavigatorButton.ForeColor = Color.Black;
      this.toolTip1.SetToolTip(this.m_pathNavigatorButton, "Select The Output Directory");
      this.m_pathNavigatorButton.UseVisualStyleBackColor = true;
      this.m_pathNavigatorButton.Visible = false;
      this.m_pathNavigatorButton.Click += new System.EventHandler(this.OnPathNavigatorClickedHandler);
      // 
      // m_pathSpec
      // 
      this.m_pathSpec.Location = new System.Drawing.Point(99, 51);
      this.m_pathSpec.Name = "m_pathSpec";
      this.m_pathSpec.Size = new System.Drawing.Size(323, 20);
      this.m_pathSpec.TabIndex = 10;
      this.toolTip1.SetToolTip(this.m_pathSpec, "Specify The Directory To Transfer To");
      this.m_pathSpec.Visible = false;
      // 
      // m_labelDicomServer
      // 
      this.m_labelDicomServer.Location = new System.Drawing.Point(8, 47);
      this.m_labelDicomServer.Name = "m_labelDicomServer";
      this.m_labelDicomServer.Size = new System.Drawing.Size(82, 23);
      this.m_labelDicomServer.TabIndex = 11;
      this.m_labelDicomServer.Text = "Dicom Server:";
      this.m_labelDicomServer.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      this.m_labelDicomServer.Visible = false;
      // 
      // m_comboBoxDicomServers
      // 
      this.m_comboBoxDicomServers.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxDicomServers.Location = new System.Drawing.Point(98, 49);
      this.m_comboBoxDicomServers.Name = "m_comboBoxDicomServers";
      this.m_comboBoxDicomServers.Size = new System.Drawing.Size(354, 22);
      this.m_comboBoxDicomServers.TabIndex = 10;
      this.toolTip1.SetToolTip(this.m_comboBoxDicomServers, "Select The Dicom Server To Transfer To");
      this.m_comboBoxDicomServers.Visible = false;
      this.m_comboBoxDicomServers.SelectedIndexChanged += new System.EventHandler(this.OnDicomServerSelected);
      // 
      // toolTip1
      // 
      this.toolTip1.Tag = "";
      // 
      // TransferDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(477, 363);
      this.Controls.Add(this.m_groupBoxDest);
      this.Controls.Add(this.m_cancel);
      this.Controls.Add(this.m_transfer);
      this.Controls.Add(this.m_seriesGroupBox);
      this.Name = "TransferDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Transfer";
      this.Controls.SetChildIndex(this.m_seriesGroupBox, 0);
      this.Controls.SetChildIndex(this.m_transfer, 0);
      this.Controls.SetChildIndex(this.m_cancel, 0);
      this.Controls.SetChildIndex(this.m_groupBoxDest, 0);
      this.m_seriesGroupBox.ResumeLayout(false);
      this.m_groupBoxDest.ResumeLayout(false);
      this.m_groupBoxDest.PerformLayout();
      this.ResumeLayout(false);

    }
    #endregion

    #region events

    /// <summary>
    /// Handles the After Select Event.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">TreeViewEventArgs</param>
    private void OnAfterSelectHandler(object sender, System.Windows.Forms.TreeViewEventArgs args)
    {
      TreeNode node = m_studyTree.SelectedNode;
      Type type = node.Tag.GetType();

      switch( type.Name )
      {
        case "Series":
          {
            Series series = ( Series)node.Tag;
            AddTransferItems(series);
            ShowCurrentStatusOfItems(series);
            break;
          }
        case "Study":
          {
            m_transferItemsListBox.Items.Clear();
            break;
          }
        case "Patient":
          {
            m_transferItemsListBox.Items.Clear();
            break;
          }
      }//Switch end.
    }//OnAfterSelectHandler( object sender, System.Windows.Forms.TreeViewEventArgs args )


    /// <summary>
    /// Handles the Before Select event 
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">TreeViewCancelEventArgs</param>
    private void OnBeforeSelectHandler(object sender, System.Windows.Forms.TreeViewCancelEventArgs args)
    {
      TreeNode node = m_studyTree.SelectedNode;
      if( node == null )
        return;

      Type type = node.Tag.GetType();
      if( type.Name == "Series" )
        GatherTransferData(( Series)node.Tag);

    }//OnBeforeSelectHandler(object sender, System.Windows.Forms.TreeViewCancelEventArgs args )


    /// <summary>
    /// Event handler for the OnSelectedIndexChanged event of the dicom servers combo box
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDicomServerSelected(object sender, System.EventArgs e)
    {
      if( m_comboBoxDicomServers.Items.Count > 0 ) m_transfer.Enabled = true;
    } // OnDicomServerSelected(object sender, System.EventArgs e)

    /// <summary>
    /// Handles the GUI toggling based on the Item in the Combo box.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnSelectedIndexChangedHandler(object sender, System.EventArgs args)
    {
      m_selectedTransferData.Clear();
      m_selectedTransferProvider = ( TransferProvider )this.m_transferProviders.SelectedItem;

      CreateStudyTree();
      System.Windows.Forms.ToolTip tooltip = new System.Windows.Forms.ToolTip();
      tooltip.SetToolTip(m_transferProviders, m_selectedTransferProvider.Description);

      m_pacsSelectionControl.Visible = false;
      m_comboBoxDicomServers.Visible = false;
      m_labelDicomServer.Visible = false;
      m_pacsServerLabel.Visible = false;
      m_transfer.Enabled = true;
      m_pathSpec.Visible = false;
      m_transferFSLabel.Visible = false;
      m_pathNavigatorButton.Visible = false;
      //m_overrideCheckBox.Visible      = true;

      if( m_selectedTransferProvider.TransferProviderTypeName == "Viatronix.Console.PACSTransferProvider, Console" ||
           m_selectedTransferProvider.TransferProviderTypeName == "Viatronix.Console.PACSTransferProviderForArchival, Console" )
      {
        m_pacsSelectionControl.Visible = true;
        m_pacsServerLabel.Visible = true;

      }
      else if( m_selectedTransferProvider.TransferProviderTypeName == "Viatronix.Console.DicomServerTransferProvider, Console" )
      {

        // Populates the dicom servers combo box
        GetViatronixDicomServers();

        m_transferProviders.Size = m_comboBoxDicomServers.Size;
        m_comboBoxDicomServers.Visible = true;
        m_labelDicomServer.Visible = true;

        if( m_comboBoxDicomServers.Items.Count == 0 )
        {
          m_transfer.Enabled = false;
        }
      }
      else if( m_selectedTransferProvider.TransferProviderTypeName ==
        "Viatronix.Console.FileSystemTransferProvider, Console" )
      {
        m_pathSpec.Visible = true;
        m_pathSpec.Text = System.Configuration.ConfigurationManager.AppSettings["FileSystemTransferDefaultPath"];
        m_transferFSLabel.Visible = true;
        m_pathNavigatorButton.Visible = true;
        m_overrideCheckBox.Visible = false;
      }

      //Toggle the Anonymization UI part based on anonimize flag.
      if( m_selectedTransferProvider.IsAnonymize )
      {
        m_anonimizeCheckbox.Visible = true;
        m_anonimizeCheckbox.Enabled = true;
        m_overrideCheckBox.Visible = true;
        m_overrideCheckBox.Enabled = false;
      }
      else
      {
        m_anonimizeCheckbox.Enabled = false;
        m_anonimizeCheckbox.Visible = false;
        m_overrideCheckBox.Visible = false;
      }

      //Add the available items in the Transfer Items check list box.
      BuildDefaultTransferData();

    }// OnSelectedIndexChangedHandler( object sender, System.EventArgs args )


    /// <summary>
    /// Anonymize Button Click handler. toggle the override boox.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">System.EventArgs</param>
    private void OnAnonymizeClickHandler(object sender, System.EventArgs args)
    {
      // Issue:     5707
      // Assigned:  Mark Kontak
      // Desc:      Will leave the override disabled until we can path it properly
      //            uncomment when the override issues are corrected.
      // m_overrideCheckBox.Enabled =  m_anonimizeCheckbox.Checked ;
      // m_overrideCheckBox.Checked = false;

    }//OnAnonymizeClickHandler( object sender, System.EventArgs args )


    /// <summary>
    /// Handles the Cancel Click Event.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnCancelHandler(object sender, System.EventArgs args)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.Close();
    }//OnCancelHandler(object sender, System.EventArgs args)


    /// <summary>
    /// OnTransfer Click event Handler 
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="aths">EventArgs</param>
    private void OnTransferClickHandler(object sender, System.EventArgs args)
    {
      
      // If a transfer directory was specified make sure that the patient directory doesn't exist
      // within the selected directory.
      if( m_selectedTransferProvider.TransferProviderTypeName ==
        "Viatronix.Console.FileSystemTransferProvider, Console" )
      {
        if (!System.IO.Directory.Exists(m_pathSpec.Text))
        {
          System.IO.Directory.CreateDirectory(m_pathSpec.Text);
        }

        if( m_pathSpec.Text == string.Empty)
        {
          Viatronix.UI.MessageBox.ShowError("Specified directory does not exist.  Please provide an " +
            "existing directory and try again.");

          return;
        } // if(m_pathSpec.Text == string.Empty || !System.IO.Directory.Exists(m_pathSpec.Text))
      } // if( m_selectedTransferProvider.TransferProviderTypeName == ... 
      
      if( m_studyTree.SelectedNode == null || m_studyTree.SelectedNode.Tag == null )
      {
        Viatronix.UI.MessageBox.ShowError("Pick a valid series item to transfer.");
        return;
      }

      /// Deafult
      m_selectedTransferProvider.TransferObject = null;

      TreeNode node = m_studyTree.SelectedNode;

      Type type = node.Tag.GetType();
      if( type.Name == "Series" )
        GatherTransferData(( Series)node.Tag);

      if( m_selectedTransferProvider.AllowItemSelection && !IsAnyItemSelectedForTransfer() )
      {
        Viatronix.UI.MessageBox.ShowError("No items were selected for transfer.");
        return;
      }

      /* -------------------------------------------------------------------------
       * Since the anonymization doesn't support anonymizing movies we need this 
       * additional check here. 
       * -------------------------------------------------------------------------*/
      if( m_anonimizeCheckbox.Checked && IsMoviesPicked() )
      {
        string warning = "Movie anonymization is not currently supported.\n" +
                         "To ensure patient privacy, make sure the associated movies do not contain patient information or deselect the movie option.\n" +
                         "Click Yes to continue transferring with movies, No to Cancel.";

        if( !Viatronix.UI.MessageBox.ShowQuestionWithWarning(warning) )
          return;
      }


      if( !m_selectedTransferProvider.IsSeriesCollectionTransfer && m_selectedTransferData.Count > 1 )
      {
        Viatronix.UI.MessageBox.ShowInfo("Transferring multiple series is not supported. Pick one of the desired series.");
        return;
      }

      this.DialogResult = System.Windows.Forms.DialogResult.OK;

      //set the Transfer Details from the UI components
      m_selectedTransferProvider.IsAnonymize = m_anonimizeCheckbox.Checked;
      m_selectedTransferProvider.IsOverride = m_overrideCheckBox.Checked;

      if( m_pacsSelectionControl.Visible && m_pacsSelectionControl.Count > 0 )
      {
        m_selectedTransferProvider.TransferObject = m_pacsSelectionControl.SelectedServer;
      } // if( m_pacsSelectionControl.Visible && m_pacsSelectionControl.Count > 0 )

      // If a transfer directory was specified make sure that the patient directory doesn't exist
      // within the selected directory.
      else if( m_selectedTransferProvider.TransferProviderTypeName ==
        "Viatronix.Console.FileSystemTransferProvider, Console" )
      {
        // Put the transferred data into a Session GUID directory so that it's more difficult for users to
        // loose data.
        m_selectedTransferProvider.TransferObject = m_pathSpec.Text;
      } // if( m_selectedTransferProvider.TransferProviderTypeName == ... )

      ///////////////////////////////////////////////////////////////////////////////////////////////////
      // If the dicom servers combo box conatins data then use this to set the transfer object which
      // will contains the selected destination aeTitle
      ///////////////////////////////////////////////////////////////////////////////////////////////////
      if( m_comboBoxDicomServers.Items.Count > 0 )
      {
        char[] sep = { ':' };
        string[] fields = m_comboBoxDicomServers.SelectedItem.ToString().Split(sep);
        m_selectedTransferProvider.TransferObject = fields[0].Trim();

      }

      //Clean the series collection for Transfer by elimination unwanted series/files
      ExcludeUnwantedSeriesObjects();
      ExcludeUnwantedTransferItemFiles();

    }//OnTransferClickHandler( object sender, System.EventArgs args )


    #endregion

    #region methods

    /// <summary>
    /// Creates the Study Tree based on the Series Collection.
    /// </summary>
    private void CreateStudyTree()
    {
      try
      {
        m_studyTree.Nodes.Clear();
        TreeNode studyNodes = new TreeNode();
        TreeNode studyNode = null;

        if( m_selectedSeries == null || ( m_selectedTransferProvider.IncludeReferences && m_selectedSeries != null ) )
        {

          /////////////////////////////////////////////////////
          // Load the SESSION's first
          ////////////////////////////////////////////////////
          foreach( Series series in m_seriesCollection.FindAll(SeriesTypes.Session) )
          {

            if( studyNode == null || !(( string )studyNode.Tag).Equals(series.StudyUid) )
            {
              studyNode = new TreeNode(series.StudyUid);
              studyNode.Tag = series.StudyUid;
              m_studyTree.Nodes.Add(studyNode);
             }

            TreeNode seriesNode = new TreeNode(series.Type.ToString() + ", " + series.Description);
            seriesNode.Tag = series;

            if( IsTypeValidForTransfer(series.Type) )
              studyNode.Nodes.Add(seriesNode);

          } // END ... for each series in the list

          ////////////////////////////////////////////////////////
          // Load all other series after the session
          ///////////////////////////////////////////////////////
          foreach( Series series in m_seriesCollection )
          {
            // Add the dicom series last
            if( series.Type == SeriesTypes.Session ) continue;


             if( studyNode == null || !(( string )studyNode.Tag).Equals(series.StudyUid) )
            {
              studyNode = new TreeNode(series.StudyUid);
              studyNode.Tag = series.StudyUid;
              m_studyTree.Nodes.Add(studyNode);

            }

            TreeNode seriesNode = new TreeNode(series.Type.ToString() + ", " + series.Description);
            seriesNode.Tag = series;

            if( IsTypeValidForTransfer(series.Type) )
              studyNode.Nodes.Add(seriesNode);
          } // END ... for each dicom series in the list

        }
        else
        {

 
          studyNode = new TreeNode(m_selectedSeries.StudyUid);
          studyNode.Tag = m_selectedSeries.StudyUid;
          studyNodes.Nodes.Add(studyNode);
          m_studyTree.Nodes.Add(studyNode);

          TreeNode seriesNode = new TreeNode(m_selectedSeries.Type.ToString() + ", " + m_selectedSeries.Description);
          seriesNode.Tag = m_selectedSeries;

          if( IsTypeValidForTransfer(m_selectedSeries.Type) )
            studyNode.Nodes.Add(seriesNode);

        }

  
        SelectFirstSeriesNode();
      }

      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("ShowCurrentStatusOfItems failed. " + ex.Message, "TransferProviderSelector", "CreateStudyTree");
        throw ex;
      }

    }//CreateStudyTree(0


    /// <summary>
    /// Is Type Valid for Transfer
    /// </summary>
    /// <param name="seriesType"></param>
    /// <returns></returns>
    private bool IsTypeValidForTransfer(SeriesTypes seriesType)
    {
      foreach( TransferItem item in m_selectedTransferProvider.SupportedTransferItems )
      {
        SeriesTypes type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), item.Name);
        SeriesTypes searchInSeries = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), item.SearchInSeries);
        if (type == seriesType || searchInSeries == seriesType)
          return true;
      }
      return false;
    }//IsTypeValidForTransfer( string seriesType )


    /// <summary>
    /// Selects the First Series Node in the Tree
    /// </summary>
    private void SelectFirstSeriesNode()
    {
      //Select the first Series from a study.
      TreeNode patinetNode = m_studyTree.Nodes[0];
      TreeNode studyNode = patinetNode.FirstNode;
      TreeNode seriesNode = studyNode.FirstNode;

      m_studyTree.SelectedNode = seriesNode;

    }//SelectFirstSeriesNode()


    /// <summary>
    /// Add Transfer Providers in to the Combo box.
    /// </summary>
    public void AddTransferProviders()
    {
      //Add Transfer Providers Name in to the Combo
      foreach( TransferProvider transferProvider in m_transferProvidersList )
        m_transferProviders.Items.Add(transferProvider);

      m_transferProviders.SelectedIndex = 0;
      if( m_transferProviders.Items.Count > 1 ) m_transferProviders.SelectAll();

    }// AddTransferProviders()


    /// <summary>
    /// Show the current status of the Transfer items in the GUI
    /// </summary>
    /// <param name="series">Series</param>
    private void ShowCurrentStatusOfItems(Series series)
    {
      if( !m_selectedTransferData.Contains(series) )
        return;

      try
      {
        TransferItemsCollection selectedTransferItems = ( TransferItemsCollection )m_selectedTransferData[series];
        if( selectedTransferItems.Count <= 0 )
          return;

        for( int index = 0; index < m_transferItemsListBox.Items.Count; index++ )
        {
          foreach( TransferItem item in selectedTransferItems )
          {
            if( item.Name == m_transferItemsListBox.Items[index].ToString() )
            {
              if( item.CheckState )
                m_transferItemsListBox.SetItemCheckState(index, System.Windows.Forms.CheckState.Checked);
              else
                m_transferItemsListBox.SetItemCheckState(index, System.Windows.Forms.CheckState.Unchecked);
            }//if ( item.Type == m_transferItemsListBox.Items[ index ].ToString() )
          }// foreach ( TransferItem item in selectedTransferItems )
        }//for ( int index =0; index < m_transferItemsListBox.Items.Count; index++ )
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("ShowCurrentStatusOfItems failed. " + ex.Message, "TransferProviderSelector", "ShowCurrentStatusOfItems");
        throw ex;
      }
    }//ShowCurrentStatusOfItems( Series series )


    /// <summary>
    /// Gets the Selected Transfer Items.
    /// </summary>
    /// <param name="series"></param>
    private void GatherTransferData(Series series)
    {
      try
      {
        TransferItemsCollection selectedTransferItems = new TransferItemsCollection();

        //Iterate through all items in the CheckedList Box.Add them in to the items collection
        for( int index = 0; index < m_transferItemsListBox.Items.Count; index++ )
        {
          string item = m_transferItemsListBox.Items[index].ToString();
          string transferType = TransferItem.GetTransferSubType(series.Application);
          TransferItem selectedItem = ( TransferItem )m_selectedTransferProvider.SupportedTransferItems.GetItem(item, transferType).Clone();

          selectedItem.CheckState = m_transferItemsListBox.GetItemCheckState(index) == System.Windows.Forms.CheckState.Checked ? true : false;

          //if ( selectedItem.CheckState )
          selectedTransferItems.Add(selectedItem);
        }
        //if ( selectedTransferItems.Count > 0 )
        m_selectedTransferData[series] = selectedTransferItems;
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("GatherTransferData failed. " + ex.Message, "TransferProviderSelector", "GatherTransferData");
        throw ex;
      }
    }//GatherTransferData ( Series series )

    /// <summary>
    /// Checks whether there are any items for Transfer
    /// </summary>
    /// <returns></returns>
    private bool IsAnyItemSelectedForTransfer()
    {
      if( m_selectedTransferData == null || m_selectedTransferData.Count < 1 )
        return false;

      IDictionaryEnumerator enumr = m_selectedTransferData.GetEnumerator();
      while( enumr.MoveNext() )
      {
        TransferItemsCollection items = ( TransferItemsCollection )enumr.Value;
        if( items != null )
        {
          foreach( TransferItem item in items )
          {
            if( item.CheckState )
              return true;
          }
        }
      }
      return false;
    }//IsAnyItemSelectedForTransfer()


    /// <summary>
    /// Is there any movies selected for anonymization
    /// </summary>
    /// <returns></returns>
    private bool IsMoviesPicked()
    {
      IDictionaryEnumerator enumerator = m_selectedTransferData.GetEnumerator();

      while( enumerator.MoveNext() )
      {
        Series series = ( Series )( enumerator.Key );
        if( series.Type == SeriesTypes.Session )
        {
          //Get the Transfer Item's Formats
          string transferSubtype = TransferItem.GetTransferSubType(series.Application);
          TransferItemsCollection transferItems = ( TransferItemsCollection )enumerator.Value;
          TransferItem movieItem = transferItems.GetItem("Movies", transferSubtype);

          if( movieItem != null && movieItem.CheckState )
          {
            //PickAllMovie Files ...fill in the movie files. Incase the study is from CD or PACS this will help
            string moviePath = System.IO.Path.Combine(series.Path, movieItem.SearchInFolder);

            if( System.IO.Directory.Exists(moviePath) )
            {
              string[] allMovieFiles = System.IO.Directory.GetFiles(moviePath);
              for( int index = 0; index < allMovieFiles.Length; index++ )
              {
                if( IsFileInTransferItemsPattern(allMovieFiles[index], movieItem) )
                  return true;
              }
            }
          }
        }
      }
      return false;
    }//IsMoviesPicked()


    /// <summary>
    /// Builds the Default Transfer Data based on the Transfer Provider Details
    /// </summary>
    private void BuildDefaultTransferData()
    {
      try
      {
        if( m_selectedTransferProvider.IncludeReferences )
        {
          foreach( Series series in m_seriesCollection )
          {
            string subType = ( series.Application.ToLower() != "vc" ? "default" : series.Application.ToLower() );

            TransferItemsCollection transferItems = m_selectedTransferProvider.SupportedTransferItems.GetStudyTypeItems(subType);

            TransferItemsCollection possibleItems = new TransferItemsCollection();

            foreach( TransferItem item in transferItems )
            {
              SeriesTypes type  = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), item.Name);
              SeriesTypes searchInSeries = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), item.SearchInSeries);

              if (searchInSeries == type )
                possibleItems.Add(item);
            }

            if( possibleItems != null && possibleItems.Count > 0 )
              m_selectedTransferData[series] = possibleItems;
          }
        }
        else
        {

          if( m_selectedSeries != null )
          {
            m_selectedTransferData[m_selectedSeries] = new TransferItemsCollection();
          }
          else
          {
            foreach( Series series in m_seriesCollection )
            {
              m_selectedTransferData[series] = new TransferItemsCollection();
            }
          }
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("Build Default transfer data failed " + ex.Message, "TransferProviderSelector", "BuildDefaultTransferData");
        throw ex;
      }
    }//BuildDefaultTransferData()




    /// <summary>
    /// Adds Valid Transfer Items to the List Box
    /// </summary>
    /// <param name="series">Viatronix.v3D.Core.Series</param>
    private void AddTransferItems(Series series)
    {
      m_transferItemsListBox.Items.Clear();

      //if ( ! m_selectedTransferProvider.AllowItemSelection ) return;

      string subType = TransferItem.GetTransferSubType(series.Application);

      TransferItemsCollection possibleItems = m_selectedTransferProvider.SupportedTransferItems.GetStudyTypeItems(subType);

      foreach( TransferItem item in possibleItems )
      {
        if( item.SearchInSeries.Equals(series.Type) )
        {
          if( IsItemValidForTransfer(item, series) )
          {
            m_transferItemsListBox.Items.Add(item.Name, item.CheckState);

            if( !m_selectedTransferProvider.AllowItemSelection )//|| series.Type == SeriesTypes.Session&&  subType.ToLower() == "vc" )
              m_transferItemsListBox.Enabled = false;
            else
              m_transferItemsListBox.Enabled = true;
          }
        }
      }
    }//AddTransferItems( Series series )


    /// <summary>
    /// Check whether a Transfer Item is valid in this Series COllection Context.
    /// </summary>
    /// <param name="item">TransferItem</param>
    /// <param name="series">Series</param>
    /// <returns>bool</returns>
    private bool IsItemValidForTransfer(TransferItem item, Series series)
    {

      SeriesTypes searchInSeries = SeriesTypes.None;
      Enum.TryParse<SeriesTypes>(item.SearchInSeries, out searchInSeries);

      //Search Series for valid items
      if (Enum.TryParse<SeriesTypes>(item.SearchInSeries, out searchInSeries) && searchInSeries != series.Type)
        return false;

      //Check whether any one of the specified format is part of this series file.
      foreach( string format in item.Formats )
      {
        if( item.SearchInFolder != string.Empty )
        {
          string searchIn = System.IO.Path.Combine(series.Path, item.SearchInFolder);
          if( !System.IO.Directory.Exists(searchIn) )
            continue;

          string[] files = System.IO.Directory.GetFiles(searchIn);
          if( DoesFilesMatchFormat(format, files) == true )
            return true;
        }
        else
        {
          if( format.Length == 0 )
          {
            return true;
          }
          else
          {
            if( DoesFilesMatchFormat(format, series.Files) == true )
              return true;
          }
        }
      }

      return false;

    }// IsItemValidForTransfer( TransferItem item, Series series )


    /// <summary>
    /// DoesFilesMatchFormat
    /// </summary>
    /// <param name="format">string</param>
    /// <param name="files">string[]</param>
    /// <returns>bool</returns>
    bool DoesFilesMatchFormat(string format, string [] files)
    {
      foreach( string file in files )
      {
        if( format.Equals("*.*") )
          return true;
        if( file.ToLower().EndsWith(format.ToLower()) )
          return true;
      }
      return false;
    }//DoesFilesMatchFormat ( string format, string [] files )


    /// <summary>
    /// DoesFilesMatchFormat
    /// </summary>
    /// <param name="format">string</param>
    /// <param name="files">string[]</param>
    /// <returns>bool</returns>
    bool DoesFilesMatchFormat(string format, List<string> files)
    {
      foreach( string file in files )
      {
        if( format.Equals("*.*") )
          return true;
        if( file.ToLower().EndsWith(format.ToLower()) )
          return true;
      }
      return false;
    }//DoesFilesMatchFormat ( string format, List<string> files)


    /// <summary>
    /// Retreives all the viatronix dicom servers that can be transferred to
    /// </summary>
    private void GetViatronixDicomServers()
    {
      List<DicomServer> servers = Global.DataProvider.GetDicomServers();

      if (servers.Count > 0)
      {
        foreach (DicomServer server in servers)      
        {
          m_comboBoxDicomServers.Items.Add(server.AETitle);
        }
 
        if (m_comboBoxDicomServers.Items.Count > 0)
        {
          m_comboBoxDicomServers.SelectedIndex = 0;
        }
        else
        {
          m_transfer.Enabled = false;
        }
      }

 
    } // GetViatronixDicomServers()


    /// <summary>
    /// Checks whether any one of the Transfer Items are selected or not
    /// </summary>
    /// <param name="transferItems">TransferItemsCollection</param>
    /// <returns>bool</returns>
    private bool IsAnyTransferItemSelected(TransferItemsCollection transferItems)
    {
      foreach( TransferItem item in transferItems )
      {
        if( item.CheckState )
          return true;
      }
      return false;
    }//IsAnyTransferItemSelected( TransferItemsCollection transferItems )


    /// <summary>
    /// Exclude any unselected Files of a Transfer Item.
    /// </summary>
    private void ExcludeUnwantedTransferItemFiles()
    {
      IDictionaryEnumerator enumerator = m_selectedTransferData.GetEnumerator();
      while( enumerator.MoveNext() )
      {
        Series series = ( Series )enumerator.Key;
        TransferItemsCollection transferItems = ( TransferItemsCollection )enumerator.Value;

        foreach( TransferItem item in transferItems )
        {
          if( item.CheckState )
            continue;

          // Checks if the file may be saved from removal in the case of movie preview images.
          bool canHaveSavingGrace = false;
          foreach( string format in item.Formats )
          {
            string formatLC = format.ToLower();
            if( formatLC == ".jpg" || formatLC == ".png" )
            {
              canHaveSavingGrace = true;
            } // if(formatLC == ".jpg" || formatLC == ".png")
            else if( formatLC == ".wmv" || formatLC == ".avi" )
            {
              canHaveSavingGrace = false;
              break;
            } // else if( formatLC == ".wmv" || formatLC == ".avi" )
          } // foreach( string format in item.Formats )

          for( int count = 0; count < series.Files.Count; count++ )
          {
            string file = series.Files[count].ToString();

            //Last minute bug fix, don't remove the VRX file.
            //We need this to transfer report items.
            if( file.ToLower().EndsWith(".vrx") )
              continue;

            //eliminate only those files in the format.
            if( IsFileInTransferItem(file, series, item) )
            {
              if( series.Type == SeriesTypes.Dicom && count <= 1 )
                continue;

              if( !canHaveSavingGrace || !HasMovieFileSavingGrace(file, series) )
              {
                series.Files.RemoveAt(count--);
              } // if(!HasMovieFileSavingGrace(file, series, item))
            }//if ( IsFileInTransferItem( file, series, item) )
          }//for ( int count=0; count < series.Files.Count;  count++ )
        }// foreach ( TransferItem item in transferItems )
      }//while ( enumerator.MoveNext() )

    }//ExcludeUnwantedTransferItemFiles ()

    /// <summary>
    /// Checks whether the file in question has a corresponding .avi or .wmv file.
    /// </summary>
    /// <param name="file">The series file that was marked for removal which could be saved.</param>
    /// <param name="series">The Series containing the list of files we're going to be looking through.</param>
    /// <returns>Whether or not the file should be pulled from the clutches of removal.</returns>
    private bool HasMovieFileSavingGrace(string file, Series series)
    {
      bool savingGrace = false;
      string fileEXTLC = Path.GetExtension(file).ToLower();
      string fileWOEXTLC = ( Path.Combine(Path.GetDirectoryName(file), Path.GetFileNameWithoutExtension(file)) ).ToLower();

      foreach( string seriesFile in series.Files )
      {
        string seriesFileEXTLC = Path.GetExtension(seriesFile).ToLower();
        string seriesFileWOEXTLC = ( Path.Combine(Path.GetDirectoryName(seriesFile),
          Path.GetFileNameWithoutExtension(seriesFile)) ).ToLower();

        if( fileWOEXTLC == seriesFileWOEXTLC && ( seriesFileEXTLC == ".avi" || seriesFileEXTLC == ".wmv" ) )
        {
          savingGrace = true;
          break;
        } // if( fileWOEXTLC == seriesFileWOEXTLC && (seriesFileEXTLC == ".avi" && seriesFileEXTLC == ".wmv" ) )
      } // foreach(string seriesFile in series.Files)

      return savingGrace;
    } // HasMovieFileSavingGrace(string file, Series series, TransferItem item)

    /// <summary>
    /// Checks whether the file belongs to a Transfer Item.
    /// </summary>
    /// <param name="seriesFile">string</param>
    /// <param name="series">Series</param>
    /// <param name="item">transferItem</param>
    /// <returns>bool</returns>
    private bool IsFileInTransferItem(string seriesFile, Series series, TransferItem item)
    {

      SeriesTypes searchInSeries = SeriesTypes.None;

      if (Enum.TryParse<SeriesTypes>(item.SearchInSeries, out searchInSeries) && series.Type == searchInSeries )
      {
        //avoid removing files which does not have a search in Folder.
        if( item.SearchInFolder == string.Empty )
          return IsFileInTransferItemsPattern(seriesFile, item);

        //If there is a search in Folder Get all the files from that folder and then match.
        string searchIn = System.IO.Path.Combine(series.Path, item.SearchInFolder);
        if( !System.IO.Directory.Exists(searchIn) )
          return false;

        string[] files = System.IO.Directory.GetFiles(searchIn);

        string seriesFileLCEXT = Path.GetExtension(seriesFile).ToLower();
        string seriesFileLCWOEXT = Path.GetFileNameWithoutExtension(seriesFile).ToLower();

        foreach( string file in files )
        {
          // ISSUE #
          // Ensure that there is not another like-named file in the directory as long as that other file
          // has a ".mmv" or ".avi" extension.  If there is the file should not be marked for removal.

          string fileLCEXT = Path.GetExtension(file).ToLower();
          string fileLCWOEXT = Path.GetFileNameWithoutExtension(file).ToLower();
          if( fileLCWOEXT == seriesFile && ( fileLCEXT == ".wmv" || fileLCEXT == ".avi" ) )
          {
            return false;
          } // if(fileLCWOEXT == (Path.GetFileNameWithoutExtension(file).ToLower()))

          if( file.ToLower().EndsWith(seriesFile.ToLower()) &&
            IsFileInTransferItemsPattern(seriesFile, item) )
          {
            return true;
          } // if ( file.ToLower().EndsWith ( seriesFile.ToLower() ) &&
        } // foreach ( string file in files )
      }
      return false;
    }//IsFileInTransferItem( string seriesFile, Series series, TransferItem item )


    /// <summary>
    /// Checks the file matches a Pattern
    /// </summary>
    /// <param name="file">string</param>
    /// <param name="item">TransferItem</param>
    /// <returns>bool</returns>
    private bool IsFileInTransferItemsPattern(string seriesFile, TransferItem item)
    {
      foreach( string format in item.Formats )
      {
        if( format.Equals("*.*") || seriesFile.ToLower().EndsWith(format.ToLower()) )
          return true;
      }
      return false;
    }//IsFileInTransferItemsPattern(  string file,  TransferItem item )


    // <summary>
    /// Exclude series where there are no Transfer Selection which are not selected
    /// </summary>
    /// <param name="seriesCollection"></param>
    private void ExcludeUnwantedSeriesObjects()
    {
      if( !m_selectedTransferProvider.AllowItemSelection ) return;

      List<Series> unwanted = new List<Series>();

      IDictionaryEnumerator enumerator = m_selectedTransferData.GetEnumerator();
      while( enumerator.MoveNext() )
      {
        Series series = ( Series )enumerator.Key;
        //Keep the Dicom Series there..
        if( series.Type == SeriesTypes.Dicom)
          continue;

        TransferItemsCollection transferItems = ( TransferItemsCollection )enumerator.Value;
        if( !IsAnyTransferItemSelected(transferItems) )
        {

          m_dataProvider.GetDependents( series);
          if ( series.Dependents.Count <= 0)
          {
            unwanted.Add(series);
            continue;
          }
        }
      }//while ( enumerator.MoveNext() )

      //Remove the series
      foreach (Series series in unwanted )
        m_selectedTransferData.Remove(series);

    }//ExcludeUnwantedSeriesObjects

    private void OnPathNavigatorClickedHandler(object sender, EventArgs e)
    {
      // Show the dialog and get the selected directory.
      FolderBrowserDialog browser = new FolderBrowserDialog();

      if( browser.ShowDialog() == DialogResult.OK )
      {
        m_pathSpec.Text = browser.SelectedPath;
      } // if(browser.ShowDialog() == DialogResult.OK)
    }

    #endregion

  }//public class TransferDialog : Viatronix.UI.Dialog
}//namespace Viatronix.Console

#region revision History

// $Log: TransferProviderSelector.cs,v $
// Revision 1.23.2.19  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.23.2.18  2010/06/02 15:44:11  kchalupa
// Issue: CONS-744  Change the PathNavigator button's forecolor to black so the elipses is visible.
//
// Revision 1.23.2.17  2008/10/15 20:03:23  kchalupa
// FileSystemTransferProvider TOC Removal and now copies into GUID subdirectory under selected directory.
//
// Revision 1.23.2.16  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.23.2.15  2008/09/24 20:16:08  mkontak
// no message
//
// Revision 1.23.2.14  2008/09/16 17:09:46  kchalupa
// Issue
// 6150  No Movie Preview Console Crashes
// VCSessionViewer
// 6122 VC Sessions Not Showing Completed
// VCLauncher
// 6073 Each Movie (VC) Is Listed Twice On Patient CD
// VCSessionProvider
// 6153 Movies that have no preview do not show up in Patient CD HTML Report
// RemovableMediaForPatient
// ReportTags
//
// Revision 1.23.2.13  2008/09/14 04:18:18  mkontak
// Fix '-' in the ae title
//
// Revision 1.23.2.12  2007/12/20 16:05:20  mkontak
// Changed to force the Session first instead of the dicom last
//
// Revision 1.23.2.11  2007/12/17 19:02:53  mkontak
// Add the dicom series after all others in the tree list view.
//
// Revision 1.23.2.10  2007/12/14 18:33:18  mkontak
// Fex the PACS selection to only allow service types of STORE
//
// Revision 1.23.2.9  2007/09/13 17:01:02  romy
// fixed the issue with MovieFolder check
//
// Revision 1.23.2.8  2007/09/13 16:45:05  romy
// commented out the Folder check from methos IsMoviePicked. Folder's existance check was failing all the time. Don't know why
//
// Revision 1.23.2.7  2007/07/02 17:55:05  romy
// added code to avoid the elimination of VRX files if the Session items are individually unselected.
//
// Revision 1.23.2.6  2007/07/02 17:50:09  romy
// added JPG type for movie fils this is for VC
//
// Revision 1.23.2.5  2007/07/02 15:14:26  mkontak
// Issue 5707: Anonymization error when sequence items are in the serialized
// string.
//
// Revision 1.23.2.4  2007/03/30 15:40:07  romy
// check added to verify the existance of Movie Folder when you try to fetch movies . VC study transfer was casuing issues .
//
// Revision 1.23.2.3  2007/03/27 14:14:56  romy
// added check to avoid removing series which has dependecnies during transfer
//
// Revision 1.23.2.2  2007/03/23 20:40:13  romy
// added code back to exclude files & series if they are not selected for transfer
//
// Revision 1.23.2.1  2007/03/23 19:08:31  romy
// logic and function name was not matching. Modified the return value of IsMoviePicked
//
// Revision 1.23  2007/03/08 15:59:52  mkontak
// Fix transfer to DICOM server. When no server are listed the Transfer button
// is disabled and fix problem with setting the transferObject property incorrectly.
//
// Revision 1.22  2007/03/02 19:40:05  romy
// added additional checks to block the anonymization if movie items are selected
//
// Revision 1.21  2007/03/02 16:04:27  romy
// modified the movie anonymization message.Issue#5439
//
// Revision 1.20  2007/03/01 20:51:39  romy
// check added for Movies if anonymization is selected. Issue# 5439
//
// Revision 1.19  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.18  2007/01/19 20:53:18  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.17  2007/01/12 18:57:02  romy
// Modified to display PACS Selection combo for PACStoArchival options
//
// Revision 1.16  2006/10/31 21:32:26  mkontak
// Disable the anonymize if we do not have a dicom series.
//
// Revision 1.15  2006/10/27 19:43:30  mkontak
// Fix for the series location when a session is created
//
// Revision 1.14  2006/10/27 19:29:13  mkontak
// Added allowItemSelection to transfer providers
//
// Revision 1.13  2006/07/24 12:53:39  mkontak
// no message
//
// Revision 1.12  2006/04/27 19:46:14  mkontak
// Make field non case sensitive
//
// Revision 1.11  2006/04/10 20:07:41  mkontak
// Added transfer parameters
//
// Revision 1.10  2006/04/10 19:12:01  mkontak
// Minor change to move combo box
//
// Revision 1.9  2006/03/14 14:18:41  mkontak
// Change text for pacs selector
//
// Revision 1.8  2006/03/10 20:07:28  romy
// GUI Doctor made a quick visit
//
// Revision 1.7  2006/03/02 14:15:27  romy
// fixed transferring of non valid items
//
// Revision 1.6  2006/03/01 22:01:00  romy
// fixed transferring of non valid items
//
// Revision 1.5  2006/02/16 20:07:27  romy
// VC Transfer added
//
// Revision 1.4  2006/02/16 18:11:07  mkontak
// Fix pacs selector
//
// Revision 1.3  2006/02/16 15:32:58  romy
// no message
//
// Revision 1.2  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.1  2006/01/26 15:54:38  romy
// Transfer Dialog facelifted
//
// Revision 1.2  2006/01/25 21:14:11  romy
// transfer check for Multi-series transfer
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.19  2005/12/12 21:14:02  romy
// Transfer revisited
//
// Revision 1.18  2005/11/09 16:46:38  romy
// added tool tip for Transfer Provider Item
//
// Revision 1.17  2005/10/31 21:10:30  romy
// added the Dir Exists check before Transfer.
//
// Revision 1.16  2005/10/29 05:12:48  romy
// fix for Multiple DICOM Series selection
//
// Revision 1.15  2005/10/29 02:34:48  romy
// new Transfer Dialog
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferProviderSelector.cs,v 1.23.2.19 2011/05/18 01:56:59 mkontak Exp $
// $Id: TransferProviderSelector.cs,v 1.23.2.19 2011/05/18 01:56:59 mkontak Exp $

#endregion revision History
