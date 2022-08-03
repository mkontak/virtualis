// $Id: VCSessionViewer.cs,v 1.12.2.3 2011/05/18 01:57:02 mkontak Exp $
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
using System.Drawing;
using System.Reflection;
using System.Collections;
using System.Diagnostics;
using System.Configuration;
using System.Drawing.Imaging;
using System.Collections.Generic;
using System.Windows.Forms;

using Viatronix.Console.Entities;
using Viatronix.UI;
using Viatronix.Logging;
using Viatronix.Reporting;
using Viatronix.Utilities.Registry;
using Viatronix.Utilities;

namespace Viatronix.Console
{
  /// <summary>
  /// Displays the snapshots, movies and reports contained in a session series.
  /// </summary>
  public class DefaultSessionViewer : SessionViewer, IDisposable
  {

    #region constants

    /// <summary>
    /// Extensions for vc movies.
    /// </summary>
    private static readonly string[] MovieSearchPatterns = new string[] { "*.wmv", "*.avi" };

    #endregion

    #region fields

    /// <summary>
    /// The list of snapshots.
    /// </summary>
    private SnapshotCollection m_snapshots = new SnapshotCollection();

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// The previously selected tab.
    /// </summary>
    private SelectedTab m_previousTab = SelectedTab.Snapshots;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DefaultSessionViewer()
    {
    } // DefaultSessionViewer()


    /// <summary>
    /// Dispose of the object.
    /// </summary>
    /// <param name="disposing"></param>
    protected override void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {
          // Unmanaged
        }
      }

      m_disposed = true;
    } // Dispose(bool bDispoing)

    #endregion

    #region properties

    /// <summary>
    /// Retrusn the default selected tab.
    /// </summary>
    protected override SessionViewer.SelectedTab DefaultTab
    {
      get { return SelectedTab.Snapshots; }
    } // DefaultTab


    /// <summary>
    /// Gets if snapshots exist.
    /// </summary>
    protected bool SnapshotsExist
    {
      get { return Directory.Exists(Path.Combine(Series.Url, "Snapshots")) && Directory.GetFiles(Path.Combine(Series.Url, "Snapshots"), "*.png").Length > 0; }
    } // SnapshotsExist


    /// <summary>
    /// Gets if movies exist.
    /// </summary>
    protected bool MoviesExist
    {
      get
      {
        string moviesDir = Path.Combine(this.Series.Path, "movies");
        if (System.IO.Directory.Exists(moviesDir))
        {
          foreach (string pattern in MovieSearchPatterns)
          {
            if (System.IO.Directory.GetFiles(moviesDir, pattern).Length > 0)
            {
              return true;
            }
          }
        }

        return false;
      }
    } // MoviesExist


    /// <summary>
    /// Gets if the report exists.
    /// </summary>
    protected bool ReportExists
    {
      get { return Directory.Exists(Path.Combine(Series.Url, "Reports")) && Directory.GetFiles(Path.Combine(Series.Url, "Reports"), "report.pdf").Length > 0; }
    } // ReportExists

    #endregion

    #region methods

    /// <summary>
    /// Initializes the tabs.
    /// </summary>
    protected override void Initialize()
    {
      try
      {
        this.TabStrip.Items[(int)SelectedTab.Snapshots].Enabled = SnapshotsExist;
        this.TabStrip.Items[(int)SelectedTab.Movies].Enabled = MoviesExist;
        this.TabStrip.Items[(int)SelectedTab.Report].Enabled = ReportExists;
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Unable to initialize session provider. [ERROR={0}]", e.Message), "VCSessionViewer", "Initialize");
      }
    } // Initialize()


    /// <summary>
    /// Shows the content for the specified tab.
    /// </summary>
    /// <param name="tab">Selected tab</param>
    protected override void DisplayTab(SessionViewer.SelectedTab tab)
    {
      // Tab is a special case.
      if (tab == SelectedTab.Report && ReportExists)
      {
        OnReportOpen(0);
        TabStrip.SelectedIndex = m_previousTab != SelectedTab.Report ? (int)m_previousTab : (int)SelectedTab.Snapshots;
        return;
      }

      this.ImageViewer.BeginInit();
      this.ImageViewer.Images.Clear();

      if (tab == SelectedTab.Snapshots)
        LoadSnapshots();

      else if (tab == SelectedTab.Movies)
        LoadMovies();

      this.ImageViewer.EndInit();
      this.ImageViewer.Refresh();

      m_previousTab = tab;
    } // DisplayTab( tab )


    /// <summary>
    /// Opens the specified image in a standalone viewer.
    /// </summary>
    /// <param name="snapshot">The snapshot image.</param>
    protected override void OnSnapshotOpen(Snapshot snapshot)
    {
      try
      {
        ProcessStartInfo info = new ProcessStartInfo(snapshot.Filename);
        info.WorkingDirectory = Path.GetDirectoryName(snapshot.Filename);
        Process.Start(info);
      }
      catch (Exception e)
      {
        Viatronix.UI.MessageBox.ShowError(Global.Frame, "Unable to display image.");
        Log.Error("Unable to display the selected snapshot in an external viewer [PATH=" + snapshot.Filename + "] [ERROR=" + e.Message + "]", "VCSessionViewer", "OnSnapshotOpen");
      }
    } // OnSnapshotOpen( snapshot )


    /// <summary>
    /// Opens the movie at the specified location in a standalone viewer.
    /// </summary>
    /// <param name="movie">The movie object.</param>
    protected override void OnMovieOpen(Movie movie)
    {
      // spawn new process
      Process process = new Process();
      process.StartInfo.RedirectStandardOutput = false;
      process.StartInfo.WorkingDirectory = Path.GetDirectoryName(movie.Filename);

      process.StartInfo.FileName = movie.Filename;
      process.StartInfo.UseShellExecute = true;
      process.Start();
    } // OnMovieOpen( movie )


    /// <summary>
    /// Opens the specified report page in a standalone viewer.
    /// </summary>
    /// <param name="index">The index of the selected page.</param>
    protected override void OnReportOpen(int index)
    {
      if (ReportExists)
      {
        try
        {
          ProcessStartInfo info = new ProcessStartInfo(Path.Combine(Series.Url, "Reports", "report.pdf"));
          Process.Start(info);
        }
        catch (Exception e)
        {
          Viatronix.UI.MessageBox.ShowError(Global.Frame, "Unable to display report.");
          Log.Error(string.Format("Unable to display the report in an external viewer.  [ERROR={1}]", e.Message), "VCSessionViewer", "OnReportOpen");
        }
      }
    } // OnReportOpen( index )


    /// <summary>
    /// Loads the Snapshots from the report.
    /// </summary>
    private void LoadSnapshots()
    {
      if (SnapshotsExist)
      {
        foreach (string file in Directory.GetFiles(Path.Combine(Series.Url, "Snapshots"), "*.png"))
        {
          using (FileStream fs = new FileStream(file, FileMode.Open))
          {
            Image image = Image.FromStream(fs);

            Snapshot snapshot = new Snapshot(image);
            snapshot.Filename = file;
            m_snapshots.Add(snapshot);
            ImageViewer.Images.Add(image);
          }
        }
      }
      else
      {
        foreach (Snapshot snapshot in m_snapshots)
        {
          ImageViewer.Images.Add(snapshot.Image);
        }
      }

      ImageViewer.Tag = m_snapshots;
    } // LoadSnapshots()


    /// <summary>
    /// Loads the movies associated with the report.
    /// </summary>
    private void LoadMovies()
    {
      Bitmap defaultImage = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.Console.res.Movie.png"));

      MovieCollection movies = new MovieCollection();
      string moviesDir = Path.Combine(this.Series.Path, "movies");
      foreach (string extension in MovieSearchPatterns)
      {
        foreach (string file in System.IO.Directory.GetFiles(moviesDir, extension))
        {
          Movie movie = new Movie();
          movie.Filename = file;

          string movieFile = Path.Combine(Path.GetDirectoryName(file), Path.GetFileNameWithoutExtension(file) + ".jpg");

          if (File.Exists(movieFile))
          {
            // If a preview image does not exist.  Console crashes.
            using (Image image = Image.FromFile(movieFile))
            {
              movie.PreviewImage = new Bitmap(image);
            } // using( Image image = Image.FromFile( movieFile ))
          } // if(File.Exists(movieFile)
          else
          {
            movie.PreviewImage = defaultImage;
          } // else

          movies.Add(movie);

          this.ImageViewer.Images.Add(movie.PreviewImage);
        }
      }

      this.ImageViewer.Tag = movies;
    } // LoadMovies()

    #endregion

  } // class VCSessionViewer
} // namespace Viatronix.Console

