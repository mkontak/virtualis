// $Id: ViewBrowser.cs,v 1.90.2.20 2011/05/18 01:56:59 mkontak Exp $
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
using System.Diagnostics;
using System.IO.IsolatedStorage;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using System.Linq;


using Viatronix.UI;
//using Viatronix.v3D.Anonymization;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

using BrightIdeasSoftware;


namespace Viatronix.Console
{


  /// <summary>
  /// Provides the GUI for browsing different datasources.
  /// </summary>
  public class ViewBrowser : UserControl, ISearchView, IBrowser, IPersistable, ISearchable, ILaunchable
  {

    #region enums

    internal enum TimerState
    {
      Started     = 0,            // Started - running waiting for tick event
      Stopped     = 1,            // Stopped - Stopped wait for processing   
      Processing  = 3             // Stopped and currently processing
    };

    #endregion

    #region fields


    private OLVColumn m_typeColumn = new OLVColumn();

    /// <summary>
    /// Configuration study state instructions:
    ///   animated        Animated icons
    ///   static          Static icons
    ///   none            No icons
    /// </summary>
    private string m_studyStateIndicator = ConfigurationManager.AppSettings["studyStateIndicator"].ToString();

    /// <summary>
    /// Header format style for the study and series list views
    /// </summary>
    private HeaderFormatStyle m_listViewHeaderFormatStyle = new HeaderFormatStyle();

    /// <summary>
    /// state image list
    /// </summary>
    private Dictionary<string, Image> m_stateImages = new Dictionary<string, Image>();

    /// <summary>
    /// Flag inticating tha the refresh is stopping
    /// </summary>
    private TimerState m_refreshTimerState = TimerState.Stopped;

    /// <summary>
    /// 
    /// </summary>
    private System.Windows.Forms.Timer m_refreshTimer = new System.Windows.Forms.Timer ();


    /// <summary>
    /// Holds the current study
    /// </summary>
    private Study m_currentStudy = null;

    /// <summary>
    /// Holds the current series
    /// </summary>
    private Series m_currentSeries = null;

    /// <summary>
    /// Maintains the path to the user's study configuration file.
    /// </summary>
    private readonly string m_studyUserPath = string.Empty;

    /// <summary>
    /// Maintains the path to the user's series configuration file.
    /// </summary>
    private readonly string m_seriesUserPath = string.Empty;
    private IContainer components;

    /// <summary>
    /// Reference to the loaded view provider.
    /// </summary>
    private IBrowserProvider m_dataProvider = null;

    /// <summary>
    /// Reference to the view's provider paframeters used to build query
    /// </summary>
    private XmlBuilder m_query = new XmlBuilder("study");

    /// <summary>
    /// Keeps the transfer provider node
    /// </summary>
    private System.Xml.XmlNodeList m_transferProvidersNode = null;

    /// <summary>
    /// clear study items handler
    /// </summary>
    private System.Windows.Forms.MethodInvoker m_searchResetHandler = null;

    /// <summary>
    /// Information used to enable and disable browser functionality.
    /// </summary>
    private ViewBrowserInfo m_viewBrowserInfo = null;

    /// <summary>
    /// Reference to a launch manager
    /// </summary>
    private LaunchManager m_launchManager = null;

    /// <summary>
    /// Launchining manager
    /// </summary>
    private Viatronix.Enterprise.Launching.LaunchManager m_launchingManager = null;

    /// <summary>
    /// Reference to a search manager
    /// </summary>
    private SearchManager m_searchManager = null;

 
    /// <summary>
    /// Cached list of studies currently retrieved from the V3D System. These studies are loaded into list view and are
    /// rebuilt during a search
    /// </summary>
    private List<Study> m_studyList = new List<Study>();


    /// <summary>
    /// Timer to call the search after form load happens.
    /// </summary>
    private System.Windows.Forms.Timer m_loadSearchTimer = new System.Windows.Forms.Timer();

    /// <summary>
    /// Occurs when the selected study has changed.
    /// </summary>
    public event StudyCollectionDelegate StudiesSelectionChanged = null;

    /// <summary>
    /// Occurs when the selected series has changed.
    /// </summary>
    public event SeriesCollectionDelegate SeriesSelectionChanged = null;

    #region gui elements

    /// <summary>
    /// The top panel of the browser view.
    /// </summary>
    private Viatronix.UI.Panel m_topPanel;

    /// <summary>
    /// The middle panel of the browser view.
    /// </summary>
    private Viatronix.UI.Panel m_middlePanel;

    /// <summary>
    /// The studies label.
    /// </summary>
    private System.Windows.Forms.Label m_studiesLabel;

    /// <summary>
    /// The series label.
    /// </summary>
    private System.Windows.Forms.Label m_seriesLabel;

    /// <summary>
    /// Displays the studies.
    /// </summary>
    private ObjectListView m_studies;

    /// <summary>
    /// Displays the series.
    /// </summary>
    private ObjectListView m_series;

    /// <summary>
    /// Displays detailed informatino for the selected series.
    /// </summary>
    private SeriesDetailsPanel m_detailsPanel;

    /// <summary>
    /// Splitter between the studies and series.
    /// </summary>
    private System.Windows.Forms.Splitter m_splitter;

    /// <summary>
    /// Event handler for the closing event.
    /// </summary>
    private CancelEventHandler m_closingHandler = null;

    /// <summary>
    /// Panel used to group the series and detailPanel.
    /// </summary>
    private System.Windows.Forms.Panel m_seriesPanel;

    /// <summary>
    /// Event handler for the view changing event.
    /// </summary>
    private CancelEventHandler m_viewChangingHandler = null;

    /// <summary>
    /// Event handler for the series listview Changed event.
    /// </summary>
    private EventHandler m_seriesChangedHandler = null;
      
    /// <summary>
    /// Splitter between the series listview and detailsPanel.
    /// </summary>
    private System.Windows.Forms.Splitter m_seriesSplitter;

    /// <summary>
    /// Study context menu
    /// </summary>
    private StudyContextMenu m_studyContextMenu = null;

    /// <summary>
    /// contains the launch control
    /// </summary>
    private Viatronix.UI.Panel m_launchPanel;

    /// <summary>
    /// label for the launch panel
    /// </summary>
    private Label m_launchLabel;

    /// <summary>
    /// lower panel
    /// </summary>
    private System.Windows.Forms.Panel m_lowerPanel;

    /// <summary>
    /// vertical splitter on the lower panel
    /// </summary>
    private Splitter m_verticalSplitter;

    /// <summary>
    /// launch control
    /// </summary>
    private ViewerSelector m_launchPane;

    /// <summary>
    /// lower series panel
    /// </summary>
    private System.Windows.Forms.Panel m_lowerSeriesPanel;
    
    /// <summary>
    /// context menu for the series
    /// </summary>
    private SeriesContextMenu m_seriesContextMenu = null;

    /// <summary>
    /// Simple mode
    /// </summary>
    private bool m_simpleMode = false;

    /// <summary>
    /// List of simple mode rules
    /// </summary>
    private SimpleModeRuleCollection m_simpleModeRules = new SimpleModeRuleCollection();

    #endregion

    #endregion

    #region construction
  
    /// <summary>
    /// Initializes a ViewBrowser instance with default values
    /// </summary>
    public ViewBrowser()
    {

      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      
      // Initialize the components
      InitializeComponent();


      m_seriesChangedHandler = new EventHandler( this.OnSeriesSelectionChangedHandler );
      //m_series.SelectedIndexChanged += m_seriesChangedHandler;

      this.BackColor = Theme.BackColor;
      m_splitter.BackColor = Theme.BackColor;
      m_topPanel.BackColor = Theme.BackColor;
      m_middlePanel.BackColor = Theme.BackColor;

      //m_splitter.MinExtra = (int)( this.Height / 2.75 );
      //m_splitter.MinSize = (int)( this.Height / 2.75 );

      m_middlePanel.Height = (int)( this.Height * .45 );
      m_detailsPanel.Height = (int)( m_seriesPanel.Height / 1.75 );

      m_searchResetHandler = new MethodInvoker(this.SearchReset);

      // create the user paths for the studies and series column preferences
      // TODO: The next two statements will crash if you leave the login screen and the console times out because the CurrentUser is NULL
      m_studyUserPath  = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewBrowser_Studies.xml" );
      m_seriesUserPath = Path.Combine(Global.CurrentUser.PreferencesDirectory, "ViewBrowser_Series.xml");

      m_closingHandler = new CancelEventHandler( this.OnClosingHandler );
      m_viewChangingHandler = new CancelEventHandler( this.OnViewChangingHandler );

      Global.Frame.Panes.PaneExpanded += new RolloutEventHandler( this.OnPaneExpandedHandler );  
      m_launchPane.ViewerSelected += new SetterDelegate< IViewerLauncher>( this.OnLauncherSelectedHandler );


      m_simpleMode = (System.Configuration.ConfigurationManager.AppSettings["SimpleMode"] == null ? false : System.Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["SimpleMode"]));

      if (m_simpleMode)
      {
        m_simpleModeRules = (SimpleModeRuleCollection)System.Configuration.ConfigurationManager.GetSection("simpleModeRules");
      }


      // Event removed
      //m_series.ShowGroupsChanged += new EventHandler(OnSeriesShowGroupsChanged);


      InitializeListViews();

      m_refreshTimer.Interval = 10000;
      m_refreshTimer.Tick += OnRefreshTimerTick;


     } // ViewBrowser()



