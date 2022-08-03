// $Id: VCSessionProvider.cs,v 1.12.2.3 2008/09/16 17:13:19 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


using System;
using Viatronix.v3D.Core;
using System.Collections;
using System.IO;
using System.Collections.Generic;
using System.Drawing;
using System.Configuration;
using System.Diagnostics;
using Viatronix.Logging;
using Viatronix.Reporting;
using Viatronix.Enterprise.Entities;
using Viatronix.Console.Entities;


namespace Viatronix.Console
{
  /// <summary>
  /// This session provider provides session details from VC Study types
  /// </summary>
  public class VCSessionProvider : ISessionProvider
  {

    #region fields

    /// <summary>
    /// Series object
    /// </summary>
    private Series m_series = null;

    /// <summary>
    /// Temp folder
    /// </summary>
    private string m_tempFolder = string.Empty;
    
    #endregion

    #region ISessionProvider methods

    /// <summary>
    /// Initializes the session provider
    /// </summary>
    /// <param name="series">Series</param>
    public void Initialize( Series series, string stageArea )
    {
      m_series = series;
      m_tempFolder = System.IO.Path.Combine( stageArea, Guid.NewGuid().ToString() );

      // The vrx file.
      string reportFile = string.Empty;
      foreach (string file in m_series.Files)
      {
        if (file.ToLower().EndsWith(".vrx"))
        {
          reportFile = System.IO.Path.Combine(m_series.Path, file);
          break;
        }
      }

      try
      {
        if(!System.IO.Directory.Exists(m_tempFolder))
        {
          System.IO.Directory.CreateDirectory(m_tempFolder);
        }

        if (reportFile != string.Empty && File.Exists(reportFile))
        {
          Process process = new Process();
          process.StartInfo.FileName = Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.ParseExpression(ConfigurationManager.AppSettings["VCViewerPath"]);
          process.StartInfo.Arguments = string.Format("-vrx {0} -out {1}", reportFile, m_tempFolder);
          process.StartInfo.UseShellExecute = false;
          process.StartInfo.CreateNoWindow = true;
          process.Start();

          process.WaitForExit();
        }
      }
      catch (Exception e)
      {
        Logging.Log.Error(string.Format("Failed to execute VC Viewer for session output.  [ERROR={0}]", e.Message), "VCSessionProvider", "Initialize");
        throw new ApplicationException("Failed to execute VC Viewer for session output.", e);
      }
    }//Initialize( Series series )


    /// <summary>
    /// Returns a list of filenames of the snapshots the session has.
    /// </summary>
    /// <returns>The list of snapshot files.</returns>
    public ArrayList GetSnapshotsCollection()
    {
      ArrayList snapshotCollection = new ArrayList();

      List<string> snapshotFiles = new List<string>();
      snapshotFiles.AddRange(System.IO.Directory.GetFiles(m_tempFolder, "*.png"));
      snapshotFiles.Sort();

      foreach (string snapshotFile in snapshotFiles)
      {
        using (FileStream fs = new FileStream(snapshotFile, FileMode.Open))
        {
          Snapshot snapshot = new Snapshot(Image.FromStream(fs));
          snapshot.Filename = snapshotFile;

          snapshotCollection.Add(snapshot.Filename);

          // Add the file to the series files collection.
          if (!m_series.Files.Contains(snapshotFile))
          {
            m_series.Files.Add(snapshotFile);
          }
        }

      }

      return snapshotCollection;
    } // GetSnapshotsCollection()


    /// <summary>
    /// Returns all files in the session movies folder.
    /// </summary>
    /// <returns>All files in the session movies folder.</returns>
    public ArrayList GetMovieCollection()
    {
      ArrayList movieCollection = new ArrayList();

      // Causes problems with patient CD Creation.
      //foreach ( string file in m_series.Files )
      //{
      //  if ( System.IO.Path.GetDirectoryName( file ).ToLower().EndsWith("movies"))
      //  {
      //    string movieFile = System.IO.Path.Combine( m_series.Path, file );
      //    movieCollection.Add ( movieFile.ToLower() );
      //  }
      //}

      // Get the preview image too.
      string movieFolder = System.IO.Path.Combine( m_series.Path, "Movies" );
      if ( System.IO.Directory.Exists( movieFolder ))
      {
        string[] movieFolderContents = System.IO.Directory.GetFiles( movieFolder );

        foreach( string file in movieFolderContents )
        {
          string fileLC = file.ToLower();
          // Check to make sure that the file wasn't added in the first pass.
          if( !movieCollection.Contains(fileLC) )
          {
            movieCollection.Add(fileLC);
          } // if( movieCollection.Contains(fileLC) )

          // Causes Problems with Patient CD Creation.
          // For VC movies the session series does not have preview images.
          // So add them.
          //string fileName = Path.Combine( "Movies", Path.GetFileName(file)); 
          //if ( !m_series.Files.Contains(  fileName ))
          //  m_series.Files.Add(fileName);
        } // foreach( string file in movieFolderContents )
      } // if ( System.IO.Directory.Exists( movieFolder ))

      return movieCollection;
    } // GetMovieCollection()


