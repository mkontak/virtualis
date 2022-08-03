// $Id: ITransferProvider.cs,v 1.15.2.1 2008/10/03 19:35:59 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections;
using System.Collections.Generic;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;



namespace Viatronix.Console
{
  
  /// <summary>
  /// Delagate to Transfer Progress Event Handler.
  /// </summary>
  public delegate void TransferProgressEventHandler( object sender, TransferProgressEventArgs args );

  /// <summary>
  /// Arguments container for TransferProvider
  /// </summary>
  public class TransferProviderArgs
  {
    #region fields

    /// <summary>
    /// Source data provider
    /// </summary>
    private IDataProvider m_dataProvider = null;

    /// <summary>
    /// Frame Object
    /// </summary>
    private System.Windows.Forms.Control m_frame = null;

    /// <summary>
    /// Stage Area Folder
    /// </summary>
    private string m_stageAreaFolder = string.Empty;

    /// <summary>
    /// Hold a transfer object
    /// </summary>
    private object m_transferObject = null;

    /// <summary>
    /// Holds the Data for Transfer operation.Series and Its Selected Transfer Items.
    /// </summary>
    private System.Collections.Hashtable m_transferData = new System.Collections.Hashtable();

    #endregion fields
 
    #region properties


    /// <summary>
    /// Source data provider
    /// </summary>
    public IDataProvider DataProvider
    {
      get { return m_dataProvider; }
      set { m_dataProvider = value; }
    } // DataProvider

    /// <summary>
    /// Gets or Sets the frame object
    /// </summary>
    public System.Windows.Forms.Control Frame
    {
      get { return m_frame; }
      set { m_frame = value;}
    }//Frame


    /// <summary>
    /// Gets or sets the Stage area folder.
    /// </summary>
    public string StageArea
    {
      get { return m_stageAreaFolder; }
      set { m_stageAreaFolder = value;}
    }//StageArea


    /// <summary>
    /// Gets or Sets the transfer object
    /// </summary>
    public object TransferObject
    {
      get { return m_transferObject; }
      set { m_transferObject = value; }
    }//TransferObject


    /// <summary>
    /// Holds the Transfer Data.
    /// </summary>
    public System.Collections.Hashtable TransferData
    {
      get { return m_transferData; }
      set { m_transferData = value; }
    }//TransferData

    /// <summary>
    /// Gets the Series Collection from the Transfer Data Object.
    /// </summary>
    public List<Series> SeriesCollection
    {
      get
      {
        List<Series> seriesCollection = new List<Series>();
        IDictionaryEnumerator enumerator = m_transferData.GetEnumerator();
        while ( enumerator.MoveNext() )
        {
          Series series = (Series)enumerator.Key;

  
          // TODO: New ( Do we need the clone)
          //seriesCollection.Add( (Series)series.Clone() );
          seriesCollection.Add(series);

        }
        return seriesCollection;
      }
    }//List<Series>

    #endregion

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="frame">Frame Object</param>
    /// <param name="stageFolder">string</param>
    /// <param name="transferObject">object</param>
    /// <param name="transferData">HashTable</param>
    /// <param name="dataProvider">Source data Provider</param>
    /// 
    public TransferProviderArgs ( System.Windows.Forms.Control frame, string stageFolder , object transferObject, System.Collections.Hashtable transferData, IDataProvider dataProvider )
    {
      m_frame             = frame;
      m_transferObject    = transferObject;
      m_stageAreaFolder   = stageFolder;
      m_dataProvider      = dataProvider;
      m_transferData      = (System.Collections.Hashtable)transferData.Clone();
    }//TransferProviderArgs( System.Windows.Forms.Control frame, string stageFolder , object transferObject, System.Collections.Hashtable transferData, IDataProvider dataProvider )

    /// <summary>
    /// Copy Constructor
    /// </summary>
    /// <param name="args"></param>
    public TransferProviderArgs ( TransferProviderArgs args )
    {
      m_frame            = args.m_frame;
      m_transferObject   = args.m_transferObject;
      m_stageAreaFolder  = args.m_stageAreaFolder;
      m_dataProvider     = args.DataProvider;
      m_transferData     = (System.Collections.Hashtable)args.m_transferData.Clone();
    }//TransferProviderArgs ( System.Windows.Forms.Control frame, List<Series> collection, string stageFolder )


    #endregion 

    #region methods
    /// <summary>
    /// Returns the Total count of Series Files
    /// </summary>
    /// <returns>int</returns>
    public int TotalSeriesFilesCount ()
    {
      int count = 0;
      IDictionaryEnumerator enumerator = m_transferData.GetEnumerator();
      while ( enumerator.MoveNext() )
      {
        Series series = (Series)enumerator.Key;
        count += series.Files.Count;
      }
      return count;
    }//TotalSeriesFilesCount()

    #endregion

  }//TransferProviderArgs


  /// <summary>
  /// Summary description for ITransferProvider.
  /// </summary>
  public interface ITransferProvider
  {
    /// <summary>
    /// Transfer Method
    /// </summary>
    /// <param name="args">TransferProviderArgs</param>
    void Transfer( TransferProviderArgs args );

    /// <summary>
    /// Abort Method
    /// </summary>
    void Abort();

    /// <summary>
    /// Transfer Progress Event Handler
    /// </summary>
    event TransferProgressEventHandler Progress;

  } // interface ITransferProvider


  /// <summary>
  /// Contains event data for the transfer's progress event.
  /// </summary>
  public class TransferProgressEventArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// Text message associated with the event.
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// The current progress of the transfer.
    /// </summary>
    private int m_value = 0;

    /// <summary>
    /// The total progress.
    /// </summary>
    private int m_total = 0;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a TransferProgressEventArgs.
    /// </summary>
    /// <param name="text">Text message associated with the event.</param>
    /// <param name="value">The current progress of the transfer.</param>
    /// <param name="total">The total progress.</param>
    public TransferProgressEventArgs( string text, int value, int total )
    {
      m_text  = text;
      m_value = value;
      m_total = total;
    } // TransferProgressEventArgs( text, value, total )
   
    #endregion

    #region properties

    /// <summary>
    /// Returns the text message associated with the event.
    /// </summary>
    public string Text
    {
      get { return m_text; }
    } // Text


    /// <summary>
    /// Returns the current progress of the transfer.
    /// </summary>
    public int Value
    {
      get { return m_value; }
    } // Value


    /// <summary>
    /// Returns the total progress.
    /// </summary>
    public int Total
    {
      get { return m_total; }
    } // Total

    #endregion

  } // class TransferProgressEventArgs


} // namespace Viatronix.v3D.Core


#region revision History

// $Log: ITransferProvider.cs,v $
// Revision 1.15.2.1  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.15  2007/02/15 23:16:20  mkontak
// Fixed problem with frame
//
// Revision 1.14  2006/02/16 12:45:30  mkontak
// Added data provider property so that the transfer can get the source of the data
//
// Revision 1.13  2005/12/13 21:37:50  romy
// fixed Import fix by adding clone elements to the Args series collectiion
//
// Revision 1.12  2005/12/12 21:09:27  romy
// more comments
//
// Revision 1.11  2005/10/27 18:14:50  romy
// Revision Macro Fix
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/interfaces/ITransferProvider.cs,v 1.15.2.1 2008/10/03 19:35:59 kchalupa Exp $
// $Id: ITransferProvider.cs,v 1.15.2.1 2008/10/03 19:35:59 kchalupa Exp $

#endregion revision History