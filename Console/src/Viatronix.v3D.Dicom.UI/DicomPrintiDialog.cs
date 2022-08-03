// $Id: DicomPrintiDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $
//
// Copyright(c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: mark kontak( mark@viatronix.net )

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using vxDicom = Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Dicom.UI
{

  #region DicomPrintDialog


  /// <summary>
	/// Print Dialog used for setup of DICOM printers.
	/// </summary>
	public class DicomPrintDialog : System.Windows.Forms.Form
	{

    #region fields

    private Viatronix.v3D.UI.Button m_buttonCancel;
    private Viatronix.v3D.UI.Button m_buttonOK;
    private Printing.Printer m_DicomPrinter = null;
    private System.Windows.Forms.Label m_labelPolarity;
    private System.Windows.Forms.ComboBox m_comboBoxPolarity;
    private System.Windows.Forms.Label m_labelOrientation;
    private System.Windows.Forms.ComboBox m_comboBoxOrientation;
    private System.Windows.Forms.Label m_labelBorders;
    private System.Windows.Forms.ComboBox m_comboBoxBorders;
    private System.Windows.Forms.Label m_labelEmptyImages;
    private System.Windows.Forms.Label m_labelFormat;
    private System.Windows.Forms.GroupBox m_groupBoxPageSetup;
    private System.Windows.Forms.Label m_labelX;
    private System.Windows.Forms.Label m_labelPrinter;
    private System.Windows.Forms.ComboBox m_comboBoxPrinters;
    private System.Windows.Forms.GroupBox m_groupBoxSupport;
    private Viatronix.v3D.UI.CheckBox m_checkBoxColor;
    private Viatronix.v3D.UI.CheckBox m_checkBoxGrayscale;
    private System.Windows.Forms.GroupBox m_groupBoxAttributesw;
    private System.Windows.Forms.ListBox m_listBoxAttributes;
    private System.Windows.Forms.GroupBox m_groupBoxPrintJob;
    private System.Windows.Forms.ComboBox m_comboBoxSmoothing;
    private System.Windows.Forms.Label m_labelSmoothing;
    private System.Windows.Forms.ComboBox m_comboBoxInterpolation;
    private System.Windows.Forms.Label m_labelCopies;
    private System.Windows.Forms.ComboBox m_comboBoxEmptyImages;
    private HostTable m_hostTable;
    private System.Windows.Forms.NumericUpDown m_numericUpDownCopies;
    private System.Windows.Forms.NumericUpDown m_numericUpDownColumns;
    private System.Windows.Forms.NumericUpDown m_numericUpDownRows;
    private System.Windows.Forms.ComboBox m_comboBoxMediumTypes;
    private System.Windows.Forms.Label m_labelMediumType;
    private System.Windows.Forms.Label m_labelFilmSize;
    private System.Windows.Forms.Label m_labelMaxDensity;
    private System.Windows.Forms.Label m_labelMinDensity;
    private System.Windows.Forms.TextBox m_textBoxMinDensity;
    private System.Windows.Forms.TextBox m_textBoxMaxDensity;
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.ComboBox m_comboBoxFilmSize;
    private System.ComponentModel.IContainer components;
    private int m_iMaximumDensityAllowed = 0;
    private int m_iMinimumDensityAllowed = 0;
    private System.Windows.Forms.Label m_labelInterpolation;
    private System.Windows.Forms.Label m_labelDPI;
    private System.Windows.Forms.NumericUpDown m_numericUpDownDPI;

    #endregion

    #region properties

    /// <summary>
    /// Printer Name Properties
    /// 
    /// Read-Only property to extract the selected printer name after selection.
    /// </summary>
    public Printing.Printer DicomPrinter
    {
      get { return m_DicomPrinter; }
    } // END ... 

    
    #endregion

    #region implementation

    /// <summary>
    /// Constructor.
    /// </summary>
		public DicomPrintDialog()
		{

			//===========================================
			// Required for Windows Form Designer support
			//===========================================
			InitializeComponent();
     

      //=====================
      // Get the host table
      //=====================
      m_hostTable = new HostTable();

      //=============================================================================
      // Build the printer combo box based on the host table and the printer entries
      //=============================================================================
      for ( int i = 0; i < m_hostTable.Count; i++ )
      {

        Host host =  m_hostTable.get_Item(i);
        if ( host.Supports("PRINT") )
        {
          m_comboBoxPrinters.Items.Add(host.Name);
        }
      } // END ... for each host entry in the host table

 
      // Print Job Group Box
      m_numericUpDownCopies.Value = 1;
      m_numericUpDownColumns.Value = 1;
      m_numericUpDownRows.Value = 1;

      
		} // DicomPrintDialog()

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">Flag to indicate if the object is being disposed or not</param>
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


    #endregion

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      this.m_buttonCancel = new Viatronix.v3D.UI.Button();
      this.m_buttonOK = new Viatronix.v3D.UI.Button();
      this.m_labelPolarity = new System.Windows.Forms.Label();
      this.m_comboBoxPolarity = new System.Windows.Forms.ComboBox();
      this.m_labelOrientation = new System.Windows.Forms.Label();
      this.m_comboBoxOrientation = new System.Windows.Forms.ComboBox();
      this.m_labelBorders = new System.Windows.Forms.Label();
      this.m_comboBoxBorders = new System.Windows.Forms.ComboBox();
      this.m_labelEmptyImages = new System.Windows.Forms.Label();
      this.m_groupBoxPageSetup = new System.Windows.Forms.GroupBox();
      this.m_numericUpDownDPI = new System.Windows.Forms.NumericUpDown();
      this.m_labelDPI = new System.Windows.Forms.Label();
      this.m_textBoxMaxDensity = new System.Windows.Forms.TextBox();
      this.m_textBoxMinDensity = new System.Windows.Forms.TextBox();
      this.m_labelMinDensity = new System.Windows.Forms.Label();
      this.m_labelMaxDensity = new System.Windows.Forms.Label();
      this.m_labelFilmSize = new System.Windows.Forms.Label();
      this.m_comboBoxFilmSize = new System.Windows.Forms.ComboBox();
      this.m_numericUpDownRows = new System.Windows.Forms.NumericUpDown();
      this.m_numericUpDownColumns = new System.Windows.Forms.NumericUpDown();
      this.m_labelX = new System.Windows.Forms.Label();
      this.m_labelFormat = new System.Windows.Forms.Label();
      this.m_comboBoxEmptyImages = new System.Windows.Forms.ComboBox();
      this.m_comboBoxMediumTypes = new System.Windows.Forms.ComboBox();
      this.m_labelMediumType = new System.Windows.Forms.Label();
      this.m_labelPrinter = new System.Windows.Forms.Label();
      this.m_comboBoxPrinters = new System.Windows.Forms.ComboBox();
      this.m_groupBoxSupport = new System.Windows.Forms.GroupBox();
      this.m_checkBoxColor = new Viatronix.v3D.UI.CheckBox();
      this.m_checkBoxGrayscale = new Viatronix.v3D.UI.CheckBox();
      this.m_groupBoxAttributesw = new System.Windows.Forms.GroupBox();
      this.m_listBoxAttributes = new System.Windows.Forms.ListBox();
      this.m_groupBoxPrintJob = new System.Windows.Forms.GroupBox();
      this.m_numericUpDownCopies = new System.Windows.Forms.NumericUpDown();
      this.m_comboBoxSmoothing = new System.Windows.Forms.ComboBox();
      this.m_labelSmoothing = new System.Windows.Forms.Label();
      this.m_comboBoxInterpolation = new System.Windows.Forms.ComboBox();
      this.m_labelInterpolation = new System.Windows.Forms.Label();
      this.m_labelCopies = new System.Windows.Forms.Label();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_groupBoxPageSetup.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownDPI)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownRows)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownColumns)).BeginInit();
      this.m_groupBoxSupport.SuspendLayout();
      this.m_groupBoxAttributesw.SuspendLayout();
      this.m_groupBoxPrintJob.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownCopies)).BeginInit();
      this.SuspendLayout();
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                       null,
                                                                       null,
                                                                       null};
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonCancel.Location = new System.Drawing.Point(560, 376);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(96, 32);
      this.m_buttonCancel.TabIndex = 17;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.m_buttonCancel_Click);
      // 
      // m_buttonOK
      // 
      this.m_buttonOK.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_buttonOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
      this.m_buttonOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonOK.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonOK.Location = new System.Drawing.Point(432, 376);
      this.m_buttonOK.Name = "m_buttonOK";
      this.m_buttonOK.Size = new System.Drawing.Size(96, 32);
      this.m_buttonOK.TabIndex = 18;
      this.m_buttonOK.Text = "OK";
      this.m_buttonOK.Click += new System.EventHandler(this.m_buttonOK_Click);
      // 
      // m_labelPolarity
      // 
      this.m_labelPolarity.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_labelPolarity.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPolarity.Location = new System.Drawing.Point(8, 128);
      this.m_labelPolarity.Name = "m_labelPolarity";
      this.m_labelPolarity.Size = new System.Drawing.Size(80, 24);
      this.m_labelPolarity.TabIndex = 20;
      this.m_labelPolarity.Text = "Polarity:";
      this.m_labelPolarity.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxPolarity
      // 
      this.m_comboBoxPolarity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxPolarity.Enabled = false;
      this.m_comboBoxPolarity.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_comboBoxPolarity.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxPolarity.Location = new System.Drawing.Point(168, 128);
      this.m_comboBoxPolarity.Name = "m_comboBoxPolarity";
      this.m_comboBoxPolarity.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxPolarity.TabIndex = 3;
      this.m_toolTip.SetToolTip(this.m_comboBoxPolarity, "Select the images polarity");
      // 
      // m_labelOrientation
      // 
      this.m_labelOrientation.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_labelOrientation.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelOrientation.Location = new System.Drawing.Point(8, 160);
      this.m_labelOrientation.Name = "m_labelOrientation";
      this.m_labelOrientation.Size = new System.Drawing.Size(88, 24);
      this.m_labelOrientation.TabIndex = 22;
      this.m_labelOrientation.Text = "Orientation:";
      // 
      // m_comboBoxOrientation
      // 
      this.m_comboBoxOrientation.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxOrientation.Enabled = false;
      this.m_comboBoxOrientation.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_comboBoxOrientation.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxOrientation.Location = new System.Drawing.Point(168, 160);
      this.m_comboBoxOrientation.Name = "m_comboBoxOrientation";
      this.m_comboBoxOrientation.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxOrientation.TabIndex = 4;
      this.m_toolTip.SetToolTip(this.m_comboBoxOrientation, "Select the orientation");
      // 
      // m_labelBorders
      // 
      this.m_labelBorders.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_labelBorders.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelBorders.Location = new System.Drawing.Point(8, 256);
      this.m_labelBorders.Name = "m_labelBorders";
      this.m_labelBorders.Size = new System.Drawing.Size(128, 24);
      this.m_labelBorders.TabIndex = 24;
      this.m_labelBorders.Text = "Border Density:";
      this.m_labelBorders.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxBorders
      // 
      this.m_comboBoxBorders.Enabled = false;
      this.m_comboBoxBorders.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_comboBoxBorders.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxBorders.Location = new System.Drawing.Point(168, 256);
      this.m_comboBoxBorders.Name = "m_comboBoxBorders";
      this.m_comboBoxBorders.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxBorders.TabIndex = 8;
      this.m_toolTip.SetToolTip(this.m_comboBoxBorders, "Select the border density or enter a density value");
      this.m_comboBoxBorders.Validating += new System.ComponentModel.CancelEventHandler(this.m_comboBoxBorders_Validating);
      // 
      // m_labelEmptyImages
      // 
      this.m_labelEmptyImages.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_labelEmptyImages.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelEmptyImages.Location = new System.Drawing.Point(8, 288);
      this.m_labelEmptyImages.Name = "m_labelEmptyImages";
      this.m_labelEmptyImages.Size = new System.Drawing.Size(160, 24);
      this.m_labelEmptyImages.TabIndex = 26;
      this.m_labelEmptyImages.Text = "Empty Image Density:";
      this.m_labelEmptyImages.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_groupBoxPageSetup
      // 
      this.m_groupBoxPageSetup.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                      this.m_numericUpDownDPI,
                                                                                      this.m_labelDPI,
                                                                                      this.m_textBoxMaxDensity,
                                                                                      this.m_textBoxMinDensity,
                                                                                      this.m_labelMinDensity,
                                                                                      this.m_labelMaxDensity,
                                                                                      this.m_labelFilmSize,
                                                                                      this.m_comboBoxFilmSize,
                                                                                      this.m_numericUpDownRows,
                                                                                      this.m_numericUpDownColumns,
                                                                                      this.m_labelX,
                                                                                      this.m_labelFormat,
                                                                                      this.m_labelPolarity,
                                                                                      this.m_comboBoxPolarity,
                                                                                      this.m_labelOrientation,
                                                                                      this.m_comboBoxOrientation,
                                                                                      this.m_labelBorders,
                                                                                      this.m_labelEmptyImages,
                                                                                      this.m_comboBoxBorders,
                                                                                      this.m_comboBoxEmptyImages,
                                                                                      this.m_comboBoxMediumTypes,
                                                                                      this.m_labelMediumType});
      this.m_groupBoxPageSetup.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_groupBoxPageSetup.ForeColor = System.Drawing.Color.Navy;
      this.m_groupBoxPageSetup.Location = new System.Drawing.Point(368, 8);
      this.m_groupBoxPageSetup.Name = "m_groupBoxPageSetup";
      this.m_groupBoxPageSetup.Size = new System.Drawing.Size(328, 352);
      this.m_groupBoxPageSetup.TabIndex = 28;
      this.m_groupBoxPageSetup.TabStop = false;
      this.m_groupBoxPageSetup.Text = "Page Setup";
      // 
      // m_numericUpDownDPI
      // 
      this.m_numericUpDownDPI.Enabled = false;
      this.m_numericUpDownDPI.ForeColor = System.Drawing.Color.Navy;
      this.m_numericUpDownDPI.Location = new System.Drawing.Point(168, 32);
      this.m_numericUpDownDPI.Minimum = new System.Decimal(new int[] {
                                                                       1,
                                                                       0,
                                                                       0,
                                                                       0});
      this.m_numericUpDownDPI.Name = "m_numericUpDownDPI";
      this.m_numericUpDownDPI.Size = new System.Drawing.Size(72, 21);
      this.m_numericUpDownDPI.TabIndex = 44;
      this.m_numericUpDownDPI.Value = new System.Decimal(new int[] {
                                                                     1,
                                                                     0,
                                                                     0,
                                                                     0});
      // 
      // m_labelDPI
      // 
      this.m_labelDPI.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelDPI.Location = new System.Drawing.Point(8, 32);
      this.m_labelDPI.Name = "m_labelDPI";
      this.m_labelDPI.TabIndex = 43;
      this.m_labelDPI.Text = "Resolution:";
      // 
      // m_textBoxMaxDensity
      // 
      this.m_textBoxMaxDensity.Enabled = false;
      this.m_textBoxMaxDensity.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxMaxDensity.Location = new System.Drawing.Point(168, 96);
      this.m_textBoxMaxDensity.Name = "m_textBoxMaxDensity";
      this.m_textBoxMaxDensity.Size = new System.Drawing.Size(72, 21);
      this.m_textBoxMaxDensity.TabIndex = 2;
      this.m_textBoxMaxDensity.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxMaxDensity, "Enter the maximum image density if required");
      this.m_textBoxMaxDensity.Validating += new System.ComponentModel.CancelEventHandler(this.m_textBoxMaxDensity_Validating);
      // 
      // m_textBoxMinDensity
      // 
      this.m_textBoxMinDensity.Enabled = false;
      this.m_textBoxMinDensity.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxMinDensity.Location = new System.Drawing.Point(168, 64);
      this.m_textBoxMinDensity.Name = "m_textBoxMinDensity";
      this.m_textBoxMinDensity.Size = new System.Drawing.Size(72, 21);
      this.m_textBoxMinDensity.TabIndex = 1;
      this.m_textBoxMinDensity.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxMinDensity, "Enter the minumum image density if required");
      this.m_textBoxMinDensity.Validating += new System.ComponentModel.CancelEventHandler(this.m_textBoxMinDensity_Validating);
      // 
      // m_labelMinDensity
      // 
      this.m_labelMinDensity.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelMinDensity.Location = new System.Drawing.Point(8, 64);
      this.m_labelMinDensity.Name = "m_labelMinDensity";
      this.m_labelMinDensity.TabIndex = 42;
      this.m_labelMinDensity.Text = "Min Density:";
      // 
      // m_labelMaxDensity
      // 
      this.m_labelMaxDensity.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelMaxDensity.Location = new System.Drawing.Point(8, 96);
      this.m_labelMaxDensity.Name = "m_labelMaxDensity";
      this.m_labelMaxDensity.TabIndex = 41;
      this.m_labelMaxDensity.Text = "Max Density:";
      // 
      // m_labelFilmSize
      // 
      this.m_labelFilmSize.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelFilmSize.Location = new System.Drawing.Point(8, 192);
      this.m_labelFilmSize.Name = "m_labelFilmSize";
      this.m_labelFilmSize.TabIndex = 40;
      this.m_labelFilmSize.Text = "Film Size:";
      this.m_labelFilmSize.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxFilmSize
      // 
      this.m_comboBoxFilmSize.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxFilmSize.Enabled = false;
      this.m_comboBoxFilmSize.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxFilmSize.Location = new System.Drawing.Point(168, 192);
      this.m_comboBoxFilmSize.Name = "m_comboBoxFilmSize";
      this.m_comboBoxFilmSize.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxFilmSize.TabIndex = 5;
      this.m_toolTip.SetToolTip(this.m_comboBoxFilmSize, "Select the film size");
      // 
      // m_numericUpDownRows
      // 
      this.m_numericUpDownRows.Enabled = false;
      this.m_numericUpDownRows.ForeColor = System.Drawing.Color.Navy;
      this.m_numericUpDownRows.Location = new System.Drawing.Point(280, 224);
      this.m_numericUpDownRows.Maximum = new System.Decimal(new int[] {
                                                                        30,
                                                                        0,
                                                                        0,
                                                                        0});
      this.m_numericUpDownRows.Minimum = new System.Decimal(new int[] {
                                                                        1,
                                                                        0,
                                                                        0,
                                                                        0});
      this.m_numericUpDownRows.Name = "m_numericUpDownRows";
      this.m_numericUpDownRows.Size = new System.Drawing.Size(40, 21);
      this.m_numericUpDownRows.TabIndex = 7;
      this.m_numericUpDownRows.Value = new System.Decimal(new int[] {
                                                                      1,
                                                                      0,
                                                                      0,
                                                                      0});
      // 
      // m_numericUpDownColumns
      // 
      this.m_numericUpDownColumns.Enabled = false;
      this.m_numericUpDownColumns.ForeColor = System.Drawing.Color.Navy;
      this.m_numericUpDownColumns.Location = new System.Drawing.Point(168, 224);
      this.m_numericUpDownColumns.Maximum = new System.Decimal(new int[] {
                                                                           30,
                                                                           0,
                                                                           0,
                                                                           0});
      this.m_numericUpDownColumns.Minimum = new System.Decimal(new int[] {
                                                                           1,
                                                                           0,
                                                                           0,
                                                                           0});
      this.m_numericUpDownColumns.Name = "m_numericUpDownColumns";
      this.m_numericUpDownColumns.Size = new System.Drawing.Size(40, 21);
      this.m_numericUpDownColumns.TabIndex = 6;
      this.m_numericUpDownColumns.Value = new System.Decimal(new int[] {
                                                                         1,
                                                                         0,
                                                                         0,
                                                                         0});
      // 
      // m_labelX
      // 
      this.m_labelX.Location = new System.Drawing.Point(240, 224);
      this.m_labelX.Name = "m_labelX";
      this.m_labelX.Size = new System.Drawing.Size(16, 21);
      this.m_labelX.TabIndex = 32;
      this.m_labelX.Text = "X";
      this.m_labelX.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_labelFormat
      // 
      this.m_labelFormat.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_labelFormat.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelFormat.Location = new System.Drawing.Point(8, 224);
      this.m_labelFormat.Name = "m_labelFormat";
      this.m_labelFormat.Size = new System.Drawing.Size(80, 24);
      this.m_labelFormat.TabIndex = 28;
      this.m_labelFormat.Text = "Format:";
      this.m_labelFormat.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_comboBoxEmptyImages
      // 
      this.m_comboBoxEmptyImages.Enabled = false;
      this.m_comboBoxEmptyImages.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_comboBoxEmptyImages.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxEmptyImages.ItemHeight = 15;
      this.m_comboBoxEmptyImages.Location = new System.Drawing.Point(168, 288);
      this.m_comboBoxEmptyImages.Name = "m_comboBoxEmptyImages";
      this.m_comboBoxEmptyImages.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxEmptyImages.TabIndex = 9;
      this.m_toolTip.SetToolTip(this.m_comboBoxEmptyImages, "Select the empty  image density or enter a density value");
      this.m_comboBoxEmptyImages.Validating += new System.ComponentModel.CancelEventHandler(this.m_comboBoxEmptyImages_Validating);
      // 
      // m_comboBoxMediumTypes
      // 
      this.m_comboBoxMediumTypes.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxMediumTypes.Enabled = false;
      this.m_comboBoxMediumTypes.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxMediumTypes.Location = new System.Drawing.Point(168, 320);
      this.m_comboBoxMediumTypes.Name = "m_comboBoxMediumTypes";
      this.m_comboBoxMediumTypes.Size = new System.Drawing.Size(152, 23);
      this.m_comboBoxMediumTypes.TabIndex = 10;
      this.m_toolTip.SetToolTip(this.m_comboBoxMediumTypes, "Select the medium type");
      // 
      // m_labelMediumType
      // 
      this.m_labelMediumType.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelMediumType.Location = new System.Drawing.Point(8, 320);
      this.m_labelMediumType.Name = "m_labelMediumType";
      this.m_labelMediumType.Size = new System.Drawing.Size(112, 23);
      this.m_labelMediumType.TabIndex = 40;
      this.m_labelMediumType.Text = "Medium Type:";
      // 
      // m_labelPrinter
      // 
      this.m_labelPrinter.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPrinter.ForeColor = System.Drawing.Color.Navy;
      this.m_labelPrinter.Location = new System.Drawing.Point(8, 16);
      this.m_labelPrinter.Name = "m_labelPrinter";
      this.m_labelPrinter.Size = new System.Drawing.Size(72, 24);
      this.m_labelPrinter.TabIndex = 29;
      this.m_labelPrinter.Text = "Printer:";
      // 
      // m_comboBoxPrinters
      // 
      this.m_comboBoxPrinters.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxPrinters.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_comboBoxPrinters.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxPrinters.Location = new System.Drawing.Point(112, 16);
      this.m_comboBoxPrinters.Name = "m_comboBoxPrinters";
      this.m_comboBoxPrinters.Size = new System.Drawing.Size(248, 23);
      this.m_comboBoxPrinters.TabIndex = 0;
      this.m_toolTip.SetToolTip(this.m_comboBoxPrinters, "Select print from list");
      this.m_comboBoxPrinters.SelectedIndexChanged += new System.EventHandler(this.m_comboBoxPrinters_SelectedIndexChanged);
      // 
      // m_groupBoxSupport
      // 
      this.m_groupBoxSupport.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.m_checkBoxColor,
                                                                                    this.m_checkBoxGrayscale});
      this.m_groupBoxSupport.Location = new System.Drawing.Point(8, 40);
      this.m_groupBoxSupport.Name = "m_groupBoxSupport";
      this.m_groupBoxSupport.Size = new System.Drawing.Size(352, 48);
      this.m_groupBoxSupport.TabIndex = 31;
      this.m_groupBoxSupport.TabStop = false;
      // 
      // m_checkBoxColor
      // 
      this.m_checkBoxColor.CustomImages = new System.Drawing.Bitmap[] {
                                                                        null,
                                                                        null,
                                                                        null};
      this.m_checkBoxColor.Enabled = false;
      this.m_checkBoxColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_checkBoxColor.ForeColor = System.Drawing.Color.Navy;
      this.m_checkBoxColor.Location = new System.Drawing.Point(24, 16);
      this.m_checkBoxColor.Name = "m_checkBoxColor";
      this.m_checkBoxColor.Size = new System.Drawing.Size(120, 24);
      this.m_checkBoxColor.TabIndex = 0;
      this.m_checkBoxColor.Text = "Supports Color";
      // 
      // m_checkBoxGrayscale
      // 
      this.m_checkBoxGrayscale.CustomImages = new System.Drawing.Bitmap[] {
                                                                            null,
                                                                            null,
                                                                            null};
      this.m_checkBoxGrayscale.Enabled = false;
      this.m_checkBoxGrayscale.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_checkBoxGrayscale.ForeColor = System.Drawing.Color.Navy;
      this.m_checkBoxGrayscale.Location = new System.Drawing.Point(192, 16);
      this.m_checkBoxGrayscale.Name = "m_checkBoxGrayscale";
      this.m_checkBoxGrayscale.Size = new System.Drawing.Size(144, 24);
      this.m_checkBoxGrayscale.TabIndex = 1;
      this.m_checkBoxGrayscale.Text = "Supports Grayscale";
      // 
      // m_groupBoxAttributesw
      // 
      this.m_groupBoxAttributesw.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                        this.m_listBoxAttributes});
      this.m_groupBoxAttributesw.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_groupBoxAttributesw.ForeColor = System.Drawing.Color.Navy;
      this.m_groupBoxAttributesw.Location = new System.Drawing.Point(8, 96);
      this.m_groupBoxAttributesw.Name = "m_groupBoxAttributesw";
      this.m_groupBoxAttributesw.Size = new System.Drawing.Size(352, 184);
      this.m_groupBoxAttributesw.TabIndex = 32;
      this.m_groupBoxAttributesw.TabStop = false;
      this.m_groupBoxAttributesw.Text = "Attributes";
      // 
      // m_listBoxAttributes
      // 
      this.m_listBoxAttributes.Enabled = false;
      this.m_listBoxAttributes.ForeColor = System.Drawing.Color.Navy;
      this.m_listBoxAttributes.ItemHeight = 15;
      this.m_listBoxAttributes.Location = new System.Drawing.Point(8, 16);
      this.m_listBoxAttributes.Name = "m_listBoxAttributes";
      this.m_listBoxAttributes.Size = new System.Drawing.Size(336, 154);
      this.m_listBoxAttributes.TabIndex = 19;
      this.m_toolTip.SetToolTip(this.m_listBoxAttributes, "Printer Attributes");
      // 
      // m_groupBoxPrintJob
      // 
      this.m_groupBoxPrintJob.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_groupBoxPrintJob.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                     this.m_numericUpDownCopies,
                                                                                     this.m_comboBoxSmoothing,
                                                                                     this.m_labelSmoothing,
                                                                                     this.m_comboBoxInterpolation,
                                                                                     this.m_labelInterpolation,
                                                                                     this.m_labelCopies});
      this.m_groupBoxPrintJob.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_groupBoxPrintJob.ForeColor = System.Drawing.Color.Navy;
      this.m_groupBoxPrintJob.Location = new System.Drawing.Point(8, 288);
      this.m_groupBoxPrintJob.Name = "m_groupBoxPrintJob";
      this.m_groupBoxPrintJob.Size = new System.Drawing.Size(352, 120);
      this.m_groupBoxPrintJob.TabIndex = 33;
      this.m_groupBoxPrintJob.TabStop = false;
      this.m_groupBoxPrintJob.Text = "Print Job";
      // 
      // m_numericUpDownCopies
      // 
      this.m_numericUpDownCopies.Enabled = false;
      this.m_numericUpDownCopies.ForeColor = System.Drawing.Color.Navy;
      this.m_numericUpDownCopies.Location = new System.Drawing.Point(136, 24);
      this.m_numericUpDownCopies.Maximum = new System.Decimal(new int[] {
                                                                          20,
                                                                          0,
                                                                          0,
                                                                          0});
      this.m_numericUpDownCopies.Minimum = new System.Decimal(new int[] {
                                                                          1,
                                                                          0,
                                                                          0,
                                                                          0});
      this.m_numericUpDownCopies.Name = "m_numericUpDownCopies";
      this.m_numericUpDownCopies.Size = new System.Drawing.Size(40, 21);
      this.m_numericUpDownCopies.TabIndex = 11;
      this.m_toolTip.SetToolTip(this.m_numericUpDownCopies, "Select the number of copies to print");
      this.m_numericUpDownCopies.Value = new System.Decimal(new int[] {
                                                                        1,
                                                                        0,
                                                                        0,
                                                                        0});
      // 
      // m_comboBoxSmoothing
      // 
      this.m_comboBoxSmoothing.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxSmoothing.Enabled = false;
      this.m_comboBoxSmoothing.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxSmoothing.Location = new System.Drawing.Point(136, 88);
      this.m_comboBoxSmoothing.Name = "m_comboBoxSmoothing";
      this.m_comboBoxSmoothing.Size = new System.Drawing.Size(200, 23);
      this.m_comboBoxSmoothing.TabIndex = 13;
      this.m_toolTip.SetToolTip(this.m_comboBoxSmoothing, "Select the smotting type");
      // 
      // m_labelSmoothing
      // 
      this.m_labelSmoothing.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelSmoothing.Location = new System.Drawing.Point(8, 88);
      this.m_labelSmoothing.Name = "m_labelSmoothing";
      this.m_labelSmoothing.Size = new System.Drawing.Size(104, 23);
      this.m_labelSmoothing.TabIndex = 4;
      this.m_labelSmoothing.Text = "Smoothing:";
      // 
      // m_comboBoxInterpolation
      // 
      this.m_comboBoxInterpolation.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxInterpolation.Enabled = false;
      this.m_comboBoxInterpolation.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxInterpolation.Location = new System.Drawing.Point(136, 56);
      this.m_comboBoxInterpolation.Name = "m_comboBoxInterpolation";
      this.m_comboBoxInterpolation.Size = new System.Drawing.Size(200, 23);
      this.m_comboBoxInterpolation.TabIndex = 12;
      this.m_toolTip.SetToolTip(this.m_comboBoxInterpolation, "Select the interpolation method");
      this.m_comboBoxInterpolation.SelectedValueChanged += new System.EventHandler(this.m_comboBoxInterpolation_SelectedValueChanged);
      // 
      // m_labelInterpolation
      // 
      this.m_labelInterpolation.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelInterpolation.Location = new System.Drawing.Point(8, 56);
      this.m_labelInterpolation.Name = "m_labelInterpolation";
      this.m_labelInterpolation.Size = new System.Drawing.Size(104, 23);
      this.m_labelInterpolation.TabIndex = 2;
      this.m_labelInterpolation.Text = "Interpolation:";
      // 
      // m_labelCopies
      // 
      this.m_labelCopies.Font = new System.Drawing.Font("Arial", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelCopies.Location = new System.Drawing.Point(8, 24);
      this.m_labelCopies.Name = "m_labelCopies";
      this.m_labelCopies.Size = new System.Drawing.Size(64, 23);
      this.m_labelCopies.TabIndex = 0;
      this.m_labelCopies.Text = "Copies:";
      // 
      // DicomPrintDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.ClientSize = new System.Drawing.Size(706, 418);
      this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                  this.m_groupBoxPrintJob,
                                                                  this.m_groupBoxAttributesw,
                                                                  this.m_groupBoxSupport,
                                                                  this.m_comboBoxPrinters,
                                                                  this.m_labelPrinter,
                                                                  this.m_groupBoxPageSetup,
                                                                  this.m_buttonOK,
                                                                  this.m_buttonCancel});
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.ForeColor = System.Drawing.Color.Navy;
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "DicomPrintDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "Dicom Print";
      this.m_groupBoxPageSetup.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownDPI)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownRows)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownColumns)).EndInit();
      this.m_groupBoxSupport.ResumeLayout(false);
      this.m_groupBoxAttributesw.ResumeLayout(false);
      this.m_groupBoxPrintJob.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownCopies)).EndInit();
      this.ResumeLayout(false);

    } //InitializeComponent()

		#endregion

    #region event handler

    /// <summary>
    /// Cancel Button Click Event. 
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="e">Event arguements</param>
    private void m_buttonCancel_Click(object sender, System.EventArgs e)
    {

      this.DialogResult = DialogResult.Cancel;
      this.Close();
    } // m_buttonCancel_Click()

    /// <summary>
    /// OK Button Click Event
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="e">Event arguements</param>
    private void m_buttonOK_Click(object sender, System.EventArgs e)
    {

      //==========================================
      // Make sure a DICOM printer was selected
      //==========================================
      if ( m_DicomPrinter != null )
      {

        //============================================
        // Set the printers polarity if enabled
        //=============================================
        if ( m_comboBoxPolarity.Enabled )
        { 
          m_DicomPrinter.CustomPolarity = m_comboBoxPolarity.Text;
        } // END ... If the polarity selection is enabled 

        //===========================================
        // Set the printer orientation if enabled
        //===========================================
        if ( m_comboBoxOrientation.Enabled )
        {
          m_DicomPrinter.CustomOrientation = m_comboBoxOrientation.Text;
        } // END ... If the orientation selection is enabled

        //====================================
        // Set the border density if enabled
        //====================================
        if ( m_comboBoxBorders.Enabled )
        {
          m_DicomPrinter.CustomBorderDensity = m_comboBoxBorders.Text;
        } // END ... If the border densiy selection is enabled

        //===========================================
        // Set the empty image density if enabled
        //===========================================
        if ( m_comboBoxEmptyImages.Enabled ) 
        {
          m_DicomPrinter.CustomEmptyImageDensity = m_comboBoxEmptyImages.Text;
        } // END ... If the empty image density selection is enabled


        // Set the number of images per column
        m_DicomPrinter.ImagesPerCol = (int)m_numericUpDownColumns.Value;

        // Set the number of images per row
        m_DicomPrinter.ImagesPerRow = (int)m_numericUpDownRows.Value;

        //=====================================
        // Set the printerd max denisty if set
        //=====================================
        if ( m_textBoxMaxDensity.Text != "" )
        {
          m_DicomPrinter.MaximumDensity = System.Convert.ToInt32(m_textBoxMaxDensity.Text);
        } // END ... If the max density was set

        //==========================================
        // Set the printer minimum density if set
        //==========================================
        if ( m_textBoxMinDensity.Text != "" )
        {
          m_DicomPrinter.MinimumDensity = System.Convert.ToInt32(m_textBoxMinDensity.Text);
        } // END ... If the minimum density was set
 
        // Set the number of copies
        m_DicomPrinter.NumberOfCopies = (int)m_numericUpDownCopies.Value;

        //===================================================
        // Set the printers interpolation method if enabled
        //===================================================
        if ( m_comboBoxInterpolation.Enabled )
        {
          m_DicomPrinter.CustomInterpolationMethod = m_comboBoxInterpolation.Text;
        } // END ... If the interpolation method is enabled

        //===========================================
        // Set the printers smoothing type if enabled
        //===========================================
        if ( m_comboBoxSmoothing.Enabled )
        {
          m_DicomPrinter.CustomSmoothing = m_comboBoxSmoothing.Text;
        } // END ... If the snoothing type is enabled

        // Set the printers resolution
        m_DicomPrinter.Resolution = (int)m_numericUpDownDPI.Value;

        // Set the dialog result to OK
        this.DialogResult = DialogResult.OK;

        // Close the dialog
        this.Close();

      } // END ... If a printer was selected
      else
      {
        Viatronix.v3D.UI.MessageBox.ShowError(this,@"No printer has been selected, press cancel");
      } // END ... Else no printer was selected

    } // m_buttonOK_Click()

    /// <summary>
    /// Printer Combo Selection Event
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="e">Event arguements</param>
    private void m_comboBoxPrinters_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      
      //==========================================================================================
      // Reset all controls to their default state so that they can be modified accoording to
      // the printer selected
      //==========================================================================================

      m_listBoxAttributes.Items.Clear();

      System.Collections.ArrayList blankArray = new System.Collections.ArrayList();

      m_comboBoxPolarity.Enabled = false;
      m_comboBoxEmptyImages.Enabled = false;
      m_comboBoxSmoothing.Enabled = false;
      m_comboBoxInterpolation.Enabled = false;
      m_comboBoxBorders.Enabled = false;
      m_comboBoxOrientation.Enabled = false;
      m_comboBoxFilmSize.Enabled = false;
      m_textBoxMinDensity.Enabled = false;
      m_textBoxMaxDensity.Enabled = false;
      m_numericUpDownDPI.Enabled = false;
      m_numericUpDownCopies.Enabled = false;
      m_numericUpDownColumns.Enabled = false;
      m_numericUpDownRows.Enabled = false;
      m_listBoxAttributes.Enabled = false;
      m_checkBoxGrayscale.Enabled = false;
      m_checkBoxColor.Enabled = false;

      m_checkBoxColor.Checked = false;
      m_checkBoxGrayscale.Checked = false;

      m_comboBoxInterpolation.DataSource = blankArray;
      m_comboBoxInterpolation.DisplayMember = "Item";

      m_comboBoxSmoothing.DataSource = blankArray;
      m_comboBoxSmoothing.DisplayMember = "Item";

      m_comboBoxPolarity.DataSource = blankArray;
      m_comboBoxPolarity.DisplayMember = "Item";

      m_comboBoxOrientation.DataSource = blankArray;
      m_comboBoxOrientation.DisplayMember = "Item";

      m_comboBoxFilmSize.DataSource = blankArray;
      m_comboBoxFilmSize.DisplayMember = "Item";

      m_comboBoxBorders.DataSource = blankArray;
      m_comboBoxBorders.DisplayMember = "Item";
      m_comboBoxBorders.Text = @"";

      m_comboBoxEmptyImages.DataSource = blankArray;
      m_comboBoxEmptyImages.DisplayMember = "Item";
      m_comboBoxEmptyImages.Text = @"";
    
      m_comboBoxMediumTypes.DataSource = blankArray;
      m_comboBoxMediumTypes.DisplayMember = "Item";

      m_numericUpDownDPI.Value = 1;
      m_numericUpDownCopies.Value = 1;
      m_numericUpDownColumns.Value = 1;
      m_numericUpDownRows.Value = 1;


      try
      {

        // Get the host entry for the printer
        Host host = m_hostTable.get_Item(m_comboBoxPrinters.Text);

        // Get the attributes for the printer
        AttributeCollection attributes =  host.Services.get_Item(0).Attributes;


        // Create a new DICOM PRINTER object initializing it with the printer name
        m_DicomPrinter = new Printing.Printer(m_comboBoxPrinters.Text);

        // Attempt a connection to retrieve some properties
        m_DicomPrinter.Connect();

        // Disconnect
        m_DicomPrinter.Disconnect();

        m_checkBoxGrayscale.Enabled = true;
        m_checkBoxColor.Enabled = true;
        m_numericUpDownDPI.Enabled = true;
        m_numericUpDownCopies.Enabled = true;
        m_listBoxAttributes.Enabled = true;

        //=====================================================================================
        // Iterate though all the attributes and load them into the attributes list view
        //=====================================================================================
        for ( int i = 0; i < attributes.Count; i++ )
        {
          Attribute attribute = attributes.get_Item(i);

          //==============================
          // Check for an attribute file
          //==============================
          if ( attribute.Name != "attribute_file" )
          {
            m_listBoxAttributes.Items.Add(attribute.ToString());
          } // END ... If the attribute is a file

        } // END ... for each atribute

        //m_comboBoxMediumTypes.DataBindings.Add("Text", m_DicomPrinter.MediumTypes, "MEDIUM_TYPES.DESCRIPTION");
        m_comboBoxMediumTypes.DataSource = m_DicomPrinter.SupportedMediumTypes;
        m_comboBoxMediumTypes.DisplayMember = "Item";

        m_comboBoxPolarity.Enabled = true;
        m_comboBoxPolarity.DataSource = m_DicomPrinter.SupportedPolarities;
        m_comboBoxPolarity.DisplayMember = "Item";

        //============================================================
        // Check if the setting of the polarity is supported or not
        //============================================================
        if ( m_comboBoxPolarity.Text == "UNSUPPORTED" )
        {
          m_comboBoxPolarity.Enabled = false;
        } // END ... If the setting of the polartiy is UNSUPPORTED

        m_comboBoxEmptyImages.Enabled = true;
        m_comboBoxEmptyImages.DataSource = m_DicomPrinter.SupportedEmptyImageDensities;
        m_comboBoxEmptyImages.DisplayMember = "Item";

        //==========================================================================
        // Check if the setting of empty image density setting is supported or not
        //==========================================================================
        if ( m_comboBoxEmptyImages.Text == "UNSUPPORTED" )
        {
          m_comboBoxEmptyImages.Enabled = false;
        } // END ... If the setting of the empty image density is UNSUPOORTED

        m_comboBoxSmoothing.Enabled = true;
        m_comboBoxSmoothing.DataSource = m_DicomPrinter.SupportedSmoothingTypes;
        m_comboBoxSmoothing.DisplayMember = "Item";

        //============================================================
        // Check if the setting of the smoothing type is supported
        //============================================================
        if ( m_comboBoxSmoothing.Text == "UNSUPPORTED" )
        {
          m_comboBoxSmoothing.Enabled = false;
        } // END ... If the setting of the smoothing type is UNSUPPORTED

        m_comboBoxInterpolation.Enabled = true;
        m_comboBoxInterpolation.DataSource = m_DicomPrinter.SupportedInterpolationMethods;
        m_comboBoxInterpolation.DisplayMember = "Item";

        //========================================================================
        // Check if the setting of the interpolation method is supported or not
        //=========================================================================
        if ( m_comboBoxInterpolation.Text == "UNSUPPORTED" )
        {
          m_comboBoxInterpolation.Enabled = false;
        } // END ... If the setting of the interpolation method is UNSUPPORTED

        m_comboBoxBorders.Enabled = true;
        m_comboBoxBorders.DataSource = m_DicomPrinter.SupportedBorderDensities;
        m_comboBoxBorders.DisplayMember = "Item";

        //==========================================================================
        // Check if the setting of border density setting is supported or not
        //==========================================================================
        if ( m_comboBoxBorders.Text == "UNSUPPORTED" )
        {
          m_comboBoxBorders.Enabled = false;
        } // END ... If the setting of the border density is UNSUPPERTED

        m_comboBoxOrientation.Enabled = true;
        m_comboBoxOrientation.DataSource = m_DicomPrinter.SupportedOrientations;
        m_comboBoxOrientation.DisplayMember = "Item";

        m_comboBoxFilmSize.Enabled = true;  
        m_comboBoxFilmSize.DataSource = m_DicomPrinter.SupportedFilmSizes;
        m_comboBoxFilmSize.DisplayMember = "Item";


        m_textBoxMinDensity.Enabled = true;
        m_iMinimumDensityAllowed = m_DicomPrinter.MinimumDensityAllowed;

        //=============================================================================
        // Check is the setting of the minimum density is supported. A setting or -1
        // indicates that the control should be disabled.
        //=============================================================================
        if ( m_iMinimumDensityAllowed == -1 )
        {
          m_textBoxMinDensity.Enabled = false;
        } // END ... If the minimum density allowed equals -1

        m_textBoxMaxDensity.Enabled = true;
        m_iMaximumDensityAllowed = m_DicomPrinter.MaximumDensityAllowed;

        //=============================================================================
        // Check is the setting of the maximum density is supported. A setting or -1
        // indicates that the control should be disabled.
        //=============================================================================
        if ( m_iMaximumDensityAllowed == -1 )
        {
          m_textBoxMinDensity.Enabled = false;
        } // END ... If the maximum density allowed is -1

        //////////////////////////////////////////////////////////
        // Set the initial values for the printer/page setup
        ////////////////////////////////////////////////////////
        
        // Sets the supports color check box
        m_checkBoxColor.Checked  = m_DicomPrinter.SupportsColor;

        // Sets the spports grayscale check box (This should always be checked)
        m_checkBoxGrayscale.Checked = m_DicomPrinter.SupportsGrayscale;

        // Set the image per column 
        m_numericUpDownColumns.Value = m_DicomPrinter.ImagesPerCol;
        m_numericUpDownColumns.Maximum = m_DicomPrinter.MaximumImagesPerColumnAllowed;

        // Set the images per row 
        m_numericUpDownRows.Value = m_DicomPrinter.ImagesPerRow;
        m_numericUpDownRows.Maximum = m_DicomPrinter.MaximumImagesPerRowAllowed;

        // Set the maximum resolution (DPI)
        int iMaxDPI = m_DicomPrinter.Attributes.GetAttributeValue("max_dpi", 300);

        /// Set the minimum resolution (DPI)
        int iMinDPI = m_DicomPrinter.Attributes.GetAttributeValue("min_dpi",1);


        m_numericUpDownDPI.Minimum = iMinDPI;
        m_numericUpDownDPI.Maximum = iMaxDPI;
        m_numericUpDownDPI.Value = iMaxDPI;


      }
      catch ( ConnectionException ex )
      {
        Viatronix.v3D.UI.MessageBox.ShowError(this,@"Could not connect to " + m_comboBoxPrinters.Text + @" : " + ex.Message);
      }
      catch ( Exception ex )
      {
        Viatronix.v3D.UI.MessageBox.ShowError(this,"Could not create DICOM PRINTER : " + ex.Message );
      }


    } // m_comboBoxPrinters_SelectedIndexChanged()

    /// <summary>
    /// Validates the minimum denisty value.
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="e">Events arguements</param>
    private void m_textBoxMinDensity_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      try
      {

        //======================================================================
        // Make sure there is a value in the minimum denisties control field
        //======================================================================
        if ( m_textBoxMinDensity.Text != "" )
        {

          // New minimum density
          int iMin = System.Convert.ToInt32(m_textBoxMinDensity.Text);

          //============================================================================
          // Validate the minimum denisty value entered to the minimum density allowed
          //============================================================================
          if ( iMin < m_iMinimumDensityAllowed )
          {
            Viatronix.v3D.UI.MessageBox.ShowError(this,"Invalid minimum density value entered, cannot be less then minimum denisity of " + System.Convert.ToString(m_iMinimumDensityAllowed));
            m_textBoxMinDensity.Text = "";
            e.Cancel = true;
          } // END ... If the current minimum is less then the old minimum

        }
      }
      catch ( Exception )
      {
        Viatronix.v3D.UI.MessageBox.ShowError(this,"Invalid minimum density value entered");
        m_textBoxMinDensity.Text = "";
        e.Cancel = true;
      }

    } // m_textBoxMinDensity_Validating()

    /// <summary>
    /// Validates the maximum density setting
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="e">Event arguements</param>
    private void m_textBoxMaxDensity_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      try
      {

        //=================================================
        // Make sure the maximum density is filled in
        //==================================================
        if ( m_textBoxMaxDensity.Text != "" )
        {
          int iMax = System.Convert.ToInt32(m_textBoxMaxDensity.Text);

          //======================================
          // Validate the maximum density setting
          //======================================
          if ( iMax > m_iMaximumDensityAllowed )
          {
            Viatronix.v3D.UI.MessageBox.ShowError(this,"Invalid maximum density value entered, cannot exceed the maximum denisity of " + System.Convert.ToString(m_iMaximumDensityAllowed));
            m_textBoxMaxDensity.Text = "";
            e.Cancel = true;
          } // END ... If the maximum density is greater then the maximum allowed density

        } // END ... If the maximum density is filled in
      }
      catch ( Exception )
      {
        Viatronix.v3D.UI.MessageBox.ShowError( this,"Invalid maximum density value entered");
        m_textBoxMaxDensity.Text = "";
        e.Cancel = true;
      }

    } // m_textBoxMaxDensity_Validating()

    /// <summary>
    /// Interpolation methods cobo box selection event
    /// </summary>
    /// <param name="sender">Object of the events senders</param>
    /// <param name="e">Events arguements</param>
    private void m_comboBoxInterpolation_SelectedValueChanged(object sender, System.EventArgs e)
    {

      //================================================================================
      // If the interpolation method is CUBIC then allow the smoothing type to be set
      // otherwise the smooting type is disabled.
      //=================================================================================
      if ( m_comboBoxInterpolation.Text != "CUBIC" )
      {
        m_comboBoxSmoothing.Enabled = false;
      }
      else
      {
        m_comboBoxSmoothing.Enabled = true;
      }

    } // m_comboBoxInterpolation_SelectedValueChanged()

    /// <summary>
    /// Border densities validation event handler
    /// </summary>
    /// <param name="sender">Origination object of the event</param>
    /// <param name="e">Events arguements</param>
    private void m_comboBoxBorders_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      try
      {
        //===================================================================
        // Make sure the border densities combo boxes text is not empty
        //===================================================================
        if ( m_comboBoxBorders.Text != "" )
        {

          //======================================================
          // Check to see if the text was entered or selected
          //======================================================
          if ( ! m_comboBoxBorders.Items.Contains(m_comboBoxBorders.Text) )
          {
 
            int iValue = System.Convert.ToInt32(m_comboBoxBorders.Text);

            //===========================================================
            // Makes sure the value entered is within the min/max range
            //===========================================================
            if ( iValue < m_iMinimumDensityAllowed || iValue > m_iMaximumDensityAllowed )
            {
              Viatronix.v3D.UI.MessageBox.ShowError( this,"Invalid density value entered, must be within range of [ " + System.Convert.ToString(m_iMinimumDensityAllowed) + " - " + System.Convert.ToString(m_iMaximumDensityAllowed) + " ]");
              m_comboBoxBorders.SelectedIndex = 0;
              e.Cancel = true;
            } // END ... If the value entered is not valid

          } // END ... If the border densities combo boxes text was not selected fro the list 

        } // END ... If the border densities combo boxes text field is filled in
      }
      catch ( Exception )
      {
        Viatronix.v3D.UI.MessageBox.ShowError( this,"Invalid density value entered");
        m_comboBoxBorders.SelectedIndex = 0;
        e.Cancel = true;
      }


    } // m_comboBoxBorders_Validating()

    /// <summary>
    /// Empty image densities validation event handler.
    /// </summary>
    /// <param name="sender">Origination object of the event</param>
    /// <param name="e">Events arguement</param>
    private void m_comboBoxEmptyImages_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      try
      {
        //===================================================================
        // Make sure the empty image densities combo boxes text is not empty
        //===================================================================
        if ( m_comboBoxEmptyImages.Text != "" )
        {

          //======================================================
          // Check to see if the text was entered or selected
          //======================================================
          if ( ! m_comboBoxEmptyImages.Items.Contains(m_comboBoxEmptyImages.Text) )
          {

            int iValue = System.Convert.ToInt32(m_comboBoxEmptyImages.Text);

            //===========================================================
            // Makes sure the value entered is within the min/max range
            //===========================================================
            if ( iValue < m_iMinimumDensityAllowed || iValue > m_iMaximumDensityAllowed )
            {
              Viatronix.v3D.UI.MessageBox.ShowError( this,"Invalid density value entered, must be within range of [ " + System.Convert.ToString(m_iMinimumDensityAllowed) + " - " + System.Convert.ToString(m_iMaximumDensityAllowed) + " ]");
              m_comboBoxEmptyImages.SelectedIndex = 0;
              e.Cancel = true;
            } // END ... If the value entered is not valid

          } // END ... If the empty image densities combo boxes text was not selected fro the list 

        } // END ... If the empty image densities combo boxes text field is filled in

      }
      catch ( Exception )
      {
        Viatronix.v3D.UI.MessageBox.ShowError( this,"Invalid density value entered");
        m_comboBoxEmptyImages.SelectedIndex = 0;
        e.Cancel = true;
      }

    } // m_comboBoxEmptyImages_Validating()

    #endregion
	
  } // class DicomPrintDialog

  #endregion

  #region ComoboItem

  /// <summary>
  /// Combo Box Obect
  /// </summary>
  public class ComboItem
  {

    private int m_ID;
    private string m_Text;

    /// <summary>
    /// ComboItem constructor
    /// </summary>
    /// <param name="id">ID</param>
    /// <param name="text">Text used for display in the combo box</param>
    public ComboItem(int id, string text)
    {
      m_ID = id;
      m_Text = text;
    } // ComboItem()

    /// <summary>
    /// Returns the text reprsentation of the combo item
    /// </summary>
    /// <returns>Returns the text member of the class</returns>
    public override string ToString()
    {
      return m_Text;
    } // ToString()

  } // class ComboItem

  #endregion

} // namespace Viatronix.v3D.Dicom.UI