    /// <summary>
    /// Initialize the list views
    /// </summary>
    private void InitializeListViews()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      if ( string.Compare(m_studyStateIndicator, "animated", true) == 0 )
      {
        m_stateImages.Add("receiving", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.receiving.gif")));
        m_stateImages.Add("processing", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.processing.gif")));
        m_stateImages.Add("locked", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.locked.gif")));
        m_stateImages.Add("failed", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.error.png")));
        m_stateImages.Add("pending", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.pending.gif")));
        m_stateImages.Add("warning", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.warning.png")));
      }
      else
      {
        m_stateImages.Add("receiving", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.receiving.png")));
        m_stateImages.Add("processing", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.processing.png")));
        m_stateImages.Add("locked", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.locked.png")));
        m_stateImages.Add("failed", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.error.png")));
        m_stateImages.Add("pending", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.pending.png")));
        m_stateImages.Add("warning", new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.warning.png")));
      }

      HeaderStateStyle headerStateStyle1 = new HeaderStateStyle();
      HeaderStateStyle headerStateStyle2 = new HeaderStateStyle();
      HeaderStateStyle headerStateStyle3 = new HeaderStateStyle();

      headerStateStyle1.BackColor = System.Drawing.Color.DarkGray;
      headerStateStyle1.ForeColor = System.Drawing.Color.WhiteSmoke;
      m_listViewHeaderFormatStyle.Hot = headerStateStyle1;

      headerStateStyle2.BackColor = System.Drawing.Color.LightGray;
      headerStateStyle2.ForeColor = System.Drawing.Color.Black;
      m_listViewHeaderFormatStyle.Normal = headerStateStyle2;

      headerStateStyle3.BackColor = Color.DarkBlue;
      headerStateStyle3.ForeColor = System.Drawing.Color.White;
      headerStateStyle3.FrameColor = System.Drawing.Color.WhiteSmoke;
      headerStateStyle3.FrameWidth = 2F;
      m_listViewHeaderFormatStyle.Pressed = headerStateStyle3;



      InitializeStudiesListView();

      InitializeSeriesListView();

    } // InitializeListViews

    /// <summary>
    /// Initializes the study list view
    /// </summary>
    private void InitializeStudiesListView()
    {
      // ===========================================================
      // Initializes the object list view with the study object
      // ==========================================================
      TypedObjectListView<Study> tlist = new TypedObjectListView<Study>(m_studies);
      tlist.GenerateAspectGetters();

 
      m_studies.FullRowSelect = true;
      m_studies.MultiSelect = true;
      m_studies.ShowGroups = false;
      m_studies.ShowImagesOnSubItems = true;
      m_studies.OwnerDraw = true;
      m_studies.UseCellFormatEvents = true;
      m_studies.UseCompatibleStateImageBehavior = false;
      m_studies.UseFilterIndicator = true;
      m_studies.UseFiltering = true;
      m_studies.UseHotItem = true;
      m_studies.UseTranslucentHotItem = true;
      m_studies.ShowCommandMenuOnRightClick = true;
      m_studies.SelectColumnsOnRightClickBehaviour = BrightIdeasSoftware.ObjectListView.ColumnSelectBehaviour.Submenu;
      m_studies.HeaderUsesThemes = false;
      m_studies.OwnerDraw = true;
      m_studies.GridLines = true;
      m_studies.ShowItemCountOnGroups = true;
      m_studies.CellToolTipGetter = delegate(OLVColumn column, object obj)
      {
        string toolTip = string.Empty;

        if (column.AspectName == "State")
          toolTip = ((Study)obj).State.ToString();

        return toolTip;
      };

      m_studies.HeaderFormatStyle = m_listViewHeaderFormatStyle;

      // =============================================================================
      // Dummy column is needed because the first column does not have bounds which
      // makes the state column not animate.
      // ============================================================================
      OLVColumn imageColumn = new OLVColumn();
      imageColumn.AspectName = "";
      imageColumn.Text = "";
      imageColumn.ShowTextInHeader = false;
      imageColumn.Width = 0;
      imageColumn.CellPadding = null;
      imageColumn.IsTileViewColumn = false;
      imageColumn.Groupable = false;
      imageColumn.Sortable = false;
      imageColumn.UseFiltering = false;
      imageColumn.Searchable = false;
      imageColumn.Hideable = true;
      imageColumn.IsVisible = false;
      m_studies.Columns.Add(imageColumn);


      // ========================================
      // State Column - Used for the icon/gif
      // ========================================
      OLVColumn olvColumn = new OLVColumn();
      olvColumn.AspectName = "State";
      olvColumn.Text = "State";
      olvColumn.ShowTextInHeader = false;
      olvColumn.Width = 28;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.Renderer = new ImageRenderer(false);
      //olvColumn.Renderer.ListView = m_studies;
      olvColumn.AspectGetter = delegate(object row)
      {

        Image image = null;

        Study study = (Study)row;

        if (study.IsReceiving)
          image = m_stateImages["receiving"];
        else if (study.IsProcessing)
          image = m_stateImages["processing"];
        else if (study.IsLocked)
          image = m_stateImages["locked"];
        else if (study.IsFailed)
          image = m_stateImages["failed"];
        else if (study.IsState(States.Pending))
          image = m_stateImages["pending"];
        else if (study.IsState(States.Warning))
          image = m_stateImages["warning"];

        return image;

      };

      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // ===============
      // Patient Name
      // ===============
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "Name";
      olvColumn.Text = "Name";
      olvColumn.Width  = 250;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.UseFiltering = false;
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // =============
      // Medical Id
      // =============
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "MedicalId";
      olvColumn.Text = "Medical Id";
      olvColumn.Width  = 250;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).MedicalId; };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // ===========
      // Scan Date
      // ===========
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "Date";
      olvColumn.Text = "Scan Date";
      olvColumn.Width  = 90;
      olvColumn.AspectToStringFormat = "{0:d}";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Date; };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // ==================
      // Study Description
      // ==================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Description";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Description";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Description; };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // ====================
      // Referring Physician
      // ====================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Referring Physician";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Physician";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Physician; };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);

 
      // ====================
      // Accession Number
      // ====================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Accession Number";
      olvColumn.Width = 150;
      olvColumn.AspectName = "AccessionNumber";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).AccessionNumber; };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);

      // ======================
      // Applcations list
      // ======================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Applications";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Applications";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)obj).Applications.ToDelimtedString(','); };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);

      // ==================
      // Modalities List
      // ==================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Modalities";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Modalities";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) {  return ((Study)obj).Modalities.ToDelimtedString(','); };
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);


      // ==============
      // Study Group
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Group";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Group";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Group; };
      m_studies.AllColumns.Add(olvColumn);


      // ==========
      // Study Id
      // ==========
      olvColumn = new OLVColumn();
      olvColumn.Text = "Id";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Id";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Id; };
      m_studies.AllColumns.Add(olvColumn);


      // ==============
      // Date of Birth
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Date of Birth";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Dob";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectToStringFormat = "{0:d}";
      olvColumn.Hideable = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Dob; };
      m_studies.AllColumns.Add(olvColumn);

      // ===============
      // Patient Wieght
      // ===============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Weight";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Weight";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Weight; };
      m_studies.AllColumns.Add(olvColumn);

      // ===============
      // Study Uid
      // ===============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Uid";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Uid";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Study)(obj)).Uid; };
      m_studies.AllColumns.Add(olvColumn);

      // ==============
      // Filler column
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "";
      olvColumn.Text = "";
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.FillsFreeSpace = true;
      m_studies.AllColumns.Add(olvColumn);
      m_studies.Columns.Add(olvColumn);

    }


    /// <summary>
    /// Initializes the study list view
    /// </summary>
    private void InitializeSeriesListView()
    {
      
      // ===========================================================
      // Initializes the object list view with the study object
      // ==========================================================
      TypedObjectListView<Series> tlist = new TypedObjectListView<Series>(m_series);
      tlist.GenerateAspectGetters();


      m_series.FullRowSelect = true;
      m_series.MultiSelect = true;
      m_series.ShowGroups = true;
      m_series.ShowImagesOnSubItems = true;
      m_series.UseCellFormatEvents = true;
      m_series.UseCompatibleStateImageBehavior = false;
      m_series.UseFilterIndicator = true;
      m_series.UseFiltering = true;
      m_series.UseHotItem = true;
      m_series.UseTranslucentHotItem = true;
      m_series.ShowCommandMenuOnRightClick = true;
      m_series.SelectColumnsOnRightClickBehaviour = BrightIdeasSoftware.ObjectListView.ColumnSelectBehaviour.Submenu;
      m_series.HeaderUsesThemes = false;
      m_series.OwnerDraw = true;
      m_series.GridLines = true;
      m_series.HeaderFormatStyle = m_listViewHeaderFormatStyle;
      m_series.ShowSortIndicators = true;
     

      // =============================================================================
      // Dummy column is needed because the first column does not have bounds which
      // makes the state column not animate.
      // ============================================================================
      //OLVColumn imageColumn = new OLVColumn();
      //imageColumn.AspectName = "";
      //imageColumn.Text = "";
      //imageColumn.ShowTextInHeader = false;
      //imageColumn.Width = 0;
      //imageColumn.CellPadding = null;
      //imageColumn.IsTileViewColumn = false;
      //imageColumn.Groupable = false;
      //imageColumn.Sortable = false;
      //imageColumn.UseFiltering = false;
      //imageColumn.Searchable = false;
      //imageColumn.Hideable = true;
      //imageColumn.IsVisible = false;

      //m_series.AllColumns.Add(dummy);
      //m_series.Columns.Add(imageColumn);


      //OLVColumn olvColumn = new OLVColumn();
      //olvColumn.AspectName = "State";
      //olvColumn.Text = "State";
      //olvColumn.ShowTextInHeader = false;
      //olvColumn.Width = 30;
      //olvColumn.CellPadding = null;
      //olvColumn.IsTileViewColumn = false;
      //olvColumn.Groupable = false;
      //olvColumn.Sortable = false;
      //olvColumn.UseFiltering = false;
      //olvColumn.Renderer = m_imageRenderer;
      //m_imageRenderer.ListView = m_series;
      //olvColumn.AspectGetter = delegate(object row)
      //{

      //  Image image = null;

      //  Series series = (Series)row;

      //  if (series.IsReceiving)
      //    image = m_stateImages["receiving"];
      //  else if (series.IsProcessing)
      //    image = m_stateImages["processing"];
      //  else if (series.IsLocked)
      //    image = m_stateImages["locked"];
      //  else if (series.IsFailed)
      //    image = m_stateImages["failed"];
      //  else if (series.IsState(States.Pending))
      //    image = m_stateImages["pending"];

      //  return image;

      //};

      //m_series.AllColumns.Add(olvColumn);
      //m_series.Columns.Add(olvColumn);




      // ======================
      // Series Description
      // ======================
      OLVColumn olvColumn = new OLVColumn();
      olvColumn.AspectName = "Description";
      olvColumn.Text = "Description";
      olvColumn.Width = 250;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Sortable = false;
      olvColumn.Groupable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Description; };
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);

      // ============
      // Series type
      // ============
      //olvColumn = new OLVColumn();
      m_typeColumn.AspectName = "Type";
      m_typeColumn.Text = "Type";
      m_typeColumn.Width = 100;
      m_typeColumn.CellPadding = null;
      m_typeColumn.Sortable = true;
      m_typeColumn.IsTileViewColumn = false;
      m_typeColumn.Groupable = true;
      m_typeColumn.UseFiltering = true;
      m_typeColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Type; };
      m_series.AllColumns.Add(m_typeColumn);
      m_series.Columns.Add(m_typeColumn);

     

