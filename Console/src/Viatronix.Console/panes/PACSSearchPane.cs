// $Id: PACSSearchPane.cs,v 1.22.2.1 2007/12/14 18:18:48 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Xml;
using System.Windows.Forms;
using System.Runtime.Remoting.Messaging;
using System.Runtime.CompilerServices;
using Viatronix.UI;
using Viatronix.v3D.Core;
using Viatronix.v3D.Dicom.UI;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{

	/// <summary>
	/// Provides a GUI for performing more specific searches.
	/// </summary>
	public class PACSSearchPane : UserControl, ISearchable, ISearchPane
	{

    #region fields


    /// <summary>
    /// Groupbox containing all of the controls.
    /// </summary>
    private Viatronix.UI.GroupBox m_searchGroupBox;

    /// <summary>
    /// The second divider in the groupbox.
    /// </summary>
    private System.Windows.Forms.GroupBox m_studyGroupBox;

    /// <summary>
    /// The first divider in the groupbox.
    /// </summary>
    private System.Windows.Forms.GroupBox m_patientGroupBox;

    /// <summary>
    /// Label for the first name textbox.
    /// </summary>
    private System.Windows.Forms.Label m_firstNameLabel;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// label for study id
    /// </summary>
    private System.Windows.Forms.Label m_labelStudyId;

    /// <summary>
    /// textbox for medical id
    /// </summary>
    private System.Windows.Forms.TextBox m_textBoxMedicalId;

    /// <summary>
    /// label for medical id
    /// </summary>
    private System.Windows.Forms.Label m_labelMedicalId;

    /// <summary>
    /// textbox for last name
    /// </summary>
    private System.Windows.Forms.TextBox m_textBoxLastName;

    /// <summary>
    /// label for modality
    /// </summary>
    private System.Windows.Forms.Label m_labelModality;

    /// <summary>
    /// combobox for modality
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxModality;

    /// <summary>
    /// textbox for study id
    /// </summary>
    private System.Windows.Forms.TextBox m_textBoxStudyId;

    /// <summary>
    /// groupbox
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox1;

    /// <summary>
    /// serach control
    /// </summary>
    private Viatronix.Console.SearchControl m_searchControl;

    /// <summary>
    /// scan date control
    /// </summary>
    private Viatronix.Console.ScanDateControl m_scanDate;

    /// <summary>
    /// groupbox for pacs
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBoxPacs;

    /// <summary>
    /// pacs selection control
    /// </summary>
    private Viatronix.v3D.Dicom.UI.PacsSelectionControl m_pacsSelectionControl;

    /// <summary>
    /// search failed event handler
    /// </summary>
    private MessageDelegate m_searchFailedHandler = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SearchPane control.
    /// </summary>
		public PACSSearchPane()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      m_searchControl.SearchPane = this;
      m_searchControl.Clear += new EventHandler( this.OnSearchClearHandler );
      m_searchControl.BeginSearch += new EventHandler(this.OnBeginSearchHandler);
      m_searchFailedHandler = new MessageDelegate( this.OnSearchFailedHandler );

      XmlNode dicomInfo = (XmlNode)ConfigurationManager.GetSection( "searchInfo" );

      int maxLength = 0;
      m_comboBoxModality.DropDownWidth = m_comboBoxModality.Width;
      m_comboBoxModality.Items.Add("All");
      m_comboBoxModality.SelectedIndex = 0;

      List<Modality> modalities = Global.DataProvider.GetModalities();

      foreach (Modality modality in modalities)
      {
        m_comboBoxModality.Items.Add(modality);
        maxLength = Math.Max(maxLength, modality.Code.Length + 3 + modality.Description.Length);
      }

      m_comboBoxModality.DropDownWidth = Math.Max((maxLength * 6 + 10), m_comboBoxModality.Width);


      m_pacsSelectionControl.ServerSelected += new EventHandler( this.OnServerSelectedHandler );

      if (m_pacsSelectionControl.Count == 0)
        m_searchControl.Enabled = false;

      m_scanDate.Style = ScanDateStyle.None;
		} // SearchPane()

    #endregion

    #region properties

    /// <summary>
    /// Gets the dicom host
    /// </summary>
    public DicomHost PacsServer
    {
      get { return m_pacsSelectionControl.SelectedServer; }
    } // PacsServer

    #endregion 

    #region methods

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
		} //  Dispose( disposing )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the pacs server selection event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnServerSelectedHandler( object sender, EventArgs args )
    {
      ((IPacsProvider)this.Searcher.BrowserProvider).Server = m_pacsSelectionControl.SelectedServer;
    } // OnServerSelectedHandler( sender, args )


    /// <summary>
    /// Handles the search failed event
    /// </summary>
    /// <param name="message">Error message</param>
    private void OnSearchFailedHandler( string message )
    {
      Viatronix.UI.MessageBox.ShowError( Global.Frame, message );
    } // OnSearchFailedHandler( message )

    /// <summary>
    /// Handles the search clear event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSearchClearHandler( object sender, EventArgs args )
    {
      ClearControls();
    } // OnSearchClearHandler( sender, args )


    /// <summary>
    /// Handles the begin search clear event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnBeginSearchHandler(object sender, EventArgs args)
    {
      ((IPacsProvider)this.Searcher.BrowserProvider).Server = m_pacsSelectionControl.SelectedServer;
    } // OnSearchClearHandler( sender, args )

    #endregion

    #region ISearchPane Members

    /// <summary>
    /// Creates aXmlBuilderfor the provided search criteria.
    /// </summary>
    /// <returns>A QueryBuilder</returns>
    public XmlBuilder CreateQuery()
    {
      XmlBuilder query = new XmlBuilder("study");

      // TODO
      if (m_pacsSelectionControl.SelectedServer != null)
        query.AddAttribute("system", m_pacsSelectionControl.SelectedServer.Name.Trim());

      query.AddAttribute("name", m_textBoxLastName.Text.Trim());
      query.AddAttribute("mid", m_textBoxMedicalId.Text.Trim());

      // append scan date information
      m_scanDate.AddDate(query);

      query.AddAttribute("id", m_textBoxStudyId.Text.Trim());

      Modality modality = m_comboBoxModality.Items[m_comboBoxModality.SelectedIndex] as Modality;

      if (modality != null)
        query.AddAttribute("mods", modality.Code);
      else
        query.AddAttribute("mods", m_comboBoxModality.Text.Trim());

      return query;
    } // CreateQueryBuilder()


    /// <summary>
    /// Clears the child controls.
    /// </summary>
    public void ClearControls()
    {
      m_textBoxStudyId.Text = string.Empty;
      m_textBoxLastName.Text  = string.Empty;
      m_textBoxMedicalId.Text = string.Empty;

      m_scanDate.Style = ScanDateStyle.None;
      
      m_comboBoxModality.SelectedIndex = 0;
    } // ClearControls()

    #endregion

    #region ISearchable Members

    /// <summary>
    /// Gets or set the search manager objcet.
    /// </summary>
    [Browsable( false )]
    public SearchManager Searcher
    {
      get { return m_searchControl.Searcher; }
      set 
      { 
        if( m_searchControl.Searcher != null )
          m_searchControl.Searcher.SearchFailed -= m_searchFailedHandler;



        m_searchControl.Searcher = value;


        m_searchControl.Searcher.SearchFailed += m_searchFailedHandler;
      }
    } // Searcher

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_searchGroupBox = new Viatronix.UI.GroupBox();
      this.m_groupBoxPacs = new System.Windows.Forms.GroupBox();
      this.m_searchControl = new Viatronix.Console.SearchControl();
      this.m_patientGroupBox = new System.Windows.Forms.GroupBox();
      this.m_textBoxLastName = new System.Windows.Forms.TextBox();
      this.m_labelMedicalId = new System.Windows.Forms.Label();
      this.m_textBoxMedicalId = new System.Windows.Forms.TextBox();
      this.m_firstNameLabel = new System.Windows.Forms.Label();
      this.m_studyGroupBox = new System.Windows.Forms.GroupBox();
      this.m_scanDate = new Viatronix.Console.ScanDateControl();
      this.m_labelStudyId = new System.Windows.Forms.Label();
      this.m_textBoxStudyId = new System.Windows.Forms.TextBox();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.m_labelModality = new System.Windows.Forms.Label();
      this.m_comboBoxModality = new System.Windows.Forms.ComboBox();

      // TODO Fix
      this.m_pacsSelectionControl = new Viatronix.v3D.Dicom.UI.PacsSelectionControl(Global.DataProvider);
      this.m_searchGroupBox.SuspendLayout();
      this.m_patientGroupBox.SuspendLayout();
      this.m_studyGroupBox.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_searchGroupBox
      // 
      this.m_searchGroupBox.BackColor = System.Drawing.Color.Transparent;
      this.m_searchGroupBox.BorderColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.m_searchGroupBox.Controls.Add(this.m_groupBoxPacs);
      this.m_searchGroupBox.Controls.Add(this.m_searchControl);
      this.m_searchGroupBox.Controls.Add(this.m_patientGroupBox);
      this.m_searchGroupBox.Controls.Add(this.m_studyGroupBox);
      this.m_searchGroupBox.Controls.Add(this.groupBox1);
      this.m_searchGroupBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_searchGroupBox.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.m_searchGroupBox.Location = new System.Drawing.Point(0, 0);
      this.m_searchGroupBox.Name = "m_searchGroupBox";
      this.m_searchGroupBox.Size = new System.Drawing.Size(296, 440);
      this.m_searchGroupBox.TabColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.m_searchGroupBox.TabIndex = 4;
      this.m_searchGroupBox.TabStop = false;
      // 
      // m_pacsSelectionControl
      //
      this.m_pacsSelectionControl.BackColor = System.Drawing.Color.Transparent;
      this.m_pacsSelectionControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_pacsSelectionControl.ForeColor = System.Drawing.Color.Navy;
      this.m_pacsSelectionControl.Location = new System.Drawing.Point(8, 16);
      this.m_pacsSelectionControl.Name = "m_pacsSelectionControl";
      this.m_pacsSelectionControl.ServiceType = (DicomServiceTypes.QueryRetrieve);
      this.m_pacsSelectionControl.Size = new System.Drawing.Size(248, 21);
      this.m_pacsSelectionControl.TabIndex = 5;
      // 
      // m_groupBoxPacs
      // 
      this.m_groupBoxPacs.BackColor = System.Drawing.Color.Transparent;
      this.m_groupBoxPacs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_groupBoxPacs.ForeColor = System.Drawing.Color.White;
      this.m_groupBoxPacs.Location = new System.Drawing.Point(16, 24);
      this.m_groupBoxPacs.Name = "m_groupBoxPacs";
      this.m_groupBoxPacs.Size = new System.Drawing.Size(264, 48);
      this.m_groupBoxPacs.TabIndex = 0;
      this.m_groupBoxPacs.TabStop = false;
      this.m_groupBoxPacs.Text = "PACS Server";
      this.m_groupBoxPacs.Controls.Add(m_pacsSelectionControl);
      // 
      // m_searchControl
      // 
      this.m_searchControl.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.m_searchControl.Location = new System.Drawing.Point(16, 408);
      this.m_searchControl.Name = "m_searchControl";
      this.m_searchControl.Searcher = null;
      this.m_searchControl.SearchPane = null;
      this.m_searchControl.Size = new System.Drawing.Size(264, 24);
      this.m_searchControl.TabIndex = 11;
      // 
      // m_patientGroupBox
      // 
      this.m_patientGroupBox.BackColor = System.Drawing.Color.Transparent;
      this.m_patientGroupBox.Controls.Add(this.m_textBoxLastName);
      this.m_patientGroupBox.Controls.Add(this.m_labelMedicalId);
      this.m_patientGroupBox.Controls.Add(this.m_textBoxMedicalId);
      this.m_patientGroupBox.Controls.Add(this.m_firstNameLabel);
      this.m_patientGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_patientGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_patientGroupBox.Location = new System.Drawing.Point(16, 80);
      this.m_patientGroupBox.Name = "m_patientGroupBox";
      this.m_patientGroupBox.Size = new System.Drawing.Size(264, 88);
      this.m_patientGroupBox.TabIndex = 1;
      this.m_patientGroupBox.TabStop = false;
      this.m_patientGroupBox.Text = "Patient Information";
      // 
      // m_textBoxLastName
      // 
      this.m_textBoxLastName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxLastName.Location = new System.Drawing.Point(72, 24);
      this.m_textBoxLastName.Name = "m_textBoxLastName";
      this.m_textBoxLastName.Size = new System.Drawing.Size(184, 20);
      this.m_textBoxLastName.TabIndex = 6;
      this.m_textBoxLastName.Text = "";
      // 
      // m_labelMedicalId
      // 
      this.m_labelMedicalId.BackColor = System.Drawing.Color.Transparent;
      this.m_labelMedicalId.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelMedicalId.ForeColor = System.Drawing.Color.White;
      this.m_labelMedicalId.Location = new System.Drawing.Point(8, 56);
      this.m_labelMedicalId.Name = "m_labelMedicalId";
      this.m_labelMedicalId.Size = new System.Drawing.Size(64, 23);
      this.m_labelMedicalId.TabIndex = 9;
      this.m_labelMedicalId.Text = "Medical ID:";
      this.m_labelMedicalId.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_textBoxMedicalId
      // 
      this.m_textBoxMedicalId.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxMedicalId.Location = new System.Drawing.Point(72, 56);
      this.m_textBoxMedicalId.Name = "m_textBoxMedicalId";
      this.m_textBoxMedicalId.Size = new System.Drawing.Size(184, 20);
      this.m_textBoxMedicalId.TabIndex = 7;
      this.m_textBoxMedicalId.Text = "";
      // 
      // m_firstNameLabel
      // 
      this.m_firstNameLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_firstNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_firstNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_firstNameLabel.Location = new System.Drawing.Point(8, 24);
      this.m_firstNameLabel.Name = "m_firstNameLabel";
      this.m_firstNameLabel.Size = new System.Drawing.Size(64, 23);
      this.m_firstNameLabel.TabIndex = 1;
      this.m_firstNameLabel.Text = "Last Name:";
      this.m_firstNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_studyGroupBox
      // 
      this.m_studyGroupBox.BackColor = System.Drawing.Color.Transparent;
      this.m_studyGroupBox.Controls.Add(this.m_scanDate);
      this.m_studyGroupBox.Controls.Add(this.m_labelStudyId);
      this.m_studyGroupBox.Controls.Add(this.m_textBoxStudyId);
      this.m_studyGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_studyGroupBox.ForeColor = System.Drawing.Color.White;
      this.m_studyGroupBox.Location = new System.Drawing.Point(16, 176);
      this.m_studyGroupBox.Name = "m_studyGroupBox";
      this.m_studyGroupBox.Size = new System.Drawing.Size(264, 152);
      this.m_studyGroupBox.TabIndex = 2;
      this.m_studyGroupBox.TabStop = false;
      this.m_studyGroupBox.Text = "Study Information";
      // 
      // m_scanDate
      // 
      this.m_scanDate.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.m_scanDate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_scanDate.Location = new System.Drawing.Point(8, 56);
      this.m_scanDate.Name = "m_scanDate";
      this.m_scanDate.Size = new System.Drawing.Size(248, 86);
      this.m_scanDate.TabIndex = 9;
      // 
      // m_labelStudyId
      // 
      this.m_labelStudyId.BackColor = System.Drawing.Color.Transparent;
      this.m_labelStudyId.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelStudyId.ForeColor = System.Drawing.Color.White;
      this.m_labelStudyId.Location = new System.Drawing.Point(8, 24);
      this.m_labelStudyId.Name = "m_labelStudyId";
      this.m_labelStudyId.Size = new System.Drawing.Size(56, 23);
      this.m_labelStudyId.TabIndex = 14;
      this.m_labelStudyId.Text = "Study ID:";
      this.m_labelStudyId.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_textBoxStudyId
      // 
      this.m_textBoxStudyId.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxStudyId.Location = new System.Drawing.Point(64, 25);
      this.m_textBoxStudyId.Name = "m_textBoxStudyId";
      this.m_textBoxStudyId.Size = new System.Drawing.Size(192, 20);
      this.m_textBoxStudyId.TabIndex = 8;
      this.m_textBoxStudyId.Text = "";
      // 
      // groupBox1
      // 
      this.groupBox1.BackColor = System.Drawing.Color.Transparent;
      this.groupBox1.Controls.Add(this.m_labelModality);
      this.groupBox1.Controls.Add(this.m_comboBoxModality);
      this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.groupBox1.ForeColor = System.Drawing.Color.White;
      this.groupBox1.Location = new System.Drawing.Point(16, 336);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(264, 60);
      this.groupBox1.TabIndex = 3;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Series Information";
      // 
      // m_labelModality
      // 
      this.m_labelModality.BackColor = System.Drawing.Color.Transparent;
      this.m_labelModality.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelModality.ForeColor = System.Drawing.Color.White;
      this.m_labelModality.Location = new System.Drawing.Point(8, 24);
      this.m_labelModality.Name = "m_labelModality";
      this.m_labelModality.Size = new System.Drawing.Size(56, 23);
      this.m_labelModality.TabIndex = 19;
      this.m_labelModality.Text = "Modality:";
      this.m_labelModality.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxModality
      // 
      this.m_comboBoxModality.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxModality.Location = new System.Drawing.Point(64, 24);
      this.m_comboBoxModality.Name = "m_comboBoxModality";
      this.m_comboBoxModality.Size = new System.Drawing.Size(192, 21);
      this.m_comboBoxModality.TabIndex = 10;
      // 
      // PACSSearchPane
      // 
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.Controls.Add(this.m_searchGroupBox);
      this.Name = "PACSSearchPane";
      this.Size = new System.Drawing.Size(296, 440);
      this.m_searchGroupBox.ResumeLayout(false);
      this.m_patientGroupBox.ResumeLayout(false);
      this.m_studyGroupBox.ResumeLayout(false);
      this.groupBox1.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

  } // class SearchPane
} // namespace Viatronix.Console

