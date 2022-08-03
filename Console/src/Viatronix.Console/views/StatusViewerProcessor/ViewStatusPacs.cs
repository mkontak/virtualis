// $Id: ViewStatusPacs.cs,v 1.15.2.2 2008/01/02 20:33:31 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Collections.Specialized;

using System.Configuration;
using Viatronix.UI;
using System.IO;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
  /// <summary>
  /// ViewStatusProcessor, Control which shows the Processor Status.
  /// </summary>
  public class ViewStatusPacs : UserControl, IView
  {

    /// <summary>
    /// Delegate Declaration for GetServerStatus method, uses for Asynchronous call
    /// </summary>
    public delegate void PacsQueueStatusDelegate(ref List<PacsJob> statusDataset);


    #region fields

    /// <summary>
    /// components, System Generated Default parameter
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    private Viatronix.UI.Panel m_pacsQueuePanel;

    private ListView m_pacsQueueListView;

    private System.Windows.Forms.Label m_pacsQueueStatus;
    
    /// <summary>
    /// Maintains the path to the user's pacs queue configuration file.
    /// </summary>
    private readonly string m_pacsQueueUserPath = string.Empty;

    /// <summary>
    /// m_pacsQueueStatusDataset, Dataset which holds the pacs queue Status
    /// </summary>
    private List<PacsJob> m_pacsQueue = new List<PacsJob>();

    /// <summary>
    /// Callback for Pacs Queue Server Status Asynchronous call
    /// </summary>
    private AsyncCallback m_pacsQueueCallBack = null;

    /// <summary>
    /// Holds the pacs queue urls
    /// </summary>
    private System.Collections.Specialized.StringCollection m_pacsQueueServerUrls = new StringCollection();

    /// <summary>
    /// m_statusDicomDelgate, Delegate declartion for pacs queue server status 
    /// </summary>
    private PacsQueueStatusDelegate m_pacsQueueStatusDelgate = null;


    /// <summary>
    /// m_refreshTimer, Timer object which refreshes the Processor Status View.
    /// </summary>
    private System.Windows.Forms.Timer m_pacsQueueRefreshTimer = new System.Windows.Forms.Timer();

    /// <summary>
    /// Eventhandler to update the PACS QUEUE status
    /// </summary>
    private EventHandler m_updatePacsQueueStatus = null;

    /// <summary>
    /// Event handler for the closing event.
    /// </summary>
    private CancelEventHandler m_closingHandler = null;

    /// <summary>
    /// Event handler for the view changing event.
    /// </summary>
    private CancelEventHandler m_viewChangingHandler = null;

 

    Viatronix.UI.ContextMenu m_contextMenuPacsQueue = new Viatronix.UI.ContextMenu();

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
        //Load the Server Service Config Values from the Appconfig file.
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

       
        //fill the dicom Server Urls
        XmlNodeList pacsQueueNodes = doc.SelectNodes( "/ServerServices/PacsQueueServices/url" );
        foreach( XmlNode node in pacsQueueNodes )
        {
          m_pacsQueueServerUrls.Add(node.Attributes.GetNamedItem("address").Value.ToString());
        } // END ... For each pacs queue service specified in the config file
    
        // load the columns for the listview controls
        XmlNode columnsNode = (XmlNode) ConfigurationManager.GetSection( "dynamicColumns" );
        XmlNode listviewNode = doc.DocumentElement.SelectSingleNode( "listviews" );
        
        XmlNode statusMessageNode = doc.SelectSingleNode( "/ServerServices/PacsQueueServices" );
        m_pacsQueueRefreshTimer.Interval   = Convert.ToInt32(statusMessageNode.Attributes["RefreshRate"].Value)* 1000;


        Viatronix.Logging.Log.Debug("Processor Status Refresh rate: "+m_pacsQueueRefreshTimer.Interval.ToString(), "ViewStatusPacs", "IView.Initialize");

        m_pacsQueueRefreshTimer.Start();

        //At the first time donot wait for the timer tick event to occur. So an explicit call.
        OnRefreshPacsQueueTimerTickHandler(null,null);

        Global.Frame.ViewChanging += m_viewChangingHandler;
        Global.Frame.Closing += m_closingHandler;
      }
      catch(Exception e)
      {
        Viatronix.Logging.Log.Error("Initialization of the PACS Queue Status View failed : "+ e.Message, "ViewStatusPacs", "IView.Initialize");
      }
    } // Initialize( xml )
   
    #endregion IView

    #region initialization
    /// <summary>
    /// Initializes a ViewStatusProcessor instance with default values
    /// </summary>
    public ViewStatusPacs()
    {
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      //Create Delegates 
      m_pacsQueueStatusDelgate        = new PacsQueueStatusDelegate(GetServerStatus);

      //Create Call Backs
      m_pacsQueueCallBack             = new AsyncCallback( RefreshPacsQueueStatus );

      //Create Timer's Tick event handler
      m_pacsQueueRefreshTimer.Tick          += new EventHandler( this.OnRefreshPacsQueueTimerTickHandler );

      //Create Event Handlers
      m_updatePacsQueueStatus  =  new EventHandler( UpdatePacsQueueStatus );
      m_closingHandler         =  new CancelEventHandler( this.OnClosingHandler );
      m_viewChangingHandler = new CancelEventHandler(this.OnViewChangingHandler);

      InitializeComponent();


      
      m_pacsQueueListView.Columns.Add("Id").Width = 85;
      m_pacsQueueListView.Columns.Add("System").Width = 80;
      m_pacsQueueListView.Columns.Add("Date Queued").Width = 150;
      m_pacsQueueListView.Columns.Add("Date Started").Width = 150;
      m_pacsQueueListView.Columns.Add("Date Completed").Width = 150;
      m_pacsQueueListView.Columns.Add("Study Uid").Width = 150;
      m_pacsQueueListView.Columns.Add("Series Uid").Width = 150;;
      m_pacsQueueListView.Columns.Add("Destination AETitle").Width = 100;
      m_pacsQueueListView.Columns.Add("Progress").Width = 80;
      m_pacsQueueListView.Columns.Add("State").Width = 80;
      m_pacsQueueListView.Columns.Add("Message").Width = 300;
      
     



      m_pacsQueueStatus.Font = Theme.HeaderFont;
      this.Font      = Theme.FormFont;
      this.BackColor = Color.Transparent;

      //Create UserPreference Files.
      m_pacsQueueUserPath        = Path.Combine( Global.CurrentUser.PreferencesDirectory, "ViewStatusPacs_PacsQueue.xml" );
    } // ViewStatusProcessor()


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {

        // Dispose the dcm timer object
        if ( m_pacsQueueRefreshTimer != null )   m_pacsQueueRefreshTimer.Dispose(); 


        if( components != null )  components.Dispose();
      }
      base.Dispose( disposing );
    }


    #endregion initialization
   
    #region Methods
  
    /// <summary>
    /// GetServerStatus,Get the Server status from the Remote Object by receiving the url.
    /// </summary>
    /// <param name="url"></param>
    private void GetServerStatus(  ref List<PacsJob> statusDataset )
    {

      try
      {
        statusDataset = Global.DataProvider.GetPacsJobs();
      }
      catch(Exception e)
      {
        Viatronix.Logging.Log.Error("GetServerStatus, "+e.Message, "ViewStatusPacs", "GetServerStatus");
      }
    }//GetServerStatus



    /// <summary>
    /// Reset All Timers
    /// </summary>
    private void ResetTimers()
    {
      //Stop and Start all timers. Otherwise it will loose synchronization.
      m_pacsQueueRefreshTimer.Stop(); 

      m_pacsQueueRefreshTimer.Start(); 
    }// ResetTimers()

  
   
    #region PACS Queue Status Methods

    /// <summary>
    /// OnRefreshTimerTickHandler, gets the status from the Server Interface.
    /// This calls the GetServerStatus Method Asynchronously. 
    /// Based on the Refresh Interval which specifies in the Config file this eventhandler gets executes.
    /// </summary>
    private void OnRefreshPacsQueueTimerTickHandler( object sender, EventArgs args )
    {
      try
      {
        //Stop the DCM Timer before the Asynchronous call.And start it after the callback
        m_pacsQueueRefreshTimer.Stop();
     
        //Call this way Else it won't refresh the GUI even if one of the servers are down.
        foreach( string url in m_pacsQueueServerUrls )
        {
          m_pacsQueueStatusDelgate.BeginInvoke( ref  m_pacsQueue, m_pacsQueueCallBack, null );
        }
      }
      catch(Exception e)
      {
        Viatronix.Logging.Log.Error( "Error refreshing the pacs queue timer : " + e.Message, "ViewStatusPacs", "OnRefreshPacsQueueTimerTickHandler");
      }
      finally
      {
        ResetTimers();
      }
    }//OnRefreshDicomTimerTickHandler(object sender, EventArgs args)


    /// <summary>
    /// Callback method which trigers the UpdatePacsQueueStatus event
    /// </summary>
    /// <param name="ar"></param>
    public void RefreshPacsQueueStatus( IAsyncResult ar )
    {
      try
      {
        if( ar.IsCompleted )
        {
          //Ends the Asynchronous Call.
          m_pacsQueueStatusDelgate.EndInvoke( ref m_pacsQueue, ar );

          // Invoke the delegate on the UI thread.
          if( m_pacsQueueListView != null )
            Global.Frame.Invoke( m_updatePacsQueueStatus );
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("Error refreshing the pacs queue status : "+ ex.Message, "ViewStatusPacs", "RefreshPacsQueueStatus");
      }

    } // RefreshPacsQueueStatus( IAsyncResult ar )


    /// <summary>
    /// Event which updates the  m_pacsQueueListView Grid.
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">EventArgs</param>
    private void UpdatePacsQueueStatus( object sender, EventArgs args )
    {
      try
      {

        m_pacsQueueListView.Items.Clear();

        foreach (PacsJob job in m_pacsQueue)
        {
          ListViewItem item = new ListViewItem(job.Id.ToString());

          item.SubItems.Add(job.Operation.ToString());
          item.SubItems.Add(job.Host);
          item.SubItems.Add(job.DateQueued.ToString());
          item.SubItems.Add(job.DateStarted.ToString());
          item.SubItems.Add(job.DateCompleted.ToString());
          item.SubItems.Add(job.StudyUid);
          item.SubItems.Add(job.SeriesUid);
          item.SubItems.Add(job.DestinationAETitle);
          item.SubItems.Add(job.Progress.ToString());
          item.SubItems.Add(job.State.ToString());
          item.SubItems.Add(job.Message);




        }

   

        //(Re)Start the Timer 
        if( m_pacsQueueRefreshTimer.Enabled == false )
          m_pacsQueueRefreshTimer.Start();
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error( "Error updating the pacs queue status : " + ex.Message, "ViewStatusPacs", "UpdateDicomStatus");
      }
    } // UpdateDicomStatus( object sender, EventArgs args )
    

    #endregion PACS Queue Status Methods
    
    

    #endregion Methods.


    #region event handlers


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
    } // OnViewChangingHandler( sender, args )

    #endregion

    #region Component Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_pacsQueuePanel = new Viatronix.UI.Panel();
      this.m_pacsQueueListView = new Viatronix.UI.DynamicListView();
      this.m_pacsQueueStatus = new System.Windows.Forms.Label();
      this.m_pacsQueuePanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_pacsQueuePanel
      // 
      this.m_pacsQueuePanel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(148)), ((System.Byte)(166)), ((System.Byte)(198)));
      this.m_pacsQueuePanel.BorderColor = System.Drawing.Color.White;
      this.m_pacsQueuePanel.BottomPadding = 0;
      this.m_pacsQueuePanel.Controls.Add(this.m_pacsQueueListView);
      this.m_pacsQueuePanel.Controls.Add(this.m_pacsQueueStatus);
      this.m_pacsQueuePanel.Curve = 10;
      this.m_pacsQueuePanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_pacsQueuePanel.Edges = Viatronix.UI.Edges.All;
      this.m_pacsQueuePanel.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(123)), ((System.Byte)(134)), ((System.Byte)(165)));
      this.m_pacsQueuePanel.LeftPadding = 0;
      this.m_pacsQueuePanel.Location = new System.Drawing.Point(0, 0);
      this.m_pacsQueuePanel.Name = "m_pacsQueuePanel";
      this.m_pacsQueuePanel.RightPadding = 0;
      this.m_pacsQueuePanel.Size = new System.Drawing.Size(896, 696);
      this.m_pacsQueuePanel.TabIndex = 9;
      this.m_pacsQueuePanel.TopPadding = 0;
      // 
      // m_pacsQueueListView
      // 
      this.m_pacsQueueListView.AllowColumnReorder = true;
      this.m_pacsQueueListView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_pacsQueueListView.ShowGroups = false;
      this.m_pacsQueueListView.FullRowSelect = true;
      this.m_pacsQueueListView.GridLines = true;
      this.m_pacsQueueListView.Location = new System.Drawing.Point(8, 37);
      this.m_pacsQueueListView.Name = "m_pacsQueueListView";
      this.m_pacsQueueListView.Size = new System.Drawing.Size(880, 651);
      this.m_pacsQueueListView.TabIndex = 4;
      this.m_pacsQueueListView.View = System.Windows.Forms.View.Details;
      this.m_pacsQueueListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnPacsQueueMouseUp);
      // 
      // m_pacsQueueStatus
      // 
      this.m_pacsQueueStatus.BackColor = System.Drawing.Color.Transparent;
      this.m_pacsQueueStatus.ForeColor = System.Drawing.Color.White;
      this.m_pacsQueueStatus.Location = new System.Drawing.Point(8, 8);
      this.m_pacsQueueStatus.Name = "m_pacsQueueStatus";
      this.m_pacsQueueStatus.Size = new System.Drawing.Size(264, 24);
      this.m_pacsQueueStatus.TabIndex = 3;
      this.m_pacsQueueStatus.Text = "PACS Queue Activity Monitor";
      this.m_pacsQueueStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // ViewStatusPacs
      // 
      this.Controls.Add(this.m_pacsQueuePanel);
      this.Name = "ViewStatusPacs";
      this.Size = new System.Drawing.Size(896, 696);
      this.m_pacsQueuePanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
    #endregion

    private void OnPacsQueueMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if ( e.Button == MouseButtons.Right && m_pacsQueueListView.SelectedItems.Count > 0 )
      {
        //Build Pending Queue Context Menu.

        //Delete Menu Item.
        m_contextMenuPacsQueue.MenuItems.Clear( );

        System.Windows.Forms.MenuItem menuItemQueueDelete = new MenuItem( "Delete", new System.EventHandler(this.OnDeletePacsQueueItem) );
        m_contextMenuPacsQueue.MenuItems.Add(menuItemQueueDelete);

        System.Drawing.Point pos = new System.Drawing.Point( e.X, e.Y );

        // Show context menu
        m_contextMenuPacsQueue.Show( m_pacsQueueListView, pos );


      } // 

    }

    /// <summary>
    /// Click event for delete pending job menuitem
    /// </summary>
    /// <param name="sender">source of event</param>
    /// <param name="e">event data</param>
    private void OnDeletePacsQueueItem( object sender, EventArgs args )
    {
      try
      {
        if( Viatronix.UI.MessageBox.ShowQuestion(Global.Frame, "Do you wish to delete the selected item(s) from the PACS queue? ") )
        {

          this.m_pacsQueueRefreshTimer.Stop();

          //Loop through all the selected items.
          foreach ( ListViewItem item in m_pacsQueueListView.SelectedItems )
          {
            PacsJob job = (PacsJob)item.Tag;

            Global.DataProvider.Delete( job);

            Viatronix.Logging.Log.Information("Deleting pacs queue job from queue [HOST=" +  job.Host + "] [OP=" + job.Operation.ToString()  + "]", "ViewStatusPacs", "OnDeletePacsQueueItem"); //+ "] [UID=" + seriesUid + "]" );
          }


          this.m_pacsQueueRefreshTimer.Start();

          

        }
      }
      catch(Exception ex)
      {
        Viatronix.UI.MessageBox.ShowError( ex );
      }
    } // OnDeletePendingJob(object sender, EventArgs args)

  }
}


