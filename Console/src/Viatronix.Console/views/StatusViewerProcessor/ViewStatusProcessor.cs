// $Id: ViewStatusProcessor.cs,v 1.5.2.3 2011/05/18 01:57:04 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

#region using

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Configuration;
using System.Collections.Specialized;
using System.Reflection;
using Viatronix.UI;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using BrightIdeasSoftware;

#endregion using

namespace Viatronix.Console
{



  #region DicomServerProperties class

  /// <summary>
  /// Class which holds the Dicom Server Properties
  /// </summary>
  public class DicomServerProperties
  {

    #region fields

    /// <summary>
    /// Server Name
    /// </summary>
    private string m_serverName = string.Empty;

    /// <summary>
    /// Listening Port
    /// </summary>
    private int m_port = 104;

    /// <summary>
    /// AE Title
    /// </summary>
    private string m_aeTitle = string.Empty;

 
    #endregion

    #region construction

    /// <summary>
    /// Constrcutor
    /// </summary>
    public DicomServerProperties()
    {
    } // DicomServerProperties()

    #endregion

    #region properties

    /// <summary>
    /// Gets or Sets the server Name
    /// </summary>
    public string ServerName
    {
      get { return m_serverName; }
      set { m_serverName = value; }
    }//ServerName

    /// <summary>
    /// Gets or Sets the port.
    /// </summary>
    public int ListeningPort
    {
      get { return m_port; }
      set { m_port = value; }
    }//ListeningPort


 

    /// <summary>
    /// Gets or Sets the AETitle
    /// </summary>
    public string AETitle
    {
      get { return m_aeTitle; }
      set { m_aeTitle = value; }
    }//AETitle

    #endregion

    #region methods

    /// <summary>
    /// Checks whether the DCM Server Parameters are empty or not
    /// </summary>
    /// <returns></returns>
    public bool IsEmpty()
    {
      if (ServerName == string.Empty || AETitle == string.Empty)
        return true;
      return false;
    }//IsEmpty()

    #endregion

  } // class DicomServerProperties

  #endregion

  #region ViewStatusProcessor class

  /// <summary>
  /// ViewStatusProcessor, Control which shows the Processor Status.
  /// </summary>
  public class ViewStatusProcessor : System.Windows.Forms.UserControl, IView, IPersistable
  {
  
    #region fields

    
    /// <summary>
    /// Startup task
    /// </summary>
    private System.Threading.Tasks.Task m_startupTask  = null;

    /// <summary>
    /// Header format style for the study and series list views
    /// </summary>
    private HeaderFormatStyle m_listViewHeaderFormatStyle = new HeaderFormatStyle();

    /// <summary>
    /// Maintains the path to the user's dicom configuration file.
    /// </summary>
    private readonly string m_dicomUserPath = string.Empty;

    /// <summary>
    /// Maintains the path to the user's wq configuration file.
    /// </summary>
    private readonly string m_wqUserPath = string.Empty;

    /// <summary>
    /// Maintains the path to the user's Pending Queue configuration file.
    /// </summary>
    private readonly string m_pendingQueueUserPath = string.Empty;

    /// <summary>
    /// Maintains the path to the user's Pending Queue configuration file.
    /// </summary>
    private readonly string m_messagesUserPath = string.Empty;

 
    /// <summary>
    /// components, System Generated Default parameter
    /// </summary>
    private System.ComponentModel.IContainer components = null;


     /// <summary>
    /// Label for Dicom Status
    /// </summary>
    private System.Windows.Forms.Label m_dicomQueueStatus;
    
    /// <summary>
    /// Label for WQ Status
    /// </summary>
    private System.Windows.Forms.Label m_labelWorkQueueStatus;

    /// <summary>
    /// Label for message Status
    /// </summary>
    private System.Windows.Forms.Label m_labelStatusMessage;

    /// <summary>
    /// Label for pending queue Status
    /// </summary>
    private System.Windows.Forms.Label m_labelPendingQueue;

 
    /// <summary>
    /// Pending jobs refresh timer
    /// </summary>
    private RefreshTimer<Job> m_pendingJobsTimer = null;

    /// <summary>
    /// Work queue refresh timer
    /// </summary>
    private RefreshTimer<Job> m_workQueueTimer = null;

    /// <summary>
    /// Disom server refresh timer
    /// </summary>
    private RefreshTimer<DicomServer> m_dicomServerTimer = null;

    /// <summary>
    /// Status message timer
    /// </summary>
    private RefreshTimer<Viatronix.Enterprise.Entities.Message> m_messageTimer = null;

    
    /// <summary>
    /// Splitter Object
    /// </summary>
    private System.Windows.Forms.Splitter splitter1;

    /// <summary>
    /// Splitter Object
    /// </summary>
    private System.Windows.Forms.Splitter splitter2;

    /// <summary>
    /// Splitter Object
    /// </summary>
    private System.Windows.Forms.Splitter splitter3;
    
    /// <summary>
    /// Event handler for the closing event.
    /// </summary>
    private CancelEventHandler m_closingHandler = null;

    /// <summary>
    /// WorkQueueServer Status Dynamic List View
    /// </summary>
    private ListView m_workQueueListView;

    /// <summary>
    /// Pending Queue Status Dynamic List View
    /// </summary>
    private ListView m_pendingQueueListView;

    /// <summary>
    /// Message Status Dynamic List View
    /// </summary>
    private ObjectListView m_messageListView;

    /// <summary>
    /// Dicom Server Status Dynamic List View
    /// </summary>
    private ListView m_dicomServerListView;
   
    /// <summary>
    /// Event handler for the view changing event.
    /// </summary>
    private CancelEventHandler m_viewChangingHandler = null;

    /// <summary>
    /// Dicom Status Panel
    /// </summary>
    private Viatronix.UI.Panel m_dicomStatusPanel;

    /// <summary>
    /// Pending Q panel
    /// </summary>
    private Viatronix.UI.Panel m_pendingQueuePanel;

    /// <summary>
    /// Status message panel
    /// </summary>
    private Viatronix.UI.Panel m_statusMessagePanel;

    /// <summary>
    /// Work queue status panel
    /// </summary>
    private Viatronix.UI.Panel m_workQueueStatusPanel;


    /// <summary>
    /// Image list
    /// </summary>
    private System.Windows.Forms.ImageList m_imageList = new ImageList();


    /// <summary>
    /// Pending Queue Menu 
    /// </summary>
    Viatronix.UI.ContextMenu m_contextMenuPendingJobs = new Viatronix.UI.ContextMenu( );

    /// <summary>
    /// DICOM status menu 
    /// </summary>
    Viatronix.UI.ContextMenu m_contextMenuDcmStatus = new Viatronix.UI.ContextMenu( );

    /// <summary>
    /// WQ Status menu 
    /// </summary>
    Viatronix.UI.ContextMenu m_contextMenuWQStatus = new Viatronix.UI.ContextMenu( );
    private ToolTip m_toolTip;

    /// <summary>
    /// Error message menu
    /// </summary>
    Viatronix.UI.ContextMenu m_contextMenuMessageStatus = new Viatronix.UI.ContextMenu( );

    
    #endregion fields

    #region IView

    /// <summary>
    /// Called when the view is initially created.
    /// </summary>
    /// <param name="xml">Additional xml configuration data</param>
    void IView.Initialize( string xml )
    {
      try
      {
        //Load the Server Service Config Values from the App config file.
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);
        
        /// Initilaize the refresh timers
        m_dicomServerTimer.Initialize(doc.SelectSingleNode( "/ServerServices/DicomServices" ));
        m_workQueueTimer.Initialize(doc.SelectSingleNode( "/ServerServices/WQServices" ));
        m_pendingJobsTimer.Initialize(doc.SelectSingleNode( "/ServerServices/PendingQueue" ));
        m_messageTimer.Initialize(doc.SelectSingleNode( "/ServerServices/StatusMessage" ));
        m_messageTimer.Enabled = true;

      

        // ======================================================================================
        // At the first time donot wait for the timer tick event to occur. Refresh all Views.
        // This refresh will start all the timer automatically
        // ======================================================================================
        DateTime start = DateTime.Now;
        RefreshAllViews();
        Log.Timing(String.Format("Time to initilalise all status views with data was {0} ms", DateTime.Now.Subtract(start).TotalMilliseconds), "ViewStatusProcessor", "Initilaize");

        Global.Frame.ViewChanging += m_viewChangingHandler;
        Global.Frame.Closing      += m_closingHandler;
      }
      catch(Exception e)
      {
        Viatronix.Logging.Log.Error("Failed to initialize the status view : "+ e.Message, "ViewStatusProcessor", "IView.Initialize");
      }
    } // Initialize( string xml )


    #endregion

    #region IPersistable Members

