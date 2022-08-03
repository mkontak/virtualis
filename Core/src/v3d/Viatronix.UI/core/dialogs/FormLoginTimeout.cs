// $Id: FormLoginTimeout.cs,v 1.1 2006/10/30 15:43:19 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Configuration;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Xml;
using Viatronix.Core;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.UI
{
  /// <summary>
  /// About Box for application. Displays information about the active plugin.
  /// </summary>
	public class FormLoginTimeout : Viatronix.UI.FormLogin
	{

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public FormLoginTimeout(ISecurityProvider securityProvider ) : base(securityProvider)
    {
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      this.AllowClose = true;

      /// Set the default messages
      m_messageLine1.Text = "Only vxservice (Viatronix Service)";
      m_messageLine2.Text = "can unlock this application.";

    } // FormLoginTimeout()

    #endregion

    #region methods

    /// <summary>
    /// Login opertation.
    /// </summary>
    /// <param name="username">user name</param>
    /// <param name="password">password</param>
    protected override void Login( string username, string password )
    {
      try
      {
        if (username == null || username == string.Empty)
        {
          throw new Exception("The username cannot be empty.");
        }
        else if (password == null || password == string.Empty)
        {
          throw new Exception("The password cannot be empty.");
        }

        User user = this.SecurityProvider.Login( username, password );

        if( user != null && (( user.Login == this.User.Login ) || user.IsAdministrator || user.IsService ))
          this.DialogResult = DialogResult.OK;
        else
        {
          Viatronix.UI.MessageBox.ShowError( "The login information is incorrect. Please try again." );
          m_textBoxLogin.Focus();
          m_textBoxLogin.SelectAll();
        }
      }
      catch( Exception ex)
      {
        Logging.Log.Error("Login failed : " + ex.Message, "FormLoginTimeout", "Login");

        Viatronix.UI.MessageBox.ShowError( "The login name or password is incorrect. Please try again" );
        m_textBoxLogin.Focus();
        m_textBoxLogin.SelectAll();
        m_textBoxPassword.Clear();
      }
    } // Login( username, password )


    /// <summary>
    /// Resets the fields on the login form.
    /// </summary>
    public override void Initialize()
    {

      // =============================================================================================
      // Added this check because if the Console is left open without logging in it will still
      // timeout and since there is no user yet we get a NullReference exception. It should be that 
      // the timeout timer is not started until after the login.
      // =============================================================================================
      if (this.User != null)
      {
        this.UserName = this.User.Login;
        this.FullName = this.User.Name;
      }
      else
      {
        this.UserName = string.Empty;
        this.FullName = string.Empty;
      }

      m_textBoxPassword.Clear();
    } // ResetLogin()


    /// <summary>
    /// Handles the dialog Closing down.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnClosing(CancelEventArgs args)
    {
      base.OnClosing(args);

      // Make sure we don't overwrite an ok.
      if (this.DialogResult != System.Windows.Forms.DialogResult.OK)
      {
        this.DialogResult = System.Windows.Forms.DialogResult.Abort;
      }
    } // OnClosing( args )


    /// <summary>
    /// Handles the close hyperlink being clicked.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnCloseLinkClicked(LinkLabelLinkClickedEventArgs args)
    {
      DialogResult = System.Windows.Forms.DialogResult.Abort;
    } // OnCloseLinkClicked( args )

    #endregion
        
  } // class FormLoginTimeout
} // namespace Viatronix.UI


