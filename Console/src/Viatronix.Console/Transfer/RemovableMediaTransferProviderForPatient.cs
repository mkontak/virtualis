// $Id: RemovableMediaTransferProviderForPatient.cs,v 1.19.2.5 2011/05/18 01:56:59 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

//using System;
//using System.Reflection;
//using Viatronix.v3D.Core;
//using Viatronix.Utilities.RemovableMedia;
//using System.Collections.Generic;
//using System.IO;
//using Viatronix.UI;
//using System.Collections;
//using System.Xml;
//using Viatronix.Reporting;
//using System.Drawing;
//using System.Configuration;
//using System.Web.UI;
//using System.Diagnostics;
//using Viatronix.Utilities.IO;
//using Viatronix.Utilities.Registry;
//using Viatronix.Enterprise.Entities;
//using Viatronix.Console.Transfer;

//namespace Viatronix.Console
//{
//  /// <summary>
//  /// RemovableMediaTransferProviderForPatient Adds the files to the media by eliminating -
//  /// the file structure heirarchy.
//  /// </summary>
//  public class RemovableMediaTransferProviderForPatient : RemovableMediaTransferProvider, IConfigurationSectionHandler
//  {

//    #region constants

//    const int maxDiskSize = 626000000;

//    #endregion

//    #region fields

//    /// <summary>
//    /// Report Template
//    /// </summary>
//    private string m_reportTemplate = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.ConfigDirectory, @"Console\PatientReportTemplate.htm" );

//    /// <summary>
//    /// Report header template
//    /// </summary>
//    private string m_reportHeaderTemplate = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.ConfigDirectory, @"Console\ReportHeaderTemplate.htm" );

//    /// <summary>
//    /// The dictionary of providers.
//    /// </summary>
//    private Dictionary<Type, ISessionProvider> m_providers = new Dictionary<Type, ISessionProvider>();

//    #endregion

//    #region construction

//    #endregion

//    #region RemovableMediaTransferProvider Methods

//    /// <summary>
//    /// Override the new files.
//    /// </summary>
//    public override void AddFiles()
//    {
//      int extractionProgress = 0;

//      IDictionaryEnumerator enumerator = m_transferProviderArgs.TransferData.GetEnumerator();
//      List<Series> seriesCollection = new List<Series>();

//      // The transfer size in bytes.
//      long transferDataSize = 0;

//      //////////////////////////////////////////////////////////////////////////////////////////////////////
//      // Check the file sizes and quit if it's more than 626000000
//      //////////////////////////////////////////////////////////////////////////////////////////////////////

//      try
//      {
//        while( enumerator.MoveNext() )
//        {
//          Series series = ( Series )enumerator.Key;
//          TransferItemsCollection transferItems = ( TransferItemsCollection )enumerator.Value;
//          string patientFolder = System.IO.Path.GetDirectoryName(System.IO.Path.GetDirectoryName(series.Directory));

//          if( series.Type == SeriesTypes.Session)
//          {
//            transferDataSize += GetSeriesFileSize(series, transferItems, "Snapshots");
//            transferDataSize += GetSeriesFileSize(series, transferItems, "Movies");
//            transferDataSize += GetSeriesFileSize(series, transferItems, "Reports");
//          }

//          //Extract DICOM.
//          if( series.Type == SeriesTypes.Dicom)
//            transferDataSize += GetSeriesFileSize(series, transferItems, "DICOM");
//        }
//      }
//      catch( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("Getting transfer size failed. " + ex.Message, "RemovableMediaTransferProviderForPatient", "AddFiles");
//        throw ex;
//      }

//      if(transferDataSize > maxDiskSize)
//      {
//        Logging.Log.Error("Data for transfer exceeds maximum size of " + maxDiskSize + " bytes.", "RemovableMediaTransferProviderForPatient", "AddFiles");
//        throw new TransferException("Data for transfer exceeds maximum size of\n" + maxDiskSize + " bytes." +
//          "  Please deselect some items.");
//      } // if(transferDataSize > maxDiskSize)

//      enumerator.Reset();

//      string message = "Gathering selected transfer data.";
//      OnProgress(new TransferProgressEventArgs(message, extractionProgress++, m_transferProviderArgs.SeriesCollection.Count));
      
//      try
//      {
//        while ( enumerator.MoveNext() )
//        {
//          Series series = ( Series ) enumerator.Key;
//          TransferItemsCollection transferItems = ( TransferItemsCollection ) enumerator.Value;
//          string patientFolder = System.IO.Path.GetDirectoryName( series.Directory );

