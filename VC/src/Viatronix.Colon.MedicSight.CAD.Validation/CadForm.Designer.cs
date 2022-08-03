// $Id: CadForm.Designer.cs,v 1.1.4.2 2010-05-17 19:50:48 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// The form for user input, specifiying the file to read and so forth.
  /// </summary>
  partial class CadForm
  {

    #region fields

    /// <summary>
    /// The file name text box.
    /// </summary>
    private System.Windows.Forms.TextBox m_cadReportFileTextBox;

    /// <summary>
    /// When the user clicks this button a dialog will popup giving them a
    /// method of selecting the file they want to convert.
    /// </summary>
    private System.Windows.Forms.Button m_cadReportFileNavigatorButton;

    /// <summary>
    /// The label to indicate the purpose of the textbox.
    /// </summary>
    private System.Windows.Forms.Label m_cadReportFileLabel;

    /// <summary>
    /// When the user clicks this button the procedure of conversion begins.
    /// </summary>
    private System.Windows.Forms.Button m_convertButton;

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Text box for specifying DICOM Path.
    /// </summary>
    private System.Windows.Forms.TextBox m_dicomPathTextBox;

    /// <summary>
    /// Button for navigating to the DICOM path.
    /// </summary>
    private System.Windows.Forms.Button m_dicomNavigatorButton;

    /// <summary>
    /// Label to indicate DICOM path.
    /// </summary>
    private System.Windows.Forms.Label m_dicomLabel;

    /// <summary>
    /// Navigator button for CAD truth file.
    /// </summary>
    private System.Windows.Forms.Button m_cadTruthFileNavigatorButton;

    /// <summary>
    /// Label to indicate the type of input required.
    /// </summary>
    private System.Windows.Forms.Label m_cadTruthLabel;
    
    /// <summary>
    /// The textbox to enter the CAD truth file.
    /// </summary>
    private System.Windows.Forms.TextBox m_cadTruthTextBox; // InitializeComponent()

    /// <summary>
    /// Indicates the Text Box refers to the output directory.
    /// </summary>
    private System.Windows.Forms.Label m_outputLabel;

    /// <summary>
    /// TextBox that holds the output directory.
    /// </summary>
    private System.Windows.Forms.TextBox m_outputTextBox;

    /// <summary>
    /// Navigator button for the output directory.
    /// </summary>
    private System.Windows.Forms.Button m_outputNavigatorButton;

    /// <summary>
    /// Indicates that the Utility is being validated and that the output path should be available.
    /// </summary>
    private System.Windows.Forms.CheckBox m_validateUtilityCheckBox;

    /// <summary>
    /// Helps the user by providing messages as to the intended input in various controls.
    /// </summary>
    private System.Windows.Forms.ToolTip toolTip1;

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.m_cadReportFileLabel = new System.Windows.Forms.Label();
      this.m_cadReportFileTextBox = new System.Windows.Forms.TextBox();
      this.m_cadReportFileNavigatorButton = new System.Windows.Forms.Button();
      this.m_convertButton = new System.Windows.Forms.Button();
      this.m_dicomLabel = new System.Windows.Forms.Label();
      this.m_dicomPathTextBox = new System.Windows.Forms.TextBox();
      this.m_dicomNavigatorButton = new System.Windows.Forms.Button();
      this.m_cadTruthFileNavigatorButton = new System.Windows.Forms.Button();
      this.m_cadTruthLabel = new System.Windows.Forms.Label();
      this.m_cadTruthTextBox = new System.Windows.Forms.TextBox();
      this.m_outputNavigatorButton = new System.Windows.Forms.Button();
      this.m_outputLabel = new System.Windows.Forms.Label();
      this.m_outputTextBox = new System.Windows.Forms.TextBox();
      this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
      this.m_validateUtilityCheckBox = new System.Windows.Forms.CheckBox();
      this.SuspendLayout();
      // 
      // m_cadReportFileLabel
      // 
      this.m_cadReportFileLabel.AutoSize = true;
      this.m_cadReportFileLabel.Location = new System.Drawing.Point(11, 12);
      this.m_cadReportFileLabel.Name = "m_cadReportFileLabel";
      this.m_cadReportFileLabel.Size = new System.Drawing.Size(72, 13);
      this.m_cadReportFileLabel.TabIndex = 1;
      this.m_cadReportFileLabel.Text = "Cad V3D File:";
      // 
      // m_cadReportFileTextBox
      // 
      this.m_cadReportFileTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cadReportFileTextBox.Location = new System.Drawing.Point(86, 9);
      this.m_cadReportFileTextBox.Name = "m_cadReportFileTextBox";
      this.m_cadReportFileTextBox.Size = new System.Drawing.Size(247, 20);
      this.m_cadReportFileTextBox.TabIndex = 0;
      this.toolTip1.SetToolTip(this.m_cadReportFileTextBox, "Pionts to the Cad report file generated by the VC plugin.");
      // 
      // m_cadReportFileNavigatorButton
      // 
      this.m_cadReportFileNavigatorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cadReportFileNavigatorButton.ForeColor = System.Drawing.Color.Black;
      this.m_cadReportFileNavigatorButton.Location = new System.Drawing.Point(339, 9);
      this.m_cadReportFileNavigatorButton.Name = "m_cadReportFileNavigatorButton";
      this.m_cadReportFileNavigatorButton.Size = new System.Drawing.Size(26, 21);
      this.m_cadReportFileNavigatorButton.TabIndex = 1;
      this.m_cadReportFileNavigatorButton.Text = "...";
      this.m_cadReportFileNavigatorButton.UseVisualStyleBackColor = true;
      this.m_cadReportFileNavigatorButton.Click += new System.EventHandler(this.OnNavigatorClickedHandler);
      // 
      // m_convertButton
      // 
      this.m_convertButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_convertButton.ForeColor = System.Drawing.Color.Black;
      this.m_convertButton.Location = new System.Drawing.Point(290, 136);
      this.m_convertButton.Name = "m_convertButton";
      this.m_convertButton.Size = new System.Drawing.Size(75, 21);
      this.m_convertButton.TabIndex = 9;
      this.m_convertButton.Text = "Verify";
      this.m_convertButton.UseVisualStyleBackColor = true;
      this.m_convertButton.Click += new System.EventHandler(this.OnVerifyClickedHandler);
      // 
      // m_dicomLabel
      // 
      this.m_dicomLabel.AutoSize = true;
      this.m_dicomLabel.Location = new System.Drawing.Point(11, 74);
      this.m_dicomLabel.Name = "m_dicomLabel";
      this.m_dicomLabel.Size = new System.Drawing.Size(70, 13);
      this.m_dicomLabel.TabIndex = 5;
      this.m_dicomLabel.Text = "DICOM Path:";
      // 
      // m_dicomPathTextBox
      // 
      this.m_dicomPathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_dicomPathTextBox.Location = new System.Drawing.Point(86, 71);
      this.m_dicomPathTextBox.Name = "m_dicomPathTextBox";
      this.m_dicomPathTextBox.Size = new System.Drawing.Size(246, 20);
      this.m_dicomPathTextBox.TabIndex = 4;
      this.toolTip1.SetToolTip(this.m_dicomPathTextBox, "Refers to the DICOM path that provides information that is helpful in converting " +
        "to DICOM coordinate system.");
      // 
      // m_dicomNavigatorButton
      // 
      this.m_dicomNavigatorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_dicomNavigatorButton.ForeColor = System.Drawing.Color.Black;
      this.m_dicomNavigatorButton.Location = new System.Drawing.Point(338, 71);
      this.m_dicomNavigatorButton.Name = "m_dicomNavigatorButton";
      this.m_dicomNavigatorButton.Size = new System.Drawing.Size(26, 21);
      this.m_dicomNavigatorButton.TabIndex = 5;
      this.m_dicomNavigatorButton.Text = "...";
      this.m_dicomNavigatorButton.UseVisualStyleBackColor = true;
      this.m_dicomNavigatorButton.Click += new System.EventHandler(this.OnDicomNavigatorClickedHandler);
      // 
      // m_cadTruthFileNavigatorButton
      // 
      this.m_cadTruthFileNavigatorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cadTruthFileNavigatorButton.ForeColor = System.Drawing.Color.Black;
      this.m_cadTruthFileNavigatorButton.Location = new System.Drawing.Point(338, 39);
      this.m_cadTruthFileNavigatorButton.Name = "m_cadTruthFileNavigatorButton";
      this.m_cadTruthFileNavigatorButton.Size = new System.Drawing.Size(26, 21);
      this.m_cadTruthFileNavigatorButton.TabIndex = 3;
      this.m_cadTruthFileNavigatorButton.Text = "...";
      this.m_cadTruthFileNavigatorButton.UseVisualStyleBackColor = true;
      this.m_cadTruthFileNavigatorButton.Click += new System.EventHandler(this.OnCadTruthNavigatorClickedHandler);
      // 
      // m_cadTruthLabel
      // 
      this.m_cadTruthLabel.AutoSize = true;
      this.m_cadTruthLabel.Location = new System.Drawing.Point(7, 43);
      this.m_cadTruthLabel.Name = "m_cadTruthLabel";
      this.m_cadTruthLabel.Size = new System.Drawing.Size(76, 13);
      this.m_cadTruthLabel.TabIndex = 9;
      this.m_cadTruthLabel.Text = "Cad Truth File:";
      // 
      // m_cadTruthTextBox
      // 
      this.m_cadTruthTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cadTruthTextBox.Location = new System.Drawing.Point(86, 40);
      this.m_cadTruthTextBox.Name = "m_cadTruthTextBox";
      this.m_cadTruthTextBox.Size = new System.Drawing.Size(246, 20);
      this.m_cadTruthTextBox.TabIndex = 2;
      this.toolTip1.SetToolTip(this.m_cadTruthTextBox, "Refers to the accepted truth file provided by the Cad vendor.");
      // 
      // m_outputNavigatorButton
      // 
      this.m_outputNavigatorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_outputNavigatorButton.Enabled = false;
      this.m_outputNavigatorButton.ForeColor = System.Drawing.Color.Black;
      this.m_outputNavigatorButton.Location = new System.Drawing.Point(339, 101);
      this.m_outputNavigatorButton.Name = "m_outputNavigatorButton";
      this.m_outputNavigatorButton.Size = new System.Drawing.Size(26, 21);
      this.m_outputNavigatorButton.TabIndex = 7;
      this.m_outputNavigatorButton.Text = "...";
      this.m_outputNavigatorButton.UseVisualStyleBackColor = true;
      this.m_outputNavigatorButton.Click += new System.EventHandler(this.OnOutputNavigatorClickedHandler);
      // 
      // m_outputLabel
      // 
      this.m_outputLabel.AutoSize = true;
      this.m_outputLabel.Enabled = false;
      this.m_outputLabel.Location = new System.Drawing.Point(12, 105);
      this.m_outputLabel.Name = "m_outputLabel";
      this.m_outputLabel.Size = new System.Drawing.Size(67, 13);
      this.m_outputLabel.TabIndex = 12;
      this.m_outputLabel.Text = "Output Path:";
      // 
      // m_outputTextBox
      // 
      this.m_outputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_outputTextBox.Enabled = false;
      this.m_outputTextBox.Location = new System.Drawing.Point(86, 102);
      this.m_outputTextBox.Name = "m_outputTextBox";
      this.m_outputTextBox.Size = new System.Drawing.Size(247, 20);
      this.m_outputTextBox.TabIndex = 6;
      this.toolTip1.SetToolTip(this.m_outputTextBox, "Refers to where the resulting files should be placed.");
      // 
      // m_validateUtilityCheckBox
      // 
      this.m_validateUtilityCheckBox.AutoSize = true;
      this.m_validateUtilityCheckBox.Location = new System.Drawing.Point(14, 139);
      this.m_validateUtilityCheckBox.Name = "m_validateUtilityCheckBox";
      this.m_validateUtilityCheckBox.Size = new System.Drawing.Size(130, 17);
      this.m_validateUtilityCheckBox.TabIndex = 8;
      this.m_validateUtilityCheckBox.Text = "Utility Validation Mode";
      this.toolTip1.SetToolTip(this.m_validateUtilityCheckBox, "Indicates that the Utility is being validated.  Indicates that the utility should" +
        " output files necessary for validation.");
      this.m_validateUtilityCheckBox.UseVisualStyleBackColor = true;
      this.m_validateUtilityCheckBox.CheckedChanged += new System.EventHandler(this.OnValidationCheckChangedHandler);
      // 
      // CadForm
      // 
      this.AcceptButton = this.m_convertButton;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(391, 175);
      this.Controls.Add(this.m_validateUtilityCheckBox);
      this.Controls.Add(this.m_outputTextBox);
      this.Controls.Add(this.m_outputLabel);
      this.Controls.Add(this.m_outputNavigatorButton);
      this.Controls.Add(this.m_cadTruthTextBox);
      this.Controls.Add(this.m_cadTruthLabel);
      this.Controls.Add(this.m_cadTruthFileNavigatorButton);
      this.Controls.Add(this.m_dicomNavigatorButton);
      this.Controls.Add(this.m_dicomPathTextBox);
      this.Controls.Add(this.m_dicomLabel);
      this.Controls.Add(this.m_cadReportFileLabel);
      this.Controls.Add(this.m_convertButton);
      this.Controls.Add(this.m_cadReportFileTextBox);
      this.Controls.Add(this.m_cadReportFileNavigatorButton);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Name = "CadForm";
      this.Text = "Cad Verifier 3.5.0.0";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

  } // CadForm
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1.4.1  2009/06/29 13:22:25  kchalupa
// Checkin for CadParser.
//
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/CadForm.Designer.cs,v 1.1.4.2 2010-05-17 19:50:48 kchalupa Exp $
// $Id: CadForm.Designer.cs,v 1.1.4.2 2010-05-17 19:50:48 kchalupa Exp $

#endregion
