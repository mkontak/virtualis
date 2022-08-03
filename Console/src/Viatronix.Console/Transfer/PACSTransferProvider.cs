// $Id: PACSTransferProvider.cs,v 1.11.2.6 2008/09/12 14:33:14 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

//using System;
//using System.IO;
//using System.Xml;
//using System.Diagnostics;
//using System.Configuration;
//using System.Collections;
//using System.Collections.Generic;

//using Viatronix.Dicom;
//using Viatronix.v3D.Core;
//using Viatronix.Utilities.Registry;
//using Viatronix.Utilities;
//using Viatronix.Encryption;
//using Viatronix.Enterprise.SDK.Providers;
//using Viatronix.Enterprise.Entities;
//using Viatronix.Console.Transfer;


//namespace Viatronix.Console
//{
//  /// <summary>
//  /// Summary description for TransferProviderDatabase.
//  /// </summary>
//  public class PACSTransferProvider : ITransferProvider, IDisposable
//  {

//    #region events

//    /// <summary>
//    /// Transfer progress event handler
//    /// </summary>
//    public event TransferProgressEventHandler Progress = null;

//    #endregion

//    #region fields

//    /// <summary>
//    /// Data provider
//    /// </summary>
//    private IDataProvider m_dataProvider;

//    /// <summary>
//    /// Disposed flag
//    /// </summary>
//    private bool m_disposed = false;

//    /// <summary>
//    /// Holds the PACS client object
//    /// </summary>
//    private PacsClient m_pacsClient = null;

//    /// <summary>
//    /// Event Handler to PACS Store Start Event 
//    /// </summary>
//    private Viatronix.Dicom.ClientEventHandler m_storeStartedEventHandler = null;

//    /// <summary>
//    /// Event Handler to PACS Store Completed Event
//    /// </summary>
//    private Viatronix.Dicom.ClientEventHandler m_storeCompletedEventHandler = null;

//    /// <summary>
//    /// Event Handler to PACS Store Failed Event
//    /// </summary>
//    private Viatronix.Dicom.ClientEventHandler m_storeFailedEventHandler = null;

//    /// <summary>
//    /// Event Handler to PACS Store Update event
//    /// </summary>
//    private Viatronix.Dicom.ClientEventHandler m_storeUpdatedEventHandler = null;
       
//    /// <summary>
//    /// Transfer Provider Args
//    /// </summary>
//    private TransferProviderArgs m_transferProviderArgs = null; 

//    /// <summary>
//    /// Transfer Item which holds the transfer Item text for progress bar.
//    /// </summary>
//    private string m_transferItem = string.Empty;

//    /// <summary>
//    /// Array Holds the Snapshots Information.
//    /// </summary>
//    private System.Collections.ArrayList m_dataSnapshots = new System.Collections.ArrayList();

//    /// <summary>
//    /// Array Holds the Movie Information
//    /// </summary>
//    private System.Collections.ArrayList m_dataMovies = new System.Collections.ArrayList();

//    /// <summary>
//    /// Holds an array of DICOM Files.
//    /// </summary>
//    private System.Collections.ArrayList m_dicomFiles = new System.Collections.ArrayList();

//    /// <summary>
//    /// Holds the Report Snapshots
//    /// </summary>
//    private System.Collections.ArrayList m_dataReportSnapshots = new System.Collections.ArrayList();

//    /// <summary>
//    /// The report file which should be PDF
//    /// </summary>
//    private string m_reportFile = string.Empty;

//    /// <summary>
//    /// Dicom file 
//    /// </summary>
//    private string m_dicomFile = string.Empty;

//    /// <summary>
//    /// Series
//    /// </summary>
//    private Series m_series = null;

//    /// <summary>
//    /// Event that signals on Transfer completion.
//    /// </summary>
//    private System.Threading.AutoResetEvent m_resetevent = new System.Threading.AutoResetEvent( false );

//    /// <summary>
//    /// Modality
//    /// </summary>
//    private string m_modality = System.Configuration.ConfigurationManager.AppSettings["StoreModality"];

//    /// <summary>
//    /// Gets the flag to determine if reports are sent as encapsulated PDF or bitmap images
//    /// </summary>
//    private bool m_bSupportsEncapsulatedPDF = Boolean.Parse(System.Configuration.ConfigurationManager.AppSettings["SuuportsEncapsulatedPDF"]);

//    /// <summary>
//    /// Create a cached session provider so we only need to initialize it once for each type.
//    /// </summary>
//    private Dictionary<Type, ISessionProvider> m_providers = new Dictionary<Type, ISessionProvider>();

//    /// <summary>
//    /// Transfers the snapshots.
//    /// </summary>
//    private bool m_transferSnapshots = false;

//    /// <summary>
//    /// Transfers the report.
//    /// </summary>
//    private bool m_transferReports = false;

//    #endregion

//    #region properties

//    #endregion properties

//    #region construction

//    /// <summary>
//    /// Constructor
//    /// </summary>
//    public PACSTransferProvider(IDataProvider dataProvider)
//    {

//      m_dataProvider = dataProvider;

//      string aeTitle = System.Configuration.ConfigurationManager.AppSettings["AEtitle"];