//          if ( series.Type == SeriesTypes.Session )
//          {
//            AddSnapshots( series, transferItems );
//            AddMovies( series, transferItems );
//            AddReport( series, transferItems );
//          }

//          //Extract DICOM.
//          if ( series.Type == SeriesTypes.Dicom )
//            AddDicom( series, transferItems );

//          if ( series.Type == SeriesTypes.Session )
//            AddHtmlReportSummary( series, transferItems );

//          if ( series.Type == SeriesTypes.Session || series.Type == SeriesTypes.Dicom )
//            seriesCollection.Add( series );
//        }
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("Add files failed during transfer. " + ex.Message, "RemovableMediaTransferProviderForPatient", "AddFiles");
//        throw ex;
//      }

//      try
//      {
//        AddHtmlReportHeader( seriesCollection );
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("Failed to create the HTML Report header. " + ex.Message, "RemovableMediaTransferProviderForPatient", "AddFiles");
//        throw ex;
//      }

//      //Add the general files
//      AddAutoRunIcon();
//      AddAutoRunFile();
//      AddLogo();

//    }//AddFiles()


//    #endregion

//    #region methods

//    /// <summary>
//    /// Gets the size, in bytes of the series files that need to be transferred.
//    /// </summary>
//    /// <param name="series"></param>
//    /// <param name="transferItems"></param>
//    /// <param name="transferItemType">"Snapshots", "DICOM" ...</param>
//    /// <returns>The size in bytes.</returns>
//    private long GetSeriesFileSize(Series series, TransferItemsCollection transferItems, 
//      string transferItemType)
//    {
//      long resultingSizeInBytes = 0;

//      //Get the Transfer Item's Formats
//      //Get the Transfer Item's Formats
//      string transferSubtype = TransferItem.GetTransferSubType(series.Application);
//      TransferItem transferItem = transferItems.GetItem(transferItemType, transferSubtype);

//      if( transferItem == null )
//      {
//        Viatronix.Logging.Log.Error("extracted " + transferItemType + " object is null.", "RemovableMediaTransferProviderForPatient", "GetSeriesFileSize");
//        return 0;
//      }

//      //Check for the 
//      if( !transferItem.CheckState )
//      {
//        Viatronix.Logging.Log.Debug(transferItemType + "is not selected for Transfer.", "RemovableMediaTransferProviderForPatient", "GetSeriesFileSize");
//        return 0;
//      }

//      //Build dicom Data Array
//      foreach( string file in series.Files )
//      {
//        resultingSizeInBytes += PathUtilities.GetSize(System.IO.Path.Combine(series.Path, file), 
//          false);
//      } // foreach( string file in series.Files )

//      return resultingSizeInBytes;
//    } // GetSeriesFileSize(Series series, TransferItemsCollection transferItems ... )

//    /// <summary>
//    /// Adds Dicom from the series collection to the  CD
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    private void AddDicom(Series series, TransferItemsCollection transferItems)
//    {
//      //Get the Transfer Item's Formats
//      //Get the Transfer Item's Formats
//      string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      TransferItem dicomItem = transferItems.GetItem( "DICOM", transferSubtype );

//      if ( dicomItem == null )
//      {
//        Viatronix.Logging.Log.Error("extracted DICOM object is null.", "RemovableMediaTransferProviderForPatient", "AddDicom");
//        return;
//      }

//      //Check for the 
//      if ( !dicomItem.CheckState )
//      {
//        Viatronix.Logging.Log.Debug("DICOM is not selected for Transfer.", "RemovableMediaTransferProviderForPatient", "AddDicom");
//        return;
//      }

//      string subFolder = TransferUtilities.CreatesubFolderName( series, "DICOM" );

//      int count = 0;
//      //Build dicom Data Array
//      foreach ( string file in series.Files )
//      {
//        if ( file.ToLower().EndsWith( ".xml" ) )
//          continue;
//        string message = "Transferring DICOM files.";
//        OnProgress( new TransferProgressEventArgs( message, count++, series.Files.Count ) );

//        //string fileName  = Path.GetFileName ( file ) ;
//        string mediaFile = System.IO.Path.Combine( subFolder, Path.GetFileName( file ) );

//        if ( mediaFile == string.Empty )
//          continue;

//        SelectedDevice.Writer.Files.Add( new MediaFile( System.IO.Path.Combine( series.Path, file ), mediaFile ));
//      }
//    }//AddDicom( Series series, TransferItemsCollection transferItems )


