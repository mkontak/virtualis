// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RegisteredServer.cs
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
  public class RegisteredServer : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string SystemId = "sysid";
      public const string Name = "name";
      public const string IP = "ip";
      public const string DateInstalled = "ins";
      public const string DateRegistered = "date";
     } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// System id
    /// </summary>
    private string m_systemId = string.Empty;

    /// <summary>
    /// System name
    /// </summary>
    private string m_systemName = string.Empty;

    /// <summary>
    ///  IP
    /// </summary>
    private string m_ip = string.Empty;


    /// <summary>
    /// Date the system was installed
    /// </summary>
    private DateTimeOffset? m_dateInstalled = null;

    /// <summary>
    /// Date the system was registered
    /// </summary>
    private DateTimeOffset? m_dateRegistered = null;

 


    #endregion



    #region properties

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
    /// Gets the systems ip address
    /// </summary>
    public string IPAddress
    {
      get { return m_ip; }
    } // IP

    /// <summary>
    /// Gets the date the license was issued
    /// </summary>
    public DateTimeOffset DateInstalled
    { get { return m_dateInstalled.Value; } }

    /// <summary>
    /// Gets the date added
    /// </summary>
    public DateTimeOffset DateRegistered
    { get { return m_dateRegistered.Value; } }

    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_systemId;
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
          writer.WriteAttributeString(Attributes.IP, m_ip);
          writer.WriteAttributeString(Attributes.DateInstalled, m_dateInstalled.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.DateRegistered, m_dateRegistered.Value.ToString("o"));
 
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
      m_ip = node.Attributes[Attributes.IP].Value;
      m_dateRegistered = DateTimeOffset.Parse(node.Attributes[Attributes.DateInstalled].Value);
      m_dateInstalled = DateTimeOffset.Parse(node.Attributes[Attributes.DateRegistered].Value);


    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      RegisteredServer type = new RegisteredServer();
      CopyTo(type);

      type.m_systemId = m_systemId;
      type.m_systemName = m_systemName;
      type.m_ip = m_ip;
      type.m_dateRegistered = m_dateRegistered;
      type.m_dateInstalled = m_dateInstalled;

      return type;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      RegisteredServer other = obj as RegisteredServer;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_systemId, other.m_systemId);

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static RegisteredServer Create(XmlNode node)
    {
      RegisteredServer server = new RegisteredServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate RegisteredServer");

      server.Load(node);

      return server;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static RegisteredServer Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Task



} // namespace Viatronix.Enterprise.Entities

