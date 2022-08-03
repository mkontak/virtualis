// $Id: ProgressDialog.cs,v 1.7 2007/06/28 19:34:09 jmeade Exp $
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

namespace Viatronix.UI
{
	/// <summary>
	/// Summary description for TransferProgressDialog.
	/// </summary>
	public class ProgressDialog : Viatronix.UI.Dialog
	{

    #region fields

    /// <summary>
    /// progress bar
    /// </summary>
    private Viatronix.UI.ProgressBar2 m_progress;

    /// <summary>
    /// cancel button
    /// </summary>
    private Viatronix.UI.Button m_cancel;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Cancel event
    /// </summary>
    public event EventHandler Cancel = null;

    /// <summary>
    /// Enable closure flag
    /// </summary>
    public bool m_enableCloseBox = true;

    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
		public ProgressDialog() : this( 0, 100 )
		{
		} // ProgressDialog()


    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="min"></param>
    /// <param name="max"></param>
    public ProgressDialog( int min, int max )
    {
      InitializeComponent();

      m_progress.Minimum = 0;
      m_progress.Maximum = 100;
    } // ProgressDialog( min, max )

    #endregion

    #region properties

    /// <summary>
    /// Enable Close box
    /// </summary>
    public bool EnableCloseBox
    {
      get { return m_enableCloseBox; }
      set { m_enableCloseBox = value; }
    } // EnableCloseBox 

    /// <summary>
    /// Maximum value
    /// </summary>
    public int Maximum
    {
      get { return m_progress.Maximum; }
      set { m_progress.Maximum = value; }
    } // Maximum


    /// <summary>
    /// Minimum value
    /// </summary>
    public int Minimum
    {
      get { return m_progress.Minimum; }
      set { m_progress.Minimum = value; }
    } // Minimum
     

    /// <summary>
    /// Gets or set the visibility of the cancel button
    /// </summary>
    public bool ShowCancel
    {
      get { return m_cancel.Visible; }
      set
      { 
        m_cancel.Visible = value;
        if (value)
        {
          this.CancelButton = m_cancel;
        }
        else
        {
          this.CancelButton = null;
        }
      }
    } // ShowCancel

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


    /// <summary>
    /// Raises the cancel button
    /// </summary>
    protected virtual void OnCancel()
    {
      if( this.Cancel != null )
        this.Cancel( this, EventArgs.Empty );
    } // OnCancel()

    #endregion

    #region methods

    /// <summary>
    /// Increments the progress bar
    /// </summary>
    public void Increment()
    {
      SetProgress( m_progress.Value + 1 );
    } // Increment()


    /// <summary>
    /// Sets the progress bar complete
    /// </summary>
    /// <param name="message"></param>
    public void SetComplete(string message)
    {
      SetProgress(m_progress.Maximum, m_progress.Maximum, message);
    } // SetComplete(string message)
    

    /// <summary>
    /// Sets the progress to the specified amount
    /// </summary>
    /// <param name="progress">current progress</param>
    public void SetProgress( int progress )
    {
      SetProgress( progress, m_progress.Maximum );
    } // SetProgress( progress )


    /// <summary>
    /// Sets the current progress and max value.
    /// </summary>
    /// <param name="progress">current progress</param>
    /// <param name="max">max value</param>
    public void SetProgress( int progress, int max )
    {
      if( InvokeRequired )
        Invoke( new SetterDelegate<int, int>( SetProgress ), progress, max );
      else
      {
        m_progress.Maximum = max;
        m_progress.Value = progress;
        m_progress.Refresh();
      }
    } // SetProgress( progress, max )


    /// <summary>
    /// Sets the current progress, max value and message.
    /// </summary>
    /// <param name="progress">current progress</param>
    /// <param name="max">max value</param>
    /// <param name="message">progress message</param>
    public void SetProgress( int progress, int max, string message )
    {
      if( InvokeRequired )
        Invoke( new SetterDelegate<int, int, string>( SetProgress ), progress, max, message );
      else
      {
        m_progress.Maximum = max;
        m_progress.Value = progress;
        m_progress.Text = message;
        m_progress.Refresh();
      }
    } // SetProgress( progress, max, message )


