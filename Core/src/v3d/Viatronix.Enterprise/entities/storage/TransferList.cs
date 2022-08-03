// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: TransferList.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{

  #region TransferList class

  /// <summary>
  /// Job object used for the pending jobs list
  /// </summary>
  public class TransferList : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string TransactionId = "tid";
      public const string Study = "study";
    } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// Studies part of this transfer
    /// </summary>
    private List<Study> m_studies = new List<Study>();


    /// <summary>
    /// Trnasaction Id
    /// </summary>
    private string m_tid = string.Empty;

 
    #endregion



    #region properties


    /// <summary>
    /// Gets the studies associated with this transfer
    /// </summary>
    public List<Study> Studies
    {
      get { return m_studies; }
    }


 

    /// <summary>
    /// Gets the transaction id
    /// </summary>
    public string TransactionId
    {
      get { return m_tid; }
    } // TransactionId



    #endregion


    #region IEntity

    /// <summary>
    /// Creates an xml representation of a Series object.
    /// </summary>
    /// <returns>Xml string representing the Series object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(Tag);
        {

          writer.WriteAttributeString(Attributes.TransactionId, m_tid);




          WriteAttributeStrings(writer);
          WriteElements(writer);

          if (m_studies.Count > 0)
          {
            writer.WriteStartElement("params");

            foreach (Study study in m_studies)
            {
              writer.WriteRaw(study.ToXml());
            }

            writer.WriteEndElement();

          }

        }


        writer.WriteEndElement();
      }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml node
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {


      base.Load(node);

      /// Validates the schema
      //node.ValidateSchema(Schema.Settings);

      m_tid = node.Attributes[Attributes.TransactionId].Value;


      foreach (XmlNode study in node.SelectNodes(Attributes.Study))
      {
        m_studies.Add(Study.Create(study));
      }

 
    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      TransferList TransferList = new TransferList();
      CopyTo(TransferList);

      TransferList.m_tid = m_tid;

      this.m_studies.ForEach(study => TransferList.m_studies.Add((Study)study.Clone()));


      return TransferList;
    } // Clone()


    /// <summary>
    /// Checks for value equality of TransferList objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      TransferList other = obj as TransferList;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_tid, other.m_tid);

    } // Equals( obj )

    #endregion

    #region static methods



    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A TransferList object.</returns>
    public static TransferList Create(XmlNode node)
    {
      TransferList TransferList = new TransferList();

      if (TransferList == null)
        throw new OutOfMemoryException("Failed to allocate TransferList");

      TransferList.Load(node);

      return TransferList;
    } // FromXml( node )

    #endregion

  } // class TransferList

  #endregion


} // namespace Viatronix.Enterprise.Entities

