// $Id: PacsConnectionDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using vxUI = Viatronix.v3D.UI;
using vxDicom = Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Dicom.UI
{
  /// <summary>
	/// Summary description for Form1.
	/// </summary>
  public class PacsConnectionDialog : vxUI.Dialog
  {

    #region fields

    /// <summary>
    /// Pacs Port Number
    /// </summary>
    private int m_pacsPort = 0;

    /// <summary>
    /// Pacs Host name
    /// </summary>
    private string m_pacsHostname = "";

    /// <summary>
    /// Pacs AE Title
    /// </summary>
    private Types.AETitle m_pacsAeTitle = new Types.AETitle();

    /// <summary>
    /// Our AE Title
    /// </summary>
    private Types.AETitle m_aeTitle = new Types.AETitle();

    private System.Windows.Forms.Label m_labelPacsAETitle;
    private System.Windows.Forms.Label m_labelPacsHostname;
    private System.Windows.Forms.Label m_labelPacsPort;
    private Viatronix.v3D.UI.Button m_buttonOk;
    private Viatronix.v3D.UI.Button m_buttonCancel;
    private Viatronix.v3D.UI.Button m_buttonTest;
    private System.Windows.Forms.Label m_labelOurAETitle;
    private System.Windows.Forms.GroupBox m_groupBoxPacs;
    private System.Windows.Forms.TextBox m_textBoxPacsAETitle;
    private System.Windows.Forms.TextBox m_textBoxPacsPort;
    private System.Windows.Forms.TextBox m_textBoxPacsHostname;
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.TextBox m_textBoxAETitle;
    private System.ComponentModel.IContainer components;

    #endregion

    #region initialization

    /// <summary>
    /// Constructor
    /// </summary>
    public PacsConnectionDialog()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      m_buttonOk.Enabled = false;

    }

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
    } // Dispose()

    #endregion

		#region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.m_labelPacsAETitle = new System.Windows.Forms.Label();
      this.m_labelPacsHostname = new System.Windows.Forms.Label();
      this.m_labelPacsPort = new System.Windows.Forms.Label();
      this.m_buttonOk = new Viatronix.v3D.UI.Button();
      this.m_buttonCancel = new Viatronix.v3D.UI.Button();
      this.m_buttonTest = new Viatronix.v3D.UI.Button();
      this.m_textBoxAETitle = new System.Windows.Forms.TextBox();
      this.m_labelOurAETitle = new System.Windows.Forms.Label();
      this.m_groupBoxPacs = new System.Windows.Forms.GroupBox();
      this.m_textBoxPacsHostname = new System.Windows.Forms.TextBox();
      this.m_textBoxPacsPort = new System.Windows.Forms.TextBox();
      this.m_textBoxPacsAETitle = new System.Windows.Forms.TextBox();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_groupBoxPacs.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_labelPacsAETitle
      // 
      this.m_labelPacsAETitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPacsAETitle.ForeColor = System.Drawing.Color.Navy;
      this.m_labelPacsAETitle.Location = new System.Drawing.Point(8, 24);
      this.m_labelPacsAETitle.Name = "m_labelPacsAETitle";
      this.m_labelPacsAETitle.Size = new System.Drawing.Size(88, 23);
      this.m_labelPacsAETitle.TabIndex = 0;
      this.m_labelPacsAETitle.Text = " AE Title:";
      // 
      // m_labelPacsHostname
      // 
      this.m_labelPacsHostname.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPacsHostname.ForeColor = System.Drawing.Color.Navy;
      this.m_labelPacsHostname.Location = new System.Drawing.Point(8, 56);
      this.m_labelPacsHostname.Name = "m_labelPacsHostname";
      this.m_labelPacsHostname.Size = new System.Drawing.Size(72, 23);
      this.m_labelPacsHostname.TabIndex = 1;
      this.m_labelPacsHostname.Text = "Hostname:";
      // 
      // m_labelPacsPort
      // 
      this.m_labelPacsPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPacsPort.ForeColor = System.Drawing.Color.Navy;
      this.m_labelPacsPort.Location = new System.Drawing.Point(8, 88);
      this.m_labelPacsPort.Name = "m_labelPacsPort";
      this.m_labelPacsPort.Size = new System.Drawing.Size(64, 23);
      this.m_labelPacsPort.TabIndex = 2;
      this.m_labelPacsPort.Text = "Port:";
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_buttonOk.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.m_buttonOk.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonOk.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonOk.Location = new System.Drawing.Point(360, 48);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.TabIndex = 10;
      this.m_buttonOk.Text = "Ok";
      this.m_toolTip.SetToolTip(this.m_buttonOk, "Press to use the settings in this dialog");
      this.m_buttonOk.Click += new System.EventHandler(this.OnOkClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                       null,
                                                                       null,
                                                                       null};
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonCancel.Location = new System.Drawing.Point(360, 80);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.TabIndex = 14;
      this.m_buttonCancel.Text = "Cancel";
      this.m_toolTip.SetToolTip(this.m_buttonCancel, "Press to cancel this configuration");
      // 
      // m_buttonTest
      // 
      this.m_buttonTest.CustomImages = new System.Drawing.Bitmap[] {
                                                                     null,
                                                                     null,
                                                                     null};
      this.m_buttonTest.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonTest.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonTest.Location = new System.Drawing.Point(360, 16);
      this.m_buttonTest.Name = "m_buttonTest";
      this.m_buttonTest.TabIndex = 8;
      this.m_buttonTest.Text = "Test";
      this.m_toolTip.SetToolTip(this.m_buttonTest, "Press to test the connections settings");
      this.m_buttonTest.Click += new System.EventHandler(this.OnTestClick);
      // 
      // m_textBoxAETitle
      // 
      this.m_textBoxAETitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxAETitle.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxAETitle.Location = new System.Drawing.Point(120, 16);
      this.m_textBoxAETitle.Name = "m_textBoxAETitle";
      this.m_textBoxAETitle.Size = new System.Drawing.Size(216, 20);
      this.m_textBoxAETitle.TabIndex = 0;
      this.m_textBoxAETitle.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxAETitle, "Enter our AE Title");
      this.m_textBoxAETitle.Leave += new System.EventHandler(this.OnAETitleLeave);
      // 
      // m_labelOurAETitle
      // 
      this.m_labelOurAETitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelOurAETitle.ForeColor = System.Drawing.Color.Navy;
      this.m_labelOurAETitle.Location = new System.Drawing.Point(16, 16);
      this.m_labelOurAETitle.Name = "m_labelOurAETitle";
      this.m_labelOurAETitle.Size = new System.Drawing.Size(80, 23);
      this.m_labelOurAETitle.TabIndex = 10;
      this.m_labelOurAETitle.Text = "AE Title:";
      // 
      // m_groupBoxPacs
      // 
      this.m_groupBoxPacs.Controls.Add(this.m_textBoxPacsHostname);
      this.m_groupBoxPacs.Controls.Add(this.m_textBoxPacsPort);
      this.m_groupBoxPacs.Controls.Add(this.m_textBoxPacsAETitle);
      this.m_groupBoxPacs.Controls.Add(this.m_labelPacsAETitle);
      this.m_groupBoxPacs.Controls.Add(this.m_labelPacsHostname);
      this.m_groupBoxPacs.Controls.Add(this.m_labelPacsPort);
      this.m_groupBoxPacs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_groupBoxPacs.ForeColor = System.Drawing.Color.Navy;
      this.m_groupBoxPacs.Location = new System.Drawing.Point(8, 48);
      this.m_groupBoxPacs.Name = "m_groupBoxPacs";
      this.m_groupBoxPacs.Size = new System.Drawing.Size(344, 120);
      this.m_groupBoxPacs.TabIndex = 11;
      this.m_groupBoxPacs.TabStop = false;
      this.m_groupBoxPacs.Text = "Pacs Connection Settings";
      // 
      // m_textBoxPacsHostname
      // 
      this.m_textBoxPacsHostname.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxPacsHostname.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxPacsHostname.Location = new System.Drawing.Point(112, 56);
      this.m_textBoxPacsHostname.Name = "m_textBoxPacsHostname";
      this.m_textBoxPacsHostname.Size = new System.Drawing.Size(216, 20);
      this.m_textBoxPacsHostname.TabIndex = 4;
      this.m_textBoxPacsHostname.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxPacsHostname, "Enter the PACS host name or IP address");
      // 
      // m_textBoxPacsPort
      // 
      this.m_textBoxPacsPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxPacsPort.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxPacsPort.Location = new System.Drawing.Point(112, 88);
      this.m_textBoxPacsPort.Name = "m_textBoxPacsPort";
      this.m_textBoxPacsPort.Size = new System.Drawing.Size(88, 20);
      this.m_textBoxPacsPort.TabIndex = 6;
      this.m_textBoxPacsPort.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxPacsPort, "Enter the PACS Quser/Retreive listening port");
      // 
      // m_textBoxPacsAETitle
      // 
      this.m_textBoxPacsAETitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxPacsAETitle.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxPacsAETitle.Location = new System.Drawing.Point(112, 24);
      this.m_textBoxPacsAETitle.Name = "m_textBoxPacsAETitle";
      this.m_textBoxPacsAETitle.Size = new System.Drawing.Size(216, 20);
      this.m_textBoxPacsAETitle.TabIndex = 2;
      this.m_textBoxPacsAETitle.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxPacsAETitle, "Enter the PACS AE Title");
      this.m_textBoxPacsAETitle.Leave += new System.EventHandler(this.OnPacsAETitleLeave);
      // 
      // PacsConnectionDialog
      // 
      this.AcceptButton = this.m_buttonOk;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(450, 176);
      this.Controls.Add(this.m_groupBoxPacs);
      this.Controls.Add(this.m_labelOurAETitle);
      this.Controls.Add(this.m_textBoxAETitle);
      this.Controls.Add(this.m_buttonTest);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonOk);
      this.Name = "PacsConnectionDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Pacs Connection";
      this.m_groupBoxPacs.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion


    #region properties

    /// <summary>
    /// Set/Returns the pacs host name
    /// </summary>
    public string PacsHostname 
    {
      get { return m_pacsHostname; }
      set { m_pacsHostname = value; }
    }

    /// <summary>
    /// Set/Returns the pacs AE Title
    /// </summary>
    public Types.AETitle PacsAETitle
    {
      get { return m_pacsAeTitle; }
      set { m_pacsAeTitle = value; }
    }

    /// <summary>
    /// Set/Gets out AE Title
    /// </summary>
    public Types.AETitle AETitle
    {
      get { return m_aeTitle; }
      set { m_aeTitle = value; }
    }

    /// <summary>
    /// Set/Gets the pacs Port number
    /// </summary>
    public int PacsPort
    {
      get { return m_pacsPort; }
      set { m_pacsPort = value; }
    }

    #endregion

    #region events handlers

    /// <summary>
    /// On Click event ofr the OK button
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnOkClick(object sender, System.EventArgs e)
    {
      m_pacsHostname = m_textBoxPacsHostname.Text;
      m_pacsAeTitle.Value = m_textBoxPacsAETitle.Text;
      m_aeTitle.Value = m_textBoxAETitle.Text;

      m_pacsPort = 0;

      if ( m_textBoxPacsPort.Text != "" )
      {
        m_pacsPort = System.Convert.ToInt32(m_textBoxPacsPort.Text);
      }
    
    } // OnOkClick()


    /// <summary>
    /// On Click event for theTest button
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTestClick(object sender, System.EventArgs e)
    {

      if ( m_textBoxPacsHostname.Text == "" )
      {
        vxUI.MessageBox.ShowError(this,"The PACS host name must be filled in");
        return;
      }

      if ( m_textBoxPacsAETitle.Text == "" )
      {
        vxUI.MessageBox.ShowError(this,"The PACS AE Title must be filled in");
        return;
      }


      if ( m_textBoxPacsPort.Text == "" )
      {
        vxUI.MessageBox.ShowError(this,"The PACS port must be filled in");
        return;
      }

      PacsClient pacs = null;

      if ( m_textBoxAETitle.Text == "" )
      {
        pacs = new PacsClient(new Host(m_textBoxPacsHostname.Text, System.Convert.ToInt32(m_textBoxPacsPort.Text), m_textBoxPacsAETitle.Text));
      }
      else
      {
        pacs = new PacsClient(new Types.AETitle(m_textBoxAETitle.Text), new Host(m_textBoxPacsHostname.Text, System.Convert.ToInt32(m_textBoxPacsPort.Text), m_textBoxPacsAETitle.Text));
      }
    
      try
      {
        pacs.Connect();
      }
      catch ( Exception ex )
      {
        vxUI.MessageBox.ShowError(this,ex);
        return;
      }

      try
      {
        pacs.SendEcho();
      }
      catch ( Exception ex )
      {
        vxUI.MessageBox.ShowError(this,ex);
        return;
      }

      m_buttonOk.Enabled = true;

    } // OnTestClick()

    /// <summary>
    /// On text changed event 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTextChanged(object sender, System.EventArgs e)
    {
      m_buttonOk.Enabled = false;
    } // OnTextChanged()


    /// <summary>
    /// Validates the AE Title upon leaving the field
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnAETitleLeave(object sender, System.EventArgs e)
    {
      try
      {
        Types.AETitle aeTitle = new Types.AETitle(m_textBoxAETitle.Text);
      }
      catch ( Exception )
      {
        vxUI.MessageBox.ShowError(this,"Invalid AE Title entered, please re-enter");
        m_textBoxAETitle.Text = "";
        m_textBoxAETitle.Focus();
      }
    } // OnAETitleLeave()

    /// <summary>
    /// Validates the AE Title upon leaving
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnPacsAETitleLeave(object sender, System.EventArgs e)
    {
      try
      {
        Types.AETitle aeTitle = new Types.AETitle(m_textBoxPacsAETitle.Text);
      }
      catch ( Exception )
      {
        vxUI.MessageBox.ShowError(this,"Invalid AE Title entered, please re-enter");
        m_textBoxPacsAETitle.Text = "";
        m_textBoxPacsAETitle.Focus();
      }

    } // OnPacsAETitleLeave()


    #endregion 



	}

}


#region revision

// $Log: PacsConnectionDialog.cs,v $
// Revision 1.5  2005/11/23 11:50:49  mkontak
// Using new MessageBox implementation
//
// Revision 1.4  2005/10/25 15:00:24  mkontak
// Changed int to uint
//
// Revision 1.3  2005/10/11 14:43:05  mkontak
// Added docking
//
// Revision 1.2  2004/07/15 18:04:40  mkontak
// Changes to make better viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:37  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/10/17 18:33:40  mkontak
// Dicom update
//
// Revision 1.3  2003/10/07 18:39:50  mkontak
// New DICOM changes
//
// Revision 1.2  2003/09/18 02:03:41  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/PacsConnectionDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $
// $Id: PacsConnectionDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $

#endregion