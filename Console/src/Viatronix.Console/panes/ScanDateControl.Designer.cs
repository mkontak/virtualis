namespace Viatronix.Console
{
  partial class ScanDateControl
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
      this.components = new System.ComponentModel.Container();
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;

      this.m_scanDateLabel = new System.Windows.Forms.Label();
      this.m_scanDate = new Viatronix.UI.DateChooser();
      this.m_scanDateStyle = new Viatronix.UI.ComboBox();
      this.SuspendLayout();
      // 
      // m_scanDateLabel
      // 
      this.m_scanDateLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_scanDateLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_scanDateLabel.ForeColor = System.Drawing.Color.White;
      this.m_scanDateLabel.Location = new System.Drawing.Point(0, -1);
      this.m_scanDateLabel.Name = "m_scanDateLabel";
      this.m_scanDateLabel.Size = new System.Drawing.Size(64, 23);
      this.m_scanDateLabel.TabIndex = 9;
      this.m_scanDateLabel.Text = "Scan Date:";
      this.m_scanDateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_scanDate
      // 
      this.m_scanDate.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_scanDate.BackColor = System.Drawing.Color.Transparent;
      this.m_scanDate.Enabled = false;
      this.m_scanDate.ForeColor = System.Drawing.Color.White;
      this.m_scanDate.IsRangeSearch = false;
      this.m_scanDate.LabelColor = System.Drawing.Color.White;
      this.m_scanDate.Location = new System.Drawing.Point(0, 30);
      this.m_scanDate.Name = "m_scanDate";
      this.m_scanDate.Size = new System.Drawing.Size(272, 56);
      this.m_scanDate.TabIndex = 2;
      // 
      // m_scanDateStyle
      // 
      this.m_scanDateStyle.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_scanDateStyle.DisplayMember = "String";
      this.m_scanDateStyle.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_scanDateStyle.Items.AddRange(new object[] {
            "Not Used",
            "Specific Date",
            "Date Range"});
      this.m_scanDateStyle.Location = new System.Drawing.Point(168, 0);
      this.m_scanDateStyle.Name = "m_scanDateStyle";
      this.m_scanDateStyle.Size = new System.Drawing.Size(96, 21);
      this.m_scanDateStyle.TabIndex = 1;
      this.m_scanDateStyle.ValueMember = "Object";
      this.m_scanDateStyle.SelectedIndexChanged += new System.EventHandler(this.OnScanDateChangedHandler);
      // 
      // ScanDateControl
      // 
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.Controls.Add(this.m_scanDate);
      this.Controls.Add(this.m_scanDateStyle);
      this.Controls.Add(this.m_scanDateLabel);
      this.Name = "ScanDateControl";
      this.Size = new System.Drawing.Size(264, 86);
      this.ResumeLayout(false);

    }

    #endregion
  }
}
