// $Id: DetailsPane.cs,v 1.19.2.3 2008/09/08 18:24:23 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Runtime.Remoting.Messaging;

using Viatronix.UI;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{

  /// <summary>
  /// Provides detailed information for the currently selected study as well as a preview image, if available.
  /// </summary>
  public class DetailsPane : System.Windows.Forms.UserControl
  {

    #region delegate

    /// <summary>
    /// method signiture for retrieving an image
    /// </summary>
    /// <param name="series">Series object</param>
    /// <returns>an Image</returns>
    private delegate Image PreviewDelegate(Series series);

    /// <summary>
    /// method signature for displaying an image
    /// </summary>
    /// <param name="image"></param>
    private delegate void DisplayPreviewDelegate(Image image);

    #endregion

    #region fields

    /// <summary>
    /// temporary preview image.
    /// </summary>
    private static readonly Image TempLogo = null;

    /// <summary>
    /// Displays the patient information.
    /// </summary>
    private System.Windows.Forms.Label m_patientLabel;

    /// <summary>
    /// Displays the date of birth information.
    /// </summary>
    private System.Windows.Forms.Label m_dobLabel;

    /// <summary>
    /// Displays the scan date information.
    /// </summary>
    private System.Windows.Forms.Label m_scanDateLabel;

    /// <summary>
    /// Displays the gender information.
    /// </summary>
    private System.Windows.Forms.Label m_genderLabel;

    /// <summary>
    /// A label for the description.
    /// </summary>
    private System.Windows.Forms.Label m_descriptionLabel;

    /// <summary>
    /// Displays the description.
    /// </summary>
    private System.Windows.Forms.Label m_description;

    /// <summary>
    /// Displays the medical id.
    /// </summary>
    private System.Windows.Forms.Label m_medicalIDLabel;

    /// <summary>
    /// The top most divider.
    /// </summary>
    private System.Windows.Forms.GroupBox m_topDivider;

    /// <summary>
    /// Displays the physician information.
    /// </summary>
    private System.Windows.Forms.Label m_physicianLabel;

    /// <summary>
    /// The bottom most divider.
    /// </summary>
    private System.Windows.Forms.GroupBox m_bottomDivider;

    /// <summary>
    /// Panel containing the previde image.
    /// </summary>
    private Viatronix.UI.Panel m_imagePanel;

    /// <summary>
    /// Displays the preview image.
    /// </summary>
    private Viatronix.UI.PictureBox m_previewImage;

    /// <summary>
    /// Groupbox containing all controls.
    /// </summary>
    private Viatronix.UI.GroupBox m_groupBox;

    /// <summary>
    /// preview delegate
    /// </summary>
    private PreviewDelegate m_previewDelegate = null;

    /// <summary>
    /// Currently displayed series
    /// </summary>
    private Series m_displayedSeries = null;


    #endregion

    #region construction

    /// <summary>
    /// Loads the temporary preview image.
    /// </summary>
    static DetailsPane()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
      TempLogo = Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.Console.res.tempLogo.PNG" ) );
    } // DetailsPane()


    /// <summary>
    /// Creates a new instance of a DetailsPane control.
    /// </summary>
    public DetailsPane()
    {
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.BackColor = Theme.PaneBackColor;
      this.TabStop = false;

      InitializeComponent();

      m_previewImage.Image = TempLogo;

      Global.Frame.ViewChanged += new CancelEventHandler( this.OnViewChangedHandler );

      if( Global.Frame.ActiveView is ViewBrowser )
      {
        this.ViewBrowser.StudiesSelectionChanged += new StudyCollectionDelegate( this.OnStudiesChangedHandler );
        this.ViewBrowser.SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
        this.ViewBrowser.Searcher.SearchCompleted += new StudyCollectionDelegate( this.OnSearchCompletedHandler );
      }

      m_previewDelegate = new PreviewDelegate( LoadPreviewImage );

      Global.Frame.Panes.PaneExpanded += new RolloutEventHandler( this.OnPaneExpandedHandler );

    } // DetailsPane()

    #endregion

    #region properties

    /// <summary>
    /// Returns a reference to the ViewBrowser.
    /// </summary>
    private ViewBrowser ViewBrowser
    {
      get { return ( ViewBrowser ) Global.Frame.ActiveView; }
    } // ViewBrowser

    #endregion

    #region methods

    /// <summary>
    /// Loads the preview image for the series
    /// </summary>
    /// <param name="series">Series object</param>
    /// <returns>an Image</returns>
    private Image LoadPreviewImage(Series series)
    {
      Image image = TempLogo;
      if ( series != null )
      {
        try
        {
          IDataProvider provider = this.ViewBrowser.BrowserProvider as IDataProvider; 
          if (provider != null)
          {
            image = provider.GetPreviewImage(series);
            if (image == null)
              image = TempLogo;
          }
        }
        catch ( Exception e )
        {
          Viatronix.Logging.Log.Error("Unable to load preview image [ERROR=" + e.Message + "]", "DetailsPane", "LoadPreviewImage");
        }
      }

      return image;
    } // LoadPreviewImage( series )


    /// <summary>
    /// Callback for loading a preview image.
    /// </summary>
    /// <param name="results">IAsyncResult object</param>
    private void PreviewLoadedCallbackHandler( IAsyncResult results )
    {
      Series series = (Series) results.AsyncState;
      if( series != m_displayedSeries )
        return;

      Image image = ( Image ) m_previewDelegate.EndInvoke( results );
      this.Invoke( new DisplayPreviewDelegate( DisplayImage ), new object [ ] { image } );
    } // PreviewLoadedCallbackHandler( results )


    /// <summary>
    /// Displays the image
    /// </summary>
    /// <param name="image">Image being displayed</param>
    private void DisplayImage( Image image )
    {
      m_previewImage.Image = image;
    } // DisplayImage( image )


    /// <summary>
    /// Displays the study information
    /// </summary>
    /// <param name="study">Study object</param>
    private void DisplayStudyInformation( string studyUid )
    {
      DisplayStudyInformation(Global.DataProvider.GetStudy( studyUid));
   } // DisplayStudyInformation( study )


    /// <summary>
    /// Displays the study information
    /// </summary>
    /// <param name="study">Study object</param>
    private void DisplayStudyInformation(Study study)
    {
      if (study == null)
      {
        m_patientLabel.Text = "Patient: ";
        m_dobLabel.Text = "DOB: ";
        m_scanDateLabel.Text = "Scan Date: ";
        m_genderLabel.Text = "Sex: ";
        m_description.Text = string.Empty;
        m_medicalIDLabel.Text = "Medical ID: ";
        m_physicianLabel.Text = "Physician: ";

        m_previewImage.Image = null;
        m_displayedSeries = null;
      }
      else
      {
        m_patientLabel.Text = "Patient: " + study.Name.LastFirstMiddle;
        m_dobLabel.Text = "DOB: " + study.Dob.ToString();
        m_scanDateLabel.Text = "Scan Date: " + study.Date.ToString("M/d/yyyy");
        m_genderLabel.Text = "Sex: " + study.Gender.ToString();
        m_description.Text = study.Description;
        m_medicalIDLabel.Text = "Medical ID: " + study.MedicalId;
        m_physicianLabel.Text = "Physician: " + study.Physician;
      }
    } // DisplayStudyInformation( study )


    #endregion

    #region event handlers

    /// <summary>
    /// Attaches to the actie view's SeriesSelectionChanged event, if one exists.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangedHandler(object sender, EventArgs args)
    {
      Study study = null;
      if ( Global.Frame.ActiveView is ViewBrowser )
      {
        this.ViewBrowser.StudiesSelectionChanged += new StudyCollectionDelegate( this.OnStudiesChangedHandler );
        this.ViewBrowser.SeriesSelectionChanged += new SeriesCollectionDelegate( this.OnSeriesChangedHandler );
        this.ViewBrowser.Searcher.SearchCompleted += new StudyCollectionDelegate( this.OnSearchCompletedHandler );

        if (this.ViewBrowser.SelectedStudies.Count > 0)
          study = this.ViewBrowser.SelectedStudies[0];
      }

      // display study information
      DisplayStudyInformation( study );

    } // OnViewChangedHandler( sender, args )


    /// <summary>
    /// Updates the labels with the inforamtion from the provided study.
    /// </summary>
    /// <param name="studies">The currently selected studies in the ViewBrowser.</param>
    private void OnStudiesChangedHandler(List<Study> studies)
    {
      // Need to check so that exceptions aren't thrown on the exit.
      //if(this.Parent == null)
      //{
      //  return;
      //} // if(this.Parent == null)

      Study study = null;
      if( studies.Count > 0 )
        study = studies [ 0 ];

      DisplayStudyInformation( study );

      if( study != null && study.Series.Count > 0 && (( Viatronix.UI.RolloutPane ) this.Parent ).Expanded )
      {
        Series series = ( this.ViewBrowser.CurrentSeries != null ) ? this.ViewBrowser.CurrentSeries : study.Series[ 0 ];
        if( series != m_displayedSeries )
        {
          m_displayedSeries = series;
          AsyncCallback asyncCallback = new AsyncCallback( this.PreviewLoadedCallbackHandler );
          m_previewDelegate.BeginInvoke( series, asyncCallback, series );
        }
      }
    } // OnStudyChangedHandler( study )


    /// <summary>
    /// Updates the preview image based on the newly selected series.
    /// </summary>
    /// <param name="series">The currently selected series in the ViewBrowser.</param>
    private void OnSeriesChangedHandler(List<Series> collection)
    {
      // Need to check so that exceptions aren't thrown on the exit.
      if( collection.Count == 0 /** || Parent == null */ )
      {
        return;
      } // if( collection.Count == 0 || Parent == null )

      DisplayStudyInformation( collection[ 0 ].StudyUid );

      if( collection.Count > 0 && ( ( Viatronix.UI.RolloutPane ) ( this.Parent ) ).Expanded )
      {
        Series series = ( this.ViewBrowser.CurrentSeries != null ) ? this.ViewBrowser.CurrentSeries : collection[ 0 ];
        if( series != m_displayedSeries )
        {
          m_displayedSeries = series;
          AsyncCallback asyncCallback = new AsyncCallback( this.PreviewLoadedCallbackHandler );
          m_previewDelegate.BeginInvoke( m_displayedSeries, asyncCallback, m_displayedSeries );
        }
      }
    } // OnSeriesChangedHandler( study )


    /// <summary>
    /// Handler for the search comlpeted handler
    /// </summary>
    /// <param name="collection">List<Study></param>
    private void OnSearchCompletedHandler( List<Study> collection )
    {
      if( collection.Count == 0 )
        DisplayStudyInformation( (Study)null );
    } // OnSearchCompletedHandler( collection )


    /// <summary>
    /// Assigns the SearchManager.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An RolloutEventArgs that contains the event data.</param>
    private void OnPaneExpandedHandler(object sender, RolloutEventArgs args)
    {
      // assign the SearchManager to the expanding pane if it implements the ISearchable interface
      if ( args.Pane.Controls [ 0 ] is DetailsPane )
      {
        if ( this.ViewBrowser.SelectedSeries.Count > 0 )
        {
          AsyncCallback asyncCallback = new AsyncCallback( this.PreviewLoadedCallbackHandler );
          if ( ViewBrowser.CurrentSeries != null )
            m_previewDelegate.BeginInvoke( this.ViewBrowser.CurrentSeries, asyncCallback, this.ViewBrowser.CurrentSeries );
          else
            m_previewDelegate.BeginInvoke(this.ViewBrowser.SelectedSeries[0], asyncCallback, this.ViewBrowser.SelectedSeries[0]);
        }
      }
    }//OnPaneExpandedHandler( sender, args )

    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_groupBox = new Viatronix.UI.GroupBox();
      this.m_imagePanel = new Viatronix.UI.Panel();
      this.m_previewImage = new Viatronix.UI.PictureBox();
      this.m_bottomDivider = new System.Windows.Forms.GroupBox();
      this.m_physicianLabel = new System.Windows.Forms.Label();
      this.m_topDivider = new System.Windows.Forms.GroupBox();
      this.m_medicalIDLabel = new System.Windows.Forms.Label();
      this.m_description = new System.Windows.Forms.Label();
      this.m_descriptionLabel = new System.Windows.Forms.Label();
      this.m_genderLabel = new System.Windows.Forms.Label();
      this.m_scanDateLabel = new System.Windows.Forms.Label();
      this.m_dobLabel = new System.Windows.Forms.Label();
      this.m_patientLabel = new System.Windows.Forms.Label();
      this.m_groupBox.SuspendLayout();
      this.m_imagePanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_groupBox
      // 
      this.m_groupBox.BackColor = System.Drawing.Color.Transparent;
      this.m_groupBox.BorderColor = System.Drawing.Color.FromArgb( ( ( System.Byte ) ( 89 ) ), ( ( System.Byte ) ( 116 ) ), ( ( System.Byte ) ( 153 ) ) );
      this.m_groupBox.Controls.Add( this.m_imagePanel );
      this.m_groupBox.Controls.Add( this.m_bottomDivider );
      this.m_groupBox.Controls.Add( this.m_physicianLabel );
      this.m_groupBox.Controls.Add( this.m_topDivider );
      this.m_groupBox.Controls.Add( this.m_medicalIDLabel );
      this.m_groupBox.Controls.Add( this.m_description );
      this.m_groupBox.Controls.Add( this.m_descriptionLabel );
      this.m_groupBox.Controls.Add( this.m_genderLabel );
      this.m_groupBox.Controls.Add( this.m_scanDateLabel );
      this.m_groupBox.Controls.Add( this.m_dobLabel );
      this.m_groupBox.Controls.Add( this.m_patientLabel );
      this.m_groupBox.FillColor = System.Drawing.Color.Transparent;
      this.m_groupBox.Location = new System.Drawing.Point( 0, 0 );
      this.m_groupBox.Name = "m_groupBox";
      this.m_groupBox.Size = new System.Drawing.Size( 296, 416 );
      this.m_groupBox.TabColor = System.Drawing.Color.Transparent;
      this.m_groupBox.TabIndex = 0;
      this.m_groupBox.TabStop = false;
      // 
      // m_imagePanel
      // 
      this.m_imagePanel.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
        | System.Windows.Forms.AnchorStyles.Left )
        | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.m_imagePanel.BackColor = System.Drawing.Color.Transparent;
      this.m_imagePanel.BorderColor = System.Drawing.Color.FromArgb( ( ( System.Byte ) ( 51 ) ), ( ( System.Byte ) ( 51 ) ), ( ( System.Byte ) ( 102 ) ) );
      this.m_imagePanel.BottomPadding = 0;
      this.m_imagePanel.Controls.Add( this.m_previewImage );
      this.m_imagePanel.Curve = 10;
      this.m_imagePanel.Edges = Viatronix.UI.Edges.All;
      this.m_imagePanel.FillColor = System.Drawing.Color.FromArgb( ( ( System.Byte ) ( 148 ) ), ( ( System.Byte ) ( 166 ) ), ( ( System.Byte ) ( 198 ) ) );
      this.m_imagePanel.LeftPadding = 0;
      this.m_imagePanel.Location = new System.Drawing.Point( 48, 200 );
      this.m_imagePanel.Name = "m_imagePanel";
      this.m_imagePanel.RightPadding = 0;
      this.m_imagePanel.Size = new System.Drawing.Size( 200, 200 );
      this.m_imagePanel.TabIndex = 11;
      this.m_imagePanel.TopPadding = 0;
      // 
      // m_previewImage
      // 
      this.m_previewImage.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
        | System.Windows.Forms.AnchorStyles.Left )
        | System.Windows.Forms.AnchorStyles.Right ) ) );
      this.m_previewImage.BackColor = System.Drawing.Color.Black;
      this.m_previewImage.Location = new System.Drawing.Point( 8, 8 );
      this.m_previewImage.Name = "m_previewImage";
      this.m_previewImage.Size = new System.Drawing.Size( 184, 184 );
      this.m_previewImage.TabIndex = 0;
      this.m_previewImage.TabStop = false;
      // 
      // m_bottomDivider
      // 
      this.m_bottomDivider.BackColor = System.Drawing.Color.Transparent;
      this.m_bottomDivider.Location = new System.Drawing.Point( 16, 184 );
      this.m_bottomDivider.Name = "m_bottomDivider";
      this.m_bottomDivider.Size = new System.Drawing.Size( 264, 8 );
      this.m_bottomDivider.TabIndex = 10;
      this.m_bottomDivider.TabStop = false;
      // 
      // m_physicianLabel
      // 
      this.m_physicianLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_physicianLabel.ForeColor = System.Drawing.Color.White;
      this.m_physicianLabel.Location = new System.Drawing.Point( 16, 128 );
      this.m_physicianLabel.Name = "m_physicianLabel";
      this.m_physicianLabel.Size = new System.Drawing.Size( 264, 23 );
      this.m_physicianLabel.TabIndex = 9;
      this.m_physicianLabel.Text = "Physician: [First Last]";
      this.m_physicianLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_topDivider
      // 
      this.m_topDivider.BackColor = System.Drawing.Color.Transparent;
      this.m_topDivider.Location = new System.Drawing.Point( 16, 88 );
      this.m_topDivider.Name = "m_topDivider";
      this.m_topDivider.Size = new System.Drawing.Size( 264, 8 );
      this.m_topDivider.TabIndex = 8;
      this.m_topDivider.TabStop = false;
      // 
      // m_medicalIDLabel
      // 
      this.m_medicalIDLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_medicalIDLabel.ForeColor = System.Drawing.Color.White;
      this.m_medicalIDLabel.Location = new System.Drawing.Point( 16, 40 );
      this.m_medicalIDLabel.Name = "m_medicalIDLabel";
      this.m_medicalIDLabel.Size = new System.Drawing.Size( 264, 23 );
      this.m_medicalIDLabel.TabIndex = 7;
      this.m_medicalIDLabel.Text = "Medical ID: 123-45-6789";
      this.m_medicalIDLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_description
      // 
      this.m_description.BackColor = System.Drawing.Color.Transparent;
      this.m_description.ForeColor = System.Drawing.Color.White;
      this.m_description.Location = new System.Drawing.Point( 80, 154 );
      this.m_description.Name = "m_description";
      this.m_description.Size = new System.Drawing.Size( 200, 30 );
      this.m_description.TabIndex = 6;
      // 
      // m_descriptionLabel
      // 
      this.m_descriptionLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_descriptionLabel.ForeColor = System.Drawing.Color.White;
      this.m_descriptionLabel.Location = new System.Drawing.Point( 16, 154 );
      this.m_descriptionLabel.Name = "m_descriptionLabel";
      this.m_descriptionLabel.Size = new System.Drawing.Size( 64, 23 );
      this.m_descriptionLabel.TabIndex = 5;
      this.m_descriptionLabel.Text = "Description:";
      // 
      // m_genderLabel
      // 
      this.m_genderLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_genderLabel.ForeColor = System.Drawing.Color.White;
      this.m_genderLabel.Location = new System.Drawing.Point( 160, 64 );
      this.m_genderLabel.Name = "m_genderLabel";
      this.m_genderLabel.Size = new System.Drawing.Size( 120, 23 );
      this.m_genderLabel.TabIndex = 4;
      this.m_genderLabel.Text = "Sex: [Male/Female]";
      this.m_genderLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // m_scanDateLabel
      // 
      this.m_scanDateLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_scanDateLabel.ForeColor = System.Drawing.Color.White;
      this.m_scanDateLabel.Location = new System.Drawing.Point( 16, 104 );
      this.m_scanDateLabel.Name = "m_scanDateLabel";
      this.m_scanDateLabel.Size = new System.Drawing.Size( 264, 23 );
      this.m_scanDateLabel.TabIndex = 3;
      this.m_scanDateLabel.Text = "Scan Date: 1/1/01";
      this.m_scanDateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_dobLabel
      // 
      this.m_dobLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_dobLabel.ForeColor = System.Drawing.Color.White;
      this.m_dobLabel.Location = new System.Drawing.Point( 16, 64 );
      this.m_dobLabel.Name = "m_dobLabel";
      this.m_dobLabel.Size = new System.Drawing.Size( 136, 23 );
      this.m_dobLabel.TabIndex = 2;
      this.m_dobLabel.Text = "Date of Birth: 1/1/01";
      this.m_dobLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_patientLabel
      // 
      this.m_patientLabel.BackColor = System.Drawing.Color.Transparent;
      this.m_patientLabel.ForeColor = System.Drawing.Color.White;
      this.m_patientLabel.Location = new System.Drawing.Point( 16, 16 );
      this.m_patientLabel.Name = "m_patientLabel";
      this.m_patientLabel.Size = new System.Drawing.Size( 264, 23 );
      this.m_patientLabel.TabIndex = 1;
      this.m_patientLabel.Text = "Patient: [Last Name, First Name]";
      this.m_patientLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // DetailsPane
      // 
      this.Controls.Add( this.m_groupBox );
      this.Name = "DetailsPane";
      this.Size = new System.Drawing.Size( 300, 416 );
      this.m_groupBox.ResumeLayout( false );
      this.m_imagePanel.ResumeLayout( false );
      this.ResumeLayout( false );

    }
    #endregion

  } // class DetailsPane
} // namespace Viatronix.Console

