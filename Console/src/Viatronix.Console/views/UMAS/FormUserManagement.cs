// $Id: FormUserManagement.cs,v 1.16.2.4 2008/09/28 02:48:36 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;

using Viatronix.UI;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Models;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
	/// <summary>
	/// Summary description for FormRoleManagement.
	/// </summary>
	public class FormUserManagement : Dialog
	{
  
    #region fields

    /// <summary>
    /// The model.
    /// </summary>
    private UserManagementModel m_model = null;

    /// <summary>
    /// Domain text boxs back color
    /// </summary>
    private Color m_comboBoxDomainBackColor = Color.White;

    /// <summary>
    /// Domain text boxs back color
    /// </summary>
    private Color m_passwordBackColor = Color.White;

    /// <summary>
    /// Domain text boxs back color
    /// </summary>
    private Color m_confirmPasswordBackColor = Color.White;

    /// <summary>
    /// User Object which gets creates from the GUI elements
    /// </summary>
		private User m_user = null;

    /// <summary>
    /// Button Object for Dialog Cancel operations
    /// </summary>
		private Viatronix.UI.Button m_buttonCancel = null;

    /// <summary>
    /// Button Object for Dlg Save Operations
    /// </summary>
		private Viatronix.UI.Button m_buttonSave = null;

		/// <summary>
		/// Textbox Object for the LoginName
		/// </summary>
    private System.Windows.Forms.TextBox m_textBoxLogin = null;

    /// <summary>
    /// Text Box object fro the User name
    /// </summary>
		private System.Windows.Forms.TextBox m_textBoxUserName = null;

    /// <summary>
    /// Text box object for the password field.
    /// </summary>
		private System.Windows.Forms.TextBox m_textBoxPassword = null;

    /// <summary>
    /// Text box object for the Confirm Password field.
    /// </summary>
		private System.Windows.Forms.TextBox m_textBoxConfirmPassword = null;

    /// <summary>
    /// User Expiration Date label object
    /// </summary>
		private System.Windows.Forms.Label m_labelUserExpirn = null;

    /// <summary>
    /// Label object for username
    /// </summary>
		private System.Windows.Forms.Label m_labelUserName = null;

    /// <summary>
    /// Label object for the Role
    /// </summary>
		private System.Windows.Forms.Label m_labelRole = null;

    /// <summary>
    /// Label obejct for the Login name
    /// </summary>
		private System.Windows.Forms.Label m_labelLogin = null;

    /// <summary>
    /// Label object for password
    /// </summary>
    private System.Windows.Forms.Label m_labelPassword = null;

    /// <summary>
    /// Label object for confirm password.
    /// </summary>
    private System.Windows.Forms.Label m_labelConfirmPassword = null;

    /// <summary>
    /// DateTimePicker Object for Expiration Date.
    /// </summary>
		private System.Windows.Forms.DateTimePicker m_expirationDatePicker = null;
	
		/// <summary>
		/// Check box for Password change user state.
		/// </summary>
    private Viatronix.UI.CheckBox m_pwdChange = null;

    /// <summary>
    /// Check box for User Lock User State
    /// </summary>
		private Viatronix.UI.CheckBox m_userLock = null;

		/// <summary>
		/// Combo box object for Avalable User Roles.
		/// </summary>
    private Viatronix.UI.ComboBox m_comboBoxRoles = null;

    /// <summary>
    /// Flag indicates whether the User managemnet operation is New or Update
    /// </summary>
		private bool m_createUser = true;

    /// <summary>
    /// Tool Tip Object
    /// </summary>
    private System.Windows.Forms.ToolTip m_toolTip = null;

    /// <summary>
    /// Combo box object for Authentication Type
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxAuthType = null;

    /// <summary>
    /// Label object for authentication
    /// </summary>
    private System.Windows.Forms.Label m_labelAuth = null;

    /// <summary>
    /// Label object for domain
    /// </summary>
    private System.Windows.Forms.Label m_labelDomain = null;

    /// <summary>
    /// Component object
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Domain combo box
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxDomain = null;

    /// <summary>
    /// Flag indicates the authentication change state
    /// </summary>
    private bool m_authenticationChange = false;

    #endregion

    #region construction

    /// <summary>
    /// FormUserManagement private constructor
    /// </summary>
    private FormUserManagement()
    {
      InitializeComponent();
    } // FormUserManagement()


    /// <summary>
    /// FormUserManagement Constructor 
    /// </summary>
    /// <param name="security">Security Provider.</param>
    /// <param name="user">User Object</param>
    /// <param name="existingRoles">ArraList of Roles</param>
    public FormUserManagement(ISecurityProvider security, User user, List<Role> existingRoles)
    {
      InitializeComponent();

      m_model = new UserManagementModel(security);

      m_createUser = (user.Login.Length == 0);

      //Store the User IN Object Locally.
      m_user = user;

      //Populate the GUI
      m_textBoxUserName.Text = m_user.Name;
      m_textBoxLogin.Text = m_user.Login;

      m_user.ExpirationDate = DateTime.Now.AddYears(m_model.DefaultUserExpiration).Date;

      // Set up the domain list
      m_comboBoxDomain.Enabled = false;
      m_comboBoxDomain.BackColor = Color.LightGray;
      m_comboBoxDomain.Items.Add(Global.DataProvider.GetDomain());
      m_comboBoxDomain.SelectedIndex = 0;

      m_comboBoxAuthType.Items.AddRange(m_model.AuthenticationTypes);
      if (m_model.DefaultAuthenticationType != null)
      {
        m_comboBoxAuthType.SelectedItem = m_model.DefaultAuthenticationType;
      }

      m_comboBoxDomainBackColor = m_comboBoxDomain.BackColor;
      m_passwordBackColor = m_textBoxPassword.BackColor;
      m_confirmPasswordBackColor = m_textBoxConfirmPassword.BackColor;

      //If a new User Design the GUI for Create else for Modification.
      if (m_createUser)
      {
        m_buttonSave.Text = "Create";
        m_textBoxLogin.ReadOnly = false;
        m_textBoxPassword.Visible = true;
        m_textBoxConfirmPassword.Visible = true;
        m_labelConfirmPassword.Visible = true;
        m_labelPassword.Visible = true;
        this.Text = "Create User";

        if(m_model.DefaultAuthenticationType.Id != "Viatronix")
        {
          m_comboBoxDomain.Enabled = true;
          m_comboBoxDomain.BackColor = m_comboBoxDomainBackColor;

          m_textBoxPassword.Visible = false;
          m_textBoxConfirmPassword.Visible = false;

          m_labelConfirmPassword.Visible = false;
          m_labelPassword.Visible = false;
          this.m_pwdChange.Location = m_textBoxPassword.Location;
          this.m_userLock.Location = m_textBoxConfirmPassword.Location;

          this.ClientSize = new System.Drawing.Size(this.Width, (m_userLock.Location.Y + m_userLock.Height + 10));
        }
      }
      else
      {
        m_buttonSave.Text = "Update";
        m_textBoxLogin.ReadOnly = true;
        m_textBoxPassword.Visible = false;
        m_textBoxConfirmPassword.Visible = false;
        m_labelConfirmPassword.Visible = false;
        m_labelPassword.Visible = false;


        foreach (object item in m_comboBoxAuthType.Items)
        {
          AuthenticationType type = (AuthenticationType)item;
          if (user.IsAuthenticationType(type.Id))
          {
            m_comboBoxAuthType.SelectedItem = type;
            break;
          }
        }

        if (!user.ViatronixAuthentication)
        {
          m_comboBoxDomain.Enabled = true;
          m_comboBoxDomain.BackColor = m_comboBoxDomainBackColor;

          if (!m_comboBoxDomain.Items.Contains(user.Domain.ToUpper()))
          {
            m_comboBoxDomain.SelectedIndex = m_comboBoxDomain.Items.Add(user.Domain.ToUpper());
          }
        }

        this.Text = "Update User";
        this.m_pwdChange.Location = m_labelPassword.Location;
        this.m_userLock.Location = m_labelConfirmPassword.Location;
        this.m_buttonSave.Location = new Point(m_buttonSave.Location.X, m_pwdChange.Location.Y + 70);
        this.m_buttonCancel.Location = new Point(m_buttonCancel.Location.X, m_pwdChange.Location.Y + 70);

        this.ClientSize = new System.Drawing.Size(this.Width, (m_buttonSave.Location.Y + m_buttonSave.Height + 10));
      }

      if (m_user.State == UserStates.Locked)
      {
        m_userLock.Checked = true;
        m_pwdChange.Enabled = false;
      }

      if ((m_user.State & UserStates.PasswordChange) == UserStates.PasswordChange)
        m_pwdChange.Checked = true;

      //If the expiration date is not selected show the maximum date
      if (m_user.ExpirationDate.Date == System.DateTime.MaxValue.Date)
      {
        m_expirationDatePicker.ShowCheckBox = true;
        m_expirationDatePicker.Checked = false;
      }
      else
      {
        m_expirationDatePicker.Value = m_user.ExpirationDate.DateTime;
        m_expirationDatePicker.ShowCheckBox = true;
        m_expirationDatePicker.Checked = true;
      }

      int roleIndex = -1;
      int nCount = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Fill the roles combo with available Roles. Only a user with a role of service cab add a another
      // service user so we filter out that requirement here
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      foreach (Role role in existingRoles)
      {
        ///////////////////////////////////////////////////////////
        // Only add roles that the current user can add user for
        //////////////////////////////////////////////////////////
        if (Global.CurrentUser.IsService)
        {
          m_comboBoxRoles.Items.Add(role);

          //Find the existing Role ID of the IN User
          if (user.Role == role.Name)
            roleIndex = nCount;

          nCount++;
        } // END ... If the use is allowed to add user for this role
      } // END ... For each role

      m_comboBoxRoles.SelectedIndex = roleIndex;
    } // FormUserManagement( security, user, existingRoles )


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
      }

      base.Dispose(disposing);
    }//void Dispose( disposing )

    #endregion

    #region methods

    /// <summary>
    /// ValidateUser, validate the GUI elements to create a valid User Object.
    /// </summary>
    /// <returns></returns>
		private bool ValidateUser()
		{
      if ( ! ValidateLogin() ) return false;
      if ( ! ValidateUserName() ) return false;
      if ( ! ValidatePassword() ) return false;

      if( m_createUser )
      {
        // Pass the selected Index as the Role ID.
        // Extract the correct Databse ID from the caller function during Save Operation.
        if( m_comboBoxRoles.SelectedIndex == -1  )
        {
          Viatronix.UI.MessageBox.ShowInfo("You must select a role for this user");
          m_comboBoxRoles.Focus();
          return false;
        }
      }

			return true;
		} // ValidateUser()


    /// <summary>
    /// Validates Password
    /// </summary>
    /// <returns></returns>
    private bool ValidatePassword()
    {
      if( m_createUser && m_comboBoxAuthType.SelectedItem.ToString() == "Viatronix" )
      {
        ////////////////////////////////
        // Check for empty password
        //////////////////////////////
        if( m_textBoxPassword.Text == string.Empty )
        {
          Viatronix.UI.MessageBox.ShowInfo("Passwords cannot be empty");
          m_textBoxPassword.Focus();
          return false;
        }

        /////////////////////////////////
        // Check for  Password match
        ////////////////////////////////
        if( m_textBoxConfirmPassword.Text != m_textBoxPassword.Text )
        {
          Viatronix.UI.MessageBox.ShowInfo("Passwords do not match");
          m_textBoxPassword.Focus();
          m_textBoxPassword.SelectAll();
          return false;
        }

        //try
        //{
        //  Global.UMAS.ValidatePassword(m_textBoxLogin.Text, Global.UMAS.EncryptPassword(m_textBoxPassword.Text));
        //}
        //catch ( Viatronix.v3D.Management.UMASException ex )
        //{
        //  Logging.Log.Error("Password is invalid [LOGIN=" + m_textBoxLogin.Text + "] : " + ex.Message, "FormUserManagement", "ValidatePassword");
        //  Viatronix.UI.MessageBox.ShowError(ex);
        //  m_textBoxPassword.Focus();
        //  m_textBoxPassword.SelectAll();
        //  return false;
        //}
      }

      return true;
    } // ValidatePassword()


    /// <summary>
    /// Validates the user name field
    /// </summary>
    /// <returns></returns>
    private bool ValidateUserName()
    {
      ///////////////////////////////////
      // Check for empty User Name
      //////////////////////////////////
      if( m_textBoxUserName.Text == string.Empty )
      {
        Viatronix.UI.MessageBox.ShowInfo("User's name must be filled in.");
        m_textBoxUserName.Focus();
        return false;
      } // 

      ///////////////////////////////////
      // check for User Name length
      /////////////////////////////////
      if( m_textBoxUserName.Text.Length > 64 )
      {
        Viatronix.UI.MessageBox.ShowInfo("User name should be less than 64 characters.");
        m_textBoxUserName.Focus();
        return false;
      }

      return true;

    } // ValidateUserName()


    /// <summary>
    /// Validates Login 
    /// </summary>
    /// <returns></returns>
    private bool ValidateLogin()
    {
      /////////////////////////////////
      // Check for empty Login Name
      /////////////////////////////////
      if (m_textBoxLogin.Text == string.Empty)
      {
        Viatronix.UI.MessageBox.ShowInfo("You must specifiy a login name.");
        m_textBoxLogin.Focus();
        return false;
      } // END ... If the login name is empty

      ///////////////////////////////////////////
      //Check the text length from the login Name
      ////////////////////////////////////////////
      if (m_textBoxLogin.Text.Length > 16 || m_textBoxLogin.Text.Length < 4)
      {
        Viatronix.UI.MessageBox.ShowInfo("Login name must be between 4 to 16 characters.");
        m_textBoxLogin.Focus();
        return false;
      } // END ... If the login name is not the proper length


      //////////////////////////////////////////////
      // Check for Blank Spaces in the Login Name
      ///////////////////////////////////////////////
      if (m_textBoxLogin.Text.IndexOf(' ', 0) != -1)
      {
        Viatronix.UI.MessageBox.ShowInfo("Spaces in the login are not allowed.");
        m_textBoxLogin.Focus();
        return false;
      } // END ... If the login name contains spaces


      ////////////////////////////////////////////////////////////////////////////////////
      //// Check for a duplicate user name if there is a change in the User Login Name
      ///////////////////////////////////////////////////////////////////////////////////
      //if( ( m_textBoxLogin.Text != m_user.Login ) || m_createUser ) 
      //{
      //  try
      //  {
      //    if ( Global.UMAS.UserExists(m_textBoxLogin.Text) ) 
      //    {
      //      Viatronix.UI.MessageBox.ShowError("Login " + m_textBoxLogin.Text + " already exists. Select another Login ");
      //      m_textBoxLogin.SelectAll();
      //      m_textBoxLogin.Focus();
      //      return false;
      //    }
      //  }
      //  catch ( Exception ex)
      //  {
      //    Logging.Log.Error("Failed to checking users existence : " + ex.Message, "FormUserManagement", "ValidateLogin");
      //    Viatronix.UI.MessageBox.ShowError("Failed checking the validity of the login, please contact service");
      //    m_textBoxLogin.Focus();
      //    return false;
      //  }
      //}

      return true;
    } // ValidateLogin()

    #endregion

    #region event handlers

    /// <summary>
    /// OnSaveUser, handles the Create or Update User Object from the GUI
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnSaveUser(object sender, System.EventArgs args)
    {
      //Validate the GUI elements before User Creation.
      if (ValidateUser() == true)
      {
        //Get validated values from the GUI
        m_user.Name = m_textBoxUserName.Text;
        m_user.Login = m_textBoxLogin.Text;
        m_user.Role = ((Role)m_comboBoxRoles.SelectedItem).Name;

        if (m_comboBoxAuthType.SelectedItem.ToString() != "Viatronix")
        {
          m_user.AuthenticationType = m_comboBoxAuthType.SelectedItem.ToString();
          m_user.Domain = m_comboBoxDomain.Text;
        }
        else
        {
          //Check if the Current auth type  modified to Viatronix.
          if (m_user.AuthenticationType != m_comboBoxAuthType.SelectedItem.ToString())
            m_authenticationChange = true;
          m_user.AuthenticationType = m_comboBoxAuthType.SelectedItem.ToString();
          m_user.Domain = string.Empty;
        }

        //Handle the User State. 
        m_user.State = UserStates.None;

        if (m_pwdChange.Checked == true) m_user.State = UserStates.PasswordChange;

        if (m_userLock.Checked == true) m_user.State = UserStates.Locked;

        //Handle the expiration date.
        //If the expiration date is not selected keep the Date from MaxValue of DateTime in the database.

        if (m_expirationDatePicker.Checked == true)
        {
          m_user.ExpirationDate = (System.DateTime)m_expirationDatePicker.Value.Date;
          System.DateTimeOffset currentDate = Global.DataProvider.GetDate();
          if (m_user.ExpirationDate.Date < currentDate.Date)
          {
            Viatronix.UI.MessageBox.ShowError("Invalid Expiration date. Check the date again.");
            return;
          }
        }
        else
          m_user.ExpirationDate = DateTime.Now.AddYears(30).Date;

        if (m_createUser)
        {
          try
          {
            Global.SecurityProvider.Insert(m_user, m_textBoxPassword.Text);
          }
          catch (Exception ex)
          {
            Logging.Log.Error("Failed to add user [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormUserManagement", "OnSaveUser");

            if (ex.Message.StartsWith("VX"))
            {
              UI.MessageBox.ShowError(ex.Message.Substring(ex.Message.IndexOf(":") + 1));
            }
            else
            {
              UI.MessageBox.ShowError(ex);
            }
            m_textBoxLogin.Focus();
            return;
          }
        }
        else
        {
          try
          {
            if (m_authenticationChange == true)
            {
              //Show Password Dialog Box for this user.
              Viatronix.UI.FormChangePassword formChangePassword = new Viatronix.UI.FormChangePassword(Global.Frame.SecurityProvider, Global.CurrentUser, m_user);
              formChangePassword.ShowDialog(this);
              formChangePassword.Dispose();
            }
            Global.SecurityProvider.Update(m_user);

          }
          catch (ViatronixException ex)
          {
            Logging.Log.Error("Failed to update user [LOGIN=" + m_user.Login + "] : " + ex.Message, "FormUserManagement", "OnSaveUser");
            UI.MessageBox.ShowError(ex);
            m_textBoxLogin.Focus();
            return;
          }
        }

        DialogResult = DialogResult.OK;

        this.Close();

      } // END ... If the user information is valid
    } // void OnSaveUser(object sender, System.EventArgs args)


    /// <summary>
    /// Handles the Cancel Button Click
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnCancel(object sender, System.EventArgs args)
    {
      this.Dispose();
    }//void OnCancel(object sender, System.EventArgs args)


    /// <summary>
    /// Handles the user lock clcik event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnUserLockClick(object send, System.EventArgs args)
    {
      if (m_userLock.Checked)
      {
        m_pwdChange.Checked = false;
        m_pwdChange.Enabled = false;
      }
      else
      {
        m_pwdChange.Enabled = true;
      }
    } // OnUserLockClick()


    /// <summary>
    /// Handles the AutheticationTypeChange Event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnAuthenticationTypeChanged( object sender, System.EventArgs args )
    {
      m_comboBoxDomain.SelectedIndex  = 0;

      if (  m_comboBoxAuthType.SelectedItem.ToString() != "Viatronix")
      {
        //Authentication type is Windows.

        string domain = Global.DataProvider.GetDomain();
        m_comboBoxDomain.Enabled        = ( domain.Length > 0 );
        m_comboBoxDomain.BackColor      = (m_comboBoxDomain.Enabled ? Color.White : Color.LightGray);

        m_textBoxPassword.Enabled = false;
        m_textBoxPassword.BackColor = Color.LightGray;
        m_textBoxConfirmPassword.Enabled = false;
        m_textBoxConfirmPassword.BackColor = Color.LightGray;
        m_pwdChange.Enabled = false;
      }
      else
      {
        m_comboBoxDomain.Enabled        = false;
        m_comboBoxDomain.BackColor      = Color.LightGray;

        m_pwdChange.Enabled = true;
        m_textBoxPassword.Enabled = true;
        m_textBoxPassword.BackColor = m_passwordBackColor;
        m_textBoxConfirmPassword.Enabled = true;
        m_textBoxConfirmPassword.BackColor = m_confirmPasswordBackColor;

      }
    }//OnAuthenticationTypeChanged(object sender, System.EventArgs args)


    private void OnPasswordMouseEnter(object sender, EventArgs e)
    {
      m_toolTip.IsBalloon = true;
      m_toolTip.AutoPopDelay = 1000000;
      m_toolTip.SetToolTip(m_textBoxPassword, m_model.GetPasswordHints());
    }


    private void OnPasswordMouseLeave(object sender, EventArgs e)
    {
      m_toolTip.AutoPopDelay = 5000;
      m_toolTip.IsBalloon = false;
    }

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormUserManagement));
      this.m_textBoxUserName = new System.Windows.Forms.TextBox();
      this.m_labelUserName = new System.Windows.Forms.Label();
      this.m_buttonCancel = new Viatronix.UI.Button();
      this.m_buttonSave = new Viatronix.UI.Button();
      this.m_labelLogin = new System.Windows.Forms.Label();
      this.m_textBoxLogin = new System.Windows.Forms.TextBox();
      this.m_userLock = new Viatronix.UI.CheckBox();
      this.m_expirationDatePicker = new System.Windows.Forms.DateTimePicker();
      this.m_labelUserExpirn = new System.Windows.Forms.Label();
      this.m_labelRole = new System.Windows.Forms.Label();
      this.m_comboBoxRoles = new Viatronix.UI.ComboBox();
      this.m_pwdChange = new Viatronix.UI.CheckBox();
      this.m_textBoxPassword = new System.Windows.Forms.TextBox();
      this.m_textBoxConfirmPassword = new System.Windows.Forms.TextBox();
      this.m_labelPassword = new System.Windows.Forms.Label();
      this.m_labelConfirmPassword = new System.Windows.Forms.Label();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_comboBoxAuthType = new System.Windows.Forms.ComboBox();
      this.m_comboBoxDomain = new System.Windows.Forms.ComboBox();
      this.m_labelAuth = new System.Windows.Forms.Label();
      this.m_labelDomain = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // m_textBoxUserName
      // 
      this.m_textBoxUserName.Location = new System.Drawing.Point(153, 45);
      this.m_textBoxUserName.Name = "m_textBoxUserName";
      this.m_textBoxUserName.Size = new System.Drawing.Size(160, 20);
      this.m_textBoxUserName.TabIndex = 1;
      this.m_toolTip.SetToolTip(this.m_textBoxUserName, "Enter the users full name");
      // 
      // m_labelUserName
      // 
      this.m_labelUserName.BackColor = System.Drawing.Color.Transparent;
      this.m_labelUserName.ForeColor = System.Drawing.Color.White;
      this.m_labelUserName.Location = new System.Drawing.Point(13, 47);
      this.m_labelUserName.Name = "m_labelUserName";
      this.m_labelUserName.Size = new System.Drawing.Size(124, 20);
      this.m_labelUserName.TabIndex = 3;
      this.m_labelUserName.Text = "Full Name:";
      this.m_labelUserName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonCancel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonCancel.Location = new System.Drawing.Point(248, 311);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.Size = new System.Drawing.Size(65, 25);
      this.m_buttonCancel.TabIndex = 11;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancel);
      // 
      // m_buttonSave
      // 
      this.m_buttonSave.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonSave.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonSave.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonSave.Location = new System.Drawing.Point(185, 311);
      this.m_buttonSave.Name = "m_buttonSave";
      this.m_buttonSave.OldStockButton = false;
      this.m_buttonSave.Size = new System.Drawing.Size(61, 25);
      this.m_buttonSave.TabIndex = 10;
      this.m_buttonSave.Text = "Save";
      this.m_buttonSave.Click += new System.EventHandler(this.OnSaveUser);
      // 
      // m_labelLogin
      // 
      this.m_labelLogin.BackColor = System.Drawing.Color.Transparent;
      this.m_labelLogin.ForeColor = System.Drawing.Color.White;
      this.m_labelLogin.Location = new System.Drawing.Point(13, 19);
      this.m_labelLogin.Name = "m_labelLogin";
      this.m_labelLogin.Size = new System.Drawing.Size(124, 20);
      this.m_labelLogin.TabIndex = 10;
      this.m_labelLogin.Text = "Login Name:";
      this.m_labelLogin.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_textBoxLogin
      // 
      this.m_textBoxLogin.Location = new System.Drawing.Point(153, 17);
      this.m_textBoxLogin.Name = "m_textBoxLogin";
      this.m_textBoxLogin.Size = new System.Drawing.Size(160, 20);
      this.m_textBoxLogin.TabIndex = 0;
      this.m_toolTip.SetToolTip(this.m_textBoxLogin, "Enter the users login name");
      // 
      // m_userLock
      // 
      this.m_userLock.BackColor = System.Drawing.Color.Transparent;
      this.m_userLock.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_userLock.ForeColor = System.Drawing.Color.White;
      this.m_userLock.Checked = false;
      this.m_userLock.Location = new System.Drawing.Point(14, 282);
      this.m_userLock.Name = "m_userLock";
      this.m_userLock.Size = new System.Drawing.Size(233, 20);
      this.m_userLock.TabIndex = 9;
      this.m_userLock.Text = "Lock account";
      this.m_toolTip.SetToolTip(this.m_userLock, "Chcek to lock the users account");
      this.m_userLock.UseVisualStyleBackColor = false;
      this.m_userLock.Click += new System.EventHandler(this.OnUserLockClick);
      // 
      // m_expirationDatePicker
      // 
      this.m_expirationDatePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
      this.m_expirationDatePicker.Location = new System.Drawing.Point(153, 159);
      this.m_expirationDatePicker.Name = "m_expirationDatePicker";
      this.m_expirationDatePicker.Size = new System.Drawing.Size(160, 20);
      this.m_expirationDatePicker.TabIndex = 5;
      this.m_toolTip.SetToolTip(this.m_expirationDatePicker, "Set an expiration date for this user");
      // 
      // m_labelUserExpirn
      // 
      this.m_labelUserExpirn.BackColor = System.Drawing.Color.Transparent;
      this.m_labelUserExpirn.ForeColor = System.Drawing.Color.White;
      this.m_labelUserExpirn.Location = new System.Drawing.Point(13, 161);
      this.m_labelUserExpirn.Name = "m_labelUserExpirn";
      this.m_labelUserExpirn.Size = new System.Drawing.Size(124, 20);
      this.m_labelUserExpirn.TabIndex = 13;
      this.m_labelUserExpirn.Text = "Account expires on:";
      this.m_labelUserExpirn.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_labelRole
      // 
      this.m_labelRole.BackColor = System.Drawing.Color.Transparent;
      this.m_labelRole.ForeColor = System.Drawing.Color.White;
      this.m_labelRole.Location = new System.Drawing.Point(13, 75);
      this.m_labelRole.Name = "m_labelRole";
      this.m_labelRole.Size = new System.Drawing.Size(124, 20);
      this.m_labelRole.TabIndex = 14;
      this.m_labelRole.Text = "Role:";
      this.m_labelRole.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxRoles
      // 
      this.m_comboBoxRoles.DisplayMember = "String";
      this.m_comboBoxRoles.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxRoles.ForeColor = System.Drawing.SystemColors.WindowText;
      this.m_comboBoxRoles.Location = new System.Drawing.Point(153, 73);
      this.m_comboBoxRoles.Name = "m_comboBoxRoles";
      this.m_comboBoxRoles.Size = new System.Drawing.Size(160, 22);
      this.m_comboBoxRoles.TabIndex = 2;
      this.m_toolTip.SetToolTip(this.m_comboBoxRoles, "Select the role for this user");
      this.m_comboBoxRoles.ValueMember = "Object";
      // 
      // m_pwdChange
      // 
      this.m_pwdChange.BackColor = System.Drawing.Color.Transparent;
      this.m_pwdChange.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_pwdChange.ForeColor = System.Drawing.Color.White;
      this.m_pwdChange.Location = new System.Drawing.Point(14, 254);
      this.m_pwdChange.Name = "m_pwdChange";
      this.m_pwdChange.Size = new System.Drawing.Size(247, 20);
      this.m_pwdChange.TabIndex = 8;
      this.m_pwdChange.Text = "User must change  password at next  logon";
      this.m_toolTip.SetToolTip(this.m_pwdChange, "Check to firce the user to change their password the next time they login");
      this.m_pwdChange.UseVisualStyleBackColor = false;
      // 
      // m_textBoxPassword
      // 
      this.m_textBoxPassword.Location = new System.Drawing.Point(153, 187);
      this.m_textBoxPassword.Name = "m_textBoxPassword";
      this.m_textBoxPassword.PasswordChar = '*';
      this.m_textBoxPassword.Size = new System.Drawing.Size(160, 20);
      this.m_textBoxPassword.TabIndex = 6;
      this.m_toolTip.SetToolTip(this.m_textBoxPassword, "Enetr the users password");
      this.m_textBoxPassword.MouseLeave += new System.EventHandler(this.OnPasswordMouseLeave);
      this.m_textBoxPassword.MouseEnter += new System.EventHandler(this.OnPasswordMouseEnter);
      // 
      // m_textBoxConfirmPassword
      // 
      this.m_textBoxConfirmPassword.Location = new System.Drawing.Point(153, 215);
      this.m_textBoxConfirmPassword.Name = "m_textBoxConfirmPassword";
      this.m_textBoxConfirmPassword.PasswordChar = '*';
      this.m_textBoxConfirmPassword.Size = new System.Drawing.Size(160, 20);
      this.m_textBoxConfirmPassword.TabIndex = 7;
      this.m_toolTip.SetToolTip(this.m_textBoxConfirmPassword, "Confirm the password you just entered above");
      // 
      // m_labelPassword
      // 
      this.m_labelPassword.BackColor = System.Drawing.Color.Transparent;
      this.m_labelPassword.ForeColor = System.Drawing.Color.White;
      this.m_labelPassword.Location = new System.Drawing.Point(13, 189);
      this.m_labelPassword.Name = "m_labelPassword";
      this.m_labelPassword.Size = new System.Drawing.Size(124, 20);
      this.m_labelPassword.TabIndex = 18;
      this.m_labelPassword.Text = "Password:";
      this.m_labelPassword.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_labelConfirmPassword
      // 
      this.m_labelConfirmPassword.BackColor = System.Drawing.Color.Transparent;
      this.m_labelConfirmPassword.ForeColor = System.Drawing.Color.White;
      this.m_labelConfirmPassword.Location = new System.Drawing.Point(13, 217);
      this.m_labelConfirmPassword.Name = "m_labelConfirmPassword";
      this.m_labelConfirmPassword.Size = new System.Drawing.Size(124, 20);
      this.m_labelConfirmPassword.TabIndex = 19;
      this.m_labelConfirmPassword.Text = "Confirm Password:";
      this.m_labelConfirmPassword.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxAuthType
      // 
      this.m_comboBoxAuthType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxAuthType.Location = new System.Drawing.Point(153, 102);
      this.m_comboBoxAuthType.MaxDropDownItems = 6;
      this.m_comboBoxAuthType.Name = "m_comboBoxAuthType";
      this.m_comboBoxAuthType.Size = new System.Drawing.Size(160, 22);
      this.m_comboBoxAuthType.TabIndex = 3;
      this.m_toolTip.SetToolTip(this.m_comboBoxAuthType, "Select the authentication type for this user");
      this.m_comboBoxAuthType.SelectedIndexChanged += new System.EventHandler(this.OnAuthenticationTypeChanged);
      // 
      // m_comboBoxDomain
      // 
      this.m_comboBoxDomain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxDomain.Location = new System.Drawing.Point(153, 131);
      this.m_comboBoxDomain.MaxDropDownItems = 6;
      this.m_comboBoxDomain.Name = "m_comboBoxDomain";
      this.m_comboBoxDomain.Size = new System.Drawing.Size(160, 22);
      this.m_comboBoxDomain.TabIndex = 24;
      this.m_toolTip.SetToolTip(this.m_comboBoxDomain, "Select the authentication type for this user");
      // 
      // m_labelAuth
      // 
      this.m_labelAuth.BackColor = System.Drawing.Color.Transparent;
      this.m_labelAuth.ForeColor = System.Drawing.Color.White;
      this.m_labelAuth.Location = new System.Drawing.Point(13, 104);
      this.m_labelAuth.Name = "m_labelAuth";
      this.m_labelAuth.Size = new System.Drawing.Size(124, 20);
      this.m_labelAuth.TabIndex = 21;
      this.m_labelAuth.Text = "Authentication Type:";
      this.m_labelAuth.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_labelDomain
      // 
      this.m_labelDomain.BackColor = System.Drawing.Color.Transparent;
      this.m_labelDomain.ForeColor = System.Drawing.Color.White;
      this.m_labelDomain.Location = new System.Drawing.Point(13, 133);
      this.m_labelDomain.Name = "m_labelDomain";
      this.m_labelDomain.Size = new System.Drawing.Size(124, 20);
      this.m_labelDomain.TabIndex = 23;
      this.m_labelDomain.Text = "Authentication Domain:";
      this.m_labelDomain.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // FormUserManagement
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.BackColor = System.Drawing.SystemColors.Desktop;
      this.ClientSize = new System.Drawing.Size(322, 390);
      this.Controls.Add(this.m_comboBoxDomain);
      this.Controls.Add(this.m_labelDomain);
      this.Controls.Add(this.m_textBoxConfirmPassword);
      this.Controls.Add(this.m_textBoxPassword);
      this.Controls.Add(this.m_textBoxLogin);
      this.Controls.Add(this.m_textBoxUserName);
      this.Controls.Add(this.m_labelAuth);
      this.Controls.Add(this.m_comboBoxAuthType);
      this.Controls.Add(this.m_labelConfirmPassword);
      this.Controls.Add(this.m_labelPassword);
      this.Controls.Add(this.m_pwdChange);
      this.Controls.Add(this.m_comboBoxRoles);
      this.Controls.Add(this.m_labelRole);
      this.Controls.Add(this.m_labelUserExpirn);
      this.Controls.Add(this.m_expirationDatePicker);
      this.Controls.Add(this.m_userLock);
      this.Controls.Add(this.m_labelLogin);
      this.Controls.Add(this.m_buttonSave);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_labelUserName);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "FormUserManagement";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "User Management";
      this.Controls.SetChildIndex(this.m_labelUserName, 0);
      this.Controls.SetChildIndex(this.m_buttonCancel, 0);
      this.Controls.SetChildIndex(this.m_buttonSave, 0);
      this.Controls.SetChildIndex(this.m_labelLogin, 0);
      this.Controls.SetChildIndex(this.m_userLock, 0);
      this.Controls.SetChildIndex(this.m_expirationDatePicker, 0);
      this.Controls.SetChildIndex(this.m_labelUserExpirn, 0);
      this.Controls.SetChildIndex(this.m_labelRole, 0);
      this.Controls.SetChildIndex(this.m_comboBoxRoles, 0);
      this.Controls.SetChildIndex(this.m_pwdChange, 0);
      this.Controls.SetChildIndex(this.m_labelPassword, 0);
      this.Controls.SetChildIndex(this.m_labelConfirmPassword, 0);
      this.Controls.SetChildIndex(this.m_comboBoxAuthType, 0);
      this.Controls.SetChildIndex(this.m_labelAuth, 0);
      this.Controls.SetChildIndex(this.m_textBoxUserName, 0);
      this.Controls.SetChildIndex(this.m_textBoxLogin, 0);
      this.Controls.SetChildIndex(this.m_textBoxPassword, 0);
      this.Controls.SetChildIndex(this.m_textBoxConfirmPassword, 0);
      this.Controls.SetChildIndex(this.m_labelDomain, 0);
      this.Controls.SetChildIndex(this.m_comboBoxDomain, 0);
      this.ResumeLayout(false);
      this.PerformLayout();
    } // InitializeComponent()

    #endregion

	} // class FormUserManagement
} // namespace Viatronix.Console


