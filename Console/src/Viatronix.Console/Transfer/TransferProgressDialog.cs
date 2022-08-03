// $Id: TransferProgressDialog.cs,v 1.3.2.1 2008/10/03 19:35:59 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using Viatronix.v3D.Core;

namespace Viatronix.Console
{
	/// <summary>
	/// Summary description for TransferProgressDialog.
	/// </summary>
	public class TransferProgressDialog : Viatronix.UI.Dialog
	{

    #region fields


    /// <summary>
    /// Abort flag
    /// </summary>
    bool m_abort = false;

    /// <summary>
    /// Pulse flag
    /// </summary>
    bool m_pulse = false;

    /// <summary>
    /// Pulse Generator
    /// </summary>
    Viatronix.UI.PulseGenerator m_pulseGenerator = null;

    /// <summary>
    /// Cancel button object
    /// </summary>
    private Viatronix.UI.Button m_cancel;
    private Viatronix.UI.ProgressBar2 m_progressBar;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    #endregion

    #region construction


    /// <summary>
    /// default constructor
    /// </summary>
    public TransferProgressDialog( bool pulse )
    {
      InitializeComponent();

      this.DialogResult = System.Windows.Forms.DialogResult.OK;


      this.CancelButton = this.m_cancel;

      m_pulse = pulse;

      if( m_pulse )
      {
        m_pulseGenerator = new Viatronix.UI.PulseGenerator(m_progressBar);

        m_pulseGenerator.StartPulse();
         
        m_progressBar.Pulse = true;
      }

      m_progressBar.Value   = 0;
      m_progressBar.Maximum = 100;

    }//TransferProgressDialog( )

    #endregion

    #region override methods

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

    }//Dispose( bool disposing )

    #endregion

    #region properties

    /// <summary>
    /// Gets the Abort flag
    /// </summary>
    public bool Abort
    {
      get { return m_abort; }
    }


    #endregion

    #region methods


    /// <summary>
    /// Sets the Progress Information on the Dialog 
    /// </summary>
    /// <param name="text">string</param>
    /// <param name="progress">int</param>
    /// <param name="total">int</param>
    public void SetProgressInformation(string text, int progress, int total)
    {
      m_progressBar.Text    = text;
      m_progressBar.Maximum = total;
      m_progressBar.Value = progress;
    }//SetProgressInformation( string text, int progress, int total )


    #endregion


#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_cancel = new Viatronix.UI.Button();
      this.m_progressBar = new Viatronix.UI.ProgressBar2();
      this.SuspendLayout();
      // 
      // m_cancel
      // 
      this.m_cancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancel.ForeColor = System.Drawing.Color.Black;
      this.m_cancel.Location = new System.Drawing.Point(112, 48);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.Size = new System.Drawing.Size(64, 23);
      this.m_cancel.TabIndex = 1;
      this.m_cancel.Text = "Cancel";
      this.m_cancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // m_progressBar
      // 
      this.m_progressBar.BackColor = System.Drawing.Color.Transparent;
      this.m_progressBar.DrawText = true;
      this.m_progressBar.Location = new System.Drawing.Point(16, 19);
      this.m_progressBar.Maximum = 100;
      this.m_progressBar.Minimum = 0;
      this.m_progressBar.Name = "m_progressBar";
      this.m_progressBar.Pulse = false;
      this.m_progressBar.Size = new System.Drawing.Size(262, 23);
      this.m_progressBar.Step = 1;
      this.m_progressBar.TabIndex = 2;
      this.m_progressBar.Value = 0;
      // 
      // TransferProgressDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(290, 78);
      this.Controls.Add(this.m_progressBar);
      this.Controls.Add(this.m_cancel);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "TransferProgressDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Transferring...";
      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.OnClosed);
      this.Controls.SetChildIndex(this.m_cancel, 0);
      this.Controls.SetChildIndex(this.m_progressBar, 0);
      this.ResumeLayout(false);

    }
		#endregion

    private void OnCancelClick(object sender, EventArgs e)
    {
      m_abort = true;
      this.DialogResult = DialogResult.Cancel;
      if ( m_pulse ) m_pulseGenerator.StartPulse();
    }

    private void OnClosed(object sender, FormClosedEventArgs e)
    {
      if (e.CloseReason == CloseReason.ApplicationExitCall)
        m_abort = true;
      else if ( ! m_abort )
        this.DialogResult = DialogResult.OK;

      if ( m_pulse ) m_pulseGenerator.StartPulse();

    }

 

	} // class TransferProgressDialog
} // namespace Viatronix.Console

#region revision history

// $Log: TransferProgressDialog.cs,v $
// Revision 1.3.2.1  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.3  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.2  2006/07/25 20:27:51  mkontak
// Issue 4887: Lockup when clicking cancel if two discs are required
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.11  2005/10/29 02:34:04  romy
// no message
//
// Revision 1.10  2005/10/20 19:00:14  romy
// added proper close method.
//
// Revision 1.9  2005/10/20 15:27:51  romy
// abort modified
//
// Revision 1.8  2005/10/13 19:48:54  romy
// Fixed the issue with resetting Max value
//
// Revision 1.7  2005/09/30 17:13:09  romy
// Transfer cancel method added
//
// Revision 1.6  2005/09/26 15:54:09  romy
// new constructor added
//
// Revision 1.5  2005/09/20 12:21:25  romy
// demo checkin
//
// Revision 1.4  2005/09/19 12:32:44  romy
// Updated Text and removed min/max buttons
//
// Revision 1.3  2005/09/12 12:51:54  gdavidson
// Modified the dialog to accept a transfer manager object.
//
// Revision 1.2  2005/09/03 01:02:25  romy
// revisited transfer
//
// Revision 1.1  2005/09/02 13:20:35  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferProgressDialog.cs,v 1.3.2.1 2008/10/03 19:35:59 kchalupa Exp $
// $Id: TransferProgressDialog.cs,v 1.3.2.1 2008/10/03 19:35:59 kchalupa Exp $

#endregion
