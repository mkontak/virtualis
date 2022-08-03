namespace DatabasePatch351
{
  partial class DatabasePatcher
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DatabasePatcher));
      this.m_buttonTest = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.m_labelScript = new System.Windows.Forms.Label();
      this.m_buttonBrowse = new System.Windows.Forms.Button();
      this.m_textBoxInstance = new System.Windows.Forms.TextBox();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_textBoxPassword = new System.Windows.Forms.TextBox();
      this.m_textBoxScript = new System.Windows.Forms.TextBox();
      this.m_textBoxAssemblies = new System.Windows.Forms.TextBox();
      this.m_buttonPatch = new System.Windows.Forms.Button();
      this.m_buttonCancel = new System.Windows.Forms.Button();
      this.m_progressBar = new System.Windows.Forms.ProgressBar();
      this.m_labelStatus = new System.Windows.Forms.Label();
      this.m_labelAssemblies = new System.Windows.Forms.Label();
      this.m_buttonAssembliesBrowse = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // m_buttonTest
      // 
      this.m_buttonTest.BackColor = System.Drawing.SystemColors.Control;
      this.m_buttonTest.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonTest.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonTest.Location = new System.Drawing.Point(254, 42);
      this.m_buttonTest.Name = "m_buttonTest";
      this.m_buttonTest.Size = new System.Drawing.Size(29, 23);
      this.m_buttonTest.TabIndex = 0;
      this.m_buttonTest.Text = "...";
      this.m_toolTip.SetToolTip(this.m_buttonTest, "Click to test database connection");
      this.m_buttonTest.UseVisualStyleBackColor = false;
      this.m_buttonTest.Click += new System.EventHandler(this.OnTestDatabase);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.ForeColor = System.Drawing.Color.AliceBlue;
      this.label1.Location = new System.Drawing.Point(48, 14);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(65, 15);
      this.label1.TabIndex = 1;
      this.label1.Text = "Instance:";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label2.ForeColor = System.Drawing.Color.AliceBlue;
      this.label2.Location = new System.Drawing.Point(22, 43);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(91, 15);
      this.label2.TabIndex = 2;
      this.label2.Text = "sa password:";
      // 
      // m_labelScript
      // 
      this.m_labelScript.AutoSize = true;
      this.m_labelScript.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelScript.ForeColor = System.Drawing.Color.AliceBlue;
      this.m_labelScript.Location = new System.Drawing.Point(2, 72);
      this.m_labelScript.Name = "m_labelScript";
      this.m_labelScript.Size = new System.Drawing.Size(111, 15);
      this.m_labelScript.TabIndex = 3;
      this.m_labelScript.Text = "Database script:";
      // 
      // m_buttonBrowse
      // 
      this.m_buttonBrowse.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonBrowse.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonBrowse.Location = new System.Drawing.Point(620, 68);
      this.m_buttonBrowse.Name = "m_buttonBrowse";
      this.m_buttonBrowse.Size = new System.Drawing.Size(29, 23);
      this.m_buttonBrowse.TabIndex = 4;
      this.m_buttonBrowse.Text = "...";
      this.m_buttonBrowse.UseVisualStyleBackColor = true;
      this.m_buttonBrowse.Click += new System.EventHandler(this.OnBrowseClick);
      // 
      // m_textBoxInstance
      // 
      this.m_textBoxInstance.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxInstance.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_textBoxInstance.Location = new System.Drawing.Point(122, 12);
      this.m_textBoxInstance.Name = "m_textBoxInstance";
      this.m_textBoxInstance.Size = new System.Drawing.Size(126, 20);
      this.m_textBoxInstance.TabIndex = 5;
      this.m_textBoxInstance.Text = " ";
      this.m_toolTip.SetToolTip(this.m_textBoxInstance, "Enter the database instance");
      this.m_textBoxInstance.TextChanged += new System.EventHandler(this.OnInstanceChanged);
      // 
      // m_textBoxPassword
      // 
      this.m_textBoxPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxPassword.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_textBoxPassword.Location = new System.Drawing.Point(122, 42);
      this.m_textBoxPassword.Name = "m_textBoxPassword";
      this.m_textBoxPassword.Size = new System.Drawing.Size(126, 20);
      this.m_textBoxPassword.TabIndex = 6;
      this.m_toolTip.SetToolTip(this.m_textBoxPassword, "Enter the sa password");
      this.m_textBoxPassword.UseSystemPasswordChar = true;
      // 
      // m_textBoxScript
      // 
      this.m_textBoxScript.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxScript.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_textBoxScript.Location = new System.Drawing.Point(122, 71);
      this.m_textBoxScript.Name = "m_textBoxScript";
      this.m_textBoxScript.Size = new System.Drawing.Size(492, 20);
      this.m_textBoxScript.TabIndex = 7;
      this.m_toolTip.SetToolTip(this.m_textBoxScript, "Enter the script to run");
      // 
      // m_textBoxAssemblies
      // 
      this.m_textBoxAssemblies.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxAssemblies.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_textBoxAssemblies.Location = new System.Drawing.Point(122, 102);
      this.m_textBoxAssemblies.Name = "m_textBoxAssemblies";
      this.m_textBoxAssemblies.Size = new System.Drawing.Size(492, 20);
      this.m_textBoxAssemblies.TabIndex = 13;
      this.m_toolTip.SetToolTip(this.m_textBoxAssemblies, "Enter the script to run");
      // 
      // m_buttonPatch
      // 
      this.m_buttonPatch.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonPatch.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonPatch.Location = new System.Drawing.Point(572, 6);
      this.m_buttonPatch.Name = "m_buttonPatch";
      this.m_buttonPatch.Size = new System.Drawing.Size(75, 23);
      this.m_buttonPatch.TabIndex = 8;
      this.m_buttonPatch.Text = "Patch";
      this.m_buttonPatch.UseVisualStyleBackColor = true;
      this.m_buttonPatch.Click += new System.EventHandler(this.OnPatch);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonCancel.Location = new System.Drawing.Point(572, 35);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(75, 23);
      this.m_buttonCancel.TabIndex = 9;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.UseVisualStyleBackColor = true;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancel);
      // 
      // m_progressBar
      // 
      this.m_progressBar.BackColor = System.Drawing.Color.SlateGray;
      this.m_progressBar.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_progressBar.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_progressBar.Location = new System.Drawing.Point(0, 133);
      this.m_progressBar.MarqueeAnimationSpeed = 50;
      this.m_progressBar.Name = "m_progressBar";
      this.m_progressBar.Size = new System.Drawing.Size(666, 18);
      this.m_progressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
      this.m_progressBar.TabIndex = 10;
      // 
      // m_labelStatus
      // 
      this.m_labelStatus.AutoSize = true;
      this.m_labelStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_labelStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelStatus.ForeColor = System.Drawing.SystemColors.Control;
      this.m_labelStatus.Location = new System.Drawing.Point(0, 118);
      this.m_labelStatus.Name = "m_labelStatus";
      this.m_labelStatus.Size = new System.Drawing.Size(0, 15);
      this.m_labelStatus.TabIndex = 11;
      this.m_labelStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      this.m_labelStatus.Visible = false;
      // 
      // m_labelAssemblies
      // 
      this.m_labelAssemblies.AutoSize = true;
      this.m_labelAssemblies.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelAssemblies.ForeColor = System.Drawing.Color.AliceBlue;
      this.m_labelAssemblies.Location = new System.Drawing.Point(29, 102);
      this.m_labelAssemblies.Name = "m_labelAssemblies";
      this.m_labelAssemblies.Size = new System.Drawing.Size(84, 15);
      this.m_labelAssemblies.TabIndex = 12;
      this.m_labelAssemblies.Text = "Assemblies:";
      // 
      // m_buttonAssembliesBrowse
      // 
      this.m_buttonAssembliesBrowse.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonAssembliesBrowse.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonAssembliesBrowse.Location = new System.Drawing.Point(620, 99);
      this.m_buttonAssembliesBrowse.Name = "m_buttonAssembliesBrowse";
      this.m_buttonAssembliesBrowse.Size = new System.Drawing.Size(29, 23);
      this.m_buttonAssembliesBrowse.TabIndex = 14;
      this.m_buttonAssembliesBrowse.Text = "...";
      this.m_buttonAssembliesBrowse.UseVisualStyleBackColor = true;
      this.m_buttonAssembliesBrowse.Click += new System.EventHandler(this.OnBrowseAssembliesClick);
      // 
      // DatabasePatcher
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.SlateGray;
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(666, 151);
      this.Controls.Add(this.m_buttonAssembliesBrowse);
      this.Controls.Add(this.m_textBoxAssemblies);
      this.Controls.Add(this.m_labelAssemblies);
      this.Controls.Add(this.m_labelStatus);
      this.Controls.Add(this.m_progressBar);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonPatch);
      this.Controls.Add(this.m_textBoxScript);
      this.Controls.Add(this.m_textBoxPassword);
      this.Controls.Add(this.m_textBoxInstance);
      this.Controls.Add(this.m_buttonBrowse);
      this.Controls.Add(this.m_labelScript);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.m_buttonTest);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "DatabasePatcher";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Database Patcher";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button m_buttonTest;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label m_labelScript;
    private System.Windows.Forms.Button m_buttonBrowse;
    private System.Windows.Forms.TextBox m_textBoxInstance;
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.TextBox m_textBoxPassword;
    private System.Windows.Forms.TextBox m_textBoxScript;
    private System.Windows.Forms.Button m_buttonPatch;
    private System.Windows.Forms.Button m_buttonCancel;
    private System.Windows.Forms.ProgressBar m_progressBar;
    private System.Windows.Forms.Label m_labelStatus;
    private System.Windows.Forms.Label m_labelAssemblies;
    private System.Windows.Forms.TextBox m_textBoxAssemblies;
    private System.Windows.Forms.Button m_buttonAssembliesBrowse;
  }
}