      // =======================================
      // Application associated with the series
      // =======================================
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "Application";
      olvColumn.Text = "Application";
      olvColumn.Width = 90;
      olvColumn.CellPadding = null;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Application; };
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);

      // =====================
      // Patient orientation
      // =====================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Orientation";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Orientation";
      olvColumn.CellPadding = null;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).PatientOrientation; };
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);


      // ===================================
      // Modality associated with the series
      // ====================================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Modality";
      olvColumn.Width = 90;
      olvColumn.AspectName = "Modality";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Modality; };
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);

      // ============
      // Image Count
      // ============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Image Count";
      olvColumn.Width = 100;
      olvColumn.AspectName = "ImageCount";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).ImageCount; };
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);



      // =============
      // Series state 
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "State";
      olvColumn.Width = 100;
      olvColumn.AspectName = "State";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)obj).State.ToString(); };
      m_series.AllColumns.Add(olvColumn);


      // =============
      // Series state 
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Message";
      olvColumn.Width = 200;
      olvColumn.AspectName = "Message";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)obj).Message; };
      m_series.AllColumns.Add(olvColumn);


      // =============
      // Series source 
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Source";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Source";
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)obj).Source; };
      m_series.AllColumns.Add(olvColumn);

      // ===============
      // Series Number
      // ==============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Number";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Number";
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)obj).Number; };
      m_series.AllColumns.Add(olvColumn);

      // ===============
      // Series Uid
      // ===============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Uid";
      olvColumn.Width = 150;
      olvColumn.AspectName = "Uid";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Uid; };
      m_series.AllColumns.Add(olvColumn);

      // ==============================
      // Date the series was received
      // =============================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Date Received";
      olvColumn.Width = 100;
      olvColumn.AspectName = "DateReceived";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.AspectToStringFormat = "{0:d}";
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).DateReceived; };
      m_series.AllColumns.Add(olvColumn);

      // ==============================
      // Date the series was processed
      // =============================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Date Processed";
      olvColumn.Width = 100;
      olvColumn.AspectName = "DateProcessed";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.AspectToStringFormat = "{0:d}";
      olvColumn.Hideable = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).DateProcessed; };
      m_series.AllColumns.Add(olvColumn);

      // ============
      // Series Date
      // ============
      olvColumn = new OLVColumn();
      olvColumn.Text = "Date";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Date";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.AspectToStringFormat = "{0:d}";
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Date; };
      m_series.AllColumns.Add(olvColumn);

      // ===================
      // Series Parameters
      // ====================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Parameters";
      olvColumn.Width = 100;
      olvColumn.AspectName = "Process";
      olvColumn.CellPadding = null;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Hideable = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).Process; };
      m_series.AllColumns.Add(olvColumn);

      // ===================
      // Series study uid
      // ====================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Study Uid";
      olvColumn.Width = 100;
      olvColumn.AspectName = "StudyUid";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.IsVisible = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.Hideable = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Series)(obj)).StudyUid; };
      m_series.AllColumns.Add(olvColumn);

      // ==========
      // Filler 
      // ==========
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "";
      olvColumn.Text = "";
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.FillsFreeSpace = true;
      m_series.AllColumns.Add(olvColumn);
      m_series.Columns.Add(olvColumn);

    }


    #endregion

    #region properties

    /// <summary>
    /// Returns a reference to the TransferProviders.
    /// </summary>
    public System.Collections.ArrayList TransferProviders
    {
       get
      {
        ArrayList transferProviders = new ArrayList();

        //Do this way. Because the previous transfer may change the state of the TransferProvider
        foreach ( XmlNode transfernode in m_transferProvidersNode )
          transferProviders.Add( TransferProvider.FromXml( transfernode ) );

        return transferProviders;
      }
    }// TransferProviders


    /// <summary>
    /// Returns a ViewBrowserIndo object.
    /// </summary>
    public ViewBrowserInfo BrowserInfo
    {
      get { return m_viewBrowserInfo; }
    } // BrowserInfo


    /// <summary>
    /// Returns the selected study;
    /// </summary>
    public List<Study> SelectedStudies
    {
      get  { return m_studies.SelectedObjects.Cast<Study>().ToList();  }
    } // SelectedStudies


    /// <summary>
    /// Returns the selected series;
    /// </summary>
    public List<Series> SelectedSeries
    {
      get { return m_series.SelectedObjects.Cast<Series>().ToList(); ; }
    } // SelectedSeries

    /// <summary>
    /// Returns the currently selected series
    /// </summary>
    public Series CurrentSeries
    {
      get { return m_currentSeries; }
    }//CurrentSeries

    /// <summary>
    /// Returns a reference to the Provider.
    /// </summary>
    public IDataProvider DataProvider
    {
      get { return (IDataProvider)m_dataProvider; }
    } // BrowserProvider

  
    /// <summary>
    /// Returns a reference to the Provider.
    /// </summary>
    public IBrowserProvider BrowserProvider
    {
      get { return (IBrowserProvider)m_dataProvider; }
    } // BrowserProvider

    /// <summary>
    /// Gets or sets the LaunchManager for the view browser.
    /// </summary>
    public LaunchManager Launcher
    {
      get { return m_launchManager; }
      set { m_launchManager = value; }
    } // Launcher


    /// <summary>
    /// Gets or sets the LaunchManager for the view browser.
    /// </summary>
    public Viatronix.Enterprise.Launching.LaunchManager LaunchManager
    {
      get { return m_launchingManager; }
      set { m_launchingManager = value; }
    } // LauncherManager

    /// <summary>
    /// Gets or set the SearchManager for the view browser.
    /// </summary>
    public SearchManager Searcher
    {
      get { return m_searchManager; }
      set { m_searchManager = value; }
    } // Searcher

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
          components.Dispose();

        Logging.Log.Debug("Disposing of the ViewerBorwser", "ViewerBrowser", "Dispose");

        StopRefreshTimer();

        PauseAnimations(true);

        // Give some time for the pause
        System.Threading.Thread.Sleep(100);
     
        m_studies.ItemRenderer = null;
        m_series.ItemRenderer = null;

        m_studies.Dispose();
        m_series.Dispose();

        m_series = null;
        m_studies = null;
      

        if( m_searchManager != null )
        {
          m_searchManager.Cancel();
          m_searchManager.Dispose();
        }

      }

      base.Dispose( disposing );
    } // Dispose( disposing )


    /// <summary>
    /// Pauses the anmations for both the studies and the series
    /// </summary>
    /// <param name="pause"></param>
    private void PauseAnimations(bool pause)
    {

      // =========================================================================================
      // Only change the animation state if we are disabling/stopping the animation or if the 
      // indicator says the icons are animated and we a starting.
      // ========================================================================================
      if ((string.Compare(m_studyStateIndicator, "animated", true) == 0 && ! pause) || pause )
      {
        m_studies.PauseAnimations(pause);
        m_series.PauseAnimations(pause);
      } // END ... If pausing or ( starting and animation is indcated )

    } // PauseAnimations


    /// <summary>
    /// Resizes the panel contained on the control.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      
      if( Global.Frame.WindowState != FormWindowState.Minimized && m_topPanel.Height != 0 )
      {
        // maintain the ratio between the toppanel and middle panel
        int previousHeight = m_topPanel.Height + m_middlePanel.Height + m_splitter.Height;
        if( previousHeight != this.Height )
        {
          try
          {
            Win32.User32.SendMessage( this.Handle, (int) Win32.Msgs.WM_SETREDRAW, 0, 0 );

            // determine percentage for the old middle panel and detail panel
            double percentage = m_middlePanel.Height / ( previousHeight * 1.0 );
            double detailPercent = m_detailsPanel.Height / ( m_middlePanel.Height * 1.0 );

          //  m_middlePanel.Height = (int)( this.Height * percentage );
            m_detailsPanel.Height = (int)( m_middlePanel.Height * detailPercent );

            int seriesHeight = m_seriesPanel.Height - m_detailsPanel.Height;
            if( seriesHeight < m_seriesSplitter.MinExtra )
              m_detailsPanel.Height = m_seriesPanel.Height - m_seriesSplitter.MinExtra;
            else if( m_detailsPanel.Height < m_seriesSplitter.MinSize ) 
              m_detailsPanel.Height = m_seriesSplitter.MinSize;
          }
          finally
          {
            Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
            Refresh();
          }
        }
      }
      
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


 
 
    /// <summary>
    /// Saves the state of the list views
    /// </summary>
    private void SaveListViewState()
    {

      return;

      DateTime start = DateTime.Now;

      // =========================
      // Saves the studies state
      // =======================
      byte[] state = m_studies.SaveState();

      FileStream fs = new FileStream(m_studyUserPath, FileMode.Create);
      fs.Write(state, 0, state.Length);

      fs.Close();


      // =========================
      // Saves the series state
      // =========================
      state = m_series.SaveState();

      fs = new FileStream(m_seriesUserPath, FileMode.Create);
      fs.Write(state, 0, state.Length);

      fs.Close();


      Logging.Log.Timing(String.Format("Saving the list view states took {0} ms", DateTime.Now.Subtract(start).TotalMilliseconds), "ViewBrowser", "SaveListViewState");

    }

 
    /// <summary>
    /// Clears the study items
    /// </summary>
    private void SearchReset()
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_searchResetHandler);
      }
      else
      {

        m_studies.Items.Clear();
        m_series.Items.Clear();
        m_launchPane.Clear();

        m_detailsPanel.Visible = false;
        m_seriesSplitter.Enabled = false;

        this.Refresh();
      }
    } // SearchReset()


    /// <summary>
    /// Launches the importer from the menu item.
    /// </summary>
    /// <param name="drivePath">The drive to import from.</param>
    public void ImportCD(string drivePath)
    {
      if (!UI.MessageBox.ShowQuestion("Study information could not be displayed.\nCD contents may be from a previous version and must be imported to be viewed.\nDo you want to continue?"))
      {
        return;
      }

      try
      {
        string importArgs = Viatronix.Encryption.Crypto.Decrypt(ConfigurationManager.AppSettings["ImporterArgs"]) + " -s " + drivePath + " -cdImport";

        System.Diagnostics.Process process = new System.Diagnostics.Process();
        process.StartInfo.FileName = Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.ParseExpression(ConfigurationManager.AppSettings["Importer"]);
        //process.StartInfo.UseShellExecute = true;
        process.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Normal;
        process.StartInfo.Arguments = importArgs;
        process.Start();
      } // try
      catch (Exception e)
      {
        UI.MessageBox.ShowError("An error was encountered when launching the importer.  Please try again.");
        Logging.Log.Error("Importer could not be launched; Exception: " + e.Message, "ViewBrowser", "Import");
      } // catch(Exception e)
    } // Import( drivePath )


    /// <summary>
    /// Starts the refresh timer
    /// </summary>
    public void StartRefreshTimer()
    {

      Logging.Log.Debug("Starting the refresh timer", "ViewerBrowser", "StartRefresh");

      // =============================================================
      // Only start the timer if it is stopped and there are studies
      // =============================================================
      if (m_refreshTimerState == TimerState.Stopped && m_studies.Items.Count > 0)
      {
        m_refreshTimer.Start();
        m_refreshTimerState = TimerState.Started;
      }
      

    } // STartRefresh()

    /// <summary>
    /// Stop the refresh timer
    /// </summary>
    public void StopRefreshTimer()
    {

      Logging.Log.Debug("Stopping the refresh timer", "ViewerBrowser", "StopRefresh");

      // =====================================
      // Only stop the timer if it is started
      // =====================================
      if (m_refreshTimerState == TimerState.Started)
        m_refreshTimer.Stop();

      // ================================================================================================
      // Set the state to stopped always. This will flip a procesing state to stopped therby stopping 
      // the handler and restarting the timer
      // =================================================================================================
      m_refreshTimerState = TimerState.Stopped;
      
    } // StopRefresh()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the refresh timer tick event. At this point the timer is stopped and the 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnRefreshTimerTick(object sender, EventArgs args)
    {

      // Stop refreshing
      StopRefreshTimer();

      // We are now processing
      m_refreshTimerState = TimerState.Processing;

      // Get the object enumerator from the ObjectListView
      IEnumerator e = m_studies.Objects.GetEnumerator();

      PauseAnimations(true);

      States state = States.None;

      // =========================================================================
      // While there are studies and we are not stopping then refresh the studies
      // =========================================================================
      while (e.MoveNext() && m_refreshTimerState == TimerState.Processing )
      {

        Study study = (Study)e.Current;
        States currentStudyState = study.State;
        m_dataProvider.GetStudy(study);
        m_studies.RefreshObject(study);
        state |= study.State;
        

        // ===================================================================================================================================
        // If there are selected studies and the selected study is the same as the current and the state of the study has changed then update
        // the series view list for this study.
        // ====================================================================================================================================
        if (m_studies.SelectedObjects.Count > 0 && ((Study)m_studies.SelectedObjects[0]).Equals(e.Current) && study.State != currentStudyState)
          UpdateStudyView(false);

      } // END ... for each study in the list


      m_refreshTimerState = TimerState.Stopped;

      // Only start the animations thread if some studies are receiving or processing
      if ( (state & States.Receiving) == States.Receiving || ( state & States.Processing ) == States.Processing || ( state & States.Pending ) == States.Pending )
        PauseAnimations(false);

      StartRefreshTimer();

    } // OnTimerTickHandler( sender, args )

    private void OnSeriesShowGroupsChanged(object sender, EventArgs args )
    {
      UpdateStudyView(true);
    }

    /// <summary>
    /// Clears the contents of the listview controls.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSearchResetHandler( object sender, EventArgs args )
    {
      SearchReset();
    } // OnSearchPaneResetHandler( sender, args )

    
    /// <summary>
    /// Handles the study selection changed event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnStudySelectionChangedHandler( object sender, EventArgs args )
    {

      // ===========================================================================================================
      // Check for a NULL. THis is done due the fact that I have caught this event after the object was disposed
      // =========================================================================================================
      if (this.IsDisposed || m_studies == null)
        return;

      // if no studies are selected reset the detailsPanel and splitter
      if( m_studies.SelectedObjects.Count == 0 )
      {

        m_series.ClearObjects();
 
        m_detailsPanel.Visible = false;
        m_seriesSplitter.Enabled = false;

        m_launchPane.Clear();

        // raise the study selected event
        if (this.StudiesSelectionChanged != null)
          this.StudiesSelectionChanged( this.SelectedStudies );
      }
      else if( m_studies.SelectedObjects.Count > 0 )
      {
        UpdateStudyView(false);
      }
        
    } // OnStudySelectionChangedHandler( sender, args )

    /// <summary>
    ///  Retrieves a fresh listing of the series within the selected study.
    /// </summary>
    public void UpdateStudyView(bool raiseStudySelectionEvenet)
    {
 
      // Get the selected study

      m_series.BeginUpdate();

      m_series.ClearObjects();
      //m_series.Groups.Clear();
      //m_series.ShowGroups = true;
      

      try
      {
        if (m_studies.SelectedObjects.Count > 1)
        {
         // m_series.ShowGroups = true;

          // =======================================
          // Loop through all the selected studies
          // =======================================
          foreach (Study study in m_studies.SelectedObjects)
          {

            // Update the series series state
            m_dataProvider.GetSeries(study);

            // Add all the studies series
            m_series.AddObjects(study.Series);

            // Create a study group
            System.Windows.Forms.ListViewGroup group = new System.Windows.Forms.ListViewGroup(study.Uid, study.Name.FullName + "   -   " + study.Description);
            m_series.Groups.Add(group);

            //=====================================================================================
            // Loop thorugh all items and add all items associated with the study to the group
            // ====================================================================================
            for (int i = 0; i < m_series.Items.Count; i++)
            {

              OLVListItem item = m_series.GetItem(i);

              if (((Series)item.RowObject).StudyUid == study.Uid)
                group.Items.Add(item);

            } // END ... For each item


          }
          m_currentStudy = null;
        }
        else if (m_studies.SelectedObject != null)
        {

          Study study = (Study)m_studies.SelectedObject;

          study.Series = m_dataProvider.GetSeries(study);
          
          m_series.BuildGroups((OLVColumn)m_series.Columns["Type"], SortOrder.Ascending);
          m_series.AddObjects(study.Series);

          m_currentStudy = (Study)m_studies.SelectedObject;

        }


         
   
        m_currentSeries = null;

        // update the selected series
        UpdateSeriesView();


      }
      finally
      {
        m_series.EndUpdate();
      }

      m_series.BuildGroups(m_typeColumn, SortOrder.None);

    }

    /// <summary>
    /// Handles the series selection changed event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSeriesSelectionChangedHandler( object sender, EventArgs args )
    {

      UpdateSeriesView();

 
    } // OnSeriesSelectionChangedHandler( sender, args )

    /// <summary>
    /// 
    /// </summary>
    private void UpdateSeriesView()
    {

 
      if (m_series.SelectedItems.Count > 0)
      {




        if (m_series.SelectedObjects.Count == 1)
        {
          // display a new details control if one is available
          m_detailsPanel.Display((Series)m_series.SelectedObjects[0]);
          m_detailsPanel.Refresh();
          m_currentSeries = (Series)m_series.SelectedItems[0].Tag;
        }
        else
        {
          m_detailsPanel.Visible = false;
          m_seriesSplitter.Enabled = false;
          m_currentSeries = null;
        }

        // fire a selection changed event for the new series
        if (this.SeriesSelectionChanged != null)
          this.SeriesSelectionChanged(this.SelectedSeries);
      }
      else
        m_launchPane.Clear();


    }

    /// <summary>
    /// Handles the double click event for the series listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSeriesDoubleClickHandler( object sender, EventArgs args )
    {
      if( m_series.SelectedObjects.Count == 0 || !m_viewBrowserInfo.CanLaunch )
        return;

      try
      {
        //this.Launcher.Launch(this.SelectedSeries);
        this.LaunchManager.Launch(this.SelectedSeries);
      }
      catch (Exception e)
      {
        UI.MessageBox.ShowError("An error was encountered when launching.  Please contact service.");
        Logging.Log.Error("Viewer application could not be launched; Exception: " + e.Message, "ViewBrowser", "OnSeriesDoubleClickHandler");

      }

    } // OnSeriesDoubleClickHandler( sender, args )


    /// <summary>
    /// Handles the double click event for the study listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnStudyDoubleClickHandler( object sender, EventArgs args )
    {
      if (m_studies.SelectedObjects.Count == 0 || !m_viewBrowserInfo.CanLaunch)
        return;

      try
      {
        if (this.SelectedStudies.Count == 1 && (Control.ModifierKeys & Keys.Alt) != Keys.Alt)
        {
          this.LaunchManager.Launch(this.SelectedStudies[0]);
          //this.Launcher.Launch(this.SelectedStudies[0]);
        }
        else
        {
          this.Launcher.Launch(this.SelectedStudies);
        }
      }
      catch (Exception e)
      {
        UI.MessageBox.ShowError("An error was encountered when launching.  Please contact service.");
        Logging.Log.Error("Viewer application could not be launched; Exception: " + e.Message, "ViewBrowser", "OnStudyDoubleClickHandler");

      }

    } // OnStudyDoubleClickHandler( sender, args )

    /// <summary>
    /// Saves out the user preferences for the listview controls.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An CancelEventArgs that contains the event data.</param>
    private void OnClosingHandler( object sender, CancelEventArgs args )
    {
      OnViewChangingHandler( sender, EventArgs.Empty );
    } // OnClosingHandler( sender, args )


    /// <summary>
    /// Saves out the user preferences for the listview controls.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangingHandler( object sender, EventArgs args )
    {
      Global.Frame.ViewChanging -= m_viewChangingHandler;
      Global.Frame.Closing -= m_closingHandler;

      SaveListViewState();

    } // OnViewChangingHandler( sender, args )



    /// <summary>
    /// Handles the splitter moved event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An SplitterEventArgs that contains the event data.</param>
    private void OnSplitterMovedHandler( object sender, SplitterEventArgs args )
    {
      if( m_splitter.Tag != null && m_seriesPanel.Height != (int)m_splitter.Tag )
      {
        int spacing = m_middlePanel.Height - m_seriesPanel.Height;
        int height = (int)( m_detailsPanel.Height * (( m_seriesPanel.Height * 1.0 ) / (int)m_splitter.Tag ));    
        int seriesHeight = m_seriesPanel.Height - height;

        if( seriesHeight < m_seriesSplitter.MinExtra )
          m_detailsPanel.Height = m_middlePanel.Height - spacing - m_seriesSplitter.MinExtra;
        else if( height < m_seriesSplitter.MinSize )
          m_detailsPanel.Height = m_seriesSplitter.MinSize;
        else
          m_detailsPanel.Height = height;
      }
      m_splitter.Tag = null;

      if( m_studies.SelectedIndices.Count > 0 )
        m_studies.EnsureVisible( m_studies.SelectedIndices[0] );

      if( m_series.SelectedIndices.Count > 0 )
        m_series.EnsureVisible( m_series.SelectedIndices[0] );
    } // OnSplitterMovedHandler( sender, args )


    /// <summary>
    /// Handles the splitter moving event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An SplitterEventArgs that contains the event data.</param>
    private void OnSpitterMovingHandler( object sender, SplitterEventArgs args )
    {
      if( m_splitter.Tag == null )
        m_splitter.Tag = m_seriesPanel.Height;
    } // OnSpitterMovingHandler( sender, args )


    /// <summary>
    /// Assigns the SearchManager.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An RolloutEventArgs that contains the event data.</param>
    private void OnPaneExpandedHandler( object sender, RolloutEventArgs args )
    {
      // assign the SearchManager to the expanding pane if it implements the ISearchable interface
      if( args.Pane.Controls[0] is ISearchable )
      {
        ISearchable searchable = (ISearchable)args.Pane.Controls[0];
        searchable.Searcher = this.Searcher;
      }

      // assign the LaunchManager to the expanding pane if it implements the ILaunchable interface
      if( args.Pane.Controls[0] is ILaunchable )
      {
        ILaunchable launchable = (ILaunchable)args.Pane.Controls[0];
        launchable.Launcher = this.Launcher;
      }
    } // OnPaneExpandedHandler( sender, args )


    /// <summary>
    /// Handles the key down events for the study listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnStudyKeyDownHandler( object sender, KeyEventArgs args )
    {


      if (args.KeyData == Keys.Delete && this.SelectedStudies.Count > 0)
      {
        // Deleting a study from the pacs tab is not allowed.
        if (!(BrowserProvider is IPacsProvider))
        {
          m_studyContextMenu.Delete();
        }
      }
      else if (args.KeyData == (Keys.Control | Keys.T) && this.SelectedStudies.Count > 0)
        m_studyContextMenu.Transfer(true);
      else if (args.KeyData == Keys.Enter && this.SelectedStudies.Count > 0)
        m_launchManager.Launch(this.SelectedStudies);
     } // OnStudyKeyDownHandler( sender, args )

    /// <summary>
    /// Handles the key down events for the series listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnSeriesKeyDownHandler( object sender, KeyEventArgs args )
    {
      if (args.KeyData == Keys.Delete && this.SelectedSeries.Count > 0)
      {
        // Deleting a series from the pacs tab is not allowed.
        if (!(BrowserProvider is IPacsProvider))
        {
          m_seriesContextMenu.Delete(this.SelectedSeries);

          OnStudySelectionChangedHandler(sender, args);
        }
      } // if( args.KeyData == Keys.Delete && this.Series.Count > 0 )
      else if ( args.KeyData == (Keys.Control | Keys.T) && this.SelectedStudies.Count > 0)
        m_seriesContextMenu.Transfer(this.SelectedSeries);
      else if (args.KeyData == Keys.Enter && this.SelectedSeries.Count > 0)
        m_launchManager.Launch(this.SelectedSeries);
    } // OnSeriesKeyDownHandler( sender, args )


    /// <summary>
    /// Handles the visibility changed event for the details panel.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDetailsPanelVisiblityHandler( object sender, EventArgs args )
    {
      m_seriesSplitter.Enabled = m_detailsPanel.Visible;
    } // OnSplitterVisibilityHandler( sender, args )


    /// <summary>
    /// Handles the viewer selected event.
    /// </summary>
    /// <param name="launcher">IViewerLauncher</param>
    private void OnLauncherSelectedHandler( IViewerLauncher launcher )
    {
      this.Launcher.Launch( launcher );
    } // OnViewerSelectedHandler( controller )



    /// <summary>
    /// Handles the timer tick event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTimerTickHandler(object sender, EventArgs args)
    {
      // disconnect everything first, because the search time may exceed the timer interval.
      m_loadSearchTimer.Tick -= OnTimerTickHandler;
      m_loadSearchTimer.Interval = Int32.MaxValue;
      m_loadSearchTimer.Stop();
      m_loadSearchTimer.Dispose();

      DateTime start = DateTime.Now;

      if ( ! ( m_dataProvider is IPacsProvider) )
        this.Searcher.Search();

      TimeSpan span = DateTime.Now.Subtract(start);


    } // OnTimerTickHandler( sender, args )


    /// <summary>
    /// Handles the form load event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnFormLoadHandler(object sender, EventArgs e)
    {
        m_loadSearchTimer.Interval = 50;
        m_loadSearchTimer.Tick += new EventHandler(OnTimerTickHandler);
        m_loadSearchTimer.Start();
    } // OnFormLoadHandler(object sender, EventArgs e)

    #endregion

    #region IView Members

    /// <summary>
    /// Called when the view is initially created.
    /// </summary>
    /// <param name="xml">Additional xml configuration data</param>
    void IView.Initialize( string xml )
    {

      try
      {
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        m_viewBrowserInfo = ViewBrowserInfo.Create(doc.DocumentElement);


        string dataProvider = string.Empty;
        if ( ! doc.DocumentElement.TryGetAttributeValue<string>("dataProvider", ref dataProvider) )
        {
          Logging.Log.Error("No data provider specified in xml, fix configuration file", "ViewBroser", "Initialize");
          throw new System.MissingMemberException("No view provider specified in xml");
        } // END ... If there is no data provider specified

        if (m_dataProvider == null)
          m_dataProvider = ProviderFactory.Create<IBrowserProvider>(dataProvider);
     

        // set the dataprovider for the launch

        if ( m_dataProvider is IPacsProvider )
        {
          m_launchManager = null;
        }
        else
        {
          m_launchManager = new LaunchManager(Global.Frame, (IDataProvider)m_dataProvider, Global.CurrentUser);
          m_launchingManager = new Viatronix.Enterprise.Launching.LaunchManager((IDataProvider)m_dataProvider, Global.CurrentUser);

          //m_launchManager.Provider = m_dataProvider;
          m_launchManager.Enabled = this.BrowserInfo.CanLaunch;
          m_launchingManager.Enabled = this.BrowserInfo.CanLaunch;
        }

        // create the search manager and reload the query builder
        m_searchManager = new SearchManager(Global.Frame,  (IAsynchronousBrowserProvider)m_dataProvider);
        m_searchManager.ResetSearch += new EventHandler(this.OnSearchResetHandler);
        //m_searchManager.StudyFound += new StudyDelegate( this.OnStudyFoundHandler );
        //m_searchManager.SearchCompleted += new StudyCollectionDelegate( this.OnSearchCompletedHandler );
        m_searchManager.XmlBuilder = m_query;

        m_detailsPanel.Launcher = m_launchManager;

        //Keep the transder Provider Node for transfer
        m_transferProvidersNode = doc.DocumentElement.SelectNodes("transferProviders/transferProvider");



        // load the columns for the listview controls
        XmlNode node = (XmlNode)ConfigurationManager.GetSection("dynamicColumns");

        XmlNode listviewNode = doc.DocumentElement.SelectSingleNode("listviews");

        // load the user preferences for the listviews
        //string xpath = "columns[ @id='{0}' ]";
        //XmlNode studyColumns = node.SelectSingleNode(string.Format(xpath, new object[] { listviewNode.Attributes["studies"].Value }));
        //m_studies.Load(studyColumns, m_studyUserPath);
       

        //XmlNode seriesColumns = node.SelectSingleNode(string.Format(xpath, new object[] { listviewNode.Attributes["series"].Value }));
        //m_series.Load(seriesColumns, m_seriesUserPath);

        Global.Frame.ViewChanging += m_viewChangingHandler;
        Global.Frame.Closing += m_closingHandler;

        // create the custom context menus for the study and series listviews
        m_studyContextMenu = new StudyContextMenu(this, m_studies, m_series);
        m_studies.ContextMenu = m_studyContextMenu;

        m_seriesContextMenu = new SeriesContextMenu(this, m_studies, m_series);
        m_series.ContextMenu = m_seriesContextMenu;

        // display launch window if launching is allowed
        m_launchPanel.Visible = (m_viewBrowserInfo.CanLaunch && m_viewBrowserInfo.DisplayLaunchPane);
        m_verticalSplitter.Visible = m_viewBrowserInfo.CanLaunch;
      }
      catch (Exception ex)
      {
        Logging.Log.Error("Failed to initialize the view : " + ex.Message, "ViewBrowser", "IView.Initialize");
        throw;
      }
     
    } // Initialize( xml )

    #endregion

    #region IPersistable Members

    /// <summary>
    /// Allows the view to load any persisted state.
    /// </summary>
    /// <param name="viewState">The view's persistable state.</param>
    void IPersistable.LoadViewState( Dictionary< string, object > viewState )
    {
      DateTime start = DateTime.Now;

      object state = null;
      if( viewState.TryGetValue( CoreState.DataProvider, out state ))
        m_dataProvider = (IBrowserProvider) state;

      if( viewState.TryGetValue( "SelectedStudies", out state ))
        m_studies.SelectedObjects  = (List<Study>) state;

      if( viewState.TryGetValue( "SelectedSeries", out state ))
        m_series.SelectedObjects = (List<Series>) state;

      if( viewState.TryGetValue( "Query", out state ))
        m_query = (XmlBuilder)state;
     
      if( viewState.TryGetValue( "DetailsPanelSize", out state ))
        m_detailsPanel.Size = (Size) state;

      if( viewState.TryGetValue( "LaunchPanelSize", out state ))
        m_launchPanel.Size = (Size) state;

      if( viewState.TryGetValue( "LowerPanelSize", out state ))
        m_lowerPanel.Size = (Size) state;

      Logging.Log.Timing(String.Format("Loading the viewers state took {0} ms", DateTime.Now.Subtract(start).TotalMilliseconds), "ViewBrowser", "SaveViewState");

    } // LoadViewState( viewState )


    /// <summary>
    /// Allows the view to persist its state.
    /// </summary>
    /// <param name="viewState">The view's persisted state.</param>
    void IPersistable.SaveViewState( Dictionary< string, object > viewState )
    {

      DateTime start = DateTime.Now;
     
      // store view state
      viewState[ "DataProvider"     ] = m_dataProvider;
      viewState[ "SelectedStudies"  ] = SelectedStudies;
      viewState[ "SelectedSeries"   ] = SelectedSeries;
      viewState[ "XmlBuilder"       ] = m_searchManager.XmlBuilder;
      viewState[ "DetailsPanelSize" ] = m_detailsPanel.Size;
      viewState[ "LaunchPanelSize"  ] = m_launchPanel.Size;
      viewState[ "LowerPanelSize"   ] = m_lowerPanel.Size;

      Logging.Log.Timing(String.Format("Saving the viewers state took {0} ms", DateTime.Now.Subtract(start).TotalMilliseconds), "ViewBrowser", "SaveViewState");

    } // SaveViewState( viewState )

    #endregion

    #region ISearchView Members

    void AddItem(Study study)
    {
      m_studies.AddObject(study);
    }

    /// <summary>
    /// Adds a single study result.
    /// </summary>
    /// <param name="study">A found study.</param>
    void ISearchView.AddSingleSearchResult( Study study )
    {
      AddItem(study);

      PauseAnimations(false);

    } // AddSingleSearchResult( study )


    /// <summary>
    /// Adds all found studies.
    /// </summary>
    /// <param name="studies">Collection of studies.</param>
    void ISearchView.AddAllSearchResults( List<Study> studies )
    {

      StopRefreshTimer();

      DateTime start = DateTime.Now;
      Logging.Log.Information("AddAllSearchResults started", "ViewBrowser", "AddAllSearchResults");

      PauseAnimations(true);

      // ======================================================================================================
      // We need to clear the series objects because if no studies are returned the code that does this is
      // never executed and the series panes has the old data.
      // ======================================================================================================
      m_series.ClearObjects();

      // update the listview with all the studies, if a batch update is allowed
      m_studies.BeginUpdate();

      Study selectedStudy = (m_studies.SelectedObjects.Count > 0 ? (Study)m_studies.SelectedObjects[0] : (studies.Count > 0 ? studies[0] : null));

      m_studyList = studies;
      
      // Get the selected object from the new list
      if ( selectedStudy != null )
        studies.TryFind(selectedStudy.Uid, ref selectedStudy);

      m_studies.ClearObjects();
      m_studies.AddObjects(studies);
      m_studies.EndUpdate();

      // select a study and series or clear out the collections
      if (selectedStudy == null)
      {
        m_detailsPanel.Visible = false;
        m_seriesSplitter.Enabled = false;
      }

      if (m_studies.SelectedItems.Count > 0)
        m_studies.SelectedItems[0].EnsureVisible();

      PauseAnimations(false);

      if ( m_studies.Items.Count > 0 )
        StartRefreshTimer();

      m_studies.Focus();

      m_studies.SelectedObject = selectedStudy;

      Logging.Log.Timing(string.Format("AddAllSearchResults finished {0}", DateTime.Now.Subtract(start).ToString()), "ViewBrowser", "AddAllSearchResults");

 
    } // AddAllSearchResults( studies )

 
    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