//      //m_progressHandler   = new ProgressEventHandler( this.OnProgressEventHandler );
//      m_pacsClient = ( string.IsNullOrEmpty(aeTitle) ? new PacsClient() : new PacsClient(aeTitle));

//      m_storeStartedEventHandler  = new Viatronix.Dicom.ClientEventHandler ( this.OnStoreStarted ); 
//      m_pacsClient.StoreStarted  += m_storeStartedEventHandler;

//      m_storeFailedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreFailed); 
//      m_pacsClient.StoreFailed   += m_storeFailedEventHandler;

//      m_storeUpdatedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreUpdated);
//      m_pacsClient.StoreUpdated  += m_storeUpdatedEventHandler;

//      m_storeCompletedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreCompleted); 
//      m_pacsClient.StoreCompleted += m_storeCompletedEventHandler;

//    }//PACSTransferProvider()

//    /// <summary>
//    /// Destructor
//    /// </summary>
//    ~PACSTransferProvider()
//    {
//      Dispose(false);
//    } //

//    /// <summary>
//    /// Dispose
//    /// </summary>
//    public void Dispose()
//    {
//      GC.SuppressFinalize(this);
//      Dispose(true);
//    } // Dispose()

//    /// <summary>
//    /// Dispose of the 
//    /// </summary>
//    /// <param name="disposing"></param>
//    private void Dispose(bool disposing)
//    {
//      if (! m_disposed)
//      {
//        if (disposing)
//        {
//          if (m_pacsClient != null)
//          {
//            m_pacsClient.Dispose();
//          }
//        }
//      }

//      m_disposed = true;
//    } // Dispose(bool bDispoing)

//    #endregion

//    #region events

//    /// <summary>
//    /// OnProgress method
//    /// </summary>
//    /// <param name="args"></param>
//    protected virtual void OnProgress( TransferProgressEventArgs args )
//    {
//      if( this.Progress != null )     
//        this.Progress( this, args );
//    }//OnProgress( TransferProgressEventArgs args )


//    /// <summary>
//    /// Called to initialize interaction for pacs store.
//    /// </summary>
//    /// <param name="sender">Event source</param>
//    /// <param name="args">Event info</param>
//    private void OnStoreStarted(object sender, Viatronix.Dicom.ClientEventArgs args)
//    {
//      string message = "Storing " + m_transferItem + " to " + ((Viatronix.Dicom.Host)m_transferProviderArgs.TransferObject).Name;
//      Viatronix.Logging.Log.Debug(message, "PACSTransferProvider", "OnStoreStarted");
//      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ));
//    } // OnStoreStarted(object sender, Viatronix.v3D.Dicom.DicomEventArgs args)


//    /// <summary>
//    /// Updates progress of, and during, pacs store.
//    /// </summary>
//    /// <param name="sender">Event source</param>
//    /// <param name="args">Event info</param>
//    private void OnStoreUpdated( object sender, Viatronix.Dicom.ClientEventArgs args )
//    {
//      string message = "Transferring " + m_transferItem  + " to " + ((Viatronix.Dicom.Host)m_transferProviderArgs.TransferObject).Name;
//      OnProgress( new TransferProgressEventArgs( message , args.Completed, args.Remaining + args.Completed ));
//    } // OnStoreUpdated( sender, args )


//    /// <summary>
//    /// Called on completion of pacs study store.
//    /// </summary>
//    /// <param name="sender">Event source</param>
//    /// <param name="args">Event info</param>
//    private void OnStoreCompleted( object sender, Viatronix.Dicom.ClientEventArgs args )
//    {
//      string message = "Finished  Storing " + m_transferItem + " to " + ((Host)m_transferProviderArgs.TransferObject).Name;
//      Viatronix.Logging.Log.Debug(message, "PACSTransferProvider", "OnStoreCompleted");
//      OnProgress( new TransferProgressEventArgs( message, args.Completed , args.Remaining + args.Completed  ));

//      m_resetevent.Set();

//    } // OnStoreTransferSnapshotsCompleted( sender, args )

    
//    /// <summary>
//    /// Indicates abnormal/error completion of pacs study store.
//    /// </summary>
//    /// <param name="sender">Event source</param>
//    /// <param name="args">Event info</param>
//    private void OnStoreFailed(object sender, Viatronix.Dicom.ClientEventArgs args)
//    {
//      Viatronix.Logging.Log.Error("Store to PACS failed : " + args.Message, "PACSTransferProvider", "OnStoreFailed");
//      // on a seperate thread invoke on UI thread
//      string message = "Failed storing to " + ((Host)m_transferProviderArgs.TransferObject).Name;
//      Viatronix.Logging.Log.Debug(message, "PACSTransferProvider", "OnStoreFailed");
//      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ));

//      if ( m_pacsClient.IsConnected )
//        m_pacsClient.Disconnect();

//      m_resetevent.Set();

//      Viatronix.UI.MessageBox.ShowError( "Store to PACS failed : " + args.Message );

//    }//OnStoreFailed(object sender, Viatronix.v3D.Dicom.DicomEventArgs args)

    
//    #endregion events

//    #region ITransferProvider Members

//    /// <summary>
//    /// Transfer Series Collection to a removable media
//    /// </summary>
//    /// <param name="seriesCollection"></param>
//    public void Transfer( TransferProviderArgs args )
//    {
//      try
//      {
//        m_transferProviderArgs = new TransferProviderArgs( args );

