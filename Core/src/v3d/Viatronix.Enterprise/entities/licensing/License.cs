// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: License.cs
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
  /// Implements the license object
  /// </summary>
  public class License : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Key = "key";
      public const string Id = "id";
      public const string Count = "cnt";
      public const string DateIssued = "iss";
      public const string IssuedBy = "by";
      public const string DateAdded = "add";
      public const string ExpirationDate = "exp";
      public const string State = "state";
      public const string ErrorMessage = "msg";
    } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// License Key
    /// </summary>
    private string m_key = string.Empty;

    /// <summary>
    /// Error message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    ///  Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Viatronix Employee that issued the license
    /// </summary>
    private string m_issuedBy = string.Empty;

    /// <summary>
    /// Date the licensed was issued by Viatronix
    /// </summary>
    private DateTimeOffset? m_dateIssued = null;

    /// <summary>
    /// Date the license was actually adde to the system
    /// </summary>
    private DateTimeOffset? m_dateAdded = null;

    /// <summary>
    /// Date the license expires
    /// </summary>
    private DateTimeOffset? m_expirationDate = null;

    /// <summary>
    /// License state
    /// </summary>
    private LicenseStates m_state = LicenseStates.Available;

    /// <summary>
    /// Count of licenses
    /// </summary>
    private int m_count = 0;

   #endregion



    #region properties

    /// <summary>
    /// Gets the lciense key
    /// </summary>
    public string Key
    {
      get { return m_key; }
    }


    /// <summary>
    /// Gets the license id
    /// </summary>
    public string Id
    {
      get { return m_id; }
    }


    /// <summary>
    /// Gets the person who issued the license
    /// </summary>
    public string IssuedBy
    {
      get { return m_issuedBy; }
    } // IssuedBy

    /// <summary>
    /// Gets the date the license was issued
    /// </summary>
    public DateTimeOffset DateIssued
    { get { return m_dateIssued.Value; } }

    /// <summary>
    /// Gets the date added
    /// </summary>
    public DateTimeOffset DateAdded
    { get { return m_dateAdded.Value; } }

    /// <summary>
    /// Gets either the expiration date of infinite
    /// </summary>
    public string Expires
    { get { return ( m_expirationDate.HasValue ? m_expirationDate.Value.ToString("o") : "Infinite" ); } }

    /// <summary>
    /// Gets the license state
    /// </summary>
    public LicenseStates State
    { get { return m_state; } }

    /// <summary>
    /// Count
    /// </summary>
    public int Count
    { get { return m_count; } }


    /// <summary>
    /// Gets the error message
    /// </summary>
    public string ErrorMessage
    {
      get { return m_message; }
    } // ErrorMessage


    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_key;
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

          writer.WriteAttributeString(Attributes.Key, m_key);
          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Count, m_count.ToString());
          writer.WriteAttributeString(Attributes.DateAdded, m_dateAdded.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.DateIssued, m_dateIssued.Value.ToString("o"));

          if ( m_expirationDate.HasValue )
            writer.WriteAttributeString(Attributes.ExpirationDate, m_expirationDate.Value.ToString("o"));

          writer.WriteAttributeString(Attributes.State, ((int)m_state).ToString());
          writer.WriteAttributeString(Attributes.ErrorMessage, m_message);

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

      m_key = node.Attributes[Attributes.Key].Value;
      m_id = node.Attributes[Attributes.Id].Value;
      m_message = node.Attributes[Attributes.ErrorMessage].Value;
      m_count = Int32.Parse(node.Attributes[Attributes.Count].Value.ToString());
      m_dateAdded = DateTimeOffset.Parse(node.Attributes[Attributes.DateAdded].Value);
      m_dateIssued = DateTimeOffset.Parse(node.Attributes[Attributes.DateIssued].Value);

      m_expirationDate = null;
      node.TryGetAttributeValue<DateTimeOffset?>(Attributes.ExpirationDate, ref m_expirationDate);
   
      m_issuedBy = node.Attributes[Attributes.IssuedBy].Value;
      m_state = (LicenseStates)Enum.Parse(typeof(LicenseStates), node.Attributes[Attributes.State].Value);


    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      License type = new License();
      CopyTo(type);

      type.m_id = m_id;
      type.m_key = m_key;
      type.m_message = m_message;
      type.m_issuedBy = m_issuedBy;
      type.m_state = m_state;
      type.m_count = m_count;
      type.m_dateAdded = m_dateAdded;
      type.m_dateIssued = m_dateIssued;
      type.m_expirationDate = m_expirationDate;

      return type;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      License other = obj as License;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_key, other.m_key);

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static License Create(XmlNode node)
    {
      License license = new License();

      if (license == null)
        throw new OutOfMemoryException("Failed to allocate License");

      license.Load(node);

      return license;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static License Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Task



} // namespace Viatronix.Enterprise.Entities