this.components = new System.ComponentModel.Container();
this.m_splitter = new System.Windows.Forms.Splitter();
this.m_topPanel = new Viatronix.UI.Panel();
this.m_studies = new ObjectListView();
this.m_studiesLabel = new System.Windows.Forms.Label();
this.m_middlePanel = new Viatronix.UI.Panel();
this.m_seriesPanel = new System.Windows.Forms.Panel();
this.m_series = new ObjectListView();
this.m_seriesSplitter = new System.Windows.Forms.Splitter();
this.m_detailsPanel = new Viatronix.Console.SeriesDetailsPanel();
this.m_seriesLabel = new System.Windows.Forms.Label();
this.m_launchPanel = new Viatronix.UI.Panel();
this.m_launchPane = new Viatronix.Console.ViewerSelector();
this.m_launchLabel = new System.Windows.Forms.Label();
this.m_lowerPanel = new System.Windows.Forms.Panel();
this.m_lowerSeriesPanel = new System.Windows.Forms.Panel();
this.m_verticalSplitter = new System.Windows.Forms.Splitter();
this.m_topPanel.SuspendLayout();
this.m_middlePanel.SuspendLayout();
this.m_seriesPanel.SuspendLayout();
this.m_launchPanel.SuspendLayout();
this.m_lowerPanel.SuspendLayout();
this.m_lowerSeriesPanel.SuspendLayout();
this.SuspendLayout();
// 
// m_splitter
// 
this.m_splitter.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_splitter.Dock = System.Windows.Forms.DockStyle.Bottom;
this.m_splitter.Location = new System.Drawing.Point(0, 357);
this.m_splitter.MinExtra = 290;
this.m_splitter.MinSize = 256;
this.m_splitter.Name = "m_splitter";
this.m_splitter.Size = new System.Drawing.Size(1016, 8);
this.m_splitter.TabIndex = 5;
this.m_splitter.TabStop = false;
this.m_splitter.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.OnSpitterMovingHandler);
this.m_splitter.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.OnSplitterMovedHandler);
// 
// m_topPanel
// 
this.m_topPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_topPanel.BorderColor = System.Drawing.Color.White;
this.m_topPanel.BottomPadding = 0;
this.m_topPanel.Controls.Add(this.m_studies);
this.m_topPanel.Controls.Add(this.m_studiesLabel);
this.m_topPanel.Curve = 10;
this.m_topPanel.Dock = System.Windows.Forms.DockStyle.Fill;
this.m_topPanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_topPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_topPanel.LeftPadding = 0;
this.m_topPanel.Location = new System.Drawing.Point(0, 0);
this.m_topPanel.Name = "m_topPanel";
this.m_topPanel.RightPadding = 0;
this.m_topPanel.Size = new System.Drawing.Size(1016, 357);
this.m_topPanel.TabIndex = 0;
this.m_topPanel.TopPadding = 0;
// 
// m_studies
// 
this.m_studies.AllowColumnReorder = true;
this.m_studies.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_studies.ShowGroups = false;
this.m_studies.FullRowSelect = true;
this.m_studies.GridLines = true;
this.m_studies.HideSelection = false;
this.m_studies.Location = new System.Drawing.Point(12, 32);
this.m_studies.Name = "m_studies";
this.m_studies.Size = new System.Drawing.Size(992, 313);
this.m_studies.TabIndex = 0;
this.m_studies.UseCompatibleStateImageBehavior = false;
this.m_studies.View = System.Windows.Forms.View.Details;
this.m_studies.SelectionChanged += new System.EventHandler(this.OnStudySelectionChangedHandler);
this.m_studies.DoubleClick += new System.EventHandler(this.OnStudyDoubleClickHandler);
this.m_studies.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnStudyKeyDownHandler);