//        if ( args.TransferObject == null )
//        {
//          Viatronix.Logging.Log.Error ( " PACS Server name is empty. Aborting transfer", "PACSTransferProvider", "Transfer" );
//          throw new TransferException ( " No Valid PACS Server Name " );
//        }

//        ExtractTransferData();

//        //if ( IsExtractedDataEmpty() )
//        //{
//        //  OnProgress(new TransferProgressEventArgs( " Could not find data for Transferring. ",0,100) );
//        //  throw new TransferException( "Could not find data for Transferring");
//        //}

//        DicomHost pacsHost = (DicomHost)args.TransferObject;
//        OnProgress(new TransferProgressEventArgs( " Attempting connection to " + pacsHost.Name ,10,100) );

//        m_pacsClient.Host = pacsHost;
//        m_pacsClient.Connect("STORE");
        
//        OnProgress(new TransferProgressEventArgs( " Connected to " + pacsHost.Name ,100,100) );

//        TransferExtractedData();

//        OnProgress(new TransferProgressEventArgs( " Finished Transferring to "+ pacsHost.Name,100,100) );
//        System.Threading.Thread.Sleep ( 100 );
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error(ex.Message, "PACSTransferProvider", "Transfer");
//        throw ex;
//      }
//      finally 
//      {
//        try
//        {
//          if ( m_pacsClient != null )
//          {
//            m_pacsClient.Dispose();
//          }
//        }
//        catch ( Exception ex )
//        {
//          Viatronix.Logging.Log.Warning("Received exception while disposing of the pacs client : " + ex.Message, "PACSTransferProvider", "Transfer");
//        }

//        try
//        {
//          Viatronix.Logging.Log.Information("Removing transfers staging area [" + args.StageArea + "]", "PACSTransferProvider", "Transfer");
//          if (System.IO.Directory.Exists(args.StageArea))
//          {
//            System.IO.Directory.Delete(args.StageArea, true);
//          }
//        }
//        catch ( Exception ex )
//        {
//          Viatronix.Logging.Log.Warning("Received exception while removing staging area : " + ex.Message, "PACSTransferProvider", "Transfer");
//        }
//      }

//      int mode;
//      if (!m_transferSnapshots && !m_transferReports)
//      {
//        mode = 0;
//      }
//      else if (m_transferSnapshots && !m_transferReports)
//      {
//        mode = 1;
//      }
//      else if (!m_transferSnapshots && m_transferReports)
//      {
//        mode = 2;
//      }
//      else
//      {
//        mode = 3;
//      }

//      if (m_series != null && mode != 0)
//      {
//        IPACSSessionTransferProvider provider = PACSSessionTransferProviderFactory.Create(m_series);

//        if (provider != null)
//        {
//          OnProgress(new TransferProgressEventArgs("Transferring Session Contents To PACS...", 0, 100));

//          provider.TransferSession(m_series, mode);

//          OnProgress(new TransferProgressEventArgs("Completed Session Contents Transfer!", 0, 100));
//        }
//      }
//    } // Transfer( TransferProviderArgs args )
   
//    #endregion ITransferProvider Members

//    #region methods

//    /// <summary>
//    /// Transfer the Extracted Data to PACS
//    /// </summary>
//    void TransferExtractedData()
//    {

//      //Store DICOM data
//      TransferDicom();
      
//      //Transfer Session Data.
//      //TransferSnapshots();

//      //Start Movies Transfer to PACS and wait for this to finish
//      //TransferMovies();

//      // Start report Transfer to PACS and wait for this to finish
//      //TransferReport();

//    }//TransferExtractedData()

  
//    /// <summary>
//    /// Extract Transfer Data for PACS.
//    /// </summary>
//    private void ExtractTransferData()
//    {
//      int extractionProgress = 0;

//      IDictionaryEnumerator enumerator = m_transferProviderArgs.TransferData.GetEnumerator();

//      while (enumerator.MoveNext())
//      {
//        Series series = (Series)enumerator.Key; 
//        //m_transferProviderArgs.TransferData[ index ];
//        TransferItemsCollection transferItems= (TransferItemsCollection)enumerator.Value;// m_transferProviderArgs.TransferData[ series ];

//        string message = "Gathering selected transfer data.";
//        OnProgress ( new TransferProgressEventArgs ( message, extractionProgress++, m_transferProviderArgs.SeriesCollection.Count ));
        
//        if ( series.Type == SeriesTypes.Session)
//        {
//          m_transferSnapshots = transferItems.GetItem("Snapshots", TransferItem.GetTransferSubType(series.Application)).CheckState;
//          m_transferReports   = transferItems.GetItem("Reports",   TransferItem.GetTransferSubType(series.Application)).CheckState;

//          m_series = series;
//          //string dcmFile = string.Empty;
//          //ExtractSnapshots ( series, dcmFile, transferItems );
//          //ExtractMovies    ( series, dcmFile, transferItems );
//          //ExtractReport    ( series, dcmFile, transferItems );
//        }
//        //Extract DICOM.
//        if (  series.Type == SeriesTypes.Dicom)
//          ExtractDicom ( series, transferItems );
//      }
//    }//ExtractTransferData()


