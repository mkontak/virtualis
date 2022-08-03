namespace Viatronix.Console
{
  partial class SearchControl
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
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_clearButton = new Viatronix.UI.Button();
      this.m_resultsLabel = new System.Windows.Forms.Label();
      this.m_searchButton = new Viatronix.UI.Button();
      this.m_cancelButton = new Viatronix.UI.Button();
      this.m_progressbar = new Viatronix.UI.ProgressBar2();
      this.SuspendLayout();
      // 
      // m_clearButton
      // 
      this.m_clearButton.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_clearButton.Checked = false;
      this.m_clearButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_clearButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_clearButton.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_clearButton.Location = new System.Drawing.Point(189, 1);
      this.m_clearButton.Name = "m_clearButton";
      this.m_clearButton.OldStockButton = false;
      this.m_clearButton.Size = new System.Drawing.Size(75, 23);
      this.m_clearButton.TabIndex = 14;
      this.m_clearButton.Text = "Clear";
      this.m_clearButton.ToggleButton = false;
      this.m_clearButton.Click += new System.EventHandler(this.OnClearClickhandler);
      // 
      // m_resultsLabel
      // 
      this.m_resultsLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
      this.m_resultsLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_resultsLabel.ForeColor = System.Drawing.Color.White;
      this.m_resultsLabel.Location = new System.Drawing.Point(3, 0);
      this.m_resultsLabel.Name = "m_resultsLabel";
      this.m_resultsLabel.Size = new System.Drawing.Size(101, 23);
      this.m_resultsLabel.TabIndex = 15;
      this.m_resultsLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // m_searchButton
      // 
      this.m_searchButton.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_searchButton.AutoSize = true;
      this.m_searchButton.Checked = false;
      this.m_searchButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_searchButton.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_searchButton.Location = new System.Drawing.Point(108, 1);
      this.m_searchButton.Name = "m_searchButton";
      this.m_searchButton.OldStockButton = false;
      this.m_searchButton.Size = new System.Drawing.Size(75, 23);
      this.m_searchButton.TabIndex = 13;
      this.m_searchButton.Text = "Search";
      this.m_searchButton.ToggleButton = false;
      this.m_searchButton.Click += new System.EventHandler(this.OnSearchClickHandler);
      // 
      // m_cancelButton
      // 
      this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cancelButton.AutoSize = true;
      this.m_cancelButton.Checked = false;
      this.m_cancelButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancelButton.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_cancelButton.Location = new System.Drawing.Point(110, 1);
      this.m_cancelButton.Name = "m_cancelButton";
      this.m_cancelButton.OldStockButton = false;
      this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
      this.m_cancelButton.TabIndex = 12;
      this.m_cancelButton.Text = "Cancel";
      this.m_cancelButton.ToggleButton = false;
      this.m_cancelButton.Visible = false;
      this.m_cancelButton.Click += new System.EventHandler(this.OnCancelClickHandler);
      // 
      // m_progressbar
      // 
      this.m_progressbar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
      this.m_progressbar.BackColor = System.Drawing.Color.Transparent;
      this.m_progressbar.DrawText = true;
      this.m_progressbar.Location = new System.Drawing.Point(4, 0);
      this.m_progressbar.Maximum = 100;
      this.m_progressbar.Minimum = 0;
      this.m_progressbar.Name = "m_progressbar";
      this.m_progressbar.Pulse = false;
      this.m_progressbar.Size = new System.Drawing.Size(98, 23);
      this.m_progressbar.Step = 1;
      this.m_progressbar.TabIndex = 16;
      this.m_progressbar.TabStop = false;
      this.m_progressbar.Value = 0;
      this.m_progressbar.Visible = false;
      // 
      // SearchControl
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.Controls.Add(this.m_clearButton);
      this.Controls.Add(this.m_resultsLabel);
      this.Controls.Add(this.m_searchButton);
      this.Controls.Add(this.m_cancelButton);
      this.Controls.Add(this.m_progressbar);
      this.ForeColor = System.Drawing.SystemColors.ControlText;
      this.Name = "SearchControl";
      this.Size = new System.Drawing.Size(264, 23);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion
  }
}
