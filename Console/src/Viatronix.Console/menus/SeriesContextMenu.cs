// $Id: SeriesContextMenu.cs,v 1.53.2.7 2008/09/15 19:22:44 mkontak Exp $
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
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;

using BrightIdeasSoftware;

using Viatronix.UI;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
	/// <summary>
	/// Provides all context menu options for studies
	/// </summary>
	public class SeriesContextMenu : Viatronix.UI.ContextMenu
	{
		
    #region fields

    /// <summary>
    /// Reference to the parent view browser window.
    /// </summary>
    private ViewBrowser m_browser = null;

    /// <summary>
    /// ListView containing series objects.
    /// </summary>
    private ObjectListView m_series = null;

    /// <summary>
    /// ListView containing study objects.
    /// </summary>
    private ObjectListView m_studies = null;


    /// <summary>
    /// Open series menu item.
    /// </summary>
    private MenuItemEx m_openItem = null;

    /// <summary>
    /// Delete series menu item.
    /// </summary>
    private MenuItemEx m_deleteItem = null;

    /// <summary>
    /// Show dicom menu item.
    /// </summary>
    private MenuItemEx m_dicomItem = null;

    /// <summary>
    /// Show directory menu item.
    /// </summary>
    private MenuItemEx m_directoryItem = null;

    /// <summary>
    /// Transfer Menu item.
    /// </summary>
    private MenuItemEx m_transfer = null;

    /// <summary>
    /// Process As menu item
    /// </summary>
    private MenuItemEx m_processAsItem = null;



    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SeriesContextMenu object.
    /// </summary>
    /// <param name="browser">Reference to a ViewBrowser.</param>
    /// <param name="studies">Reference to the study's listview</param>
    /// <param name="series">Reference to the series' listview</param>
    public SeriesContextMenu(ViewBrowser browser, ObjectListView studies, ObjectListView series)
    {

      m_browser = browser;
      m_series  = series;
      m_studies = studies;

      m_openItem      = new MenuItemEx( "Open..." );
      m_transfer      = new MenuItemEx( "Transfer...", new EventHandler( this.OnTransferHandler ));
      m_deleteItem    = new MenuItemEx( "Delete", new EventHandler( this.OnDeleteHandler ));
      m_dicomItem     = new MenuItemEx( "Show DICOM Info...", new EventHandler( this.OnDicomHandler ));
      m_directoryItem = new MenuItemEx( "Show Directory", new EventHandler( this.OnShowDirectoryHandler ));
      m_processAsItem = new MenuItemEx( "Process...", new EventHandler( this.OnProcessAsHandler ) );

      this.MenuItems.Add( m_openItem );
      this.MenuItems.Add( new MenuItemEx( "-" ));

      this.MenuItems.Add( m_processAsItem );

      this.MenuItems.Add( new MenuItemEx( "-" ));

      this.MenuItems.Add( m_transfer );
      this.MenuItems.Add( m_deleteItem );
      this.MenuItems.Add( new MenuItemEx( "-" ));

      this.MenuItems.Add( m_dicomItem );
      this.MenuItems.Add( m_directoryItem );


    } // StudyContextMenu( browser, studies, series )

    #endregion

    #region override methods

    /// <summary>
    /// Displays the menu items that are appropriate to the selected listview item. 
    /// 
    /// NOTE: If a copy session is added then we need to check if the user is allowed
    ///       Global.CurrentUser.IsAllowed("CopySession") == true
    /// 
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnPopup( EventArgs args )
    {
      base.OnPopup( args );

      List<Viatronix.Enterprise.Entities.Application> apps = Global.DataProvider.GetApplications();

      bool receiving  = false;
      bool processing = false;
      bool failed     = false;
      bool locked     = false;
      bool aborted    = false;
     

      foreach ( Series series in m_browser.SelectedSeries )
      {
        // refresh the series object
        m_browser.BrowserProvider.GetSeries(series);

        if ( ( locked = series.IsLocked ) )
          break;

        processing = series.IsProcessing;
 
        ///////////////////////////////////////////////////////////////////////////
        // Make sure the series is not still being received as in manual mode
        //////////////////////////////////////////////////////////////////////////
        if ( series.IsProcess("Receiving") && ! series.IsCompleted )
        {
          receiving = true;
          //break;
        } // END ... If still receiving

        //////////////////////////////////////////////////////
        // Check for a failed state in the selected series
        //////////////////////////////////////////////////////
        if ( series.IsFailed )
        {
          failed = true;
          //break;
        } // END ... If the series is failed

        //////////////////////////////////////////////////////
        // Check for a aborted state in the selected series
        //////////////////////////////////////////////////////
        if (series.IsAborted)
        {
          aborted = true;
          break;
        } // END ... If the series is failed

      } // END ... For each Series


      User currentUser = Global.CurrentUser;


      bool enable = !aborted && !failed && !processing && !receiving && !locked;

      // Open Series
      m_openItem.Enabled = enable && (m_browser.SelectedSeries.Count > 0) && m_browser.BrowserInfo.CanLaunch;
      m_openItem.MenuItems.Clear();

      if ( enable && m_browser.BrowserInfo.CanLaunch )
      {
        List<Viatronix.Enterprise.Entities.Application> applications = m_browser.DataProvider.GetApplications(m_browser.SelectedSeries);

        foreach ( Viatronix.Enterprise.Entities.Application application in applications )
        {
          MenuItemEx menu = new MenuItemEx(application.Name, this.OnLaunchHandler);

          m_openItem.MenuItems.Add(menu);   
        }

        m_openItem.Enabled &= ( m_openItem.MenuItems.Count > 0 );
      }

      m_transfer.Enabled   = enable && (m_browser.SelectedStudies.Count == 1) && (m_browser.SelectedSeries.Count > 0) && m_browser.BrowserInfo.CanTransfer && Global.CurrentUser.IsAllowed("Transfer");
      m_deleteItem.Enabled = Global.CurrentUser.IsAllowed("DeleteSeries") && !processing && !receiving && !locked && (m_browser.SelectedSeries.Count > 0) && m_browser.BrowserInfo.CanDelete;
      m_dicomItem.Enabled = enable && (m_browser.SelectedSeries.Count > 0) && m_browser.BrowserInfo.CanShowDicomInformation;


      m_directoryItem.Visible = Global.CurrentUser.IsService;
      m_directoryItem.Enabled = !receiving && (m_browser.SelectedSeries.Count > 0) && m_browser.BrowserInfo.CanShowDicomFolder;


      SeriesTypes seriesType = (m_browser.SelectedSeries.Count > 0 ? m_browser.SelectedSeries[0].Type : SeriesTypes.None );
      string seriesState     = (m_browser.SelectedSeries.Count > 0 ? m_browser.SelectedSeries[0].State.ToString() : string.Empty);
      int processCount       = (m_browser.SelectedSeries.Count > 0 ? Global.DataProvider.GetTemplates(m_browser.SelectedSeries[0].Application, string.Empty).Count : 0);

      char [] sep = { ',' };
      
      string [] processAsTypes = new string [0];

      if (System.Configuration.ConfigurationManager.AppSettings["AllowedProcessSeriesTypes"] != null)
      {
        processAsTypes = System.Configuration.ConfigurationManager.AppSettings["AllowedProcessSeriesTypes"].Split(sep);
      }


      m_processAsItem.Enabled = Global.CurrentUser.IsAllowed("ProcessData") && enable && processCount > 0 && apps.Count > 0 && 
                                    (m_browser.SelectedSeries.Count == 1 && Array.IndexOf(processAsTypes, seriesType, 0) > -1 && seriesState != "RECEIVING" && m_browser.BrowserInfo.CanProcess);


    } // OnPopup( args )

    #endregion

    #region methods


    /// <summary>
    /// Process As
    /// </summary>
    /// <param name="collection">Collection of series items being deleted.</param>
    public void ProcessAs( List<Series> collection )
    {    
      try
      {
        //Loop through all the selected rows.
        foreach ( Series series in collection )
        {
          Study study = Global.DataProvider.GetStudy( series);

          //Show the CommandLineProvider Form to pick parameters.
          Viatronix.Console.ProcessAsDialog processAsDialog = new Viatronix.Console.ProcessAsDialog( series, study.Name.ToString() );

          if (processAsDialog.ShowDialog() == DialogResult.OK)
          {
          try
          {
            // strip off the base path from the series
            string basePath = series.Path.Replace(series.Directory, "");
              Global.DataProvider.Create(series, processAsDialog.Template);
          }
            catch (Exception ex)
          {
            Viatronix.UI.MessageBox.ShowError(ex);
          }
          }
          else
          {
            return;
          }
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed to preprocess series : " + ex.Message, "SeriesContextMenu", "ProcessAs");
        Viatronix.UI.MessageBox.ShowError("Unexpected error occurred. Please contact Viatronix Customer Service");
      }

    } // ProcessAs()

    /// <summary>
    /// Transfer
    /// </summary>
    public void Transfer(List<Series> seriesCollection)
    {
      if (seriesCollection.Count == 0 || !m_browser.BrowserInfo.CanTransfer || !Global.CurrentUser.IsAllowed("Transfer")) return;

      if (m_browser.TransferProviders.Count <= 0)
      {
        Viatronix.UI.MessageBox.ShowError(Global.Frame, " Transfer not supported ");
        return;
      }

      bool receiving = false;
      bool processing = false;
      bool failed = false;
      bool locked = false;
      bool aborted = false;

      foreach (Series series in seriesCollection)
      {
        m_browser.BrowserProvider.GetSeries(series);

        locked = series.IsLocked;

        if (locked)
          break;

        processing = series.IsProcessing;
 
        ///////////////////////////////////////////////////////////////////////////
        // Make sure the series is not still being received as in manual mode
        //////////////////////////////////////////////////////////////////////////
        if (series.IsReceiving && ! series.IsCompleted )
        {
          receiving = true;
          //break;
        } // END ... If still reeciving

        //////////////////////////////////////////////////////
        // Check for a failed state in the selected series
        //////////////////////////////////////////////////////
        if (series.IsFailed )
        {
          failed = true;
          //break;
        } // END ... If the series is failed

        //////////////////////////////////////////////////////
        // Check for a aborted state in the selected series
        //////////////////////////////////////////////////////
        if (series.IsAborted)
        {
          aborted = true;
          break;
        } // END ... If the series is failed

      } // END ... For each Series

      // Check if any of the flags are set
      if (aborted || failed || processing || receiving || locked) return;



      //Get the Transfer details from the user.
      List<Series> col = new List<Series>();

      Series selectedSeries = (seriesCollection.Count == 1 ? seriesCollection[0] : null);

      //////////////////////////////////////////////////////
      // Loop  through all the series in the selected list
      //////////////////////////////////////////////////////
      foreach (Series series in seriesCollection)
      {

        /// Adds the serie and all references to the list
        AddSeriesToCollection(col, series);

      } // END ... For each series selected 

      /*
        Study study = series.Study;

        foreach( Series refSeries in series.References )
        {
          if ( ! m_browser.Series.Contains(refSeries) )
          {
            Series coreSeries = study.Series.Find(refSeries);

            if ( ! seriesCollection.Contains(coreSeries) ) seriesCollection.Add(coreSeries);
          }
        }

        if ( ! seriesCollection.Contains(series) ) seriesCollection.Add(series);

      }
      */

      //seriesCollection.Add ( m_browser.Series );

      // ===================================================
      // TODO: REST       Transfer Needs to be addresssed
      // ===================================================
      //foreach (Series series in col)
      //{
      //  Global.LockManager.Lock(series.StudyUid);
      //}

      //Transfer Selected study.

      Viatronix.UI.MessageBox.ShowWarning("Need to implement calling to Transfer Utility.");

      //TransferManager transferManager = new TransferManager(m_browser.DataProvider);
      //if (transferManager.SelectValidTransferProvider(m_browser.TransferProviders, col, selectedSeries))
      //  transferManager.Transfer();

      // ===================================================
      // TODO: REST       Transfer Needs to be addresssed
      // ===================================================
 
      //foreach (Series series in col)
      //{
      //  Global.LockManager.ClearLock(series.StudyUid);
      //}

    } // Transfer()


    /// <summary>
    /// Determines if the transfer is allowed
    /// </summary>
    /// <returns></returns>
    public bool IsTransferAllowed(List<Series> seriesCollection)
    {
      bool transferAllowed = seriesCollection.Count > 0 && m_browser.BrowserInfo.CanTransfer && Global.CurrentUser.IsAllowed("Transfer") && m_browser.TransferProviders.Count >= 1;

      if (transferAllowed)
      {
        bool receiving = false;
        bool processing = false;
        bool failed = false;
        bool locked = false;
        bool aborted = false;

        foreach (Series series in seriesCollection)
        {
          Global.DataProvider.GetSeries(series);

          locked = series.IsLocked;

          if (locked)
            break;
         

          processing = series.IsProcessing;;
 
          ///////////////////////////////////////////////////////////////////////////
          // Make sure the series is not still being received as in manual mode
          //////////////////////////////////////////////////////////////////////////
          if ( series.IsProcess("Receiving") && !series.IsCompleted)
          {
            receiving = true;
            //break;
          } // END ... If still reeciving

          //////////////////////////////////////////////////////
          // Check for a failed state in the selected series
          //////////////////////////////////////////////////////
          if (series.IsFailed)
          {
            failed = true;
            //break;
          } // END ... If the series is failed

          //////////////////////////////////////////////////////
          // Check for a aborted state in the selected series
          //////////////////////////////////////////////////////
          if (series.IsAborted)
          {
            aborted = true;
            break;
          } // END ... If the series is failed

        } // END ... For each Series

        // Check if any of the flags are set
        transferAllowed = transferAllowed && !aborted && !failed && !processing && !receiving && !locked;

      } // END ... Check the series 

      return transferAllowed;

    } // IsTRansferAllowed()

    /// <summary>
    /// Deletes the series items in the provided collection.
    /// </summary>
    /// <param name="collection">Collection of series items being deleted.</param>
    public void Delete( List<Series> collection )
    {


      try
      {
        // Only delete if the user is allowed.
        if (!Global.CurrentUser.IsAllowed("DeleteSeries"))
          return;



        if (Viatronix.UI.MessageBox.ShowQuestion(Global.Frame, "Are you sure you want to remove the series?"))
        {

          System.Windows.Forms.Application.DoEvents();


          Study selectedStudy = (Study)m_studies.SelectedObjects[0];


          m_browser.DataProvider.Delete(m_browser.SelectedSeries);


          // re-issue search 
          m_browser.Searcher.Search();

        }


      }
      catch (LockedException ex)
      {

        if (m_series.SelectedObjects.Count == 1)
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Cannot delete the selected series, series is in use.  " + ((Series)m_series.SelectedObject).Description);
              else
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Cannot delete the selected series, series is in use.  " + ex.Message);

        return;
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Delete failed [ERROR=" + ex.Message + "]", "SeriesContextMenu", "Delete");

        if (m_series.SelectedItems.Count == 1)
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Failed to delete series  " + ((Series)m_series.SelectedObject).Description + " from the system.");
        else
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Failed to delete series  from the system " + ex.Message);

      }

     } // Delete( series )

    /// <summary>
    /// Deletes the series items in the provided collection.
    /// </summary>
    /// <param name="collection">Collection of series items being deleted.</param>
    public bool IsDeleteAllowed(List<Series> seriesCollection)
    {

      bool deleteAllowed = seriesCollection.Count > 0 && m_browser.BrowserInfo.CanDelete && Global.CurrentUser.IsAllowed("DeleteSeries");


      if (deleteAllowed)
      {
        bool receiving = false;
        bool processing = false;
        bool locked = false;

        foreach (Series series in seriesCollection)
        {

          Global.DataProvider.GetSeries(series);

          locked = series.IsLocked;

          if (locked)
            break;

          processing = series.IsProcessing;
 
          ///////////////////////////////////////////////////////////////////////////
          // Make sure the series is not still being received as in manual mode
          //////////////////////////////////////////////////////////////////////////
          if ( series.IsReceiving && ! series.IsCompleted )
          {
            receiving = true;
            //break;
          } // END ... If still receiving

        } // END ... For each Series

        // Check if any of the flags are set
        deleteAllowed = deleteAllowed && !processing && !receiving && !locked;

      } // END ... Check the series 

      return deleteAllowed; 
       
 
    } // IsDeleteAllowed( series )

    /// <summary>
    /// Selects the next series in the listview.
    /// </summary>
    /// <param name="seriesIndex">The index of the current series item.</param>
    /// <param name="studyIndex">The index of the current study item.</param>
    private void SelectNextSeries( int seriesIndex, int studyIndex )
    {
      if( seriesIndex < m_series.Items.Count )
        m_series.Items[ seriesIndex ].Selected = true;
      else if( m_series.Items.Count > 0 )
        m_series.Items[ m_series.Items.Count - 1 ].Selected = true;
      else
      {
        if( studyIndex < m_studies.Items.Count )
          m_studies.Items[ studyIndex ].Selected = true;
        else if( m_studies.Items.Count > 0 )
          m_studies.Items[ m_studies.Items.Count - 1 ].Selected = true;
      }
    } // SelectNextSeries( index )

    /// <summary>
    /// Adds the series to the collection and all the referenced series too branching out to all reference series
    /// </summary>
    /// <param name="col"></param>
    /// <param name="series"></param>
    private void AddSeriesToCollection(List<Series> col, Series series)
    {


      Study study = m_browser.BrowserProvider.GetStudy( series);

      foreach (Series rs in series.References)
      {
        Series baseSeries = null;

        if (study.Series.TryFind(rs.Uid, ref baseSeries))
        {
          AddSeriesToCollection(col, baseSeries);

          if (!col.Contains(baseSeries)) col.Add(baseSeries);
        }

      }

      if (!col.Contains(series)) col.Add(series);

    } // AddSeriesToCollection(Series series)

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the open event for the studies listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLaunchHandler( object sender, EventArgs args )
    {
      if( m_series.SelectedItems.Count == 0 || !m_browser.BrowserInfo.CanLaunch )
        return;

      try
      {

        //System.Windows.Forms.Application.DoEvents();

        //MenuItemEx item  = (MenuItemEx) sender;
        //IViewerLauncher launcher = (IViewerLauncher) item.Data;
        //m_browser.Launcher.Launch( launcher );
        m_browser.LaunchManager.Launch(m_browser.SelectedSeries);

      }
      catch (Exception e)
      {
        UI.MessageBox.ShowError("An error was encountered when launching.  Please contact service.");
        Logging.Log.Error("Viewer application could not be launched; Exception: " + e.Message, "ViewBrowser", "OnSeriesDoubleClickHandler");

      }

    } // OnOpenHandler( sender, args )


    /// <summary>
    /// Handles the delete study menu item event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDeleteHandler( object sender, EventArgs args )
    {
      Delete(m_browser.SelectedSeries);
    } // OnDeleteStudyClickHandler( sender, args )


    /// <summary>
    /// Handles the reprocess series menu item event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnProcessAsHandler( object sender, EventArgs args )
    {
      ProcessAs(m_browser.SelectedSeries);
    } // OnProcessAsHandler( sender, args )


    /// <summary>
    /// Handles the show dicom information menu item event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnDicomHandler( object sender, EventArgs args )
    {
      try
      {
  
          
          string dicomHeaderXml = m_browser.DataProvider.GetDicomHeader( m_browser.SelectedSeries[0].Uid);

          Viatronix.v3D.Dicom.UI.DicomHeaderDialog dicomHeaderDialog = new Viatronix.v3D.Dicom.UI.DicomHeaderDialog();

          dicomHeaderDialog.EnableContextMenu = false;
          dicomHeaderDialog.SetElements( (Viatronix.Dicom.Image)Viatronix.Dicom.ImageFactory.Create(dicomHeaderXml, Viatronix.Dicom.ParameterType.HEADER) );

          dicomHeaderDialog.ShowDialog();

          dicomHeaderDialog.Dispose();
       }
      catch ( Exception ex )
      {
        Logging.Log.Error( "Failed to retrieve the dicom series : " + ex.Message, "SeriesContextMenu", "OnDicomHandler");
      }
    } // OnDicomHandler( sender, args )


    /// <summary>
    /// Handles the show directory menu item event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnShowDirectoryHandler( object sender, EventArgs args )
    {
      try
      {
        Series series = m_browser.SelectedSeries[0];
        Global.DataProvider.GetSeriesPath(series);

        ProcessStartInfo info = new ProcessStartInfo(m_browser.SelectedSeries[0].Path);
        info.UseShellExecute = true;

        Process.Start( info );
      }
      catch( Exception e )
      {
        Viatronix.Logging.Log.Error("Unable to launch series directory [PATH=" + m_browser.SelectedSeries[0].Path + "] " + e.Message, "SeriesContextMenu", "OnShowDirectoryHandler");
        Viatronix.UI.MessageBox.ShowError( Global.Frame, "Unable to open the series directory." );
      }
    } // OnShowDirectoryHandler( object sender, EventArgs args )

    /// <summary>
    /// Handles the Transfer click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnTransferHandler( object sender, EventArgs args )
    {
      Transfer(m_browser.SelectedSeries);
    } // OnTransferHandler( sender, args )


    #endregion

	} // class SeriesContextMenu
} // namespace Viatronix.Console

