namespace Viatronix.Console
{
  partial class DatabaseSearchPane
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
      this.m_searchControl = new Viatronix.Console.SearchControl();
      this.m_searchGroupBox = new Viatronix.UI.GroupBox();
      this.m_seriesGroupBox = new System.Windows.Forms.GroupBox();
      this.m_comboBoxProduct = new System.Windows.Forms.ComboBox();
      this.m_labelModality = new System.Windows.Forms.Label();
      this.m_comboBoxModality = new System.Windows.Forms.ComboBox();
      this.m_labelSubtype = new System.Windows.Forms.Label();
      this.m_studyGroupBox = new System.Windows.Forms.GroupBox();
      this.m_comboBoxStudyGroups = new System.Windows.Forms.ComboBox();
      this.label2 = new System.Windows.Forms.Label();
      this.m_physicianLabel = new System.Windows.Forms.Label();
      this.m_physicianTextBox = new System.Windows.Forms.TextBox();
      this.m_scanDate = new Viatronix.Console.ScanDateControl();
      this.m_patientGroupBox = new System.Windows.Forms.GroupBox();
      this.m_patientNameTextBox = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.m_medicalIdTextBox = new System.Windows.Forms.TextBox();
      this.m_patientNameLabel = new System.Windows.Forms.Label();
      this.m_searchGroupBox.SuspendLayout();
      this.m_seriesGroupBox.SuspendLayout();
      this.m_studyGroupBox.SuspendLayout();
      this.m_patientGroupBox.SuspendLayout();
      this.m_searchControl.SuspendLayout();
      this.SuspendLayout();

