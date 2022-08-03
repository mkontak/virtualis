// $Id: PACSTransferProviderForArchival.cs,v 1.4.2.3 2008/09/12 14:33:14 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Xml;
using System.Collections;
using Viatronix.Console.Transfer;
using Viatronix.Utilities;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
  /// <summary>
  /// Summary description for TransferProviderDatabase.
  /// </summary>
  public class PACSTransferProviderForArchival : ITransferProvider
  {

    #region fields

    /// <summary>
    /// Modality used for storage
    /// </summary>
    private string m_modality = System.Configuration.ConfigurationManager.AppSettings["StoreModality"];

    /// <summary>
    /// Transfer progress event handler
    /// </summary>
    public event TransferProgressEventHandler Progress = null;

    /// <summary>
    /// Holds the PACS client object
    /// </summary>
    private Console.PacsClient m_pacsClient = null;

    /// <summary>
    /// Event Handler to PACS Store Start Event 
    /// </summary>
    private Viatronix.Dicom.ClientEventHandler m_storeStartedEventHandler = null;

    /// <summary>
    /// Event Handler to PACS Store Completed Event
    /// </summary>
    private Viatronix.Dicom.ClientEventHandler m_storeCompletedEventHandler = null;

    /// <summary>
    /// Event Handler to PACS Store Failed Event
    /// </summary>
    private Viatronix.Dicom.ClientEventHandler m_storeFailedEventHandler = null;

    /// <summary>
    /// Event Handler to PACS Store Update event
    /// </summary>
    private Viatronix.Dicom.ClientEventHandler m_storeUpdatedEventHandler = null;

    /// <summary>
    /// Transfer Provider Args
    /// </summary>
    private TransferProviderArgs m_transferProviderArgs = null;

    /// <summary>
    /// Transfer Item which holds the transfer Item text for progress bar.
    /// </summary>
    private string m_transferItem = string.Empty;

    /// <summary>
    /// Array Holds the Snapshots Information.
    /// </summary>
    private System.Collections.ArrayList m_dataSnapshots = new System.Collections.ArrayList();

    /// <summary>
    /// Array Holds the Movie Information
    /// </summary>
    private System.Collections.ArrayList m_dataMovies = new System.Collections.ArrayList();

    /// <summary>
    /// Holds an array of DICOM Files.
    /// </summary>
    private System.Collections.ArrayList m_dicomFiles = new System.Collections.ArrayList();

    /// <summary>
    /// Holds the Report Snapshots
    /// </summary>
    private System.Collections.ArrayList m_dataReportSnapshots = new System.Collections.ArrayList();

    /// <summary>
    /// Event that signals on Transfer completion.
    /// </summary>
    private System.Threading.AutoResetEvent m_resetevent = new System.Threading.AutoResetEvent( false );


    #endregion

    #region properties

    #endregion properties

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    public PACSTransferProviderForArchival()
    {
      string aeTitle = System.Configuration.ConfigurationManager.AppSettings["AEtitle"];


      //m_progressHandler   = new ProgressEventHandler( this.OnProgressEventHandler );
      m_pacsClient = (string.IsNullOrEmpty(aeTitle) ? new PacsClient() : new PacsClient(aeTitle));

      m_storeStartedEventHandler = new Viatronix.Dicom.ClientEventHandler( this.OnStoreStarted );
      m_pacsClient.StoreStarted += m_storeStartedEventHandler;

      m_storeFailedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreFailed);
      m_pacsClient.StoreFailed += m_storeFailedEventHandler;

      m_storeUpdatedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreUpdated);
      m_pacsClient.StoreUpdated += m_storeUpdatedEventHandler;

      m_storeCompletedEventHandler = new Viatronix.Dicom.ClientEventHandler(this.OnStoreCompleted);
      m_pacsClient.StoreCompleted += m_storeCompletedEventHandler;

    }//PACSTransferProvider()

    #endregion

    #region events

    /// <summary>
    /// OnProgress method
    /// </summary>
    /// <param name="args"></param>
    protected virtual void OnProgress(TransferProgressEventArgs args)
    {
      if ( this.Progress != null )
        this.Progress( this, args );
    }//OnProgress( TransferProgressEventArgs args )


    /// <summary>
    /// Called to initialize interaction for pacs store.
    /// </summary>
    /// <param name="sender">Event source</param>
    /// <param name="args">Event info</param>
    private void OnStoreStarted(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      string message = "Storing " + m_transferItem + " to " + ( ( Viatronix.Dicom.Host ) m_transferProviderArgs.TransferObject ).Name;
      Viatronix.Logging.Log.Debug(message, "PACSTransferProviderForArchival", "OnStoreStarted");
      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ) );
    } // OnStoreStarted(object sender, Viatronix.v3D.Dicom.DicomEventArgs args)


    /// <summary>
    /// Updates progress of, and during, pacs store.
    /// </summary>
    /// <param name="sender">Event source</param>
    /// <param name="args">Event info</param>
    private void OnStoreUpdated(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      string message = "Transferring " + m_transferItem + " to " + ( ( Viatronix.Dicom.Host ) m_transferProviderArgs.TransferObject ).Name;
      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ) );
    } // OnStoreUpdated( object sender, Viatronix.v3D.Dicom.DicomEventArgs args )


    /// <summary>
    /// Called on completion of pacs study store.
    /// </summary>
    /// <param name="sender">Event source</param>
    /// <param name="args">Event info</param>
    private void OnStoreCompleted(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      string message = "Finished  Storing " + m_transferItem + " to " + ( ( Viatronix.Dicom.Host ) m_transferProviderArgs.TransferObject ).Name;
      Viatronix.Logging.Log.Debug(message, "PACSTransferProviderForArchival", "OnStoreCompleted");
      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ) );

      m_resetevent.Set();

    } // OnStoreTransferSnapshotsCompleted( object sender, Viatronix.v3D.Dicom.DicomEventArgs args )


    /// <summary>
    /// Indicates abnormal/error completion of pacs study store.
    /// </summary>
    /// <param name="sender">Event source</param>
    /// <param name="args">Event info</param>
    private void OnStoreFailed(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      Viatronix.Logging.Log.Error("Store to PACS failed" + args.Message, "PACSTransferProviderForArchival", "OnStoreFailed");
      // on a seperate thread invoke on UI thread
      string message = "Failed storing to " + ( ( Viatronix.Dicom.Host ) m_transferProviderArgs.TransferObject ).Name;
      Viatronix.Logging.Log.Debug(message, "PACSTransferProviderForArchival", "OnStoreFailed");
      OnProgress( new TransferProgressEventArgs( message, args.Completed, args.Remaining + args.Completed ) );

      if ( m_pacsClient.IsConnected )
        m_pacsClient.Disconnect();

      m_pacsClient.Dispose();
      throw new TransferException( "Store to PACS failed:" + args.Message );
    }//OnStoreFailed(object sender, Viatronix.v3D.Dicom.DicomEventArgs args)


    #endregion events

    #region ITransferProvider Members

    /// <summary>
    /// Transfer Series Collection to a removable media
    /// </summary>
    /// <param name="seriesCollection"></param>
    public void Transfer(TransferProviderArgs args)
    {

      try
      {
        m_transferProviderArgs = new TransferProviderArgs( args );

        if ( args.TransferObject == null )
        {
          Viatronix.Logging.Log.Error ( " PACS Server name is empty. Aborting transfer ", "PACSTransferProviderForArchival", "Transfer" );
          throw new TransferException ( " No Valid PACS Server Name " );
        }

        DicomHost pacsHost = (DicomHost)args.TransferObject;
        OnProgress(new TransferProgressEventArgs( " Attempting connection to " + pacsHost.Name ,10,100) );

        m_pacsClient.Host = pacsHost;
        m_pacsClient.Connect("STORE");
        
        OnProgress(new TransferProgressEventArgs( " Connected to " + pacsHost.Name ,100,100) );


        int value  = 0;
        int increment = 100 / args.SeriesCollection.Count;

        ////////////////////////////////////////////////////////
        // Loop through all the series storing each one to PACS
        /////////////////////////////////////////////////////////
        foreach (Series series in args.SeriesCollection )
        {
          //////////////////////////////////////
          // Only store none DICOM series
          ////////////////////////////////////
          if ( series.Type != SeriesTypes.Dicom )
          {
            string file = FindRootDicomFile(series);
            Viatronix.Logging.Log.Debug("Attempting to store series [UID=" + series.Uid + "] [FULL_PATH=" + series.Path + "]", "PACSTransferProviderForArchival", "Transfer");
            OnProgress(new TransferProgressEventArgs(" Transferring " + series.Type + " series", value, 100));
            m_pacsClient.StoreSeriesPath(series, m_modality, file, true);
          } // END ... If the series is not dicom

          // increment the progress value
          value += increment;

        } // END ... For each series


        OnProgress(new TransferProgressEventArgs( " Finished Transferring to "+ pacsHost.Name,100,100) );

        System.Threading.Thread.Sleep ( 100 );

      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "PACSTransferProviderForArchival", "Transfer");
        throw ex;
      }
      finally 
      {
        try
        {
          if ( m_pacsClient != null )
          {
            m_pacsClient.Dispose();
          }
        }
        catch ( Exception ex )
        {
          Viatronix.Logging.Log.Warning("Received exception while disposing of the pacs client : " + ex.Message, "PACSTransferProviderForArchival", "Transfer");
        }
      }

    }//Transfer( TransferProviderArgs args )


    #endregion ITransferProvider Members

    #region methods


    /// <summary>
    /// Finds the root Dicom File for this series.
    /// </summary>
    /// <param name="series">Series</param>
    /// <returns>string</returns>
    private string FindRootDicomFile(Series series)
    {
      string dcmFile = null;
      Series DicomSeries = null;
      if ( series.Type == SeriesTypes.Dicom )
      {
        DicomSeries = series;
        //dcmFolder  = System.IO.Path.Combine ( series.Path.ToString(), "Dicom" );
      }
      else
      {

        if (!series.References.TryFindType(SeriesTypes.Dicom, ref DicomSeries))
          throw new Exception( "Could not find referenced DICOM series." );
      }

      foreach ( string file in DicomSeries.Files )
      {
        if ( file.ToLower().EndsWith( ".dcm" ) )
        {
          dcmFile = System.IO.Path.Combine( DicomSeries.Path, file );
          break;
        }
      }
      if ( dcmFile == null || dcmFile.Length <= 0 )
        throw new Exception( "Could not find referenced DICOM series." );

      return dcmFile;

    }//FindRootDicomFile ( Series series, List<Series> seriesCollection )


    /// <summary>
    /// Disconnects from the PACS server
    /// </summary>
    private void Disconnect()
    {
      if ( m_pacsClient.IsConnected )
      {
        Viatronix.Logging.Log.Information( "Disconnecting from PACS server", "PACSTransferProviderForArchival", "Disconnect" );
        m_pacsClient.Disconnect();
      }
    } // Disconnect()


    /// <summary>
    /// Abort Method
    /// </summary>
    public void Abort()
    {
      Disconnect();
      throw new TransferAbortException( " User aborted Transfer Operation " );
    }// Abort()


    #endregion methods

  }// public class DatabaseTransferProvider : ITransferProvider

}//namespace Viatronix.Console


#region revision History
// $Log: PACSTransferProviderForArchival.cs,v $
// Revision 1.4.2.3  2008/09/12 14:33:14  mkontak
// Issue 6120: Allows the modality to be set for storing to the PACS
//
// Revision 1.4.2.2  2007/10/18 19:56:06  mkontak
// Fix for transfers AETitle issue
//
// Revision 1.4.2.1  2007/10/08 15:19:57  mkontak
// Issue 5829: Object ref error - Due to trying to connect as a QR insetaed of a STORE
//
// Revision 1.4  2007/03/08 13:53:08  mkontak
// Do not transfer DICOM series
//
// Revision 1.3  2007/02/09 14:11:33  mkontak
// Fix exception description "Couldnot" to "Could not"
//
// Revision 1.2  2007/01/19 20:53:18  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.1  2007/01/12 18:56:04  romy
// Initial version
//
// Revision 1.9  2006/11/17 21:30:15  romy
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/PACSTransferProviderForArchival.cs,v 1.4.2.3 2008/09/12 14:33:14 mkontak Exp $
// $Id: PACSTransferProviderForArchival.cs,v 1.4.2.3 2008/09/12 14:33:14 mkontak Exp $

#endregion revision History