#region revision history

// $Log: FormUserManagement.cs,v $
// Revision 1.16.2.4  2008/09/28 02:48:36  mkontak
// Issue 6030
//
// Revision 1.16.2.3  2008/01/02 20:33:01  mkontak
// Password hints
//
// Revision 1.16.2.2  2007/09/10 17:17:54  mkontak
// Issue 5774
//
// Revision 1.16.2.1  2007/05/11 19:33:41  mkontak
// Issue 5655
//
// Revision 1.16  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.15  2006/05/19 17:53:05  mkontak
// Domain combo box background color
//
// Revision 1.14  2006/05/19 17:41:22  mkontak
// Removed the default domain, modified to have the domain selectable only.
//
// Revision 1.13  2006/04/20 14:19:33  romy
// text aligment
//
// Revision 1.12  2006/04/14 17:11:12  romy
// fixed GUI standards issue
//
// Revision 1.11  2006/03/28 21:03:00  romy
// text fix
//
// Revision 1.10  2006/02/16 17:40:49  gdavidson
// Changed the base class to Viatronix.UI.Dialog
//
// Revision 1.9  2006/02/05 05:56:02  romy
// font color fix
//
// Revision 1.8  2006/02/02 16:26:46  romy
// GUI fixes
//
// Revision 1.7  2006/02/01 22:05:04  romy
// avoids empty Domain name
//
// Revision 1.6  2005/12/02 02:23:08  romy
// coding standards and minor fixes
//
// Revision 1.5  2005/12/01 17:35:39  romy
// fixed the verifypassword  method.
//
// Revision 1.4  2005/10/26 12:51:19  mkontak
// Minor changes
//
// Revision 1.3  2005/10/20 15:11:12  romy
// FIXED past expiration date  selection issue
//
// Revision 1.2  2005/08/05 18:28:18  romy
// code cleaning
//
// Revision 1.1  2005/08/05 14:20:16  romy
// Rearranged Files
//
// Revision 1.18  2005/08/05 13:35:28  romy
// Bug fixes
//
// Revision 1.17  2005/06/08 19:33:01  romy
// Removed Test Code
//
// Revision 1.16  2005/05/27 15:09:48  mkontak
// Fixes
//
// Revision 1.15  2005/05/27 14:14:46  mkontak
// Added user level authentication
//
// Revision 1.14  2005/05/26 18:03:49  mkontak
// Changes for UMAS
//
// Revision 1.13  2005/05/25 13:32:21  mkontak
// Changes to UMAS
//
// Revision 1.12  2005/05/19 18:44:30  romy
// Added Header Information
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/UMAS/FormUserManagement.cs,v 1.16.2.4 2008/09/28 02:48:36 mkontak Exp $
// $Id: FormUserManagement.cs,v 1.16.2.4 2008/09/28 02:48:36 mkontak Exp $

#endregion