    /// <summary>
    /// Sets the progress message
    /// </summary>
    /// <param name="message">message</param>
    public void SetMessage( string message )
    {
      if( InvokeRequired )
        Invoke( new SetterDelegate<string>( SetMessage ), message );
      else
      {
        m_progress.Text = message;
        m_progress.Refresh();
      }
    } // SetMessage( message )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the cancel click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCancelClickHandler( object sender, EventArgs args )
    {
      Close();
    } // OnCancelClickHandler( sender, args )

    
    /// <summary>
    /// Handles the closing click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An CancelEventArgs that contains the event data.</param>
    private void OnClosingEventHandler( object sender, CancelEventArgs args )
    {
      if (this.ControlBox && m_enableCloseBox )
      {
        if (m_progress.Value < m_progress.Maximum)
          OnCancel();
      }
      else
      {
        if (m_progress.Value < m_progress.Maximum)
          args.Cancel = true;
      }
    } // OnClosingEventHandler( sender, args )

    #endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_progress = new Viatronix.UI.ProgressBar2();
      this.m_cancel = new Viatronix.UI.Button();
      this.SuspendLayout();
      // 
      // m_progress
      // 
      this.m_progress.BackColor = System.Drawing.Color.Transparent;
      this.m_progress.DrawText = true;
      this.m_progress.Location = new System.Drawing.Point(16, 16);
      this.m_progress.Maximum = 100;
      this.m_progress.Minimum = 0;
      this.m_progress.Name = "m_progress";
      this.m_progress.Pulse = false;
      this.m_progress.Size = new System.Drawing.Size(264, 23);
      this.m_progress.Step = 1;
      this.m_progress.TabIndex = 0;
      this.m_progress.Value = 0;
      // 
      // m_cancel
      // 
      this.m_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_cancel.ForeColor = System.Drawing.Color.Black;
      this.m_cancel.Location = new System.Drawing.Point(112, 48);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.Size = new System.Drawing.Size(64, 23);
      this.m_cancel.TabIndex = 1;
      this.m_cancel.Text = "Cancel";
      this.m_cancel.Click += new System.EventHandler(this.OnCancelClickHandler);
      // 
      // ProgressDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(290, 78);
      this.Controls.Add(this.m_cancel);
      this.Controls.Add(this.m_progress);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ProgressDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Closing += new System.ComponentModel.CancelEventHandler(this.OnClosingEventHandler);
      this.Controls.SetChildIndex(this.m_progress, 0);
      this.Controls.SetChildIndex(this.m_cancel, 0);
      this.ResumeLayout(false);

    }
		#endregion

	} // class ProgressDialog
} // namespace Viatronix.UI

#region revision history

// $Log: ProgressDialog.cs,v $
// Revision 1.7  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.6.2.3  2007/03/26 20:40:14  mkontak
// Added SetComplete method
//
// Revision 1.6.2.2  2007/03/23 15:02:08  mkontak
// Added a EnabelCloseBox property to disable the X in the upper right hand corner
//
// Revision 1.6.2.1  2007/03/23 14:54:07  mkontak
// Added a EnabelCloseBox property to disable the X in the upper right hand corner
//
// Revision 1.6  2007/03/01 16:05:48  gdavidson
// Commented code
//
// Revision 1.5  2006/10/10 21:21:54  gdavidson
// Fixed cross threading errors
//
// Revision 1.4  2006/07/13 20:41:43  romy
// added delegate for Progress dialog
//
// Revision 1.3  2005/10/26 19:57:33  gdavidson
// Added code to allow 'esc' to cancel the dialog, if cancels are allowed.
//
// Revision 1.2  2005/10/11 14:10:31  gdavidson
// Set the dialog to center on parent.
//
// Revision 1.1  2005/10/06 15:52:31  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ProgressDialog.cs,v 1.7 2007/06/28 19:34:09 jmeade Exp $
// $Id: ProgressDialog.cs,v 1.7 2007/06/28 19:34:09 jmeade Exp $

#endregion