#region revision history

// Revision History:
// $Log: DicomPrintiDialog.cs,v $
// Revision 1.5  2005/11/23 11:50:49  mkontak
// Using new MessageBox implementation
//
// Revision 1.4  2005/10/21 17:49:29  mkontak
// Changed the implementation of the DicomHosts table and the way the clients
// are used.
//
// Revision 1.3  2005/10/11 14:43:05  mkontak
// Added docking
//
// Revision 1.2  2005/06/03 14:44:00  mkontak
// Changes to view dicom
//
// Revision 1.1.1.1  2004/03/02 03:58:37  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/02/20 14:26:50  mkontak
// Merge with CONSOLE_2-0
//
// Revision 1.5.2.1  2004/02/12 15:58:29  mkontak
// no message
//
// Revision 1.5  2003/09/04 19:38:47  mkontak
// Merge 09/04/2003
//
// Revision 1.4.2.1  2003/09/04 19:38:05  mkontak
// Dicom changes
//
// Revision 1.4  2003/06/17 19:06:13  mkontak
// Coding standards
//
// Revision 1.3  2003/05/15 16:09:06  mkontak
// Change to dicom
//
// Revision 1.2  2003/05/08 21:18:39  mkontak
// Modified to use  the UI's message box
//
// Revision 1.1  2003/04/29 13:47:40  geconomos
// Initial revision.
//
// Revision 1.9  2003/04/22 12:04:59  mkontak
// Coding standards
//
// Revision 1.8  2003/04/18 16:46:38  geconomos
// Inherited from v3D dialog class.
//
// Revision 1.7  2003/04/02 16:37:38  mkontak
// Changes in the Dicom Print Dialog
//
// Revision 1.6  2003/03/31 21:30:17  mkontak
// New DICOM
//
// Revision 1.5  2003/02/18 15:29:19  mkontak
// Fixed minor issue with min/max resolution
//
// Revision 1.4  2003/02/14 20:38:45  mkontak
// Added resolution, and disabled format selection.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/DicomPrintiDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $
// $Id: DicomPrintiDialog.cs,v 1.5 2005/11/23 11:50:49 mkontak Exp $

#endregion