//    /// <summary>
//    ///  Adds Snapshots from the series collection to the  PACS
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    private void AddSnapshots(Series series, TransferItemsCollection transferItems)
//    {
//      string transferSubtype = TransferItem.GetTransferSubType( series.Application );

//      //Get the Transfer Item's Formats
//      TransferItem snapshotItem = transferItems.GetItem( "Snapshots", transferSubtype ); 

//      //Check for the 
//      if ( snapshotItem == null   )
//      {
//        Viatronix.Logging.Log.Error("SnapshotItem object is null.", "RemovableMediaTransferProviderForPatient", "AddSnapshots");
//        return;
//      }

//      //Check for the 
//      if ( !snapshotItem.CheckState )
//      {
//        Viatronix.Logging.Log.Debug("Snapshots are not selected for Transfer", "RemovableMediaTransferProviderForPatient", "AddSnapshots");
//        return;
//      }

     
//      //Build snapshots file array
//      Type type = Type.GetType(snapshotItem.SessionTypeName);
//      ISessionProvider sessionProvider = null;
//      if (!m_providers.TryGetValue(type, out sessionProvider))
//      {
//        sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//        sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//        m_providers.Add(type, sessionProvider);
//      }

//      System.Collections.ArrayList snapshotFiles = sessionProvider.GetSnapshotsCollection();

//      string subFolder = TransferUtilities.CreatesubFolderName( series, "Snapshots" );

//      int count = 0;
//      foreach ( string file in snapshotFiles )
//      {
//        string message = "Transferring Snapshots.";
//        OnProgress( new TransferProgressEventArgs( message, count++, snapshotFiles.Count ) );

//        string fileName = Path.GetFileName( file );
//        string mediaFile = System.IO.Path.Combine( subFolder, fileName );

//        if ( mediaFile == string.Empty )
//          continue;

//        SelectedDevice.Writer.Files.Add( new MediaFile( file, mediaFile ) );

//      }// foreach ( string file in snapshotFiles )
//    }//AddSnapshots( Series series, TransferItemsCollection transferItems )



//    /// <summary>
//    /// Adds Movies Files from the series
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    private void AddMovies(Series series, TransferItemsCollection transferItems)
//    {
//      string imageLocation = string.Empty;
//      string[] resources = Assembly.GetExecutingAssembly().GetManifestResourceNames();
      
//      foreach ( string resource in resources )
//      {
//        if(resource.EndsWith("movie.png"))
//        {
//          imageLocation = resource;
//          break;
//        } // if(resource.EndsWith("movie.png"))
//      } // foreach ( string resource in resources )

//      Bitmap defaultImage = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream(
//        imageLocation));

//      //Get the Transfer Item's Formats
//      string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      TransferItem movieItem = transferItems.GetItem( "Movies", transferSubtype );
//      TransferItem snapshotItem = transferItems.GetItem("Snapshots", transferSubtype);

//      //Check for the 
//      if ( movieItem == null )
//      {
//        Viatronix.Logging.Log.Error("MovieItem object is null", "RemovableMediaTransferProviderForPatient", "AddMovies");
//        return;
//      }

//      //Check for the 
//      if ( !movieItem.CheckState )
//      {
//        Viatronix.Logging.Log.Debug("Movies are not selected for Transfer.", "RemovableMediaTransferProviderForPatient", "AddMovies");
//        return;
//      }

//      Type type = Type.GetType( movieItem.SessionTypeName );
//      ISessionProvider sessionProvider = null;
//      if (!m_providers.TryGetValue(type, out sessionProvider))
//      {
//        sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//        sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//        m_providers.Add(type, sessionProvider);
//      }

//      ArrayList movieFiles = sessionProvider.GetMovieCollection();

//      string subFolder = TransferUtilities.CreatesubFolderName( series, "Movies" );

//      int count = 0;
//      foreach ( string file in movieFiles )
//      {
//        string message = "Transferring movies.";
//        OnProgress( new TransferProgressEventArgs( message, count++, movieFiles.Count ) );

//        string fileName = Path.GetFileName( file );
//        string mediaFile = System.IO.Path.Combine( subFolder, fileName );
//        if ( mediaFile == string.Empty )
//          continue;

//        if ( IsAnImage( file, snapshotItem ) )
//        {
//          //Adjust the aspect ratio and store in the temp area
//          string aspectRaioAdjustedFile = System.IO.Path.Combine( m_transferProviderArgs.StageArea, fileName );
//          PreserveMovieThumbnailAspectRatio( file, aspectRaioAdjustedFile );

