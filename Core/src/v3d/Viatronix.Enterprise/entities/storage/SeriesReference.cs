// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: SeriesReference.cs
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
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// SeriesReference object used for the pending jobs list
  /// </summary>
  public class SeriesReference : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string SeriesUid = "uid";
      public const string ReferencedSeriesUid = "ruid";
    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Refrenced Uid
    /// </summary>
    private string m_referencedUid = string.Empty;


    #endregion

    #region properties

    /// <summary>
    /// Get the series uid
    /// </summary>
    public string SeriesUid
    { get { return m_uid; } }

    /// <summary>
    /// Gets the referenced series uid
    /// </summary>
    public string ReferncedSeriesUid
    { get { return m_referencedUid; } }




    #endregion

    #region methods

    #region static methods


    /// <summary>
    /// Creates a new instance of a SeriesReference object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A SeriesReference object.</returns>
    public static SeriesReference Create(XmlNode node)
    {


      SeriesReference reference = new SeriesReference();

      if (reference == null)
        throw new OutOfMemoryException("Failed to allocate SeriesReference");

      reference.Load(node);

      return reference;
    } // Create( node )

    #endregion


    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_uid.GetHashCode();
    }



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


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.SeriesUid, m_uid);
          writer.WriteAttributeString(Attributes.ReferencedSeriesUid, m_referencedUid);

        }

        writer.WriteEndElement();


      }


      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {

      base.Load(node);


      // validate job schema
      //node.ValidateSchema(Schema.Settings);

      m_uid = node.Attributes[Attributes.SeriesUid].Value;
      m_referencedUid = node.Attributes[Attributes.ReferencedSeriesUid].Value;


    } // Load(XmlNode node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      SeriesReference path = new SeriesReference();
      CopyTo(path);

      path.m_referencedUid = m_referencedUid;
      path.m_uid = m_uid;

      return path;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      SeriesReference other = obj as SeriesReference;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_referencedUid, other.m_referencedUid) &&
              string.Equals(m_uid, other.m_uid);

    } // Equals( obj )

    #endregion


    #endregion

  } // class SeriesReference
} // namespace Viatronix.Enterprise.Entities
