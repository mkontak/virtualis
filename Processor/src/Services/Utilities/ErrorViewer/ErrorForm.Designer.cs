namespace ErrorViewer
{
  partial class ErrorForm
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ErrorForm));
      this.m_buttonOk = new System.Windows.Forms.Button();
      this.m_textBoxError = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.BackColor = System.Drawing.SystemColors.Control;
      this.m_buttonOk.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonOk.Location = new System.Drawing.Point(240, 194);
      this.m_buttonOk.Margin = new System.Windows.Forms.Padding(4);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.Size = new System.Drawing.Size(112, 28);
      this.m_buttonOk.TabIndex = 0;
      this.m_buttonOk.Text = "Ok";
      this.m_buttonOk.UseVisualStyleBackColor = false;
      this.m_buttonOk.Click += new System.EventHandler(this.OnButtonClick);
      // 
      // m_textBoxError
      // 
      this.m_textBoxError.BackColor = System.Drawing.SystemColors.Control;
      this.m_textBoxError.CausesValidation = false;
      this.m_textBoxError.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxError.ForeColor = System.Drawing.Color.Maroon;
      this.m_textBoxError.Location = new System.Drawing.Point(18, 15);
      this.m_textBoxError.Margin = new System.Windows.Forms.Padding(4);
      this.m_textBoxError.Multiline = true;
      this.m_textBoxError.Name = "m_textBoxError";
      this.m_textBoxError.ReadOnly = true;
      this.m_textBoxError.Size = new System.Drawing.Size(553, 158);
      this.m_textBoxError.TabIndex = 1;
      this.m_textBoxError.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ErrorForm
      // 
      this.AcceptButton = this.m_buttonOk;
      this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.SystemColors.Control;
      this.CancelButton = this.m_buttonOk;
      this.ClientSize = new System.Drawing.Size(591, 234);
      this.Controls.Add(this.m_textBoxError);
      this.Controls.Add(this.m_buttonOk);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ForeColor = System.Drawing.Color.Maroon;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Margin = new System.Windows.Forms.Padding(4);
      this.Name = "ErrorForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Error";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button m_buttonOk;
    private System.Windows.Forms.TextBox m_textBoxError;
  }
}