    /// <summary>
    /// Allows the view to load any persisted state. Onlt the panel sizes are saved.
    /// </summary>
    /// <param name="viewState">The view's persistable state.</param>
    void IPersistable.LoadViewState( Dictionary< string, object > viewState )
    {
      object state = null;
      if( viewState.TryGetValue( "DicomStatusPanel", out state ))
        m_dicomStatusPanel.Size = (Size) state;

      if( viewState.TryGetValue( "WorkQueueStatusPanel", out state ))
        m_workQueueStatusPanel.Size = (Size) state;

      if( viewState.TryGetValue( "PendingQueuePanel", out state ))
        m_pendingQueuePanel.Size = (Size) state;

      if( viewState.TryGetValue( "StatusMessagePanel", out state ))
        m_statusMessagePanel.Size = (Size) state;
    } // LoadViewState( viewState )


    /// <summary>
    /// Allows the view to persist its state.
    /// </summary>
    /// <param name="viewState">The view's persisted state.</param>
    void IPersistable.SaveViewState( Dictionary< string, object > viewState )
    {
      // store view state
      viewState[ "DicomStatusPanel" ]     = m_dicomStatusPanel.Size;
      viewState[ "WorkQueueStatusPanel" ] = m_workQueueStatusPanel.Size;
      viewState[ "PendingQueuePanel" ]    = m_pendingQueuePanel.Size;
      viewState[ "StatusMessagePanel" ]   = m_statusMessagePanel.Size;
    } // SaveViewState( viewState )
   

    #endregion

    #region constructors/destructors


    /// <summary>
    /// Initializes a ViewStatusProcessor instance with default values
    /// </summary>
    public ViewStatusProcessor()
    {


      m_workQueueTimer      = new RefreshTimer<Job>("WorkQueue", this);
      m_dicomServerTimer    = new RefreshTimer<DicomServer>("DicomServer", this);
      m_messageTimer  = new RefreshTimer<Enterprise.Entities.Message>("StatusMessage", this);
      m_pendingJobsTimer    = new RefreshTimer<Job>("PendingJob", this);


      //Set UI Styles
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      m_workQueueTimer.Update = new RefreshTimer<Job>.UpdateDelegate(UpdateWorkQueue);
      m_workQueueTimer.Refresh = new RefreshTimer<Job>.RefreshDelegate(RefreshWorkQueue);

      m_dicomServerTimer.Update = new RefreshTimer<DicomServer>.UpdateDelegate(UpdateDicomServer);
      m_dicomServerTimer.Refresh = new RefreshTimer<DicomServer>.RefreshDelegate(RefreshDicomServer);


      m_pendingJobsTimer.Update = new RefreshTimer<Job>.UpdateDelegate(UpdatePendingJobs);
      m_pendingJobsTimer.Refresh = new RefreshTimer<Job>.RefreshDelegate(RefreshPendingJobs);

      m_messageTimer.Update = new RefreshTimer<Enterprise.Entities.Message>.UpdateDelegate(UpdateStatusMessages);
      m_messageTimer.Refresh = new RefreshTimer<Enterprise.Entities.Message>.RefreshDelegate(RefreshStatusMessages);

      InitializeComponent();

      m_dicomServerListView.Columns.Add("Server").Width = 90;
      m_dicomServerListView.Columns.Add("AE Title").Width = 90;
      m_dicomServerListView.Columns.Add("Name").Width = 150;
      m_dicomServerListView.Columns.Add("Orientation").Width = 90;
      m_dicomServerListView.Columns.Add("Count").Width = 90;
      m_dicomServerListView.Columns.Add("Date").Width = 170;

      m_pendingQueueListView.Columns.Add("State").Width = 70;
      m_pendingQueueListView.Columns.Add("Application").Width = 60;
      m_pendingQueueListView.Columns.Add("Task").Width = 70;
      m_pendingQueueListView.Columns.Add("Patient").Width = 150;
      m_pendingQueueListView.Columns.Add("Count").Width = 90;
      m_pendingQueueListView.Columns.Add("Orientation").Width = 100;
      m_pendingQueueListView.Columns.Add("Modality").Width = 90;
      m_pendingQueueListView.Columns.Add("Created").Width = 150;



      m_workQueueListView.Columns.Add("Patient").Width = 150;
      m_workQueueListView.Columns.Add("Orientation").Width = 90;
      m_workQueueListView.Columns.Add("Task").Width = 90;
      m_workQueueListView.Columns.Add("Stage").Width = 90;
      m_workQueueListView.Columns.Add("Progress").Width = 90;
      m_workQueueListView.Columns.Add("Duration").Width = 90;
 

      m_dicomQueueStatus.Font     = Theme.HeaderFont;
      m_labelWorkQueueStatus.Font = Theme.HeaderFont;
      m_labelStatusMessage.Font   = Theme.HeaderFont;
      m_labelPendingQueue.Font    = Theme.HeaderFont;

      InitializeListViews();



      this.Font      = Theme.FormFont;
      this.BackColor = Color.Transparent;

      //Create UserPreference Files.
      m_dicomUserPath        = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewStatusProcessor_Dicom.xml" );
      m_wqUserPath           = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewStatusProcessor_Wq.xml" );
      m_pendingQueueUserPath = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewStatusProcessor_pendingJobs.xml" );
      m_messagesUserPath     = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewStatusProcessor_messages.xml" );

      //Set Colors
      splitter1.BackColor = Theme.BackColor;
      splitter2.BackColor = Theme.BackColor;
      splitter3.BackColor = Theme.BackColor;
      this.BackColor      = Color.Transparent;
    } // ViewStatusProcessor()


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {

      if( disposing )
      {
 
        if( components != null )  
          components.Dispose();

        m_dicomServerTimer.Stop();
        m_workQueueTimer.Stop();
        m_pendingJobsTimer.Stop();
        m_messageTimer.Stop();

      }

      base.Dispose( disposing );

    } // Dispose( bool disposing )


    #endregion constructors/destructors
   