//          SelectedDevice.Writer.Files.Add( new MediaFile( aspectRaioAdjustedFile, mediaFile ) );
//        }
//        else
//        {
//          string fileLCEXT = Path.GetExtension(file).ToLower();
//          if(fileLCEXT == ".wmv" || fileLCEXT == ".avi")
//          {
//            // Look for a suitable preview image, if one doesn't exist, then use the default image.
//            string[] previewImagesPng = System.IO.Directory.GetFiles(Path.GetDirectoryName(file), Path.GetFileNameWithoutExtension(file) + ".png");
//            string[] previewImagesJpg = System.IO.Directory.GetFiles(Path.GetDirectoryName(file), Path.GetFileNameWithoutExtension(file) + ".jpg");

//            // If there was no preview image...
//            if(previewImagesPng.Length == 0 && previewImagesJpg.Length == 0)
//            {
//              bool previewSaved = false;

//              string previewFileName = Path.Combine(Path.Combine(m_transferProviderArgs.StageArea, "Movies"), Path.GetFileNameWithoutExtension(file)) + ".png";
//              string previewMediaFile = System.IO.Path.Combine(subFolder, Path.GetFileName(previewFileName));
              
//              // Try to save an image to the disk within the staging area.
//              try
//              {
//                System.IO.Directory.CreateDirectory(Path.Combine(m_transferProviderArgs.StageArea, "Movies"));
//                defaultImage.Save(previewFileName, System.Drawing.Imaging.ImageFormat.Png);
//                previewSaved = true;
//              } // try
//              catch ( IOException e )
//              {
//                Logging.Log.Error("Error in saving default preview image for movie; Exception" + e.Message, "RemovableMediaTransferProviderForPatient", "AddMovies");
//              } // catch ( IOException e )

//              // If the save succeeded...
//              if(previewSaved)
//              {
//                // Add the preview to the series files list and add it to the table of files to be written.
//                series.Files.Add(previewFileName);
//                SelectedDevice.Writer.Files.Add(new MediaFile(previewFileName, previewMediaFile));

//                //// Currently the preview and movie have to be in the save directory for the entry to be 
//                //// written in the HTML Report, so remove it from the file list, copy it to the same location
//                //// as the preview and add the new file location to the series file list.
//                //series.Files.Remove(file);
//                //string destination = Path.Combine(m_transferProviderArgs.StageArea, Path.GetFileName(file));
//                //File.Copy(file, destination);
//                //series.Files.Add(destination);

//                //// Add the movie (within the staging area) to the list of files to be written out.
//                //SelectedDevice.Writer.Files.Add(new MediaFile(destination, mediaFile));
//              } // if(previewSaved)
//            } // if(previewImages.Length == 0)
//          } // if(fileLCEXT == ".wmv" || fileLCEXT == ".avi")

//          SelectedDevice.Writer.Files.Add(new MediaFile(file, mediaFile));
//        } // else
//      }
//    }//AddMovies( Series series, TransferItemsCollection transferItems )


//    /// <summary>
//    /// Checks to see if the image is of type snapshot
//    /// </summary>
//    /// <param name="file">string</param>
//    /// <param name="?">TransferItem</param>
//    /// <returns>bool</returns>
//    private bool IsAnImage(string file, TransferItem snapshotItem )
//    {
//      foreach ( string format in snapshotItem.Formats )
//      {
//        if ( file.ToLower().EndsWith( format.ToLower() ))
//          return true;
//      }
//      return false;
//    }//IsAnImage(string file, TransferItem snapshotItem )

   

//    /// <summary>
//    /// Adds Report Files from the series
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    private void AddReport(Series series, TransferItemsCollection transferItems)
//    {
//      //Get the Transfer Item's Formats
//      string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      TransferItem reportItem = transferItems.GetItem( "Reports", transferSubtype );

//      //Check for the 
//      if ( reportItem == null )
//      {
//        Viatronix.Logging.Log.Error("ReportsItem object is null", "RemovableMediaTransferProviderForPatient", "AddReport");
//        return;
//      }

//      //Check for the 
//      if ( !reportItem.CheckState )
//      {
//        Viatronix.Logging.Log.Debug("Reports are not selected for Transfer.", "RemovableMediaTransferProviderForPatient", "AddReport");
//        return;
//      }


//      try
//      {
//        OnProgress( new TransferProgressEventArgs( "Transferring Report data", 0, 100 ) );

//        //Build Snapshots from report
//        Type type = Type.GetType( reportItem.SessionTypeName );
//        ISessionProvider sessionProvider = null;
//        if (!m_providers.TryGetValue(type, out sessionProvider))
//        {
//          sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//          sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//          m_providers.Add(type, sessionProvider);
//        }