#region revision history

// $Log: SeriesContextMenu.cs,v $
// Revision 1.53.2.7  2008/09/15 19:22:44  mkontak
// Fix for delete series showing up again
//
// Revision 1.53.2.6  2008/02/13 17:03:32  mkontak
// Added Series Delete and Transfer Buttons but made them optional through a app.config
// setting. Also allow the transfer dialog to launch by Alt-T key combination.
//
// Revision 1.53.2.5  2008/02/11 14:18:15  mkontak
// FIx 2 same orientation opening
//
// Revision 1.53.2.4  2007/09/10 17:18:25  mkontak
// Added aborted check
//
// Revision 1.53.2.3  2007/05/10 21:21:28  romy
// fixed issue#5652
//
// Revision 1.53.2.2  2007/05/10 20:23:46  mkontak
// Issue 5649
//
// Revision 1.53.2.1  2007/03/23 15:11:46  mkontak
// Handle locked studies and series
//
// Revision 1.53  2007/03/06 19:00:44  gdavidson
// Added class comments
//
// Revision 1.52  2007/02/28 17:32:25  gdavidson
// Issue #5433: Reorderer the launch menus
//
// Revision 1.51  2007/02/09 13:33:50  mkontak
// Retreive the series dependencies instead of using the ones already in the list prior to
// deletionb top make sure the list in not stale.
//
// Revision 1.50  2007/01/19 20:53:01  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.49  2006/11/14 16:01:16  mkontak
// Fix to prevent null series being added to coolection
//
// Revision 1.48  2006/10/30 16:23:14  gdavidson
// Changed IViewerController to IViewerLauncher
//
// Revision 1.47  2006/10/27 19:29:30  mkontak
// Added allowItemSelection to transfer providers
//
// Revision 1.46  2006/08/21 12:36:48  mkontak
// Issue 4948: Processing volume.dicom series.
//
// Revision 1.45  2006/07/24 18:50:35  mkontak
// Issue 4393
//
// Revision 1.44  2006/07/18 14:52:35  mkontak
// Fix rights and transferring into the system
//
// Revision 1.43  2006/06/06 19:47:24  mkontak
// changed deprecated code
//
// Revision 1.42  2006/05/05 20:41:48  gdavidson
// Issue #4767: Added functionality to deactivate Process As conext menu item
//
// Revision 1.41  2006/05/05 19:52:00  mkontak
// Check for a state of processing just in case the viewe has not been updatred
//
// Revision 1.40  2006/05/05 13:50:19  mkontak
// Fix proecss as and allow deletion of failed series
//
// Revision 1.39  2006/05/04 19:54:00  mkontak
// Issue 4765
//
// Revision 1.38  2006/05/04 19:18:19  mkontak
// Added additional code to disable menu selections base on the selected
// objects states such as receiving, failed and processing.
//
// Revision 1.37  2006/05/01 19:32:40  mkontak
// *** empty log message ***
//
// Revision 1.36  2006/05/01 17:27:27  mkontak
// Issue 4746
//
// Revision 1.35  2006/04/28 19:25:44  mkontak
// No longer allow any operation  if the series is being received
//
// Revision 1.34  2006/04/28 13:02:53  romy
// Error checks added in DICOM Handler
//
// Revision 1.33  2006/04/27 19:46:04  mkontak
// Make field non case sensitive
//
// Revision 1.32  2006/04/19 14:18:49  mkontak
// Added additional field OPERATION to allow for finer granularity of the locks
//
// Revision 1.31  2006/03/22 05:16:03  romy
// lockmanager modified
//
// Revision 1.30  2006/03/20 15:12:46  mkontak
// New process as for the study browser
//
// Revision 1.29  2006/03/17 18:26:54  romy
// lock added
//
// Revision 1.28  2006/02/24 14:00:33  romy
// GUI standards
//
// Revision 1.27  2006/02/23 21:14:07  romy
// spell checks and GUI Standards
//
// Revision 1.26  2006/02/21 17:14:57  gdavidson
// Added ellipses to menu items that open dialogs
//
// Revision 1.25  2006/02/16 12:51:11  mkontak
// Transfer for PACS query to push to our dicom servers
//
// Revision 1.24  2006/02/14 16:47:59  gdavidson
// Disallowed transfer option if multiple studies are selected
//
// Revision 1.23  2006/02/07 18:05:03  gdavidson
// Disabled transfer context menu if no series are selected.
//
// Revision 1.22  2006/01/31 21:20:00  romy
// added attributes to view browser
//
// Revision 1.21  2006/01/31 19:51:31  romy
// spell check for message box
//
// Revision 1.20  2006/01/26 21:06:23  romy
// auditing added
//
// Revision 1.19  2006/01/26 18:22:28  romy
// modifed transfer desgin
//
// Revision 1.18  2006/01/09 19:50:17  gdavidson
// Added multiple selection of Study objects.
//
// Revision 1.17  2006/01/03 16:01:50  gdavidson
// Removed DataProvider's RetrieveData calls.
//
// Revision 1.16  2005/12/14 19:57:43  gdavidson
// Added submenus for the possible launch options.
//
// Revision 1.15  2005/12/12 21:12:27  romy
// Transfer revisited
//
// Revision 1.14  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.13  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.12  2005/11/23 12:13:47  mkontak
// Using new MessageBox implementation
//
// Revision 1.11  2005/11/23 02:07:25  romy
// added series deletion from dataprovider series collection upon delete
//
// Revision 1.10  2005/11/22 17:54:36  gdavidson
// Set the parent form for the message boxes created by the delete method
//
// Revision 1.9  2005/11/17 23:12:01  gdavidson
// Added exception handling around the show directory's process start
//
// Revision 1.8  2005/11/16 19:55:37  romy
// exception handling added for delete
//
// Revision 1.7  2005/11/16 13:55:47  gdavidson
// Allowed multiselect for series.
//
// Revision 1.6  2005/11/15 17:12:33  romy
// checks the referenced series count before delete
//
// Revision 1.5  2005/11/08 21:39:36  gdavidson
// Added methods for deletion.
//
// Revision 1.4  2005/11/04 21:16:38  romy
// added transfer from the series level.
//
// Revision 1.3  2005/11/03 19:30:10  romy
// added Transfer in the series Level.
//
// Revision 1.2  2005/11/02 14:21:41  gdavidson
// Added permission checks for the menu items.
//
// Revision 1.1  2005/10/26 19:51:09  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/menus/SeriesContextMenu.cs,v 1.53.2.7 2008/09/15 19:22:44 mkontak Exp $
// $Id: SeriesContextMenu.cs,v 1.53.2.7 2008/09/15 19:22:44 mkontak Exp $

#endregion
