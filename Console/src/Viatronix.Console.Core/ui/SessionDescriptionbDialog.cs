// $Id: SessionDescriptionbDialog.cs,v 1.1.2.5 2008/04/15 19:15:35 mkontak Exp $
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

namespace Viatronix.Console
{
	/// <summary>
	/// Summary description for SaveAsDialog.
	/// </summary>
	public class SessionDescriptionDialog : Viatronix.UI.Dialog
	{

    #region fields

    /// <summary>
    /// textbox for the description
    /// </summary>
    private System.Windows.Forms.TextBox m_description;

    /// <summary>
    /// label for the description
    /// </summary>
    private System.Windows.Forms.Label m_descriptionLabel;

    /// <summary>
    /// save as button
    /// </summary>
    private Viatronix.UI.Button m_continueButton;

    #endregion
    private ToolTip m_toolTip;
    private Viatronix.UI.Button m_buttonCancel;
    private IContainer components;

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
		public SessionDescriptionDialog()
		{
			InitializeComponent();
      this.StartPosition = ( Global.Frame != null && Global.Frame.Visible ) ? FormStartPosition.CenterParent : FormStartPosition.CenterScreen; 
		} // SaveAsDialog()

    #endregion

    #region properties
    

    /// <summary>
    /// Gets/Sets the allow cancel 
    /// </summary>
    public bool AllowCancel
    {
      get { return m_buttonCancel.Visible; }
      set { m_buttonCancel.Visible = value; }
    } // AllowCancel()

    /// <summary>
    /// Gets or sets the description
    /// </summary>
    public string Description
    {
      get
      {
        m_description.Text = m_description.Text.Replace( "\r", string.Empty );
        m_description.Text = m_description.Text.Replace( "\n", string.Empty );
        return m_description.Text; 
      }
      set 
      { 
        m_description.Text = value; 
        m_description.SelectAll();
      }
    } // Description

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
		} // Dispose( disposing )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles thecontinue click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnContinueClickHandler( object sender, EventArgs args )
    {
      this.DialogResult = DialogResult.OK;
      Close();
    } // OnContinueClickHandler( sender, args )

    /// <summary>
    ///  Handles the clicking of th cancel button
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnCancelClick(object sender, EventArgs e)
    {
      this.DialogResult = DialogResult.Cancel;
      Close();
    } // OnCancelClick(object sender, EventArgs e)


    #endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      this.m_description = new System.Windows.Forms.TextBox();
      this.m_descriptionLabel = new System.Windows.Forms.Label();
      this.m_continueButton = new Viatronix.UI.Button();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_buttonCancel = new Viatronix.UI.Button();
      this.SuspendLayout();
      // 
      // m_description
      // 
      this.m_description.Location = new System.Drawing.Point(8, 32);
      this.m_description.MaxLength = 255;
      this.m_description.Multiline = true;
      this.m_description.Name = "m_description";
      this.m_description.Size = new System.Drawing.Size(344, 72);
      this.m_description.TabIndex = 1;
      this.m_toolTip.SetToolTip(this.m_description, "Modify the session description");
      // 
      // m_descriptionLabel
      // 
      this.m_descriptionLabel.Location = new System.Drawing.Point(8, 8);
      this.m_descriptionLabel.Name = "m_descriptionLabel";
      this.m_descriptionLabel.Size = new System.Drawing.Size(340, 23);
      this.m_descriptionLabel.TabIndex = 1;
      this.m_descriptionLabel.Text = "Description of the session";
      this.m_descriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_continueButton
      // 
      this.m_continueButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_continueButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_continueButton.ForeColor = System.Drawing.Color.Black;
      this.m_continueButton.Location = new System.Drawing.Point(152, 112);
      this.m_continueButton.Name = "m_continueButton";
      this.m_continueButton.OldStockButton = false;
      this.m_continueButton.Size = new System.Drawing.Size(96, 23);
      this.m_continueButton.TabIndex = 2;
      this.m_continueButton.Text = "Continue";
      this.m_toolTip.SetToolTip(this.m_continueButton, "Click continue to launch the viewer");
      this.m_continueButton.Click += new System.EventHandler(this.OnContinueClickHandler);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Black;
      this.m_buttonCancel.Location = new System.Drawing.Point(256, 112);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.Size = new System.Drawing.Size(96, 23);
      this.m_buttonCancel.TabIndex = 3;
      this.m_buttonCancel.Text = "Cancel";
      this.m_toolTip.SetToolTip(this.m_buttonCancel, "Click to cancel");
      this.m_buttonCancel.Visible = false;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // SessionDescriptionDialog
      // 
      this.AcceptButton = this.m_continueButton;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(360, 142);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_description);
      this.Controls.Add(this.m_continueButton);
      this.Controls.Add(this.m_descriptionLabel);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "SessionDescriptionDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Session Description";
      this.TopMost = true;
      this.Controls.SetChildIndex(this.m_descriptionLabel, 0);
      this.Controls.SetChildIndex(this.m_continueButton, 0);
      this.Controls.SetChildIndex(this.m_description, 0);
      this.Controls.SetChildIndex(this.m_buttonCancel, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion

 

	} // class SaveAsDialog
} // namespace Viatronix.v3D.Viewer.Plugins

#region revision history

// $Log: SessionDescriptionbDialog.cs,v $
// Revision 1.1.2.5  2008/04/15 19:15:35  mkontak
// Made the continue button the accept button so that it becomes the default
//
// Revision 1.1.2.4  2008/04/14 14:57:33  mkontak
// no message
//
// Revision 1.1.2.3  2007/11/15 20:59:13  mkontak
// Fix for Kodak launcher
//
// Revision 1.1.2.2  2007/09/10 19:33:09  mkontak
// Fixed as per Tom
//
// Revision 1.1.2.1  2007/09/07 15:14:12  mkontak
// Added SessionDescriptionDialog for VC session relabeling
//
// Revision 1.4  2007/03/01 19:29:28  gdavidson
// Commented code
//
// Revision 1.3  2007/01/18 20:13:42  romy
// ELIMINATED THE \r and \n CHARACTERS FROM THE SESSION DESCRIPTION
//
// Revision 1.2  2005/10/26 20:07:21  gdavidson
// Modified the dialog to open in the center of the screen if a parent window is not provided.
//
// Revision 1.1  2005/10/11 14:09:29  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ui/SessionDescriptionbDialog.cs,v 1.1.2.5 2008/04/15 19:15:35 mkontak Exp $
// $Id: SessionDescriptionbDialog.cs,v 1.1.2.5 2008/04/15 19:15:35 mkontak Exp $

#endregion
