// $Id: ImportDialog.cs,v 1.1.2.20 2011/05/19 16:00:38 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Text;
using System.Drawing;
using System.Configuration;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;

using Viatronix.UI;
using Viatronix.Logging;
using Viatronix.Encryption;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Importer.Data;
using Viatronix.Utilities.Importer.Filters;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// GUI for performing the import.
  /// </summary>
  public partial class ImportDialog : Dialog
  {

    #region ColumnSorter class

    /// <summary>
    /// Represents a sorter for rows in a ListView.
    /// </summary>
    private class ColumnSorter : System.Collections.IComparer
    {

      #region fields

      /// <summary>
      /// The index into the row for comparison.
      /// </summary>
      private int m_columnIndex = -1;

      /// <summary>
      /// Sort ascending or descending?
      /// </summary>
      private bool m_ascending = true;

      #endregion

      #region properties

      /// <summary>
      /// Gets/sets the column index.
      /// </summary>
      public int ColumnIndex
      {
        get { return m_columnIndex; }
        set { m_columnIndex = value; }
      } // ColumnIndex


      /// <summary>
      /// Gets/sets the sorting mode.
      /// </summary>
      public bool Ascending
      {
        get { return m_ascending; }
        set { m_ascending = value; }
      } // Ascending

      #endregion

      #region methods

      /// <summary>
      /// Performs a comparison on the two objects.
      /// </summary>
      /// <param name="lhs">Left hand object.</param>
      /// <param name="rhs">Right hand object.</param>
      /// <returns>If ascending l comapred to r otherwise r compared to l</returns>
      virtual public int Compare(object lhs, object rhs)
      {
        int ascValue = 1;

        if (!m_ascending)
        {
          ascValue = -1;
        }

        ListViewItem left = (ListViewItem)lhs;
        ListViewItem right = (ListViewItem)rhs;

        if (left != null && right != null)
        {
          if (m_columnIndex > -1 && m_columnIndex < left.SubItems.Count && m_columnIndex < left.SubItems.Count)
          {
            string leftStr = left.SubItems[m_columnIndex].ToString().ToLower();
            string rightStr = right.SubItems[m_columnIndex].ToString().ToLower();

            if (leftStr == rightStr)
            {
              return 0;
            }

            int value = leftStr.CompareTo(rightStr);
            return ascValue * value;
          }
          else
          {
            // Column index is not valid.
            return 0;
          }
        }
        else
        {
          // If the objects passed in aren't ListViewItems we've got big problems up above, so just return their being equal.
          return 0;
        }
      } // Compare(ListViewItem lhs, ListViewItem rhs)

      #endregion

    } // class ResultsSorter

    #endregion

    #region constants

    /// <summary>
    /// Console user login name.
    /// </summary>
    private const string CONSOLEUSER = "NOZ5vKs94KCUOo/rJCPM/Q==";

    /// <summary>
    /// Service user login name.
    /// </summary>
    private const string SERVICEUSER = "NlkL+Gm0NYWp/dVDbc1rNQ==";

    #endregion

    #region events

    /// <summary>
    /// Notifies that the import should be cancelled.
    /// </summary>
    public event EventHandler Cancelled;

    #endregion

    #region fields

    /// <summary>
    /// The list of providers mapped to the name security/data/pacs.
    /// </summary>
    private static List<IProvider> m_providers = new List<IProvider>();

    /// <summary>
    /// Security provider
    /// </summary>
    private static  ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// Are we currently importing?
    /// </summary>
    private bool m_currentlyImporting = false;

    /// <summary>
    /// Did the user cancel out?
    /// </summary>
    private bool m_cancelled = false;

    /// <summary>
    /// Sorts the columns depending on which column was clicked.
    /// </summary>
    private ColumnSorter m_sorter = new ColumnSorter();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    public ImportDialog(ImporterArgs args)
    {
      InitializeComponent();

      Global.Progress += new ProgressEventHandler(OnProgressHandler);

      DirectoryImportStatusRecord.StatusSet += new StatusDelegate(this.OnStatusSetHandler);
      Global.MainDialog = this;
      m_directoryStatusListView.ListViewItemSorter = (System.Collections.IComparer)m_sorter;
            
      string dest = Registry.SystemSettings.CurrentSystemSettings.CacheBasePath;
      if( args.Destination != null && Directory.Exists(args.Destination) && args.IsDebugEnabled && args.PerformCopy )
      {
        dest = args.Destination;

        m_destinationLabel.Visible = true;
        m_destinationText.Visible = true;
        m_destinationNavButton.Visible = true;
      }

      // Set the text to either the arugment or the default.
      m_destinationText.Text = dest;

 
      // ==========================================================================================
      // If the renewal value is not present or 0 in the secuirt providers xml attrobites 
      // list then the timer is disabled.
      // ==========================================================================================
      IProvider provider = null;

      if (m_providers.TryFind(ProviderType.Renewal, ref provider))
        ((IRenewalProvider)provider).RenewalFailed += new RenewalEventHandler(OnRenewalFailed);


      ISecurityProvider securityProvider = (ISecurityProvider)provider;
      if (securityProvider.User.IsService)
      {
        m_copyCheckBox.Visible = true;
        m_copyCheckBox.Enabled = true;

        m_databaseInsertCheckBox.Visible = true;
        m_databaseInsertCheckBox.Enabled = true;

        m_includeSessionCheckBox.Visible = true;
        m_includeSessionCheckBox.Enabled = true;
      }
      else
      {
        m_copyCheckBox.Visible = false;
        m_copyCheckBox.Enabled = false;

        m_databaseInsertCheckBox.Visible = false;
        m_databaseInsertCheckBox.Enabled = false;

        m_includeSessionCheckBox.Visible = false;
        m_includeSessionCheckBox.Enabled = false;
      }
    } // ImportDialog( args )

    #endregion

    #region methods

    #region IRenewalProvider methods

    /// <summary>
    /// Handles the cross thread on a RenewalFailed event
    /// </summary>
    /// <param name="message"></param>
    private void RenewalFailedHander(string message)
    {
      if (this.InvokeRequired)
      {
        Invoke(new Viatronix.UI.SetterDelegate<string>(this.RenewalFailedHander), message);
      }
      else
      {
        System.Windows.Forms.MessageBox.Show(this, "Renewal failed : " + message + "\r\n\r\n Application will be closed", "Renewal Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        this.Close();
      }

    }

    /// <summary>
    /// Handles the renewal failed event from the IRenewalProvider
    /// </summary>
    /// <param name="args"></param>
    private void OnRenewalFailed(ProviderEventArgs args)
    {
      RenewalFailedHander(args.Message);
    }

    #endregion

    /// <summary>
    /// Add the directory/status to the list view.
    /// </summary>
    /// <param name="directory">The directory that was imported.</param>
    /// <param name="status">Result of import.</param>
    private void AddDirectoryStatus(string directory, Status status)
    {
      string directoryName = Path.GetFileName(directory);

      if (directoryName == string.Empty)
      {
        directoryName = directory;
      }

      bool found = false;
      foreach (ListViewItem item in m_directoryStatusListView.Items)
      {
        if (item.SubItems[0].Text.ToLower() == directoryName.ToLower())
        {
          // Item was added previously.
          item.SubItems[1].Text = status.ToString();
          found = true;
          break;
        }
      }

      if (!found)
      {
        // Item was not added yet.
        ListViewItem item = new ListViewItem(directoryName);
        item.SubItems.Add(status.ToString());
        m_directoryStatusListView.Items.Add(item);
      }
    } // AddDirectoryStatus( directory, status )


    /// <summary>
    /// Handles the closing of the form to cancel if importing.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnClosing(CancelEventArgs args)
    {
      base.OnClosing(args);

      args.Cancel = m_currentlyImporting;
    } // OnClosing( args )


    /// <summary>
    /// Called when the window has closed.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnClosed(EventArgs args)
    {
      base.OnClosed(args);
      m_securityProvider.Logout();
    } // OnClosed( args )


    /// <summary>
    /// Handles progress changed to update the gui.
    /// </summary>
    /// <param name="message">The message to dipslay.</param>
    /// <param name="progress">The current progress.</param>
    private void OnProgress(string message, int progress)
    {
      m_progressBar.Text = message;
      m_progressBar.Value = progress;
    } // OnProgress( message, progress )

    #endregion

    #region main

    /// <summary>
    /// Initialize the theme.
    /// </summary>
    private static void InitializeTheme()
    {
      try
      {
        Viatronix.UI.Theme.LoadThemeFromXml( (XmlNode) ConfigurationManager.GetSection( "theme" ) );
      }
      catch ( Exception ex)
      {
        Viatronix.Logging.Log.Error( "Failed to Initialize the theme"  + ex.Message, "ImportDialog", "InitializeTheme" );
        throw ex;
      }
    } // InitializeTheme()


    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main(string[] args)
    {
      // Need to add a check such that multiple instances of the Importer can not be run.  If this is the 
      // case it will return at that point.  Get the name of the process.
      string strModName = System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName;
      string strProcName = System.IO.Path.GetFileNameWithoutExtension(strModName);

      System.Diagnostics.Process[] appProc = System.Diagnostics.Process.GetProcessesByName(strProcName);
      if( appProc.Length > 1 )
      {
        // Log a warning.
        Log.Error("Attempted to run multiple instances of Importer, bailing out.", "ImportDialog", "Main");

        // Inform the user that the open process should be closed.
        Viatronix.UI.MessageBox.ShowError("Importer is already running.\nPlease close the " +
          "running program and try again.\nIf the program is not visible please ensure that it doesn't " +
          "appear in the process table.");

        return;
      } // if( appProc.Length > 1 )

      Log.Initialize();
      Utils.CreateLogFile();
      InitializeTheme();

      m_providers = ProviderFactory.Create();

      // ==========================================================================================
      // If the renewal value is not present or 0 in the secuirt providers xml attrobites 
      // list then the timer is disabled.
      // ==========================================================================================
      IProvider provider = null;

      if (m_providers.TryFind(ProviderType.Security, ref provider))
        m_securityProvider = (ISecurityProvider)provider;
      else
        throw new ApplicationException("No security provider specified");

      ImporterArgs impArgs = ParseArguments(args);

      
      if(!m_securityProvider.IsAuthenticated)
      {
        int counter = 0;
        while (!m_securityProvider.IsAuthenticated)
        {
          LoginForm loginDialog = new LoginForm();
          DialogResult result = loginDialog.ShowDialog();
          if (result == DialogResult.OK)
          {
            try
            {
              m_securityProvider.Login(loginDialog.Username, loginDialog.Password);
            }
            catch (Exception e)
            {
              Log.Warning("Invalid username/password combination.", "ImportDialog", "Main");
              Viatronix.UI.MessageBox.ShowError("Invalid username/password combination.");
            }
          }
          else if (result == DialogResult.Cancel)
          {
            Viatronix.UI.MessageBox.ShowError("User cancelled authentication.  Importer will now exit.");
            return;
          }

          if (!m_securityProvider.IsAuthenticated && counter++ > 2)
          {
            Viatronix.UI.MessageBox.ShowError("Identity could not be confirmed.  Importer will now exit.");
            return;
          }
        }
      }

      if(impArgs.Source != string.Empty && Directory.Exists(impArgs.Source))
      {
        Utils.Import(impArgs);
      }
      else
      {
        ImportDialog dialog = new ImportDialog(impArgs);
        System.Windows.Forms.Application.Run(dialog);
      }
    } // Main(string[] args)


    /// <summary>
    /// Parses the provided commandline arguments.
    /// </summary>
    /// <param name="args">List of arguments.</param>
    /// <returns>Importer arguments.</returns>
    public static ImporterArgs ParseArguments(string[] args)
    {
      string source = string.Empty;
      string destination = string.Empty;
      bool performCopy = true;
      bool performDatabaseRecord = true;
      bool enableDebug = false;
      bool importSession = true;
      string username = string.Empty;
      string password = string.Empty;

      int i;
      for (i = 0; i < args.Length; i++)
      {
        if (args[i] == "-s" && i + 1 < args.Length)
        {
          source = args[i + 1];
        }
        else if (args[i] == "-d" && i + 1 < args.Length)
        {
          destination = args[i + 1];
        }
        else if (args[i] == "-nocopy")
        {
          performCopy = false;
        }
        else if (args[i] == "-nodb")
        {
          performDatabaseRecord = false;
        }
        else if (args[i] == "-debug")
        {
          enableDebug = true;
        }
        else if (args[i] == "-nosession")
        {
          importSession = false;
        }
        else if ((args[i] == "-u" || args[i] == "-user") && i + 1 < args.Length)
        {
          username = args[i + 1];
        }
        else if ((args[i] == "-pw" || args[i] == "-password") && i + 1 < args.Length)
        {
          password = args[i + 1];
        }
      }

      try
      {
        m_securityProvider.Login(username, password);
      }
      catch (Exception e)
      {
        Log.Warning("Could not authenticate from command line parameters.  Login window will be shown.", "ImportDialog", "ParseArguments");
      }


      ImporterArgs impArgs = new ImporterArgs((IDataProvider)m_providers.GetProvider(ProviderType.Data), source, destination, performCopy, performDatabaseRecord, importSession);
      impArgs.IsDebugEnabled = enableDebug;

      return impArgs;
    } // ParseArguments( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the result list view's column clicked event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnResultsColumnClickedHandler(object sender, ColumnClickEventArgs e)
    {
      if (e.Column > -1 && e.Column < m_directoryStatusListView.Columns.Count)
      {
        if (e.Column == m_sorter.ColumnIndex)
        {
          m_sorter.Ascending = !m_sorter.Ascending;
        }

        m_sorter.ColumnIndex = e.Column;
        m_directoryStatusListView.Sort();
      }
    } // OnResultsColumnClickedHandler( sender, e )


    /// <summary>
    /// Handles the showing of the FolderBrowser dialog and capture of desired source path.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnSourcePathButtonClick(object sender, EventArgs e)
    {
      // Show the browser for the source and if a directory was selected, make the proper text
      // make an instance to the path.
      FolderBrowserDialog dialog = new FolderBrowserDialog();
      
      // Set the active dialog to the directory in the destination text if it exists.
      if( Directory.Exists(m_sourceTexBox.Text) )
      {
        dialog.SelectedPath = m_sourceTexBox.Text;
      } // if( Directory.Exists(m_sourcePathSpec.Text) )

      dialog.ShowDialog();

      // Was a folder selected?
      if (dialog.SelectedPath != null && !dialog.SelectedPath.Equals("") )
      {
        m_sourceTexBox.Text = dialog.SelectedPath;
      } // if (dialog.SelectedPath != null && !dialog.SelectedPath.Equals("") )
    } //OnSourcePathButtonClick()


    /// <summary>
    /// Handles the destination button clicked event by showing a folder browser dialog.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnDestinationNaviatorClickHandler(object sender, EventArgs e)
    {
      // Show the browser for the source and if a directory was selected, make the proper text
      // make an instance to the path.
      FolderBrowserDialog dialog = new FolderBrowserDialog();

      // Set the active dialog to the directory in the destination text if it exists.
      if( Directory.Exists(m_destinationText.Text) )
      {
        dialog.SelectedPath = m_destinationText.Text;
      } // if( Directory.Exists(m_sourcePathSpec.Text) )

      dialog.ShowDialog();

      // Was a folder selected?
      if( dialog.SelectedPath != null && !dialog.SelectedPath.Equals("") )
      {
        m_destinationText.Text = dialog.SelectedPath;
      } // if (dialog.SelectedPath != null && !dialog.SelectedPath.Equals("") )
    } // OnDestinationNaviatorClickHandler(object sender, EventArgs e)


    /// <summary>
    /// Parses the directory and it's subdirectories (excluding their subdirectories) for study and patient
    /// information.  
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnImportClick(object sender, EventArgs e)
    {
      m_cancelled = false;
      m_directoryStatusListView.Items.Clear();
      
      try
      {
        string source = m_sourceTexBox.Text;
        string dest = m_destinationText.Text;

        // Make sure the user gave enough information in the form.
        if( !source.Equals(string.Empty) && Directory.Exists(source))
        {
          // Check existing problem cases.
          if (Directory.GetFiles(source, "study.xml").Length > 0)
          {
            // User chose the study directory as the top level. Log a failure and return.

            Viatronix.UI.MessageBox.ShowError("Importing from a study directory is not supported.  Please import from the patient level directory.\ne.g. LASTNAME_FIRSTNAME_MEDICALID");
            Log.Error("User selected study directory, import failed!", "ImportDialog", "OnImportClick");
            return;
          }
          else if (Directory.GetFiles(source, "series.xml").Length > 0)
          {
            // User chose the series directory as the top level. Log a failure and return.

            Viatronix.UI.MessageBox.ShowError("Importing from a series directory is not supported.  Please import from the patient level directory.\ne.g. LASTNAME_FIRSTNAME_MEDICALID");
            Log.Error("User selected series directory, import failed!", "ImportDialog", "OnImportClick");
            return;
          }

          m_currentlyImporting = true;

          m_buttonImport.Enabled = false;
          m_copyCheckBox.Enabled = false;
          m_databaseInsertCheckBox.Enabled = false;
          m_destinationLabel.Enabled = false;
          m_destinationNavButton.Enabled = false;
          m_destinationText.Enabled = false;
          m_includeSessionCheckBox.Enabled = false;
          m_sourceLabel.Enabled = false;
          m_sourceNavButton.Enabled = false;
          m_sourceTexBox.Enabled = false;
          //ShowSystemMneu = false;

          m_buttonCancel.Text = "Cancel";

          // Start the pulse.
          m_progressBar.Show();

          // Change the cursor to waiting.
          this.Cursor = Cursors.WaitCursor;

          string destination;

          if (m_destinationText.Visible)
          {
            destination = m_destinationText.Text;
          } // if(m_destinationPathSpec.Visible)
          else
          {
            destination = Registry.SystemSettings.CurrentSystemSettings.CacheBasePath;
          } // else

          ImporterArgs impArgs = new ImporterArgs((IDataProvider)m_providers.GetProvider(ProviderType.Data), m_sourceTexBox.Text, destination, m_copyCheckBox.Checked, m_databaseInsertCheckBox.Checked, m_includeSessionCheckBox.Checked);
          m_bgWorker.RunWorkerAsync(impArgs);
        } // if (!source.Equals(string.Empty) && Directory.Exists(Path.GetDirectoryName(source)) ...
        else
        {
          // Show the appropriate message box describing the error.
          if (source.Equals(string.Empty))
          {
            Viatronix.UI.MessageBox.Show(this, "Please enter a folder to import from.", "", MessageBoxButtons.OK, MessageBoxIcon.Error);
          } // else if (source == null || source.Equals(string.Empty))

          else if( !Directory.Exists(source))
          {
            Viatronix.UI.MessageBox.Show(this, "Source directory: " + source + " doesn't exist.", "", MessageBoxButtons.OK, MessageBoxIcon.Error);
          } // else if (!Directory.Exists(source))
        } // else
      }
      catch (Exception)
      {
        Viatronix.UI.MessageBox.ShowError(this, "An error occurred in the import process, please contact Customer Service.");
      }
    } // OnImportClick(object sender, EventArgs e)


    /// <summary>
    /// Signifies that the background worker is done working.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnWorker1WorkCompleted(object sender, RunWorkerCompletedEventArgs e)
    {
      m_currentlyImporting = false;
      m_buttonCancel.Enabled = true;

      m_buttonImport.Enabled = true;
      m_copyCheckBox.Enabled = true;
      m_databaseInsertCheckBox.Enabled = true;
      m_destinationLabel.Enabled = true;
      m_destinationNavButton.Enabled = true;
      m_destinationText.Enabled = true;
      m_includeSessionCheckBox.Enabled = true;
      m_sourceLabel.Enabled = true;
      m_sourceNavButton.Enabled = true;
      m_sourceTexBox.Enabled = true;
      //ShowSystemMneu = true;

      m_buttonCancel.Text = "Close";

      // Reset the cursor.
      this.Cursor = Cursors.Default;

      // Stop the pulse.
      m_progressBar.Hide();

      int succeeded = 0;
      int failed = 0;

      foreach (KeyValuePair<string, DirectoryImportStatusRecord> keyPair in Global.DirectoryToRecord)
      {
        if (keyPair.Value.Status == Status.Successful)
        {
          ++succeeded;
        }
        else if(keyPair.Value.Status == Status.Failed)
        {
          ++failed;
        }
      }

      string msg = string.Empty;

      if (m_cancelled)
      {
        msg += "Import Canceled.  ";
      }

      msg += succeeded + " patient(s) imported, " + failed + " patient(s) failed.";

      if (failed > 0)
      {
        msg += "  Please contact Customer Service.";
      }

      Viatronix.UI.MessageBox.ShowWarning(msg);
    } // OnWorker1WorkCompleted(object sender, RunWorkerCompletedEventArgs e)


    /// <summary>
    /// Sets the background worker to get to work.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void BackgroundWorker1DoWork(object sender, DoWorkEventArgs e)
    {
      // Set up the source and destination prarmeters.
      ImporterArgs args = e.Argument as ImporterArgs;
      if (args != null)
      {
        Utils.Import(args);
      }
    } // BackgroundWorker1DoWork(object sender, DoWorkEventArgs e)


    /// <summary>
    /// Handles the import status set event.
    /// </summary>
    /// <param name="directory">The directory that was attempted to import.</param>
    /// <param name="status">What was the status of the import.</param>
    private void OnStatusSetHandler(string directory, Status status)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(new StatusDelegate(this.AddDirectoryStatus), directory, status);
      }
      else
      {
        AddDirectoryStatus(directory, status);
      }
    } // OnImportStatusSetHandler( directory, status )


    /// <summary>
    /// Handle the cancel button's clicked event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnCancelClickedHandler(object sender, EventArgs e)
    {
      if (m_currentlyImporting)
      {
        Global.NotifyProgress("Import cancelled, this may take a moment.", 100);
        m_buttonCancel.Enabled = false;
        m_cancelled = true;

        if (this.Cancelled != null)
        {
          this.Cancelled(this, EventArgs.Empty);
        }
      }
      else
      {
        System.Windows.Forms.Application.Exit();
      }
    } // OnCancelClickedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles proress was notified.
    /// </summary>
    /// <param name="message"></param>
    /// <param name="progress"></param>
    private void OnProgressHandler(string message, int progress)
    {
      if (InvokeRequired)
      {
        Invoke(new Viatronix.Utilities.Importer.Data.ProgressEventHandler(OnProgress), message, progress);
      }
      else
      {
        OnProgress(message, progress);
      }
    } // OnProgressHandler( message, progress )

    #endregion

  } // ImportDialog
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: ImportDialog.cs,v $
// Revision 1.1.2.20  2011/05/19 16:00:38  kchalupa
// CONS-1640
// Call Logger's Initialize instead of manually getting the section.  Removed unnecessary logs directory check.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.19  2011/05/18 18:07:51  kchalupa
// CONS-1640
// Updated the references to the SystemSettings to the new data structures.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.18  2011/05/06 15:12:22  kchalupa
// Made logger settings configurable from the config file.  This includes the conscise log and verifier log.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.17  2010/05/20 15:08:40  kchalupa
// Issue: CONS-15 Cancel Button Exits the program when not currently importing.
//
// Revision 1.1.2.16  2009/11/12 15:07:30  kchalupa
// Update exception message box.
//
// Revision 1.1.2.15  2009/11/11 21:47:47  kchalupa
// Corrected error message boxes.
//
// Revision 1.1.2.14  2009/11/05 21:24:40  kchalupa
// Allow importing from CD from the Importer main dialog.
//
// Revision 1.1.2.13  2009/10/30 13:59:33  kchalupa
// Added cancel functionality, status list box for status and sorting for the status list box.
//
// Revision 1.1.2.12  2009/10/28 14:57:37  kchalupa
// Use the console theme for the importer dialogs.
//
// Revision 1.1.2.11  2009/10/26 20:34:02  kchalupa
// Break the study message into two halves.
//
// Revision 1.1.2.10  2009/10/26 19:41:22  kchalupa
// Added CD Import commandline option.
//
// Revision 1.1.2.9  2009/10/20 14:01:36  kchalupa
// Restricted import on CD, study and series directories, show a message box.
//
// Revision 1.1.2.8  2009/06/04 19:01:48  kchalupa
// Show import status dialog if service user; coding standards.
//
// Revision 1.1.2.7  2009/02/25 15:14:43  kchalupa
// Included message for invalid import from logical drive.
//
// Revision 1.1.2.6  2009/02/23 20:18:54  kchalupa
// Coding standards
//
// Revision 1.1.2.5  2009/02/23 20:05:55  kchalupa
// Importer enhancement.
//
// Revision 1.1.2.4  2008/10/15 14:36:58  kchalupa
// Added Login Functionality Enhancement
//
// Revision 1.1.2.3  2008/10/13 15:25:02  kchalupa
// Capability to launch importer from console was enhanced.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ImportDialog.cs,v 1.1.2.20 2011/05/19 16:00:38 kchalupa Exp $
// $Date: 2011/05/19 16:00:38 $24 July 2008
// $Id: ImportDialog.cs,v 1.1.2.20 2011/05/19 16:00:38 kchalupa Exp $000000004

#endregion