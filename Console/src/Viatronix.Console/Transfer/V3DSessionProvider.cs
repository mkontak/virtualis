// $Id: V3DSessionProvider.cs,v 1.12.2.4 2011/05/18 01:57:00 mkontak Exp $
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
using Viatronix.Reporting;
using System.Drawing;
using System.Reflection;
using Viatronix.Logging;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;




namespace Viatronix.Console
{
  /// <summary>
  /// This session provider provides session details from v3D Study types.XP,VAS.ANG ... Series
  /// </summary>
  public class V3DSessionProvider : ISessionProvider
  {
    #region

    /// <summary>
    /// Series object
    /// </summary>
    private Series m_series = null;

    /// <summary>
    /// App domain object.
    /// </summary>
    private AppDomain m_appDomain = null;

    /// <summary>
    /// Temp folder
    /// </summary>
    private string m_tempFolder = string.Empty;

    /// <summary>
    /// Session details object
    /// </summary>
    private ISessionDetails m_sessionDetails = null;
     

    #endregion

    #region properites

    /// <summary>
    /// Gets the Report Temp folder
    /// </summary>
    public string ReportTempFolder
    {
      get 
      { 
        return System.IO.Path.Combine ( m_tempFolder, "Reports" );
      }
    }//ReportTempFolder


    /// <summary>
    /// Gets the Snapshots Temp folder
    /// </summary>
    public string SnapshotsTempFolder
    {
      get 
      { 
        return System.IO.Path.Combine ( m_tempFolder, "Snapshots" );
      }
    }//SnapshotsTempFolder


    #endregion


    #region ISessionProvider methods

    /// <summary>
    /// Initializes the session provider object
    /// </summary>
    /// <param name="series"></param>
    public void Initialize( Series series, string stageArea )
    {
      m_series = series;
      InitializeAppDomain();

      m_tempFolder = System.IO.Path.Combine( stageArea, Guid.NewGuid().ToString() );
    }//Initialize( Series series )

   
    /// <summary>
    /// Implements GetSnapshotsCollection method
    /// </summary>
    /// <returns></returns>
    public ArrayList GetSnapshotsCollection()
    {
      ArrayList snapshotCollection = new ArrayList();

      if ( m_series.Type == SeriesTypes.Session )
      {
        foreach ( string file in m_series.Files )
        {
          if ( System.IO.Path.GetDirectoryName( file ).ToLower().EndsWith( "snapshots")  && !file.ToLower().EndsWith( ".xml" ))
             snapshotCollection.Add( System.IO.Path.Combine( m_series.Path, file ) );
        }
      }
      return snapshotCollection;
    }//GetSnapshotsCollection

   
    /// <summary>
    /// Implements the GetMovie collection
    /// </summary>
    /// <returns></returns>
    public ArrayList GetMovieCollection()
    {
      ArrayList movieCollection = new ArrayList();

      foreach ( string file in m_series.Files )
      {
        if ( System.IO.Path.GetDirectoryName ( file ).ToLower ( ).EndsWith ( "movies" ) )
        {
          //if (System.IO.Path.GetDirectoryName(file).ToLower().EndsWith("movies") && !fileLower.EndsWith(".xml") && !fileLower.EndsWith(".png") )
          if ( file.ToLower ( ).EndsWith ( ".png" ) || file.ToLower ( ).EndsWith ( ".avi" ) || file.ToLower ( ).EndsWith ( ".wma" ) || file.ToLower ( ).EndsWith ( ".wmv" ) )
            movieCollection.Add ( System.IO.Path.Combine ( m_series.Path, file ) );
        }
      }
      return movieCollection;
    }//GetMovieCollection


    /// <summary>
    /// Implements IsReportAvailable method
    /// </summary>
    /// <param name="reportExtension"></param>
    /// <returns></returns>
    public bool IsReportAvailable( string reportExtension )
    {
      foreach ( string file in m_series.Files )
      {
        if ( file.ToLower().EndsWith( reportExtension.ToLower() ))
          return true;
      }
      return false;
    }//IsReportAvailable



