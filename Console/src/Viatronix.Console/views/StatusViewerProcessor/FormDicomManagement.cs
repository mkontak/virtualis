// $Id: FormDicomManagement.cs,v 1.4 2007/03/07 22:17:54 gdavidson Exp $ 
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
using System.ComponentModel;
using System.Windows.Forms;



namespace Viatronix.Console
{
	/// <summary>
	/// FormDicomManagement displays the DICOM Server parameters for the user for management.
	/// </summary>
	public class DicomServerManager :  Viatronix.UI.Dialog
	{
    
    #region fields

    /// <summary>
    /// CheckBox Object for the Receiving Mode
    /// </summary>
    private Viatronix.UI.CheckBox    m_receivingMode;

    /// <summary>
    /// Ok Button object
    /// </summary>
    private Viatronix.UI.Button      m_ok;

    /// <summary>
    /// Cancel Button Object
    /// </summary>
    private Viatronix.UI.Button      m_cancel;

    /// <summary>
    /// Label object for System
    /// </summary>
    private System.Windows.Forms.Label   m_labelSystem;

    /// <summary>
    /// Label object for Receiving Mode
    /// </summary>
    private System.Windows.Forms.Label   m_labelRecMode;

    /// <summary>
    /// Label object for Port
    /// </summary>
    private System.Windows.Forms.Label m_labelPort;

    /// <summary>
    /// Text Box object for Port
    /// </summary>
    private System.Windows.Forms.TextBox m_textPort;

    /// <summary>
    /// Label for AE Title
    /// </summary>
    private System.Windows.Forms.Label m_aeTitleLabel;

    /// <summary>
    /// Text box for AE Title
    /// </summary>
    private System.Windows.Forms.TextBox m_aeTitle;

    /// <summary>
    /// Text box for system
    /// </summary>
    private System.Windows.Forms.TextBox m_system;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    #endregion fields

    #region constructors and finalizers

    /// <summary>
    /// Dicom Server Manager Constructor
    /// </summary>
    /// <param name="dicomServerProperties">Dicom server properties</param>
    /// <param name="AutoMode">Automatic mode flag</param>
    public DicomServerManager(DicomServerProperties dicomServerProperties, bool AutoMode)
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //Get and Set the Receiving Mode from the DCM Server .
      if (AutoMode)
        m_receivingMode.Checked = true;
      else
        m_receivingMode.Checked = false;

