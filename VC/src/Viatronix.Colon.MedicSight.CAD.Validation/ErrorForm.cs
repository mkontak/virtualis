// $Id: ErrorForm.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
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
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Shows the errors that occured in validation.
  /// </summary>
  public partial class ErrorForm : Form
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ErrorForm()
    {
      InitializeComponent();
    } // ErrorForm()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the error text that will be displayed.
    /// </summary>
    public string ErrorText
    {
      get { return m_errorTextBox.Text;  }
      set { m_errorTextBox.Text = value; }
    } // ErrorText

    #endregion

  } // ErrorForm
} // namespace Viatronix.v3D.Utilities.CadParser

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/ErrorForm.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: ErrorForm.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