    /// <summary>
    /// Implements the Report Available method
    /// </summary>
    /// <param name="reportExtension"></param>
    /// <returns></returns>
    public bool IsReportAvailable(string reportExtension)
    {
      foreach ( string file in m_series.Files )
      {
        if ( file.ToLower().EndsWith( reportExtension.ToLower() ))
          return true;
      }
      return false;
    } // IsReportAvailable( reportExtension )


    /// <summary>
    /// Returns the file name of the report as a pdf.
    /// </summary>
    /// <returns>The filename of the pdf file.</returns>
    public string ExportReportToPdf()
    {
      string[] pdfFiles = System.IO.Directory.GetFiles(m_tempFolder, "*.pdf");
      return pdfFiles.Length > 0 ? pdfFiles[0] : string.Empty;
    } // ExportReportToPdf()


    /// <summary>
    /// Gets the report as a series of Bitmaps.
    /// </summary>
    /// <returns>An arraylist of snapshots containing the files.</returns>
    public System.Collections.ArrayList ExtractReportSnapshots()
    {
      // TODO: REST
      //string reportFile = string.Empty;
      //foreach ( string file in m_series.Files )
      //{
      //  if ( file.ToLower().EndsWith( ".vrx" ))
      //  {
      //    reportFile = System.IO.Path.Combine( m_series.Path, file );
      //    break;
      //  }
      //}

      //if ( !System.IO.File.Exists ( reportFile ))
      //  return new ArrayList();

      //System.Collections.ArrayList snapshotCollection = new ArrayList();

      //try
      //{
      //  using (Viatronix.Reporting.VRXReader reader = new Viatronix.Reporting.VRXReader(reportFile))
      //  {
      //    reader.Read();
      //    foreach( Bitmap image in reader.ReportPages )
      //      snapshotCollection.Add( image.Clone() );
      //  }
      //}
      //catch ( Exception ex )
      //{
      //  Viatronix.Logging.Log.Error( "Error occurred in extracting report snapshots from VRX" + ex.Message, "VCSessionProvider", "ExtractReportSnapshots" );
      //  throw ex;
      //}

      //return snapshotCollection;

      return new ArrayList();
    } // ExtractReportSnapshots()

    #endregion

  } // class VCSessionProvider
} // namespace Viatronix.Console

// $Log: VCSessionProvider.cs,v $
// Revision 1.12.2.3  2008/09/16 17:13:19  kchalupa
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
// Revision 1.12.2.3  2008/09/09 16:15:47  kchalupa
// Issue #6073
// GetMovieCollection had bugs that would add multiple
// movie previews.
//
// Revision 1.12.2.2  2007/07/02 17:42:11  romy
// added JPG type for movie fils this is for VC
//
// Revision 1.12.2.1  2007/03/20 14:10:48  romy
// saving the image as PNG format
//
// Revision 1.12  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.11  2007/02/15 23:17:14  mkontak
// Fixed problem with frame acces and cross thread issue
//
// Revision 1.10  2006/11/13 19:04:13  romy
// Fixed conversion issue
//
// Revision 1.9  2006/10/30 16:21:20  gdavidson
// Changed IViewerRule to ILaunchRule
//
// Revision 1.8  2006/07/28 13:15:43  mkontak
// Changed the VRX report creator
//
// Revision 1.7  2006/07/27 20:15:32  romy
// Close was missing for report revision
//
// Revision 1.6  2006/06/06 19:48:11  mkontak
// changed deprecated code
//
// Revision 1.5  2006/05/04 17:19:15  romy
// cleaning up of Temp folder added
//
// Revision 1.4  2006/03/06 14:46:42  romy
// modified Report creation in PDF
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/VCSessionProvider.cs,v 1.12.2.3 2008/09/16 17:13:19 kchalupa Exp $
// $Id: VCSessionProvider.cs,v 1.12.2.3 2008/09/16 17:13:19 kchalupa Exp $