      // 
      // m_searchGroupBox
      // 
      this.m_searchGroupBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.m_searchGroupBox.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
      this.m_searchGroupBox.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_searchGroupBox.Controls.Add(this.m_seriesGroupBox);
      this.m_searchGroupBox.Controls.Add(this.m_studyGroupBox);
      this.m_searchGroupBox.Controls.Add(this.m_patientGroupBox);
      this.m_searchGroupBox.Controls.Add(this.m_searchControl);
      this.m_searchGroupBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_searchGroupBox.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_searchGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_searchGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_searchGroupBox.Location = new System.Drawing.Point(0, 0);
      this.m_searchGroupBox.Margin = new System.Windows.Forms.Padding(3);
      this.m_searchGroupBox.Name = "m_searchGroupBox";
      this.m_searchGroupBox.Padding = new System.Windows.Forms.Padding(3);
      this.m_searchGroupBox.Size = new System.Drawing.Size(296, 471);
      this.m_searchGroupBox.TabColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_searchGroupBox.TabIndex = 0;
      this.m_searchGroupBox.TabStop = false;
      //
      // Search Constrol
      //
      this.m_searchControl.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
      this.m_searchControl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_searchControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_searchControl.Location = new System.Drawing.Point(16, 433);
      this.m_searchControl.Name = "m_searchControl";
      this.m_searchControl.Searcher = null;
      this.m_searchControl.SearchPane = null;
      this.m_searchControl.Size = new System.Drawing.Size(264, 24);
      this.m_searchControl.TabIndex = 3;
      // 
      // m_seriesGroupBox
      // 
      this.m_seriesGroupBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_seriesGroupBox.Controls.Add(this.m_comboBoxProduct);
      this.m_seriesGroupBox.Controls.Add(this.m_labelModality);
      this.m_seriesGroupBox.Controls.Add(this.m_comboBoxModality);
      this.m_seriesGroupBox.Controls.Add(this.m_labelSubtype);
      this.m_seriesGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_seriesGroupBox.Location = new System.Drawing.Point(16, 299);
      this.m_seriesGroupBox.Name = "m_seriesGroupBox";
      this.m_seriesGroupBox.Size = new System.Drawing.Size(264, 88);
      this.m_seriesGroupBox.TabIndex = 2;
      this.m_seriesGroupBox.TabStop = false;
      this.m_seriesGroupBox.Text = "Series Information";
      // 
      // m_comboBoxProduct
      // 
      this.m_comboBoxProduct.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxProduct.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_comboBoxProduct.ForeColor = System.Drawing.Color.Black;
      this.m_comboBoxProduct.ItemHeight = 13;
      this.m_comboBoxProduct.Location = new System.Drawing.Point(72, 24);
      this.m_comboBoxProduct.Name = "m_comboBoxProduct";
      this.m_comboBoxProduct.Size = new System.Drawing.Size(184, 21);
      this.m_comboBoxProduct.TabIndex = 8;
      // 
      // m_labelModality
      // 
      this.m_labelModality.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_labelModality.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelModality.ForeColor = System.Drawing.Color.White;
      this.m_labelModality.Location = new System.Drawing.Point(8, 55);
      this.m_labelModality.Name = "m_labelModality";
      this.m_labelModality.Size = new System.Drawing.Size(56, 23);
      this.m_labelModality.TabIndex = 62;
      this.m_labelModality.Text = "Modality:";
      this.m_labelModality.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxModality
      // 
      this.m_comboBoxModality.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxModality.DropDownWidth = 250;
      this.m_comboBoxModality.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_comboBoxModality.ForeColor = System.Drawing.Color.Black;
      this.m_comboBoxModality.Location = new System.Drawing.Point(72, 55);
      this.m_comboBoxModality.Name = "m_comboBoxModality";
      this.m_comboBoxModality.Size = new System.Drawing.Size(184, 21);
      this.m_comboBoxModality.TabIndex = 9;
      // 
      // m_labelSubtype
      // 
      this.m_labelSubtype.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_labelSubtype.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelSubtype.ForeColor = System.Drawing.Color.White;
      this.m_labelSubtype.Location = new System.Drawing.Point(8, 23);
      this.m_labelSubtype.Name = "m_labelSubtype";
      this.m_labelSubtype.Size = new System.Drawing.Size(64, 23);
      this.m_labelSubtype.TabIndex = 64;
      this.m_labelSubtype.Text = "Application:";
      this.m_labelSubtype.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_studyGroupBox
      // 
      this.m_studyGroupBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_studyGroupBox.Controls.Add(this.m_comboBoxStudyGroups);
      this.m_studyGroupBox.Controls.Add(this.label2);
      this.m_studyGroupBox.Controls.Add(this.m_physicianLabel);
      this.m_studyGroupBox.Controls.Add(this.m_physicianTextBox);
      this.m_studyGroupBox.Controls.Add(this.m_scanDate);
      this.m_studyGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_studyGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_studyGroupBox.Location = new System.Drawing.Point(16, 117);
      this.m_studyGroupBox.Name = "m_studyGroupBox";
      this.m_studyGroupBox.Size = new System.Drawing.Size(264, 178);
      this.m_studyGroupBox.TabIndex = 1;
      this.m_studyGroupBox.TabStop = false;
      this.m_studyGroupBox.Text = "Study Information";
      // 
      // m_comboBoxStudyGroups
      // 
      this.m_comboBoxStudyGroups.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxStudyGroups.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_comboBoxStudyGroups.ForeColor = System.Drawing.Color.Black;
      this.m_comboBoxStudyGroups.ItemHeight = 13;
      this.m_comboBoxStudyGroups.Location = new System.Drawing.Point(73, 21);
      this.m_comboBoxStudyGroups.Name = "m_comboBoxStudyGroups";
      this.m_comboBoxStudyGroups.Size = new System.Drawing.Size(184, 21);
      this.m_comboBoxStudyGroups.TabIndex = 67;
      // 
      // label2
      // 
      this.label2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label2.ForeColor = System.Drawing.Color.White;
      this.label2.Location = new System.Drawing.Point(9, 22);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(56, 23);
      this.label2.TabIndex = 66;
      this.label2.Text = "Group:";
      this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_physicianLabel
      // 
      this.m_physicianLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_physicianLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_physicianLabel.ForeColor = System.Drawing.Color.White;
      this.m_physicianLabel.Location = new System.Drawing.Point(8, 49);
      this.m_physicianLabel.Name = "m_physicianLabel";
      this.m_physicianLabel.Size = new System.Drawing.Size(56, 23);
      this.m_physicianLabel.TabIndex = 64;
      this.m_physicianLabel.Text = "Physician:";
      this.m_physicianLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_physicianTextBox
      // 
      this.m_physicianTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_physicianTextBox.Location = new System.Drawing.Point(73, 52);
      this.m_physicianTextBox.Name = "m_physicianTextBox";
      this.m_physicianTextBox.Size = new System.Drawing.Size(184, 20);
      this.m_physicianTextBox.TabIndex = 6;
      this.m_physicianTextBox.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnKeyUpHandler);
      // 
      // m_scanDate
      // 
      this.m_scanDate.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_scanDate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_scanDate.Location = new System.Drawing.Point(8, 84);
      this.m_scanDate.Name = "m_scanDate";
      this.m_scanDate.Size = new System.Drawing.Size(248, 88);
      this.m_scanDate.TabIndex = 7;
      // 
      // m_patientGroupBox
      // 
      this.m_patientGroupBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_patientGroupBox.Controls.Add(this.m_patientNameTextBox);
      this.m_patientGroupBox.Controls.Add(this.label1);
      this.m_patientGroupBox.Controls.Add(this.m_medicalIdTextBox);
      this.m_patientGroupBox.Controls.Add(this.m_patientNameLabel);
      this.m_patientGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_patientGroupBox.Location = new System.Drawing.Point(16, 16);
      this.m_patientGroupBox.Name = "m_patientGroupBox";
      this.m_patientGroupBox.Size = new System.Drawing.Size(264, 97);
      this.m_patientGroupBox.TabIndex = 0;
      this.m_patientGroupBox.TabStop = false;
      this.m_patientGroupBox.Text = "Patient Information";
      // 
      // m_patientNameTextBox
      // 
      this.m_patientNameTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_patientNameTextBox.Location = new System.Drawing.Point(59, 23);
      this.m_patientNameTextBox.Name = "m_patientNameTextBox";
      this.m_patientNameTextBox.Size = new System.Drawing.Size(199, 20);
      this.m_patientNameTextBox.TabIndex = 4;
      this.m_patientNameTextBox.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnKeyUpHandler);
      // 
      // label1
      // 
      this.label1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.ForeColor = System.Drawing.Color.White;
      this.label1.Location = new System.Drawing.Point(6, 55);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(64, 27);
      this.label1.TabIndex = 63;
      this.label1.Text = "Medical ID:";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_medicalIdTextBox
      // 
      this.m_medicalIdTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_medicalIdTextBox.Location = new System.Drawing.Point(74, 59);
      this.m_medicalIdTextBox.Name = "m_medicalIdTextBox";
      this.m_medicalIdTextBox.Size = new System.Drawing.Size(184, 20);
      this.m_medicalIdTextBox.TabIndex = 5;
      this.m_medicalIdTextBox.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnKeyUpHandler);
      // 
      // m_patientNameLabel
      // 
      this.m_patientNameLabel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_patientNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_patientNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_patientNameLabel.Location = new System.Drawing.Point(8, 16);
      this.m_patientNameLabel.Name = "m_patientNameTextBoxLabel";
      this.m_patientNameLabel.Size = new System.Drawing.Size(44, 27);
      this.m_patientNameLabel.TabIndex = 61;
      this.m_patientNameLabel.Text = "Name:";
      this.m_patientNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // DatabaseSearchPane
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.Controls.Add(this.m_searchGroupBox);
      this.Name = "DatabaseSearchPane";
      this.Size = new System.Drawing.Size(296, 471);
      this.m_searchGroupBox.ResumeLayout(false);
      this.m_seriesGroupBox.ResumeLayout(false);
      this.m_seriesGroupBox.PerformLayout();
      this.m_studyGroupBox.ResumeLayout(false);
      this.m_studyGroupBox.PerformLayout();
      this.m_patientGroupBox.ResumeLayout(false);
      this.m_patientGroupBox.PerformLayout();
      this.m_searchControl.ResumeLayout(false);
      this.m_searchControl.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion
  }
}
