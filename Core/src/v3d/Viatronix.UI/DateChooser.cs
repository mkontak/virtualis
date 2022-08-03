// $Id: DateChooser.cs,v 1.7 2006/04/07 20:18:42 gdavidson Exp $
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

namespace Viatronix.UI
{
  
  #region enumerations

  public enum DateComparisons
  {
    LessThan,
    LessThanEqual,
    Equal,
    GreaterThanEqual,
    GreaterThan,
  }

  #endregion

	/// <summary>
	/// Summary description for DateChooser.
	/// </summary>
	public class DateChooser : System.Windows.Forms.UserControl
	{

    #region fields

    private readonly DateTime MaxDate = new DateTime( DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 23, 59, 59 );

    private System.Windows.Forms.Panel m_rangePanel;
    private System.Windows.Forms.DateTimePicker m_rangeEndPicker;
    private System.Windows.Forms.DateTimePicker m_rangeStartPicker;
    private System.Windows.Forms.Label m_rangeEndLabel;
    private System.Windows.Forms.Label m_rangeStartLabel;
    private System.Windows.Forms.Panel m_specificPanel;
    private System.Windows.Forms.DateTimePicker m_specificDatePicker;
    private System.Windows.Forms.Label m_specificDateLabel;
    private System.Windows.Forms.ComboBox m_specificSearchType;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
    private System.Windows.Forms.Label label1;

    private bool m_isRangeStyle = false;

    private EventHandler m_dateChangedHandler = null;

    public event EventHandler DateStyleChanged = null;

    #endregion

    #region constuction

		public DateChooser()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

			InitializeComponent();

			m_rangeStartPicker.Value = DateTime.Now.AddDays( -1 );
      m_rangeEndPicker.MaxDate = MaxDate;
      m_specificDatePicker.MaxDate = MaxDate;

      m_specificPanel.Visible = !m_isRangeStyle;
      m_rangePanel.Visible = m_isRangeStyle;

      m_specificSearchType.SelectedIndex = 0;
      m_dateChangedHandler = new EventHandler( this.OnDateChangedHandler );
      m_rangeStartPicker.ValueChanged += m_dateChangedHandler;
      m_rangeEndPicker.ValueChanged += m_dateChangedHandler;

      m_specificDatePicker.Enter += new EventHandler(OnDatePickerEnterHandler);
      m_specificDatePicker.MouseEnter += new EventHandler(OnDatePickerEnterHandler);

      m_rangeStartPicker.Enter += new EventHandler(OnDatePickerEnterHandler);
      m_rangeStartPicker.MouseEnter += new EventHandler(OnDatePickerEnterHandler);

      m_rangeEndPicker.Enter += new EventHandler(OnDatePickerEnterHandler);
      m_rangeEndPicker.MouseEnter += new EventHandler(OnDatePickerEnterHandler);
    }

    #endregion

    #region properties

    public bool IsRangeSearch
    {
      get { return m_isRangeStyle; }
      set 
      { 
        m_isRangeStyle = value; 
        OnDateStyleChanged();
      }
    }


    public DateTime[] SelectedDates
    {
      get
      {
        if( IsRangeSearch )
        {
          DateTime[] dates = new DateTime[2];
          dates[0] = m_rangeStartPicker.Value;
          dates[1] = m_rangeEndPicker.Value;

          return dates;
        }
        else
          return new DateTime[] { m_specificDatePicker.Value };
      }
    }


    public DateComparisons[] SelectedComparisions
    {
      get
      {
        if( IsRangeSearch )
          return new DateComparisons[] { DateComparisons.GreaterThanEqual, DateComparisons.LessThanEqual };
        else
          return new DateComparisons[] { GetSpecificComparision() };
      }      
    }


    public Color LabelColor
    {
      get { return m_specificDateLabel.ForeColor; }
      set 
      {
        m_rangeEndLabel.ForeColor = value;
        m_rangeStartLabel.ForeColor = value;
        m_specificDateLabel.ForeColor = value;
        Invalidate();      
      }
    }

    #endregion

    #region methods

    public void SetDates( DateTime[] dates )
    { 
      if( dates.Length > 1 )
      {
        m_rangeStartPicker.Value = dates[0];
        m_rangeEndPicker.Value = dates[1];

        m_rangeStartPicker.MaxDate = dates[1].AddDays( -1 );
        m_rangeEndPicker.MinDate = dates[0].AddDays( 1 );
      }
      else
        m_specificDatePicker.Value = dates[0];       
    }