#region revision history

// $Log: DetailsPane.cs,v $
// Revision 1.19.2.3  2008/09/08 18:24:23  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
// Revision 1.19.2.2  2007/03/28 21:02:44  gdavidson
// Issue #5535: Reset the DetailsPane when a search doesn't return any results
//
// Revision 1.19.2.1  2007/03/26 18:22:11  gdavidson
// Issue #5516: Don't display preview image if the series has changed
//
// Revision 1.19  2007/03/09 20:10:54  gdavidson
// Removed unneccesary access to cache
//
// Revision 1.18  2007/02/28 17:35:49  gdavidson
// Populated study information in the ViewChanged event handler
//
// Revision 1.17  2007/02/26 19:00:12  gdavidson
// Commented Code
//
// Revision 1.16  2007/02/26 17:58:41  gdavidson
// Set the TabStop option to false
//
// Revision 1.15  2007/01/18 18:42:42  romy
// setting the preview image when study selection changes
//
// Revision 1.14  2007/01/18 16:19:52  romy
// setting the preview image when study selection changes
//
// Revision 1.13  2006/07/24 14:17:53  gdavidson
// Issue #4874: Added general exception handling when loading the preview image
//
// Revision 1.12  2006/07/19 13:48:27  gdavidson
// Added exception handling when loading the preview image
//
// Revision 1.11  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.10  2006/03/02 17:12:36  gdavidson
// Swaped the winform's picturebox for a Viatronix.UI.PictureBox
//
// Revision 1.9  2006/01/09 19:50:26  gdavidson
// Added multiple selection of Study objects.
//
// Revision 1.8  2005/12/14 19:54:12  gdavidson
// Modified the DetailsPane to work with multiple series selection.
//
// Revision 1.7  2005/11/21 19:13:44  romy
// added fix to clear last detail info during tab change.
//
// Revision 1.6  2005/10/26 19:10:13  gdavidson
// Modified the pane to accept null series.
//
// Revision 1.5  2005/08/31 12:33:31  gdavidson
// Made the loading of the preview image asynchronous.
//
// Revision 1.4  2005/06/24 14:53:04  gdavidson
// Modified the preview image.
//
// Revision 1.3  2005/06/22 16:04:05  gdavidson
// Retrieve preview image from DataProvider.
//
// Revision 1.2  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.1  2005/05/26 17:46:12  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/panes/DetailsPane.cs,v 1.19.2.3 2008/09/08 18:24:23 kchalupa Exp $
// $Id: DetailsPane.cs,v 1.19.2.3 2008/09/08 18:24:23 kchalupa Exp $

#endregion