// $Id: LoginForm.Designer.cs,v 1.1.2.5 2009/10/20 14:06:26 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Allows the user to login.
  /// </summary>
  partial class LoginForm
  {

    #region fields

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// The label to indicate that the user name should be entered into the adjacent text field.
    /// </summary>
    private System.Windows.Forms.Label m_usernameLabel;

    /// <summary>
    /// The label to indicate that the password should be entered into the adjacent text field.
    /// </summary>
    private System.Windows.Forms.Label m_passwordLabel;

    /// <summary>
    /// Accepts the username.
    /// </summary>
    private System.Windows.Forms.TextBox m_usernameText;

    /// <summary>
    /// Button to state input is over.
    /// </summary>
    private Viatronix.UI.Button m_okButton;

    /// <summary>
    /// Button to cancel the login.
    /// </summary>
    private Viatronix.UI.Button m_cancelButton;

    /// <summary>
    /// Accepts the password.
    /// </summary>
    private System.Windows.Forms.TextBox m_passwordText;

    #endregion

    #region methods

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if( disposing && ( components != null ) )
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
      this.m_usernameLabel = new System.Windows.Forms.Label();
      this.m_passwordLabel = new System.Windows.Forms.Label();
      this.m_usernameText = new System.Windows.Forms.TextBox();
      this.m_okButton = new Viatronix.UI.Button();
      this.m_cancelButton = new Viatronix.UI.Button();
      this.m_passwordText = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // m_usernameLabel
      // 
      this.m_usernameLabel.AutoSize = true;
      this.m_usernameLabel.Location = new System.Drawing.Point(12, 19);
      this.m_usernameLabel.Name = "m_usernameLabel";
      this.m_usernameLabel.Size = new System.Drawing.Size(59, 14);
      this.m_usernameLabel.TabIndex = 99;
      this.m_usernameLabel.Text = "Username:";
      // 
      // m_passwordLabel
      // 
      this.m_passwordLabel.AutoSize = true;
      this.m_passwordLabel.Location = new System.Drawing.Point(11, 51);
      this.m_passwordLabel.Name = "m_passwordLabel";
      this.m_passwordLabel.Size = new System.Drawing.Size(60, 14);
      this.m_passwordLabel.TabIndex = 99;
      this.m_passwordLabel.Text = "Password:";
      // 
      // m_usernameText
      // 
      this.m_usernameText.Location = new System.Drawing.Point(85, 16);
      this.m_usernameText.Name = "m_usernameText";
      this.m_usernameText.Size = new System.Drawing.Size(248, 20);
      this.m_usernameText.TabIndex = 0;
      // 
      // m_okButton
      // 
      this.m_okButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.m_okButton.ForeColor = System.Drawing.Color.Black;
      this.m_okButton.Location = new System.Drawing.Point(177, 88);
      this.m_okButton.Name = "m_okButton";
      this.m_okButton.OldStockButton = false;
      this.m_okButton.Size = new System.Drawing.Size(75, 23);
      this.m_okButton.TabIndex = 2;
      this.m_okButton.Text = "OK";
      this.m_okButton.UseVisualStyleBackColor = true;
      // 
      // m_cancelButton
      // 
      this.m_cancelButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_cancelButton.ForeColor = System.Drawing.Color.Black;
      this.m_cancelButton.Location = new System.Drawing.Point(258, 88);
      this.m_cancelButton.Name = "m_cancelButton";
      this.m_cancelButton.OldStockButton = false;
      this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
      this.m_cancelButton.TabIndex = 3;
      this.m_cancelButton.Text = "Cancel";
      this.m_cancelButton.UseVisualStyleBackColor = true;
      // 
      // m_passwordText
      // 
      this.m_passwordText.Location = new System.Drawing.Point(85, 48);
      this.m_passwordText.Name = "m_passwordText";
      this.m_passwordText.PasswordChar = '*';
      this.m_passwordText.Size = new System.Drawing.Size(248, 20);
      this.m_passwordText.TabIndex = 1;
      // 
      // LoginForm
      // 
      this.AcceptButton = this.m_okButton;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.m_cancelButton;
      this.ClientSize = new System.Drawing.Size(350, 129);
      this.Controls.Add(this.m_usernameLabel);
      this.Controls.Add(this.m_passwordText);
      this.Controls.Add(this.m_cancelButton);
      this.Controls.Add(this.m_okButton);
      this.Controls.Add(this.m_usernameText);
      this.Controls.Add(this.m_passwordLabel);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "LoginForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Login To Use Importer";
      this.Controls.SetChildIndex(this.m_passwordLabel, 0);
      this.Controls.SetChildIndex(this.m_usernameText, 0);
      this.Controls.SetChildIndex(this.m_okButton, 0);
      this.Controls.SetChildIndex(this.m_cancelButton, 0);
      this.Controls.SetChildIndex(this.m_passwordText, 0);
      this.Controls.SetChildIndex(this.m_usernameLabel, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    #endregion

  } // LoginForm
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: LoginForm.Designer.cs,v $
// Revision 1.1.2.5  2009/10/20 14:06:26  kchalupa
// Set cancel and default button on the login form.
//
// Revision 1.1.2.4  2009/06/04 18:51:22  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/LoginForm.Designer.cs,v 1.1.2.5 2009/10/20 14:06:26 kchalupa Exp $
// $Id: LoginForm.Designer.cs,v 1.1.2.5 2009/10/20 14:06:26 kchalupa Exp $

#endregion
