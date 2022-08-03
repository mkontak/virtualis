// $Id: FormChangePassword.cs,v 1.1.2.1 2012/11/13 21:00:26 mkontak Exp $
//
// Copyright © 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Xml;
using System.Drawing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Models;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.UI
{
	/// <summary>
	/// User Interface for changing the password.
	/// </summary>
	public class FormChangePassword : Viatronix.UI.Dialog 
	{

    #region fields

    /// <summary>
    /// Security provider
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// The password change model.
    /// </summary>
    private PasswordChangeModel m_model = null;

    /// <summary>
    /// Label object for new password
    /// </summary>
    private System.Windows.Forms.Label m_labelNewPassword;

    /// <summary>
    /// Label object for confirm password field
    /// </summary>
    private System.Windows.Forms.Label m_labelConfirmNewPassword;

    /// <summary>
    /// TextBox object for New password
    /// </summary>
    private System.Windows.Forms.TextBox m_newPassword;

    /// <summary>
    /// TextBox object for ConfirmPassword.
    /// </summary>
    private System.Windows.Forms.TextBox m_confirmPassword;

    /// <summary>
    /// Ok button object
    /// </summary>
    private Viatronix.UI.Button m_buttonOk;

    /// <summary>
    /// cancel button object
    /// </summary>
    private Viatronix.UI.Button m_buttonCancel;


    /// <summary>
    /// Passsword hints
    /// </summary>
    private string m_passwordHints = string.Empty;

    /// <summary>
    /// User to change password
    /// </summary>
    private User m_user;


    /// <summary>
    /// Tool tips control
    /// </summary>
    private ToolTip m_toolTip;


    /// <summary>
    /// Controls container
    /// </summary>
    private IContainer components;

    #endregion fields

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="user"></param>
    public FormChangePassword(ISecurityProvider securityProvider, User currentUser, User user)
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      m_user = user;

      m_securityProvider = securityProvider;
      m_model = new PasswordChangeModel(m_securityProvider);
      m_passwordHints = string.Empty;

      ///////////////////////////////////////////////////////////////////////////////////////////
      // If the logged in user has the permission to modify users no need of the old password.
      //////////////////////////////////////////////////////////////////////////////////////////
      if (!currentUser.Equals(user) && !currentUser.IsAllowed("UserManagement"))
      {
        UI.MessageBox.ShowError("You do not have the rights to change a password");
        this.Close();
      }

      m_newPassword.Focus();
    } // FormChangePassword(User user, string currentPassword)

    #endregion

    #region properties
    #endregion properties

    #region methods



		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		} //Dispose( bool disposing )


		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      this.m_newPassword = new System.Windows.Forms.TextBox();
      this.m_confirmPassword = new System.Windows.Forms.TextBox();
      this.m_labelNewPassword = new System.Windows.Forms.Label();
      this.m_labelConfirmNewPassword = new System.Windows.Forms.Label();
      this.m_buttonOk = new Viatronix.UI.Button();
      this.m_buttonCancel = new Viatronix.UI.Button();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.SuspendLayout();
      // 
      // m_newPassword
      // 
      this.m_newPassword.Location = new System.Drawing.Point(128, 18);
      this.m_newPassword.Name = "m_newPassword";
      this.m_newPassword.PasswordChar = '*';
      this.m_newPassword.Size = new System.Drawing.Size(176, 20);
      this.m_newPassword.TabIndex = 2;
      this.m_newPassword.MouseLeave += new System.EventHandler(this.OnMouseLeaveNewPassword);
      this.m_newPassword.MouseEnter += new System.EventHandler(this.OnMouseEnterNewPassword);
      // 
      // m_confirmPassword
      // 
      this.m_confirmPassword.Location = new System.Drawing.Point(128, 49);
      this.m_confirmPassword.Name = "m_confirmPassword";
      this.m_confirmPassword.PasswordChar = '*';
      this.m_confirmPassword.Size = new System.Drawing.Size(176, 20);
      this.m_confirmPassword.TabIndex = 3;
      this.m_toolTip.SetToolTip(this.m_confirmPassword, "Reenter the new passsword");
      this.m_confirmPassword.Enter += new System.EventHandler(this.OnConfirmPasswordEnter);
      // 
      // m_labelNewPassword
      // 
      this.m_labelNewPassword.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelNewPassword.ForeColor = System.Drawing.Color.White;
      this.m_labelNewPassword.Location = new System.Drawing.Point(12, 23);
      this.m_labelNewPassword.Name = "m_labelNewPassword";
      this.m_labelNewPassword.Size = new System.Drawing.Size(86, 20);
      this.m_labelNewPassword.TabIndex = 5;
      this.m_labelNewPassword.Text = "New Password:";
      // 
      // m_labelConfirmNewPassword
      // 
      this.m_labelConfirmNewPassword.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelConfirmNewPassword.ForeColor = System.Drawing.Color.White;
      this.m_labelConfirmNewPassword.Location = new System.Drawing.Point(12, 50);
      this.m_labelConfirmNewPassword.Name = "m_labelConfirmNewPassword";
      this.m_labelConfirmNewPassword.Size = new System.Drawing.Size(116, 20);
      this.m_labelConfirmNewPassword.TabIndex = 6;
      this.m_labelConfirmNewPassword.Text = "Confirm Password:";
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonOk.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonOk.ForeColor = System.Drawing.Color.Black;
      this.m_buttonOk.Location = new System.Drawing.Point(175, 86);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.OldStockButton = false;
      this.m_buttonOk.Size = new System.Drawing.Size(63, 26);
      this.m_buttonOk.TabIndex = 4;
      this.m_buttonOk.Text = "OK";
      this.m_buttonOk.Click += new System.EventHandler(this.OnOk);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Black;
      this.m_buttonCancel.Location = new System.Drawing.Point(244, 86);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.Size = new System.Drawing.Size(63, 26);
      this.m_buttonCancel.TabIndex = 5;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancel);
      // 
      // m_toolTip
      // 
      this.m_toolTip.IsBalloon = true;
      // 
      // FormChangePassword
      // 
      this.AcceptButton = this.m_buttonOk;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(319, 132);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonOk);
      this.Controls.Add(this.m_labelConfirmNewPassword);
      this.Controls.Add(this.m_labelNewPassword);
      this.Controls.Add(this.m_confirmPassword);
      this.Controls.Add(this.m_newPassword);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "FormChangePassword";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Change Password";
      this.Controls.SetChildIndex(this.m_newPassword, 0);
      this.Controls.SetChildIndex(this.m_confirmPassword, 0);
      this.Controls.SetChildIndex(this.m_labelNewPassword, 0);
      this.Controls.SetChildIndex(this.m_labelConfirmNewPassword, 0);
      this.Controls.SetChildIndex(this.m_buttonOk, 0);
      this.Controls.SetChildIndex(this.m_buttonCancel, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

		#endregion

    /// <summary>
    /// Handles the OK click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnOk( object sender, System.EventArgs args )
    {

      try
      {
        if (m_newPassword.Text != m_confirmPassword.Text)
        {
          Viatronix.UI.MessageBox.ShowError("Passwords do not match.");
          m_newPassword.Focus();
          m_newPassword.SelectAll();
          return;
        }

        m_securityProvider.ChangePassword( m_user, m_newPassword.Text);

      }
      catch (UnauthorizedAccessException )
      {
        UI.MessageBox.ShowError("You do not have the rights to change password");
        this.Dispose();
        return;
      }
      catch (PasswordInvalidException ex)
      {
        Logging.Log.Error("Password change failed [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormChangePassword", "OnOK");
        UI.MessageBox.ShowError("Invalid password specified : " + ex.Message);
        m_newPassword.Focus();
        m_newPassword.SelectAll();
        return;

      }
      catch (Exception ex)
      {
        Logging.Log.Error("Password change failed [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormChangePassword", "OnOK");
        if (ex.Message.StartsWith("VX"))
        {
          Viatronix.UI.MessageBox.ShowError(ex.Message.Substring(ex.Message.IndexOf(":") + 1));
        }
        else
        {
          Viatronix.UI.MessageBox.ShowError("Password change failed, please contact service");
        }
        m_newPassword.Focus();
        m_newPassword.SelectAll();
        return;
      }


      ////////////////////////////////////////////////////////////////////////////////////////
      //// if the User has Modify User Permissions he doesn't need to know the old password.
      ///////////////////////////////////////////////////////////////////////////////////////
      //if( Global.CurrentUser.Equals( m_user ) || ( ! Global.CurrentUser.Equals(m_user) && Global.CurrentUser.IsAllowed("UserManagement")) )
      //{
      //  if( m_currentPassword !=  Viatronix.Utilities.Crypt.EncryptReadable( m_oldPassword.Text) )
      //  {
      //    Viatronix.UI.MessageBox.ShowError("Old password is incorrect.");
      //    m_oldPassword.SelectAll();
      //    m_oldPassword.Focus();
      //    return;
      //  }
      //}
      //else
      //{
      //  UI.MessageBox.ShowError("You do not have the rights to change password");
      //  this.Dispose();
      //  return;
      //}

      //try
      //{
      //  Global.UMAS.ValidatePassword( m_user.Login, Global.UMAS.EncryptPassword(m_newPassword.Text));
      //  Global.UMAS.ChangePassword( m_user, Viatronix.Utilities.Crypt.EncryptReadable(m_newPassword.Text));
      //}
      //catch ( UMASException ex )
      //{
      //  Logging.Log.Error("Password change failed [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormChangePassword", "OnOK");
      //  Viatronix.UI.MessageBox.ShowError(ex);
      //  m_newPassword.Focus();
      //  m_newPassword.SelectAll();
      //  return;
      //}
      //catch ( Exception ex )
      //{
      //  Logging.Log.Error("Password change failed [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormChangePassword", "OnOK");
      //  Viatronix.UI.MessageBox.ShowError("Password change failed, please contact service");
      //  m_newPassword.Focus();
      //  m_newPassword.SelectAll();
      //  return;
      //}

      DialogResult = DialogResult.OK;

      this.Close();

    }//OnOk( object sender, System.EventArgs args )



    /// <summary>
    /// Handles the password enter confirmtaion
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnConfirmPasswordEnter(object sender, System.EventArgs args)
    {
      m_confirmPassword.SelectAll();
    }//OnConfirmPasswordEnter(object sender, System.EventArgs args)


    /// <summary>
    /// Handles the Cancel Method
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnCancel(object sender, System.EventArgs args)
    {
      this.DialogResult = DialogResult.Cancel;
      this.Close();
    }

    /// <summary>
    /// Hanldes the mouse enter event for the new password text box 
    /// control
    /// </summary>
    /// <param name="sender">Sender</param>
    /// <param name="e">Event args</param>
    private void OnMouseEnterNewPassword(object sender, EventArgs e)
    {
      m_toolTip.IsBalloon = true;
      m_toolTip.AutoPopDelay = 1000000;
      m_toolTip.SetToolTip(m_newPassword, m_passwordHints);
    } // OnMouseEnterNewPassword(object sender, EventArgs e)

    /// <summary>
    /// Handles the mouse leave event for when the mouse leave the new password text box control
    /// </summary>
    /// <param name="sender">Sender </param>
    /// <param name="e">Event args</param>
    private void OnMouseLeaveNewPassword(object sender, EventArgs e)
    {

      m_toolTip.IsBalloon = false;
      m_toolTip.AutoPopDelay = 5000;
    }//OnMouseLeaveNewPassword(object sender, EventArgs e)


    #endregion methods

	} // class FormChangePassword : Viatronix.UI.Dialog 
} // namespace Viatronix.UI