    #region Component Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
this.components = new System.ComponentModel.Container();
this.m_dicomQueueStatus = new System.Windows.Forms.Label();
this.m_labelWorkQueueStatus = new System.Windows.Forms.Label();
this.m_labelPendingQueue = new System.Windows.Forms.Label();
this.m_labelStatusMessage = new System.Windows.Forms.Label();
this.m_dicomStatusPanel = new Viatronix.UI.Panel();
this.m_dicomServerListView = new ListView();
this.splitter1 = new System.Windows.Forms.Splitter();
this.m_workQueueStatusPanel = new Viatronix.UI.Panel();
this.m_workQueueListView = new ListView();
this.splitter2 = new System.Windows.Forms.Splitter();
this.m_pendingQueuePanel = new Viatronix.UI.Panel();
this.m_messageListView = new ObjectListView();
this.splitter3 = new System.Windows.Forms.Splitter();
this.m_statusMessagePanel = new Viatronix.UI.Panel();
this.m_pendingQueueListView = new ListView();
this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
this.m_dicomStatusPanel.SuspendLayout();
this.m_workQueueStatusPanel.SuspendLayout();
this.m_pendingQueuePanel.SuspendLayout();
this.m_statusMessagePanel.SuspendLayout();
this.SuspendLayout();
// 
// m_dicomQueueStatus
// 
this.m_dicomQueueStatus.BackColor = System.Drawing.Color.Transparent;
this.m_dicomQueueStatus.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
this.m_dicomQueueStatus.ForeColor = System.Drawing.Color.White;
this.m_dicomQueueStatus.Location = new System.Drawing.Point(8, 8);
this.m_dicomQueueStatus.Name = "m_dicomQueueStatus";
this.m_dicomQueueStatus.Size = new System.Drawing.Size(264, 24);
this.m_dicomQueueStatus.TabIndex = 0;
this.m_dicomQueueStatus.Text = "DICOM Services Activity Monitor";
this.m_dicomQueueStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
// 
// m_labelWorkQueueStatus
// 
this.m_labelWorkQueueStatus.BackColor = System.Drawing.Color.Transparent;
this.m_labelWorkQueueStatus.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
this.m_labelWorkQueueStatus.ForeColor = System.Drawing.Color.White;
this.m_labelWorkQueueStatus.Location = new System.Drawing.Point(8, 8);
this.m_labelWorkQueueStatus.Name = "m_labelWorkQueueStatus";
this.m_labelWorkQueueStatus.Size = new System.Drawing.Size(288, 24);
this.m_labelWorkQueueStatus.TabIndex = 2;
this.m_labelWorkQueueStatus.Text = "Pipeline Services Activity Monitor";
// 
// m_labelPendingQueue
// 
this.m_labelPendingQueue.BackColor = System.Drawing.Color.Transparent;
this.m_labelPendingQueue.Font = new System.Drawing.Font("Times New Roman", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
this.m_labelPendingQueue.ForeColor = System.Drawing.Color.White;
this.m_labelPendingQueue.Location = new System.Drawing.Point(8, 8);
this.m_labelPendingQueue.Name = "m_labelPendingQueue";
this.m_labelPendingQueue.Size = new System.Drawing.Size(288, 23);
this.m_labelPendingQueue.TabIndex = 4;
this.m_labelPendingQueue.Text = "Pending Queue Status";
// 
// m_labelStatusMessage
// 
this.m_labelStatusMessage.BackColor = System.Drawing.Color.Transparent;
this.m_labelStatusMessage.Font = new System.Drawing.Font("Times New Roman", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
this.m_labelStatusMessage.ForeColor = System.Drawing.Color.White;
this.m_labelStatusMessage.Location = new System.Drawing.Point(8, 8);
this.m_labelStatusMessage.Name = "m_labelStatusMessage";
this.m_labelStatusMessage.Size = new System.Drawing.Size(488, 24);
this.m_labelStatusMessage.TabIndex = 6;
this.m_labelStatusMessage.Text = "System Information, Warning and Error Messages";
// 
// m_dicomStatusPanel
// 
this.m_dicomStatusPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_dicomStatusPanel.BorderColor = System.Drawing.Color.White;
this.m_dicomStatusPanel.BottomPadding = 0;
this.m_dicomStatusPanel.Controls.Add(this.m_dicomServerListView);
this.m_dicomStatusPanel.Controls.Add(this.m_dicomQueueStatus);
this.m_dicomStatusPanel.Curve = 10;
this.m_dicomStatusPanel.Dock = System.Windows.Forms.DockStyle.Top;
this.m_dicomStatusPanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_dicomStatusPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_dicomStatusPanel.LeftPadding = 0;
this.m_dicomStatusPanel.Location = new System.Drawing.Point(0, 0);
this.m_dicomStatusPanel.Name = "m_dicomStatusPanel";
this.m_dicomStatusPanel.RightPadding = 0;
this.m_dicomStatusPanel.Size = new System.Drawing.Size(896, 149);
this.m_dicomStatusPanel.TabIndex = 0;
this.m_dicomStatusPanel.TopPadding = 0;
// 
// m_dicomServerListView
// 
this.m_dicomServerListView.AllowColumnReorder = true;
this.m_dicomServerListView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_dicomServerListView.ShowGroups = false;
this.m_dicomServerListView.FullRowSelect = true;
this.m_dicomServerListView.GridLines = true;
this.m_dicomServerListView.Location = new System.Drawing.Point(8, 32);
this.m_dicomServerListView.Name = "m_dicomServerListView";
this.m_dicomServerListView.Size = new System.Drawing.Size(880, 109);
this.m_dicomServerListView.TabIndex = 0;
this.m_dicomServerListView.UseCompatibleStateImageBehavior = false;
this.m_dicomServerListView.View = System.Windows.Forms.View.Details;
this.m_dicomServerListView.DoubleClick += new System.EventHandler(this.OnDicomStatusDoubleClick);
this.m_dicomServerListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnDicomStatusMouseClick);
this.m_dicomServerListView.MouseEnter += new System.EventHandler(this.OnDicomStatusMouseEnter);
this.m_dicomServerListView.MouseMove += new System.Windows.Forms.MouseEventHandler(this.OnDicomStatusMouseMove);
this.m_dicomServerListView.MouseLeave += new System.EventHandler(this.OnDicomStatusMouseLeave);
// 
// splitter1
// 
this.splitter1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.splitter1.Dock = System.Windows.Forms.DockStyle.Top;
this.splitter1.Location = new System.Drawing.Point(0, 149);
this.splitter1.MinExtra = 200;
this.splitter1.MinSize = 100;
this.splitter1.Name = "splitter1";
this.splitter1.Size = new System.Drawing.Size(896, 8);
this.splitter1.TabIndex = 9;
this.splitter1.TabStop = false;
// 
// m_workQueueStatusPanel
// 
this.m_workQueueStatusPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_workQueueStatusPanel.BorderColor = System.Drawing.Color.White;
this.m_workQueueStatusPanel.BottomPadding = 0;
this.m_workQueueStatusPanel.Controls.Add(this.m_workQueueListView);
this.m_workQueueStatusPanel.Controls.Add(this.m_labelWorkQueueStatus);
this.m_workQueueStatusPanel.Curve = 10;
this.m_workQueueStatusPanel.Dock = System.Windows.Forms.DockStyle.Top;
this.m_workQueueStatusPanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_workQueueStatusPanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_workQueueStatusPanel.LeftPadding = 0;
this.m_workQueueStatusPanel.Location = new System.Drawing.Point(0, 157);
this.m_workQueueStatusPanel.Name = "m_workQueueStatusPanel";
this.m_workQueueStatusPanel.RightPadding = 0;
this.m_workQueueStatusPanel.Size = new System.Drawing.Size(896, 144);
this.m_workQueueStatusPanel.TabIndex = 1;
this.m_workQueueStatusPanel.TopPadding = 0;
// 
// m_workQueueListView
// 
this.m_workQueueListView.AllowColumnReorder = true;
this.m_workQueueListView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_workQueueListView.ShowGroups = false;
this.m_workQueueListView.FullRowSelect = true;
this.m_workQueueListView.GridLines = true;
this.m_workQueueListView.Location = new System.Drawing.Point(8, 32);
this.m_workQueueListView.Name = "m_workQueueListView";
this.m_workQueueListView.Size = new System.Drawing.Size(880, 104);
this.m_workQueueListView.TabIndex = 1;
this.m_workQueueListView.UseCompatibleStateImageBehavior = false;
this.m_workQueueListView.View = System.Windows.Forms.View.Details;
this.m_workQueueListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnWorkQueueStatusMouseClick);
// 
// splitter2
// 
this.splitter2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.splitter2.Dock = System.Windows.Forms.DockStyle.Top;
this.splitter2.Location = new System.Drawing.Point(0, 301);
this.splitter2.MinExtra = 200;
this.splitter2.MinSize = 100;
this.splitter2.Name = "splitter2";
this.splitter2.Size = new System.Drawing.Size(896, 8);
this.splitter2.TabIndex = 11;
this.splitter2.TabStop = false;
// 
// m_pendingQueuePanel
// 
this.m_pendingQueuePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_pendingQueuePanel.BorderColor = System.Drawing.Color.White;
this.m_pendingQueuePanel.BottomPadding = 5;
this.m_pendingQueuePanel.Controls.Add(this.m_messageListView);
this.m_pendingQueuePanel.Controls.Add(this.m_labelStatusMessage);
this.m_pendingQueuePanel.Curve = 10;
this.m_pendingQueuePanel.Dock = System.Windows.Forms.DockStyle.Fill;
this.m_pendingQueuePanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_pendingQueuePanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_pendingQueuePanel.LeftPadding = 0;
this.m_pendingQueuePanel.Location = new System.Drawing.Point(0, 482);
this.m_pendingQueuePanel.Name = "m_pendingQueuePanel";
this.m_pendingQueuePanel.RightPadding = 0;
this.m_pendingQueuePanel.Size = new System.Drawing.Size(896, 214);
this.m_pendingQueuePanel.TabIndex = 3;
this.m_pendingQueuePanel.TopPadding = 0;
// 
// m_messageListView
// 
this.m_messageListView.AllowColumnReorder = true;
this.m_messageListView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_messageListView.ShowGroups = false;
this.m_messageListView.FullRowSelect = true;
this.m_messageListView.GridLines = true;
this.m_messageListView.HideSelection = false;
this.m_messageListView.Location = new System.Drawing.Point(8, 32);
this.m_messageListView.Name = "m_messageListView";
this.m_messageListView.Size = new System.Drawing.Size(880, 166);
this.m_messageListView.Sorting = System.Windows.Forms.SortOrder.Ascending;
this.m_messageListView.TabIndex = 3;
this.m_messageListView.UseCompatibleStateImageBehavior = false;
this.m_messageListView.View = System.Windows.Forms.View.Details;
this.m_messageListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnErrorMessagesMouseUp);
// 
// splitter3
// 
this.splitter3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.splitter3.Dock = System.Windows.Forms.DockStyle.Top;
this.splitter3.Location = new System.Drawing.Point(0, 474);
this.splitter3.MinExtra = 200;
this.splitter3.MinSize = 100;
this.splitter3.Name = "splitter3";
this.splitter3.Size = new System.Drawing.Size(896, 8);
this.splitter3.TabIndex = 13;
this.splitter3.TabStop = false;
// 
// m_statusMessagePanel
// 
this.m_statusMessagePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(148)))), ((int)(((byte)(166)))), ((int)(((byte)(198)))));
this.m_statusMessagePanel.BorderColor = System.Drawing.Color.White;
this.m_statusMessagePanel.BottomPadding = 0;
this.m_statusMessagePanel.Controls.Add(this.m_pendingQueueListView);
this.m_statusMessagePanel.Controls.Add(this.m_labelPendingQueue);
this.m_statusMessagePanel.Curve = 10;
this.m_statusMessagePanel.Dock = System.Windows.Forms.DockStyle.Top;
this.m_statusMessagePanel.Edges = ((Viatronix.UI.Edges)(((((Viatronix.UI.Edges.TopLeft | Viatronix.UI.Edges.TopRight) 
            | Viatronix.UI.Edges.BottomRight) 
            | Viatronix.UI.Edges.BottomLeft) 
            | Viatronix.UI.Edges.All)));
