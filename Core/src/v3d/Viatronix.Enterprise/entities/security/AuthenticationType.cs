// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: AuthenticationType.cs
//
// Description: Object holding the authentication type used in the security system
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


  /// <summary>
  /// Authentication type object used for user objects
  /// </summary>
  public class AuthenticationType : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Description = "desc";
      public const string Default = "default";
    } // class Attributes

    #endregion


    #region fields


    /// <summary>
    ///  Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

 
    /// <summary>
    /// Message
    /// </summary>
    private bool m_default = false;

    #endregion



    #region properties

    /// <summary>
    /// Id of task
    /// </summary>
    public string Id
    {
      get { return m_id; }
    }

 
    /// <summary>
    /// Sets the description 
    /// </summary>
    public string Description
    {
      get { return m_description; }
    } // Jid

    /// <summary>
    /// Gets defauilt flag
    /// </summary>
    public bool Default
    {
      get { return m_default; }
    } // Uid


    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
 	    return m_id;
    } // ToString()

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


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Default, m_default.ToString());
 
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

      m_id = node.Attributes[Attributes.Id].Value;
      m_description = node.Attributes[Attributes.Description].Value;
      m_default = (node.Attributes[Attributes.Default].Value == "1" || string.Compare(node.Attributes[Attributes.Default].Value, "true", true) == 0 ? true : false);
 
    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      AuthenticationType type = new AuthenticationType();
      CopyTo(type);

      type.m_id = m_id;
      type.m_description = m_description;
      type.m_default = m_default;


      return type;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      AuthenticationType other = obj as AuthenticationType;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_id, other.m_id) ;

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static AuthenticationType Create(XmlNode node)
    {
      AuthenticationType type = new AuthenticationType();

      if (type == null)
        throw new OutOfMemoryException("Failed to allocate AuthenticationType");

      type.Load(node);

      return type;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static AuthenticationType Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class AuthenticationType



} // namespace Viatronix.Enterprise.Entities

