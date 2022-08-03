// $Id: TransferManager.cs,v 1.27.2.8 2011/05/18 01:56:59 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Collections.Specialized;
//using Viatronix.v3D.Core;
//using Viatronix.v3D.Anonymization;
using System.Windows.Forms;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
  /// <summary>
  /// Summary description for TransferThread.
  /// </summary>
  public class TransferManager
  {

    #region delegate

    /// <summary>
    /// Excepyion Invoke Delegate
    /// </summary>
    private delegate void ExceptionInvokeDelegate( string text );

    /// <summary>
    /// Progress Invoke delegate.
    /// </summary>
    private delegate void ProgressInvokeDelegate(string text, int progress, int total);

    /// <summary>
    /// Excepyion Invoke Delegate
    /// </summary>
    private delegate void AbortInvokeDelegate();

    #endregion

    #region fields

    /// <summary>
    /// Progress Dialog Progress invoke delegate.
    /// </summary>
    private ProgressInvokeDelegate m_progressDelegate = null;

    /// <summary>
    /// Abort invoke delegate.
    /// </summary>
    private AbortInvokeDelegate m_abortDelegate = null;


    /// <summary>
    /// Source Data provider
    /// </summary>
    private IDataProvider m_dataProvider = null;
    
    /// <summary>
    /// TransferDetails
    /// </summary>
    private TransferProvider m_transferProvider = null;

    /// <summary>
    /// ITransferProvider interface
    /// </summary>
    private ITransferProvider m_provider = null;


    /// <summary>
    /// Anonymization Wizard Object
    /// </summary>
    //private Viatronix.v3D.Anonymization.AnonymizerWizard m_anonWizard = null;

    /// <summary>
    /// Staging Area folder name
    /// </summary>
    private string m_stageAreaFolder = string.Empty;

    /// <summary>
    /// Anonymization Rules File Name
    /// </summary>
    private string m_anonymizationRulesFile = string.Empty;
    
    /// <summary>
    /// Transfer Progress Dialog Object
    /// </summary>
    private TransferProgressDialog m_progressDialog = null;

    /// <summary>
    /// Exception Invoke Delegate
    /// </summary>
    private ExceptionInvokeDelegate m_delegate = null;

    /// <summary>
    /// Selected Transfer Data Object.
    /// </summary>
    private System.Collections.Hashtable m_selectedTransferData = new Hashtable();

    /// <summary>
    /// Handles the cd progress event
    /// </summary>
    private Viatronix.UI.SetterDelegate< int, int, string > m_cdProgressHandler;

    /// <summary>
    /// Used for the FileSystemTransferProvider
    /// </summary>
    private string m_transferDirectory = string.Empty; 

    #endregion

    #region construction

    
    /// <summary>
    /// Transfer Manager constructor
    /// </summary>
    public TransferManager(IDataProvider dataProvider)
    {

      m_dataProvider = dataProvider;

      m_progressDelegate  = new ProgressInvokeDelegate(this.SetProgressInformation);
      m_delegate          = new ExceptionInvokeDelegate( this.OnExceptionInvokeHandler );
      m_abortDelegate     = new AbortInvokeDelegate( this.Abort );

      m_cdProgressHandler = new Viatronix.UI.SetterDelegate< int,int,string >( this.OnCDProgressHandler );
    }//TransferManager()


    #endregion Construction

    #region properties

    /// <summary>
    /// Source of the transfer data
    /// </summary>
    public IDataProvider DataProvider
    {
      get { return m_dataProvider; }
      set { m_dataProvider = value; }
    } // DataProvider

    /// <summary>
    /// Gets the Staging Area folder Name
    /// </summary>
    private string StagingAreaFolder
    {
      get
      {
        if ( m_stageAreaFolder == string.Empty )
          m_stageAreaFolder = Viatronix.Utilities.IO.PathUtilities.CombinePaths ( SystemSettings.CurrentSystemSettings.TempDirectory, Viatronix.Utilities.IO.PathUtilities.GenerateDirectoryName()) ;

        return m_stageAreaFolder;
      }
    }//StagingAreaFolder


    /// <summary>
    /// Gets the anonymization Rules File Name
    /// </summary>
    private string AnonymizationRulesFile
    {
      get
      {
        if ( m_anonymizationRulesFile == string.Empty )
        {
          string configDir = System.IO.Path.Combine ( SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console" );
          m_anonymizationRulesFile = System.IO.Path.Combine( configDir , "AnonymizationRules.xml" );
        }
        return m_anonymizationRulesFile; 
      }
    }//AnonymizationRulesFile


    /// <summary>
    /// Gets the Transfer Provider
    /// </summary>
    public ITransferProvider TransferProvider
    {
      get
      {
        if( m_provider == null )
        {
          m_provider = (ITransferProvider) Activator.CreateInstance( Type.GetType( m_transferProvider.TransferProviderTypeName ));
          m_provider.Progress += new TransferProgressEventHandler( this.OnTransferProgressEventHandler );
        }
        return m_provider;
      }
      set
      {
        m_provider = value;
      }
    }//TransferProvider

    /// <summary>
    /// Gets the destination directory for the FileSystemTransfer
    /// </summary>
    public string TransferDestination
    {
      get { return m_transferDirectory; }
    } // TransferDestination

    #endregion

    #region methods

    /// <summary>
    /// laucnehs the Transfer Provider Selector GUI
    /// </summary>
    /// <param name="transferProviders">Transfer Providers</param>
    /// <param name="seriesCollection">List<Series></param>
    /// <returns></returns>
    public bool SelectValidTransferProvider( ArrayList transferProviders, List<Series> seriesCollection, Series selectedSeries )
    {
      // TODO: New
      //Auditor.Log( seriesCollection, Global.CurrentUser , "Transfer initiated", "Transfer" );

      //Check to see whether you need to retrieve data
      bool bRetrieve = false;
      foreach ( Series series in seriesCollection )
      {
        if ( series.Files.Count == 0 )
        {
          bRetrieve = true;
          break;
        }
      }//foreach ( Series seriesInColl in seriesCollection )

      //If it is already retrieved and the dataProvider is Database show all the options the user can pick
      if ( bRetrieve && this.DataProvider is IDataProvider )
      {
        Cursor.Current = Cursors.WaitCursor;

        foreach (Series series in seriesCollection)
        {
          this.DataProvider.GetFiles(series);
        }
        
        Cursor.Current = Cursors.Default;
      }

      //Get the Transfer details from the user.
      TransferDialog transferDialog = new TransferDialog( this.DataProvider, transferProviders, seriesCollection, selectedSeries );
      if( transferDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK )
        return false;
      m_transferProvider     = new TransferProvider( transferDialog.SelectedTransferProvider );
      m_selectedTransferData = transferDialog.SelectedTransferData;
      transferDialog.Dispose();

      return true;
    }//SelectValidTransferProvider( ArrayList transferProviders, List<Series> seriesCollection, Series series )

    /// <summary>
    /// Transfer Series collection
    /// </summary>
    public void Transfer()
    {
      try
      {
        if ( m_transferProvider == null )
        {
          Viatronix.UI.MessageBox.ShowError( "Select a transfer provider." );
          return;
        }
        if ( m_selectedTransferData.Count > 0 )
        {

          if (m_transferProvider.Asynchronous)
          {
            bool pulse = m_transferProvider.TransferProviderTypeName == "Viatronix.Console.FileSystemTransferProvider, Console" || m_transferProvider.TransferProviderTypeName == "Viatronix.Console.PACSTransferProvider, Console";
            m_progressDialog = new TransferProgressDialog(pulse);
          }
    
          System.Windows.Forms.MethodInvoker invoker = new System.Windows.Forms.MethodInvoker( StartTransfer );
          invoker.BeginInvoke( new AsyncCallback( this.OnAsyncCallbackHandler ), null );

          if (m_progressDialog != null)
          {
            m_progressDialog.ShowDialog();

            if ( m_progressDialog.Abort )
            {
              this.Abort();
            }
          }

        }
      }
      catch ( TransferException ex )
      {
        Viatronix.Logging.Log.Error( "Transfer failed. " + ex.Message, "TransferManager", "Transfer");
        Viatronix.UI.MessageBox.ShowError ( "Failed to transfer the selected item." );
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Transfer failed. " + ex.Message, "TransferManager", "Transfer");
        Viatronix.UI.MessageBox.ShowError("Failed to transfer the selected item.");
      }
    }//Transfer( )



    
    /// <summary>
    /// Transfers a seires to the Provider
    /// </summary>
    private void StartTransfer()
    {
      string stagingAreaFullPath = StagingAreaFolder;
      string tempDirectory = SystemSettings.CurrentSystemSettings.TempDirectory;

      //If it says Anonymize then anonymize

      /// Creates a series collection
      IDictionaryEnumerator enumerator = m_selectedTransferData.GetEnumerator();
      List<Series> seriesCollection = new List<Series>();

      ////////////////////////////////////////////////////////////////////////////////
      // loop though the has table transferring the series into the collection
      ///////////////////////////////////////////////////////////////////////////////
      while (enumerator.MoveNext())
      {
        seriesCollection.Add((Series)enumerator.Key);
      } // END .. while there are series in the hash table


      try
      {
        if (!System.IO.Directory.Exists(stagingAreaFullPath))
        {
          Logging.Log.Debug("Creating the transfers staging ares [" + stagingAreaFullPath + "]", "TransferManager", "StartTransfer");
          System.IO.Directory.CreateDirectory(stagingAreaFullPath);
        }
      }
      catch (System.IO.IOException ex)
      {
        Viatronix.Logging.Log.Error("Failed anonymization staging area folder creation. " + ex.Message, "TransferManager", "StartTransfer");
        throw new TransferException("Failed the transfer staging area folder creation. " + ex.Message);
      }

      try
      {
        //if (m_transferProvider.IsAnonymize)
        //{
        //  //Anonymize
        //  Anonimize(seriesCollection, stagingAreaFullPath);
        //}

        if (TransferProvider != null)
        {
          //Starts the Transfer on the Transfer Data.
          TransferProviderArgs args = new TransferProviderArgs(Global.Frame, stagingAreaFullPath,
            m_transferProvider.TransferObject, m_selectedTransferData, m_dataProvider);
          TransferProvider.Transfer(args);

          if(TransferProvider is FileSystemTransferProvider)
          {
            Logging.Log.Information("Transferred data was successfully saved to directory: " +
              ( string )args.TransferObject + ".  Please make a note of it.", "TransferManager", "StartTransfer");

            UI.MessageBox.ShowInfo("Transferred data was successfully saved to directory: " + 
              (string)args.TransferObject + ".  Please make a note of it.");
          } // if(TransferProvider is FileSystemTransferProvider)
        }
      }
      //catch (Viatronix.v3D.Anonymization.AbortException ex)
      //{
      //  Viatronix.Logging.Log.Error(ex.Message, "TransferManager", "StartTransfer");
      //  return;
      //}
      //catch (Viatronix.v3D.Anonymization.AnonymizationException ex)
      //{
      //  Viatronix.Logging.Log.Error("Anonymization Error occured. " + ex.Message, "TransferManager", "StartTransfer");
      //  OnExceptionInvokeHandler(ex.Message);
      //  throw ex;
      //}
      catch (TransferAbortException ex)
      {
        Viatronix.Logging.Log.Debug(ex.Message, "TransferManager", "StartTransfer");
        this.Abort();
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Transfer failed. " + ex.Message, "TransferManager", "StartTransfer");
        OnExceptionInvokeHandler("Transfer failed. " + ex.Message);
        // throw new TransferException ( "Failed Transfer " , ex );
      }
      finally
      {
        try
        {
          if (System.IO.Directory.Exists(stagingAreaFullPath) && string.Compare(stagingAreaFullPath,tempDirectory,true) != 0  )
          {
            string currentWorkingDir = System.IO.Directory.GetCurrentDirectory();
            System.IO.Directory.SetCurrentDirectory( tempDirectory );

            Logging.Log.Debug("Creating the transfers staging ares [" + stagingAreaFullPath + "]", "TransferManager", "StartTransfer");

            System.IO.Directory.Delete(stagingAreaFullPath, true);
            System.IO.Directory.SetCurrentDirectory( currentWorkingDir );
          }
        }
        catch (System.IO.IOException ex)
        {
          Viatronix.Logging.Log.Error("Failed staging area folder Deletion. " + ex.Message, "TransferManager", "StartTransfer");
          throw new TransferException("Failed staging area folder Deletion. " + ex.Message);
        }
      }

    }// StartTransfer()

  

    /// <summary>
    /// Handles the Abort operation
    /// </summary>
    public void Abort()
    {

      Logging.Log.Debug("Aborting Transfer", "TransferManager", "Abort");

      try
      {
        if ( TransferProvider != null )
          this.TransferProvider.Abort();
      }
      catch ( TransferAbortException ex )
      {
        //if it is to abort Kill the anon Wizard and kill the progress dialog box.
        //if ( m_anonWizard != null )
        //  m_anonWizard.Abort( true );

        if (m_progressDialog != null)
        {
          if (m_progressDialog.InvokeRequired)
            m_progressDialog.Invoke(m_abortDelegate, new object[] { } );
          else
            Cleanup();
        } // END ... If the dialog is not NULL

        Viatronix.Logging.Log.Debug(ex.Message, "TransferManager", "Abort");
      }
      catch ( TransferAbortCancelException ex )
      {
        Viatronix.Logging.Log.Debug(ex.Message, "TransferManager", "Abort");
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed Abort. " + ex.Message, "TransferManager", "Abort");
      }
    }//Abort()

    /// <summary>
    /// Cleanup
    /// </summary>
    private void Cleanup()
    {
      m_progressDialog.Dispose();
    }//Cleanup()


    /// <summary>
    /// Handles the Abort operation
    /// </summary>
    public void Close()
    {
      //Cleanup 
      Viatronix.Logging.Log.Information(" Closing the Transfer manager ", "TransferManager", "Close");
    }//Close()

   
    /// <summary>
    /// Anonimize the given Series Collection
    /// </summary>
    /// <param name="collectionToAnonimize">List<Series></param>
    //private void Anonimize( List<Series> seriesCollection, string destFolder )
    //{
    //  try
    //  {
    //    Viatronix.Logging.Log.Information( " Performing anonymization ", "TransferManager", "Anonymize" );
    //    m_anonWizard = new AnonymizerWizard( AnonymizationRulesFile );
    //    m_anonWizard.anonymizationEvent += new Viatronix.v3D.Anonymization.AnonymizerWizard.AnonymizationEventHandler( OnTransferProgressEventHandler );

    //    // TODO: New
    //    //Auditor.Log( seriesCollection, Global.CurrentUser , "Anonymization initiated", "Anonymization" );

    //    m_anonWizard.Anonymize( m_progressDialog, seriesCollection, destFolder , m_transferProvider.IsOverride );

    //    Viatronix.Logging.Log.Information(" Anonymization is done ", "TransferManager", "Anonymize");
    //  }
    //  catch ( Viatronix.v3D.Anonymization.AbortException ex )
    //  {
    //    throw ex;
    //  }
    //  catch ( Viatronix.v3D.Anonymization.AnonymizationException ex )
    //  {
    //    Viatronix.Logging.Log.Error("Failed Anonymization : " + ex.Message, "TransferManager", "Anonymize");
    //    throw new TransferException(  "Failed Anonymization"  );
    //  }
    //  catch ( Exception ex )
    //  {
    //    Viatronix.Logging.Log.Error("Failed Anonymization : " + ex.Message, "TransferManager", "Anonymize");
    //    throw ex;
    //  }
    //}//Anonimize( List<Series> collectionToAnonimize )


    /// <summary>
    /// Checks for the existence of a directory with the same name as the patient location within the
    /// args object.
    /// </summary>
    /// <param name="args">Contains the output location and series collection that are needed.</param>
    /// <returns>
    /// True if everything is alright or false if the user needs to be prompted for another directory.
    /// </returns>
    private bool CheckForDirectoryExists(List<Series> coll, string destinationDirectory)
    {
      ///////////////////////////////////////////////////////////////////////////////////////
      // Check for the existence of the patient directory within the transfer directory and
      // ask the user if he wishes for it to be removed and the new directory to take it's place.
      ///////////////////////////////////////////////////////////////////////////////////////

      Study study = Global.DataProvider.GetStudy( coll[0].StudyUid);
      string destinationLocation = Path.Combine(destinationDirectory, study.Directory);

      if (System.IO.Directory.Exists(destinationLocation))
      {
        return true;
      } // if(System.IO.Directory.Exists(Path.Combine(args.TransferDestinationLocation, ...

      return false;
    } // CheckForDirectoryExists(TransferProviderArgs args)
  
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the progress event for the cd device
    /// </summary>
    /// <param name="progress">current progress</param>
    /// <param name="total">total progress</param>
    /// <param name="message">progress message</param>
    private void OnCDProgressHandler( int progress, int total, string message )
    {
      if( m_progressDialog.InvokeRequired )
        m_progressDialog.Invoke( m_cdProgressHandler, progress, total, message );
      else
        m_progressDialog.SetProgressInformation( message, progress, total );
    } // OnCDProgressHandler( progress, total, message )


    /// <summary>
    /// On Exception Invoke handler
    /// </summary>
    /// <param name="text"></param>
    private void OnExceptionInvokeHandler( string text )
    {
      // on a seperate thread invoke on UI thread
      if( Global.Frame.InvokeRequired )
        Global.Frame.Invoke( m_delegate, new object[] { text });
      else
      {
        // on UI thread display dialog
        Viatronix.UI.MessageBox.ShowError( text );
      }
    }//OnExceptionInvokeHandler( string text )


    /// <summary>
    /// OnProgress event 
    /// </summary>
    /// <param name="args"></param>
    protected virtual void OnTransferProgressEventHandler(object sender, TransferProgressEventArgs args)
    {
      if (m_progressDialog.InvokeRequired)
        m_progressDialog.Invoke(m_progressDelegate, new object[] { args.Text, args.Value, args.Total });
      else
        m_progressDialog.SetProgressInformation(args.Text, args.Value, args.Total);
    }//OnTransferProgressEventHandler( TransferProgressEventArgs args )


 
    /// <summary>
    /// Callback function for asynchronous Transfer.
    /// </summary>
    /// <param name="result">The result of the asynchronous operation.</param>
    private void OnAsyncCallbackHandler( IAsyncResult result )
    {
      if ( result.IsCompleted )
      {
        if (m_stageAreaFolder.Length > 0 && System.IO.Directory.Exists(m_stageAreaFolder))
        {
          Logging.Log.Information("Deleting the Transfers staging area [" + m_stageAreaFolder + "]", "TransferManager", "OnAsyncCallbackHandler");

          try
          {
            System.IO.Directory.Delete(m_stageAreaFolder, true);
          }
          catch (Exception ex)
          {
            Logging.Log.Warning("Failed to delete the transfers staging area : " + ex.Message, "TransferManager", "OnAsyncCallbackHandler");
          }
        }

        Viatronix.Logging.Log.Debug(" Calling Close Method ", "TransferManager", "OnAsyncCallbackHandler");
        if ( m_progressDialog != null && !m_progressDialog.IsDisposed )
        {
          System.Windows.Forms.MethodInvoker closeMethod = new System.Windows.Forms.MethodInvoker( m_progressDialog.Close );
          m_progressDialog.Invoke( closeMethod );
        }

      }
    } // OnAsyncCallbackHandler( result )


    /// <summary>
    /// Sets the Progress Information on the Dialog 
    /// </summary>
    /// <param name="text">string</param>
    /// <param name="progress">int</param>
    /// <param name="total">int</param>
    public void SetProgressInformation(string text, int progress, int total)
    {
      m_progressDialog.SetProgressInformation(text, progress, total);
    }//SetProgressInformation( string text, int progress, int total )


    #endregion

  }//TransferManager
}//namespace Viatronix.Console