#region revision history

// $Log: VCSessionViewer.cs,v $
// Revision 1.12.2.3  2011/05/18 01:57:02  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.2  2008/09/16 17:13:29  kchalupa
// Issue
// 6150  No Movie Preview Console Crashes
// VCSessionViewer
// 6122 VC Sessions Not Showing Completed
// VCLauncher
// 6073 Each Movie (VC) Is Listed Twice On Patient CD
// VCSessionProvider
// 6153 Movies that have no preview do not show up in Patient CD HTML Report
// RemovableMediaForPatient
// ReportTags
//
// Revision 1.14  2008/09/08 17:00:15  kchalupa
// Issue #6073
// If a preview image for a movie does not exist
// console puts a default image in it's place.
//
// Revision 1.13  2008/09/08 14:12:57  kchalupa
// Issue #6801
// Added Dispose method to clean up the temp
// directories that are created whilst browsing
// the sessions.
//
// Revision 1.12  2006/07/28 13:15:43  mkontak
// Changed the VRX report creator
//
// Revision 1.11  2006/07/27 14:10:19  mkontak
// Fixed problem with the VC VRX file being locked with reading
//
// Revision 1.10  2006/07/24 13:30:10  gdavidson
// Issue #4873: Select movie tab when report and snapshots fail
//
// Revision 1.9  2006/07/20 14:55:28  gdavidson
// Issue #4832: corrected exception handling when displaying a report
//
// Revision 1.8  2006/07/11 18:14:19  gdavidson
// Issue #4832: Reworked SessionViewer
//
// Revision 1.7  2006/06/30 14:18:10  gdavidson
// Issue #4832: Invoke on main thread when modifying controls
//
// Revision 1.6  2006/05/12 18:34:06  gdavidson
// Issue #  4745 : Closed the report
//
// Revision 1.5  2006/05/10 19:45:25  gdavidson
// Issue #4788: Loads image associated with the movie
//
// Revision 1.4  2006/05/09 20:57:40  gdavidson
// Issue #4737: Check for movies in the movie sub directory
//
// Revision 1.3  2006/05/08 13:04:17  gdavidson
// Issue #4778: Don't display snapshot tab if the report is blank.
//
// Revision 1.2  2006/02/10 21:31:21  gdavidson
// Removed asynchronous event call for setting the active tab.
//
// Revision 1.1  2006/01/16 14:33:02  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/Browser Plugins/VCSessionViewer.cs,v 1.12.2.3 2011/05/18 01:57:02 mkontak Exp $
// $Id: VCSessionViewer.cs,v 1.12.2.3 2011/05/18 01:57:02 mkontak Exp $

#endregion
