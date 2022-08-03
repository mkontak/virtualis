namespace Viatronix.UI
{
  partial class ImageListView
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
      this.m_panelPrev = new System.Windows.Forms.Panel();
      this.m_panelNext = new System.Windows.Forms.Panel();
      this.m_buttonNext = new Viatronix.UI.Button();
      this.m_buttonPrev = new Viatronix.UI.Button();
      this.SuspendLayout();
      // 
      // m_panelPrev
      // 
      this.m_panelPrev.BackColor = System.Drawing.Color.Transparent;
      this.m_panelPrev.Dock = System.Windows.Forms.DockStyle.Left;
      this.m_panelPrev.Location = new System.Drawing.Point(39, 0);
      this.m_panelPrev.Name = "m_panelPrev";
      this.m_panelPrev.Size = new System.Drawing.Size(34, 150);
      this.m_panelPrev.TabIndex = 2;
      this.m_panelPrev.Tag = "PREV";
      this.m_panelPrev.Visible = false;
      this.m_panelPrev.MouseLeave += new System.EventHandler(this.OnMouseLeaveButton);
      this.m_panelPrev.Paint += new System.Windows.Forms.PaintEventHandler(this.OnPrevPaint);
      this.m_panelPrev.Click += new System.EventHandler(this.OnPrevClick);
      this.m_panelPrev.MouseEnter += new System.EventHandler(this.OnMouseEnterButton);
      // 
      // m_panelNext
      // 
      this.m_panelNext.BackColor = System.Drawing.Color.Transparent;
      this.m_panelNext.Dock = System.Windows.Forms.DockStyle.Right;
      this.m_panelNext.Location = new System.Drawing.Point(255, 0);
      this.m_panelNext.Name = "m_panelNext";
      this.m_panelNext.Size = new System.Drawing.Size(34, 150);
      this.m_panelNext.TabIndex = 3;
      this.m_panelNext.Tag = "NEXT";
      this.m_panelNext.Visible = false;
      this.m_panelNext.MouseLeave += new System.EventHandler(this.OnMouseLeaveButton);
      this.m_panelNext.Paint += new System.Windows.Forms.PaintEventHandler(this.OnNextPaint);
      this.m_panelNext.Click += new System.EventHandler(this.OnNextClick);
      this.m_panelNext.MouseEnter += new System.EventHandler(this.OnMouseEnterButton);
      // 
      // m_buttonNext
      // 
      this.m_buttonNext.Checked = false;
      this.m_buttonNext.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonNext.Dock = System.Windows.Forms.DockStyle.Right;
      this.m_buttonNext.ForeColor = System.Drawing.Color.Black;
      this.m_buttonNext.Location = new System.Drawing.Point(289, 0);
      this.m_buttonNext.Name = "m_buttonNext";
      this.m_buttonNext.OldStockButton = false;
      this.m_buttonNext.Size = new System.Drawing.Size(39, 150);
      this.m_buttonNext.TabIndex = 1;
      this.m_buttonNext.ToggleButton = false;
      this.m_buttonNext.UseVisualStyleBackColor = true;
      this.m_buttonNext.Click += new System.EventHandler(this.OnNextClick);
      // 
      // m_buttonPrev
      // 
      this.m_buttonPrev.Checked = false;
      this.m_buttonPrev.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonPrev.Dock = System.Windows.Forms.DockStyle.Left;
      this.m_buttonPrev.ForeColor = System.Drawing.Color.Black;
      this.m_buttonPrev.Location = new System.Drawing.Point(0, 0);
      this.m_buttonPrev.Name = "m_buttonPrev";
      this.m_buttonPrev.OldStockButton = false;
      this.m_buttonPrev.Size = new System.Drawing.Size(39, 150);
      this.m_buttonPrev.TabIndex = 0;
      this.m_buttonPrev.ToggleButton = false;
      this.m_buttonPrev.UseVisualStyleBackColor = true;
      this.m_buttonPrev.Click += new System.EventHandler(this.OnPrevClick);
      // 
      // ImageListView
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.m_panelNext);
      this.Controls.Add(this.m_panelPrev);
      this.Controls.Add(this.m_buttonNext);
      this.Controls.Add(this.m_buttonPrev);
      this.DoubleBuffered = true;
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Name = "ImageListView";
      this.Size = new System.Drawing.Size(328, 150);
      this.Click += new System.EventHandler(this.OnClick);
      this.ResumeLayout(false);

    }

    #endregion

    private Button m_buttonPrev;
    private Button m_buttonNext;
    private System.Windows.Forms.Panel m_panelPrev;
    private System.Windows.Forms.Panel m_panelNext;
  }
}
