// $Id: PacsSelectionDialog.cs,v 1.3 2007/03/08 21:56:13 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
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

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Implements a pacs selection dialog for selection the pacs avaiable
	/// </summary>
	public class PacsSelectionDialog : Viatronix.v3D.UI.Dialog
	{


    #region fields

    /// <summary>
    /// Cancel button
    /// </summary>
    private Viatronix.v3D.UI.Button m_buttonCancel;

    /// <summary>
    /// OK Button
    /// </summary>
    private Viatronix.v3D.UI.Button m_buttonOk;

    /// <summary>
    /// Selected server
    /// </summary>
    private Host m_selectedServer;

    /// <summary>
    /// Pacs selection control
    /// </summary>
    private Viatronix.v3D.Dicom.UI.PacsSelectionControl m_pacsSelectionControl;


    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    #endregion


    #region constructor

    /// <summary>
    /// Constructor
    /// </summary>
		public PacsSelectionDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      m_pacsSelectionControl.ServerSelected += new EventHandler(OnSelectedServerHandler);
    } // PacsSelectionDialog()

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
    /// <param name="disposing">disposing flag</param>
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
    } // Dispose( bool disposing )

    #endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_buttonCancel = new Viatronix.v3D.UI.Button();
      this.m_buttonOk = new Viatronix.v3D.UI.Button();
      this.m_pacsSelectionControl = new Viatronix.v3D.Dicom.UI.PacsSelectionControl();
      this.SuspendLayout();
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                       null,
                                                                       null,
                                                                       null};
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonCancel.Location = new System.Drawing.Point(192, 56);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.TabIndex = 3;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClickHandler);
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_buttonOk.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonOk.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonOk.Location = new System.Drawing.Point(80, 56);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.OldStockButton = false;
      this.m_buttonOk.TabIndex = 4;
      this.m_buttonOk.Text = "Ok";
      this.m_buttonOk.Click += new System.EventHandler(this.OnOkClickHandler);
      // 
      // m_pacsSelectionControl
      // 
      this.m_pacsSelectionControl.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_pacsSelectionControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_pacsSelectionControl.ForeColor = System.Drawing.Color.Navy;
      this.m_pacsSelectionControl.Location = new System.Drawing.Point(8, 16);
      this.m_pacsSelectionControl.Name = "m_pacsSelectionControl";
      this.m_pacsSelectionControl.ServiceType = "Any";
      this.m_pacsSelectionControl.Size = new System.Drawing.Size(328, 23);
      this.m_pacsSelectionControl.TabIndex = 5;
      // 
      // PacsSelectionDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(346, 88);
      this.Controls.Add(this.m_pacsSelectionControl);
      this.Controls.Add(this.m_buttonOk);
      this.Controls.Add(this.m_buttonCancel);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "PacsSelectionDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "PACS Selection";
      this.Controls.SetChildIndex(this.m_buttonCancel, 0);
      this.Controls.SetChildIndex(this.m_buttonOk, 0);
      this.Controls.SetChildIndex(this.m_pacsSelectionControl, 0);
      this.ResumeLayout(false);

    }
		#endregion


    #region event handlers

    /// <summary>
    /// Handles the selected server event
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">evenet args</param>
    private void OnSelectedServerHandler(object sender, System.EventArgs e)
    {
      m_selectedServer = m_pacsSelectionControl.SelectedServer;
    } // OnSelectedServerHandler(object sender, System.EventArgs e)

    /// <summary>
    /// Handles the ok click event
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">evenet args</param>
    private void OnOkClickHandler(object sender, System.EventArgs e)
    {
      m_selectedServer = m_pacsSelectionControl.SelectedServer;
      this.DialogResult = DialogResult.OK;
      Close();
    } //  OnOkClickHandler(object sender, System.EventArgs e)

    /// <summary>
    /// Handles the cancel click event
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">evenet args</param>
    private void OnCancelClickHandler(object sender, System.EventArgs e)
    {
      m_selectedServer = new Host();
      this.DialogResult = DialogResult.Cancel;
      Close();
    } // OnCancelClickHandler(object sender, System.EventArgs e)
 
    #endregion

  } // class PacsSelectionDialog

} // namespace Viatronix.v3D.Dicom.UI


#region revision history

// $Log: PacsSelectionDialog.cs,v $
// Revision 1.3  2007/03/08 21:56:13  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/PacsSelectionDialog.cs,v 1.3 2007/03/08 21:56:13 mkontak Exp $
// $Id: PacsSelectionDialog.cs,v 1.3 2007/03/08 21:56:13 mkontak Exp $

#endregion
