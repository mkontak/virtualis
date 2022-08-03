// $Id: LaunchManager.cs,v 1.7.2.6 2011/05/18 01:57:00 mkontak Exp $
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
using System.ComponentModel;
using System.Configuration;
using System.Collections.Generic;
using System.Xml;
using System.Windows.Forms;

using Viatronix.UI;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
  /// <summary>
  /// Implements functionality for all launch operations.
  /// </summary>
  public sealed class LaunchManager
  {

    #region events

    /// <summary>
    /// Notifies that a viewer has launched.
    /// </summary>
    public static event EventHandler ViewerLaunched;

    /// <summary>
    /// Notifies that the viewer was closed.
    /// </summary>
    public static event EventHandler ViewerClosed;

    #endregion

    #region delegates

    ///// <summary>
    ///// Defines the launch method for a report viewer.
    ///// </summary>
    //private delegate void LaunchReportDelegate( Series series, int page );

    ///// <summary>
    ///// Defiens the method for displaying a message using a message box.
    ///// </summary>
    //private delegate void MessageBoxDelegate( string message );

    #endregion

    #region fields

    /// <summary>
    /// ID for the viewer tab
    /// </summary>
    public static string ViewerTab = "76C21181-0420-473b-B7F9-E61D8A69DEB7";

    /// <summary>
    /// Collection of launch rules.
    /// </summary>
    private static List<ILaunchRule> m_rules = null;

    /// <summary>
    /// Reference to an IDataProvider.
    /// </summary>
    private IDataProvider m_dataProvider = null;

    /// <summary>
    /// EventHandler for the IViewerController's closed event.
    /// </summary>
    private EventHandler m_closedHandler = null;

    /// <summary>
    /// EventHandler for the IConsole's saved event.
    /// </summary>
    private EventHandler m_seriesSavedHandler = null;

    /// <summary>
    /// determines if the launch manager is enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// Reference to the parent form.
    /// </summary>
    private Viatronix.UI.Frame m_frame = null;

    /// <summary>
    /// Reference to the active IViewerLauncher.
    /// </summary>
    private IViewerLauncher m_activeLauncher = null;

    /// <summary>
    /// User
    /// </summary>
    private User m_user = null;

    #endregion

    #region contruction

    /// <summary>
    /// default constructor
    /// </summary>
    public LaunchManager( Viatronix.UI.Frame frame, IDataProvider provider, User user )
    {
      m_dataProvider = provider;
      m_user      = user;
      this.Frame = frame;

      m_closedHandler       = new EventHandler( this.OnControllerClosedHandler );
      m_seriesSavedHandler  = new EventHandler( this.OnSeriesSavedHandler );

      // check if a viewer is already running
      object runningLauncher = null;
      if( Global.ApplicationState.TryGetValue( ConsoleState.LaunchManager, out runningLauncher ) )
      {
        // store the active viewer and attach to its events
        m_activeLauncher = (IViewerLauncher) runningLauncher;
        m_activeLauncher.Closed += m_closedHandler;
        m_activeLauncher.Updated += m_seriesSavedHandler;
      }
    } // LaunchManager()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the form
    /// </summary>
    public Viatronix.UI.Frame Frame
    {
      get { return m_frame; }
      set 
      { 
        m_frame = value;

        #if !V3D_NO_LICENSING
        GetLaunchRules(m_dataProvider.GetApplications());
        #else
        GetLaunchRules();
        #endif
      }
    } // Frame


    /// <summary>
    /// Returns a reference to the Provider.
    /// </summary>
    public IDataProvider DataProvider
    {
      get { return m_dataProvider; }
    } // DataProvider


    /// <summary>
    /// Gets or sets if the launch manager is enabled.
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    } // Enabled


    /// <summary>
    /// Gets or sets the active IViewerLauncher
    /// </summary>
    public IViewerLauncher ActiveLauncher
    {
      get { return m_activeLauncher; }
      set { m_activeLauncher = value; }
    } // ActiveLauncher 

    #endregion

    #region methods

    /// <summary>
    /// Launches a study
    /// </summary>
    /// <param name="study"></param>
    public void Launch( Study study )
    {
      IViewerLauncher launcher = GetBestPossibleViewer( study.Series );
      if( launcher != null )
        Launch( launcher );
      else
        Launch( study.Series );
    } // Launch( study )


    /// <summary>
    /// Launches a study
    /// </summary>
    /// <param name="collection"></param>
    public void Launch(List<Study> studies)
    {
      List<Series> collection = new List<Series>();
      foreach( Study study in studies )
        collection.AddRange( study.Series );

      Launch( collection );
    } // Launch( study )


    /// <summary>
    /// Laucnhs a colleciton of series objects
    /// </summary>
    /// <param name="collection">series collection</param>
    public void Launch(List<Series> collection)
    {
       LaunchViewer( collection );
    } // Launch( collection )


    /// <summary>
    /// Launches the viewer using the provided series collection.
    /// </summary>
    /// <param name="collection">series collection</param>
    private void LaunchViewer(List<Series> collection)
    {
      if( this.Enabled == false )
        return;

      // close any open viewers
      if( !CloseOpenViewer() )
        return;

      try
      {
        this.Frame.Cursor = Cursors.WaitCursor;
        
        
        // TODO: New 
        //collection.Sort(SeriesFields.on);

        // get the possible viewers that can launch the series collection
        List<IViewerLauncher> possibleViewers = GetPossibleViewers( collection );
        if( possibleViewers.Count == 0 )
        {
          Viatronix.UI.MessageBox.ShowInfo( "The selected series cannot be viewed." );
          Viatronix.Logging.Log.Debug("Unable to find a launch rule for the provided series.", "LuanchManager", "LaunchViewer");
          return;
        }
        
        //Lock the collection now.
        IViewerLauncher launcher = null;
        if( possibleViewers.Count == 1 )
          launcher = possibleViewers[0];
        else
        {
          // allow the user to select the viewer
          SelectViewerDialog dialog = new SelectViewerDialog();
          dialog.AddViewers( possibleViewers );
          if( dialog.ShowDialog( m_frame ) == DialogResult.OK )
          {
            launcher = dialog.SelectedLauncher;
            System.Windows.Forms.Application.DoEvents();
          }
          else
          {
            Viatronix.Logging.Log.Debug("User aborted the launch operation.", "LauncherManager", "LaunchViewer");
            return;
          }
        }


        foreach (List<Series> col in launcher.Datasets)
        {
          foreach (Series series in col)
          {
            m_dataProvider.GetStudy( series);
            m_dataProvider.GetReferences( series);
          }
        }

        Launch( launcher );    
      }
      finally
      {
        this.Frame.Cursor = Cursors.Default;
      }
    } // LaunchViewer( collection )


    /// <summary>
    /// Launches  a viewer using the provided IViewerController.
    /// </summary>
    /// <param name="controller">IViewerController</param>
    public void Launch( IViewerLauncher launcher )
    {
      if( this.Enabled == false )
        return;

      // close any open viewers
      if( !CloseOpenViewer() )
        return;

      try
      {
        this.Frame.Cursor = Cursors.WaitCursor;

        
        launcher.LaunchViewer();

        if (launcher.CanNavigate)
        {
          // add a viewer tab
          this.Frame.WorkflowTabs.BeginInit();

          TabStripItem item = new TabStripItem();
          item.ID = ViewerTab;
          item.Text = launcher.Text;
          item.Enabled = true;
          item.Data = launcher;
          this.Frame.WorkflowTabs.Items.Add(item);

          this.Frame.WorkflowTabs.EndInit();
        }

        launcher.Closed += m_closedHandler;
        launcher.Updated += m_seriesSavedHandler;

        this.ActiveLauncher = launcher;
        Global.ApplicationState[ConsoleState.LaunchManager] = launcher;

        // Notifies that a viewer has launched.
        if (ViewerLaunched != null)
        {
          ViewerLaunched(this, EventArgs.Empty);
        }
      }
      catch (LockedException lockEx)
      {
        Viatronix.UI.MessageBox.ShowError(m_frame, lockEx.Message);
      }
      catch (LaunchException e)
      {
        if (e.LaunchStage == LaunchStage.Cancelled)
        {
          // Don't show the cancellation message.
          //Viatronix.UI.MessageBox.ShowInfo(m_frame, e.Message);
        }
        else
        {
          string message = "Console was unable to start the viewer.";
          string additionalInfo = " [PLUGINID=" + launcher.ID + "]";

          Viatronix.Logging.Log.Error(message + additionalInfo + " [ERROR=" + e.Message + "]", "LauncherManager", "Launch");

          WindowUtilities.ShowWindow(this.Frame, this.Frame.Bounds, this.Frame.WindowState);
          Viatronix.UI.MessageBox.ShowError(this.Frame, message);

        }
      }
      catch (Exception e)
      {
        string message = "Console was unable to start the viewer.";
        string additionalInfo = " [PLUGINID=" + launcher.ID + "]";

        Viatronix.Logging.Log.Error(message + additionalInfo + " [ERROR=" + e.Message + "]", "LauncherManager", "Launch");

        WindowUtilities.ShowWindow(this.Frame, this.Frame.Bounds, this.Frame.WindowState);
        Viatronix.UI.MessageBox.ShowError(this.Frame, message);
      }
      finally
      {
        this.Frame.Cursor = Cursors.Default;
      }
    } // LaunchViewer( controller )


    /// <summary>
    /// Returns a collection of possible viewer controllers.
    /// </summary>
    /// <param name="collection">Collection of series being launched.</param>
    /// <returns>Possible viewer controllers.</returns>
    public List<IViewerLauncher> GetPossibleViewers(List<Series> collection)
    {

      DateTime start = DateTime.Now;

      try
      {
        // TODO: New
        //List<Series> col = (List<Series>)collection.Clone();
        //List<Series> col = collection;

        //foreach (Series series in col)
        //{

        //  m_dataProvider.GetReferences( series);
        //  foreach (Series referencedSeries in series.References)
        //  {
        //    m_dataProvider.GetReferences( referencedSeries);
        //    m_dataProvider.GetDependents( referencedSeries);
        //  }

        //  m_dataProvider.GetDependents( series);
        //  foreach (Series depdendentSeries in series.Dependents)
        //  {
        //    m_dataProvider.GetReferences( depdendentSeries);
        //    m_dataProvider.GetDependents( depdendentSeries);
        //  }

        //}

        // find all rules that are valid
        List<IViewerLauncher> possibleViewers = new List<IViewerLauncher>();


        List<Series> completedSeries = new List<Series>();
        foreach (Series series in collection)
        {
          if (series.IsCompleted || series.Type == SeriesTypes.Session && series.State == States.None)
            completedSeries.Add(series);
        }

        LauncherArgs args = CreateLauncherArgs();
        foreach (ILaunchRule rule in GetLaunchRules())
        {
          List<IViewerLauncher> launchers = rule.CreateLaunchers(completedSeries);
          if (launchers != null && launchers.Count > 0)
          {
            launchers.Sort();
            foreach (IViewerLauncher launcher in launchers)
            {
              launcher.Initialize(args, m_user);
              possibleViewers.Add(launcher);
            }
          }
        }

        return possibleViewers;
      }
      finally
      {
        Viatronix.Logging.Log.Debug(string.Format("GetPossibleViewers {0}", DateTime.Now.Subtract(start)), "LaunchManager", "GetPossibleViewers");

      }
    } // GetPossibleViewers( collection )


    /// <summary>
    /// Returns the best possible viewer controller for the collection.
    /// </summary>
    /// <param name="collection">Collection of series being launched.</param>
    /// <returns>Best possible viewer controller.</returns>
    public IViewerLauncher GetBestPossibleViewer(List<Series> collection)
    {
      List<Series> completedSeries = new List<Series>();
      foreach( Series series in collection )
      {
        if( series.IsCompleted || ( series.Type == SeriesTypes.Session && series.State == States.None ))
          completedSeries.Add( series );
      }

      SeriesComparer comparer = new SeriesComparer(Series.Attributes.DateReceived);

      completedSeries.Sort(comparer);

      //// retrieve all series information for the rules
      //this.DataProvider.RetrieveData( completedSeries, true );

      DateTimeOffset? bestDate = null;
      IViewerLauncher bestLauncher = null;

      List<IViewerLauncher> launchers = GetPossibleViewers( completedSeries );
      foreach( IViewerLauncher launcher in launchers )
      {
        // find all launchers containing sessions
        if( launcher.IsSession )
        {
          // find the newest session
          if( bestDate < launcher.Datasets[0][0].DateProcessed )
          {
            bestLauncher = launcher;
            bestDate = launcher.Datasets[0][0].DateProcessed;
          }
        }
      }

      if( bestLauncher != null )
        return bestLauncher;
      else if( launchers.Count > 0 )
        return launchers[0];
      else
        return null;
    } // GetBestPossibleViewer( collection )


    /// <summary>
    /// Closes the open viewer if one exists
    /// </summary>
    /// <returns>true the viewer was closed; otherwise false</returns>
    private bool CloseOpenViewer()
    {
      if( this.ActiveLauncher == null )
        return true;

      Viatronix.UI.MessageBox.ShowInfo( "Please close the current viewer before continuing." );
      this.Frame.WorkflowTabs.SelectedItem = this.Frame.WorkflowTabs.Items[ ViewerTab ];
      return false;
    } // CloseOpenViewer()


    /// <summary>
    /// Creates launcher args
    /// </summary>
    /// <returns>LauncherArgs</returns>
    private LauncherArgs CreateLauncherArgs()
    {
      return new LauncherArgs(this.Frame, m_dataProvider, Global.CurrentUser, Global.Site, Global.IdleInterval);
    } // CreateLauncherArgs()


    /// <summary>
    /// Returns a collection of launch rules
    /// </summary>
    /// <returns>collection of launch rules</returns>
    public static List<ILaunchRule> GetLaunchRules()
    {
    

      if( m_rules == null )
        m_rules = (List<ILaunchRule>) ConfigurationManager.GetSection( "launchRules" );

      return m_rules;
    } // GetLaunchRules()


    #if !V3D_NO_LICENSING

    /// <summary>
    /// Returns a collection of launch rules
    /// </summary>
    /// <param name="products">product collection</param>
    /// <returns>collection of launch rules</returns>
    public static List<ILaunchRule> GetLaunchRules(List<Viatronix.Enterprise.Entities.Application> applications)
    {
      if( m_rules == null )
      {
        m_rules = GetLaunchRules();
        for( int i = m_rules.Count - 1; i >= 0; --i )
        {
          ILaunchRule rule = (ILaunchRule) m_rules[i];

          // check that viewer associated with the rule is licensed
          bool isLicensed = false;
          foreach (Viatronix.Enterprise.Entities.Application application in applications)
          {
            if (application.Id.Length == 0 || string.Compare(application.Id, rule.ID, true) == 0)
            {
              isLicensed = true;
              break;
            }
          }

          // skip this rule if a viewer license does not exist
          if( isLicensed == false )
          {
            m_rules.RemoveAt( i );
            Viatronix.Logging.Log.Debug("The launch rule [ID=" + rule.ID + "] is not licensed.", "LauncherManager", "GetLaunchRules");
          }
        }
      }

      return m_rules;
    } // GetLaunchRules( products )

    #endif

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the closing event for the IViewerController.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnControllerClosedHandler( object sender, EventArgs args )
    {
      // Notify that the viewer close occurred.
      if (ViewerClosed != null)
      {
        ViewerClosed(sender, args);
      }

      try
      {
        if( this.ActiveLauncher.CanNavigate )
        {
          this.Frame.WorkflowTabs.BeginInit();

          TabStripItem item = this.Frame.WorkflowTabs.Items[ViewerTab];
          if( item != null )
            this.Frame.WorkflowTabs.Items.Remove( item );

          this.Frame.WorkflowTabs.EndInit();
        }

        this.ActiveLauncher.Closed -= m_closedHandler;
        this.ActiveLauncher.Updated -= m_seriesSavedHandler;

        List<string> uids = new List<string>();
        foreach (List<Series> coll in m_activeLauncher.Datasets)
        {
          foreach (Series series in coll)
          {
            if (!uids.Contains(series.StudyUid))
            {
              uids.Add(series.StudyUid);
            }
          }
        }

      }
      finally
      {
        Global.ApplicationState.Remove( ConsoleState.LaunchManager );
        this.ActiveLauncher = null;
      }
    } // OnControllerClosedHandler( sender, args )


    /// <summary>
    /// Handles the save series event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSeriesSavedHandler( object sender, EventArgs args )
    {
      ISearchable searchableView = m_frame.ActiveView as ISearchable;
      if( searchableView != null )
      {
        searchableView.Searcher.Reset();
        searchableView.Searcher.Search();
      }
    } // OnSeriesSaved( object sender, EventArgs args )

    #endregion

  } // class LaunchManager
} // namespace Viatronix.Console