//    /// <summary>
//    /// Transfer Snapshots from the series collection to the  PACS
//    /// </summary>
//    private void ExtractSnapshots( Series series, string dcmFile, TransferItemsCollection transferItems )
//    {
//      //string transferSubtype = TransferItem.GetTransferSubType( series.Application );

//      ////Get the Transfer Item's Formats
//      //TransferItem snapshotItem = transferItems.GetItem( "Snapshots", transferSubtype ); 

//      ////Check for the 
//      //if ( snapshotItem == null   )
//      //{
//      //  Viatronix.Logging.Log.Error("SnapshotItem object is null.", "PACSTransferProvider", "ExtractSnapshots");
//      //  return;
//      //}

//      ////Check for the 
//      //if ( !snapshotItem.CheckState )
//      //{
//      //  Viatronix.Logging.Log.Debug("Snapshots are not selected for Transfer", "PACSTransferProvider", "ExtractSnapshots");
//      //  return;
//      //}

//      ////Build snapshots file array
//      //Type type = Type.GetType(snapshotItem.SessionTypeName);
//      //ISessionProvider sessionProvider = null;
//      //if (!m_providers.TryGetValue(type, out sessionProvider))
//      //{
//      //  sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//      //  sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//      //  m_providers.Add(type, sessionProvider);
//      //}

//      //System.Collections.ArrayList snapshotFiles = sessionProvider.GetSnapshotsCollection();
           
     
//      ////Build Snapshots Data Array
//      //if ( snapshotFiles.Count > 0 )
//      //{
//      //  if ( dcmFile == string.Empty )
//      //    dcmFile = FindRootDicomFile ( series );
//      //  m_dataSnapshots.Add ( new SnapshotData( series, snapshotFiles, dcmFile ));
//      //}
//    }//ExtractSnapshots( Series series, string dcmFile, TransferItemsCollection transferItems )

   
//    /// <summary>
//    /// Extract Movies Files from the series
//    /// </summary>
//    private void ExtractMovies( Series series, string dcmFile, TransferItemsCollection transferItems )
//    {
//      ////Get the Transfer Item's Formats
//      //string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      //TransferItem movieItem = transferItems.GetItem( "Movies", transferSubtype );
 
//      ////Check for the 
//      //if ( movieItem == null )
//      //{
//      //  Viatronix.Logging.Log.Error("MovieItem object is null", "PACSTransferProvider", "ExtractMovies");
//      //  return;
//      //}
      
//      ////Check for the 
//      //if ( !movieItem.CheckState )
//      //{
//      //  Viatronix.Logging.Log.Debug("Movies are not selected for Transfer.", "PACSTransferProvider", "ExtractMovies");
//      //  return;
//      //}

//      //Type type = Type.GetType( movieItem.SessionTypeName );
//      //ISessionProvider sessionProvider = null;
//      //if (!m_providers.TryGetValue(type, out sessionProvider))
//      //{
//      //  sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//      //  sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//      //  m_providers.Add(type, sessionProvider);
//      //}

//      //System.Collections.ArrayList allMovieFiles = sessionProvider.GetMovieCollection();

//      ////Since the Movie collection gives all files including preview PNGs
//      ////we need to sort out only the movie files here
//      //System.Collections.ArrayList movieFiles = new ArrayList ( );
//      //foreach ( string file in allMovieFiles )
//      //{
//      //  if ( !file.ToLower().EndsWith ( ".png" ) || !file.ToLower().EndsWith ( ".jpg" ))
//      //    movieFiles.Add ( file );
//      //}

//      ////Build Movies Data Array
//      //foreach ( string movieFile in movieFiles )
//      //{
//      //    if ( dcmFile == string.Empty )
//      //      dcmFile = FindRootDicomFile ( series );

//      //   // string movieFile = System.IO.Path.Combine ( series.Path, file );
//      //    m_dataMovies.Add( new  MovieData( series, movieFile, dcmFile ));
//      //}
//    }//ExtractMovies( Series series, string dcmFile, TransferItemsCollection transferItems )
  

//    /// <summary>
//    /// Extract DICOM Files from the series
//    /// </summary>
//    /// <param name="series"></param>
//    private void ExtractDicom ( Series series, TransferItemsCollection transferItems ) 
//    {
//      //Get the Transfer Item's Formats
//      //Get the Transfer Item's Formats
//      string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      TransferItem dicomItem = transferItems.GetItem( "DICOM", transferSubtype ); 
      
//      if ( dicomItem == null )
//      {
//        Viatronix.Logging.Log.Error("extracted DICOM object is null.", "PACSTransferProvider", "ExtractDicom");
//        return;
//      }
      
//      //Check for the 
//      if ( !dicomItem.CheckState)
//      {
//        Viatronix.Logging.Log.Debug("DICOM is not selected for Transfer.", "PACSTransferProvider", "ExtractDicom");
//        return;
//      }

//      //Build dicom Data Array
//      foreach ( string file in series.Files )
//      {
//        if ( DoesFileValidForTransfer( file, dicomItem ))
//          m_dicomFiles.Add( System.IO.Path.Combine ( series.Path, file) );
//      }
//    }//ExtractDicom ( Series series, TransferItemsCollection transferItems ) 