    protected virtual void OnDateStyleChanged()
    {
      try
      {
        m_specificPanel.Visible = !m_isRangeStyle;
        m_rangePanel.Visible = m_isRangeStyle;

        if( this.DateStyleChanged != null )
          this.DateStyleChanged( this, EventArgs.Empty );
      }
      finally
      {
        Refresh();
      }
    } // OnDateStyleChanged


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


    private DateComparisons GetSpecificComparision()
    {
      switch( m_specificSearchType.SelectedIndex )
      {
        case 0: 
          return DateComparisons.Equal;
        case 1: 
          return DateComparisons.LessThan;
        case 2: 
          return DateComparisons.LessThanEqual;
        case 3: 
          return DateComparisons.GreaterThan;
        case 4: 
          return DateComparisons.GreaterThanEqual;
        default:
          throw new NotSupportedException( "The selected comparision is not supported." );
      }
    }

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the datepicker's Enter/MouseEnter events.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDatePickerEnterHandler(object sender, EventArgs args)
    {
      DateTimePicker datePicker = sender as DateTimePicker;
      if (datePicker != null)
      {
        DateTime max = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 23, 59, 59);
        datePicker.MaxDate = max;

     }
    } // OnDatePickerEnterHandler( sender, args )


    private void OnDateChangedHandler( object sender, System.EventArgs args )
    {
      m_rangeStartPicker.MaxDate = m_rangeEndPicker.Value.AddDays( -1 );
      m_rangeEndPicker.MinDate = m_rangeStartPicker.Value.AddDays( 1 );
    }

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_rangePanel = new System.Windows.Forms.Panel();
      this.m_rangeEndPicker = new System.Windows.Forms.DateTimePicker();
      this.m_rangeStartPicker = new System.Windows.Forms.DateTimePicker();
      this.m_rangeEndLabel = new System.Windows.Forms.Label();
      this.m_rangeStartLabel = new System.Windows.Forms.Label();
      this.m_specificPanel = new System.Windows.Forms.Panel();
      this.m_specificSearchType = new System.Windows.Forms.ComboBox();
      this.m_specificDatePicker = new System.Windows.Forms.DateTimePicker();
      this.m_specificDateLabel = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.m_rangePanel.SuspendLayout();
      this.m_specificPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_rangePanel
      // 
      this.m_rangePanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_rangePanel.Controls.Add(this.m_rangeEndPicker);
      this.m_rangePanel.Controls.Add(this.m_rangeStartPicker);
      this.m_rangePanel.Controls.Add(this.m_rangeEndLabel);
      this.m_rangePanel.Controls.Add(this.m_rangeStartLabel);
      this.m_rangePanel.ForeColor = System.Drawing.Color.Black;
      this.m_rangePanel.Location = new System.Drawing.Point(0, 0);
      this.m_rangePanel.Name = "m_rangePanel";
      this.m_rangePanel.Size = new System.Drawing.Size(304, 56);
      this.m_rangePanel.TabIndex = 6;
      this.m_rangePanel.Visible = false;
      // 
      // m_rangeEndPicker
      // 
      this.m_rangeEndPicker.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_rangeEndPicker.Format = System.Windows.Forms.DateTimePickerFormat.Short;
      this.m_rangeEndPicker.Location = new System.Drawing.Point(64, 32);
      this.m_rangeEndPicker.Name = "m_rangeEndPicker";
      this.m_rangeEndPicker.Size = new System.Drawing.Size(232, 20);
      this.m_rangeEndPicker.TabIndex = 9;
      // 
      // m_rangeStartPicker
      // 
      this.m_rangeStartPicker.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_rangeStartPicker.Format = System.Windows.Forms.DateTimePickerFormat.Short;
      this.m_rangeStartPicker.Location = new System.Drawing.Point(64, 1);
      this.m_rangeStartPicker.Name = "m_rangeStartPicker";
      this.m_rangeStartPicker.Size = new System.Drawing.Size(232, 20);
      this.m_rangeStartPicker.TabIndex = 8;
      // 
      // m_rangeEndLabel
      // 
      this.m_rangeEndLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.m_rangeEndLabel.ForeColor = System.Drawing.Color.Black;
      this.m_rangeEndLabel.Location = new System.Drawing.Point(0, 31);
      this.m_rangeEndLabel.Name = "m_rangeEndLabel";
      this.m_rangeEndLabel.Size = new System.Drawing.Size(56, 23);
      this.m_rangeEndLabel.TabIndex = 7;
      this.m_rangeEndLabel.Text = "End Date:";
      this.m_rangeEndLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_rangeStartLabel
      // 
      this.m_rangeStartLabel.ForeColor = System.Drawing.Color.Black;
      this.m_rangeStartLabel.Location = new System.Drawing.Point(0, 0);
      this.m_rangeStartLabel.Name = "m_rangeStartLabel";
      this.m_rangeStartLabel.Size = new System.Drawing.Size(64, 23);
      this.m_rangeStartLabel.TabIndex = 6;
      this.m_rangeStartLabel.Text = "Start Date:";
      this.m_rangeStartLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_specificPanel
      // 
      this.m_specificPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_specificPanel.Controls.Add(this.m_specificSearchType);
      this.m_specificPanel.Controls.Add(this.m_specificDatePicker);
      this.m_specificPanel.Controls.Add(this.m_specificDateLabel);
      this.m_specificPanel.Controls.Add(this.label1);
      this.m_specificPanel.Location = new System.Drawing.Point(0, 0);
      this.m_specificPanel.Name = "m_specificPanel";
      this.m_specificPanel.Size = new System.Drawing.Size(304, 56);
      this.m_specificPanel.TabIndex = 7;
      // 
      // m_specificSearchType
      // 
      this.m_specificSearchType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_specificSearchType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_specificSearchType.Items.AddRange(new object[] {
                                                              "On",
                                                              "Before",
                                                              "On or Before",
                                                              "After",
                                                              "On or After"});
      this.m_specificSearchType.Location = new System.Drawing.Point(80, 32);
      this.m_specificSearchType.Name = "m_specificSearchType";
      this.m_specificSearchType.Size = new System.Drawing.Size(216, 21);
      this.m_specificSearchType.TabIndex = 11;
      // 
      // m_specificDatePicker
      // 
      this.m_specificDatePicker.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_specificDatePicker.Format = System.Windows.Forms.DateTimePickerFormat.Short;
      this.m_specificDatePicker.Location = new System.Drawing.Point(80, 1);
      this.m_specificDatePicker.Name = "m_specificDatePicker";
      this.m_specificDatePicker.Size = new System.Drawing.Size(216, 20);
      this.m_specificDatePicker.TabIndex = 8;
      // 
      // m_specificDateLabel
      // 
      this.m_specificDateLabel.ForeColor = System.Drawing.Color.Black;
      this.m_specificDateLabel.Location = new System.Drawing.Point(0, 0);
      this.m_specificDateLabel.Name = "m_specificDateLabel";
      this.m_specificDateLabel.Size = new System.Drawing.Size(72, 23);
      this.m_specificDateLabel.TabIndex = 6;
      this.m_specificDateLabel.Text = "Search Date:";
      this.m_specificDateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label1
      // 
      this.label1.BackColor = System.Drawing.Color.Transparent;
      this.label1.Location = new System.Drawing.Point(0, 31);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(80, 23);
      this.label1.TabIndex = 12;
      this.label1.Text = "Date Criteria:";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // DateChooser
      // 
      this.Controls.Add(this.m_specificPanel);
      this.Controls.Add(this.m_rangePanel);
      this.Name = "DateChooser";
      this.Size = new System.Drawing.Size(304, 56);
      this.m_rangePanel.ResumeLayout(false);
      this.m_specificPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion
    
	} // class DateChooser
} // namespace Viatronix.UI

#region revision history

// $Log: DateChooser.cs,v $
// Revision 1.7  2006/04/07 20:18:42  gdavidson
// Changed the text for the date criteria.
//
// Revision 1.6  2006/02/16 17:37:55  gdavidson
// Modified the GetSpecificComparision method
//
// Revision 1.5  2006/02/14 16:53:47  gdavidson
// Modified the range search functionality.
//
// Revision 1.4  2005/10/26 19:55:51  gdavidson
// Added a check that the end date must come after the start date.
//
// Revision 1.3  2005/05/26 17:19:11  gdavidson
// Added support for a double buffering and transparent background.
//
// Revision 1.2  2005/03/29 01:03:37  gdavidson
// Added a SetDates method.
//
// Revision 1.1  2005/02/24 19:12:33  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DateChooser.cs,v 1.7 2006/04/07 20:18:42 gdavidson Exp $
// $Id: DateChooser.cs,v 1.7 2006/04/07 20:18:42 gdavidson Exp $

#endregion