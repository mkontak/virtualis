// $Id: TransferItem.cs,v 1.6 2007/02/28 20:51:39 romy Exp $
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
using System.Xml;
using System.Runtime.Serialization;

namespace Viatronix.Console
{
  /// <summary>
  /// Class holds the Transfer Item details
  /// </summary>
  /// <summary>
  /// Series data type
  /// </summary>
  [Serializable]
  public class TransferItem : ICloneable
  {
   
    #region fields

    
    /// <summary>
    /// Holds the Transfer Item Type
    /// </summary>
    private string m_itemName = string.Empty;

    /// <summary>
    /// The type of study type
    /// </summary>
    private string m_studyType = string.Empty;

    /// <summary>
    /// Holds the transfer Item's formats.
    /// </summary>
    private string [] m_formats=null;

    /// <summary>
    /// Flag indicates whether this item needs transfer or not.
    /// </summary>
    private bool m_checkState = false;

    /// <summary>
    /// The Search in Series Type name
    /// </summary>
    private string m_searchInSeries = string.Empty;


    /// <summary>
    /// Indicates any special folders of a series to search for  patterns
    /// </summary>
    private string m_searchInFolder = string.Empty;

    
    /// <summary>
    /// Indicates any special folders of a series to search for  patterns
    /// </summary>
    private string m_sessionTypeName = string.Empty;

   

    #endregion

    #region properties

    public string TransferStudyType
    {
      get { return m_studyType; }
      set { m_studyType = value;}
    }

    /// <summary>
    /// Gets or Sets TransferItem Type.
    /// </summary>
    public string Name
    {
      get { return m_itemName;  }
      set { m_itemName = value; }
    }//Type


    /// <summary>
    /// Gets or Sets the Formats
    /// </summary>
    public string[] Formats
    {
      get { return m_formats;  }
      set { m_formats = value; }
    }//Formats



    /// <summary>
    /// Gets or Sets the CheckState flag of the item in the GUI.
    /// </summary>
    public bool CheckState
    {
      get { return m_checkState;  }
      set { m_checkState = value; }
    }//CheckState

    /// <summary>
    /// Search this Series for Items.
    /// </summary>
    public string SearchInSeries
    {
      get { return m_searchInSeries; }
      set { m_searchInSeries = value; }
    }//SearchInSeries

    /// <summary>
    /// Search this Folder for Item formats.
    /// </summary>
    public string SearchInFolder
    {
      get { return m_searchInFolder; }
      set { m_searchInFolder = value; }
    }//SearchInFolder

    /// <summary>
    /// Search this Folder for Item formats.
    /// </summary>
    public string SessionTypeName
    {
      get { return m_sessionTypeName; }
      set { m_sessionTypeName = value; }
    }//SearchInFolder


    
    
    #endregion

    #region construction
    
    /// <summary>
    /// Default constructor
    /// </summary>
    public TransferItem()
    {
    }//TransferItem()


    #endregion construction

    #region Clone Method

    /// <summary>
    /// Creates a new object that is a copy of the current instance.
    /// </summary>
    /// <returns>A new object that is a copy of this instance.</returns>
    public object Clone()
    {
      TransferItem item = new TransferItem();

      item.Name = this.Name;
      item.TransferStudyType = this.TransferStudyType;
      item.Formats = (string[])this.Formats.Clone();
      item.CheckState = this.CheckState;
      item.SearchInSeries  = this.SearchInSeries;
      item.SearchInFolder  = this.SearchInFolder;
      item.SessionTypeName = this.SessionTypeName;
     
      return item;
    }
     

    #endregion 

    #region methods

    /// <summary>
    /// Get the transfer items subtypes
    /// </summary>
    /// <param name="seriesType">string</param>
    /// <returns></returns>
    public static string GetTransferSubType( string seriesType )
    {
      string subType = "default";
      if ( seriesType == string.Empty || seriesType.ToLower() != "vc" )
        subType = "default";
      else
        subType = seriesType.ToLower();

      return subType;
    }//GetTransferSubType( string seriesType )

    #endregion

  }//class TransferItem


  /// <summary>
  /// Collection of Transfer Item objects
  /// </summary>
  [Serializable]
  public class TransferItemsCollection : CollectionBase, ICloneable
  {

    #region construction
    public TransferItemsCollection()
    {
    }
    #endregion

    #region typed methods

    /// <summary>
    /// Gets a Transfer Item object at the specified index in the collection.
    /// </summary>
    public TransferItem this[ int index ]
    {
      get { return (TransferItem)List[index]; }
    } // Indexer



    /// <summary>
    /// Gets all series in the collection of the specified type
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public TransferItem GetItem( string name, string studyType )
    {
     // ArrayList list = new ArrayList();
      // Loop though all the transfer items
      foreach ( TransferItem item in List )
      {
        if ( item.Name == name && item.TransferStudyType == studyType  )
          return item;
      } 
      return null;
    } // GetItem( string type )