// 
// m_studiesLabel
// 
this.m_studiesLabel.BackColor = System.Drawing.Color.Transparent;
this.m_studiesLabel.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Bold);
this.m_studiesLabel.ForeColor = System.Drawing.Color.White;
this.m_studiesLabel.Location = new System.Drawing.Point(12, 8);
this.m_studiesLabel.Name = "m_studiesLabel";
this.m_studiesLabel.Size = new System.Drawing.Size(320, 23);
this.m_studiesLabel.TabIndex = 1;
this.m_studiesLabel.Text = "Studies";
this.m_studiesLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
// 
// m_middlePanel
// 
this.m_middlePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_middlePanel.BorderColor = System.Drawing.Color.White;
this.m_middlePanel.BottomPadding = 5;
this.m_middlePanel.Controls.Add(this.m_seriesPanel);
this.m_middlePanel.Controls.Add(this.m_seriesLabel);
this.m_middlePanel.Curve = 10;
this.m_middlePanel.Dock = System.Windows.Forms.DockStyle.Fill;
this.m_middlePanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_middlePanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_middlePanel.LeftPadding = 0;
this.m_middlePanel.Location = new System.Drawing.Point(0, 0);
this.m_middlePanel.Name = "m_middlePanel";
this.m_middlePanel.RightPadding = 0;
this.m_middlePanel.Size = new System.Drawing.Size(547, 411);
this.m_middlePanel.TabIndex = 4;
this.m_middlePanel.TopPadding = 0;
// 
// m_seriesPanel
// 
this.m_seriesPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_seriesPanel.Controls.Add(this.m_series);
this.m_seriesPanel.Controls.Add(this.m_seriesSplitter);
this.m_seriesPanel.Controls.Add(this.m_detailsPanel);
this.m_seriesPanel.Location = new System.Drawing.Point(12, 32);
this.m_seriesPanel.Name = "m_seriesPanel";
this.m_seriesPanel.Size = new System.Drawing.Size(523, 363);
this.m_seriesPanel.TabIndex = 0;
// 
// m_series
// 
this.m_series.AllowColumnReorder = true;
this.m_series.Dock = System.Windows.Forms.DockStyle.Fill;
this.m_series.ShowGroups = false;
this.m_series.FullRowSelect = true;
this.m_series.GridLines = true;
this.m_series.HideSelection = false;
this.m_series.Location = new System.Drawing.Point(0, 0);
this.m_series.Name = "m_series";
this.m_series.Size = new System.Drawing.Size(523, 264);
this.m_series.TabIndex = 1;
this.m_series.UseCompatibleStateImageBehavior = false;
this.m_series.View = System.Windows.Forms.View.Details;
this.m_series.DoubleClick += new System.EventHandler(this.OnSeriesDoubleClickHandler);
this.m_series.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnSeriesKeyDownHandler);
this.m_series.SelectionChanged += new EventHandler(this.OnSeriesSelectionChangedHandler);
// 
// m_seriesSplitter
// 
this.m_seriesSplitter.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_seriesSplitter.Dock = System.Windows.Forms.DockStyle.Bottom;
this.m_seriesSplitter.Enabled = false;
this.m_seriesSplitter.Location = new System.Drawing.Point(0, 264);
this.m_seriesSplitter.MinExtra = 104;
this.m_seriesSplitter.MinSize = 104;
this.m_seriesSplitter.Name = "m_seriesSplitter";
this.m_seriesSplitter.Size = new System.Drawing.Size(523, 3);
this.m_seriesSplitter.TabIndex = 2;
this.m_seriesSplitter.TabStop = false;
// 
// m_detailsPanel
// 
this.m_detailsPanel.BackColor = System.Drawing.Color.Transparent;
this.m_detailsPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
this.m_detailsPanel.Launcher = null;
this.m_detailsPanel.Location = new System.Drawing.Point(0, 267);
this.m_detailsPanel.Name = "m_detailsPanel";
this.m_detailsPanel.Size = new System.Drawing.Size(523, 96);
this.m_detailsPanel.TabIndex = 1;
this.m_detailsPanel.Visible = false;
this.m_detailsPanel.VisibleChanged += new System.EventHandler(this.OnDetailsPanelVisiblityHandler);
// 
// m_seriesLabel
// 
this.m_seriesLabel.BackColor = System.Drawing.Color.Transparent;
this.m_seriesLabel.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Bold);
this.m_seriesLabel.ForeColor = System.Drawing.Color.White;
this.m_seriesLabel.Location = new System.Drawing.Point(12, 8);
this.m_seriesLabel.Name = "m_seriesLabel";
this.m_seriesLabel.Size = new System.Drawing.Size(101, 23);
this.m_seriesLabel.TabIndex = 2;
this.m_seriesLabel.Text = "Series";
this.m_seriesLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
// 
// m_launchPanel
// 
this.m_launchPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_launchPanel.BorderColor = System.Drawing.Color.White;
this.m_launchPanel.BottomPadding = 5;
this.m_launchPanel.Controls.Add(this.m_launchPane);
this.m_launchPanel.Controls.Add(this.m_launchLabel);
this.m_launchPanel.Curve = 10;
this.m_launchPanel.Dock = System.Windows.Forms.DockStyle.Right;
this.m_launchPanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_launchPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_launchPanel.LeftPadding = 0;
this.m_launchPanel.Location = new System.Drawing.Point(557, 0);
this.m_launchPanel.Name = "m_launchPanel";
this.m_launchPanel.RightPadding = 0;
this.m_launchPanel.Size = new System.Drawing.Size(459, 411);
this.m_launchPanel.TabIndex = 6;
this.m_launchPanel.TopPadding = 0;
// 
// m_launchPane
// 
this.m_launchPane.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_launchPane.Location = new System.Drawing.Point(12, 32);
this.m_launchPane.Name = "m_launchPane";
this.m_launchPane.Size = new System.Drawing.Size(435, 363);
this.m_launchPane.TabIndex = 3;
// 
// m_launchLabel
// 
this.m_launchLabel.BackColor = System.Drawing.Color.Transparent;
this.m_launchLabel.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Bold);
this.m_launchLabel.ForeColor = System.Drawing.Color.White;
this.m_launchLabel.Location = new System.Drawing.Point(12, 8);
this.m_launchLabel.Name = "m_launchLabel";
this.m_launchLabel.Size = new System.Drawing.Size(154, 23);
this.m_launchLabel.TabIndex = 2;
this.m_launchLabel.Text = "Launch Options";
this.m_launchLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
// 
// m_lowerPanel
// 
this.m_lowerPanel.Controls.Add(this.m_lowerSeriesPanel);
this.m_lowerPanel.Controls.Add(this.m_verticalSplitter);
this.m_lowerPanel.Controls.Add(this.m_launchPanel);
this.m_lowerPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
this.m_lowerPanel.Location = new System.Drawing.Point(0, 365);
this.m_lowerPanel.Name = "m_lowerPanel";
this.m_lowerPanel.Size = new System.Drawing.Size(1016, 411);
this.m_lowerPanel.TabIndex = 6;
// 
// m_lowerSeriesPanel
// 
this.m_lowerSeriesPanel.Controls.Add(this.m_middlePanel);
this.m_lowerSeriesPanel.Dock = System.Windows.Forms.DockStyle.Fill;
this.m_lowerSeriesPanel.Location = new System.Drawing.Point(0, 0);
this.m_lowerSeriesPanel.Name = "m_lowerSeriesPanel";
this.m_lowerSeriesPanel.Size = new System.Drawing.Size(547, 411);
this.m_lowerSeriesPanel.TabIndex = 2;
// 
// m_verticalSplitter
// 
this.m_verticalSplitter.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_verticalSplitter.Dock = System.Windows.Forms.DockStyle.Right;
this.m_verticalSplitter.Location = new System.Drawing.Point(547, 0);
this.m_verticalSplitter.MinExtra = 256;
this.m_verticalSplitter.MinSize = 256;
this.m_verticalSplitter.Name = "m_verticalSplitter";
this.m_verticalSplitter.Size = new System.Drawing.Size(10, 411);
this.m_verticalSplitter.TabIndex = 5;
this.m_verticalSplitter.TabStop = false;
// 
// ViewBrowser
// 
this.Controls.Add(this.m_topPanel);
this.Controls.Add(this.m_splitter);
this.Controls.Add(this.m_lowerPanel);
this.Name = "ViewBrowser";
this.Size = new System.Drawing.Size(1016, 776);
this.Load += new System.EventHandler(this.OnFormLoadHandler);
this.m_topPanel.ResumeLayout(false);
this.m_middlePanel.ResumeLayout(false);
this.m_seriesPanel.ResumeLayout(false);
this.m_launchPanel.ResumeLayout(false);
this.m_lowerPanel.ResumeLayout(false);
this.m_lowerSeriesPanel.ResumeLayout(false);
this.ResumeLayout(false);

    }
    #endregion

  } // class ViewBrowser

  #region ViewBrowserInfo class

  public class ViewBrowserInfo
  {
    
    #region fields

    /// <summary>
    /// determines if the view can launch a series.
    /// </summary>
    private bool m_canLaunch = false;

    /// <summary>
    /// determines if the launch pane is displayed or not (Only if canLaunch = true is this used)
    /// </summary>
    private bool m_displayLaunchPane = false;

    /// <summary>
    /// determines if the view can transfer
    /// </summary>
    private bool m_canTransfer = false;

    /// <summary>
    /// determines if animations are supported by this view.
    /// </summary>
    private bool m_useAnimation = false;

    /// <summary>
    /// determines if the view can perform the delete operation
    /// </summary>
    private bool m_canDelete = false;

    /// <summary>
    /// determines if the view can import studies.
    /// </summary>
    private bool m_canImport = false;

    /// <summary>
    /// determines if the view can show DICOM directory.
    /// </summary>
    private bool m_canShowDicomDirectory = false;

    /// <summary>
    /// determines if the view can show DICOM Infomaion.
    /// </summary>
    private bool m_canShowDicomInfo= false;

    /// <summary>
    /// determines if the view can process the selected series
    /// </summary>
    private bool m_canProcess = false;

    #endregion

    #region properties

    /// <summary>
    /// Determines if the view can perform the launch operation.
    /// </summary>
    public bool CanLaunch
    {
      get { return m_canLaunch; }
    } // CanLaunch

    /// <summary>
    /// Determines if the view will display the launch pane if the can launch is true.
    /// </summary>
    public bool DisplayLaunchPane
    {
      get { return m_displayLaunchPane; }
    } // DisplayLaunchPane


    /// <summary>
    /// Determines if the view can perform the transfer operation.
    /// </summary>
    public bool CanTransfer
    {
      get { return m_canTransfer; }
    } // CanTransfer


    /// <summary>
    /// Determines if the view can perform the delete operation.
    /// </summary>
    public bool CanDelete
    {
      get { return m_canDelete; }
    } // CanDelete


    /// <summary>
    /// Determines if the view can perform the import operation.
    /// </summary>
    public bool CanImport
    {
      get { return m_canImport; }
    } // CanImport


 

    /// <summary>
    /// Determines if the view can show DICOM Folders.
    /// </summary>
    public bool CanShowDicomFolder
    {
      get { return m_canShowDicomDirectory; }
    } // CanShowDicomFolder


    /// <summary>
    /// Determines if the view can show DICOM Folders.
    /// </summary>
    public bool CanShowDicomInformation
    {
      get { return m_canShowDicomInfo; }
    } // CanShowDicomFolder


    /// <summary>
    /// Determines if the view can process a series
    /// </summary>
    public bool CanProcess
    {
      get { return m_canProcess; }
    } // CanProcess
    

    #endregion

    #region methods

    /// <summary>
    /// creates a view browser info object from xml.
    /// </summary>
    /// <param name="node">Xml information</param>
    /// <returns>A ViewBrowserInfo object.</returns>
    public static ViewBrowserInfo Create( XmlNode node )
    {
      ViewBrowserInfo info = new ViewBrowserInfo();

      info.m_canLaunch = bool.Parse( node.Attributes[ "canLaunch" ].Value );
      info.m_displayLaunchPane = (node.Attributes["displayLaunchPane"] != null ? bool.Parse(node.Attributes["displayLaunchPane"].Value) : false);
      info.m_canTransfer = bool.Parse( node.Attributes[ "canTransfer" ].Value );
      info.m_useAnimation = bool.Parse( node.Attributes[ "useAnimation" ].Value );
      info.m_canDelete = bool.Parse( node.Attributes[ "canDelete" ].Value );
      info.m_canImport = bool.Parse( node.Attributes[ "canImport" ].Value );
      info.m_canShowDicomDirectory = bool.Parse( node.Attributes[ "canShowDicomFolder" ].Value );
      info.m_canShowDicomInfo = bool.Parse( node.Attributes[ "canShowDicomInfo" ].Value );
      info.m_canProcess = bool.Parse( node.Attributes[ "canProcess" ].Value );

      return info;
    } // Create( node )


    #endregion



  } // class ViewBrowserInfo

  #endregion

} // namespace Viatronix.Console