//        string file = sessionProvider.ExportReportToPdf();

//        if ( !System.IO.File.Exists( file ) )
//          return;

//        string mediaFile = System.IO.Path.Combine( TransferUtilities.CreatesubFolderName( series, "Reports" ), Path.GetFileName( file ) );

//        // string fullFilePath =  Path.Combine( series.Path, file );
//        SelectedDevice.Writer.Files.Add( new MediaFile( file, mediaFile ));
//        OnProgress( new TransferProgressEventArgs( "Transferring Report data", 100, 100 ) );

//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error(" Error in creating Report Snapshots. " + ex.Message, "RemovableMediaTransferProviderForPatient", "AddReport");
//        throw ex;
//      }
//    }//AddReport ( Series series, TransferItemsCollection transferItems )


//    /// <summary>
//    /// Add autorun file in to the CD
//    /// </summary>
//    private void AddAutoRunFile()
//    {
//      string autoRunTemplate = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.ConfigDirectory, @"Console\Autorun.inf" );

//      StreamReader reader = new StreamReader( autoRunTemplate, System.Text.Encoding.Default );
//      string buffer = reader.ReadToEnd();
//      buffer = buffer.Replace( "fileName", "index.html" );
//      reader.Close();

//      // string autoRunFile = System.IO.Path.Combine ( SystemSettings.CurrentSystemSettings.TempDirectory, "Autorun.inf" );
//      string autoRunFile = System.IO.Path.Combine( m_transferProviderArgs.StageArea, "Autorun.inf" );
//      StreamWriter writer = new StreamWriter( autoRunFile );
//      writer.BaseStream.Seek( 0, SeekOrigin.End );
//      writer.WriteLine( buffer );
//      writer.Flush();
//      writer.Close();

//      SelectedDevice.Writer.Files.Add( new MediaFile( autoRunFile , "Autorun.inf" ));
//    }//AddAutoRunFile


//    /// <summary>
//    /// Add Logo file in to the CD
//    /// </summary>
//    private void AddLogo()
//    {
//      string logoFolder = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.ConfigDirectory, @"Console" );

//      string [ ] logoFiles = System.IO.Directory.GetFiles( logoFolder, "logo*" );
//      if ( logoFiles.Length < 1 )
//        return;
//      string logoFile = logoFiles [ 0 ];

//      SelectedDevice.Writer.Files.Add( new MediaFile( logoFile , System.IO.Path.Combine( "Logo", System.IO.Path.GetFileName( logoFile )) ));
//    }//AddLogo


//    /// <summary>
//    /// Adds the Auto run icon
//    /// </summary>
//    private void AddAutoRunIcon()
//    {
//      string iconFolder = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.ConfigDirectory, @"Console" );

//      string [ ] iconFiles = System.IO.Directory.GetFiles( iconFolder, "autorun.ico" );
//      if ( iconFiles.Length < 1 )
//        return;
//      string iconFile = iconFiles [ 0 ];

//      SelectedDevice.Writer.Files.Add( new MediaFile( iconFile , System.IO.Path.GetFileName( iconFile )));
//    }//AddAutoRunIcon


//    /// <summary>
//    /// Checks whether the file is valid for transfer or not.
//    /// </summary>
//    /// <param name="file">string</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    /// <returns></returns>
//    private bool IsFileValidForTransfer(string file, TransferItemsCollection transferItems)
//    {
//      foreach ( TransferItem item in transferItems )
//      {
//        if ( !item.CheckState )
//          continue;

//        foreach ( string format in item.Formats )
//        {
//          if ( format.Equals( "*.*" ) || file.ToLower().EndsWith( format.ToLower() ) )
//            return true;
//        }
//      }
//      return false;

//    }//IsFileValidForTransfer ( string file, TransferItemsCollection transferItems ) 


//    /// <summary>
//    /// Creates a new image by keeping the aspect ratio for the destination area
//    /// </summary>
//    /// <param name="fileName">string</param>
//    /// <param name="newFileName">string</param>
//    private void PreserveMovieThumbnailAspectRatio(string fileName, string newFileName)
//    {
//      //Create the XmlDocument.
//      XmlDocument doc = new XmlDocument();
//      doc.Load( m_reportTemplate );

//      ///area where the image aspect ratio is preserved for moving to CO
//      try
//      {

//        XmlNodeList snapshotlist = doc.SelectNodes( "//*[starts-with( local-name(.), 'v3d_movies' )]" );
//        XmlNode snapshotNode = snapshotlist [ 0 ];

