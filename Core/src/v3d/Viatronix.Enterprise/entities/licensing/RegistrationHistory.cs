// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RegistrationHistory.cs
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
  public class RegistrationHistory : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string SystemId = "sysid";
      public const string Name = "name";
      public const string Operation = "op";
      public const string Status = "status";
      public const string Date = "date";
    } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// Id
    /// </summary>
    private Int64 m_id = 0;

    /// <summary>
    /// System id
    /// </summary>
    private string m_systemId = string.Empty;

    /// <summary>
    /// System name
    /// </summary>
    private string m_systemName = string.Empty;

    /// <summary>
    ///  Operation performed
    /// </summary>
    private string m_operation = string.Empty;


    /// <summary>
    /// Date of the operation
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Status of operation
    /// </summary>
    private string m_status = string.Empty;




    #endregion



    #region properties

    /// <summary>
    /// Gets the Id
    /// </summary>
    public Int64 Id
    { get { return m_id; } }

    /// <summary>
    /// Gets the system id
    /// </summary>
    public string SystemId
    {
      get { return m_systemId; }
    }


    /// <summary>
    /// Gets the system name
    /// </summary>
    public string SystemName
    {
      get { return m_systemName; }
    }


    /// <summary>
    /// Gets the operation performed
    /// </summary>
    public string Operation
    {
      get { return m_operation; }
    } // Operation

    /// <summary>
    /// Gets the date of the operation
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value; } }

    /// <summary>
    /// Gets the status of the operation
    /// </summary>
    public string Status
    { get { return m_status; } }

    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_id.ToString();
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

          writer.WriteAttributeString(Attributes.SystemId, m_systemId);
          writer.WriteAttributeString(Attributes.Name, m_systemName);
          writer.WriteAttributeString(Attributes.Id, m_id.ToString());
          writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.Status, m_status);
          writer.WriteAttributeString(Attributes.Operation, m_operation);

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

      m_systemId = node.Attributes[Attributes.SystemId].Value;
      m_systemName = node.Attributes[Attributes.Name].Value;
      m_operation = node.Attributes[Attributes.Operation].Value;
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_status = node.Attributes[Attributes.Status].Value;


    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      RegistrationHistory type = new RegistrationHistory();
      CopyTo(type);

      type.m_systemId = m_systemId;
      type.m_systemName = m_systemName;
      type.m_operation = m_operation;
      type.m_date = m_date;
      type.m_status = m_status;

      return type;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      RegistrationHistory other = obj as RegistrationHistory;

      return other != null &&
             base.Equals(other) &&
             (m_id == other.m_id);

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static RegistrationHistory Create(XmlNode node)
    {
      RegistrationHistory history = new RegistrationHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to allocate RegistrationHistory");

      history.Load(node);

      return history;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static RegistrationHistory Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Task



} // namespace Viatronix.Enterprise.Entities

