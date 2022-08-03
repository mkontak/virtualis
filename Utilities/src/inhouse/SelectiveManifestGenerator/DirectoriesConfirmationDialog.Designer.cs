namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  partial class DirectoriesConfirmationDialog
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

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_cancelButton = new System.Windows.Forms.Button();
      this.m_okButton = new System.Windows.Forms.Button();
      this.m_confirmationLabel = new System.Windows.Forms.Label();
      this.m_directoriesListBox = new System.Windows.Forms.ListBox();
      this.SuspendLayout();
      // 
      // m_cancelButton
      // 
      this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_cancelButton.Location = new System.Drawing.Point(392, 195);
      this.m_cancelButton.Name = "m_cancelButton";
      this.m_cancelButton.Size = new System.Drawing.Size(87, 25);
      this.m_cancelButton.TabIndex = 3;
      this.m_cancelButton.Text = "Cancel";
      this.m_cancelButton.UseVisualStyleBackColor = true;
      // 
      // m_okButton
      // 
      this.m_okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.m_okButton.Location = new System.Drawing.Point(311, 195);
      this.m_okButton.Name = "m_okButton";
      this.m_okButton.Size = new System.Drawing.Size(75, 26);
      this.m_okButton.TabIndex = 2;
      this.m_okButton.Text = "OK";
      this.m_okButton.UseVisualStyleBackColor = true;
      // 
      // m_confirmationLabel
      // 
      this.m_confirmationLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_confirmationLabel.Location = new System.Drawing.Point(13, 13);
      this.m_confirmationLabel.Name = "m_confirmationLabel";
      this.m_confirmationLabel.Size = new System.Drawing.Size(466, 22);
      this.m_confirmationLabel.TabIndex = 0;
      this.m_confirmationLabel.Text = "Are you sure you want to write out manifest data for the following directories?";
      this.m_confirmationLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // m_directoriesListBox
      // 
      this.m_directoriesListBox.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_directoriesListBox.FormattingEnabled = true;
      this.m_directoriesListBox.ItemHeight = 14;
      this.m_directoriesListBox.Location = new System.Drawing.Point(26, 49);
      this.m_directoriesListBox.Name = "m_directoriesListBox";
      this.m_directoriesListBox.Size = new System.Drawing.Size(453, 130);
      this.m_directoriesListBox.TabIndex = 1;
      // 
      // DirectoriesConfirmationDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.LightSteelBlue;
      this.ClientSize = new System.Drawing.Size(493, 233);
      this.Controls.Add(this.m_directoriesListBox);
      this.Controls.Add(this.m_confirmationLabel);
      this.Controls.Add(this.m_okButton);
      this.Controls.Add(this.m_cancelButton);
      this.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ForeColor = System.Drawing.Color.DarkBlue;
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Name = "DirectoriesConfirmationDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Confirm Directories To Include In Output";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button m_cancelButton;
    private System.Windows.Forms.Button m_okButton;
    private System.Windows.Forms.Label m_confirmationLabel;
    private System.Windows.Forms.ListBox m_directoriesListBox;

  }
}