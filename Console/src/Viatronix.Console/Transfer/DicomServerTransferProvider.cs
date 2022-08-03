// $Id: DicomServerTransferProvider.cs,v 1.4 2007/03/13 17:20:33 mkontak Exp $ 
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
using System.Xml;
using System.Collections;
using Viatronix.Utilities;
using System.Collections.Generic;
using System.IO;
using Viatronix.UI;
using System.Threading;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;



namespace Viatronix.Console
{
  /// <summary>
  /// Summary description for TransferProviderDatabase.
  /// </summary>
  public class DicomServerTransferProvider : ITransferProvider
  {
  
    #region fields

    /// <summary>
    /// Data provider 
    /// </summary>
    public IDataProvider m_dataProvider = null;

    /// <summary>
    /// Transfer Progress Event Handler object
    /// </summary>
    public event TransferProgressEventHandler Progress = null;

    /// <summary>
    /// Transfer Item which holds the transfer Item text for progress bar.
    /// </summary>
    private string m_transferItem = string.Empty;

    /// <summary>
    /// Event that signals on Transfer completion.
    /// </summary>
    private System.Threading.AutoResetEvent m_resetevent = new System.Threading.AutoResetEvent( false );

    /// <summary>
    /// ShowMessage handler
    /// </summary>
    private SetterDelegate<string> m_showMessageHandler = null;

    /// <summary>
    /// Transfer Provider Args.
    /// </summary>
    protected TransferProviderArgs m_transferProviderArgs = null;

    #endregion

    #region properties

    #endregion properties

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    public DicomServerTransferProvider()
    {
      m_showMessageHandler = new SetterDelegate<string>(ShowMessageBox);
    }//DicomServerTransferProvider()

    #endregion

    #region ITransferProvider Members

    /// <summary>
    /// Transfer Series Collection to a removable media
    /// </summary>
    /// <param name="seriesCollection"></param>
    public void Transfer( TransferProviderArgs args )
    {
      m_transferProviderArgs = args;

      try
      {

        if ( args.TransferObject == null )
        {
          Viatronix.Logging.Log.Error("Dicom Server aetitle is empty. Aborting transfer ", "DicomServerTransferProvider", "Transfer");
          throw new TransferException ( " No Valid Dicom Server aetitle " );
        }

        string aeTitle = (string)args.TransferObject;

        foreach( Series series in args.SeriesCollection )
        {
          string studyUID  = series.StudyUid;
          string seriesUID = series.Uid;

          Viatronix.Logging.Log.Debug("Pushing series from " + args.DataProvider.Source + " to " + aeTitle + " ... [STUDY_UID=" + studyUID + "] {SERIES_UID=" + seriesUID + "]", "DicomServerTransferProvider", "Transfer");
          Global.DataProvider.Create( PacsJob.CreatePushJob(args.DataProvider.Source, aeTitle, studyUID, seriesUID) );
        }

        ShowMessageBox("Push to " + aeTitle + " has been queued, check the PACS Queue status");

      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "DicomServerTransferProvider", "Transfer");
        throw ex;
      }
      finally 
      {
      }
      
    }//Transfer( TransferProviderArgs args )


    /// <summary>
    /// Display the number of Disks reauired message box.
    /// </summary>
    /// <param name="number"></param>
    private void ShowMessageBox(string text)
    {

      if (m_transferProviderArgs.Frame != null)
      {
        // on a seperate thread invoke on UI thread
        if (m_transferProviderArgs.Frame.InvokeRequired)
          m_transferProviderArgs.Frame.Invoke(m_showMessageHandler, new object[] { text });
        else
        {
          // on UI thread display dialog
          Viatronix.UI.MessageBox.ShowInfo(m_transferProviderArgs.Frame, text);
        }
      }
      else
      {
        // on a seperate thread invoke on UI thread
        if (Global.Frame.InvokeRequired)
          Global.Frame.Invoke(m_showMessageHandler, new object[] { text });
        else
        {
          // on UI thread display dialog
          Viatronix.UI.MessageBox.ShowInfo(Global.Frame, text);
        }
      }
    }//ShowMessageBox ( string text )


    #endregion ITransferProvider Members

    #region methods

    /// <summary>
    /// Abort Method
    /// </summary>
    public void Abort()
    {
    }// Abort()


    #endregion methods
  
  }// public class DatabaseTransferProvider : ITransferProvider
    
}//namespace Viatronix.Console


#region revision History
#endregion revision History