    /// <summary>
    /// Gets the study type items
    /// </summary>
    /// <param name="studyType">string</param>
    /// <returns></returns>
    public TransferItemsCollection GetStudyTypeItems( string studyType )
    {
      TransferItemsCollection items = new TransferItemsCollection();

      foreach ( TransferItem item in List )
      {
        if ( item.TransferStudyType == studyType  )
          items.Add( item );
      } 

      return items;
    }//GetStudyTypeItems( string studyType )


   
    
    /// <summary>
    /// Removes the series with the uid specified
    /// </summary>
    /// <param name="uid">UID</param>
    public void Remove( string name, string studyType )
    {
      for ( int index = 0; index < List.Count; ++index )
      {
        TransferItem item = ( TransferItem )List[index];
        if ( item.Name.Equals( name ) &&   item.TransferStudyType.Equals( studyType ))
        {
          RemoveAt(index);
          break;
        }
      }

    } // Remove()

   
    /// <summary>
    /// Removes the study with the uid specified
    /// </summary>
    /// <param name="uid">UID</param>
    public void Remove( TransferItem item )
    {
      Remove( item.Name, item.TransferStudyType );
    } // Remove()

    /// <summary>
    /// Adds new Transfer Item to collection
    /// </summary>
    /// <param name="series">Series to add</param>
    public void Add( TransferItem item )
    {
      // Makes sure the Transfer Items does not already exist
      if ( ! Contains( item ) )
        List.Add( item );

    } // Add()

   

    /// <summary>
    /// Determines if the Transfer Item with the specified type is in the collection
    /// </summary>
    /// <param name="Transfer Item">item to check</param>
    /// <returns>True if it's in the list, false otherwise</returns>
    public bool Contains( TransferItem searchItem )  
    {
      if( searchItem == null )
        return false;

      for ( int index = 0; index < List.Count; ++index )
      {
        TransferItem item = ( TransferItem )List[index];
        if ( item.TransferStudyType.Equals( searchItem.TransferStudyType ) &&   item.Name.Equals( searchItem.Name ) )
        {
          RemoveAt(index);
          return true;
        }
      }

      return false;
    } // Contains()

   
    #endregion 

    #region ICloneable Members

    /// <summary>
    /// Clone method
    /// </summary>
    /// <returns></returns>
    public object Clone()
    {
      TransferItemsCollection collection = new TransferItemsCollection();
      foreach( TransferItem item in this.List )
        collection.Add( (TransferItem) item.Clone() );

      return collection;
    }//Clone()

    #endregion

    #region methods

    /// <summary>
    /// Gets the Transfer Items from the App Config.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a TransferItemsCollection object.</param>
    /// <returns>A TransferInfo object.</returns>
    public static TransferItemsCollection FromXml( XmlNode node )
    {
      TransferItemsCollection transferItems = new TransferItemsCollection();
      //Get the excluded Series Types
     
      try
      {
        XmlNodeList itemList = node.SelectNodes("item");
        foreach ( XmlNode transferitemNode in itemList )
        {
          TransferItem transferItem = new TransferItem(); 

          transferItem.TransferStudyType = ( transferitemNode.Attributes[ "subtype" ] != null ? transferitemNode.Attributes[ "subtype" ].Value : string.Empty );
          transferItem.Name              = ( transferitemNode.Attributes[ "name" ] != null ? transferitemNode.Attributes[ "name" ].Value : string.Empty );
          transferItem.SearchInSeries    = ( transferitemNode.Attributes[ "searchInSeries" ] != null ? transferitemNode.Attributes[ "searchInSeries" ].Value : string.Empty );
          transferItem.SearchInFolder    = ( transferitemNode.Attributes[ "searchInFolder" ] != null ? transferitemNode.Attributes[ "searchInFolder" ].Value : string.Empty );
          transferItem.Formats           = ( transferitemNode.Attributes[ "formats" ] != null ? transferitemNode.Attributes[ "formats" ].Value.ToString().Split(',') : new string [0] );
          transferItem.CheckState        = ( transferitemNode.Attributes[ "checkState" ] != null ? ( transferitemNode.Attributes[ "checkState" ].Value.ToString() == "true" ? true : false ) : false );

          try
          {
            transferItem.SessionTypeName   = transferitemNode.Attributes[ "sessiontypename" ].Value;
          }
          catch ( Exception ex )
          {
            Viatronix.Logging.Log.Information("No session type for " + transferItem.Name + ex.Message, "TransferItemsCollection", "FromXml");         
          }

          transferItems.Add ( transferItem );
        }
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Invalid transfer item : " + ex.Message, "TransferItemsCollection", "FromXml");         
        throw;
      }

      return transferItems;
    } // FromXml( node )
      

    #endregion

  }//TransferItemsCollection

}


#region revision History

// $Log: TransferItem.cs,v $
// Revision 1.6  2007/02/28 20:51:39  romy
// added comments before the code review
//
// Revision 1.5  2006/04/05 19:47:07  mkontak
// Fix problem
//
// Revision 1.4  2006/02/16 15:31:19  romy
// no message
//
// Revision 1.3  2005/10/27 18:14:50  romy
// Revision Macro Fix
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/TransferItem.cs,v 1.6 2007/02/28 20:51:39 romy Exp $
// $Id: TransferItem.cs,v 1.6 2007/02/28 20:51:39 romy Exp $

#endregion revision History