#region revision history

// $Log: ViewBrowser.cs,v $
// Revision 1.90.2.20  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.90.2.19  2009/11/03 14:07:47  mkontak
// Fix a problem where I was checking the provider name against "Database" and
// it was actually DataBase so the series was not being updated.
//
// Revision 1.90.2.18  2009/10/30 14:04:47  kchalupa
// Updated dialog for CD transfer for 2.0 style CDs.
//
// Revision 1.90.2.17  2009/10/26 19:49:25  kchalupa
// Added ImportCD method and delay search shortly after the load to give the form time to load.
//
// Revision 1.90.2.16  2009/10/16 17:24:16  mkontak
// Only attempt to update the series if we are connected to the database.
// This was added for Cardiac.
//
// Revision 1.90.2.15  2009/09/03 19:15:03  mkontak
// Fix issue with UpdatePhaseInformation
//
// Revision 1.90.2.14  2009/08/25 12:44:07  mkontak
// Changed UpdatePhaseInformation method
//
// Revision 1.90.2.13  2008/10/13 15:25:02  kchalupa
// Capability to launch importer from console was enhanced.
//
// Revision 1.90.2.12  2008/09/25 14:05:47  mkontak
// Fix CD tab issue
//
// Revision 1.90.2.11  2008/09/15 19:49:42  mkontak
// Fix for delete series showing up again
//
// Revision 1.90.2.10  2008/09/15 19:24:21  mkontak
// Fix for delete series showing up again
//
// Revision 1.90.2.9  2008/09/15 19:22:45  mkontak
// Fix for delete series showing up again
//
// Revision 1.90.2.7  2008/09/08 18:24:23  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
//
// Revision 1.90.2.7  2008/08/04 12:33:52  kchalupa
// Issue #6138 Fix
// Cleared the series list in the OnStudySelectionChangedHandler event handler.
// Gets a fresh Series list from the database on each study selection.
//
// Revision 1.90.2.6  2008/05/01 14:27:47  mkontak
// Fix issue 6078 were the PACS query fr the series pane showed no data. This
// was due to the simple mode rules and we need to ignore simple mode when
// querying the PACS.
//
// Revision 1.90.2.5  2008/02/13 17:03:32  mkontak
// Added Series Delete and Transfer Buttons but made them optional through a app.config
// setting. Also allow the transfer dialog to launch by Alt-T key combination.
//
// Revision 1.90.2.4  2008/01/10 18:30:46  mkontak
// Simple Mode Added
//
// Revision 1.90.2.3  2008/01/02 20:33:31  mkontak
// Password hints
//
// Revision 1.90.2.2  2007/10/08 11:27:46  mkontak
// Relabels the series upon a study change event
//
// Revision 1.90.2.1  2007/09/07 15:13:33  mkontak
// Added displayLaunchPane to viewer pane config
//
// Revision 1.90  2007/03/08 18:07:52  mkontak
// Added exception handling to try and find the arithmetic overflow
//
// Revision 1.89  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.88  2007/03/02 19:32:34  romy
// Keeps the transfer Provider Node for creating transferProviders each time Transfer requests for TransferProviders.
//
// Revision 1.87  2007/03/01 21:17:48  mkontak
// Fix the study menu when clicking emty space
//
// Revision 1.86  2007/02/28 17:36:11  gdavidson
// Modified the selection of studies and series
//
// Revision 1.85  2007/02/28 15:44:17  mkontak
// Issue 5431: Fix the series list not being cleared when the last study is deleted.
//
// Revision 1.84  2007/02/26 17:58:23  gdavidson
// Issue #5426: Modified the tab order
//
// Revision 1.83  2007/02/23 21:53:11  gdavidson
// Commented code
//
// Revision 1.82  2007/02/23 21:48:59  gdavidson
// Adjusted splitter minimum size
//
// Revision 1.81  2007/02/23 17:12:03  gdavidson
// Added launch panel
//
// Revision 1.80  2007/02/21 15:41:42  gdavidson
// Added additional logging for tracking down crashes
//
// Revision 1.79  2007/01/18 18:42:42  romy
// setting the preview image when study selection changes
//
// Revision 1.78  2006/11/16 20:07:43  mkontak
// Fix jiggling in session details display
//
// Revision 1.76  2006/11/15 23:17:37  gdavidson
// Modified the study and series selection
//
// Revision 1.75  2006/10/30 16:20:58  gdavidson
// Impliments Viatronix.UI.IPersistable
//
// Revision 1.74  2006/09/21 20:55:27  mkontak
// Issue 4995
//
// Revision 1.73  2006/09/13 15:53:05  gdavidson
// Issue #4985: Refresh series when the study changes or the associated series are missing
//
// Revision 1.72  2006/09/06 22:27:01  mkontak
// Issue 4928
//
// Revision 1.71  2006/07/18 14:52:27  mkontak
// Fix rights and transferring into the system
//
// Revision 1.70  2006/07/10 18:38:20  gdavidson
// Issue #4836: Modified SearchManager
//
// Revision 1.69  2006/06/12 19:42:43  mkontak
// Fixed invalid operation exception due to unsafe operations on UI controls
// from different threads.
//
// Revision 1.68  2006/06/06 19:48:27  mkontak
// changed deprecated code
//
// Revision 1.67  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.66  2006/05/08 13:12:48  gdavidson
// Issue #4777: Reset the session viewers on searches
//
// Revision 1.65  2006/05/05 20:41:48  gdavidson
// Issue #4767: Added functionality to deactivate Process As conext menu item
//
// Revision 1.64  2006/04/24 15:48:01  gdavidson
// Implemented immediate launch on a double clicked study
//
// Revision 1.63  2006/03/02 18:34:48  romy
// fixed session reading issues from CDs
//
// Revision 1.62  2006/02/23 21:59:01  gdavidson
// Set the Enabled property for the LaunchManager
//
// Revision 1.61  2006/02/14 16:52:20  gdavidson
// Optimized the the drawing of series listview items when multiple studies are selected.
//
// Revision 1.60  2006/02/10 21:30:04  gdavidson
// Clear the session details pane when search returns nothing.
//
// Revision 1.59  2006/02/09 21:46:39  romy
// Bug fixes
//
// Revision 1.58  2006/02/07 15:38:47  romy
// Caching of control sizes adjusted on Resize
//
// Revision 1.57  2006/02/07 15:06:35  romy
// keeps the size in cache
//
// Revision 1.56  2006/01/31 21:19:46  romy
// added attributes to view browser
//
// Revision 1.55  2006/01/18 16:12:20  gdavidson
// Modified the series splitters back color.
//
// Revision 1.54  2006/01/16 14:29:46  gdavidson
// Modified how the selected studies and series are set.
//
// Revision 1.53  2006/01/09 19:50:10  gdavidson
// Added multiple selection of Study objects.
//
// Revision 1.52  2006/01/03 16:10:37  gdavidson
// During the study changed event the DataProvider retrieves the associated series information.
//
// Revision 1.51  2005/12/14 19:59:59  gdavidson
// Made the selected series multiple objects.
//
// Revision 1.50  2005/12/12 20:59:59  romy
// added application State caching mechanism
//
// Revision 1.49  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.48  2005/11/23 18:33:55  geconomos
// launching from console revisted
//
// Revision 1.47  2005/11/16 14:00:34  gdavidson
// Allowed multiselect for series.
//
// Revision 1.46  2005/11/08 21:39:18  gdavidson
// Added KeyDown events for deletion.
//
// Revision 1.45  2005/11/02 14:33:34  gdavidson
// Added dsiposal of the IDataProvider and PaneExpanded event handler.
//
// Revision 1.44  2005/10/26 19:50:13  gdavidson
// Moved the context menus to seperate class.
//
// Revision 1.43  2005/10/25 18:57:22  romy
// added transfer object
//
// Revision 1.42  2005/10/25 15:01:53  romy
// Added TransferItem Collection
//
// Revision 1.41  2005/10/21 17:47:39  mkontak
// Changed implementation of DicomHosts and how the server is defined
//
// Revision 1.40  2005/10/20 15:31:38  romy
// initializing Transfer Items with an empty array.
//
// Revision 1.39  2005/10/18 17:53:58  gdavidson
// Removed commented code.
//
// Revision 1.38  2005/10/18 17:46:41  gdavidson
// Implemented the ILaunchable and ISearchable interfaces.
//
// Revision 1.37  2005/10/14 10:45:43  mkontak
// Fixed path build for dicom file
//
// Revision 1.36  2005/10/12 13:25:03  mkontak
// Added null check for attributes
//
// Revision 1.35  2005/10/11 16:57:09  mkontak
// Added Popup menu event handler for the dicom information
//
// Revision 1.34  2005/10/11 14:10:22  romy
// aded transfer In code on Drag Drop
//
// Revision 1.33  2005/10/11 14:02:12  gdavidson
// Modified the IDataProvider interface.
//
// Revision 1.32  2005/10/06 15:46:46  gdavidson
// Modified the launching of a viewer and added the Theme's font to the view.
//
// Revision 1.31  2005/10/05 13:42:02  mkontak
// Removed test code
//
// Revision 1.30  2005/10/05 12:42:43  mkontak
// Added Show Dicom Information to series menu
//
// Revision 1.29  2005/10/03 15:08:58  romy
// modified Transfer Providers
//
// Revision 1.28  2005/09/30 17:13:09  romy
// Transfer cancel method added
//
// Revision 1.27  2005/09/26 15:58:28  romy
// new Transfer Mechansim
//
// Revision 1.26  2005/09/26 14:09:07  gdavidson
// Added viewer launching for study and series double click.
//
// Revision 1.25  2005/09/23 18:23:51  romy
// added CDTransfer Providers
//
// Revision 1.24  2005/09/20 12:20:42  romy
// Added wait cursor on Transfer click
//
// Revision 1.23  2005/09/18 02:54:25  romy
// Null check added on Transfer
//
// Revision 1.22  2005/09/15 18:40:13  romy
// Removed temp warning message on Transfer Exclusion checks
//
// Revision 1.21  2005/09/14 16:51:23  romy
// modified anonymization reference
//
// Revision 1.20  2005/09/12 12:52:44  gdavidson
// Modified the transfer click method.
//
// Revision 1.19  2005/09/03 01:02:45  romy
// revisited transfer
//
// Revision 1.18  2005/09/02 12:57:55  romy
// Modified for Anonimization
//
// Revision 1.17  2005/08/31 18:34:22  romy
// Transfer - Temporary Implementation
//
// Revision 1.16  2005/08/31 18:32:36  romy
// Transfer - Temporary Implementation
//
// Revision 1.15  2005/08/31 16:21:51  romy
// added Transfer Menu
//
// Revision 1.14  2005/08/31 13:49:11  gdavidson
// Added RetrieveData methods to DataProvider.
//
// Revision 1.13  2005/08/31 12:32:15  gdavidson
// Added LaunchRules for study and series double click.
//
// Revision 1.12  2005/08/02 12:40:04  gdavidson
// Added launch methods for viewers and reports.
//
// Revision 1.11  2005/06/24 14:59:36  gdavidson
// Changed the speeling for the DataProvider's Initialize method.
//
// Revision 1.10  2005/06/22 15:20:57  gdavidson
// Modifeid the sizing of the splitter controls.
//
// Revision 1.9  2005/06/16 13:04:59  gdavidson
// Added a splitter to the series panel.
//
// Revision 1.8  2005/06/02 13:36:11  gdavidson
// Commented code.
//
// Revision 1.7  2005/05/27 12:50:40  gdavidson
// Changed the name of the section handler to DynamicListViewConfigSectionHandler.
//
// Revision 1.6  2005/05/26 19:35:44  gdavidson
// Moved the listview configuration to the app.config.
//
// Revision 1.5  2005/05/26 17:40:38  gdavidson
// Inital Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/ViewBrowser.cs,v 1.90.2.20 2011/05/18 01:56:59 mkontak Exp $
// $Id: ViewBrowser.cs,v 1.90.2.20 2011/05/18 01:56:59 mkontak Exp $

#endregion
