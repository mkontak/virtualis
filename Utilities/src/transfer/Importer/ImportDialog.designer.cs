// $Id: ImportDialog.designer.cs,v 1.1.2.7 2010/05/20 15:08:09 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Designer class for the import dialog.
  /// </summary>
  partial class ImportDialog
  {

    #region fields

    /// <summary>
    /// Enter the source path here.
    /// </summary>
    private System.Windows.Forms.TextBox m_sourceTexBox;
      
    /// <summary>
    /// Label to indicate the input for the adjacent text field.
    /// </summary>
    private System.Windows.Forms.Label m_sourceLabel;
      
    /// <summary>
    /// Button to navigate to the source folder or root folder.
    /// </summary>
    private Viatronix.UI.Button m_sourceNavButton;
      
    /// <summary>
    /// Browser dialog for source and destination.
    /// </summary>
    private System.Windows.Forms.FolderBrowserDialog m_folderBrowserDialog;
      
    /// <summary>
    /// Indicates that an activity is being performed.
    /// </summary>
    private Viatronix.UI.ProgressBar2 m_progressBar;
      
    /// <summary>
    /// Button to allow navigation to the destination folder.
    /// </summary>
    private Viatronix.UI.Button m_destinationNavButton;
      
    /// <summary>
    /// Label to indicate the adjacent text field.
    /// </summary>
    private System.Windows.Forms.Label m_destinationLabel;
      
    /// <summary>
    /// Text box to insert the destination root directory.
    /// </summary>
    private System.Windows.Forms.TextBox m_destinationText;
      
    /// <summary>
    /// Background worker to perform the import while the form is still interactive.
    /// </summary>
    private System.ComponentModel.BackgroundWorker m_bgWorker;
      
    /// <summary>
    /// Panel to contain the buttons for import and exit as well as the various checkboxes.
    /// </summary>
    private System.Windows.Forms.Panel m_lowerPanel;
      
    /// <summary>
    /// Checkbox to indicate the session should or shouldn't be included.
    /// </summary>
    private System.Windows.Forms.CheckBox m_includeSessionCheckBox;
      
    /// <summary>
    /// Checkbox to indicate database insertion should or shouldn't be performed.
    /// </summary>
    private System.Windows.Forms.CheckBox m_databaseInsertCheckBox;
      
    /// <summary>
    /// Checkbox to indicate that copy should or shouldn't be performed.
    /// </summary>
    private System.Windows.Forms.CheckBox m_copyCheckBox;
      
    /// <summary>
    /// Button to exit the application.
    /// </summary>
    private Viatronix.UI.Button m_buttonCancel;

    /// <summary>
    /// Starts the importing procedure.
    /// </summary>
    private Viatronix.UI.Button m_buttonImport;

    /// <summary>
    /// The directory list view.
    /// </summary>
    private System.Windows.Forms.ListView m_directoryStatusListView;

    /// <summary>
    /// Column for the directory name.
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_directoryNameColumn;
      
    /// <summary>
    /// Column for the import status.
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_statusColumn;

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("Passed", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup2 = new System.Windows.Forms.ListViewGroup("Failed", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup3 = new System.Windows.Forms.ListViewGroup("Partial", System.Windows.Forms.HorizontalAlignment.Left);
      this.m_sourceTexBox = new System.Windows.Forms.TextBox();
      this.m_sourceLabel = new System.Windows.Forms.Label();
      this.m_sourceNavButton = new Viatronix.UI.Button();
      this.m_folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
      this.m_progressBar = new Viatronix.UI.ProgressBar2();
      this.m_destinationNavButton = new Viatronix.UI.Button();
      this.m_destinationLabel = new System.Windows.Forms.Label();
      this.m_destinationText = new System.Windows.Forms.TextBox();
      this.m_bgWorker = new System.ComponentModel.BackgroundWorker();
      this.m_lowerPanel = new System.Windows.Forms.Panel();
      this.m_includeSessionCheckBox = new System.Windows.Forms.CheckBox();
      this.m_databaseInsertCheckBox = new System.Windows.Forms.CheckBox();
      this.m_copyCheckBox = new System.Windows.Forms.CheckBox();
      this.m_buttonCancel = new Viatronix.UI.Button();
      this.m_buttonImport = new Viatronix.UI.Button();
      this.m_directoryStatusListView = new System.Windows.Forms.ListView();
      this.m_directoryNameColumn = new System.Windows.Forms.ColumnHeader();
      this.m_statusColumn = new System.Windows.Forms.ColumnHeader();
      this.m_lowerPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_sourceTexBox
      // 
      this.m_sourceTexBox.Location = new System.Drawing.Point(85, 12);
      this.m_sourceTexBox.Name = "m_sourceTexBox";
      this.m_sourceTexBox.Size = new System.Drawing.Size(538, 20);
      this.m_sourceTexBox.TabIndex = 0;
      // 
      // m_sourceLabel
      // 
      this.m_sourceLabel.AutoSize = true;
      this.m_sourceLabel.Location = new System.Drawing.Point(16, 16);
      this.m_sourceLabel.Name = "m_sourceLabel";
      this.m_sourceLabel.Size = new System.Drawing.Size(45, 14);
      this.m_sourceLabel.TabIndex = 5;
      this.m_sourceLabel.Text = "Source:";
      // 
      // m_sourceNavButton
      // 
      this.m_sourceNavButton.CustomImages = new System.Drawing.Bitmap[] {
    null,
    null,
    null};
      this.m_sourceNavButton.ForeColor = System.Drawing.Color.Black;
      this.m_sourceNavButton.Location = new System.Drawing.Point(644, 11);
      this.m_sourceNavButton.Name = "m_sourceNavButton";
      this.m_sourceNavButton.OldStockButton = false;
      this.m_sourceNavButton.Size = new System.Drawing.Size(28, 23);
      this.m_sourceNavButton.TabIndex = 1;
      this.m_sourceNavButton.Text = "...";
      this.m_sourceNavButton.UseVisualStyleBackColor = true;
      this.m_sourceNavButton.Click += new System.EventHandler(this.OnSourcePathButtonClick);
      // 
      // m_progressBar
      // 
      this.m_progressBar.BackColor = System.Drawing.Color.Transparent;
      this.m_progressBar.DrawText = true;
      this.m_progressBar.Location = new System.Drawing.Point(16, 90);
      this.m_progressBar.Maximum = 100;
      this.m_progressBar.Minimum = 0;
      this.m_progressBar.Name = "m_progressBar";
      this.m_progressBar.Size = new System.Drawing.Size(657, 24);
      this.m_progressBar.Step = 1;
      this.m_progressBar.TabIndex = 17;
      this.m_progressBar.Text = string.Empty;
      this.m_progressBar.Value = 0;
      this.m_progressBar.Visible = false;
      // 
      // m_destinationNavButton
      // 
      this.m_destinationNavButton.CustomImages = new System.Drawing.Bitmap[] {
    null,
    null,
    null};
      this.m_destinationNavButton.ForeColor = System.Drawing.Color.Black;
      this.m_destinationNavButton.Location = new System.Drawing.Point(644, 49);
      this.m_destinationNavButton.Name = "m_destinationNavButton";
      this.m_destinationNavButton.OldStockButton = false;
      this.m_destinationNavButton.Size = new System.Drawing.Size(28, 23);
      this.m_destinationNavButton.TabIndex = 3;
      this.m_destinationNavButton.Text = "...";
      this.m_destinationNavButton.UseVisualStyleBackColor = true;
      this.m_destinationNavButton.Visible = false;
      this.m_destinationNavButton.Click += new System.EventHandler(this.OnDestinationNaviatorClickHandler);
      // 
      // m_destinationLabel
      // 
      this.m_destinationLabel.AutoSize = true;
      this.m_destinationLabel.Location = new System.Drawing.Point(13, 54);
      this.m_destinationLabel.Name = "m_destinationLabel";
      this.m_destinationLabel.Size = new System.Drawing.Size(63, 14);
      this.m_destinationLabel.TabIndex = 19;
      this.m_destinationLabel.Text = "Destination:";
      this.m_destinationLabel.Visible = false;
      // 
      // m_destinationText
      // 
      this.m_destinationText.Location = new System.Drawing.Point(85, 50);
      this.m_destinationText.Name = "m_destinationText";
      this.m_destinationText.Size = new System.Drawing.Size(538, 20);
      this.m_destinationText.TabIndex = 2;
      this.m_destinationText.Visible = false;
      // 
      // m_bgWorker
      // 
      this.m_bgWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.BackgroundWorker1DoWork);
      this.m_bgWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.OnWorker1WorkCompleted);
      // 
      // m_lowerPanel
      // 
      this.m_lowerPanel.Controls.Add(this.m_includeSessionCheckBox);
      this.m_lowerPanel.Controls.Add(this.m_databaseInsertCheckBox);
      this.m_lowerPanel.Controls.Add(this.m_copyCheckBox);
      this.m_lowerPanel.Controls.Add(this.m_buttonCancel);
      this.m_lowerPanel.Controls.Add(this.m_buttonImport);
      this.m_lowerPanel.Location = new System.Drawing.Point(16, 408);
      this.m_lowerPanel.Name = "m_lowerPanel";
      this.m_lowerPanel.Size = new System.Drawing.Size(661, 31);
      this.m_lowerPanel.TabIndex = 21;
      // 
      // m_includeSessionCheckBox
      // 
      this.m_includeSessionCheckBox.AutoSize = true;
      this.m_includeSessionCheckBox.Checked = true;
      this.m_includeSessionCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.m_includeSessionCheckBox.Location = new System.Drawing.Point(0, 7);
      this.m_includeSessionCheckBox.Name = "m_includeSessionCheckBox";
      this.m_includeSessionCheckBox.Size = new System.Drawing.Size(115, 18);
      this.m_includeSessionCheckBox.TabIndex = 5;
      this.m_includeSessionCheckBox.Text = "Include Session";
      this.m_includeSessionCheckBox.UseVisualStyleBackColor = false;
      this.m_includeSessionCheckBox.Visible = false;
      // 
      // m_databaseInsertCheckBox
      // 
      this.m_databaseInsertCheckBox.AutoSize = true;
      this.m_databaseInsertCheckBox.Checked = true;
      this.m_databaseInsertCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.m_databaseInsertCheckBox.Location = new System.Drawing.Point(110, 7);
      this.m_databaseInsertCheckBox.Name = "m_databaseInsertCheckBox";
      this.m_databaseInsertCheckBox.Size = new System.Drawing.Size(138, 18);
      this.m_databaseInsertCheckBox.TabIndex = 7;
      this.m_databaseInsertCheckBox.Text = "Insert Into Database";
      this.m_databaseInsertCheckBox.UseVisualStyleBackColor = true;
      this.m_databaseInsertCheckBox.Visible = false;
      // 
      // m_copyCheckBox
      // 
      this.m_copyCheckBox.AutoSize = true;
      this.m_copyCheckBox.Checked = true;
      this.m_copyCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.m_copyCheckBox.Location = new System.Drawing.Point(242, 7);
      this.m_copyCheckBox.Name = "m_copyCheckBox";
      this.m_copyCheckBox.Size = new System.Drawing.Size(92, 18);
      this.m_copyCheckBox.TabIndex = 6;
      this.m_copyCheckBox.Text = "Perform Copy";
      this.m_copyCheckBox.UseVisualStyleBackColor = true;
      this.m_copyCheckBox.Visible = false;
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
    null,
    null,
    null};
      this.m_buttonCancel.Enabled = true;
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Black;
      this.m_buttonCancel.Location = new System.Drawing.Point(583, 4);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.Size = new System.Drawing.Size(74, 24);
      this.m_buttonCancel.TabIndex = 9;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.UseVisualStyleBackColor = true;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClickedHandler);
      // 
      // m_buttonImport
      // 
      this.m_buttonImport.CustomImages = new System.Drawing.Bitmap[] {
    null,
    null,
    null};
      this.m_buttonImport.ForeColor = System.Drawing.Color.Black;
      this.m_buttonImport.Location = new System.Drawing.Point(503, 4);
      this.m_buttonImport.Name = "m_buttonImport";
      this.m_buttonImport.OldStockButton = false;
      this.m_buttonImport.Size = new System.Drawing.Size(74, 24);
      this.m_buttonImport.TabIndex = 8;
      this.m_buttonImport.Text = "Import";
      this.m_buttonImport.UseVisualStyleBackColor = true;
      this.m_buttonImport.Click += new System.EventHandler(this.OnImportClick);
      // 
      // m_directoryStatusListView
      // 
      this.m_directoryStatusListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
        this.m_directoryNameColumn,
        this.m_statusColumn});
      listViewGroup1.Header = "Passed";
      listViewGroup1.Name = "m_passedGroup";
      listViewGroup2.Header = "Failed";
      listViewGroup2.Name = "m_failedGroup";
      listViewGroup3.Header = "Partial";
      listViewGroup3.Name = "m_partialGroup";
      this.m_directoryStatusListView.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
        listViewGroup1,
        listViewGroup2,
        listViewGroup3});
      this.m_directoryStatusListView.Location = new System.Drawing.Point(16, 131);
      this.m_directoryStatusListView.Name = "m_directoryStatusListView";
      this.m_directoryStatusListView.ShowGroups = false;
      this.m_directoryStatusListView.Size = new System.Drawing.Size(657, 268);
      this.m_directoryStatusListView.Sorting = System.Windows.Forms.SortOrder.Ascending;
      this.m_directoryStatusListView.TabIndex = 4;
      this.m_directoryStatusListView.UseCompatibleStateImageBehavior = false;
      this.m_directoryStatusListView.View = System.Windows.Forms.View.Details;
      this.m_directoryStatusListView.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.OnResultsColumnClickedHandler);
      // 
      // m_directoryNameColumn
      // 
      this.m_directoryNameColumn.Text = "Directory Name";
      this.m_directoryNameColumn.Width = 531;
      // 
      // m_statusColumn
      // 
      this.m_statusColumn.Text = "Status";
      this.m_statusColumn.Width = 81;
      // 
      // ImportDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(694, 446);
      this.Controls.Add(this.m_directoryStatusListView);
      this.Controls.Add(this.m_destinationNavButton);
      this.Controls.Add(this.m_destinationLabel);
      this.Controls.Add(this.m_lowerPanel);
      this.Controls.Add(this.m_destinationText);
      this.Controls.Add(this.m_progressBar);
      this.Controls.Add(this.m_sourceNavButton);
      this.Controls.Add(this.m_sourceLabel);
      this.Controls.Add(this.m_sourceTexBox);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ImportDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Importer";
      this.Controls.SetChildIndex(this.m_sourceTexBox, 0);
      this.Controls.SetChildIndex(this.m_sourceLabel, 0);
      this.Controls.SetChildIndex(this.m_sourceNavButton, 0);
      this.Controls.SetChildIndex(this.m_progressBar, 0);
      this.Controls.SetChildIndex(this.m_destinationText, 0);
      this.Controls.SetChildIndex(this.m_lowerPanel, 0);
      this.Controls.SetChildIndex(this.m_destinationLabel, 0);
      this.Controls.SetChildIndex(this.m_destinationNavButton, 0);
      this.Controls.SetChildIndex(this.m_directoryStatusListView, 0);
      this.AcceptButton = m_buttonImport;
      this.CancelButton = m_buttonCancel;
      this.m_lowerPanel.ResumeLayout(false);
      this.m_lowerPanel.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

  } // class ImportDialog
} // namespace Viatronix.Utilities.Importer

  #region revision history

  // $Log: ImportDialog.designer.cs,v $
  // Revision 1.1.2.7  2010/05/20 15:08:09  kchalupa
  // Cancel Button Exits the program when not currently importing.
  //
  // Revision 1.1.2.6  2009/10/30 18:46:59  kchalupa
  // Corrected the tab order.
  //
  // Revision 1.1.2.5  2009/10/30 14:00:19  kchalupa
  // Added new controls directory status and cancel button.
  //
  // Revision 1.1.2.4  2009/06/04 18:54:21  kchalupa
  // Coding standards.
  //
  // $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ImportDialog.designer.cs,v 1.1.2.7 2010/05/20 15:08:09 kchalupa Exp $
  // $Id: ImportDialog.designer.cs,v 1.1.2.7 2010/05/20 15:08:09 kchalupa Exp $

  #endregion