#region revision history

// $Log: ViewStatusPacs.cs,v $
// Revision 1.15.2.2  2008/01/02 20:33:31  mkontak
// Password hints
//
// Revision 1.15.2.1  2007/05/11 19:39:10  gdavidson
// Issue #5650: Assign handler to ViewChanging event
//
// Revision 1.15  2007/02/15 16:22:05  mkontak
// Changed the hard caded type to a referenced type for the IService
//
// Revision 1.14  2006/10/30 16:20:03  gdavidson
// Removed CacheViewState and RetrieveViewState
//
// Revision 1.13  2006/06/30 14:13:21  romy
// no message
//
// Revision 1.12  2006/06/29 15:28:58  romy
// fixed the menu problem
//
// Revision 1.11  2006/06/28 14:08:30  romy
// temp fix for the Refresh crash
//
// Revision 1.10  2006/06/06 19:48:40  mkontak
// changed deprecated code
//
// Revision 1.9  2006/05/15 14:16:27  mkontak
// Issue 4799
//
// Revision 1.8  2006/02/16 13:00:52  mkontak
// Added Delete to PACS Queue
//
// Revision 1.7  2006/02/06 19:22:07  romy
// fixed font issue
//
// Revision 1.6  2006/02/01 21:45:07  romy
// fixed Font issue
//
// Revision 1.5  2005/12/13 14:31:50  romy
// added application state caching
//
// Revision 1.4  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.3  2005/10/06 17:27:56  romy
// Added System.Windows.Forms.Timer namespace because of clash with VizDLL
//
// Revision 1.2  2005/09/07 16:37:34  mkontak
// Implemented PACS QUEUE
//
// Revision 1.1  2005/08/05 14:23:27  romy
// Rearranged Files
//
// Revision 1.1  2005/06/03 13:07:23  romy
// added PACS status View
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/StatusViewerPACS/ViewStatusPacs.cs,v 1.15.2.2 2008/01/02 20:33:31 mkontak Exp $
// $Id: ViewStatusPacs.cs,v 1.15.2.2 2008/01/02 20:33:31 mkontak Exp $

#endregion 