//    /// <summary>
//    /// Extract Report file from the series
//    /// </summary>
//    /// <param name="series"></param>
//    private void ExtractReport ( Series series, string dcmFile, TransferItemsCollection transferItems )
//    {
//      ////Get the Transfer Item's Formats
//      //string transferSubtype = TransferItem.GetTransferSubType( series.Application );
//      //TransferItem reportItem = transferItems.GetItem( "Reports", transferSubtype ); 
      
//      ////Check for the 
//      //if ( reportItem == null )
//      //{
//      //  Viatronix.Logging.Log.Error("ReportsItem object is null", "PACSTransferProvider", "ExtractReports");
//      //  return;
//      //}

//      ////Check for the 
//      //if ( !reportItem.CheckState )
//      //{
//      //  Viatronix.Logging.Log.Debug("Reports are not selected for Transfer.", "PACSTransferProvider", "ExtractReports");
//      //  return;
//      //}

//      //try
//      //{

//      //  if (m_bSupportsEncapsulatedPDF)
//      //  {

         
//      //    m_series = series;

//      //    m_reportFile = string.Empty;
//      //    m_dicomFile = dcmFile;

//      //    string reportDir = System.IO.Path.Combine(Database.GetBasePath(), System.IO.Path.Combine(series.Location, "report"));

//      //    /// Look for PDF
//      //    string[] pdfFiles = System.IO.Directory.GetFiles(reportDir, "*.pdf");

//      //    if (pdfFiles.Length > 0)
//      //      m_reportFile = pdfFiles[0];
 
//      //  }
//      //  else
//      //  {
//      //    //Build Snapshots from report
//      //    Type type = Type.GetType(reportItem.SessionTypeName);
//      //    ISessionProvider sessionProvider = null;
//      //    if (!m_providers.TryGetValue(type, out sessionProvider))
//      //    {
//      //      sessionProvider = (ISessionProvider)Activator.CreateInstance(type);
//      //      sessionProvider.Initialize(series, m_transferProviderArgs.StageArea);

//      //      m_providers.Add(type, sessionProvider);
//      //    }

//      //    System.Collections.ArrayList imageCollection = sessionProvider.ExtractReportSnapshots();
//      //    if (imageCollection.Count > 0)
//      //    {
//      //      if (dcmFile == string.Empty)
//      //        dcmFile = FindRootDicomFile(series);

//      //      m_dataReportSnapshots.Add(new SnapshotData(series, imageCollection, dcmFile));
//      //    }
//      //  }
//      //}
//      //catch ( Exception ex )
//      //{
//      //  Viatronix.Logging.Log.Error(" Error in creating Report Snapshots ", "PACSTransferProvider", "ExtractReports");
//      //  throw ex;
//      //}
//    }//ExtractReports ( Series series, string dcmFile, TransferItemsCollection transferItems )


//    /// <summary>
//    /// Check whether the ectracted information data is empty or not
//    /// </summary>
//    /// <returns></returns>
//    private bool IsExtractedDataEmpty()
//    {
//      if ( m_dataSnapshots.Count <=0 && m_dataMovies.Count <=0 && m_dicomFiles.Count <= 0 && m_dataReportSnapshots.Count <= 0 )
//        return true;
//      return false;
//    }//IsExtractedDataEmpty()


//    /// <summary>
//    /// Transfer Snapshots from the series collection to the  PACS
//    /// </summary>
//    private void TransferSnapshots()
//    {
//      //try
//      //{
//      //  string seriesPath = m_series.Path;
//      //  string[] files = System.IO.Directory.GetFiles(seriesPath, "*.vrx");
//      //  string vrx = files.Length > 0 ? files[0] : string.Empty;

//      //  if(vrx != string.Empty)
//      //  {
//      //    OnProgress(new TransferProgressEventArgs("Transferring Snapshots to PACS...", 0, 100));
          
//      //    Process process = new Process();
//      //    string vcViewerPath = ConfigurationManager.AppSettings["VCViewerPath"];
//      //    process.StartInfo.FileName = SystemSettings.CurrentSystemSettings.ParseExpression(vcViewerPath);
//      //    process.StartInfo.Arguments = string.Format("-pacs 1 -uid {0} -vrx {1}", m_series.UID, vrx);

//      //    process.Start();

          

//      //    process.WaitForExit();

//      //    OnProgress(new TransferProgressEventArgs("Completed Snapshots Transfer", 100, 100));
//      //  }
//      //}
//      //catch ( Exception ex )
//      //{
//      //  Viatronix.Logging.Log.Error("Failed to store snapshots" + ex.Message, "PACSTransferProvider", "TransferSnapshots");
//      //  throw new TransferException ( "Failed to Transfer Snapshots to PACS" + ex.Message );
//      //}
//    }//TransferSnapshots()


//    /// <summary>
//    /// Transfers the report to the pacs.
//    /// </summary>
//    private void TransferReport()
//    {
//      //try
//      //{
//      //  string seriesPath = m_series.Path;
//      //  string[] files = System.IO.Directory.GetFiles(seriesPath, "*.vrx");
//      //  string vrx = files.Length > 0 ? files[0] : string.Empty;

//      //  if (vrx != string.Empty)
//      //  {
//      //    OnProgress(new TransferProgressEventArgs("Transferring Report to PACS...", 0, 100));

