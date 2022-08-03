// $Id: Launcher.cs,v 1.13.2.13 2011/05/18 17:50:03 kchalupa Exp $
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
using System.Diagnostics;
using System.IO;
using System.Drawing;
using Viatronix.Logging;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.Console
{

#region enums

  /// <summary>
  /// Identifies the souce fo the launch 
  ///   CONSOLE is for a console
  ///   LAUNCHER is for a launcher 
  /// </summary>
  public enum LaunchSource
  {
    CONSOLE,
    LAUNCHER
  };

#endregion

	/// <summary>
	/// Base class for all launchers.
	/// </summary>
  public abstract class Launcher : MarshalByRefObject, IViewerLauncher
  {

    #region fields

    /// <summary>
    /// Launch source
    /// </summary>
    private LaunchSource m_source = LaunchSource.CONSOLE;

    /// <summary>
    /// The plugin id of the viewer.
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// The display text for the viewer.
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// The associated viewer's description.
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// The viewer's logo.
    /// </summary>
    private Image m_logo = null;

    /// <summary>
    /// The path to the viewer.
    /// </summary>
    private string m_executable = string.Empty;

    /// <summary>
    /// The command line arguments.
    /// </summary>
    private string m_commandLine = string.Empty;

    /// <summary>
    /// Determines if the controller is launching an existing session.
    /// </summary>
    private bool m_hasExistingSession = false;

    /// <summary>
    /// The viewer's process
    /// </summary>
    private Process m_process = null;

    /// <summary>
    /// List of series collections.
    /// </summary>
    private List< List<Series> > m_datasets = null;

    /// <summary>
    /// Initialization arguments.
    /// </summary>
    private LauncherArgs m_launcherArgs = null;

    /// <summary>
    /// Reference to a windows form
    /// </summary>
    private System.Windows.Forms.Form m_form = null;

    /// <summary>
    /// Reference to an IDataProvider.
    /// </summary>
    private IDataProvider m_dataProvider = null;

    /// <summary>
    /// Handler for the form closing event.
    /// </summary>
    private System.ComponentModel.CancelEventHandler m_formClosingHandler = null;

    /// <summary>
    /// Determines if the viewer has already been closed.
    /// </summary>
    private bool m_viewerClosed = false;

    /// <summary>
    /// Determines if the console can navigate to the viewer.
    /// </summary>
    private bool m_canNavigate = true;

    /// <summary>
    /// Additional initialization information
    /// </summary>
    private string m_additionalInfo = string.Empty;

    /// <summary>
    /// process exited handler
    /// </summary>
    private EventHandler m_processExitedHandler = null;
    
    /// <summary>
    /// Closed event.
    /// </summary>
    public event EventHandler Closed = null;

    /// <summary>
    /// Updated event.
    /// </summary>
    public event EventHandler Updated = null;

    /// <summary>
    /// User
    /// </summary>
    private User m_user = null;

    /// <summary>
    /// Determines if the viewer should create a new session, if not launched with a session series.
    /// </summary>
    private bool m_newSession = true;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public Launcher()
    {
      m_formClosingHandler = new System.ComponentModel.CancelEventHandler( this.OnFormClosingHandler );
      m_processExitedHandler = new EventHandler( this.OnProcessExitedHandler );
    } // ViewerController()

    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the launch source
    /// </summary>
    public LaunchSource Source
    {
      get { return m_source; }
      set { m_source = value; }
    } // Source

    /// <summary>
    /// Gets or sets the plugin id for the viewer.
    /// </summary>
    public string ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID


    /// <summary>
    /// Gets or sets the display text.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text


    /// <summary>
    /// Gets or sets the associated viewer's description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    } // Description


    /// <summary>
    /// Gets or sets the logo.
    /// </summary>
    public Image Logo
    {
      get { return m_logo; }
      set { m_logo = value; }
    } // Logo


    /// <summary>
    /// Gets or sets the full path to the viewer's executable.
    /// </summary>
    public string Executable
    {
      get { return m_executable; }
      set { m_executable = value; }
    } // Executable


    /// <summary>
    /// Gets or sets the command line arguments
    /// </summary>
    public string CommandLine
    {
      get { return m_commandLine; }
      set { m_commandLine = value; }
    } // CommandLine


    /// <summary>
    /// Additional information stored in xml. 
    /// </summary>
    public string AdditionalInfo
    {
      get { return m_additionalInfo; }
      set { m_additionalInfo = value; }
    } // AdditionalInfo



    /// <summary>
    /// Gets or sets if the viewer is launching an existing session.
    /// </summary>
    public bool IsSession
    {
      get { return m_hasExistingSession; }
      set { m_hasExistingSession = value; }
    } // HasExistingSession


    /// <summary>
    /// Returns a collections of series collections
    /// </summary>
    public List< List<Series> > Datasets
    {
      get { return m_datasets; }
      set { m_datasets = value; }
    } // Datasets


    /// <summary>
    /// Returns the process for the viewer.
    /// </summary>
    public Process ViewerProcess
    {
      get { return m_process; }
      set { m_process = value; }
    } // ViewerProcess


    /// <summary>
    /// Gets or sets if the console can navigate to the viewer
    /// </summary>
    public bool CanNavigate
    {
      get { return m_canNavigate; }
      set { m_canNavigate = value; }
    } // CanNavigate


    /// <summary>
    /// Returns the form that launches the viewer.
    /// </summary>
    protected System.Windows.Forms.Form Form
    {
      get { return m_form; }
      set { m_form = value; }
    } // Form


    /// <summary>
    /// Gets or sets the IDataProvider.
    /// </summary>
    protected IDataProvider DataProvider
    {
      get { return m_dataProvider; }
    } // DataProvider


    /// <summary>
    /// Returns the intialization arguments.
    /// </summary>
    protected LauncherArgs LauncherArgs
    {
      get { return m_launcherArgs; }
    } // LauncherArgs


    /// <summary>
    /// Gets or sets the new session flag.
    /// </summary>
    protected bool NewSession
    {
      get { return m_newSession; }
      set { m_newSession = value; }
    } // NewSession
    
    #endregion


    #region virtual methods

    /// <summary>
    /// Initializes the launcher.
    /// </summary>
    /// <param name="args"></param>
      public virtual void Initialize(LauncherArgs args, User user) 
    {
      m_launcherArgs = args;
      m_user = user;

      m_form = args.Form;
      m_dataProvider = args.Provider;
    } // Initialize( args ) 


    /// <summary>
    /// Initializes any setting prior to launch
    /// </summary>
    protected virtual void OnInitializeLaunch() 
    {
      if (m_dataProvider == null)
        return;

      foreach (List<Series> collection in this.Datasets)
      {
        foreach (Series series in collection)
        {
          m_dataProvider.GetFiles( series);
          //m_dataProvider.GetReferences( series);

          foreach (Series rs in series.References)
          {
            m_dataProvider.GetFiles( rs);
          }
        }
       }
    } // OnInitializeLaunch() 


    /// <summary>
    /// Creates the viewer's process
    /// </summary>
    /// <returns>the viewer's process</returns>
    protected virtual Process OnCreateViewerProcess()
    {
      // start the viewer's process and wait for it to signal it is ready
      //NamedEvent namedEvent = new NamedEvent( "C5D9F4E1-6B67-4f8b-A904-BBE1FC64A1C6", true );
      try
      {

        string parameters = "-" + m_source.ToString().ToLower() + " " + ( m_user != null ? "-user " + m_user.Login + "||" + Global.CurrentUser.ContextId : "" ) + this.CommandLine;

        Log.Information("Launching process [PATH=" + this.Executable + "] [PARAMETERS=" + parameters + "]", "Launcher", "OnCreateViewerProcess");

        // creates the process which executes the viewer
        ProcessStartInfo info = new ProcessStartInfo(this.Executable, parameters);
        info.WorkingDirectory = Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.ParseExpression(this.Executable);
        info.ErrorDialog = false;
        info.UseShellExecute = true;

        Process process = Process.Start( info );
      
        System.Threading.Thread.Sleep(1000);
        //namedEvent.WaitOne();

        return process;
      }
      catch( Exception e )
      {
        throw new LaunchException( LaunchStage.StartProcess, "Unable to start the viewer's process. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e );
      }
      finally
      {
        //namedEvent.Close();
      }      
    } // OnCreateViewerProcess()


    /// <summary>
    /// Raises the Closed event.
    /// </summary>
    protected virtual void OnViewerClosed()
    {
      if( m_process != null )
        m_process.Exited -= m_processExitedHandler;

      if( m_form != null )
        m_form.Closing -= m_formClosingHandler;

      if( this.Closed != null )
        this.Closed( this, EventArgs.Empty );     
    } // OnClosed()


    /// <summary>
    /// Raises the updated event.
    /// </summary>
    protected virtual void OnUpdated()
    {
      if( this.Updated != null )
        this.Updated( this, EventArgs.Empty );
    } // OnUpdated()


    /// <summary>
    /// Terminates the viewer's process.
    /// </summary>
    protected virtual void OnTerminateLaunch()
    {
      if( m_process != null )
      {
        if ( ! m_process.HasExited && m_process.CloseMainWindow() == false )
          m_process.Kill();

        m_process = null;
      }

      if( m_form != null )
        m_form.Visible = true;
    } // TerminateViewer()


    /// <summary>
    /// Clean's up any work performed by the launcher
    /// </summary>
    protected virtual void OnCleanupLaunch()
    {
    } // OnCleanupLaunch()


    /// <summary>
    /// Displays the form
    /// </summary>
    /// <param name="bounds">form bounds</param>
    /// <param name="state">window state</param>
    protected virtual void OnShowForm( Rectangle bounds, System.Windows.Forms.FormWindowState state )
    {
      if( m_form != null )
      {
        m_form.Bounds = bounds;
        m_form.WindowState = state;
        m_form.Visible = true;
      }
    } // OnShowForm( bounds, state )

    #endregion
    
    #region methods

    /// <summary>
    /// Obtains a lifetime service object to control the lifetime policy for this instance.
    /// </summary>
    /// <returns>null</returns>
    public override object InitializeLifetimeService()
    {
      return null;
    } // InitializeLifetimeService()


    /// <summary>
    /// Launches the viewer.
    /// </summary>
    public virtual void LaunchViewer()
    {
      if( m_form != null && m_form.InvokeRequired )
          this.Form.Invoke(new System.Windows.Forms.MethodInvoker(this.LaunchViewer));
      else
      {
        try
        {
          m_viewerClosed = false;

          if( this.LauncherArgs == null )
            throw new LaunchException( LaunchStage.InitializeLaunch, "The launcher has not been intialized. [PLUGIN=" + this.ID + "]" );

          // initialize anything that is necessary for launching the selected viewer
          try
          {
            OnInitializeLaunch();
          }
          catch (LaunchException e)
          {
            throw e;
          }
          catch (Exception e)
          {
            throw new LaunchException(LaunchStage.InitializeLaunch, "Unable to initialize launch. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e);
          }

          m_process = OnCreateViewerProcess();
          m_process.EnableRaisingEvents = true;
          m_process.Exited += m_processExitedHandler;

          // initialize cross process communication
          //try
          //{
          //  OnInitializeCommunication();
          //}
          //catch( Exception e )
          //{
          //  OnTerminateLaunch();
          //  throw new LaunchException( LaunchStage.InitializeCommunication, "Unable to communicate with viewer. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e );
          //}

          //// display the viewer
          //try
          //{
          //  OnShowViewer();
          //}
          //catch( Exception e )
          //{
          //  OnTerminateLaunch();
          //  throw new LaunchException( LaunchStage.DisplayViewer, "Unable to display viewer. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e );
          //}

          // tell the viewer what to do (start new, load session, etc...)
        //  try
        //  {
        //    OnStartViewer();

        //    if( this.Form != null )
        //      this.Form.Closing += m_formClosingHandler;
        //  }
        //  catch( Exception e )
        //  {
        //    OnTerminateLaunch();
        //    throw new LaunchException( LaunchStage.StartViewer, "Unable to start the viewer. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e );
        //  }
        }
        catch( LaunchException e )
        {
          OnCleanupLaunch();
          throw e;
        }
        catch
        {
          OnTerminateLaunch();
          OnCleanupLaunch();
          throw new Exception( "Unmanaged exception occurred during the launch operation." );
        }

        if ( Global.Frame != null && Global.Frame.ActiveView is ISearchable)
        {
          ISearchable view = (ISearchable)(Global.Frame.ActiveView);
          view.Searcher.Reset();
          view.Searcher.Search();
        }

      }

    } // LaunchViewer()



    
    /// <summary>
    /// Displays the form
    /// </summary>
    /// <param name="bounds">form bounds</param>
    /// <param name="state">windows state</param>
    protected void ShowForm( Rectangle bounds, System.Windows.Forms.FormWindowState state )
    {
      if( m_form != null && m_form.InvokeRequired )
        this.Form.Invoke( new Viatronix.UI.SetterDelegate< Rectangle, System.Windows.Forms.FormWindowState >( this.OnShowForm ), bounds, state );
      else
        OnShowForm( bounds, state );
    } // ShowForm( bounds, state )


    /// <summary>
    /// Raises the Closed event.
    /// </summary>
    protected void ViewerClosed()
    {
      if( !m_viewerClosed )
      {
        if( m_form != null && m_form.InvokeRequired )
          this.Form.Invoke( new System.Windows.Forms.MethodInvoker( this.OnViewerClosed ) );
        else
          OnViewerClosed();
      }

      m_viewerClosed = true;

      foreach(List<Series> collection in m_datasets)
      {
        foreach(Series series in collection)
        {
          // TODO: New
          //Database.LogView(series, m_id);
        } // foreach(Series series in collection)
      } // foreach(List<Series> collection in m_datasets)

    } // OnClosed()    


    /// <summary>
    /// Terminates the launch
    /// </summary>
    protected void TerminateLaunch()
    {
      if( m_form != null && m_form.InvokeRequired )
        this.Form.Invoke( new System.Windows.Forms.MethodInvoker( this.OnTerminateLaunch ) );
      else
        TerminateLaunch();
    } // TerminateLaunch()


    /// <summary>
    /// Cleansup the launch
    /// </summary>
    protected void CleanupLaunch()
    {
      if( m_form != null && m_form.InvokeRequired )
        this.Form.Invoke( new System.Windows.Forms.MethodInvoker( this.OnCleanupLaunch ) );
      else
        CleanupLaunch();
    } // CleanupLaunch()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the exited event of a process.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnProcessExitedHandler( object sender, EventArgs args )
    {
      ShowForm( m_form.Bounds, m_form.WindowState );
      ViewerClosed();
    } // OnProcessExitedHandler( sender, args )


    /// <summary>
    /// Handles the form closing event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An CancelEventArgs that contains the event data.</param>
    private void OnFormClosingHandler( object sender, System.ComponentModel.CancelEventArgs args )
    {
      Viatronix.UI.MessageBox.ShowInfo( "A viewer is still open. Please close it before exiting Console." );

      args.Cancel = true;
    } // OnFormClosingHandler( sender, args )

    #endregion

    #region IComparable<IViewerLauncher> Memebers

    /// <summary>
    /// Compares the current instance with another object of the same type. 
    /// </summary>
    /// <param name="other">IViewerLauncher</param>
    /// <returns>A 32-bit signed integer that indicates the relative order of the objects being compared</returns>
    public int CompareTo( IViewerLauncher other )
    {
      int result = this.Text.CompareTo( other.Text );
      if( result == 0 )
      {
        // priorty goes the launcher with multiple datasets
        if( this.Datasets.Count > other.Datasets.Count )
          result = -1;
        else if( this.Datasets.Count < other.Datasets.Count )
          result = 1;
        else
          result = this.Description.CompareTo( other.Description );
      }

      return result;
    } // CompareTo( other )

    #endregion

  } // class ViewerController
} // namespace Viatronix.Console

#region revision history

// $Log: Launcher.cs,v $
// Revision 1.13.2.13  2011/05/18 17:50:03  kchalupa
// CONS-1640
// Working directory, parse with SystemSettings::ParseExpression instead of Path::GetDirectoryName()
//
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.12  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.11  2009/04/24 19:46:29  mkontak
// Fix -console
//
// Revision 1.13.2.10  2009/04/03 14:28:58  mkontak
// Added a source property to identify Console or Launcher as the source.
//
// Revision 1.13.2.9  2009/02/06 19:19:13  kchalupa
// Region crashproofing.
//
// Revision 1.13.2.8  2008/10/03 19:33:30  kchalupa
// Pay Per Procedure Functionality
//
// Revision 1.13.2.7  2008/06/12 15:04:09  mkontak
// Fix for kodak launcher
//
// Revision 1.13.2.6  2008/04/10 20:20:12  mkontak
// Added the ability to cancel the laucnh for a VC
//
// Revision 1.13.2.5  2007/12/14 19:04:02  mkontak
// Upon return from the viewer launch the Console will be refreshed
//
// Revision 1.13.2.4  2007/07/25 04:15:22  mkontak
// Added condition to ceck if the process has exited before exiting
//
// Revision 1.13.2.3  2007/05/07 15:21:42  gdavidson
// Issue #5641: Added checks for null before unassigning process exited handler
//
// Revision 1.13.2.2  2007/05/04 20:26:13  gdavidson
// Removed unnecessary log messages
//
// Revision 1.13.2.1  2007/05/04 20:13:14  gdavidson
// Issue #5550: Removed process exited event handler when viewer closes
//
// Revision 1.13  2007/03/08 15:10:43  gdavidson
// Issue #5463: Added log message for a successful launch
//
// Revision 1.12  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.11  2007/03/05 16:17:30  gdavidson
// Issue #5338: Perform all launch operations on the main thread
//
// Revision 1.10  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.9  2007/02/20 18:44:08  gdavidson
// Added log messages
//
// Revision 1.8  2007/02/13 18:47:40  gdavidson
// Added log messages during the launch operation
//
// Revision 1.7  2007/02/08 15:48:57  gdavidson
// Issue #5373 and 5375: Changed how IViewerLauchers are sorted.
//
// Revision 1.6  2007/01/23 15:19:36  gdavidson
// Fixed a problem with maintaing window bounds and state when switching applications
//
// Revision 1.5  2007/01/17 18:43:16  gdavidson
// Cleaned up error message
//
// Revision 1.4  2006/12/18 16:11:16  gdavidson
// Added functionality to intialize with additional xml data and changed wait event to use a guid
//
// Revision 1.3  2006/11/11 01:34:35  gdavidson
// Modified the OnCreateViewerProcess method
//
// Revision 1.2  2006/11/02 22:41:09  gdavidson
// Added configurable command line arguments
//
// Revision 1.1  2006/10/30 15:41:21  gdavidson
// Moved from Viatronix.Console (IVewerController renamed ILauncher)
//
// Revision 1.11  2006/05/01 19:31:38  gdavidson
// Issue #4748: Prompt the user to close open viewer before leaving Console.
//
// Revision 1.10  2006/04/27 19:45:21  mkontak
// Make field non case sensitive
//
// Revision 1.9  2006/04/19 14:18:41  mkontak
// Added additional field OPERATION to allow for finer granularity of the locks
//
// Revision 1.8  2006/03/22 05:16:03  romy
// lockmanager modified
//
// Revision 1.7  2006/03/16 22:01:08  romy
// Added Locking
//
// Revision 1.6  2006/03/09 21:09:23  gdavidson
// Removed ViewerBounds property from IViewerController
//
// Revision 1.5  2006/02/23 21:08:25  gdavidson
// Added functionality to display the console when the viewer exits abnormally.
//
// Revision 1.4  2006/02/13 16:27:25  gdavidson
// Made the LaunchViewer method virtual.
//
// Revision 1.3  2006/01/09 19:49:41  gdavidson
// Added Logo property to IViewerController interface
//
// Revision 1.2  2006/01/03 16:08:48  gdavidson
// Removed the RetrieveSeriesData method.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/Launcher.cs,v 1.13.2.13 2011/05/18 17:50:03 kchalupa Exp $
// $Id: Launcher.cs,v 1.13.2.13 2011/05/18 17:50:03 kchalupa Exp $

#endregion
