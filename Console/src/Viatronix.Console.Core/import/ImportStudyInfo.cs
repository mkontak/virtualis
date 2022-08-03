// $Id: ImportStudyInfo.cs,v 1.6.2.1 2008/01/31 21:01:58 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// User interface for providing study information.
	/// </summary>
	public class ImportStudyInfo : System.Windows.Forms.UserControl
	{

    #region fields

    /// <summary>
    /// study info label
    /// </summary>
    private System.Windows.Forms.Label m_studyInfoLabel;

    /// <summary>
    /// description text box
    /// </summary>
    private System.Windows.Forms.TextBox m_description;

    /// <summary>
    /// description label
    /// </summary>
    private System.Windows.Forms.Label m_descriptionLabel;

    /// <summary>
    /// date label
    /// </summary>
    private System.Windows.Forms.Label m_dateLabel;

    /// <summary>
    /// date picker
    /// </summary>
    private System.Windows.Forms.DateTimePicker m_date;

    /// <summary>
    /// physician label
    /// </summary>
    private System.Windows.Forms.Label m_firstNameLabel;

    /// <summary>
    /// groupbox 
    /// </summary>
    private System.Windows.Forms.GroupBox m_firstDivider;

    /// <summary>
    /// groupbox
    /// </summary>
    private System.Windows.Forms.GroupBox m_secondDivider;

    /// <summary>
    /// physician textbox
    /// </summary>
    private System.Windows.Forms.TextBox m_firstName;
    private GroupBox m_pysicianGroup;
    private Label m_lastNameLabel;
    private TextBox m_lastName;
    private Label m_labelGroup;
    private Viatronix.v3D.UI.ComboBox m_comboBoxGroup;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
		public ImportStudyInfo()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      // Get the list of study groups from the database
      StudyGroupCollection studyGroups  = Viatronix.v3D.Provider.Database.GetStudyGroups();
      if (studyGroups.Count > 0)
      {
        foreach (StudyGroup studyGroup in studyGroups)
        {
          m_comboBoxGroup.Items.Add(studyGroup.Name);
        }
      }
      else
      {
        // Default to only one selection
        m_comboBoxGroup.Items.Add("");
      }

      m_comboBoxGroup.SelectedIndex = 0;

		} // ImportStudyInfo()

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
		}

    /// <summary>
    /// Saves the study information
    /// </summary>
    /// <param name="study"></param>
    public void Save(Study study)
    {
      study.Description = m_description.Text;
      study.Group = (string)m_comboBoxGroup.SelectedItem;
      study.Date = m_date.Value;
      study.ReferringPhysician = new Viatronix.v3D.Core.PersonName( m_lastName.Text, m_firstName.Text );
    } // Save( study )


    /// <summary>
    /// Initializes the form
    /// </summary>
    /// <param name="study"></param>
    public void Initialize( Study study )
    {

      m_comboBoxGroup.SelectedIndex = 0;
      if (study.Group.Length > 0 && m_comboBoxGroup.Items.Count > 0)
      {
        for ( int i = 0; i < m_comboBoxGroup.Items.Count; ++i )
        {
          StudyGroup sg = (StudyGroup)m_comboBoxGroup.Items[i];
          if (string.Compare(sg.Name, study.Group, true) == 0)
          {
            m_comboBoxGroup.SelectedIndex = i;
            break;
          }
        }
      } // END ... 

      m_description.Text = study.Description;
      m_date.Value = study.Date;
      m_firstName.Text = study.ReferringPhysician.FirstName;
      m_lastName.Text = study.ReferringPhysician.LastName;
    } // Initialize( study )


    /// <summary>
    /// Resets the form
    /// </summary>
    public void Reset()
    {
      m_description.Text = string.Empty;
      m_date.Value = DateTime.Now;
      m_firstName.Text = string.Empty;
    } // Reset()

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_studyInfoLabel = new System.Windows.Forms.Label();
      this.m_descriptionLabel = new System.Windows.Forms.Label();
      this.m_description = new System.Windows.Forms.TextBox();
      this.m_dateLabel = new System.Windows.Forms.Label();
      this.m_date = new System.Windows.Forms.DateTimePicker();
      this.m_firstNameLabel = new System.Windows.Forms.Label();
      this.m_firstName = new System.Windows.Forms.TextBox();
      this.m_firstDivider = new System.Windows.Forms.GroupBox();
      this.m_secondDivider = new System.Windows.Forms.GroupBox();
      this.m_pysicianGroup = new System.Windows.Forms.GroupBox();
      this.m_lastNameLabel = new System.Windows.Forms.Label();
      this.m_lastName = new System.Windows.Forms.TextBox();
      this.m_labelGroup = new System.Windows.Forms.Label();
      this.m_comboBoxGroup = new Viatronix.v3D.UI.ComboBox();
      this.m_pysicianGroup.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_studyInfoLabel
      // 
      this.m_studyInfoLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_studyInfoLabel.ForeColor = System.Drawing.Color.White;
      this.m_studyInfoLabel.Location = new System.Drawing.Point(8, 0);
      this.m_studyInfoLabel.Name = "m_studyInfoLabel";
      this.m_studyInfoLabel.Size = new System.Drawing.Size(216, 23);
      this.m_studyInfoLabel.TabIndex = 1;
      this.m_studyInfoLabel.Text = "Study Information";
      this.m_studyInfoLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_descriptionLabel
      // 
      this.m_descriptionLabel.ForeColor = System.Drawing.Color.White;
      this.m_descriptionLabel.Location = new System.Drawing.Point(16, 40);
      this.m_descriptionLabel.Name = "m_descriptionLabel";
      this.m_descriptionLabel.Size = new System.Drawing.Size(64, 23);
      this.m_descriptionLabel.TabIndex = 4;
      this.m_descriptionLabel.Text = "Description:";
      this.m_descriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_description
      // 
      this.m_description.Location = new System.Drawing.Point(104, 40);
      this.m_description.Name = "m_description";
      this.m_description.Size = new System.Drawing.Size(328, 20);
      this.m_description.TabIndex = 5;
      // 
      // m_dateLabel
      // 
      this.m_dateLabel.ForeColor = System.Drawing.Color.White;
      this.m_dateLabel.Location = new System.Drawing.Point(16, 97);
      this.m_dateLabel.Name = "m_dateLabel";
      this.m_dateLabel.Size = new System.Drawing.Size(80, 23);
      this.m_dateLabel.TabIndex = 8;
      this.m_dateLabel.Text = "Study Date:";
      this.m_dateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_date
      // 
      this.m_date.Format = System.Windows.Forms.DateTimePickerFormat.Short;
      this.m_date.Location = new System.Drawing.Point(104, 98);
      this.m_date.Name = "m_date";
      this.m_date.Size = new System.Drawing.Size(112, 20);
      this.m_date.TabIndex = 9;
      // 
      // m_firstNameLabel
      // 
      this.m_firstNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_firstNameLabel.Location = new System.Drawing.Point(6, 24);
      this.m_firstNameLabel.Name = "m_firstNameLabel";
      this.m_firstNameLabel.Size = new System.Drawing.Size(65, 23);
      this.m_firstNameLabel.TabIndex = 15;
      this.m_firstNameLabel.Text = "First Name:";
      this.m_firstNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_firstName
      // 
      this.m_firstName.Location = new System.Drawing.Point(70, 25);
      this.m_firstName.Name = "m_firstName";
      this.m_firstName.Size = new System.Drawing.Size(136, 20);
      this.m_firstName.TabIndex = 16;
      // 
      // m_firstDivider
      // 
      this.m_firstDivider.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_firstDivider.Location = new System.Drawing.Point(8, 24);
      this.m_firstDivider.Name = "m_firstDivider";
      this.m_firstDivider.Size = new System.Drawing.Size(428, 8);
      this.m_firstDivider.TabIndex = 17;
      this.m_firstDivider.TabStop = false;
      // 
      // m_secondDivider
      // 
      this.m_secondDivider.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_secondDivider.Location = new System.Drawing.Point(8, 125);
      this.m_secondDivider.Name = "m_secondDivider";
      this.m_secondDivider.Size = new System.Drawing.Size(428, 8);
      this.m_secondDivider.TabIndex = 19;
      this.m_secondDivider.TabStop = false;
      // 
      // m_pysicianGroup
      // 
      this.m_pysicianGroup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_pysicianGroup.Controls.Add(this.m_lastNameLabel);
      this.m_pysicianGroup.Controls.Add(this.m_firstNameLabel);
      this.m_pysicianGroup.Controls.Add(this.m_lastName);
      this.m_pysicianGroup.Controls.Add(this.m_firstName);
      this.m_pysicianGroup.ForeColor = System.Drawing.Color.White;
      this.m_pysicianGroup.Location = new System.Drawing.Point(10, 145);
      this.m_pysicianGroup.Name = "m_pysicianGroup";
      this.m_pysicianGroup.Size = new System.Drawing.Size(428, 61);
      this.m_pysicianGroup.TabIndex = 20;
      this.m_pysicianGroup.TabStop = false;
      this.m_pysicianGroup.Text = "Referring Physician";
      // 
      // m_lastNameLabel
      // 
      this.m_lastNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_lastNameLabel.Location = new System.Drawing.Point(226, 24);
      this.m_lastNameLabel.Name = "m_lastNameLabel";
      this.m_lastNameLabel.Size = new System.Drawing.Size(65, 23);
      this.m_lastNameLabel.TabIndex = 21;
      this.m_lastNameLabel.Text = "Last Name:";
      this.m_lastNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_lastName
      // 
      this.m_lastName.Location = new System.Drawing.Point(292, 25);
      this.m_lastName.Name = "m_lastName";
      this.m_lastName.Size = new System.Drawing.Size(130, 20);
      this.m_lastName.TabIndex = 22;
      // 
      // m_labelGroup
      // 
      this.m_labelGroup.ForeColor = System.Drawing.Color.White;
      this.m_labelGroup.Location = new System.Drawing.Point(17, 65);
      this.m_labelGroup.Name = "m_labelGroup";
      this.m_labelGroup.Size = new System.Drawing.Size(64, 23);
      this.m_labelGroup.TabIndex = 21;
      this.m_labelGroup.Text = "Group:";
      this.m_labelGroup.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxGroup
      // 
      this.m_comboBoxGroup.DisplayMember = "String";
      this.m_comboBoxGroup.FormattingEnabled = true;
      this.m_comboBoxGroup.Location = new System.Drawing.Point(104, 67);
      this.m_comboBoxGroup.Name = "m_comboBoxGroup";
      this.m_comboBoxGroup.Size = new System.Drawing.Size(160, 21);
      this.m_comboBoxGroup.TabIndex = 22;
      this.m_comboBoxGroup.ValueMember = "Object";
      // 
      // ImportStudyInfo
      // 
      this.Controls.Add(this.m_comboBoxGroup);
      this.Controls.Add(this.m_labelGroup);
      this.Controls.Add(this.m_pysicianGroup);
      this.Controls.Add(this.m_secondDivider);
      this.Controls.Add(this.m_firstDivider);
      this.Controls.Add(this.m_date);
      this.Controls.Add(this.m_dateLabel);
      this.Controls.Add(this.m_description);
      this.Controls.Add(this.m_descriptionLabel);
      this.Controls.Add(this.m_studyInfoLabel);
      this.Name = "ImportStudyInfo";
      this.Size = new System.Drawing.Size(448, 344);
      this.m_pysicianGroup.ResumeLayout(false);
      this.m_pysicianGroup.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion


  } // class ImportStudyInfo
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: ImportStudyInfo.cs,v $
// Revision 1.6.2.1  2008/01/31 21:01:58  mkontak
// Added GROUP setting for study
//
// Revision 1.6  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.5  2007/03/06 20:30:57  gdavidson
// Corrected miss spelling
//
// Revision 1.4  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.3  2007/03/06 15:04:59  gdavidson
// Issue #5442: Split referring physician into two fields
//
// Revision 1.2  2007/02/07 21:50:20  gdavidson
// Added comments
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.6  2006/02/07 18:06:50  gdavidson
// Modified the import dialog
//
// Revision 1.5  2005/11/07 16:35:56  gdavidson
// Removed IImportInfo interface.
//
// Revision 1.4  2005/08/31 18:57:55  romy
// did add  namesapce to PersonName struct
//
// Revision 1.3  2005/05/26 17:44:22  gdavidson
// Changed the look of the dialog.
//
// Revision 1.2  2005/03/29 01:05:44  gdavidson
// Output ID as a string.
//
// Revision 1.1  2001/01/02 12:47:09  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/ImportStudyInfo.cs,v 1.6.2.1 2008/01/31 21:01:58 mkontak Exp $
// $Id: ImportStudyInfo.cs,v 1.6.2.1 2008/01/31 21:01:58 mkontak Exp $

#endregion