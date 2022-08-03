// $Id: FormLogin.cs,v 1.5 2007/02/26 18:41:17 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Xml;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.UI
{
  /// <summary>
  /// About Box for application. Displays information about the active plugin.
  /// </summary>
	public class FormLogin : Viatronix.UI.FormAbout
	{

    #region fields

    /// <summary>
    /// allowed maximum attempts in a row
    /// </summary>
    private const int MaximumAllowedAttempts = 3;


    /// <summary>
    /// Security provider
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// Login attempts
    /// </summary>
    private int m_attempts = 0;

    /// <summary>
    /// login panel
    /// </summary>
    private Viatronix.UI.Panel m_panelLogin;

    /// <summary>
    /// username label
    /// </summary>
    private System.Windows.Forms.Label m_labelLogin;

    /// <summary>
    /// password label
    /// </summary>
    private System.Windows.Forms.Label m_labelPassword;

    /// <summary>
    /// password text box
    /// </summary>
    protected System.Windows.Forms.TextBox m_textBoxPassword;

    /// <summary>
    /// login button
    /// </summary>
    private Viatronix.UI.Button m_buttonLogin;

    /// <summary>
    /// username textbox
    /// </summary>
    protected System.Windows.Forms.TextBox m_textBoxLogin;

    /// <summary>
    /// Designer components object
    /// </summary>
    private System.ComponentModel.Container components = null;
    
    /// <summary>
    /// First line of the login message.
    /// </summary>
    protected System.Windows.Forms.Label m_messageLine1;

    /// <summary>
    /// Second line of the login message.
    /// </summary>
    protected System.Windows.Forms.Label m_messageLine2;

    /// <summary>
    /// Panel for message area
    /// </summary>
    private Viatronix.UI.Panel m_messageArea;

    /// <summary>
    /// User logged in
    /// </summary>
    private User m_user = null;

 
    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    protected FormLogin() : base()
    {
      InitializeComponent();

      this.DrawBullets = false;

      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.UserPaint, true);
    } // FormLogin()

    /// <summary>
    /// Initializes FormLogin with default values.
    /// </summary>
    public FormLogin(ISecurityProvider securityProvider) : this()
		{		
      m_securityProvider = securityProvider;
		} // FormLogin()

    #endregion

    #region properties


    /// <summary>
    /// Gets the security provider
    /// </summary>
    protected ISecurityProvider SecurityProvider
    {
      get { return m_securityProvider; }
    } // SecurityProvider

    /// <summary>
    /// Gets the user logged in, null if login failed
    /// </summary>
    public User User
    {
      get { return m_user; }
      set { m_user = value; }
    } // User


    /// <summary>
    /// Gets or sets the username
    /// </summary>
    public string UserName
    {
      get 
      { 
        if( m_messageLine1.Text.Length > 5 )
          return m_messageLine1.Text.Substring( 5 ); 
        else
          return string.Empty;
      }
      set 
      { 
        if( value != string.Empty )
        {
          m_messageLine1.Text = "Only " + value; 
          m_textBoxLogin.Text = value;
          
          m_textBoxPassword.TabIndex = 2;
          m_buttonLogin.TabIndex = 3;
          m_textBoxLogin.TabIndex = 4;
        }
      }
    } // UserName


    /// <summary>
    /// Gets or sets the fullname
    /// </summary>
    public string FullName
    {
      get
      {
        int index = m_messageLine1.Text.IndexOf( "(" );
        if( index != -1 )
          return m_messageLine1.Text.Substring( index + 1 ).Remove( m_messageLine1.Text.LastIndexOf( ")" ), 1 );
        
        return string.Empty;
      }
      set 
      {
        m_messageLine1.Text = "Only " + this.UserName;
        if( value != string.Empty )
          m_messageLine1.Text += " (" + value + ")"; 
      }
    } // FullName



    #endregion

    #region methods

    /// <summary>
    /// Clears all data
    /// </summary>
    public void ClearData()
    {
      m_user = null;
      m_textBoxLogin.Text = string.Empty;
      m_textBoxPassword.Text = string.Empty;
    } // ClearData()

    /// <summary>
    /// Resets the form
    /// </summary>
    public virtual void Initialize() {}


    /// <summary>
    /// Login operation
    /// </summary>
    /// <param name="username">username</param>
    /// <param name="password">password</param>
    protected virtual void Login( string username, string password )
    {
      m_attempts++;
      bool newPasswordRequired = false;



      try
      {

        m_user = m_securityProvider.Login(username, password);

        if ((m_user.State & UserStates.PasswordChange) == UserStates.PasswordChange)
        {
          newPasswordRequired = true;
        } // END ... If the user is required to change their password
        else
        {
          DialogResult = DialogResult.OK;
          return;
        } // END ... Else no password change is required

      }
      catch (NotFoundException ex)
      {
        Logging.Log.Error("Could not retrieve user [LOGIN=" + username + "] from UMAS : " + ex.Message, "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError("The login name or password is incorrect. Please try again");
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();

        if (m_attempts > MaximumAllowedAttempts)
        {
          DialogResult = DialogResult.Cancel;
          m_user = null;
        }

        return;
      }
      catch (PasswordInvalidException)
      {
        Logging.Log.Error("Invalid password received [LOGIN=" + username + "]", "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError("The login name or password is incorrect. Please try again");
        m_textBoxPassword.Focus();
        m_textBoxPassword.SelectAll();

        if (m_attempts > MaximumAllowedAttempts)
        {
          DialogResult = DialogResult.Cancel;
          m_user = null;
        }

        return;
      }
      catch (NoAvailableLicensesException)
      {
        Logging.Log.Error("No available licenses", "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError("No available licenses. Please try again");


        return;
      }
      catch (PasswordChangeRequiredException)
      {
        Logging.Log.Error("Password expired [LOGIN=" + username + "]", "FormLogin", "Login");
        newPasswordRequired = true;
      }
      catch (LockedException ex)
      {
        m_textBoxPassword.Text = string.Empty;
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();
        Viatronix.UI.MessageBox.ShowError(ex);
        m_user = null;
        return;
      }
      catch (SystemDisabledException ex)
      {
        Viatronix.UI.MessageBox.ShowError(ex);
        return;
      }
      catch (InvalidIdentityException ex)
      {
        Logging.Log.Error("Failed to build user [LOGIN=" + username + "] : " + ex.Message, "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError(ex.Message + "Please contact your system administrator.");
        m_textBoxPassword.Text = string.Empty;
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();

        if (m_attempts > MaximumAllowedAttempts)
          DialogResult = DialogResult.Cancel;

        return;
      }
      catch (ViatronixException ex)
      {
        Logging.Log.Error("login failed [LOGIN=" + username + "] : " + ex.Message, "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError("The login name or password is incorrect. Please try again");
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();

        if (m_attempts > MaximumAllowedAttempts)
        {
          DialogResult = DialogResult.Cancel;
          m_user = null;
        }

        return;
      }
      catch (Exception ex)
      {
        Logging.Log.Error("Failed to login [LOGIN=" + username + "] : " + ex.Message, "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError(ex.Message + ", Please contact your system administrator.");
        m_textBoxPassword.Text = string.Empty;
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();

        if (m_attempts > MaximumAllowedAttempts)
          DialogResult = DialogResult.Cancel;

        return;

      }

      if (newPasswordRequired)
      {
        if (ChangePassword() != DialogResult.OK)
        {
          m_user = null;
          this.Close();
        }
        else
        {
          DialogResult = DialogResult.OK;
          return;
        }
      }
      else
      {
        Logging.Log.Error("Password has expired and requires a change [LOGIN=" + username + "]", "FormLogin", "Login");
        Viatronix.UI.MessageBox.ShowError("Password has expired, please contact your administrator");
      }

    } // Login()

    /// <summary>
    /// Changes user password.
    /// </summary>
    /// <param name="user"></param>
    protected virtual  DialogResult ChangePassword()
    {
      //Show the password change form now.
      FormChangePassword formChangePassword = new FormChangePassword(m_securityProvider, m_user, m_user);

      if (DialogResult.OK == formChangePassword.ShowDialog(this))
      {
        formChangePassword.Dispose();

        try
        {
          //Reset the account with ChangePassword option false
          m_user.State = UserStates.None;
          m_securityProvider.Update(m_user);
          return DialogResult.OK;
        }
        catch (Exception ex)
        {
          Viatronix.UI.MessageBox.ShowError(ex);
          m_textBoxLogin.Focus();
          m_textBoxLogin.SelectAll();
          return DialogResult.Cancel;
        }
      }
      else
      {
        formChangePassword.Dispose();
        return DialogResult.Cancel;
      }
    } // ChangePassword( user, password )



    /// <summary>
    /// Called when the visibility of the bullets area changes.
    /// </summary>
    /// <param name="visible">true if the area is visible; otherwise false</param>
    protected override void OnBulletsVisibleChanged( bool visible )
    {
      base.OnBulletsVisibleChanged( visible );

      m_messageArea.Visible = !visible;
    } // OnBulletsVisibleChanged( visible )


    /// <summary>
    /// Disposes the resources associated with this instance
    /// </summary>
    /// <param name="disposing">True to dispose managed and unmanaged resources; False to dipose unmanaged only</param>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
          components.Dispose();
      }
      base.Dispose( disposing );
    } // Dispose( bool disposing )


    #endregion

    #region event handlers

    /// <summary>
    /// handles the login click
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLoginClickHandler( object sender, EventArgs args )
    {
      Login(m_textBoxLogin.Text, m_textBoxPassword.Text);
    } // OnLoginClick( sender, args )


    /// <summary>
    /// Handles the password enter event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnPasswordEnterHandler( object sender, EventArgs args )
    {
      m_textBoxPassword.SelectAll();
    } // OnPasswordEnter( sender, args )


    /// <summary>
    /// Handles the key down event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnEnterKeyDownHandler( object sender, KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Enter )
        OnLoginClickHandler( sender, EventArgs.Empty );
    } // OnEnterKeyDownHandler( sender, args )

    #endregion

		#region windows form designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_panelLogin = new Viatronix.UI.Panel();
      this.m_buttonLogin = new Viatronix.UI.Button();
      this.m_textBoxPassword = new System.Windows.Forms.TextBox();
      this.m_textBoxLogin = new System.Windows.Forms.TextBox();
      this.m_labelPassword = new System.Windows.Forms.Label();
      this.m_labelLogin = new System.Windows.Forms.Label();
      this.m_messageArea = new Viatronix.UI.Panel();
      this.m_messageLine2 = new System.Windows.Forms.Label();
      this.m_messageLine1 = new System.Windows.Forms.Label();
      this.m_panelLogin.SuspendLayout();
      this.m_messageArea.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_panelLogin
      // 
      this.m_panelLogin.BackColor = System.Drawing.Color.Transparent;
      this.m_panelLogin.BorderColor = System.Drawing.Color.Transparent;
      this.m_panelLogin.BottomPadding = 10;
      this.m_panelLogin.Controls.Add(this.m_buttonLogin);
      this.m_panelLogin.Controls.Add(this.m_textBoxPassword);
      this.m_panelLogin.Controls.Add(this.m_textBoxLogin);
      this.m_panelLogin.Controls.Add(this.m_labelPassword);
      this.m_panelLogin.Controls.Add(this.m_labelLogin);
      this.m_panelLogin.Curve = 10;
      this.m_panelLogin.FillColor = System.Drawing.Color.Transparent;
      this.m_panelLogin.LeftPadding = 10;
      this.m_panelLogin.Location = new System.Drawing.Point(571, 376);
      this.m_panelLogin.Name = "m_panelLogin";
      this.m_panelLogin.RightPadding = 10;
      this.m_panelLogin.Size = new System.Drawing.Size(211, 82);
      this.m_panelLogin.TabIndex = 6;
      this.m_panelLogin.TopPadding = 10;
      // 
      // m_buttonLogin
      // 
      this.m_buttonLogin.ForeColor = System.Drawing.Color.Black;
      this.m_buttonLogin.Location = new System.Drawing.Point(127, 55);
      this.m_buttonLogin.Name = "m_buttonLogin";
      this.m_buttonLogin.OldStockButton = false;
      this.m_buttonLogin.Size = new System.Drawing.Size(75, 23);
      this.m_buttonLogin.TabIndex = 2;
      this.m_buttonLogin.Text = "Login";
      this.m_buttonLogin.Click += new System.EventHandler(this.OnLoginClickHandler);
      // 
      // m_textBoxPassword
      // 
      this.m_textBoxPassword.Location = new System.Drawing.Point(71, 30);
      this.m_textBoxPassword.Name = "m_textBoxPassword";
      this.m_textBoxPassword.PasswordChar = '*';
      this.m_textBoxPassword.Size = new System.Drawing.Size(131, 20);
      this.m_textBoxPassword.TabIndex = 1;
      this.m_textBoxPassword.Enter += new System.EventHandler(this.OnPasswordEnterHandler);
      this.m_textBoxPassword.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnEnterKeyDownHandler);
      // 
      // m_textBoxLogin
      // 
      this.m_textBoxLogin.Location = new System.Drawing.Point(71, 4);
      this.m_textBoxLogin.Name = "m_textBoxLogin";
      this.m_textBoxLogin.Size = new System.Drawing.Size(131, 20);
      this.m_textBoxLogin.TabIndex = 0;
      this.m_textBoxLogin.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnEnterKeyDownHandler);
      // 
      // m_labelPassword
      // 
      this.m_labelPassword.Location = new System.Drawing.Point(11, 29);
      this.m_labelPassword.Name = "m_labelPassword";
      this.m_labelPassword.Size = new System.Drawing.Size(59, 23);
      this.m_labelPassword.TabIndex = 1;
      this.m_labelPassword.Text = "Password:";
      this.m_labelPassword.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_labelLogin
      // 
      this.m_labelLogin.Location = new System.Drawing.Point(11, 3);
      this.m_labelLogin.Name = "m_labelLogin";
      this.m_labelLogin.Size = new System.Drawing.Size(61, 23);
      this.m_labelLogin.TabIndex = 0;
      this.m_labelLogin.Text = "Username:";
      this.m_labelLogin.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_messageArea
      // 
      this.m_messageArea.BackColor = System.Drawing.Color.Transparent;
      this.m_messageArea.BorderColor = System.Drawing.Color.Transparent;
      this.m_messageArea.BottomPadding = 10;
      this.m_messageArea.Controls.Add(this.m_messageLine2);
      this.m_messageArea.Controls.Add(this.m_messageLine1);
      this.m_messageArea.Curve = 10;
      this.m_messageArea.FillColor = System.Drawing.Color.Transparent;
      this.m_messageArea.LeftPadding = 10;
      this.m_messageArea.Location = new System.Drawing.Point(570, 205);
      this.m_messageArea.Name = "m_messageArea";
      this.m_messageArea.RightPadding = 10;
      this.m_messageArea.Size = new System.Drawing.Size(212, 165);
      this.m_messageArea.TabIndex = 0;
      this.m_messageArea.TopPadding = 10;
      // 
      // m_messageLine2
      // 
      this.m_messageLine2.Location = new System.Drawing.Point(24, 50);
      this.m_messageLine2.Name = "m_messageLine2";
      this.m_messageLine2.Size = new System.Drawing.Size(168, 30);
      this.m_messageLine2.TabIndex = 0;
      this.m_messageLine2.Text = "";
      this.m_messageLine2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
      // 
      // m_messageLine1
      // 
      this.m_messageLine1.Location = new System.Drawing.Point(8, 32);
      this.m_messageLine1.Name = "m_messageLine1";
      this.m_messageLine1.Size = new System.Drawing.Size(200, 23);
      this.m_messageLine1.TabIndex = 0;
      this.m_messageLine1.Text = "";
      this.m_messageLine1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // FormLogin
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(800, 600);
      this.Controls.Add(this.m_panelLogin);
      this.Controls.Add(this.m_messageArea);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
      this.Name = "FormLogin";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Viatronix Login";
      this.m_panelLogin.ResumeLayout(false);
      this.m_panelLogin.PerformLayout();
      this.m_messageArea.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

  } // class FormLogin
} // namespace Viatronix.UI

#region revision history

// $Log: FormLogin.cs,v $
// Revision 1.5  2007/02/26 18:41:17  gdavidson
// Changed to Viatronix panels that use DoubleBuffering
//
// Revision 1.4  2006/10/30 16:07:04  gdavidson
// Added a User property
//
// Revision 1.3  2006/05/05 20:28:38  gdavidson
// Issue #4679: Changed the Idle timer
//
// Revision 1.2  2006/01/18 20:30:03  gdavidson
// Fixed the tab order
//
// Revision 1.1  2006/01/18 15:41:00  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/FormLogin.cs,v 1.5 2007/02/26 18:41:17 gdavidson Exp $
// $Id: FormLogin.cs,v 1.5 2007/02/26 18:41:17 gdavidson Exp $

#endregion
