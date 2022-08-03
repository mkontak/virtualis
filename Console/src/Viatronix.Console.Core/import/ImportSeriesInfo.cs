// $Id: ImportSeriesInfo.cs,v 1.5.2.2 2007/03/19 18:04:28 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.IO;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using Viatronix.v3D.Core;
using Viatronix.v3D.UI;
using System.Globalization;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// User interface for providing series information.
	/// </summary>
	public class ImportSeriesInfo : System.Windows.Forms.UserControl
	{

    #region fields

    /// <summary>
    /// textbox for description
    /// </summary>
    private System.Windows.Forms.TextBox m_description;

    /// <summary>
    /// label for description
    /// </summary>
    private System.Windows.Forms.Label m_descriptionLabel;

    /// <summary>
    /// label for series info
    /// </summary>
    private System.Windows.Forms.Label m_seriesInfoLabel;

    /// <summary>
    /// label
    /// </summary>
    private System.Windows.Forms.Label label1;

    /// <summary>
    /// groupbox
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox2;

    /// <summary>
    /// label for orientation
    /// </summary>
    private System.Windows.Forms.Label m_orientationLabel;

    /// <summary>
    /// textbox for orientation
    /// </summary>
    private System.Windows.Forms.TextBox m_orientation;

    /// <summary>
    /// groupbox
    /// </summary>
    private System.Windows.Forms.GroupBox groupBox3;

    /// <summary>
    /// textbox for modality
    /// </summary>
    private System.Windows.Forms.TextBox m_modality;

    /// <summary>
    /// label for modality
    /// </summary>
    private System.Windows.Forms.Label m_modalityLabel;

    /// <summary>
    /// textbox for subtype
    /// </summary>
    private System.Windows.Forms.TextBox m_subtype;

    /// <summary>
    /// label for subtype
    /// </summary>
    private System.Windows.Forms.Label m_subtypeLabel;

    /// <summary>
    /// textbox for series type
    /// </summary>
    private System.Windows.Forms.TextBox m_seriesType;

    /// <summary>
    /// label for series type
    /// </summary>
    private System.Windows.Forms.Label m_seriesTypeLabel;

    /// <summary>
    /// Label for state
    /// </summary>
    private System.Windows.Forms.Label m_stateLabel;

    /// <summary>
    /// label for image count
    /// </summary>
    private System.Windows.Forms.Label m_imageCountLabel;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// textbox for acquisition number
    /// </summary>
    private System.Windows.Forms.TextBox m_acquisitionNumber;

    /// <summary>
    /// textbox for image count
    /// </summary>
    private System.Windows.Forms.TextBox m_imageCount;

    /// <summary>
    /// textbox for state
    /// </summary>
    private System.Windows.Forms.TextBox m_state;

    /// <summary>
    /// Initialization series
    /// </summary>
    private Series m_series;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
		public ImportSeriesInfo()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

			InitializeComponent();
		} // ImportSeriesInfo()

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
    /// Saves information to a series
    /// </summary>
    /// <param name="series"></param>
    public void Save( Series series )
    {
      series.AcquisitionNumber = long.Parse( m_acquisitionNumber.Text );
      series.Description = m_description.Text;
      series.Modality = m_modality.Text;
//      series.Type = m_seriesType.Text;
//      series.SubType = m_subtype.Text;
//      series.State = m_state.Text;
//      series.PatientOrientation = m_orientation.Text;

      try
      {
        series.ImageCount = int.Parse( m_imageCount.Text );
      }
      catch
      {
        series.ImageCount = 0;
      }
    } // Save( series )


    /// <summary>
    /// Initializes control from a series
    /// </summary>
    /// <param name="series">Series object</param>
    public void Initialize( Series series )
    {
      m_series = series;
      m_acquisitionNumber.Text = series.AcquisitionNumber.ToString();
      m_description.Text = series.Description;

      ///////////////////////////////////////////////////////////////////////////
      // Set the modality and only allow the user to enter a new one otherwise 
      // do not let the users override what we have found.
      ////////////////////////////////////////////////////////////////////////////
      m_modality.Text = series.Modality.ToUpper();
      if (m_modality.Text.Length > 0)
        m_modality.ReadOnly = true;
      else
        m_modality.ReadOnly = false;

      m_seriesType.Text = series.Type;
      m_subtype.Text = series.SubType;
      m_state.Text = series.State;
      m_imageCount.Text = series.ImageCount.ToString();
      m_orientation.Text = series.PatientOrientation;
    } // Initialize( series )


    /// <summary>
    /// Resets the control
    /// </summary>
    public void Reset()
    {
      m_acquisitionNumber.Text = string.Empty;
      m_description.Text = string.Empty;
      m_modality.Text = string.Empty;
      m_seriesType.Text = string.Empty;
      m_subtype.Text = string.Empty;
      m_state.Text = string.Empty;
      m_imageCount.Text = string.Empty;
      m_orientation.Text = string.Empty;
    } // Reset()

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_description = new System.Windows.Forms.TextBox();
      this.m_descriptionLabel = new System.Windows.Forms.Label();
      this.m_seriesInfoLabel = new System.Windows.Forms.Label();
      this.m_acquisitionNumber = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.m_orientationLabel = new System.Windows.Forms.Label();
      this.m_orientation = new System.Windows.Forms.TextBox();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.m_modality = new System.Windows.Forms.TextBox();
      this.m_modalityLabel = new System.Windows.Forms.Label();
      this.m_subtype = new System.Windows.Forms.TextBox();
      this.m_subtypeLabel = new System.Windows.Forms.Label();
      this.m_seriesType = new System.Windows.Forms.TextBox();
      this.m_seriesTypeLabel = new System.Windows.Forms.Label();
      this.m_state = new System.Windows.Forms.TextBox();
      this.m_stateLabel = new System.Windows.Forms.Label();
      this.m_imageCount = new System.Windows.Forms.TextBox();
      this.m_imageCountLabel = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // m_description
      // 
      this.m_description.Location = new System.Drawing.Point( 90, 64 );
      this.m_description.Name = "m_description";
      this.m_description.Size = new System.Drawing.Size( 344, 20 );
      this.m_description.TabIndex = 2;
      // 
      // m_descriptionLabel
      // 
      this.m_descriptionLabel.ForeColor = System.Drawing.Color.White;
      this.m_descriptionLabel.Location = new System.Drawing.Point( 16, 64 );
      this.m_descriptionLabel.Name = "m_descriptionLabel";
      this.m_descriptionLabel.Size = new System.Drawing.Size( 64, 23 );
      this.m_descriptionLabel.TabIndex = 32;
      this.m_descriptionLabel.Text = "Description:";
      this.m_descriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_seriesInfoLabel
      // 
      this.m_seriesInfoLabel.Font = new System.Drawing.Font( "Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ( (byte) ( 0 ) ) );
      this.m_seriesInfoLabel.ForeColor = System.Drawing.Color.White;
      this.m_seriesInfoLabel.Location = new System.Drawing.Point( 8, 0 );
      this.m_seriesInfoLabel.Name = "m_seriesInfoLabel";
      this.m_seriesInfoLabel.Size = new System.Drawing.Size( 216, 23 );
      this.m_seriesInfoLabel.TabIndex = 29;
      this.m_seriesInfoLabel.Text = "Series Information";
      this.m_seriesInfoLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_acquisitionNumber
      // 
      this.m_acquisitionNumber.Location = new System.Drawing.Point( 90, 40 );
      this.m_acquisitionNumber.Name = "m_acquisitionNumber";
      this.m_acquisitionNumber.Size = new System.Drawing.Size( 344, 20 );
      this.m_acquisitionNumber.TabIndex = 1;
      this.m_acquisitionNumber.Validating += new CancelEventHandler(this.OnValidatingAcquisitionNumberHandler);
      // 
      // label1
      // 
      this.label1.ForeColor = System.Drawing.Color.White;
      this.label1.Location = new System.Drawing.Point( 16, 40 );
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size( 72, 23 );
      this.label1.TabIndex = 38;
      this.label1.Text = "Acquisition #:";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // groupBox2
      // 
      this.groupBox2.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.groupBox2.Location = new System.Drawing.Point( 8, 24 );
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size( 428, 8 );
      this.groupBox2.TabIndex = 40;
      this.groupBox2.TabStop = false;
      // 
      // m_orientationLabel
      // 
      this.m_orientationLabel.ForeColor = System.Drawing.Color.White;
      this.m_orientationLabel.Location = new System.Drawing.Point( 16, 104 );
      this.m_orientationLabel.Name = "m_orientationLabel";
      this.m_orientationLabel.Size = new System.Drawing.Size( 64, 23 );
      this.m_orientationLabel.TabIndex = 41;
      this.m_orientationLabel.Text = "Orientation:";
      this.m_orientationLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_orientation
      // 
      this.m_orientation.Location = new System.Drawing.Point( 80, 104 );
      this.m_orientation.Name = "m_orientation";
      this.m_orientation.ReadOnly = true;
      this.m_orientation.Size = new System.Drawing.Size( 96, 20 );
      this.m_orientation.TabIndex = 3;
      // 
      // groupBox3
      // 
      this.groupBox3.Anchor = ( (System.Windows.Forms.AnchorStyles) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                  | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.groupBox3.Location = new System.Drawing.Point( 8, 88 );
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size( 428, 8 );
      this.groupBox3.TabIndex = 43;
      this.groupBox3.TabStop = false;
      // 
      // m_modality
      // 
      this.m_modality.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
      this.m_modality.Location = new System.Drawing.Point( 248, 104 );
      this.m_modality.Name = "m_modality";
      this.m_modality.Size = new System.Drawing.Size( 40, 20 );
      this.m_modality.TabIndex = 4;
      // 
      // m_modalityLabel
      // 
      this.m_modalityLabel.ForeColor = System.Drawing.Color.White;
      this.m_modalityLabel.Location = new System.Drawing.Point( 184, 104 );
      this.m_modalityLabel.Name = "m_modalityLabel";
      this.m_modalityLabel.Size = new System.Drawing.Size( 56, 23 );
      this.m_modalityLabel.TabIndex = 44;
      this.m_modalityLabel.Text = "Modality:";
      this.m_modalityLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_subtype
      // 
      this.m_subtype.Location = new System.Drawing.Point( 248, 128 );
      this.m_subtype.Name = "m_subtype";
      this.m_subtype.ReadOnly = true;
      this.m_subtype.Size = new System.Drawing.Size( 48, 20 );
      this.m_subtype.TabIndex = 7;
      // 
      // m_subtypeLabel
      // 
      this.m_subtypeLabel.ForeColor = System.Drawing.Color.White;
      this.m_subtypeLabel.Location = new System.Drawing.Point( 184, 128 );
      this.m_subtypeLabel.Name = "m_subtypeLabel";
      this.m_subtypeLabel.Size = new System.Drawing.Size( 64, 23 );
      this.m_subtypeLabel.TabIndex = 48;
      this.m_subtypeLabel.Text = "Application:";
      this.m_subtypeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_seriesType
      // 
      this.m_seriesType.Location = new System.Drawing.Point( 80, 128 );
      this.m_seriesType.Name = "m_seriesType";
      this.m_seriesType.ReadOnly = true;
      this.m_seriesType.Size = new System.Drawing.Size( 96, 20 );
      this.m_seriesType.TabIndex = 6;
      // 
      // m_seriesTypeLabel
      // 
      this.m_seriesTypeLabel.ForeColor = System.Drawing.Color.White;
      this.m_seriesTypeLabel.Location = new System.Drawing.Point( 16, 128 );
      this.m_seriesTypeLabel.Name = "m_seriesTypeLabel";
      this.m_seriesTypeLabel.Size = new System.Drawing.Size( 72, 23 );
      this.m_seriesTypeLabel.TabIndex = 46;
      this.m_seriesTypeLabel.Text = "Series Type:";
      this.m_seriesTypeLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_state
      // 
      this.m_state.Location = new System.Drawing.Point( 344, 104 );
      this.m_state.Name = "m_state";
      this.m_state.ReadOnly = true;
      this.m_state.Size = new System.Drawing.Size( 88, 20 );
      this.m_state.TabIndex = 5;
      // 
      // m_stateLabel
      // 
      this.m_stateLabel.ForeColor = System.Drawing.Color.White;
      this.m_stateLabel.Location = new System.Drawing.Point( 312, 104 );
      this.m_stateLabel.Name = "m_stateLabel";
      this.m_stateLabel.Size = new System.Drawing.Size( 48, 23 );
      this.m_stateLabel.TabIndex = 50;
      this.m_stateLabel.Text = "State:";
      this.m_stateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_imageCount
      // 
      this.m_imageCount.Location = new System.Drawing.Point( 384, 128 );
      this.m_imageCount.Name = "m_imageCount";
      this.m_imageCount.Size = new System.Drawing.Size( 48, 20 );
      this.m_imageCount.TabIndex = 8;
      this.m_imageCount.Validating += new CancelEventHandler(this.OnValidatingImageCountHandler);
      // 
      // m_imageCountLabel
      // 
      this.m_imageCountLabel.ForeColor = System.Drawing.Color.White;
      this.m_imageCountLabel.Location = new System.Drawing.Point( 312, 128 );
      this.m_imageCountLabel.Name = "m_imageCountLabel";
      this.m_imageCountLabel.Size = new System.Drawing.Size( 72, 23 );
      this.m_imageCountLabel.TabIndex = 52;
      this.m_imageCountLabel.Text = "Image Count:";
      this.m_imageCountLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // ImportSeriesInfo
      // 
      this.Controls.Add( this.m_imageCount );
      this.Controls.Add( this.m_imageCountLabel );
      this.Controls.Add( this.m_state );
      this.Controls.Add( this.m_stateLabel );
      this.Controls.Add( this.m_subtype );
      this.Controls.Add( this.m_subtypeLabel );
      this.Controls.Add( this.m_seriesType );
      this.Controls.Add( this.m_seriesTypeLabel );
      this.Controls.Add( this.m_modality );
      this.Controls.Add( this.m_modalityLabel );
      this.Controls.Add( this.groupBox3 );
      this.Controls.Add( this.m_orientation );
      this.Controls.Add( this.m_orientationLabel );
      this.Controls.Add( this.groupBox2 );
      this.Controls.Add( this.m_acquisitionNumber );
      this.Controls.Add( this.label1 );
      this.Controls.Add( this.m_description );
      this.Controls.Add( this.m_descriptionLabel );
      this.Controls.Add( this.m_seriesInfoLabel );
      this.Name = "ImportSeriesInfo";
      this.Size = new System.Drawing.Size( 448, 280 );
      this.ResumeLayout( false );
      this.PerformLayout();

    }
		#endregion


    #region evenat handlers

    /// <summary>
    /// Validates the acqusistion number
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="args">event args</param>
    private void OnValidatingAcquisitionNumberHandler(object sender, CancelEventArgs args)
    {

      args.Cancel = false;

      Series series = new Series();

      try
      {
        long an = long.Parse(m_acquisitionNumber.Text);
      }
      catch (Exception )
      {
        UI.MessageBox.ShowError("Invalid value in acquisition number, must be numeric");
        m_acquisitionNumber.Text = m_series.AcquisitionNumber.ToString();
        args.Cancel = true;
      }

    } // OnValidatingAcquisitionNumberHandler(object sender, CancelEventArgs args)

    /// <summary>
    /// Validates the image count
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="args">event args</param>
    private void OnValidatingImageCountHandler(object sender, CancelEventArgs args)
    {

      args.Cancel = false;

      Series series = new Series();

      try
      {
        int ic = int.Parse(m_imageCount.Text);
      }
      catch (Exception )
      {
        UI.MessageBox.ShowError("Invalid value in image count must be numeric");
        m_imageCount.Text = m_series.ImageCount.ToString();
        args.Cancel = true;
      }

    } // OnValidatingImageCountHandler(object sender, CancelEventArgs args)


  

    #endregion

  } // class ImportSeriesInfo
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: ImportSeriesInfo.cs,v $
// Revision 1.5.2.2  2007/03/19 18:04:28  mkontak
// Do not allow the user to override the modality if we know what it is.
//
// Revision 1.5.2.1  2007/03/19 17:28:32  mkontak
// Validate the image count and acquisition number.
//
// Revision 1.5  2007/03/08 18:09:48  gdavidson
// Issue #5468: Save modality
//
// Revision 1.4  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/21 21:21:34  gdavidson
// Commented code
//
// Revision 1.2  2007/01/17 16:04:52  gdavidson
// Issue #5297: Fixed tab order
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.10  2006/04/24 16:08:37  gdavidson
// Issue #4733: Made columns sortable and changed subtype to application
//
// Revision 1.9  2006/02/07 18:06:50  gdavidson
// Modified the import dialog
//
// Revision 1.8  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.7  2005/11/23 20:07:51  gdavidson
// Removed saving of readonly data
//
// Revision 1.6  2005/11/16 22:42:41  gdavidson
// Marked type, subtype, state and orientation fields readonly.
//
// Revision 1.5  2005/11/07 16:35:56  gdavidson
// Removed IImportInfo interface.
//
// Revision 1.4  2005/05/26 17:44:22  gdavidson
// Changed the look of the dialog.
//
// Revision 1.3  2005/04/19 20:02:34  gdavidson
// Replaced the Comboboxes with Textboxes
//
// Revision 1.2  2005/03/29 01:05:44  gdavidson
// Output ID as a string.
//
// Revision 1.1  2001/01/02 12:47:09  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/ImportSeriesInfo.cs,v 1.5.2.2 2007/03/19 18:04:28 mkontak Exp $
// $Id: ImportSeriesInfo.cs,v 1.5.2.2 2007/03/19 18:04:28 mkontak Exp $

#endregion