// $Id: DatabaseTransferProvider.cs,v 1.9 2007/02/28 19:58:15 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections.Generic;
using System.Configuration;
using Viatronix.v3D.Core;
using System.Xml;
using System.Collections;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Console;

namespace Viatronix.Console
{
  /// <summary>
  /// Summary description for TransferProviderDatabase.
  /// </summary>
  public class DatabaseTransferProvider : ITransferProvider
  {

    #region fields

    /// <summary>
    /// Database data provider
    /// </summary>
    private IDataProvider m_dataProvider;

    /// <summary>
    /// Transfer progress event handler
    /// </summary>
    public event TransferProgressEventHandler Progress = null;

    #endregion

    #region properties

    #endregion properties

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    public DatabaseTransferProvider()
    {
      //m_progressHandler   = new ProgressEventHandler( this.OnProgressEventHandler );
    }//DatabaseTransferProvider()

    #endregion

    #region events

    /// <summary>
    /// Abort 
    /// </summary>
    public void Abort()
    {
    }//abort

    /// <summary>
    /// Handler the On Progress event
    /// </summary>
    /// <param name="args"></param>
    protected virtual void OnProgress( TransferProgressEventArgs args )
    {
      if( this.Progress != null )     
        this.Progress( this, args );
    }//OnProgress( TransferProgressEventArgs args )

    // TODO: New
    /// <summary>
    /// Handles the Importing progress from the cache client
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">ImportEventArgs</param>
    //protected void OnCacheImporting(object sender, ImportEventArgs args)
    //{
    //  TransferProgressEventArgs transferProgressArgs = new TransferProgressEventArgs("Importing...", args.Imported, args.Total );
    //  OnProgress( transferProgressArgs );
    //} // OnCacheImporting( sender, args)
    

    #endregion events
    
    #region ITransferProvider Members

    /// <summary>
    /// Transfer Series Collection to a removable media
    /// </summary>
    /// <param name="seriesCollection"></param>
    public void Transfer( TransferProviderArgs args )
    {
      try
      {

        foreach (Series series in args.SeriesCollection )
        {
          m_dataProvider.GetFiles( series);
        }

        int totalFiles =  args.TotalSeriesFilesCount() + 10;

        OnProgress( new TransferProgressEventArgs( "Updating the server...", 0, totalFiles  ));

        //Find the roor folder for all these series collection.
        Study study = new Study();
        string rootPath = string.Empty;

        // Add references at the end, so all series are added first.
        List<KeyValuePair<Series, Series>> references = new List<KeyValuePair<Series, Series>>();

        IDictionaryEnumerator seriesEnumerator = args.TransferData.GetEnumerator();
        while (seriesEnumerator.MoveNext())
        {
          Series series = (Series)seriesEnumerator.Key;

          string studyPath    = System.IO.Path.GetDirectoryName(System.IO.Path.GetDirectoryName(series.Path));
          string patientPath  = System.IO.Path.GetDirectoryName(studyPath);

          if (patientPath.Equals(string.Empty))
          {
            Viatronix.Logging.Log.Error("Transfer failed. Empty Path for series" + series.Uid, "DatabaseTransferProvider", "Transfer");
            throw new TransferException("Transfer failed. Empty Path ");
          }

          //Import files to the Server.
          if (rootPath != patientPath)
          {
            List<Series> seriesCollection = args.SeriesCollection;
            rootPath = patientPath;
            Viatronix.Logging.Log.Debug(" Importing to Cache From " + rootPath, "DatabaseTransferProvider", "Transfer");

          }


          // TODO: New So we need to read the study.xml?
          //Add the Study by getting the study Node from study XML
          //Study newStudy = Study.FromFile(TransferUtilities.GetStudyXml(series));
          Study newStudy = new Study();

          if (!study.Equals(newStudy))
          {
            study = newStudy;


            m_dataProvider.Insert( newStudy );
          }

          int position = series.Path.LastIndexOf('\\');
          series.Directory = series.Path.Substring(position + 1, series.Path.Length - (position + 1));
          study.Series.Add(series);
          m_dataProvider.Insert( series);

          foreach (Series reference in series.References)
          {
            references.Add(new KeyValuePair<Series, Series>(series, reference));
          }
        }

        // Add the references after everything else was added.
        foreach (KeyValuePair<Series, Series> keyPair in references)
        {
          // TODO: New ( Add References )
          // Database.AddSeriesReference(keyPair.Key, keyPair.Value);
        }

        OnProgress( new TransferProgressEventArgs( "finished transfer...", totalFiles, totalFiles  ));
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed to transfer selected study " + ex.Message, "DatabaseTransferProvider", "Transfer");
        throw new TransferException( "Failed to transfer selected study. " + ex.Message );
      }
    }//Transfer( seriesCollection )



    #endregion ITransferProvider Members

    #region methods

    
    #endregion methods
  
  }// public class DatabaseTransferProvider : ITransferProvider

}//namespace Viatronix.Console


#region revision History

// $Log: DatabaseTransferProvider.cs,v $
// Revision 1.9  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.8  2006/10/05 13:27:40  romy
// fixed build issue - reverted back
//
// Revision 1.7  2006/10/05 13:16:57  romy
// fixed build issue
//
// Revision 1.6  2006/10/04 17:44:43  mkontak
// Changes for transfer fix
//
// Revision 1.5  2006/07/06 20:28:16  romy
// added Retrieve data from data provider to fill the files information
//
// Revision 1.4  2006/02/16 15:09:17  romy
// using the ISessionProvider
//
// Revision 1.3  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.2  2005/12/13 21:36:59  romy
// fixed Progress event on Import
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.15  2005/12/02 02:41:43  romy
// coding standards
//
// Revision 1.14  2005/11/09 16:53:52  romy
// added exception handling for cache Export
//
// Revision 1.13  2005/11/04 19:28:53  romy
// Modified the IMport
//
// Revision 1.12  2005/11/04 16:17:12  romy
// commented export
//
// Revision 1.11  2005/11/01 15:39:04  romy
// fixed duplicate  series issue
//
// Revision 1.10  2005/10/27 18:15:53  romy
// Args  change
//
// Revision 1.9  2005/10/20 15:22:26  romy
// modified the order of transfer -1st files then database
//
// Revision 1.8  2005/10/11 13:46:06  romy
// transfer change
//
// Revision 1.7  2005/10/05 12:27:49  romy
// temp code added for study Transfer.
//
// Revision 1.6  2005/10/04 12:22:54  romy
// added cache client Import Method
//
// Revision 1.5  2005/09/30 16:05:38  romy
// added args param
//
// Revision 1.4  2005/09/26 15:51:45  romy
// temp transfer
//
// Revision 1.3  2005/09/22 16:16:13  romy
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/DatabaseTransferProvider.cs,v 1.9 2007/02/28 19:58:15 romy Exp $
// $Id: DatabaseTransferProvider.cs,v 1.9 2007/02/28 19:58:15 romy Exp $

#endregion revision History