#region revision history

// $Log: LaunchManager.cs,v $
// Revision 1.7.2.6  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.5  2008/04/10 20:20:12  mkontak
// Added the ability to cancel the laucnh for a VC
//
// Revision 1.7.2.4  2008/02/20 14:17:01  mkontak
// Issue 6021: Problem launching CAD
//
// Revision 1.7.2.3  2008/02/11 14:18:15  mkontak
// FIx 2 same orientation opening
//
// Revision 1.7.2.2  2007/05/25 17:45:13  mkontak
// Added AcquisitionDate
//
// Revision 1.7.2.1  2007/04/27 15:28:01  gdavidson
// Issue #5619: Check to see if a global IViewrLauncher exisits and if it does assign it to the local variable
//
// Revision 1.7  2007/03/13 12:18:09  mkontak
// coding standards
//
// Revision 1.6  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.5  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.4  2007/02/20 18:44:09  gdavidson
// Added log messages
//
// Revision 1.3  2006/11/18 00:04:45  gdavidson
// Issue #5178: Launches the newest session when possible
//
// Revision 1.2  2006/11/16 22:27:27  romy
// Issue #5153: Added unlocking when a viewer closes
//
// Revision 1.1  2006/10/30 15:35:55  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.38  2006/09/21 20:55:27  mkontak
// Issue 4995
//
// Revision 1.37  2006/08/23 18:38:05  gdavidson
// Sort launch items according to ordering in the config file
//
// Revision 1.36  2006/08/18 16:27:04  gdavidson
// Issue #4953: Added log message when a launch rule is excluded because a product is not licensed
//
// Revision 1.35  2006/06/07 17:45:25  gdavidson
// Added back checking the license for vaild products
//
// Revision 1.34  2006/06/06 19:46:42  mkontak
// changed deprecated code
//
// Revision 1.33  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.32  2006/04/28 00:04:25  mkontak
// minor fix
//
// Revision 1.31  2006/04/27 19:45:54  mkontak
// Make field non case sensitive
//
// Revision 1.30  2006/04/24 18:54:08  romy
// moved Locking to a different spot
//
// Revision 1.29  2006/04/24 15:48:01  gdavidson
// Implemented immediate launch on a double clicked study
//
// Revision 1.28  2006/04/19 14:19:38  gdavidson
// Issue #4699: Won't launch session if there is a state specified in the series
//
// Revision 1.27  2006/04/19 14:18:34  mkontak
// Added additional field OPERATION to allow for finer granularity of the locks
//
// Revision 1.26  2006/04/05 17:32:16  gdavidson
// Issue #4681: session series to do not need to be marked completed
//
// Revision 1.25  2006/03/22 05:16:03  romy
// lockmanager modified
//
// Revision 1.24  2006/03/17 14:51:31  romy
// Lock fix
//
// Revision 1.23  2006/03/17 14:04:29  romy
// Lock fix
//
// Revision 1.22  2006/03/16 22:00:45  romy
// Added Locking
//
// Revision 1.21  2006/03/13 16:02:34  gdavidson
// Won't launch series that are not completed
//
// Revision 1.20  2006/03/09 21:10:39  gdavidson
// Moved ShowWindow method from Viatroinx.v3D.UI.Utilities to Viatroinx.v3D.Core.WindowUtilities
//
// Revision 1.19  2006/02/23 21:58:43  gdavidson
// Added an Enabled property.
//
// Revision 1.18  2006/02/21 14:56:15  gdavidson
// New version of licensing
//
// Revision 1.17  2006/01/09 19:52:12  gdavidson
// Added a Launch method which accepts a StudyCollection.
//
// Revision 1.16  2005/12/14 19:57:26  gdavidson
// Reworked the LaunchRules and launch operation.
//
// Revision 1.15  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.14  2005/11/24 02:55:12  gdavidson
// Added a user friendly message when a viewer cannot launch
//
// Revision 1.13  2005/11/24 01:24:42  geconomos
// temporarily disallowing opening new viewer when one is active
//
// Revision 1.12  2005/11/23 19:36:49  vxconfig
// fixed compilation issues
//
// Revision 1.11  2005/11/23 18:33:55  geconomos
// launching from console revisted
//
// Revision 1.10  2005/11/23 12:13:33  mkontak
// Using new MessageBox implementation
//
// Revision 1.9  2005/11/23 03:58:32  geconomos
// viewer launching revisted
//
// Revision 1.8  2005/11/23 00:55:58  gdavidson
// Added a delegate to display a message box on the ui thread.
//
// Revision 1.7  2005/11/22 17:58:34  gdavidson
// Commented the code
//
// Revision 1.6  2005/11/21 20:39:35  gdavidson
// Execute the launch code in an asynchronous call
//
// Revision 1.5  2005/11/19 04:31:24  gdavidson
// Added a check to exclude launch rules that require multiple series when a single series is selected
//
// Revision 1.4  2005/11/18 21:10:59  gdavidson
// Added exception handling
//
// Revision 1.3  2005/11/16 13:55:29  gdavidson
// Added a method that launches a study.
//
// Revision 1.2  2005/11/10 19:51:02  gdavidson
// Added check for null before retrieving data.
//
// Revision 1.1  2005/11/09 16:03:15  gdavidson
// Moved from the folder views\launchers
//
// Revision 1.4  2005/11/02 14:19:10  gdavidson
// Removed the PaneExpanded event handlers
//
// Revision 1.3  2005/10/31 16:32:14  gdavidson
// Added methods to carryout complex remoting calls.
//
// Revision 1.2  2005/10/26 19:48:37  gdavidson
// Made it so a user can cancel the viewer's close operation.
//
// Revision 1.1  2005/10/18 17:43:43  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/LaunchManager.cs,v 1.7.2.6 2011/05/18 01:57:00 mkontak Exp $
// $Id: LaunchManager.cs,v 1.7.2.6 2011/05/18 01:57:00 mkontak Exp $

#endregion