//      //    Process process = new Process();
//      //    string vcViewerPath = ConfigurationManager.AppSettings["VCViewerPath"];
//      //    process.StartInfo.FileName = SystemSettings.CurrentSystemSettings.ParseExpression(vcViewerPath);
//      //    process.StartInfo.Arguments = string.Format("-pacs 2 -uid {0} -vrx {1}", m_series.UID, vrx);

//      //    process.Start();

//      //    process.WaitForExit();

//      //    OnProgress(new TransferProgressEventArgs("Completed Report Transfer", 100, 100));
//      //  }
//      //}
//      //catch (Exception ex)
//      //{
//      //  Viatronix.Logging.Log.Error("Failed to store report" + ex.Message, "PACSTransferProvider", "TransferSnapshots");
//      //  throw new TransferException("Failed to Transfer Report to PACS" + ex.Message);
//      //}
//    } // TransferReport()


//    /// <summary>
//    /// Transfer Snapshots from the series collection to the  PACS
//    /// </summary>
//    private void TransferMovies()
//    {
//      //ArrayList movieFrames = null;

//      //try
//      //{
//      //  //Transfer the above array list to PACS
//      //  if (m_dataMovies.Count > 0)
//      //  {
//      //    m_transferItem = "Movies";

//      //    string message = "Storing " + m_transferItem + " to " + ((Host)m_transferProviderArgs.TransferObject).Name;

//      //    for (int index = 0; index < m_dataMovies.Count; index++)
//      //    {
//      //      MovieData movie = (MovieData)m_dataMovies[index];
//      //      if (!Viatronix.Utilities.Movies.Supports(movie.MovieFile))
//      //      {
//      //        Viatronix.Logging.Log.Debug(movie.MovieFile + " is not a supported movie format", "PACSTransferProvider", "TransferMovies");
//      //        continue;
//      //      }

//      //      //Extract the frames now. This is better than extracting them in the Exctract Movie method.
//      //      OnProgress(new TransferProgressEventArgs("Extracting movie frames", 0, 100));
//      //      Viatronix.Utilities.MoviesExtractionCallback movieExtractionCallback = new Viatronix.Utilities.MoviesExtractionCallback(ExtractionProgress);
//      //      movieFrames = Viatronix.Utilities.Movies.GetImages(movie.MovieFile, movieExtractionCallback);

//      //      //Store Movies to the PACS.
//      //      OnProgress(new TransferProgressEventArgs(message, 0, 100));
//      //      m_pacsClient.StoreMovie(movie.Series, m_modality, movieFrames, movie.InitializerFile, false);
//      //      m_resetevent.WaitOne();

//      //      if (movieFrames != null)
//      //      {
//      //        foreach (System.Drawing.Bitmap bitmap in movieFrames)
//      //        {
//      //          bitmap.Dispose();
//      //        }

//      //        movieFrames.Clear();
//      //        movieFrames = null;
//      //      }

//      //    }
//      //  }
//      //}
//      //catch (Viatronix.Dicom.PacsException ex)
//      //{
//      //  Viatronix.Logging.Log.Error(" Failed to store Movies." + ex.Message, "PACSTransferProvider", "TransferMovies");
//      //  throw ex;
//      //}
//      //catch (Exception ex)
//      //{
//      //  Viatronix.Logging.Log.Error(" Failed to store Movies." + ex.Message, "PACSTransferProvider", "TransferMovies");
//      //  throw ex;
//      //}
//      //finally
//      //{
//      //  if (movieFrames != null)
//      //  {
//      //    foreach (System.Drawing.Bitmap bitmap in movieFrames)
//      //    {
//      //      bitmap.Dispose();
//      //    }

//      //    movieFrames.Clear();
//      //  }
//      //}
//    }//TransferMovies()


//    /// <summary>
//    /// Call back for Movie Frame extraction.
//    /// </summary>
//    /// <param name="totalFrames">int</param>
//    /// <param name="extractedFrames">int</param>
//    private void ExtractionProgress ( int totalFrames , int extractedFrames )
//    {
//      OnProgress( new TransferProgressEventArgs( "Extracting movie frames", extractedFrames, totalFrames ));
//    }//ExtractionProgress ( int totalFrames , int extractedFrames )



//    /// <summary>
//    /// Transfer DICOM files to the PACS.
//    /// </summary>
//    private void TransferDicom()
//    {
//      try
//      {
//        if ( m_dicomFiles.Count > 0 )
//        {
//          //Copy the files in to a string array and transfer to PACS.
//          string [] dicomFiles = new string[ m_dicomFiles.Count ];
          
//          int index = 0;
//          foreach ( string file in m_dicomFiles )
//            dicomFiles.SetValue( file, index++ );

//          m_transferItem = " DICOM ";
//          string message = "Storing " + m_transferItem + " to PACS..."; // + ((Host)m_transferProviderArgs.TransferObject).Name;
//          OnProgress ( new TransferProgressEventArgs ( message, 0, 100 ));
          
