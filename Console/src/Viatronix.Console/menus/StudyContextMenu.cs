// $Id: StudyContextMenu.cs,v 1.43.2.18 2011/05/18 01:57:01 mkontak Exp $
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
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Configuration;
using System.Runtime.InteropServices;

using BrightIdeasSoftware;

using Viatronix.UI;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.Console
{
  /// <summary>
	/// Provides all context menu options for series
	/// </summary>
  public class StudyContextMenu : Viatronix.UI.ContextMenu
  {

    #region User32 declarations

    // used for the bring to front on the importer.
    [DllImport("User32.dll")]
    public extern static int ShowWindowAsync(IntPtr hWnd, int swCommand);

    private enum ShowWindowConstants : int
    {
      SW_HIDE = 0,
      SW_SHOWNORMAL = 1,
      SW_NORMAL = 1,
      SW_SHOWMINIMIZED = 2,
      SW_SHOWMAXIMIZED = 3,
      SW_MAXIMIZE = 3,
      SW_SHOWNOACTIVATE = 4,
      SW_SHOW = 5,
      SW_MINIMIZE = 6,
      SW_SHOWMINNOACTIVE = 7,
      SW_SHOWNA = 8,
      SW_RESTORE = 9,
      SW_SHOWDEFAULT = 10,
      SW_FORCEMINIMIZE = 11,
      SW_MAX = 11,
    } // ShowWindowConstants

    #endregion

    #region fields


    /// <summary>
    /// Reference to the ViewBrowser view.
    /// </summary>
    private ViewBrowser m_browser = null;

    /// <summary>
    /// Reference to the ListView containing the study objects.
    /// </summary>
    private ObjectListView m_studies = null;

    /// <summary>
    /// Reference to the ListView containing the series objects.
    /// </summary>
    private ListView m_series = null;

    /// <summary>
    /// Menu item for opening study objects.
    /// </summary>
    private MenuItemEx m_openItem = null;

    /// <summary>
    /// Menu item for deleting study objects.
    /// </summary>
    private MenuItemEx m_deleteItem = null;

    /// <summary>
    /// Menu item for setting the group
    /// </summary>
    private MenuItemEx m_setGroupItem = null;

    /// <summary>
    /// Menu item for trasfering studies.
    /// </summary>
    private MenuItemEx m_transferItem = null;

    /// <summary>
    /// Menu item for importing studies.
    /// </summary>
    private MenuItemEx m_importItem = null;

    /// <summary>
    /// The process ID for the launched importer process.
    /// </summary>
    private int m_importerProcessID = -1;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a StudyContextMenu object.
    /// </summary>
    /// <param name="browser">Reference to the ViewBrowser view.</param>
    /// <param name="studies">Reference to the ListView contain inf studies.</param>
    public StudyContextMenu( ViewBrowser browser, ObjectListView studies, ListView series )
    {
      m_browser = browser;
      m_studies = studies;
      m_series = series;

      m_openItem = new MenuItemEx( "Open", new EventHandler( this.OnOpenHandler ));
      m_deleteItem = new MenuItemEx( "Delete", new EventHandler( this.OnDeleteHandler ));
      m_transferItem = new MenuItemEx( "Transfer...", new EventHandler( this.OnTransferHandler ));
      m_setGroupItem = new MenuItemEx("Set Group");
      m_importItem = new MenuItemEx("Import", new EventHandler(this.OnImportHandler));

      this.MenuItems.Add( m_openItem );
      this.MenuItems.Add( m_transferItem );
      this.MenuItems.Add( m_deleteItem );
      this.MenuItems.Add( "-" );
      this.MenuItems.Add( m_importItem );

      if (Global.StudyGroups.Count > 0)
      {
        // seperator
        this.MenuItems.Add(new MenuItemEx("-"));
        this.MenuItems.Add(m_setGroupItem);
        m_setGroupItem.MenuItems.Clear();
        m_setGroupItem.Enabled = false;


        foreach (StudyGroup studyGroup in Global.StudyGroups)
        {
          m_setGroupItem.MenuItems.Add(new MenuItemEx((studyGroup.Name.Length == 0 ? "Default" : studyGroup.Name), new EventHandler(this.OnSetGroupHandler)));
        }
      }

      // Link up to the closing event to close the importer only when the frame is closing.
      Global.Frame.FormClosing +=
        new System.Windows.Forms.FormClosingEventHandler(this.OnFrameClosingHandler);

    } // StudyContextMenu( launcher, transfer )

    #endregion

    #region properties

    /// <summary>
    /// The Process ID of the Importer that was launched.  -1 indicates that no importer was launched.
    /// </summary>
    public int ImporterProcessID
    {
      get { return m_importerProcessID;  }
      set { m_importerProcessID = value; }
    } // ImporterProcessID

    #endregion

    #region override methods

    /// <summary>
    /// Displays the menu items that are appropriate to the selected listview item.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnPopup( EventArgs args )
    {
      base.OnPopup( args );

      List<Study> studies = m_browser.SelectedStudies;

      bool receiving          = false;
      bool processing         = false;
      bool locked             = false;
      bool hasSelectedStudies = (m_browser.SelectedStudies.Count > 0);
      bool isPacsProvider = (m_browser.DataBindings is IPacsProvider);


      if (hasSelectedStudies)
      {

        if(m_browser.BrowserProvider is IDataProvider)
           m_setGroupItem.Enabled = true;

        m_importItem.Enabled = false;
      } // if(studies.Count > 0)
      else
      {
        m_importItem.Enabled = true;    
      } // else

      if (isPacsProvider)
        m_importItem.Enabled = false;

      for ( int i = 0; i < studies.Count && ! receiving;  ++i )
      {

        // Refresh the strudies state
        m_browser.BrowserProvider.GetStudy(studies[i]);


        /// ==================================
        /// Check if the study is processing
        /// ==================================
        if ( (processing = studies[i].IsProcessing) == true )
          break;

        /// ===============================
        /// Check if the study is locked
        /// ===============================
        if ((locked = studies[i].IsLocked) == true)
          break;

        /// =================================
        /// Check if the study is receiving
        /// =================================
        if ((receiving = studies[i].IsState(States.Receiving)) == true)
          break;


      } // END ... For each study selecetd

      //////////////////////////////////////////////////////////////////////////////////////////////////////
      // Set the snabled flag based of the number of studies selected the receiving flag and the processing 
      // flag
      //////////////////////////////////////////////////////////////////////////////////////////////////////
      bool enabled = hasSelectedStudies && !receiving && !processing && !locked;


      // Open Study
      m_openItem.Enabled = enabled && m_browser.BrowserInfo.CanLaunch;

      // Transfer Study
      m_transferItem.Enabled = enabled && (studies.Count == 1) && m_browser.BrowserInfo.CanTransfer && Global.CurrentUser.IsAllowed("Transfer") && !isPacsProvider;

      // Delete Study
      m_deleteItem.Enabled = hasSelectedStudies && !receiving && !processing && !locked && m_browser.BrowserInfo.CanDelete && Global.CurrentUser.IsAllowed("DeleteStudy");

    } // OnPopup( args )


    /// <summary>
    /// Handles the console's frame Closing event to close the importer, if it was launched from the console.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFrameClosingHandler(object sender, EventArgs args)
    {
      Process[] appProc = Process.GetProcessesByName(GetImporterProcessName());
      if( appProc.Length > 0 && appProc[0].Id == m_importerProcessID )
      {
        Viatronix.UI.MessageBox.ShowInfo("Console is closing down, the importer process will be closed as well.");
        Logging.Log.Information("Importer was killed off when the ViewBrowser was closed.", "StudyContextMenu", "OnFrameClosingHandler");
        appProc[0].Kill();
      }
    } // OnFrameClosingHandler( sender, args )

    #endregion

    #region methods

    /// <summary>
    /// Gets the importer's process name from the config file.
    /// </summary>
    /// <returns>The importer's process name.</returns>
    private string GetImporterProcessName()
    {
      return Path.GetFileNameWithoutExtension(ConfigurationManager.AppSettings["Importer"]);
    } // GetImporterProcessName()

    /// <summary>
    /// 
    /// </summary>
    public void Transfer(bool checkSeries)
    {
      if (m_browser.SelectedStudies.Count != 1 || !m_browser.BrowserInfo.CanTransfer || !Global.CurrentUser.IsAllowed("Transfer") ) return;

      if (checkSeries)
      {
        List<Study> studies = m_browser.SelectedStudies;

        bool receiving = false;
        bool processing = false;
        bool locked = false;



        for (int i = 0; i < studies.Count && !receiving; ++i)
        {


          ////////////////////////////////////////////////////////////////////////////////////////////////////////
          // If we are processing any of the studies series then set the flag and break out of the loop we need 
          // look no more.
          ///////////////////////////////////////////////////////////////////////////////////////////////////////
          if ((processing = studies[i].IsProcessing ) == true)
          {
            break;
          } // END ... If the study is being processed



          Global.DataProvider.GetStudy(studies[i]);

 
          if (!studies[i].IsLocked)
          {
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            // Loop though all the series to see if any are still being received. This check is to make sure
            // that if we are manula mode you cannot do anything
            //////////////////////////////////////////////////////////////////////////////////////////////////
            foreach (Series series in studies[i].Series)
            {

              //////////////////////////////////////////////////
              // Check if the series is still being received.
              //////////////////////////////////////////////////
              if (series.IsProcess("Receiving") && !series.IsCompleted)
              {
                receiving = true;
                break;
              } // END ... If the series is still being received

            } // END ... For each series in the study
          } // END ... If the study is not locked
        } // END ... FOr each study selected

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Set the enabled flag based of the number of studies selected the receiving flag and the processing 
        // flag
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( receiving || processing || locked ) return;

      }

      if (m_browser.TransferProviders.Count <= 0)
      {
        Viatronix.UI.MessageBox.ShowError(Global.Frame, " Transfer not supported ");
        return;
      }


      foreach (Study study in m_browser.SelectedStudies)
      {

        // find any failed series and ask the user before transferring
        foreach (Series series in study.Series)
        {
          if (series.IsFailed)
          {
            if (Viatronix.UI.MessageBox.ShowQuestionWithWarning(Global.Frame, "One or more of the series being transferred have failed.\n" +
              "Click 'Yes' to transfer all series.\n" +
              "Click 'No' to cancel the transfer operation. If you do not want to include failed series, delete them and retry.") == false)
            {
              return;
            }

            break;
          }


          Global.DataProvider.GetReferences( series);
 
        }
      }


      // TODO: REST
      //Global.LockManager.Lock(m_browser.Studies[0]);

      //Transfer Selected study.
      //TransferManager transferManager = new TransferManager(m_browser.DataProvider);
      //if (transferManager.SelectValidTransferProvider(m_browser.TransferProviders, m_browser.SelectedStudies[0].Series, null))
      //  transferManager.Transfer();

      Viatronix.UI.MessageBox.ShowWarning("Need to implement Console launching the transfer utility.");

      // TODO: REST
      // Global.LockManager.ClearLock(m_browser.Studies[0].Uid);

    } // Transfer()


    /// <summary>
    /// Deletes the study item.
    /// </summary>
    public void Delete()
    {


      try
      {

        m_browser.StopRefreshTimer();

        // ===============================================================================================
        // Ask the if the user is sure about deleting all the studies selected or the one study selected
        // ================================================================================================
        if (m_studies.SelectedObjects.Count  > 1)
        {
          if (Viatronix.UI.MessageBox.ShowQuestion(Global.Frame, "There are " + m_studies.SelectedObjects.Count.ToString() + " studies selected for deletion. Do you want to continue ? ") == false)
            return;



        }
        else
        {

          Study study = (Study)m_studies.SelectedObject;

          if (Viatronix.UI.MessageBox.ShowQuestion(Global.Frame,
                                 "Are you sure you want to remove the selected study \'" +
                                 study.Name.LastFirstMiddle +
                                 "\'" +
                                 (string.IsNullOrEmpty(study.Description) ? "?" : ("\nwith description \'" + study.Description + "\'?"))) == false)
            return;
        }


        System.Windows.Forms.Application.DoEvents();

        m_studies.BeginUpdate();

        List<Study> studies = m_browser.SelectedStudies;

        m_browser.DataProvider.Delete(studies);

        m_studies.RemoveObjects(studies);

      }
      catch (LockedException ex)
      {

        if (  m_studies.SelectedObjects.Count == 1 )
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Cannot delete the selected study, study is in use.  " + ((Study)m_studies.SelectedObject).Name.ToString());
        else
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Cannot delete the selected studies, study is in use.  " + ex.Message);
 
        return;
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Delete failed [ERROR=" + ex.Message + "]", "StudyContextMenu", "Delete");

        if ( m_studies.SelectedItems.Count == 1  )
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Failed to delete study  " + ((Study)m_studies.SelectedObject).Name.ToString() + " from the system.");
        else
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Failed to delete studies  from the system " + ex.Message);

      }
      finally
      {
        m_studies.EndUpdate();
        m_browser.StartRefreshTimer();

      }

      // TODO: REST do we need this
      //SelectNextStudy( index );

      ///////////////////////////////////////////////////////////////////////////////////////
      // Make sure the series list view is cleared out if the last study has been deleted
      ///////////////////////////////////////////////////////////////////////////////////////
      if (m_studies.Items.Count == 0)
      {
        m_series.Items.Clear();
        m_series.Groups.Clear();
      } // END ... If there are no more studies in the list

    } // Delete( study )


    /// <summary>
    /// Selects the next study in the listview.
    /// </summary>
    /// <param name="index">The index of the current item.</param>
    private void SelectNextStudy( int index )
    {
      if( index < m_studies.Items.Count - 1 )
        m_studies.Items[ index ].Selected = true;
      else if( m_studies.Items.Count > 0 )
        m_studies.Items[ m_studies.Items.Count - 1 ].Selected = true;
    } // SelectNextStudy( index )


    /// <summary>
    /// Launches the importer from the menu item.
    /// </summary>
    private void Import()
    {
      try
      {
        //m_browser.Parent.Visible = false;
        //m_browser.Parent.Enabled = false;

        string importArgs = Viatronix.Encryption.Crypto.Decrypt(ConfigurationManager.AppSettings["ImporterArgs"]);

        System.Diagnostics.Process process = new System.Diagnostics.Process();
        process.StartInfo.FileName = Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.ParseExpression(ConfigurationManager.AppSettings["Importer"]);

        //process.StartInfo.UseShellExecute = true;
        process.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Normal;
        process.StartInfo.Arguments = importArgs;
        process.Start(); 

        m_importerProcessID = process.Id;

        //process.WaitForExit();

        //m_browser.Parent.Visible = true;
        //m_browser.Parent.Enabled = true;
      } // try
      catch(Exception e)
      {
        UI.MessageBox.ShowError("Importer could not be launched!");
        Logging.Log.Error("Importer could not be launched; Exception: " + e.Message, "StudyContextMenu", "Import");
      } // catch(Exception e)
    } // Import()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the set open event for the studies listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSetGroupHandler(object sender, EventArgs args)
    {
      if (m_studies.SelectedItems.Count == 0)
        return;

      MenuItemEx menuItem = (MenuItemEx)sender;

      foreach (ListViewItem item in m_studies.SelectedItems)
      {
        Study study = (Study)item.Tag;
        study.Group = (string.Compare(menuItem.Text, "Default", true) == 0 ? string.Empty : menuItem.Text);
        Global.DataProvider.Update(study);
      }

      m_browser.Searcher.Search();

    } // OnOpenHandler( sender, args )



    /// <summary>
    /// Handles the open event for the studies listview.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnOpenHandler( object sender, EventArgs args )
    {

      // ======================================================
      // TODO we should check if we can open multiple studies
      // ======================================================
      if( m_studies.SelectedItems.Count == 0 || ! m_browser.BrowserInfo.CanLaunch )
        return;

      try
      { 

        //System.Windows.Forms.Application.DoEvents();

        m_browser.LaunchManager.Launch(m_browser.SelectedStudies[0]);

        //m_browser.Launcher.Launch(m_browser.SelectedStudies);
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
      Delete();
    } // OnDeleteStudyClickHandler( sender, args )

    /// <summary>
    /// Handles the Transfer on a Study
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnTransferHandler( object sender, System.EventArgs args )
    {
      Transfer(false);
    } // OnTransferHandler( sender, args )

    /// <summary>
    /// Handles the Import directive.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnImportHandler(object sender, System.EventArgs args)
    {
      // Check if an importer process exists and disable the Importer menu item if one exists.
      Process[] appProc = System.Diagnostics.Process.GetProcessesByName(GetImporterProcessName());
      if (appProc.Length > 0)
      {
        ShowWindowAsync(appProc[0].MainWindowHandle, (int)(ShowWindowConstants.SW_SHOWMINIMIZED));
        ShowWindowAsync(appProc[0].MainWindowHandle, (int)(ShowWindowConstants.SW_RESTORE));
      }
      else
      {
        Import();
      }
    } // OnImportHandler(object sender, System.EventArgs args)

    #endregion

	} // class StudyContextMenu
} // namespace Viatronix.Console

#region revision history

// $Log: StudyContextMenu.cs,v $
// Revision 1.43.2.18  2011/05/18 01:57:01  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.43.2.17  2010/03/23 12:23:58  kchalupa
// Issue: CONS-3
// Deleting multiple studies gives could not retrieve series.  Need to remove ListViewItem before delete from database to ensure selection changed event doesn't break.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.43.2.16  2009/10/26 19:48:07  kchalupa
// Implelemnt pop to front if import item is clicked and importer is running.
//
// Revision 1.43.2.15  2009/10/12 20:39:23  kchalupa
// Don't allow import in PACS or CD transfer views.
//
// Revision 1.43.2.14  2009/03/03 22:05:09  mkontak
// Removed the ability to transfer at the study level
//
// Revision 1.43.2.13  2008/10/30 14:35:07  kchalupa
// Disable Import menu item if a study is selected.
//
// Revision 1.43.2.12  2008/10/13 15:25:02  kchalupa
// Capability to launch importer from console was enhanced.
//
// Revision 1.43.2.11  2008/09/28 02:36:34  mkontak
// Issu 6158
//
// Revision 1.43.2.10  2008/09/12 19:05:35  kchalupa
// Fix for the right click due to importing being removed
//
// Revision 1.43.2.9  2008/09/08 18:24:23  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
// Revision 1.43.2.8  2008/04/10 19:35:39  mkontak
// Set Groups Fix
//
// Revision 1.43.2.7  2008/02/13 17:03:32  mkontak
// Added Series Delete and Transfer Buttons but made them optional through a app.config
// setting. Also allow the transfer dialog to launch by Alt-T key combination.
//
// Revision 1.43.2.6  2008/02/01 16:22:58  mkontak
// Added ability to set the group
//
// Revision 1.43.2.4  2007/05/11 17:10:33  vxconfig
// deletepaient check for studies
//
// Revision 1.43.2.3  2007/05/11 15:42:35  vxconfig
// Added code to check the DeletePatient option before delete. #5649
//
// Revision 1.43.2.2  2007/04/20 16:34:18  mkontak
// Fix locking
//
// Revision 1.43.2.1  2007/03/23 15:11:46  mkontak
// Handle locked studies and series
//
// Revision 1.43  2007/03/06 19:00:44  gdavidson
// Added class comments
//
// Revision 1.42  2007/02/28 15:43:05  mkontak
// Issue 5431: Fix the series list not being cleared when the last study is deleted.
//
// Revision 1.41  2007/01/19 20:53:01  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.40  2006/10/30 16:22:27  gdavidson
// Movied ImportDialiog to Viatronix.Console.Core
//
// Revision 1.39  2006/08/08 14:25:15  gdavidson
// Issue #4909: Fixed the string representing the DeleteStudy right.
//
// Revision 1.38  2006/07/20 20:22:13  mkontak
// Issue 4851
//
// Revision 1.37  2006/07/18 14:52:27  mkontak
// Fix rights and transferring into the system
//
// Revision 1.36  2006/05/11 14:55:14  gdavidson
// Issue #4785: Corrected spelling
//
// Revision 1.35  2006/05/10 14:48:25  gdavidson
// Issue #4785: Display dialog box when failed series are being transfered.
//
// Revision 1.34  2006/05/05 14:02:26  mkontak
// Fix to allow the deltion of a stuy that contains failed series
//
// Revision 1.33  2006/05/04 19:18:19  mkontak
// Added additional code to disable menu selections base on the selected
// objects states such as receiving, failed and processing.
//
// Revision 1.32  2006/05/01 17:32:42  mkontak
// Fixed probelm with the import option being tied to the study list, which is
// wrong. (Issue 4751)
//
// Revision 1.31  2006/05/01 17:27:31  mkontak
// Issue 4746
//
// Revision 1.30  2006/04/28 19:25:44  mkontak
// No longer allow any operation  if the series is being received
//
// Revision 1.29  2006/04/27 19:46:04  mkontak
// Make field non case sensitive
//
// Revision 1.28  2006/04/19 14:18:49  mkontak
// Added additional field OPERATION to allow for finer granularity of the locks
//
// Revision 1.27  2006/03/22 14:03:06  romy
// no clear lock now
//
// Revision 1.26  2006/03/22 05:16:03  romy
// lockmanager modified
//
// Revision 1.25  2006/03/17 18:26:54  romy
// lock added
//
// Revision 1.24  2006/03/17 16:23:10  romy
// lock added
//
// Revision 1.23  2006/02/21 17:14:57  gdavidson
// Added ellipses to menu items that open dialogs
//
// Revision 1.22  2006/02/16 12:51:11  mkontak
// Transfer for PACS query to push to our dicom servers
//
// Revision 1.21  2006/02/09 21:46:39  romy
// Bug fixes
//
// Revision 1.20  2006/02/06 19:50:52  romy
// multiple delete with a single confirmation.
//
// Revision 1.19  2006/02/02 18:33:34  romy
// rearranged study delete description
//
// Revision 1.18  2006/02/02 14:30:58  romy
// Transfer fix
//
// Revision 1.17  2006/01/26 21:05:51  romy
// auditing added
//
// Revision 1.16  2006/01/26 18:22:28  romy
// modifed transfer desgin
//
// Revision 1.15  2006/01/09 19:50:17  gdavidson
// Added multiple selection of Study objects.
//
// Revision 1.14  2006/01/03 16:01:50  gdavidson
// Removed DataProvider's RetrieveData calls.
//
// Revision 1.13  2005/12/12 21:13:37  romy
// Transfer revisited
//
// Revision 1.12  2005/12/07 17:37:13  romy
// updated the study delete message
//
// Revision 1.11  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.10  2005/11/23 12:13:47  mkontak
// Using new MessageBox implementation
//
// Revision 1.9  2005/11/23 02:08:14  romy
// on delete show the message box with Patient name
//
// Revision 1.8  2005/11/22 17:54:36  gdavidson
// Set the parent form for the message boxes created by the delete method
//
// Revision 1.7  2005/11/16 19:55:37  romy
// exception handling added for delete
//
// Revision 1.6  2005/11/08 21:39:36  gdavidson
// Added methods for deletion.
//
// Revision 1.5  2005/11/07 16:44:02  gdavidson
// Modified the import operation.
//
// Revision 1.4  2005/11/03 19:32:11  romy
// changed Transfer Provider Type Name
//
// Revision 1.3  2005/11/02 14:21:41  gdavidson
// Added permission checks for the menu items.
//
// Revision 1.2  2005/10/27 19:11:22  romy
// added new Transfer Mechanism.
//
// Revision 1.1  2005/10/26 19:51:09  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/menus/StudyContextMenu.cs,v 1.43.2.18 2011/05/18 01:57:01 mkontak Exp $
// $Id: StudyContextMenu.cs,v 1.43.2.18 2011/05/18 01:57:01 mkontak Exp $

#endregion