this.m_statusMessagePanel.FillColor = System.Drawing.Color.FromArgb(((int)(((byte)(123)))), ((int)(((byte)(134)))), ((int)(((byte)(165)))));
this.m_statusMessagePanel.LeftPadding = 0;
this.m_statusMessagePanel.Location = new System.Drawing.Point(0, 309);
this.m_statusMessagePanel.Name = "m_statusMessagePanel";
this.m_statusMessagePanel.RightPadding = 0;
this.m_statusMessagePanel.Size = new System.Drawing.Size(896, 165);
this.m_statusMessagePanel.TabIndex = 2;
this.m_statusMessagePanel.TopPadding = 0;
// 
// m_pendingQueueListView
// 
this.m_pendingQueueListView.AllowColumnReorder = true;
this.m_pendingQueueListView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
this.m_pendingQueueListView.ShowGroups = false;
this.m_pendingQueueListView.FullRowSelect = true;
this.m_pendingQueueListView.GridLines = true;
this.m_pendingQueueListView.HideSelection = false;
this.m_pendingQueueListView.Location = new System.Drawing.Point(8, 32);
this.m_pendingQueueListView.Name = "m_pendingQueueListView";
this.m_pendingQueueListView.Size = new System.Drawing.Size(880, 122);
this.m_pendingQueueListView.TabIndex = 2;
this.m_pendingQueueListView.UseCompatibleStateImageBehavior = false;
this.m_pendingQueueListView.View = System.Windows.Forms.View.Details;
this.m_pendingQueueListView.DoubleClick += new System.EventHandler(this.OnPendingQueueDoubleClick);
this.m_pendingQueueListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnPendingQueueStatusMouseUp);
this.m_pendingQueueListView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPendingQueueKeyDownHandler);
// 
// m_toolTip
// 
this.m_toolTip.UseAnimation = true;
// 
// ViewStatusProcessor
// 
this.Controls.Add(this.m_pendingQueuePanel);
this.Controls.Add(this.splitter3);
this.Controls.Add(this.m_statusMessagePanel);
this.Controls.Add(this.splitter2);
this.Controls.Add(this.m_workQueueStatusPanel);
this.Controls.Add(this.splitter1);
this.Controls.Add(this.m_dicomStatusPanel);
this.Name = "ViewStatusProcessor";
this.Size = new System.Drawing.Size(896, 696);
this.m_dicomStatusPanel.ResumeLayout(false);
this.m_workQueueStatusPanel.ResumeLayout(false);
this.m_pendingQueuePanel.ResumeLayout(false);
this.m_statusMessagePanel.ResumeLayout(false);
this.ResumeLayout(false);

    }
    #endregion

    #region Methods & Event Handlers

    #region initialization

    /// <summary>
    /// Initializes the list views
    /// </summary>
    private void InitializeListViews()
    {
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


      InitializeMessageListView();

    } // InitialzieListViews()


    /// <summary>
    /// Initialzie the message list view
    /// </summary>
    private void InitializeMessageListView()
    {
      // ===========================================================
      // Initializes the object list view with the study object
      // ==========================================================
      TypedObjectListView<Viatronix.Enterprise.Entities.Message> tlist = new TypedObjectListView<Viatronix.Enterprise.Entities.Message>(m_messageListView);
      tlist.GenerateAspectGetters();


      m_messageListView.FullRowSelect = true;
      m_messageListView.MultiSelect = true;
      m_messageListView.ShowGroups = false;
      m_messageListView.ShowImagesOnSubItems = true;
      m_messageListView.OwnerDraw = true;
      m_messageListView.UseCellFormatEvents = true;
      m_messageListView.UseCompatibleStateImageBehavior = false;
      m_messageListView.UseFilterIndicator = false;
      m_messageListView.UseFiltering = false;
      m_messageListView.UseHotItem = true;
      m_messageListView.UseTranslucentHotItem = true;
      m_messageListView.ShowCommandMenuOnRightClick = true;
      m_messageListView.ShowItemCountOnGroups = false;
      m_messageListView.SelectColumnsOnRightClickBehaviour = BrightIdeasSoftware.ObjectListView.ColumnSelectBehaviour.Submenu;
      m_messageListView.HeaderUsesThemes = false;
      m_messageListView.OwnerDraw = true;
      m_messageListView.GridLines = true;
      m_messageListView.HeaderFormatStyle = m_listViewHeaderFormatStyle;

      //Create an Icon list for System Information messages.
      Assembly assembly = Assembly.GetExecutingAssembly();

      ImageList imageList = new ImageList();
      imageList.ImageSize = new Size(12, 12);
      imageList.Images.Add(new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.Information.bmp")));
      imageList.Images.Add(new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.warning.bmp")));
      imageList.Images.Add(new Bitmap(assembly.GetManifestResourceStream("Viatronix.Console.res.error.bmp")));

      m_messageListView.SmallImageList = imageList;


  
      // ========================================
      // State Column - Used for the icon/gif
      // ========================================
      //OLVColumn olvColumn = new OLVColumn();
      //olvColumn.AspectName = "Type";
      //olvColumn.Text = "Type";
      //olvColumn.ShowTextInHeader = true;
      //olvColumn.Width = 50;
      //olvColumn.CellPadding = null;
      //olvColumn.IsTileViewColumn = false;
      //olvColumn.Groupable = false;
      //olvColumn.Sortable = false;
      //olvColumn.UseFiltering = true;
      //olvColumn.ImageGetter = delegate(object row) { return (int)((Viatronix.Enterprise.Entities.Message)row).Type; };
      //olvColumn.AspectGetter = delegate(object row) { string nothing = string.Empty; return nothing; };
      //m_messageListView.AllColumns.Add(olvColumn);
      //m_messageListView.Columns.Add(olvColumn);

      // ===============
      // Patient Name
      // ===============
      OLVColumn olvColumn = new OLVColumn();
      olvColumn.AspectName = "Name";
      olvColumn.Text = "Patient";
      olvColumn.Width = 200;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = true;
      olvColumn.Sortable = true;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Viatronix.Enterprise.Entities.Message)(obj)).Name; };
      olvColumn.ImageGetter = delegate(object row) { return (int)((Viatronix.Enterprise.Entities.Message)row).Type; };
      m_messageListView.AllColumns.Add(olvColumn);
      m_messageListView.Columns.Add(olvColumn);


      // =============
      // Medical Id
      // =============
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "Orientation";
      olvColumn.Text = "Orientation";
      olvColumn.Width = 90;
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.AspectGetter = delegate(object obj) { return ((Viatronix.Enterprise.Entities.Message)(obj)).Orientation; };
      m_messageListView.AllColumns.Add(olvColumn);
      m_messageListView.Columns.Add(olvColumn);


      // ===========
      // Scan Date
      // ===========
      olvColumn = new OLVColumn();
      olvColumn.AspectName = "Date";
      olvColumn.Text = "Date";
      olvColumn.Width = 100;
      olvColumn.AspectToStringFormat = "{0:g}";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Viatronix.Enterprise.Entities.Message)(obj)).Date; };
      m_messageListView.AllColumns.Add(olvColumn);
      m_messageListView.Columns.Add(olvColumn);
  
      m_messageListView.PrimarySortColumn = olvColumn;
      m_messageListView.PrimarySortOrder = SortOrder.Ascending;
      


      // ==================
      // Study Description
      // ==================
      olvColumn = new OLVColumn();
      olvColumn.Text = "Message";
      olvColumn.AspectName = "Text";
      olvColumn.CellPadding = null;
      olvColumn.IsTileViewColumn = false;
      olvColumn.Groupable = false;
      olvColumn.Sortable = false;
      olvColumn.UseFiltering = false;
      olvColumn.FillsFreeSpace = true;
      olvColumn.AspectGetter = delegate(object obj) { return ((Viatronix.Enterprise.Entities.Message)(obj)).Text; };
      m_messageListView.AllColumns.Add(olvColumn);
      m_messageListView.Columns.Add(olvColumn);



    } // InitialzieMessageListView()

    #endregion

    #region General Methods

    /// <summary>
    /// Refreshes all views irrespective of timer click event
    /// </summary>
    private void RefreshAllViews()
    {
      m_workQueueTimer.RunSynchronously();
      m_dicomServerTimer.RunSynchronously();
      m_pendingJobsTimer.RunSynchronously();
      m_messageTimer.RunSynchronously();


     
    }//RefreshAllViews()
  

 
   /// <summary>
    /// Reset All Timers
    /// </summary>
    private void ResetTimers()
    {
      m_workQueueTimer.Reset();
      m_dicomServerTimer.Reset();
      m_pendingJobsTimer.Reset();
      m_messageTimer.Reset();
     } // ResetTimers()


    #endregion

    #region General Event Handlers
    
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


      //m_workQueueListView.Save( m_wqUserPath );
      //m_pendingQueueListView.Save( m_pendingQueueUserPath );
      //m_messageListView.Save( m_messagesUserPath );    
    } // OnViewChangingHandler( sender, args )


    #endregion



    #region Work Queue

    #region Work Queue Event Handlers


    /// <summary>
    /// Handles the Work Queue mouse click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">MouseEventArgs</param>
    private void OnWorkQueueStatusMouseClick(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      if (m_workQueueListView.Items.Count == 0 || m_workQueueListView.SelectedItems.Count <= 0)
        return;

      //Don't proceed if there is nothing in the Working Queue
      Job job = (Job)m_workQueueListView.SelectedItems[0].Tag;

      if (args.Button == MouseButtons.Right)
      {

        m_contextMenuWQStatus.MenuItems.Clear();
        System.Windows.Forms.MenuItem menuItemCancel = new MenuItem("Cancel", new System.EventHandler(this.OnCancelJob));

        //Build the Context Menu
        m_contextMenuWQStatus.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] { menuItemCancel });
        m_contextMenuWQStatus.Show(m_workQueueListView, new System.Drawing.Point(args.X, args.Y));
      }//if(e.Button == MouseButtons.Right)

    }//OnWorkQueueStatusMouseClick( object sender, System.Windows.Forms.MouseEventArgs args )


    /// <summary>
    /// Manages the Cancel Operation on a Job in the WQ
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventARgs</param>
    private void OnCancelJob(object sender, EventArgs args)
    {

      m_workQueueTimer.Stop();

      //Get the Selected Row Items.
      foreach (ListViewItem item in m_workQueueListView.SelectedItems)
      {
        Job job = (Job)item.Tag;

        try
        {
          Global.DataProvider.Cancel( job );
        }
        catch (Exception ex)
        {
          Viatronix.UI.MessageBox.ShowError(string.Format("Failed to cancel job [{0}-{1}]: {2}", job.Id, job.Task.Name, ex.Message));
        }
        finally
        {
          m_workQueueTimer.RunSynchronously();
        }

      }

    }//OnCancleTask( object sender,EventArgs args )


    #endregion

    #region Work Queue Methods


    /// <summary>
    /// Gets the work queue status
    /// </summary>
    /// <param name="url">string</param>
    /// <param name="statusDataset">DataSet</param>
    private void RefreshWorkQueue(ref List<Job> status)
    {

      try
      {
        Viatronix.Logging.Log.Debug("Refreshing the Work Queue Server Status", "ViewStatusProcessor", "RefreshWorkQueue");

        status = Global.DataProvider.GetJobs( Enterprise.XmlBuilder.Create("job", (new Dictionary<string, string>() { { "state", ((int)(States.Processing)).ToString() } })));
      }
      catch (Exception e)
      {
        Viatronix.Logging.Log.Debug("GetJobs failed :  " + e.Message, "ViewStatusProcessor", "GetServerStatus");
      }
    }//UpdateWorkQueueStatus( ref List<Job> status)


 

    /// <summary>
    /// Updates the work queue status on the GUID
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void UpdateWorkQueue(object sender, EventArgs args)
    {
      m_workQueueListView.BeginUpdate();

      try
      {

        if (m_workQueueListView != null)
        {


          m_workQueueListView.Items.Clear();

          foreach (Job job in m_workQueueTimer.List)
          {

            ListViewItem item = new ListViewItem(job.PatientName);
            item.SubItems.Add(job.Orientation);
            item.SubItems.Add(job.Task.Name);


            double duration = job.Duration;

            //item.SubItems.Add(task.Message);
            //item.SubItems.Add(((int)task.Progress).ToString());
            item.SubItems.Add(job.Stage);
            item.SubItems.Add(job.Progress > 0.0F ? ((int)job.Progress).ToString() : string.Empty);
            item.SubItems.Add(duration > 0 ? string.Format("{0:F2} sec", duration) : string.Empty);

            item.Tag = job;


            m_workQueueListView.Items.Add(item);
          }
        }

        //(Re)Start the Timer 
        if (m_workQueueTimer.Enabled == false)
          m_workQueueTimer.Start();
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Debug("UpdateWorkQueueStatus, " + ex.Message, "ViewStatusProcessor", "UpdateWorkQueueStatus");
      }
      finally
      {
        m_workQueueListView.EndUpdate();
      }

    } //UpdateWorkQueueStatus( object sender, EventArgs args ) 

    #endregion 

    #endregion


    #region Dicom Server

    #region Dicom Status Event Handlers


    /// <summary>
    /// OnDicomStatusDoubleClick, Handles the ondouble click event on DicomStatus Grid.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDicomStatusDoubleClick(object sender, EventArgs args)
    {
      m_dicomServerTimer.Stop();

      try
      {
        ManageDicomServerProperties(sender, null);
      }
      finally
      {
        m_dicomServerTimer.Start();
      }
    } // OnDicomStatusDoubleClick(object sender, EventArgs args)


    /// <summary>
    /// Handles the DicomStatusViewMouse Up event.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">MouseEventArgs</param>
    private void OnDicomStatusMouseClick(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      if (args.Button == MouseButtons.Right)
      {
        m_contextMenuDcmStatus.MenuItems.Clear();
        System.Windows.Forms.MenuItem menuItemProperties = new MenuItem("Properties", new System.EventHandler(this.ManageDicomServerProperties));

        if (m_dicomServerTimer.List.Count > 0 && m_dicomServerListView.SelectedItems.Count > 0)
          m_contextMenuDcmStatus.MenuItems.Add(0, menuItemProperties);

        m_contextMenuDcmStatus.Show(m_dicomServerListView, new System.Drawing.Point(args.X, args.Y));
      }//if(e.Button == MouseButtons.Right)

    }//OnDicomStatusMouseClick( object sender, System.Windows.Forms.MouseEventArgs args )


    #endregion

    #region Dicom Status Methods


    /// <summary>
    /// Gets the dicom server status.
    /// </summary>
    /// <param name="url">string</param>
    /// <param name="statusDataset">DataSet</param>
    private void RefreshDicomServer(ref List<DicomServer> status)
    {
      try
      {
        Viatronix.Logging.Log.Debug("Refreshing the Dicom Server Status", "ViewStatusProcessor", "RefreshDicomServer");

        status = Global.DataProvider.GetDicomServers();
      }
      catch (Exception e)
      {
        Viatronix.Logging.Log.Debug("GetDicomServers failed : " + e.Message, "ViewStatusProcessor", "RefreshDicomServer");
      }
    }//OnDicomGetStatusHandler( ref List<DicomServer> status)


    /// <summary>
    /// UpdateDicomStatus, event which updates the  m_dicomServerListView Grid.
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">EventArgs</param>
    private void UpdateDicomServer(object sender, EventArgs args)
    {

      m_dicomServerListView.BeginUpdate();

      try
      {
        //Bind the dataset to the status view.
        if (m_dicomServerListView != null)
        {
          m_dicomServerListView.Items.Clear();

          foreach (DicomServer server in m_dicomServerTimer.List)
          {

            if (server.Receivers.Count > 0)
            {
              foreach (Receiver receiver in server.Receivers)
              {
                ListViewItem item = new ListViewItem(receiver.Server);
                item.Tag = receiver;
                item.SubItems.Add(receiver.AETitle);
                item.SubItems.Add(receiver.Name);
                item.SubItems.Add(receiver.Orientation);
                item.SubItems.Add(receiver.Count.ToString());
                item.SubItems.Add(receiver.Date.ToString());
                m_dicomServerListView.Items.Add(item);
              }
            }
            else
            {
              Receiver receiver = new Receiver();
              receiver.AETitle = server.AETitle;
              receiver.Date = server.Date;
              receiver.Server = server.Server;

              ListViewItem item = new ListViewItem(server.Server);
              item.Tag = receiver;

              item.SubItems.Add(server.AETitle);
              item.SubItems.Add(string.Empty);
              item.SubItems.Add(string.Empty);
              item.SubItems.Add(string.Empty);
              item.SubItems.Add(server.Date.ToString());
              m_dicomServerListView.Items.Add(item);
            }


          }

        }

        //(Re)Start the Timer 
        if (m_dicomServerTimer.Enabled == false)
          m_dicomServerTimer.Start();
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Debug("ViewStatusProcessor.UpdateDicomStatus, " + ex.Message, "ViewStatusProcessor", "UpdateDicomStatus");
      }
      finally
      {
        m_dicomServerListView.EndUpdate();
      }
    } // UpdateDicomStatus( object sender, EventArgs args )



    /// <summary>
    /// Manages the Dicom Server properties.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventARgs</param>
    private void ManageDicomServerProperties(object sender, EventArgs args)
    {
      m_dicomServerTimer.Stop();

      try
      {
        //// get the binding object that synchronizes the dataset with the grid control

        DicomServerProperties dcmServerProperties = new DicomServerProperties();
        GetDicomServerProperties(dcmServerProperties);


        bool autoMode = Global.DataProvider.GetAutoProcessing();

        DicomServerManager dicomProperty = new DicomServerManager(dcmServerProperties, (bool)autoMode);

        //Set the Property Values here.
        if (dicomProperty.ShowDialog(m_dicomServerListView) == DialogResult.OK)
        {
          Global.DataProvider.SetAutoProcessing(dicomProperty.AutomaticMode);
        }
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to get DICOM Server Parameters : " + ex.Message, "ViewStatusProcessor", "ManageDicomServerProperties");
        Viatronix.UI.MessageBox.ShowError("Failed to get DICOM Server Parameters.");
      }
      finally
      {
        m_dicomServerTimer.Start();
      }

    }//ManageDicomServerProperties( object sender,EventArgs args )


    /// <summary>
    /// Gets the DICOM Server Properties.
    /// </summary>
    /// <param name="dcmServerProperties"></param>
    private void GetDicomServerProperties(DicomServerProperties dcmServerProperties)
    {
      if (m_dicomServerListView.SelectedItems.Count <= 0)
        return;

      //Get the Selected Row Items.
      Receiver receiver = (Receiver)m_dicomServerListView.SelectedItems[0].Tag;
      try
      {
        m_dicomServerTimer.Stop();
        // get the binding object that synchronizes the dataset with the grid control
        dcmServerProperties.ServerName = receiver.Server;
        dcmServerProperties.AETitle = receiver.AETitle;
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to get DICOM Server Parameters : " + ex.Message, "ViewStatusProcessor", "GetDicomServerProcessor");
        Viatronix.UI.MessageBox.ShowError("Failed to get DICOM Server Parameters.");
      }
    }//GetDicomServerProperties ( DicomServerProperties dcmServerProperties )

    #endregion

    #endregion
 
   
    #region PendingQueue Event Handlers


    
    /// <summary>
    /// Builds the Pending Queue Context Menu. Handles the MouseUP event.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnPendingQueueStatusMouseUp(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // If there are no selected items or no DICOM server then we cannot perform any operations on the pending jobs
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if ( m_pendingQueueListView.SelectedItems.Count > 0 )  
      {

        if ( args.Button == MouseButtons.Right )
        {

          m_pendingJobsTimer.Stop( );
          m_contextMenuPendingJobs.MenuItems.Clear( );

          try
          {
            bool processAsDefault = true; ;

            //Return if any one the selected Item have the Status as Pending.
            foreach ( ListViewItem item in m_pendingQueueListView.SelectedItems )
            {
              Job job = ( Job ) item.Tag;

              // If any of the rows do not contain a sub type only allow process as
              if ( ! string.IsNullOrEmpty(job.Application) ) 
                processAsDefault = false;

 

              /// This should no be necessary since no jobs being displayed should have a 
              /// state of processing.
              if ( job.IsProcessing ) return;

            } // END ... For each selected row


            // Build Pending Queue Context Menu.
            // Make sure we can process as default 
            if ( processAsDefault )
            {
              //Start Process Menu Item.
              System.Windows.Forms.MenuItem menuItemStartProcess = new MenuItem( "Save Dicom", new System.EventHandler( this.OnStartPendingJob ) );
              m_contextMenuPendingJobs.MenuItems.Add( menuItemStartProcess );
            } // END ... If we can processs as default

            if ( m_pendingQueueListView.SelectedItems.Count == 1 )
            {
              //Start Processing Menu Item.
              System.Windows.Forms.MenuItem menuItemStartProcessAs = new MenuItem( "Start Processing As...", new System.EventHandler( this.OnStartPendingJobWithParams ) );
              m_contextMenuPendingJobs.MenuItems.Add( menuItemStartProcessAs );
            }
            //Menu Seperator.
            m_contextMenuPendingJobs.MenuItems.Add( "-" );

            // Save Dicom.
            if (Convert.ToBoolean(ConfigurationManager.AppSettings["ShowDicomSave"]))
            {
              m_contextMenuPendingJobs.MenuItems.Add(new MenuItem("Save Dicom", new System.EventHandler(OnSaveDicomClickHandler)));
            }

            //Delete Menu Item.
            System.Windows.Forms.MenuItem menuItemQueueDelete = new MenuItem( "Delete", new System.EventHandler( this.OnDeletePendingJob ) );
            m_contextMenuPendingJobs.MenuItems.Add( menuItemQueueDelete );

            System.Drawing.Point pos = new System.Drawing.Point( args.X, args.Y );

            //stop the timer
            m_contextMenuPendingJobs.Show( m_pendingQueueListView, pos );
          }
          finally
          {
            m_pendingJobsTimer.Start( );
          }
        }
      }
   
    }//OnPendingQueueStatusMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)

    
    /// <summary>
    /// OnJobProperties, Show the CommandLine Property Grid for Selecting CommandLine
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnStartPendingJobWithParams(object sender,EventArgs args)
    {
      StartProcessingWithParams();
    }//OnStartPendingJobWithParams(object sender,EventArgs args)


    /// <summary>
    /// Click event for delete menuitem
    /// </summary>
    /// <param name="sender">source of event</param>
    /// <param name="e">event data</param>
    private void OnStartPendingJob(object sender, EventArgs args)
    {
      StartDefaultProcessing();
    } // OnStartPendingJob(object sender, EventArgs args)


    /// <summary>
    /// Saves the dicom.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSaveDicomClickHandler(object sender, EventArgs args)
    {
      // Stops the timer
      m_pendingJobsTimer.Stop();

      try
      {
        foreach (ListViewItem pendingJob in m_pendingQueueListView.SelectedItems)
        {
          Job job = (Job)pendingJob.Tag;
          Global.DataProvider.Delete( job);
        }
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to save dicom : {0}", e.Message), "ViewStatusProcessor", "OnSaveDicomClickHandler");
      }
      finally
      {
        m_pendingJobsTimer.RunSynchronously();
      }

    } // OnSaveDicomClickHandler( sender, args )


    /// <summary>
    /// Handles the Pending Queue Double Click event.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnPendingQueueDoubleClick(object sender, System.EventArgs args)
    {
      /////////////////////////////////////////////////////////////////////
      // If there are no dicom server then we cannot start the pending job
      ///////////////////////////////////////////////////////////////////////
      if(  m_dicomServerTimer.List.Count > 0 )
      {
        StartProcessingWithParams();
      }

    }//OnPendingQueueDoubleClick(object sender, System.EventArgs args)


    /// <summary>
    /// Click event for delete pending job menuitem
    /// </summary>
    /// <param name="sender">source of event</param>
    /// <param name="e">event data</param>
    private void OnDeletePendingJob( object sender, EventArgs args )
    {

      m_pendingJobsTimer.Stop();

      try
      {
        if (Viatronix.UI.MessageBox.ShowQuestion(Global.Frame, "Do you wish to delete the selected item(s) from the pending queue? "))
        {
          //Loop through all the selected rows.
          foreach (ListViewItem item in m_pendingQueueListView.SelectedItems)
          {

            Job job = (Job)item.Tag;

            Global.DataProvider.Delete( job);

          }

         }
      }
      catch (Exception ex)
      {
        Viatronix.UI.MessageBox.ShowError(ex);
      }
      finally
      {
        //Refresh the Pending Jobs Grid
        m_pendingJobsTimer.RunSynchronously();
      }
    } // OnDeletePendingJob(object sender, EventArgs args)


    /// <summary>
    /// OnpendingQueueKeyDownHandler
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">KeyEventArgs</param>
    private void OnPendingQueueKeyDownHandler( object sender, KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Delete && m_pendingQueueListView.SelectedItems.Count > 0 )
        OnDeletePendingJob( sender, EventArgs.Empty );
    }//OnpendingQueueKeyDownHandler( object sender, KeyEventArgs args )

    #endregion

    #region PendingQueue Status Methods

    /// <summary>
    /// Refreshes the pending jobs list
    /// </summary>
    /// <param name="list"></param>
    private void RefreshPendingJobs(ref List<Job> list)
    {

      try
      {
        list = Global.DataProvider.GetPendingJobs();

      }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to refresh the pending jobs : {0}", e.Message), "ViewStatusProcessor", "RefreshPendingJobs");
      }
    }
    /// <summary>
    /// Updates the pending jobs on the GUI
    /// </summary>
    private void UpdatePendingJobs(object sender, EventArgs args)
    {

      m_pendingQueueListView.BeginUpdate();

      try
      {


        m_pendingQueueListView.Items.Clear();

        foreach( Job job in m_pendingJobsTimer.List)
        {
          ListViewItem item = new ListViewItem(job.State.ToString());
          item.SubItems.Add(job.Application);
          item.SubItems.Add(job.Task.Name);
          item.SubItems.Add(job.PatientName);
          item.SubItems.Add(job.ImageCount.ToString());
          item.SubItems.Add(job.Orientation);
          item.SubItems.Add(job.Modality);
          item.SubItems.Add(job.DateCreated.ToString());

          item.Tag = job;
 
          m_pendingQueueListView.Items.Add(item);

        }

       
      }
      catch( Exception ex )
      {
        //Viatronix.UI.MessageBox.ShowError("GetPendingJobs, " + ex.Message);
        Log.Error(string.Format("Failed to update the pending jobs : {0}", ex.Message), "ViewStatusProcessor", "UpdatePendingJobs");

      }
      finally
      {
        //m_pendingJobsTimer.Start();
        m_pendingQueueListView.EndUpdate();

      }
    } //  GetPendingJobs()

    /// <summary>
    /// Determines if any selected jobs are pending
    /// </summary>
    /// <returns></returns>
    private bool IsAnySelectedJobsPending()
    {
      foreach ( ListViewItem item in m_pendingQueueListView.SelectedItems )
      {
        Job job = (Job)item.Tag;
        /// If any of the rows are pending do not allow processing to continue
        if ( job.State == States.Pending ) return true;
      }
      return false;
    } // IsAnySelectedJobsPending()

    /// <summary>
    /// StartDefaultProcessing
    /// </summary>
    private void StartDefaultProcessing()
    {

      if ( IsAnySelectedJobsPending() ) return;

      m_pendingJobsTimer.Stop();
     

      try
      {
        Cursor = Cursors.WaitCursor;
        //Loop through all the selected rows.
        foreach (ListViewItem item in m_pendingQueueListView.SelectedItems )
        {

         
          Job job = (Job)item.Tag;

          Global.DataProvider.Start(job);

          Viatronix.Logging.Log.Information("Dispatched for Processing " + job.PatientName, "ViewStatusProcessor", "StartDefaultProcessing");
        }
      }
      catch(Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to start pending job : " + ex.Message, "ViewStatusProcessor", "StartDefaultProcessing");
        Viatronix.UI.MessageBox.ShowError("Failed to start pending job. Contact Viatronix Customer Service");
      }
      finally
      {
        Cursor = Cursors.Default;
        RefreshAllViews();
        m_pendingJobsTimer.Start();
      }
    }//StartDefaultProcessing


    /// <summary>
    /// StartProcessingWithParams
    /// </summary>
    private void StartProcessingWithParams()
    {

      if ( IsAnySelectedJobsPending() ) return;

      m_pendingJobsTimer.Stop();

      try
      {
        //Loop through all the selected rows.
        foreach ( ListViewItem item in m_pendingQueueListView.SelectedItems )
        {
          Job job = (Job)item.Tag;


          ProcessAsDialog processAsDialog = new ProcessAsDialog( job);

  
          if( DialogResult.OK != processAsDialog.ShowDialog() )
            return;

          job.TemplateId = processAsDialog.Template.Id;
          Global.DataProvider.Start(job);
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed to start  job : " + ex.Message, "ViewStatusProcessor", "StartProcessingWithParams");
        Viatronix.UI.MessageBox.ShowError("Unexpected error occurred. Please contact Viatronix Customer Service");
      }
      finally
      {
        m_pendingJobsTimer.Start();
      }
    }//StartProcessingWithParams


    #endregion


    #region Status Message 

    /// <summary>
    /// Handles the Status Message Timer click event.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void RefreshStatusMessages( ref List<Viatronix.Enterprise.Entities.Message> list )
    {
      try
      {
        Viatronix.Logging.Log.Debug("Refreshing the Messages Status", "ViewStatusProcessor", "RefreshStatusMessages");

        list = Global.DataProvider.GetMessages( Enterprise.XmlBuilder.Create("message"));
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to refresh the status message : {0}", e.Message), "ViewStatusProcessor", "RefreshStatusMessages");
      }

    }//OnRefreshStatusMessageTimerTickHandler( object sender, EventArgs args )


    /// <summary>
    /// GetStatusMessages,Get the status messages from the database.
    /// </summary>
    private void  UpdateStatusMessages(object sender, EventArgs args)
    {
      m_messageListView.BeginUpdate();

      try
      {

        m_messageListView.ClearObjects();

        m_messageListView.AddObjects(m_messageTimer.List);
 
       }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to update the status messages : {0}", e.Message), "ViewStatusProcessor", "UpdateStatusMessages");
      }
      finally
      {
        //m_messageTimer.Start();
        m_messageListView.EndUpdate();
      }

    }//GetStatusMessages()


    /// <summary>
    /// Prepare Menu for StatusMessagesGrid,Handles MouseUP event.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnErrorMessagesMouseUp( object sender, System.Windows.Forms.MouseEventArgs args )
    {
      if (m_messageTimer.List.Count == 0)
        return;
     
      if( args.Button == MouseButtons.Right )
      {
        m_messageTimer.Stop();
        m_contextMenuMessageStatus.MenuItems.Clear( );
        System.Windows.Forms.MenuItem menuItemDelete          = new MenuItem( "Delete",new System.EventHandler(this.OnDeleteErrorStatus) );
        System.Windows.Forms.MenuItem menuItemDeleteAll       = new MenuItem( "Delete All",new System.EventHandler(this.OnDeleteAllErrorStatus ));
        
        //Build the Context Menu
        m_contextMenuMessageStatus.MenuItems.AddRange( new System.Windows.Forms.MenuItem [] { menuItemDelete, menuItemDeleteAll } );
        m_contextMenuMessageStatus.Show( m_messageListView, new System.Drawing.Point( args.X, args.Y ) );
        m_messageTimer.Start();
      }//if(e.Button == MouseButtons.Right)
    }//OnErrorMessagesMouseUp(object sender, System.Windows.Forms.MouseEventArgs args )


    /// <summary>
    /// OnDeleteErrorStatus, Handles Delete MenuClick
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDeleteErrorStatus( object sender, EventArgs args )
    {

      m_messageTimer.Stop();

      try
      {
        //delete the selected rows
        foreach (Enterprise.Entities.Message message in m_messageListView.SelectedObjects)
        {
          Global.DataProvider.DeleteMessages( Enterprise.XmlBuilder.Create("message", (new Dictionary<string, string>() { { "id", message.Id.ToString() } })));
        }

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to delete the selected item." + ex.Message, "ViewStatusProcessor", "OnDeleteErrorStatus");
        Viatronix.UI.MessageBox.ShowError("Failed to delete the selected item.");
      }
      finally
      {
        //Refresh the Messages Grid
        m_messageTimer.RunSynchronously();
      }
     } // OnDeleteErrorStatus


    /// <summary>
    /// OnDeleteAllErrorStatus, Handles DeleteAll menu click
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnDeleteAllErrorStatus( object sender, EventArgs args )
    {

      m_messageTimer.Stop();

      try
      {
        Global.DataProvider.DeleteMessages( Enterprise.XmlBuilder.Create("message"));

       }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to delete the selected item(s)." + ex.Message, "ViewStatusProcessor", "OnDeleteAllErrorStatus");
        Viatronix.UI.MessageBox.ShowError("Failed to delete the selected item(s).");
      }
      finally
      {
        //Refresh the Messages Grid
        m_messageTimer.RunSynchronously();
      }
     }//OnDeleteAllErrorStatus

  
    #endregion


 
    #endregion

    private void OnDicomStatusMouseMove(object sender, MouseEventArgs e)
    {

      ListViewHitTestInfo info = m_dicomServerListView.HitTest(e.X, e.Y);

      if (info.Item != null)
      {
        Receiver receiver = (Receiver)info.Item.Tag;
        try
        {
          string toolTip = "AETitle = " + receiver.AETitle;

          string oldToolTip = m_toolTip.GetToolTip(m_dicomServerListView);
          if (toolTip != oldToolTip)
            m_toolTip.SetToolTip(m_dicomServerListView, toolTip);
        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Error("Failed to set the tool tip for the dicom server : " + ex.Message, "ViewStatusProcessor", "OnDicomStatusMouseMove");
        }

      }
      else
      {
        m_toolTip.SetToolTip(m_dicomServerListView, "");
      }
    }

    private void OnDicomStatusMouseEnter(object sender, EventArgs e)
    {
      m_toolTip.AutoPopDelay = 5000;
      m_toolTip.InitialDelay = 1000;
      m_toolTip.ReshowDelay = 5000;

    }

    private void OnDicomStatusMouseLeave(object sender, EventArgs e)
    {
      m_toolTip.AutoPopDelay = 5000;
      m_toolTip.InitialDelay = 500;
      m_toolTip.ReshowDelay = 100;


    }



    
  } // class ViewStatusProcessor

  #endregion

}//namespace Viatronix.Console