//          //Transfer to PACS
//          m_pacsClient.StoreImages( dicomFiles, false );
//          m_resetevent.WaitOne();
//        }
//      }
//      catch ( Viatronix.Dicom.PacsException ex )
//      {
//        Viatronix.Logging.Log.Error("Failed to store DICOM files." + ex.Message, "PACSTransferProvider", "TransferDicom");
//        throw new TransferException( "Failed to Transfer DICOM files to PACS." + ex.Message );
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("Failed to store DICOM files" + ex.Message, "PACSTransferProvider", "TransferDicom");
//        throw new TransferException ( " Failed to Transfer DICOM to PACS." + ex.Message );
//      }
//    }//TransferDicom()


//    /// <summary>
//    /// Checks whether the file has an extension which specifies in the Transfer Item.
//    /// </summary>
//    /// <param name="file"></param>
//    /// <param name="item"></param>
//    /// <returns></returns>
//    private bool DoesFileValidForTransfer ( string file, TransferItem item )
//    {
//      foreach ( string format in item.Formats )
//      {
//        if ( file.ToLower().EndsWith( format ))
//          return true;
//      }
//      return false;
//    }//DoesFileValidForTransfer ( string file, TransferItem item )


//    /// <summary>
//    /// Finds the root Dicom File for this series.
//    /// </summary>
//    /// <param name="series">Series</param>
//    /// <returns>string</returns>
//    private string  FindRootDicomFile ( Series series )
//    {
//      string dcmFile = null;
//      Series DicomSeries = null;
//      if ( series.Type == SeriesTypes.Dicom)
//      {
//        DicomSeries = series;
//        //dcmFolder  = System.IO.Path.Combine ( series.Path.ToString(), "Dicom" );
//      }
//      else
//      {

//        m_dataProvider.GetReferences( series);
 
//        Series dicom = null;
//        foreach (Series reference in series.References)
//        {
//          if ( reference.Type == SeriesTypes.Dicom )
//          {
//            dicom = reference;
//            break;
//          }
//        }


//        if ( dicom == null )
//          throw new Exception( "Could not find referenced DICOM series." );
//        else
//          DicomSeries = dicom;
//      }

//      string dcmDirectory = Path.Combine(SystemSettings.CurrentSystemSettings.CacheBasePath, DicomSeries.Directory, "dicom");
//      foreach (string file in System.IO.Directory.GetFiles(dcmDirectory, "*.dcm"))
//      {
//        if ( file.ToLower().EndsWith( ".dcm" ) )
//        {
//          dcmFile = System.IO.Path.Combine ( DicomSeries.Path, file );
//          break;
//        }
//      }
//      if ( dcmFile == null || dcmFile.Length <= 0 )
//        throw new Exception( "Could not find referenced DICOM series." );

//      return dcmFile;

//    }//FindRootDicomFile ( Series series, List<Series> seriesCollection )

    
//    /// <summary>
//    /// Disconnects from the PACS server
//    /// </summary>
//    private void Disconnect()
//    {
//      if ( m_pacsClient.IsConnected )
//      {
//        Viatronix.Logging.Log.Information("Disconnecting from PACS server ", "PACSTransferProvider", "Disconnect");
//        m_pacsClient.Disconnect();
//      } 
//    } // Disconnect()


//    /// <summary>
//    /// Abort Method
//    /// </summary>
//    public void Abort()
//    {
//      Disconnect();
//      throw new TransferAbortException ( " User aborted Transfer Operation ");
//    }// Abort()


//    #endregion methods
  
//  } // class PACSTransferProvider


  ///// <summary>
  ///// Class SnapshotData
  ///// </summary>
  //internal class SnapshotData
  //{
  //  #region fields
  //  /// <summary>
  //  /// Series Object
  //  /// </summary>
  //  private Seriesm_series = null;

  //  /// <summary>
  //  /// Dicom File associated with the series
  //  /// </summary>
  //  private string m_dcmFile = string.Empty;

  //  /// <summary>
  //  /// Snapshots from the series
  //  /// </summary>
  //  private System.Collections.ArrayList m_snapshots;

  //  #endregion 

  //  #region properties
  //  /// <summary>
  //  /// Gets or Sets the Series Object
  //  /// </summary>
  //  public Series series
  //  {
  //    get { return m_series; }
  //    set { m_series = value; }
  //  }//Series

  //  /// <summary>
  //  /// Gets or Sets the DICOM File
  //  /// </summary>
  //  public string DicomFile
  //  {
  //    get { return m_dcmFile; }
  //    set { m_dcmFile = value;}
  //  }//DicomFile

  //  /// <summary>
  //  /// Gets or Sets the Snapshots Objects
  //  /// </summary>
  //  public System.Collections.ArrayList Snapshots
  //  {
  //    get { return m_snapshots; }
  //    set { m_snapshots = value; }
  //  }//Snapshots


  //  #endregion 

  //  #region construction

  //  /// <summary>
  //  /// Construction
  //  /// </summary>
  //  /// <param name="series">Series Object</param>
  //  /// <param name="snapshotFiles">Snapshot Files</param>
  //  /// <param name="dcmFile">Dicom File object</param>
  //  public SnapshotData( Series series, System.Collections.ArrayList snapshots, string dcmFile )
  //  {
  //    m_series    = series;
  //    m_dcmFile   = dcmFile;
  //    m_snapshots = (System.Collections.ArrayList)snapshots.Clone();
  //  }//SnapshotsInformation( Series series, System.Collections.ArrayList snapshotFiles, string dcmFile )


  //  #endregion 

  //}// internal class SnapshotData


  ///// <summary>
  ///// Class MovieData.
  ///// </summary>
  //internal class MovieData
  //{
  //  #region fields
  //  /// <summary>
  //  /// Dicom File
  //  /// </summary>
  //  private string m_dicom = string.Empty;


  //  /// <summary>
  //  /// Series
  //  /// </summary>
  //  private Series m_series;
  //   /// <summary>
  //  /// Movie file from the series
  //  /// </summary>
  //  private string m_movieFile;

  //  #endregion 

  //  #region properties

  //  /// <summary>
  //  /// Gets or Sets the Series
  //  /// </summary>
  //  public Series Series
  //  {
  //    get { return m_series; }
  //    set { m_series = value; }
  //  }//Series

  //  /// <summary>
  //  /// Gets or Sets the dicom Initializer File.
  //  /// </summary>
  //  public string InitializerFile
  //  {
  //    get { return m_dicom; }
  //    set { m_dicom = value;}
  //  }//DicomElementList

  //  /// <summary>
  //  /// Gets or Sets the Movie File
  //  /// </summary>
  //  public string MovieFile
  //  {
  //    get { return m_movieFile; }
  //    set { m_movieFile = value; }
  //  }//Movie


  //  #endregion 

  //  #region construction

  //  /// <summary>
  //  /// Movie data COnstructor
  //  /// </summary>
  //  /// <param name="subtype">string</param>
  //  /// <param name="movieFile">string</param>
  //  /// <param name="dcmFile">string</param>
  //  public MovieData( Series series, string movieFile, string dcmFile )
  //  {
  //    m_series = series;
  //    m_dicom     = dcmFile;
  //    m_movieFile = movieFile;
  //  }//MovieData(string subtype, string movieFile, string dcmFile )

  //  #endregion 
  //}// internal class MovieData


