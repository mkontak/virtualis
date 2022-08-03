namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  partial class CADConsoleForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if ( disposing && ( components != null ) )
      {
        components.Dispose();
      }
      base.Dispose( disposing );
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CADConsoleForm));
      this.m_buttonRun = new Viatronix.v3D.UI.Button();
      this.m_Close = new Viatronix.v3D.UI.Button();
      this.m_appIds = new System.Windows.Forms.ComboBox();
      this.m_labelAppId = new System.Windows.Forms.Label();
      this.m_browseDICOM = new Viatronix.v3D.UI.Button();
      this.button1 = new Viatronix.v3D.UI.Button();
      this.m_OK = new Viatronix.v3D.UI.Button();
      this.button2 = new Viatronix.v3D.UI.Button();
      this.m_buttonOk = new Viatronix.v3D.UI.Button();
      this.m_Cancel = new Viatronix.v3D.UI.Button();
      this.checkBox1 = new Viatronix.v3D.UI.CheckBox();
      this.comboBox1 = new System.Windows.Forms.ComboBox();
      this.m_destinationBrowse = new Viatronix.v3D.UI.Button();
      this.m_labelDestination = new System.Windows.Forms.Label();
      this.m_destinationFolder = new System.Windows.Forms.TextBox();
      this.m_labelDCMFolder = new System.Windows.Forms.Label();
      this.m_buttonDICOMBrowse = new Viatronix.v3D.UI.Button();
      this.m_dicomFolder = new System.Windows.Forms.TextBox();
      this.m_preProcessCheck = new Viatronix.v3D.UI.CheckBox();
      this.m_progressbar = new Viatronix.v3D.UI.ProgressBar2();
      this.SuspendLayout();
      // 
      // m_buttonRun
      // 
      this.m_buttonRun.ForeColor = System.Drawing.Color.Black;
      this.m_buttonRun.Location = new System.Drawing.Point(211, 123);
      this.m_buttonRun.Name = "m_buttonRun";
      this.m_buttonRun.OldStockButton = false;
      this.m_buttonRun.Size = new System.Drawing.Size(59, 23);
      this.m_buttonRun.TabIndex = 6;
      this.m_buttonRun.Text = "Run";
      this.m_buttonRun.UseVisualStyleBackColor = true;
      this.m_buttonRun.Click += new System.EventHandler(this.OnRunClickHandler);
      // 
      // m_Close
      // 
      this.m_Close.ForeColor = System.Drawing.Color.Black;
      this.m_Close.Location = new System.Drawing.Point(273, 123);
      this.m_Close.Name = "m_Close";
      this.m_Close.OldStockButton = false;
      this.m_Close.Size = new System.Drawing.Size(57, 23);
      this.m_Close.TabIndex = 7;
      this.m_Close.Text = "Close";
      this.m_Close.UseVisualStyleBackColor = true;
      this.m_Close.Click += new System.EventHandler(this.OnCloseClickHandler);
      // 
      // m_appIds
      // 
      this.m_appIds.FormattingEnabled = true;
      this.m_appIds.Location = new System.Drawing.Point(87, 60);
      this.m_appIds.Name = "m_appIds";
      this.m_appIds.RightToLeft = System.Windows.Forms.RightToLeft.No;
      this.m_appIds.Size = new System.Drawing.Size(213, 22);
      this.m_appIds.TabIndex = 2;
      this.m_appIds.Leave += new System.EventHandler(this.OnLeaveFocus);
      // 
      // m_labelAppId
      // 
      this.m_labelAppId.AutoSize = true;
      this.m_labelAppId.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.m_labelAppId.Location = new System.Drawing.Point(36, 60);
      this.m_labelAppId.Name = "m_labelAppId";
      this.m_labelAppId.Size = new System.Drawing.Size(47, 14);
      this.m_labelAppId.TabIndex = 9;
      this.m_labelAppId.Text = "Plugin ID";
      // 
      // m_browseDICOM
      // 
      this.m_browseDICOM.ForeColor = System.Drawing.Color.Black;
      this.m_browseDICOM.Location = new System.Drawing.Point(367, 138);
      this.m_browseDICOM.Name = "m_browseDICOM";
      this.m_browseDICOM.OldStockButton = false;
      this.m_browseDICOM.Size = new System.Drawing.Size(28, 20);
      this.m_browseDICOM.TabIndex = 4;
      this.m_browseDICOM.Text = "...";
      this.m_browseDICOM.UseVisualStyleBackColor = true;
      // 
      // button1
      // 
      this.button1.ForeColor = System.Drawing.Color.Black;
      this.button1.Location = new System.Drawing.Point(407, 342);
      this.button1.Name = "button1";
      this.button1.OldStockButton = false;
      this.button1.Size = new System.Drawing.Size(75, 23);
      this.button1.TabIndex = 1;
      this.button1.Text = "button1";
      this.button1.UseVisualStyleBackColor = true;
      // 
      // m_OK
      // 
      this.m_OK.ForeColor = System.Drawing.Color.Black;
      this.m_OK.Location = new System.Drawing.Point(407, 342);
      this.m_OK.Name = "m_OK";
      this.m_OK.OldStockButton = false;
      this.m_OK.Size = new System.Drawing.Size(75, 23);
      this.m_OK.TabIndex = 1;
      this.m_OK.Text = "button1";
      this.m_OK.UseVisualStyleBackColor = true;
      // 
      // button2
      // 
      this.button2.ForeColor = System.Drawing.Color.Black;
      this.button2.Location = new System.Drawing.Point(488, 342);
      this.button2.Name = "button2";
      this.button2.OldStockButton = false;
      this.button2.Size = new System.Drawing.Size(75, 23);
      this.button2.TabIndex = 2;
      this.button2.Text = "button2";
      this.button2.UseVisualStyleBackColor = true;
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.ForeColor = System.Drawing.Color.Black;
      this.m_buttonOk.Location = new System.Drawing.Point(419, 342);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.OldStockButton = false;
      this.m_buttonOk.Size = new System.Drawing.Size(63, 23);
      this.m_buttonOk.TabIndex = 1;
      this.m_buttonOk.Text = "Run";
      this.m_buttonOk.UseVisualStyleBackColor = true;
      // 
      // m_Cancel
      // 
      this.m_Cancel.ForeColor = System.Drawing.Color.Black;
      this.m_Cancel.Location = new System.Drawing.Point(488, 342);
      this.m_Cancel.Name = "m_Cancel";
      this.m_Cancel.OldStockButton = false;
      this.m_Cancel.Size = new System.Drawing.Size(75, 23);
      this.m_Cancel.TabIndex = 2;
      this.m_Cancel.Text = "Close";
      this.m_Cancel.UseVisualStyleBackColor = true;
      // 
      // checkBox1
      // 
      this.checkBox1.AutoSize = true;
      this.checkBox1.Location = new System.Drawing.Point(142, 58);
      this.checkBox1.Name = "checkBox1";
      this.checkBox1.Size = new System.Drawing.Size(80, 18);
      this.checkBox1.TabIndex = 3;
      this.checkBox1.Text = "checkBox1";
      this.checkBox1.UseVisualStyleBackColor = true;
      // 
      // comboBox1
      // 
      this.comboBox1.FormattingEnabled = true;
      this.comboBox1.Location = new System.Drawing.Point(142, 96);
      this.comboBox1.Name = "comboBox1";
      this.comboBox1.Size = new System.Drawing.Size(121, 21);
      this.comboBox1.TabIndex = 8;
      // 
      // m_destinationBrowse
      // 
      this.m_destinationBrowse.ForeColor = System.Drawing.Color.Black;
      this.m_destinationBrowse.Location = new System.Drawing.Point(306, 8);
      this.m_destinationBrowse.Name = "m_destinationBrowse";
      this.m_destinationBrowse.OldStockButton = false;
      this.m_destinationBrowse.Size = new System.Drawing.Size(26, 20);
      this.m_destinationBrowse.TabIndex = 4;
      this.m_destinationBrowse.Text = "...";
      this.m_destinationBrowse.UseVisualStyleBackColor = true;
      this.m_destinationBrowse.Click += new System.EventHandler(this.OnDestFolderBrowseClickHandler);
      // 
      // m_labelDestination
      // 
      this.m_labelDestination.AutoSize = true;
      this.m_labelDestination.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.m_labelDestination.Location = new System.Drawing.Point(4, 9);
      this.m_labelDestination.Name = "m_labelDestination";
      this.m_labelDestination.Size = new System.Drawing.Size(79, 14);
      this.m_labelDestination.TabIndex = 11;
      this.m_labelDestination.Text = "Working Folder";
      // 
      // m_destinationFolder
      // 
      this.m_destinationFolder.Location = new System.Drawing.Point(87, 8);
      this.m_destinationFolder.Name = "m_destinationFolder";
      this.m_destinationFolder.Size = new System.Drawing.Size(213, 20);
      this.m_destinationFolder.TabIndex = 0;
      // 
      // m_labelDCMFolder
      // 
      this.m_labelDCMFolder.AutoSize = true;
      this.m_labelDCMFolder.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.m_labelDCMFolder.Location = new System.Drawing.Point(11, 35);
      this.m_labelDCMFolder.Name = "m_labelDCMFolder";
      this.m_labelDCMFolder.Size = new System.Drawing.Size(72, 14);
      this.m_labelDCMFolder.TabIndex = 6;
      this.m_labelDCMFolder.Text = "DICOM Folder";
      // 
      // m_buttonDICOMBrowse
      // 
      this.m_buttonDICOMBrowse.ForeColor = System.Drawing.Color.Black;
      this.m_buttonDICOMBrowse.Location = new System.Drawing.Point(306, 34);
      this.m_buttonDICOMBrowse.Name = "m_buttonDICOMBrowse";
      this.m_buttonDICOMBrowse.OldStockButton = false;
      this.m_buttonDICOMBrowse.Size = new System.Drawing.Size(26, 20);
      this.m_buttonDICOMBrowse.TabIndex = 5;
      this.m_buttonDICOMBrowse.Text = "...";
      this.m_buttonDICOMBrowse.UseVisualStyleBackColor = true;
      this.m_buttonDICOMBrowse.Click += new System.EventHandler(this.OnDICOMBrowseClickHandler);
      // 
      // m_dicomFolder
      // 
      this.m_dicomFolder.Location = new System.Drawing.Point(87, 34);
      this.m_dicomFolder.Name = "m_dicomFolder";
      this.m_dicomFolder.Size = new System.Drawing.Size(213, 20);
      this.m_dicomFolder.TabIndex = 1;
      // 
      // m_preProcessCheck
      // 
      this.m_preProcessCheck.AutoSize = true;
      this.m_preProcessCheck.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.m_preProcessCheck.Location = new System.Drawing.Point(78, 88);
      this.m_preProcessCheck.Name = "m_preProcessCheck";
      this.m_preProcessCheck.Size = new System.Drawing.Size(133, 18);
      this.m_preProcessCheck.TabIndex = 3;
      this.m_preProcessCheck.Text = "Peform preprocessing";
      this.m_preProcessCheck.UseVisualStyleBackColor = true;
      this.m_preProcessCheck.Click += new System.EventHandler(this.OnPreprocessCheckHandler);
      // 
      // m_progressbar
      // 
      this.m_progressbar.BackColor = System.Drawing.Color.Transparent;
      this.m_progressbar.DrawText = true;
      this.m_progressbar.Location = new System.Drawing.Point(2, 152);
      this.m_progressbar.Maximum = 100;
      this.m_progressbar.Minimum = 0;
      this.m_progressbar.Name = "m_progressbar";
      this.m_progressbar.Pulse = false;
      this.m_progressbar.Size = new System.Drawing.Size(336, 18);
      this.m_progressbar.Step = 1;
      this.m_progressbar.TabIndex = 13;
      this.m_progressbar.Value = 0;
      // 
      // CADConsoleForm
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(340, 171);
      this.Controls.Add(this.m_progressbar);
      this.Controls.Add(this.m_destinationBrowse);
      this.Controls.Add(this.m_destinationFolder);
      this.Controls.Add(this.m_labelDestination);
      this.Controls.Add(this.m_dicomFolder);
      this.Controls.Add(this.m_labelAppId);
      this.Controls.Add(this.m_appIds);
      this.Controls.Add(this.m_labelDCMFolder);
      this.Controls.Add(this.m_preProcessCheck);
      this.Controls.Add(this.m_Close);
      this.Controls.Add(this.m_buttonDICOMBrowse);
      this.Controls.Add(this.m_buttonRun);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "CADConsoleForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Viatronix CAD Processor Test Console";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OnFormClosing);
      this.Controls.SetChildIndex(this.m_buttonRun, 0);
      this.Controls.SetChildIndex(this.m_buttonDICOMBrowse, 0);
      this.Controls.SetChildIndex(this.m_Close, 0);
      this.Controls.SetChildIndex(this.m_preProcessCheck, 0);
      this.Controls.SetChildIndex(this.m_labelDCMFolder, 0);
      this.Controls.SetChildIndex(this.m_appIds, 0);
      this.Controls.SetChildIndex(this.m_labelAppId, 0);
      this.Controls.SetChildIndex(this.m_dicomFolder, 0);
      this.Controls.SetChildIndex(this.m_labelDestination, 0);
      this.Controls.SetChildIndex(this.m_destinationFolder, 0);
      this.Controls.SetChildIndex(this.m_destinationBrowse, 0);
      this.Controls.SetChildIndex(this.m_progressbar, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private Viatronix.v3D.UI.Button m_buttonRun;
    private Viatronix.v3D.UI.Button m_Close;
    private System.Windows.Forms.ComboBox m_appIds;
    private System.Windows.Forms.Label m_labelAppId;
    private Viatronix.v3D.UI.Button m_browseDICOM;
    private Viatronix.v3D.UI.Button button1;
    private Viatronix.v3D.UI.Button m_OK;
    private Viatronix.v3D.UI.Button button2;
    private Viatronix.v3D.UI.Button m_buttonOk;
    private Viatronix.v3D.UI.Button m_Cancel;
    private Viatronix.v3D.UI.CheckBox checkBox1;
    private System.Windows.Forms.ComboBox comboBox1;
    private Viatronix.v3D.UI.Button m_destinationBrowse;
    private System.Windows.Forms.Label m_labelDestination;
    private System.Windows.Forms.TextBox m_destinationFolder;
    private System.Windows.Forms.Label m_labelDCMFolder;
    private Viatronix.v3D.UI.Button m_buttonDICOMBrowse;
    private System.Windows.Forms.TextBox m_dicomFolder;
    private Viatronix.v3D.UI.CheckBox m_preProcessCheck;
    private Viatronix.v3D.UI.ProgressBar2 m_progressbar;
  }
}

