// $Id: ErrorForm.Designer.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
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
  /// Shows the errors that occured in validation.
  /// </summary>
  partial class ErrorForm
  {

    #region fields

    /// <summary>
    /// Text box for listing errors.
    /// </summary>
    private System.Windows.Forms.TextBox m_errorTextBox;

    /// <summary>
    /// The label to indicate these are validation errors.
    /// </summary>
    private System.Windows.Forms.Label m_validationErrorsLabel;

    /// <summary>
    /// The button to dismiss the error form.
    /// </summary>
    private System.Windows.Forms.Button m_closeButton;

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_errorTextBox = new System.Windows.Forms.TextBox();
      this.m_validationErrorsLabel = new System.Windows.Forms.Label();
      this.m_closeButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // m_errorTextBox
      // 
      this.m_errorTextBox.Location = new System.Drawing.Point(12, 35);
      this.m_errorTextBox.Multiline = true;
      this.m_errorTextBox.Name = "m_errorTextBox";
      this.m_errorTextBox.ReadOnly = true;
      this.m_errorTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.m_errorTextBox.Size = new System.Drawing.Size(327, 199);
      this.m_errorTextBox.TabIndex = 1;
      // 
      // m_validationErrorsLabel
      // 
      this.m_validationErrorsLabel.AutoSize = true;
      this.m_validationErrorsLabel.Location = new System.Drawing.Point(12, 9);
      this.m_validationErrorsLabel.Name = "m_validationErrorsLabel";
      this.m_validationErrorsLabel.Size = new System.Drawing.Size(87, 14);
      this.m_validationErrorsLabel.TabIndex = 2;
      this.m_validationErrorsLabel.Text = "Validation Errors";
      // 
      // m_closeButton
      // 
      this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_closeButton.ForeColor = System.Drawing.Color.Black;
      this.m_closeButton.Location = new System.Drawing.Point(264, 243);
      this.m_closeButton.Name = "m_closeButton";
      this.m_closeButton.Size = new System.Drawing.Size(75, 23);
      this.m_closeButton.TabIndex = 3;
      this.m_closeButton.Text = "Close";
      this.m_closeButton.UseVisualStyleBackColor = true;
      // 
      // ErrorForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.m_closeButton;
      this.ClientSize = new System.Drawing.Size(351, 275);
      this.Controls.Add(this.m_errorTextBox);
      this.Controls.Add(this.m_closeButton);
      this.Controls.Add(this.m_validationErrorsLabel);
      this.Name = "ErrorForm";
      this.Text = "The Following Errors Occured In Validation";
      this.Controls.SetChildIndex(this.m_validationErrorsLabel, 0);
      this.Controls.SetChildIndex(this.m_closeButton, 0);
      this.Controls.SetChildIndex(this.m_errorTextBox, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

  } // ErrorForm
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:33  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/ErrorForm.Designer.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: ErrorForm.Designer.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