//}//namespace Viatronix.Console


#region revision History
// $Log: PACSTransferProvider.cs,v $
// Revision 1.11.2.6  2008/09/12 14:33:14  mkontak
// Issue 6120: Allows the modality to be set for storing to the PACS
//
// Revision 1.11.2.5  2008/03/18 20:22:34  mkontak
// Fix removal of staging area upon error
//
// Revision 1.11.2.4  2007/10/18 19:56:06  mkontak
// Fix for transfers AETitle issue
//
// Revision 1.11.2.3  2007/07/02 17:40:49  romy
// added JPG type for movie fils this is for VC
//
// Revision 1.11.2.2  2007/05/01 14:14:26  mkontak
// Dispose of the bitamps after their use
//
// Revision 1.11.2.1  2007/03/29 14:42:39  mkontak
// Fix for pac client disposal
//
// Revision 1.11  2007/02/15 23:17:14  mkontak
// Fixed problem with frame acces and cross thread issue
//
// Revision 1.10  2007/02/09 14:11:33  mkontak
// Fix exception description "Couldnot" to "Could not"
//
// Revision 1.9  2006/11/17 21:30:15  romy
// fixed issue with Transferring png images to PACS for Movie collection
//
// Revision 1.8  2006/07/13 18:10:13  romy
// spell correction
//
// Revision 1.7  2006/05/09 17:37:13  romy
// spell check
//
// Revision 1.6  2006/05/04 17:19:15  romy
// cleaning up of Temp folder added
//
// Revision 1.5  2006/05/02 20:01:49  romy
// Added the error message on transfer failure.
//
// Revision 1.4  2006/03/07 18:03:06  romy
// modified the way of getting referenced dcm file
//
// Revision 1.3  2006/02/16 15:07:32  romy
// using the ISessionProvider
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.19  2005/12/12 21:10:39  romy
// extra check for supported movie types added
//
// Revision 1.18  2005/12/07 19:18:57  romy
// modified Movie Transfer to PACS
//
// Revision 1.17  2005/12/07 18:38:22  romy
// modified Movie Transfer
//
// Revision 1.16  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.15  2005/12/02 02:41:43  romy
// coding standards
//
// Revision 1.14  2005/11/15 18:07:46  mkontak
// Added logging message if the pacs client fails to dispose
//
// Revision 1.13  2005/11/09 16:54:23  romy
// Exceptions revisited
//
// Revision 1.12  2005/10/31 21:11:35  romy
// StoreReport added
//
// Revision 1.11  2005/10/29 03:07:28  romy
// added signal event
//
// Revision 1.10  2005/10/27 18:16:29  romy
// Args  change
//
// Revision 1.9  2005/10/25 18:55:16  romy
// Added Multiple snapshots transfer
//
// Revision 1.8  2005/10/25 15:42:00  romy
// Added TransferItem Collection
//
// Revision 1.7  2005/10/21 17:47:22  mkontak
// Changed implementation of DicomHosts and how the server is defined
//
// Revision 1.6  2005/10/13 20:34:50  romy
// More comments
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/PACSTransferProvider.cs,v 1.11.2.6 2008/09/12 14:33:14 mkontak Exp $
// $Id: PACSTransferProvider.cs,v 1.11.2.6 2008/09/12 14:33:14 mkontak Exp $

#endregion revision History