#region revision history

// $Log: PACSSearchPane.cs,v $
// Revision 1.22.2.1  2007/12/14 18:18:48  mkontak
// Fix the PACS combo to only list the QR service types
//
// Revision 1.22  2007/02/26 19:06:00  gdavidson
// Commented Code
//
// Revision 1.21  2006/10/27 13:10:57  mkontak
// Issue 5018
//
// Revision 1.20  2006/09/06 13:16:31  mkontak
// Issue 4969
//
// Revision 1.19  2006/08/07 17:37:02  mkontak
// Issue 4915
//
// Revision 1.18  2006/08/07 03:25:56  mkontak
// no message
//
// Revision 1.17  2006/07/31 14:59:47  mkontak
// Issue 4912
//
// Revision 1.16  2006/07/21 13:10:17  mkontak
// Change the celection control to not include the label
//
// Revision 1.15  2006/07/10 18:35:01  gdavidson
// Issue #4836: Modified SearchManager
//
// Revision 1.14  2006/06/06 19:46:55  mkontak
// changed deprecated code
//
// Revision 1.13  2006/04/21 18:34:32  gdavidson
// Issue #4729: Changed modality text to 'All' and unbolded.
//
// Revision 1.12  2006/03/13 15:58:03  gdavidson
// Search ignores leading and trailing spaces.
//
// Revision 1.11  2006/03/02 18:54:24  gdavidson
// Changed the tab ordering
//
// Revision 1.10  2006/02/28 14:43:17  mkontak
// Fixed PACS server selection
//
// Revision 1.9  2006/02/23 21:05:08  gdavidson
// Implemented ISearchPane interface and add the SearchControl and ScanDateControl usercontrols.
//
// Revision 1.8  2006/02/16 21:53:16  gdavidson
// Hides the search results label after 5 seconds.
//
// Revision 1.7  2006/02/14 16:47:22  gdavidson
// Added a clear button
//
// Revision 1.6  2006/02/09 21:46:39  romy
// Bug fixes
//
// Revision 1.5  2006/02/03 13:48:23  mkontak
// Added cursor wait when search is clicked
//
// Revision 1.4  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.3  2005/11/15 16:09:20  mkontak
// Added modality and subtypes
//
// Revision 1.2  2005/11/01 18:08:44  mkontak
// Added Modality search
//
// Revision 1.1  2005/10/31 17:18:38  mkontak
// PACS Query
//
// Revision 1.7  2005/10/18 17:48:16  gdavidson
// Implemented the ISearchable interface.
//
// Revision 1.6  2005/10/11 14:01:36  gdavidson
// Modified the IDataProvider interface.
//
// Revision 1.5  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.4  2005/05/26 17:45:01  gdavidson
// Changed the look of the pane.
//
// Revision 1.3  2005/05/03 18:18:13  frank
// reflected a change in the enumerations
//
// Revision 1.2  2005/03/29 01:06:02  gdavidson
// Added reference to a DataProvider.
//
// Revision 1.1  2005/02/24 19:08:54  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/panes/PACSSearchPane.cs,v 1.22.2.1 2007/12/14 18:18:48 mkontak Exp $
// $Id: PACSSearchPane.cs,v 1.22.2.1 2007/12/14 18:18:48 mkontak Exp $

#endregion