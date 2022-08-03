// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Store.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a store object containing information about out storage devices
  /// </summary>
  public class Store : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Local = "local";
      public const string Remote = "remote";
      public const string Server = "server";
      public const string Priority = "priority";
      public const string Enabled = "enabled";
      public const string Capacity = "cap";
      public const string Available = "avail";
      public const string Used = "used";
      public const string Message = "msg";
     } // class Attributes

    #endregion



    #region fields

 
    /// <summary>
    /// Name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Local root
    /// </summary>
    private string m_local = string.Empty;

    /// <summary>
    /// Remote root path
    /// </summary>
    private string m_remote = string.Empty;

    /// <summary>
    /// Server name
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// Message if the values are 0
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Priority
    /// </summary>
    private uint m_priority = 0;

    /// <summary>
    /// Enabled flag
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// Total capacity
    /// </summary>
    private Int64 m_capacity = 0;

    /// <summary>
    /// Used bytes
    /// </summary>
    private Int64 m_used = 0;

    /// <summary>
    /// Available bytes
    /// </summary>
    private Int64 m_available = 0;

    #endregion


    #region properties


    /// <summary>
    /// Gets the name
    /// </summary>
    public string Name
    { 
      get { return m_name;  }
      private set { SetProperty<string>("Name", value, ref m_name);  }
    }

    /// <summary>
    /// Gets the Message
    /// </summary>
    public string Message
    {
      get { return m_message; }
      private set { SetProperty<string>("Message", value, ref m_message); }
    }

    /// <summary>
    /// Gets the local path
    /// </summary>
    public string Local
    {
      get { return m_local; }
      private set { SetProperty<string>("Local", value, ref m_local); }
    }

    /// <summary>
    /// Gets the remote path
    /// </summary>
    public string Remote
    {
      get { return m_remote; }
      private set { SetProperty<string>("Remote", value, ref m_remote); }
    }

    /// <summary>
    /// Gets the server name
    /// </summary>
    public string Server
    {
      get { return m_server; }
      private set { SetProperty<string>("Server", value, ref m_server); }
    }

    /// <summary>
    /// Gets the priority
    /// </summary>
    public uint Priority
    {
      get { return m_priority; }
      private set { SetProperty<uint>("Priority", value, ref m_priority); }
    }

    /// <summary>
    /// Gets the enabled flag
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      private set { SetProperty<bool>("Enabled", value, ref m_enabled); }
    }

    /// <summary>
    /// Gets the total capacity
    /// </summary>
    public Int64 Capacity
    {
      get { return m_capacity; }
      private set { SetProperty<Int64>("Capacity", value, ref m_capacity); }
    }

    /// <summary>
    /// Gets the byte used
    /// </summary>
    public Int64 Used
    {
      get { return m_used; }
      private set { SetProperty<Int64>("Used", value, ref m_used); }
    }

    /// <summary>
    /// Gets the byte available
    /// </summary>
    public Int64 Available
    {
      get { return m_available; }
      private set { SetProperty<Int64>("Available", value, ref m_available); }
    }
    #endregion


    #region methods


    /// <summary>
    /// Creates the store from the XmlNode
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Store Create(XmlNode node)
    {

      Store store = new Store();

      if (store == null)
        throw new OutOfMemoryException("Failed to allocate Store");

      store.Load(node);

      return store;

    } // Create(XmlNode node)


    /// <summary>
    /// Creates the Store from the xml string
    /// </summary>
    /// <param name="xml"></param>
    /// <returns></returns>
    public static Store Create(string xml)
    {

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      return Create(doc.DocumentElement);
    } // Create(string xml)
   
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

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Local, m_local);
          writer.WriteAttributeString(Attributes.Remote, m_remote);

          writer.WriteAttributeString(Attributes.Server, m_server);
          writer.WriteAttributeString(Attributes.Priority, m_priority.ToString());
          writer.WriteAttributeString(Attributes.Capacity, m_capacity.ToString());
          writer.WriteAttributeString(Attributes.Used, m_used.ToString());
          writer.WriteAttributeString(Attributes.Available, m_available.ToString());
          writer.WriteAttributeString(Attributes.Enabled, m_enabled.ToString().ToLower());

          WriteAttributeStrings(writer);
          WriteElements(writer);

         }


        writer.WriteEndElement();
      }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Creates the store from the XmlNode
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_name = node.Attributes[Attributes.Name].Value;
      m_local = node.Attributes[Attributes.Local].Value;
      m_remote = node.Attributes[Attributes.Remote].Value;
      m_server = node.Attributes[Attributes.Server].Value;
      m_message = node.Attributes[Attributes.Message].Value;
      m_priority = UInt32.Parse(node.Attributes[Attributes.Priority].Value);

      string enabled = node.Attributes[Attributes.Enabled].Value;
      m_enabled = (enabled.Length == 1 ? (enabled == "1" ? true : false) : Boolean.Parse(enabled));

      node.TryGetAttributeValue<Int64>(Attributes.Capacity, ref m_capacity);
      node.TryGetAttributeValue<Int64>(Attributes.Used, ref m_used);
      node.TryGetAttributeValue<Int64>(Attributes.Available, ref m_available);


    } // Load(XmlNode node)

    /// <summary>
    /// Clones the store from the XmlNode
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {

      Store store = new Store();

      if (store == null)
        throw new OutOfMemoryException("Failed to allocate Store");

      store.Name = m_name;
      store.Local = m_local;
      store.Remote = m_remote;
      store.Server = m_server;
      store.Priority = m_priority;
      store.Enabled = m_enabled;

      store.Capacity = m_capacity;
      store.Used = m_used;
      store.Available = m_available;

       return store;

    } // Clone()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Store other = obj as Store;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion
  } // Store

} // namespace 
