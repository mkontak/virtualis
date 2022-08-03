// $Id: TransferProviderDetails.cs,v 1.9.2.1 2008/10/03 19:35:59 kchalupa Exp $
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
using Viatronix.v3D.Core;

namespace Viatronix.Console
{
  /// <summary>
  /// Summary description for TransferInfo.
  /// </summary>
  public class TransferProvider
  {
    #region fields

    /// <summary>
    /// Parameters used in the transfer
    /// </summary>
    private System.Collections.Hashtable m_parameters = new System.Collections.Hashtable();

    /// <summary>
    /// Transfer Provider Name
    /// </summary>
    private string m_name;

    /// <summary>
    /// Include reference series 
    /// </summary>
    private bool m_includeReferences = true;

    /// <summary>
    /// Transfer Provider Type
    /// </summary>
    private string m_typeName;

    /// <summary>
    /// Transfer Provider Description
    /// </summary>
    private string m_description;

    /// <summary>
    /// Transfer Provider Anonymization flag
    /// </summary>
    private bool m_anonymize = false;

    /// <summary>
    /// Determines whether override is allowed in this Transfer Provider.
    /// </summary>
    private bool m_override = false;

    /// <summary>
    /// Allow item selection 
    /// </summary>
    private bool m_allowItemSelection = true;

    /// <summary>
    /// Determines if the provider is async or sync. Async implies that the progress will be returned as in
    /// events, but sync impliesthey will not
    /// </summary>
    private bool m_asynchronous = true;

    /// <summary>
    /// Series collection Transfer indicator
    /// </summary>
    private bool m_seriesCollectionTransfer = true;

    /// <summary>
    /// Selected Transfer Items for this Transfer Provider
    /// </summary>
    private TransferItemsCollection m_selectedTransferItems = new TransferItemsCollection();

    /// <summary>
    /// Hold a transfer object. Using for PACS.
    /// </summary>
    private object m_transferObject = null;


    #endregion

    #region properties


    /// <summary>
    /// Sets/Gets the include referenced series flag
    /// </summary>
    public bool IncludeReferences
    {
      get { return m_includeReferences; }
      set { m_includeReferences = value; }
    } // IncludeReferences

    /// <summary>
    /// Transfer parameters
    /// </summary>
    public System.Collections.Hashtable Parameters
    {
      get { return m_parameters; }
      set { m_parameters = value; }
    }//Parameters

    /// <summary>
    /// Gets or Sets the Asynchronous flag
    /// </summary>
    public bool Asynchronous
    {
        get {return m_asynchronous;}
        set {m_asynchronous = value;}
    }//Asynchronous


    /// <summary>
    /// Gets/Sets the allow item selection flag
    /// </summary>
    public bool AllowItemSelection
    {
      set { m_allowItemSelection = value; }
      get { return m_allowItemSelection; }
    } // AllowItemSeection

    /// <summary>
    /// Gets or Sets the TransferInfo Name
    /// </summary>
    public string TransferProviderType
    {
      get {return m_name;}
      set {m_name = value;}
    }//TransferProviderType


    /// <summary>
    /// gets of sets the Type Name 
    /// </summary>
    public string TransferProviderTypeName
    {
      get {return m_typeName;}
      set {m_typeName = value;}
    }//TransferProviderTypeName


    /// <summary>
    /// Transfer Provider Description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value;}
    }//Description


    /// <summary>
    /// Gets or Sets the aonymize Flag
    /// </summary>
    public bool IsAnonymize
    {
      get { return m_anonymize; }
      set { m_anonymize = value; }
    }//IsAnonymize


    /// <summary>
    /// Gets or Sets the aonymize Flag
    /// </summary>
    public bool IsOverride
    {
      get { return m_override; }
      set { m_override = value; }
    }//IsOverride


    /// <summary>
    /// Gets or sets the Series collelction Transfer indicator
    /// </summary>
    public bool IsSeriesCollectionTransfer
    {
      get { return m_seriesCollectionTransfer; }
      set { m_seriesCollectionTransfer = value; }
    }//IsList<Series>Transfer
    

    /// <summary>
    /// Gets or Sets the transfer object
    /// </summary>
    public object TransferObject
    {
      get { return m_transferObject; }
      set { m_transferObject = value; }
    }//TransferObject


    /// <summary>
    /// Selected TransferItem Collection
    /// </summary>
    public TransferItemsCollection SupportedTransferItems
    {
      get { return m_selectedTransferItems;}
      set { m_selectedTransferItems = value;}
    }//SelectdTransferItems


    #endregion

    #region Construction

    /// <summary>
    /// Construction
    /// </summary>
    public TransferProvider()
    {
    }//TransferProvider( )


