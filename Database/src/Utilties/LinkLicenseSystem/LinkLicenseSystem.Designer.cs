namespace Viatronix.Utilities.Database
{
  partial class LinkLicenseSystem
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LinkLicenseSystem));
      this.label2 = new System.Windows.Forms.Label();
      this.m_textBoxEnterprisePassword = new System.Windows.Forms.TextBox();
      this.m_textBoxEnterpriseServer = new System.Windows.Forms.TextBox();
      this.m_labelEnterpriseServer = new System.Windows.Forms.Label();
      this.m_buttonEnterpriseTest = new System.Windows.Forms.Button();
      this.m_buttonCancel = new System.Windows.Forms.Button();
      this.m_buttonLink = new System.Windows.Forms.Button();
      this.m_buttonLicenseTest = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.m_textBoxLicensePassword = new System.Windows.Forms.TextBox();
      this.m_textBoxLicenseServer = new System.Windows.Forms.TextBox();
      this.m_labelLicenseServer = new System.Windows.Forms.Label();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.SuspendLayout();
      // 
      // label2
      // 
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label2.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.label2.Location = new System.Drawing.Point(305, 22);
      this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(75, 21);
      this.label2.TabIndex = 41;
      this.label2.Text = "Password:";
      // 
      // m_textBoxEnterprisePassword
      // 
      this.m_textBoxEnterprisePassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxEnterprisePassword.Location = new System.Drawing.Point(381, 19);
      this.m_textBoxEnterprisePassword.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxEnterprisePassword.Name = "m_textBoxEnterprisePassword";
      this.m_textBoxEnterprisePassword.PasswordChar = '*';
      this.m_textBoxEnterprisePassword.Size = new System.Drawing.Size(146, 21);
      this.m_textBoxEnterprisePassword.TabIndex = 2;
      this.m_toolTip.SetToolTip(this.m_textBoxEnterprisePassword, "ENter the sa password for the V3D Enterprise System");
      // 
      // m_textBoxEnterpriseServer
      // 
      this.m_textBoxEnterpriseServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxEnterpriseServer.Location = new System.Drawing.Point(135, 19);
      this.m_textBoxEnterpriseServer.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxEnterpriseServer.Name = "m_textBoxEnterpriseServer";
      this.m_textBoxEnterpriseServer.Size = new System.Drawing.Size(154, 21);
      this.m_textBoxEnterpriseServer.TabIndex = 1;
      this.m_toolTip.SetToolTip(this.m_textBoxEnterpriseServer, "Enter the V3D Enterprise System Database Server Name (SERVER\\INSTANCE)");
      // 
      // m_labelEnterpriseServer
      // 
      this.m_labelEnterpriseServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelEnterpriseServer.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelEnterpriseServer.Location = new System.Drawing.Point(8, 19);
      this.m_labelEnterpriseServer.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.m_labelEnterpriseServer.Name = "m_labelEnterpriseServer";
      this.m_labelEnterpriseServer.Size = new System.Drawing.Size(136, 21);
      this.m_labelEnterpriseServer.TabIndex = 39;
      this.m_labelEnterpriseServer.Text = "Enterprise Server:";
      this.m_labelEnterpriseServer.Click += new System.EventHandler(this.m_labelEnterpriseServer_Click);
      // 
      // m_buttonEnterpriseTest
      // 
      this.m_buttonEnterpriseTest.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonEnterpriseTest.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonEnterpriseTest.Location = new System.Drawing.Point(543, 22);
      this.m_buttonEnterpriseTest.Name = "m_buttonEnterpriseTest";
      this.m_buttonEnterpriseTest.Size = new System.Drawing.Size(72, 23);
      this.m_buttonEnterpriseTest.TabIndex = 3;
      this.m_buttonEnterpriseTest.Text = "Test";
      this.m_toolTip.SetToolTip(this.m_buttonEnterpriseTest, "Click to test the V3D Enterprise Systems databse parameters");
      this.m_buttonEnterpriseTest.UseVisualStyleBackColor = false;
      this.m_buttonEnterpriseTest.Click += new System.EventHandler(this.OnTestEnterpriseClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonCancel.Location = new System.Drawing.Point(317, 94);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(72, 23);
      this.m_buttonCancel.TabIndex = 8;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.UseVisualStyleBackColor = false;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // m_buttonLink
      // 
      this.m_buttonLink.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonLink.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonLink.Location = new System.Drawing.Point(216, 94);
      this.m_buttonLink.Name = "m_buttonLink";
      this.m_buttonLink.Size = new System.Drawing.Size(72, 23);
      this.m_buttonLink.TabIndex = 7;
      this.m_buttonLink.Text = "Link";
      this.m_toolTip.SetToolTip(this.m_buttonLink, "Click the link the V3D License System with the V3D Enterprise System");
      this.m_buttonLink.UseVisualStyleBackColor = false;
      this.m_buttonLink.Click += new System.EventHandler(this.OnLinkClick);
      // 
      // m_buttonLicenseTest
      // 
      this.m_buttonLicenseTest.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonLicenseTest.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonLicenseTest.Location = new System.Drawing.Point(543, 56);
      this.m_buttonLicenseTest.Name = "m_buttonLicenseTest";
      this.m_buttonLicenseTest.Size = new System.Drawing.Size(72, 23);
      this.m_buttonLicenseTest.TabIndex = 6;
      this.m_buttonLicenseTest.Text = "Test";
      this.m_toolTip.SetToolTip(this.m_buttonLicenseTest, "Click to test the V3D License Systems database parameters");
      this.m_buttonLicenseTest.UseVisualStyleBackColor = false;
      this.m_buttonLicenseTest.Click += new System.EventHandler(this.OnTestLicenseClick);
      // 
      // label1
      // 
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.label1.Location = new System.Drawing.Point(305, 56);
      this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(75, 21);
      this.label1.TabIndex = 48;
      this.label1.Text = "Password:";
      // 
      // m_textBoxLicensePassword
      // 
      this.m_textBoxLicensePassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxLicensePassword.Location = new System.Drawing.Point(381, 56);
      this.m_textBoxLicensePassword.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxLicensePassword.Name = "m_textBoxLicensePassword";
      this.m_textBoxLicensePassword.PasswordChar = '*';
      this.m_textBoxLicensePassword.Size = new System.Drawing.Size(146, 21);
      this.m_textBoxLicensePassword.TabIndex = 5;
      this.m_toolTip.SetToolTip(this.m_textBoxLicensePassword, "Enter the sa password for the V3D Licesning System");
      this.m_textBoxLicensePassword.Enter += new System.EventHandler(this.OnEnterLicensePassword);
      // 
      // m_textBoxLicenseServer
      // 
      this.m_textBoxLicenseServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxLicenseServer.Location = new System.Drawing.Point(135, 53);
      this.m_textBoxLicenseServer.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
      this.m_textBoxLicenseServer.Name = "m_textBoxLicenseServer";
      this.m_textBoxLicenseServer.Size = new System.Drawing.Size(154, 21);
      this.m_textBoxLicenseServer.TabIndex = 4;
      this.m_toolTip.SetToolTip(this.m_textBoxLicenseServer, "ENter the V3D License Systems databse name (SERVER\\INSTANCE)");
      // 
      // m_labelLicenseServer
      // 
      this.m_labelLicenseServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelLicenseServer.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelLicenseServer.Location = new System.Drawing.Point(9, 56);
      this.m_labelLicenseServer.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.m_labelLicenseServer.Name = "m_labelLicenseServer";
      this.m_labelLicenseServer.Size = new System.Drawing.Size(107, 21);
      this.m_labelLicenseServer.TabIndex = 47;
      this.m_labelLicenseServer.Text = "License Server:";
      // 
      // LinkLicenseSystem
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.SlateGray;
      this.ClientSize = new System.Drawing.Size(623, 142);
      this.Controls.Add(this.m_buttonLicenseTest);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.m_textBoxLicensePassword);
      this.Controls.Add(this.m_textBoxLicenseServer);
      this.Controls.Add(this.m_labelLicenseServer);
      this.Controls.Add(this.m_buttonEnterpriseTest);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonLink);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.m_textBoxEnterprisePassword);
      this.Controls.Add(this.m_textBoxEnterpriseServer);
      this.Controls.Add(this.m_labelEnterpriseServer);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "LinkLicenseSystem";
      this.Text = "Link V3D License System";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox m_textBoxEnterprisePassword;
    private System.Windows.Forms.TextBox m_textBoxEnterpriseServer;
    private System.Windows.Forms.Label m_labelEnterpriseServer;
    private System.Windows.Forms.Button m_buttonEnterpriseTest;
    private System.Windows.Forms.Button m_buttonCancel;
    private System.Windows.Forms.Button m_buttonLink;
    private System.Windows.Forms.Button m_buttonLicenseTest;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox m_textBoxLicensePassword;
    private System.Windows.Forms.TextBox m_textBoxLicenseServer;
    private System.Windows.Forms.Label m_labelLicenseServer;
    private System.Windows.Forms.ToolTip m_toolTip;
  }
}