//        XmlAttribute heightAttrib = snapshotNode.Attributes [ "height" ];
//        XmlAttribute widthAttrib = snapshotNode.Attributes [ "width" ];

//        int height = System.Convert.ToInt32( heightAttrib.Value );
//        int width = System.Convert.ToInt32( widthAttrib.Value );

//        RectangleF bounds = new RectangleF( 0.0F, 0.0F, width, height );

//        //Source image
//        System.Drawing.Image image = Image.FromFile( fileName );

//        // size of source image
//        SizeF size = new SizeF( image.Width / image.HorizontalResolution, image.Height / image.VerticalResolution );

//        // scale and keep aspect ratio of source image
//        float scale = Math.Min( bounds.Width / size.Width, bounds.Height / size.Height );
//        size.Width *= scale;
//        size.Height *= scale;

//        // new image layout in pixels
//        RectangleF layout = new RectangleF( bounds.X + ( bounds.Width - size.Width ) / 2.0F,
//          bounds.Y + ( bounds.Height - size.Height ) / 2.0F,
//          size.Width, size.Height );

//        // draw a new image into the layout
//        System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap( height, width, image.PixelFormat );
//        using ( System.Drawing.Graphics gfx = System.Drawing.Graphics.FromImage( bitmap ) )
//        {
//          gfx.DrawImage( image, layout.X, layout.Y, layout.Width, layout.Height );
//          bitmap.Save( newFileName, image.RawFormat );
//        }
//      }
//      catch ( Exception ex )
//      {
//        Logging.Log.Error( "Failed to get the specified image size from the report template file. " + ex.Message, "RemovableMediaTransferProviderForPatient", "PreserveAspectRatio");
//      }
//    }//PreserveAspectRatio(string fileName, string newFileName)


//    /// <summary>
//    /// Check to see if the image size is Square
//    /// </summary>
//    /// <param name="fileName">string</param>
//    /// <returns>bool</returns>
//    private bool IsImageSizeSquare(string fileName)
//    {
//      System.Drawing.Image image = Image.FromFile( fileName );
//      if ( image.Width == image.Height )
//        return true;

//      return false;
//    }//IsImageSizeSquare( string fileName )
    
    
//    /// <summary>
//    /// Adds the Summary to the report file
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <param name="transferItems">TransferItemsCollection</param>
//    private void AddHtmlReportSummary(Series series, TransferItemsCollection transferItems)
//    {
//      //string tempFolder = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.TempDirectory, Guid.NewGuid().ToString());
//      string tempFolder = System.IO.Path.Combine( m_transferProviderArgs.StageArea, Guid.NewGuid().ToString() );

//      tempFolder = System.IO.Path.Combine( tempFolder, TransferUtilities.CreatesubFolderName( series ) );
//      string reportFile = System.IO.Path.Combine( tempFolder, "Report.html" );

//      try
//      {
//        Hashtable reportMacros = ( Hashtable ) ConfigurationManager.GetSection( "reportMacros" );

//        //Create the XmlDocument.
//        XmlDocument doc = new XmlDocument();
//        doc.Load( m_reportTemplate );

//        XmlNodeList list = doc.SelectNodes( "//*[starts-with( local-name(.), 'v3d_' )]" );

//        foreach ( XmlNode node in list )
//        {
//          string type = string.Empty;
//          try
//          {
//            type = reportMacros [ node.Name ].ToString();
//          }
//          catch ( Exception ex )
//          {
//            Viatronix.Logging.Log.Error("Invalid tag name" + node.Name + ex.Message, "RemovableMediaTransferProviderForPatient", "AddHtmlReportSummary");
//            continue;
//          }
//          Type objectType = Type.GetType( type );
//          if ( objectType == null )
//          {
//            Viatronix.Logging.Log.Error("Invalid object type" + type, "RemovableMediaTransferProviderForPatient", "AddHtmlReportSummary");
//            continue;
//          }

//          ITag tag = ( ITag ) Activator.CreateInstance( objectType );
//          tag.Initialize( series, node.OuterXml );

//          if ( !IsTagParsingRequired( tag, transferItems ) )
//            continue;

//          System.Text.StringBuilder stringBuilder = new System.Text.StringBuilder();
//          using ( System.IO.StringWriter stringWriter = new System.IO.StringWriter( stringBuilder ) )
//          {
//            System.Web.UI.HtmlTextWriter htmlWriter = new System.Web.UI.HtmlTextWriter( stringWriter );
//            tag.RenderHtml( htmlWriter );

