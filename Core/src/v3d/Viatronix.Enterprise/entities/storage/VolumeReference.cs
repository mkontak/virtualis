// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: VolumeRefernecs.cs
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
  /// VolumeRefernecs object used for the pending jobs list
  /// </summary>
  public class VolumeReference : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string SeriesUid = "uid";
      public const string VolumeSeriesUid = "vuid";
      public const string ConversionId = "cid";
    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Refrenced Volume Series Uid
    /// </summary>
    private string m_volumeUid = string.Empty;

    /// <summary>
    /// Refernces volume conversion id
    /// </summary>
    private string m_conversionId = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Get the series uid
    /// </summary>
    public string SeriesUid
    { get { return m_uid; } }

    /// <summary>
    /// Gets the referenced volume series uid
    /// </summary>
    public string VolumeSeriesUid
    { get { return m_volumeUid; } }

    /// <summary>
    /// Gets the references volume's conversion id
    /// </summary>
    public string ConversionId
    { get { return m_conversionId; } }


    #endregion

    #region methods

    #region static methods


    /// <summary>
    /// Creates a new instance of a VolumeRefernecs object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A VolumeRefernecs object.</returns>
    public static VolumeReference Create(XmlNode node)
    {


      VolumeReference reference = new VolumeReference();

      if (reference == null)
        throw new OutOfMemoryException("Failed to allocate VolumeReference");

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
          writer.WriteAttributeString(Attributes.VolumeSeriesUid, m_volumeUid);
          writer.WriteAttributeString(Attributes.ConversionId, m_conversionId);

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
      m_volumeUid = node.Attributes[Attributes.VolumeSeriesUid].Value;
      m_conversionId = node.Attributes[Attributes.ConversionId].Value;


    } // Load(XmlNode node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      VolumeReference reference = new VolumeReference();
      CopyTo(reference);

      reference.m_volumeUid = m_volumeUid;
      reference.m_uid = m_uid;
      reference.m_conversionId = m_conversionId;

      return reference;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      VolumeReference other = obj as VolumeReference;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_volumeUid, other.m_volumeUid) &&
              string.Equals(m_uid, other.m_uid) &&
                string.Equals(m_conversionId, other.m_conversionId);

    } // Equals( obj )

    #endregion


    #endregion

  } // class VolumeReference
} // namespace Viatronix.Enterprise.Entities
