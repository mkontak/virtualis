// $Id: ImportPatientInfo.cs,v 1.7 2007/03/13 19:16:56 gdavidson Exp $
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
	/// User interface for providing patient information.
	/// </summary>
	public class ImportPatientInfo : System.Windows.Forms.UserControl
	{
   
    #region fields

    /// <summary>
    /// label
    /// </summary>
    private System.Windows.Forms.Label label1;

    /// <summary>
    /// group box
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox1;

    /// <summary>
    /// label
    /// </summary>
    private System.Windows.Forms.Label label6;

    /// <summary>
    /// grpup box
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox2;

    /// <summary>
    /// group box
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox3;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Textbox for the middle name.
    /// </summary>
    private System.Windows.Forms.TextBox m_middleName;

    /// <summary>
    /// Label for the middle name textbox.
    /// </summary>
    private System.Windows.Forms.Label m_middleNameLabel;

    /// <summary>
    /// Textbox for the last name.mm
    /// </summary>
    private System.Windows.Forms.TextBox m_lastname;

    /// <summary>
    /// Label for the last name textbox.
    /// </summary>
    private System.Windows.Forms.Label m_lastNameLabel;

    /// <summary>
    /// Textbox for the first name.
    /// </summary>
    private System.Windows.Forms.TextBox m_firstname;

    /// <summary>
    /// Lable for the first name textbox.
    /// </summary>
    private System.Windows.Forms.Label m_firstNameLabel;

    /// <summary>
    /// Textbox for the prefix.
    /// </summary>
    private System.Windows.Forms.TextBox m_prefix;

    /// <summary>
    /// Label for the prefix textbox.
    /// </summary>
    private System.Windows.Forms.Label m_prefixLabel;

    /// <summary>
    /// textbox for the suffix
    /// </summary>
    private System.Windows.Forms.TextBox m_suffix;

    /// <summary>
    /// label fro address
    /// </summary>
    private System.Windows.Forms.Label m_addressLabel;

    /// <summary>
    /// textbox for address
    /// </summary>
    private System.Windows.Forms.TextBox m_address1;

    /// <summary>
    /// textbox from address
    /// </summary>
    private System.Windows.Forms.TextBox m_address2;

    /// <summary>
    /// label for city
    /// </summary>
    private System.Windows.Forms.Label m_cityLabel;

    /// <summary>
    /// textbox for city
    /// </summary>
    private System.Windows.Forms.TextBox m_city;

    /// <summary>
    /// label for state
    /// </summary>
    private System.Windows.Forms.Label m_stateLabel;

    /// <summary>
    /// textbox for state
    /// </summary>
    private System.Windows.Forms.TextBox m_state;

    /// <summary>
    /// label for zipcode
    /// </summary>
    private System.Windows.Forms.Label m_zipcodeLabel;

    /// <summary>
    /// textbox for zipcode
    /// </summary>
    private System.Windows.Forms.TextBox m_zipcode;

    /// <summary>
    /// textbox for weight
    /// </summary>
    private System.Windows.Forms.TextBox m_weight;

    /// <summary>
    /// label for weight
    /// </summary>
    private System.Windows.Forms.Label m_weightLabel;

    /// <summary>
    /// label for DOB
    /// </summary>
    private System.Windows.Forms.Label m_dobLabel;

    /// <summary>
    /// label for gender
    /// </summary>
    private System.Windows.Forms.Label m_genderLabel;

    /// <summary>
    /// textbox for date of birth
    /// </summary>
    private TextBox m_dob;

    /// <summary>
    /// combobox for gender
    /// </summary>
    private System.Windows.Forms.ComboBox m_gender;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a ImportPatientInfo control.
    /// </summary>
    /// <param name="patientNode">TreeNode containing display information.</param>
		public ImportPatientInfo()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
		} // ImportPatientInfo( patientNode )

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
		} // Dispose( disposing )


    /// <summary>
    /// Initialize from a patient
    /// </summary>
    /// <param name="patient">Patient object</param>
    public void Initialize( Patient patient )
    {
      m_prefix.Text = patient.Name.Prefix;
      m_firstname.Text = patient.Name.FirstName;
      m_lastname.Text = patient.Name.LastName;
      m_middleName.Text = patient.Name.MiddleName;
      m_suffix.Text = patient.Name.Suffix;

      m_address1.Text = patient.Address.Line1;
      m_address2.Text = patient.Address.Line2;
      m_city.Text = patient.Address.City;
      m_state.Text = patient.Address.State;
      m_zipcode.Text = patient.Address.ZipCode;
      m_weight.Text = patient.Weight.ToString();

      m_gender.Items.Add( Gender.Unknown );
      m_gender.Items.Add( Gender.Male );
      m_gender.Items.Add( Gender.Female );
      m_gender.Items.Add( Gender.Other );
      m_gender.SelectedIndex = 0;

      m_dob.Text = patient.Dob.ToShortDateString();
    } // Initialize( patient )


    /// <summary>
    /// Saves information to a patient object
    /// </summary>
    /// <param name="patient"></param>
    public void Save( Patient patient )
    {
//      patient.Name.Prefix = m_prefix.Text;
//      patient.Name.FirstName = m_firstname.Text;
//      patient.Name.LastName = m_lastname.Text;
//      patient.Name.MiddleName = m_middleName.Text;
//      patient.Name.Suffix = m_suffix.Text;

      patient.Address.Line1 = m_address1.Text;
      patient.Address.Line2 = m_address2.Text;
      patient.Address.City = m_city.Text;
      patient.Address.State = m_state.Text;
      patient.Address.ZipCode = m_zipcode.Text;

      if( m_gender.SelectedItem != null )
        patient.Gender = (Gender) m_gender.SelectedItem;

      try
      {
        patient.Weight = int.Parse( m_weight.Text );
      }
      catch( Exception )
      {}
    } // Save( patient )


    /// <summary>
    /// Resets the control
    /// </summary>
    public void Reset()
    {
      m_prefix.Text = string.Empty;
      m_firstname.Text = string.Empty;
      m_lastname.Text = string.Empty;
      m_middleName.Text = string.Empty;
      m_suffix.Text = string.Empty;

      m_address1.Text = string.Empty;
      m_address2.Text = string.Empty;
      m_city.Text = string.Empty;
      m_state.Text = string.Empty;
      m_zipcode.Text = string.Empty;
      m_weight.Text = string.Empty;

      m_gender.Items.Clear();
    } // Reset()

    #endregion

 		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.m_middleName = new System.Windows.Forms.TextBox();
      this.m_middleNameLabel = new System.Windows.Forms.Label();
      this.m_lastname = new System.Windows.Forms.TextBox();
      this.m_lastNameLabel = new System.Windows.Forms.Label();
      this.m_firstname = new System.Windows.Forms.TextBox();
      this.m_firstNameLabel = new System.Windows.Forms.Label();
      this.m_prefix = new System.Windows.Forms.TextBox();
      this.m_prefixLabel = new System.Windows.Forms.Label();
      this.m_suffix = new System.Windows.Forms.TextBox();
      this.label6 = new System.Windows.Forms.Label();
      this.m_addressLabel = new System.Windows.Forms.Label();
      this.m_address1 = new System.Windows.Forms.TextBox();
      this.m_address2 = new System.Windows.Forms.TextBox();
      this.m_cityLabel = new System.Windows.Forms.Label();
      this.m_city = new System.Windows.Forms.TextBox();
      this.m_stateLabel = new System.Windows.Forms.Label();
      this.m_state = new System.Windows.Forms.TextBox();
      this.m_zipcodeLabel = new System.Windows.Forms.Label();
      this.m_zipcode = new System.Windows.Forms.TextBox();
      this.m_weight = new System.Windows.Forms.TextBox();
      this.m_weightLabel = new System.Windows.Forms.Label();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.m_dobLabel = new System.Windows.Forms.Label();
      this.m_genderLabel = new System.Windows.Forms.Label();
      this.m_gender = new System.Windows.Forms.ComboBox();
      this.m_dob = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // label1
      // 
      this.label1.Font = new System.Drawing.Font( "Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ( (byte) ( 0 ) ) );
      this.label1.ForeColor = System.Drawing.Color.White;
      this.label1.Location = new System.Drawing.Point( 8, 0 );
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size( 216, 23 );
      this.label1.TabIndex = 0;
      this.label1.Text = "Patient Information";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.groupBox1.Location = new System.Drawing.Point( 8, 24 );
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size( 428, 8 );
      this.groupBox1.TabIndex = 1;
      this.groupBox1.TabStop = false;
      // 
      // m_middleName
      // 
      this.m_middleName.Location = new System.Drawing.Point( 344, 64 );
      this.m_middleName.Name = "m_middleName";
      this.m_middleName.ReadOnly = true;
      this.m_middleName.Size = new System.Drawing.Size( 88, 20 );
      this.m_middleName.TabIndex = 3;
      // 
      // m_middleNameLabel
      // 
      this.m_middleNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_middleNameLabel.Location = new System.Drawing.Point( 296, 64 );
      this.m_middleNameLabel.Name = "m_middleNameLabel";
      this.m_middleNameLabel.Size = new System.Drawing.Size( 48, 23 );
      this.m_middleNameLabel.TabIndex = 7;
      this.m_middleNameLabel.Text = "Middle:";
      this.m_middleNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_lastname
      // 
      this.m_lastname.Location = new System.Drawing.Point( 56, 88 );
      this.m_lastname.Name = "m_lastname";
      this.m_lastname.ReadOnly = true;
      this.m_lastname.Size = new System.Drawing.Size( 224, 20 );
      this.m_lastname.TabIndex = 4;
      // 
      // m_lastNameLabel
      // 
      this.m_lastNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_lastNameLabel.Location = new System.Drawing.Point( 16, 88 );
      this.m_lastNameLabel.Name = "m_lastNameLabel";
      this.m_lastNameLabel.Size = new System.Drawing.Size( 40, 23 );
      this.m_lastNameLabel.TabIndex = 5;
      this.m_lastNameLabel.Text = "Last:";
      this.m_lastNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_firstname
      // 
      this.m_firstname.Location = new System.Drawing.Point( 56, 64 );
      this.m_firstname.Name = "m_firstname";
      this.m_firstname.ReadOnly = true;
      this.m_firstname.Size = new System.Drawing.Size( 224, 20 );
      this.m_firstname.TabIndex = 2;
      // 
      // m_firstNameLabel
      // 
      this.m_firstNameLabel.ForeColor = System.Drawing.Color.White;
      this.m_firstNameLabel.Location = new System.Drawing.Point( 16, 64 );
      this.m_firstNameLabel.Name = "m_firstNameLabel";
      this.m_firstNameLabel.Size = new System.Drawing.Size( 40, 23 );
      this.m_firstNameLabel.TabIndex = 3;
      this.m_firstNameLabel.Text = "First:";
      this.m_firstNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_prefix
      // 
      this.m_prefix.Location = new System.Drawing.Point( 56, 40 );
      this.m_prefix.Name = "m_prefix";
      this.m_prefix.ReadOnly = true;
      this.m_prefix.Size = new System.Drawing.Size( 48, 20 );
      this.m_prefix.TabIndex = 1;
      // 
      // m_prefixLabel
      // 
      this.m_prefixLabel.ForeColor = System.Drawing.Color.White;
      this.m_prefixLabel.Location = new System.Drawing.Point( 16, 40 );
      this.m_prefixLabel.Name = "m_prefixLabel";
      this.m_prefixLabel.Size = new System.Drawing.Size( 40, 23 );
      this.m_prefixLabel.TabIndex = 0;
      this.m_prefixLabel.Text = "Prefix:";
      this.m_prefixLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_suffix
      // 
      this.m_suffix.Location = new System.Drawing.Point( 344, 88 );
      this.m_suffix.Name = "m_suffix";
      this.m_suffix.ReadOnly = true;
      this.m_suffix.Size = new System.Drawing.Size( 48, 20 );
      this.m_suffix.TabIndex = 5;
      // 
      // label6
      // 
      this.label6.ForeColor = System.Drawing.Color.White;
      this.label6.Location = new System.Drawing.Point( 296, 88 );
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size( 40, 23 );
      this.label6.TabIndex = 8;
      this.label6.Text = "Suffix:";
      this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_addressLabel
      // 
      this.m_addressLabel.ForeColor = System.Drawing.Color.White;
      this.m_addressLabel.Location = new System.Drawing.Point( 16, 128 );
      this.m_addressLabel.Name = "m_addressLabel";
      this.m_addressLabel.Size = new System.Drawing.Size( 56, 23 );
      this.m_addressLabel.TabIndex = 11;
      this.m_addressLabel.Text = "Address:";
      this.m_addressLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_address1
      // 
      this.m_address1.Location = new System.Drawing.Point( 72, 128 );
      this.m_address1.Name = "m_address1";
      this.m_address1.Size = new System.Drawing.Size( 360, 20 );
      this.m_address1.TabIndex = 6;
      // 
      // m_address2
      // 
      this.m_address2.Location = new System.Drawing.Point( 72, 152 );
      this.m_address2.Name = "m_address2";
      this.m_address2.Size = new System.Drawing.Size( 360, 20 );
      this.m_address2.TabIndex = 7;
      // 
      // m_cityLabel
      // 
      this.m_cityLabel.ForeColor = System.Drawing.Color.White;
      this.m_cityLabel.Location = new System.Drawing.Point( 40, 176 );
      this.m_cityLabel.Name = "m_cityLabel";
      this.m_cityLabel.Size = new System.Drawing.Size( 32, 23 );
      this.m_cityLabel.TabIndex = 14;
      this.m_cityLabel.Text = "City:";
      this.m_cityLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_city
      // 
      this.m_city.Location = new System.Drawing.Point( 72, 176 );
      this.m_city.Name = "m_city";
      this.m_city.Size = new System.Drawing.Size( 120, 20 );
      this.m_city.TabIndex = 8;
      // 
      // m_stateLabel
      // 
      this.m_stateLabel.ForeColor = System.Drawing.Color.White;
      this.m_stateLabel.Location = new System.Drawing.Point( 200, 176 );
      this.m_stateLabel.Name = "m_stateLabel";
      this.m_stateLabel.Size = new System.Drawing.Size( 48, 23 );
      this.m_stateLabel.TabIndex = 16;
      this.m_stateLabel.Text = "State:";
      this.m_stateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_state
      // 
      this.m_state.Location = new System.Drawing.Point( 240, 176 );
      this.m_state.Name = "m_state";
      this.m_state.Size = new System.Drawing.Size( 48, 20 );
      this.m_state.TabIndex = 9;
      // 
      // m_zipcodeLabel
      // 
      this.m_zipcodeLabel.ForeColor = System.Drawing.Color.White;
      this.m_zipcodeLabel.Location = new System.Drawing.Point( 296, 176 );
      this.m_zipcodeLabel.Name = "m_zipcodeLabel";
      this.m_zipcodeLabel.Size = new System.Drawing.Size( 56, 23 );
      this.m_zipcodeLabel.TabIndex = 18;
      this.m_zipcodeLabel.Text = "Zip Code:";
      this.m_zipcodeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_zipcode
      // 
      this.m_zipcode.Location = new System.Drawing.Point( 344, 176 );
      this.m_zipcode.Name = "m_zipcode";
      this.m_zipcode.Size = new System.Drawing.Size( 88, 20 );
      this.m_zipcode.TabIndex = 10;
      // 
      // m_weight
      // 
      this.m_weight.Location = new System.Drawing.Point( 312, 216 );
      this.m_weight.Name = "m_weight";
      this.m_weight.Size = new System.Drawing.Size( 72, 20 );
      this.m_weight.TabIndex = 13;
      // 
      // m_weightLabel
      // 
      this.m_weightLabel.ForeColor = System.Drawing.Color.White;
      this.m_weightLabel.Location = new System.Drawing.Point( 240, 216 );
      this.m_weightLabel.Name = "m_weightLabel";
      this.m_weightLabel.Size = new System.Drawing.Size( 72, 23 );
      this.m_weightLabel.TabIndex = 23;
      this.m_weightLabel.Text = "Weight (lbs):";
      this.m_weightLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // groupBox2
      // 
      this.groupBox2.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.groupBox2.Location = new System.Drawing.Point( 8, 112 );
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size( 428, 8 );
      this.groupBox2.TabIndex = 25;
      this.groupBox2.TabStop = false;
      // 
      // groupBox3
      // 
      this.groupBox3.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.groupBox3.Location = new System.Drawing.Point( 8, 200 );
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size( 428, 8 );
      this.groupBox3.TabIndex = 26;
      this.groupBox3.TabStop = false;
      // 
      // m_dobLabel
      // 
      this.m_dobLabel.ForeColor = System.Drawing.Color.White;
      this.m_dobLabel.Location = new System.Drawing.Point( 16, 216 );
      this.m_dobLabel.Name = "m_dobLabel";
      this.m_dobLabel.Size = new System.Drawing.Size( 72, 23 );
      this.m_dobLabel.TabIndex = 28;
      this.m_dobLabel.Text = "Date of Birth:";
      this.m_dobLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_genderLabel
      // 
      this.m_genderLabel.ForeColor = System.Drawing.Color.White;
      this.m_genderLabel.Location = new System.Drawing.Point( 16, 240 );
      this.m_genderLabel.Name = "m_genderLabel";
      this.m_genderLabel.Size = new System.Drawing.Size( 72, 23 );
      this.m_genderLabel.TabIndex = 29;
      this.m_genderLabel.Text = "Gender:";
      this.m_genderLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_gender
      // 
      this.m_gender.Location = new System.Drawing.Point( 96, 241 );
      this.m_gender.Name = "m_gender";
      this.m_gender.Size = new System.Drawing.Size( 104, 21 );
      this.m_gender.TabIndex = 14;
      // 
      // m_dob
      // 
      this.m_dob.Location = new System.Drawing.Point( 96, 214 );
      this.m_dob.Name = "m_dob";
      this.m_dob.ReadOnly = true;
      this.m_dob.Size = new System.Drawing.Size( 104, 20 );
      this.m_dob.TabIndex = 12;
      // 
      // ImportPatientInfo
      // 
      this.Controls.Add( this.m_dob );
      this.Controls.Add( this.m_gender );
      this.Controls.Add( this.m_genderLabel );
      this.Controls.Add( this.m_dobLabel );
      this.Controls.Add( this.groupBox3 );
      this.Controls.Add( this.groupBox2 );
      this.Controls.Add( this.m_weight );
      this.Controls.Add( this.m_weightLabel );
      this.Controls.Add( this.m_zipcode );
      this.Controls.Add( this.m_zipcodeLabel );
      this.Controls.Add( this.m_state );
      this.Controls.Add( this.m_stateLabel );
      this.Controls.Add( this.m_city );
      this.Controls.Add( this.m_cityLabel );
      this.Controls.Add( this.m_address2 );
      this.Controls.Add( this.m_address1 );
      this.Controls.Add( this.m_addressLabel );
      this.Controls.Add( this.groupBox1 );
      this.Controls.Add( this.label1 );
      this.Controls.Add( this.m_lastname );
      this.Controls.Add( this.m_prefixLabel );
      this.Controls.Add( this.m_prefix );
      this.Controls.Add( this.m_firstNameLabel );
      this.Controls.Add( this.m_firstname );
      this.Controls.Add( this.m_lastNameLabel );
      this.Controls.Add( this.label6 );
      this.Controls.Add( this.m_suffix );
      this.Controls.Add( this.m_middleNameLabel );
      this.Controls.Add( this.m_middleName );
      this.Name = "ImportPatientInfo";
      this.Size = new System.Drawing.Size( 448, 280 );
      this.ResumeLayout( false );
      this.PerformLayout();

    }
		#endregion

  } // class ImportPatientInfo
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: ImportPatientInfo.cs,v $
// Revision 1.7  2007/03/13 19:16:56  gdavidson
// Issue #5478: Updated tab ordering
//
// Revision 1.6  2007/03/09 21:38:26  gdavidson
// Issue #5471: Made DOB readonly
//
// Revision 1.5  2007/03/07 19:17:12  gdavidson
// Issue #5459: Set the dob for the patient
//
// Revision 1.4  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/21 21:17:37  gdavidson
// Commented code
//
// Revision 1.2  2007/01/17 16:04:51  gdavidson
// Issue #5297: Fixed tab order
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.10  2006/02/07 18:06:50  gdavidson
// Modified the import dialog
//
// Revision 1.9  2005/11/23 20:07:51  gdavidson
// Removed saving of readonly data
//
// Revision 1.8  2005/11/21 20:47:12  gdavidson
// Fixed label for zip code
//
// Revision 1.7  2005/11/17 23:12:33  gdavidson
// Made the patient name fields readonly
//
// Revision 1.6  2005/11/07 16:35:56  gdavidson
// Removed IImportInfo interface.
//
// Revision 1.5  2005/08/31 18:57:55  romy
// did add  namesapce to PersonName struct
//
// Revision 1.4  2005/05/26 17:44:22  gdavidson
// Changed the look of the dialog.
//
// Revision 1.3  2005/05/20 14:25:35  mkontak
// no message
//
// Revision 1.2  2005/04/19 19:56:09  gdavidson
// Commented code.
//
// Revision 1.1  2001/01/02 12:47:09  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/ImportPatientInfo.cs,v 1.7 2007/03/13 19:16:56 gdavidson Exp $
// $Id: ImportPatientInfo.cs,v 1.7 2007/03/13 19:16:56 gdavidson Exp $

#endregion