//            XmlDocument htmlDoc = new XmlDocument();
//            string result = stringBuilder.ToString();
//            if ( result != string.Empty )
//            {
//              htmlDoc.LoadXml( stringBuilder.ToString() );
//              node.ParentNode.ReplaceChild( doc.ImportNode( htmlDoc.DocumentElement, true ), node );
//            }
//          }
//        }

//        if ( !System.IO.Directory.Exists( tempFolder ) )
//          System.IO.Directory.CreateDirectory( tempFolder );

//        doc.Save( reportFile );

//        string mediaFile = System.IO.Path.Combine( TransferUtilities.CreatesubFolderName( series ), System.IO.Path.GetFileName( reportFile ) );
//        SelectedDevice.Writer.Files.Add( new MediaFile( reportFile, mediaFile ));
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error(ex.Message, "RemovableMediaTransferProviderForPatient", "AddHtmlReportSummary");
//        throw ex;
//      }
//    }//AddHtmlReportSummary(Series series, TransferItemsCollection transferItems)


//    /// <summary>
//    /// Checks whether Parsing is required for the Tag Item
//    /// </summary>
//    /// <param name="tag"></param>
//    /// <param name="transferItems"></param>
//    /// <returns></returns>
//    private bool IsTagParsingRequired(ITag tag, TransferItemsCollection transferItems)
//    {
//      string tagName = tag.GetTagType();
//      if ( tagName.Equals( string.Empty ) )
//        return true;

//      ///If the selected item is selected Parsing is required
//      foreach ( TransferItem item in transferItems )
//      {
//        if ( item.Name.ToLower().Equals( tagName.ToLower() ) )
//        {
//          if ( item.CheckState )
//            return true;
//        }
//      }

//      //i.e. the Transfer items is missing from the selection.
//      return false;
//    }//IsTagParsingRequired(ITag tag, TransferItemsCollection transferItems)

//    /// <summary>
//    /// Adds HTML Report Summary
//    /// </summary>
//    /// <param name="series"></param>
//    private void AddHtmlReportHeader(List<Series> seriesCollection)
//    {
//      //string tempFolder = System.IO.Path.Combine( SystemSettings.CurrentSystemSettings.TempDirectory, Guid.NewGuid().ToString());
//      string tempFolder = System.IO.Path.Combine( m_transferProviderArgs.StageArea, Guid.NewGuid().ToString() );
//      string reportFile = System.IO.Path.Combine( tempFolder, "index.html" );

//      try
//      {
//        Hashtable reportMacros = ( Hashtable ) ConfigurationManager.GetSection( "reportMacros" );

//        //Create the XmlDocument.
//        XmlDocument doc = new XmlDocument();
//        doc.Load( m_reportHeaderTemplate );

//        XmlNodeList list = doc.SelectNodes( "//*[starts-with( local-name(.), 'v3d_' )]" );

//        foreach ( XmlNode node in list )
//        {
//          string type = string.Empty;
//          try
//          {
//            type = reportMacros [ node.Name ].ToString();
//          }
//          catch ( Exception ex )
//          {
//            Viatronix.Logging.Log.Error("Invalid tag name" + node.Name + ex.Message, "RemovableMediaTransferProviderForPatient", "AddHtmlReportHeader");
//            continue;
//          }
//          Type objectType = Type.GetType( type );
//          if ( objectType == null )
//          {
//            Viatronix.Logging.Log.Error("Invalid object type" + type, "RemovableMediaTransferProviderForPatient", "AddHtmlReportHeader");
//            continue;
//          }

//          ITag tag = ( ITag ) Activator.CreateInstance( objectType );
//          tag.Initialize( seriesCollection, node.OuterXml );

//          System.Text.StringBuilder stringBuilder = new System.Text.StringBuilder();
//          using ( System.IO.StringWriter stringWriter = new System.IO.StringWriter( stringBuilder ) )
//          {
//            System.Web.UI.HtmlTextWriter htmlWriter = new System.Web.UI.HtmlTextWriter( stringWriter );
//            tag.RenderHtml( htmlWriter );

//            XmlDocument htmlDoc = new XmlDocument();
//            htmlDoc.LoadXml( stringBuilder.ToString() );
//            node.ParentNode.ReplaceChild( doc.ImportNode( htmlDoc.DocumentElement, true ), node );
//          }
//        }

//        if ( !System.IO.Directory.Exists( tempFolder ) )
//          System.IO.Directory.CreateDirectory( tempFolder );