    /// <summary>
    /// Copy Constructor
    /// </summary>
    /// <param name="transferProviderDetails">TransferProviderDetails</param>
    public TransferProvider ( TransferProvider  transferProviderDetails )
    {
      this.m_name           = transferProviderDetails.m_name;
      this.m_typeName       = transferProviderDetails.m_typeName;
      this.m_description    =  transferProviderDetails.m_description;
      this.m_anonymize      = transferProviderDetails.m_anonymize;
      this.m_asynchronous   = transferProviderDetails.m_asynchronous;
      this.m_override       = transferProviderDetails.m_override;
      this.m_seriesCollectionTransfer = transferProviderDetails.m_seriesCollectionTransfer;
      this.m_selectedTransferItems  = (TransferItemsCollection)transferProviderDetails.m_selectedTransferItems.Clone();
      this.m_transferObject = transferProviderDetails.m_transferObject;
      this.m_parameters     = transferProviderDetails.m_parameters;
      this.m_allowItemSelection = transferProviderDetails.AllowItemSelection;
    }//TransferProviderDetails ( TransferProviderDetails  transferProviderDetails )


    #endregion

    #region methods

    /// <summary>
    /// ToString mnethod
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return this.TransferProviderType;
    }//ToString

    /// <summary>
    /// Creates a new instance of a transferInfo object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a TransferInfo object.</param>
    /// <returns>A TransferInfo object.</returns>
    public static TransferProvider FromXml( XmlNode node )
    {
      TransferProvider transferProvider = new TransferProvider();

      transferProvider.TransferProviderType     = node.Attributes[ "name" ].Value;
      transferProvider.TransferProviderTypeName = node.Attributes[ "typename" ].Value;
      transferProvider.Description              = ( node.Attributes[ "description" ] != null ? node.Attributes[ "description" ].Value.ToString() : string.Empty );
      transferProvider.IsAnonymize              = ( node.Attributes[ "anonymize" ] != null ? (node.Attributes[ "anonymize" ].Value.ToString() == "true" ? true : false ) : false );
      transferProvider.IsOverride               = ( node.Attributes[ "override" ] != null ? (node.Attributes[ "override" ].Value.ToString()  == "true" ? true : false ) : false );
      transferProvider.Asynchronous             = ( node.Attributes[ "asynchronous" ] != null ? (node.Attributes[ "asynchronous" ].Value.ToString() == "true" ? true : false ) : true);
      transferProvider.AllowItemSelection       = ( node.Attributes[ "allowItemSelection" ] != null ? ( node.Attributes[ "allowItemSelection" ].Value.ToString() == "true" ? true : false ) : true );
      transferProvider.IncludeReferences  = ( node.Attributes[ "includeReferences" ] != null ? (node.Attributes["includeReferences"].Value.ToString() == "true" ? true : false) : true);
      try
      {
        transferProvider.IsSeriesCollectionTransfer  = ( node.Attributes[ "seriesCollectionTransfer" ] != null ? (node.Attributes[ "seriesCollectionTransfer" ].Value.ToString()  == "true" ? true : false ) : false);
      }
      catch (Exception ex )
      {
        Viatronix.Logging.Log.Error ( "Attribute seriesCollectionTransfer not available for node " + node.Name + ex.Message, "TransferProviderDetails", "FromXml");
      }

      //Get if there are any specific Transfer Items
      System.Xml.XmlNode transferParameters = node.SelectSingleNode( "transferParameters" );

      if ( transferParameters != null )
      {

        foreach( System.Xml.XmlNode parameter in transferParameters.SelectNodes("transferParameter") )
        {
          transferProvider.Parameters.Add(parameter.Attributes["name"].Value.ToString(), parameter.Attributes["value"].Value.ToString());
        }
      }

      //Get if there are any specific Transfer Items
      System.Xml.XmlNode transferItems = node.SelectSingleNode( "transferItems" );

      //Build id there are any specific Transfer Items
      if ( transferItems != null )
        transferProvider.SupportedTransferItems = TransferItemsCollection.FromXml( transferItems );
      else
        transferProvider.SupportedTransferItems = TransferItemsCollection.FromXml(  (XmlNode) System.Configuration.ConfigurationManager.GetSection( "transferItems" ));


      return transferProvider;
    } // FromXml( node )


    #endregion
     
  }//public class TransferProviderDetails
}//namespace Viatronix.Console

#region revision History

// $Log: TransferProviderDetails.cs,v $
// Revision 1.9.2.1  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.9  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.8  2007/01/19 20:53:18  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.7  2006/10/27 19:29:13  mkontak
// Added allowItemSelection to transfer providers
//
// Revision 1.6  2006/06/06 19:47:56  mkontak
// changed deprecated code
//
// Revision 1.5  2006/04/10 20:07:41  mkontak
// Added transfer parameters
//
// Revision 1.4  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.3  2006/02/07 15:08:01  romy
// added log details
//
// Revision 1.2  2006/01/25 21:15:03  romy
// transfer check for Multi-series transfer
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.6  2005/12/12 21:14:55  romy
// Transfer revisited
//
// Revision 1.5  2005/10/27 18:14:50  romy
// Revision Macro Fix
//
// Revision 1.4  2005/10/27 18:12:05  romy
// Revision Fix
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferProviderDetails.cs,v 1.9.2.1 2008/10/03 19:35:59 kchalupa Exp $
// $Id: TransferProviderDetails.cs,v 1.9.2.1 2008/10/03 19:35:59 kchalupa Exp $

#endregion revision History


