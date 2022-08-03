// $Id: SelectViewerDialog.cs,v 1.4.2.1 2008/10/29 18:31:55 kchalupa Exp $
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.v3D.Core;

namespace Viatronix.Console
{
	/// <summary>
	/// Dialog which allows a user to select a launch operation.
	/// </summary>
	public class SelectViewerDialog : System.Windows.Forms.Form
	{
 
    #region fields

    /// <summary>
    /// ViewerSelector user control
    /// </summary>
    private Viatronix.Console.ViewerSelector m_selector;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// The back panel
    /// </summary>
    private Viatronix.UI.Panel m_backPanel;

    /// <summary>
    /// The selected IViewerController.
    /// </summary>
    private IViewerLauncher m_selectedLauncher = null;

    #endregion

    #region construction

    /// <summary>
    /// default constructor.
    /// </summary>
		public SelectViewerDialog()
		{
			InitializeComponent();
		} // SelectViewerDialog()

    #endregion

    #region properties

    /// <summary>
    /// Returns the selected IViewerLauncher.
    /// </summary>
    public IViewerLauncher SelectedLauncher
    {
      get { return m_selectedLauncher; }
    } // SelectedLauncher

    #endregion

    #region methods

    /// <summary>
    /// Adds a collection of IViewerControllers to the selection control.
    /// </summary>
    /// <param name="collection">collection of IViewerControllers</param>
    public void AddViewers( List<IViewerLauncher> collection )
    {
      // Sort the launchers.
      collection.Sort(new ViewerLauncherComparer());

      foreach( IViewerLauncher controller in collection )
        m_selector.AddViewerController( controller );

      this.Height = this.Height - m_selector.Height + 120 + collection.Count * 18;
    } // AddViewers( collection )


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
    /// Handles the selection event.
    /// </summary>
    /// <param name="controller">IViewerController</param>
    private void OnViewerSelectedHandler( IViewerLauncher launcher )
    {
      m_selectedLauncher = launcher;
      this.DialogResult = DialogResult.OK;
    } // OnViewerSelectedHandler( controller )

    #endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_selector = new Viatronix.Console.ViewerSelector();
      this.m_backPanel = new Viatronix.UI.Panel();
      this.m_backPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_selector
      // 
      this.m_selector.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_selector.Location = new System.Drawing.Point(8, 8);
      this.m_selector.Name = "m_selector";
      this.m_selector.Size = new System.Drawing.Size(440, 136);
      this.m_selector.TabIndex = 0;
      this.m_selector.ViewerSelected += new Viatronix.UI.SetterDelegate<IViewerLauncher>(this.OnViewerSelectedHandler);
      // 
      // m_backPanel
      // 
      this.m_backPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_backPanel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(148)), ((System.Byte)(166)), ((System.Byte)(198)));
      this.m_backPanel.BorderColor = System.Drawing.Color.White;
      this.m_backPanel.BottomPadding = 0;
      this.m_backPanel.Controls.Add(this.m_selector);
      this.m_backPanel.Curve = 10;
      this.m_backPanel.Edges = Viatronix.UI.Edges.All;
      this.m_backPanel.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(123)), ((System.Byte)(134)), ((System.Byte)(165)));
      this.m_backPanel.LeftPadding = 0;
      this.m_backPanel.Location = new System.Drawing.Point(0, 0);
      this.m_backPanel.Name = "m_backPanel";
      this.m_backPanel.RightPadding = 0;
      this.m_backPanel.Size = new System.Drawing.Size(456, 152);
      this.m_backPanel.TabIndex = 1;
      this.m_backPanel.TopPadding = 0;
      // 
      // SelectViewerDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(456, 152);
      this.Controls.Add(this.m_backPanel);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Name = "SelectViewerDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Launch Viewer";
      this.m_backPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    } // InitializeComponent()

		#endregion

	} // class SelectViewerDialog
} // namespace Viatronix.Console

#region revision history

// $Log: SelectViewerDialog.cs,v $
// Revision 1.4.2.1  2008/10/29 18:31:55  kchalupa
// Issue #5802
// Sessions in Launch Pane are not sorted the same way as in Series Pane.
//
// Revision 1.4  2007/03/13 12:18:09  mkontak
// coding standards
//
// Revision 1.3  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.2  2007/03/01 18:53:15  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 15:35:55  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.2  2006/01/09 19:51:07  gdavidson
// Adjust the size of the dialog depending on the number of items being displayed.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/SelectViewerDialog.cs,v 1.4.2.1 2008/10/29 18:31:55 kchalupa Exp $
// $Id: SelectViewerDialog.cs,v 1.4.2.1 2008/10/29 18:31:55 kchalupa Exp $

#endregion