#region revision history

// $Log: ViewStatusProcessor.cs,v $
// Revision 1.5.2.3  2011/05/18 01:57:04  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.2  2010/03/23 12:25:45  kchalupa
// Issue: CONS-4
// Changed Start Processing to Save Dicom.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.1  2008/01/02 20:33:31  mkontak
// Password hints
//
// Revision 1.5  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.4  2007/02/26 18:47:46  gdavidson
// Issue #5404: Modified the tab order
//
// Revision 1.3  2007/02/21 21:57:32  romy
// fixed tab order
//
// Revision 1.2  2006/10/30 16:18:34  gdavidson
// Impliments Viatronix.UI.IPersistable
//
// Revision 1.1  2006/10/11 13:13:59  vxconfig
// modified filter name
//
// Revision 1.50  2006/10/06 06:16:04  mkontak
// Change for viewing status
//
// Revision 1.49  2006/08/21 12:50:39  mkontak
// Issue 4948: Processing volume.dicom series. Fixes the deletion of
// the preprocess parameters set from the console.
//
// Revision 1.48  2006/06/28 15:52:42  romy
// temp fix for the Refresh crash
//
// Revision 1.47  2006/06/28 14:08:24  romy
// temp fix for the Refresh crash
//
// Revision 1.46  2006/06/07 19:25:32  mkontak
// Status panes were not being updated due to timer issue, Fixed timer issue to be
// more singular and not call the ResetTimers() method.
//
// Revision 1.45  2006/06/07 19:20:48  mkontak
// Status panes were not being updated due to timer issue, Fixed timer issue to be
// more singular and not call the ResetTimers() method.
//
// Revision 1.44  2006/06/06 19:48:54  mkontak
// changed deprecated code
//
// Revision 1.43  2006/05/04 02:30:01  mkontak
// Changed comment to indicate a jobid being passed to the dicom server
//
// Revision 1.42  2006/04/21 15:07:07  mkontak
// Fix problem with double click on pending jobs
//
// Revision 1.41  2006/04/19 14:37:04  mkontak
// Minor changes
//
// Revision 1.40  2006/03/24 18:34:26  mkontak
// Use the ProcessAsDialog
//
// Revision 1.39  2006/03/09 15:29:42  romy
// log messages priority decreaed for refreshes
//
// Revision 1.38  2006/02/17 19:21:06  romy
// row height adjusted
//
// Revision 1.37  2006/02/17 15:41:52  romy
// changed the Parameter file location
//
// Revision 1.36  2006/02/16 17:37:24  gdavidson
// Fixed a bug in the ListView's SmallImageList property.
//
// Revision 1.35  2006/02/16 13:00:40  mkontak
// no message
//
// Revision 1.34  2006/02/15 20:00:21  gdavidson
// Modified the images for the status message listview
//
// Revision 1.33  2006/02/09 21:46:39  romy
// Bug fixes
//
// Revision 1.32  2006/02/07 15:05:46  romy
// keeps the size in cache
//
// Revision 1.31  2006/02/06 19:21:59  romy
// fixed font issue
//
// Revision 1.30  2006/02/02 15:52:52  mkontak
// Fixed WQ url problem and cancel of job
//
// Revision 1.29  2006/02/01 21:42:21  romy
// spell correction
//
// Revision 1.28  2006/02/01 20:02:50  mkontak
// Fix issue with manual start not passing the correct subtype override
//
// Revision 1.27  2006/01/27 21:08:43  romy
// added Icons for messages window
//
// Revision 1.26  2006/01/25 20:15:16  romy
// bug fix with Selection problem
//
// Revision 1.25  2006/01/25 16:17:09  romy
// bug fixes
//
// Revision 1.24  2006/01/24 21:32:06  romy
// minor menu fix
//
// Revision 1.23  2006/01/23 19:44:37  romy
// preprocessId added
//
// Revision 1.22  2006/01/20 04:57:19  romy
// minor fix in error message
//
// Revision 1.21  2006/01/19 22:23:18  romy
// new additions
//
// Revision 1.20  2006/01/09 18:28:03  romy
// intermediate code for Process with Params
//
// Revision 1.19  2005/12/30 17:17:44  mkontak
// New DB changes
//
// Revision 1.18  2005/12/14 15:08:43  romy
// Re-arranged
//
// Revision 1.17  2005/12/13 14:31:50  romy
// added application state caching
//
// Revision 1.16  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.15  2005/12/02 02:24:27  romy
// coding standards
//
// Revision 1.14  2005/11/23 11:53:03  mkontak
// Using new MessageBox implementation
//
// Revision 1.13  2005/11/17 19:20:07  mkontak
// Fix localhost issue
//
// Revision 1.12  2005/11/15 16:56:38  romy
// added code to keep the selection of list view rows
//
// Revision 1.11  2005/11/09 18:19:27  romy
// removed temp code
//
// Revision 1.10  2005/11/09 16:56:07  romy
// Temp version with COmmandline params
//
// Revision 1.9  2005/11/02 20:24:00  romy
// MOVED time taking server methods to async
//
// Revision 1.8  2005/10/29 02:36:28  romy
// theme added
//
// Revision 1.7  2005/10/11 14:20:21  romy
// Added SendDicom app
//
// Revision 1.6  2005/10/06 17:28:04  romy
// Added System.Windows.Forms.Timer namespace because of clash with VizDLL
//
// Revision 1.5  2005/10/05 15:12:06  romy
// removed debug message boxes
//
// Revision 1.4  2005/09/07 16:37:18  mkontak
// Minor mods
//
// Revision 1.3  2005/08/17 17:54:53  mkontak
// Fixes in the way the processing parameters are handled
//
// Revision 1.2  2005/08/09 14:34:10  romy
// More resets added.
//
// Revision 1.1  2005/08/05 14:20:01  romy
// Rearranged Files
//
// Revision 1.31  2005/08/03 19:00:20  romy
// Added the Timer Pause Mechanism
//
// Revision 1.30  2005/06/28 14:03:10  romy
// Added BuildImage Column method to display Icons in status messages
//
// Revision 1.29  2005/06/15 20:27:40  romy
// Changed the Context Menu creation Logic for Pending Queue Status.
//
// Revision 1.28  2005/06/15 12:15:19  romy
// Fixed the issue with Patient Name field. Changed that to SeriesUID.
//
// Revision 1.27  2005/06/14 15:55:04  romy
// Added Properties menu to the Dicom Status Viewer.
//
// Revision 1.26  2005/06/13 20:04:09  romy
// System checks the url for machine name.
//
// Revision 1.25  2005/06/08 17:14:57  romy
// Code Cleaning
//
// Revision 1.24  2005/06/08 13:02:43  romy
// Removed test Code.
//
// Revision 1.23  2005/06/07 12:32:12  romy
// Modified based on the new Database methods.
//
// Revision 1.22  2005/06/03 20:38:58  romy
// Added Item Selection
//
// Revision 1.21  2005/06/03 19:37:37  romy
// Added the column settings.
//
// Revision 1.20  2005/06/03 19:16:01  romy
// Added the column settings.
//
// Revision 1.19  2005/06/03 18:46:05  romy
// Added Dynamic Columns for the Processor Status
//
// Revision 1.17  2005/06/03 14:49:41  gdavidson
// Modified for Dynamic column header
//
// Revision 1.16  2005/06/03 13:15:32  romy
// Added Refresh Rates
//
// Revision 1.15  2005/06/03 13:13:34  romy
// Added Refresh Rates
//
// Revision 1.14  2005/06/03 13:06:58  romy
// added different refresh rates
//
// Revision 1.13  2005/06/01 18:48:38  romy
// Added Job Start with Commandline Parameters
//
// Revision 1.12  2005/05/27 14:15:34  romy
// Fixed the zero row selecetd issue
//
// Revision 1.11  2005/05/19 14:42:37  mkontak
// Fixed peniding jobs and message updating
//
// Revision 1.10  2005/05/05 11:47:20  romy
// Reading Connection string from the Global Area.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/StatusViewerProcessor/ViewStatusProcessor.cs,v 1.5.2.3 2011/05/18 01:57:04 mkontak Exp $
// $Id: ViewStatusProcessor.cs,v 1.5.2.3 2011/05/18 01:57:04 mkontak Exp $

#endregion 