    /// <summary>
    /// Initialize the App Domain
    /// </summary>
    private void InitializeAppDomain()
    {
      string basePath = System.IO.Path.Combine(SystemSettings.CurrentSystemSettings.ViewersDirectory, m_series.Application);

      ///TODO: Change the hardcoded names after the release.
      string assemblyFile = System.IO.Path.Combine(basePath, "Viatronix.Viewer.Core.dll");
      string assemblyTypename = "Viatronix.Viewer.Plugins.SessionDetails";
      ////////////////////////////////////////////////

      m_appDomain = AppDomain.CreateDomain("Report Domain", AppDomain.CurrentDomain.Evidence, basePath, basePath, false);
      AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(this.OnAssemblyResolveHandler);

      try
      {
        m_sessionDetails = (ISessionDetails)m_appDomain.CreateInstanceFromAndUnwrap(assemblyFile, assemblyTypename);
        m_sessionDetails.Initialize(m_series);
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Unable to create Report object [TYPE=" + assemblyTypename + "]  [ERROR=" + ex.Message + "]", "V3DSessionProvider", "InitializeAppDomain");
        throw ex;
      }
    }//InitializeAppDomain()


    /// <summary>
    /// Export Reports to PDF
    /// </summary>
    /// <returns></returns>
    public string ExportReportToPdf()
    {
      if (!IsReportXmlAvailable())
        return string.Empty;

      try
      {
        string pdfFile = System.IO.Path.Combine( ReportTempFolder, TransferUtilities.CreatesubFolderName( m_series ) + "_Report.pdf" );
        m_sessionDetails.SaveReportToPDF(pdfFile);

        return pdfFile;
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed to export report " + ex.Message, "V3DSessionProvider", "ExportReportToPdf");
        throw ex;
      }
    }//ExportReportToPdf


    /// <summary>
    /// Checks whether the report Xml is there or not
    /// </summary>
    /// <returns></returns>
    private bool IsReportXmlAvailable()
    {
      foreach (string file in m_series.Files)
      {
        if (file.EndsWith( @"report\report.xml" ))
          return true;
      }
      return false;
    }//IsReportXmlAvailable()


    /// <summary>
    /// Extract the report as snapshots
    /// </summary>
    /// <returns>collection of report page images</returns>
    public System.Collections.ArrayList ExtractReportSnapshots()
    {
      System.Collections.ArrayList imageCollection = new System.Collections.ArrayList();
      try
      {
        string[] imageData = m_sessionDetails.GetReport();
        foreach (string data in imageData)
        {
          using (MemoryStream stream = new MemoryStream(Convert.FromBase64String(data)))
          {
            Image image = System.Drawing.Image.FromStream(stream);
            imageCollection.Add( image );
          }
        }
      }
      catch (Exception ex)
      {
        Log.Error("Unable to get images from report=" + ex.Message + "]", "V3DSessionViewer", "ExtractReportSnapshots");
        throw ex;
      }
      return imageCollection;
    }// ExtractReportSnapshots()

  
    #endregion

    #region eventHandlers
    /// <summary>
    /// Represents the method that handles the AssemblyResolve events of an AppDomain.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">A ResolveEventArgs that contains the event data.</param>
    /// <returns>The Assembly that resolves the type, assembly, or resource.</returns>
    private Assembly OnAssemblyResolveHandler( object sender, ResolveEventArgs args )
    {
      string searchPattern = args.Name.Substring( 0, args.Name.IndexOf( ',' )) + ".???";
      string[] files = System.IO.Directory.GetFiles( m_appDomain.BaseDirectory, searchPattern );
      if( files.Length > 0 )
        return Assembly.LoadFrom( files[0] );
    
      throw new FileNotFoundException( "Unable to load assembly, " + args.Name );
    } // OnAssemblyResolveHandler( sender, args )


    #endregion

  }//V3DSessionProvider
}//Viatronix.Console



#region revisionLog

// $Log: V3DSessionProvider.cs,v $
// Revision 1.12.2.4  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.3  2007/06/13 21:43:42  romy
// code added to check for report FIle before exporting to PDF
//
// Revision 1.12.2.2  2007/06/13 14:45:00  romy
// modified code to get the Report pages as snapshots
//
// Revision 1.12.2.1  2007/06/13 14:08:03  romy
// code modified to get the ReportSnapshots from V3D Viewers. Issue#5682
//
// Revision 1.12  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.11  2007/02/15 23:17:14  mkontak
// Fixed problem with frame acces and cross thread issue
//
// Revision 1.10  2006/11/17 21:31:46  romy
// fixed issue with Transferring png images to PACS for Movie collection
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/V3DSessionProvider.cs,v 1.12.2.4 2011/05/18 01:57:00 mkontak Exp $
// $Id: V3DSessionProvider.cs,v 1.12.2.4 2011/05/18 01:57:00 mkontak Exp $

#endregion