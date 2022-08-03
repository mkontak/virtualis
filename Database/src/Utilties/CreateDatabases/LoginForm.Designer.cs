namespace Viatronix.Utilities.Database
{
  partial class LoginForm
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
      this.label2 = new System.Windows.Forms.Label();
      this.m_textBoxPassword = new System.Windows.Forms.TextBox();
      this.m_textBoxServer = new System.Windows.Forms.TextBox();
      this.m_labelServer = new System.Windows.Forms.Label();
      this.m_buttonLogin = new System.Windows.Forms.Button();
      this.m_buttonCancel = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // label2
      // 
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label2.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.label2.Location = new System.Drawing.Point(13, 35);
      this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(71, 21);
      this.label2.TabIndex = 35;
      this.label2.Text = "Password:";
      // 
      // m_textBoxPassword
      // 
      this.m_textBoxPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxPassword.Location = new System.Drawing.Point(107, 37);
      this.m_textBoxPassword.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxPassword.Name = "m_textBoxPassword";
      this.m_textBoxPassword.PasswordChar = '*';
      this.m_textBoxPassword.Size = new System.Drawing.Size(181, 21);
      this.m_textBoxPassword.TabIndex = 3;
      // 
      // m_textBoxServer
      // 
      this.m_textBoxServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxServer.Location = new System.Drawing.Point(107, 10);
      this.m_textBoxServer.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxServer.Name = "m_textBoxServer";
      this.m_textBoxServer.ReadOnly = true;
      this.m_textBoxServer.Size = new System.Drawing.Size(181, 21);
      this.m_textBoxServer.TabIndex = 1;
      this.m_textBoxServer.TabStop = false;
      // 
      // m_labelServer
      // 
      this.m_labelServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelServer.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelServer.Location = new System.Drawing.Point(13, 9);
      this.m_labelServer.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.m_labelServer.Name = "m_labelServer";
      this.m_labelServer.Size = new System.Drawing.Size(71, 21);
      this.m_labelServer.TabIndex = 30;
      this.m_labelServer.Text = "Server:";
      // 
      // m_buttonLogin
      // 
      this.m_buttonLogin.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonLogin.Location = new System.Drawing.Point(12, 71);
      this.m_buttonLogin.Name = "m_buttonLogin";
      this.m_buttonLogin.Size = new System.Drawing.Size(281, 23);
      this.m_buttonLogin.TabIndex = 36;
      this.m_buttonLogin.Text = "Login";
      this.m_buttonLogin.UseVisualStyleBackColor = false;
      this.m_buttonLogin.Click += new System.EventHandler(this.OnLoginClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Location = new System.Drawing.Point(11, 100);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(282, 23);
      this.m_buttonCancel.TabIndex = 37;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.UseVisualStyleBackColor = false;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // LoginForm
      // 
      this.AcceptButton = this.m_buttonLogin;
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.SlateGray;
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(301, 135);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonLogin);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.m_textBoxPassword);
      this.Controls.Add(this.m_textBoxServer);
      this.Controls.Add(this.m_labelServer);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.Name = "LoginForm";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Database Login";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox m_textBoxPassword;
    private System.Windows.Forms.TextBox m_textBoxServer;
    private System.Windows.Forms.Label m_labelServer;
    private System.Windows.Forms.Button m_buttonLogin;
    private System.Windows.Forms.Button m_buttonCancel;
  }
}