      m_system.Text = dicomServerProperties.ServerName;
      m_aeTitle.Text = dicomServerProperties.AETitle;
      m_textPort.Text = dicomServerProperties.ListeningPort.ToString();
 
    } // DicomServerManager( DicomServerProperties dicomServerProperties, bool AutoMode )


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
    }//Dispose( bool disposing )

    #endregion

    #region properties

    /// <summary>
    /// Returns the receiving Mode
    /// </summary>
    public bool AutomaticMode
    {
      get { return m_receivingMode.Checked ?  true : false ; }
    } // AutomaticMode


    #endregion properties

    #region event handlers

    /// <summary>
    /// Handles the OK button click
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnOKClickEventHandler(object sender, System.EventArgs args)
    {
      this.DialogResult = DialogResult.OK;
      this.Close();
    }//OnOK(object sender, System.EventArgs args)


    /// <summary>
    /// Handles the Cancel Button click
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCancelClickEventHandler(object sender, System.EventArgs args)
    {
      this.DialogResult = DialogResult.Cancel;
      this.Dispose();
    }//OnCancel(object sender, System.EventArgs args)

    #endregion

    #region Windows Form Designer generated code
    /// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_receivingMode = new Viatronix.UI.CheckBox();
      this.m_ok = new Viatronix.UI.Button();
      this.m_cancel = new Viatronix.UI.Button();
      this.m_labelSystem = new System.Windows.Forms.Label();
      this.m_aeTitleLabel = new System.Windows.Forms.Label();
      this.m_labelPort = new System.Windows.Forms.Label();
      this.m_system = new System.Windows.Forms.TextBox();
      this.m_textPort = new System.Windows.Forms.TextBox();
      this.m_labelRecMode = new System.Windows.Forms.Label();
      this.m_aeTitle = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // m_receivingMode
      // 
      this.m_receivingMode.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_receivingMode.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_receivingMode.Location = new System.Drawing.Point(114, 101);
      this.m_receivingMode.Name = "m_receivingMode";
      this.m_receivingMode.Size = new System.Drawing.Size(104, 24);
      this.m_receivingMode.TabIndex = 1;
      this.m_receivingMode.Text = "Automatic Mode";
      // 
      // m_ok
      // 
      this.m_ok.Checked = false;
      this.m_ok.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_ok.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_ok.Location = new System.Drawing.Point(158, 134);
      this.m_ok.Name = "m_ok";
      this.m_ok.OldStockButton = false;
      this.m_ok.Size = new System.Drawing.Size(60, 20);
      this.m_ok.TabIndex = 2;
      this.m_ok.Text = "Update";
      this.m_ok.ToggleButton = false;
      this.m_ok.Click += new System.EventHandler(this.OnOKClickEventHandler);
      // 
      // m_cancel
      // 
      this.m_cancel.Checked = false;
      this.m_cancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancel.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_cancel.Location = new System.Drawing.Point(224, 134);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.Size = new System.Drawing.Size(60, 20);
      this.m_cancel.TabIndex = 3;
      this.m_cancel.Text = "Cancel";
      this.m_cancel.ToggleButton = false;
      this.m_cancel.Click += new System.EventHandler(this.OnCancelClickEventHandler);
      // 
      // m_labelSystem
      // 
      this.m_labelSystem.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelSystem.Location = new System.Drawing.Point(6, 18);
      this.m_labelSystem.Name = "m_labelSystem";
      this.m_labelSystem.Size = new System.Drawing.Size(87, 14);
      this.m_labelSystem.TabIndex = 4;
      this.m_labelSystem.Text = "System:";
      // 
      // m_aeTitleLabel
      // 
      this.m_aeTitleLabel.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_aeTitleLabel.Location = new System.Drawing.Point(6, 44);
      this.m_aeTitleLabel.Name = "m_aeTitleLabel";
      this.m_aeTitleLabel.Size = new System.Drawing.Size(100, 16);
      this.m_aeTitleLabel.TabIndex = 13;
      this.m_aeTitleLabel.Text = "AE Title:";
      // 
      // m_labelPort
      // 
      this.m_labelPort.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelPort.Location = new System.Drawing.Point(6, 72);
      this.m_labelPort.Name = "m_labelPort";
      this.m_labelPort.Size = new System.Drawing.Size(87, 13);
      this.m_labelPort.TabIndex = 6;
      this.m_labelPort.Text = "Port:";
      // 
      // m_system
      // 
      this.m_system.Location = new System.Drawing.Point(114, 16);
      this.m_system.Name = "m_system";
      this.m_system.ReadOnly = true;
      this.m_system.Size = new System.Drawing.Size(165, 20);
      this.m_system.TabIndex = 14;
      // 
      // m_textPort
      // 
      this.m_textPort.Location = new System.Drawing.Point(114, 72);
      this.m_textPort.Name = "m_textPort";
      this.m_textPort.ReadOnly = true;
      this.m_textPort.Size = new System.Drawing.Size(165, 20);
      this.m_textPort.TabIndex = 9;
      // 
      // m_labelRecMode
      // 
      this.m_labelRecMode.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelRecMode.Location = new System.Drawing.Point(6, 101);
      this.m_labelRecMode.Name = "m_labelRecMode";
      this.m_labelRecMode.Size = new System.Drawing.Size(87, 23);
      this.m_labelRecMode.TabIndex = 10;
      this.m_labelRecMode.Text = "Receiving Mode:";
      // 
      // m_aeTitle
      // 
      this.m_aeTitle.Location = new System.Drawing.Point(114, 44);
      this.m_aeTitle.Name = "m_aeTitle";
      this.m_aeTitle.ReadOnly = true;
      this.m_aeTitle.Size = new System.Drawing.Size(165, 20);
      this.m_aeTitle.TabIndex = 15;
      // 
      // DicomServerManager
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(288, 159);
      this.Controls.Add(this.m_aeTitle);
      this.Controls.Add(this.m_labelRecMode);
      this.Controls.Add(this.m_textPort);
      this.Controls.Add(this.m_aeTitleLabel);
      this.Controls.Add(this.m_system);
      this.Controls.Add(this.m_labelPort);
      this.Controls.Add(this.m_labelSystem);
      this.Controls.Add(this.m_cancel);
      this.Controls.Add(this.m_ok);
      this.Controls.Add(this.m_receivingMode);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "DicomServerManager";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "DICOM Server Property";
      this.Controls.SetChildIndex(this.m_receivingMode, 0);
      this.Controls.SetChildIndex(this.m_ok, 0);
      this.Controls.SetChildIndex(this.m_cancel, 0);
      this.Controls.SetChildIndex(this.m_labelSystem, 0);
      this.Controls.SetChildIndex(this.m_labelPort, 0);
      this.Controls.SetChildIndex(this.m_system, 0);
      this.Controls.SetChildIndex(this.m_aeTitleLabel, 0);
      this.Controls.SetChildIndex(this.m_textPort, 0);
      this.Controls.SetChildIndex(this.m_labelRecMode, 0);
      this.Controls.SetChildIndex(this.m_aeTitle, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion

  } // class DicomServerManager
} // namespace Viatronix.Console.views

#region revision history

// $Log: FormDicomManagement.cs,v $
// Revision 1.4  2007/03/07 22:17:54  gdavidson
// Code review
//
// Revision 1.3  2007/03/06 16:37:50  mkontak
// Coding standards
//
// Revision 1.2  2007/03/06 15:30:32  mkontak
// Coding standards
//
// Revision 1.1  2006/10/11 13:13:59  vxconfig
// modified filter name
//
// Revision 1.8  2006/05/04 19:19:06  mkontak
// Removed unused label
//
// Revision 1.7  2006/03/10 20:07:46  romy
// GUI Doctor made a quick visit
//
// Revision 1.6  2006/02/23 21:14:07  romy
// spell checks and GUI Standards
//
// Revision 1.5  2006/02/17 15:04:44  romy
// GUI redesigned
//
// Revision 1.3  2006/02/15 13:58:38  romy
// modified the font and color
//
// Revision 1.2  2005/10/11 13:46:59  romy
// Added DCMServer Params object as arg
//
// Revision 1.1  2005/08/05 14:20:01  romy
// Rearranged Files
//
// Revision 1.4  2005/06/08 17:17:06  romy
// Removed Test Code
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/StatusViewerProcessor/FormDicomManagement.cs,v 1.4 2007/03/07 22:17:54 gdavidson Exp $
// $Id: FormDicomManagement.cs,v 1.4 2007/03/07 22:17:54 gdavidson Exp $

#endregion

