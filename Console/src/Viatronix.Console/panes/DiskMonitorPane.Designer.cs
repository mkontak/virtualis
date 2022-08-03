namespace Viatronix.Console
{
  partial class DiskMonitorPane
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
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_timer = new System.Windows.Forms.Timer(this.components);
      this.SuspendLayout();
      // 
      // DiskMonitorPane
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Name = "DiskMonitorPane";
      this.Size = new System.Drawing.Size(300, 78);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.Timer m_timer;


  }
}