#region revision History
// $Log: TransferManager.cs,v $
// Revision 1.27.2.8  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.27.2.7  2008/10/15 20:03:23  kchalupa
// FileSystemTransferProvider TOC Removal and now copies into GUID subdirectory under selected directory.
//
// Revision 1.27.2.6  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.27.2.5  2008/03/18 20:22:49  mkontak
// Fix removal of staging area upon error
//
// Revision 1.27.2.4  2007/12/20 16:05:20  mkontak
// Changed to force the Session first instead of the dicom last
//
// Revision 1.27.2.3  2007/09/14 20:00:02  mkontak
// Fix for non-removal of temp directories
//
// Revision 1.27.2.2  2007/06/05 16:49:39  mkontak
// Removed spacing in error message
//
// Revision 1.27.2.1  2007/03/23 20:37:51  romy
// removed the cancel button. Issue#5518
//
// Revision 1.27  2007/03/07 17:37:49  romy
// Delete the temp area modified
//
// Revision 1.26  2007/03/06 16:21:43  romy
// If the files are missing try to retrieve data
//
// Revision 1.25  2007/03/06 16:21:19  romy
// If the files are missing try to retrieve data
//
// Revision 1.24  2007/03/05 20:10:12  romy
// ok..read the rules.xml from the configuration folder
//
// Revision 1.23  2007/03/02 19:38:09  romy
//  If the study is from Database retrieving the series data before showing up the Transfer Dialog.
//
// Revision 1.22  2007/02/28 20:00:06  romy
// added comments before the code review
//
// Revision 1.21  2007/02/28 19:49:35  gdavidson
// Issue #5403: Added event handler for cd progress
//
// Revision 1.20  2007/01/19 20:53:18  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.19  2006/10/04 17:43:05  mkontak
// no message
//
// Revision 1.18  2006/09/21 20:55:27  mkontak
// Issue 4995
//
// Revision 1.17  2006/07/27 20:45:48  romy
// removed debug messages
//
// Revision 1.16  2006/07/26 21:07:56  romy
// rearranged the exeception handlers
//
// Revision 1.15  2006/07/26 19:58:59  romy
// no message
//
// Revision 1.14  2006/07/25 20:27:56  mkontak
// Issue 4887: Lockup when clicking cancel if two discs are required
//
// Revision 1.13  2006/07/20 20:21:30  mkontak
// Removed code for failed series check
//
// Revision 1.12  2006/07/18 14:51:55  mkontak
// Issue 4822
//
// Revision 1.11  2006/07/06 17:54:26  romy
// null check for control
//
// Revision 1.10  2006/07/05 18:54:32  romy
// testing the close method
//
// Revision 1.9  2006/06/29 20:14:02  romy
// invoke fix
//
// Revision 1.8  2006/05/04 17:19:15  romy
// cleaning up of Temp folder added
//
// Revision 1.7  2006/05/02 19:56:42  romy
// Added the error message on transfer failure.
//
// Revision 1.6  2006/02/16 15:11:19  romy
// using ISessionProvider
//
// Revision 1.5  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.4  2006/02/02 14:57:09  romy
// neat exception messages
//
// Revision 1.3  2006/01/26 21:05:14  romy
// auditing added
//
// Revision 1.2  2006/01/26 18:22:28  romy
// modifed transfer desgin
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.24  2005/12/12 21:14:35  romy
// Transfer revisited
//
// Revision 1.23  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.22  2005/11/09 16:54:50  romy
// Exceptions revisited
//
// Revision 1.21  2005/11/01 15:38:31  romy
// log spell corection
//
// Revision 1.20  2005/10/29 05:13:38  romy
// using system Date Time to create temp folder
//
// Revision 1.19  2005/10/27 18:10:39  romy
// Modified for Transfer Data
//
// Revision 1.18  2005/10/25 18:54:14  romy
// changed pacs server to PACS object
//
// Revision 1.17  2005/10/25 15:10:46  romy
// Added TransferItem Collection
//
// Revision 1.16  2005/10/21 17:47:22  mkontak
// Changed implementation of DicomHosts and how the server is defined
//
// Revision 1.15  2005/10/20 19:00:35  romy
// added proper close method.
//
// Revision 1.14  2005/10/20 15:27:15  romy
// modified temp path to Viatronix Temp path.
//
// Revision 1.13  2005/10/13 19:47:22  romy
// added TransferProvider Arg param.
//
// Revision 1.12  2005/10/11 13:56:30  romy
// modified with Transfer Args
//
// Revision 1.11  2005/10/03 15:11:39  romy
// modified Transfer Providers
//
// Revision 1.10  2005/09/30 18:41:18  romy
// Fixed the cancel issue
//
// Revision 1.9  2005/09/30 17:13:09  romy
// Transfer cancel method added
//
// Revision 1.8  2005/09/26 15:53:36  romy
// added new Transfer mechanism
//
// Revision 1.7  2005/09/22 17:28:48  romy
// added comments
//
// Revision 1.6  2005/09/20 12:21:25  romy
// demo checkin
//
// Revision 1.5  2005/09/18 02:53:45  romy
// added fieldselector
//
// Revision 1.4  2005/09/15 18:36:53  romy
// anonimization progress event added
//
// Revision 1.3  2005/09/14 16:51:37  romy
// modified anonymization reference
//
// Revision 1.2  2005/09/12 12:51:23  gdavidson
// Updated the transfer mechanism.
//
// Revision 1.1  2005/09/03 01:02:01  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferManager.cs,v 1.27.2.8 2011/05/18 01:56:59 mkontak Exp $
// $Id: TransferManager.cs,v 1.27.2.8 2011/05/18 01:56:59 mkontak Exp $

#endregion revision History


