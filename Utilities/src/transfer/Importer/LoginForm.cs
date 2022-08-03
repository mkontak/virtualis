// $Id: LoginForm.cs,v 1.1.2.3 2009/06/04 18:50:38 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using Viatronix.Encryption;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Allows the user to login.
  /// </summary>
  public partial class LoginForm : Viatronix.UI.Dialog
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public LoginForm()
    {
      InitializeComponent();
    } // LoginForm()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the username.
    /// </summary>
    public string Username
    {
      get { return m_usernameText.Text; }
    } // Username


    /// <summary>
    /// Gets or sets the password.
    /// </summary>
    public string Password
    {
      get { return m_passwordText.Text; }
    } // Password

    #endregion

  } // LoginForm
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: LoginForm.cs,v $
// Revision 1.1.2.3  2009/06/04 18:50:38  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/LoginForm.cs,v 1.1.2.3 2009/06/04 18:50:38 kchalupa Exp $
// $Id: LoginForm.cs,v 1.1.2.3 2009/06/04 18:50:38 kchalupa Exp $

#endregion