//        doc.Save( reportFile );
//        SelectedDevice.Writer.Files.Add( new MediaFile( reportFile, System.IO.Path.GetFileName( reportFile ) ));
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error(ex.Message, "RemovableMediaTransferProviderForPatient", "AddHtmlReportHeader");
//        throw ex;
//      }
//    }//AddHtmlReportHeader( List<Series> seriesCollection  )

//    #endregion

//    #region IConfigurationSectionHandler Members

//    /// <summary>
//    /// Cretaes the ReportMacros object
//    /// </summary>
//    /// <param name="parent">object</param>
//    /// <param name="configContext">object</param>
//    /// <param name="section">XmlNode</param>
//    /// <returns></returns>
//    public object Create(object parent, object configContext, XmlNode section)
//    {
//      Hashtable reportMacros = new Hashtable();

//      foreach ( XmlNode node in section.SelectNodes( "macro" ) )
//      {
//        string id = node.Attributes [ "id" ].Value;
//        string typename = node.Attributes [ "typename" ].Value;

//        reportMacros [ id ] = typename;
//      }
//      return reportMacros;
//    } // Create( parent, configContext, section )

//    #endregion

//  }//class RemovableMediaTransferProviderForPatient

//}//namespace Viatronix.Console


#region revision History

// $Log: RemovableMediaTransferProviderForPatient.cs,v $
// Revision 1.19.2.5  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.4  2009/07/17 13:17:26  kchalupa
// Patient CD burning has problems with showing the movie in HTML session report.
//
// Revision 1.19.2.3  2008/10/28 19:49:52  kchalupa
// Issue #5799
// Patient CDs that span more than one CD have incorrect links.
//
// Revision 1.19.2.2  2008/09/16 17:12:35  kchalupa
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
// Revision 1.19.2.2  2008/09/09 16:13:21  kchalupa
// Issue #6153
// Add the default movie preview to the staging area if a preview doesn't exist when creating patient CDs.
//
// Revision 1.19.2.1  2007/03/19 19:42:01  romy
// Don't save the actual snaphots for its aspect ratio. Issue# 5501
//
// Revision 1.19  2007/03/06 20:59:33  romy
// Keep the image format during the aspect ration preserving
//
// Revision 1.18  2007/03/05 16:43:47  romy
// more comments added
//
// Revision 1.17  2007/03/05 15:29:54  romy
// Issue # 5447 Patient CD transfer, code added to preserve the aspect ratio of thumbnail images
//
// Revision 1.16  2007/03/02 21:34:58  romy
// Issue # 5446 Patient CD transfer, deselecting Movies or Reports leave dead links on the CD.
// Added a hook to match the tags related to the selected items. Then the tag is compared against selected transfer items.
//
// Revision 1.15  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.14  2006/06/06 19:47:09  mkontak
// changed deprecated code
//
// Revision 1.13  2006/05/04 17:19:15  romy
// cleaning up of Temp folder added
//
// Revision 1.12  2006/04/04 13:07:20  romy
// Added autorun.inf file
//
// Revision 1.11  2006/03/06 14:42:33  romy
// minor GUI modification
//
// Revision 1.10  2006/02/27 20:55:21  romy
// adjusted for VC studies
//
// Revision 1.9  2006/02/16 21:46:32  romy
// report header added
//
// Revision 1.8  2006/02/16 19:45:38  romy
// VC Transfer added
//
// Revision 1.7  2006/02/16 15:10:15  romy
// using ISessionProvider
//
// Revision 1.6  2006/01/24 16:39:55  romy
// check for bad media file names
//
// Revision 1.5  2006/01/20 14:47:45  romy
// autorun icon added
//
// Revision 1.4  2006/01/20 08:43:31  romy
// minor fixes
//
// Revision 1.3  2006/01/17 02:52:49  romy
// added HTML Report Creation
//
// Revision 1.2  2006/01/09 20:15:08  romy
// added AppDomain mechanism to load CS Reports
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.6  2005/12/12 21:12:03  romy
// Transfer revisited
//
// Revision 1.5  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.4  2005/11/16 16:56:08  romy
// Fixed  the Image load issue.
//
// Revision 1.3  2005/11/16 15:58:23  romy
// Fixed  the Image load issue.
//
// Revision 1.2  2005/11/09 16:47:22  romy
// Temp version which allows the user to Transfer Reports
//
// Revision 1.1  2005/11/03 20:00:40  romy
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/RemovableMediaTransferProviderForPatient.cs,v 1.19.2.5 2011/05/18 01:56:59 mkontak Exp $
// $Id: RemovableMediaTransferProviderForPatient.cs,v 1.19.2.5 2011/05/18 01:56:59 mkontak Exp $

#endregion revision History