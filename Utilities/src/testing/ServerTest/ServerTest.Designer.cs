namespace Viatronix.Utilities.Dicom
{
  partial class ServerTestForm
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
      this.components = new System.ComponentModel.Container();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_listView = new System.Windows.Forms.ListView();
      this.m_chId = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chState = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chCalled = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chCalling = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chPort = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_chMessage = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.m_menuStrip = new System.Windows.Forms.MenuStrip();
      this.startToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.abortToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.m_menuStrip.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_listView
      // 
      this.m_listView.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
      this.m_listView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.m_chId,
            this.m_chType,
            this.m_chCount,
            this.m_chState,
            this.m_chCalled,
            this.m_chCalling,
            this.m_chPort,
            this.m_chMessage});
      this.m_listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_listView.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_listView.GridLines = true;
      this.m_listView.Location = new System.Drawing.Point(0, 24);
      this.m_listView.Name = "m_listView";
      this.m_listView.Size = new System.Drawing.Size(1382, 418);
      this.m_listView.TabIndex = 5;
      this.m_listView.UseCompatibleStateImageBehavior = false;
      this.m_listView.View = System.Windows.Forms.View.Details;
      // 
      // m_chId
      // 
      this.m_chId.Text = "Id";
      this.m_chId.Width = 306;
      // 
      // m_chType
      // 
      this.m_chType.Text = "Type";
      this.m_chType.Width = 100;
      // 
      // m_chCount
      // 
      this.m_chCount.Text = "Count";
      this.m_chCount.Width = 100;
      // 
      // m_chState
      // 
      this.m_chState.Text = "State";
      this.m_chState.Width = 150;
      // 
      // m_chCalled
      // 
      this.m_chCalled.Text = "Called AETitle";
      this.m_chCalled.Width = 150;
      // 
      // m_chCalling
      // 
      this.m_chCalling.Text = "Calling AETitle";
      this.m_chCalling.Width = 150;
      // 
      // m_chPort
      // 
      this.m_chPort.Text = "Port";
      // 
      // m_chMessage
      // 
      this.m_chMessage.Text = "Message";
      this.m_chMessage.Width = 700;
      // 
      // m_menuStrip
      // 
      this.m_menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.startToolStripMenuItem,
            this.abortToolStripMenuItem});
      this.m_menuStrip.Location = new System.Drawing.Point(0, 0);
      this.m_menuStrip.Name = "m_menuStrip";
      this.m_menuStrip.Size = new System.Drawing.Size(1382, 24);
      this.m_menuStrip.TabIndex = 7;
      this.m_menuStrip.Text = "menuStrip1";
      // 
      // startToolStripMenuItem
      // 
      this.startToolStripMenuItem.Name = "startToolStripMenuItem";
      this.startToolStripMenuItem.Size = new System.Drawing.Size(43, 20);
      this.startToolStripMenuItem.Text = "Start";
      this.startToolStripMenuItem.Click += new System.EventHandler(this.OnStartClick);
      // 
      // abortToolStripMenuItem
      // 
      this.abortToolStripMenuItem.Name = "abortToolStripMenuItem";
      this.abortToolStripMenuItem.Size = new System.Drawing.Size(43, 20);
      this.abortToolStripMenuItem.Text = "Stop";
      this.abortToolStripMenuItem.Click += new System.EventHandler(this.OnAbortClick);
      // 
      // ServerTestForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1382, 442);
      this.Controls.Add(this.m_listView);
      this.Controls.Add(this.m_menuStrip);
      this.MainMenuStrip = this.m_menuStrip;
      this.Name = "ServerTestForm";
      this.Text = "Dicom Server Test";
      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.ServerTestForm_FormClosed);
      this.m_menuStrip.ResumeLayout(false);
      this.m_menuStrip.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.ListView m_listView;
    private System.Windows.Forms.ColumnHeader m_chId;
    private System.Windows.Forms.ColumnHeader m_chType;
    private System.Windows.Forms.ColumnHeader m_chCount;
    private System.Windows.Forms.ColumnHeader m_chState;
    private System.Windows.Forms.ColumnHeader m_chCalled;
    private System.Windows.Forms.ColumnHeader m_chCalling;
    private System.Windows.Forms.ColumnHeader m_chMessage;
    private System.Windows.Forms.ColumnHeader m_chPort;
    private System.Windows.Forms.MenuStrip m_menuStrip;
    private System.Windows.Forms.ToolStripMenuItem startToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem abortToolStripMenuItem;
  }
}

