namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  /// <summary>
  /// The main form for the application.
  /// </summary>
  partial class MainForm
  {

    #region fields

    private System.Windows.Forms.Label m_directoryLabel;
    private System.Windows.Forms.TextBox m_directoryTextBox;
    private System.Windows.Forms.Button m_directoryNavigator;
    private System.Windows.Forms.TreeView m_directoriesTreeView;
    private System.Windows.Forms.Button m_loadButton;
    private System.Windows.Forms.Button m_selectAllButton;
    private System.Windows.Forms.Button m_selectNoneButton;
    private System.Windows.Forms.Button m_generateButton;

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    #endregion

    #region methods

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    } // Dispose( disposing )

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_directoryLabel = new System.Windows.Forms.Label();
      this.m_directoryTextBox = new System.Windows.Forms.TextBox();
      this.m_directoryNavigator = new System.Windows.Forms.Button();
      this.m_directoriesTreeView = new System.Windows.Forms.TreeView();
      this.m_generateButton = new System.Windows.Forms.Button();
      this.m_loadButton = new System.Windows.Forms.Button();
      this.m_selectAllButton = new System.Windows.Forms.Button();
      this.m_selectNoneButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // m_directoryLabel
      // 
      this.m_directoryLabel.AutoSize = true;
      this.m_directoryLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_directoryLabel.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_directoryLabel.Location = new System.Drawing.Point(12, 16);
      this.m_directoryLabel.Name = "m_directoryLabel";
      this.m_directoryLabel.Size = new System.Drawing.Size(60, 14);
      this.m_directoryLabel.TabIndex = 0;
      this.m_directoryLabel.Text = "Directory:";
      // 
      // m_directoryTextBox
      // 
      this.m_directoryTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_directoryTextBox.Location = new System.Drawing.Point(93, 13);
      this.m_directoryTextBox.Name = "m_directoryTextBox";
      this.m_directoryTextBox.Size = new System.Drawing.Size(271, 20);
      this.m_directoryTextBox.TabIndex = 1;
      // 
      // m_directoryNavigator
      // 
      this.m_directoryNavigator.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_directoryNavigator.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_directoryNavigator.Location = new System.Drawing.Point(372, 13);
      this.m_directoryNavigator.Name = "m_directoryNavigator";
      this.m_directoryNavigator.Size = new System.Drawing.Size(30, 22);
      this.m_directoryNavigator.TabIndex = 2;
      this.m_directoryNavigator.Text = "...";
      this.m_directoryNavigator.UseVisualStyleBackColor = true;
      this.m_directoryNavigator.Click += new System.EventHandler(this.OnDirectoryNavigatorClickHandler);
      // 
      // m_directoriesTreeView
      // 
      this.m_directoriesTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_directoriesTreeView.CheckBoxes = true;
      this.m_directoriesTreeView.FullRowSelect = true;
      this.m_directoriesTreeView.Location = new System.Drawing.Point(15, 54);
      this.m_directoriesTreeView.Name = "m_directoriesTreeView";
      this.m_directoriesTreeView.ShowNodeToolTips = true;
      this.m_directoriesTreeView.Size = new System.Drawing.Size(463, 285);
      this.m_directoriesTreeView.TabIndex = 4;
      this.m_directoriesTreeView.BeforeCheck += new System.Windows.Forms.TreeViewCancelEventHandler(this.OnDirectoriesTreeViewBeforeCheckHandler);
      this.m_directoriesTreeView.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.OnDirectoriesTreeViewAfterCheckHandler);
      // 
      // m_generateButton
      // 
      this.m_generateButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_generateButton.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_generateButton.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_generateButton.Location = new System.Drawing.Point(385, 350);
      this.m_generateButton.Name = "m_generateButton";
      this.m_generateButton.Size = new System.Drawing.Size(93, 25);
      this.m_generateButton.TabIndex = 7;
      this.m_generateButton.Text = "Generate";
      this.m_generateButton.UseVisualStyleBackColor = true;
      this.m_generateButton.Click += new System.EventHandler(this.OnGenerateClickHandler);
      // 
      // m_loadButton
      // 
      this.m_loadButton.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_loadButton.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_loadButton.Location = new System.Drawing.Point(418, 13);
      this.m_loadButton.Name = "m_loadButton";
      this.m_loadButton.Size = new System.Drawing.Size(60, 23);
      this.m_loadButton.TabIndex = 3;
      this.m_loadButton.Text = "Load";
      this.m_loadButton.UseVisualStyleBackColor = true;
      this.m_loadButton.Click += new System.EventHandler(this.OnLoadButtonClickHandler);
      // 
      // m_selectAllButton
      // 
      this.m_selectAllButton.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_selectAllButton.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_selectAllButton.Location = new System.Drawing.Point(15, 350);
      this.m_selectAllButton.Name = "m_selectAllButton";
      this.m_selectAllButton.Size = new System.Drawing.Size(84, 23);
      this.m_selectAllButton.TabIndex = 5;
      this.m_selectAllButton.Text = "Select All";
      this.m_selectAllButton.UseVisualStyleBackColor = true;
      this.m_selectAllButton.Click += new System.EventHandler(this.OnSelectAllClickHandler);
      // 
      // m_selectNoneButton
      // 
      this.m_selectNoneButton.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_selectNoneButton.ForeColor = System.Drawing.Color.DarkBlue;
      this.m_selectNoneButton.Location = new System.Drawing.Point(105, 350);
      this.m_selectNoneButton.Name = "m_selectNoneButton";
      this.m_selectNoneButton.Size = new System.Drawing.Size(95, 23);
      this.m_selectNoneButton.TabIndex = 6;
      this.m_selectNoneButton.Text = "Select None";
      this.m_selectNoneButton.UseVisualStyleBackColor = true;
      this.m_selectNoneButton.Click += new System.EventHandler(this.OnSelectNoneClickHandler);
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.LightSteelBlue;
      this.ClientSize = new System.Drawing.Size(490, 388);
      this.Controls.Add(this.m_selectNoneButton);
      this.Controls.Add(this.m_selectAllButton);
      this.Controls.Add(this.m_loadButton);
      this.Controls.Add(this.m_generateButton);
      this.Controls.Add(this.m_directoriesTreeView);
      this.Controls.Add(this.m_directoryNavigator);
      this.Controls.Add(this.m_directoryTextBox);
      this.Controls.Add(this.m_directoryLabel);
      this.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.Name = "MainForm";
      this.Text = "Selective Manifest Generator For QA";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

  } // class MainForm
} // Viatronix.Utilities.SelectiveManifestGenerator
