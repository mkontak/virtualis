// $Id: SoftwareAgreementForm.cs,v 1.3 2007/03/01 14:54:19 gdavidson Exp $
//
// Copyright(c) 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille(mailto:frank@viatronix.com)

#region usings

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Configuration;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.Text;

#endregion

namespace Viatronix.UI
{
  public class SoftwareAgreementForm : Viatronix.UI.Dialog
  {
    #region fields

    /// <summary>
    /// The message that is displayed in the text box
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// The text box that displays the agreement
    /// </summary>
    private System.Windows.Forms.RichTextBox m_textBox = null;

    /// <summary>
    /// The accept button
    /// </summary>
    private Viatronix.UI.Button m_acceptButton = null;

    /// <summary>
    /// The decline button
    /// </summary>
    private Viatronix.UI.Button m_declineButton = null;

    /// <summary>
    /// Default button width
    /// </summary>
    static private readonly Size m_buttonSize = new Size( 60, 25 );

    /// <summary>
    /// The text on the accept button
    /// </summary>
    static private string m_acceptText = "I Accept";

    /// <summary>
    /// The text on the decline button
    /// </summary>
    static private string m_declineText = "I Decline";

    /// <summary>
    /// Width of the control
    /// </summary>
    private static int m_overallWidth = 400;

    /// <summary>
    /// Height of the control
    /// </summary>
    private static int m_overallHeight = 400;

    /// <summary>
    /// The space around various elements
    /// </summary>
    private static int m_padding = 10;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the message that is displayed in the text box
    /// </summary>
    public string Message
    {
      get { return m_message; }
      set
      {
        m_message = value;
        m_textBox.Text = value;
      }
    } // Message

    #endregion

    #region initialization

    /// <summary>
    /// constructor
    /// </summary>
    public SoftwareAgreementForm()
    {
      InitializeComponent();

      CreateButtons();
      this.CancelButton = m_declineButton;

      FileInfo fileInfo = new FileInfo( Assembly.GetEntryAssembly().Location );
      string filename = Path.Combine( fileInfo.DirectoryName, ConfigurationManager.AppSettings[ "SoftwareLicenseAgreement" ] );
      StreamReader reader = new StreamReader( filename );
      this.m_textBox.Rtf = reader.ReadToEnd();
      this.StartPosition = FormStartPosition.CenterScreen;
    } // SoftwareAgreementForm()


    /// <summary>
    /// Creates the buttons
    /// </summary>
    private void CreateButtons()
    {

      //
      // create the agree button
      //
      m_acceptButton = new Viatronix.UI.Button();

      // set the buttons properties
      m_acceptButton.Text = m_acceptText;
      m_acceptButton.DialogResult = DialogResult.Yes;
      m_acceptButton.Size = m_buttonSize;
      m_acceptButton.Font = new Font( "Arial", 8 );
      m_acceptButton.Location = new System.Drawing.Point( m_overallWidth / 2 - m_buttonSize.Width - m_padding, m_overallHeight - m_padding - m_buttonSize.Height );
      m_acceptButton.Click += new EventHandler( this.OnAcceptClickedHandler );

      // add the button to the form
      this.Controls.Add( m_acceptButton );

      //
      // create the decline button
      //
      m_declineButton = new Viatronix.UI.Button();

      // set the buttons properties
      m_declineButton.Text = m_declineText;
      m_declineButton.DialogResult = DialogResult.No;
      m_declineButton.Size = m_buttonSize;
      m_declineButton.Font = new Font( "Arial", 8 );
      m_declineButton.Location = new System.Drawing.Point( m_overallWidth / 2 + m_padding, m_overallHeight - m_padding - m_buttonSize.Height );
      m_declineButton.Click += new EventHandler( this.OnDeclineClickHandle );

      // add the button to the form
      this.Controls.Add( m_declineButton );

      //
      // create the software agreement text box
      //
      m_textBox = new System.Windows.Forms.RichTextBox();
      m_textBox.Multiline = true;
      m_textBox.Location = new System.Drawing.Point( m_padding, m_padding );
      m_textBox.Size = new System.Drawing.Size( m_overallWidth - 2 * m_padding, m_overallHeight - 3 * m_padding - m_buttonSize.Height );

      // add the text box to the form
      this.Controls.Add( m_textBox );

    } // CreateButtons()

    #endregion

    #region windows form designer generated code
		
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {

      // 
      // MessageBoxForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(m_overallWidth, m_overallHeight);
      this.ControlBox = false;
      this.Name = "Software License Agreement";
      this.Text = "Software License Agreement";

    }
		
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the accept button's click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnAcceptClickedHandler( object sender, EventArgs args )
    {
      this.DialogResult = DialogResult.Yes;
      this.Close();
    } // OnAcceptClickedHandler( sender, args )


    /// <summary>
    /// Handles the decline button's click handler
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDeclineClickHandle( object sender, EventArgs args )
    {
      this.DialogResult = DialogResult.No;
      this.Close();
    } // OnDeclineClickHandle( sender, args )

    #endregion

  } // class SoftwareAgreementForm
} // namespace Viatronix.UI

#region revision history

// $Log: SoftwareAgreementForm.cs,v $
// Revision 1.3  2007/03/01 14:54:19  gdavidson
// Commented code
//
// Revision 1.2  2005/11/18 21:08:03  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.2.2  2005/08/09 16:20:59  geconomos
// added default cancel button
//
// Revision 1.1.2.1  2005/08/08 19:36:49  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/SoftwareAgreementForm.cs,v 1.3 2007/03/01 14:54:19 gdavidson Exp $
// $Id: SoftwareAgreementForm.cs,v 1.3 2007/03/01 14:54:19 gdavidson Exp $

#endregion